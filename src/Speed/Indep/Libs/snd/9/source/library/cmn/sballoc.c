#include "./sndcmn.h"

int SNDBANKI_alloc() {
    int i;

    for (i = 0; i < sndgs.sso.set.maxbanks; i++) {
        if (sndgs.banklist[i].phdr == NULL) {
            return i;
        }
    }

    return -9;
}

TAGGEDPATCH *SNDBANKI_getppatch(BANKVER5 *pb, int patnum) {
    if (patnum >= pb->numpatches) return NULL;

    if (pb->patch[patnum] == NULL) {
        return NULL;
    }

    return reinterpret_cast<TAGGEDPATCH *>((int)&pb->patch[patnum] + (int)pb->patch[patnum]);
}
