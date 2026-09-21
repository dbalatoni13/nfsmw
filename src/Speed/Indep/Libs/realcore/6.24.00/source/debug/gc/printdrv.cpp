#include "../cmn/printi.h"

#include <dolphin/os.h>

// The retail split disambiguates this TU-local function with an address suffix.
// Bind that identity at the C entry point; these directives emit no instructions.
__asm__(".text\n.type PRINT_console__F12PRINTCHANNELPCc_8039989C, @function\nPRINT_console__F12PRINTCHANNELPCc_8039989C:\n");

static void PRINT_console(PRINTCHANNEL, const char *string) {
    OSReport("%s", string);
}

__asm__(".size PRINT_console__F12PRINTCHANNELPCc_8039989C, .L_fPRINT_console__F12PRINTCHANNELPCc_e-.L_fPRINT_console__F12PRINTCHANNELPCc_s\n");

PRINTDEVICEINFO PRINTdevicelist[8] = {
    {"print_dev_console", PRINT_console, true},
};
