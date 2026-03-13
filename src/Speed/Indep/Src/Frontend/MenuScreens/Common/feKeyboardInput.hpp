#ifndef FRONTEND_MENUSCREENS_COMMON_FEKEYBOARDINPUT_H
#define FRONTEND_MENUSCREENS_COMMON_FEKEYBOARDINPUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct FEPackage;

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

    FEKeyboard(ScreenConstructorData *sd);
    ~FEKeyboard() override {}

    void NotificationMessage(unsigned long msg, FEObject *pObject, unsigned long param1, unsigned long param2) override;

    void SetString(char *pStr);
    void SetMaxLength(int nLength);
    int IsKeyButton(FEObject *pObj);
    void AppendSpace();
    void AppendChar(char c);

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
