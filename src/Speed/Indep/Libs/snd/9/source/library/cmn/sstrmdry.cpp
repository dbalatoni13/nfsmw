#include "./sndcmn.h"

int SNDSTRM_drylevel(int sndstreamhandle, int level) {
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);
    int rc;

    if (pssc != NULL) {
        pssc->po.drylevel = level;
        SNDCTRL_drylevel(pssc->shandle, level);
        rc = 0;
    } else {
        rc = -8;
    }

    return rc;
}
