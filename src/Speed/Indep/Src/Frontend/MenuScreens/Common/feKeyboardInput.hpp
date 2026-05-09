#ifndef FRONTEND_MENUSCREENS_COMMON_FEKEYBOARDINPUT_H
#define FRONTEND_MENUSCREENS_COMMON_FEKEYBOARDINPUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/feimage.h"

// total size: 0xC
class FEngTextInputObject {
  private:
    void DrawCursor();
    void RedrawString(bool pIncludeCursor);

  public:
    FEngTextInputObject(MenuScreen *pkg, FEString *obj, uint32 mode, const char *start_string, uint32 max_text_length);
    ~FEngTextInputObject();
    void Notify(uint32 msg);
    void ReturnPressed();
    void EscapePressed();
    char *GetEditedString();

  private:
    FEString *DisplayString;   // offset 0x0, size 0x4
    MenuScreen *ParentPackage; // offset 0x4, size 0x4
    int mBlinkTime;            // offset 0x8, size 0x4
    static int sCursorBlinkCycleTime;
};

// total size: 0x418
class KeyboardEditString {
    friend class FEngTextInputObject;

  private:
    bool AddChar();
    bool PassesThroughFilter();
    uint32 GetModeFlags() {
        return ModeFlags;
    }

  public:
    KeyboardEditString();
    void StartCapture(FEngTextInputObject *txt, uint32 mode, const char *start_string, uint32 max_text_length);
    bool IsCapturing() {
        return mEnabled && TextInputObject != nullptr;
    }
    void EndCapture();
    void Enable() {}
    void Disable() {}
    void NotifyChar(int keycode);
    void ProcessHotkeys(int keycode);
    void CursorLeft();
    void CursorRight();
    void CursorHome();
    void CursorEnd();
    void GetStringForDisplay(char *buffer, int size);
    void RevertToOriginalString();
    void SyncEditIntoPacked();
    char *GetEditedString();

  private:
    char InitialString[256];              // offset 0x0, size 0x100
    uint16 EditStringUCS2[256];           // offset 0x100, size 0x200
    int CursorPosUCS2;                    // offset 0x300, size 0x4
    char EditStringPacked[256];           // offset 0x304, size 0x100
    uint32 ModeFlags;                     // offset 0x404, size 0x4
    int KeysProcessed;                    // offset 0x408, size 0x4
    int MaxTextLength;                    // offset 0x40C, size 0x4
    bool mEnabled;                        // offset 0x410, size 0x1
    FEngTextInputObject *TextInputObject; // offset 0x414, size 0x4
};

extern KeyboardEditString gKeyboardManager;

// total size: 0x360
class FEKeyboard : public MenuScreen {
  public:
    enum MODE {
        MODE_ALL_KEYS = 0,
        MODE_ALPHANUMERIC = 1,
        MODE_ALPHANUMERIC_PASSWORD = 2,
        MODE_FILENAME = 3,
        MODE_EMAIL = 4,
        MODE_PROFILE_ENTRY = 5,
    };

    FEKeyboard(ScreenConstructorData *sd);
    ~FEKeyboard() override {}
    static void ShowModal(const char *pstrParent, MODE nMode, u32 nAcceptHash, int nDeclineHash, int nMaxLength, u32 nDefaultTextHash);
    void NotificationMessage(u32 msg, FEObject *pObject, u32 param1, u32 param2) override;
    void AppendChar(char c);
    void AppendSpace();
    void AppendBackspace();
    void MoveCursor(int nDelta);
    void Dispose(bool bFreeMem);

  private:
    void SetString(char *pStr);
    void SetMaxLength(int nLength);
    void Initialize();
    void UpdateVisuals();
    void UpdateStringVisual();
    void UpdateCursorPosition();
    int IsKeyButton(FEObject *pObj);
    int IsControlButton(FEObject *pObj);
    bool IsSymbol(char character);
    bool IsNumericSymbol(char character);
    bool IsEmailSymbol(char character);
    bool IsNotOkForEmail(char character);
    void AppendLetter(int nButton);
    char GetLetterMap(int nButton);
    void HighlightButton(int nButton, bool bHighlight);
    void ToggleSpecialCharacters();
    void ToggleCapsLock();
    void ToggleShift();
    int GetCase();

    static const char *KeyboardPackage; // size: 0x4, address: 0xFFFFFFFF
  private:
    static const struct FEColor ButtonHighlight; // size: 0x10, address: 0x80473028
    static const struct FEColor LetterHighlight; // size: 0x10, address: 0x80473038
    static const struct FEColor ButtonIdle;      // size: 0x10, address: 0x80473048
    static const struct FEColor LetterIdle;      // size: 0x10, address: 0x80473058

    int mnLetterMapIndex;                   // offset 0x2C
    int mnCursorIndex;                      // offset 0x30
    int mnMaxLength;                        // offset 0x34
    bool mbIsFirstKey;                      // offset 0x38
    bool mbShift;                           // offset 0x3C
    bool mbCaps;                            // offset 0x40
    bool mbOnSpecialCharacters;             // offset 0x44
    static const char mLetterMap[8][2][45]; // size: 0x2D0, address: 0x803E59E5
    static const char mEmailSymbols[45];    // size: 0x2D, address: 0xFFFFFFFF
    FEString *mpInputString;                // offset 0x48
    FEObject *mpCursor;                     // offset 0x4C
    FEImage *mpTextBox;                     // offset 0x50
    FEString *mpKeyName[45];                // offset 0x54
    FEString *mpKeyNameShadow[45];          // offset 0x108
    FEObject *mpKeyButton[45];              // offset 0x1BC
    FEObject *mpKeyDisable[45];             // offset 0x270
    char *mString;                          // offset 0x324
    FEPackage *mThis;                       // offset 0x328
    u32 mnAcceptHash;                       // offset 0x32C
    u32 mnDeclineHash;                      // offset 0x330
    MODE mnMode;                            // offset 0x334
    int mnWindowStartIdx;                   // offset 0x338
    FEString *mpCursorTestString;           // offset 0x33C
    char mDisplayString[31];                // offset 0x340
    static const int WindowSize;            // size: 0x4, address: 0xFFFFFFFF
    static const int WindowSkipSize;        // size: 0x4, address: 0xFFFFFFFF
};

#endif
