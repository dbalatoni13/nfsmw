#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

#include <dolphin/os.h>

volatile u32 *GetBusClock() { return &__OSBusClock; }
