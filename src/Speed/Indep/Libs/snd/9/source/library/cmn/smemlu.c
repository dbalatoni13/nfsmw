#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "snd/sndo.h"

int SNDmemlargestunused(int *paddr) {
    SNDSYS_entercritical();
    int ret = SNDPLATFORM_memlargestunused(paddr);
    SNDSYS_leavecritical();

    return ret;
}
