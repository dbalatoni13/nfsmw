#ifndef FEJOYPAD_H_
#define FEJOYPAD_H_

#include "types.h"

// File: speed/indep/src/feng/FEJoyPad.h
// total size: 0x88
// Decl: speed/indep/src/feng/FEJoyPad.h:78
class FEJoyPad {
  private:
    u32 LastMask, CurMask; // offset 0x0,0x4, size 0x4, Decl: speed/indep/src/feng/FEJoyPad.h:80
    u32 HeldCount[32];     // offset 0x8, size 0x80, Decl: speed/indep/src/feng/FEJoyPad.h:81

  public:
    FEJoyPad(); // Decl: speed/indep/src/feng/FEJoyPad.h:84

    void Reset(); // Decl: speed/indep/src/feng/FEJoyPad.h:86

    void Update(u32 NewMask, u32 tDelta);

    bool WasActive() const {
        return (LastMask | CurMask) != 0;
    }

    bool WasPressed(u32 Mask);

    bool WasHeld(u32 Mask);

    u32 HeldFor(u32 Mask);

    bool WasReleased(u32 Mask);

    void DecrementHold(u32 Mask, u32 Amount);
};

#endif
