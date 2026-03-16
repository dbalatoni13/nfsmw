#ifndef FRONTEND_MENUSCREENS_COMMON_FEKEYBOARDINPUT_H
#define FRONTEND_MENUSCREENS_COMMON_FEKEYBOARDINPUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct FEPackage;
struct FEString;

// total size: 0xC
struct FEngTextInputObject {
    FEString *DisplayString;              // offset 0x0, size 0x4
    MenuScreen *ParentPackage;            // offset 0x4, size 0x4
    int mBlinkTime;                       // offset 0x8, size 0x4
    static int sCursorBlinkCycleTime;

    FEngTextInputObject(MenuScreen *pkg, FEString *obj, unsigned int mode, const char *start_string,
                        unsigned int max_text_length);
    ~FEngTextInputObject();
    void Notify(unsigned int msg);
    void ReturnPressed();
    void EscapePressed();
    void RedrawString(bool pIncludeCursor);
    char *GetEditedString();
};

// total size: 0x418
struct KeyboardEditString {
    char InitialString[256];              // offset 0x0, size 0x100
    unsigned short EditStringUCS2[256];   // offset 0x100, size 0x200
    int CursorPosUCS2;                    // offset 0x300, size 0x4
    char EditStringPacked[256];           // offset 0x304, size 0x100
    unsigned int ModeFlags;               // offset 0x404, size 0x4
    int KeysProcessed;                    // offset 0x408, size 0x4
    int MaxTextLength;                    // offset 0x40C, size 0x4
    bool mEnabled;                        // offset 0x410, size 0x1
    FEngTextInputObject *TextInputObject; // offset 0x414, size 0x4

    bool IsCapturing() {
        return mEnabled && TextInputObject != nullptr;
    }

    KeyboardEditString();
    void SyncEditIntoPacked();
    char *GetEditedString();
    void EndCapture();
    unsigned int GetModeFlags() { return ModeFlags; }

    void GetStringForDisplay(char *buffer, int size);
    void RevertToOriginalString();
};

extern KeyboardEditString gKeyboardManager;

// total size: 0x360
struct FEKeyboard : public MenuScreen {
    enum MODE {
        MODE_ALL_KEYS = 0,
        MODE_ALPHANUMERIC = 1,
        MODE_ALPHANUMERIC_PASSWORD = 2,
        MODE_FILENAME = 3,
        MODE_EMAIL = 4,
        MODE_PROFILE_ENTRY = 5,
    };

    static FEColor ButtonHighlight;
    static FEColor LetterHighlight;
    static FEColor ButtonIdle;
    static FEColor LetterIdle;

    FEKeyboard(ScreenConstructorData *sd);
    ~FEKeyboard() override {}

    void NotificationMessage(unsigned long msg, FEObject *pObject, unsigned long param1, unsigned long param2) override;

    void SetString(char *pStr);
    void SetMaxLength(int nLength);
    int IsKeyButton(FEObject *pObj);
    void AppendSpace();
    void AppendChar(char c);
    void Initialize();
    void UpdateVisuals();
    void UpdateStringVisual();
    void MoveCursor(int nDelta);
    bool IsSymbol(char character);
    bool IsNotOkForEmail(char character);
    bool IsEmailSymbol(char character);
    void AppendBackspace();
    void ToggleSpecialCharacters();
    int GetCase();
    void AppendLetter(int nButton);
    char GetLetterMap(int nButton);
    bool IsNumericSymbol(char character);
    void UpdateCursorPosition();
    void ToggleCapsLock();
    void ToggleShift();
    void Dispose(bool bFreeMem);

    int mnLetterMapIndex;           // offset 0x2C
    int mnCursorIndex;              // offset 0x30
    int mnMaxLength;                // offset 0x34
    bool mbIsFirstKey;              // offset 0x38
    bool mbShift;                   // offset 0x3C
    bool mbCaps;                    // offset 0x40
    bool mbOnSpecialCharacters;     // offset 0x44
    FEString *mpInputString;        // offset 0x48
    FEObject *mpCursor;             // offset 0x4C
    FEImage *mpTextBox;             // offset 0x50
    FEString *mpKeyName[45];        // offset 0x54
    FEString *mpKeyNameShadow[45];  // offset 0x108
    FEObject *mpKeyButton[45];      // offset 0x1BC
    FEObject *mpKeyDisable[45];     // offset 0x270
    char *mString;                  // offset 0x324
    FEPackage *mThis;               // offset 0x328
    unsigned long mnAcceptHash;     // offset 0x32C
    unsigned long mnDeclineHash;    // offset 0x330
    MODE mnMode;                    // offset 0x334
    int mnWindowStartIdx;           // offset 0x338
    FEString *mpCursorTestString;   // offset 0x33C
    char mDisplayString[31];        // offset 0x340
};

#endif
