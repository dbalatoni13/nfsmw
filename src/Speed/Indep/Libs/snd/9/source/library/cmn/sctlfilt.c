#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDCTRL_filteradd(int shandle, SNDFILTERDEF *psfd) {
    int voice = SNDVOICEI_get(shandle);

    if (voice >= 0) {
        int setvoice = -1;
        while (iSNDpatchkey(voice, &setvoice)) {
            SNDPLATFORM_filteradd(setvoice, psfd);
        }
    }

    return voice;
}

#ifdef __cplusplus
}
#endif
