#include "spch/spch.h"

extern SPCHType_ExtVecs gExtVecs;

int SPCHEXT_gettick(void) {
    int tick = 0;
    if (gExtVecs.spchGetTick != 0) {
        tick = gExtVecs.spchGetTick();
    }
    return tick;
}
