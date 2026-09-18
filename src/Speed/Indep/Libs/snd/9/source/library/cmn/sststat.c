#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_status(int sndstreamhandle, SNDSTREAMSTATUS *psss) {
    psss->timebuffered = 0;
    psss->currentrequest = 0;
    psss->outstandingrequests = 0;

    SNDSTREAMCHANNEL *pssc = SNDSTRMI_getstreamptr(sndstreamhandle);

    if (pssc == NULL) {
        return -8;
    }

    if ((psss->outstandingrequests = pssc->allocatedrequests.items) != 0) {
        Snd::SNDSTREAMREQUEST *preq = (Snd::SNDSTREAMREQUEST *)pssc->allocatedrequests.phead;

        psss->currentrequest = preq->sndrequesthandle;

        if (pssc->cursf.samplerate != 0) {
            psss->timebuffered =
                SNDPKTPLAY_framesoutstanding(pssc->packetinstancehandle) * 1000 / pssc->cursf.samplerate;

            if (psss->timebuffered == 0) {
                preq = SNDSTRMI_getrequestptr(psss->currentrequest);

                if (preq->avgdatarate != 0) {
                    unsigned int datarate = STREAM_gettable(pssc->streamhandle);

                    if (datarate > 4000000) {
                        datarate = 4000000;
                    }

                    psss->timebuffered = datarate * 1000 / preq->avgdatarate;
                }
            }
        }
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
