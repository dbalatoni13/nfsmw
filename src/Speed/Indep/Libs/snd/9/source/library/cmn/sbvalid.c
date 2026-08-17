#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"

int SNDBANKI_valid(int bhandle) {
    if (bhandle < 0 || bhandle >= sndgs.sso.set.maxbanks) {
        return -8;
    }

    if (sndgs.banklist[bhandle].phdr == NULL) {
        return -8;
    }

    if (sndgs.banklist[bhandle].locked != 0) {
        return -18;
    }

    return 0;
}
