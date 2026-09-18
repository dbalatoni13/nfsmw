#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDvol(int shandle, int vol) {
    int voice = SNDVOICEI_get(shandle);

    if (voice >= 0) {
        float newvol = (float)vol * 0.007874016f;
        int setvoice = -1;

        while (iSNDpatchkey(voice, &setvoice)) {
            CHANPUB *pVoice = &sndgs.chan[setvoice];
            int i;

            for (i = 0; i < pVoice->numchan; i++) {
                int chan = pVoice->voices[i];
                CHANPUB *pv = &sndgs.chan[chan];

                pv->fadePer100Hz = 0.0f;
                if (pv->programmedVol != newvol) {
                    pv->programmedVol = newvol;
                    iSNDcalcvol(chan);
                    Snd::Hal::SetVol(chan);
                }
            }
        }
    }

    return voice;
}

#ifdef __cplusplus
}
#endif

void Snd::Hal::SetVol(int voice) {
    SetVolInternal(voice);

    CHANPUB *pv = &sndgs.chan[voice];

    if (pv->rendermode & 4) {
        int i;

        for (i = 0; i < Snd::gMaxFxBuses; i++) {
            SNDPLATFORM_setfxlevel(voice, i);
        }
    } else {
        SNDPLATFORM_setfxlevel(voice, 0);
    }
}
