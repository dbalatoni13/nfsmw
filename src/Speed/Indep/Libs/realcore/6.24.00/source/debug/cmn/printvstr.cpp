#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"

void PRINT_string(PRINTCHANNEL channel, const char *format, ...) {
    va_list args;

    va_start(args, format);
    PRINT_vstring(channel, format, args);
}
