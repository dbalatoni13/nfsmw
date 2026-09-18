#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"

#include <dolphin/os.h>

static void PRINT_console(PRINTCHANNEL channel, const char *string) {
    OSReport("%s", string);
}

PRINTDEVICEINFO PRINTdevicelist[8] = {
    {"print_dev_console", PRINT_console, true},
};
