#include "./smixi.h"

#ifdef __cplusplus
extern "C" {
#endif

int MIX_getframe(int voice) {
    MIXVOICE *pmv = &sndmix.voices[voice];
    int rc;

    if (pmv->getframe == 0) {
        rc = -1;
    } else {
        rc = pmv->getframe(pmv->pclient);
    }

    return rc;
}

#ifdef __cplusplus
}
#endif
