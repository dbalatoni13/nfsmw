#include "FEJoyPad.h"
#include <string.h>

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

void FEJoyPad::Update(unsigned long NewMask, unsigned long tDelta) {
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

bool FEJoyPad::WasPressed(unsigned long Mask) {
    bool result = false;
    unsigned long cur = CurMask & Mask;
    if (cur == Mask && (LastMask & cur) != cur) {
        result = true;
    }
    return result;
}

bool FEJoyPad::WasHeld(unsigned long Mask) {
    bool result = false;
    unsigned long cur = CurMask & Mask;
    if (cur == Mask && (LastMask & cur) == cur) {
        result = true;
    }
    return result;
}

unsigned long FEJoyPad::HeldFor(unsigned long Mask) {
    unsigned long result = 0xFFFFFFFF;
    for (int i = 0; i < 32; i++) {
        if (Mask & (1 << i)) {
            unsigned long v = HeldCount[i];
            if (v > result) {
                v = result;
            }
            result = v;
        }
    }
    return result;
}

bool FEJoyPad::WasReleased(unsigned long Mask) {
    bool result = false;
    if ((CurMask & Mask) != Mask && (LastMask & Mask) == Mask) {
        result = true;
    }
    return result;
}

void FEJoyPad::DecrementHold(unsigned long Mask, unsigned long Amount) {
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