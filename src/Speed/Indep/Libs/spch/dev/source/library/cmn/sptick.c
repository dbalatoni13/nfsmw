#include "spch/spch.h"

unsigned int SPCHEXT_gettick() {
    unsigned int result;

    result = 0;
    if (gExtVecs.spchGetTick != 0) {
        result = gExtVecs.spchGetTick();
    }
    return result;
}
