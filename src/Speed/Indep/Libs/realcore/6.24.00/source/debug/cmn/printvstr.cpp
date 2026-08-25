#include "../../../../../../Src/EAGL4Anim/eagl4supportprintmessage.h"

#include <stdarg.h>

void PRINT_vstring(PRINTCHANNEL channel, const char *format, va_list args);

void PRINT_string(PRINTCHANNEL channel, const char *format, ...) {
    va_list args;
    va_start(args, format);
    PRINT_vstring(channel, format, args);
    va_end(args);
}
