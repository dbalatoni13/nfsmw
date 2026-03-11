#ifndef MISC_EASTEREGGS_H
#define MISC_EASTEREGGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct EasterEggs {
    void HandleJoy();
    virtual ~EasterEggs();
};

extern EasterEggs gEasterEggs;

#endif
