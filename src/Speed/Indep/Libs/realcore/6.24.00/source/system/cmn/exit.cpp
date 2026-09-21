#include "types.h"

#include "../../../include/common/realcore/system.h"

static void (*exitfunctions[64])();

void SYSTEM_addexit(void (*exitfunc)()) {
    int j;
    for (j = 0; j < 64; j++) {
        if (exitfunctions[j] == exitfunc) {
            return;
        }
    }
    for (j = 0; j < 64; j++) {
        if (exitfunctions[j] == nullptr) {
            exitfunctions[j] = exitfunc;
            break;
        }
    }
}
