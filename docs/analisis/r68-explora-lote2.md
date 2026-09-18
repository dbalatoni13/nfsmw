# r68 — exploración del lote 2: parches de DATOS en asm (82 bloques)

Solo lectura. No se ha tocado src/, config/, keep.lst ni build/. Los experimentos se compilaron y enlazaron en
`scratchpad/lote2_68/exp` (borrado al terminar; quedan los scripts `q.py`, `q2.py`, `q3.py`, `refs.py`,
`alloc_cmp.py` en `scratchpad/lote2_68/`).

Fuentes usadas en cada fila: símbolos del ELF original (`orig/GOWE69/NFSMWRELEASE.ELF`, con STT_FILE), variables
del volcado DWARF (`symbols/mw_dwarfdump.nothpp`, con su orden y `address:`), bytes del DOL original,
reubicaciones de los objetos extraídos (`build/GOWE69/obj/...`) y alineación de sección de nuestros objetos y de los
extraídos.

Resumen: **R1 2 · R2 36 · R3 12 · R4 32**.

---

## 1. Reglas nuevas, MEDIDAS

### 1.1 El estripado quita `size & ~(alineación de la SECCIÓN − 1)` por delante (no siempre `& ~7`)

Mini-TU con variables muertas y enlace real con `ngcld -strip-unused-data -keep` (solo se conserva `f__Fv`):

| .data de la TU | muerta | resultado en el enlace |
|---|---|---|
| alineada a 4 (t2, t3) | `float` 4 B, `float[3]` 12 B, `float[5]` 20 B, `int[4]` 16 B, `bool`, objeto asm local o global de 12 B | **desaparecen enteras** (símbolo y bytes) |
| alineada a 8 (t4, con un `double` vivo) | `float` 225.0 y 300.0 (4 B) | **se quedan enteras**, sin símbolo |
| alineada a 8 | `float[5]={1,2,3,4,20}` | se quedan los **4 últimos B** (20.0f) |
| alineada a 8 | `float[3]`, `S60` (60 B) | se queda la cola de 4 B (7.0f, 15) |
| alineada a 8 | `float[2]` (8 B) | desaparece entera |
| .rodata alineada a 4 | `$LC` de 22 B y de 14 B | cadáver de 2 B (`"_\0"`, `"r\0"`), no de 6 |

Con esto encajan las dos medidas del proyecto que parecían contradecirse: el "cadáver `size & ~7`" (medido en
.rodata, que va alineada a 8) y "un hueco de 4 B se estripa entero" (zBWare y el `static float` de SunE, con .data
alineada a 4). Además:

- un símbolo **sin tamaño** (`.globl` sin `.size`) no se estripa nunca (t3, `nosz12`);
- una entrada de keep.lst que nombra un símbolo inexistente **no da error** y el enlace sale idéntico (t4b);
- con keep.lst, una variable C muerta se conserva **entera** y en su sitio (t2k4: `dead4`, `dead12`).

### 1.2 En este compilador `bool` ocupa 4 B en el objeto

`deadb`/`usedb` salen con `.size 4` y valor `00000001` (t2.o). El DWARF dice `size: 0x1`, pero lo que manda es el
objeto. Por eso tres bools muertos del original ocupan 12 B.

### 1.3 En el original, los huecos de .data con nombre en el DWARF son COLAS de variables muertas

El DWARF marca `address: 0xFFFFFFFF` en una definición de nivel superior cuando **el enlazador original se la
llevó**. En el orden del volcado, cada hueco cae entre sus dos vecinas vivas y **los bytes cuadran con la regla 1.1
aplicada a una .data alineada a 8**:

- `Platform_G` 8041F72C, 12 B a cero: `TweakDumpVMStats`, `TweakResetVMStats` y `UseAllMemWhenRunningIn48MBMode`.
  Son 3 bools de 4 B.
- `JoyE` 8041F818, `38 BF 40 00 | 0C 08 0C 08 | 00 00 00 03`:
  - `DigitalAnalogScannerTriggerThresholdMin/Max` y `ButtonThresholdMin` son 1 B cada una (0x38, 0xBF, 0x40) y se
    quedan enteras;
  - `ValidJoystickConfigurationTable[3]` (60 B) deja su cola de 4 B;
  - `NumValidJoystickConfigurations` = 3 se queda entera;
  - `RawRumbleValues[6][4]`, `RawRumbleValuesUpdated[4]` y `JoystickPhase[4]` son múltiplos de 8 y desaparecen.
- `JoyE` 8041F82C: `JoystickRingBufferPreparedTop`.
- `JoyE` 8041F834 `{0, 0x3C, 0x1E}`: `ReadJoystickDataInVBlank` = 0, `RumbleStartRange` = 60, `RumbleStopRange` = 30.
- `SunE` 8041F84C = 0.5f: `SunBlend`.
- zAI 804153F8, 16 B: `bSpawnRacer`, `gRacer`, `bAiDriveReplay` y `bAiDriveReplayOld`.
- zAI 8041541C `{2.0f, 6.0f}`: `yDeltScale` y `DestColliderRadius`.
- zAI 80415260: `AICopManager::mCopCarNow`.
- zAI 804152A8: `ForcePursuitStart`.
- zAI 804152C0 = 85.0f: `RoadNetSpeed`.
- zAI 804152C8: `bDownVelTest`.
- zAI 80415224/28 = 225.0f/300.0f: `AITrafficManager::mTrafficMinSpawnDist` y `mTrafficMaxSpawnDist`. Nuestra
  fuente los define a 0.0f.
- zMisc, trece huecos con nombre exacto (fila zMisc.cpp:3699).
- Ninguno de los `lbl_` de estos huecos recibe reubicación en el objeto extraído (`refs.py`): son datos muertos,
  no etiquetas vivas.

### 1.4 La alineación de la sección original se lee en la dirección de arranque

| unidad | .data arranca en | ¿múltiplo de 8? | objeto extraído | nuestro objeto |
|---|---|---|---|---|
| zAI | 80415180 | sí | 8 | **4** |
| zPlatform | 8041F728 | sí | 8 | **4** |
| zSpeech | 80435970 | sí | 8 | 8 (la da el `.balign 8` del asm de 80435E78) |
| zMisc | 8041E508 | sí | 8 | 8 (la da el `.balign 8` del bloque) |
| zMain | **8041D7EC** | **no** | 4 | 4 |

Consecuencias:

- **zMain no pudo tener .data alineada a 8.** Sus huecos de .data no son colas de variables muertas: en una sección
  alineada a 4, una variable muerta desaparece entera.
- **En zSpeech y zMisc una variable C muerta de 4 B reproduce el byte sin keep.lst.**
- **En zAI y zPlatform no:** nuestra .data está alineada a 4 y la variable se iría entera. Hace falta la alineación 8
  del original (el dato que la da no lo he identificado) o una entrada de keep.lst por nombre real. keep.lst solo
  sirve cuando la variable mide menos de 8 B; para una cola (tabla de JoyE, `aAiSeparationMax`) hace falta la
  alineación.

### 1.5 El original no tenía símbolos `$LC`

`NFSMWRELEASE.ELF` tiene 1.711 símbolos en .rodata y **ningún `$LC`**; nuestro `main.elf` tiene 1.250. Como un
símbolo sin tamaño no se estripa (1.1), eso explica que las cadenas muertas del original sigan **enteras**: VIRTUAL,
(unknown), Attrib::Gen::*, GRaceStatus, aud_moment_strm. Nuestro cc1plus les da `.type/.size`, así que nuestras
copias necesitan `# @lc`, igual que ya hacen zMain (79 entradas), zSpeech (95) y zAI (51).

### 1.6 NgcAs rellena la sección hasta su alineación

`static T *g __attribute__((section(".sdata"), aligned(8))) = 0;` sale con `.sdata` de 8 B (a1.o). Sin `aligned`,
4 B (a2.o). Compilado allocator.cpp con y sin su asm: `.text` y `.sdata` son **idénticas byte a byte**; solo cambian
`.symtab`, `.strtab` y los índices de `.rela.text`.

### 1.7 Dos literales iguales en una misma TU salen siempre una sola vez

Medido con dos `static inline` y con `inline` más array `static const char[]`. El `"%s  %s"` duplicado de pathbank no
tiene forma C conocida. El primer muerto con `""` y luego `"Assert: %s:%i %s (%s)"` los emite en ese orden con
`.align 2` (p3).

---

## 2. Patrones

| patrón | bloques | casos comprobados |
|---|---|---|
| **A. Cola de variable muerta en .data alineada a 8** (reglas 1.1 y 1.3) | 16 R1/R2 + 7 R4 parciales | Platform_G, JoyE ×3, SunE, zAI 804153F8/8041541C/80415260/804152A8/804152C0/804152C8, SpeechManager:124, zMisc (13 huecos) |
| **B. Cadena muerta de cabecera, sin símbolo en el original** (1.5) | 22 (R2) | zMain ×12 más E911Call, zAI:639, zPlatform:171, zSpeech 142/161/289, SpeechManager:2155, InputDeviceGC:196, rcmpbase |
| **C. Prefijo bWare/STL de 92 B** (GAMECUBE, `__FILE__` "d:/mw/speed/indep/bware/inc/bware.hpp", bad_alloc, %f,%f,%f, %f,%f,%f,%f, STL) | 7 (R3) | zAI:78, zMain:41, zSpeech:38, zOnline:28, y como cabeza de zPlatform:5, zGameModes:15 y zMisc:65 |
| **D. Constantes o cadenas cuyo origen no está en el árbol** (pool de un cuerpo estripado, printf de depuración) | 13 (R4) | InputDevice:25, EventSequencer ×2, EAIEngineRev/ENISBrakelock, actionqueue:156, SteeringWheel:271, InputDeviceGC:579, zMain:512/824, Scheduler:34 |
| **E. Huecos de .data de zMain** (sección a 4: no pueden ser colas) | 5 (R4) | zMain 715/734/864/883, SteeringWheelDevice:22 |
| **F. Compensadores de tamaño**, no son dato del original | zAI:718, `.space 64` de zSpeech:289, zPlatform:248 | — |
| **G. No son datos** | sndvd `.long 1` (código), libsn.h ×3 (macros de trampa de ProDG que ninguna unidad compila), ALIAS `_overlay_start` | — |
| **H. Relleno de sección automático** | allocator (R1) | a1/a2 y allocator con y sin el asm |

---

## 3. Tabla bloque a bloque

Tests: unidad promocionada → reenlace y DOL 9619ba57. Unidad pendiente → `dolwhere <unidad>` sin empeorar y con las
secciones cuadrando. "@lc" = entrada `# @lc <unidad> "<cadena>"` en keep.lst más `lcfix`. "align8" = que nuestra
.data de la unidad salga alineada a 8, como la del original.

| fichero:línea | clase | qué era en el original (y de dónde) | veredicto | C o paquete propuesto | test | riesgo |
|---|---|---|---|---|---|---|
| src/LibSN/sndvd.c:333 | HUECO 4B | `.long 1` DENTRO de .text: opcode ilegal que salta a la trampa del Target Manager. Es código, no dato | R3 | ninguno (medido r24: `__builtin_trap` da `tw`, y es noreturn) | — | — |
| Packages/vp6/.../allocator.cpp:41 | HUECO 4B gap_08_804FF5A4_sdata | Relleno de sección: el DWARF de allocator.cpp solo tiene `gAllocator`; la .sdata de 8 B la da `aligned(8)` (1.6) | **R1** | borrar el `asm(...)`; no hay entrada en keep.lst | DOL 9619ba57 (objeto medido: .text/.sdata idénticas) | ninguno visible |
| Speed/GameCube/Src/JoyE.cpp:20 | DATOS 12B lbl_8041F818 | `unsigned char DigitalAnalogScannerTriggerThresholdMin=0x38, ...Max=0xBF, ...ButtonMin=0x40` + cola de `ValidJoystickConfigurationTable[3]` (0C080C08) + `NumValidJoystickConfigurations=3`, todos 0xFFFFFFFF en el DWARF (L1866960, L1871905-7) | R2 | esas 5 definiciones en JoyE.cpp en el orden del objetivo (solo importan los 4 últimos B de la tabla) + align8 | dolwhere zPlatform; comprobar el orden tabla/umbrales | la cola solo sale con align8, keep.lst no la da; orden a confirmar |
| JoyE.cpp:40 | HUECO 4B gap_06_8041F82C | `int JoystickRingBufferPreparedTop` muerta (L1871912) | R2 | `int JoystickRingBufferPreparedTop = 0;` + (align8 o keep `zPlatform.o:JoystickRingBufferPreparedTop`) | dolwhere zPlatform | con .data a 4 y sin keep se va entera |
| JoyE.cpp:59 | DATOS 12B lbl_8041F834 | `ReadJoystickDataInVBlank=0`, `RumbleStartRange=60`, `RumbleStopRange=30` muertas (L1871914, L1871919-20) | R2 | tres `int` + align8/keep | dolwhere zPlatform | idem |
| Speed/GameCube/Src/Platform_G.cpp:52 | HUECO 12B gap_06_8041F72C | `bool TweakDumpVMStats, TweakResetVMStats, UseAllMemWhenRunningIn48MBMode` muertas (L1871768-72), 3×4 B | R2 | tres `bool = false;` tras snProfilerEnable + align8/keep | dolwhere zPlatform | idem |
| Speed/GameCube/Src/Render/SunE.cpp:45 | DATOS 4B lbl_8041F84C+gap | `float SunBlend = 0.5f` muerta (L1872011) | R2 | `float SunBlend = 0.5f;` entre SunVisibility y DoSunVisibility + align8/keep | dolwhere zPlatform | la nota r63 ("static float se va entero") es la regla 1.1 con .data a 4 |
| Speed/GameCube/Src/xSparks.cpp:101 | HUECO 4B gap_06_8043573C | Sin candidata en el DWARF entre randomSeed y gComment1 | R4 | — | decide: variables locales estáticas muertas de xSparks/MemoryCardImp en el volcado (entradas con sangría) | — |
| .../SN/include/libsn.h:100 | HUECO 4B | macro `snFlushTtyBuffer` → `asm(" .long 4")`. NINGUNA unidad la usa: no es dato | R3 | ninguno (cabecera del SDK) | — | — |
| libsn.h:98 | HUECO 4B | `snPause` → `.long 1`; NINGUNA | R3 | ninguno | — | — |
| libsn.h:99 | HUECO 4B | `snPollHost` → `.long 2`; NINGUNA | R3 | ninguno | — | — |
| Speed/Indep/Libs/path/.../pathbank.cpp:312 | HUECO 16B | Dos `"%s  %s"` muertos seguidos (80413A38 y 80413A40) | R3 | GCC funde los literales iguales de una TU (1.7); forma desconocida | — | — |
| SourceLists/zAI.cpp:312 | HUECO 172B pad_06_80415180 | Últimos 8 B = `AITrafficManager::mTrafficMinSpawnDist=225f` y `mTrafficMaxSpawnDist=300f`, muertas (L128408-9); los 164 B de delante sin candidata | R4 (8 B son R2) | 8 B: cambiar los inicializadores de AITrafficManager.cpp:37-38 y quitar 8 B del pad + align8/keep | decide: DWARF de zAI antes de L132732 (anónimos y locales estáticas) | nuestro =0.0f hoy se estripa entero |
| zAI.cpp:400 | HUECO 44B gap_06_80415230 | Sin candidata entre RandomSortTCDir y ICopMgr::mDisableCops | R4 | — | decide: variables muertas de las cabeceras AIVehiclePursuit/CopCar/Target/RoadBlock | — |
| zAI.cpp:421 | HUECO 4B gap_06_804152A8 | `bool ForcePursuitStart` muerta (L135448, detrás de crossvblockgoals) | R2 | `bool ForcePursuitStart = false;` al final de las tablas de AICopManager.cpp + align8/keep | dolwhere zAI | bool = 4 B: con .data a 4 se va entera |
| zAI.cpp:444 | DATOS 4B lbl_804152C0 | `float RoadNetSpeed = 85.0f` muerta (L135554, AIActionHeliPursuit) | R2 | `float RoadNetSpeed = 85.0f;` + align8/keep | dolwhere zAI | `bTrackY` (L135548) tiene que ir a .bss (sin inicializador) |
| zAI.cpp:457 | HUECO 4B gap_06_804152C8 | `bool bDownVelTest` muerta (L135559) | R2 | `bool bDownVelTest = false;` tras NeverIgnoreHeliSheet + align8/keep | dolwhere zAI | idem |
| zAI.cpp:486 | DATOS 8B lbl_804152DC | {0, 0.5f}: `fDragDifficulty` muerta (L135585) es candidata a 0.5f; el primer word no tiene candidata | R4 | — | decide: DWARF de AIActionStaticRoadBlock.cpp | — |
| zAI.cpp:508 | HUECO 8B gap_06_80415370 | `ForcePursuitHeatLevel` muerta (4 B) + 4 B sin candidata | R4 | — | idem | — |
| zAI.cpp:521 | HUECO 16B gap_06_804153F8 | `bool bSpawnRacer`, `HSIMABLE* gRacer`, `bool bAiDriveReplay`, `bool bAiDriveReplayOld`, todas muertas (L135811-17): 4×4 = 16 B a cero | R2 | las cuatro definiciones en AIVehicleRacecar.cpp tras PidIntegralData + align8/keep | dolwhere zAI | idem |
| zAI.cpp:543 | HUECO 4B gap_06_80415414 | Sin candidata entre gHeliVehicle y kHeliVisualSphere | R4 | — | decide: DWARF de AIVehiclePursuit.cpp | — |
| zAI.cpp:604 | DATOS 68B lbl_80415438 | Candidatas muertas desde L136001 (DebugCopFormation=1, CopFormationOverriden, giOverrideMaxCops, numberIPV_misses, ForcePursuitNeverEnd, ForcePursuitBail, kTimePerPerpHeatBump, ForcePursuitEnd, testPursuitBar, gHideGPSArrow = 40 B); faltan 28 B y el 120.0f del word 14 | R4 | — | decide: DWARF de AIPursuit/AIRoadBlock/AIRoadBlockSetups | — |
| zAI.cpp:639 | HUECO 761B (asciz) | 56 cadenas muertas de cabecera (WorldConn.h, MGeneric.h, Attrib::Gen::*, M*.h), sin símbolo en el original (1.5) | R2 | colocarlas por orden de parseo (cabeceras/primer) + @lc de las que el enlace estripa; quitar el bloque y su compensador (zAI.cpp:718) | dolwhere zAI y lcfix --check | renumeración de $LC; cadáveres parásitos (r58: 11 cadenas que el objetivo no tiene) |
| zAI.cpp:699 | HUECO 40B gap_06_80415498 | Relleno final tras Gps::mInstance, sin candidata | R4 | — | decide: DWARF muertas de Gps.cpp/AIMath.cpp | — |
| zAI.cpp:718 | HUECO 280B (.space) | Compensador de tamaño de la .rodata (r61), no es dato del original | R2 | se retira dentro del paquete de zAI.cpp:639 | dolwhere zAI | hay que descontarlo byte a byte |
| zAI.cpp:78 | DATOS 92B lbl_803C8C60 | Prefijo bWare/STL: literales de inlines de bWare.hpp y de _alloc.h; el `__FILE__` es de la máquina de EA; sin símbolo en el original | R3 | forma C desconocida: "%f,%f,%f" y el usuario de `__FILE__` no existen en el árbol (con eso más `#line` pasaría a R2) | — | — |
| SourceLists/zGameModes.cpp:15 | DATOS 848B pad_05_803EBB48+2 lbl | Prefijo bWare + etiquetas Attrib + Gen::effects/audioimpact/audioscrape/simsurface + paquetes WorldConn + cadenas y floats de EmotionManager (cuerpos estripados por el enlace) | R3 (el prefijo; el resto es R2) | resto: literales naturales de EmotionManager.cpp/WorldConn.h + @lc; los floats de 4 B sobreviven solos (.rodata a 8) | DOL 9619ba57 | unidad promocionada; lbl_803EBE90 está vivo |
| SourceLists/zMain.cpp:117 | HUECO 80B | "Attrib::Gen::effects/audioimpact/audioscrape": etiquetas USE_ATTRIB_ALLOC de las cabeceras generadas, muertas y sin símbolo | R2 | quitar ATTRIB_TAGS_HAND_POOL en ese tramo (como hace zAI.cpp:274) + @lc de las 3 | DOL 9619ba57 + lcfix --check | renumera las 79 @lc de zMain; etiquetas parásitas (induction/transmission) |
| zMain.cpp:162 | HUECO 92B | Gen::speech/pvehicle/engine/engineaudio (ATTRIB_TAG) | R2 | idem + @lc | DOL | idem |
| zMain.cpp:191 | HUECO 280B | Gen de persecución, ecar, camerainfo, audiosystem, "GRaceStatus", SMS_MESSAGE_%d(_FROM/_VOICE/_SUBJECT) (DEAD_STR de FEDatabase.hpp) | R2 | ATTRIB_TAG + DEAD_STR naturales + @lc | DOL | DEAD_STR apagado cambia código (medido en zSpeech r59) |
| zMain.cpp:222 | HUECO 40B | "VehicleParams" (PVehicle.h, DEAD_STR) + Gen::speechtune | R2 | idem | DOL | idem |
| zMain.cpp:301 | HUECO 24B | Gen::visuallook | R2 | ATTRIB_TAG + @lc | DOL | idem |
| zMain.cpp:330 | HUECO 100B | CarSoundConn, Pkt_Car_Open, Pkt_Car_Service, HeliSoundConn, Pkt_Heli_Open, Pkt_Heli_Service (DECLARE_*PACKET de una cabecera de conexión de sonido) | R2 | incluir esa cabecera en su punto + @lc | DOL | orden de vtables e inlines (r58) |
| zMain.cpp:41 | DATOS 92B lbl_803EF580 | Prefijo bWare/STL | R3 | como zAI.cpp:78 | — | — |
| zMain.cpp:441 | HUECO 20B | Gen::world | R2 | ATTRIB_TAG + @lc | DOL | idem |
| zMain.cpp:463 | HUECO 28B | Gen::rigidbodyspecs | R2 | idem | DOL | idem |
| zMain.cpp:512 | HUECO 36B | "High","Medium","Low","Reflection","?": no están en ninguna cabecera del árbol | R4 | — | decide: buscar el enum-a-cadena del original (debug_lines de ENISDetail) | — |
| zMain.cpp:558 | HUECO 56B | Gen::emittergroup/emitterdata | R2 | ATTRIB_TAG + @lc | DOL | idem |
| zMain.cpp:715 | HUECO 4B .data 8041D804 | .data a 4 (1.4): no es cola de variable muerta. Sin candidata | R4 | — | decide: qué dato vivo sin símbolo precede a gMakeEventCallbacks | — |
| zMain.cpp:734 | HUECO 4B .data 8041DFA0 | El DWARF da `static bool gVerbose` (namespace Query) EN 8041DFA0; el ELF no le da símbolo | R4 | candidata `static bool gVerbose = false;` en QueryDefs.cpp, pero GCC 2.95 no emite una estática sin uso | decide: qué uso la emite | — |
| zMain.cpp:74 | HUECO 40B | Gen::simsurface + "Attrib::TAttrib" | R2 | ATTRIB_TAG + @lc | DOL | idem |
| zMain.cpp:752 | HUECO 956B | Las 45 etiquetas Attrib::Gen::* de codegen.cpp | R2 | ATTRIB_TAG natural + 45 @lc | DOL | el mayor bloque de @lc |
| zMain.cpp:824 | HUECO 12B | "<unknown>" entre action.cpp y actionqueue.cpp; no está en el árbol | R4 | — | decide: fuente de action.cpp/actionqueue.h del original | — |
| zMain.cpp:864 | DATOS 12B .data lbl_8041E470 | {0x2E28,0,0}, sin reubicaciones; con .data a 4 no es cola. Candidata `VirtualMemory::fgStats` (C, muerta) no cuadra con la regla | R4 | — | decide: medir si fgStats/fgDebugDisplay están vivas en el .text del objetivo | — |
| zMain.cpp:883 | HUECO 32B .data 8041E4E8 | Ceros al final de la .data; con .data a 4 no son colas | R4 | — | decide: DWARF de SteeringWheelDevice.cpp (sPedalScale muerta) | — |
| zMain.cpp:98 | HUECO 104B | Gen::gameplay/milestonetypes, versiones 16.1.0/16.2.1/1.2.3/1.8.1/19.8.31, "GManager" (DEAD_STR de GRaceDatabase.h/GManager.h) | R2 | ATTRIB_TAG + DEAD_STR naturales + @lc; las de ≤7 B sobreviven solas | DOL | idem |
| SourceLists/zMisc.cpp:3699 | DATOS 779B (toda la .data) | Globales con nombre del ELF más 13 huecos, todos variables muertas del DWARF: 8041E5A4 CurrentTicksDiff; 8041E5AC SaveTheLoopCounter1/2; 8041E5CC Previous{Cpu,Gpu}Frame{Time,Rate}, PreviousSimFrameTime, PreviousRenderFrameTime, PreviousSimCallCount, PreviousProfileTime, DisplayPolyCount, gDoFrameRateSummary (40 B exactos); lbl_8041E604 MainCycleCounter=0x17, tframe, tframe30, over30, DisplayOver30, ResetOver30 (24 B); 8041E72C PrintChunks; 8041E7B0 pMemorySpongeMemory; 8041E7C0 MemoryProfileFile; 8041E7DC SwappedFrenchTextures; 8041E7E4 PermanentMemoryFile; 8041E7F0 ExitDemoResourcesLoaded + ExitDemoDiscJoyHandlerHit; 8041E804 bShakeTest | R2 | pasar la .data a C en sus .cpp (Main.cpp, Timer.cpp, ResourceLoader.cpp…) con las muertas en su sitio. .data a 8 en los dos lados: **las muertas no necesitan keep.lst** | dolwhere zMisc | es un solo bloque; punteros a $LC y StomperTable; mantener la alineación 8; quedan por mapear gap_8041E50C (136 B: nQuickSpline* son 8), 8041EA38, 8041EA7C, 8041EA9C, 8041EC24, 8041ED40 y lbl_8041EA40/EA8C/EABD/ED54 |
| zMisc.cpp:4080 | DATOS 44B (.bss) | `frames_elapsed`, `loop_ticker`, `bFileMutex` son globales reales (.bss en el DWARF); 804800C4 y 804801A0 sin candidata | R2 (los 2 huecos R4) | definiciones C en su orden (con nombre el orden sí manda) | dolwhere zMisc | .bss no viaja en el DOL: solo cuenta el tamaño |
| zMisc.cpp:65 | DATOS 7329B (pool .rodata) | Prefijo bWare + etiquetas + pool de la unidad (cadenas referenciadas por tablas de .data) | R3 (el prefijo; el resto R2) | resto: literales naturales + @lc (hoy zMisc no tiene ninguna) + orden | dolwhere zMisc | tamaño; 150 cadenas duplicadas (deadstr) |
| SourceLists/zOnline.cpp:21 | ALIAS 0B _overlay_start | Marca de arranque de .over de tamaño cero: no es dato | R4 | — | decide: compilar `char _overlay_start[] __attribute__((section(".text"))) = {};` y ver el símbolo | DOL (unidad promocionada) |
| zOnline.cpp:28 | DATOS 92B lbl_803a4234… | Prefijo bWare/STL de una unidad sin código vivo | R3 | como zAI.cpp:78 | — | — |
| SourceLists/zPlatform.cpp:171 | HUECO 600B (asciz) | 38 cadenas de cabecera (Attrib, WorldConn.h, MGeneric, M*, bba_com.cpp...) en el orden del objetivo pero colocadas al final | R2 | juego de cabeceras en su punto de parseo + @lc; nota r64: bloquean 11 literales parásitos | dolwhere zPlatform | alto; cadáveres que keep.lst no puede quitar |
| zPlatform.cpp:229 | HUECO 36B gap_06_80435744 | Final de la .data tras gComment1, sin candidata | R4 | — | decide: DWARF de MemoryCardImp.cpp tras L1873050 | — |
| zPlatform.cpp:248 | HUECO 32B (.bss) | Tamaño de .bss (NOBITS), sin candidata | R4 | — | decide: .bss muertas con cola en el DWARF de zPlatform | alineación de .bss (medido r59) |
| zPlatform.cpp:5 | DATOS 820B lbl_80402108 | Prefijo bWare (92 B) + 728 B de etiquetas Attrib y paquetes WorldConn | R3 (el prefijo; el resto R2) | resto: cabeceras + @lc | dolwhere zPlatform | idem |
| SourceLists/zSpeech.cpp:142 | HUECO 6B asciz "1.8.1" | Cadena muerta de versión entre "1.2.3" y "19.8.31" (en el árbol vive en FEDatabase.cpp); 6 B: en .rodata a 8 sobrevive entera sin keep | R2 | internarla en su punto de parseo (cabecera o primer) y borrar el asm | dolwhere zSpeech | negativo r59 (13ª cadena del primer: +8 y .text reordenado) medido antes de este asm: veda que puede estar caducada |
| zSpeech.cpp:161 | HUECO 12B "GRaceStatus" | Cadena muerta de cabecera (GRaceStatus) | R2 | literal en su punto + @lc (12 B deja cadáver) | dolwhere zSpeech | renumeración |
| zSpeech.cpp:245 | HUECO 20B gap_06_80435E78 | Entre la última InterfaceId (80435E70) y Observer; sin candidata | R4 | — | decide: DWARF de Observer.cpp | **su `.balign 8` es lo que alinea a 8 nuestra .data de zSpeech** |
| zSpeech.cpp:269 | HUECO 36B gap_06_80435F24 | Tras NumberOfColourHashToSoundColourMaps (MiscSpeech), final de la unidad; sin candidata | R4 | — | decide: DWARF de MiscSpeech.cpp | — |
| zSpeech.cpp:289 | HUECO 105B (asciz + .space 64) | "GManager" (GManager.h), "MForcePursuitStart" (AICopManager.hpp), "MinHeatLevel" en el sitio equivocado + 64 B de compensador | R2 | cadenas en su punto + @lc; el `.space` se retira | dolwhere zSpeech | DEAD_STR entero cambia código (r59) |
| zSpeech.cpp:38 | DATOS 92B lbl_80405E10 | Prefijo bWare/STL | R3 | como zAI.cpp:78 | — | — |
| Src/AI/Actions/AIActionRace.cpp:950 | DATOS 12B lbl_80415348 | {20.0f,0,0}: el primer word es la cola de `aAiSeparationMax[5]` muerta (L135624; 20 B → queda 1); 8 B sin candidata | R4 | cola: `float aAiSeparationMax[5] = {…, 20.0f};` con align8 | decide: DWARF de AIActionRace.cpp | AiSeparationMaxTable (.bss) la referencia: comprobar que la construcción estática esté muerta |
| Src/AI/Activities/AICopManager.cpp:68 | HUECO 4B gap_06_80415260 | `static int mCopCarNow` de AICopManager, muerta (L128468, L68720) | R2 | `int AICopManager::mCopCarNow = 0;` entre mDisableCops y mCopMinSpawnDist (declarada en la clase si falta) + align8 o keep `zAI.o:_12AICopManager.mCopCarNow` | dolwhere zAI | toca AICopManager.hpp |
| Src/AI/Common/AIVehicleHelicopter.cpp:18 | DATOS 8B lbl_8041541C | `float yDeltScale = 2.0f; float DestColliderRadius = 6.0f;` muertas (L135872-3) | R2 | esas dos líneas tras kHeliVisualSphere + align8/keep | dolwhere zAI | idem |
| Src/EAXSound/Stream/SpeechManager.cpp:124 | HUECO 4B gap_06_8043597C | `int SPEECHFLOW_DISPLAY_DITCHED` muerta (L2116604), entre SPEECH_DISPLAY_HISTORY (línea 80) y Manager::m_SpeechModule | **R1** | `int SPEECHFLOW_DISPLAY_DITCHED = 0;` en el sitio del asm. .data a 8 en los dos lados: sin keep | dolwhere zSpeech sin empeorar; .data 0x5D8 | la alineación 8 la da el asm de zSpeech.cpp:245; si se quita antes, la variable se va entera |
| SpeechManager.cpp:1534 | HUECO 12B gap_06_804359C4 | Tras max_samplerequests y antes de TRACKSTREAMER_BACKLOG_THRESH; sin candidata de nivel superior | R4 | — | decide: locales estáticas muertas de Manager en el volcado | — |
| SpeechManager.cpp:2155 | HUECO 29B asciz | "Attrib::Gen::aud_moment_strm": etiqueta de aud_moment_strm.h (existe en el árbol) | R2 | incluir aud_moment_strm.h antes de MGamePlayMoment.h con ATTRIB_TAG + @lc | dolwhere zSpeech | 29 B dejan cadáver sin @lc |
| Src/Generated/Events/E911Call.cpp:8 | HUECO 12B "MGeneric" | Literal `_GetKind` de MGeneric.h | R2 | MGeneric.h en ese punto + @lc | DOL 9619ba57 | renumeración en zMain |
| Generated/Events/EAIEngineRev.cpp:39 | HUECO 4B | 4 ceros tras "EAIEngineRev"; candidatas: `""` muerto (se queda entero) o 0.0f de un cuerpo estripado | R4 | — | decide: compilar la TU y ver qué $LC muerto nace ahí | DOL |
| Generated/Events/ENISBrakelock.cpp:33 | HUECO 4B | idem, tras "ENISBrakelock" | R4 | — | idem | DOL |
| Src/Input/Common/InputDevice.cpp:25 | HUECO 48B | 12 floats {1,.5,0,.5,-.5,1,0,.5,0,1,.5,-.5}: pool de una función que el enlace estripó | R4 | — | decide: subprogramas de InputDevice/InputMapping en el DWARF que falten en el árbol | DOL |
| Input/Common/SteeringWheelDevice.cpp:22 | HUECO 4B .data 0x37800080 | Sin reubicaciones; .data de zMain a 4 → no es cola | R4 | — | decide: qué dato vivo (sPedalScale está muerta) | DOL |
| SteeringWheelDevice.cpp:271 | HUECO 8B "LGWheel" | Cadena muerta; no está en el árbol | R4 | — | decide: origen (LGWheels.h del original) | DOL |
| Input/Common/actionqueue.cpp:156 | HUECO 40B | "ACTIONQUEUE: %3d  Port %d, id =%2d  %s": formato de printf de depuración; no está en el árbol | R4 | — | decide: función de volcado de ActionQueue en el DWARF | DOL |
| Input/Common/iomodule.cpp:28 | HUECO 48B | Constantes estáticas de VirtualMemory muertas en el orden del DWARF (L1425863-70): kDefaultSizeARAM=0x200000, kDefaultSizePhysical=0x80000, kDefaultAddressPhysical=0x7E000000, kDefaultAlign=0x1000, kAlign=0x1000, kInfoSize=0x40, kUseSentinel=true (4 B), más "VIRTUAL" (de kMemClassName) y "(unknown)" | R2 (los 10 B de "(unknown)" son R4) | `const unsigned int VirtualMemory::kDefaultSizeARAM = 0x200000;`… en VirtualMemory.cpp (declararlas en la clase); en .rodata a 8 sobreviven sin keep; @lc "VIRTUAL" | DOL 9619ba57 | toca la cabecera de VirtualMemory; renumeración |
| Input/GC/InputDeviceGC.cpp:196 | HUECO 24B | `""` + "GameDevice(port)" muertas (el literal existe en InputDeviceGC.cpp) | R2 | literal en su uso natural + @lc | DOL | idem |
| InputDeviceGC.cpp:579 | HUECO 32B | "GameDevice::SteeringWheelDevice"; no está en el árbol | R4 | — | decide: origen | DOL |
| Src/Main/Common/EventSequencer.cpp:248 | HUECO 12B | 12 ceros tras el pool de System::IsPaused; candidatas: floats 0.0 de un cuerpo estripado | R4 | — | decide: subprogramas muertos de EventSequencer | DOL |
| EventSequencer.cpp:8 | HUECO 36B | 0,0,1.0,-1.0,0,0 + "EventBuffer"; "EventBuffer" no está en el árbol | R4 | — | idem | DOL |
| Src/Main/Common/Scheduler.cpp:34 | HUECO 10B "Scheduler" | Viva en el original (nota r59); aquí nace muerta | R4 | — | decide: qué llamada del original pasa "Scheduler" (¿new con nombre?) | DOL |
| Src/Speech/SoundAI.cpp:57 | HUECO 4B gap_06_80435EA4 | Entre Singleton<SoundAI>::mInstance y DESTROY_COPS_ON_INACTIVITY; sin candidata | R4 | — | decide: DWARF de SoundAI.cpp | — |
| egami/rcmp/.../rcmpbase.cpp:2 | HUECO 28B | `""` (4 B) + "Assert: %s:%i %s (%s)" + 2 B: literales del assert de RCMP, muertos, en cada TU de egami | R2 | la inline de cabecera del assert de RCMP, o primer (medido p3: sale en ese orden) + @lc de "Assert…" (22 B deja cadáver) | DOL 9619ba57 | igual en avplayer, rcmp_mad_codec, rcmp_vp6_codec y bigswizzler: paquete común |

---

## 4. Dudas (R4) y la medida que las decide

- **Huecos de zAI sin candidata** (80415230 44B, 804152DC 4B, 80415370 4B, 80415414, 80415438 28B, 80415498,
  cola de 80415348 y los 164 B de la cabeza): listar las entradas CON SANGRÍA (locales estáticas) con
  `address: 0xFFFFFFFF` de la CU de zAI entre cada par de vecinas, y comprobar la aritmética de la regla 1.1 con
  sección a 8.
- **Qué da la alineación 8 a la .data de zAI y de zPlatform en el original**: no hay ningún `double` en su DWARF.
  Medida: sonda en temporal con la .data de la unidad sin sus asm `.balign`, para ver qué objeto sube la
  alineación. Decide si los R2 de zAI/zPlatform bajan a "C más keep.lst" o quedan en "C más dato de alineación".
- **zMain .data** (715, 734, 864, 883, SteeringWheel:22): con la sección a 4 no pueden ser colas. Hay que saber qué
  código vivo del original las toca: diff de `reladdr` entre el .text extraído y el nuestro alrededor de
  VirtualMemory, InputDeviceGC y SteeringWheelDevice.
- **Constantes de pool y cadenas sin origen** (InputDevice:25, EventSequencer ×2, EAIEngineRev/ENISBrakelock,
  "LGWheel", "ACTIONQUEUE…", "GameDevice::SteeringWheelDevice", "<unknown>", "High/Medium/Low/Reflection/?",
  "EventBuffer", "(unknown)", "Scheduler"): subprogramas del DWARF con `low_pc` estripado en esas CU y
  debug_lines, para recuperar la función muerta que los usaba.
- **`_overlay_start`**: compilar una etiqueta de tamaño cero en C y ver si el enlazador la coloca en el arranque
  de .over.
