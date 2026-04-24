#ifndef FENG_FEMOUSE_H
#define FENG_FEMOUSE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

// total size: 0x8
struct FEMouseInfo {
    short XPos;              // offset 0x0, size 0x2
    short YPos;              // offset 0x2, size 0x2
    short WheelDelta;        // offset 0x4, size 0x2
    unsigned short ButtonMask; // offset 0x6, size 0x2
};

// total size: 0x24
struct FEMouse {
    int XPos;                  // offset 0x0, size 0x4
    int YPos;                  // offset 0x4, size 0x4
    int WheelDelta;            // offset 0x8, size 0x4
    unsigned short LastMask;   // offset 0xC, size 0x2
    unsigned short CurMask;    // offset 0xE, size 0x2
    unsigned long HeldCount[3]; // offset 0x10, size 0xC
    bool bDragging;            // offset 0x1C, size 0x1
    bool bMoved;               // offset 0x20, size 0x1

    FEMouse();

    void Reset();
    void Update(FEMouseInfo& Info, unsigned long tDelta);
    bool WasPressed(unsigned short Mask);
    bool WasHeld(unsigned short Mask);
    bool IsDown(unsigned short Mask);
    unsigned long HeldFor(unsigned short Mask);
    bool WasReleased(unsigned short Mask);
    void DecrementHold(unsigned short Mask, unsigned long Amount);

    inline int GetXPos() const { return XPos; }
    inline int GetYPos() const { return YPos; }
    inline bool MouseMoved() const { return bMoved; }
};

#endif
