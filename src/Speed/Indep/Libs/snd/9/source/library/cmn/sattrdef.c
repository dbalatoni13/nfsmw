#include "snd/sndo.h"
#include <cstddef>

int SND_attrsetdef(SNDSAMPLEATTR *pssa) {
    pssa->priority = 0;
    pssa->detune = 0;
    pssa->fxlevel0 = 0;
    pssa->vol = 0x7F;
    pssa->pan = 0x40;
    pssa->bendrange = 0;
    pssa->platformver = 2;
    pssa->rendermode = 0x200;

    int i;
    for (i = 0; i < 4; i++) {
        pssa->puserdata[i] = NULL;
        pssa->userdatasize[i] = 0;
    }

    for (i = 0; i < 6; i++) {
        pssa->azimuth[i] = 0;
        pssa->ptsdata[i] = NULL;
    }

    return 0;
}
