#include "spch/spch.h"

extern SPCHType_ExtVecs gExtVecs;

SPCHType_ExtVecs *SPCH_GetExtVecs(void) {
    return &gExtVecs;
}
