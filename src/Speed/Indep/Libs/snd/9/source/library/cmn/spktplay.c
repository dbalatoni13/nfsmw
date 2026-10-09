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

int SNDPKTPLAY_start(int packetinstancehandle, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDPLAYOPTS *pspo) {
    SNDIPATCHHEADER ph;
    SNDPACKETCHAN *ppchan;
    int minvoicerange;
    int maxvoicerange;
    int rendermodeindex = 0;
    int rendermode;
    int shandle;
    int voice;
    int ret;
    int voicesneeded;
    int i;
    int done = 0;

    SNDI_checkplayopts(pspo);

    voicesneeded = pssf->channels;
    ph.numchan = pssf->channels;
    ph.rendermode = pssa->rendermode;
    rendermodeindex = 0;

    ppchan = sndpps.ppc[packetinstancehandle];
    ppchan->sampleformat = *pssf;
    ppchan->lastpackethandle = 0;
    ppchan->outstandingframes = 0;
    ppchan->outstandingplatformframes = 0;
    ppchan->masterCurrentPacket = 0;
    ppchan->masterReleasePacket = 0;
    ppchan->submitpacket = 0;
    ppchan->releasepacket = 0;

    for (i = 0; i < ppchan->sampleformat.channels; i++) {
        ppchan->numPacketsProcessed[i] = 0;
        ppchan->outstandingpackets[i] = 0;
        ppchan->playpacket[i] = 0;
        ppchan->ptsdata[i] = pssa->ptsdata[i];
    }

    while (!done && (rendermode = SNDI_validrendermode(&rendermodeindex, &ph))) {
        CHANPUB *pVoice;
        SNDPLATFORM_getvoicerange(rendermode, &minvoicerange, &maxvoicerange);

        voice = SNDVOICEI_alloc(voicesneeded, 0x65, &shandle, minvoicerange, maxvoicerange);
        if (voice < 0) {
            ppchan->shandle = -9;
            continue;
        }
        ppchan->shandle = shandle;

        ppchan->masterpacketchan = SNDPKTPLAYI_gethighchannel(voice, pssf->channels);

        pVoice = &sndgs.chan[voice];
        pVoice->sustainend = -1;
        pVoice->frames = 0;
        pVoice->samplerate = pssf->samplerate;
        pVoice->samplerep = pssf->samplerep;
        pVoice->numchan = pssf->channels;

        if (pssf->channels == 1) {
            pVoice->azimuth = pspo->azimuth;
        } else {
            Snd::Util::SetDefaultAzimuths(pVoice);
        }

        pVoice->bhandle = -1;
        pVoice->builtinazimuth = 0;
        pVoice->initialdetune = pssa->detune;
        pVoice->progpitch = pspo->pitchmult;
        pVoice->pitchbend = pspo->bend;
        pVoice->progfxlevel = pspo->fxlevel0;
        pVoice->pFxVolume->fxLevel = pspo->fxlevel0 * (1.0f / 127.0f);
        pVoice->bendrange = pssa->bendrange * 100;
        pVoice->patchkey = 0;
        pVoice->ismaster = 0;
        pVoice->pbendtable = NULL;
        pVoice->ppitchlfo = NULL;
        pVoice->programmedVol = pspo->vol * (1.0f / 127.0f);

        for (i = 0; i < pssf->channels; i++) {
            pVoice->azimuthOffsets[i] = pssa->azimuth[i] - sndgs.srcchancfg3d[pssf->channels - 1][i];

            CHANPUB *pSourceVoice = &sndgs.chan[pVoice->voices[i]];
            pSourceVoice->rendermode = rendermode;
            pSourceVoice->drylevel = pspo->drylevel;
            pSourceVoice->builtinvol = pssa->vol;
            pSourceVoice->programmedVol = pVoice->programmedVol;
            pSourceVoice->pvoltable = NULL;
            pSourceVoice->numenvelopes = 1;
            pSourceVoice->releaseenvelope = 0;
            pSourceVoice->curenvelope = 0;
            pSourceVoice->fadePer100Hz = 0.0f;
            pSourceVoice->envvol = 0x7F0000;
            pSourceVoice->envduration = 0x7FFFFFFF;
            pSourceVoice->envpertick = 0;
            pSourceVoice->paenvelope = NULL;
            pSourceVoice->pvollfo = NULL;
            Snd::Util::FastVol(pSourceVoice);

            if (i == 5) {
                pSourceVoice->isLfe = 1;
            } else {
                pSourceVoice->isLfe = 0;
            }
            pSourceVoice->elevation = 0;
        }

        if (pssf->channels == 1) {
            pVoice->elevation = pspo->elevation;
        }

        for (i = 0; i < pssf->channels; i++) {
            CHANPUB *pSourceVoice = &sndgs.chan[pVoice->voices[i]];
            pSourceVoice->azimuth += pVoice->azimuthOffsets[i];
        }
        pVoice->detunepitch = 0;

        iSNDcalcpitch(voice);
        ret = SNDPLATFORM_packetplay(packetinstancehandle, voice, pspo->timemult, pspo->lowpasscutoff, pspo->highpasscutoff, pssf, pssa);
        if (ret >= 0) {
            break;
        }

        for (i = 0; i < voicesneeded; i++) {
            SNDVOICEI_free(sndgs.chan[voice].voices[i]);
        }

        ppchan->shandle = ret;
    }

    return ppchan->shandle;
}

int SNDPKTPLAY_submit(int packetinstancehandle, SNDPACKET *psp) {
    SNDPACKETCHAN *ppchan = sndpps.ppc[packetinstancehandle];
    SNDPACKETENTRY *pentry;
    int packethandle;
    int i;

    for (i = 0; i < ppchan->sampleformat.channels; i++) {
        if (ppchan->outstandingpackets[i] >= ppchan->maxpackets - 1) {
            return -13;
        }
    }

    pentry = &ppchan->pe[ppchan->submitpacket];
    pentry->numframes = psp->numframes;
    pentry->continuation = psp->continuation;
    pentry->handle = ppchan->lastpackethandle;

    for (i = 0; i < ppchan->sampleformat.channels; i++) {
        pentry->psamples[i] = psp->psamples[i];
        ppchan->outstandingpackets[i]++;
    }

    ppchan->outstandingframes += psp->numframes;
    packethandle = ppchan->lastpackethandle;
    ppchan->lastpackethandle++;
    ppchan->submitpacket++;

    if (ppchan->submitpacket >= ppchan->maxpackets) {
        ppchan->submitpacket = 0;
    }

    return packethandle;
}

int SNDPKTPLAY_submitspace(int packetinstancehandle) {
    SNDPACKETCHAN *ppchan = sndpps.ppc[packetinstancehandle];
    int greatestOutstandingPackets = 0;
    int i;

    for (i = 0; i < ppchan->sampleformat.channels; i++) {
        if (ppchan->outstandingpackets[i] >= greatestOutstandingPackets) {
            greatestOutstandingPackets = ppchan->outstandingpackets[i];
        }
    }

    return ppchan->maxpackets - greatestOutstandingPackets - 1;
}

int SNDPKTPLAY_framesoutstanding(int packetinstancehandle) {
    SNDPACKETCHAN *ppchan = sndpps.ppc[packetinstancehandle];

    return ppchan->outstandingframes + ppchan->outstandingplatformframes;
}

int SNDPKTPLAY_purge(int packetinstancehandle, int starthandle, int endhandle) {
    return -15;
}

int SNDPKTPLAY_stop(int packetinstancehandle) {
    SNDPACKETCHAN *ppchan = sndpps.ppc[packetinstancehandle];
    int i;

    SNDstop(ppchan->shandle);
    SNDPKTPLAYI_flushcallbackdata();
    SNDPKTPLAY_purge(packetinstancehandle, 0, 0x7FFFFFFF);
    ppchan->shandle = -1;

    for (i = 0; i < ppchan->sampleformat.channels; i++) {
        if (ppchan->ptsdata[i] != NULL) {
            SNDMEMI_free(ppchan->ptsdata[i]);
        }
    }

    return 0;
}

int SNDPKTPLAY_destroy(int packetinstancehandle) {
    SNDSYS_entercritical();

    SNDPLATFORM_packetplaydestroy(packetinstancehandle);
    sndpps.ppc[packetinstancehandle] = NULL;

    SNDSYS_leavecritical();

    return 0;
}

void *SNDPKTPLAYI_get(int packetinstancehandle, int chan, int *pframes, int *pcontinuation) {
    SNDPACKETCHAN *ppchan = sndpps.ppc[packetinstancehandle];
    SNDPACKETENTRY *pentry;
    SNDPACKETENTRY *pspe;
    int i;
    int releaseFlag = 1;
    int increaseMaster = 1;

    for (i = 0; i < ppchan->sampleformat.channels; i++) {
        if (ppchan->numPacketsProcessed[i] <= ppchan->masterReleasePacket) {
            releaseFlag = 0;
        }
    }

    if (releaseFlag != 0) {
        pspe = &ppchan->pe[ppchan->releasepacket];
        if (ppchan->preleasefunc != NULL) {
            SNDIPACKETCALLBACK *pspcb = &sndpps.spcb[sndpps.callbackindex];
            pspcb->type = 1;
            pspcb->chan = packetinstancehandle;
            pspcb->u.psamples = pspe->psamples[0];
            sndpps.callbackindex++;
        }

        ppchan->releasepacket++;
        ppchan->masterReleasePacket++;

        if (ppchan->releasepacket >= ppchan->maxpackets) {
            ppchan->releasepacket = 0;
        }
    }

    if (ppchan->outstandingpackets[chan] == 0) {
        return NULL;
    }

    pentry = &ppchan->pe[ppchan->playpacket[chan]];

    *pframes = pentry->numframes;
    *pcontinuation = pentry->continuation;
    ppchan->playpacket[chan]++;
    ppchan->numPacketsProcessed[chan]++;

    i = 0;
    do {
        if (ppchan->numPacketsProcessed[i] <= ppchan->masterCurrentPacket) {
            increaseMaster = 0;
        }
        i++;
    } while (increaseMaster == 1 && i < ppchan->sampleformat.channels);

    if (increaseMaster != 0) {
        ppchan->masterCurrentPacket++;
        ppchan->outstandingframes -= pentry->numframes;
        ppchan->outstandingplatformframes += pentry->numframes;
    }

    if (ppchan->playpacket[chan] >= ppchan->maxpackets) {
        ppchan->playpacket[chan] = 0;
    }

    ppchan->outstandingpackets[chan]--;
    if (pentry->psamples[chan] == NULL) {
        return (void *)-1;
    }

    return pentry->psamples[chan];
}

void SNDPKTPLAYI_freeframes(int packetinstancehandle, int chan, int frames) {
    SNDPACKETCHAN *ppchan = sndpps.ppc[packetinstancehandle];

    if (chan == ppchan->masterpacketchan) {
        ppchan->outstandingplatformframes -= frames;
        if (ppchan->pframesfunc != NULL) {
            SNDIPACKETCALLBACK *pspcb = &sndpps.spcb[sndpps.callbackindex];
            pspcb->type = 0;
            pspcb->chan = packetinstancehandle;
            pspcb->u.frames = frames;
            sndpps.callbackindex++;
        }
    }
}

void SNDPKTPLAYI_flushcallbackdata() {
    SNDPACKETCHAN *ppchan;
    SNDIPACKETCALLBACK *pspcb;
    int i;

    for (i = 0; i < sndpps.callbackindex; i++) {
        pspcb = &sndpps.spcb[i];
        ppchan = sndpps.ppc[pspcb->chan];
        if (pspcb->type == 0) {
            ppchan->pframesfunc(pspcb->chan, pspcb->u.frames, ppchan->pclientdata);
        } else {
            ppchan->preleasefunc(pspcb->u.psamples, ppchan->pclientdata);
        }
    }

    sndpps.callbackindex = 0;
}
