#include <nds.h>
#include <stdio.h>

// First simple scene data

// Text formatting
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
    const char *speaker; // Will be null for narration
    TextPosition position; 
    TextType type;
} Scene;

typedef struct {
    Scene *scenes;
    int length;
} SceneSet;

Scene introScene[] = {
    {"Welcome to the Duck Song! \n (VN Version)", NULL, TEXT_MIDDLE, TEXT_NARRATION},
    {"Original Song by Bryant Oden 2009", NULL, TEXT_MIDDLE, TEXT_NARRATION},
    {"Adapted to VN format by Gabi, Christina, Esther, John", NULL, TEXT_MIDDLE, TEXT_NARRATION},
    {"Let's get started.", NULL, TEXT_MIDDLE, TEXT_NARRATION},
    {"Would you like to hear a tale?", NULL, TEXT_MIDDLE, TEXT_NARRATION},
    {"Okay :)", NULL, TEXT_MIDDLE, TEXT_NARRATION}
};

Scene scene1[] = {
    {"Let me tell you the duck story...", NULL, TEXT_TOP, TEXT_NARRATION},
    {"A duck walked up to a lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'No, we just sell lemonade.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Can I get you a glass?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'I'll pass.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION}
};

Scene scene2[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'No, like I said yesterday,'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'We just sell lemonade, okay?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Why not give it a try?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Goodbye.' :)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION}
};

Scene scene3[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Got any grapes?' >:)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Look, this is getting old.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'I mean, lemonade's all we've ever sold.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Why not give it a go?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'How 'bout... no.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION}
};

Scene scene4[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Got any grapes?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The man said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'THAT'S IT!' >:(", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'If you don't stay away, Duck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'I'll glue you to a tree and leave you there all day, stuck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'So don't get too close!' >:(", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Adios.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", NULL, TEXT_BOTTOM, TEXT_NARRATION}
};

Scene scene5[] = {
    {"The next day...", NULL, TEXT_TOP, TEXT_NARRATION},
    {"When the duck walked up to the lemonade stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"And he said to the man, running the stand.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Hey!' \n (Bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Got any glue?' :)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"The man paused and said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'... What?'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Got any glue?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'No, why would I--'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Oh!'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"Then one more question for you.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Got any grapes?' \n (Bum bum bum, bum bum bum)", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE}
    
};

Scene scene6[] = {
    {"The man just stopped, then he started to smile.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"He started to laugh, he laughed for a while.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"He said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Come on, Duck.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Let's walk to the store.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'I'll buy you some grapes, so you won't have to ask anymore.'", "Man", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"So they walked to the store and the man bought some grapes.", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"He gave one to the duck and the duck said,", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"'Hmmm... No thanks.'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'But y'know what sounds good?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'It would make my day...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Do you think this store...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'Has any...'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"'lemonade?'", "Duck", TEXT_MIDDLE, TEXT_DIALOGUE},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Then he waddled away. \n (Waddle waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION},
    {"Then he waddled away. \n (Waddle waddle)", NULL, TEXT_BOTTOM, TEXT_NARRATION}
};

Scene endScene[] = {
    {"You've reached the end.", NULL, TEXT_MIDDLE, TEXT_NARRATION},
    {"Thank you for reading The Duck Song!", NULL, TEXT_MIDDLE, TEXT_NARRATION},
    {"We hope you enjoyed it. :D", NULL, TEXT_MIDDLE, TEXT_NARRATION}
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
            // printWrapped(getTextRow(activeScene[currentLine].position), activeScene[currentLine].text);
            Scene current = activeScene[currentLine];
            int baseRow = getTextRow(current.position);
            if (current.type == TEXT_DIALOGUE) {
                if (current.speaker != NULL) {
                    // Prints speaker name on top
                    char nameBuffer[64];
                    sprintf(nameBuffer, "%s:", current.speaker);
                    printWrapped(baseRow - 2, nameBuffer);
                    // printf("\x1b[1;1H%s:", current.speaker);
                }
                // Dialogue printed below the speaker name
                // printWrapped(5, current.text);
                printWrapped(baseRow, current.text);
            } else {
                // Prints narration
                // printWrapped(getTextRow(current.position), current.text);
                printWrapped(baseRow, current.text);
            }
            
            // Bottom Screen
            consoleSelect(&bottomScreen);
            consoleClear();
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