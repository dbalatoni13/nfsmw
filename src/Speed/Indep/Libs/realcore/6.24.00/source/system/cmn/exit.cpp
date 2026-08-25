#include "types.h"

#include "../../../include/common/realcore/system.h"

static void (*exitfunctions[64])();

void SYSTEM_addexit(void (*exitfunc)()) {
    int j;
    j = 0;
    for (; j < 64; j++) {
        if (exitfunctions[j] == exitfunc) {
            return;
        }
    }
    j = 0;
    if (*exitfunctions == nullptr) {
        exitfunctions[0] = exitfunc;
        return;
    }
    do {
        j++;
        if (j > 63) {
            return;
        }
    } while (exitfunctions[j] != nullptr);
    exitfunctions[j] = exitfunc;
}
