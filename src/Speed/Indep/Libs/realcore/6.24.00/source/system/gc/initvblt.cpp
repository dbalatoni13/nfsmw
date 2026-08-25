#include "types.h"

extern volatile int vblticks;
extern void (*vbltmrsub[8])();

void ttDoVTimerMsg() {
    int i;
    vblticks++;
    for (i = 0; i < 8; i++) {
        if (vbltmrsub[i] != nullptr) {
            vbltmrsub[i]();
        }
    }
}
