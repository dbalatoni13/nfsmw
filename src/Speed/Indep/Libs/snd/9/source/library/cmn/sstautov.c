#include "./sndcmn.h"

namespace Snd {

void StreamFader(void *pClientData) {
    StreamSourceChannelState *pState = (StreamSourceChannelState *)pClientData;
    float zero = 0.0f;
    float vol = pState->vol + pState->fader.incrementPerUpdate;
    pState->vol = vol;

    if (pState->fader.incrementPerUpdate < zero) {
        if (vol > pState->fader.targetVol)
            goto done;
    } else {
        if (vol < pState->fader.targetVol)
            goto done;
    }
    pState->vol = pState->fader.targetVol;
    pState->fader.incrementPerUpdate = zero;

done:
    SNDSTREAMCHANNEL *pssc =
        (SNDSTREAMCHANNEL *)((char *)pState - 0x13C - pState->sourceChannel * sizeof(StreamSourceChannelState));
    int voice = SNDVOICEI_get(pssc->shandle);
    if (voice >= 0) {
        CHANPUB *pVoice = &sndgs.chan[voice];
        int channelIndex = Snd::gChannelToVoiceIndexLut[pVoice->numchan - 1][pState->sourceChannel];
        if (channelIndex == -0x80)
            goto done2;

        int setvoice = pVoice->voices[channelIndex];
        CHANPUB *pchan = &sndgs.chan[setvoice];
        pchan->programmedVol = pState->vol;
        Snd::Util::FastVol(pchan);
        Snd::Hal::SetVol(setvoice);
    }

done2:
    if (pState->fader.incrementPerUpdate == 0.0f) {
        Snd::Util::RemoveVariableTimerClient(&pState->fader.variableTimerClient);
    }
}

} // namespace Snd

int SNDSTRM_autovol(int sndstreamhandle, int time, int targetvol) {
    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);
    if (pssc == NULL) {
        return -8;
    }

    float targetVol = (float)targetvol * 0.007874016f;
    if (time <= 0) {
        return SNDSTRM_setvol(sndstreamhandle, (Snd::Channel)-1, targetVol);
    }

    int i = 0;
    do {
        Snd::StreamSourceChannelState *pState = &pssc->sourceChannelState[i];
        if (pState->fader.incrementPerUpdate != 0.0f) {
            Snd::Util::RemoveVariableTimerClient(&pState->fader.variableTimerClient);
        }
        pState->fader.targetVol = targetVol;
        pState->fader.incrementPerUpdate =
            (targetVol - pState->vol) * Snd::gVariableTimerPeriod / (float)time;
        if (pState->fader.incrementPerUpdate != 0.0f) {
            pState->fader.variableTimerClient.pClientFunc = Snd::StreamFader;
            pState->fader.variableTimerClient.pClientData = pState;
            Snd::Util::AddVariableTimerClient(&pState->fader.variableTimerClient);
        }
        i++;
    } while (i <= 5);

    return 0;
}
