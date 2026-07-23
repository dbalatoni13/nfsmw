#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"

#if __cplusplus
extern "C" {
#endif

int SND3dpos(STREAMHANDLE shandle, int azimuth, int elevation) {
    elevation = SNDI_clipint32(elevation, -0x4000, 0x3FFF);

    int voice = SNDVOICEI_get(shandle);
    if (voice >= 0) {
        int setvoice = -1;
        while (iSNDpatchkey(voice, &setvoice)) {
            CHANPUB *pVoice = &sndgs.chan[setvoice];
            if (pVoice->numchan != 1) continue;

            pVoice->elevation = elevation;
            pVoice->azimuth = pVoice->builtinazimuth + azimuth;
            pVoice->azimuth += pVoice->azimuthOffsets[0];

            if (Snd::gFoldDownTarget == FOLDDOWNTARGET_MONO) {
                pVoice->azimuth = 0;
            }
            Snd::Hal::SetPan(setvoice);
        }
    }

    return voice;
}

#if __cplusplus
}
#endif
