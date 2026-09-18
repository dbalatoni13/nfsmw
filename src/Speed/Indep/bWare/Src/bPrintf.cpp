#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

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

// El codigo del juego llama a memset SIN prototipo: el objetivo emite `crclr cr1eq`
// delante de cada `bl memset` (y esta unidad no incluye <string.h>). En el C++ de
// GCC 2.95, `()` dentro de extern "C" declara una funcion sin prototipo. MSVC no
// lo admite junto a su <string.h> (C2733).
#ifdef _MSC_VER
#include <string.h>
#else
extern "C" void *memset();
#endif

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
        bVPrintf(fmt, argList);
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

// STRIPPED
int bVPrintf(char terminal_channel, char *fmt, char *argList) {}

int bSPrintf(char *destString, const char *fmt, ...) {
    va_list argList;
    va_start(argList, fmt);
    bVSPrintf(destString, fmt, argList);
    va_end(argList);
}

int bSNPrintf(char *buf, int max_len, const char *format, ...) {
    va_list argList;
    va_start(argList, format);
    bVSNPrintf(buf, max_len, format, argList);
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

enum STATE { ST_NORMAL, ST_PERCENT, ST_FLAG, ST_WIDTH, ST_DOT, ST_PRECIS, ST_SIZE, ST_TYPE };

enum CHARTYPE {
    CH_REGULAR = 0,
    CH_PERCENT = 1,
    CH_DOT = 2,
    CH_STAR = 3,
    CH_ZERO = 4,
    CH_DIGIT = 5,
    CH_FLAG = 6,
    CH_SIZE = 7,
    CH_TYPE = 8,
};

#define FL_SIGN 0x0001
#define FL_SIGNSP 0x0002
#define FL_LEFT 0x0004
#define FL_LEADZERO 0x0008
#define FL_LONG 0x0010
#define FL_SHORT 0x0020
#define FL_ALTERNATE 0x0080
#define FL_NEGATIVE 0x0100
#define FL_FORCEOCTAL 0x0200
#define FL_SIGNED 0x0400
#define FL_EXPONENTIAL 0x0800
#define FL_NOOUTPUT 0x1000
#define FL_GROUP 0x2000
#define FL_LONG64 0x4000

static const char statetable[91] = {0x06, 0x00, 0x00, 0x06, 0x06, 0x01, 0x00, 0x00, 0x10, 0x00, 0x03, 0x06, 0x00, 0x06, 0x02, 0x10, 0x04, 0x45, 0x45,
                                    0x45, 0x05, 0x05, 0x05, 0x05, 0x05, 0x35, 0x30, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x20, 0x28, 0x38, 0x50, 0x58,
                                    0x07, 0x08, 0x00, 0x37, 0x30, 0x30, 0x57, 0x50, 0x07, 0x00, 0x00, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08,
                                    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x70, 0x78, 0x78, 0x78, 0x78, 0x78, 0x08, 0x07, 0x08, 0x00, 0x00,
                                    0x07, 0x00, 0x08, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08};

void _stuff_char(bOutputInfo *output_info, const char ch, int *outLen);
void _stuff_str(bOutputInfo *output_info, const char *str, int strLen, int *outLen);

// TODO review (dwarf is wrong)
int _bOutput(bOutputInfo *output_info, const char *fmt, va_list argList) {
    unsigned int flags = 0;
    STATE state;
    CHARTYPE charType;
    int outLen;
    int width = 0;
    int precision = 0;
    char ch;
    char *stringOut = nullptr;
    int stringLength = 0;
    char prefix[2];
    int prefixSz = 0;
    int padding;
    char cvtbuf[64];

    cvtbuf[63] = '\0';
    state = ST_NORMAL;
    outLen = 0;

    if (output_info->StdOut) {
        bBufferedTerminalChannel = static_cast<char>(output_info->TerminalChannel);
    }

    ch = *fmt++;

    while (ch != '\0' && outLen >= 0) {
        if (static_cast<unsigned char>(ch - 0x20) <= 0x5A) {
            charType = static_cast<CHARTYPE>(statetable[ch - 0x20] & 0x0F);
        } else {
            charType = CH_REGULAR;
        }

        state = static_cast<STATE>(static_cast<signed char>(statetable[charType * 8 + state]) >> 4);

        switch (state) {
            case ST_NORMAL:
                _stuff_char(output_info, ch, &outLen);
                break;

            case ST_PERCENT:
                flags = 0;
                width = 0;
                prefixSz = 0;
                stringOut = cvtbuf;
                precision = -1;
                break;

            case ST_FLAG:
                switch (ch) {
                    case '-':
                        flags |= FL_LEFT;
                        break;
                    case '+':
                        flags |= FL_SIGN;
                        break;
                    case ' ':
                        flags |= FL_SIGNSP;
                        break;
                    case '#':
                        flags |= FL_ALTERNATE;
                        break;
                    case '0':
                        flags |= FL_LEADZERO;
                        break;
                    case '$':
                        flags |= FL_GROUP;
                        break;
                }
                break;

            case ST_WIDTH:
                if (ch == '*') {
                    width = va_arg(argList, int);
                    if (width < 0) {
                        flags |= FL_LEFT;
                        width = -width;
                    }
                } else {
                    width = width * 10 + (ch - '0');
                }
                break;

            case ST_DOT:
                precision = 0;
                break;

            case ST_PRECIS:
                if (ch == '*') {
                    precision = va_arg(argList, int);
                    if (precision < 0) {
                        precision = -1;
                    }
                } else {
                    precision = precision * 10 + (ch - '0');
                }
                break;

            case ST_SIZE:
                switch (ch) {
                    case 'l':
                        flags |= FL_LONG;
                        break;
                    case 'h':
                        flags |= FL_SHORT;
                        break;
                    case 'i':
                    case 'I':
                        if (fmt[0] == '6' && fmt[1] == '4') {
                            flags |= FL_LONG64;
                            fmt += 2;
                        }
                        break;
                }
                break;

            case ST_TYPE: {
                int radix;
                int hexAdd = 0;

                switch (ch) {
                    case 'c': {
                        cvtbuf[0] = static_cast<char>(va_arg(argList, int));
                        stringLength = 1;
                        break;
                    }

                    case 'x':
                        hexAdd = 0x27;
                        goto GENERIC_HEX;
                    case 'p':
                        precision = 8;
                    case 'X':
                        hexAdd = 7;
                    GENERIC_HEX:
                        if (flags & FL_ALTERNATE) {
                            prefixSz = 2;
                            prefix[1] = static_cast<char>(hexAdd + 0x51);
                            prefix[0] = '0';
                        }
                        radix = 16;
                        goto GENERIC_INT;

                    case 'b':
                        radix = 2;
                        goto GENERIC_INT;

                    case 'o':
                        radix = 8;
                        if (flags & FL_ALTERNATE) {
                            flags |= FL_FORCEOCTAL;
                        }
                        goto GENERIC_INT;

                    case 'd':
                    case 'i':
                        flags |= FL_SIGNED;
                    case 'u':
                        radix = 10;

                    GENERIC_INT: {
                        char *p = &cvtbuf[62];
                        unsigned long long number;
                        long long tempNumber;
                        char digit;
                        int size;
                        int digit_count;
                        bool group_flag;

                        if (flags & FL_SHORT) {
                            size = 16;
                            if (flags & FL_SIGNED) {
                                tempNumber = static_cast<short>(va_arg(argList, int));
                            } else {
                                tempNumber = static_cast<unsigned short>(va_arg(argList, int));
                            }
                        } else if (flags & FL_LONG64) {
                            size = 64;
                            tempNumber = va_arg(argList, long long);
                        } else {
                            size = 32;
                            if (flags & FL_SIGNED) {
                                tempNumber = va_arg(argList, int);
                            } else {
                                tempNumber = static_cast<unsigned int>(va_arg(argList, int));
                            }
                        }

                        if (radix == 16 && width != 0) {
                            long long nn = (tempNumber << (64 - size)) >> (64 - size);
                            int shift = width << 2;
                            nn >>= shift;
                            if (nn == -1LL) {
                                tempNumber = static_cast<long long>(static_cast<unsigned long long>(tempNumber) & ((1ULL << shift) - 1));
                            }
                        }

                        if ((flags & FL_SIGNED) && tempNumber < 0) {
                            number = static_cast<unsigned long long>(-tempNumber);
                            flags |= FL_NEGATIVE;
                        } else {
                            number = static_cast<unsigned long long>(tempNumber);
                        }

                        if (precision < 0) {
                            precision = 1;
                        } else {
                            flags &= ~FL_LEADZERO;
                        }

                        if (number == 0) {
                            prefixSz = 0;
                        }

                        digit_count = 0;
                        group_flag = false;

                        if (flags & FL_GROUP) {
                            if (g_locale.group_len > 0) {
                                group_flag = true;
                            }
                        }

                        while (precision-- > 0 || number != 0) {
                            if (group_flag) {
                                digit_count++;
                                if (digit_count > g_locale.group_len) {
                                    *p = g_locale.group_char;
                                    p--;
                                    digit_count = 1;
                                }
                            }
                            digit = static_cast<char>((number % radix) + '0');
                            number = number / radix;
                            if (digit > '9') {
                                digit = static_cast<char>(digit + hexAdd);
                            }
                            *p = digit;
                            p--;
                        }
                        stringLength = static_cast<int>(&cvtbuf[63] - p) - 1;
                        if (flags & FL_FORCEOCTAL) {
                            if (stringLength == 0 || *(p + 1) != '0') {
                                stringLength++;
                                *p = '0';
                                p--;
                            }
                        }

                        stringOut = p + 1;
                        break;
                    }

                    case 'e':
                        prefix[0] = 'e';
                        flags |= FL_EXPONENTIAL;
                        goto GENERIC_FLOAT;

                    case 'g':
                        prefix[0] = 'e';
                        flags |= FL_EXPONENTIAL;
                        goto GENERIC_FLOAT;

                    case 'G':
                    case 'E':
                        prefix[0] = 'E';
                        flags |= FL_EXPONENTIAL;
                        goto GENERIC_FLOAT;

                    case 'f':
                    GENERIC_FLOAT: {
                        double d;
                        double number;
                        int count;
                        char digit;
                        char *p;
                        unsigned int offset;

                        flags |= FL_SIGNED;
                        number = va_arg(argList, double);
                        stringOut = nullptr;
                        offset = reinterpret_cast<unsigned int>(&number) + 4;
                        if ((*reinterpret_cast<unsigned int *>(offset) & 0x7FF00000) == 0x7FF00000) {
                            if (*reinterpret_cast<unsigned int *>(offset) == 0xFFF00000) {
                                stringOut = _nan_table[0];
                            } else if (*reinterpret_cast<unsigned int *>(offset) == 0x7FF00000) {
                                stringOut = _nan_table[1];
                            } else {
                                stringOut = _nan_table[2];
                            }
                            stringLength = 7;
                        }

                        if (stringOut == nullptr) {
                            int digit_pos;
                            bool group_flag;
                            char decimalChr;

                            stringOut = cvtbuf + 1;
                            cvtbuf[0] = '0';
                            p = stringOut;
                            d = 1.0;

                            if ((flags & FL_SIGNED) && number < 0.0) {
                                flags |= FL_NEGATIVE;
                                number = -number;
                            }

                            if (precision == -1) {
                                precision = 3;
                            }

                            count = 0;

                            if ((flags & FL_EXPONENTIAL) && number != 0.0) {
                                while (number >= 1.0) {
                                    number *= 0.1;
                                    count++;
                                }
                                while (number < 1.0) {
                                    count--;
                                    number *= 10.0;
                                }
                            } else {
                                while (d < number) {
                                    d *= 10.0;
                                    count++;
                                }
                                if (d > number) {
                                    d *= 0.1;
                                    count--;
                                }
                            }

                            digit_pos = count + 1;
                            group_flag = (flags & FL_GROUP) && g_locale.group_len > 0;
                            decimalChr = (flags & FL_GROUP) ? g_locale.decimal_char : '.';

                            if (d >= 1.0) {
                                do {
                                    if (group_flag) {
                                        if (digit_pos % g_locale.group_len == 0 && digit_pos != count + 1) {
                                            *p++ = g_locale.group_char;
                                        }
                                        digit_pos--;
                                    }

                                    digit = static_cast<char>(number / d);
                                    number -= digit * d;
                                    d *= 0.1;

                                    *p++ = '0' + digit;
                                } while (d >= 1.0);
                            }

                            if (p == stringOut) {
                                *stringOut = '0';
                                p = stringOut + 1;
                            }

                            if (precision != 0 || (flags & FL_ALTERNATE)) {
                                *p++ = decimalChr;
                            }

                            precision--;
                            if (precision != -1) {
                                do {
                                    number *= 10.0;
                                    digit = static_cast<char>(number);
                                    number -= digit;
                                    *p++ = '0' + digit;
                                } while (precision-- != 0);
                            }

                            if (number * 10.0 >= 5.0) {
                                {
                                    char *q = p - 1;
                                    for (;; q--) {
                                        if (*q == '9') {
                                            *q = '0';
                                        } else if (*q != decimalChr) {
                                            if (!group_flag || *q != g_locale.group_char) {
                                                break;
                                            }
                                        }
                                    }
                                    (*q)++;
                                    if (q < stringOut) {
                                        stringOut--;
                                    }
                                }
                            }

                            if (flags & FL_EXPONENTIAL) {
                                *p++ = prefix[0];

                                if (count < 0) {
                                    count = -count;
                                    *p++ = '-';
                                } else {
                                    *p++ = '+';
                                }

                                prefixSz = 0;
                                digit = static_cast<char>(count / 100);
                                *p++ = static_cast<char>('0' + digit);
                                count -= digit * 100;

                                digit = static_cast<char>(count / 10);
                                *p++ = static_cast<char>('0' + digit);
                                count -= digit * 10;

                                digit = static_cast<char>(count % 10);
                                *p++ = static_cast<char>('0' + digit);
                            }

                            stringLength = static_cast<int>(p - stringOut);
                        }
                        break;
                    }

                    case 'z': {
                        char *p;
                        unsigned int tempNumber;
                        unsigned int upperVal;
                        unsigned int lowerVal;
                        char digit;
                        int desiredPrecision;
                        unsigned int divisor;

                        p = &cvtbuf[62];
                        flags |= FL_SIGNED;

                        tempNumber = va_arg(argList, unsigned int);

                        if (tempNumber > 0x7FFFFFFEu) {
                            tempNumber = ~tempNumber + 1;
                            flags |= FL_NEGATIVE;
                        }

                        upperVal = tempNumber >> 16;
                        lowerVal = tempNumber & 0xFFFF;
                        divisor = 0x10000u;

                        if (precision < 0) {
                            precision = 2;
                        }
                        desiredPrecision = precision;

                        if (lowerVal != 0 || precision != 0) {
                            p -= (precision - 1);

                            while (precision > 0) {
                                {
                                    int goesInto = 0;
                                    precision--;
                                    if (divisor > lowerVal) {
                                        lowerVal *= 10;
                                    }
                                    while (divisor * static_cast<unsigned int>(goesInto) <= lowerVal) {
                                        goesInto++;
                                    }
                                    goesInto--;
                                    *p = static_cast<char>('0' + goesInto);
                                    lowerVal -= divisor * static_cast<unsigned int>(goesInto);
                                    p++;
                                }
                            }

                            if (lowerVal == 0) {
                                if (precision > 0) {
                                    while (precision > 0) {
                                        *p = '0';
                                        precision--;
                                        p++;
                                    }
                                }
                            }

                            *p = '\0';
                            p -= desiredPrecision + 1;
                            *p = '.';
                            p--;
                        }

                        if (lowerVal != 0) {
                            int goesInto = 0;
                            if (divisor > lowerVal) {
                                lowerVal *= 10;
                            }
                            while (divisor * static_cast<unsigned int>(goesInto) <= lowerVal) {
                                goesInto++;
                            }
                            goesInto--;

                            if (goesInto > 4) {
                                char *q = &cvtbuf[62];
                                while (true) {
                                    if (*q == '9') {
                                        *q = '0';
                                        if (q[-1] == '.') {
                                            upperVal++;
                                            break;
                                        }
                                        q--;
                                    } else {
                                        (*q)++;
                                        break;
                                    }
                                }
                            }
                        }

                        if (upperVal != 0) {
                            do {
                                *p = static_cast<char>('0' + (upperVal % 10));
                                upperVal /= 10;
                                p--;
                            } while (upperVal != 0);
                        } else {
                            *p-- = '0';
                        }
                        stringOut = p + 1;
                        stringLength = static_cast<int>((&cvtbuf[63] - p) - 1);
                        break;
                    }

                    case 'v': {
                        char tempBuffer[64];
                        int vectType;

                        if (precision < 0) {
                            if (width == 0) {
                                precision = 2;
                                width = 8;
                            }
                        }

                        memset(tempBuffer, 0, 64);

                        vectType = *fmt - '0';
                        if (static_cast<unsigned int>(*fmt - '2') <= 2) {
                            bVector4 *vect;
                            vect = va_arg(argList, bVector4 *);

                            if (bIsValidPointer(vect, 1)) {
                                if (vectType == 2) {
                                    bSPrintf(tempBuffer, "[%*.*f,%*.*f]", width, precision, vect->x, width, precision, vect->y);
                                } else if (vectType == 3) {
                                    bSPrintf(tempBuffer, "[%*.*f,%*.*f,%*.*f]", width, precision, vect->x, width, precision, vect->y, width,
                                             precision, vect->z);
                                } else if (vectType == 4) {
                                    bSPrintf(tempBuffer, "[%*.*f,%*.*f,%*.*f,%*.*f]", width, precision, vect->x, width, precision, vect->y, width,
                                             precision, vect->z, width, precision, vect->w);
                                }
                            } else {
                                bStrCpy(tempBuffer, badptr);
                            }
                        }

                        flags |= FL_NOOUTPUT;
                        fmt++;
                        _stuff_str(output_info, tempBuffer, bStrLen(tempBuffer), &outLen);
                        break;
                    }

                    case 's': {
                        const char *p;
                        int i;

                        stringOut = va_arg(argList, char *);
                        if (stringOut == nullptr) {
                            stringOut = nullstr;
                        } else if (!bIsValidPointer(stringOut, 1)) {
                            stringOut = badptr;
                        }
                        p = stringOut;
                        i = precision;
                        if (precision < 0) {
                            i = 0x7fffffff;
                        }
                        do {
                            if (*p++ == '\0')
                                break;
                        } while (i-- != 0);
                        if (p == stringOut) {
                            break;
                        }
                        stringLength = static_cast<int>(p - stringOut) - 1;
                        break;
                    }

                    case 'n': {
                        int *addr;
                        addr = va_arg(argList, int *);
                        *addr = outLen;
                        flags |= FL_NOOUTPUT;
                        break;
                    }

                    default:
                        break;
                }
            }

                if (flags & FL_NOOUTPUT) {
                    break;
                }

                if (flags & FL_SIGNED) {
                    if (flags & FL_NEGATIVE) {
                        prefixSz = 1;
                        prefix[0] = '-';
                    } else if (flags & FL_SIGN) {
                        prefixSz = 1;
                        prefix[0] = '+';
                    } else if (flags & FL_SIGNSP) {
                        prefixSz = 1;
                        prefix[0] = ' ';
                    }
                }

                padding = width - stringLength - prefixSz;
                if (padding < 0) {
                    padding = 0;
                }

                if (padding != 0 && !(flags & (FL_LEFT | FL_LEADZERO))) {
                    while (padding-- != 0) {
                        _stuff_char(output_info, ' ', &outLen);
                    }
                }

                if (prefixSz != 0) {
                    _stuff_str(output_info, prefix, prefixSz, &outLen);
                }

                if (padding > 0 && !(flags & FL_LEFT)) {
                    while (padding-- != 0) {
                        _stuff_char(output_info, '0', &outLen);
                    }
                }

                _stuff_str(output_info, stringOut, stringLength, &outLen);

                if (padding > 0) {
                    while (padding-- != 0) {
                        _stuff_char(output_info, ' ', &outLen);
                    }
                }
                break;
        }

        ch = *fmt++;
    }

    if (output_info->StdOut) {
        bFlushBufferedPutChar();
    }

    if (output_info->DestString != nullptr) {
        if (outLen != -1) {
            *output_info->DestString = '\0';
        }
    }

    return outLen;
}

#undef FL_SIGN
#undef FL_SIGNSP
#undef FL_LEFT
#undef FL_LEADZERO
#undef FL_LONG
#undef FL_SHORT
#undef FL_ALTERNATE
#undef FL_NEGATIVE
#undef FL_FORCEOCTAL
#undef FL_SIGNED
#undef FL_EXPONENTIAL
#undef FL_NOOUTPUT
#undef FL_GROUP
#undef FL_LONG64

void _stuff_char(bOutputInfo *output_info, const char ch, int *outLen) {
    if (output_info->StdOut) {
        bBufferedPutChar(ch);
    } else {
        if (*outLen >= output_info->DestStringLen - 1) {
            return;
        }

        if (output_info->DestString) {
            *output_info->DestString++ = ch;
        }
    }
    *outLen += 1;
}

void _stuff_str(bOutputInfo *output_info, const char *str, int strLen, int *outLen) {
    if (output_info->StdOut) {
        while (strLen != 0) {
            _stuff_char(output_info, *str++, outLen);
            strLen--;
        }
    } else {
        int room = (output_info->DestStringLen - *outLen) - 1;
        if (strLen > room) {
            strLen = room;
        }

        *outLen = *outLen + strLen;
        if (output_info->DestString != nullptr) {
            while (strLen > 0) {
                // TODO why doesn't this work without a temporary?
                char *dest = output_info->DestString;
                *dest = *str;
                str++;
                dest++;
                output_info->DestString = dest;
                strLen--;
            }
        }
    }
}
