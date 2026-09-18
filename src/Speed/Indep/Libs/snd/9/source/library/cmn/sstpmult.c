#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_pitchmult(int sndstreamhandle, int pitchmult) {
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);

    if (pssc == NULL) {
        return -8;
    }

    if (pitchmult > 0x4000) {
        pitchmult = 0x4000;
    }

    pssc->po.pitchmult = pitchmult;
    SNDpitchmult(pssc->shandle, pitchmult);

    return 0;
}

#ifdef __cplusplus
}
#endif
