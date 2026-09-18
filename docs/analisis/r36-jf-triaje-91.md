# Las 91 funciones que faltan, clasificadas por TIPO de diferencia

`matched` 98,10 % son **91 funciones y 74.688 B**. El porcentaje no dice si una
funcion esta a un `mr` o a un bucle entero, asi que `scripts/triaje.py` diffea
cada unidad y cuenta las diferencias por clase:

    regs    solo cambia el registro del operando   -> PERMUTADOR, mecanico
    falta   instrucciones de menos                 -> falta una local o una sentencia
    sobra   instrucciones de mas                   -> codigo defensivo que el original no tiene
    otro    opcode distinto                        -> ESTRUCTURA, hay que leer

El veredicto `PERMUTADOR` (todas las diferencias son de registro) marca las que
puede cerrar la maquina sin que nadie lea nada. `falta codigo` con `falta`=1 o 2
suele ser **una sola local** que el DWARF del original tiene y nosotros no: eso
lo dice `regmap.py` en un minuto.

## Ensayo del permutador, para calibrar

`ICEMover::Update` (3.868 B, 99,907 %, veredicto PERMUTADOR, 15 diferencias de
registro) con `--guided -j 8`: 393 variantes de un cambio, 9,19 s cada una.
**No cierra.** El mejor candidato sigue con el mismo racimo de registros, asi que
para esta hace falta profundidad 2 o recocido, no un solo cambio.

## El permutador de UN cambio no cierra ninguna de las dos probadas

| funcion | B | % | catalogo | resultado |
|---|---|---|---|---|
| `ICEMover::Update` | 3.868 | 99,907 | 393 variantes (50 al foco), 9,19 s | **no cierra**, mismo racimo de registros |
| `TrackCopCameraMover::Update` | 948 | 99,156 | 160 variantes, 6,28 s | **no cierra**, mismo `mr`/`stfs` |
| `TrackCopCameraMover::Update` | 948 | 99,156 | **profundidad 2 + recocido**, 400 combinaciones | **no cierra**: `score` IDENTICO (0,995781) |

En la segunda `regmap` dice **IDENTICO** (mismas locales y mismo reparto), o sea
que no es una local: es el desempate del planificador. Y la de 948 B **tampoco cierra con profundidad 2 y recocido** (400
combinaciones): el `score` sale identico al de un solo cambio, con la misma
instruccion descolocada. O sea que ese empate del planificador **no es
alcanzable desde la fuente** con el catalogo del permutador -- ni moviendo
sentencias, ni conmutando, ni con temporales. Barrido cerrado.

## Vedas medidas sobre los empates del planificador

Tres de las funciones mas cercanas son **una sola instruccion mal colocada**, y
la fuente no la mueve. Barrido, para que nadie lo repita:

**`AV_PLAYER::GetFirstFrame` (656 B, 98,78 %)** -- el `mr r29, r5` que guarda el
tercer argumento va una ranura antes que en el objetivo (el objetivo mete el
`lwz r3, 0x34(r31)` por delante). Probado y SIN EFECTO:

- intercambiar las dos declaraciones (`audio_stream` / `video_latency`)
- **eliminar** la local `video_latency` y usar el argumento directamente

Lo coloca el asignador al guardar el argumento en un registro no volatil, y no
depende de la forma de la fuente.

**`TrackCopCameraMover::Update` (948 B, 99,16 %)** -- el primer `stfs f30,
0x58(r1)` de `bScale(&hcomp, &hcomp, 0.0f)` va antes del `mr r5, r3` que prepara
el argumento de `eMulVector`. Probado y SIN EFECTO: cambiar las tres
asignaciones `look_offset.x/.y/.z = 0.0f` por el constructor
`bVector3 look_offset(0.0f, 0.0f, 0.0f)` --que ademas es lo que dice el mapa de
lineas del original, `bMath.hpp:915`.

**`VDevice_RecalcGammaTable` (216 B, 99,80 %)** -- nuestro marco es 0x30 y el del
objetivo 0x20: usamos **tres** huecos de pila de 8 B (0x8, 0x10, 0x18) donde el
objetivo reutiliza **uno**. Probado y SIN EFECTO: quitar las llaves del
`if`/`else`, y pasarlo a operador ternario. **Ojo: `steering.c` es Metrowerks,
no GCC** (`cflags_libsn_mw_steering` en `configure.py`), asi que el mecanismo de
`free_temp_slots` de GCC no aplica y esto se ataca por flags, no por fuente.

## `CarLoader::DefragmentPool`: el arbol de bloques no se arregla con llaves

`regmap` da el diagnostico mas accionable de todo el triaje:

    b1/b0: el original abre 1 bloque anonimo y nosotros 0
    el bloque b1/b0/b0 FALTA en el nuestro (el original declara: hole)
    "hole": el original en b1/b0/b0, nosotros en b1/b0
    3 locales SOLO NUESTRAS: params (r26), table (r17), zero (r18)

y avisa: **eso va primero, los registros se recolocan solos**. Dos intentos de
crear ese nivel, los dos SIN EFECTO (99,269 % antes y despues, y `regmap` sigue
diciendo lo mismo):

1. envolver el `for (;;)` entero en un `{ }` -- **un bloque que no declara nada
   no sale en el DWARF**, asi que no cuenta;
2. envolver el CUERPO del `for` (con la declaracion de `hole` dentro) en un
   `{ }` -- GCC 2.9 con `-O1` funde ese bloque con el del `for`.

O sea que el nivel que falta no se fabrica con llaves: tiene que venir de una
construccion que abra ambito de verdad (otro bucle, un `if` con declaracion, o
las tres locales `params`/`table`/`zero` puestas donde el original las tiene).
Queda como el candidato mejor documentado del triaje.

## `epCalculateLocalDirectionalPOS16`: el mejor premio con el diagnóstico más pequeño

2.072 B al 93,31 % y `regmap` dice **89 locales iguales y sólo 3 registros
movidos**, los tres flotantes del mismo bloque de `EcstasyEx.cpp`:

    dcba   original f2   nuestro f4
    dcrg   original f3   nuestro f5
    scba   original f4   nuestro f13

Los tres corridos hacia arriba, y `scba` mucho mas. Ensayo: **declarar `dcba`
antes que `dcrg`** (el original le da a `dcba` el registro mas bajo).

    scba   f13 -> f8    <- mejora mucho
    dcba/dcrg           <- se INVIERTEN, y el orden que teniamos ya era el bueno
    neto: 93,305 % -> 92,681 %, de 155 a 170 diferencias.  REVERTIDO

O sea que los tres estan **acoplados**: mover el par de arriba arrastra a `scba`.
El orden relativo `dcba` antes de `dcrg` ya lo teniamos bien; lo que falta es
bajar los tres dos registros, no reordenarlos entre si.

## La palanca que sí movió algo: la barrera SELECTIVA

`asm("" : "+f"(x));` —extendido, **no volátil**, ata sólo lo que nombra— cerró
**34 de las 91 diferencias** de `WRoadNav::HolePunchAvoidables` (2.980 B):

    95,455 %  ->  98,130 %      91 diferencias -> 57
    y de paso `extra_width` casa, y `avoidable_delta_offset`/`offset_change`
    pasan a COMPARTIR registro como en el objetivo

Cuatro rondas (r25-r28) habían barrido seis formas de esa sentencia con binario
idéntico. **El diagnóstico ya estaba escrito en la propia fuente y era
correcto** —«sched adelanta este `fmuls` 17 filas y por eso pide un flotante
salvado (f30) donde el objetivo usa f2»—; lo que faltaba era la palanca, no el
análisis. Y con la base nueva **caducó una veda**: el cambio de `right_diagonal`
que antes rompía dos filas ahora no rompe ninguna (57 -> 55).

### Dónde vale y dónde no, medido

La firma la da `regmap`: **una local en registro PRESERVADO (f26..f31, r14..r31)
donde el objetivo usa uno VOLÁTIL (f0..f13, r0..r12)**. Eso es un adelanto del
planificador por encima de una llamada.

Cinco intentos más, todos negativos, para no repetirlos:

| función | qué se ató | resultado |
|---|---|---|
| `UpdatePlatInfo` | `envmap_min_g` antes de su uso | 98,659 -> **96,838 %**, +4 B |
| `UpdatePlatInfo` | `envmap_min_scale` antes de su definición | 98,659 -> **92,012 %**, +8 B |
| `GetLoadingPriority` | `adjusted_distance` después de calcularla | neutro |
| `GenerateIndex` | el PUNTERO `index` detrás de un store | 99,036 -> **95,781 %**, +12 B |
| `ICEMover::Update` | materializar `frame` (r8 en el original) | 99,907 -> **99,302 %** |
| `InitAtSegment` | el `0.0f` de `SetLaneOffset` antes de la llamada | 99,152 -> **96,132 %** |

**Uno de siete.** La regla que sale: **valor de coma flotante, atado ANTES de la
sentencia que lo consume, y sólo cuando el valor acaba en un registro PRESERVADO
por haber cruzado una llamada**. En `InitAtSegment` el diagnóstico parecía el
mismo —tres `stfs` adelantados, señalados por el propio permutador— y aun así
cuesta 3 puntos: ahí los `stfs` no cruzan ninguna llamada, así que el valor
nunca pedía un registro salvado y la barrera sólo estorba. Si `regmap` dice ciclo o desplazamiento entre volátiles es el
asignador, no el planificador, y la barrera sólo mete presión de registros.

## Segunda capa del triaje: el diagnóstico de `regmap` sobre las mayores

`triaje.py` dice de qué CLASE es la diferencia; `regmap --scan` dice si hay una
local detrás. Pasado sobre siete unidades:

| unidad | función | B | % | veredicto de regmap |
|---|---|---|---|---|
| zCamera | `ICEMover::Update` | 3868 | 99,91 | REPARTO, 4 registros (`bLerpLag`/`n_state` intercambiados) |
| zEcstasy | `epCalculateLocalDirectionalPOS16` | 2072 | 93,31 | REPARTO, **sólo 3 registros** (`dcba`/`dcrg`/`scba`) |
| zEcstasy | `eLightMaterialPlatInterface::UpdatePlatInfo` | 2044 | 98,66 | REPARTO, 11 registros |
| zWorld2 | `WRoadNav::HolePunchAvoidables` | 2980 | 95,46 | REPARTO, 5 registros |
| zWorld2 | `WRoadNav::InitAtSegment` | 816 | 99,15 | REPARTO, **1 registro** (`laneInd` r24/r26) |
| zTrack | `TrackStreamer::GetLoadingPriority` | 708 | 97,18 | REPARTO, 8; y el marco 0x120/0x110 |
| zWorld | `CarLoader::DefragmentPool` | 684 | 99,27 | **ESTRUCTURA**: árbol de bloques + 3 locales nuestras |
| zGameplay | `GRaceParameters::GenerateIndex` | 1680 | 99,04 | **ESTRUCTURA**: `flags` y `pflags` no existen en el original |
| zWorld | `CarRenderInfo::RenderFlaresOnCar` | 2908 | 98,62 | REPARTO, 3 registros |
| zAI | `UpdateAllAvoidables`, `AssignClosestOffsets` | 2908+1684 | 99,6 | IDÉNTICO: temporales |
| zCamera | `TrackCar/TrackCop::Update`, `TerrainVelocityNoise` | 992+948+1192 | 99,1-99,6 | IDÉNTICO: temporales |
| zEcstasy | `GenerateHorizonFogDisplayList`, `EmitterSystem::Render` | 796+696 | 98-99 | IDÉNTICO: temporales |

**La lectura**: de las mayores, sólo DOS tienen diagnóstico estructural
(`DefragmentPool` y `GenerateIndex`, las dos con locales que el original no
tiene). Las demás son reparto de registros o temporales del compilador. Y de las
de reparto, las dos con el diagnóstico más pequeño —**3 registros en 2.072 B** y
**1 registro en 816 B**— son con diferencia el mejor sitio donde meter horas de
permutador.

## La tabla

```
      B unidad       funcion                                           %  difs  veredicto
   3868 zCamera      Update__8ICEMoverf                          99.907    15  PERMUTADOR    regs=15 falta=0 sobra=0 otro=0
   3604 zCamera      __static_initialization_and_destruction_0   97.112    54  ESTRUCTURA    regs=30 falta=9 sobra=13 otro=2
   3120 zPhysics     __9SmackableRCQ25UMath7Matrix4RCQ36Attrib3  99.262    26  ESTRUCTURA    regs=20 falta=3 sobra=1 otro=2
   2980 zWorld2      HolePunchAvoidables__8WRoadNavP9NavCookiei  95.455    91  ESTRUCTURA    regs=60 falta=13 sobra=13 otro=5
   2908 zWorld       RenderFlaresOnCar__13CarRenderInfoP5eViewP  98.618    18  ESTRUCTURA    regs=8 falta=4 sobra=5 otro=1
   2908 zAI          UpdateAllAvoidables__11AIAvoidablef         99.601    14  ESTRUCTURA    regs=10 falta=0 sobra=0 otro=4
   2544 zGameplay    __8GTriggerRCUi                             99.583     4  ESTRUCTURA    regs=1 falta=1 sobra=1 otro=1
   2352 zEagl4Anim   Initialize__Q25EAGL413DynamicLoaderPFPCcRb  98.876    28  ESTRUCTURA    regs=14 falta=0 sobra=1 otro=13
   2072 zEcstasy     epCalculateLocalDirectionalPOS16__FPUiT0iP  93.305   155  ESTRUCTURA    regs=124 falta=13 sobra=13 otro=5
   2044 zEcstasy     UpdatePlatInfo__27eLightMaterialPlatInterf  98.659    75  falta codigo  regs=73 falta=1 sobra=1 otro=0
   1684 zAI          AssignClosestOffsets__9AIPursuitRQ33UTL3St  99.667     8  ESTRUCTURA    regs=6 falta=1 sobra=0 otro=1
   1680 zGameplay    GenerateIndex__15GRaceParametersP14GRaceIn  99.036     5  falta codigo  regs=1 falta=2 sobra=2 otro=0
   1588 zPlatform    ActualReadJoystickData__Fv                  99.244    19  falta codigo  regs=17 falta=2 sobra=0 otro=0
   1572 zFe2         RenderString__8FEngFontRC7FEColorPCsP8FESt  93.323    86  ESTRUCTURA    regs=61 falta=9 sobra=9 otro=7
   1488 zPhysicsBehaviors Add__6RBGridUiR9RigidBodyRCQ25UMath7Vector  99.301     8  ESTRUCTURA    regs=4 falta=0 sobra=0 otro=4
   1480 zEagl4Anim   EvalSQT__Q29EAGL4Anim12FnStatelessQfPfPCQ2  99.459     2  sobra codigo  regs=0 falta=0 sobra=2 otro=0
   1284 zWorld2      CookieTrailCurvature__8WRoadNavRCQ25UMath7  99.657     3  falta codigo  regs=2 falta=1 sobra=0 otro=0
   1240 zEAXSound2   GenerateRoadNoise__16CARSFX_RoadNoise       97.113    23  falta codigo  regs=15 falta=4 sobra=4 otro=0
   1192 zCamera      TerrainVelocityNoise__11CameraMoverP8bMatr  99.077    14  falta codigo  regs=12 falta=1 sobra=1 otro=0
   1156 zCamera      __Q33UTL11Collectionst8_Storage2ZPQ28Camer  97.834    16  ESTRUCTURA    regs=10 falta=2 sobra=3 otro=1
   1008 zEAXSound    SetupNextLoad__14EAXAemsManager             98.671    23  falta codigo  regs=21 falta=1 sobra=1 otro=0
    992 zCamera      Update__19TrackCarCameraMoverf              99.629    15  PERMUTADOR    regs=15 falta=0 sobra=0 otro=0
    992 steering     Effect_Update                               99.839     6  PERMUTADOR    regs=6 falta=0 sobra=0 otro=0
    948 zCamera      Update__19TrackCopCameraMoverf              99.156     2  falta codigo  regs=0 falta=1 sobra=1 otro=0
    936 sfir         calcFIRCoeffs__FP11SNDFIRSTATEi             89.252    75  ESTRUCTURA    regs=52 falta=10 sobra=11 otro=2
    924 steering     SimThread_Step                              95.671    68  ESTRUCTURA    regs=61 falta=3 sobra=2 otro=2
    876 zWorld       UpdateWheelYRenderOffset__13CarRenderInfo   99.384     7  falta codigo  regs=6 falta=1 sobra=0 otro=0
    856 zPhysicsBehaviors UpdateLoaded__Q217SuspensionTraffic4Tireff  97.210    21  ESTRUCTURA    regs=15 falta=2 sobra=3 otro=1
    836 zWorld       CullParts__13CarPartCullerP8bVector3Us      99.426     5  falta codigo  regs=4 falta=1 sobra=0 otro=0
    816 zWorld2      InitAtSegment__8WRoadNavscf                 99.152    42  PERMUTADOR    regs=42 falta=0 sobra=0 otro=0
    808 zFeOverlay   NotificationMessage__13CustomizeMainUlP8FE  97.896     9  falta codigo  regs=5 falta=2 sobra=2 otro=0
    796 zEcstasy     GenerateHorizonFogDisplayList__FPPvPUl9_GX  98.995     2  falta codigo  regs=0 falta=1 sobra=1 otro=0
    780 zEAXSound2   MsgPlayMiscSound__10SFX_CommonRC10MMiscSou  99.405    28  PERMUTADOR    regs=28 falta=0 sobra=0 otro=0
    740 criticalpath VP6_PredictFilteredBlock                    94.395    40  ESTRUCTURA    regs=31 falta=3 sobra=3 otro=3
    720 zEagl4Anim   FindMatchTime__CQ29EAGL4Anim12FnRunBlender  97.361     8  ESTRUCTURA    regs=3 falta=2 sobra=2 otro=1
    708 zTrack       GetLoadingPriority__13TrackStreamerP21Trac  97.181    71  ESTRUCTURA    regs=67 falta=0 sobra=0 otro=4
    696 zEcstasy     Render__13EmitterSystemP5eView              98.075    34  falta codigo  regs=32 falta=2 sobra=0 otro=0
    684 zWorld       DefragmentPool__9CarLoader                  99.269    23  PERMUTADOR    regs=23 falta=0 sobra=0 otro=0
    672 zEAXSound2   ProcessUpdate__12CARSFX_Turbo               97.500     8  falta codigo  regs=4 falta=2 sobra=2 otro=0
    656 avplayer     GetFirstFrame__Q24RCMP9AV_PLAYERUii         98.780     2  falta codigo  regs=0 falta=1 sobra=1 otro=0
91 funciones, 74688 B
```
