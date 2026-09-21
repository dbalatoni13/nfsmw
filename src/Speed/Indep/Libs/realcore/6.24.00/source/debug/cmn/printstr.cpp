#include "printi.h"

#include <stdarg.h>
#include <stdio.h>

static PRINTCHANNELINFO PRINTchannellist[64];

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
