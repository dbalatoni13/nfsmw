#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include <cstddef>

namespace Snd {

const signed char gChannelToVoiceIndexLut[6][6] = {
    { 0, -128, -128, -128, -128, -128 },
    { -128, 0, 1, -128, -128, -128 },
    { -128, -128, -128, -128, -128, -128 },
    { -128, 0, 1, 2, 3, -128 },
    { -128, -128, -128, -128, -128, -128 },
    { 1, 0, 2, 3, 4, 5 }
};

} // namespace Snd


#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_setvol(int sndStrmHandle, Channel sourceChannel, float volume) {
    int i = 0;
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndStrmHandle);

    if (pssc == NULL) {
        return -8;
    }

    if (sourceChannel == -1) {
        for (; i < 6; i++) {
            Snd::StreamVolSetState(&pssc->sourceChannelState[i], volume);
        }
    } else {
        Snd::StreamVolSetState(&pssc->sourceChannelState[sourceChannel], volume);
    }

    int voice = SNDVOICEI_get(pssc->shandle);
    if (voice >= 0) {
        if (sourceChannel == -1) {
            CHANPUB *pVoice = &sndgs.chan[voice];
            bool volChanged = false;
            for (i = 0; i < pVoice->numchan; i++) {
                CHANPUB *pv = &sndgs.chan[pVoice->voices[i]];;
                int sourceVoice = pVoice->voices[i];

                if (pv->programmedVol != volume) {
                    pv->programmedVol = volume;
                    Snd::Util::FastVol(pv);
                    Snd::Hal::SetVol(sourceVoice);
                    volChanged = true;
                }
            }
        } else {
            CHANPUB *pVoice = &sndgs.chan[voice];
            int channelIndex = Snd::gChannelToVoiceIndexLut[pVoice->numchan - 1][sourceChannel];
            if (channelIndex == -0x80) goto done;

            CHANPUB *pv = &sndgs.chan[pVoice->voices[channelIndex]];
            int sourceVoice = pVoice->voices[channelIndex];

            pv->fadePer100Hz = 0.0f;
            if (pv->programmedVol != volume) {
                pv->programmedVol = volume;
                Snd::Util::FastVol(pv);
                Snd::Hal::SetVol(sourceVoice);
            }
        }
    }

done:
    return 0;
}

#ifdef __cplusplus
}
#endif
