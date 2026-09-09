#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"

int SNDpitchmult(int shandle, int pitchmult) {
    CHANPUB *pchan;
    int voice = SNDVOICEI_get(shandle);
    int setvoice;

    if (voice >= 0) {
        setvoice = -1;
        while (iSNDpatchkey(voice, &setvoice)) {
            pchan = &sndgs.chan[setvoice];
            if (pchan->progpitch == pitchmult) {
                return 0;
            }

            pchan->progpitch = pitchmult;
            iSNDcalcpitch(setvoice);
            SNDPLATFORM_setpitch(setvoice);
        }
    }

    return voice;
}
