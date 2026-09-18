// Comprueba en ARM64 que smixport.h reparte igual que smixer.c. Si algun campo
// no coincide, la compilacion falla y dice cual.
#include "smixport.h"
#define offsetof(t, m) __builtin_offsetof(t, m)

typedef struct SNDMIXSTATE_REF {   // la secuencia de campos de smixer.c
    MIXCONFIG mc;
    unsigned short outputframeincr;
    unsigned char resamplequality;
    char pad[1];
    void (*fxadd)(int);
    int *wetbusactive;
    UNPACKINITFN *filterunpackinit[6][3];
    int filterunpacksize[6][3];
    float *punalignedtempbuf[2];
    float *ptempbuf[2];
    float *punalignedmixbuf[6];
    float *pmixbuf[6];
    float stoppedvoiceaccdry[6];
    float *stoppedvoiceaccwet;
    SFILTERNODE *outputfilterhead[6];
    LPFRCSTATE *plps[6];
    SNDMIXPORT_FT24 ft24s[6];
    SNDMIXVOICE *smv;
    void (*mix)(int, float, void *, void *);
    void (*decode16)(int, short *, float *);
} R;

#define CMP(c) static_assert(offsetof(SNDMIXSTATE, c) == offsetof(R, c), "diverge: " #c)
CMP(mc); CMP(outputframeincr); CMP(fxadd); CMP(wetbusactive);
CMP(filterunpackinit); CMP(filterunpacksize);
CMP(punalignedtempbuf); CMP(ptempbuf); CMP(punalignedmixbuf); CMP(pmixbuf);
CMP(stoppedvoiceaccdry); CMP(stoppedvoiceaccwet);
CMP(outputfilterhead); CMP(plps); CMP(ft24s); CMP(smv); CMP(mix); CMP(decode16);
static_assert(sizeof(SNDMIXSTATE) == sizeof(R), "diverge el tamano total");

// y que los alias caen sobre su campo
static_assert(offsetof(SNDMIXSTATE, unpackinit) == offsetof(SNDMIXSTATE, filterunpackinit), "alias unpackinit");
static_assert(offsetof(SNDMIXSTATE, unpackstatesize) == offsetof(SNDMIXSTATE, filterunpacksize), "alias unpackstatesize");
static_assert(offsetof(SNDMIXSTATE, voices) == offsetof(SNDMIXSTATE, smv), "alias voices");
static_assert(offsetof(SNDMIXSTATE, platformver) == offsetof(SNDMIXSTATE, resamplequality), "alias platformver");

// y la vista de supf/suplf/suppf: decode16 es el ultimo campo
static_assert(offsetof(SNDMIXSTATE, decode16) + sizeof(void (*)(int, short *, float *)) == sizeof(SNDMIXSTATE), "decode16 no cierra la struct");
// FT24_32STATE de smixer.c y SNDMIXPORT_FT24 reparten igual (SFILTERNODE solo)
typedef struct FT24_32STATE { SFILTERNODE sfn; } FT24_32STATE;
static_assert(sizeof(FT24_32STATE) == sizeof(SNDMIXPORT_FT24), "ft24 diverge");
