#include "./sndcmn.h"

namespace Snd {
extern const unsigned char gAzimuthFoldDownLut[4][6][6];
extern const unsigned char gAzimuthSpacingsLut[6][6];
} // namespace Snd

// slinki.c
SNDLINKNODE *SNDLINKI_pop(SNDLINKLIST *plist);
void SNDLINKI_init(SNDLINKLIST *plist);
void SNDLINKI_push(SNDLINKLIST *plist, SNDLINKNODE *pnode);
void SNDLINKI_pushtail(SNDLINKLIST *plist, SNDLINKNODE *pnode);
void SNDLINKI_remove(SNDLINKLIST *plist, SNDLINKNODE *pnode);

typedef struct STREAMCHUNKHDR {
    int type; // offset 0x0, size 0x4
    int size; // offset 0x4, size 0x4
} STREAMCHUNKHDR;

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void *s1, const void *s2, unsigned int n);
void *memset(void *s, int c, unsigned int n);

// realstream/stream.cpp
STREAMCHUNKHDR *STREAM_get(int sndstreamhandle);
void STREAM_release(int sndstreamhandle, STREAMCHUNKHDR *chunk);
void STREAM_kill(int sndstreamhandle);
void STREAM_destroy(int sndstreamhandle);
int STREAM_state(int sndstreamhandle);
int STREAM_buffersize(int sndstreamhandle);
int STREAM_create(int requests, int filters, int taps, void *buffer, int size);
int STREAM_queuefile(int handle, const char *fname, int offset, int endchunkid);
int STREAM_queuemem(int handle, void *address, int length, int endchunkid);

// spktplay.c
int SNDPKTPLAY_submitspace(int packetinstancehandle);

#ifdef __cplusplus
}
#endif

int SNDSTRMI_create(SNDPLAYOPTS *pspo, int maxrequests, int maxchunks, void *pmem, int memsize, int streamhandle, int fromtap);
int SNDSTRMI_queue(int sndstreamhandle, int holdtime, char *param1, int param2, int queuetype);
void SNDSTRMI_startstream(SNDSTREAMCHANNEL *pssc);
int SNDSTRMI_calcdatarate(SNDSAMPLEFORMAT *pssf);
int SNDSTRMI_destroyall();
void SNDSTRMI_removerequest(int sndrequesthandle);
void SNDSTRMI_releasecallback(void *psamples, void *pclientdata);
void SNDSTRMI_framescallback(int pktchan, int numframes, void *pclientdata);
int SNDSTRMI_parseheader(int chan, STREAMCHUNKHDR *chunk);
void SNDSTRMI_parsedata(SNDSTREAMCHANNEL *pssc, STREAMCHUNKHDR *pchunk);
int SNDSTRMI_parsechunk(int chan, STREAMCHUNKHDR *pchunk);
int SNDSTRMI_isheld(SNDSTREAMCHANNEL *pssc);
void SNDSTRMI_service();
int SNDSTRMI_numcreated();

namespace Snd {

signed char gVoiceIndexToChannelLut[6][6] = {
    { 0, -128, -128, -128, -128, -128 },
    { 1, 2, -128, -128, -128, -128 },
    { -128, -128, -128, -128, -128, -128 },
    { 1, 2, 3, 4, -128, -128 },
    { -128, -128, -128, -128, -128, -128 },
    { 1, 0, 2, 3, 4, 5 }
};

} // namespace Snd

SNDSTREAMSTATE sndss;

static inline void Copy(int &dst, int src) {
    dst = src;
}

static inline void PutB(int &dst, int src) {
    Copy(dst, src);
}

namespace Snd {

void StreamRemoveFaders(SNDSTREAMCHANNEL *pSndStrmChannel) {
    int i;

    for (i = 0; i < 6; i++) {
        StreamSourceChannelState *pSourceChannelState = &pSndStrmChannel->sourceChannelState[i];

        if (pSourceChannelState->fader.incrementPerUpdate != 0.0f) {
            Util::RemoveVariableTimerClient(&pSourceChannelState->fader.variableTimerClient);
        }
    }
}

} // namespace Snd

void SNDSTRMI_startstream(SNDSTREAMCHANNEL *pssc) {
    unsigned int i;
    int j;
    int voice;

    pssc->shandle = SNDPKTPLAY_start(pssc->packetinstancehandle, &pssc->cursf, &pssc->cursa, &pssc->po);
    voice = SNDVOICEI_get(pssc->shandle);

    for (i = 0; i < pssc->cursf.channels; i++) {
        int sourceVoice = sndgs.chan[voice].voices[i];
        CHANPUB *pSourceVoice = &sndgs.chan[sourceVoice];
        Snd::Channel sourceChannel = (Snd::Channel)Snd::gVoiceIndexToChannelLut[pssc->cursf.channels - 1][i];
        Snd::StreamSourceChannelState *pSourceChannelState = &pssc->sourceChannelState[sourceChannel];

        for (j = 0; j < Snd::gMaxFxBuses; j++) {
            pSourceVoice->pFxVolume[j].fxLevel = pssc->pFxVolume[j].fxLevel;
        }

        pSourceVoice->programmedVol = pSourceChannelState->vol;
        Snd::Util::FastVol(pSourceVoice);
        Snd::Hal::SetVol(sourceVoice);
    }

    if (pssc->useOldAzimuthBehaviour == 0) {
        const unsigned char *pFoldedAzimuths = Snd::gAzimuthFoldDownLut[Snd::gFoldDownTarget][pssc->cursf.channels - 1];
        const unsigned char *pAzimuthSpacings = Snd::gAzimuthSpacingsLut[pssc->cursf.channels - 1];

        for (i = 0; i < pssc->cursf.channels; i++) {
            int sourceVoice = sndgs.chan[voice].voices[i];
            CHANPUB *pSourceVoice = &sndgs.chan[sourceVoice];
            Snd::Channel sourceChannel = (Snd::Channel)Snd::gVoiceIndexToChannelLut[pssc->cursf.channels - 1][i];
            Snd::StreamSourceChannelState *pSourceChannelState = &pssc->sourceChannelState[sourceChannel];

            if (pSourceChannelState->azimuth == -1000000.0f) {
                pSourceVoice->azimuth = pFoldedAzimuths[i] << 8;
            } else {
                pSourceVoice->azimuth = SNDI_ftoifast(pSourceChannelState->azimuth * 182.04445f);
                if (pSourceChannelState->isDirect == 0) {
                    pSourceVoice->azimuth += pAzimuthSpacings[i] << 8;
                }
            }

            Snd::Hal::SetPan(sourceVoice);
        }
    }

    for (i = 0; i < sizeof(pssc->filter) / sizeof(pssc->filter[0]); i++) {
        if (pssc->filter[i].filter != NULL) {
            SNDCTRL_filteradd(pssc->shandle, &pssc->filter[i]);
        }
    }

    pssc->state = 1;
}

int SNDSTRMI_calcdatarate(SNDSAMPLEFORMAT *pssf) {
    int avgdatarate = pssf->samplerate;
    int ratio = 0;

    avgdatarate *= pssf->channels;

    if (pssf->samplerep == 10) {
        ratio = 136;
    } else if (pssf->samplerep == 4) {
        ratio = 51;
    } else if (pssf->samplerep == 22) {
        ratio = 102;
    } else if (pssf->samplerep == 7) {
        ratio = 512;
    } else if (pssf->samplerep == 16) {
        return pssf->channels * 8;
    } else if (pssf->samplerep == 23) {
        return pssf->channels * 8;
    }

    avgdatarate *= ratio;
    avgdatarate >>= 8;

    return avgdatarate;
}

int SNDSTRMI_destroyall() {
    int i;

    for (i = 0; i < sndgs.sso.set.maxstreams; i++) {
        SNDSTRM_destroy(i);
    }

    return 0;
}

SNDSTREAMCHANNEL *SNDSTRMI_getstreamptr(int sndstreamhandle) {
    if (sndstreamhandle >= sndgs.sso.set.maxstreams || sndstreamhandle < 0) {
        return NULL;
    }

    return sndss.pssc[sndstreamhandle];
}

void SNDSTRMI_removerequest(int sndrequesthandle) {
    SNDSTREAMCHANNEL *pssc = sndss.pssc[sndrequesthandle & 0xFF];
    Snd::SNDSTREAMREQUEST *pssr = SNDSTRMI_getrequestptr(sndrequesthandle);

    SNDLINKI_remove(&pssc->allocatedrequests, (SNDLINKNODE *)pssr);
    SNDLINKI_push(&pssc->freerequests, (SNDLINKNODE *)pssr);

    if (pssc->parsingrequest == pssr) {
        pssc->parsingrequest = NULL;
    }
}

void SNDSTRMI_releasecallback(void *psamples, void *pclientdata) {
    SNDSTREAMCHANNEL *pssc;
    unsigned int *psndrequesthandle = ((unsigned int **)psamples)[-1];

    pssc = sndss.pssc[*psndrequesthandle & 0xFF];

    STREAM_release(pssc->streamhandle, (STREAMCHUNKHDR *)psndrequesthandle);
}

void SNDSTRMI_framescallback(int pktchan, int numframes, void *pclientdata) {
    SNDSTREAMCHANNEL *pssc = (SNDSTREAMCHANNEL *)pclientdata;
    Snd::SNDSTREAMREQUEST *pssr;
    int residual = 0;
    int timeout = 0;

nextrequest:
    timeout++;
    pssr = (Snd::SNDSTREAMREQUEST *)pssc->allocatedrequests.phead;

    if (timeout > 200) {
        SNDSTRMI_removerequest(pssr->sndrequesthandle);
        return;
    }

    if (numframes > pssr->outstandingframes) {
        residual = numframes - pssr->outstandingframes;
        numframes -= residual;
    }

    pssr->currentframe += numframes;
    pssr->outstandingframes -= numframes;

    if (pssr->currentframe >= pssr->totalframes) {
        SNDSTRMI_removerequest(pssr->sndrequesthandle);
    }

    if (residual != 0) {
        numframes = residual;
        residual = 0;
        goto nextrequest;
    }
}

int SNDSTRMI_parseheader(int chan, STREAMCHUNKHDR *chunk) {
    SNDSTREAMCHANNEL *pssc = sndss.pssc[chan];
    Snd::SNDSTREAMREQUEST *pssr;
    SNDSAMPLEDESC ssd;
    SNDUSERDATACBINFO udinfo;
    int userdatachunks = 0;
    int i;

    if (pssc->parsingrequest == NULL) {
        pssc->parsingrequest = (Snd::SNDSTREAMREQUEST *)pssc->allocatedrequests.phead;
    } else {
        pssc->parsingrequest = (Snd::SNDSTREAMREQUEST *)pssc->parsingrequest->node.pnext;
    }

    pssr = (Snd::SNDSTREAMREQUEST *)pssc->parsingrequest;

    SNDI_patchtohdr(NULL, (TAGGEDPATCH *)(chunk + 1), &pssc->newsf, &pssc->newsa, &ssd,
                    (unsigned char *)&pssr->isgeneric);

    pssr->totalframes = ssd.totalframes;
    pssr->continuation = 0;

    while (pssc->newsa.puserdata[userdatachunks] != NULL) {
        udinfo.type = 3;
        udinfo.pdata = pssc->newsa.puserdata[userdatachunks];
        udinfo.size = pssc->newsa.userdatasize[userdatachunks];
        udinfo.sndstrmrequest = pssr->sndrequesthandle;
        pssc->newsa.puserdata[userdatachunks] = NULL;
        pssc->newsa.userdatasize[userdatachunks] = 0;
        userdatachunks++;

        for (i = 0; i < sndgs.numuserdataclients; i++) {
            sndgs.userdataclient[i](&udinfo);
        }
    }

    STREAM_release(pssc->streamhandle, chunk);

    pssr->avgdatarate = SNDSTRMI_calcdatarate(&pssc->newsf);

    if (memcmp(&pssc->cursf, &pssc->newsf, sizeof(SNDSAMPLEFORMAT)) != 0 ||
        memcmp(&pssc->cursa, &pssc->newsa, sizeof(SNDSAMPLEATTR)) != 0 || pssc->newsa.ptsdata[0] != NULL) {
        if (pssc->cursf.samplerate != 0) {
            pssc->state = 2;
            return 0;
        }

        pssc->cursf = pssc->newsf;
        pssc->cursa = pssc->newsa;
        pssc->newsa.ptsdata[0] = NULL;
    }

    if (pssc->state != 1) {
        SNDSTRMI_startstream(pssc);
        pssc->state = 1;
    }

    return 0;
}

void SNDSTRMI_parsedata(SNDSTREAMCHANNEL *pssc, STREAMCHUNKHDR *pchunk) {
    SNDPACKET packet;
    Snd::SNDSTREAMREQUEST *pssr = (Snd::SNDSTREAMREQUEST *)pssc->parsingrequest;
    int i;
    char *psampledata;
    unsigned int *pint;
    int *intTemp;
    int *intTemp1;
    int temp;

    intTemp = (int *)(pchunk + 1);

    if (pssr->isgeneric != 0) {
        PutB(temp, *intTemp);
        packet.numframes = temp;
    } else {
        packet.numframes = *intTemp;
    }

    pint = (unsigned int *)(pchunk + 1) + 1;
    psampledata = (char *)&pint[pssc->cursf.channels];

    for (i = 0; i < pssc->cursf.channels; i++) {
        if (pssr->isgeneric != 0) {
            PutB(temp, pint[i]);
            packet.psamples[i] = psampledata + temp;
        } else {
            packet.psamples[i] = psampledata + pint[i];
        }
    }

    if (packet.numframes == 0) {
        STREAM_release(pssc->streamhandle, pchunk);
    } else {
        pint = (unsigned int *)packet.psamples[0];
        pint[-1] = (sndptrint)pchunk;
        intTemp = (int *)pchunk;
        *intTemp = pssr->sndrequesthandle;
        pssr->outstandingframes += packet.numframes;
        packet.continuation = pssr->continuation;
        SNDPKTPLAY_submit(pssc->packetinstancehandle, &packet);
        pssr->continuation = 1;
    }
}

int SNDSTRMI_parsechunk(int chan, STREAMCHUNKHDR *pchunk) {
    SNDSTREAMCHANNEL *pssc = sndss.pssc[chan];

    if (pchunk->type == 0x5343446C) {
        SNDSTRMI_parsedata(pssc, pchunk);
    } else if (pchunk->type == 0x5343486C) {
        SNDSTRMI_parseheader(chan, pchunk);
        return 0;
    } else {
        STREAM_release(pssc->streamhandle, pchunk);
    }

    return 1;
}

int SNDSTRMI_isheld(SNDSTREAMCHANNEL *pssc) {
    Snd::SNDSTREAMREQUEST *pssr = (Snd::SNDSTREAMREQUEST *)pssc->parsingrequest;
    unsigned int bufferedtime;
    unsigned int gettable;

    if (pssr == NULL) {
        return 0;
    }

    if (pssr->avgdatarate == 0) {
        return 0;
    }

    if (pssr->holdtime < 0) {
    held:
        return 1;
    }

    if (pssr->holdtime != 0) {
        gettable = STREAM_gettable(pssc->streamhandle);

        if (gettable > 4000000) {
            gettable = 4000000;
        }

        bufferedtime = gettable * 1000 / pssr->avgdatarate;

        if (bufferedtime >= pssr->holdtime || STREAM_state(pssc->streamhandle) == 2) {
            pssr->holdtime = 0;
            return 0;
        }

        if (pssc->freerequests.items > 0) {
            goto held;
        }

        if (STREAM_state(pssc->streamhandle) != 0) {
            goto held;
        }

        pssr->holdtime = 0;
    }

    return 0;
}

void SNDSTRMI_service() {
    STREAMCHUNKHDR *pchunk;
    SNDSTREAMCHANNEL *pssc;
    int chan;
    int ret;
    int space;

    SNDSYS_entercritical();

    for (chan = 0; chan < sndgs.sso.set.maxstreams; chan++) {
        pssc = sndss.pssc[chan];

        if (pssc != NULL && pssc->allocatedrequests.items != 0) {
            if (pssc->state == 2) {
                if ((int)SNDPKTPLAY_framesoutstanding(pssc->packetinstancehandle) > 0) {
                    continue;
                }

                pssc->cursf = pssc->newsf;
                pssc->cursa = pssc->newsa;
                pssc->newsa.ptsdata[0] = NULL;
                SNDPKTPLAY_stop(pssc->packetinstancehandle);
                SNDSTRMI_startstream(pssc);
            }

            if (SNDSTRMI_isheld(pssc) == 0) {
                if (pssc->state == 1) {
                    space = SNDPKTPLAY_submitspace(pssc->packetinstancehandle);

                    if (space <= 0) {
                        continue;
                    }
                } else {
                    space = 8;
                }

                ret = 0;

                do {
                    space--;
                    pchunk = STREAM_get(pssc->streamhandle);

                    if (pchunk != NULL) {
                        ret = SNDSTRMI_parsechunk(chan, pchunk);
                    }
                } while (ret != 0 && space > 0);
            }
        }
    }

    SNDSYS_leavecritical();
}

int SNDSTRMI_numcreated() {
    int i;
    int numcreated = 0;

    for (i = 0; i < sndgs.sso.set.maxstreams; i++) {
        if (sndss.pssc[i] != NULL) {
            numcreated++;
        }
    }

    return numcreated;
}

int SNDSTRMI_create(SNDPLAYOPTS *pspo, int maxrequests, int maxchunks, void *pmem, int memsize, int streamhandle,
                    int fromtap) {
    SNDSTREAMCHANNEL *pssc;
    Snd::SNDSTREAMREQUEST *prequest;
    void *ppacketmem;
    int sndstreamhandle;
    int i;
    int overhead;
    unsigned int temphandle;
    void *pMemForFx;
    float floatVol;

    SNDSYS_entercritical();

    for (i = 0; i < sndgs.sso.set.maxstreams; i++) {
        if (sndss.pssc[i] == NULL) {
            goto allocated;
        }
    }

    SNDSYS_leavecritical();

    return SNDERR_ALLOCATE;

allocated:
    sndstreamhandle = i;
    pssc = (SNDSTREAMCHANNEL *)pmem;
    pmem = (char *)pmem + sizeof(SNDSTREAMCHANNEL) + Snd::gMaxFxBuses * sizeof(Snd::SNDSTREAMFXVOLUME);
    memsize -= sizeof(SNDSTREAMCHANNEL) + Snd::gMaxFxBuses * sizeof(Snd::SNDSTREAMFXVOLUME);
    pMemForFx = (char *)pssc + sizeof(SNDSTREAMCHANNEL);
    memset(pssc, 0, sizeof(SNDSTREAMCHANNEL));

    prequest = (Snd::SNDSTREAMREQUEST *)pmem;
    pmem = (char *)pmem + maxrequests * sizeof(Snd::SNDSTREAMREQUEST);
    memsize -= maxrequests * sizeof(Snd::SNDSTREAMREQUEST);

    ppacketmem = pmem;
    overhead = SNDPKTPLAY_overhead(maxchunks);
    pmem = (char *)pmem + overhead;
    memsize -= overhead;

    SNDLINKI_init(&pssc->allocatedrequests);
    SNDLINKI_init(&pssc->freerequests);

    for (i = 0; i < maxrequests; i++) {
        SNDLINKI_push(&pssc->freerequests, (SNDLINKNODE *)&prequest[i]);
    }

    pssc->packetinstancehandle = SNDPKTPLAY_create(SNDSTRMI_releasecallback, SNDSTRMI_framescallback, pssc, ppacketmem,
                                                  SNDPKTPLAY_overhead(maxchunks));

    if (pssc->packetinstancehandle < 0) {
        SNDSYS_leavecritical();

        return pssc->packetinstancehandle;
    }

    if (fromtap != 0) {
        pssc->streamhandle = streamhandle;
        pssc->fromtap = 1;
    } else {
        temphandle = STREAM_create(maxrequests + 2, 1, 1, pmem, memsize);

        if (temphandle == 0) {
            SNDPKTPLAY_destroy(pssc->packetinstancehandle);
            SNDSYS_leavecritical();

            return SNDERR_ALLOCATE;
        }

        pssc->streamhandle = temphandle;
        pssc->fromtap = fromtap;
    }

    pssc->lastsndrequesthandle = 0;
    pssc->shandle = -1;
    pssc->po = *pspo;
    pssc->pFxVolume = (Snd::SNDSTREAMFXVOLUME *)pMemForFx;

    for (i = 0; i < Snd::gMaxFxBuses; i++) {
        pssc->pFxVolume[i].fxLevel = pssc->po.fxlevel0 * 0.007874016f;
    }

    floatVol = pspo->vol * 0.007874016f;

    for (i = 0; i < 6; i++) {
        Snd::StreamSourceChannelState *pSourceChannelState = &pssc->sourceChannelState[i];

        pSourceChannelState->sourceChannel = i;
        pSourceChannelState->vol = floatVol;
        pSourceChannelState->isDirect = 0;
        pSourceChannelState->azimuth = -1000000.0f;
    }

    pssc->useOldAzimuthBehaviour = 1;

    if (SNDSTRMI_numcreated() == 0) {
        iSNDserveraddclient(SNDSTRMI_service);
        sndgs.streamrestore = SNDSTRMI_destroyall;
    }

    sndss.pssc[sndstreamhandle] = pssc;

    if (fromtap == 0) {
        SNDSTRM_setgreedylevel(sndstreamhandle, STREAM_buffersize(pssc->streamhandle) / 3);
    }

    pssc->newsa.ptsdata[0] = NULL;
    SNDSTRM_purge(sndstreamhandle);
    SNDSYS_leavecritical();

    return sndstreamhandle;
}

int SNDSTRMI_queue(int sndstreamhandle, int holdtime, char *param1, int param2, int queuetype) {
    SNDSTREAMCHANNEL *pssc;
    Snd::SNDSTREAMREQUEST *pssr;
    int streamrequestid;
    int sndrequesthandle;

    pssc = SNDSTRMI_getstreamptr(sndstreamhandle);

    if (pssc == NULL) {
        return SNDERR_INV_PARAM;
    }

    if (pssc->freerequests.items == 0) {
        return SNDERR_TOO_MANY;
    }

    if (queuetype == 0) {
        param2 = STREAM_queuefile(pssc->streamhandle, param1, param2, 0x6C454353);
    } else if (queuetype == 1) {
        param2 = STREAM_queuemem(pssc->streamhandle, param1, 0, 0x6C454353);
    }

    streamrequestid = param2;

    if (streamrequestid == 0) {
        return SNDERR_GENERAL;
    }

    SNDSYS_entercritical();

    pssr = (Snd::SNDSTREAMREQUEST *)SNDLINKI_pop(&pssc->freerequests);
    memset(pssr, 0, sizeof(Snd::SNDSTREAMREQUEST));
    SNDLINKI_pushtail(&pssc->allocatedrequests, (SNDLINKNODE *)pssr);

    pssr->streamrequestid = streamrequestid;
    pssc->lastsndrequesthandle += 0x100;

    if (pssc->lastsndrequesthandle < 0) {
        pssc->lastsndrequesthandle = 0;
    }

    pssr->sndrequesthandle = pssc->lastsndrequesthandle | sndstreamhandle;
    pssr->holdtime = holdtime;
    sndrequesthandle = pssr->sndrequesthandle;

    SNDSYS_leavecritical();

    return sndrequesthandle;
}

#ifdef __cplusplus
extern "C" {
#endif

int SNDSTRM_create(SNDPLAYOPTS *pspo, int maxrequests, int maxchunks, void *pmem, int memsize) {
    int ret = SNDSTRMI_create(pspo, maxrequests, maxchunks, pmem, memsize, 0, 0);

    return ret;
}

int SNDSTRM_destroy(int sndstreamhandle) {
    SNDSTREAMCHANNEL *pssc;
    int streamhandle;
    int fromtap;

    SNDSYS_entercritical();

    pssc = SNDSTRMI_getstreamptr(sndstreamhandle);

    if (pssc == NULL) {
        SNDSYS_leavecritical();

        return SNDERR_INV_PARAM;
    }

    SNDSTRM_purge(sndstreamhandle);
    Snd::StreamRemoveFaders(pssc);

    if (SNDSTRMI_numcreated() == 1) {
        iSNDserverremoveclient(SNDSTRMI_service);
        sndgs.streamrestore = NULL;
    }

    SNDPKTPLAY_destroy(pssc->packetinstancehandle);

    streamhandle = pssc->streamhandle;
    fromtap = pssc->fromtap;
    sndss.pssc[sndstreamhandle] = NULL;

    SNDSYS_leavecritical();

    if (fromtap == 0) {
        STREAM_destroy(streamhandle);
    }

    return 0;
}

int SNDSTRM_queuefile(int sndstreamhandle, int holdtime, const char *filename, int offset) {
    int ret = SNDSTRMI_queue(sndstreamhandle, holdtime, (char *)filename, offset, 0);

    return ret;
}

int SNDSTRM_purge(int sndStrmHandle) {
    SNDSTREAMCHANNEL *pssc;
    Snd::SNDSTREAMREQUEST *pssr;
    int i;

    SNDSYS_entercritical();

    pssc = SNDSTRMI_getstreamptr(sndStrmHandle);

    if (pssc == NULL) {
        SNDSYS_leavecritical();

        return SNDERR_INV_PARAM;
    }

    if (pssc->shandle >= 0) {
        SNDPKTPLAY_stop(pssc->packetinstancehandle);
    }

    pssc->shandle = -1;

    if (pssc->fromtap == 0) {
        STREAM_kill(pssc->streamhandle);
    }

    if (pssc->newsa.ptsdata[0] != NULL) {
        for (i = 0; i < pssc->newsf.channels; i++) {
            SNDMEMI_free(pssc->newsa.ptsdata[i]);
        }
    }

    do {
        pssr = (Snd::SNDSTREAMREQUEST *)SNDLINKI_pop(&pssc->allocatedrequests);

        if (pssr != NULL) {
            SNDLINKI_push(&pssc->freerequests, (SNDLINKNODE *)pssr);
        }
    } while (pssr != NULL);

    pssc->parsingrequest = NULL;
    pssc->state = 0;

    memset(&pssc->cursf, 0, sizeof(SNDSAMPLEFORMAT));
    memset(&pssc->newsf, 0, sizeof(SNDSAMPLEFORMAT));
    memset(&pssc->cursa, 0, sizeof(SNDSAMPLEATTR));
    memset(&pssc->newsa, 0, sizeof(SNDSAMPLEATTR));

    SNDSYS_leavecritical();

    return 0;
}

#ifdef __cplusplus
}
#endif
