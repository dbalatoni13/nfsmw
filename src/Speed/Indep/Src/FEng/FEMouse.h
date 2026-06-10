#ifndef FEMOUSE_H_
#define FEMOUSE_H_

#include "types.h"

// File: speed/indep/src/feng/FEMouse.h
// total size: 0x8
// Decl: speed/indep/src/feng/FEMouse.h:26
struct FEMouseInfo {
    i16 XPos;       // offset 0x0, size 0x2, Decl: speed/indep/src/feng/FEMouse.h:27
    i16 YPos;       // offset 0x2, size 0x2, Decl: speed/indep/src/feng/FEMouse.h:27
    i16 WheelDelta; // offset 0x4, size 0x2, Decl: speed/indep/src/feng/FEMouse.h:28
    u16 ButtonMask; // offset 0x6, size 0x2, Decl: speed/indep/src/feng/FEMouse.h:29
};

// total size: 0x24
// Decl: speed/indep/src/feng/FEMouse.h:34
class FEMouse {
  private:
    i32 XPos, YPos, WheelDelta; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEMouse.h:36
    u16 LastMask, CurMask;      // offset 0xC, size 0x2, Decl: speed/indep/src/feng/FEMouse.h:37

    u32 HeldCount[3]; // offset 0x10, size 0xC, Decl: speed/indep/src/feng/FEMouse.h:39
    bool bDragging;   // offset 0x1C, size 0x1, Decl: speed/indep/src/feng/FEMouse.h:40
    bool bMoved;      // offset 0x20, size 0x1, Decl: speed/indep/src/feng/FEMouse.h:41

  public:
    FEMouse(); // Decl: speed/indep/src/feng/FEMouse.h:44

    void Reset(); // Decl: speed/indep/src/feng/FEMouse.h:46

    void Update(FEMouseInfo &Info, u32 tDelta);

    i32 GetXPos() const {
        return XPos;
    }

    i32 GetYPos() const {
        return YPos;
    }

    bool WasPressed(u16 Mask);

    bool WasHeld(u16 Mask);

    bool IsDown(u16 Mask);

    u32 HeldFor(u16 Mask);

    bool WasReleased(u16 Mask);

    bool MouseMoved() const {
        return bMoved;
    }

    void DecrementHold(u16 Mask, u32 Amount);
};

#endif
