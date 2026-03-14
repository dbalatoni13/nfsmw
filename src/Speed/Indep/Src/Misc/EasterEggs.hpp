#ifndef MISC_EASTEREGGS_H
#define MISC_EASTEREGGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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

struct EasterEggs {
    void HandleJoy();
    bool IsEasterEggUnlocked(EasterEggsSpecial egg);
    void UnActivate();
    virtual ~EasterEggs();
};

extern EasterEggs gEasterEggs;

#endif
