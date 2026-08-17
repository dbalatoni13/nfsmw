
#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
int SNDbankheadersize(int bhandle) {
    return sndgs.banklist[bhandle].phdr->hdrsize;
}
