#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int FEPrintf(FEString *text, const char *fmt, ...);
extern int FEPrintf(const char *pkg_name, FEObject *obj, const char *fmt, ...);
extern Timer RealTimer;
Timer KBCreationTimer;
extern FEKeyboard *gFEKeyboard;
extern bool KeyboardActive;
extern char FEKeyboard_mLetterMap[720] asm("_10FEKeyboard.mLetterMap");
extern FEImage *FEngFindImage(const char *pkg_name, int hash);
extern void FEngSetButtonTexture(FEImage *img, unsigned int hash);
extern FEString *FEngFindString(const char *pkg_name, int hash);
extern int FEngSNPrintf(char *buf, int size, const char *fmt, ...);
extern unsigned long FEHashUpper(const char *str);
extern void FEngSetScript(FEObject *obj, unsigned int script_hash, bool b);
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool b);
extern eLanguages GetCurrentLanguage();
extern void FEngSetLanguageHash(const char *pkg_name, unsigned int object_hash, unsigned int language_hash);
extern void FEngSNMakeHidden(char *dst, int size, const char *src);
extern void PackedStringToWideString(unsigned short *dst, int maxLen, const char *src);
extern void WideToCharString(char *dst, unsigned int maxLen, short *src);
extern void *FindFont_impl(unsigned int hash) asm("FindFont__FUi");
extern float GetLineWidth_impl(void *font, short *str, unsigned long flags, int param, bool b) asm("GetLineWidth__8FEngFontPCsUlUlb");
extern void FEngGetSize(FEObject *obj, float &w, float &h);
extern void FEngGetTopLeft(FEObject *obj, float &x, float &y);
extern void FEngSetTopLeft(FEObject *obj, float x, float y);
extern void FESetString(FEString *str, short *text);
extern int bStrLen(const unsigned short *str) asm("bStrLen__FPCUs");

FEColor FEKeyboard::ButtonHighlight(0xC8CFE9F2);
FEColor FEKeyboard::LetterHighlight(0xFFFFFFFF);
FEColor FEKeyboard::ButtonIdle(0x50549AC0);
FEColor FEKeyboard::LetterIdle(0xFF323232);

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
    AppendChar(GetLetterMap(nButton));
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
    mpInputString->Flags |= 0x400000;

    mnCursorIndex = bStrLen(mString);
    KeyboardActive = true;
    gFEKeyboard = this;
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
            mpKeyName[i]->Flags |= 0x400000;
            mpKeyNameShadow[i]->string = bla;
            mpKeyNameShadow[i]->Flags |= 0x400000;
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
        void *font = FindFont_impl(mpInputString->Format);
        unsigned long flags = mpInputString->Flags;
        unsigned int width = mpInputString->MaxWidth;
        short *fitstring = reinterpret_cast<short *>(widestring);
        for (; *fitstring != 0; fitstring += 4) {
            if (static_cast<float>(GetLineWidth_impl(font, fitstring, flags, 0, false)) <= static_cast<float>(width)) {
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
            if (static_cast<float>(GetLineWidth_impl(font, fitstring, flags, 0, false)) <= static_cast<float>(width)) {
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

void FEKeyboard::NotificationMessage(unsigned long msg, FEObject *pObject, unsigned long param1, unsigned long param2) {
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

char FEKeyboard::GetLetterMap(int nButton) {
    int c = GetCase();
    char returnChar = FEKeyboard_mLetterMap[nButton + c * 0x2D + mnLetterMapIndex * 0x5A];
    if (mnMode == 0 && mbOnSpecialCharacters) {
        c = GetCase();
        return FEKeyboard_mLetterMap[nButton + c * 0x2D + 0x276];
    }
    if (mnMode - 1U > 2) {
        if (mnMode == MODE_EMAIL) {
            if (IsEmailSymbol(returnChar) || IsNumericSymbol(returnChar)) {
                returnChar = FEKeyboard_mLetterMap[nButton + mnLetterMapIndex * 0x5A];
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
            return FEKeyboard_mLetterMap[nButton + mnLetterMapIndex * 0x5A];
        }
    }
    if (IsSymbol(returnChar)) {
        returnChar = 0;
    }
    return returnChar;
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
