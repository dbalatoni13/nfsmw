#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"

#include <stdio.h>

static PRINTCHANNELINFO PRINTchannellist[64] = {
    {"print_ch_all", true, true},
    {"print_ch_real", false, true},
    {"print_ch_alert", true, true},
    {"print_ch_debug", false, true},
    {"print_ch_std", false, true},
    {"print_ch_system", false, true},
    {"print_ch_codec", false, true},
    {"print_ch_math", false, true},
    {"print_ch_memory", false, true},
    {"print_ch_file", false, true},
    {"print_ch_stream", false, true},
    {"print_ch_input", false, true},
    {"print_ch_eaglcore", false, true},
    {"print_ch_eaglanim", false, true},
    {"print_ch_eaglrend", false, true},
};

extern PRINTDEVICEINFO PRINTdevicelist[8];

void PRINT_vstring(PRINTCHANNEL channel, const char *format, va_list args) {
    if (PRINTchannellist[channel].enabled) {
        char buffer[0x800];

        vsnprintf(buffer, sizeof(buffer), format, args);

        for (int i = 0; i < 8; i++) {
            if (PRINTdevicelist[i].enabled && PRINTdevicelist[i].func) {
                PRINTdevicelist[i].func(channel, buffer);
            }
        }
    }
}
