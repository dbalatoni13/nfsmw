#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

#include <stdarg.h>

char *_nan_table[4] = {
    "-1.#INF",
    "1.#INF ",
    "1.#QNAN",
    "1.#SNAN",
};
static char *nullstr = "(null)";
static char *badptr = "(badptr)";

static bPrintfLocaleInfo g_locale = {'.', ',', 3};
static char bPutCharBuffer[160];
static long bPutCharBufferPos = 0;
static char bBufferedTerminalChannel;
float bVSPrintfTime = 0.0f;
int bVSPrintfCount = 0;

int _bOutput(bOutputInfo *output_info, const char *fmt, va_list argList);

void bPrintfSetLocaleInfo(char decimal_char, char group_char, char group_len) {
    g_locale.decimal_char = decimal_char;
    g_locale.group_char = group_char;
    g_locale.group_len = group_len;
}

void bFlushBufferedPutChar() {
    if (bPutCharBufferPos != 0) {
        bPutCharBuffer[bPutCharBufferPos] = '\0';
        bReleasePutString(bBufferedTerminalChannel, bPutCharBuffer);
        bPutCharBufferPos = 0;
    }
}

void bBufferedPutChar(char c) {
    bPutCharBuffer[bPutCharBufferPos] = c;
    bPutCharBufferPos++;
    bPutCharBufferPos &= 0x7f;
    if (bPutCharBufferPos == 0x7f) {
        bFlushBufferedPutChar();
    }
}

int bReleasePrintf(const char *fmt, ...) {
    va_list argList;
    if (EnableReleasePrintf) {
        va_start(argList, fmt);
        // TODO returning this causes issues??
        bVPrintf(fmt, &argList);
        va_end(argList);
    } else {
        return 0;
    }
}

int bVPrintf(const char *fmt, va_list argList) {
    bOutputInfo output_info;

    output_info.DestString = nullptr;
    output_info.DestStringLen = 0;
    output_info.StdOut = true;
    output_info.TerminalChannel = 0;
    return _bOutput(&output_info, fmt, argList);
}

int bSPrintf(char *destString, const char *fmt, ...) {
    va_list argList;
    va_start(argList, fmt);
    bVSPrintf(destString, fmt, &argList);
    va_end(argList);
}

int bSNPrintf(char *buf, int max_len, const char *format, ...) {
    va_list argList;
    va_start(argList, format);
    bVSNPrintf(buf, max_len, format, &argList);
    va_end(argList);
}

int bVSPrintf(char *destString, const char *fmt, va_list argList) {
    int retVal = 0;
    int start_ticks = bGetTicker();
    bOutputInfo output_info;

    output_info.DestString = destString;
    output_info.DestStringLen = 0x7fffffff;
    output_info.StdOut = false;
    output_info.TerminalChannel = 0;

    retVal = _bOutput(&output_info, fmt, argList);
    bVSPrintfTime += bGetTickerDifference(start_ticks);
    bVSPrintfCount++;
    return retVal;
}

int bVSNPrintf(char *destString, int max_len, const char *fmt, va_list argList) {
    bOutputInfo output_info;
    int retVal;

    if (max_len >= 1) {
        output_info.DestString = destString;
        output_info.DestStringLen = max_len;
        output_info.StdOut = false;
        output_info.TerminalChannel = 0;
        retVal = _bOutput(&output_info, fmt, argList);
    } else {
        retVal = 0;
    }
    return retVal;
}
