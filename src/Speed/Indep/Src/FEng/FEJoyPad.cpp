#include "FEJoyPad.h"

FEJoyPad::FEJoyPad() {
    Reset();
}

void FEJoyPad::Reset() {
    CurMask = 0;
    LastMask = 0;
    for (int i = 0; i < 32; i++) {
        HeldCount[i] = 0;
    }
}

void FEJoyPad::Update(u32 NewMask, u32 tDelta) {
    LastMask = CurMask;
    CurMask = NewMask;
    for (int i = 0; i < 32; i++) {
        if (CurMask & (1 << i)) {
            if (LastMask & (1 << i)) {
                HeldCount[i] += tDelta;
            } else {
                HeldCount[i] = 0;
            }
        }
    }
}

bool FEJoyPad::WasPressed(u32 Mask) {
    bool bResult = false;
    if ((CurMask & Mask) == Mask) {
        if ((LastMask & Mask) != Mask)
            bResult = true;
    }
    return bResult;
}

bool FEJoyPad::WasHeld(u32 Mask) {
    bool bResult = false;
    if ((CurMask & Mask) == Mask) {
        if ((LastMask & Mask) == Mask)
            bResult = true;
    }
    return bResult;
}

u32 FEJoyPad::HeldFor(u32 Mask) {
    u32 uResult = 0xFFFFFFFF;
    for (int i = 0; i < 32; i++) {
        if (Mask & (1 << i)) {
            uResult = HeldCount[i] > uResult ? uResult : HeldCount[i];
        }
    }
    return uResult;
}

bool FEJoyPad::WasReleased(u32 Mask) {
    bool bResult = false;
    if ((CurMask & Mask) != Mask) {
        if ((LastMask & Mask) == Mask)
            bResult = true;
    }
    return bResult;
}

void FEJoyPad::DecrementHold(u32 Mask, u32 Amount) {
    for (int i = 0; i < 32; i++) {
        if (Mask & (1 << i)) {
            if (HeldCount[i] > Amount) {
                HeldCount[i] -= Amount;
            } else {
                HeldCount[i] = 0;
            }
        }
    }
}
