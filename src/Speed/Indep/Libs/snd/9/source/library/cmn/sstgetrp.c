#include "./sndcmn.h"

Snd::SNDSTREAMREQUEST *SNDSTRMI_getrequestptr(int sndrequesthandle) {
    if (sndrequesthandle < 0) {
        return NULL;
    }

    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndrequesthandle & 0xFF);

    if (pssc == NULL) {
        return NULL;
    }

    Snd::SNDSTREAMREQUEST *preq = (Snd::SNDSTREAMREQUEST *)pssc->allocatedrequests.phead;

    while (preq != NULL && preq->sndrequesthandle != sndrequesthandle) {
        preq = (Snd::SNDSTREAMREQUEST *)preq->node.pnext;
    }

    return preq;
}
