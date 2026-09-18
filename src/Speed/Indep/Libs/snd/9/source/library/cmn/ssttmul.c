#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_timemult(int sndstreamhandle, int timemult) {
    if (timemult > 0x2000) {
        timemult = 0x2000;
    } else if (timemult <= 0x7FF) {
        timemult = 0x800;
    }

    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);

    if (pssc == NULL) {
        return -8;
    }

    pssc->po.timemult = timemult;
    SNDCTRL_timemult(pssc->shandle, timemult);

    return 0;
}

#ifdef __cplusplus
}
#endif
