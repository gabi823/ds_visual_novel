#include <nds.h>
#include <stdio.h>

// First simple scene data

// Text formatting
typedef enum {
    TEXT_TOP,
    TEXT_MIDDLE,
    TEXT_BOTTOM
} TextPosition;

int getTextRow(TextPosition pos) {
    switch (pos) {
        case TEXT_TOP:    return 2;
        case TEXT_MIDDLE: return 10;
        case TEXT_BOTTOM: return 18;
        default:          return 10;
    }
}

// Centers text in the middle of the screen
void printCentered(int row, const char* text) {
    // DS screen is 32 characters wide
    int col = (32 - strlen(text)) / 2;
    if (col < 0) {
        col = 0;
    }
    printf("\x1b[%d;%dH%s", row, col, text);
}

// Wraps text by splitting by words, doesn't cut words in half
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

        // If word won't fit on current line, move to next line
        if (col + wordLen >= 32) {
            row++;
            col = 0;
        }
        printf("\x1b[%d;%dH%s", row, col, word);
        col += wordLen;

        // Print space if there was one
        if(*text == ' ') {
            if(col + 1 >= 32) {
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

// Scene creation
typedef struct {
    const char *text;
    TextPosition position; 
} Scene;

typedef struct {
    Scene *scenes;
    int length;
} SceneSet;

Scene introScene[] = {
    {"Welcome to the Duck Song! \n (VN Version)", TEXT_MIDDLE},
    {"Original Song by Bryant Oden 2009", TEXT_MIDDLE},
    {"Adapted to VN format by Gabi, Christina, Esther, John", TEXT_MIDDLE},
    {"Let's get started.", TEXT_MIDDLE},
    {"Would you like to hear a tale?", TEXT_MIDDLE},
    {"Okay :)", TEXT_MIDDLE}
};

Scene scene1[] = {
    {"Let me tell you the duck story...", TEXT_TOP},
    {"A duck walked up to a lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' \n (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any grapes?' >:)", TEXT_MIDDLE},
    {"The man said,", TEXT_BOTTOM},
    {"'No, we just sell lemonade.'", TEXT_MIDDLE},
    {"'Can I get you a glass?'", TEXT_MIDDLE},
    {"The duck said,", TEXT_BOTTOM},
    {"'I'll pass.'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM}
};

Scene scene2[] = {
    {"The next day...", TEXT_TOP},
    {"When the duck walked up to the lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' \n (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any grapes?' >:)", TEXT_MIDDLE},
    {"The man said,", TEXT_BOTTOM},
    {"'No, like I said yesterday,'", TEXT_MIDDLE},
    {"'We just sell lemonade, okay?'", TEXT_MIDDLE},
    {"'Why not give it a try?'", TEXT_MIDDLE},
    {"The duck said,", TEXT_BOTTOM},
    {"'Goodbye.' :)", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM},
};

Scene scene3[] = {
    {"The next day...", TEXT_TOP},
    {"When the duck walked up to the lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' \n (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any grapes?' >:)", TEXT_MIDDLE},
    {"The man said,", TEXT_BOTTOM},
    {"'Look, this is getting old.'", TEXT_MIDDLE},
    {"'I mean, lemonade's all we've ever sold.'", TEXT_MIDDLE},
    {"'Why not give it a go?'", TEXT_MIDDLE},
    {"The duck said,", TEXT_BOTTOM},
    {"'How 'bout... no.'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM},
};

Scene scene4[] = {
    {"The next day...", TEXT_TOP},
    {"When the duck walked up to the lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' \n (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any grapes?'", TEXT_MIDDLE},
    {"The man said,", TEXT_BOTTOM},
    {"'THAT'S IT!' >:(", TEXT_MIDDLE},
    {"'If you don't stay away, Duck.'", TEXT_MIDDLE},
    {"'I'll glue you to a tree and leave you there all day, stuck.'", TEXT_MIDDLE},
    {"'So don't get too close!' >:(", TEXT_MIDDLE},
    {"The duck said,", TEXT_BOTTOM},
    {"'Adios.'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM},
};

Scene scene5[] = {
    {"The next day...", TEXT_TOP},
    {"When the duck walked up to the lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' \n (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any glue?' :)", TEXT_MIDDLE},
    {"The man paused and said,", TEXT_BOTTOM},
    {"'... What?'", TEXT_MIDDLE},
    {"'Got any glue?'", TEXT_MIDDLE},
    {"'No, why would I--'", TEXT_MIDDLE},
    {"'Oh!'", TEXT_MIDDLE},
    {"Then one more question for you.", TEXT_BOTTOM},
    {"'Got any grapes?' \n (Bum bum bum, bum bum bum)", TEXT_MIDDLE},
    
};

Scene scene6[] = {
    {"The man just stopped, then he started to smile.", TEXT_BOTTOM},
    {"He started to laugh, he laughed for a while.", TEXT_BOTTOM},
    {"He said,", TEXT_BOTTOM},
    {"'Come on, Duck.'", TEXT_MIDDLE},
    {"'Let's walk to the store.'", TEXT_MIDDLE},
    {"'I'll buy you some grapes, so you won't have to ask anymore.'", TEXT_MIDDLE},
    {"So they walked to the store and the man bought some grapes.", TEXT_BOTTOM},
    {"He gave one to the duck and the duck said,", TEXT_BOTTOM},
    {"'Hmmm... No thanks.'", TEXT_MIDDLE},
    {"'But y'know what sounds good?'", TEXT_MIDDLE},
    {"'It would make my day...'", TEXT_MIDDLE},
    {"'Do you think this store...'", TEXT_MIDDLE},
    {"'Do you think this store...'", TEXT_MIDDLE},
    {"'Do you think this store...'", TEXT_MIDDLE},
    {"'Has any...'", TEXT_MIDDLE},
    {"'lemonade?'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
};

Scene endScene[] = {
    {"You've reached the end.", TEXT_MIDDLE},
    {"Thank you for reading The Duck Song!", TEXT_MIDDLE},
    {"We hope you enjoyed it. :D", TEXT_MIDDLE}
};

SceneSet allScenes[] = {
    {introScene, sizeof(introScene) / sizeof(Scene)},
    {scene1, sizeof(scene1) / sizeof(Scene)},
    {scene2, sizeof(scene2) / sizeof(Scene)},
    {scene3, sizeof(scene3) / sizeof(Scene)},
    {scene4, sizeof(scene4) / sizeof(Scene)},
    {scene5, sizeof(scene5) / sizeof(Scene)},
    {scene6, sizeof(scene6) / sizeof(Scene)},
    {endScene, sizeof(endScene) / sizeof(Scene)}
};

PrintConsole topScreen;
PrintConsole bottomScreen;

int main(void) {
    // Set up DS
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    // Initialize console for top  and bottom screen
    consoleInit(&topScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 0, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    // Mechanics to move through the scenes
    int currentSceneSet = 0;
    int currentLine = 0;
    int totalSceneSets = sizeof(allScenes) / sizeof(SceneSet);
    int needsRedraw = 1;

    // Blinking effect variables
    int blinkCounter = 0;
    int showPrompt = 1;

    while (1) {
        scanKeys();
        touchPosition touch;
        touchRead(&touch);

        int keyPressed = keysDown();

        SceneSet *activeSet = &allScenes[currentSceneSet];
        Scene *activeScene = activeSet->scenes;

        if (needsRedraw) {
            // Top screen
            consoleSelect(&topScreen);
            consoleClear();
            // printf("\x1b[%d;0H%s", getTextRow(activeScene[currentLine].position),
            //     activeScene[currentLine].text);
            printWrapped(getTextRow(activeScene[currentLine].position), activeScene[currentLine].text);
            
            // Bottom Screen
            consoleSelect(&bottomScreen);
            consoleClear();
            // iprintf("[Tap or press A to continue]");
            printCentered(10, "[Tap or press A to continue]");

            needsRedraw = 0;
        }

        // Blinking effect
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

        // Advance the screen
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