#include "../../../../../../Src/EAGL4Anim/eagl4supportprintmessage.h"

#include <stdarg.h>
#include <stdio.h>

typedef void (*PRINTFUNCTION)(PRINTCHANNEL, const char *);

struct PRINTCHANNELINFO {
    const char *name;
    bool enabled;
    bool library_channel;
};

struct PRINTDEVICEINFO {
    const char *name;
    PRINTFUNCTION func;
    bool enabled;
};

static PRINTCHANNELINFO PRINTchannellist[64];
extern PRINTDEVICEINFO PRINTdevicelist[8];

void PRINT_vstring(PRINTCHANNEL channel, const char *format, va_list arglist) {
    char tempstr[2048];
    int i;

    if (PRINTchannellist[channel].enabled) {
        vsnprintf(tempstr, 2048, format, arglist);
        for (i = 8; i != 0; i--) {
            if (PRINTdevicelist[8 - i].enabled) {
                if (PRINTdevicelist[8 - i].func) {
                    PRINTdevicelist[8 - i].func(channel, tempstr);
                }
            }
        }
    }
}
