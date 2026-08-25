#include "../../../../../../Src/EAGL4Anim/eagl4supportprintmessage.h"

#include <stdarg.h>
#include <stdio.h>

void PRINT_string(PRINTCHANNEL channel, const char *format, ...);
void DEBUG_break();

static void SYSTEM_abortmessage(const char *format, ...);

static char AbortMsgBuffer[2048];
static char SystemAbortBuffer[2048];
static void (*abort_handler_fn)(const char *format, ...)
    = SYSTEM_abortmessage;

void REAL_abortmessage(const char *format, ...) {
    va_list val;
    if (format) {
        va_start(val, format);
        vsnprintf(AbortMsgBuffer, 2048, format, val);
        va_end(val);
    } else {
        AbortMsgBuffer[0] = '\0';
    }
    (*abort_handler_fn)("%s", AbortMsgBuffer);
}

static void SYSTEM_abortmessage(const char *format, ...) {
    va_list val;
    if (format) {
        va_start(val, format);
        vsnprintf(SystemAbortBuffer, 2048, format, val);
        va_end(val);
    } else {
        SystemAbortBuffer[0] = '\0';
    }
    PRINT_string(static_cast<PRINTCHANNEL>(2), "System abort: %s", SystemAbortBuffer);
    DEBUG_break();
}
