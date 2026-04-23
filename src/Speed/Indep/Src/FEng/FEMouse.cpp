#include "FEMouse.h"

FEMouse::FEMouse() {
    Reset();
}

void FEMouse::Reset() {
    WheelDelta = 0;
    YPos = 0;
    XPos = 0;
    CurMask = 0;
    LastMask = 0;
    HeldCount[2] = 0;
    HeldCount[1] = 0;
    HeldCount[0] = 0;
    bDragging = false;
    bMoved = false;
}

void FEMouse::Update(FEMouseInfo& Info, unsigned long tDelta) {
    bool moved = false;
    if (Info.XPos != XPos || Info.YPos != YPos) {
        moved = true;
    }
    bMoved = moved;
    XPos = Info.XPos;
    YPos = Info.YPos;
    WheelDelta = Info.WheelDelta;
    LastMask = CurMask;
    CurMask = Info.ButtonMask;
    for (int i = 0; i < 3; i++) {
        if (CurMask & (1 << i)) {
            if (LastMask & (1 << i)) {
                HeldCount[i] += tDelta;
            } else {
                HeldCount[i] = 0;
            }
        }
    }
}

bool FEMouse::IsDown(unsigned short Mask) {
    return (CurMask & Mask) == Mask;
}