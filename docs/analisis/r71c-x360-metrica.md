# r71c: METRICA X360 — cuanto de nuestro arbol compila el cl.exe del XDK

Sonda ejecutada el 2026-09-14 con `build/compilers/X360/14.00.2110/cl.exe`
(Version 14.00 2110 for PowerPC, era del prototipo EUROPEGERMILESTONE).
Continuacion de `r71c-x360-sonda.md`.

## Resumen

| clase | n | % | significado |
|---|---|---|---|
| **OK** | **397** | **46.3** | produce .obj (8.411.963 B de .obj en total) |
| GCCASM | 82 | 9.6 | falla por asm de GCC: pins `register X asm("rN")`, barreras `__asm__("" : ...)`, alias `__asm__("simbolo")`, pools `asm(".section .rodata")` |
| PLATAFORMA | 61 | 7.1 | include de dolphinsdk / rama GC sin alternativa XENON |
| OTRO | 317 | 37.0 | error C++ real del arbol (ver desglose: 2 causas dominan) |
| SHIM | 0 | 0 | ninguna cabecera estandar falta ya (XDK in-tree + shims) |
| **TOTAL** | **857** | | los 857 .cpp internos que las 34 SourceLists incluyen |

Lectura: **el 46% del arbol compila tal cual para X360**. Otro 10% falla
EXACTAMENTE por los andamios GCC del puerto (lista abajo). El resto se reparte
entre codigo GC genuino (7%) y dos inconsistencias del arbol que el unity build
esconde (37%, una sola causa raiz absorbe 202 de los 317).

## Como se compilo (lo que hace falta para reproducir)

Comando equivalente (flags con GUION; los `/flags` los destroza el MSYS):

```
build/compilers/X360/14.00.2110/cl.exe -c -nologo -O2 -GR- -EHsc \
  -D_XBOX -DXBOX -D_WIN32 -DEA_PLATFORM_XENON -D_USE_MATH_DEFINES -DNDEBUG \
  -DBUILD_VERSION=EUROPEGERMILESTONE -DVERSION_EUROPEGERMILESTONE \
  -DLUA_NUMBER=float -DMILESTONE_OPT -DDEFAULT_ALLOCATOR=0 \
  -I"src/Packages/xenonsdk/2.0.2135.2/installed/include/xbox" -I"src/Packages" -I"src" \
  -I"src/Speed/Indep/Libs/allocator/1.5.0" -I"src/Speed/Indep/Libs/csis/dev/include" \
  -I"src/Packages/eathread/1.1.0/include" -I"src/Speed/Indep/Libs/snd/9/include" \
  -I"src/Speed/Indep/Libs/spch/dev/include" -I"src/Speed/Indep/Libs/path/5.01.04/include" \
  -I"tools/scratch/x360inc" -I"src/Speed/Indep/bWare/Inc" \
  -FI<abs>\tools\scratch\x360inc\prelude.h \
  -Tp<fichero.cpp> -Fo<salida.obj>
```

Hallazgos clave del bootstrap:

1. **El XDK completo YA esta en el arbol**: `src/Packages/xenonsdk/2.0.2135.2/
   installed/include/xbox/` (213 ficheros: CRT completa, `ppcintrinsics.h`,
   `new`, STL...). La "alternativa real" que pedia r71c-x360-sonda.md ya no es
   necesaria: con ese `-I` los shims CRT previos quedan como fallback y
   **SHIM = 0**.
2. **`-Tp` no `-Tc`**: `-Tc` compila como C (`bool`/`extern "C"` explotan en
   `types.h:66` — el "muro" de la sesion anterior era esto, no un typedef).
3. **`-FI` exige ruta absoluta Windows** (`C:\...\prelude.h`); relativa falla.
4. **`-Fo` tambien**: ruta `/tmp/...` de MSYS acaba en `C:\tmp` silenciosamente.
   Los .obj van a `%TEMP%` real.
5. `types.h` ya tiene rama `EA_PLATFORM_XENON` completa (`__int16`, `Bool`...):
   no hizo falta `x360plat.h`.
6. Defines del build X360 originales (sacados de `configure.py` seccion X360):
   `LUA_NUMBER=float MILESTONE_OPT DEFAULT_ALLOCATOR=0` — sin ellos, ~10
   ficheros fallan de mas.

### prelude.h (tools/scratch/x360inc/prelude.h)

Solo declaraciones: `#include <new>` (bWare.hpp llama `::operator new[]`
cualificado y MSVC exige declaracion previa), `#define __attribute__(x)`
(GCC-ism de sintaxis), fallback `nullptr 0` (cl.exe 14.00 es C++03; types.h
hace el mismo fallback) y `zscaf.h`.

### zscaf.h (generado por tools/scratch/x360_sonda/gen_zscaf.py)

Las SourceLists son unity builds: declaran `extern const float _zmfB8
asm("$LC2151631152");` y `#define ZMISC_POOL(off) (_bwarePrefix + (off))`
antes de cada `#include`. La sonda compila los .cpp internos FUERA del unity,
asi que zscaf.h re-emite **239 externs + macros de andamio sin la clausula
`asm()`** (el alias original queda en comentario). Es la unica forma de que el
fichero interno vea los mismos simbolos que ve en el unity.

## Tabla por unidad

| unidad | n | OK | GCCASM | PLATAFORMA | SHIM | SCAFDATA | OTRO | FALTA | TIMEOUT | bytes .obj |
|---|---|---|---|---|---|---|---|---|---|---|
| zAI | 36 | 21 | 0 | 1 | 0 | 0 | 14 | 0 | 0 | 849551 |
| zAnim | 23 | 7 | 8 | 0 | 0 | 0 | 8 | 0 | 0 | 48647 |
| zAttribSys | 8 | 6 | 0 | 0 | 0 | 0 | 2 | 0 | 0 | 320130 |
| zBWare | 15 | 4 | 2 | 0 | 0 | 0 | 9 | 0 | 0 | 26832 |
| zCamera | 28 | 5 | 1 | 19 | 0 | 0 | 3 | 0 | 0 | 26291 |
| zDebug | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 4954 |
| zDynamics | 3 | 0 | 2 | 0 | 0 | 0 | 1 | 0 | 0 | 0 |
| zEAXSound | 35 | 12 | 2 | 1 | 0 | 0 | 20 | 0 | 0 | 175946 |
| zEAXSound2 | 68 | 18 | 5 | 0 | 0 | 0 | 45 | 0 | 0 | 154645 |
| zEagl4Anim | 40 | 29 | 5 | 0 | 0 | 0 | 6 | 0 | 0 | 228612 |
| zEcstasy | 23 | 4 | 2 | 10 | 0 | 0 | 7 | 0 | 0 | 24965 |
| zFEng | 31 | 18 | 10 | 0 | 0 | 0 | 3 | 0 | 0 | 183172 |
| zFe | 57 | 10 | 5 | 5 | 0 | 0 | 37 | 0 | 0 | 91589 |
| zFe2 | 71 | 11 | 2 | 0 | 0 | 0 | 58 | 0 | 0 | 169221 |
| zFeOverlay | 17 | 1 | 0 | 3 | 0 | 0 | 13 | 0 | 0 | 719 |
| zFoundation | 16 | 5 | 6 | 0 | 0 | 0 | 5 | 0 | 0 | 108867 |
| zGameModes | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 7193 |
| zGameplay | 17 | 10 | 2 | 0 | 0 | 0 | 5 | 0 | 0 | 250222 |
| zLua | 5 | 2 | 2 | 0 | 0 | 0 | 1 | 0 | 0 | 250370 |
| zMain | 176 | 126 | 4 | 1 | 0 | 0 | 45 | 0 | 0 | 1611886 |
| zMisc | 29 | 23 | 0 | 0 | 0 | 0 | 5 | 0 | 0 | 492878 |
| zMiscSmall | 2 | 2 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 8964 |
| zMission | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| zPhysics | 14 | 8 | 2 | 0 | 0 | 0 | 4 | 0 | 0 | 413190 |
| zPhysicsBehaviors | 30 | 24 | 0 | 1 | 0 | 0 | 5 | 0 | 0 | 1792051 |
| zPlatform | 18 | 4 | 0 | 14 | 0 | 0 | 0 | 0 | 0 | 100699 |
| zRender | 3 | 3 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 8360 |
| zSim | 16 | 10 | 1 | 0 | 0 | 0 | 5 | 0 | 0 | 335272 |
| zSpeech | 17 | 4 | 11 | 0 | 0 | 0 | 2 | 0 | 0 | 5171 |
| zTrack | 12 | 6 | 3 | 2 | 0 | 0 | 1 | 0 | 0 | 151082 |
| zWorld | 29 | 11 | 4 | 4 | 0 | 0 | 10 | 0 | 0 | 162383 |
| zWorld2 | 17 | 11 | 2 | 0 | 0 | 0 | 4 | 0 | 0 | 408101 |
| **TOTAL** | **857** | **397** | **82** | **61** | **0** | **0** | **317** | **0** | **0** | |

(El tamanio de .obj NO es comparable con el .text X360 todavia: falta el
decodificador .pdata para aparear funciones; es solo indicador de volumen.)

## LA LISTA: 82 ficheros GCCASM con la linea que falla

El andamio que el puerto X360 no traga. Patrones: `register X asm("rN"/"frN")`
(pins), `__asm__("" : ...)` (barreras), `extern ... __asm__("simbolo")`
(alias a mangled GCC), `asm(".section .rodata/.data")` (pools de literales y
hand-pools), `__asm__("memset")` (redirecciones de libc).

- `Libs/Support/Miscellaneous/Carp.cpp:50` — asm(
- `Libs/Support/Utility/UBezierLite.cpp:35` — asm(
- `Libs/Support/Utility/UEALibs.cpp:34` — asm(
- `Libs/Support/Utility/UFoundationBody.cpp:9` — asm(
- `Libs/Support/Utility/UMath.cpp:35` — asm(
- `Libs/Support/Utility/USpline.cpp:96` — asm(
- `Src/Animation/AnimCandidates.cpp:51` — asm(
- `Src/Animation/AnimCtrl.cpp:148` — asm(
- `Src/Animation/AnimEntity_WorldEntity.cpp:108` — asm(
- `Src/Animation/AnimLocator.cpp:25` — asm(
- `Src/Animation/AnimPlayer.cpp:76` — asm(
- `Src/Animation/AnimScene.cpp:28` — asm(
- `Src/Animation/WorldAnimCtrl.cpp:10` — asm(
- `Src/Animation/WorldAnimInstanceDirectory.cpp:18` — asm(
- `Src/Camera/ChaseCamAI.cpp:8` — asm(".section \".data\"\n"
- `Src/EAGL4Anim/DeltaChan.cpp:553` — char gapAnteQt0[8] asm("gap_07_8045B1B8_bss");
- `Src/EAGL4Anim/FnStatelessQ.cpp:101` — __asm__("subi %0,%1,1" : "=r"(lastKey) : "r"(statelessQ->mNumKeys));
- `Src/EAGL4Anim/MemoryPoolManager.cpp:118` — register char *blockStorage asm("r8") = gMemoryPoolFree;
- `Src/EAGL4Anim/RawStateChan.cpp:314` — register int keyOffset asm("r9") = i * c->GetKeySize();
- `Src/EAGL4Anim/eagl4supportdlopen.cpp:93` — int lbl_80417104 __asm__("lbl_80417104") = 1;
- `Src/EAXSound/CARSFX/CARSFX_WindNoise.cpp:337` — __asm__("");
- `Src/EAXSound/CARSFX/SFXObj_WorldObject.cpp:337` — __asm__("");
- `Src/EAXSound/Dynamic_Mixer/NFSMixMapState.cpp:197` — register int offset asm("r0");
- `Src/EAXSound/EaxSoundTypes.cpp:37` — extern EventHistory gManagerGlobalHistory __asm__("_Q26Speech7Manager.mGlobalHistory");
- `Src/EAXSound/Ginsu/ginsudata.cpp:165` — register int currentBlock __asm__("r0");
- `Src/EAXSound/SND_GEN/ENVIRO_AEMS.cpp:337` — __asm__("");
- `Src/EAXSound/realstream/src/stream.cpp:492` — register int nopendingrequest asm("r30");
- `Src/Ecstasy/EmitterSystem.cpp:57` — asm(
- `Src/Ecstasy/Texture.cpp:59` — asm(
- `Src/FEng/FECodeListBox.cpp:180` — asm(
- `Src/FEng/FEKeyInterpLinear.cpp:270` — asm(
- `Src/FEng/FEKeyTypes.cpp:21` — asm(
- `Src/FEng/FEListBox.cpp:30` — asm(
- `Src/FEng/FEObject.cpp:232` — asm(
- `Src/FEng/FEPackage.cpp:425` — asm(
- `Src/FEng/FEScript.cpp:19` — asm(
- `Src/FEng/FETypeLib.cpp:178` — asm(
- `Src/FEng/FEngStandard.cpp:33` — asm(
- `Src/FEng/FEngine.cpp:341` — asm("" : "+r"(iIterationTicks), "+r"(iTicksRemaining));
- `Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.cpp:539` — register FEObject *volatile object_reg asm("r30") = object;
- `Src/Frontend/Localization/WideCharHistogram.cpp:6` — asm(
- `Src/Frontend/MemoryCard/MemoryCard.cpp:17` — void *memset_noproto(...) __asm__("memset");
- `Src/Frontend/MemoryCard/MemoryCardCallbacks.cpp:17` — void *memset_noproto(...) __asm__("memset");
- `Src/Frontend/MenuScreens/Common/feWidget.cpp:24` — static void *operator new(size_t size) __asm__("__builtin_new");
- `Src/Frontend/MenuScreens/InGame/uiSMSMessage.cpp:24` — static void *operator new(size_t size) __asm__("__builtin_new");
- `Src/Frontend/SubTitle.cpp:130` — register float thetime_ms asm("fr1");
- `Src/Gameplay/GIcon.cpp:32` — int TrackStreamerGetCombinedSectionNumber(...) __asm__("GetCombinedSectionNumber__13TrackStreamer");
- `Src/Gameplay/GRuntimeInstance.cpp:24` — asm(
- `Src/Generated/Events/EBailPursuit.cpp:37` — extern EventHistory gManagerGlobalHistory __asm__("_Q26Speech7Manager.mGlobalHistory");
- `Src/Generated/Events/EDDaySpeech.cpp:37` — idem
- `Src/Generated/Events/EDispIntroRace.cpp:37` — idem
- `Src/Generated/Events/EJumpToStrategyFlow.cpp:37` — idem
- `Src/Lua/LuaAttributes.cpp:266` — asm(".section .rodata
- `Src/Lua/LuaBindery.cpp:152` — asm(".section .rodata
- `Src/Misc/MissionEdit.cpp:5` — asm(
- `Src/Physics/Common/VehicleSystem.cpp:12` — asm(
- `Src/Physics/Dynamics/Articulation.cpp:30` — static JoinList Joints __asm__("_Q28Dynamics12Articulation.Joints");
- `Src/Physics/Dynamics/Collision.cpp:9` — asm(
- `Src/Physics/PhysicsUpgrades.cpp:109` — extern "C" Attrib::Key PUClassKeyExternal() asm("ClassKey__Q36Attrib3gen8pvehicle");
- `Src/Sim/Common/SimSurface.cpp:7` — asm(
- `Src/Speech/EAXAirSupport.cpp:37` — extern EventHistory gManagerGlobalHistory __asm__("_Q26Speech7Manager.mGlobalHistory");
- `Src/Speech/EAXCharacter.cpp:37` — idem
- `Src/Speech/EAXCop.cpp:37` — idem
- `Src/Speech/EAXDispatch.cpp:37` — idem
- `Src/Speech/MiscSpeech.cpp:37` — idem
- `Src/Speech/MusicFlow.cpp:37` — idem
- `Src/Speech/Observer.cpp:37` — idem
- `Src/Speech/PursuitFlow.cpp:37` — idem
- `Src/Speech/RoadblockFlow.cpp:37` — idem
- `Src/Speech/SoundAI.cpp:37` — idem
- `Src/Speech/StrategyFlow.cpp:37` — idem
- `Src/World/CarInfo.cpp:927` — asm(".section .rodata\n"   /* r64 pool: obj 224 */
- `Src/World/CarLoader.cpp:307` — ".previous\n");
- `Src/World/Common/WRoadNetwork.cpp:2010` — register float wld_cutx asm("fr11") = cut_to_position.x;
- `Src/World/Common/WTrigger.cpp:434` — __asm__("" : "=f"(srRadiusPlusVel) : "0"(srRadiusPlusVel), "r"(trig));
- `Src/World/SkyRender.cpp:134` — asm(".section .rodata\n"   /* r64 pool: obj 205 */
- `Src/World/TrackInfo.cpp:13` — asm(".section \".data\"\n"
- `Src/World/TrackPath.cpp:193` — register TrackPathZone *found_zone asm("r26") = nullptr;
- `Src/World/TrackStreamer.cpp:31` — extern void _ClearTable_ool(bBitTable *) asm("ClearTable__9bBitTable");
- `Src/World/rain.cpp:361` — asm(".section \".data\"\n"
- `bWare/Src/bMemory.cpp:200` — asm(
- `bWare/Src/bPrintf.cpp:31` — extern "C" void *bp_memset(void *dst, ...) __asm__("memset");

Familias dentro de los 82:

- **19x alias `gManagerGlobalHistory __asm__("_Q26Speech7Manager.mGlobalHistory")`**
  (todo Src/Speech/* + EaxSoundTypes + 4 Generated/Events): UNA sola linea
  plantada por plantilla de eventos; es un solo fix conceptual.
- **~35x `asm(".section ...")`** de pools/hand-pools (FEng, Animation,
  Support/Utility, World): colocacion manual de rodata del decomp.
- **~14x pins `register ... asm("r0/r8/r9/r26/r30/fr1/fr11")`**: los 124 pins
  del inventario r71 que viven en .cpp compilables (el resto esta en ficheros
  PLATAFORMA/OTRO y aparece como `asm_lines` textual).
- **6x redirecciones `__asm__("memset")`/`__asm__("__builtin_new")`**.

Ademas, 96 ficheros NO-GCCASM contienen `asm()` textual (57 OTRO, 28
PLATAFORMA, 11 OK — estos ultimos en ramas `#ifdef ZMAIN_HAND_POOL`/GC muertas):
el inventario real de andamios excede los 82 que la sonda eleva a error.

## PLATAFORMA (61)

Includes GC que cortan la compilacion: `dolphin/mtx.h` (38 ficheros, via
`CameraMover.hpp`/bMath.hpp), `dolphin.h` (15), `dolphin/gx.h` (3), GXEnum (1).
Concentrados en zCamera (19), zPlatform (14), zEcstasy (10). 28 de los 61
tienen ademas asm() textual. Para compilarlos habria que dar rama XENON a
esos includes (bMath.hpp ya la tiene; CameraMover.hpp no).

## OTRO (317) — dos causas dominan

- **202x C2011 `OnlineRaceParameters` redefinido**: `Frontend/Database/
  RaceDB.hpp:53` lo define `class` (0x10) y `Online/OnlineCfg.hpp:27` lo
  define `struct` (0xc). Dos descompilaciones de la misma clase que nunca
  chocaron porque ninguna unidad unity incluye ambas; la sonda por-fichero SI
  las junta. **Un solo fix en src/ desbloquearia 202 ficheros para re-sondar.**
- **22x C2664 `eScrollDir` a `eScrollDir`**: `uiSMS.hpp:16` hace forward-decl
  `enum eScrollDir;` — GCC lo acepta, MSVC C++03 no (dos tipos distintos).
  GCC-ism de enum forward, masa en Frontend.
- **9x `PlatConvertColor` not found**: la define solo
  `src/Speed/GameCube/Src/Ecstasy/eLightPlat.hpp` — plataforma GC sin rama
  XENON (este grupo es conceptualmente PLATAFORMA).
- 5x C2371 `Csis::InterfaceId` redefinido, 5x C2061 `GActivity` (orden de
  declaraciones), ~10x C4716 funciones sin return (cuerpos decomp
  incompletos), 2x C2177 `3.4028235e38f > FLT_MAX` (GCC clampa, MSVC error),
  5x C2440 iteradores STL del XDK mas estrictos, y ~50 de cola larga
  (identificadores que solo existen por orden de includes del unity, tipos
  sin forward-decl, etc.).

## Fidelidad de la sonda

- `cl.exe` paralelos mienten ocasionalmente con C1083 espureos (colisiones de
  temporales en %TMP%): sonda.py reintenta serial los SHIM; tras el fix, 0.
- QuickSpline.cpp (objetivo fase 1): OK, .obj de 24.177 B.
- Los 11 OK con asm textual estan verificados: asm en `#ifdef` muertos.

## Reproducir / siguiente ronda

```
python tools/scratch/x360_sonda/gen_zscaf.py        # regenera zscaf.h
python tools/scratch/x360_sonda/sonda.py <fichero> # clasifica UNO (JSON)
python tools/scratch/x360_sonda/sonda.py --all --jobs 4   # metrica completa
# resultados: tools/scratch/x360_sonda/results.json (857 entradas)
```

Siguiente paso (no hecho): aparear funciones de los 397 .obj con el .text
X360 (`tools/scratch/x360_text.bin`, VA 0x4F0000) — necesita el decodificador
de .pdata (formato 8B propietario, ver r71c-x360-sonda.md).


## RE-SONDA r71e (tras OnlineRaceParameters + enum forward-decl)

| clase | antes | ahora |
|---|---|---|
| **OK** | 397 (46,3%) | **521 (60,8%)** — 10,9 MB de .obj |
| GCCASM | 82 | 107 (los desbloqueados revelan su razon real) |
| PLATAFORMA | 61 | 67 |
| OTRO | 317 | **162** |

Dos fixes de fuente movieron +124 ficheros a OK. El OTRO restante esta
diversificado (maximo cluster: 27x C2664 ArrayScroller en AIPursuit.cpp;
11x C2371 Csis; 10x C3861 PlatConvertColor) — cada uno una investigacion
pequena, sin mas causas unicas de orden 200.

## RE-SONDA r71f-x360 (tanda agent/x360-otro-r1: 46 fixes, 56 ficheros)

| clase | antes | ahora |
|---|---|---|
| **OK** | 547 (63,8%) | **590 (68,8%)** — 13,9 MB de .obj |
| GCCASM | 123 | 129 (7 ficheros OTRO eran asm real tras su fix de declaraciones) |
| PLATAFORMA | 75 | 78 (CameraMover dolphin en ICEManager; Mtx44/PSMTX44* dolphin en SoundConn y EAXSound.cpp) |
| OTRO | 112 | **60** |

Metodo: el mismo bucle de la r71e (sonda → include/extern/fwd-decl → sonda,
max 8 iteraciones). Cero emision en GC verificado por unidad tras cada grupo
(build_direct + measure contra baseline): zAI 272796/272796, zAttribSys
48776/48776, zEAXSound 151092/151092, zEagl4Anim 110208/113016 (gap
preexistente), zFe+zFe2+zFeOverlay 566156/566156, zMain 159776/159776,
zMisc 78008/78008, zAnim 42292/42292, zFoundation 36000/36000, zCamera
120412/125008 (gap preexistente), zPhysicsBehaviors 235320/236176 (gap
preexistente), zWorld 156184/160956 (gap preexistente).

### Hallazgos de la tanda

1. **zFe estaba ROTO desde fix 13 de la r71e** (nadie lo recompilo; el .o
   quedo del 6-sep): el include de feDialogBox.hpp anadido a uiWorldMap.cpp
   enfrentaba en el unity las DOS descompilaciones del subsistema de
   dialogos (feDialogBox.hpp vs DialogInterface.hpp: eDialogTitle,
   eDialogFirstButtons, feDialogConfig y DialogInterface duplicados).
   Reparado con guarda ODR compartida (patron OnlineRaceParameters) +
   uiWorldMap pasa a DialogInterface.hpp (las llamadas existen identicas en
   ambas) + uiRepSheetRaceEvents dedup RaceStarter (su clase local
   duplicaba Frontend/RaceStarter.hpp canonico).
2. **u32 NO es portable en decls crudas**: en ProDG/GC `u32 = unsigned
   long`, en MSVC `unsigned int`. Las decls locales `unsigned long
   FEHashUpper(...)` de uiRepSheetRival/uiOptionsController casaban en GC y
   daban C2556 en MSVC: la forma que casa en ambas es escribir `u32` a secas.
3. **La guarda ANIMWORLDTYPES_EMIT_MESSAGE_HASHES** (que enciende cada
   SourceList para controlar que unidades emiten los 8 hashes WAM_*) hay que
   definirla ANTES del primer include del .cpp: ControlScenario.hpp ya trae
   AnimWorldTypes.hpp por la cadena.
4. **_STL es STLport-4.5 GC puro** (`_STLP_USE_OWN_NAMESPACE`): PVehicle,
   PhysicsObject y AIVehicle lo usan cualificado y el XDK no lo tiene — sin
   fix valido (habria que aliasar namespace, no es include/extern/fwd-decl).
5. **Huecos de decomp detectados** (identificadores que NO existen en el
   arbol): FastPolarizedQuatBlend (FnDeltaQFast/FnDeltaSingleQ lo llaman y
   nadie lo define), Tweak_ForceICEReplay (ICEReplay), CarRenderInfo SIN
   definicion completa (CarRenderConn, VehicleFragmentConn y
   VehiclePartDamage la desreferencian), CarPartModel.

### Tabla por unidad (final r71f-x360)

| unidad | n | OK | GCCASM | PLATAFORMA | OTRO |
|---|---|---|---|---|---|
| zAI | 36 | 29 | 4 | 1 | 2 |
| zAnim | 23 | 9 | 9 | 0 | 5 |
| zAttribSys | 8 | 8 | 0 | 0 | 0 |
| zBWare | 15 | 4 | 2 | 0 | 9 |
| zCamera | 28 | 6 | 1 | 20 | 1 |
| zDebug | 1 | 1 | 0 | 0 | 0 |
| zDynamics | 3 | 0 | 2 | 0 | 1 |
| zEAXSound | 35 | 23 | 7 | 4 | 1 |
| zEAXSound2 | 68 | 53 | 12 | 0 | 3 |
| zEagl4Anim | 40 | 32 | 5 | 0 | 3 |
| zEcstasy | 23 | 4 | 5 | 13 | 1 |
| zFEng | 31 | 18 | 10 | 0 | 3 |
| zFe | 57 | 34 | 13 | 7 | 3 |
| zFe2 | 71 | 57 | 9 | 2 | 3 |
| zFeOverlay | 17 | 9 | 0 | 7 | 1 |
| zFoundation | 15 | 5 | 6 | 0 | 4 |
| zGameModes | 1 | 1 | 0 | 0 | 0 |
| zGameplay | 17 | 10 | 6 | 0 | 1 |
| zLua | 5 | 2 | 3 | 0 | 0 |
| zMain | 176 | 168 | 6 | 1 | 1 |
| zMisc | 28 | 27 | 0 | 0 | 1 |
| zMiscSmall | 2 | 2 | 0 | 0 | 0 |
| zMission | 1 | 0 | 1 | 0 | 0 |
| zPhysics | 14 | 8 | 2 | 0 | 4 |
| zPhysicsBehaviors | 30 | 29 | 0 | 1 | 0 |
| zPlatform | 18 | 4 | 0 | 14 | 0 |
| zRender | 3 | 3 | 0 | 0 | 0 |
| zSim | 16 | 10 | 4 | 1 | 1 |
| zSpeech | 17 | 5 | 11 | 0 | 1 |
| zTrack | 12 | 6 | 4 | 2 | 0 |
| zWorld | 29 | 12 | 5 | 5 | 7 |
| zWorld2 | 17 | 11 | 2 | 0 | 4 |
| **TOTAL** | **857** | **590** | **129** | **78** | **60** |

### Los 60 OTRO restantes por causa (sin clusters accionables)

- **24x C4716** funciones sin return (cuerpos decomp incompletos): UGroup,
  AnimBank/Chooser/EngineManager/BasicCharacter/WorldScene, CARSFX_Skids,
  SFXObj_Reverb, NFSMixMap, eStreamingPack, FEJoyInput, WCollider,
  WCollisionAssets, ParameterMaps, RaceParameters, SimpleModelAnim,
  SpaceNode, SpeedScript, Strings, bChunk, bDebug, bFunkPlat, bMath,
  bQuaternion.
- **6x C2440** iterator de std::vector a puntero crudo (el STL del XDK tiene
  _Vector_iterator clase): ExpressionEvaluator, SpeechManager, GActivity,
  Bounds, WCollisionMgr, WWorldPos.
- **5x C2143**: AICopManager (template find r57, ANDAMBLIO no tocar),
  VehicleDB y CustomizeManager (case-range GCC), SimEntity
  (statement-expr GCC), InputDeviceGC.
- **3x C2653 _STL** (STLport GC): PVehicle, PhysicsObject, AIVehicle.
- **3x C2511** firma .cpp != firma .h (FEJoyPad/FEMouse/FETypes; cambiar la
  firma esta vedado).
- **2x C2027 CarRenderInfo** sin definicion + VehiclePartDamage (C2065
  CarPartModel): hueco de decomp.
- **2x C3861 FastPolarizedQuatBlend** (no existe en el arbol) + Geometry
  (__builtin_fabsf GCC).
- **2x C2177** constantes 3.4028235e38 > FLT_MAX (FEngRender, bVector).
- **2x C2259** clase abstracta instanciada (FEAnyTutorialScreen,
  uiEATraxJukebox: overrides sin implementar).
- Cola: FastMem/bList (asm volatile), UVectorMath/bMemoryOverloads (C2084),
  FnTurnBlender (C2001), EAXSND8Wrapper (C2757 System),
  feIconScrollerMenu (C2738), SmokeableInfo (C2061), ICEReplay
  (Tweak_ForceICEReplay no existe).

## r71f: los 5 huecos linked diagnosticados

| unidad | delta | causa |
|---|---|---|
| zWorld | rodata-64 | literales muertos que el extraido conserva (keep) y nuestro ya no genera; ademas 39/254 entradas keep de zWorld son FOSILES (no estan en ningun objeto) — limpieza previa obligatoria |
| zWorld2 | bss+64 | pendiente de diagnosticar (posible rango splits) |
| zAI | rodata-112 | deficit de contenido preexistente (no sobran cadenas: FALTAN) |
| zMisc | rodata+264 data+32 | 14 doubles 2^52 (112 B bloqueados), resto varios |
| steering | text-8 bss+32 | unidad NonMatching (70,7%); NO esta promocionada; sus ranges ya estan corregidos en splits |
| zFe2 | ENLACE FALLA | racimo: nuestros scope-ids (.35773) difieren de los del extraido (.32137) en 16 simbolos Callback/dtor — promocionar con la unidad hermana o nada |

**Conclusion**: el frente linked esta a 5 huecos pequenos + 1 racimo de que
se sepa el mecanismo exacto. zPlatform/zPhysics/zSpeech/zLua/zGameplay ya
enlazan IGUAL (promocionados o sin delta).


## r71f-zWorld: el -64 es FRONTERA DE SPLITS (cerrado el diagnostico)

Investigacion completa (enlace directo + diff byte a byte + relocs):
- Los $LC duplicados (616 B de dupstr) estan TODOS MUERTOS: el enlazador los
  estripa, no cuentan. Quitar las 3 entradas keep empeora (-64 → -120) porque
  el EXTRAIDO conserva sus copias.
- El extraido de zWorld tiene **808 referencias .data→.rodata (16.400 B
  vivos)**; el nuestro tiene CERO: esas tablas de punteros las traen otras
  unidades en el enlace real. El -64 es la porcion de rodata que splits
  atribuye a zWorld y que en el enlace base reside en su rango.
- La zona divergente del .rodata enlazado (0x80409f94+) contiene cadenas
  Attrib::Gen::* y tablas reordenadas — el layout difiere porque los 808
  punteros vivos del extraido colocan datos que nosotros ponemos en otra
  unidad.
- NO es arreglable desde fuente: exige mover la frontera de rodata en
  splits.txt (la misma clase que zGameplay/zGameModes de ventana-pendiente).


## r72b: zAI -112 analizado (mismo metodo que zWorld)

Cadenas SOLO en el enlace base (72 B alineados):
- `CarLoaderDefragment` (24 B): en CarLoader.cpp:2066 pero NO la genera zAI
- `unexpected end of file in %s` (32 B): lundump.c (unidad zLua)
- `(__gc` (8 B): sin dueno en fuente
- `ffA ` (8 B): **bytes de un literal float** (final de 0.9f aprox) — tabla
  de constantes 1.0/0.278/1e-5/20.0/0.2/0.9/-1.0/0.5/0.01 que zAI no genera

Los otros ~40 B: datos binarios (floats) de la misma tabla.
Mismo patron que zWorld: datos vivos del extraido no generados por nuestro.


## r73b: zAI -112 CERRADO como contabilidad de keep.lst

Diagnóstico completo:
- Ambos .o (extraído y nuestro) tienen el prefijo bWare y TODOS los tags
  (RoadBlock×38/95, AutoSpawnMode×4/17, CopsEnabled×4/20...)
- El extraído tiene MÁS copias keep-nombradas (los $LC del nuestro se
  renumeran y el lcfix no puede seguirlos → 57 FALLOS acumulados)
- El -112 = 256 B del base que el nuestro no conserva - 144 B que el
  nuestro conserva y el base no.
- **NO es arreglable desde fuente**: es la diferencia de qué símbolos
  keep-nombrados sobreviven al estripar. Requiere limpiar los 57 FALLOS
  del keep.lst (higiene de ventana-pendiente §3) y luego re-medir.
