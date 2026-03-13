#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"

extern int FEPrintf(FEString *text, const char *fmt, ...);

void FEKeyboard::SetString(char *pStr) {
    FEPrintf(mpInputString, pStr);
}

void FEKeyboard::SetMaxLength(int nLength) {
    if (nLength > 0x9C) {
        nLength = 0x9C;
    }
    mnMaxLength = nLength;
}

int FEKeyboard::IsKeyButton(FEObject *pObj) {
    int i = 0;
    do {
        if (mpKeyButton[i] == pObj) {
            return i;
        }
        i = i + 1;
    } while (i < 0x2D);
    return -1;
}

void FEKeyboard::AppendSpace() {
    if (mnMode == MODE_ALL_KEYS) {
        AppendChar(0x20);
    }
}
