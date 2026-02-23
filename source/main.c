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

// Scene creation
typedef struct {
    const char *text;
    TextPosition position; 
} Scene;

typedef struct {
    Scene *scenes;
    int length;
} SceneSet;

Scene scene1[] = {
    {"Welcome to the Duck Song! \n(VN Version)", TEXT_MIDDLE},
    {"Let me tell you the duck story...", TEXT_MIDDLE},
    {"A duck walked up to a lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any grapes? >:)'", TEXT_MIDDLE},
    {"The man said, 'No, we just sell lemonade.'", TEXT_BOTTOM},
    {"'Can I get you a glass?'", TEXT_MIDDLE},
    {"The duck said, 'I'll pass.'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM},
    {"end 1.", TEXT_MIDDLE}
};

Scene scene2[] = {
    {"The next day...", TEXT_MIDDLE},
    {"When the duck walked up to the lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any grapes?'", TEXT_MIDDLE},
    {"The man said, 'No, like I said yesterday,'", TEXT_BOTTOM},
    {"'We just sell lemonade, okay?'", TEXT_MIDDLE},
    {"'Why not give it a try?'", TEXT_MIDDLE},
    {"The duck said,", TEXT_BOTTOM},
    {"'Goodbye. :)'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM},
    {"end 2.", TEXT_MIDDLE}
};

Scene scene3[] = {
    {"The next day...", TEXT_MIDDLE},
    {"When the duck walked up to the lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any grapes?'", TEXT_MIDDLE},
    {"The man said, 'Look, this is getting old.'", TEXT_BOTTOM},
    {"'I mean, lemonade's all we've ever sold.'", TEXT_MIDDLE},
    {"'Why not give it a go?'", TEXT_MIDDLE},
    {"The duck said,", TEXT_BOTTOM},
    {"'How 'bout... no.'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM},
    {"end 3.", TEXT_MIDDLE}
};

Scene scene4[] = {
    {"When the duck walked up to the lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any grapes?'", TEXT_MIDDLE},
    {"The man said, 'THAT'S IT!' >:(", TEXT_BOTTOM},
    {"'If you don't stay away, Duck.'", TEXT_MIDDLE},
    {"'I'll glue you to a tree and leave you there all day, stuck.'", TEXT_MIDDLE},
    {"'So don't get too close!' >:(", TEXT_MIDDLE},
    {"The duck said,", TEXT_BOTTOM},
    {"'Adios.'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle waddle)", TEXT_BOTTOM},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM},
    {"end 4.", TEXT_MIDDLE}
};

Scene scene5[] = {
    {"When the duck walked up to the lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"'Hey!' (Bum bum bum)", TEXT_MIDDLE},
    {"'Got any glue?'", TEXT_MIDDLE},
    {"The man paused and said,", TEXT_BOTTOM},
    {"'... What?'", TEXT_MIDDLE},
    {"'Got any glue?'", TEXT_MIDDLE},
    {"'No, why would I--'", TEXT_MIDDLE},
    {"'Oh!'", TEXT_MIDDLE},
    {"Then one more question for you.", TEXT_BOTTOM},
    {"'Got any grapes?' \n (Bum bum bum, bum bum bum)", TEXT_MIDDLE},
    {"end 5.", TEXT_MIDDLE}
    
};

Scene scene6[] = {
    {"The man just stopped, then he started to smile.", TEXT_BOTTOM},
    {"He started to laugh, he laughed for a while.", TEXT_BOTTOM},
    {"He said, 'Come on, Duck.'", TEXT_BOTTOM},
    {"'Let's walk to the store.'", TEXT_MIDDLE},
    {"'I'll buy you some grapes, so you won't have to ask anymore.'", TEXT_MIDDLE},
    {"So they walked to the store and the man bought some grapes.", TEXT_BOTTOM},
    {"He gave one to the duck and the duck said.", TEXT_BOTTOM},
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
    {"end 6.", TEXT_MIDDLE}
};

SceneSet allScenes[] = {
    {scene1, sizeof(scene1) / sizeof(Scene)},
    {scene2, sizeof(scene2) / sizeof(Scene)},
    {scene3, sizeof(scene3) / sizeof(Scene)},
    {scene4, sizeof(scene4) / sizeof(Scene)},
    {scene5, sizeof(scene5) / sizeof(Scene)},
    {scene6, sizeof(scene6) / sizeof(Scene)}
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
            printf("\x1b[%d;0H%s", getTextRow(activeScene[currentLine].position),
                activeScene[currentLine].text);
            
            // Bottom Screen
            consoleSelect(&bottomScreen);
            consoleClear();
            iprintf("[Tap or press A to continue]");

            needsRedraw = 0;
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
            swiWaitForVBlank();
        }
    }
    return 0;
}