#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

#include <stdio.h>
#include <stdarg.h>

// TODO
void *memset(void *, int, unsigned long, ...);

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

// STRIPPED
int bVPrintf(char terminal_channel, char *fmt, char *argList) {}

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
    unsigned int flags;
    STATE state;
    int outLen;
    int width;
    int precision;
    char ch;
    char *stringOut;
    int stringLength;
    char prefix[2];
    int prefixSz;
    int padding;
    char cvtbuf[64];

    outLen = 0;
    stringLength = 0;
    stringOut = nullptr;
    flags = 0;
    width = 0;
    precision = 0;
    prefixSz = 0;
    state = ST_NORMAL;
    cvtbuf[63] = '\0';

    if (output_info->StdOut) {
        bBufferedTerminalChannel = static_cast<char>(output_info->TerminalChannel);
    }

    ch = *fmt++;

    while (ch != '\0') {
        int ci = ch - 0x20;
        int charType;
        if (static_cast<unsigned char>(ci) <= 0x5A) {
            charType = statetable[ci] & 0x0F;
        } else {
            charType = 0;
        }

        state = static_cast<STATE>(static_cast<signed char>(statetable[charType * 8 + state]) >> 4);

        switch (state) {
            case ST_NORMAL:
                _stuff_char(output_info, ch, &outLen);
                break;

            case ST_PERCENT:
                width = 0;
                flags = 0;
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
                int hexAdd = 0;
                int radix;

                switch (ch) {
                    case 'c': {
                        cvtbuf[0] = static_cast<char>(va_arg(argList, int));
                        stringLength = 1;
                        goto OUTPUT;
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
                        char *p;
                        unsigned long long number;
                        long long tempNumber;
                        char digit;
                        int size;
                        int digit_count;
                        volatile int group_flag;

                        if (flags & FL_SHORT) {
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
                                tempNumber = va_arg(argList, unsigned int);
                            }
                        }

                        if (radix == 16 && width != 0) {
                            long long nn;
                            int shift = width * 4;
                            nn = (tempNumber << (64 - size)) >> (64 - size);
                            nn >>= shift;
                            if (nn == -1LL) {
                                unsigned long long mask = (1ULL << shift) - 1;
                                tempNumber = static_cast<long long>(static_cast<unsigned long long>(tempNumber) & mask);
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

                        p = cvtbuf + 63;
                        digit_count = 0;
                        group_flag = 0;

                        if (flags & FL_GROUP) {
                            if (g_locale.group_len > 0) {
                                group_flag = 1;
                            }
                        }

                        {
                            char *cvtbuf_base = cvtbuf;
                            while (precision-- > 0 || number != 0) {
                                if (group_flag != 0) {
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
                            size = static_cast<int>(cvtbuf_base - (p - 63));
                        }

                        stringLength = size - 1;
                        if (flags & FL_FORCEOCTAL) {
                            if (stringLength == 0 || *(p + 1) != '0') {
                                stringLength = size;
                                *p = '0';
                                p--;
                            }
                        }

                        stringOut = p + 1;
                        goto OUTPUT;
                    }

                    case 'e':
                        *reinterpret_cast<unsigned short *>(prefix) = static_cast<unsigned short>(static_cast<unsigned char>(prefix[1]) | 0x6500);
                        flags |= FL_EXPONENTIAL;
                        goto GENERIC_FLOAT;

                    case 'g':
                        *reinterpret_cast<unsigned short *>(prefix) = static_cast<unsigned short>(static_cast<unsigned char>(prefix[1]) | 0x6500);
                        flags |= FL_EXPONENTIAL;
                        goto GENERIC_FLOAT;

                    case 'G':
                    case 'E':
                        *reinterpret_cast<unsigned short *>(prefix) = static_cast<unsigned short>(static_cast<unsigned char>(prefix[1]) | 0x4500);
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
                        d = va_arg(argList, double);

                        number = d;
                        stringOut = nullptr;
                        offset = reinterpret_cast<unsigned int *>(&number)[1];
                        if ((offset & 0x7FF00000) == 0x7FF00000) {
                            if (offset == 0xFFF00000) {
                                stringOut = _nan_table[0];
                            } else if (offset == 0x7FF00000) {
                                stringOut = _nan_table[1];
                            } else {
                                stringOut = _nan_table[2];
                            }
                            stringLength = 7;
                        }

                        if (stringOut != nullptr) {
                            goto OUTPUT;
                        }

                        {
                            int digit_pos;
                            bool group_flag;
                            char decimalChr;

                            stringOut = cvtbuf + 1;
                            cvtbuf[0] = '0';
                            d = 1.0;
                            p = stringOut;

                            if ((flags & FL_SIGNED) && number < 0.0) {
                                flags |= FL_NEGATIVE;
                                number = -number;
                            }

                            if (precision == -1) {
                                precision = 3;
                            }

                            count = 0;
                            int savedPrecision;

                            if (flags & FL_EXPONENTIAL) {
                                if (number != 0.0) {
                                    savedPrecision = precision - 1;
                                    if (number >= 1.0) {
                                        do {
                                            number *= 0.1;
                                            count++;
                                        } while (number >= 1.0);
                                        if (number >= 1.0)
                                            goto positioned;
                                    }
                                    do {
                                        count--;
                                        number *= 10.0;
                                    } while (number < 1.0);
                                    goto positioned;
                                }
                            }

                            savedPrecision = precision - 1;
                            if (d >= number) {
                                // already positioned
                            } else {
                                do {
                                    d *= 10.0;
                                    count++;
                                } while (d < number);
                            }
                            if (d > number) {
                                d *= 0.1;
                                count--;
                            }

                        positioned:
                            digit_pos = count + 1;
                            group_flag = false;

                            if (flags & FL_GROUP) {
                                if (g_locale.group_len > 0) {
                                    group_flag = true;
                                }
                                decimalChr = g_locale.decimal_char;
                            } else {
                                decimalChr = '.';
                            }

                            if (d >= 1.0) {
                                int initialDigitPos = digit_pos;

                                do {
                                    if (group_flag) {
                                        int groupLen = static_cast<int>(static_cast<signed char>(g_locale.group_len));
                                        int mod = (digit_pos / groupLen) * groupLen;
                                        if (digit_pos == mod && digit_pos != initialDigitPos) {
                                            *p++ = g_locale.group_char;
                                        }
                                    }
                                    digit_pos--;

                                    int digitInt = static_cast<int>(number / d);
                                    digit = static_cast<char>(digitInt);
                                    double digitDouble = static_cast<double>(static_cast<int>(digit));
                                    number -= digitDouble * d;
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

                            precision = savedPrecision;
                            if (precision != -1) {
                                do {
                                    number *= 10.0;
                                    int digitInt = static_cast<int>(number);
                                    digit = static_cast<char>(digitInt);
                                    double digitDouble = static_cast<double>(static_cast<int>(digit));
                                    number -= digitDouble;
                                    *p++ = '0' + digit;
                                } while (precision-- != 0);
                            }

                            if (number * 10.0 >= 0.5) {
                                {
                                    char *q = p - 1;
                                    char c = *q;
                                    while (true) {
                                        if (c == '9') {
                                            *q = '0';
                                        } else if (c != decimalChr) {
                                            if (!group_flag || c != g_locale.group_char) {
                                                *q = static_cast<char>(c + 1);
                                                if (q < stringOut) {
                                                    stringOut--;
                                                }
                                                break;
                                            }
                                        }
                                        q--;
                                        c = *q;
                                    }
                                }
                            }

                            if (flags & FL_EXPONENTIAL) {
                                *p++ = static_cast<char>(*reinterpret_cast<short *>(prefix) >> 8);
                                prefixSz = 0;

                                if (count < 0) {
                                    count = -count;
                                    *p++ = '-';
                                } else {
                                    *p++ = '+';
                                }

                                char hundreds = static_cast<char>(count / 100);
                                *p++ = static_cast<char>('0' + hundreds);
                                count -= hundreds * 100;

                                char tens = static_cast<char>(count / 10);
                                *p++ = static_cast<char>('0' + tens);
                                count -= tens * 10;

                                char ones = static_cast<char>(count % 10);
                                *p++ = static_cast<char>('0' + ones);
                            }

                            stringLength = static_cast<int>(p - stringOut);
                        }
                        goto OUTPUT;
                    }

                    case 'z': {
                        unsigned int tempNumber;
                        unsigned int upperVal;
                        unsigned int lowerVal;
                        char digit;
                        int desiredPrecision;
                        unsigned int divisor;
                        char *p;

                        p = &cvtbuf[62];
                        char *pSaved = p;
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

                        if (lowerVal != 0 || precision > 0) {
                            p -= (precision - 1);
                            int fracSize = precision + 1;

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
                            p -= fracSize;
                            *p = '.';
                            p--;

                            {
                                char *intSavedP = p - 1;

                                if (lowerVal != 0) {
                                    {
                                        int goesInto = 0;
                                        if (divisor > lowerVal) {
                                            lowerVal *= 10;
                                        }
                                        while (divisor * static_cast<unsigned int>(goesInto) <= lowerVal) {
                                            goesInto++;
                                        }
                                        goesInto--;

                                        if (goesInto > 4) {
                                            {
                                                unsigned char *q = reinterpret_cast<unsigned char *>(pSaved);
                                                while (true) {
                                                    int c = *q;
                                                    if (c != '9') {
                                                        *q = static_cast<unsigned char>(c + 1);
                                                        break;
                                                    }
                                                    unsigned char *tmp = q;
                                                    int prevC = *(--tmp);
                                                    *q = '0';
                                                    if (prevC == '.') {
                                                        upperVal++;
                                                        goto Z_INT;
                                                    }
                                                    q = tmp;
                                                }
                                            }
                                        }
                                    }
                                }

                            Z_INT:
                                if (upperVal != 0) {
                                    do {
                                        *p = static_cast<char>('0' + (upperVal % 10));
                                        upperVal /= 10;
                                        p--;
                                    } while (upperVal != 0);
                                } else {
                                    *p = '0';
                                    p = intSavedP;
                                }
                            }
                        }

                        stringOut = p + 1;
                        stringLength = static_cast<int>((&cvtbuf[63] - p) - 1);
                        goto OUTPUT;
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

                        {
                            char c = *fmt;
                            vectType = c - '0';
                            if (static_cast<unsigned int>(c - '2') > 2) {
                                goto VECT_OUTPUT;
                            }
                        }

                        {
                            bVector4 *vect;
                            vect = va_arg(argList, bVector4 *);

                            if (bIsValidPointer(vect, 1)) {
                                if (vectType == 2) {
                                    bSPrintf(tempBuffer, "%*.*f, %*.*f", width, precision, vect->x, width, precision, vect->y);
                                } else if (vectType == 3) {
                                    bSPrintf(tempBuffer, "%*.*f, %*.*f, %*.*f", width, precision, vect->x, width, precision, vect->y, width,
                                             precision, vect->z);
                                } else if (vectType == 4) {
                                    bSPrintf(tempBuffer, "%*.*f, %*.*f, %*.*f, %*.*f", width, precision, vect->x, width, precision, vect->y, width,
                                             precision, vect->z, width, precision, vect->w);
                                }
                            } else {
                                bStrCpy(tempBuffer, badptr);
                            }
                        }

                    VECT_OUTPUT:
                        flags |= FL_NOOUTPUT;
                        stringLength = bStrLen(tempBuffer);
                        fmt++;
                        _stuff_str(output_info, tempBuffer, stringLength, &outLen);
                        break;
                    }

                    case 's': {
                        stringOut = va_arg(argList, char *);
                        if (stringOut == nullptr) {
                            stringOut = nullstr;
                        } else if (!bIsValidPointer(stringOut, 1)) {
                            stringOut = badptr;
                        }
                        {
                            const char *p = stringOut;
                            int i = precision;
                            if (precision < 0) {
                                i = 0x7fffffff;
                            }
                            do {
                                if (*p++ == '\0')
                                    break;
                            } while (i-- != 0);
                            if (p == stringOut) {
                                goto OUTPUT;
                            }
                            stringLength = static_cast<int>(p - stringOut) - 1;
                        }
                        goto OUTPUT;
                    }

                    case 'n': {
                        int *addr;
                        addr = va_arg(argList, int *);
                        *addr = outLen;
                        flags |= FL_NOOUTPUT;
                        goto OUTPUT;
                    }

                    default:
                        break;
                }
                break;

            OUTPUT:
                if (flags & FL_NOOUTPUT) {
                    break;
                }

                if (flags & FL_SIGNED) {
                    if (flags & FL_NEGATIVE) {
                        prefixSz = 1;
                        *reinterpret_cast<unsigned short *>(prefix) = 0x2d00 | static_cast<unsigned char>(prefix[1]);
                    } else if (flags & FL_SIGN) {
                        prefixSz = 1;
                        *reinterpret_cast<unsigned short *>(prefix) = 0x2b00 | static_cast<unsigned char>(prefix[1]);
                    } else if (flags & FL_SIGNSP) {
                        prefixSz = 1;
                        *reinterpret_cast<unsigned short *>(prefix) = 0x2000 | static_cast<unsigned char>(prefix[1]);
                    }
                }

                padding = width - stringLength - prefixSz;
                if (padding < 0) {
                    padding = 0;
                }

                if (padding > 0 && !(flags & (FL_LEFT | FL_LEADZERO))) {
                    while (padding != 0) {
                        _stuff_char(output_info, ' ', &outLen);
                        padding--;
                    }
                }

                if (prefixSz > 0) {
                    _stuff_str(output_info, prefix, prefixSz, &outLen);
                }

                if (padding > 0 && !(flags & FL_LEFT)) {
                    while (padding != 0) {
                        _stuff_char(output_info, '0', &outLen);
                        padding--;
                    }
                }

                _stuff_str(output_info, stringOut, stringLength, &outLen);

                if (padding > 0) {
                    padding--;
                    if (padding != 0) {
                        do {
                            _stuff_char(output_info, ' ', &outLen);
                            padding--;
                        } while (padding != 0);
                    }
                }
                break;
            }
        }

        ch = *fmt++;
        if (outLen < 0) {
            break;
        }
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
