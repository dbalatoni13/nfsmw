# r63 -- las 27 abiertas, decompiladas con las DOS piezas puestas

Un solo proyecto Ghidra con el **sleigh Gekko/Broadway** y la **extension DWARF1
parcheada** a la vez. Las 27 funciones abiertas (mas `ActualReadJoystickData`,
cerrada en la r61, que va de control) salen **enteras**:

- **28 de 28 con cobertura 100,0 %** (33.668 B de 33.668), cero truncadas,
  cero `DECOMPILE_FAILED`, cero `halt_baddata`.
- **375 instrucciones paired-single decodificadas** dentro de esas 28.
- **22 de 28 con firma real del DWARF** (`IMPORTED`): tipos, nombres de
  parametro, nombres de miembro y de enum. Las 6 restantes son exactamente las
  6 de `steering`, que no compilo GCC y no tiene DWARF.
- `GQR0 = 0` fijado antes del analisis, de modo que `psq_l`/`psq_st` se leen
  como cargas y almacenes normales.

Entregables:

| fichero | que es |
|---|---|
| `docs/analisis/r63-ghidra-27-decomp.c` | el C de las 28, con cabecera por funcion (tamano, instrucciones, cobertura, numero de paired-singles, origen de la firma y prototipo) |
| `docs/analisis/r63-ghidra-27-asm.txt` | el listado de instrucciones de las 28 con la palabra en hexadecimal -- **sin esto no se puede afirmar nada sobre `&&` contra `||`** |
| `ghidra_scripts/SetGqr.java` | fija GQR0=0 en los bloques ejecutables (va de `-preScript`, antes del auto-analisis) |
| `ghidra_scripts/Dwarf1Run.java` | lanza el `DWARF1ProgramAnalyzer` por reflexion y escribe el censo antes/despues |
| `ghidra_scripts/ExportOpen.java` | exporta C + asm + resumen de una lista `addr,tam,etiqueta` |

## 0. La tabla, tal como la escribe el script

Salida literal de `ExportOpen.java` (`scratchpad/ghidra63/both/out/resumen27.txt`).
La columna `firma` dice si el prototipo viene del DWARF (`IMPORTED`) o lo invento
Ghidra (`DEFAULT`); `hueco` seria la primera direccion sin desensamblar.

```
LANGUAGE = PowerPC:BE:32:Gekko_Broadway
GHIDRA   = 11.4
GQR0 en 80109364 = 0

addr      tam    insn   cover%  ps    hueco     firma     nombre
8007c390  3868   967    100.0   12    -         IMPORTED  Update__8ICEMoverf
8008095c  3604   901    100.0   18    -         IMPORTED  staticinit_zCamera
802feab0  2980   745    100.0   36    -         IMPORTED  HolePunchAvoidables__8WRoadNavP9NavCookieiff
802cb51c  2908   727    100.0   28    -         IMPORTED  RenderFlaresOnCar__13CarRenderInfo
8008f098  2352   588    100.0   0     -         IMPORTED  Initialize__Q25EAGL413DynamicLoader
80109358  2072   518    100.0   185   -         IMPORTED  epCalculateLocalDirectionalPOS16
80109bb8  2044   511    100.0   36    -         IMPORTED  UpdatePlatInfo__27eLightMaterialPlatInterface
8026c574  1588   397    100.0   14    -         IMPORTED  ActualReadJoystickData__Fv
800d80fc  1240   310    100.0   8     -         IMPORTED  GenerateRoadNoise__16CARSFX_RoadNoise
80075c44  992    248    100.0   6     -         IMPORTED  Update__19TrackCarCameraMoverf
80321474  924    231    100.0   0     -         DEFAULT   SimThread_Step
802c9fb8  876    219    100.0   14    -         IMPORTED  UpdateWheelYRenderOffset__13CarRenderInfo
8024b310  856    214    100.0   6     -         IMPORTED  UpdateLoaded__Q217SuspensionTraffic4Tire
8030183c  816    204    100.0   6     -         IMPORTED  InitAtSegment__8WRoadNavscf
803598fc  740    185    100.0   0     -         IMPORTED  VP6_PredictFilteredBlock
802bc0a4  708    177    100.0   6     -         IMPORTED  GetLoadingPriority__13TrackStreamer
802dfcac  684    171    100.0   0     -         IMPORTED  DefragmentPool__9CarLoader
8034bac4  632    158    100.0   0     -         IMPORTED  IdctColumn
802a2248  596    149    100.0   0     -         IMPORTED  Setup__Q26Speech13RoadblockFlow
8031f7fc  588    147    100.0   0     -         DEFAULT   HandleTriggers
8034bd3c  516    129    100.0   0     -         IMPORTED  IdctRow
800a3090  456    114    100.0   0     -         IMPORTED  EvalState__Q29EAGL4Anim14FnRawStateChanf
80321330  324    81     100.0   0     -         DEFAULT   SimThread_Init
802dd360  304    76     100.0   0     -         IMPORTED  SetMemoryPoolSize__9CarLoaderi
80320840  276    69     100.0   0     -         DEFAULT   Effect_Init
8010ac40  268    67     100.0   0     -         IMPORTED  eProject__FfffPA3_fPfN44
8031fa48  260    65     100.0   0     -         DEFAULT   CookValues
803210b8  196    49     100.0   0     -         DEFAULT   Effect_PerformEnvelope
```

Los seis `DEFAULT` son `SimThread_Step`, `HandleTriggers`, `SimThread_Init`,
`Effect_Init`, `CookValues` y `Effect_PerformEnvelope`: las seis de `steering`.
Es la tercera medida independiente de que esa unidad no la compilo GCC.

## 1. La receta, reproducible desde cero

**Instalacion elegida: Ghidra 11.4 DEV.** Trae el sleigh Gekko de fabrica y ya
tiene instalada la extension DWARF1 parcheada (`Ghidra/Extensions/ghidra-dwarf1/`).
El 12.1.2 tambien vale, pero su sleigh lo porto a mano el agente `gekko`; el 11.4
esta intacto. Comprobado antes de empezar:

```
$ ls .../ghidra_11.4_DEV/Ghidra/Processors/PowerPC/data/languages/ | grep -i gekko
ppc_gekko_broadway.cspec  ppc_gekko_broadway.pspec  ppc_gekko_broadway.sla  ppc_gekko_broadway.slaspec
$ grep -c Gekko_Broadway .../ppc.ldefs        -> 1
$ ls .../ghidra_11.4_DEV/Ghidra/Extensions/  -> ghidra-dwarf1
```

**Proyecto propio, fuera del repo**: `C:\Users\jferr\AppData\Local\Temp\ghidra_r63both`.
No se ha tocado `ghidra_project/` del usuario ni una sola vez (ademas, el
proyecto del usuario es de 12.1.2 y el 11.4 no puede abrirlo: hay que reimportar).

### Paso 1 -- importar con el lenguaje Gekko, fijando GQR0 ANTES del analisis

`scratchpad/ghidra63/both/import.bat`:

```bat
call "%GH%\support\launch.bat" fg jdk GhidraHeadless 16G "-XX:ParallelGCThreads=4" ^
  ghidra.app.util.headless.AnalyzeHeadless "%PRJ%" NFSMW ^
  -import "...\orig\GOWE69\NFSMWRELEASE.ELF" ^
  -processor PowerPC:BE:32:Gekko_Broadway ^
  -scriptPath "%SC%" -preScript SetGqr.java
```

`SetGqr.java` va de **preScript**: escribe `GQR0=0` sobre los tres bloques
ejecutables antes de que el auto-analisis desensamble nada.

```
SETGQR: GQR0=0 en .init [80003100 .. 8000348b]
SETGQR: GQR0=0 en .text [800034a0 .. 803a41b7]
SETGQR: GQR0=0 en .over [803a41b8 .. 803c8b4f]
```

**Con auto-analisis completo** (esto es lo que la fase 1 dejo pendiente: aquel
proyecto iba con `-noanalysis`): `Total Time 295 secs`, `Import succeeded`,
`Save succeeded`. Los tramos gordos: `Disassemble Entry Points` 66,7 s,
`PowerPC Constant Reference Analyzer` 95,2 s, `Stack` 90,3 s,
`Decompiler Switch Analysis` 6,8 s. Con el analisis hecho, **las tablas de salto
de los `switch` estan resueltas**, que era la unica pega real del decompilado de
la fase 1.

### Paso 2 -- el DWARF, por reflexion, sobre el proyecto ya analizado

```bat
  ... AnalyzeHeadless "%PRJ%" NFSMW -process NFSMWRELEASE.ELF -noanalysis ^
      -scriptPath "%SC%" -postScript Dwarf1Run.java "%WD%\dwarf1.txt"
```

`process() = true en 35 s`, y el censo antes/despues sobre ESTE proyecto (no el
de la fase 1: aqui las funciones ya vienen del auto-analisis, 18.446, no 86):

```
                        ANTES     DESPUES
funciones                18446      18442
firma == IMPORTED           86      13824
con >=1 parametro           83      12968
structs                     11    4076 (con campos: 3802)
unions                       0         49
enums                        2       2872
tipos en el DTM            271      10898
```

Cuadra con lo que midio el agente `dwarf1` sobre el proyecto sin analizar
(13.824 firmas, 4.076 structs, 2.872 enums): **el auto-analisis previo no le
quita ni le anade nada al DWARF**. Las 4 funciones que desaparecen son fusiones
del importador.

### Paso 3 -- exportar

```bat
  ... -process NFSMWRELEASE.ELF -noanalysis -readOnly ^
      -scriptPath "%SC%" -postScript ExportOpen.java lista27.csv out.c out.asm resumen.txt
```

28 funciones en menos de un minuto. `-readOnly` para poder repetirlo sin tocar
el proyecto.

### Trampas que costaron tiempo (las de la fase 1 siguen valiendo)

- `analyzeHeadless.bat` fija `MAXMEM=2G` y no llega; hay que llamar a
  `launch.bat` directo. **El quinto argumento (vmargs) no puede ir vacio**.
- Desde bash hay que lanzarlo con la herramienta PowerShell, no con `cmd /c`.
- El import de 105 MB no cabe en el timeout de 10 min de Bash: en segundo plano.

## 2. Tres avisos antes de leer el `.c`

### (a) Los 284 `Removing unreachable block` NO son un defecto: son el precio de GQR0

Reparto por funcion:

```
91 epCalculate | 36 UpdatePlatInfo | 36 HolePunchAvoidables | 29 RenderFlaresOnCar
18 staticinit_zCamera | 14 UpdateWheelY | 14 ActualRead | 12 ICEMover
 8 GenerateRoadNoise | 6 TrackCar | 6 UpdateLoaded | 6 InitAtSegment
 6 GetLoadingPriority | 2 Effect_Init
```

En `Update__8ICEMoverf` los 12 avisos son 8007c39c..8007c3b0 y 8007d288..8007d29c,
y el listado dice que esas 12 palabras son EXACTAMENTE las 12 `psq_st`/`psq_l`
del prologo y del epilogo:

```
  8007c39c  f3410188  psq_st f26,0x188(r1),0x0,GQR0
  8007c3a0  f3610190  psq_st f27,0x190(r1),0x0,GQR0
  ...
```

El sleigh modela el GQR de verdad y expande cada `psq_` en un arbol de 15 ramas
por tipo de cuantizacion; con GQR0 fijado a cero, 14 de esas 15 son inalcanzables
y el decompilador las tira **avisando**. La correlacion es 1:1 con la columna
`ps` de la tabla en 12 de las 14 funciones afectadas.

**La excepcion es `Effect_Init`, que tiene 0 paired-singles y 2 avisos**, y ahi el
aviso SI dice algo (seccion 4).

### (b) Los nombres de miembro del DWARF pueden salir DESPLAZADOS -- medido

En `Tire::UpdateLoaded` Ghidra imprime `this->mSlip = fwd_vel`, y es **falso**.
El listado y el volcado DWARF del original:

```
  8024b360  d3ff00f0  stfs f31,0xf0(r31)      <- offset 0xF0
symbols/mw_dwarfdump.nothpp, struct SuspensionTraffic::Tire:
  float mSlip;      // offset 0xE4
  float mRoadSpeed; // offset 0xF0     <-- este es
```

Nuestra fuente (`this->mRoadSpeed = fwd_vel;`) es la correcta y la de Ghidra esta
corrida. Mas puntos del mismo caso, todos verificados contra el listado:

| offset | DWARF (y nuestra fuente) | lo que imprime Ghidra |
|---|---|---|
| 0xC4 / 0xD0 / 0xD4 | `mRadius` / `mAV` / `mLoad` | iguales -- **correcto** |
| 0xF0 | `mRoadSpeed` | `mSlip` |
| 0xF8 (`lwz r10,0xf8`) | `mBrakes` | `mWheelIndex` |
| 0x100 (`lwz r11,0x100`) | `mAxleIndex` | `mSpecs` |
| 0x104 | `mSlipping` | `mBrakes` |

O sea: los miembros de la clase base salen bien y los de la derivada salen
corridos. **Regla de uso: un nombre de miembro solo vale si el offset del
listado lo confirma.** En `RoadblockFlow::Setup` (mismo fichero de salida) los
nombres SI son exactos -- `mFlags` 0x24, `mSpikeOffset` 0x28, `mNumBlocks` 0x30 --,
asi que el defecto no es universal y no se puede dar por bueno ni por malo sin
mirar.

Lo mismo con los tipos: en `epCalculate`, `light_material->PlatInfo` sale tipado
como `bNode *` y los 12 campos del `eLightMaterialPlatInfo` como `pbVar40[7].Next`;
y el `eDynamicLightContext` de nuestra fuente sale como `light_context[0x31].Type`
porque el DWARF-1 no describe la conversion. Los offsets siguen siendo legibles.

### (c) La regla de la fase 1 sigue en pie

El decompilador **normaliza la forma booleana**: sus `&&` y `||` no prueban nada
por si solos (ver `docs/analisis/r63-decomp15.md` seccion 0, el `or.` de siete
terminos de `IdctColumn` partido en siete `&&`). Por eso se entrega el `.asm`.
Lo que si es de fiar: **el numero de instrucciones de llamada**, **el orden de
los stores** y **la posicion fisica de un bloque**.

## 3. El barrido que si da un resultado duro: contar llamadas

Conteo de sitios de llamada en el decompilado contra los de nuestra fuente, en
las funciones donde algun callee aparece mas de una vez. **De todo el barrido
sale UNA sola diferencia.**

| funcion | callee | original | nuestra fuente | |
|---|---|---|---|---|
| `Update__8ICEMoverf` | `Cubic1D::GetVal` | 7 | 7 (+1 de `Cubic3D::GetVal`, otro simbolo) | igual |
| | `bMulMatrix`/`bCross`/`PSMTX44Identity`/`GetContext`/`GetFOV` | 3/2/2/3/2 | 3 `ICE::MulMatrix`, 2 `ICE::Cross`, 2 `ICE::Identity`, 3/2 | igual |
| `Initialize` DynamicLoader | `strlen`/`strcpy`/`strcmp` | 3/2/2 | 3/2/2 | igual |
| `HolePunchAvoidables` | `v3dotprod`/`v3scale` | 3/2 | 3 `UMath::Dot`, 2 `UMath::Scale` | igual |
| `DefragmentPool` | `bFree`/`bMalloc`/`bMemSet`/`bGetMallocSize`/`bGetMallocName`/`FreeDefragmentStorage` | 3/2/2/2/2/2 | 3/2/2/2/2/2 | igual |
| `SimThread_Step` | `SIControlSteering` | 2 | 2 | igual |
| `UpdateWheelYRenderOffset` | `RideInfo::GetPart` | 3 | 3 | igual |
| `Setup` RoadblockFlow | `GetRandomCop`/`GetRoadblock`/`bRandom`/`RBWarning`/`IsCopSpeechPlaying`/`RBUpdate` | 3/2/2/2/2/2 | 3/2/2/2/2/2 | igual |
| **`RenderFlaresOnCar`** | **`eRenderLightFlare`** | **2** | **3** | **DIFERENCIA** |

### La diferencia: `RenderFlaresOnCar` (802cb51c, 2908 B, zWorld, 98,62 %)

Los `bl` del cuerpo, contados sobre el listado:

```
5 x 0x802cb3d8 (coplightflicker2)   2 x 0x800fb468   2 x 0x800fa59c
1 x cada uno de: 0x80329e5c 0x802cb344 0x802cb15c 0x802782fc 0x800fe3e8
                 0x800fb704 0x800fb648 0x800fb3e0 0x8005ceec
```

Dos sitios de llamada a `eRenderLightFlare`, no tres. Y el decompilado dice
exactamente donde se fueron los otros:

```c
    bVar14 = (renderFlareFlags & 1U) == 0;           /* una sola vez, fuera del bucle */
    ...
            if (bVar3) {                              /* bVar3 = (reflexion == 0) */
              eRenderLightFlare(view,light_flare,l_w,intensity_scale,REF_NONE,(uint)!bVar14,0.0,0,
                                sizescale);
            }
            else {
              eRenderLightFlare(view,light_flare,l_w,intensity_scale,REF_TOPO,FLARE_REFLECT,
                                this->mCar_elevation,0,1.0);
            }
```

Nuestra fuente (`CarRender.cpp:2982-2991`) parte la primera en dos:

```c
                if ((renderFlareFlags & 1) != 0) {
                    eRenderLightFlare(view, light_flare, local_world, intensity, REF_NONE, FLARE_ENV,  lbl_8040AD04, 0, sizescale);
                } else {
                    eRenderLightFlare(view, light_flare, local_world, intensity, REF_NONE, FLARE_NORM, lbl_8040AD04, 0, sizescale);
                }
```

El original NO tiene ese `if`: **el tipo de flare es el sexto argumento
calculado**. Con `FLARE_NORM = 0` y `FLARE_ENV = 1` (`eLight.hpp:33-34`),
`(uint)!bVar14` es exactamente `(renderFlareFlags & 1) != 0`. Receta:

```c
                eRenderLightFlare(view, light_flare, local_world, intensity, REF_NONE,
                                  static_cast<eFlareType>((renderFlareFlags & 1) != 0),
                                  lbl_8040AD04, 0, sizescale);
```

Es la misma familia que el hallazgo de `VP6_PredictFilteredBlock` de la fase 1
(cuatro llamadas a `FilterBlock` fundidas en una) y descansa en la misma medida
dura: el numero de instrucciones `bl`. **No probada** -- compilar exige tocar
`src/`, prohibido en esta tanda.

Ojo con lo que NO es diferencia: el original tambien usa `(renderFlareFlags & 1)`
en el otro sitio (`if (!bVar14) { ... sizescale = 10.0; }`, nuestra linea 2915)
y lo calcula UNA vez en un registro. Eso es CSE del compilador, no forma.

## 4. `Effect_Init` (80320840, 276 B, steering, 88,62 %): el peor porcentaje de las que nadie habia mirado

Tres cosas, las tres leidas del listado.

**1) El orden de los dos `sthx` esta al reves que en nuestra fuente.** El bucle
de tablas del original termina asi:

```
  80320920  7c1df32e  sthx r0,r29,r30     <- r29 = 0x804B9F7C
  80320924  d8010010  stfd f0,0x10(r1)
  80320928  80010014  lwz r0,0x14(r1)
  8032092c  7c1cf32e  sthx r0,r28,r30     <- r28 = 0x804B9EFC
```

y `config/GOWE69/symbols.txt:37517-37518` dice, sin pasar por Ghidra:

```
g_iQuarterSineTable = .bss:0x804B9EFC;
g_iRampUpTable      = .bss:0x804B9F7C;
```

**El original escribe primero la RAMPA y despues el SENO.** Nuestra fuente
(`steering.c:1032-1035`) escribe primero el seno y despues la rampa. Candidato a
receta: intercambiar las dos sentencias. Cautela honesta: el store del seno
depende de `sin()` + `fctiwz` + ida y vuelta por pila, asi que un planificador
podria hundirlo solo; pero el orden de los stores es, de todo lo que imprime
Ghidra, lo que menos se inventa. El barrido de la r48 anotado en la cabecera del
fichero dice "swap 1024*sin neutro", que no es este intercambio.

**2) El bucle de cola de bytes SI se emite, y confirma nuestra forma.** Los 2
avisos de `unreachable` de esta funcion son 80320894 y 80320898, que son:

```
  80320888  54a507bf  rlwinm. r5,r5,0x0,0x1e,0x1f   <- n & 3, con r5 = 0x40
  8032088c  38640003  addi r3,r4,0x3
  80320890  41820014  beq 0x803208a4
  80320894  38000000  li r0,0x0
  80320898  34a5ffff  subic. r5,r5,0x1
  8032089c  9c030001  stbu r0,0x1(r3)
```

MWCC **no pliega** `sizeof(EFFECT) & 3` aunque cargue el tamano con un
`li r5,0x40` cinco instrucciones antes: emite la mascara, el salto y el bucle.
Ghidra si lo pliega, y por eso avisa. Nuestra fuente tiene ese bucle (`n = n & 3;
q = (u8*)p + 3; if (n) do { *++q = 0; } while (--n);`) y **hace bien en tenerlo**.

**3) El `asm { li clear_value, 0 }` reproduce una instruccion real** (`li r0,0x0`
en 80320858) y el bucle de palabras casa entero, incluido el `stwu` del octavo
almacenamiento (`puVar4 = puVar4 + 8; *puVar4 = 0;` en el decompilado =
`stwu r0,0x20(r4)` en 80320880).

## 5. Fichas: las 28, una a una

Formato: direccion, tamano, unidad, porcentaje de `build/GOWE69/report.json`
(del build del 9-sep 21:54; con nueve agentes en el arbol puede haber cambiado),
fuente nuestra, y el veredicto de forma. El C completo esta en
`docs/analisis/r63-ghidra-27-decomp.c`; el listado, en `-asm.txt`.

### 8007c390 `ICEMover::Update` -- 3868 B, zCamera, 99,91 %, 12 ps
`src/Speed/Indep/Src/Camera/ICE/ICEMover.cpp:519`. **Forma confirmada, y el
conteo de llamadas casa una a una**: 7 `Cubic1D::GetVal` + 1 `Cubic3D::GetVal`
(el octavo `GetVal(` de nuestra fuente, el de `pAccelOffset`, es la otra
sobrecarga y sale como `Cubic3D__GetVal`), 3 `bMulMatrix` = las 3 `ICE::MulMatrix`
(640, 643, 692), 2 `bCross` = las 2 `ICE::Cross` (612, 614), 2 `PSMTX44Identity`
= las 2 `ICE::Identity` (541, 606), 3 `ICETrack::GetContext`, 2 `GetFOV`. Y
`CreateLookAtMatrix` **no esta inlineada** en el original: es una llamada, como en
nuestra fuente. Sin andamios `asm` hoy. Es la funcion mas grande de la lista y la
que menos le queda: no hay frente de forma aqui.

### 8008095c `__static_initialization_and_destruction_0` -- 3604 B, zCamera, 97,11 %, 18 ps
Generada por el compilador: no tiene fuente propia, la decide **el orden de
emision de la unidad**. El decompilado es, literalmente, la lista ordenada de
todo lo que zCamera inicializa en arranque: los miembros de cada `CameraNoise*`
con sus valores (`CameraNoiseHandheldFrequency.x = 0.213`, `.w = 0.192`,
`.y = 0.175`, ...), los 20 de `CameraNoiseSpeedData[0..4]`, el registro del
subsistema (`_Physics_System_CameraAI.mInit = Init`, `.mRestore = Shutdown`,
`.mSig.mCRC = stringhash32("CameraAI")`, `.mNext = _Q23Sim9SubSystem_mHead`,
`.mName = "CameraAI"`), la `Vector<IPlayer*,16>` de la `Director` con su
`OnGrowRequest` y su `push_back`, 12 `bStringHash`, 10 `SetMinMax` y 8
`stringhash32`. **Quien trabaje zCamera tiene aqui el orden de referencia
sentencia a sentencia**; eso es lo que le falta al 2,89 % que resta.

### 802feab0 `WRoadNav::HolePunchAvoidables` -- 2980 B, zWorld2, 99,97 %, 36 ps
`WRoadNetwork.cpp:1708`. **Forma confirmada** (3 `VU0_v3dotprod` = las 3
`UMath::Dot`, 2 `VU0_v3scale` = las 2 `UMath::Scale`; los 2 `_IList::Find` del
decompilado salen de una inline de `IHandle`, no hay con que compararlos por
texto). Andamios vivos: `register float wld_cutx
asm("fr11")` (linea 1968) y `asm("" : "+f"(offset_change))` (2151). Al 99,97 % lo
que queda no es forma.

### 802cb51c `CarRenderInfo::RenderFlaresOnCar` -- 2908 B, zWorld, 98,62 %, 28 ps
`CarRender.cpp:2689`. **DIFERENCIA REAL, seccion 3**: 3 sitios de
`eRenderLightFlare` contra 2 del original. Sin andamios `asm` en el cuerpo.

### 8008f098 `EAGL4::DynamicLoader::Initialize` -- 2352 B, zEagl4Anim, 98,88 %, 0 ps
`eagl4supportdlopen.cpp:408`. **Forma confirmada** (3 `strlen`, 2 `strcpy`, 2
`strcmp`, iguales), y la fase 1 ya habia confirmado la lista de `case` por los
pivotes 0x70000005/7. Sin andamios.

### 80109358 `epCalculateLocalDirectionalPOS16` -- 2072 B, zEcstasy, 93,31 %, 185 ps
`EcstasyEx.cpp:3927`. La funcion con mas paired-singles del juego, y **sale
entera y legible**: 518 instrucciones, prototipo completo del DWARF con los diez
nombres de parametro. Comprobado contra nuestra fuente:

- **El primer `switch` (el de `data_stride_adjust`) casa exactamente**: mismos
  16 `case`, misma particion (`{0,2,8,0xA,0x10,0x12,0x18,0x1A}` -> 1 y
  `{4,6,0xC,0xE,0x14,0x16,0x1C,0x1E}` -> 2) y arbol de decision compatible
  (pivotes 0xe, 6, 2, 0, 4, 10, 0xc, 8, 0x16, 0x12, 0x10, 0x14, 0x1a, 0x18,
  0x1c, 0x1e).
- **El segundo `switch` (el de por vertice) tiene 9 `case` en el original**:
  `{0, 4, 0xC, 0xE, 0x14, 0x1C, 0x1E, 0x20, 0x3C}`. Nuestra fuente tiene esos 9
  **mas un `case -1: break;`** (linea 4117), que es un andamio de forma del arbol
  (familia "la etiqueta que abre un bloque vacio"). En el arbol del original no
  hay ninguna comparacion contra -1; no prueba que sobre --un `case` vacio en el
  extremo del rango puede mover el pivote sin emitir comparacion propia-- pero
  quien lo puso deberia remedirlo.
- Sin andamios `asm` en el cuerpo. El diagnostico vivo del fichero (la ranura
  huerfana de 8 B de `alter_reg`, r54) no lo toca nada de esto.

### 80109bb8 `eLightMaterialPlatInterface::UpdatePlatInfo` -- 2044 B, zEcstasy, 99,33 %, 36 ps
`eLightE.cpp:437`. Sale entera (511 instrucciones). **Cinco andamios vivos**: el
pin `register float envmap_min_scale asm("fr5")` (501), tres
`__asm__("" : "+f"(envmap_power))` seguidos (535-537) y un `__asm__("")` (624).
Al 99,33 % el residuo es reparto; no hay diferencia de forma a la vista.

### 8026c574 `ActualReadJoystickData` -- 1588 B, zPlatform, 99,32 % (CERRADA en r61), 14 ps
`JoyE.cpp:177`. Va de control: cerrada, y sale al 100 % de cobertura con 397
instrucciones. Siete andamios vivos (`register int guard asm("r8")` y seis
`asm("" : "+r"(guard))`), que son los que la cerraron.

### 800d80fc `CARSFX_RoadNoise::GenerateRoadNoise` -- 1240 B, zEAXSound2, 97,71 %, 8 ps
`CARSFX_Roadnoise.cpp:448`. **Forma confirmada, y confirma ademas el diagnostico
r50 del propio fichero**: en el decompilado la acumulacion
`fVar5 = fVar5 * 0.1 + fVar5` (el `slipBoost`) aparece **dentro del preambulo del
`bLength` izquierdo**, no detras -- que es exactamente donde la fuente la puso a
proposito. Lo demas casa: `speed` calculado una vez y usado dos, los cinco
`bLength` con su Newton-Raphson, los clamps en el mismo orden, y los cuatro
stores finales en el orden LTVol, RTVol, LTPitch, RTPitch. Un andamio vivo:
`register float slipBoost asm("fr10")` (480).

### 80075c44 `TrackCarCameraMover::Update` -- 992 B, zCamera, 99,63 %, 6 ps
`TrackCar.cpp:364`. **Forma confirmada en lo grueso**, con tres detalles:

- Las **dos llamadas al constructor de copia** `__8bVector3RC8bVector3` estan en
  el original: nuestras `bVector3 displacement(Eye - ...)` y
  `bVector3 lookdir(Look - Eye)` son la forma buena.
- **`look_offset` se escribe z, x, y** en el original
  (`local_a0.z = TrackCarLookOffsetZ[iVar4]` primero), y nuestra fuente lo
  escribe x, y, z. Es un orden de stores, o sea de los observables fiables, pero
  son tres `stfs` independientes que el planificador puede permutar. Candidato
  barato para quien trabaje zCamera; no probado.
- El `bScale(&hcomp, &hcomp, 0.0f)` muerto tambien esta en el original (los tres
  `local_b0.x/y/z = 0.0`), y el `bClamp(fov, 800, 13100)` sale con los mismos
  limites (0x332c).

Sin andamios `asm` en el cuerpo (los tres `__asm__` del fichero son alias de
simbolo en las declaraciones, no codigo).

### 80321474 `SimThread_Step` -- 924 B, steering, 95,67 %, 0 ps
`steering.c:1497`. Sin DWARF (MWCC). Confirmada por la fase 1 (los dos `&&` son
corto-circuito real, dos saltos cada uno); mi conteo anade que las **2** llamadas
a `SIControlSteering` casan. Sin andamios.

### 802c9fb8 `CarRenderInfo::UpdateWheelYRenderOffset` -- 876 B, zWorld, 99,38 %, 14 ps
`CarRender.cpp:1974`. **Forma confirmada** (3 `RideInfo::GetPart`, y las dos
primeras con el resultado DESCARTADO --`GetPart(...,0x42)` y `(...,0x43)`, cada
una bajo su propio `if (pRideInfo != 0)`-- estan tambien en el original). Sin
andamios.

### 8024b310 `SuspensionTraffic::Tire::UpdateLoaded` -- 856 B, zPhysicsBehaviors, 97,21 %, 6 ps
`SuspensionTraffic.cpp:388`. **Forma confirmada sentencia a sentencia** una vez
corregidos los nombres desplazados (seccion 2b): el `if (mLoad <= 0)` inicial,
los dos bloques de freno con su `if (mAV > 0) bt = -bt`, el `Atan2a`, el
`skid_speed` con `sqrt`, el reparto `mSlipping = true` + division por
`skid_speed`, y el `mAV` final. Sin andamios. Lo unico que Ghidra ensena de mas:
`(-fwd_vel + -fwd_vel)` y `(fVar2 + fVar2)` donde la fuente escribe `* 2`, que es
la misma cosa.

### 8030183c `WRoadNav::InitAtSegment(short,char,float)` -- 816 B, zWorld2, 99,66 %, 6 ps
`WRoadNetwork.cpp:2800`. **Forma confirmada.** Dos detalles del listado que NO
son forma: el original escribe `fDeadEnd` (`stb r0,0x2c0`) **antes** de `fValid`
--planificacion, y la propia fuente documenta que las 12 filas que quedan son un
swap r10<->r11--; y en la rama negativa pone `fNodeInd = flags & 0x40` (un cero
que ya tenia en registro) en vez de `li 0`. Tres andamios vivos: el
`guard asm("r28")` (2923-2924) y `asm("" : "+f"(endOffset) : "r"(guard))` (2934).

### 803598fc `VP6_PredictFilteredBlock` -- 740 B, criticalpath, 94,39 %, 0 ps
`criticalpath.c:1468`. Ya diagnosticada en la fase 1 (UNA llamada a `FilterBlock`
contra nuestras cuatro, polaridad del test de salida, `else` muerto). Mi
exportacion la reproduce igual. Cinco andamios vivos, todos pines de registro
(`OutputPtr` r24, `bp` r23, `TempPtr2` r27, `ModY` r25, `Stride` r26): la receta
de la fase 1 avisa de conservarlos mientras se prueba.

### 802bc0a4 `TrackStreamer::GetLoadingPriority` -- 708 B, zTrack, 97,18 %, 6 ps
`TrackStreamer.cpp:2174`. **Forma confirmada**, incluido lo que la fuente ya
sospechaba:

- `bClamp(angle,20,90)` sale como los dos `if` de resta en el mismo orden;
  `bMin(speed*K,1.0f)` sale con el mismo orden de argumentos (el 1.0 de partida y
  el producto sustituyendolo).
- El arbol de la multiplicacion es el de DOS sentencias:
  `distance * (1 - ((90-clamp)*K1) * speed_factor * K2)`, que es la forma que la
  r61 dejo puesta a pesar de que el porcentaje empeoraba.
- El **temporal de 16 B** que la fuente reconstruyo con el inicializador agregado
  se ve en el decompilado: tras `bNormalize(&local_50,&local_58)` el original
  copia el resultado a DOS sitios (`local_48/local_44` y `local_58.x/.y`), que es
  el `v = bNormalize(v)` por valor.
- `bACos` sale como `0x4000 - bASin(dot)`, o sea inline: no es una llamada de
  menos.

Sin andamios `asm` en el cuerpo.

### 802dfcac `CarLoader::DefragmentPool` -- 684 B, zWorld, 99,27 %, 0 ps
`CarLoader.cpp:2093`. **Forma confirmada** por la fase 1 y por mi conteo de
llamadas (3 `bFree`, 2 `bMalloc`, 2 `bMemSet`, 2 `bGetMallocSize`, 2
`bGetMallocName`, 2 `FreeDefragmentStorage`). Sin andamios.

### 8034bac4 `IdctColumn` -- 632 B, madidct, 50,45 %, 0 ps
`madidct.cpp:22`. **Confirmo la medida de la fase 1 con mi propia exportacion**:
el bloque de salida del camino largo empieza por `dest[56]`.

```
  8034bd08  912f00e0  stw r9,0xe0(r15)     <- dest[56], PRIMERO
  8034bd0c  900f0000  stw r0,0x0(r15)      <- dest[0]
  8034bd10  934f0020  stw r26,0x20(r15)    <- dest[8] ... hasta dest[48]
```

Y el camino corto (todo cero) si va en orden 0x0, 0x20, ..., 0xe0. La receta de
la fase 1 (subir `dest[56] = t6 - t4;` a la primera escritura del bloque) sigue
en pie, sin probar. Sin andamios.

### 802a2248 `Speech::RoadblockFlow::Setup` -- 596 B, zSpeech, 98,25 %, 0 ps
`RoadblockFlow.cpp:241`. **Forma confirmada**, y con dos regalos del DWARF:

- Los `||` son reales (hay llamada en el segundo operando):
  `Manager__IsCopSpeechPlaying(kSPCH1_EventID_DispRBReply) || ...DispRBUpdate`.
  **Los dos `static_cast<SPCHType_1_EventID>(0x4d)` y `(0x4e)` de nuestra fuente
  tienen nombre real**: `kSPCH1_EventID_DispRBReply` y
  `kSPCH1_EventID_DispRBUpdate`.
- Las **dos** llamadas a `GetRoadblock()` de nuestra fuente estan en el original,
  y el tercer argumento de `RBUpdate` es un `char` (`'\0'` / `'\x01'`).

Tres andamios vivos: `__asm__("# a")`, `("# b")`, `("# c")` (250, 258, 277).

### 8031f7fc `HandleTriggers` -- 588 B, steering, 98,30 %, 0 ps
`steering.c:431`. Sin DWARF. Confirmada en la fase 1 (las dos mitades son
identicas, mismo `ble +0xac`). Sin andamios.

### 8034bd3c `IdctRow` -- 516 B, madidct, 46,37 %, 0 ps
`madidct.cpp:294`. **Confirmo la medida de la fase 1**: la primera escritura del
bloque de salida es `dest[7]`.

```
  8034bf0c  9144001c  stw r10,0x1c(r4)     <- dest[7], PRIMERO
  8034bf10  90040000  stw r0,0x0(r4)       <- dest[0] ... y el resto en orden
```

Sin andamios.

### 800a3090 `EAGL4Anim::FnRawStateChan::EvalState` -- 456 B, zEagl4Anim, 98,07 %, 0 ps
`RawStateChan.cpp:222`. **Forma confirmada, y confirmado el fallo del juego**: en
el decompilado del original aparece `if ((float)puVar5 <= time)`, o sea el
puntero comparado como `float` sin deref -- que es lo que nuestra fuente escribe
con `*reinterpret_cast<float*>(&k)`. No es una errata nuestra; **dejarlo**. Sin
andamios.

### 80321330 `SimThread_Init` -- 324 B, steering, 94,69 %, 0 ps
`steering.c:1414`. Sin DWARF. Confirmada 1 a 1 en la fase 1. Sin andamios.

### 802dd360 `CarLoader::SetMemoryPoolSize` -- 304 B, zWorld, 97,37 %, 0 ps
`CarLoader.cpp:554`. **Forma confirmada 1 a 1**, incluido el `return` temprano si
la `LoadedRideInfoList` no quedo vacia y el orden
`FlushHibernatingSections` -> `MakeSpaceInPool` -> `bMalloc` ->
`bGetFreeMemoryPoolNum` -> `bInitMemoryPool(..., "Cars")`. Sin andamios.

### 80320840 `Effect_Init` -- 276 B, steering, 88,62 %, 0 ps
Seccion 4: **el orden de los dos stores de tabla esta al reves**. Sin DWARF
(MWCC). Tres andamios vivos, los tres `asm { ... }` de MWCC (999, 1025, 1031).

### 8010ac40 `eProject` -- 268 B, zEcstasy, 93,97 %, 0 ps
`eMathE.cpp:216`. **Confirmo la medida de la fase 1 con listado**: el original
**no tiene salida temprana**; escribe `*sz` UNA vez en el punto de union
(`b 0x8010ad34`) con `-2.0f` ya cargado desde la rama. Y el orden de la rama nula
es `*sy` antes que `*sx` (`stfs f13,0x0(r28)` con r28 = arg 8 = `sy`, y luego
r29 = arg 7 = `sx`), que es como ya lo tiene nuestra fuente. Un andamio vivo:
`__asm__("" : "+f"(halfVP2))` (241) -- la receta de la fase 1 pide remedirlo al
mover el store.

### 8031fa48 `CookValues` -- 260 B, steering, 95,77 %, 0 ps
`steering.c:469`. Sin DWARF. Diagnosticada en la fase 1 (la rama del maximo relee
el miembro: `extsb` sobre el byte almacenado). Un andamio vivo:
`asm { extsb _mx, _bits }` (487), que es justo lo que la receta propone quitar.

### 803210b8 `Effect_PerformEnvelope` -- 196 B, steering, 93,27 %, 0 ps
`steering.c:1267`. Sin DWARF. Confirmada en la fase 1, con candidato de pines
(`mag` en r9 y `out` en r5). Sin andamios.

## 6. Andamios `asm` vivos hoy, por funcion

Censo con `scratchpad/ghidra63/both/andamios.py`, que delimita el cuerpo de cada
funcion y descarta comentarios (un `grep asm` sobre estos ficheros cuenta
cientos de menciones que estan dentro de comentarios).

| funcion | andamios | cuales |
|---|---|---|
| `ActualReadJoystickData` | 7 | `guard asm("r8")` + 6 `asm("":"+r"(guard))` |
| `UpdatePlatInfo` | 5 | pin `fr5` + 3 `asm("":"+f"(envmap_power))` + `asm("")` |
| `VP6_PredictFilteredBlock` | 5 | 5 pines de registro (r23..r27) |
| `Setup` RoadblockFlow | 3 | `# a`, `# b`, `# c` |
| `InitAtSegment` | 3 | `guard asm("r28")` + 2 barreras |
| `Effect_Init` | 3 | `asm { li }`, `asm { lis }`, `asm { fsubs }` (MWCC) |
| `HolePunchAvoidables` | 2 | pin `fr11` + `asm("":"+f"(offset_change))` |
| `GenerateRoadNoise` | 1 | pin `fr10` (`slipBoost`) |
| `eProject` | 1 | `asm("":"+f"(halfVP2))` |
| `CookValues` | 1 | `asm { extsb }` (MWCC) |
| las otras 17 | 0 | -- |

(`epCalculate` no tiene `asm`, pero si un `case -1: break;` que es un andamio de
otra clase; ver su ficha.)

## 7. Limitaciones

1. **Ninguna receta esta probada.** Compilar exige tocar `src/`, prohibido en
   esta tanda. Las dos nuevas (`RenderFlaresOnCar` y `Effect_Init`) son hipotesis
   con evidencia dura --conteo de `bl` y orden de stores--, no resultados
   medidos. Quien las aplique tiene que medir con objdiff y con el DOL.
2. **No he verificado la semantica del p-code de la cuantizacion.** Igual que el
   agente `gekko`: se que decodifica y que los operandos son correctos, no que
   reproduzca el Gekko. Si una funcion con mucha cuantizacion no casa, sospechar
   de ahi antes que de la fuente.
3. **Los nombres de miembro del DWARF salen desplazados en al menos una clase
   derivada** (`Tire`, seccion 2b). No he auditado en cuantas mas pasa: solo
   comprobe `Tire` (mal) y `RoadblockFlow` (bien). Cualquier afirmacion sobre un
   nombre de miembro necesita el offset del listado.
4. **Los porcentajes** salen de `build/GOWE69/report.json` del 9-sep 21:54. Con
   nueve agentes en el arbol pueden estar rancios. Los tamanos en bytes si estan
   verificados: Ghidra cubre el 100 % de cada rango del encargo.
5. **`GQR0 = 0` es una suposicion medida para ESTE binario** (5 escrituras, todas
   cero, medidas en la fase 1). Los otros GQR no se tocan.
6. No he auditado los 54.930 mensajes truncados del `MessageLog` del importador
   DWARF (`Unexpected child of class type`, `Failed to process debug info entry`),
   ni las 126 funciones que la fase 1 dijo que se pierden por
   `OverlappingFunctionException`.
7. El barrido de conteo de llamadas solo mira callees que aparecen **mas de una
   vez** en el decompilado. Una llamada de mas o de menos con multiplicidad 1 se
   me escapa.
8. No toque `src/`, `config/`, `configure.py` ni `build/`; tampoco
   `ghidra_project/`. Lo que escribi: `docs/analisis/r63-ghidra-27*`,
   `ghidra_scripts/{SetGqr,Dwarf1Run,ExportOpen}.java` y
   `scratchpad/ghidra63/both/`.

## 8. Para quien siga

- **El proyecto ya montado esta en
  `C:\Users\jferr\AppData\Local\Temp\ghidra_r63both\NFSMW`** (Ghidra 11.4 DEV,
  lenguaje Gekko_Broadway, auto-analisis hecho, DWARF importado y guardado,
  GQR0=0). Sacar cualquier otra funcion cuesta cambiar una linea de
  `lista27.csv` y volver a lanzar `export.bat`: menos de un minuto. **Un headless
  por proyecto**: no lanzar dos a la vez sobre el mismo `.rep`.
- **Las dos recetas nuevas**, por relacion valor/coste:
  1. `RenderFlaresOnCar` (2908 B, 98,62 %): fundir las dos llamadas a
     `eRenderLightFlare` de la rama `REF_NONE` en una sola con el tipo de flare
     como argumento calculado.
  2. `Effect_Init` (276 B, 88,62 %): intercambiar el store de la rampa y el del
     seno en el bucle de tablas.
- **Las cuatro de la fase 1 siguen sin probar** (`VP6`, `IdctColumn`/`IdctRow`,
  `eProject`, `CookValues`) y esta exportacion **confirma las tres primeras con
  listado propio**.
- **Lo que ya no hay que volver a barrer**: forma de fuente en `ICEMover`,
  `HolePunchAvoidables`, `DynamicLoader::Initialize`, `GenerateRoadNoise`,
  `UpdateLoaded`, `InitAtSegment`, `GetLoadingPriority`, `UpdateWheelYRenderOffset`,
  `DefragmentPool`, `SetMemoryPoolSize`, `Setup` RoadblockFlow, `EvalState` y
  `TrackCarCameraMover::Update` (salvo el orden z,x,y de `look_offset`), mas el
  arbol de `switch` de `epCalculate` (salvo el `case -1`). En todas ellas el
  conteo de llamadas, el arbol de `switch` o las sentencias casan; lo que falta
  es reparto, planificacion o colocacion.
- **La palanca que queda sin explotar** en `zCamera`: el
  `__static_initialization_and_destruction_0` decompilado es la lista ordenada de
  la inicializacion entera de la unidad. Es el unico sitio donde el orden de
  emision del original esta escrito de corrido.
