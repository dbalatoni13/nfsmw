#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"

int SNDCTRL_drylevel(int shandle, int level) {
    int setvoice;
    int voice = SNDVOICEI_get(shandle);

    if (voice >= 0) {
        setvoice = -1;
        while (iSNDpatchkey(voice, &setvoice)) {
            CHANPUB *pVoice = &sndgs.chan[setvoice];
            int i;
            for (i = 0; i < pVoice->numchan; i++) {
                int sourceVoice = pVoice->voices[i];
                sndgs.chan[sourceVoice].drylevel = level;
                Snd::Hal::SetDry(sourceVoice);
            }
        }
    }

    return voice;
}
