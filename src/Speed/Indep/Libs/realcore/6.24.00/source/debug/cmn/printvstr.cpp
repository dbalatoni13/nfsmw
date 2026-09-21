#include "../../../../../../Src/EAGL4Anim/eagl4supportprintmessage.h"

#include <stdarg.h>

void PRINT_vstring(PRINTCHANNEL channel, const char *format, va_list args);

void PRINT_string(PRINTCHANNEL channel, const char *format, ...) {
    va_list arglist;
    va_start(arglist, format);
    PRINT_vstring(channel, format, arglist);
    va_end(arglist);
}
