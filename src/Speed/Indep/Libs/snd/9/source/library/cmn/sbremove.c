#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "snd/sndo.h"
#include <cstddef>
#include <types.h>

int SNDbankremove(int bhandle) {
    BANKVER5 *pb;
    CHANPUB *pchan;
    TAGGEDPATCH *ptp;
    SNDIPATCHHEADER ph;
    int moretags;
    int i;

    if (bhandle == -1) {
        for (i = 0; i < sndgs.sso.set.maxbanks; i++) {
            SNDbankremove(i);
        }
        return 0;
    }

    if (SNDBANKI_valid(bhandle) != 0) {
        return -8;
    }

    SNDSYS_entercritical();

    pb = sndgs.banklist[bhandle].phdr;

    for (i = 0; i < sndgs.voicestotal; i++) {
        pchan = &sndgs.chan[i];
        if (pchan->bhandle == bhandle) {
            SNDstop(pchan->handle);
        }
    }

    for (i = 0; i < pb->numpatches; i++) {
        ptp = SNDBANKI_getppatch(pb, i);
        if (ptp != NULL) {
            ptp = reinterpret_cast<TAGGEDPATCH *>(reinterpret_cast<int *>(ptp) + 1);
            do {
                moretags = SNDI_parsetimbre((void **)&ptp, &ph);
                SNDBANKI_userdatacallback(&ph, -1, 2);
            } while (moretags != 0);
        }
    }

    if (sndgs.banklist[bhandle].pspuram != NULL) {
        SNDPLATFORM_memfree(0x8, reinterpret_cast<uintptr_t>(sndgs.banklist[bhandle].pspuram));
    }

    sndgs.banklist[bhandle].phdr = NULL;
    sndgs.banklist[bhandle].locked = 0;
    SNDSYS_leavecritical();
    return 0;
}
