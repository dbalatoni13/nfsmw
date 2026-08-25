#include "../../../../../../Src/EAGL4Anim/eagl4supportprintmessage.h"

#include <dolphin/os.h>

static void PRINT_console(PRINTCHANNEL channel, const char *message) {
    OSReport("%s", message);
}
