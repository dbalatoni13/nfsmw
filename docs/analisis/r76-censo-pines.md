# r76 — El censo definitivo del asm que queda, y qué sostiene cada pieza

Encargo de MEDIDA (solo lectura). Fecha 2026-09-16, rama `agent/vedas-oraculo-r2`,
HEAD `c99ec268`. Ningún fichero de `src/`, `config/` ni `build/` tocado.

Herramienta nueva: **`scripts/censoasm.py`** (sin commitear). Quita los
comentarios con una máquina de estados ANTES de contar y clasifica cada
aparición de `asm(...)` en cinco clases. Datos intermedios en
`scratchpad/r76_censo/`.

---

## 0. La respuesta a la pregunta

> «¿los mismos pines que sostienen funciones al 100 % son los que nos impiden
> avanzar, porque esconden la forma real de la función?»

**Sí, y está medido: 34 de las 66 funciones que hoy marcan 100 % con andamio
(26.248 B de `.text`) tienen al menos un pin o barrera sobre una variable que
el DWARF del original NO CONTIENE.** Ese 100 % no describe la función del
original: describe una función distinta a la que hemos atado el asignador
hasta que el objeto coincidiera. Mientras el andamio siga puesto, la forma
verdadera no se puede ni buscar (la variable inventada ocupa el registro que
la forma buena necesita) ni medir (cualquier prueba parte de 100 %).

El caso más limpio del informe cabe en tres líneas:
`EAGL4Anim::MemoryPoolManager::NewBlockAux` son **84 bytes** con **tres pines**
(`blockStorage` r8, `poolFree` r11, `nextPoolFree` r9) y el DWARF del original
dice que esa función tiene exactamente **cuatro** entradas: `this` r3, `size`
r4, `idx` r4 y **una sola** local `r` (char*, r3). Tres pines sosteniendo tres
locales que nunca existieron, en una función de 84 B que está «al 100 %».

---

## 1. CENSO EXACTO

Barrido de `src/` completo (`.c .cpp .cc .h .hpp .inl .cxx`), comentarios
eliminados antes de contar.

| clase | qué es | apariciones | ficheros |
|---|---|---:|---:|
| **PIN** | `register T x asm("rN"/"frN")` | **54** | 32 |
| **BARRERA** | `asm("" : ...)` / `__asm__("")`, con o sin operandos | **69** | 44 |
| **INSTR** | bloque con mnemónicos **PPC** reales | 118 | 30 |
| **INSTR-OTRA** | mnemónicos MIPS/VU0/SPARC (ramas PS2 y STL de terceros) | 48 | 9 |
| **DATOS** | `asm(".section ...")`, `.byte/.4byte/.asciz`, `.set` | 468 | 161 |
| **ALIAS** | `__asm__("simbolo")` sobre una declaración — **no es andamio** | 556 | 130 |
| | **TOTAL** | **1.313** | **306** |
| | **ANDAMIO REAL (PIN + BARRERA)** | **123** | **61** |

**Por qué hacía falta quitar los comentarios**: hay **365 menciones más** de
`asm(...)` dentro de comentarios, **38 de ellas con la forma literal
`register X asm("rN")`**. Medido:
`grep -rnE 'register[^;]*asm[[:space:]]*\(' src/` devuelve **101** líneas
donde hay **54** pines — casi la mitad de lo que cuenta son notas de
diagnóstico de rondas pasadas. Y la otra trampa es la contraria: hay pines y
barreras que ningún grep de la forma canónica encuentra, como las cuatro
`__asm__("# a")` de `RoadblockFlow.cpp` (plantilla sólo de comentario, cero
bytes emitidos, barrera de planificador de pleno derecho) y las macros
`__asm__(#name)` de `fdlibm.h`/`libioP.h`, que son ALIAS y no barreras.

### Desglose de las clases que NO son andamio

`ALIAS` (556) — renombres, el mecanismo del unity build y del troceador:

| familia | n |
|---|---:|
| `$LCnnn` (pool de literal del unity) | 210 |
| nombre suelto (`g_nLidState_804B4FD1`, `_f_powf`, `huge`, `zanim_lc_*`…) | 162 |
| `lbl_XXXXXXXX` (símbolo del troceador) | 90 |
| nombre manglado de GCC (`Metodo__7Clase…`) | 47 |
| redirección a libc/builtin (`memset`, `__builtin_new`…) | 17 |
| `gap_`/`pad_` (relleno del troceador) | 14 |
| construido por macro (`#name`, `VFPRINTF_SYM_*`, `_G_VTABLE_LABEL_PREFIX`) | 13 |
| estático de clase manglado (`_Q28Dynamics12Articulation.Joints`) | 3 |

`DATOS` (468) — **458** son bloques `.section … .previous` (los pools y
hand-pools de colocación de `.rodata`/`.data` del decomp), 4 `.set`, 6 cola.
Es un frente distinto al de los pines: no toca el asignador, pero rompe
igualmente cualquier compilador que no sea GCC.

`INSTR` (118) — código PPC escrito a mano. Concentrado: `UVectorMath.cpp` 32,
`eViewPlat.cpp` 19, `bMath.hpp` 8, `criticalpath.c` 7 (el códec VP6 del
original, que **es** del original), `zFe2.cpp` 5, `SFXCTL_3DObjPos.cpp` 5.

`INSTR-OTRA` (48) — `UVectorMath.hpp` 21, `bMath.hpp` 12, `eekernel.h` 5,
`_sparc_atomic.h`/`_threads.h` 4: ramas PS2 (VU0) y STLport. No son andamio
del puerto GC.

### Los 61 ficheros con andamio, por unidad

| unidad | % unidad | PIN | BAR | ficheros | funciones | B al 100 % |
|---|---:|---:|---:|---:|---:|---:|
| zEcstasy | 99,89 | 6 | 11 | 6 | 7 | 2.900 |
| zFe | 100,00 | 9 | 6 | 4 | 5 | 1.472 |
| zTrack | 99,94 | 6 | 1 | 3 | 4 | 3.124 |
| zEAXSound | 100,00 | 2 | 5 | 5 | 5 | 3.720 |
| zSpeech | 100,00 | 0 | 6 | 2 | 3 | 1.400 |
| zEagl4Anim | 99,99 | 4 | 0 | 2 | 2 | 316 |
| zEAXSound2 | 99,97 | 3 | 1 | 4 | 3 | 1.644 |
| zGameplay | 99,99 | 2 | 3 | 3 | 4 | 4.604 |
| csis | 100,00 | 3 | 2 | 1 | 2 | 624 |
| gc_driver | 100,00 | 2 | 2 | 1 | 2 | 1.156 |
| zWorld | 99,99 | 3 | 1 | 1 | 2 | 1.816 |
| zFeOverlay | 100,00 | 1 | 3 | 1 | 2 | 2.516 |
| zFEng | 100,00 | 1 | 2 | 1 | 2 | 3.856 |
| zCamera | 99,99 | 0 | 3 | 2 | 2 | 1.116 |
| zMain | 100,00 | 2 | 1 | 2 | 2 | 1.520 |
| zFe2 | 100,00 | 2 | 1 | 2 | 2 | 712 |
| sndvd | 100,00 | 1 | 2 | 1 | 1 | 628 |
| zPlatform | 99,92 | 1 | 1 | 2 | 2 | 616 |
| snddrv · sfsplit · sfir · spchsamp · srandom · spchrand · pathsnd | 100,00 | 3 | 8 | 7 | 7 | 2.660 |
| zSim · zPhysics · zDynamics · zAI · zAnim · zBWare · zFoundation | 100,00 | 2 | 6 | 7 | 7 | 8.116 |
| FSasync · ppc2D2 · avplayer | 100,00 | 1 | 2 | 3 | 3 | 1.116 |
| `UTLVector.h` (plantilla, sin unidad propia) | — | 0 | 2 | 1 | — | — |
| **TOTAL** | | **54** | **69** | **61** | **73**\* | **45.632** |

\* 69 con función nombrada + 4 sitios en cabecera/inline que `report.json` no
sabe nombrar (`UTLVector.h` ×2, `ENVIRO_AEMS.h`, `PackedDecimal`).

(`%` de unidad tomado de `build/GOWE69/report.json`; para las 6 unidades cuyo
`.o` es más nuevo que el informe —zEcstasy, zEAXSound, zTrack, zEagl4Anim,
zEAXSound2, zPlatform— se remidió con `scripts/pctsnap.py` y se usan las
cifras frescas.)

La tabla completa fichero a fichero está en
`scratchpad/r76_censo/salida_familias.txt` (sección «TABLA COMPACTA»).
Los diez ficheros con más andamio:

| fichero | unidad | PIN | BAR |
|---|---|---:|---:|
| `Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.cpp` | zFe | 6 | 2 |
| `Speed/Indep/Src/Ecstasy/EmitterSystem.cpp` | zEcstasy | 3 | 3 |
| `Speed/Indep/Libs/csis/.../csis.cpp` | csis | 3 | 2 |
| `Speed/Indep/Src/World/CarRender.cpp` | zWorld | 3 | 1 |
| `Packages/realmemcard/.../gc_driver.cpp` | gc_driver | 2 | 2 |
| `Speed/GameCube/Src/Ecstasy/eLightE.cpp` | zEcstasy | 1 | 4 |
| `Speed/Indep/Src/EAGL4Anim/MemoryPoolManager.cpp` | zEagl4Anim | 3 | 0 |
| `Speed/Indep/Src/World/TrackStreamer.cpp` | zTrack | 3 | 0 |
| `Speed/Indep/Src/Frontend/.../CarCustomize.cpp` | zFeOverlay | 1 | 3 |
| `Speed/Indep/Src/World/WeatherMan.cpp` | zTrack | 2 | 1 |

---

## 2. CRUCE CON EL PORCENTAJE — los FALSOS 100

El andamio toca **73 funciones**. De ellas:

- **66 marcan 100 %** → **45.632 B** de `.text` que hoy se contabilizan como
  terminados y dependen de un `asm` de GCC.
- **7 NO llegan al 100 %**: el andamio está puesto y ni siquiera basta.

| función | unidad | B | % | andamio |
|---|---|---:|---:|---|
| `eProject` | zEcstasy | 268 | 93,97 | 1 barrera (`halfVP2`) |
| `CARSFX_RoadNoise::GenerateRoadNoise` | zEAXSound2 | 1.240 | 96,80 | 1 pin (`slipBoost`) |
| `eLightMaterialPlatInterface::UpdatePlatInfo` | zEcstasy | 2.044 | 99,96 | 1 pin + 4 barreras |
| `UTLVector.h::assign` (plantilla) | zCamera+zEAXSound | — | — | 2 barreras |
| `ENVIRO_AEMS.h` (`FX_ROADNOISE`) | zEAXSound2 | — | — | 1 barrera |
| `PackedDecimal::PackedDecimal` (inline) | zGameplay | — | — | 1 barrera |

### Los 66 al 100 %, por tamaño (extracto; la lista entera en `salida_falsos.txt`)

| B | unidad | función | PIN | BAR |
|---:|---|---|---:|---:|
| 3.080 | zFEng | `FEngine::ProcessPadsForPackage` | 1 | 1 |
| 2.544 | zGameplay | `GTrigger::GTrigger` | 0 | 1 |
| 1.908 | zSim | `QuickGame::CreateCars` | 1 | 1 |
| 1.896 | zDynamics | `Dynamics::Collision::Geometry::SphereVsBox` | 0 | 1 |
| 1.708 | zFeOverlay | `CustomizeParts::Setup` | 1 | 0 |
| 1.684 | zAI | `AIPursuit::AssignClosestOffsets` | 0 | 1 |
| 1.680 | zGameplay | `GRaceParameters::GenerateIndex` | 0 | 1 |
| 1.592 | zEAXSound | `SFXCTL_Physics::UpdateNIS` | 0 | 1 |
| 1.488 | zEAXSound | `SFXCTL_3DObjPos::GenerateSinglePlayerMix` | 0 | 2 |
| 1.412 | zPhysics | `PhysicsObject::PhysicsObject` | 1 | 0 |
| 1.348 | zMain | `GameDevice::PollDevice` | 0 | 1 |
| 1.304 | zEAXSound2 | `SFXObj_Collision::InitSFX` | 1 | 0 |
| 1.236 | zEcstasy | `MaybeChangeViewMode` | 0 | 1 |
| 1.208 | zTrack | `RegionQuery::CalculateRegionInfo` | 2 | 1 |
| 1.068 | zWorld | `CarRenderInfo::UpdateLightStateTextures` | 0 | 1 |
| … | | (51 más, de 948 B a 52 B) | | |

**Marcado de FALSO 100 con prueba**: 34 de esas 66 (**26.248 B**) tienen al
menos un andamio del grupo A del §4 — el oráculo dice que la variable atada no
existe en el original. Ésos no son «100 % con una muleta»: son **100 % de una
función que no es la del original**. Lista completa en el §5.

> Aviso de método: «depende de un pin» en sentido estricto sólo se prueba
> recompilando sin él, y este encargo es de solo lectura. Lo que aquí se prueba
> es más fuerte para 34 de los 66: la **variable** que el pin ata no existe en
> el original, luego la fuente no es la del original aunque el objeto lo sea.
> Para los otros 32 el veredicto es «pin sobre una variable real» (§4 grupo B):
> hace falta una prueba de compilación, no un oráculo.

---

## 3. CRUCE CON PORTABILIDAD (X360)

Fuente: `tools/scratch/x360_sonda/results.json`, sonda del **15-sep (r71f)**
—un día más antigua que el árbol—, con `cl.exe` 14.00.2110 del XDK:
**OK 590 · GCCASM 129 · PLATAFORMA 78 · OTRO 60** sobre los 857 `.cpp`
internos. (El documento `r71c-x360-metrica.md` empieza con la lista de **82**;
ésa es la primera sonda. La cifra viva es 129: al arreglar causas de `OTRO`
afloraron más ficheros cuyo bloqueo real es el asm.)

### Los 129 GCCASM, por el tipo de asm que la sonda señala primero

| tipo señalado | n | ¿lo arregla el despineo? |
|---|---:|---|
| bloque `asm(` de `.section` (pool/hand-pool) | 58 | no — frente de DATOS |
| **ALIAS** de nombre manglado | 42 | no — frente de renombres |
| **BARRERA** | 15 | **sí** |
| **PIN** | 13 | **sí** |
| otra | 1 | — |

Las familias de ALIAS que bloquean X360 no son 42 problemas: son **6**.
`extern EventHistory gManagerGlobalHistory __asm__("_Q26Speech7Manager.mGlobalHistory")`
sale **20 veces** (todo `Src/Speech/*` + 4 `Generated/Events`), el
`operator new __asm__("__builtin_new")` 5, `__asm__("memset")` 4, y luego
singletons.

### La prueba de que el despineo desbloquea de verdad

De los **24** ficheros GCCASM cuyo primer error es un pin o una barrera,
**10 ya no lo tienen** (el censo r76 dice 0) porque la r75 y el piloto
`zWorld2` los despinaron después de la sonda:

`CARSFX_PreColWoosh.cpp`, `CARSFX_TrafficFX.cpp`, `CARSFX_WindNoise.cpp`,
`SFXObj_TruckFX.cpp`, `SFXObj_WorldObject.cpp`, `NFSMixMapState.cpp`,
`ENVIRO_AEMS.cpp`, `Registration.cpp`, `WRoadNetwork.cpp`, `WTrigger.cpp`.

Quedan **14** pendientes con la misma forma exacta de desbloqueo.
(Conviene **re-sondar**: la métrica X360 viva es mejor que 129 GCCASM, y
`ENVIRO_AEMS.h:337` sigue teniendo una barrera que la sonda ya no ve porque
mide el `.cpp`, no la cabecera.)

### Los 61 ficheros con andamio VIVO, ordenados por «cuántas plataformas desbloquea»

| grupo | ficheros | pines | barreras | qué desbloquea al quitarlo |
|---|---:|---:|---:|---|
| **GCCASM puro** (el asm es el ÚNICO bloqueo) | **30** | **35** | **29** | **X360 + PS2 + PC, el fichero entero, sin más trabajo** |
| con otro bloqueo (PLATAFORMA/OTRO) | 16 | 9 | 21 | PS2/PC; X360 necesita además la rama XENON o el fix C++ |
| no sondados (middleware fuera de las 34 SourceLists) | 15 | 10 | 19 | PS2/PC; la sonda X360 ni los mira todavía |

**Los 30 GCCASM puros, por cantidad de andamio** (quitarlo = fichero
compilable en los tres compiladores):

```
 6 pin  2 bar  Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.cpp
 3 pin  3 bar  Indep/Src/Ecstasy/EmitterSystem.cpp
 3 pin  0 bar  Indep/Src/EAGL4Anim/MemoryPoolManager.cpp
 3 pin  0 bar  Indep/Src/World/TrackStreamer.cpp
 2 pin  0 bar  Indep/Src/Ecstasy/eSolid.cpp
 1 pin  2 bar  Indep/Src/FEng/FEngine.cpp
 2 pin  0 bar  Indep/Src/Frontend/FEngInterfaces/FEngInterface.cpp
 2 pin  0 bar  Indep/Src/Gameplay/GManager.cpp
 2 pin  0 bar  Indep/Src/Input/Common/SteeringWheelDevice.cpp
 0 pin  4 bar  Indep/Src/Speech/RoadblockFlow.cpp
 1 pin  1 bar  Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.cpp
 1 pin  1 bar  Indep/Src/Frontend/SubTitle.cpp
 0 pin  3 bar  Indep/Src/Frontend/MemoryCard/MemoryCardCallbacks.cpp
 1 pin  1 bar  Indep/Src/Frontend/MenuScreens/InGame/FEpkg_MU_Keyboard.cpp
 1 pin  1 bar  Indep/Src/Sim/Activities/QuickGame.cpp
 1 pin  0 bar  Indep/Src/EAGL4Anim/RawStateChan.cpp
 1 pin  0 bar  Indep/Src/EAXSound/CARSFX/CARSFX_Roadnoise.cpp
 1 pin  0 bar  Indep/Src/EAXSound/CARSFX/SFXObj_Collision.cpp
 1 pin  0 bar  Indep/Src/EAXSound/Ginsu/ginsudata.cpp
 1 pin  0 bar  Indep/Src/EAXSound/realstream/src/stream.cpp
 0 pin  2 bar  Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.cpp
 1 pin  0 bar  Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.cpp
 0 pin  2 bar  Indep/Src/Gameplay/GRaceDatabase.cpp
 0 pin  2 bar  Indep/Src/Speech/EAXDispatch.cpp
 1 pin  0 bar  Indep/Src/World/TrackPath.cpp
 0 pin  1 bar  Indep/Src/AI/Common/AIPursuit.cpp
 0 pin  1 bar  Indep/Src/Animation/AnimEntity_WorldEntity.cpp
 0 pin  1 bar  Indep/Src/EAXSound/STICH_PlayBack.cpp
 0 pin  1 bar  Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.cpp
 0 pin  1 bar  Indep/Src/Gameplay/GTrigger.cpp
```

Los 16 mixtos (el andamio no es el único bloqueo): `EcstasyE.cpp`,
`EcstasyEx.cpp`, `TextureInfoPlat.cpp`, `eLightE.cpp`, `eMathE.cpp`,
`LGWheels.cpp`, `ICEManager.cpp`, `TrackCop.cpp`, `CarCustomize.cpp`,
`CarRender.cpp`, `WeatherMan.cpp` (PLATAFORMA: includes `dolphin/*`) y
`FastMem.cpp`, `InputDeviceGC.cpp`, `PhysicsObject.cpp`, `Geometry.cpp`,
`bList.cpp` (OTRO: error C++ del árbol).

Los 15 no sondados: `csis.cpp` (3+2), `gc_driver.cpp` (2+2), `sndvd.c` (1+2),
`snddrv.c` (2+0), `sfsplit.c` (1+1), `FSasync.c` (1+0), `sfir.c`,
`spchsamp.c`, `UTLVector.h`, `avplayer.cpp`, `ppc2D2.c`, `pathsnd.cpp`,
`srandom.c`, `spchrand.c`, `ENVIRO_AEMS.h`.

**Aparte del andamio**, para X360 quedan 58 ficheros bloqueados por los pools
`asm(".section")` y 42 por alias (6 familias). Son tres frentes distintos;
este informe sólo cubre el primero.

---

## 4. CRUCE CON EL ORÁCULO — lo más valioso

`python scripts/dwarf1.py fn <función>` sobre `orig/GOWE69/NFSMWRELEASE.ELF`.
Consultadas **72 funciones** (todas las que tocan andamio), **65 con DWARF**.
Las 7 sin oráculo: `Csis::System::Subscribe`, `Csis::System::Unsubscribe`,
`DSIHandler`, `CompletePCreadAsync`, `__cvt_fp2unsigned`,
`PackedDecimal::PackedDecimal` (inline sin DIE) y
`Speech::RoadblockFlow::Setup`. Se comprobó, para **los 54 pines** y para
**las 69 barreras** (por sus operandos), si la variable atada aparece en el
original y en qué registro. Las fichas quedan en
`scratchpad/r76_censo/dwarf/`.

Cuatro veredictos reales, con las correcciones manuales anotadas al final:

| veredicto | pines | barreras | total |
|---|---:|---:|---:|
| **A — la variable NO EXISTE** en el original | 31 | 22 | **53** |
| **B — existe** (mismo registro, otro registro, o barrera seca) | 18 | 43 | **61** |
| **C — sin oráculo** (libc / LibSN / MWCC) | 5 | 4 | **9** |

### Muestra representativa (los 15 pines de mayor tamaño, verificados)

| función | B | variable pineada | pin | el ORIGINAL |
|---|---:|---|---|---|
| `FEngine::ProcessPadsForPackage` | 3.080 | `padMask` | r30 | **NO existe** — el bloque `0x80186954` sólo tiene `PadIndex` r25; los `PadMask` r28 del DWARF son de otros tres bloques |
| `eLightMaterialPlatInterface::UpdatePlatInfo` | 2.044 | `envmap_min_scale` | fr5 | **existe, f5** — el pin reproduce un reparto REAL |
| `QuickGame::CreateCars` | 1.908 | `collisionPos` | r4 | **NO existe** — el original tiene `startLoc` r4 (param) e `initialPos` en `[r1+8]` |
| `CustomizeParts::Setup` | 1.708 | `vinyl_group_number` | r21 | **existe, r21** |
| `PhysicsObject::PhysicsObject` | 1.412 | `bodyOwner` | r3 | **NO existe** — `owner` r31 es param de un ctor INLINE, no una local; además el pin vive dentro de un `({ … })` (otra extensión GCC) |
| `SFXObj_Collision::InitSFX` | 1.304 | `collisionReverbSlot` | r10 | **NO existe** (149 locales, ninguna así) |
| `CARSFX_RoadNoise::GenerateRoadNoise` | 1.240 | `slipBoost` | fr10 | **NO existe** (ya documentado en `bf6edb07`) |
| `RegionQuery::CalculateRegionInfo` | 1.208 | `fogStart` | fr11 | **NO existe** — `oldDistFogStart` es un ESTÁTICO (`=0x80437108`), no una local |
| `RegionQuery::CalculateRegionInfo` | 1.208 | `fogFalloffY` | fr0 | **NO existe** (71 locales) |
| `CarRenderInfo::CreateCarLightFlares` | 748 | `slot_base`, `raw_model`, `model` | r11/r0/r19 | **NO existen**: el original tiene `model` con **localización vacía** (optimizada fuera). Tres pines para una variable que el original ni siquiera mantiene viva |
| `EmitterSystem::Render` | 696 | `e36guard` | r19 | **NO existe** |
| `EmitterSystem::Render` | 696 | `sprite_hack_flags`, `world_size` | r24/fr6 | **existen, r24 y f6** |
| `TrackPathManager::FindZone` | 664 | `found_zone` | r26 | **existe, r26** |
| `TrackStreamer::GetPredictedZone` | 660 | `predict_position_used` | r28 | **existe, r28** |
| `ArrayScroller::ScrollVer` | 620 | `original_index` | r11 | **NO existe** — es una COPIA de `new_index`, que el original tiene en r31 |
| `MemoryPoolManager::NewBlockAux` | 84 | `blockStorage`, `poolFree`, `nextPoolFree` | r8/r11/r9 | **NO existen**: el original tiene **una** local, `r` (char*, r3) |

### Dos hallazgos de método

1. **La localización vacía es un tercer veredicto.** `SubTitler::GetElapsedTime`
   tiene `thetime_ms` en el DWARF… con **loc vacía: optimizado fuera**. El
   nombre existió en el fuente pero el original no la mantiene en ningún
   registro: pinarla a `fr1` es un parche igual que inventarla. Mismo caso
   `CarRenderInfo::CreateCarLightFlares` (`model`) y
   `TextureInfoPlatInterface::LockPalette` (`Pal32`).
2. **Las sobrecargas engañan al informe.** `report.json` empareja
   `eSolid::GetPostionMarker` con la sobrecarga equivocada (88 B en vez de
   92 B). Con la buena, `position_marker_table` **existe en r4** (grupo B) y
   `last_marker` **no existe**: el original declara `next_marker` a nivel de
   FUNCIÓN y en r3. Cualquier automatismo sobre `report.json` necesita
   comprobar el tamaño contra el `symtab` del DWARF.

Correcciones manuales aplicadas al automatismo (7 a grupo A, 3 a grupo B):
`FEngine.cpp:618`, `WeatherMan.cpp:182`, `CarRender.cpp:2398`,
`PhysicsObject.cpp:75`, `FEngInterfaceFEObjects.cpp:539` y `:596`,
`feArrayScrollerMenu.cpp:228` → A; `eSolid.cpp:368` → B; y
`WeatherMan.cpp:188` (`DistFogStart` es un GLOBAL) y `STICH_PlayBack.cpp:209`
(`this->ActiveSamplesRefs` es un MIEMBRO) → B, porque el DWARF sólo lista
locales y su ausencia ahí no prueba nada.

---

## 5. LISTA DE ATAQUE

### GRUPO A — el DWARF dice que la variable NO existe: hay que reescribir (53: 31 pines + 22 barreras)

Ordenado por bytes de `.text` que hoy se dan por terminados con una fuente
que no es la del original. **Este grupo es el trabajo real.**

| B | % | unidad | función | andamio A | fichero:línea |
|---:|---:|---|---|---|---|
| 3.080 | 100 | zFEng | `FEngine::ProcessPadsForPackage` | pin `padMask` r30 | `FEngine.cpp:618` |
| 2.544 | 100 | zGameplay | `GTrigger::GTrigger` | barrera `boxmat` | `GTrigger.cpp:92` |
| 1.908 | 100 | zSim | `QuickGame::CreateCars` | pin + barrera `collisionPos` | `QuickGame.cpp:304,308` |
| 1.684 | 100 | zAI | `AIPursuit::AssignClosestOffsets` | barrera `next` (dentro de un `__extension__({…})`) | `AIPursuit.cpp:1077` |
| 1.680 | 100 | zGameplay | `GRaceParameters::GenerateIndex` | barrera `f` | `GRaceDatabase.cpp:1078` |
| 1.592 | 100 | zEAXSound | `SFXCTL_Physics::UpdateNIS` | barrera `current_state` + clobber r9 | `SFXCTL_NISReving.cpp:355` |
| 1.412 | 100 | zPhysics | `PhysicsObject::PhysicsObject` | pin `bodyOwner` r3 en un `({…})` | `PhysicsObject.cpp:75` |
| 1.304 | 100 | zEAXSound2 | `SFXObj_Collision::InitSFX` | pin `collisionReverbSlot` r10 | `SFXObj_Collision.cpp:76` |
| 1.240 | 96,80 | zEAXSound2 | `CARSFX_RoadNoise::GenerateRoadNoise` | pin `slipBoost` fr10 | `CARSFX_Roadnoise.cpp:506` |
| 1.208 | 100 | zTrack | `RegionQuery::CalculateRegionInfo` | pines `fogStart` fr11, `fogFalloffY` fr0 | `WeatherMan.cpp:182,191` |
| 948 | 100 | zCamera | `TrackCopCameraMover::Update` | barrera `dst` | `TrackCop.cpp:372` |
| 808 | 100 | zFeOverlay | `CustomizeMain::NotificationMessage` | 3 barreras `mgrp`,`fe`,`pkg` | `CarCustomize.cpp:2066,2094,2096` |
| 796 | 100 | zEcstasy | `GenerateHorizonFogDisplayList` | barrera `half` | `EcstasyEx.cpp:953` |
| 748 | 100 | zWorld | `CarRenderInfo::CreateCarLightFlares` | 3 pines `slot_base`,`raw_model`,`model` | `CarRender.cpp:2387,2398,2409` |
| 696 | 100 | zEcstasy | `EmitterSystem::Render` | pin `e36guard` r19 + su barrera | `EmitterSystem.cpp:1360,1591` |
| 656 | 100 | avplayer | `RCMP::AV_PLAYER::GetFirstFrame` | barrera `audio_stream` | `avplayer.cpp:611` |
| 620 | 100 | zFe2 | `ArrayScroller::ScrollVer` | pin `original_index` r11 | `feArrayScrollerMenu.cpp:228` |
| 512 | 100 | zFe | `MemcardCallbacks::FoundEntry` | barrera `nm` | `MemoryCardCallbacks.cpp:354` |
| 404 | 100 | pathsnd | `PathToSnd::CreateStreamTrack` | barrera `framePad` | `pathsnd.cpp:46` |
| 400 | 100 | snddrv | `SNDDRV_audiocallback` | 2 pines `mixSource` r4 | `snddrv.c:599,617` |
| 340 | 100 | zEAXSound2 | `GinsuSynthData::BindToData` | pin `currentBlock` r0 | `ginsudata.cpp:165` |
| 332 | 100 | zFe | `cFEng::PushErrorPackage` | 2 pines `wasPaused` r0 | `FEngInterface.cpp:72,94` |
| 256 | 100 | sfsplit | `SFILTER_splitter` | pin + barrera `copySource`/`copySize` | `sfsplit.c:66,75` |
| 256×2 | 100 | zFe | `FEngSetScaleX` / `FEngSetScaleY` | 3+3: `object_reg` (copia del param), `initialScale`, su barrera | `FEngInterfaceFEObjects.cpp:539,549,559,596,604,606` |
| 232 | 100 | zEagl4Anim | `FnRawStateChan::FindTime` | pin `keyOffset` r9 | `RawStateChan.cpp:381` |
| 232 | 100 | zEAXSound | `SFXCTL_AccelTrans::UpdateParams` | pin + barrera `isAccelerating` | `SFXCTL_AccelTrans.cpp:114,115` |
| 172 | 100 | zMain | `SteeringWheelDevice::ConvertWheelRotation` | pin `linearScale` fr13 | `SteeringWheelDevice.cpp:327` |
| 168 | 100 | zCamera | `ICEManager::LoadCameraShakes` | barrera `guard` | `ICEManager.cpp:1014` |
| 116 | 100 | zFe | `SubTitler::GetElapsedTime` | pin `thetime_ms` fr1 (**optimizada fuera** en el original) | `SubTitle.cpp:130` |
| 92 | 100 | zFe2 | `FEKeyboard::ToggleCapsLock` | pin `off` r10 + barrera (el original tiene **sólo `this`**) | `FEpkg_MU_Keyboard.cpp:544,545` |
| 88 | 100 | zEcstasy | `eSolid::GetPostionMarker` | pin `last_marker` r9 (el original: `next_marker`, r3, ámbito de FUNCIÓN) | `eSolid.cpp:393` |
| 84 | 100 | zEagl4Anim | `MemoryPoolManager::NewBlockAux` | 3 pines (el original tiene **una** local, `r` r3) | `MemoryPoolManager.cpp:118,129,140` |
| 392/232 | 100 | csis | `Csis::System::Subscribe`/`Unsubscribe` | barreras `node`,`nullNode` — **sin DWARF**, pero sus pines son C | `csis.cpp:400,444` |

### GRUPO B — la variable existe: falta la FORMA DE FUENTE (61: 18 pines + 43 barreras)

El andamio reproduce algo real; lo que falta es escribir la sentencia que lo
produzca sola. Subgrupos:

- **18 pines sobre una variable que el original tiene en ESE MISMO registro**
  (`envmap_min_scale` f5, `vinyl_group_number` r21, `sprite_hack_flags` r24,
  `world_size` f6, `found_zone` r26, `predict_position_used` r28,
  `num_sections_unactivated` r30, `n` r28, `nBytesRead` r25, `magnitude` r27,
  `result` r25, `nopendingrequest` r30, `tableSize` r30, `lastSlash` r3,
  `originalVal` f0, `data` r31 ×2, `position_marker_table` r4).
  Aquí el pin no miente sobre la fuente: miente sobre el **reparto**. Palancas
  ya catalogadas: pin de registro → forma de fuente que suba `n_refs`,
  `REG_ALLOC_ORDER` para FPR, orden/ámbito de declaración.
- **23 barreras secas** (`__asm__("")` sin operandos, incluidas las cuatro
  `__asm__("# a"…)` de `RoadblockFlow.cpp` que ningún grep de `asm("")`
  encuentra): son barreras de ORDEN. Candidato legítimo documentado:
  `do { } while (0);`.
- **20 barreras con operandos que sí existen** (`Held`/`JoyMask` r19/r20,
  `a_lp` r31, `left_light_state`, `halfLen`/`pfir`/`sum`, `iTicksRemaining`,
  `did_swap` r28, `halfVP2` f0, `hcomp` `[r1+88]`, `iGuessSize`, `fDefault`…).

Tabla completa en `scratchpad/r76_censo/salida_resumen.txt`, sección
«GRUPO B».

### GRUPO C — sin oráculo, último recurso (9: 5 pines + 4 barreras)

| B | unidad | función | andamio | por qué no hay oráculo |
|---:|---|---|---|---|
| 628 | sndvd | `DSIHandler` | pin `savedContext` r29 | LibSN precompilada, sin CU |
| 392 | csis | `Csis::System::Subscribe` | pines `node` r7, `nullNode` r9 | CU sin DWARF |
| 388 | FSasync | `CompletePCreadAsync` | pin `block_high` r30 | LibSN |
| 232 | csis | `Csis::System::Unsubscribe` | pin `node` r10 | CU sin DWARF |
| 72 | ppc2D2 | `__cvt_fp2unsigned` | barrera `lim` | LibSN |
| — | zCamera+zEAXSound | `UTL::Vector::assign` (plantilla) | 2 barreras | cabecera; la instancia no tiene DIE propio |
| — | zGameplay | `PackedDecimal::PackedDecimal` | barrera `negative` | inline sin DIE |

Frontera del grupo: `Speech::RoadblockFlow::Setup` (596 B, 4 barreras
`__asm__("# a".."# e")`) tampoco tiene DIE, pero sus barreras son **secas**:
no atan ninguna variable, así que se atacan como grupo B (barrera de orden,
`do { } while (0);`) sin necesitar oráculo.

---

## 6. Reproducir

```
python scripts/censoasm.py                             # totales + tabla por fichero
python scripts/censoasm.py --clase PIN                 # detalle de una clase
python scripts/censoasm.py --json scratchpad/r76_censo/censo.json
python scripts/pctsnap.py -o pct.json zEcstasy zTrack  # refrescar unidades rancias
python scripts/dwarf1.py fn <funcion>                  # el oraculo
python scratchpad/r76_censo/{cruce,veredicto,barreras,x360b,falsos,grupos}.py
```

Nota sobre la frescura: `build/GOWE69/report.json` es del 16-sep 02:39 y hay
**17 objetos** más nuevos (la r75 y `zEagl4Anim` r75). Las 6 unidades con
andamio afectadas se remidieron con `pctsnap.py`; el resto del informe usa
`report.json` tal cual.
