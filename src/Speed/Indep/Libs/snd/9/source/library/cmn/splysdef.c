#include "./sndcmn.h"

int SNDplaysetdef(SNDPLAYOPTS *pspo) {
    pspo->keynum = 60;
    pspo->pitchmult = 4096;
    pspo->timemult = 4096;
    pspo->highpasscutoff = 0;
    pspo->velocity = 127;
    pspo->vol = 127;
    pspo->tempomult = 4096;
    pspo->lowpasscutoff = 0xFFFF;
    pspo->fxlevel0 = 0;
    pspo->drylevel = 127;
    pspo->bend = 64;
    pspo->azimuth = 0;
    pspo->elevation = 0;

    return 0;
}
