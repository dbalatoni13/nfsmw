#include "./sndcmn.h"
#include <cstddef>
#include <cstring>

namespace Sndgendef {

// total size: 0x8
typedef struct GENTAGGEDPATCH {
    unsigned char id[4];        // offset 0x0, size 0x4
    unsigned char MajorVersion; // offset 0x4, size 0x1
    unsigned char MinorVersion; // offset 0x5, size 0x1
    unsigned char PatchVersion; // offset 0x6, size 0x1
    unsigned char pad[1];       // offset 0x7, size 0x1
} GENTAGGEDPATCH;

}; // namespace Sndgendef

// spatgen.c
void SNDI_patchtohdrgen(void *pbank, Sndgendef::GENTAGGEDPATCH *ptp, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDSAMPLEDESC *pssd);

void SNDI_patchtohdr(void *pbank /* r23 */, struct TAGGEDPATCH *ptp /* r30 */, struct SNDSAMPLEFORMAT *pssf /* r28 */, struct SNDSAMPLEATTR *pssa /* r31 */, struct SNDSAMPLEDESC *pssd /* r29 */, unsigned char *isgeneric /* r27 */) {
    SNDTAGINFO sti;
    int userdatachunks = 0;
    int playloc = 0x200;
    int *paudioramaddr = NULL;
    int i;
    unsigned char *tempID;

    memset(pssf, 0, sizeof(SNDSAMPLEFORMAT));
    memset(pssa, 0, sizeof(SNDSAMPLEATTR));
    memset(pssd, 0, sizeof(SNDSAMPLEDESC));

    tempID = (unsigned char *)ptp;

    if (((tempID[0] != 'P') || (tempID[1] != 'T')) &&
        ((tempID[0] == 'G') && (tempID[1] == 'S') && (tempID[2] == 'T') && (tempID[3] == 'R'))) {
        SNDI_patchtohdrgen(NULL, (Sndgendef::GENTAGGEDPATCH *)ptp, pssf, pssa, pssd);

        if (isgeneric) {
            *isgeneric = 1;
        }

        return;
    }

    if (isgeneric) {
        *isgeneric = 0;
    }

    SND_attrsetdef(pssa);

    sti.p = (unsigned char *)ptp + 4;
    pssf->samplerate = 24000;
    pssf->channels = 1;
    pssf->samplerep = 7;
    pssd->totalframes = 0;

    while (SNDI_gettag(&sti)) {
        if (sti.tag == 0xA0) {
            pssf->samplerep = sti.size;
        } else if (sti.tag == 0x9C) {
            pssa->azimuth[0] = sti.size;
        } else if (sti.tag == 0x9D) {
            pssa->azimuth[1] = sti.size;
        } else if (sti.tag == 0x9E) {
            pssa->azimuth[2] = sti.size;
        } else if (sti.tag == 0x9F) {
            pssa->azimuth[3] = sti.size;
        } else if (sti.tag == 0xA6) {
            pssa->azimuth[4] = sti.size;
        } else if (sti.tag == 0xA7) {
            pssa->azimuth[5] = sti.size;
        } else if (sti.tag == 0x98) {
            pssa->ptsdata[0] = SNDMEMI_allocz(sti.datasize);
            Snd::Util::MemCpy(pssa->ptsdata[0], sti.pdata, sti.datasize);
            pssa->tsdatasize[0] = sti.datasize;
        } else if (sti.tag == 0x99) {
            pssa->ptsdata[1] = SNDMEMI_allocz(sti.datasize);
            Snd::Util::MemCpy(pssa->ptsdata[1], sti.pdata, sti.datasize);
            pssa->tsdatasize[1] = sti.datasize;
        } else if (sti.tag == 0x9A) {
            pssa->ptsdata[2] = SNDMEMI_allocz(sti.datasize);
            Snd::Util::MemCpy(pssa->ptsdata[2], sti.pdata, sti.datasize);
            pssa->tsdatasize[2] = sti.datasize;
        } else if (sti.tag == 0x9B) {
            pssa->ptsdata[3] = SNDMEMI_allocz(sti.datasize);
            Snd::Util::MemCpy(pssa->ptsdata[3], sti.pdata, sti.datasize);
            pssa->tsdatasize[3] = sti.datasize;
        } else if (sti.tag == 0xA4) {
            pssa->ptsdata[4] = SNDMEMI_allocz(sti.datasize);
            Snd::Util::MemCpy(pssa->ptsdata[4], sti.pdata, sti.datasize);
            pssa->tsdatasize[4] = sti.datasize;
        } else if (sti.tag == 0xA5) {
            pssa->ptsdata[5] = SNDMEMI_allocz(sti.datasize);
            Snd::Util::MemCpy(pssa->ptsdata[5], sti.pdata, sti.datasize);
            pssa->tsdatasize[5] = sti.datasize;
        } else if (sti.tag == 0x80) {
            pssa->platformver = sti.size;
        } else if (sti.tag == 0x82) {
            pssf->channels = sti.size;
        } else if (sti.tag == 0x84) {
            pssf->samplerate = sti.size;
        } else if (sti.tag == 0x85) {
            pssd->totalframes = sti.size;
        } else if (sti.tag == 0x8A) {
            paudioramaddr = (int *)sti.pdata;
        } else if (sti.tag == 0x13) {
            pssa->fxlevel0 = sti.size;
        } else if (sti.tag == 0x0A) {
            pssa->bendrange = sti.size;
        } else if (sti.tag == 0x06) {
            pssa->priority = sti.size;
        } else if (sti.tag == 0x8C) {
            playloc = sti.size;
        } else if (sti.tag == 0x88) {
            pssd->psamples[0] = (void *)sti.size;
        } else if (sti.tag == 0x89) {
            pssd->psamples[1] = (void *)sti.size;
        } else if (sti.tag == 0x94) {
            pssd->psamples[2] = (void *)sti.size;
        } else if (sti.tag == 0x95) {
            pssd->psamples[3] = (void *)sti.size;
        } else if (sti.tag == 0xA2) {
            pssd->psamples[4] = (void *)sti.size;
        } else if (sti.tag == 0xA3) {
            pssd->psamples[5] = (void *)sti.size;
        } else if (sti.tag == 0x14) {
            pssa->puserdata[userdatachunks] = sti.pdata;
            pssa->userdatasize[userdatachunks] = sti.datasize;
            userdatachunks++;
        }
    }

    pssa->rendermode = playloc;

    for (i = pssf->channels - 1; i >= 0; i--) {
        if (Snd::gFoldDownTarget == Snd::FOLDDOWNTARGET_STEREO && pssf->channels == 2) {
            pssa->azimuth[i] = 0;
        }

        pssa->azimuth[i] = sndgs.srcchancfg3d[pssf->channels - 1][i] + pssa->azimuth[i];

        if ((pssa->rendermode & 4) || pssa->rendermode == 0) {
            pssd->psamples[i] = (void *)((sndptrint)pbank + (sndptrint)pssd->psamples[i]);
        }

        if ((pssa->rendermode & 0x308) && paudioramaddr) {
            if (i == 0) {
                pssd->psamples[i] = (void *)*paudioramaddr;
            } else {
                pssd->psamples[i] = (void *)(*paudioramaddr + ((sndptrint)pssd->psamples[i] - (sndptrint)pssd->psamples[0]));
            }
        }
    }
}
