#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "snd/sndo.h"

int SNDBANK_play(int bhandle, int patnum, SNDPLAYOPTS *pspo) {
    BANKVER5 *pb;
    SNDI_checkplayopts(pspo);
    if (SNDBANKI_valid(bhandle) < 0) {
        return -8;
    }

    pb = sndgs.banklist[bhandle].phdr;
    if (patnum < 0 || patnum >= pb->numpatches) {
        return -8;
    }
    SNDBANKI_playpatch(pb, SNDBANKI_getppatch(pb, patnum), bhandle, patnum, pspo);
}
