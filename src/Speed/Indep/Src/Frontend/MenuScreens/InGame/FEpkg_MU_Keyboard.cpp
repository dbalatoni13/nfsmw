#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int FEPrintf(FEString *text, const char *fmt, ...);
extern Timer RealTimer;
extern Timer KBCreationTimer;
extern FEKeyboard *gFEKeyboard;
extern bool KeyboardActive;

FEKeyboard::FEKeyboard(ScreenConstructorData *sd)
    : MenuScreen(sd)
{
    mnWindowStartIdx = 0;
    mThis = sd->pPackage;
    KBCreationTimer = RealTimer;
    Initialize();
    UpdateVisuals();
}

int FEKeyboard::GetCase() {
    if (mbShift) {
        return !mbCaps;
    }
    if (!mbCaps) {
        return 0;
    }
    return 1;
}

void FEKeyboard::MoveCursor(int nDelta) {
    mbIsFirstKey = false;
    mnCursorIndex = mnCursorIndex + nDelta;
    if (mnCursorIndex < 0) {
        mnCursorIndex = 0;
    }
    int stringLength = bStrLen(mString);
    if (mnCursorIndex >= stringLength) {
        mnCursorIndex = stringLength;
    }
    UpdateVisuals();
}

bool FEKeyboard::IsSymbol(char character) {
    char symbols[28] = "!@#$%^&*-_=+[{]}\\|;:'\",<.>/";
    for (unsigned int i = 0; i <= 0x1B; i++) {
        if (character == symbols[i]) {
            return true;
        }
    }
    return false;
}

bool FEKeyboard::IsNotOkForEmail(char character) {
    char symbols[24] = "!#$%^&*=+[{]}\\|;'\",<>";
    for (int i = 0; i <= 0x17; i++) {
        if (character == symbols[i]) {
            return true;
        }
    }
    return false;
}

bool FEKeyboard::IsEmailSymbol(char character) {
    char symbols[5] = ":;()";
    for (int i = 0; i <= 3; i++) {
        if (character == symbols[i]) {
            return true;
        }
    }
    return false;
}

void FEKeyboard::AppendLetter(int nButton) {
    char letter = GetLetterMap(nButton);
    AppendChar(letter);
}

void FEKeyboard::AppendBackspace() {
    if (mbIsFirstKey) {
        mbIsFirstKey = false;
        mString[0] = 0;
        mnCursorIndex = 0;
    }
    int len = bStrLen(mString);
    if (len > 0 && mnCursorIndex > 0) {
        int i = mnCursorIndex - 1;
        for (; i < len; i++) {
            mString[i] = mString[i + 1];
        }
        mString[len - 1] = 0;
        mnCursorIndex = mnCursorIndex - 1;
    }
    UpdateStringVisual();
}

void FEKeyboard::ToggleSpecialCharacters() {
    bool newVal = true;
    if (mbOnSpecialCharacters == true) {
        newVal = false;
    }
    mbOnSpecialCharacters = newVal;
    if (mnMode == MODE_ALL_KEYS) {
        g_pEAXSound->PlayUISoundFX(static_cast< eMenuSoundTriggers >(0x2E));
    } else {
        mbOnSpecialCharacters = false;
    }
    UpdateVisuals();
}

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

void FEKeyboard::ToggleCapsLock() {
    if (mnMode != MODE_PROFILE_ENTRY) {
        mbCaps = mbCaps != 1;
        mbShift = false;
        if (mnMode == MODE_FILENAME) {
            mbCaps = true;
        }
        UpdateVisuals();
    }
}

void FEKeyboard::ToggleShift() {
    mbShift = mbShift != 1;
    if (mnMode == MODE_FILENAME) {
        mbShift = false;
    }
    UpdateVisuals();
}

bool FEKeyboard::IsNumericSymbol(char character) {
    char symbols[10] = "!@#$%^&*(";
    for (unsigned int i = 0; i <= 9; i++) {
        if (character == symbols[i]) {
            return true;
        }
    }
    return false;
}

void FEKeyboard::Dispose(bool bBack) {
    if (bBack) {
        bMemSet(mString, 0, 0x9c);
    }
    if (bBack == true) {
        cFEng::Get()->QueueGameMessage(mnDeclineHash, mThis->GetParentPackage()->GetName(), 0xff);
    } else {
        cFEng::Get()->QueueGameMessage(mnAcceptHash, mThis->GetParentPackage()->GetName(), 0xff);
    }
    cFEng::Get()->QueuePackagePop(1);
    gFEKeyboard = nullptr;
    KeyboardActive = false;
}

MenuScreen *CreateFEKeyboard(ScreenConstructorData *sd) {
    return new ("", 0) FEKeyboard(sd);
}
