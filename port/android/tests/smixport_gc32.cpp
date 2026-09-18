// En 32 bits smixport.h tiene que dar EXACTAMENTE los offsets medidos contra el
// asm de GameCube, los que estan comentados en smixer.c y en smixi.h.
#include "smixport.h"
#define OFF(c) __builtin_offsetof(SNDMIXSTATE, c)
static_assert(OFF(mc) == 0x0, "mc");
static_assert(OFF(outputframeincr) == 0xC, "outputframeincr");
static_assert(OFF(platformver) == 0xE, "platformver (smixi.h decia 0xE)");
static_assert(OFF(fxadd) == 0x10, "fxadd");
static_assert(OFF(wetbusactive) == 0x14, "wetbusactive");
static_assert(OFF(unpackinit) == 0x18, "unpackinit (smixi.h decia 0x18)");
static_assert(OFF(unpackstatesize) == 0x60, "unpackstatesize (smixi.h decia 0x60)");
static_assert(OFF(punalignedtempbuf) == 0xA8, "punalignedtempbuf");
static_assert(OFF(outputfilterhead) == 0x104, "outputfilterhead");
static_assert(OFF(ft24s) == 0x134, "ft24s");
static_assert(OFF(voices) == 0x1DC, "voices (smixi.h decia 0x1DC)");
static_assert(OFF(mix) == 0x1E0, "mix");
static_assert(OFF(decode16) == 0x1E4, "decode16 (supf.c decia 0x1E4)");
static_assert(sizeof(SNDMIXSTATE) == 0x1E8, "tamano total 0x1E8");
static_assert(sizeof(SNDMIXVOICE) == 0x5C, "MIXVOICE/SNDMIXVOICE 0x5C");
static_assert(__builtin_offsetof(SNDMIXVOICE, pfilterhead) == 0x40, "pfilterhead 0x40");
static_assert(sizeof(MIXCONFIG) == 0xC, "MIXCONFIG 0xC");
