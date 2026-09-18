#include "./sndcmn.h"

namespace Snd {
extern const unsigned char gAzimuthFoldDownLut[4][6][6];
extern const unsigned char gAzimuthSpacingsLut[6][6];
} // namespace Snd

int SNDSTRM_setazimuth(int sndstreamhandle, int sourcechannel, float azimuth) {
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);
    if (pssc == NULL) {
        return -8;
    }

    int uaz = (unsigned short)SNDI_ftoifast(azimuth * (65536.0f / 360.0f));

    register int isdirect = 0;
    pssc->useOldAzimuthBehaviour = 0;

    if (sourcechannel == -1) {
        register float newAzimuth = azimuth;
        if (azimuth == -1000000.0f) {
            newAzimuth = -1000000.0f;
        }


        for (int i = 0; i < 6; i++) {
            Snd::StreamSourceChannelState *pState = &pssc->sourceChannelState[i];
            pState->isDirect = 0;
            pState->azimuth = newAzimuth;
        }
    } else {
        Snd::StreamSourceChannelState *pState = &pssc->sourceChannelState[sourcechannel];

        if (azimuth == -1000000.0f) {
            pState->azimuth = -1000000.0f;
        } else {
            pState->azimuth = azimuth;
            isdirect = 1;
        }
        pState->isDirect = isdirect;
    }

    int voice = SNDVOICEI_get(pssc->shandle);
    if (voice >= 0) {
        CHANPUB *pchan = sndgs.chan;
        CHANPUB *pVoice = &pchan[voice];

        if (sourcechannel == -1) {
            if (azimuth == -1000000.0f) {
                const unsigned char *plut =
                    Snd::gAzimuthFoldDownLut[Snd::gFoldDownTarget][pVoice->numchan - 1];
                for (int i = 0; i < pVoice->numchan; i++) {
                    int v = pVoice->voices[i];
                    sndgs.chan[v].azimuth = plut[i] << 8;
                    Snd::Hal::SetPan(v);
                }
            } else {
                const unsigned char *plut = Snd::gAzimuthSpacingsLut[pssc->cursf.channels - 1];
                for (int i = 0; i < pVoice->numchan; i++) {
                    int v = pVoice->voices[i];
                    sndgs.chan[v].azimuth = uaz + (plut[i] << 8);
                    Snd::Hal::SetPan(v);
                }
            }
        } else {
            int channelIndex = Snd::gChannelToVoiceIndexLut[pVoice->numchan - 1][sourcechannel];
            int setvoice = pVoice->voices[channelIndex];
            CHANPUB *pv = &pchan[setvoice];

            if (channelIndex == -0x80)
                goto done;

            if (azimuth == -1000000.0f) {
                const unsigned char *plut =
                    Snd::gAzimuthFoldDownLut[Snd::gFoldDownTarget][pVoice->numchan - 1];
                pv->azimuth = plut[channelIndex] << 8;
            } else {
                pv->azimuth = uaz;
            }
            Snd::Hal::SetPan(setvoice);
        }
    }

done:
    return 0;
}
