#include <types.h>
#include "./sndcmn.h"

int SNDBANK_patchinfo(int bhandle, int patnum, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDSAMPLEDESC *pssd) {
    TAGGEDPATCH *ptp;
    int i;

    if (SNDBANKI_valid(bhandle) >= 0) {
        ptp = SNDBANKI_getppatch(sndgs.banklist[bhandle].phdr, patnum);

        if (ptp == 0) {
            return -8;
        }
    } else {
        return -8;
    }

    SNDSYS_entercritical();

    SNDI_patchtohdr(sndgs.banklist[bhandle].phdr, ptp, pssf, pssa, pssd, NULL);

    for (i = 0; i < pssf->channels; i++) {
        if (pssa->ptsdata[i] != NULL) {
            SNDMEMI_free(pssa->ptsdata[i]);
            pssa->ptsdata[i] = NULL;
            pssa->tsdatasize[i] = 0;
        }
    }

    SNDSYS_leavecritical();

    return 0;
}
