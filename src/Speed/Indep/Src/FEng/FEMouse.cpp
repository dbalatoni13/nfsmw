#include "FEMouse.h"

FEMouse::FEMouse() {
    Reset();
}

void FEMouse::Reset() {
    XPos = YPos = WheelDelta = 0;
    LastMask = CurMask = 0;
    HeldCount[0] = HeldCount[1] = HeldCount[2] = 0;
    bDragging = false;
    bMoved = false;
}

void FEMouse::Update(FEMouseInfo &Info, u32 tDelta) {
    bMoved = Info.XPos != XPos || Info.YPos != YPos;
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
