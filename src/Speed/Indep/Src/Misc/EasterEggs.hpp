#ifndef MISC_EASTEREGGS_H
#define MISC_EASTEREGGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class ActionQueue;

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
        XX = 0,
        UP = 25,
        DN = 26,
        LT = 27,
        RT = 28,
        TR = 34,
        R2 = 38,
        L2 = 40,
        SQ = 41,
        L1 = 43,
        R1 = 44,
    };

    enum EasterEggGroups {
        EASTER_EGG_NO_GROUP = 0,
        EASTER_EGG_RENDERING_GROUP = 1,
    };

    // total size: 0x3C
    struct EasterEggsData {
        EasterEggButtons buttons[8]; // offset 0x0
        unsigned int type;           // offset 0x20
        unsigned int item;           // offset 0x24
        unsigned int group;          // offset 0x28
        unsigned int unlock_message; // offset 0x2C
        bool unlocked;               // offset 0x30
        bool persistent;             // offset 0x34
        bool enabled;                // offset 0x38
    };

    ActionQueue *EasterEggActionQ[2]; // offset 0x0

    EasterEggs();
    virtual ~EasterEggs();

    void Activate();
    void UnActivate();
    void ClearNonPersistent();
    void ActivateEasterEgg(int egg);
    void HandleJoy();
    void ClearButtons();
    bool IsEasterEggUnlocked(unsigned int type, unsigned int item);
    bool IsEasterEggUnlocked(EasterEggsSpecial egg);
    void ClearGroup(unsigned int group);
    void TriggerSpecial(unsigned int special);

private:
    bool ButtonsEnabled;                   // offset 0x8
    bool HaveUnlockMessage;                // offset 0xC
    unsigned int UnlockMessage;            // offset 0x10
    EasterEggsData *EasterEggsTable;       // offset 0x14
    unsigned int NumberOfEasterEggs;        // offset 0x18
    EasterEggButtons ButtonBuffer[8];      // offset 0x1C
    unsigned int NumberOfCurrentButtons;   // offset 0x3C
    unsigned int CurrentStartButton;       // offset 0x40
};

#endif
