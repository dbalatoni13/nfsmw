#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"

int SNDCTRL_lowpass(int shandle, int lowpasscutoff) {
    int setvoice;
    int voice = SNDVOICEI_get(shandle);

    if (voice >= 0) {
        setvoice = -1;
        while (iSNDpatchkey(voice, &setvoice)) {
            SNDPLATFORM_lowpass(setvoice, lowpasscutoff);
        }
    }

    return voice;
}
