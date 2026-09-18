#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDover(int shandle) {
    return SNDVOICEI_get(shandle) < 0;
}

#ifdef __cplusplus
}
#endif
