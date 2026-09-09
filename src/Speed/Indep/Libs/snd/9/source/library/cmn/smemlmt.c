#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"

int SNDmemlimits(int startaddr, int endaddr) {
    SNDSYS_entercritical();
    int ret = SNDPLATFORM_memlimits(startaddr, endaddr);
    SNDSYS_leavecritical();

    return ret;
}
