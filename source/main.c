#include <nds.h>
#include <stdio.h>
#include <string.h>

#include "lemonade_sample.h"
#include "duck_on_the_right.h"


typedef enum {
    TEXT_TOP,
    TEXT_MIDDLE,
    TEXT_BOTTOM
} TextPosition;

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

void printCentered(int row, const char* text) {
    int col = (32 - strlen(text)) / 2;
    if (col < 0) col = 0;
    printf("\x1b[%d;%dH%s", row, col, text);
}

void printWrapped(int startRow, const char* text) {
    int col = 0;
    int row = startRow;
    char word[64];
    int wordLen = 0;

    while (*text) {
        if (*text == '\n') {
            row++;
            col = 0;
            text++;
            continue;
        }
        wordLen = 0;
        while (*text && *text != ' ' && *text != '\n') {
            word[wordLen++] = *text++;
        }
        word[wordLen] = '\0';

        if (col + wordLen >= 32) {
            row++;
            col = 0;
        }
        printf("\x1b[%d;%dH%s", row, col, word);
        col += wordLen;

        if (*text == ' ') {
            if (col + 1 >= 32) {
                row++;
                col = 0;
            } else {
                printf("\x1b[%d;%dH ", row, col);
                col++;
            }
            text++;
        }
    }
}


typedef struct {
    const unsigned int *bitmap;
    const unsigned short *palette;
} Background;

// Define all your backgrounds here
Background bg_lemonade = { lemonade_sampleBitmap, lemonade_samplePal };
Background duck_on_right = { duck_on_the_rightBitmap, duck_on_the_rightPal };
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

typedef struct {
    const char *text;
    const char *speaker;
    TextPosition position;
    TextType type;
    Background *bg;  // NULL = keep current, set = swap to this
} Scene;

typedef struct {
    Scene *scenes;
    int length;
    Background *bg;  // Default background for this scene set
} SceneSet;


// NULL = keep current background. &bg_X = swap to that image.
Scene introScene[] = {
    {"Welcome to the Duck Song! \n (VN Version)", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL},
    {"Original Song by Bryant Oden 2009", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL},
    {"Adapted to VN format by Gabi, Christina, Esther, John", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL},
    {"Let's get started.", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL},
    {"Would you like to hear a tale?", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL},
    {"Okay :)", NULL, TEXT_MIDDLE, TEXT_NARRATION, NULL}
};

Scene scene1[] = {
    {"Let me tell you the duck story...", NULL, TEXT_TOP, TEXT_NARRATION, NULL},
    {"A duck walked up to a lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, &duck_on_right},  
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},               
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade},               
    {"'No, we just sell lemonade.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Can I get you a glass?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, &duck_on_right},            
    {"'I'll pass.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},                      
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_lemonade}, 
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL}
};

Scene scene2[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, NULL},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'No, like I said yesterday,'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'We just sell lemonade, okay?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Why not give it a try?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Goodbye.' :)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL}
};

Scene scene3[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, NULL},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Look, this is getting old.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'I mean, lemonade's all we've ever sold.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Why not give it a go?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'How 'bout... no.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL}
};

Scene scene4[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, NULL},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Got any grapes?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'THAT'S IT!' >:(", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'If you don't stay away, Duck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'I'll glue you to a tree and leave you there all day, stuck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'So don't get too close!' >:(", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Adios.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL}
};

Scene scene5[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION, NULL},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Got any glue?' :)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"The man paused and said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'... What?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Got any glue?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'No, why would I--'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Oh!'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"Then one more question for you.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Got any grapes?' \n (Bum bum bum, bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL}
};

Scene scene6[] = {
    {"The man just stopped, then he started to smile.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"He started to laugh, he laughed for a while.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"He said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Come on, Duck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Let's walk to the store.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'I'll buy you some grapes, so you won't have to ask anymore.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    // Example: if you had a store background, you'd swap it here:
    // {"So they walked to the store...", NULL, TEXT_BOTTOM, TEXT_NARRATION, &bg_store},
    {"So they walked to the store and the man bought some grapes.", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"He gave one to the duck and the duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"'Hmmm... No thanks.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'But y'know what sounds good?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'It would make my day...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'Has any...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"'lemonade?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION, NULL}
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
    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    bgId = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    PrintConsole topScreen;
    consoleInit(&topScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 22, 6, true, true);

    bgSetPriority(topScreen.bgId, 0);
    bgSetPriority(bgId, 3);

    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&bottomScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    int currentSceneSet = 0;
    int currentLine = 0;
    int totalSceneSets = ARRAY_LEN(allScenes);
    int needsRedraw = 1;
    int lastSceneSet = -1;

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
                printWrapped(baseRow - 2, nameBuffer);
            }
            printWrapped(baseRow, current.text);

            needsRedraw = 0;
        }

        blinkCounter++;
        if (blinkCounter > 30) {
            showPrompt = !showPrompt;
            blinkCounter = 0;
        }
        consoleSelect(&bottomScreen);
        printf("\x1b[10;0H                                ");
        if (showPrompt) {
            printCentered(10, "[Tap or press A to continue]");
        }

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