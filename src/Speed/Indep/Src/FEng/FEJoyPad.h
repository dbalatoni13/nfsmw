#ifndef FENG_FEJOYPAD_H
#define FENG_FEJOYPAD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

// total size: 0x88
struct FEJoyPad {
    unsigned long LastMask;      // offset 0x0, size 0x4
    unsigned long CurMask;       // offset 0x4, size 0x4
    unsigned long HeldCount[32]; // offset 0x8, size 0x80

    FEJoyPad();

    void Reset();
    void Update(unsigned long NewMask, unsigned long tDelta);
    bool WasPressed(unsigned long Mask);
    bool WasHeld(unsigned long Mask);
    unsigned long HeldFor(unsigned long Mask);
    bool WasReleased(unsigned long Mask);
    void DecrementHold(unsigned long Mask, unsigned long Amount);

    inline bool WasActive() const { return (LastMask | CurMask) != 0; }
};

#endif
