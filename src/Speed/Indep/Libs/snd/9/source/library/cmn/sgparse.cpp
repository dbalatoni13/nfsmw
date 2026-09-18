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

// Nombres, tipos y orden de declaracion de las locales tomados del volcado
// DWARF del original: stc/pstc son las tablas de los tags "comunes" 0x00..0x2A
// y stp/pstp las de los genericos 0x80..0xAD. `moretags` no llega a tener
// registro -su asignacion dentro del bucle es muerta- pero sin ella el
// optimizador de saltos invierte el par bne/b del final del bucle.
void SNDI_patchtohdrgen(void *pbank, Sndgendef::GENTAGGEDPATCH *ptp, SNDSAMPLEFORMAT *pssf, SNDSAMPLEATTR *pssa, SNDSAMPLEDESC *pssd) {
    int stc[43];
    void *pstc[43];
    int stp[46];
    void *pstp[46];
    int userdatachunks = 0;
    SNDTAGINFO sti;
    int moretags;
    int i;

    memset(pssf, 0, sizeof(SNDSAMPLEFORMAT));
    memset(pssa, 0, sizeof(SNDSAMPLEATTR));
    memset(pssd, 0, sizeof(SNDSAMPLEDESC));

    SND_attrsetdef(pssa);

    sti.p = (unsigned char *)ptp + 8;

    for (i = 0; i < 43; i++) {
        pstc[i] = NULL;
    }

    for (i = 0; i < 46; i++) {
        pstp[i] = NULL;
    }

    stp[0x82 - 0x80] = 1;
    stp[0x84 - 0x80] = 48000;
    stp[0x85 - 0x80] = 0;
    stp[0xA0 - 0x80] = 10;

    while ((moretags = SNDI_gettag(&sti)) != 0) {
        if (sti.tag <= 0x2A) {
            stc[sti.tag] = sti.size;
            pstc[sti.tag] = sti.pdata;

            if (sti.tag == 0x14) {
                pssa->puserdata[userdatachunks] = sti.pdata;
                pssa->userdatasize[userdatachunks] = sti.datasize;
                userdatachunks++;
            }
        } else if (sti.tag < 0x80 || sti.tag > 0xAD) {
            if (sti.tag == 0xFE) {
                moretags = 0;
                break;
            }
        } else {
            stp[sti.tag - 0x80] = sti.size;
            pstp[sti.tag - 0x80] = sti.pdata;
        }
    }

    pssf->channels = stp[0x82 - 0x80];
    pssf->samplerep = stp[0xA0 - 0x80];
    pssf->samplerate = stp[0x84 - 0x80];
    pssd->totalframes = stp[0x85 - 0x80];
    pssa->rendermode = 4;
    pssa->platformver = 3;

    for (i = 0; i < pssf->channels; i++) {
        pssa->azimuth[i] = sndgs.srcchancfg3d[pssf->channels - 1][i];
    }
}
