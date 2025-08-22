#include "snd/source/library/cmn/sndcmn.h"

int SNDtimeremaining(int shandle) {
    CHANPUB *ppubv;
    int currentframe;
    int framesleft;
    int timeleft;
    int voice = SNDVOICEI_get(shandle);
    int setvoice;
    int rc = -7;

    if (voice >= 0) {
        setvoice = -1;

        while (iSNDpatchkey(voice, &setvoice) != 0) {
            ppubv = &sndgs.chan[setvoice];

            if (ppubv->sustainend > 0) {
                currentframe = -7;
            } else {
                currentframe = SNDPLATFORM_getcurframe(setvoice);
            }
            if (currentframe < 0) continue;

            framesleft = ppubv->frames;
            framesleft -= currentframe;
             
            framesleft = (framesleft) * 0x64 / ppubv->samplerate;
            
            if ((unsigned int)ppubv->finalpitch > 0x10) {
                timeleft = (framesleft << 12) / ppubv->finalpitch;
                timeleft = (timeleft << 12) / ppubv->timemult;
            } else {
                timeleft = 0x7FFFFFFF;
            }

            if (timeleft <= rc) continue;

            rc = timeleft;
        }
    }

    return rc;
}
