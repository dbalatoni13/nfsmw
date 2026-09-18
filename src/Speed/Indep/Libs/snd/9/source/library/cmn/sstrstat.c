#include "./sndcmn.h"

Snd::SNDSTREAMREQUEST *SNDSTRMI_getrequestptr(int sndrequesthandle);

int SNDSTRM_requeststatus(int sndrequesthandle, SNDREQUESTSTATUS *psrs) {
    psrs->state = 0;
    psrs->currenttime = 0;
    psrs->timetoend = 0;
    psrs->timebuffered = 0;

    if (sndrequesthandle < 0) {
        return -8;
    }

    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndrequesthandle & 0xFF);
    if (pssc == NULL) {
        return -8;
    }

    Snd::SNDSTREAMREQUEST *psr = SNDSTRMI_getrequestptr(sndrequesthandle);
    if (psr != NULL) {
        if (psr->continuation != 0) {
            unsigned short samplerate;

            if ((Snd::SNDSTREAMREQUEST *)pssc->allocatedrequests.phead == psr) {
                psrs->state = 2;
                samplerate = pssc->cursf.samplerate;
            } else {
                psrs->state = 1;
                samplerate = pssc->newsf.samplerate;
            }

            float rate = 1000.0f / samplerate;

            psrs->currenttime = SNDI_ftoiround(psr->currentframe * rate);
            psrs->timetoend = SNDI_ftoiround((psr->totalframes - psr->currentframe) * rate);
            psrs->timebuffered = SNDI_ftoiround(psr->outstandingframes * rate);
        } else {
            psrs->state = 0;
        }
    } else {
        psrs->state = 3;
    }

    return 0;
}
