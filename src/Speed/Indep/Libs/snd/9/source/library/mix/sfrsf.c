#include <snd/sfilter.h>

typedef void RSFFN(int, float *, float *, unsigned int *, unsigned int *, unsigned int, unsigned int);

extern "C" RSFFN rsflc;

typedef struct RSFSTATE {
    SFILTERNODE sfn;
    unsigned int incr16_16;
    unsigned int acc16_16;
    RSFFN *rsf;
    short firstdone;
    short centersample;
    float history[4];
} RSFSTATE;

int SFILTER_rsf(void *pstate, int outputframes, void *psrcp, void *pdstp, int requester);

void SFILTER_rsfsetpitch(RSFSTATE *prss, int incrementor) {
    prss->incr16_16 = incrementor;
}

int SFILTER_rsf(void *pstate, int outputframes, void *psrcp, void *pdstp, int requester) {
    RSFSTATE *prss;
    unsigned int incwhole;
    unsigned int incfrac;
    unsigned int accfrac;
    float *psrc;
    float *pdst;
    float *ptemp;
    int inputframes;
    int temp;
    int i;
    unsigned int accwhole;

    prss = (RSFSTATE *)pstate;
    psrc = (float *)psrcp;
    pdst = (float *)pdstp;
    temp = outputframes;

    if (prss->incr16_16 == 0x10000) {
        inputframes = outputframes;
        if (prss->firstdone == 0) {
            inputframes = outputframes + (4 - prss->centersample);
        } else {
            for (i = 0; i < 4 - prss->centersample; i++) {
                *pdst++ = prss->history[i + prss->centersample];
            }
        }
        if (prss->sfn.pfnnext != 0) {
            temp = prss->sfn.pfnnext->filterfn(prss->sfn.pfnnext, inputframes, psrcp, pdst,
                                              prss->sfn.requester);
        }
        if (temp <= 0) {
            if (prss->firstdone == 0) {
                prss->firstdone = 1;
            }
            prss->history[0] = 0.0f;
            prss->history[1] = 0.0f;
            prss->history[2] = 0.0f;
            prss->history[3] = 0.0f;
            return temp;
        }
        if (prss->firstdone == 0) {
            prss->firstdone = 1;
            ptemp = pdst + (outputframes - prss->centersample);
        } else {
            ptemp = pdst + (outputframes - 4);
        }
        prss->history[0] = ptemp[0];
        prss->history[1] = ptemp[1];
        prss->history[2] = ptemp[2];
        prss->history[3] = ptemp[3];
        return outputframes;
    } else {
        inputframes = (prss->incr16_16 * outputframes + prss->acc16_16) >> 16;
        if (prss->firstdone == 0) {
            inputframes = inputframes + (4 - prss->centersample);
            for (i = 0; i < prss->centersample; i++) {
                *psrc++ = 0.0f;
            }
        } else {
            psrc[0] = prss->history[0];
            psrc[1] = prss->history[1];
            psrc[2] = prss->history[2];
            psrc[3] = prss->history[3];
            psrc += 4;
        }
        if (inputframes > 0 && prss->sfn.pfnnext != 0) {
            temp = prss->sfn.pfnnext->filterfn(prss->sfn.pfnnext, inputframes, pdstp, psrc,
                                              prss->sfn.requester);
            if (temp <= 0) {
                if (prss->firstdone == 0) {
                    prss->firstdone = 1;
                }
                prss->history[0] = 0.0f;
                prss->history[1] = 0.0f;
                prss->history[2] = 0.0f;
                prss->history[3] = 0.0f;
                return temp;
            }
        }
        if (prss->firstdone != 0) {
            psrc -= 4 - prss->centersample;
        } else {
            prss->firstdone = 1;
        }
        incfrac = prss->incr16_16 << 16;
        incwhole = prss->incr16_16 >> 16;
        accfrac = prss->acc16_16 << 16;
        accwhole = 0;
        prss->rsf(outputframes, psrc, (float *)pdstp, &accwhole, &accfrac, incwhole, incfrac);
        prss->acc16_16 = (accwhole << 16) | (accfrac >> 16);
        ptemp = psrc + ((prss->acc16_16 >> 16) - prss->centersample);
        prss->history[0] = ptemp[0];
        prss->history[1] = ptemp[1];
        prss->history[2] = ptemp[2];
        prss->history[3] = ptemp[3];
        prss->acc16_16 &= 0xFFFF;
    }
    return outputframes;
}

void SFILTER_rsfinit(RSFSTATE *prss, int cpu, int method) {
    int i;

    prss->sfn.filterfn = SFILTER_rsf;
    prss->sfn.restorefn = 0;
    prss->acc16_16 = 0;
    prss->firstdone = 0;
    prss->centersample = 0;
    if (method == 0) {
        prss->rsf = rsflc;
    }
    for (i = 0; i <= 3; i++) {
        prss->history[i] = 0.0f;
    }
}
