#include "snd/source/library/cmn/sndcmn.h"

int SNDCTRL_getprogvol(int shandle) {
    int voice = SNDVOICEI_get(shandle);
    int vol;

    if (voice < 0) return -8;

    vol = SNDI_ftoifast(sndgs.chan[voice].programmedVol);

    return vol;
}
