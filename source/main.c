#include <nds.h>
#include <stdio.h>

const char* script[] = {
    "hello!",
    "testing out nds visual novel",
    "top screen = scene",
    "bottom screen = dialogue",
    "tap or press A to continue.",
    NULL
};

int main(void) {
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    PrintConsole* top;
    PrintConsole* bottom;

    // Top screen console
    top = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256,
                      31, 0, true, true);

    // Bottom screen console
    bottom = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256,
                         30, 0, false, true);

    int line = 0;

    // Initial text
    consoleSelect(top);
    consoleClear();
    iprintf("Scene Screen\n");

    consoleSelect(bottom);
    consoleClear();
    iprintf("%s\n", script[line++]);

    while (1) {
        scanKeys();
        touchPosition touch;
        touchRead(&touch);

        int pressed = keysDown();

        if (pressed & (KEY_A | KEY_TOUCH)) {
            consoleSelect(bottom);
            consoleClear();
            if (script[line]) {
                iprintf("%s\n", script[line]);
                line++;
            }
        }

        swiWaitForVBlank();
    }
}
