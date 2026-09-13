#ifndef FEMOUSE_H_
#define FEMOUSE_H_

#include "Speed/Indep/Src/FEng/FETypes.h"

static const u32 FEMouseIndex_Left = 0;   // size: 0x4, Decl: 16
static const u32 FEMouseIndex_Right = 1;  // size: 0x4, Decl: 17
static const u32 FEMouseIndex_Middle = 2; // size: 0x4, Decl: 18

static const u32 FEMouse_Left = 1;   // size: 0x4, Decl: 20
static const u32 FEMouse_Right = 2;  // size: 0x4, Decl: 21
static const u32 FEMouse_Middle = 4; // size: 0x4, Decl: 22

// total size: 0x8
// Decl: 26
struct FEMouseInfo {
    i16 XPos;       // offset 0x0, size 0x2, Decl: 27
    i16 YPos;       // offset 0x2, size 0x2, Decl: 27
    i16 WheelDelta; // offset 0x4, size 0x2, Decl: 28
    u16 ButtonMask; // offset 0x6, size 0x2, Decl: 29
};

// total size: 0x24
// Decl: 34
class FEMouse {
  private:
    i32 XPos, YPos, WheelDelta; // offset 0x0, size 0x4, Decl: 36
    u16 LastMask, CurMask;      // offset 0xC, size 0x2, Decl: 37

    u32 HeldCount[3]; // offset 0x10, size 0xC, Decl: 39
    bool bDragging;   // offset 0x1C, size 0x1, Decl: 40
    bool bMoved;      // offset 0x20, size 0x1, Decl: 41

  public:
    FEMouse(); // Decl: 44

    void Reset(); // Decl: 46

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
