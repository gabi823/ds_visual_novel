#include <nds.h>
#include <stdio.h>
#include <string.h>

#include "lemonade_sample.h"
#include "duck_on_the_right.h"
#include "duck.h"
#include "man.h"
#include "general.h"
#include "blank.h"
#include "store.h"

#define COLOR_BLACK  "\x1b[30m"
#define COLOR_RESET  "\x1b[39m"

// The white box on general.png / blank.png occupies roughly these rows/columns
// on the 24-row, 32-column console. Adjust if the box sits differently on your art.
#define BOX_TOP_ROW    17
#define BOX_BOTTOM_ROW 21
#define BOX_WIDTH      26   // Text wraps within this width (centered on screen)

// The white speech box on duck.png sits in the bottom-right of the image.
// Baseline taken from scene1 " Hey! \n (Bum bum bum)" which rendered correctly
// at TEXT_MIDDLE (row 10) + offsetY=8 = row 18, offsetX=12.
#define DUCK_BOX_TOP_ROW    17
#define DUCK_BOX_BOTTOM_ROW 20
#define DUCK_BOX_LEFT_COL   10
#define DUCK_BOX_RIGHT_COL  29

// The white speech box on man.png sits in the bottom-left of the image —
// mirror of the duck box, same Y range but shifted to the left side.
#define MAN_BOX_TOP_ROW    17
#define MAN_BOX_BOTTOM_ROW 20
#define MAN_BOX_LEFT_COL   0
#define MAN_BOX_RIGHT_COL  19

// Text formatting/layout
typedef enum {
    TEXT_TOP,
    TEXT_MIDDLE,
    TEXT_BOTTOM
} TextPosition;

// Distinction between speakers
typedef enum {
    TEXT_NARRATION,
    TEXT_DIALOGUE
} TextType;

int getTextRow(TextPosition pos) {
    switch (pos) {
        case TEXT_TOP:    return 2;
        case TEXT_MIDDLE: return 10;
        case TEXT_BOTTOM: return 18;
        default:          return 10;
    }
}

// Centers text in the middle of the screen
void printCentered(int row, const char* text, int offsetX, int offsetY) {
    // DS screen is 32 characters wide
    int col = (32 - strlen(text)) / 2 + offsetX;
    if (col < 0) col = 0;
    printf("\x1b[%d;%dH%s", row + offsetY, col, text);
}

// Wraps text by splitting words, doesn't cut words in half
void printWrapped(int startRow, const char* text, int offsetX, int offsetY) {
    int col = offsetX;
    int row = startRow + offsetY;
    char word[64];
    int wordLen = 0;

    while (*text) {
        if (*text == '\n') {
            row++;
            col = offsetX;
            text++;
            continue;
        }
        wordLen = 0;
        while (*text && *text != ' ' && *text != '\n') {
            word[wordLen++] = *text++;
        }
        word[wordLen] = '\0';

        // If word won't fit on current line, move to next line
        if (col + wordLen >= 32) {
            row++;
            col = offsetX;
        }
        printf("\x1b[%d;%dH%s", row, col, word);
        col += wordLen;

        // Print space if there was one
        if (*text == ' ') {
            if (col + 1 >= 32) {
                row++;
                col = offsetX;
            } else {
                printf("\x1b[%d;%dH ", row, col);
                col++;
            }
            text++;
        }
    }
}

// Counts how many lines the text will occupy when wrapped to maxWidth columns,
// respecting explicit '\n' breaks. Does NOT print anything.
int countWrappedLines(const char* text, int maxWidth) {
    if (*text == '\0') return 0;
    int lines = 1;
    int lineLen = 0;
    int wordLen = 0;
    const char *p = text;

    while (*p) {
        if (*p == '\n') {
            lines++;
            lineLen = 0;
            p++;
            continue;
        }

        // Measure next word
        wordLen = 0;
        while (*p && *p != ' ' && *p != '\n') {
            wordLen++;
            p++;
        }

        // Would this word (plus leading space if needed) overflow?
        int needed = wordLen + (lineLen > 0 ? 1 : 0);
        if (lineLen + needed > maxWidth) {
            lines++;
            lineLen = wordLen;
        } else {
            lineLen += needed;
        }

        if (*p == ' ') p++;
    }

    return lines;
}

// Wraps text to maxWidth and centers each line horizontally between leftCol..rightCol.
// Use leftCol=0, rightCol=31 to center on the full screen.
void printWrappedCenteredCols(int startRow, const char* text, int maxWidth, int leftCol, int rightCol) {
    int row = startRow;
    int regionWidth = rightCol - leftCol + 1;
    char line[64]; // generous buffer
    int lineLen = 0;
    char word[64];
    int wordLen = 0;

    while (*text) {
        if (*text == '\n') {
            line[lineLen] = '\0';
            if (lineLen > 0) {
                int col = leftCol + (regionWidth - lineLen) / 2;
                if (col < 0) col = 0;
                printf("\x1b[%d;%dH%s", row, col, line);
            }
            row++;
            lineLen = 0;
            text++;
            continue;
        }

        wordLen = 0;
        while (*text && *text != ' ' && *text != '\n') {
            word[wordLen++] = *text++;
        }
        word[wordLen] = '\0';

        // If word won't fit on current line, flush and start a new line
        if (lineLen + wordLen + (lineLen > 0 ? 1 : 0) > maxWidth) {
            line[lineLen] = '\0';
            int col = leftCol + (regionWidth - lineLen) / 2;
            if (col < 0) col = 0;
            printf("\x1b[%d;%dH%s", row, col, line);
            row++;
            lineLen = 0;
        }

        // Add space before word if not first on line
        if (lineLen > 0) {
            line[lineLen++] = ' ';
        }

        // Append word to current line buffer
        for (int i = 0; i < wordLen; i++) {
            line[lineLen++] = word[i];
        }

        if (*text == ' ') text++;
    }

    // Flush remaining text
    if (lineLen > 0) {
        line[lineLen] = '\0';
        int col = leftCol + (regionWidth - lineLen) / 2;
        if (col < 0) col = 0;
        printf("\x1b[%d;%dH%s", row, col, line);
    }
}

// Centers text both horizontally AND vertically within an arbitrary rectangle.
// Long text grows from the middle outward instead of overflowing.
void printBoxCenteredAt(int topRow, int bottomRow, int leftCol, int rightCol, const char* text) {
    int maxWidth = rightCol - leftCol + 1;
    int lines = countWrappedLines(text, maxWidth);
    int boxHeight = bottomRow - topRow + 1;
    int startRow = topRow + (boxHeight - lines) / 2;
    if (startRow < topRow) startRow = topRow;
    printWrappedCenteredCols(startRow, text, maxWidth, leftCol, rightCol);
}

// Centers text in the main narration box (general.png / blank.png) — full-screen-wide
// region but limited to BOX_WIDTH for wrapping.
void printBoxCentered(const char* text) {
    int screenCenter = 16; // half of 32
    int halfWidth = BOX_WIDTH / 2;
    int leftCol = screenCenter - halfWidth;
    int rightCol = screenCenter + halfWidth - 1;
    printBoxCenteredAt(BOX_TOP_ROW, BOX_BOTTOM_ROW, leftCol, rightCol, text);
}

// Background creation
typedef struct {
    const unsigned int *bitmap;
    const unsigned short *palette;
} Background;

// Define all your backgrounds here
Background bg_lemonade = { lemonade_sampleBitmap, lemonade_samplePal };
Background duck_on_right = { duck_on_the_rightBitmap, duck_on_the_rightPal };
Background duck = { duckBitmap, duckPal };
Background man = { manBitmap, manPal };
Background general = { generalBitmap, generalPal };
Background blank = { blankBitmap, blankPal };
Background store = { storeBitmap, storePal };



// basically the variable within the array here use the variable name + Bitmap or Pal

Background bg_none     = { NULL, NULL };

int bgId;

void loadBackground(Background *bg) {
    if (bg->bitmap != NULL && bg->palette != NULL) {
        dmaCopy(bg->bitmap, bgGetGfxPtr(bgId), 256 * 192);
        dmaCopy(bg->palette, BG_PALETTE, 256 * 2);
    } else {
        dmaFillWords(0, bgGetGfxPtr(bgId), 256 * 192);
    }
    BG_PALETTE[0] = 0;
    BG_PALETTE[255] = 0x7FFF;
}

// Scene creation
typedef struct {
    const char *text;
    const char *speaker; // Will be NULL for narration
    TextPosition position;
    TextType type;
    Background *bg;  // NULL = keep current, set = swap to this
    int offsetX;     // horizontal offset for this line
    int offsetY;     // vertical offset for this line
} Scene;

typedef struct {
    Scene *scenes;
    int length;
    Background *bg;  // Default background for this scene set
} SceneSet;


// NULL = keep current background. &bg_X = swap to that image.
Scene introScene[] = {
    {"Welcome to the Duck Song! \n (VN Version)", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0},
    {"Original Song by Bryant Oden 2009", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL},
    {"Adapted to VN format by Gabi, Christina, Esther, John", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0},
    {"Let's get started.", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0}
};

Scene scene1[] = {
    {"Okay :)", NULL, TEXT_MIDDLE, TEXT_NARRATION, &blank, 0, 0},
    {"Let me tell you the duck story...", NULL, TEXT_TOP, TEXT_NARRATION, &blank, 0, 0},
    {"A duck walked up to a lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general, 0, 0},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general, 0, 0},
    {" Hey! \n (Bum bum bum)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"Got any grapes? >:)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 11, 8},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {" No, we just \n sell lemonade.", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {" Can I get \n you a glass?", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"I'll pass.", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 14, 8},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general}
};

Scene scene2[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, &blank},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {" Hey! \n (Bum bum bum)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"Got any grapes? >:)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 11, 8},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {" No, like I \n said yesterday,", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {" We just sell \n lemonade, okay?", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {" Why not give \n it a try?", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Goodbye. :)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general}
};

Scene scene3[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, &blank},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {" Hey! \n (Bum bum bum)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"Got any grapes? >:)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 11, 8},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {" Look, this is \n getting old.", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {" I mean, lemonade's \n all we've ever sold.", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 1, 9},
    {" Why not give \n it a go?", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"How 'bout... no.", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general}
};

Scene scene4[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, &blank},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {" Hey! \n (Bum bum bum)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"Got any grapes?", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"THAT'S IT! >:(", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {" If you don't \n stay away, Duck.", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {" I'll glue you to \n a tree and leave \n you there all day, \n stuck.", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 1.5, 8},
    {" So don't get \n too close!' >:(", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Adios.", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 14, 8},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general}
};

Scene scene5[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, &blank},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {" Hey! \n (Bum bum bum)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"Got any glue? :)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"The man paused and said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"... What?", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 8},
    {"Got any glue?", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8},
    {"No, why would I--", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 8},
    {"Oh!", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &man, 2, 9},
    {"Then one more \n question for you.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &duck, 12, 8},
    {" Got any grapes? \n (Bum bum bum, bum bum bum)", NULL, TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 12, 8}
};

Scene scene6[] = {
    {"The man just stopped, then he started to smile.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"He started to laugh, he laughed for a while.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"He said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &general},
    {"'Come on, Duck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'Let's walk to the store.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'I'll buy you some grapes, so you won't have to ask anymore.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    // Example: if you had a store background, you'd swap it here:
    // {"So they walked to the store...", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_store},
    {"So they walked to the store and the man bought some grapes.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &store},
    {"He gave one to the duck and the duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &store},
    {"'Hmmm... No thanks.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'But y'know what sounds good?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'It would make my day...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Has any...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'lemonade?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &duck},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &duck},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &duck}
};

Scene firstChoiceRefuse[] = {
    {"Boooo.", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0},
    {"You're no fun. :C", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0},
    {"Well then.", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0},
    {"The Duck, the Man, and we the team bid you goodbye.", NULL, TEXT_MIDDLE, TEXT_NARRATION, 0, 0}
};

Scene endScene[] = {
    {"You've reached the end.", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL},
    {"Thank you for reading The Duck Song!", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL},
    {"We hope you enjoyed it. :D", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL}
};


#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

SceneSet allScenes[] = {
    //  scenes       length                    default background
    { introScene,    ARRAY_LEN(introScene),    &bg_none },
    { scene1,        ARRAY_LEN(scene1),        &bg_lemonade },
    { scene2,        ARRAY_LEN(scene2),        &bg_lemonade },
    { scene3,        ARRAY_LEN(scene3),        &bg_lemonade },
    { scene4,        ARRAY_LEN(scene4),        &bg_lemonade },
    { scene5,        ARRAY_LEN(scene5),        &bg_lemonade },
    { scene6,        ARRAY_LEN(scene6),        &bg_lemonade },
    { firstChoiceRefuse, ARRAY_LEN(firstChoiceRefuse), &bg_none},
    { endScene,      ARRAY_LEN(endScene),      &bg_none },
};


PrintConsole bottomScreen;

// Choice menu function
void drawChoiceMenu(int selectedChoice) {
    consoleSelect(&bottomScreen);
    consoleClear();
    printCentered(8, "Would you like to hear a tale?", 0, 0);
    if (selectedChoice == 0) {
        printCentered(10, "> Yes", 0, 0);
    } else {
        printCentered(10, "  Yes", 0, 0);
    }
    if (selectedChoice == 1) {
        printCentered(12, "> No", 0, 0);
    } else {
        printCentered(12, "  No", 0, 0);
    }
}

int main(void) {
    // Set up DS
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    bgId = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    // Initialize consoles for top and bottom screens, load in backgrounds
    PrintConsole topScreen;
    // Changed map base from 22 to 24 so that text and bitmap layer don't overlap in VRAM
    consoleInit(&topScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 24, 6, true, true);

    bgSetPriority(topScreen.bgId, 0);
    bgSetPriority(bgId, 3);

    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&bottomScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    // Variables to move through scenes
    int currentSceneSet = 0;
    int currentLine = 0;
    int totalSceneSets = ARRAY_LEN(allScenes);
    int needsRedraw = 1;
    int lastSceneSet = -1;

    // Track the currently displayed background so we know when to center text in the box
    Background *currentBg = NULL;

    // Choice variables for branching scenes
    int inChoice = 0;
    int selectedChoice = 0;

    // Blinking effects variables
    int blinkCounter = 0;
    int showPrompt = 1;

    while (1) {
        scanKeys();
        int keyPressed = keysDown();

        SceneSet *activeSet = &allScenes[currentSceneSet];
        Scene *activeScene = activeSet->scenes;

        int atTheEnd = (currentSceneSet == totalSceneSets - 1 && currentLine == activeSet->length - 1);

        // Triggers choice at the end of introScene
        if (currentSceneSet == 0 && currentLine == activeSet->length - 1 && !inChoice) {
            inChoice = 1;
            selectedChoice = 0;
        }

        if (currentSceneSet != lastSceneSet) {
            loadBackground(activeSet->bg);
            currentBg = activeSet->bg;
            lastSceneSet = currentSceneSet;
        }
        if (needsRedraw) {
            Scene current = activeScene[currentLine];

            if (current.bg != NULL) {
                loadBackground(current.bg);
                currentBg = current.bg;
            }

            // Displaying for top screen
            consoleSelect(&topScreen);
            consoleClear();

            // Black text on backgrounds, white text on blank/none
            if (current.bg != NULL || activeSet->bg->bitmap != NULL) {
                BG_PALETTE[255] = RGB15(0, 0, 0);
            } else {
                BG_PALETTE[255] = RGB15(31, 31, 31);
            }
            int baseRow = getTextRow(current.position);

            // Skip the "Speaker:" label when text is routed into a speech/narration
            // box — the background image already identifies who is talking.
            int usesBoxRouting = (currentBg == &general || currentBg == &blank ||
                                  currentBg == &store || currentBg == &duck ||
                                  currentBg == &man);

            if (current.type == TEXT_DIALOGUE && current.speaker != NULL && !usesBoxRouting) {
                char nameBuffer[64];
                sprintf(nameBuffer, "%s:", current.speaker);
                printWrapped(baseRow - 2, nameBuffer, current.offsetX, current.offsetY);
            }

            // Route text by background so it lands in the correct speech/narration box.
            if (currentBg == &general || currentBg == &blank || currentBg == &store) {
                printBoxCentered(current.text);
            } else if (currentBg == &duck) {
                printBoxCenteredAt(DUCK_BOX_TOP_ROW, DUCK_BOX_BOTTOM_ROW,
                                   DUCK_BOX_LEFT_COL, DUCK_BOX_RIGHT_COL,
                                   current.text);
            } else if (currentBg == &man) {
                printBoxCenteredAt(MAN_BOX_TOP_ROW, MAN_BOX_BOTTOM_ROW,
                                   MAN_BOX_LEFT_COL, MAN_BOX_RIGHT_COL,
                                   current.text);
            } else {
                printWrapped(baseRow, current.text, current.offsetX, current.offsetY);
            }

            needsRedraw = 0;
        }

        // Choice handler
        if (inChoice) {
            if (keyPressed & KEY_UP) {
                selectedChoice--;
                if (selectedChoice < 0) {
                    selectedChoice = 1;
                }
            }
            if (keyPressed & KEY_DOWN) {
                selectedChoice++;
                if (selectedChoice > 1) {
                    selectedChoice = 0;
                }
            }
            drawChoiceMenu(selectedChoice);
            if (keyPressed & KEY_A) {
                inChoice = 0;
                if (selectedChoice == 0) {
                    currentSceneSet = 1; // normal story
                } else {
                    currentSceneSet = 7; // refusal scene
                }
                currentLine = 0;
                needsRedraw = 1;
            }
            swiWaitForVBlank();
            continue;
        }

        // Blinking effects
        blinkCounter++;
        if (blinkCounter > 45) {
            showPrompt = !showPrompt;
            blinkCounter = 0;
        }

        // Displaying for bottom screen
        consoleSelect(&bottomScreen);
        consoleClear();
        printf("\x1b[10;0H                                ");
        printf("\x1b[11;1H                                ");
        if (showPrompt) {
            if (atTheEnd) {
                printCentered(10, "[Press START to restart]", 0, 0);
            } else {
                printCentered(10, "[Tap/press A to continue]", 0, 0);
                printCentered(11, "[Press B to go back]", 0, 0);
            }
        }

        // Advance to the next screen
        if ((!atTheEnd && (keyPressed & KEY_A)) || (keyPressed & KEY_TOUCH)) {
            currentLine++;
            if (currentLine >= activeSet->length) {
                // Skip refusal scene if coming from main story
                if (currentSceneSet == 6) {
                    currentSceneSet = 8;
                } else {
                    currentSceneSet++;
                }
                currentLine = 0;
                if (currentSceneSet >= totalSceneSets) {
                    currentSceneSet = totalSceneSets - 1;
                    currentLine = activeSet->length - 1;
                }
            }
            needsRedraw = 1;
        }

        // Go back to the previous screen
        if (keyPressed & KEY_B) {
            currentLine--;
            if (currentLine < 0) {
                currentSceneSet--;
                if (currentSceneSet < 0) {
                    currentSceneSet = 0;
                    currentLine = 0;
                } else {
                    SceneSet *previousSet = &allScenes[currentSceneSet];
                    currentLine = previousSet->length - 1;
                }
            }
            needsRedraw = 1;
        }

        // Restart back to beginning of visual novel
        if (atTheEnd && (keyPressed & KEY_START)) {
            currentSceneSet = 0;
            currentLine = 0;
            lastSceneSet = -1;
            needsRedraw = 1;
        }

        swiWaitForVBlank();
    }

    return 0;
}