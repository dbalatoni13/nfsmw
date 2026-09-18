# r36e -- ventana: verificacion, promocion y la criba de contenido

DOL verificado: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, el esperado.

| | al empezar la r36d | ahora |
|---|---|---|
| matched | 98,33 % | **98,43 %** |
| codigo casado | 3.880.024 B | **3.884.092 B** |
| funciones | 18.338 | **18.352** |
| linked | 476 / 617 | 476 / 617 |

## Lo que cerre yo en la ventana

| funcion | B | como |
|---|---|---|
| `DoFSReadHeader` | 140 | barrera selectiva contra el pliegue de `16 << 8` |
| los 7 alias de 64 bits de `ppc2D2` | 28 | `asm()` de ambito de fichero |
| `__cvt_fp2unsigned` | 72 | puntero local **mas** barrera |

Y `AddToQueue` pasa a 252/252 B con la prueba de nulo redundante
traida; le quedan dos filas de planificacion.

## La regla que sale de la ventana

`@l(base)` contra `addi @l` mas `0x0(rN)` dice si la fuente TOMA la
direccion, y las dos direcciones quedaron medidas el mismo dia:
`CompletePCreadAsync` sobraba el `addi` (habia un `&`, se quito) y
`__cvt_fp2unsigned` faltaba (no habia puntero, se metio con barrera).
No es `-fforce-addr`: ese flag da el objeto identico.

## Herramientas nuevas

- `scripts/tamfn.py` -- compila UN fichero con sus cflags reales y da el
  tamano por funcion. Un segundo por vuelta en vez de minutos de ninja.
- `scripts/fncmp.py` -- **la criba que faltaba**: que funciones no son
  IDENTICAS, no cuales tienen otro tamano. Ve las unidades de middleware
  que `triaje.py` no ve. Validada contra el informe oficial en las 18
  unidades libres: coincide al byte en 17.

El censo por tamano enganaba de largo: en FSasync daba 1 funcion
pendiente y son 2; en filesys daba 1 y son 3; en steering daba 2 y son 11.

## Cola de trabajo en territorio libre (34 funciones, 12104 B)

Ordenada por instrucciones distintas, que es lo que predice el esfuerzo.

| insn | B | funcion | unidad |
|---|---|---|---|
| 1 | 268 | `TIMER_init__Fi` | inittmr |
| 1 | 120 | `fn_8030EA80` | metrotrk |
| 1 | 44 | `fn_8030F79C` | metrotrk |
| 2 | 316 | `EXI2TCHandler` | FSasync |
| 2 | 264 | `EXI2_Poll` | DebuggerDriver |
| 2 | 252 | `AddToQueue__13FILEOPERATION` | filesys |
| 3 | 412 | `iSPCH_ChooseSamples__FP16SentencePickInfoP11` | spchpick |
| 3 | 176 | `fn_8030F6EC` | metrotrk |
| 4 | 432 | `EXI2_WriteN` | DebuggerDriver |
| 4 | 108 | `AmcEXISetExiCallback` | DebuggerDriver |
| 5 | 524 | `iSPCH_MakeSampleRequests__FP8VoxEventP11VoxS` | spchpick |
| 5 | 116 | `VDevice_GetFreeEffect` | steering |
| 6 | 992 | `Effect_Update` | steering |
| 6 | 188 | `VDevice_DownloadEffect` | steering |
| 8 | 424 | `PATH_createstreamimp__Fiif` | pathtrack |
| 8 | 324 | `SimThread_Init` | steering |
| 11 | 216 | `VDevice_RecalcGammaTable` | steering |
| 12 | 396 | `PATHI_loadbankdata__FP9PATHTRACKii` | pathbank |
| 12 | 380 | `__VMBASESetupExceptionHandlers` | vmbase |
| 13 | 260 | `CookValues` | steering |
| 14 | 336 | `PATHI_calcwaitbeat__FiiiP12PATHBEATINFO` | pathnode |
| 15 | 136 | `iSPCH_GetSampleSizeData__FP10VOXBANKHDRiPUiT` | spchsamp |
| 26 | 276 | `Effect_Init` | steering |
| 37 | 740 | `VP6_PredictFilteredBlock` | criticalpath |

## Y las SourceLists, que son de los agentes

| B sin casar | unidad | matched |
|---|---|---|
| 11928 | zCamera | 90.458 % |
| 5876 | zEcstasy | 95.972 % |
| 5608 | zWorld | 96.516 % |
| 5008 | zEagl4Anim | 95.569 % |
| 3900 | zEAXSound2 | 97.709 % |
| 3796 | zWorld2 | 97.277 % |
| 2908 | zAI | 98.934 % |
| 2344 | zPhysicsBehaviors | 99.008 % |
| 1748 | zEAXSound | 98.843 % |
| 1696 | zGameplay | 98.801 % |
| 1588 | zPlatform | 94.595 % |
| 912 | zSpeech | 99.49 % |
| 708 | zTrack | 98.889 % |
| 384 | zFe2 | 99.847 % |

## Vedas medidas en la ventana

- `zSim`, la guarda gemela de `~FixedVector`: las dos variantes BAJAN
  (-400 B y 398/402; -924 B y 396/402). La receta de `_Storage` no
  transfiere porque aquel destructor era implicito y este es virtual
  con cuerpo.
- `zCamera`, el orden de `ICEReplay.cpp`: ya es el ultimo de los 28
  includes y la unidad es UNA sola TU (un solo `STT_FILE`). Los cuatro
  inicializadores dinamicos que faltan son estaticos ausentes en los
  `.cpp` vacios, no orden de enlace.
- `EXI2TCHandler`: seis formas mas de barrera sobre los dos `stw`
  (`+m` en tres posiciones, `+r` en dos, las dos a la vez). Van 16.
- `sfir::calcFIRCoeffs`: siete formas y cinco flags. Sobra UNA
  instruccion y es cse1, no gcse; la barrera no la toca.
- `DebuggerDriver`: los cflags son correctos (3 de 31 funciones), y las
  tres que fallan son el mismo orden de epilogo. `-opt noschedule` y
  `-opt nopeephole` lo destrozan (26 y 27 funciones).
- `iSPCH_ChooseSamples`: cuatro formas mas, ninguna baja de 3 filas.
