#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

void STREAM_setgreedylevel(int streamhandle, int greedylevel);

#ifdef __cplusplus
}
#endif

int SNDSTRM_setgreedylevel(int sndstreamhandle, int greedylevel) {
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);
    int rc;

    if (pssc != NULL) {
        STREAM_setgreedylevel(pssc->streamhandle, greedylevel);
        rc = 0;
    } else {
        rc = -8;
    }

    return rc;
}
