#include <nds.h>
#include <stdio.h>
#include <string.h>

#include "lemonade_sample.h"
#include "duck_on_the_right.h"
#include "duck.h"
#include "man.h"

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
    {"Let's get started.", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0},
    {"Would you like to hear a tale?", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0},
    {"Okay :)", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL, 0, 0}
};

Scene scene1[] = {
    {"Let me tell you the duck story...", NULL, TEXT_TOP, TEXT_NARRATION, NULL, 0, 0},
    {"A duck walked up to a lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL, 0, 0},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL, 0, 0},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck, 10, 10},  
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},               
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},               
    {"'No, we just sell lemonade.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'Can I get you a glass?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},            
    {"'I'll pass.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},                      
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade}, 
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade}
};

Scene scene2[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, &bg_lemonade},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'No, like I said yesterday,'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'We just sell lemonade, okay?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'Why not give it a try?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Goodbye.' :)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade}
};

Scene scene3[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, &bg_lemonade},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Look, this is getting old.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'I mean, lemonade's all we've ever sold.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'Why not give it a go?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'How 'bout... no.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade}
};

Scene scene4[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, &bg_lemonade},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Got any grapes?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'THAT'S IT!' >:(", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'If you don't stay away, Duck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'I'll glue you to a tree and leave you there all day, stuck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'So don't get too close!' >:(", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Adios.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade}
};

Scene scene5[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, &bg_lemonade},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Got any glue?' :)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"The man paused and said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'... What?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'Got any glue?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'No, why would I--'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'Oh!'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"Then one more question for you.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &duck},
    {"'Got any grapes?' \n (Bum bum bum, bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck}
};

Scene scene6[] = {
    {"The man just stopped, then he started to smile.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"He started to laugh, he laughed for a while.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"He said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Come on, Duck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'Let's walk to the store.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    {"'I'll buy you some grapes, so you won't have to ask anymore.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, &man},
    // Example: if you had a store background, you'd swap it here:
    // {"So they walked to the store...", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_store},
    {"So they walked to the store and the man bought some grapes.", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"He gave one to the duck and the duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"'Hmmm... No thanks.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'But y'know what sounds good?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'It would make my day...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'Has any...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"'lemonade?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade}
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
    { endScene,      ARRAY_LEN(endScene),      &bg_none },
};


PrintConsole bottomScreen;

int main(void) {
    // Set up DS
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    bgId = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    // Initialize consoles for top and bottom screens, load in backgrounds
    PrintConsole topScreen;
    consoleInit(&topScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 22, 6, true, true);

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

    // Blinking effects variables
    int blinkCounter = 0;
    int showPrompt = 1;

    while (1) {
        scanKeys();
        int keyPressed = keysDown();

        SceneSet *activeSet = &allScenes[currentSceneSet];
        Scene *activeScene = activeSet->scenes;

        if (currentSceneSet != lastSceneSet) {
            loadBackground(activeSet->bg);
            lastSceneSet = currentSceneSet;
        }

        if (needsRedraw) {
            Scene current = activeScene[currentLine];

            if (current.bg != NULL) {
                loadBackground(current.bg);
            }

            consoleSelect(&topScreen);
            consoleClear();

            int baseRow = getTextRow(current.position);

            if (current.type == TEXT_DIALOGUE && current.speaker != NULL) {
                char nameBuffer[64];
                sprintf(nameBuffer, "%s:", current.speaker);
                printWrapped(baseRow - 2, nameBuffer, current.offsetX, current.offsetY);
            }
            printWrapped(baseRow, current.text, current.offsetX, current.offsetY);

            needsRedraw = 0;
        }

        // Blinking effects
        blinkCounter++;
        if (blinkCounter > 30) {
            showPrompt = !showPrompt;
            blinkCounter = 0;
        }
        consoleSelect(&bottomScreen);
        printf("\x1b[10;0H                                ");
        if (showPrompt) {
            printCentered(10, "[Tap or press A to continue]", 0, 0);
        }

        // Advance to the next screen
        if ((keyPressed & KEY_A) || (keyPressed & KEY_TOUCH)) {
            currentLine++;
            if (currentLine >= activeSet->length) {
                currentSceneSet++;
                currentLine = 0;
                if (currentSceneSet >= totalSceneSets) {
                    currentSceneSet = totalSceneSets - 1;
                    currentLine = activeSet->length - 1;
                }
            }
            needsRedraw = 1;
        }

        swiWaitForVBlank();
    }

    return 0;
}