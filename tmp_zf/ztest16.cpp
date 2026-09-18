#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

u32 t_addr_init __attribute__((address(0x80000100))) = 1;
u32 t_plain_init = 2;

float UsedFn(float x) { return x * kFloatScaleUp; }
