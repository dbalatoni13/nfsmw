#include "../../../../../../../../../include/dol2asm.h"

SECTION_DATA void (*vbltmrsub[8])() = {0};
SECTION_DATA void (*tmrsub[8])() = {0};

SECTION_SDATA volatile int vblticks = 0;
SECTION_SDATA int TIMERhz = 0;
SECTION_SDATA volatile int ticks = 0;
SECTION_SDATA volatile int libticks = 0;
