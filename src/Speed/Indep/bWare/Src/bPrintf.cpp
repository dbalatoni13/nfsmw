#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

#include <stdio.h>
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

void _stuff_char(bOutputInfo *output_info, const char ch, int *outLen) {
    if (!output_info->StdOut) {
        if (output_info->DestStringLen - 1 <= *outLen) {
            return;
        }

        char *dest = output_info->DestString;
        if (dest != nullptr) {
            *dest = ch;
            output_info->DestString = dest + 1;
        }
    } else {
        bBufferedPutChar(ch);
    }

    *outLen = *outLen + 1;
}

void _stuff_str(bOutputInfo *output_info, const char *str, int strLen, int *outLen) {
    if (output_info->StdOut != 0) {
        if (strLen != 0) {
            do {
                char ch = *str;
                str = str + 1;
                _stuff_char(output_info, ch, outLen);
                strLen = strLen - 1;
            } while (strLen != 0);
        }
    } else {
        int max_len = (output_info->DestStringLen - *outLen) - 1;
        if (strLen > max_len) {
            strLen = max_len;
        }

        *outLen = *outLen + strLen;
        if (output_info->DestString != nullptr) {
            while (strLen > 0) {
                char *dest = output_info->DestString;
                *dest = *str;
                str = str + 1;
                dest = dest + 1;
                output_info->DestString = dest;
                strLen = strLen - 1;
            }
        }
    }
}

static void _stuff_pad(bOutputInfo *output_info, char pad, int count, int *outLen) {
    while (count > 0) {
        _stuff_char(output_info, pad, outLen);
        count--;
    }
}

int _bOutput(bOutputInfo *output_info, const char *fmt, va_list argList) {
    int outLen = 0;

    if (output_info->StdOut) {
        bBufferedTerminalChannel = static_cast<char>(output_info->TerminalChannel);
    }

    while (*fmt != '\0') {
        if (*fmt != '%') {
            _stuff_char(output_info, *fmt, &outLen);
            fmt++;
            continue;
        }

        fmt++;

        bool left_justify = false;
        bool force_sign = false;
        bool space_sign = false;
        bool alt_form = false;
        bool zero_pad = false;

        while (true) {
            if (*fmt == '-') {
                left_justify = true;
            } else if (*fmt == '+') {
                force_sign = true;
            } else if (*fmt == ' ') {
                space_sign = true;
            } else if (*fmt == '#') {
                alt_form = true;
            } else if (*fmt == '0') {
                zero_pad = true;
            } else {
                break;
            }
            fmt++;
        }

        int width = 0;
        int precision = -1;
        bool short_arg = false;
        bool long64_arg = false;

        if (*fmt == '*') {
            width = va_arg(argList, int);
            if (width < 0) {
                left_justify = true;
                width = -width;
            }
            fmt++;
        } else {
            while ((*fmt >= '0') && (*fmt <= '9')) {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }
        }

        if (*fmt == '.') {
            fmt++;
            precision = 0;
            if (*fmt == '*') {
                precision = va_arg(argList, int);
                fmt++;
            } else {
                while ((*fmt >= '0') && (*fmt <= '9')) {
                    precision = precision * 10 + (*fmt - '0');
                    fmt++;
                }
            }

            if (precision < 0) {
                precision = -1;
            }
        }

        if (*fmt == 'h') {
            short_arg = true;
            fmt++;
        } else if ((*fmt == 'I') && (fmt[1] == '6') && (fmt[2] == '4')) {
            long64_arg = true;
            fmt += 3;
        }

        char spec = *fmt;
        if (spec == '\0') {
            break;
        }
        fmt++;

        if (spec == '%') {
            _stuff_char(output_info, '%', &outLen);
            continue;
        }

        if (spec == 'n') {
            int *written = va_arg(argList, int *);
            *written = outLen;
            continue;
        }

        if (spec == 'c') {
            char temp[2];
            temp[0] = static_cast<char>(va_arg(argList, int));
            temp[1] = '\0';

            if (!left_justify) {
                _stuff_pad(output_info, ' ', width - 1, &outLen);
            }
            _stuff_char(output_info, temp[0], &outLen);
            if (left_justify) {
                _stuff_pad(output_info, ' ', width - 1, &outLen);
            }
            continue;
        }

        if (spec == 's') {
            char *str = va_arg(argList, char *);
            if (str == nullptr) {
                str = nullstr;
            } else if (bIsValidPointer(str, 1) == 0) {
                str = badptr;
            }

            int strLen = bStrLen(str);
            if ((precision >= 0) && (precision < strLen)) {
                strLen = precision;
            }

            if (!left_justify) {
                _stuff_pad(output_info, ' ', width - strLen, &outLen);
            }
            _stuff_str(output_info, str, strLen, &outLen);
            if (left_justify) {
                _stuff_pad(output_info, ' ', width - strLen, &outLen);
            }
            continue;
        }

        if (spec == 'v') {
            char temp[160];
            char *bad = badptr;
            int len = bStrLen(bad);
            _stuff_str(output_info, bad, len, &outLen);
            if ((*fmt >= '2') && (*fmt <= '4')) {
                fmt++;
            }
            continue;
        }

        if (spec == 'z') {
            char temp[64];
            unsigned int fixed = va_arg(argList, unsigned int);
            sprintf(temp, "%u.%02u", fixed >> 16, ((fixed & 0xffff) * 100U) >> 16);
            int len = bStrLen(temp);
            if (!left_justify) {
                _stuff_pad(output_info, ' ', width - len, &outLen);
            }
            _stuff_str(output_info, temp, len, &outLen);
            if (left_justify) {
                _stuff_pad(output_info, ' ', width - len, &outLen);
            }
            continue;
        }

        {
            char format[32];
            char temp[256];
            int pos = 0;

            format[pos++] = '%';
            if (alt_form) {
                format[pos++] = '#';
            }
            if (force_sign) {
                format[pos++] = '+';
            } else if (space_sign) {
                format[pos++] = ' ';
            }
            if (left_justify) {
                format[pos++] = '-';
            } else if (zero_pad) {
                format[pos++] = '0';
            }

            if (width > 0) {
                pos += sprintf(format + pos, "%d", width);
            }
            if (precision >= 0) {
                format[pos++] = '.';
                pos += sprintf(format + pos, "%d", precision);
            }
            if (short_arg) {
                format[pos++] = 'h';
            } else if (long64_arg) {
                format[pos++] = 'l';
                format[pos++] = 'l';
            }

            format[pos++] = spec;
            format[pos] = '\0';

            temp[0] = '\0';
            switch (spec) {
            case 'd':
            case 'i':
                if (long64_arg) {
                    sprintf(temp, format, va_arg(argList, long long));
                } else {
                    sprintf(temp, format, va_arg(argList, int));
                }
                break;

            case 'u':
            case 'o':
            case 'x':
            case 'X':
                if (long64_arg) {
                    sprintf(temp, format, va_arg(argList, unsigned long long));
                } else {
                    sprintf(temp, format, va_arg(argList, unsigned int));
                }
                break;

            case 'p':
                sprintf(temp, format, va_arg(argList, void *));
                break;

            case 'e':
            case 'E':
            case 'f':
            case 'g':
            case 'G':
                sprintf(temp, format, va_arg(argList, double));
                break;

            default:
                temp[0] = spec;
                temp[1] = '\0';
                break;
            }

            _stuff_str(output_info, temp, bStrLen(temp), &outLen);
        }
    }

    if (!output_info->StdOut) {
        if ((output_info->DestString != nullptr) && (output_info->DestStringLen > 0)) {
            *output_info->DestString = '\0';
        }
    } else {
        bFlushBufferedPutChar();
    }

    return outLen;
}
