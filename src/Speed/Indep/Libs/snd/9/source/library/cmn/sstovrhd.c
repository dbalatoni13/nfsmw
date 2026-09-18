#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_overheadtap(int maxrequests, int maxchunks) {
    int size = Snd::gMaxFxBuses * sizeof(Snd::SNDSTREAMFXVOLUME) + sizeof(SNDSTREAMCHANNEL);
    size += maxrequests * sizeof(Snd::SNDSTREAMREQUEST);

    return size + SNDPKTPLAY_overhead(maxchunks);
}

int SNDSTRM_overhead(int maxrequests, int maxchunks) {
    int size = SNDSTRM_overheadtap(maxrequests, maxchunks);

    size += STREAM_overhead(maxrequests + 2, 1, 1);

    return size;
}

#ifdef __cplusplus
}
#endif
