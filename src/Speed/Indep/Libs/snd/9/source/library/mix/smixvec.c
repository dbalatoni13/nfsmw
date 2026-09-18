// smixvec.c -- el vector de punteros del mezclador: la indireccion que
// `SNDSYS_linkmaincpumixer()` (slinkmix.c) rellena en arranque y que `snddrv.c`
// llama.
//
// La unidad emite 28 B de `.bss` en 0x804CC680..0x804CC69C y nada mas
// (.text 0x8036BF68..0x8036BF68, 0 B). Los siete punteros y su orden salen de
// la CU de DWARF1 del ELF original, que los da con AT_location explicito:
//
//   MIXinitfn        0x804CC680     MIXplayfn      0x804CC690
//   MIXrestorefn     0x804CC684     MIXstopfn      0x804CC694
//   MIXaudioslicefn  0x804CC688     MIXsetpitchfn  0x804CC698
//   MIXplayinitfn    0x804CC68C
typedef struct MIXCONFIG MIXCONFIG;

void (*MIXinitfn)(MIXCONFIG *);
void (*MIXrestorefn)();
void (*MIXaudioslicefn)(void **, int);
void (*MIXplayinitfn)(int, int, int, void *, void *, void *, int, int, int, int, void *, void *,
                      int, int);
void (*MIXplayfn)(int);
void (*MIXstopfn)(int);
void (*MIXsetpitchfn)(int, int);
