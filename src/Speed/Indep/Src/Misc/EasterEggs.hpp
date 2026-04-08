#ifndef MISC_EASTEREGGS_H
#define MISC_EASTEREGGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Input/ActionQueue.h"

enum EasterEggsSpecial {
    EASTER_EGG_UNLOCK_ALL_THINGS = 0,
    EASTER_EGG_SKIP_DDAY = 1,
    EASTER_EGG_DISABLE_MIKE_MANN_BUILD = 2,
    EASTER_EGG_BURGER_KING = 3,
    EASTER_EGG_CASTROL = 4,
    EASTER_EGG_DEMO_CHEAT = 5,
    EASTER_EGG_SPECIAL_LANGUAGES_TOGGLE = 6,
    EASTER_EGG_SPECIAL_XBOX_SAFEZONE = 7,
    EASTER_EGG_EREG2_200 = 8,
    EASTER_EGG_EREG2_VINYL = 9,
    EASTER_EGG_PREORDER = 10,
};

// total size: 0x48
class EasterEggs {
  public:
    enum EasterEggsType {
        EASTER_EGG_CARS = 0,
        EASTER_EGG_VISUAL_PARTS = 1,
        EASTER_EGG_PERF_PARTS = 2,
        EASTER_EGG_DRIFT_PHYSICS = 3,
        EASTER_EGG_TRACK = 4,
        EASTER_EGG_SPECIAL = 5,
        EASTER_EGG_PRESETCARS = 6,
        EASTER_EGG_MANUFACTURER_VINYL = 7,
    };

    enum EasterEggButtons {
        L1 = 43,
        L2 = 40,
        R1 = 44,
        R2 = 38,
        LT = 27,
        RT = 28,
        SQ = 41,
        TR = 34,
        UP = 25,
        DN = 26,
        XX = 0,
    };

    enum EasterEggGroups {
        EASTER_EGG_NO_GROUP = 0,
        EASTER_EGG_RENDERING_GROUP = 1,
    };

    // total size: 0x3C
    struct EasterEggsData {
        EasterEggButtons buttons[8]; // offset 0x0
        uint32 type;                 // offset 0x20
        uint32 item;                 // offset 0x24
        uint32 group;                // offset 0x28
        uint32 unlock_message;       // offset 0x2C
        bool unlocked;               // offset 0x30
        bool persistent;             // offset 0x34
        bool enabled;                // offset 0x38
    };

    EasterEggs();
    virtual ~EasterEggs();

    void Activate();
    void UnActivate();
    void ClearNonPersistent();
    void HandleJoy();
    void ClearButtons();
    bool IsEasterEggUnlocked(uint32 type, uint32 item);
    bool IsEasterEggUnlocked(EasterEggsSpecial egg);

    ActionQueue *EasterEggActionQ[2]; // offset 0x0, size 0x8

  private:
    void ClearGroup(uint32 group);
    void TriggerSpecial(uint32 special);
    void ActivateEasterEgg(int egg);

    bool ButtonsEnabled;              // offset 0x8, size 0x1
    bool HaveUnlockMessage;           // offset 0xC, size 0x1
    uint32 UnlockMessage;             // offset 0x10, size 0x4
    EasterEggsData *EasterEggsTable;  // offset 0x14, size 0x4
    uint32 NumberOfEasterEggs;        // offset 0x18, size 0x4
    EasterEggButtons ButtonBuffer[8]; // offset 0x1C, size 0x20
    uint32 NumberOfCurrentButtons;    // offset 0x3C, size 0x4
    uint32 CurrentStartButton;        // offset 0x40, size 0x4
};

extern EasterEggs gEasterEggs;

#endif
