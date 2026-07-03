#ifndef __FEPKG_KEYBOARD_HPP__
#define __FEPKG_KEYBOARD_HPP__

#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp
// total size: 0x360
// Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:25
class FEKeyboard : public MenuScreen {
  public:
    typedef enum {
        MODE_ALL_KEYS = 0,
        MODE_ALPHANUMERIC = 1,
        MODE_ALPHANUMERIC_PASSWORD = 2,
        MODE_FILENAME = 3,
        MODE_EMAIL = 4,
        MODE_PROFILE_ENTRY = 5
    } MODE;

    FEKeyboard(ScreenConstructorData *sd);
    ~FEKeyboard() override {} // Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:29

    static void ShowModal(const char *pstrParent, MODE nMode, u32 nAcceptHash, int nDeclineHash, int nMaxLength, u32 nDefaultTextHash);

    void NotificationMessage(u32 msg, FEObject *pObject, u32 param1, u32 param2) override;

    void AppendChar(char ch);

    void AppendSpace();

    void AppendBackspace();

    void MoveCursor(int nDelta);

    void Dispose(bool bBack);

  private:
    void SetString(char *pStr);

    void SetMaxLength(int nLength);

    void Initialize();

    void UpdateVisuals();

    void UpdateStringVisual();

    void UpdateCursorPosition();

    int IsKeyButton(FEObject *pObject);

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

  public:
    static const char *KeyboardPackage; // size: 0x4, address: 0xFFFFFFFF

  private:
    static const FEColor
        ButtonHighlight; // size: 0x10, address: 0x80473028, Decl: speed/indep/src/frontend/menuscreens/ingame/FEpkg_MU_Keyboard.cpp:171
    static const FEColor
        LetterHighlight;             // size: 0x10, address: 0x80473038, Decl: speed/indep/src/frontend/menuscreens/ingame/FEpkg_MU_Keyboard.cpp:172
    static const FEColor ButtonIdle; // size: 0x10, address: 0x80473048, Decl: speed/indep/src/frontend/menuscreens/ingame/FEpkg_MU_Keyboard.cpp:173
    static const FEColor LetterIdle; // size: 0x10, address: 0x80473058, Decl: speed/indep/src/frontend/menuscreens/ingame/FEpkg_MU_Keyboard.cpp:174

    int mnLetterMapIndex;       // offset 0x2C, size 0x4, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:100
    int mnCursorIndex;          // offset 0x30, size 0x4
    int mnMaxLength;            // offset 0x34, size 0x4
    bool mbIsFirstKey;          // offset 0x38, size 0x1
    bool mbShift;               // offset 0x3C, size 0x1, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:102
    bool mbCaps;                // offset 0x40, size 0x1, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:103
    bool mbOnSpecialCharacters; // offset 0x44, size 0x1, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:104
    FEString *mpInputString;    // offset 0x48, size 0x4
    FEObject *mpCursor;         // offset 0x4C, size 0x4
    FEImage *mpTextBox;         // offset 0x50, size 0x4

    static const char mLetterMap[8][2]
                                [45]; // size: 0x2D0, address: 0x803E59E5, Decl: speed/indep/src/frontend/menuscreens/ingame/FEpkg_MU_Keyboard.cpp:32
    static const char
        mEmailSymbols[45]; // size: 0x2D, address: 0xFFFFFFFF, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:107

    FEString *mpKeyName[45];       // offset 0x54, size 0xB4, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:109
    FEString *mpKeyNameShadow[45]; // offset 0x108, size 0xB4, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:110
    FEObject *mpKeyButton[45];     // offset 0x1BC, size 0xB4, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:111
    FEObject *mpKeyDisable[45];    // offset 0x270, size 0xB4, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:112
    char *mString;                 // offset 0x324, size 0x4
    FEPackage *mThis;              // offset 0x328, size 0x4, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:114
    u32 mnAcceptHash;              // offset 0x32C, size 0x4
    u32 mnDeclineHash;             // offset 0x330, size 0x4
    MODE mnMode;                   // offset 0x334, size 0x4

    static const int WindowSize;     // size: 0x4, address: 0xFFFFFFFF
    static const int WindowSkipSize; // size: 0x4, address: 0xFFFFFFFF
    int mnWindowStartIdx;            // offset 0x338, size 0x4
    FEString *mpCursorTestString;    // offset 0x33C, size 0x4
    char mDisplayString[31];         // offset 0x340, size 0x1F
};

const FEColor FEKeyboard::ButtonHighlight(0xC8CFE9F2); // size: 0x10, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:87

const FEColor FEKeyboard::LetterHighlight(0xFFFFFFFF); // size: 0x10, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:88

const FEColor
    FEKeyboard::ButtonIdle(0x50549AC0); // size: 0x10, address: 0x80473048, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:89

const FEColor
    FEKeyboard::LetterIdle(0xFF323232); // size: 0x10, address: 0x80473058, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:90

const char FEKeyboard::mLetterMap[8][2][45] = {
    {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '_', '-', '.', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', '?', '!', 'n', 'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ',', '&'},
     {'/', '\\', '|', '`', '~', '#', '%', '+', '*', '=', '\"', ':', ';', '(', ')', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J',  'K', 'L', 'M', '<', '>', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']'}},
    {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '_', '-', '.', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', '?', '!', 'n', 'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ',', '&'},
     {'/', '\\', '|', '`', '~', '#', '%', '+', '*', '=', '\"', ':', ';', '(', ')', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J',  'K', 'L', 'M', '<', '>', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']'}},
    {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '_', '-', '.', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', '?', '!', 'n', 'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ',', '&'},
     {'/', '\\', '|', '`', '~', '#', '%', '+', '*', '=', '\"', ':', ';', '(', ')', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J',  'K', 'L', 'M', '<', '>', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']'}},
    {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '_', '-', '.', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', '?', '!', 'n', 'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ',', '&'},
     {'/', '\\', '|', '`', '~', '#', '%', '+', '*', '=', '\"', ':', ';', '(', ')', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J',  'K', 'L', 'M', '<', '>', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']'}},
    {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '_', '-', '.', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', '?', '!', 'n', 'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ',', '&'},
     {'/', '\\', '|', '`', '~', '#', '%', '+', '*', '=', '\"', ':', ';', '(', ')', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J',  'K', 'L', 'M', '<', '>', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']'}},
    {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '_', '-', '.', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', '?', '!', 'n', 'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ',', '&'},
     {'/', '\\', '|', '`', '~', '#', '%', '+', '*', '=', '\"', ':', ';', '(', ')', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J',  'K', 'L', 'M', '<', '>', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']'}},
    {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '_', '-', '.', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', '?', '!', 'n', 'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ',', '&'},
     {'/', '\\', '|', '`', '~', '#', '%', '+', '*', '=', '\"', ':', ';', '(', ')', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J',  'K', 'L', 'M', '<', '>', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ']'}},
    {{0xFA, 0xF9, 0xFB, 0xFC, 0xDF, 0xA1, 0xBF, 0,    0,    0,    0,    0,    0,    0,    0,    0xE1, 0xE0, 0xE2, 0xE5, 0xE4, 0xE6, 0xE7, 0xE9,
      0xE8, 0xEA, 0xEB, 0,    0,    0,    0,    0xED, 0xEC, 0xEE, 0xEF, 0xF1, 0xF8, 0xF3, 0xF2, 0xF4, 0xF6, 0x9C, 0,    0,    0,    0},
     {0xDA, 0xD9, 0xDB, 0xDC, 0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0xC1, 0xC0, 0xC2, 0xC5, 0xC4, 0xC6, 0xC7, 0xC9,
      0xC8, 0xCA, 0xCB, 0,    0, 0, 0, 0xCD, 0xCC, 0xCE, 0xCF, 0xD1, 0xD8, 0xD3, 0xD2, 0xD4, 0xD6, 0,    0,    0,    0,    0}}};
; // size: 0x2D0, address: 0x803E59E5, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:106

const char mEmailSymbols[45] = {}; // size: 0x2D, Decl: speed/indep/src/frontend/menuscreens/ingame/FEPkg_MU_Keyboard.hpp:107

extern Timer KBCreationTimer;

#endif
