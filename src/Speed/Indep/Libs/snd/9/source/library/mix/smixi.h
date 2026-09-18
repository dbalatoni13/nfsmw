#ifndef SMIXI_H
#define SMIXI_H

// Internal header for the snd mix library (smix* / sinit* units).
// Layout reconstructed from build/GOWE69/asm mix units.
// NOTE: the snd lib is compiled with -x c++; SFILTER_* helpers keep their
// mangled names, so they are declared as plain C++ functions.

#if defined(__ANDROID__)
// En 64 bits los rellenos de esta vista dejan de cuadrar con el reparto
// real de smixer.c: ver smixport.h, que da UNA sola definicion con los dos
// vocabularios de nombres.
#include "smixport.h"
#else
typedef struct TIMESTRETCHSTATE TIMESTRETCHSTATE;
typedef struct UNPACKINITPARAMS UNPACKINITPARAMS;

// total size: 0x5C
typedef struct MIXVOICE {
    char pad0[0x40];
    struct SFILTERNODE *pfilterhead;     // offset 0x40
    int (*getframe)(void *pclient);      // offset 0x44
    void *pclient;                       // offset 0x48
    TIMESTRETCHSTATE *ptimestretch;      // offset 0x4C
    struct RSFSTATE *prsf;               // offset 0x50
    struct LPFRCSTATE *plpf;             // offset 0x54
    struct HPFSTATE *phpf;               // offset 0x58
} MIXVOICE;

typedef struct SNDMIXSTATE {
    char pad0[0xE];
    unsigned char platformver;           // offset 0xE
    char pad0b[0x18 - 0xF];
    void (*unpackinit[12])(void *pstate, UNPACKINITPARAMS *pparams); // offset 0x18
    char pad1[0x60 - 0x48];
    int unpackstatesize[12];             // offset 0x60
    char pad2[0x1DC - 0x90];
    MIXVOICE *voices;                    // offset 0x1DC
} SNDMIXSTATE;

extern SNDMIXSTATE sndmix;
#endif

void SFILTER_timestretchsetratio(TIMESTRETCHSTATE *pts, int ratio);
void SFILTER_unpackfinit(void *pstate, UNPACKINITPARAMS *pparams);
void SFILTER_unpacklfinit(void *pstate, UNPACKINITPARAMS *pparams);
void SFILTER_unpackpfinit(void *pstate, UNPACKINITPARAMS *pparams);
void SFILTER_unpackxafinit(void *pstate, UNPACKINITPARAMS *pparams);
void SFILTER_unpackxalfinit(void *pstate, UNPACKINITPARAMS *pparams);
void SFILTER_unpackxapfinit(void *pstate, UNPACKINITPARAMS *pparams);
void SFILTER_unpackmtfinit(void *pstate, UNPACKINITPARAMS *pparams);
void SFILTER_unpackmtlfinit(void *pstate, UNPACKINITPARAMS *pparams);
void SFILTER_unpackmtpfinit(void *pstate, UNPACKINITPARAMS *pparams);

#endif
