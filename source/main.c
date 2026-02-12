#include <nds.h>
#include <stdio.h>

// First simple scene data

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

typedef struct {
    const char *text;
    TextPosition position; 
} Scene;

Scene scene1[] = {
    {"Welcome to the Duck Song! \n(VN Version)", TEXT_MIDDLE},
    {"Let me tell you the duck story...", TEXT_MIDDLE},
    {"A duck walked up to a lemonade stand.", TEXT_BOTTOM},
    {"And he said to the man, running the stand.", TEXT_BOTTOM},
    {"Hey! (Bum bum bum)", TEXT_MIDDLE},
    {"Got any grapes? >:)", TEXT_MIDDLE},
    {"The man said, 'No, we just sell lemonade.'", TEXT_BOTTOM},
    {"'Can I get you a glass?'", TEXT_MIDDLE},
    {"The duck said, 'I'll pass.'", TEXT_MIDDLE},
    {"Then he waddled away. \n (Waddle waddle)", TEXT_BOTTOM},
    {"Til the very next day. \n (Bum bum bum bum, bum da dum)", TEXT_BOTTOM},
    {"end.", TEXT_MIDDLE}
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

    int currentScene = 0;
    int totalScenes = sizeof(scene1) / sizeof(Scene);

    while (1) {
        scanKeys();
        touchPosition touch;
        touchRead(&touch);

        int keyPressed = keysDown();

        // Top screen
        consoleSelect(&topScreen);
        consoleClear();
        printf("\x1b[%d;0H%s", getTextRow(scene1[currentScene].position), scene1[currentScene].text);

        
        // Bottom Screen
        consoleSelect(&bottomScreen);
        consoleClear();
        iprintf("[Tap or press A to continue]");

        // Advance the screen
        if ((keyPressed & KEY_A) || (keyPressed & KEY_TOUCH)) {
            currentScene++;
            if (currentScene >= totalScenes) {
                currentScene = totalScenes - 1;
            }
        }
        swiWaitForVBlank();
    }
    return 0;
}