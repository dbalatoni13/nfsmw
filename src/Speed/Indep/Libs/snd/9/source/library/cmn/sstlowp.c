#include "./sndcmn.h"

int SNDSTRM_lowpass(int sndstreamhandle, int lowpasscutoff) {
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);
    int rc;

    if (pssc != NULL) {
        pssc->po.lowpasscutoff = lowpasscutoff;
        SNDCTRL_lowpass(pssc->shandle, lowpasscutoff);
        rc = 0;
    } else {
        rc = -8;
    }

    return rc;
}
