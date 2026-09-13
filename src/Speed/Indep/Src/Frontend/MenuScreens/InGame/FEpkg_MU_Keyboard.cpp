#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_MU_Keyboard.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
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
    switch (msg) {
        case __PAD_LEFT__:
            g_pEAXSound->PlayUISoundFX(UISND_COMMON_LEFT);
            return;
        case __PAD_RIGHT__:
            g_pEAXSound->PlayUISoundFX(UISND_COMMON_RIGHT);
            return;
        case __PAD_UP__:
            g_pEAXSound->PlayUISoundFX(UISND_COMMON_UP);
            return;
        case __PAD_DOWN__:
            g_pEAXSound->PlayUISoundFX(UISND_COMMON_DOWN);
            return;
        case __BUTTON_PRESSED__:
            if (pObject != nullptr) {
                int nButton = IsKeyButton(pObject);
                if (nButton > -1 && GetLetterMap(nButton) != 0) {
                    g_pEAXSound->PlayUISoundFX(UISND_UGNEW_KBTYPE);
                    AppendLetter(nButton);
                    return;
                }
                g_pEAXSound->PlayUISoundFX(UISND_COMMON_WRONG);
            }
            return;
        case FEHASH_EXITCOMPLETE:
            if (bStrCmp(mString, "") == 0 && mnMode == MODE_FILENAME) {
                return;
            }
            g_pEAXSound->PlayUISoundFX(UISND_UGNEW_ENTER);
            Dispose(false);
            return;
        case 0xC1A6F000: // __SPACE_BAR__
            if (mnMode == MODE_PROFILE_ENTRY) {
                g_pEAXSound->PlayUISoundFX(UISND_COMMON_WRONG);
                return;
            }
            AppendSpace();
            g_pEAXSound->PlayUISoundFX(UISND_UGNEW_KBTYPE);
            return;
        case 0xDB3D597C: //__BACKSPACE__
            g_pEAXSound->PlayUISoundFX(UISND_UGNEW_DELETE);
            AppendBackspace();
            return;
        case 0xD7AD0DD9:
            if (mnMode == MODE_PROFILE_ENTRY && !(FEDatabase->IsOnlineMode()) && !(FEDatabase->IsLANMode())) {
                g_pEAXSound->PlayUISoundFX(UISND_COMMON_WRONG);
                return;
            }
            ToggleCapsLock();
            g_pEAXSound->PlayUISoundFX(UISND_UGNEW_DELETE);
            return;
        case __PAD_START__:
            if (bStrCmp(mString, "") == 0) {
                cFEng::Get()->QueuePackageMessage(0x8CB81F09, GetPackageName(), nullptr);
                return;
            }
            g_pEAXSound->PlayUISoundFX(UISND_UGNEW_ENTER);
            Dispose(false);
            return;
        case __PAD_LTRIGGER__:
            MoveCursor(-1);
            return;
        case __PAD_RTRIGGER__:
            MoveCursor(1);
            return;
        case __PAD_BUTTON5__:
            if (GetCurrentLanguage() == eLANGUAGE_KOREAN) {
                return;
            }
            ToggleSpecialCharacters();
            return;
        case __PAD_BACK__:
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
    mnMode = static_cast<MODE>(FEDatabase->mFEKeyboardSettings.Mode);
    mnCursorIndex = 0;

    eLanguages language = GetCurrentLanguage();
    switch (language) {
        case eLANGUAGE_GERMAN:
            mnLetterMapIndex = 2;
            break;
        case eLANGUAGE_SPANISH:
            mnLetterMapIndex = 4;
            break;
        case eLANGUAGE_ITALIAN:
        case eLANGUAGE_DUTCH:
        case eLANGUAGE_FRENCH:
            mnLetterMapIndex = language;
            break;
        case eLANGUAGE_SWEDISH:
            mnLetterMapIndex = language;
            break;
        case eLANGUAGE_ENGLISH:
        default:
            mnLetterMapIndex = 0;
            break;
    }

    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x5BC), 0x5BC);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x682), 0x682);

    mbShift = false;
    mbCaps = false;
    mbOnSpecialCharacters = false;
    mbIsFirstKey = true;
    if (mnMode == MODE_FILENAME || mnMode == MODE_PROFILE_ENTRY) {
        mbCaps = true;
    }

    mpInputString = FEngFindString(GetPackageName(), 0xADDA7E89);
    mpCursor = FEngFindObject(GetPackageName(), 0xA2DEEF46);
    mpTextBox = FEngFindImage(GetPackageName(), 0x128FDCB8);
    mpCursorTestString = reinterpret_cast<FEString *>(FEngFindObject(GetPackageName(), 0x95D7D3D2));

    char tmp[32];
    for (int i = 0; i < 45; i++) {
        uint32 letter_hash;

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
        FEngSetInvisible(GetPackageName(), 0x2C99C4E2);
        FEngSetScript(GetPackageName(), 0xDCBC8286, FEHASH_SHOW, true);
    }

    if (mnMode == MODE_PROFILE_ENTRY) {
        FEngSetScript(GetPackageName(), FEHashUpper("KEY 61 DISABLE"), FEHashUpper("SHOW"), true);
    }

    FEngSetLanguageHash(GetPackageName(), FEHASH_HEADERTEXT, 0x7F042BCD);

    mpInputString->SetString(mDisplayString);

    mnCursorIndex = bStrLen(mString);
    KeyboardActive = true;
    gFEKeyboard = this;
}

int FEKeyboard::GetCase() {
    if (mbShift) {
        return mbCaps ? 0 : 1;
    }
    return mbCaps ? 1 : 0;
}

void FEKeyboard::UpdateVisuals() {
    const u32 SHOW_SCRIPT = FEHASH_SHOW;
    const u32 HIDE_SCRIPT = FEHASH_HIDE;

    for (int i = 0; i < 45; i++) {
        char ch = GetLetterMap(i);
        if (GetCurrentLanguage() == eLANGUAGE_KOREAN) {
            i16 bla[16];
            bMemSet(bla, 0, sizeof(bla));
            bla[0] = static_cast<i16>(ch);
            mpKeyName[i]->SetString(bla);
            mpKeyNameShadow[i]->SetString(bla);
        } else {
            FEPrintf(mpKeyName[i], "%c", ch);
            FEPrintf(mpKeyNameShadow[i], "%c", ch);
        }

        bool disabled = ch == 0;
        if (!disabled) {
            if (mnMode == MODE_ALPHANUMERIC || mnMode == MODE_ALPHANUMERIC_PASSWORD || mnMode == MODE_FILENAME) {
                disabled = IsSymbol(ch);
            } else if (mnMode == MODE_EMAIL) {
                disabled = IsNotOkForEmail(ch);
            }
        }

        FEngSetScript(mpKeyDisable[i], disabled ? SHOW_SCRIPT : HIDE_SCRIPT, true);
    }

    if (GetCase()) {
        FEngSetLanguageHash(GetPackageName(), 0xDE045B20, 0x36F274D0);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xDE045B20, 0x20BC9973);
    }
    UpdateStringVisual();
}

// UNSOLVED
void FEKeyboard::UpdateStringVisual() {
    mpInputString->Flags |= 2;
    if (mnMode == MODE_ALPHANUMERIC_PASSWORD) {
        char aHidden[64];
        FEngSNMakeHidden(aHidden, sizeof(aHidden), mString);
        SetString(aHidden);
    } else {
        mnWindowStartIdx = 0;
        uint16 widestring[156];
        PackedStringToWideString(widestring, sizeof(widestring), mString);
        FEngFont *font = FindFont(mpInputString->Format);
        int width = mpInputString->MaxWidth;
        int flags = mpInputString->Flags;
        short *fitstring = reinterpret_cast<short *>(widestring);
        while (*fitstring != 0) {
            if (font->GetLineWidth(fitstring, flags, 0, false) <= static_cast<float>(width)) {
                break;
            }
            fitstring += 4;
            mnWindowStartIdx += 4;
        }

        while (mnCursorIndex < mnWindowStartIdx) {
            if (mnWindowStartIdx - 4 < 0) {
                mnWindowStartIdx = 0;
                fitstring = reinterpret_cast<short *>(widestring);
            } else {
                mnWindowStartIdx = mnWindowStartIdx - 4;
                fitstring = fitstring - 4;
            }
        }

        while (*fitstring != 0) {
            if (font->GetLineWidth(fitstring, flags, 0, false) <= static_cast<float>(width)) {
                break;
            }
            widestring[bStrLen(widestring) - 1] = 0;
        }

        FESetString(mpInputString, fitstring);
    }
    UpdateCursorPosition();
}

void FEKeyboard::UpdateCursorPosition() {
    mpCursorTestString->Flags |= 2;
    i16 *pSrc = mpInputString->GetString();
    char pBuf[150];
    bMemSet(pBuf, 0, sizeof(pBuf));
    WideToCharString(pBuf, static_cast<unsigned int>(mnCursorIndex - mnWindowStartIdx + 1), pSrc);
    FEPrintf(GetPackageName(), mpCursorTestString, "%s", pBuf);
    float width = FEngGetSizeX(mpCursorTestString);
    int maxLineWidth;
    float string_x = mpInputString->GetObjData()->Pos.x;
    float cursor_x = string_x + width;
    FEngSetTopLeftX(mpCursor, cursor_x);
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
    if (nLength > 156) {
        nLength = 156;
    }
    mnMaxLength = nLength;
}

int FEKeyboard::IsKeyButton(FEObject *pObject) {
    for (int i = 0; i < 45; i++) {
        if (mpKeyButton[i] == pObject) {
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

// UNSOLVED
char FEKeyboard::GetLetterMap(int nButton) {
    char returnChar = mLetterMap[mnLetterMapIndex][GetCase()][nButton];
    if (mnMode == MODE_ALL_KEYS && mbOnSpecialCharacters) {
        returnChar = mLetterMap[7][GetCase()][nButton];
    } else {
        if (mnMode - 1U > 2) {
            if (mnMode == MODE_EMAIL) {
                if (IsEmailSymbol(returnChar) || IsNumericSymbol(returnChar)) {
                    returnChar = mLetterMap[mnLetterMapIndex][0][nButton];
                }
                if (IsNotOkForEmail(returnChar)) {
                    return 0;
                }
            } else if (mnMode == MODE_PROFILE_ENTRY) {
                if (!IsNumericSymbol(returnChar)) {
                    if (IsSymbol(returnChar)) {
                        return 0;
                    }
                } else {
                    returnChar = mLetterMap[mnLetterMapIndex][0][nButton];
                }
            }
        }
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
    if (!ch) {
        return;
    }
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

// UNSOLVED
void FEKeyboard::ToggleCapsLock() {
    if (mnMode != MODE_PROFILE_ENTRY) {
        mbCaps = mbCaps != true;
        mbShift = false;
        if (mnMode == MODE_FILENAME) {
            mbCaps = true;
        }
        UpdateVisuals();
    }
}

void FEKeyboard::ToggleShift() {
    mbShift = mbShift != true;
    if (mnMode == MODE_FILENAME) {
        mbShift = false;
    }
    UpdateVisuals();
}

void FEKeyboard::ToggleSpecialCharacters() {
    mbOnSpecialCharacters = mbOnSpecialCharacters == true ? false : true;
    if (mnMode == MODE_ALL_KEYS) {
        extern EAXSound *g_pEAXSound;
        g_pEAXSound->PlayUISoundFX(UISND_UGNEW_KBTYPE);
    } else {
        mbOnSpecialCharacters = false;
    }
    UpdateVisuals();
}
