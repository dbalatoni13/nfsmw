#include "./sndcmn.h"

#ifdef __cplusplus
extern "C" {
#endif

void *memset(void *pdst, ...);

#ifdef __cplusplus
}
#endif

ENVELOPE snddefaultenvelope = { 0x7FFFFFFF, 127 };

int SNDI_parsetimbre(void **pptp, SNDIPATCHHEADER *pph) {
    int stc[43];
    void *pstc[43];
    int stp[46];
    void *pstp[46];
    int azoffsets[6];
    SNDTAGINFO sti;
    int moretags;
    int i;

    memset(azoffsets, 0, sizeof(azoffsets));

    sti.p = (unsigned char *)*pptp;
    pph->ptimbre = sti.p;

    for (i = 0; i < 43; i++) {
        pstc[i] = 0;
    }

    for (i = 0; i < 46; i++) {
        pstp[i] = 0;
    }

    for (i = 0; i < 6; i++) {
        pph->ptimestretchdata[i] = 0;
        pph->timestretchsize[i] = 0;
    }

    for (i = 0; i < 6; i++) {
        pph->pcodebookdata[i] = 0;
        pph->ploopstatedata[i] = 0;
    }

    stc[1] = 0;
    stc[2] = 127;
    stc[3] = 0;
    stc[4] = 127;
    stc[6] = 0;
    stc[7] = 60;
    stc[8] = -1;
    stc[9] = 1;
    stc[10] = 0;
    stc[12] = 64;
    stc[13] = 0;
    stc[14] = 127;
    stc[15] = 0;
    stc[16] = 0;
    stc[17] = 0;
    stc[18] = 0;
    stc[19] = 0;
    pph->numuserdata = 0;
    stc[23] = 0;
    stc[25] = (sndptrint)&snddefaultenvelope;
    stc[26] = 0;
    stc[38] = 0;
    stc[39] = 0;
    stc[40] = 0;
    stc[41] = 0;
    stc[42] = 0;
    stc[28] = 127;
    stc[29] = 0;
    stc[30] = 0;
    stc[31] = 0;
    stc[32] = 0;
    stc[33] = 0;
    stc[34] = 0;
    stc[35] = 0;
    stc[36] = 0;
    stc[37] = 1;

    stp[0] = 2;
    stp[2] = 1;
    stp[4] = 24000;
    stp[5] = 0;
    stp[6] = -1;
    stp[7] = -1;
    stp[8] = 0;
    stp[28] = 0;
    stp[9] = 0;
    stp[29] = 0;
    stp[20] = 0;
    stp[30] = 0;
    stp[21] = 0;
    stp[31] = 0;
    stp[34] = 0;
    stp[38] = 0;
    stp[35] = 0;
    stp[39] = 0;
    stp[32] = 7;
    stp[12] = 0x200;

    moretags = 0;

    while (SNDI_gettag(&sti) != 0) {
        if (sti.tag <= 42) {
            stc[sti.tag] = sti.size;
            pstc[sti.tag] = sti.pdata;

            if (sti.tag == 20) {
                pph->puserdata[pph->numuserdata] = sti.pdata;
                pph->userdatasize[pph->numuserdata] = sti.datasize;
                pph->numuserdata++;
            }
        } else if (sti.tag >= 128 && sti.tag <= 173) {
            stp[sti.tag - 128] = sti.size;
            pstp[sti.tag - 128] = sti.pdata;

            if (sti.tag == 0x98) {
                pph->ptimestretchdata[0] = sti.pdata;
                pph->timestretchsize[0] = sti.datasize;
            } else if (sti.tag == 0x99) {
                pph->ptimestretchdata[1] = sti.pdata;
                pph->timestretchsize[1] = sti.datasize;
            } else if (sti.tag == 0x9A) {
                pph->ptimestretchdata[2] = sti.pdata;
                pph->timestretchsize[2] = sti.datasize;
            } else if (sti.tag == 0x9B) {
                pph->ptimestretchdata[3] = sti.pdata;
                pph->timestretchsize[3] = sti.datasize;
            } else if (sti.tag == 0xA4) {
                pph->ptimestretchdata[4] = sti.pdata;
                pph->timestretchsize[4] = sti.datasize;
            } else if (sti.tag == 0xA5) {
                pph->ptimestretchdata[5] = sti.pdata;
                pph->timestretchsize[5] = sti.datasize;
            }
        } else if (sti.tag == 254) {
            moretags = 1;
            break;
        }
    }

    pph->velmin = stc[1];
    pph->velmax = stc[2];
    pph->keymin = stc[3];
    pph->keymax = stc[4];
    pph->priority = stc[6];
    pph->basekey = stc[7];
    pph->releaseenvelope = stc[8];
    pph->numenvelopes = stc[9];
    pph->bendrange = stc[10];
    pph->pan = stc[12];
    pph->randpan = stc[13];
    pph->vol = stc[14];
    pph->randvol = stc[15];
    pph->detune = stc[16];
    pph->randdetune = stc[17];
    pph->pvoltable = (char *)pstc[18] + stc[18];
    pph->fxlevel0 = stc[19];
    pph->pbendtable = (char *)pstc[23] + stc[23];
    pph->penvelope = (char *)pstc[25] + stc[25];
    pph->loopoffset[0] = stc[26];
    pph->loopoffset[1] = stc[38];
    pph->loopoffset[2] = stc[39];
    pph->loopoffset[3] = stc[40];
    pph->loopoffset[4] = stc[41];
    pph->loopoffset[5] = stc[42];
    pph->initialenvvol = stc[28];
    pph->plfovol = (char *)pstc[29] + stc[29];
    pph->lfovollength = stc[30];
    pph->lfovolrand = stc[31];
    pph->plfopitch = (char *)pstc[32] + stc[32];
    pph->lfopitchlength = stc[33];
    pph->lfopitchdepth = stc[34];
    pph->lfopitchrand = stc[35];
    pph->masterranddetune = stc[36];
    pph->panmult = stc[37];

    pph->platformver = stp[0];
    pph->numchan = stp[2];
    pph->samplerep = stp[32];
    pph->samplerate = stp[4];
    pph->totalframes = stp[5];
    pph->sustainstart = stp[6];
    pph->sustainend = stp[7];
    pph->rendermode = stp[12];
    pph->paudioramaddr = (int *)pstp[10];
    pph->sampleoffset[0] = stp[8];
    azoffsets[0] = stp[28];
    pph->sampleoffset[1] = stp[9];
    azoffsets[1] = stp[29];
    pph->sampleoffset[2] = stp[20];
    azoffsets[2] = stp[30];
    pph->sampleoffset[3] = stp[21];
    azoffsets[3] = stp[31];
    pph->sampleoffset[4] = stp[34];
    azoffsets[4] = stp[38];
    pph->sampleoffset[5] = stp[35];
    azoffsets[5] = stp[39];

    pph->pcodebookdata[0] = pstp[15];
    pph->pcodebookdata[1] = pstp[16];
    pph->ploopstatedata[0] = pstp[13];
    pph->ploopstatedata[1] = pstp[14];

    if (pph->numchan > 2) {
        pph->pcodebookdata[2] = pstp[17];
        pph->ploopstatedata[2] = pstp[11];
    }

    if (pph->numchan > 3) {
        pph->pcodebookdata[3] = pstp[43];
        pph->ploopstatedata[3] = pstp[40];
    }

    if (pph->numchan > 4) {
        pph->pcodebookdata[4] = pstp[44];
        pph->ploopstatedata[4] = pstp[41];
    }

    if (pph->numchan > 5) {
        pph->pcodebookdata[5] = pstp[45];
        pph->ploopstatedata[5] = pstp[42];
    }

    for (i = 0; i < pph->numchan; i++) {
        if (Snd::gFoldDownTarget == Snd::FOLDDOWNTARGET_STEREO && pph->numchan == 2) {
            azoffsets[i] = 0;
        }

        pph->azimuth[i] = sndgs.srcchancfg3d[pph->numchan - 1][i] + azoffsets[i];
    }

    *pptp = sti.p;

    return moretags;
}
