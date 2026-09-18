#include "./sndcmn.h"

namespace Snd {

// sdownmix.c
extern const unsigned char gAzimuthFoldDownLut[FOLDDOWNTARGET_MAX][6][6];

} // namespace Snd

// srrange.c
int randrange(int range);

// spantoaz.c
int SNDI_pantoazimuth(int pan);

// srender.c
int SNDI_validrendermode(int *pindex, SNDIPATCHHEADER *pph);

// salloc.c
int SNDVOICEI_alloc(int voicesneeded, int priority, int *phandle, int minvoicerange, int maxvoicerange);
void SNDVOICEI_free(int voice);

// snddrv.c
void SNDPLATFORM_getvoicerange(int playloc, int *minvoicerange, int *maxvoicerange);
int SNDPLATFORM_playtimbre(SNDIPATCHHEADER *pph, void *psampledata, int voice, int timemult, int lowpasscutoff, int highpasscutoff);

int SNDBANKI_playtimbre(int bhandle, int patnum, void *psampledata, SNDPLAYOPTS *pspo, SNDIPATCHHEADER *pph, int keynum, int velocity,
                        int masterranddetune);

#ifdef __cplusplus
extern "C" {
#endif

// sover.c
int SNDover(int shandle);

#ifdef __cplusplus
}
#endif

int SNDBANKI_findfreekey() {
    int i;
    CHANPUB *pchan;
    static unsigned char freekey = 0;

retry:
    freekey++;
    if (freekey == 0) {
        freekey++;
    }

    for (i = 0; i < sndgs.voicestotal; i++) {
        pchan = &sndgs.chan[i];
        if (pchan->status != 0 && pchan->patchkey == freekey) {
            goto retry;
        }
    }

    return freekey;
}

int SNDBANKI_playtimbre(int bhandle, int patnum, void *psampledata, SNDPLAYOPTS *pspo, SNDIPATCHHEADER *pph, int keynum, int velocity,
                        int masterranddetune) {
    CHANPUB *pv;
    int rendermodeindex;
    int rendermode;
    int minvoicerange;
    int maxvoicerange;
    int voice;
    int bend;
    int builtinvol;
    int fxlevel0;
    int pan;
    int handle;
    int ret;
    int i;
    int done;

    handle = -9;
    bend = pspo->bend;
    fxlevel0 = pspo->fxlevel0;

    done = 0;
    rendermodeindex = 0;

    while (!done && (rendermode = SNDI_validrendermode(&rendermodeindex, pph)) != 0) {
        float programmedVol;
        float shortFxLevel;

        SNDPLATFORM_getvoicerange(rendermode, &minvoicerange, &maxvoicerange);
        voice = SNDVOICEI_alloc(pph->numchan, pph->priority, &handle, minvoicerange, maxvoicerange);
        if (voice < 0) {
            continue;
        }

        pv = &sndgs.chan[voice];
        pan = pph->pan;
        if (pph->randpan != 0) {
            pan += randrange(pph->randpan);
            if (pan < 0) {
                pan = 0;
            } else if (pan > 127) {
                pan = 127;
            }
        }
        pv->builtinazimuth = SNDI_pantoazimuth(pan);

        pv->initialdetune = pph->detune;
        pv->initialdetune -= (pph->basekey - keynum) * 100;
        if (masterranddetune != 0) {
            pv->initialdetune += masterranddetune;
        }
        if (pph->randdetune != 0) {
            pv->initialdetune += randrange(pph->randdetune);
        }

        pv->pbendtable = (SCALINGTABLE *)pph->pbendtable;

        builtinvol = pph->vol;
        if (pph->randvol != 0) {
            builtinvol += randrange(pph->randvol);
        }
        if (builtinvol > 127) {
            builtinvol = 127;
        }
        if (builtinvol < 0) {
            builtinvol = 0;
        }

        pv->bendrange = pph->bendrange * 100;
        pv->pitchbend = bend;
        builtinvol = builtinvol * velocity / 127;
        pv->paenvelope = (ENVELOPE *)pph->penvelope;
        pv->envvol = pph->initialenvvol << 16;
        pv->envduration = pv->paenvelope->duration;
        if (pv->envduration < 0) {
            pv->envduration = 0x7FFFFFFF;
        }
        pv->envpertick = ((pv->paenvelope->targetvol << 16) - pv->envvol) / pv->envduration;

        pv->ppitchlfo = (SCALINGTABLE *)pph->plfopitch;
        pv->pitchlfolength = pph->lfopitchlength;
        pv->pitchlfodepth = pph->lfopitchdepth;
        if (pph->lfovolrand != 0) {
            pv->curvollfoentry = iSNDrandom() % pph->lfovolrand;
        } else {
            pv->curvollfoentry = 0;
        }
        if (pph->lfopitchrand != 0) {
            pv->curpitchlfoentry = iSNDrandom() % pph->lfopitchlength;
        } else {
            pv->curpitchlfoentry = 0;
        }

        pv->timemult = pspo->timemult;
        pv->progpitch = pspo->pitchmult;
        pv->detunepitch = 0;
        iSNDcalcpitch(voice);

        pv->bhandle = bhandle;
        pv->patnum = patnum;
        pv->sustainend = pph->sustainend;
        pv->frames = pph->totalframes;
        pv->samplerate = pph->samplerate;
        pv->samplerep = pph->samplerep;
        pv->numchan = pph->numchan;

        programmedVol = pspo->vol / 127.0f;

        for (i = 0; i < pph->numchan; i++) {
            CHANPUB *pSourceVoice;
            int sourceVoice;

            pv->azimuthOffsets[i] = pph->azimuth[i] - sndgs.srcchancfg3d[pph->numchan - 1][i];
            sourceVoice = pv->voices[i];
            pSourceVoice = &sndgs.chan[sourceVoice];
            pSourceVoice->rendermode = rendermode;
            pSourceVoice->builtinvol = builtinvol;
            pSourceVoice->drylevel = pspo->drylevel;
            pSourceVoice->programmedVol = programmedVol;
            pSourceVoice->pvoltable = (SCALINGTABLE *)pph->pvoltable;
            pSourceVoice->fadePer100Hz = 0.0f;
            pSourceVoice->numenvelopes = pph->numenvelopes;
            pSourceVoice->releaseenvelope = pph->releaseenvelope;
            pSourceVoice->curenvelope = 0;
            pSourceVoice->envvol = pv->envvol;
            pSourceVoice->envduration = pv->envduration;
            pSourceVoice->envpertick = pv->envpertick;
            pSourceVoice->paenvelope = pv->paenvelope;
            pSourceVoice->vollfolength = pph->lfovollength;
            pSourceVoice->curvollfoentry = pv->curvollfoentry;
            pSourceVoice->pvollfo = (SCALINGTABLE *)pph->plfovol;
            iSNDcalcvol(sourceVoice);
            if (i == Snd::CHANNEL_LFE) {
                pSourceVoice->isLfe = 1;
            } else {
                pSourceVoice->isLfe = 0;
            }
        }

        if (pph->numchan == 1) {
            pv->azimuth = pv->builtinazimuth + pspo->azimuth;
            pv->azimuth += pv->azimuthOffsets[0];
            pv->elevation = pspo->elevation;
        } else {
            const unsigned char *pFoldedAzimuths = Snd::gAzimuthFoldDownLut[Snd::gFoldDownTarget][pph->numchan - 1];

            for (i = 0; i < pph->numchan; i++) {
                int sourceVoice;
                CHANPUB *pSourceVoice;

                sourceVoice = pv->voices[i];
                pSourceVoice = &sndgs.chan[sourceVoice];
                pSourceVoice->azimuth = pFoldedAzimuths[i] << 8;
                pSourceVoice->azimuth += pv->azimuthOffsets[i];
                pSourceVoice->elevation = 0;
            }
        }

        shortFxLevel = fxlevel0 / 127.0f;

        for (i = 0; i < pph->numchan; i++) {
            int sourceVoice;
            CHANPUB *pSourceVoice;

            sourceVoice = pv->voices[i];
            pSourceVoice = &sndgs.chan[sourceVoice];
            pSourceVoice->progfxlevel = fxlevel0;

            {
                int j;

                for (j = 0; j < Snd::gMaxFxBuses; j++) {
                    pSourceVoice->pFxVolume[j].fxLevel = shortFxLevel;
                }
            }
        }

        ret = SNDPLATFORM_playtimbre(pph, psampledata, voice, pspo->timemult, pspo->lowpasscutoff, pspo->highpasscutoff);
        if (ret >= 0) {
            break;
        }

        for (i = 0; i < pph->numchan; i++) {
            SNDVOICEI_free(sndgs.chan[voice].voices[i]);
        }
        handle = -9;
    }

    return handle;
}

int SNDBANKI_playpatch(void *psampledata, TAGGEDPATCH *ptp, int bhandle, int patnum, SNDPLAYOPTS *pspo) {
    CHANPUB *pchan = 0;
    SNDIPATCHHEADER ph;
    int shandle[12];
    int numtimbres = 0;
    int firsttimbreparsed = 0;
    int masterranddetune = 0;
    int velocity;
    int keynum;
    int patchkey;
    int moretags;
    int voice;
    int temp;
    int i;
    int rc = -9;

    if (ptp == 0) {
        return -8;
    }

    if (psampledata == 0) {
        psampledata = ptp;
    }

    keynum = pspo->keynum;
    velocity = pspo->velocity;
    ptp = (TAGGEDPATCH *)&ptp->hdrsize;

    patchkey = SNDBANKI_findfreekey();

    do {
        moretags = SNDI_parsetimbre((void **)&ptp, &ph);

        if (firsttimbreparsed == 0) {
            if (ph.masterranddetune != 0) {
                masterranddetune = randrange(ph.masterranddetune);
            }
            firsttimbreparsed = 1;
        }

        if (velocity >= ph.velmin && velocity <= ph.velmax && keynum >= ph.keymin && keynum <= ph.keymax) {
            rc = SNDBANKI_playtimbre(bhandle, patnum, psampledata, pspo, &ph, keynum, velocity, masterranddetune);
            if (rc < 0) {
                goto abort;
            }
            shandle[numtimbres++] = rc;
            SNDBANKI_userdatacallback(&ph, rc, 1);
        }
    } while (moretags != 0);

    temp = numtimbres;
    numtimbres = 0;
    for (i = 0; i < temp; i++) {
        if (SNDover(shandle[i]) == 0) {
            shandle[numtimbres++] = shandle[i];
        }
    }

    if (numtimbres == 0) {
        goto abort;
    }

    if (numtimbres == 1) {
        voice = SNDVOICEI_get(shandle[0]);
        if (voice >= 0) {
            pchan = &sndgs.chan[voice];
            pchan->patchkey = 0;
            pchan->ismaster = 0;
        }
    } else {
        for (i = 0; i < numtimbres; i++) {
            voice = SNDVOICEI_get(shandle[i]);
            if (voice >= 0) {
                pchan = &sndgs.chan[voice];
                pchan->patchkey = patchkey;
                pchan->ismaster = 0;
            }
        }
        pchan->ismaster = 1;
    }

    return rc;

abort:
    for (i = 0; i < numtimbres; i++) {
        SNDstop(shandle[i]);
    }

    return rc;
}
