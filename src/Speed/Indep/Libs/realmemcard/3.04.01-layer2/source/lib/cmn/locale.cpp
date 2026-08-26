#include "../../../include/common/realmemcard/memcard_interface.h"

#include <stdarg.h>
#include <string.h>

namespace Realmc {

namespace Locale {

static const int TRC_MSG_BUFFER_SIZE = 1024;
static const int NUM_TRC_MSG_BUFFERS = 5;
static unsigned short gTrcMsgBuffer[NUM_TRC_MSG_BUFFERS][TRC_MSG_BUFFER_SIZE];
static int gTrcMsgBufferIndex = 0;
static const char *(*gLocaleCallback)(int) = nullptr;

void SetLocaleGetStrCallback(const char *(*cb)(int)) {
    gLocaleCallback = cb;
}

const wchar_t *GetString(int strID, char *parameterTypes, ...) {
    int numParameters = parameterTypes ? strlen(parameterTypes) : 0;
    const int MAX_LOCALE_PARAMETERS = 8;
    va_list val;

    va_start(val, parameterTypes);
    int param[MAX_LOCALE_PARAMETERS];
    char *pType = parameterTypes;
    for (int i = 0; i < numParameters; i++, pType++) {
        switch (*pType) {
        case 'c':
            param[i] = va_arg(val, char);
            break;
        case 'd':
            param[i] = va_arg(val, int);
            break;
        case 'p':
            param[i] = reinterpret_cast<int>(va_arg(val, void *));
            break;
        case 's':
            param[i] = reinterpret_cast<int>(va_arg(val, short *));
            break;
        }
    }
    va_end(val);

    const char *pStr = gLocaleCallback(strID);
    if (pStr == nullptr) {
        return nullptr;
    }

    int bufindex = gTrcMsgBufferIndex;
    short *stri = reinterpret_cast<short *>(const_cast<char *>(pStr));
    short *strd = reinterpret_cast<short *>(gTrcMsgBuffer[bufindex]);
    char numstr[20];
    bool nextcmd = false;
    int pi = 0;

    do {
        if (nextcmd) {
            short c = *stri;
            pi = c - '1';
            if (parameterTypes[pi] != 'd') {
                if (parameterTypes[pi] == 's') {
                    short *s = reinterpret_cast<short *>(param[pi]);

                    while (*s != 0) {
                        *strd++ = *s++;
                    }
                }
            } else {
                int argval = param[pi];

                if (argval < 0) {
                    *strd++ = '-';
                    argval = -argval;
                }

                int ibackup = argval;
                int numdigits = 0;
                do {
                    ibackup /= 10;
                    numdigits++;
                } while (ibackup != 0);
                numstr[numdigits] = 0;

                do {
                    numdigits--;
                    numstr[numdigits] = argval % 10 + '0';
                    argval /= 10;
                } while (numdigits != 0);

                char *cptr = numstr;
                while (*cptr != 0) {
                    *strd++ = *cptr++;
                }
            }

            nextcmd = false;
        } else {
            if (*stri == '%') {
                nextcmd = true;
            } else if (*stri != '"') {
                *strd++ = *stri;
            }
        }

        stri++;
    } while (*stri != 0);

    *strd = 0;
    gTrcMsgBufferIndex = (gTrcMsgBufferIndex + 1) % NUM_TRC_MSG_BUFFERS;
    return reinterpret_cast<const wchar_t *>(gTrcMsgBuffer[bufindex]);
}

int GetWstrLength(const wchar_t *str) {
    int len = 0;

    if (str != nullptr) {
        while (*str++ != 0) {
            len++;
        }
    }

    return len;
}

} // namespace Locale

} // namespace Realmc
