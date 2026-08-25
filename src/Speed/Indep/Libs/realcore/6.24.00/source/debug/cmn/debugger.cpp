#include <dolphin/os.h>

void DEBUG_break() {
    OSPanic("", 0, "");
}
