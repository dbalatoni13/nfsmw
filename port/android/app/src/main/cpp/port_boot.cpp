#include "port_boot.h"
#include "assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *kGameFlow[] = {
    "GLOBAL/THINSCREEN_GLOBAL.BUN",
    "GLOBAL/WIDESCREEN_GLOBAL.BUN",
    "GLOBAL/GLOBALB.BUN",
    "GLOBAL/ATTRIBUTES.BIN",
    "GLOBAL/FE_ATTRIB.BIN",
    "GLOBAL/GlobalMemoryFile.bin",
    "FRONTEND/FRONTA.BUN",
    "FRONTEND/FRONTB.LZC",
    "FRONTEND/DEMO_FRONTEND.BUN",
    "fng/FadeScreen.fng",
    "fng/EA_TRAX.fng",
    "fng/PressStart.fng",
    "fng/PC_Loading.fng",
    "fng/DiscErrorPC.fng",
    "fng/Loading.fng",
    NULL
};

static char g_status[128] = "boot: off";

void port_boot_scan(void) {
    int i, ok = 0, n = 0;
    unsigned char *p;
    int sz;
    for (i = 0; kGameFlow[i]; i++) {
        n++;
        p = NULL;
        sz = 0;
        if (assets_copy(kGameFlow[i], &p, &sz) && p && sz > 0) {
            ok++;
            free(p);
        }
    }
    snprintf(g_status, sizeof(g_status), "boot: %d/%d GameFlow files", ok, n);
}

const char *port_boot_status(void) {
    return g_status;
}
