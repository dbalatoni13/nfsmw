# r68 — exploración de parches de datos, lote 6 (82 bloques)

HEAD 3c08534b, DOL de referencia 9619ba57. **Solo lectura**: nada de `src/`, `config/`,
`build/` tocado. Las compilaciones y enlaces de prueba van a temporales propios
(`scratchpad/lote668/`, scripts conservados, volcados borrados).

Veredictos: **R1 7 · R2 39 · R3 14 · R4 22**.

## 1. Lo medido en esta exploración (no son deducciones)

| # | medida | resultado |
|---|---|---|
| M1 | zFoundation: `lbl_803EB444/450` (UMath) y `lbl_803EB73C/740/744` (USpline) sustituidos por el **literal** en `BuildRotate` y `EvaluateCurvatureXZ`, borrando `extern` y `asm()` (`cmpvar.py`) | `.text` 36184, `.rodata` 2328, `.data` 248, `.bss`, `.ctors`: **IDÉNTICAS** byte a byte. Control base = `build/GOWE69/src/.../zFoundation.o`: IDÉNTICO. Ningún otro objeto (590) referencia esos `lbl_` (`undref.py`). |
| M2 | TokenEntropy.c sin el `asm(.skip 2)` | ALLOC **IDÉNTICAS**; `ScanBandUpdateProbs` sigue en +24: GCC alinea a 4 los arrays de char solo. |
| M3 | zCamera con las muertas del DWARF escritas en C (`NearClipInCar`, los 6 Tweak/test, `nWhichChaseCamera`, `DebugWorldCameraMover::SlowSpeed`) en lugar de 4 `asm()` (`mkcam.py`+`compobj.py`) | ALLOC **IDÉNTICAS** (.text 136020, .rodata 9872, .data 3136, .bss 2000) y cada símbolo en el MISMO offset. Control contra build/: IDÉNTICO. |
| M4 | Enlace completo (`dolwhere_obj.py`, copia de dolwhere con `--obj`/`--keep`) de M3 **sin** tocar keep.lst | **LAS SECCIONES NO COINCIDEN** (-0x20): los 36 B de GLOBALES muertos nombrados se van ENTEROS. |
| M5 | Enlace de M3 con una **copia** de keep.lst con los nombres nuevos (`zCamera.o:NearClipInCar`, …) | **15.557 B distintos, idéntico al enlace base (15.557), diff de rangos vacío**. El paquete C+keep.lst es neutro en el DOL. |
| M6 | `dolwhere zGameplay` (promoción) con `lbl_803EBE90` definido GLOBAL en nuestro zGameModes.o **y** en nuestro zGameplay.o | el enlace NO falla (35.438 B distintos): ngcld tolera el duplicado, pero el dato de zGameplay es ajeno. |
| M7 | `ngccc -S` de un caso mínimo con los flags de zFoundation | ver reglas R-a, R-g. |

## 2. Reglas nuevas medidas sobre cómo coloca GCC/ngcld los datos

- **R-a (pool con `.size`).** cc1plus 2.95.3 SN emite CADA entrada de pool y CADA literal
  de cadena como `$LCn` con `.type @object` y `.size` (`$LC2: .long 0x43300000,0x80000000
  / .size $LC2,8`). Consecuencia: escribir una función estripada recupera su pool, pero con
  `-strip-unused-data` solo sobreviven sin ayuda los flotantes de 4 B (size&~7 = 0); los
  dobles de 8 B (el sesgo int→double) y las cadenas ≥8 B necesitan su entrada en keep.lst.
- **R-b (GLOBAL muerto se va entero en NUESTRO enlace).** M4: un global de `.data` de 4 B sin
  referencia y sin keep.lst desaparece completo. Confirma `gapchk.py` y acota la nota
  «uno de 4 B se queda entero» (ventana-de-32) a los `$LC`/locales.
- **R-c (el original conservaba `size % 8` también de GLOBALES).** En 0x80418978 el DOL
  tiene 4 B a cero justo detrás de `szMixMapFiles` y `nUnityMixScale = 0x7FFF` cae en
  0x8041898C: son el **cadáver de `szMainMapStates[13]`** (52 B, GLOBAL, `address
  0xFFFFFFFF` en el DWARF). Un cadáver parcial de un global no se reproduce con keep.lst
  (conserva el símbolo entero): queda R3.
- **R-d (el volcado nombra los huecos).** `symbols/mw_dwarfdump.nothpp` conserva las variables
  estripadas con `address: 0xFFFFFFFF` en ORDEN DE FUENTE, y las funciones estripadas con
  `// Range: 0xFFFFFFFF`. Los huecos de `.data` de este lote son variables muertas de 4 B en
  ese orden (sumas exactas al byte); los tramos anónimos de `.rodata` entre dos pools vivos
  son pools de las funciones estripadas de ese intervalo. Herramientas: `gapdw.py`,
  `deadvars.py`, `dwfuncs.py`.
- **R-e (PS2 como segunda fuente de nombres).** `symbols/PS2/PS2_globals.hpp` y
  `orig/SLES-53558-A124/NFS.MAP` listan esas muertas en orden (Demo1On/Demo2On,
  UpdateSmokeableWooshs, DEBUGPRINT_MIXERCONNECT, bPRINTDOPPLERINMIXER, DBGPRNT_NIS,
  cstringname, SND_AI_FollowAICar…). Diferencias de plataforma a vigilar:
  `CameraFollowDefaultCar/ThisCarInstead` solo existen en PS2.
- **R-f (puntero muerto a cadena muerta).** El DOL lleva el ADDEND: el desplazamiento de la
  cadena dentro de la `.rodata` del OBJETO. zMiscSmall: 0x90 → "10/19/05", 0x9C → "";
  zEAXSound2 `cstringname` = 0x5C → "" (justo detrás del prefijo de 92 B).
- **R-g.** `bool` de ámbito de fichero = `.size 4`, `.align 2`; un `static` sin referencias
  se emite igual a -O1; miembro estático = `_<len><Clase>.<miembro>`
  (`_21DebugWorldCameraMover.SlowSpeed`).
- **R-h.** Una `extern const float lbl_…` que es la entrada del pool de una función VIVA se
  sustituye por el literal sin mover un byte (M1). Donde el `asm()` estaba al final del
  fichero (DebugWorld `kLookDistance`) el literal cambia el sitio: allí es R2.

## 3. Hallazgos de riesgo fuera de la tabla

1. **`lbl_803EBE90` NO es de zGameplay**: cae en la `.rodata` de zGameModes
   (0x803EBB48..0x803EBE98); nuestro zGameModes.o promocionado ya lo define GLOBAL y lo
   referencia desde su static-init (+0x348, igual que el extraído). La copia de
   GRaceStatus.cpp es un duplicado sin referencias.
2. **UMath.cpp:27 «`Floor` NO existe en el objetivo» es falso**: el DWARF tiene
   `STRIP float Floor` justo detrás de `Ceil`; los 16 B son su pool. Los −8 B medidos al
   escribirla eran el doble de sesgo estripado (R-a), no una función inexistente.
3. **r56 «gap_06_8041A570_data = szMainMapStates» es falso** (R-c: su cadáver está en 0x80418978).
4. **zEAXSound2: `lbl_80418978`/`lbl_8041897C` están delante de `szMixMapFiles`**
   (nuestro .o: +BAC/+BB0 y szMixMapFiles +BB4; extraído: szMixMapFiles +BC0 y los dos
   detrás). `szMixMapFiles` (2 reubicaciones desde `CreateMainMainMap`) queda 8 B tarde.
5. **Compensación .bss.** Varias muertas de zEAXSound2/zEAXSound son hoy definiciones
   tentativas (`int X;` → COMMON/.bss: DEBUG_EVTMIXCTL está en .bss+E44). Pasarlas a `.data`
   con `= 0` cambia el TAMAÑO de la `.bss` de la ventana: el `.space 56` (r62) y el
   `.space 16` hay que re-barrerlos en el mismo paquete.
6. Fuera de lote: el DWARF atribuye 0x8041D198..0x8041D1C0 (`gQuatCompMask`, `seedarray`,
   estáticos) a zFoundation.cpp; splits.txt da ese rango a zFEng.

## 4. Tabla bloque a bloque

Abreviaturas: «keep» = sustituir en `config/GOWE69/keep.lst` la línea del `gap_/lbl_` por
los nombres C (R-b); «DWARF» = `mw_dwarfdump.nothpp`; «PS2» = PS2_globals/NFS.MAP;
«ALLOC=» = secciones ALLOC del objeto idénticas al actual.

| fichero:línea | clase | qué era en el original (y de dónde) | veredicto | C o paquete propuesto | test | riesgo |
|---|---|---|---|---|---|---|
| Packages/vp6/.../TokenEntropy.c:13 | HUECO 2B | relleno de alineación: GCC alinea a 4 `ScanBandUpdateProbs` (array de char) | **R1** (medido M2) | borrar el `asm()` | ALLOC= medido; relink DOL | ninguno (símbolo sin keep ni refs) |
| dolphin/gx/GXVert.h:34 | ALIAS | `GXWGFifo` símbolo ABSOLUTO 0xCC008000 (DWARF: variable con esa dirección en cada CU GCC) | R2 | `extern volatile PPCWGPipe GXWGFifo;` en la cabecera + `GXWGFifo = 0xCC008000;` en el ldscript (config) | recompilar usuarios + relink DOL | cabecera compartida por muchas unidades; comprobar que ningún extraído ya lo define |
| Libs/.../Carp.cpp:50 | HUECO 4B .data 8041D1F0 | `static bool CARP::gResolveWarnings` (DWARF VIVO en 8041D1F0; PS2 entre gHaveInitialized y gDiagnosticFunc) | R2 | `static bool gResolveWarnings = false;` entre las dos; keep `zFoundation.o:gResolveWarnings` (la entrada `gap_06_8041D1F0_data` hoy no nombra nada) | ALLOC=; relink DOL | LOCAL 4B: probablemente sobrevive sin keep; ponerlo igual |
| Libs/.../UBezierLite.cpp:35 | HUECO 12B .data 8041D1D0 | `char *UDevCons_gMap` (muerta), `bool gMemCapture` (muerta), `static int gGroupDataMemoryType` (VIVA en 8041D1D8) — DWARF, ceros en DOL | R2 | `char *UDevCons_gMap = 0; bool gMemCapture = false;` + `static int gGroupDataMemoryType = 0;`; keep ×3 por `gap_06_8041D1D0_data` | ALLOC=; relink DOL | orden de emisión si gGroupDataMemoryType va en UGroup.cpp (UEALibs no tiene .data) |
| Libs/.../UEALibs.cpp:34 | HUECO 48B .rodata | pool de estripadas entre rsincos y v3unit: MATRIX4_vect4mult(3), m3toquat, quattom3, m3toxyzrot, m3xform(xlatev), v3dotprod, m3set*rot (DWARF STRIP; 0/1/0,5 flotantes) | R2 | escribir esos cuerpos en su orden DWARF (firmas y locales en el volcado); solo flotantes 4B → sin keep | .rodata=; .text crece (lo tira el enlace); relink | orden de creación de constantes; cuerpos no verificables (texto estripado) |
| Libs/.../UFoundationBody.cpp:9 | HUECO 4B .data 8041D1E4 | sin candidato: nada en DWARF entre UFoundation_AssertMessage y gResolversSorted; PS2 deja 8 B sin listar ahí | R4 | — | decide: DWARF/mapa de otra plataforma (X360) o estático sin DIE | — |
| Libs/.../UMath.cpp:107 | DATOS lbl_803EB444 | primera entrada del pool de `BuildRotate` (1/360 = 0x3B360B61) | **R1** (medido M1) | `r *= 0.0027777778f;` y borrar los 4 `extern const float lbl_803EB44x` y el `asm()` | ALLOC= medido; relink DOL | ninguno medido |
| Libs/.../UMath.cpp:173 | DATOS lbl_803EB450 | cuarta entrada del pool de `BuildRotate` (2π = 0x40C90FDB) | **R1** (medido M1) | `angle = r * 6.2831855f;` y borrar el `asm()` | idem | idem |
| Libs/.../UMath.cpp:181 | HUECO 156B | pools de estripadas BuildRotTrans×2, BuildQuatTrans×2, ExtractRotTrans×2, Matrix3ToQuat, ExtractQuatTrans×2, TransformPoint/Vector×4, Determinant4x4, Inverse, TriIntersect…CalcPlaneY (DWARF), con un doble de sesgo sin signo | R2 | cuerpos + keep `$LC` para el doble de 8B | .rodata=; relink; lcfix | `$LC` de doble sin `@lc` (renumeración) |
| Libs/.../UMath.cpp:35 | HUECO 16B | pool de `Floor` (DWARF `STRIP float Floor` tras Ceil): 0, sesgo 4330000080000000, 1.0f | R2 | `float Floor(const float x) { return bFloor(x); }` + keep del `$LC` de 8B | .rodata=; relink | corrige el comentario UMath.cpp:27 |
| Libs/.../UMath.cpp:68 | HUECO 8B (1.0f, 0) | candidato: cadáveres de 4B de `Matrix3::kIdentity`/`kZero` (36B muertos), pero el orden DWARF los pone tras Matrix4::kIdentity | R4 | — | decide: compilar con Matrix3::k* definidos y ver offsets + enlace | son GLOBALES: R-b los quitaría enteros |
| Libs/.../UMath.cpp:81 | HUECO 84B | pools de estripadas BytesToCoord×2 (1/255 + sesgo sin signo), ValidateMatrix (0,9/1,1/0,001), BuildSRT (1/360, 2π), BuildScale×2, BuildTranslate | R2 | cuerpos + keep ×2 dobles (la entrada `lbl_803EB3F0` de keep.lst hoy no nombra nada) | .rodata=; relink | idem 181 |
| Libs/.../USpline.cpp:100 | HUECO 16B | pool de estripadas `USpline::Smooth` y `BuildSpline` (DWARF STRIP entre GetTangentBasisMatrix y BuildSplineEx): 3.0, 2/3, 1/3, 0 | R2 | cuerpos; solo flotantes | .rodata=; relink | — |
| Libs/.../USpline.cpp:123 | HUECO 12B | pool de la sobrecarga estripada `USpline::EvaluateSpline` (STRIP entre BuildSplineEx y EvaluateSpline): 1.0, 0,5, 0 | R2 | cuerpo | idem | — |
| Libs/.../USpline.cpp:186 | HUECO 16B | pool de `USpline::EvaluateCurvature` (STRIP justo antes de EvaluateCurvatureXZ): mismas constantes −1e-4/1e-4/1000 | R2 | cuerpo (gemelo XYZ de EvaluateCurvatureXZ) | idem | — |
| Libs/.../USpline.cpp:196 | DATOS lbl_803EB73C/740/744 | pool de `EvaluateCurvatureXZ` | **R1** (medido M1) | `UMath::Clamp(t3, -0.0001f, 0.0001f)` / `return 1000.0f;`; borrar externs y `asm()` | ALLOC= medido | ninguno medido |
| Libs/.../UVectorMath.cpp:613 | DATOS 88B (13 nombres + 8 anónimas) | nombres = pools de VU0_sqrt [0,1,½], VU0_rsqrt [0,1], MATRIX4setyrot [2π], m4toquat [0,1,½], Matrix4ToEuler [−1,1,0,0,001]; anónimas = pools de las STRIP intercaladas (setxrot [1,2π], setzrot [2π], InitPlatformSpecifics/QuatToEuler/EulerToQuat [1,0,−1,0,001,2π], EulerToMatrix4 [2π]) en orden DWARF exacto | R2 | literales en las 5 vivas (M1) + cuerpos de las STRIP; todo flotante 4B → sin keep | .rodata=; relink | colocar cada cuerpo en su sitio DWARF |
| SourceLists/zCamera.cpp:12 | DATOS 1476B | prefijo bWare/STL de 92 B (`__FILE__` de EA) + 1384 B: pool propio de Camera/CameraMover/CameraAI (flotantes, "JR2Server", "Seeulator", nombres Attrib, CM_*) en el orden del original | R3 | prefijo: irreducible hoy (medida posible: `#line` en una copia de bWare.hpp); pool: orden de creación de constantes (r63-cam) | dolwhere zCamera | keep protege el bloque por nombre |
| SourceLists/zCamera.cpp:235 | HUECO 8B .bss | guardas anónimas de parejas `_.tmp_N` de estáticos de inlines (extraído: gap_07 120 B) | R3 | — (mecanismo: estáticos locales de inlines no instanciados) | dolwhere zCamera | recalibrar si cambia la .bss |
| SourceLists/zDebug.cpp:14 | DATOS 328B | prefijo 92B + nombres de tipos AttribSys ("Attrib::Attribute"…"Attrib::Gen::emitterdata", "%s") de inlines de cabecera muertos | R3 | — (ver zFoundation.cpp:129) | relink DOL | unidad promocionada |
| SourceLists/zEAXSound.cpp:169 | DATOS 410B (26 trozos) | muertas de EAXSound.cpp y vecinos, por DWARF+PS2, sumas exactas en ≥18 trozos: 6B8 gLastFETRAXmask, gLastIGTRAXmask (estáticos vivos), g_fPS2DemoVolume=0x3F7FFFEF, UsedSPURam, UsedMainRam, SoundRegionsLoadedCallback(+Param), bSoundRegionLoadingInProgress, StartLoadingSoundRegionsTicks · 750 DEBUG_GAME_STATE, MaxVoiceCnt, nDebug3DMix, nt1, nt2, gnStopMusic, rs_cur · 770 nTestNFSMixShape, nIncrement · 790 uDynMixUpdateTicks…ExhManufacterer (10) · 7BC DEBUG_SNDPAUSE · 810 DEBUG_PERFORM_MEMORY_PROFILING, PerformingProfile, testBankLoading · 820 DEBUG_PRINT_FE_SND, g_fTableTurnSpeed=1.0f, DEBUG_ONLINE_ENGINES, g_AI_PITCH_OFFSET · 840 SND_AI_FollowAICar · 898 nLargestAllocation, CsisCnt, g_PC_UpdateRate=50.0f, g_PC_SampleRate=44100, …=0x86, 0x6E, WRITEAIFF, SAVEAIFF, g_HACKTRAINVOLUME · 8F8 SPEW_AI_STATE_INFO · 9FC ToggleReflection, DEBUG_PRINT_TUNNEL, DEBUG_REVERB_CHANGES · A0C DebugOcclusionProfile · A9E m_PlyrCarAzimToCam…fratio_2 · ABC DEBUG_PATHFINDER, VALIDATE_MUSFILE. Sin nombre: 638 (24B), 654 (56 de 60B, primero SND11MemoryPoolNum=−1), 7D0 | R2 (parcial) | definir cada una con su valor en su .cpp (punto de parseo), keep por nombre (~70 líneas por 26), borrar el bloque; lo sin nombre queda en `asm()` anónimos en su sitio | dolwhere zEAXSound (pendiente) sin empeorar | la `.data` pasa de un bloque al final a su sitio: mover .bss/.data; `.space 16` a re-barrer |
| SourceLists/zEAXSound.cpp:4 | DATOS 92B | prefijo bWare/STL | R3 | — | — | — |
| SourceLists/zEAXSound.cpp:407 | HUECO 32B | "AEMSMGR: async bank load buffer": cc1plus ya la emite como `$LC`, pero muerta (R-a) | R2 | `# @lc zEAXSound "AEMSMGR: async bank load buffer"` + entrada; borrar `asm()` | dolwhere zEAXSound; lcfix | `$LC` renumerados |
| SourceLists/zEAXSound.cpp:416 | HUECO 120B | palabras de pool que faltan a la ventana (2 sesgos, 1.0×3, 32767, 512) + 21 ceros de ajuste de ventana | R4 | — | decide: `rodmap.py zEAXSound` tras reordenar el pool (r64-ecs: 60 tramos) | compensa a otros bloques |
| SourceLists/zEAXSound.cpp:471 | HUECO .bss 16B | 2 parejas guarda/_.tmp de más en el objetivo (34 contra 32; 4 ranuras sin DIE ni refs, r62) | R3 | — | — | recalibrar con cualquier cambio de .bss |
| SourceLists/zEAXSound.cpp:98 | HUECO 999B .asciz | 71 cadenas muertas (nombres Attrib, razones de pausa, etiquetas de perfilado) de inlines/funciones estripadas; hoy al final, orden del original perdido | R3 | — (forma C = sus usos en su sitio + keep @lc por cadena ≥8B) | — | posición: r61/r64 |
| SourceLists/zEAXSound2.cpp:104 | HUECO 8B .data E44 | E44 = `int PRINT_PERFORMANCE_LVL` (PS2 tras EAXCar::s_StateInfo; EAXCar.cpp:163 hoy tentativa); E48 sin nombre | R2 (parcial) | `int PRINT_PERFORMANCE_LVL = 0;` + keep; 4B anónimos para E48 | dolwhere zEAXSound2 | COMMON→.data: re-barrer `.space 56` |
| SourceLists/zEAXSound2.cpp:116 | HUECO 28B .data E84 | sin candidato (PS2 no lista nada entre TWO_PLYR_XFADE_AZIM_MIN_DIST y SndBase::s_TypeInfo) | R4 | — | decide: dump de NFS.MAP del objeto ordenado por dirección | — |
| SourceLists/zEAXSound2.cpp:137 | HUECO 4B EC8 | `int DEBUGPRINT_MIXERCONNECT` (PS2 entre CSTATE_Base y CSTATE_Collision::s_StateInfo) | R2 | STATE_Base.cpp:37 `= 0`; keep; borrar `asm()` | dolwhere zEAXSound2 | idem .bss |
| SourceLists/zEAXSound2.cpp:164 | HUECO 4B F3C | `int CSTATEMGR_DriveBy::UpdateSmokeableWooshs` (PS2 justo tras bUsingGinsu; declarado en STATEMGR_DriveBy.hpp:23) | R2 | definición `= 0`; keep `_17CSTATEMGR_DriveBy.UpdateSmokeableWooshs` | offset en .data = +0x194; dolwhere | si va en STATEMGR_DriveBy.cpp puede caer tras otros .cpp: comprobar offset |
| SourceLists/zEAXSound2.cpp:205 | HUECO 12B FFC | `gfTestRatio`, `gfTestCurVel`, `debugPrintWindNoise` (DWARF, CARSFX_WindNoise.cpp hoy tentativas) | R2 | `= 0` en las tres; keep ×3 | dolwhere | idem .bss |
| SourceLists/zEAXSound2.cpp:222 | HUECO 4B 804180D0 | sin candidato (PS2 también deja 8B entre PreColWoosh y Nitrous) | R4 | — | decide: NFS.MAP por objeto | — |
| SourceLists/zEAXSound2.cpp:247 | HUECO 4B 80418188 | `bool bPRINTDOPPLERINMIXER` (PS2 tras TrafficEngine::s_TypeInfo; CARSFX_TrafficFX.cpp:100 tentativa) | R2 | `= false`; keep | dolwhere | idem |
| SourceLists/zEAXSound2.cpp:264 | HUECO 12B 8041829C | sin nombre entre CARSFX_Siren::s_TypeInfo y PURSUIT_TO_LIC_DELAY | R4 | — | decide: NFS.MAP | — |
| SourceLists/zEAXSound2.cpp:279 | HUECO 4B 8041833C | `int DBGPRNT_NIS` (PS2 tras g_bWasLastNISaStart; SFXObj_NISStream.cpp:19 tentativa) | R2 | `= 0`; keep | dolwhere | idem |
| SourceLists/zEAXSound2.cpp:298 | DATOS lbl_804183DC (=0x5C) | `char *cstringname` (PS2 tras SIZE_OF_g_REVERBFXMODULES); 0x5C = addend hacia el "" en .rodata+0x5C (R-f) | R4 | candidato `char *cstringname = "";` | decide: enlazar y ver si ngcld escribe 0x5C con el `$LC` estripado | valor del puntero |
| SourceLists/zEAXSound2.cpp:329 | HUECO 24B 80418950 | `g_NFSLiveLink`, `InputMsgBuffer`, `g_iInputSndBuffer[1]`, `g_iOutputSndBuffer[1]`, `iTestDisplay`, `fTestDisplay` (DWARF, 6×4B exactos hasta szMixMapFiles); fichero `nfslivelink.cpp` del original (file_names.txt), en el árbol mide 0 B y no se incluye | R2 | escribir NFSLiveLink.cpp con las seis `= 0` e `#include` entre TURBO.cpp y NFSMixMap.cpp; keep ×6 | offset +0xBA8; dolwhere | — |
| SourceLists/zEAXSound2.cpp:343 | DATOS lbl_80418978 | cadáver de 4B de `szMainMapStates` (R-c) | R3 | mover el `asm()` anónimo DETRÁS de `szMainMapStates` en NFSMixMap.cpp (hoy va antes de szMixMapFiles, hallazgo 4) | dolwhere zEAXSound2: szMixMapFiles vuelve a +0 | paquete con :352 |
| SourceLists/zEAXSound2.cpp:352 | DATOS lbl_8041897C | `int DEBUG_EVTMIXCTL` (DOL 0 en 97C; nUnityMixScale en 98C lo fija) | R2 | NFSMixMap.cpp:14 `= 0`; keep `DEBUG_EVTMIXCTL` por `lbl_8041897C` | dolwhere | sale de .bss (+E44): re-barrer `.space 56` |
| SourceLists/zEAXSound2.cpp:369 | HUECO 52B 8041A570 | sin candidato; la atribución r56 a szMainMapStates queda refutada | R4 | — | decide: NFS.MAP y DWARF de SFXObj_Reverb | — |
| SourceLists/zEAXSound2.cpp:395 | HUECO 575B .asciz | 47 cadenas muertas (nombres Attrib, bancos, eventos) de usos estripados | R3 | — | — | posición |
| SourceLists/zEAXSound2.cpp:4 | DATOS 92B | prefijo bWare/STL | R3 | — | — | — |
| SourceLists/zEAXSound2.cpp:445 | HUECO 16B | "Reflection", "Part": `$LC` ya emitidos por cc1plus pero estripados (R-a) | R2 | `# @lc` + entradas keep; borrar `asm()` | dolwhere; lcfix | renumeración |
| SourceLists/zEAXSound2.cpp:452 | SECCION 0B | `asm(".section .data" ".previous")` vacío, solo comentario | **R1** | borrar | cmphead: ALLOC= | ninguno (0 B) |
| SourceLists/zEAXSound2.cpp:460 | SECCION 0B | idem | **R1** | borrar | idem | ninguno |
| SourceLists/zEAXSound2.cpp:480 | HUECO .bss 56B | relleno de parejas guarda/_.tmp (r62, barrido medido: 56 es mínimo) | R3 | — | — | se descalibra con cada COMMON→.data de esta tabla |
| SourceLists/zEAXSound2.cpp:506 | HUECO 72B .rodata | palabras de pool que faltan a la ventana (½×2, sesgo×2, 1000×2, 1, 2, 70, 18,8, 6 ceros) | R4 | — | decide: rodmap zEAXSound2 | compensación |
| SourceLists/zEAXSound2.cpp:61 | HUECO 132B pad_06_80417DA8 | cabeza de la .data de zEAXSound2 antes de EAXCar::g_ShiftInfo; sin DIE ni entrada PS2 | R4 | — | decide: NFS.MAP objeto zEAXSound2 / límite con zEAXSound | — |
| SourceLists/zFoundation.cpp:129 | HUECO 164B | nombres AttribSys ("Attrib::Attribute"…"Attrib::Blob") y "MGeneric": literales de inlines de cabecera, en el punto donde el original los usó primero | R3 | — (forma C: sus usos; mismos 9 nombres en zDebug/zMiscSmall/zCamera) | relink | promocionada |
| SourceLists/zFoundation.cpp:152 | HUECO 56B | 0, 1.0f y 12 enteros primos (857, 2999, 3767…): pool y tabla de `EncryptDecrypt` (única STRIP con cuerpo entre ResolveTagReferences y hash32) | R2 | cuerpo con su `static const int[12]` + keep (48B LOCAL se va entero) | .rodata=; relink | identidad por intervalo, cuerpo no verificable |
| SourceLists/zFoundation.cpp:168 | HUECO 92B | "StringStoreBlock", "StringPool Attrib::StringKey[]", "StringRegistry::CreateLowerCaseKey": nombres de reserva de StringRegistry; la función usuaria no se identifica (las STRIP del intervalo son el sort de STL) | R4 | — | decide: cotejar con PS2 (ps2fn.py) qué función los pasa | — |
| SourceLists/zFoundation.cpp:192 | DATOS lbl_8041D204 180B .data | 204 `StringStoreBlockPtr::sCacheInitialized` (muerta; PS2 tras gInited); 208 `warned_already` (estático VIVO); 20C..2B8 172B sin nombre (0x2C8 en +0x2C) | R4 | parcial: `bool StringStoreBlockPtr::sCacheInitialized = false;` | decide: DWARF de estáticos de función / NFS.MAP | keep ya protege lbl_8041D204 |
| SourceLists/zFoundation.cpp:42 | HUECO 76B | pools de STRIP UBezierLite::EvaluateForY/EvaluateTangent, (De)CompressQuaternion, Get/SetFloatBitField, PrepareBitField ("<unknown>") | R2 | cuerpos + keep para dobles y "<unknown>" (10B) | .rodata=; relink; lcfix | idem UMath:181 |
| SourceLists/zFoundation.cpp:6 | HUECO 92B pad_05_803EB230 | prefijo bWare/STL | R3 | — | — | — |
| SourceLists/zFoundation.cpp:60 | HUECO 20B | 1.0, 0, 1/2π, 0, 1.0: cola del pool de v3unit + STRIP del intervalo (v3dot…v4angle) | R2 | cuerpos (flotantes) | .rodata=; relink | — |
| SourceLists/zFoundation.cpp:72 | HUECO 16B | "UData", "UGroup": nombres de reserva de STRIP UDataGroupDecodeTag/RecursiveCloneUGroup | R2 | cuerpos; cadenas <8B sobreviven sin keep (R-a) | .rodata=; relink | identidad por intervalo |
| SourceLists/zFoundation.cpp:91 | HUECO 364B | `kVideoModeNames[4]` = {"Undefined","NTSC","PAL","PAL60"} (muerta; PS2 tras gMasterVideoMode) + cadenas de STRIP SetFoundationVideoMode, FoundationAbort, Platform_Abort, AssertMessage/FileLine/Line… | R2 | la tabla + los cuerpos de UFoundationBody.cpp + keep @lc de las cadenas ≥8B | .rodata=; relink; lcfix | la tabla GLOBAL muerta se va entera: solo quedan las cadenas |
| SourceLists/zGameplay.cpp:269 | HUECO 68B .data 8041D53C | sin DIE ni PS2 entre el último kObjectTemplateKey y zLua (0x8041D580 múltiplo de 128) | R4 | — | decide: `sh_addralign` de la .data de zLua compilado de verdad (hipótesis c de r64) | — |
| SourceLists/zGameplay.cpp:89 | DATOS 1620B | 100 cadenas muertas (R3) + lbl_803ED330/333: el sesgo 4330000080000000 partido 3+5 por dtk = `$LC678`, que cc1plus emite y el enlace estripa | R3 | último tramo: keep del `$LC678` (R2 dentro del bloque) | dolwhere zGameplay | posición de las cadenas |
| SourceLists/zGameplay.cpp:9 | DATOS 92B | prefijo bWare/STL | R3 | — | — | — |
| SourceLists/zMiscSmall.cpp:107 | DATOS 620B .data | globales de Config.cpp, todos en DWARF/PS2: BuildVersionChangelistName (viva), BuildVersionChangelistNumber=191683, BuildVersionDate→0x90, BuildVersionName/FullBuildVersionName→0x9C, SkipFE* (con nombre), SkipFEPlayerModelOverride→0x9C (lbl_8041EDC0), SkipFEPrintPerformances (gap EE04), ForceJuiceConnect/ForceJuiceConnectIP (lbl_8041EE18), gDisableWorldEvents/gDebugEventStrings (gap EE24), DrawXenonShadows, ForceXenonFramerateTo15, OverideMinimumLOD, AnimCfg_DisableDemoFlyIn… | R2 | transcribir Config.cpp con valores e inicializadores; keep por nombre en lugar de lbl_/gap_ | relink DOL | los `char*` muertos valen el addend (R-f): medir que ngcld lo reproduzca |
| SourceLists/zMiscSmall.cpp:16 | DATOS 348B .rodata | prefijo 92B (R3) + inicializadores de Config.cpp ("191527 191537…", "10/19/05", "bmwm3gtre46", "911turbo", "10.10.235.145") + nombres Attrib | R2 (parcial) | salen de los inicializadores de :107 en orden; keep @lc de las muertas | relink DOL | paquete con :107 |
| Src/Camera/Camera.cpp:64 | HUECO 52B 80416490 | sin DIE ni PS2 entre cameralink y bStreamingPositionFromICE | R4 | — | decide: NFS.MAP ordenado / X360 | — |
| Src/Camera/Camera.cpp:81 | DATOS lbl_804164C8 (0.4f) | `float NearClipInCar = 0.4f` (DWARF muerta entre bStreamingPositionFromICE y JR2ServerExists; PS2 y NFS.MAP). `kJRCaffeineRate` es un nombre inventado | R2 (**medido M3+M5**) | `float NearClipInCar = 0.4f;` en lugar de `static const float kJRCaffeineRate` + `asm()`; keep `zCamera.o:NearClipInCar` | ALLOC= y enlace 15.557=15.557 medidos | sin keep rompe secciones (M4) |
| Src/Camera/Camera.cpp:94 | HUECO 80B 804164DC | sin DIE; PS2 deja 152B sin listar entre LastUpdateTimeJR2 y CameraDebugWatchCar | R4 | — | decide: NFS.MAP / X360 | — |
| Src/Camera/CameraMover.cpp:21 | HUECO 24B 80416538 | `bool Tweak_EnableRoadEditor`, `int Tweak_PursuitStartCamTest, Tweak_JumpCamTest, bICETest, bCinematicSlowdownTest, bShowcaseTest` (DWARF + NFS.MAP) | R2 (**medido M3+M5**) | esas seis definiciones `= 0`; keep ×6 por `gap_06_80416538_data` | medido | idem |
| Src/Camera/ChaseCamAI.cpp:8 | HUECO 4B 80416558 | sin candidato de 4B (ChaseParamsOn/Off son 24B: cadáver 0) | R4 | — | decide: NFS.MAP | — |
| Src/Camera/ICE/ICEManager.cpp:380 | HUECO 8B 80417054 | sin DIE entre GenericCategoryNames y gIceOverlays | R4 | — | decide: NFS.MAP | — |
| Src/Camera/ICE/ICEOverlays.cpp:28 | HUECO 67B 80417085 | 3B de alineación tras gOverlay + 64B sin nombre hasta el fin de la .data | R4 | — | decide: NFS.MAP / estáticos de ICEReplay | — |
| Src/Camera/Movers/Cubic.cpp:482 | HUECO 8B 80416FE8 | `bool Demo1On`, `bool Demo2On` (DWARF muertas; NFS.MAP 5e0aa4/5e0aa8 exactamente entre CameraImpcatCurveV y TrackCopCameraMover_IdleSim) | R2 | `bool Demo1On = false; bool Demo2On = false;`; keep ×2 | mismo patrón que M3/M5 (no compilado) | — |
| Src/Camera/Movers/DebugWorld.cpp:298 | DATOS lbl_803D27F4 (20.0f) | sin DIE: entrada del pool de `DebugWorldCameraMover::Update` (2 reubicaciones en el extraído) | R2 | literal `20.0f` en Update; borrar `extern kLookDistance __asm__` y el `asm()` | dolwhere zCamera (.rodata cambia de sitio: al pool de Update) | posición (el `asm()` iba al final del fichero) |
| Src/Camera/Movers/DebugWorld.cpp:35 | DATOS lbl_80417008 (0.05f) | `float DebugWorldCameraMover::SlowSpeed` (declarada en DebugWorld.hpp:28; DWARF muerta entre SuperTurboSpeed y TurboOn) | R2 (**medido M3+M5**) | `float DebugWorldCameraMover::SlowSpeed = 0.05f;`; keep `zCamera.o:_21DebugWorldCameraMover.SlowSpeed` | medido | — |
| Src/Camera/Movers/TrackCop.cpp:44 | DATOS lbl_80416FF4 (=2) | `eChaseCam nWhichChaseCamera` (DWARF muerta tras TrackCopCameraMover_IdleSim; NFS.MAP) | R2 (**medido M3+M5**) | `eChaseCam nWhichChaseCamera = <enumerador 2 del volcado>;`; keep | medido (con enum de prueba) | traer `enum eChaseCam` del volcado |
| Src/Gameplay/GManager.cpp:115 | HUECO 4B 8041D3A0 | `static bool sPrintObjectStateAlloc` (DWARF VIVO en 8041D3A0; PS2 tras GManager::mObj) | R2 | `static bool sPrintObjectStateAlloc = false;` tras `GManager::mObj`; keep | dolwhere zGameplay | — |
| Src/Gameplay/GManager.cpp:81 | HUECO 8B 8041D38C | 38C `static bool sVerbose` (DWARF VIVO; PS2); 390 4B sin nombre | R2 (parcial) | `static bool sVerbose = false;` + 4B anónimos | dolwhere | — |
| Src/Gameplay/GRaceStatus.cpp:2223 | DATOS lbl_803EBE90 (1.0f) | dato de **zGameModes** (.rodata 803EBB48..803EBE98), ya emitido y referenciado por nuestro zGameModes.o promocionado | **R1** | quitar `lbl_803EBE90` del `asm()` (las otras etiquetas del bloque no son de esta fila) | dolwhere zGameplay: 35.438 B antes (M6) | 4B menos en la .rodata de zGameplay (ventana de 32) |
| Src/Gameplay/GRaceStatus.cpp:81 | HUECO 8B 8041D310 | candidato `bool bPreloadAllDragRaceTraffic` (muerta) + 4B sin nombre | R4 | — | decide: NFS.MAP | — |
| Src/Gameplay/GRuntimeInstance.cpp:24 | HUECO 60B 8041D2D0 | 308 = `bool bSendRaceLengthToBigBang` (DWARF muerta; NFS.MAP justo antes de GRaceStatus::fObj); 2D0..308 56B sin nombre | R4 | parcial: `bool bSendRaceLengthToBigBang = false;` | decide: NFS.MAP | — |
| egami/.../rcmp_vp6_codec.cpp:257 | HUECO 24B | "Assert: %s:%i %s (%s)" del assert de RCMP; candidata la STRIP `VP6_CODEC_INTERNAL::operator new(size, msg, …)`; no comprobado | R4 | — | decide: escribir el inline con su assert (egami va sin -O) y ver si sale en ese orden | patrón en 5 TU de egami |
| egami/.../bigswizzler.cpp:184 | HUECO 16B | pool de `tBigSwizzler_DrawSetup` (DWARF STRIP, última función): sesgo i→d + 0.0625f + 0 | R2 | cuerpo de DrawSetup + keep del doble | .rodata=; relink | — |
| egami/.../bigswizzler.cpp:4 | HUECO 28B | "" + "Assert: %s:%i %s (%s)" en cabeza de la TU (mismo patrón egami) | R4 | — | idem rcmp:257 | — |
| libc/stdio.c:9 | ALIAS gcc2_compiled._80313A40 | artefacto de dtk: `__sread` (0x80313A40) comparte dirección con la etiqueta local `gcc2_compiled.` y fopen.o, sn_buf.o y auto_01_80312DC4_text.o extraídos lo nombran así | R2 | config: symbols.txt para que el troceado use `__sread` y re-extraer esos 3; o esperar a promoverlos | relink DOL | re-extracción |

## 5. Scripts (scratchpad/lote668/)

`gapdw.py` (hueco → vecinas vivas y muertas del DWARF), `deadvars.py`, `dwvars.py`,
`dwfuncs.py` (funciones STRIP en orden), `relscan.py` (refs y binding de gap_/lbl_),
`undref.py`, `dolrd.py` (bytes del DOL), `cmpvar.py`/`compobj.py` (compilar variante a
temporal y comparar ALLOC), `dolwhere_obj.py` (dolwhere con `--obj` y `--keep`), `mkcam.py`.
