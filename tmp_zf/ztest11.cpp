#include <dolphin/os.h>

#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

volatile u32 *GetBusClock() { return &__OSBusClock; }
