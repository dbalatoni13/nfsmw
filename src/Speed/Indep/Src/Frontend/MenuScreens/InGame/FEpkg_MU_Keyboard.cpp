#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_MU_Keyboard.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

Timer KBCreationTimer;
extern FEKeyboard *gFEKeyboard;
extern bool KeyboardActive;

MenuScreen *CreateFEKeyboard(ScreenConstructorData *sd) {
    return new ("FEKeyboard", 0) FEKeyboard(sd);
}

FEKeyboard::FEKeyboard(ScreenConstructorData *sd) : MenuScreen(sd) {
    mnWindowStartIdx = 0;
    mThis = sd->pPackage;
    KBCreationTimer = RealTimer;
    Initialize();
    UpdateVisuals();
}

// STRIPPED
void FEKeyboard::ShowModal(const char *pstrParent, MODE nMode, u32 nAcceptHash, int nDeclineHash, int nMaxLength, u32 nDefaultTextHash) {}

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

void FEKeyboard::NotificationMessage(u32 msg, FEObject *pObject, u32 param1, u32 param2) {
    int nButton = -1;
    switch (msg) {
        case 0x9120409E:
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(2));
            return;
        case 0xB5971BF1:
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(3));
            return;
        case 0x72619778:
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0));
            return;
        case 0x911C0A4B:
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(1));
            return;
        case 0xC407210:
            if (!pObject) {
                return;
            }
            nButton = IsKeyButton(pObject);
            if (nButton > -1 && GetLetterMap(nButton) != 0) {
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x2E));
                AppendLetter(nButton);
                return;
            }
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(7));
            return;
        case 0xE1FDE1D1:
            if (bStrCmp(mString, "") == 0 && mnMode == MODE_FILENAME) {
                return;
            }
            goto dispose_keyboard;
        case 0xC1A6F000:
            if (mnMode == MODE_PROFILE_ENTRY) {
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(7));
                return;
            }
            AppendSpace();
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x2E));
            return;
        case 0xDB3D597C:
        play_backspace_sound:
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x30));
            AppendBackspace();
            return;
        case 0xD7AD0DD9:
            if (mnMode == MODE_PROFILE_ENTRY && !(FEDatabase->GetGameMode() & 8) && !(FEDatabase->GetGameMode() & 0x40)) {
                g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(7));
                return;
            }
            ToggleCapsLock();
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x30));
            return;
        case 0xB5AF2461:
            if (bStrCmp(mString, "") == 0) {
                cFEng::Get()->QueuePackageMessage(0x8CB81F09, GetPackageName(), nullptr);
                return;
            }
            goto dispose_keyboard;
        dispose_keyboard:
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x2F));
            Dispose(false);
            return;
        case 0x5073EF13:
            MoveCursor(-1);
            return;
        case 0xD9FEEC59:
            MoveCursor(1);
            return;
        case 0xC519BFC4:
            if (GetCurrentLanguage() == eLANGUAGE_KOREAN) {
                return;
            }
            ToggleSpecialCharacters();
            return;
        case 0x911AB364:
            if (mnDeclineHash == -1U) {
                return;
            }
            Dispose(true);
            return;
        default:
            return;
    }
}

void FEKeyboard::Initialize() {
    mString = FEDatabase->mFEKeyboardSettings.Buffer;
    SetMaxLength(FEDatabase->mFEKeyboardSettings.MaxTextLength);
    mnAcceptHash = FEDatabase->mFEKeyboardSettings.AcceptCallbackHash;
    mnDeclineHash = FEDatabase->mFEKeyboardSettings.DeclineCallbackHash;
    mnCursorIndex = 0;
    mnMode = static_cast<MODE>(FEDatabase->mFEKeyboardSettings.Mode);

    eLanguages language = static_cast<eLanguages>(GetCurrentLanguage());
    switch (language) {
        case eLANGUAGE_ITALIAN:
        case eLANGUAGE_DUTCH:
        case eLANGUAGE_SWEDISH:
            mnLetterMapIndex = language;
            break;
        case eLANGUAGE_FRENCH:
            mnLetterMapIndex = language;
            break;
        case eLANGUAGE_GERMAN:
            mnLetterMapIndex = 2;
            break;
        case eLANGUAGE_SPANISH:
            mnLetterMapIndex = 4;
            break;
        default:
            mnLetterMapIndex = 0;
            break;
    }

    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x5BC), 0x5BC);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x682), 0x682);

    mbOnSpecialCharacters = false;
    mbIsFirstKey = true;
    mbShift = false;
    mbCaps = false;
    if (mnMode == MODE_FILENAME || mnMode == MODE_PROFILE_ENTRY) {
        mbCaps = true;
    }

    mpInputString = FEngFindString(GetPackageName(), 0xADDA7E89);
    mpCursor = FEngFindObject(GetPackageName(), 0xA2DEEF46);
    mpTextBox = FEngFindImage(GetPackageName(), 0x128FDCB8);
    mpCursorTestString = reinterpret_cast<FEString *>(FEngFindObject(GetPackageName(), 0x95D7D3D2));

    char tmp[32];
    for (int i = 0; i < 0x2D; i++) {
        unsigned int letter_hash;

        FEngSNPrintf(tmp, 0x20, "KEYNUM %.2d", i + 1);
        letter_hash = FEHashUpper(tmp);
        mpKeyName[i] = reinterpret_cast<FEString *>(FEngFindObject(GetPackageName(), letter_hash));

        FEngSNPrintf(tmp, 0x20, "KEYNUM SHADOW %.2d", i + 1);
        letter_hash = FEHashUpper(tmp);
        mpKeyNameShadow[i] = reinterpret_cast<FEString *>(FEngFindObject(GetPackageName(), letter_hash));

        FEngSNPrintf(tmp, 0x20, "KEY %.2d", i + 1);
        letter_hash = FEHashUpper(tmp);
        mpKeyButton[i] = FEngFindObject(GetPackageName(), letter_hash);

        FEngSNPrintf(tmp, 0x20, "KEY %.2d DISABLE", i + 1);
        letter_hash = FEHashUpper(tmp);
        mpKeyDisable[i] = FEngFindObject(GetPackageName(), letter_hash);
    }

    if (mnMode != MODE_ALL_KEYS || GetCurrentLanguage() == eLANGUAGE_KOREAN) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x2C99C4E2));
        FEngSetScript(GetPackageName(), 0xDCBC8286, 0x1CA7C0, true);
    }

    if (mnMode == MODE_PROFILE_ENTRY) {
        unsigned int disableHash = FEHashUpper("KEY 61 DISABLE");
        unsigned int showHash = FEHashUpper("SHOW");
        FEngSetScript(GetPackageName(), disableHash, showHash, true);
    }

    FEngSetLanguageHash(GetPackageName(), 0x42ADB44C, 0x7F042BCD);

    mpInputString->string = mDisplayString;
    mpInputString->Flags |= FF_DirtyCode;

    mnCursorIndex = bStrLen(mString);
    KeyboardActive = true;
    gFEKeyboard = this;
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

void FEKeyboard::UpdateVisuals() {
    const unsigned long SHOW_SCRIPT = 0x16A259;
    const unsigned long HIDE_SCRIPT = 0x1CA7C0;

    for (int i = 0; i < 0x2D; i++) {
        char ch = GetLetterMap(i);
        if (GetCurrentLanguage() == eLANGUAGE_KOREAN) {
            short bla[16];
            bMemSet(bla, 0, 0x20);
            bla[0] = static_cast<short>(ch);
            mpKeyName[i]->string = bla;
            mpKeyName[i]->Flags |= FF_DirtyCode;
            mpKeyNameShadow[i]->string = bla;
            mpKeyNameShadow[i]->Flags |= FF_DirtyCode;
        } else {
            FEPrintf(mpKeyName[i], "%c", ch);
            FEPrintf(mpKeyNameShadow[i], "%c", ch);
        }

        bool disabled = ch == 0;
        if (!disabled) {
            if (mnMode - 1U < 3) {
                disabled = IsSymbol(ch);
            } else if (mnMode == MODE_EMAIL) {
                disabled = IsNotOkForEmail(ch);
            }
        }

        unsigned long script = SHOW_SCRIPT;
        if (disabled) {
            script = HIDE_SCRIPT;
        }
        FEngSetScript(mpKeyDisable[i], script, true);
    }

    if (GetCase() == 0) {
        FEngSetLanguageHash(GetPackageName(), 0xDE045B20, 0x20BC9973);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xDE045B20, 0x36F274D0);
    }
    UpdateStringVisual();
}

void FEKeyboard::UpdateStringVisual() {
    mpInputString->Flags |= 2;
    if (mnMode == MODE_ALPHANUMERIC_PASSWORD) {
        char aHidden[64];
        FEngSNMakeHidden(aHidden, 0x40, mString);
        SetString(aHidden);
    } else {
        mnWindowStartIdx = 0;
        unsigned short widestring[156];
        PackedStringToWideString(widestring, 0x9C, mString);
        FEngFont *font = FindFont(mpInputString->Format);
        unsigned long flags = mpInputString->Flags;
        unsigned int width = mpInputString->MaxWidth;
        short *fitstring = reinterpret_cast<short *>(widestring);
        for (; *fitstring != 0; fitstring += 4) {
            if (font->GetLineWidth(fitstring, flags, 0, false) <= static_cast<float>(width)) {
                break;
            }
            mnWindowStartIdx = mnWindowStartIdx + 4;
        }

        int wsi = mnWindowStartIdx;
        while (mnCursorIndex < wsi) {
            if (wsi - 4 < 0) {
                mnWindowStartIdx = 0;
                fitstring = reinterpret_cast<short *>(widestring);
            } else {
                mnWindowStartIdx = wsi - 4;
                fitstring = fitstring - 4;
            }
            wsi = mnWindowStartIdx;
        }

        while (*fitstring != 0) {
            if (font->GetLineWidth(fitstring, flags, 0, false) <= static_cast<float>(width)) {
                break;
            }
            int wlen = bStrLen(widestring);
            widestring[wlen - 1] = 0;
        }

        FESetString(mpInputString, fitstring);
    }
    UpdateCursorPosition();
}

void FEKeyboard::UpdateCursorPosition() {
    mpCursorTestString->Flags |= 2;
    short *pSrc = mpInputString->string;
    char pBuf[150];
    bMemSet(pBuf, 0, 0x96);
    WideToCharString(pBuf, static_cast<unsigned int>(mnCursorIndex - mnWindowStartIdx + 1), pSrc);
    FEPrintf(GetPackageName(), mpCursorTestString, "%s", pBuf);
    float width;
    float h;
    FEngGetSize(mpCursorTestString, width, h);
    float string_x = reinterpret_cast<float *>(mpInputString->pData)[7];
    float cursor_x;
    float y;
    FEngGetTopLeft(mpCursor, cursor_x, y);
    FEngSetTopLeft(mpCursor, string_x + width, y);
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

// STRIPPED
void FEKeyboard::HighlightButton(int nButton, bool bHighlight) {}

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
    for (int i = 0; i < 0x2D; i++) {
        if (mpKeyButton[i] == pObj) {
            return i;
        }
    }
    return -1;
}

bool FEKeyboard::IsSymbol(char character) {
    char symbols[28] = {'-', '=', '/', '?', '\\', ';', '\'', ',', '.', '!', '@', '#',  '~', '%',
                        '`', '&', '*', '(', ')',  '_', '+',  '[', ']', '|', ':', '\"', '<', '>'};
    for (int i = 0; i < sizeof(symbols); i++) {
        if (character == symbols[i]) {
            return true;
        }
    }
    return false;
}

bool FEKeyboard::IsNotOkForEmail(char character) {
    char symbols[24] = {'=', '/', '?', '\\', ';', '\'', ',', '!', '#', '~', '%', '`', '&', '*', '(', ')', '+', '[', ']', '|', ':', '\"', '<', '>'};
    for (int i = 0; i < sizeof(symbols); i++) {
        if (character == symbols[i]) {
            return true;
        }
    }
    return false;
}

bool FEKeyboard::IsNumericSymbol(char character) {
    char symbols[10] = {'/', '\\', '|', '`', '~', '#', '%', '+', '*', '='};
    for (int i = 0; i < sizeof(symbols); i++) {
        if (character == symbols[i]) {
            return true;
        }
    }
    return false;
}

bool FEKeyboard::IsEmailSymbol(char character) {
    char symbols[4] = {':', ';', '(', ')'};
    for (int i = 0; i < sizeof(symbols); i++) {
        if (character == symbols[i]) {
            return true;
        }
    }
    return false;
}

void FEKeyboard::AppendLetter(int nButton) {
    AppendChar(GetLetterMap(nButton));
}

char FEKeyboard::GetLetterMap(int nButton) {
    char returnChar = mLetterMap[mnLetterMapIndex][GetCase()][nButton];
    if (mnMode == MODE_ALL_KEYS && mbOnSpecialCharacters) {
        return mLetterMap[7][GetCase()][nButton];
    }
    if (mnMode - 1U > 2) {
        if (mnMode == MODE_EMAIL) {
            if (IsEmailSymbol(returnChar) || IsNumericSymbol(returnChar)) {
                returnChar = mLetterMap[mnLetterMapIndex][0][nButton];
            }
            if (IsNotOkForEmail(returnChar)) {
                returnChar = 0;
            }
            return returnChar;
        }
        if (mnMode != MODE_PROFILE_ENTRY) {
            return returnChar;
        }
        if (IsNumericSymbol(returnChar)) {
            return mLetterMap[mnLetterMapIndex][0][nButton];
        }
    }
    if (IsSymbol(returnChar)) {
        returnChar = 0;
    }
    return returnChar;
}

void FEKeyboard::AppendSpace() {
    if (mnMode == MODE_ALL_KEYS) {
        AppendChar(0x20);
    }
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

void FEKeyboard::AppendChar(char ch) {
    if (ch) {
        if (mbIsFirstKey) {
            mbIsFirstKey = false;
            mString[0] = 0;
            mnCursorIndex = 0;
        }
        int len = bStrLen(mString);
        if (len < mnMaxLength) {
            int i = 0x9A;
            for (; i > mnCursorIndex; i--) {
                mString[i] = mString[i - 1];
            }
            mString[mnCursorIndex] = ch;
            mString[0x9B] = 0;
            mnCursorIndex = mnCursorIndex + 1;
            if (mnCursorIndex > mnMaxLength) {
                mnCursorIndex = mnMaxLength;
            }
            UpdateStringVisual();
        }
        if (mbShift && ch != 0x20) {
            ToggleShift();
        }
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

void FEKeyboard::ToggleSpecialCharacters() {
    bool newVal = true;
    if (mbOnSpecialCharacters == true) {
        newVal = false;
    }
    mbOnSpecialCharacters = newVal;
    if (mnMode == MODE_ALL_KEYS) {
        g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x2E));
    } else {
        mbOnSpecialCharacters = false;
    }
    UpdateVisuals();
}
