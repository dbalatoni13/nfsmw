#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDCTRL_timemult(int shandle, int timemult) {
    if (timemult > 0x2000) {
        timemult = 0x2000;
    } else if (timemult <= 0x7FF) {
        timemult = 0x800;
    }

    int voice = SNDVOICEI_get(shandle);

    if (voice >= 0) {
        int setvoice = -1;
        while (iSNDpatchkey(voice, &setvoice)) {
            sndgs.chan[setvoice].timemult = timemult;
            SNDPLATFORM_timemult(setvoice, timemult);
        }
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
