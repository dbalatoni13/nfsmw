#ifndef SMIXPORT_H
#define SMIXPORT_H
// smixport.h -- UN SOLO reparto de sndmix, y solo para el port de 64 bits.
//
// EL FALLO QUE ARREGLA. `sndmix` es un global unico que catorce unidades ven a
// traves de CUATRO declaraciones distintas: la completa de smixer.c y sfxrevc.c
// (20 campos, sin relleno), la parcial de smixi.h (`unpackinit[12]` mas rellenos
// `char padN[0xA - 0xB]`) y la de dos campos de supf/suplf/suppf. En GameCube
// las cuatro coinciden porque los rellenos estan medidos contra el asm; **en
// arm64 se separan**, porque cada puntero pasa de 4 a 8 bytes y un relleno
// escrito como cuenta de bytes no crece con ellos.
//
// Medido: `sinit16.c` escribe `sndmix.unpackinit[1] = SFILTER_unpacklfinit` en
// el byte 0x20 de SU reparto, y `smixer.c` lee ese mismo byte como parte de
// `outputfilterhead`. De ahi la traza que mataba el render -- MIX_audioslice
// llamando a una funcion de INIT como si fuera un filtro. No era el mezclador ni
// el decomp: eran dos vistas de la misma memoria que dejaron de coincidir al
// pasar a 64 bits.
//
// COMO LO ARREGLA. Copia la secuencia de campos de smixer.c tal cual --que es la
// real, sacada del asm-- y ofrece **los dos vocabularios** sobre ella con
// uniones anonimas, porque las dos familias de nombres estan en uso:
// `unpackinit`/`filterunpackinit`, `unpackstatesize`/`filterunpacksize`,
// `voices`/`smv`. Al no llevar ni un relleno, el reparto lo calcula el
// compilador y sale igual en todas las unidades, en 32 y en 64 bits.
//
// GameCube NO ve este fichero: todo su uso va bajo `#if defined(__ANDROID__)`.
// Si algun dia hay que tocarlo, la referencia es smixer.c, no este fichero.

#include <snd/sfilter.h>

typedef struct MIXCONFIG {
    int outputrate;
    void (*chanovercallback)(int);
    unsigned char inputchannels;
    unsigned char outputchannels;
    unsigned char outputinterleaved;
} MIXCONFIG;

typedef struct UNPACKINITPARAMS UNPACKINITPARAMS;
typedef struct TIMESTRETCHSTATE TIMESTRETCHSTATE;
typedef void UNPACKINITFN(void *pstate, UNPACKINITPARAMS *pparams);

// Los estados de filtro entran aqui SOLO como puntero, asi que van adelantados:
// definirlos completos chocaria con smixhip.c, smixlowp.c y smixptch.c, que ya
// traen los suyos. (Medido: sin esto, "redefinition of HPFSTATE" en las tres.)
typedef struct RSFSTATE RSFSTATE;
typedef struct LPFRCSTATE LPFRCSTATE;
typedef struct HPFSTATE HPFSTATE;

// Este si va por valor dentro de sndmix, asi que necesita cuerpo. Lleva nombre
// propio para no chocar con el FT24_32STATE de sfft24.c y smixer.c; el reparto
// es el mismo, que es lo unico que importa aqui.
typedef struct SNDMIXPORT_FT24 { SFILTERNODE sfn; } SNDMIXPORT_FT24;

// La voz del mezclador. smixi.h la llamaba MIXVOICE y la abria con
// `char pad0[0x40]`, que en el original tapa DOS PUNTEROS (wetbusgainprev y
// wetbusgain): por eso alli tampoco cuadraba en 64 bits. Aqui va entera.
typedef struct SNDMIXVOICE {
    char status;
    signed char gainchanged;
    short pad;
    float drybusgainprev[6];
    float drybusgain[6];
    float *wetbusgainprev;
    float *wetbusgain;
    float lastmixed;
    union { SFILTERNODE *filterhead; SFILTERNODE *pfilterhead; };
    union { int (*unpackgetframe)(void *); int (*getframe)(void *); };
    union { void *pups; void *pclient; };
    union { TIMESTRETCHSTATE *ptss; TIMESTRETCHSTATE *ptimestretch; };
    union { RSFSTATE *prss; RSFSTATE *prsf; };
    union { LPFRCSTATE *plps; LPFRCSTATE *plpf; };
    HPFSTATE *phpf;
} SNDMIXVOICE;
typedef SNDMIXVOICE MIXVOICE;

typedef struct SNDMIXSTATE {
    MIXCONFIG mc;
    unsigned short outputframeincr;
    // el mismo byte: smixer.c lo llama resamplequality y smixi.h platformver
    union { unsigned char resamplequality; unsigned char platformver; };
    char pad[1];
    void (*fxadd)(int);
    int *wetbusactive;
    // [18] y [6][3] tienen EL MISMO reparto; los indices que usa el arbol son
    // 0..11, que caen en las cuatro primeras filas.
    union {
        UNPACKINITFN *unpackinit[18];
        UNPACKINITFN *filterunpackinit[6][3];
    };
    union {
        int unpackstatesize[18];
        int filterunpacksize[6][3];
    };
    float *punalignedtempbuf[2];
    float *ptempbuf[2];
    float *punalignedmixbuf[6];
    float *pmixbuf[6];
    float stoppedvoiceaccdry[6];
    float *stoppedvoiceaccwet;
    SFILTERNODE *outputfilterhead[6];
    LPFRCSTATE *plps[6];
    SNDMIXPORT_FT24 ft24s[6];
    union { SNDMIXVOICE *smv; SNDMIXVOICE *voices; };
    void (*mix)(int, float, void *, void *);
    void (*decode16)(int, short *, float *);
} SNDMIXSTATE;

extern SNDMIXSTATE sndmix;

#endif
