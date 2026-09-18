#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"

#include <stdio.h>

static char AbortMsgBuffer[0x800];
static char SystemAbortBuffer[0x800];

static void SYSTEM_abortmessage(const char *format, ...);

static void (*abort_handler_fn)(const char *, ...) = SYSTEM_abortmessage;

void REAL_abortmessage(const char *format, ...) {
    if (format != 0) {
        va_list args;

        va_start(args, format);
        vsnprintf(AbortMsgBuffer, sizeof(AbortMsgBuffer), format, args);
    } else {
        AbortMsgBuffer[0] = 0;
    }

    abort_handler_fn("%s", AbortMsgBuffer);
}

static void SYSTEM_abortmessage(const char *format, ...) {
    if (format != 0) {
        va_list args;

        va_start(args, format);
        vsnprintf(SystemAbortBuffer, sizeof(SystemAbortBuffer), format, args);
    } else {
        SystemAbortBuffer[0] = 0;
    }

    PRINT_string(PRINT_ALERT, "ERROR: %s", SystemAbortBuffer);
    DEBUG_break();
}
