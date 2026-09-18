#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_fxlevel(int sndstreamhandle, int bus, int level) {
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);

    if (pssc == NULL) {
        return -8;
    }

    pssc->pFxVolume[bus].fxLevel = (float)level * 0.007874016f;

    int voice = SNDVOICEI_get(pssc->shandle);

    if (voice >= 0) {
        int i;

        for (i = 0; i < pssc->cursf.channels; i++) {
            int chan = sndgs.chan[voice].voices[i];
            CHANPUB *target = (CHANPUB *)((unsigned int)sndgs.chan + chan * sizeof(CHANPUB));

            target->pFxVolume[bus].fxLevel = pssc->pFxVolume[bus].fxLevel;
        }

        SNDfxlevel(pssc->shandle, bus, level);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
