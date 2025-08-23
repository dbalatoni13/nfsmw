#include "snd/source/library/cmn/sndcmn.h"

int SNDBANK_patchinfo(int bhandle, int patnum, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDSAMPLEDESC *pssd) {
    TAGGEDPATCH *ptp;
    int i;
    
    if (SNDBANKI_valid(bhandle) >= 0) {
        ptp = SNDBANKI_getppatch(sndgs.banklist[bhandle].phdr, patnum);

        if (!ptp) return -8;
    } else return -8;

    SNDSYS_entercritical();

    SNDI_patchtohdr(sndgs.banklist[bhandle].phdr, ptp, pssf, pssa, pssd, 0);

    for (i = 0; i < pssf->channels; i++) {
        if (pssa->ptsdata[i] != 0) {
            SNDMEMI_free(pssa->ptsdata[i]);
            pssa->ptsdata[i] = 0;
            pssa->tsdatasize[i] = 0;
        }
    }

    SNDSYS_leavecritical();

    return 0;
}
