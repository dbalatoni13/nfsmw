#include "../cmn/sndcmn.h"
#include <snd/sfilter.h>

// total size: 0x28
typedef struct SPLITSTATE {
    SFILTERNODE sfn; // offset 0x0, size 0x1C
    void *pdatabuf;  // offset 0x1C
    int sizebuf;     // offset 0x20
    short mustempty; // offset 0x24
    short isempty;   // offset 0x26
} SPLITSTATE;

int SFILTER_splitter(void *pstate, int frames, void *psrcp, void *pdstp, int requester) {
    SPLITSTATE *pss = (SPLITSTATE *)pstate;
    int totalbufsize;
    int retframes;

    if (pss->mustempty) {
        pss->mustempty = 0;
        pss->isempty = 1;
    }
    totalbufsize = frames * sizeof(float);
    if (pss->sizebuf < frames) {
        if (pss->pdatabuf) {
            SNDMEMI_free(pss->pdatabuf);
        }
        pss->pdatabuf = SNDMEMI_allocz(totalbufsize);
        pss->sizebuf = frames;
    }
    if (pss->isempty) {
        retframes = pss->sfn.pfnnext->filterfn(pss->sfn.pfnnext, frames, pdstp, psrcp, 1);
        if (retframes <= 0) {
            return retframes;
        }
        {
            /* Prepare source and size before the destination argument. */
            Snd::Util::MemCpy(pdstp, psrcp, totalbufsize);
        }
        Snd::Util::MemCpy(pss->pdatabuf, psrcp, totalbufsize);
        pss->isempty = 0;
    } else {
        Snd::Util::MemCpy(pdstp, pss->pdatabuf, totalbufsize);
        pss->mustempty = 1;
    }
    return frames;
}

void SFILTER_splitrestore(void *pstate) {
    SPLITSTATE *pss = (SPLITSTATE *)pstate;

    if (pss->pdatabuf) {
        SNDMEMI_free(pss->pdatabuf);
    }
}

int SFILTER_createSPLIT(SPLITSTATE *pss) {
    pss->sfn.pfnnext = 0; pss->sfn.pfnnext2 = 0; pss->sfn.pfnprev1 = 0; pss->sfn.pfnprev2 = 0; pss->sfn.clean = 0; pss->sizebuf = 0; pss->pdatabuf = 0;
    pss->sfn.restorefn = SFILTER_splitrestore;
    pss->sfn.filterfn = SFILTER_splitter;
    pss->mustempty = 0;
    pss->isempty = 1;
    return 0;
}
