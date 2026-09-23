#include "./sndcmn.h"
#include "snd/sndo.h"

SNDPACKETSTATE sndpps;

int SNDPKTPLAYI_gethighchannel(int chan, int channels) {
    int highestchan = -1;
    int highestindex = -1;
    int i;

    for (i = 0; i < channels; i++) {
        if (sndgs.chan[chan].voices[i] > highestchan) {
            highestchan = sndgs.chan[chan].voices[i];
            highestindex = i;
        }
    }

    return highestindex;
}

int SNDPKTPLAYI_overhead(int maxpackets) {
    return maxpackets * 32 + 0x9C;
}

int SNDPKTPLAY_overhead(int maxpackets) {
    int ret = SNDPKTPLAYI_overhead(maxpackets);
    return ret + SNDPLATFORM_packetoverhead();
}

int SNDPKTPLAY_create(void (*preleasefunc)(void *, void *), void (*pframesfunc)(int, int, void *), void *pclientdata, void *pmem, int memsize) {
    SNDPACKETCHAN *ppchan;
    int packetinstancehandle;
    void *pplatformmem;
    int i;

    SNDSYS_entercritical();

    for (i = 0; i < sndgs.sso.set.maxstreams; i++) {
        if (sndpps.ppc[i] == NULL) {
            goto allocated;
        }
    }

    SNDSYS_leavecritical();
    return -9;

allocated:
    packetinstancehandle = SNDPLATFORM_packetplaycreate(i, pmem);
    if (packetinstancehandle < 0) {
        SNDSYS_leavecritical();
        return -6;
    }

    pplatformmem = pmem;
    pmem = reinterpret_cast<void *>(reinterpret_cast<int>(pmem) + SNDPLATFORM_packetoverhead());
    ppchan = reinterpret_cast<SNDPACKETCHAN *>(pmem);
    memsize -= SNDPLATFORM_packetoverhead();

    ppchan->maxpackets = (memsize - 0x9C) / sizeof(SNDPACKETENTRY);
    ppchan->pplatformmem = pplatformmem;
    ppchan->preleasefunc = preleasefunc;
    ppchan->pframesfunc = pframesfunc;
    ppchan->pclientdata = pclientdata;
    ppchan->shandle = -1;
    sndpps.ppc[i] = ppchan;

    SNDSYS_leavecritical();
    return i;
}

int SNDPKTPLAY_submit(int packetinstancehandle, SNDPACKET *psp) {}
