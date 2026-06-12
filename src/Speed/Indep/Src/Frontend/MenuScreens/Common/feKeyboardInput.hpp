#ifndef __KEYBOARD_INPUT_HPP__
#define __KEYBOARD_INPUT_HPP__

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"

#define FEMSG_ACCEPT_EDITED_TEXT 0xda5b8712  // :17
#define FEMSG_DECLINE_EDITED_TEXT 0xc9d30688 // :18

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

extern KeyboardEditString
    gKeyboardManager; // size: 0x418, address: 0x80473360, Decl: speed/indep/src/frontend/menuscreens/common/feKeyboardInput.hpp:97

#endif
