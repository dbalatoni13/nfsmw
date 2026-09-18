#include "../cmn/sndcmn.h"
#include <snd/sfilter.h>

// total size: 0x1834
typedef struct TIMESTRETCHSTATE {
    SFILTERNODE sfn;              // offset 0x0, size 0x1C
    unsigned char *psideband;     // offset 0x1C
    int packetinstancehandle;     // offset 0x20
    float stretchratio;           // offset 0x24
    float sampleerror;            // offset 0x28
    int windowsize;               // offset 0x2C
    int framesleft;               // offset 0x30
    int inputsamples;             // offset 0x34
    int outputsamples;            // offset 0x38
    int outputstart;              // offset 0x3C
    float input[510];             // offset 0x40
    float output[1020];           // offset 0x838
    float *plocalsrc;             // offset 0x1828
    int srcBufSize;               // offset 0x182C
    int staticPingPongBufSize;    // offset 0x1830
} TIMESTRETCHSTATE;

extern "C" {
void *memset(void *dst, int c, unsigned int bytes);
float fabsf(float x);
}

int SNDDRV_getmastervoice(int voice);

static void crossfade(float *src1, float *src2, float *dest, int windowsize, int delta) {
    float fade;
    float faderate;
    int delay;
    int i;

    if (delta > 0) {
        delay = delta;
        fade = 1.0f;
        faderate = -1.0f / windowsize;
    } else {
        delay = -delta;
        fade = 0.0f;
        faderate = 1.0f / windowsize;
    }
    for (i = delay; i < windowsize; i++) {
        dest[i - delay] = fade * src1[i] + (1.0f - fade) * src1[i - delay];
        fade += faderate;
    }
    for (i = 0; i < delay; i++) {
        dest[windowsize - delay + i] = fade * src2[i] + (1.0f - fade) * src1[windowsize - delay + i];
        fade += faderate;
    }
}

static int determineaction(int windowsize, float stretchratio, unsigned char **ppsideband, float *sampleerror) {
    float error1;
    float error2;
    int delay;

    delay = **ppsideband + windowsize / 2;
    if (stretchratio < 1.0f) {
        error1 = windowsize + *sampleerror - stretchratio * windowsize;
        error2 = windowsize * 2 - delay + *sampleerror - stretchratio * 2 * windowsize;
        if (fabsf(error1) < fabsf(error2)) {
            *ppsideband += 1;
            *sampleerror = error1;
            return 0;
        }
        *ppsideband += 2;
        *sampleerror = error2;
        return -delay;
    } else {
        error1 = windowsize + *sampleerror - stretchratio * windowsize;
        error2 = windowsize + delay + *sampleerror - stretchratio * windowsize;
        if (fabsf(error1) < fabsf(error2)) {
            *ppsideband += 1;
            *sampleerror = error1;
            return 0;
        }
        *ppsideband += 1;
        *sampleerror = error2;
        return delay;
    }
}

static void applyaction(TIMESTRETCHSTATE *ts, float **ppsampledata, int delta) {
    float *src1;
    float *src2;

    if (ts->inputsamples > 0) {
        src1 = ts->input;
        src2 = *ppsampledata;
    } else {
        src1 = *ppsampledata;
        src2 = src1 + ts->windowsize;
    }
    if (delta == 0) {
        Snd::Util::MemCpy(ts->output, src1, ts->windowsize * sizeof(float));
        ts->framesleft -= ts->windowsize;
        ts->outputsamples = ts->windowsize;
        *ppsampledata = src2;
    } else if (delta > 0) {
        Snd::Util::MemCpy(ts->output, src1, delta * sizeof(float));
        crossfade(src1, src2, &ts->output[delta], ts->windowsize, delta);
        ts->framesleft -= ts->windowsize;
        ts->outputsamples = ts->windowsize + delta;
        *ppsampledata = src2;
    } else {
        crossfade(src1, src2, ts->output, ts->windowsize, delta);
        Snd::Util::MemCpy(&ts->output[ts->windowsize], src2 - delta, (ts->windowsize + delta) * sizeof(float));
        ts->framesleft -= ts->windowsize * 2;
        ts->outputsamples = ts->windowsize * 2 + delta;
        *ppsampledata = src2 + ts->windowsize;
    }
    ts->inputsamples = 0;
    ts->outputstart = 0;
}

static int transferframes(TIMESTRETCHSTATE *ts, int *poutputframes, float **ppoutputdata) {
    float *src;
    float *dest;
    int count;

    src = &ts->output[ts->outputstart];
    dest = *ppoutputdata;
    if (*poutputframes <= ts->outputsamples) {
        count = *poutputframes;
    } else {
        count = ts->outputsamples;
    }
    Snd::Util::MemCpy(dest, src, count * sizeof(float));
    ts->outputstart += count;
    ts->outputsamples -= count;
    *poutputframes -= count;
    *ppoutputdata += count;
    return count;
}

int stretchframesneeded(void *ptsstate, int outputframes) {
    TIMESTRETCHSTATE *ts = (TIMESTRETCHSTATE *)ptsstate;
    unsigned char *psideband;
    float sampleerror;
    int framesneeded;
    int framesleft;
    int framesconsumed;
    int framesgenerated;
    int delta;

    if (outputframes > ts->outputsamples) {
        framesneeded = 0;
        delta = 0;
        psideband = ts->psideband;
        sampleerror = ts->sampleerror;
        outputframes -= ts->outputsamples;
        framesleft = ts->framesleft;
        while (outputframes > 0) {
            if (framesleft < ts->windowsize) {
                break;
            }
            if (framesleft < ts->windowsize * 2) {
                delta = 0;
            } else {
                delta = determineaction(ts->windowsize, ts->stretchratio, &psideband, &sampleerror);
            }
            if (delta >= 0) {
                framesconsumed = ts->windowsize;
                framesgenerated = ts->windowsize + delta;
            } else {
                framesconsumed = ts->windowsize * 2;
                framesgenerated = ts->windowsize * 2 + delta;
            }
            framesneeded += framesconsumed;
            framesleft -= framesconsumed;
            if (outputframes <= framesgenerated) {
                outputframes = 0;
            } else {
                outputframes -= framesgenerated;
            }
        }
        if (delta > 0) {
            framesneeded += ts->windowsize;
        }
        if (outputframes > 0) {
            if (outputframes > framesleft) {
                framesneeded += framesleft;
            } else {
                framesneeded += outputframes;
            }
        }
        if (framesneeded > ts->inputsamples) {
            return framesneeded - ts->inputsamples;
        }
    }
    return 0;
}

int stretch(void *ptsstate, int outputframes, float *psampledata, float *poutputdata) {
    TIMESTRETCHSTATE *ts = (TIMESTRETCHSTATE *)ptsstate;
    int framesoutput;
    int delta;

    framesoutput = transferframes(ts, &outputframes, &poutputdata);
    if (outputframes == 0) {
        return framesoutput;
    }
    delta = 0;
    while (outputframes > 0) {
        if (ts->framesleft < ts->windowsize) {
            break;
        }
        if (ts->framesleft < ts->windowsize * 2) {
            delta = 0;
        } else {
            delta = determineaction(ts->windowsize, ts->stretchratio, &ts->psideband, &ts->sampleerror);
        }
        applyaction(ts, &psampledata, delta);
        framesoutput += transferframes(ts, &outputframes, &poutputdata);
    }
    if (delta > 0) {
        Snd::Util::MemCpy(ts->input, psampledata, ts->windowsize * sizeof(float));
        ts->inputsamples = ts->windowsize;
        psampledata += ts->windowsize;
    }
    if (outputframes > 0) {
        int count;

        if (outputframes > ts->framesleft) {
            count = ts->framesleft;
        } else {
            count = outputframes;
        }
        framesoutput += count;
        Snd::Util::MemCpy(poutputdata, psampledata, count * sizeof(float));
        ts->framesleft -= count;
    }
    return framesoutput;
}

void SFILTER_timestretchsetratio(TIMESTRETCHSTATE *ptss, int timemult) {
    ptss->stretchratio = timemult / 4096.0f;
}

int SFILTER_timestretch(void *pstate, int outputframes, void *psrcpassed, void *pdstpassed, int) {
    TIMESTRETCHSTATE *ptss = (TIMESTRETCHSTATE *)pstate;
    int inputframes;
    float *psrc = (float *)psrcpassed;
    float *pdst = (float *)pdstpassed;
    int retVal;

    if (ptss->framesleft == 0) {
        return ptss->sfn.pfnnext->filterfn(ptss->sfn.pfnnext, 200, pdst, psrc, ptss->sfn.requester);
    }
    inputframes = stretchframesneeded(ptss, outputframes);
    if (ptss->packetinstancehandle >= 0 &&
        inputframes > (int)SNDPKTPLAY_framesoutstanding(ptss->packetinstancehandle)) {
        return 0;
    }
    if (inputframes > 0) {
        int totalNeededBufSize;

        if (inputframes * (int)sizeof(float) > ptss->staticPingPongBufSize) {
            totalNeededBufSize = inputframes & ~63;
            totalNeededBufSize += 64;
            if (ptss->srcBufSize < totalNeededBufSize) {
                Snd::Util::ReallocBuf((void **)&ptss->plocalsrc, &ptss->srcBufSize, totalNeededBufSize, 4);
            }
            psrc = ptss->plocalsrc + 2;
        }
        retVal = ptss->sfn.pfnnext->filterfn(ptss->sfn.pfnnext, inputframes, pdst, psrc, ptss->sfn.requester);
        if (retVal <= 0) {
            return retVal;
        }
    }
    retVal = stretch(ptss, outputframes, psrc, pdst);
    if (retVal < outputframes) {
        float *ptemp = pdst + retVal;

        memset(ptemp, 0, (outputframes - retVal) * sizeof(float));
    }
    return outputframes;
}

void SFILTER_timestretchrestore(void *pstate) {
    TIMESTRETCHSTATE *ptss = (TIMESTRETCHSTATE *)pstate;

    if (ptss->plocalsrc) {
        SNDMEMI_free(ptss->plocalsrc);
        ptss->plocalsrc = 0;
        ptss->srcBufSize = 0;
    }
}

int SFILTER_timestretchinit(TIMESTRETCHSTATE *ptss, unsigned char *psideband, int voice) {
    ptss->sfn.filterfn = SFILTER_timestretch;
    ptss->sfn.restorefn = SFILTER_timestretchrestore;
    if (voice >= 0) {
        ptss->packetinstancehandle = SNDPKTPLAYI_voicetopackethandle(SNDDRV_getmastervoice(voice));
    } else {
        ptss->packetinstancehandle = -1;
    }
    ptss->psideband = psideband + 6;
    ptss->stretchratio = 1.0f;
    ptss->sampleerror = 0.0f;
    ptss->windowsize = psideband[1] * 2;
    ptss->framesleft = SNDI_getb(psideband + 2, 4);
    ptss->inputsamples = 0;
    ptss->outputsamples = 0;
    ptss->outputstart = 0;
    ptss->plocalsrc = 0;
    ptss->srcBufSize = 0;
    ptss->staticPingPongBufSize = 0xABC;
    return 0;
}
