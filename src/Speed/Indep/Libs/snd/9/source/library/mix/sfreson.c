#include <snd/sfilter.h>

float SNDI_cos(float x);
float SNDI_rootof1plusx(float x);

// total size: 0x40
typedef struct RESONSTATE {
    SFILTERNODE sfn;  // offset 0x0, size 0x1C
    int samplerate;   // offset 0x1C
    float resonfreq;  // offset 0x20
    float bw;         // offset 0x24
    float r;          // offset 0x28
    float costheta;   // offset 0x2C
    float a0;         // offset 0x30
    float xn;         // offset 0x34
    float ynminus1;   // offset 0x38
    float ynminus2;   // offset 0x3C
} RESONSTATE;

void resonx87(RESONSTATE *prs, int frames, void *psrcp, void *pdstp) {
    float *psrc;
    float *pdst;
    int i;

    psrc = (float *)psrcp;
    pdst = (float *)pdstp;
    if (prs->bw < 2.0f * prs->samplerate * (1.0f / 3.1415927f) && prs->resonfreq > 0.0f &&
        prs->resonfreq < (float)(prs->samplerate >> 1)) {
        for (i = 0; i < frames; i++) {
            pdst[i] = prs->a0 * (psrc[i] + 1e-20f) +
                      2.0f * prs->r * prs->costheta * prs->ynminus1 -
                      prs->r * prs->r * prs->ynminus2;
            prs->ynminus2 = prs->ynminus1;
            prs->ynminus1 = pdst[i];
        }
    } else {
        for (i = 0; i < frames; i++) {
            pdst[i] = psrc[i];
        }
    }
}

int SFILTER_reson(void *pstate, int frames, void *psrcp, void *pdstp, int requester) {
    RESONSTATE *prs = (RESONSTATE *)pstate;
    int temp;

    if (prs->sfn.pfnnext) {
        temp = prs->sfn.pfnnext->filterfn(prs->sfn.pfnnext, frames, pdstp, psrcp,
                                          prs->sfn.requester);
        if (temp <= 0) {
            return temp;
        }
    } else {
        temp = frames;
    }
    resonx87(prs, temp, psrcp, pdstp);
    return temp;
}

int SFILTER_createRESON(RESONSTATE *prs) {
    prs->sfn.pfnnext = 0; prs->sfn.pfnnext2 = 0; prs->sfn.pfnprev1 = 0; prs->sfn.pfnprev2 = 0;
    prs->sfn.clean = 0;
    prs->xn = 0.0f;
    prs->ynminus1 = 0.0f;
    prs->sfn.restorefn = 0;
    prs->sfn.filterfn = SFILTER_reson;
    prs->ynminus2 = 0.0f;
    return 0;
}

void SFILTER_modifyRESON(RESONSTATE *prs, int *params) {
    int centrefreq;
    int samplerate;
    int bandwidth;
    float gain;
    float rsquare;

    centrefreq = params[0] >> 8;
    samplerate = params[1] >> 8;
    bandwidth = params[2] >> 8;
    gain = (float)params[3] * (1.0f / 256.0f);
    prs->samplerate = samplerate;
    prs->resonfreq = centrefreq;
    prs->bw = bandwidth;
    prs->r = 1.0f - bandwidth * 3.1415927f / samplerate;
    rsquare = prs->r * prs->r;
    prs->costheta = 2.0f * prs->r / (rsquare + 1.0f) *
                    SNDI_cos(centrefreq * 6.2831855f / samplerate);
    prs->a0 = (1.0f - rsquare) * gain * SNDI_rootof1plusx(-prs->costheta * prs->costheta);
}
