#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndenum.h"

struct GlobalFxProcessorData {
    int fxBusTag; // offset 0x0, size 0x4
    int busId; // offset 0x4, size 0x4
    float fxBusOutputLevel[6]; // offset 0x8, size 0x18
    Snd::Device device; // offset 0x20, size 0x4
    int isReset; // offset 0x24, size 0x4
    unsigned int instanceHandle; // offset 0x28, size 0x4
};
