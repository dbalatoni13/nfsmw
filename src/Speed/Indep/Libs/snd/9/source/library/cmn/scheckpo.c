#include "snd/sndo.h"

void SNDI_checkplayopts(SNDPLAYOPTS *pspo) {
    if (pspo->timemult > 0x2000) {
        pspo->timemult = 0x2000;
        return;
    }
    if (pspo->timemult < 0x800) {
        pspo->timemult = 0x800;
    }
}
