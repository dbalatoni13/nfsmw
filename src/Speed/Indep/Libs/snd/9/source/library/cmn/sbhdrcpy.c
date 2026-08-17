#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include <cstring>
int SNDbankheadercopy(void *pmem, int bhandle) {
    int memneeded;
    int rc = 0;

    memneeded = SNDbankheadersize(bhandle);
    if (memneeded < 0) {
        rc = memneeded;
        goto abort;
    }

    memmove(pmem, sndgs.banklist[bhandle].phdr, memneeded);
    sndgs.banklist[bhandle].phdr = reinterpret_cast<BANKVER5 *>(pmem);

abort:
    return rc;
}
