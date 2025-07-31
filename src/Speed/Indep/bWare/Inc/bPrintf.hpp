#ifndef BWARE_BPRINTF_H
#define BWARE_BPRINTF_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <stdarg.h>

struct bPrintfLocaleInfo {
    // total size: 0x3
    char decimal_char; // offset 0x0, size 0x1
    char group_char;   // offset 0x1, size 0x1
    char group_len;    // offset 0x2, size 0x1
};

struct bOutputInfo {
    // total size: 0x10
    char *DestString;    // offset 0x0, size 0x4
    int DestStringLen;   // offset 0x4, size 0x4
    bool StdOut;         // offset 0x8, size 0x1
    int TerminalChannel; // offset 0xC, size 0x4
};

void bReleasePutString(char terminal_channel, const char *s);
int bReleasePrintf(const char *fmt, ...);
int bVPrintf(const char *fmt, va_list *argList);
int bSPrintf(char *destString, const char *fmt, ...);
int bSNPrintf(char *buf, int max_len, const char *format, ...);
int bVSPrintf(char *destString, const char *fmt, va_list *argList);
int bVSNPrintf(char *destString, int max_len, const char *fmt, va_list *argList);

#endif
