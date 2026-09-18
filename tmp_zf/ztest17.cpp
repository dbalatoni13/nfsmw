#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

#include <dolphin/types.h>

extern "C" {
u32 __OSBusClock __attribute__((address(0x800000F8))) = 0;
u32 __OSCoreClock __attribute__((address(0x800000FC))) = 0;
}

float UsedFn(float x) { return x * kFloatScaleUp; }
