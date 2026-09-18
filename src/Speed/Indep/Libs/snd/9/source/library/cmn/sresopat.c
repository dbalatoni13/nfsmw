#include "./sndcmn.h"

int SNDBANKI_asyncresolvepatch(int playloc, TAGGEDPATCH *ptp, char *pdata, int *pfirstoffset) {
    SNDIPATCHHEADER ph;
    void *ptimbre = &ptp->hdrsize;

    int result;

    do {
        result = SNDI_parsetimbre(&ptimbre, &ph);

        if (ph.rendermode & playloc) {
            SNDPLATFORM_asyncresolvetimbre(&ph, pdata, pfirstoffset);
            SNDBANKI_userdatacallback(&ph, -1, 0);
        }
    } while (result != 0);

    return 0;
}
