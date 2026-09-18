# Middleware: la partición delta / muro

Medido el 3-sep-2026 sobre las 85 unidades con déficit (101.492 B).

| | bytes | funciones |
|---|---|---|
| **Delta de mnemónicos** — falta código | **54.360** | 74 |
| Muro del asignador | 19.072 | 60 |
| Sin objeto extraído | — | 12 |

**El middleware es mucho más estructural que las SourceLists** (74% del déficit
es legible, contra 55% allí). Es el mejor sitio para meter agentes ahora.

## El premio: `_vfprintf_r` + `_vfiprintf_r` = 11.888 B con UN arreglo

Las dos tienen **la misma firma**: `ble+9, bgt-9, lwz-9, li+6/7`. Y son **el mismo
fuente compilado dos veces**: `vfprintf_1.c` es literalmente `#define INTEGER_ONLY`
más `#include "vfprintf.c"`, el patrón `vfiprintf.c` de newlib. Arreglar
`vfprintf.c` cierra las dos.

Nueve `ble` nuestros contra nueve `bgt` suyos es **una comparación con la polaridad
invertida, repetida nueve veces**. En un printf eso apunta a las macros de relleno
(`PAD`/`PRINT`), que en newlib prueban `> 0` y `> PADSIZE`. Y los `lwz-9` dicen que
al original le faltan nueve cargas respecto a nosotros: probablemente porque
newlib construye un **iovec** (`__sprint`/`uio`) donde nosotros escribimos directo
con `memcpy` sobre `fp->_p`.

## Ranking por cuánto código baila

```
|d|  bytes   %         unidad                                   funcion                        delta
55   4728    91.058    criticalpath                             VP6_DecodeBlock                add+8, lbz+8, slwi+7, stw+6
53   6128    83.413    vfprintf                                 _vfprintf_r                    ble+9, bgt-9, lwz-9, li+6
52   5760    83.560    vfprintf_1                               _vfiprintf_r                   ble+9, bgt-9, lwz-9, li+7
44   848     68.958    spchpick                                 iSPCH_ChooseSentence__FPUi     add+8, addi+7, clrrwi+4, li+3
36   276     29.232    steering                                 Effect_Init                    stw+7, lfd-4, psq_st-4, stfd-4
33   3252    67.669    pathaction                               PATHI_serviceaction__FP9PATHEV blt+4, bne+3, slwi-3, cmpwi+2
29   936     76.419    sfir                                     calcFIRCoeffs__FP11SNDFIRSTATE mr-7, stw-4, addi+3, fsub-2
23   580     82.503    pathinit                                 PATH_addmapfile__FPc           lwz+18, subi-1, stwx+1, li+1
19   868     84.613    e_rem_pio2                               __ieee754_rem_pio2             lis-4, addi-2, stfd+2, stw-2
13   2184    93.462    k_rem_pio2                               __kernel_rem_pio2              lis-8, addi-2, li+2, mr-1
13   2180    93.514    kf_rem_pio2                              __kernel_rem_pio2f             lis-8, addi-2, li+2, fmr-1
12   664     91.687    DebuggerDriver                           DBGEXIImm                      cmpwi-2, b-2, bge+2, cmpw+2
12   316     83.848    filesys                                  FILE_nametodevice__FPCc        beq+3, cmpwi+2, mr-2, lmw-1
10   924     92.355    steering                                 SimThread_Step                 addi+2, lwz-2, mr+2, lwzx+2
10   704     90.415    borders                                  UpdateUMVBorder                mr+4, beq+2, ble-2, mullw+1
9    700     94.154    DebuggerDriver                           EXI2_ReadN                     addi-3, add+2, cmpwi-1, subfc+1
8    608     91.092    DebuggerDriver                           DBWrite                        srwi+3, cntlzw+3, bne+1, beq-1
8    580     94.069    DebuggerDriver                           AmcEXIImm                      cmpwi-1, srw+1, sraw-1, bne-1
7    408     74.412    DebuggerDriver                           AmcEXISync                     add-2, lwz-2, stw-2, addi+1
7    264     88.803    simpledeblocker                          FilterVert_Simple_C            lbzx-2, lbz+2, subf+1, add+1
7    188     66.511    pathtrack                                PATHI_getmastertrack__Fv       lwz+4, mr-1, li+1, lwzx+1
6    696     79.011    decodemode                               VP6_BuildModeTree              add+3, addi-3
6    440     92.455    dvd_device                               StartNonAlignedAyncRead__FP11D mr+6
6    252     84.444    filesys                                  Find__18FileOperationQueueib   cmpwi+2, bne+1, b-1, li-1
6    188     69.468    srender                                  SNDI_validrendermode__FPiP15SN lhzx+1, add+1, bne+1, mr+1
6    120     63.333    saemstimupdt                             SNDAEMSI_timerupdate__FPv      mr+4, lwz-1, stw-1
5    516     46.372    madidct                                  IdctRow                        mfctr+2, mr+2, mtctr+1
5    252     83.651    filesys                                  AddToQueue__13FILEOPERATION    mr.+1, mr-1, bge+1, blt-1
5    152     90.526    spchpick                                 iSPCH_SentenceLength__FP11VoxS srwi.+1, srwi-1, bge-1, cmpw-1
5    40      82.400    eathread_thread                          __static_initialization_and_de mtctr+1, bne-1, subi-1, bdnz+1

```

## Muro del asignador — no gastar presupuesto

```
MURO, los 10 mayores:
     1256 B   97.755%  itoa                               fftoa
      996 B   98.273%  fseek                              fseek
      992 B   99.435%  steering                           Effect_Update
      896 B   97.545%  pathnode                           PATHI_queuenode__FP9PATHTRACK
      740 B   93.232%  criticalpath                       VP6_PredictFilteredBlock
      692 B   99.451%  gc_driver                          OpenFile__Q26Realmc8GCDriverRCQ26R
      672 B   98.810%  pathsnd                            CheckStatus__Q24Path18PathTrackSnd
      656 B   98.780%  avplayer                           GetFirstFrame__Q24RCMP9AV_PLAYERUi
      644 B   94.963%  dering                             DeringBlockWeak_C
      628 B   98.439%  sndvd                              DSIHandler
```

## Unidades enteras SIN una línea de fuente

- `libc/e_pow` (2.000 B, `pow`) y `libc/ef_pow` (1.780 B, `powf`): vacías. Son
  fdlibm, escribibles.
- `ppc2D2` (2.448 B) y `snd/.../eaxadecf` (1.316 B): **no existe el fichero**.
- `LibSN/metrotrk` (12.136 B): es trabajo de ventana (`block_relocations`).

`libc/kf_rem_pio2` se escribió el 3-sep y quedó al 93,51%, el mismo techo del pool
de literales que su gemela `k_rem_pio2` (93,46%), con el mismo exceso de 36 B.

## Diagnósticos hechos a mano el 3-sep

### `SNDAEMSI_timerupdate` (120 B, 63,33%) — NO es forma de fuente

Desensambladas las dos y comparadas instrucción a instrucción. La diferencia no
está en el cuerpo, está en **la estrategia de salvado de registros**:

```
OBJETIVO                          NUESTRO
stwu r1, -0x38(r1)                stwu r1, -0x10(r1)
stmw r20, 0x8(r1)                 mflr r0 / stmw r30, 0x8(r1) / stw r0, 0x14(r1)
  ... dentro del bucle:
  mflr r22                          (LR ya está en el marco)
  mtlr r3
  mr r20, r0 / mr r21, r31
  blrl
  mr r31, r21 / mr r0, r20
  mtlr r22
```

**El objetivo no guarda LR en el prólogo**: lo aparca en `r22` dentro del bucle,
y salva **r20–r31** donde nosotros salvamos r30–r31 (marco 0x38 contra 0x10).
Los cuatro `mr` que nos faltan son ese trasiego (`r20`/`r21` alrededor del `blrl`).

Barrido: declarar las cuatro locales al principio de la función a la C89 en vez
de en el punto de uso — **63,3333% exacto, 112 B, sin mover una instrucción**.

Lectura: para que GCC aparque LR en un salvado hace falta que la función ya esté
salvando ese rango de registros, o sea que **al original le hacían falta más
valores vivos que a nosotros**. La veta no es reordenar lo que hay: es averiguar
qué tiene el original de más en esa rama `else`. Mientras no se sepa, el reparto
no converge.
