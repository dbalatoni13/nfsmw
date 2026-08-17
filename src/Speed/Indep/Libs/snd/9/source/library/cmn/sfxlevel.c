#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"

int SNDfxlevel(int shandle, int bus, int level) {
    int setvoice;
    int voice = SNDVOICEI_get(shandle);

    if (voice >= 0) {
        setvoice = -1;
        while (iSNDpatchkey(voice, &setvoice)) {
            CHANPUB *ppubv = &sndgs.chan[setvoice];
            for (int i = 0; i < ppubv->numchan; i++) {
                CHANPUB *ppubvTemp = &sndgs.chan[ppubv->voices[i]];
                ppubvTemp->pFxVolume[bus].fxLevel = level * 0.007874016f;
                SNDPLATFORM_setfxlevel(ppubv->voices[i], bus);
            }
        }
    }

    return voice;
}
