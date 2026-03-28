#include "snd/source/library/cmn/sndcmn.h"

int SNDSTRM_getprogvol(int sndstreamhandle) {
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);

    int vol = -8;
    if (!pssc) return vol;

    vol = SNDCTRL_getprogvol(pssc->shandle);
    if (vol >= 0) return vol;
    
    return SNDI_ftoifast(pssc->sourceChannelState[0].vol);
}
