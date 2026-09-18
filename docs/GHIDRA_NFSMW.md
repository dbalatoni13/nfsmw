# Ghidra — análisis del ELF original GameCube (NFSMWRELEASE.ELF)

Documento de referencia para el trabajo de decompilación. Generado 2026-09-10 con
Ghidra 12.1.2 headless. El repo no se modificó para este análisis salvo este fichero.

## 1. Dónde está cada cosa

| Qué | Ruta |
|---|---|
| Ghidra 12.1.2 | `C:\Users\jferr\Desktop\ghidra_12.1.2_PUBLIC_20260605\ghidra_12.1.2_PUBLIC\` (headless: `support\analyzeHeadless.bat`, Java 21 en `C:\Program Files\Microsoft\jdk-21.0.12.101-hotspot`) |
| **Proyecto Ghidra `NFSMW`** | `C:\Users\jferr\AppData\Local\Temp\opencode\ghidra_project\` (`NFSMW.gpr` + `NFSMW.rep/`), programa `/NFSMWRELEASE.ELF`, lenguaje `PowerPC:BE:32:default`, imagebase `0x80003100`, **con auto-análisis completo guardado** |
| ELF original | `orig/GOWE69/NFSMWRELEASE.ELF` (105.374.584 B; ELF32 BE PPC, entry `0x80003100` = igual que nuestro link) |
| Scripts usados | `C:\Users\jferr\AppData\Local\Temp\opencode\ghidra_scripts\` (`DumpInfo.java`, `ExportSymbols.java`, `ExportWalls.java`, `ExportWalls2.java`) |
| Símbolos (CSV, 25.645 filas) | `C:\Users\jferr\AppData\Local\Temp\opencode\ghidra_symbols.csv` (columnas `addr,name,source,external,exec`) |
| Xrefs muros | `...\ghidra_walls.txt` (callers/callees de las 28) |
| Detalle + decompilados | `...\ghidra_walls2.txt` |
| Logs | `...\ghidra_console*.log`, `ghidra_full_out.log`, `ghidra_dump/export/walls*.log` |

Abrir en GUI: Ghidra → Open Project → `...\ghidra_project\NFSMW.gpr` → programa
`NFSMWRELEASE.ELF`. **No abrir en GUI mientras corra un headless sobre el mismo proyecto.**

Comando de referencia (headless, con `JAVA_HOME` al JDK 21):
`analyzeHeadless.bat <projdir> NFSMW -process NFSMWRELEASE.ELF -noanalysis -scriptPath <scripts> -postScript <Script>.java <args>`

## 2. Mapa de memoria del original (24 bloques)

| Bloque | Rango | Tamaño | Permisos |
|---|---|---|---|
| `.init` | 80003100–8000348b | 908 B | rwx |
| `.text` | 800034a0–803a41b7 | 3.804.440 B | r-x |
| `.over` | 803a41b8–803c8b4f | 149.912 B | r-x (= `zFeOverlay`; confirma la trampa de sección `.over`) |
| `.ctors` / `.dtors` | 803c8b60–803c8c5f | 196 / 32 B | rw |
| `.rodata` | 803c8c60–8041516b | 312.588 B | r-- |
| `.data` | 80415180–8045655b | 267.228 B | rw |
| `.bss` | 80456560–804fedab | 690.252 B | rw (no init) |
| `.sdata` / `.sbss` / `.sdata2` | 804fedc0–80500c97 | 2796 / 1656 / 3416 B | rw |
| `EXTERNAL` | 80501000–… | 296 B | rw |
| `.debug` | (debug) | **91.947.256 B** | — DWARF del SN ProDG: minable con `tools/dwarf*.py` |
| `.line` / `.symtab` / `.strtab` | (debug) | 5,4 MB / 420 KB / 878 KB | — |
| `.debug_pubnames` / `_sfnames` / `_srcinfo` | (debug) | 501 KB / 243 KB / 1,3 MB | — nombres públicos, fuentes |

Nota del loader: salta `.sbss2` (tamaño 0). Sin efecto.

## 3. Símbolos: el ELF trae la tabla completa

- **25.645 símbolos**, **18.899 en memoria ejecutable**, con nombres C++ reales mangled
  (`AITrafficManager::Update`, `AICopManager::SpawnCop`, `rand_point_in_circle__Fv`…).
- Cruce contra `build/GOWE69/report.json` (18.432 funciones nuestras por `virtual_address`):
  **solo 116 símbolos exec sin correspondencia, todos explicados, ninguno es juego sin mapear**:
  - Internos Dolphin OS (`__DBVECTOR`, `__OSEV*`, `__OSSystemCallVectorEnd`, `NotDvdDsi`, `__RAS_OSDisableInterrupts_end`).
  - Etiquetas locales del codec VP6 (`intra_loop`, `idctcolumn64`, `filt2d_loop1/2`, `idct64_*`, `unpack_loop`, `filt1d_loop`).
  - Runtime MWCC (`_savegpr_15…31`, `_restgpr_15…31`).
  - Stubs debugger SN ProDG (`cmdFS_*`, `FS_Continue`, `tunerprotocol`, `checkexternal`, `cmdThreadList`).
  - Runtime VM (`__VMBASESetupVMRegisters_*`).
- **Conclusión: nuestro mapa de funciones está completo.** No hay código de juego escondido.

## 4. Estado del matching en el momento del análisis

18.432 funciones, 18.403 matched (99,84%), **28 sin cerrar**, 518/616 unidades completas.
Código 99,14 %, dato 40,04 %.

| vaddr | B | % | Unidad | Función |
|---|---|---|---|---|
| 8007c390 | 3868 | 99,91 | zCamera | `Update__8ICEMoverf` |
| 8008095c | 3604 | 97,11 | zCamera | `__static_initialization_and_destruction_0` |
| 802feab0 | 2980 | 99,97 | zWorld2 | `HolePunchAvoidables__8WRoadNavP9NavCookieiff` |
| 802cb51c | 2908 | 98,62 | zWorld | `RenderFlaresOnCar__…` |
| 8008f098 | 2352 | 98,88 | zEagl4Anim | `Initialize__Q25EAGL413DynamicLoader…` |
| 80109358 | 2072 | 93,31 | zEcstasy | `epCalculateLocalDirectionalPOS16__…` |
| 80109bb8 | 2044 | 99,33 | zEcstasy | `UpdatePlatInfo__27eLightMaterialPlatInterface` |
| 8026c574 | 1588 | 99,32 | zPlatform | `ActualReadJoystickData__Fv` |
| 800d80fc | 1240 | 97,71 | zEAXSound2 | `GenerateRoadNoise__16CARSFX_RoadNoise` |
| 80075c44 | 992 | 99,63 | zCamera | `Update__19TrackCarCameraMoverf` |
| 80321474 | 924 | 95,67 | steering | `SimThread_Step` |
| 802c9fb8 | 876 | 99,38 | zWorld | `UpdateWheelYRenderOffset__13CarRenderInfo` |
| 8024b310 | 856 | 97,21 | zPhysicsBehaviors | `UpdateLoaded__Q217SuspensionTraffic4Tireffff` |
| 8030183c | 816 | 99,66 | zWorld2 | `InitAtSegment__8WRoadNavscf` |
| 803598fc | 740 | 94,39 | criticalpath | `VP6_PredictFilteredBlock` |
| 802bc0a4 | 708 | 97,18 | zTrack | `GetLoadingPriority__…` |
| 802dfcac | 684 | 99,27 | zWorld | `DefragmentPool__9CarLoader` |
| 8034bac4 | 632 | 50,45 | madidct | `IdctColumn` (a medio gas: objetivo para agentes) |
| 802a2248 | 596 | 98,26 | zSpeech | `Setup__Q26Speech13RoadblockFlow` |
| 8031f7fc | 588 | 98,30 | steering | `HandleTriggers` |
| 8034bd3c | 516 | 46,37 | madidct | `IdctRow` (a medio gas: objetivo para agentes) |
| 800a3090 | 456 | 98,07 | zEagl4Anim | `EvalState__Q29EAGL4Anim14FnRawStateChan…` |
| 80321330 | 324 | 94,69 | steering | `SimThread_Init` |
| 802dd360 | 304 | 97,37 | zWorld | `SetMemoryPoolSize__9CarLoaderi` |
| 80320840 | 276 | 88,62 | steering | `Effect_Init` |
| 8010ac40 | 268 | 93,97 | zEcstasy | `eProject__FfffPA3_fPfN44` |
| 8031fa48 | 260 | 95,77 | steering | `CookValues` |
| 803210b8 | 196 | 93,27 | steering | `Effect_PerformEnvelope` |

## 5. Auto-análisis completo

396 s, `Analysis succeeded`, guardado. **18.455 funciones** (23 más que las nuestras:
thunks/fragmentos descubiertos). Sin errores.

### 5.1 Grafos de llamadas de los muros (ground truth)

| Muro | Caller(s) | Callees |
|---|---|---|
| `Setup` RoadblockFlow | `Service__Q26Speech13RoadblockFlow` | `GetRoadblock`, `RBUpdate`, `GetRandomCop`, `SubRBReply`, `RBPosition`, `GetCopInRB`, `GetRandomActiveCop`, `IsCopSpeechPlaying`, `bRandom`, `RBWarning` (10) |
| `Effect_Init` | `VDevice_DownloadEffect` | `sin` |
| `eProject` | `eRotTransPers` | `PSMTXMultVec` (confirma guess PSMTX) |
| `SimThread_Step` | `SteeringSamplingCallback` | `OSGetTick`, `SIControlSteering`, `Effect_Update` |
| `SimThread_Init` | `VDevice_Init` | `OSGetTick` |
| `CookValues` | `SteeringSamplingCallback`, `LGOpen`, `InitDevice` | `HandleTriggers`, `HandlePedals` |
| `HandleTriggers` | `CookValues` | 0 |
| `Effect_PerformEnvelope` | `Effect_Update` | 0 |
| `DefragmentPool` | `FinishedGameLoading`, `MakeSpaceInCarMemoryPool`, `RemoveSomethingFromCarMemoryPool`, `CompositeSkin` | 16 (`bMalloc`/`bFree`, `DefragmentAllocation`, `RefreshAllRenderInfo`…) |
| `SetMemoryPoolSize` | `LoadGlobalChunks`, `BeginLoading` | 10 (pool/tracker) |
| `Initialize` DynamicLoader | su ctor | `strstr/strcmp/strcpy/strlen/memmove`, `DefaultMalloc`, `elfhash` |
| `EvalState` RawStateChan | 0 directos | `Decode` |
| `epCalculate…` | `epRenderStrips` | 0 (puro cómputo) |
| `UpdatePlatInfo` | `BuildData__14eLightMaterial` | 0 |
| `ActualReadJoystickData` | `DVDErrorTask`, `eExStartup` | 0 |
| `GenerateRoadNoise` | `UpdateParams__16CARSFX_RoadNoisef` | 0 |
| `InitAtSegment` | `GetSpawnLocation__14AISpawnManager` | 0 |
| `GetLoadingPriority` | `AssignLoadingPriority`, `ChooseSectionToJettison` (×2 sitios) | 0 |
| `IdctColumn` / `IdctRow` | `idctcompute` | 0 (puro cómputo) |
| `VP6_PredictFilteredBlock` | `VP6_ReconstructBlock` | `Var16Point`, `VP6_PredictFiltered` |
| `static_init` zCamera | `_GLOBAL_.I._6Camera.StopUpdating` (ctor global) | 0 |

### 5.2 Causa raíz de los "cuerpos de 4–12 bytes" (RESUELTO)

13 muros gordos salían con cuerpo de 4–12 B, 0 callers y 1 data-ref. No son virtuales
ni stubs: **Ghidra trunca el cuerpo en la primera instrucción `psq_st` (paired-singles
cuantizado de Gekko), que su sleigh stock no decodifica**. Probado leyendo el ELF:

| Función | Dirección corte | Word | Opcode |
|---|---|---|---|
| `GenerateRoadNoise` | 800d8104 (`f3810020`) | `psq_st` | 60 |
| `static_init` zCamera | 80080964 (`f2e10060`) | `psq_st` | 60 |
| `epCalculate` | 80109364 (`f1c100e0`) | `psq_st` | 60 |

El patrón es siempre `stwu / mflr / [mfcr]` (8–12 B) y luego el primer spill `psq_st`
→ `Bad instruction - Truncating control flow`. Ghidra 12.1.2 no trae variante de
lenguaje Gekko/paired-singles (revisado `Processors/PowerPC/data/languages/`: no hay
`gekko`, solo `4xx/e500/altivec/vle/SPE`). Implicaciones:

- Símbolos, callers y data-refs de Ghidra **sí son válidos** para esas funciones.
- Cuerpos y decompilado de funciones con `ps_*` **no**: seguir con `fndiff.py`/`lmap.py`.
- Futuro (no hecho): sleigh Gekko de la comunidad o script que marque `psq_*` como
  datos-para-saltar; re-importar son solo unos minutos.

## 6. Decompilados C de referencia (Ghidra, PowerPC BE 32)

### 6.1 `Setup__Q26Speech13RoadblockFlow` (802a2248, 596 B) — COMPLETO

Estructura exacta de ramas (plantilla para reordenar nuestro C++). Puntos clave:
`param_1+0x24`: `(x^1)&1` decide rama; `&0x100` subrama; al final `&= 0xefffffff`.
Event IDs `0x4d/0x4e`, umbrales float en `DAT_804077c0/804077c4`, slots de vtable
`0x2e4/0x2e0`, `0x2d4/0x2d0`, `0x34c/0x348`, `0x2cc/0x2c8`, `0x2f4/0x2f0`, `0x9c/0x98`.

```c
void Setup__Q26Speech13RoadblockFlow(int param_1)
{
  bool bVar1;
  int *piVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  double dVar6;

  iVar4 = _Q33UTL11Collectionst9Singleton1Z7SoundAI_mInstance;
  if (((*(uint *)(param_1 + 0x24) ^ 1) & 1) == 0) {
    if ((*(uint *)(param_1 + 0x24) & 0x100) == 0) {
      bVar1 = false;
      iVar5 = IsCopSpeechPlaying__Q26Speech7Manager18SPCHType_1_EventID(0x4d);
      if ((iVar5 != 0) ||
         (iVar5 = IsCopSpeechPlaying__Q26Speech7Manager18SPCHType_1_EventID(0x4e), iVar5 != 0)) {
        bVar1 = true;
      }
      dVar6 = (double)bRandom__Ff((double)DAT_804077c0);
      if ((dVar6 <= (double)DAT_804077c4) || (bVar1)) {
        RBWarning__10MiscSpeech();
      }
      else {
        piVar2 = (int *)GetRandomActiveCop__7SoundAIib(iVar4,1,0);
        if (piVar2 == (int *)0x0) {
          return;
        }
        RBUpdate__11EAXDispatchP6EAXCopSc(*(undefined4 *)(iVar4 + 0xe0),piVar2,1);
        (**(code **)(*piVar2 + 0x2e4))((int)piVar2 + (int)*(short *)(*piVar2 + 0x2e0));
      }
    }
    else {
      bVar1 = true;
      iVar5 = GetRoadblock__7SoundAI(_Q33UTL11Collectionst9Singleton1Z7SoundAI_mInstance);
      if (iVar5 != 0) {
        iVar5 = GetRoadblock__7SoundAI(iVar4);
        iVar5 = (**(code **)(*(int *)(iVar5 + 4) + 0x9c))
                          (iVar5 + *(short *)(*(int *)(iVar5 + 4) + 0x98));
        bVar1 = iVar5 < 2;
      }
      if (bVar1) {
        RBPosition__10MiscSpeechi(*(undefined4 *)(param_1 + 0x28));
      }
      else {
        piVar2 = (int *)GetCopInRB__7SoundAI(iVar4);
        if ((piVar2 == (int *)0x0) &&
           (piVar2 = (int *)GetRandomCop__7SoundAIi(iVar4,2), piVar2 == (int *)0x0)) {
          RBWarning__10MiscSpeech();
        }
        else {
          (**(code **)(*piVar2 + 0x2d4))((int)piVar2 + (int)*(short *)(*piVar2 + 0x2d0));
        }
      }
    }
  }
  else {
    piVar2 = (int *)GetRandomCop__7SoundAIi(_Q33UTL11Collectionst9Singleton1Z7SoundAI_mInstance,1);
    piVar3 = (int *)GetRandomCop__7SoundAIi(iVar4,2);
    if ((*(int *)(param_1 + 0x30) < 2) || (piVar3 == (int *)0x0)) {
      RBUpdate__11EAXDispatchP6EAXCopSc(*(undefined4 *)(iVar4 + 0xe0),piVar2,0);
      iVar4 = (**(code **)(*piVar2 + 0x34c))((int)piVar2 + (int)*(short *)(*piVar2 + 0x348));
      if (iVar4 == 0) {
        (**(code **)(*piVar2 + 0x2cc))((int)piVar2 + (int)*(short *)(*piVar2 + 0x2c8));
      }
    }
    else {
      dVar6 = (double)bRandom__Ff((double)DAT_804077c0);
      if (dVar6 <= (double)DAT_804077c4) {
        SubRBReply__11EAXDispatch(*(undefined4 *)(iVar4 + 0xe0));
      }
      else {
        (**(code **)(*piVar3 + 0x2f4))((int)piVar3 + (int)*(short *)(*piVar3 + 0x2f0));
      }
    }
  }
  *(uint *)(param_1 + 0x24) = *(uint *)(param_1 + 0x24) & 0xefffffff;
  return;
}
```

### 6.2 `Effect_Init` (80320840, 276 B) — COMPLETO

Cero-inicialización con patrón `do/while` de 2×8 words + cola, y si el flag (vía r13,
small-data) está a 0, bucle de 0x40 que rellena `g_iRampUpTable[i] = acc/0x3f` y
`g_iQuarterSineTable[i] = amp*sin(...)`, poniendo el flag a 1. (`unaff_r13` = base r13.)

```c
void Effect_Init(int param_1)
{
  float fVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  int unaff_r13;
  uint uVar5;
  int iVar6;
  double dVar7;

  puVar4 = (undefined4 *)(param_1 + -4);
  iVar3 = 2;
  do {
    puVar4[1] = 0;
    iVar3 = iVar3 + -1;
    puVar4[2] = 0;
    puVar4[3] = 0;
    puVar4[4] = 0;
    puVar4[5] = 0;
    puVar4[6] = 0;
    puVar4[7] = 0;
    puVar4 = puVar4 + 8;
    *puVar4 = 0;
  } while (iVar3 != 0);
  if (*(int *)(unaff_r13 + -0x74f0) == 0) {
    uVar5 = 0;
    iVar6 = 0;
    iVar3 = 0;
    do {
      dVar7 = sin((double)(*(float *)(unaff_r13 + -0x65a8) *
                           (float)((double)CONCAT44(0x43300000,uVar5 ^ 0x80000000) -
                                  *(double *)(unaff_r13 + -0x65b0)) *
                           *(float *)(unaff_r13 + -0x65c0)));
      uVar5 = uVar5 + 1;
      iVar2 = iVar6 / 0x3f;
      fVar1 = *(float *)(unaff_r13 + -0x65a4);
      iVar6 = iVar6 + 0x400;
      *(short *)((int)&g_iRampUpTable + iVar3) = (short)iVar2;
      *(short *)((int)&g_iQuarterSineTable + iVar3) = (short)(int)(fVar1 * (float)dVar7);
      iVar3 = iVar3 + 2;
    } while ((int)uVar5 < 0x40);
    *(undefined4 *)(unaff_r13 + -0x74f0) = 1;
  }
  return;
}
```

### 6.3 `GenerateRoadNoise` y `static_init` zCamera — decompilado NO disponible

Fallan con `Bad instruction - Truncating control flow` por el §5.2 (`psq_st` en
800d8104 y 80080964). Usar callers/xrefs + `fndiff.py`.

## 7. Cómo usar este documento con agentes

- `Setup` zSpeech: darles §6.1 como plantilla de orden de llamadas/ramas; el tie es
  scheduling entre esas 10 llamadas (verificar con `fndiff.py` por bloques).
- `Effect_Init`/`eProject`/`CookValues`/`SimThread_*`: grafos §5.1 para comprobar
  orden y presencia (`sin`, `PSMTXMultVec`, `HandlePedals`, `OSGetTick`).
- `IdctColumn`/`IdctRow` (50 %/46 %, 0 callees): puro cómputo, sin ayuda de grafo;
  comparar contra libjpeg/mad de referencia por nombre ya conocido.
- Funciones con `ps_*`: no pedir cuerpos a Ghidra; usar `lmap.py` + este doc solo
  para callers (p. ej. `GenerateRoadNoise` ← `UpdateParams`, `epCalculate` ←
  `epRenderStrips`, `static_init` ← `_GLOBAL_.I._6Camera.StopUpdating`).
- DWARF de 91 MB (§2): pendiente de minar con `tools/dwarf*.py` (rutas de fuente,
  líneas, tipos originales).

## 8. Pendiente / trabajo futuro

1. Minar `.debug` con herramientas propias (puede dar líneas fuente exactas por función).
2. Sleigh Gekko comunitario para `psq_*` → re-import (minutos) → cuerpos+decomp
   de las 13 del §5.2.
3. Exportar data-xrefs (qué global toca cada muro) — no hecho; pedir si hace falta.
4. Decompilar bajo demanda cualquier otra función: `ExportWalls2.java` ya sabe
   (`decompileAt`), solo añadir direcciones.

## 9. Resumen global del proyecto (`ExportSummary.java` → `ghidra_summary.txt`)

18.455 funciones con cuerpo, 2.903.538 B mapeados (el resto de `.text` hasta 3,8 MB:
truncados `ps_*`, datos embebidos y alineación).

Top subsistemas por bytes: `Fv` (501 helpers `void f(void)`, 80 KB de long tail),
`static_init` (45 TUs, 28,6 KB), `Realm::GCInterface` (memory card, 24,5 KB),
`bChunk` (93 loaders de chunks de pista, 22 KB), `VP6` (54 fns vídeo, 21,5 KB),
`gcc2_compiled` (70 TUs), `GManager`, `Speech::{Manager,Cache,StrategyFlow,
PursuitFlow}` (~30 KB: el sistema de voces de polis), `lua_State` (122 fns: **Lua
embebido** + `LuaBindery::BindToGameCode` 4508 B + `BindMessagesToLua` 7700 B),
`EAXCop` (74), `EAXSound`, `TrackStreamer` (32), `SoundAI`, `GRaceParameters` (71).

Mayores funciones: `InitializeTables__Fv` 9908 B (80195d3c), `~SimSystem` 8300 B,
`static_init` 7996 B (8022a350), `BindMessagesToLua` 7700 B, `ResolveCarBanks`
7508 B (audio de coches), `NotificationMessage__13UIQRCarSelect` 5096 B,
`VP6_DecodeBlock` 4728 B, `UpdateListing__8PVehicle` 4616 B (garaje/BD vehículos),
`hash64__6Attrib` 4416 B, `Init2__Q26Speech7Manager` 4064 B,
`BaseNotifySound__10MenuScreen` 3784 B, `UpdateTaskTrc{Load,Save}File` (3780/3476 B,
memory card), `EvalSQTMask` anim (3728 B), `SetupOptions__9PauseMenu` 3460 B,
`HUFF_decompress` 3392 B, `PATHI_serviceaction` 3252 B (pathfinding),
`DrawPartName__18FEShoppingCartItem` 3144 B (tienda tuning),
`ProcessUpdate__16CARSFX_RoadNoise` 3136 B, `ProcessPadsForPackage__7FEngine` 3080 B,
`_Transform__3MD5`, `qsort`, `__GXInitGX` 2360 B (init GX Dolphin).

Arranque: `entry@80003100` (cuerpo 340 B) → `SNDebugInit`, `__init_user`, `memset`,
`main@801fbc94`, `__init_vm`, `exit`, `OSInit`, `DBInit`, `OSReport`,
`__init_hardware` (→ `__OSPSInit`, `__OSCacheInit`, `__OSFPRInit`). `main` sale con
cuerpo 8 B (truncado `ps_*`, pendiente §8.2); el juego arranca en
`eExStartup__Fv@801032f8` (búsqueda exacta de símbolo, no por prefijo).

## 10. Flujo de arranque entry → eExStartup (+ MainLoop) — RESUELTO

Método: decompilado Ghidra de `entry`/helpers `.init` (`ghidra_boot*.txt`) +
decoder PPC propio (`Temp\opencode\ppc_boot.py` → `ppc_boot_out.txt`) que salta el
límite `ps_*` y resuelve cada `bl` contra `ghidra_symbols.csv`.
`main`, `eExStartup` y `MainLoop` decodificados al 100 % (Ghidra los truncaba a 8 B).

### 10.1 `entry@80003100` (.init, 340 B, decompilado completo)

1. `FUN_80003300()` = no-op (`return 0`, probable `__init_fp`).
2. `FUN_80003254()` — ojo: va ANTES de OSInit en el binario pero lo listo aquí por
   claridad: parsea args del apploader (`*0x800000f4`, fixups de reloc) y devuelve
   `(argc, argv)` que reciben `__init_vm` y `main`.
3. `FUN_80003334()` = chequea SPR HID2 bit 29 (¡paired singles!) y devuelve
   `DAT_80003328` (arena). Truncado por `ps` tras el chequeo: probable init de
   memoria/arena + habilitación PS.
4. `__init_hardware()` → `__OSPSInit`, `__OSCacheInit`, `__OSFPRInit`.
5. `*0x80510cbc = *0x80510cb8 = 0xFFFFFFFF`; `memset(bss-ish, 0, 0xa884c)`;
   `memset(g_lgInitialized, 0, 0x678)`; `*0x80000044 = 0`.
6. Si apploader pide debugger → `SNDebugInit()` (`SNDVDEmuInit`, `DCFlushRange`,
   `snInitFileserver`, `ICInvalidateRange`).
7. `DBInit()`, `OSInit()`; si combo reset → `FUN_800032c0()` → `OSResetSystem(0,0,0)`.
8. `__init_vm()` (vacía) + `__init_vm(argc, argv)`.
9. `__init_user()` → `__init_cpp` = **constructores estáticos (.ctors)**.
10. Si debugger: `OSReport` versión LibSN + "Waiting for SN Debugger" + espera.
11. `main(argc, argv)` → `exit(status)`.

### 10.2 `main@801fbc94` (decodificado crudo, ~120 insns)

1. `__main@80319eec` → `__do_global_ctors` (segunda capa de ctors, estilo MWCC).
2. `InitializeEverything(argc, argv)` (§10.3).
3. `WriteFreekerBaseAddressBeacon()`.
4. Si flag → `StartSkipFERace` si no `LoadFrontend(GameFlowManager)`; marca ready=1.
5. **Bucle**: `Synchronize(Scheduler Timer)` → si frame pendiente: `bGetTicker` ×2,
   `bGetTickerDifference`, `MainLoop(dt)` (§10.6); sale cuando se apaga el flag.

### 10.3 `InitializeEverything__FiPPc@801fb4d8` (56 inits, orden real)

`bPListInit`, `DebugWorld`, `Physics::Info`, `QueuedFiles`, `EmitterSystem`,
`DebugVehicleSelection`, `MathTimingTest`, `FirstBreakpoint`, `GManager`,
`MemoryCreatePersistentPool`, `FrontendDatabase`, `CarLoader`, `VehicleDamage`,
`Hermes::System`, `BigFiles`, `SoundDriver`, `EventSequencer`, `FEManager::Input`,
`SpaceNodes`, `Platform`, `Joylog`, `DisplaySystem`, `StandardModels`,
`IOModule::EnableUpdating`, `BootLoadingScreen`, `Scheduler`, `GRaceDatabase`,
`FEManager`, `Config`, `SeedRandomNumber`, `WWorld`, `ReserveMemoryPool`,
`SmokeableSpawner`, `LoadGlobalAChunks`, (`IsAmerica`), `FEPackageManager`,
**`eInitEngine`**, `LoadGlobalChunks`, `eMathInit`, `UnloadFrontEndVault`,
`bGetTicker`, `GetIOModule`, `InitSharedStringPool`, `ResourceLoader`, `Stomper`,
`AnimCtrls`, `MemoryCard`, `GInfractionManager`, `ICEManager`, `emEventManager`,
`IOModule::Initialize`, `GManager::InitializeRaceStreaming`, `bInitTicker`,
`DemoDiscManager`, `CarRender`, `EventManager`.

### 10.4 `eInitEngine@800f486c` → `eInitEnginePlat@800fe0e8`

`eInitEngine`: `eAllocateFrameMallocBuffers`, `bNewSlotPool`, **`eInitEnginePlat`**,
`eInitSolids`, `eInitModels`, `eInitTextures`, `elInit`, `InitNFSAnimEngine`,
`eInitEnvMap`, `epInitViews`, `InitSimpleModelAnim`.
`eInitEnginePlat` (único caller de `eExStartup`, confirmado por xrefs):
`eInitSunPat`, `eInitGX`, `eDrawStartup`, **`eExStartup`**, `eInitTexture`,
`eInitEnvMap`, `eSetDisplaySystem`, `InitSlotPools`, `SetScreenBuffers`.

### 10.5 `eExStartup__Fv@801032f8` (decodificado crudo, 27 llamadas, fin `blr`)

1. `PlatformInitJoystick`; poll `ActualReadJoystickData` hasta éxito o 500 intentos.
2. `Init__12cSpecularMap` + 3× `Init__15cQuarterSizeMap` (`1,1,5` / `0,6,4` / `0,1,7`).
3. `eInitContrastSurface`, `eInitHorizonFogDisplayList`.
4. `PSMTXScale` → `PSMTXTrans` → `PSMTXConcat` (matriz base).
5. `eDEMOInitROMFont`; `OSGetResetCode` → rama progresivo (`OSGetProgressiveMode`,
   `VIGetDTVStatus` → `eProgressiveScan_EURGB60SetMode`) o TV (`VIGetTvFormat` ×2,
   `OSGetEuRgb60Mode` → `EURGB60` o `NTSC/PAL`) o `__InitRenderMode` + ModeChecks.
6. `vsVtxAttrFmt(0)`, `eDEMODeleteROMFont`. Siguiente símbolo: `eWaitRetrace`.

### 10.6 `MainLoop__Ff@801fbb1c` (decodificado crudo, 38 llamadas, fin `blr`)

`bGetTicker`, `MainLoopBreakpoint`, `PrepareRealTimestep(dt)`, `bSyncTaskRun`,
`IOModule::{Get,Update}`, `Scheduler::Run`, `Attrib::Database::CollectGarbage`,
`Main_AnimateFrame`, `PrepareWorldTimestep`, `ServiceJoylog`,
`CheckTweakerTriggers`, `GameFlowManager::Service`, `CheckForDemoDiscTimeout`,
`emProcessAllEvents`, si `Sim::Exists` → `World_Service`, `ServicePlatform`,
`ServicePreculler`, `FEManager::{Get,Update}`, `EventManager::RunEvents`,
`EAXSound::Update`, `ServiceResourceLoading`, `MainLoopCheckForFatalDiscError`,
`RenderConn::UpdateLoading`, `TrackStreamer::ServiceNonGameState`,
`HandleTrackStreamerLoadingBar`, `TickOverTimeOfday`, `Main_DisplayFrame`,
`Sim::StartProfile`, `VerifyJoylogChecksum`, `AdvanceWorldTime`,
`MaybePrintUnusedTextures`, `MaybeDoMemoryProfile`, `AdvanceRealTime`,
`bCountFreeMemory` → `OSReport` (aviso memoria). Notas del decoder: `OP_59`
en `801fbb48` = float single (probable `fsel`/clamp del dt que entra a
`PrepareRealTimestep`); `XL_193` en `801fbc74` = `cror`.

## 11. Las 56 inicializaciones de `InitializeEverything@801fb4d8` (orden real)

Decodificado crudo (`ppc_boot_out.txt`): 58 `bl` (56 subsistemas;
`bReserveMemoryPool` ×2 y `GetIOModule` ×2). Lógica del orden: memoria →
plataforma → sistema base → motor → presentación → sonido → frontend → IO →
eventos → sim → gameplay → coches → debug/física. Los inits de debug van
DESPUÉS del gameplay (para engancharse); física e ICE cierran (necesitan mundo).

| # | Dirección | Llamada (args) | Propósito | Código nuestro |
|---|---|---|---|---|
| 1 | 80062210 | `bReserveMemoryPool(7)` | Reserva pool memoria 7 | bWare |
| 2 | 80062210 | `bReserveMemoryPool(8)` | Reserva pool memoria 8 | bWare |
| 3 | 80062e20 | `bMemoryCreatePersistentPool(4096)` | Pool persistente 4 KB | bWare |
| 4 | 8005c410 | `bInitTicker(f)` | Calibra ticker (float rodata) | bWare |
| 5 | 8005c414 | `bGetTicker()` | Marca temporal base (marker) | bWare |
| 6 | 80063e78 | `bInitSharedStringPool(0x8000)` | Pool strings 32 KB | bWare |
| 7 | 8026b534 | `InitPlatform()` | Plataforma GC base | `GameCube/Src/Platform_G.cpp` |
| 8 | 8020de98 | `FirstBreakpoint()` | Hook breakpoint debugger SN (sin fichero en src) | — (cf. stubs SN) |
| 9 | 8005d5e0 | `bMathTimingTest()` | Autotest timing bMath | `bWare/Src/bMath.cpp` |
| 10 | 801fb34c | `IsAmerica()` | Región NTSC-U vs PAL | BuildRegion |
| 11 | 8026db40 | `Init__15DemoDiscManager(argc,argv)` | Disco demo/kiosco | `Misc/DemoDisc.cpp`, `GameCube/DemoDisc_G.cpp` |
| 12 | 8005c874 | `bPListInit(6144)` | Allocator pool 6 KB | bWare |
| 13 | 8020dc1c | `InitConfig()` | Configuración/tweaks | `Misc/Config.cpp` |
| 14 | 8020158c | `InitJoylog()` | Grabador de input (demos/replays) | `Misc/Joylog.cpp` |
| 15 | 801fb3d0 | `SeedRandomNumber()` | Semilla RNG | — |
| 16 | 80202128 | `InitQueuedFiles()` | Cola ficheros asíncronos | `Misc/QueuedFile.cpp` |
| 17 | 802c2aec | `emEventManagerInit()` | Gestor eventos EM | `World/EventManager.cpp` |
| 18 | 8010aab4 | `eMathInit()` | Librería math Ecstasy | zEcstasy |
| 19 | 800f486c | `eInitEngine()` | Motor Ecstasy (§10.4) | zEcstasy |
| 20 | 8026b674 | `InitDisplaySystem()` | Display/GX | `GameCube/Src/Platform_G.cpp` |
| 21 | 80112bc4 | `Init__13EmitterSystem` | Partículas | `Ecstasy/EmitterSystem.cpp` |
| 22 | 801fc7c0 | `InitResourceLoader()` | Cargador de recursos | `Misc/ResourceLoader.cpp` |
| 23 | 800a95b8 | `InitializeSoundDriver()` | Driver sonido EAX | `EAXSound/` (fichero pendiente) |
| 24 | 8016d43c | `Init__16FEPackageManager` | Paquetes UI | `Frontend/FEPackageManager.cpp` |
| 25 | 801ff53c | `LoadGlobalAChunks()` | Chunks globales audio | flujo/chunks |
| 26 | 80134534 | `Init__9FEManager` | Manager frontend | `FEng/` |
| 27 | 801fff40 | `BootLoadingScreen()` | Pantalla de carga | `Frontend/MenuScreens/Loading/` |
| 28 | 801fb440 | `InitBigFiles()` | Archivos .big (fichero pendiente) | — |
| 29 | 80137870 | `InitMemoryCard()` | Memory card | `Frontend/MemoryCard/`, `GameCube/MemoryCardImp.cpp` |
| 30 | 801ff5a0 | `LoadGlobalChunks()` | Chunks globales | flujo/chunks |
| 31 | 80163070 | `InitFrontendDatabase()` | BD frontend (`cFrontendDatabase`) | zFe2 |
| 32 | 801ee234 | `GetIOModule()` | Puntero IOModule (marker) | `Input/IOModule.h` |
| 33 | 801ee970 | `Initialize__8IOModule` | Inicializa IO | Input |
| 34 | 801345cc | `InitInput__9FEManager` | Input del FE | `FEng/` |
| 35 | 801ee234 | `GetIOModule()` | Re-obtiene puntero (marker) | Input |
| 36 | 801ee290 | `EnableUpdating__8IOModule(1)` | Activa updates IO | Input |
| 37 | 801eb634 | `Init__12EventManager` | Eventos de mundo (≠ EM) | `World/EventManager.cpp` |
| 38 | 8020adb4 | `Init__Q26Hermes6System` | Bus de mensajes | `Misc/Hermes.cpp` |
| 39 | 801ed864 | `Init__9Scheduler(f)` | Planificador tareas | `Main/Common/Scheduler.cpp` |
| 40 | 80307688 | `Init__6WWorld` | Mundo | `World/Common/WWorld.cpp` |
| 41 | 801eb990 | `Init__14EventSequencer(f)` | Motor eventos/sim | `Main/Common/EventSequencer.cpp` |
| 42 | 801ff374 | `UnloadFrontEndVault()` | Descarga vault FE | `Gameplay/GVault.cpp` |
| 43 | 801ab068 | `Init__8GManager(str)` | Game manager (string modo) | `Gameplay/GManager.cpp` |
| 44 | 801a3ab0 | `Init__13GRaceDatabase` | BD carreras | `Gameplay/GRaceDatabase.cpp` |
| 45 | 801ab9e4 | `InitializeRaceStreaming` | Streaming carreras | `Gameplay/GManager.cpp:261` |
| 46 | 801b4574 | `Init__18GInfractionManager` | Infracciones/polis | `Gameplay/GInfractionManager.cpp` |
| 47 | 80221294 | `Init__16SmokeableSpawner` | Rompibles persecución | `Physics/`, `World/VisibleSection` |
| 48 | 802c7644 | `InitCarRender()` | Render coches | `World/CarRender.cpp` |
| 49 | 802c77d0 | `InitStandardModels()` | Modelos estándar | `World/CarRender.cpp:1073` |
| 50 | 802dd210 | `InitCarLoader()` | Cargador coches | `World/CarLoader.cpp:533` |
| 51 | 802e8d74 | `InitVehicleDamage()` | Daños | `World/VehiclePartDamage.cpp:446` |
| 52 | 802c5f30 | `Init__21DebugVehicleSelection` | Selección debug | `World/DebugVehicleSelection` (zWorld) |
| 53 | 802c6750 | `Init__10DebugWorld` | Mundo debug | `World/DebugWorld.cpp` (zWorld) |
| 54 | 802087c8 | `InitStomper()` | Guardián memoria debug | `Misc/Stomper.cpp` |
| 55 | 80225df4 | `Init__Q27Physics4Infov` | Info física | `Physics/` (zPhysics) |
| 56 | 8004710c | `InitAnimCtrls()` | Controladores animación | `Animation/AnimCtrl.cpp:19` |
| 57 | 802d91cc | `InitSpaceNodes()` | Partición espacial | `World/SpaceNode.cpp:134` |
| 58 | 8007dc64 | `Init__10ICEManager(ptr)` | Cámaras cinemáticas (con puntero datos) | `Camera/ICE/ICEManager.cpp` (zCamera) |

Epílogo: `counter++` en global y `blr` (siguiente símbolo: `WriteFreekerBaseAddressBeacon`).

## 12. DWARF1: 10.188 tipos + 13.824 firmas reales (Ghidra 11.4 + ext)

Setup: Ghidra `11.4_DEV_20250425` + `ghidra-dwarf1` 11.3.1 (release experimental,
instalada en `Ghidra/Extensions/ghidra-dwarf1` de ese árbol), proyecto NUEVO
`NFSMW_DWARF` en `Temp\opencode\ghidra_project11` (el bueno de 12.1.2 intacto).
Claves técnicas: el analizador se llama `DWARF1` y viene con
`setDefaultEnablement(false)` → hay que lanzarlo como one-time analysis
(`AutoAnalysisManager.getAnalyzer("DWARF1")` + `scheduleOneTimeAnalysis` +
`startAnalysis` + `waitForAnalysis`; ver `Temp\opencode\ghidra_scripts\RunDwarf1.java`).
API 11.4 DEV difiere de 12.x: sin `DataTypeIterator` (`Iterator<DataType>`),
sin `Function.getPlateComment` (`getComment`), sin `getAnalyzers()`, paquete
decompiler `ghidra.app.decompiler`. Rareza: el `.debug` abre con `length=156,
version=17` (no DWARF1 canónico) pero la extensión parsea DIEs sin chequear
versión y funciona igual.

Resultado (exports en `Temp\opencode\`: `ghidra_fnsigs.csv` 1,6 MB,
`ghidra_structs.csv` 2 MB; scripts `ExportDwarfProbe.java`, `ExportDwarfFull.java`):
**18.442 funciones, 13.824 (75 %) con firma real** (nombre de clase + tipos y
nombres de parámetros + retorno); **4.065 structs `/DWARF/`, 48.688 miembros con
offset**. Las 28 unmatched, todas con firma:

| vaddr | Firma DWARF |
|---|---|
| 80075c44 | `CameraMover::Update(TrackCarCameraMover*, float dT)` |
| 8007c390 | `CameraMover::Update(ICEMover*, float dT)` (virtual distinta) |
| 8008095c | `__static_initialization_and_destruction_0(int, int)` (forma GCC) |
| 8008f098 | `DynamicLoader::Initialize(this, void*(*)(char*, bool*))` |
| 800a3090 | `bool FnAnim::EvalState(FnRawStateChan*, float time, State*)` (ns `FnAnim`) |
| 800d80fc | `CARSFX_RoadNoise::GenerateRoadNoise(this)` |
| 80109358 | `epCalculateLocalDirectionalPOS16(uint*, uint*, int, ushort*, int*, uchar*, int, int, eLightMaterial*, eLightContext*)` (10 params con nombre) |
| 80109bb8 | `eLightMaterialPlatInterface::UpdatePlatInfo(this)` |
| 8010ac40 | `eProject(float x, float y, float z, float[4]*, float*, float*, float*, float*, float*)` |
| 8024b310 | `Tire::UpdateLoaded(this, float lat_vel, float fwd_vel, float load, float dT)` |
| 8026c574 | `int ActualReadJoystickData()` (retorna int) |
| 802a2248 | `RoadblockFlow::Setup(this)` |
| 802bc0a4 | `int TrackStreamer::GetLoadingPriority(this, section*, position_entry*, bool calculating_jettison)` |
| 802c9fb8 | `CarRenderInfo::UpdateWheelYRenderOffset(this)` |
| 802cb51c | `CarRenderInfo::RenderFlaresOnCar(this, eView*, bVector3*, bMatrix4*, int force_light_state, int reflexion, int renderFlareFlags)` |
| 802dd360 | `CarLoader::SetMemoryPoolSize(this, int size)` |
| 802dfcac | `int CarLoader::DefragmentPool(this)` (retorna int) |
| 802feab0 | `WRoadNav::HolePunchAvoidables(this, NavCookie*, int, float current_offset, float delta_offset)` |
| 8030183c | `WRoadNav::InitAtSegment(this, short segInd, char laneInd, float timeStep)` (¡tipos estrechos!) |
| 8034bac4/bd3c | `IdctColumn/Row(int* src, int* dest)` |
| 803598fc | `VP6_PredictFilteredBlock(PB_INSTANCE*, short*, bp)` |
| 8031f7fc..80321474 | `HandleTriggers/CookValues/Effect_Init/Effect_PerformEnvelope/SimThread_Init/SimThread_Step` → **DEFAULT, 0 params: steering.c NO tiene DWARF** (compilado sin `-g`; consistente con ser los muros más duros) |

Structs clave (offsets exactos en `ghidra_structs.csv`): `CARSFX_RoadNoise`
(base 40 B + `m_pWheelCtl@40`, vols L/R@112-124), `RoadblockFlow`
(`mFlags@36`, `mPertinentRB@44`, `mNumBlocks@48`, handlers@52-64),
`eLightMaterial` (`NameHash@12`, `Name[28]@20`, `feData@48` =
`eLightMaterialData` 30 floats Dif/Spec/Env), `CarLoader`
(`MayNeedDefragmentation@36`, listas@40-88, `MemoryPoolMem@96/Size@100`),
`CarRenderInfo` (134 miembros: `mVelocity@4`, `LightMaterial_*@204-228`…),
`WRoadNav` (`mCurrentCookie@8`, filtros bool@76-96), `CameraMover`
(`Type@12`, `pView@24`, `pCamera@28`, `mWPos@40`, base 128 B para
`CubicCameraMover`/`CopViewCameraMover`/`DebugWorldCameraMover`),
`TrackStreamer` (79), `DynamicLoader` (`mSearchCallback@28`,
`mIsResolved@44`), `eView`/`eViewPlatInfo` (4 matrices 64 B),
`bVector3` (x,y,z,pad), `PB_INSTANCE` (150), `_DefragmentParams`,
`InputEffectState` (¡para steering aunque sin firmas!).
Moraleja para agentes: verificar prototipos y layouts contra este § antes de
pelear scheduling (p. ej. `short/char` en `InitAtSegment`, `bool` en
`GetLoadingPriority`, `int` en `ActualReadJoystickData`/`DefragmentPool`).

## 13. Oleada 13 agentes DWARF (2026-09-11): 0 cierres, 3 unidades al 100 %

13 agentes en paralelo (uno por grupo de muros), todos completados sin
regresiones (todo no-100 % revertido; `git` limpio en sus ámbitos). Estado
global tras la oleada: código 99,220 %, funciones 18.405/18.432 (99,854 %),
525/619 unidades. Unmatched: 25 (eran 28).

**Cerradas/verificadas al 100 % (no tocar):** `Setup` RoadblockFlow (zSpeech,
703 fns 100 %), `ActualReadJoystickData` (zPlatform, 136 fns 100 %, audit ok),
`VP6_PredictFilteredBlock` (criticalpath, 21 fns 100 %, audit 21/21).

**Cifras reales medidas** (corrigen las del §4/§12, que eran rancias):
static_init 99,839 %, Initialize DynamicLoader 99,643 %, GenerateRoadNoise
96,803 %, UpdatePlatInfo 99,961 %, GetLoadingPriority 94,972 %, IdctRow 496 B
(no 516) al 46,37 %, CookValues 99,077 %, Effect_Init 95,0 %.

**Vedas confirmadas por segunda vía + pistas nuevas:**
- zWorld2: H1 (bool→int explota a docenas de diffs), I1 (`SetLaneOffset(0)`
  neutra exacta), I2 (`const` es API). Hole = empate allocno 334 vs 901
  (pri 60); Init = 176 (5/30→3333) vs 212 (2/8→2500), falta live(212)≤6 o
  live(176)≥41.
- steering: layout sdata de la TU irreducible (`@961/1129/1130` vs
  `@348/454/455`); la plantilla §6.2 rompe scheduling (280 B).
- madidct: al objetivo le viven CTR+LR (`mtctr+mfctr+2mr` que nos falta en
  IdctRow); constantes y early-exit idénticos.
- zTrack: 2.640 formas, gradiente cero; hace falta evidencia nueva
  (`.greg` original), no más barridos.
- zEcstasy: epCalculate marco 0x170 vs 0x178 (temp de reload); eProject ciclo
  f0/f13; UpdatePlatInfo `fmuls` conmutado load-bearing.
- zWorld: RenderFlares necesita `@ha` del 0.0f vivo en preservado; Defrag
  necesita live(222)≤47; SetMemoryPool empate `rank_for_schedule`.
- zPhysics: veda dura cse2 + corte de bloque (r54/r61/r63 del `.cpp`).
- zEAXSound2: `slipBoost` NO está en DWARF (confirmado); queda bloque de
  unión + pareja f8/f10 (sched2 `INSN_PRIORITY`).
- zCamera/zEagl4Anim: techos de asignador (permutador 409+48 variantes, cero
  mejora); firmas y layouts ya correctos en fuente.

## 14. Oleada 2 zEcstasy (3 agentes, sin cierres, vedas nuevas)

Estado global tras la oleada: código 99,220 %, funciones 18.405/18.432.
Todo revertido salvo notas.

### 14.1 `epCalculate` — el frame NO es padding por declaración (16 órdenes)
Barrido de 16 órdenes de declaración de `sn_ps` (bloques e interiores,
`alpha_rng` en medio como test de sensibilidad) midiendo SOLO `stwu`:
**ninguno mueve el frame de 0x178** (ni siquiera a 0x180 el test que debía
crecer). El slot extra NO es `assign_stack_local` por declaración: apunta a
temp de reload (`alter_reg` r54) / presión por orden de código, no de decls.
(Requiere otra vía: presionar recargas, no reordenar locales.)

### 14.2 `eProject` — schedule sin salida por fuente simple + restricción clipZ-last
Banco mini-TU (`w2proj_eproj`, 1,4 s, reproduce exacto). Medido SIN asm
(quitar barrera): 86,12 % / 23 (confirma r49/r65). 6 órdenes de decl puras:
efecto cero. 9 usos muertos (`+x*0.0f`, `(void)`, `x=x`, dummies): se pliegan
antes de flow, no añaden refs. Acortar vidas: siempre peor (71-81 %).
`1.0/(-eye.z)` vs `eRecip`: `fdivs` +8 B (veda: `fres` es lo correcto).
CON barrier: `const clipX` neutro; 5 perms clip (solo `XYZ`/`YXZ` neutras).
**Nuevo: clipZ-last** — X/Y permutables sin coste, Z última es restricción.
**Mecanismo** (`local-alloc.c:1490-1515`): con `-fschedule-insns2` hay
`fake_birth/death ±2`; vidas disjuntas (107 muere antes que 143 nazca) reciben
regs distintos (f0 vs f12) en vez de compartir f13: haría falta gap>4 aun con
fake, no se consigue moviendo assigns.

### 14.3 `UpdatePlatInfo` — filas 333/334 cerradas en teoría (imposibles en fuente)
Análisis pre-sched2 completo (rtldump `-dS`/`-dg`/`-dl`): bloque 73 post-sched1
= pre-sched2 con `lis6,lfs6,...,barrera,...,lis22,lis86,lfs22,lfs86`; final
idéntico en ambos. `.lreg`: 282(6.0) 2/4 pri~5000, 284(0.22) 2/6 pri~3333,
286(0.86) 2/6 pri~3333; `.greg`: 282→r9, 284→r9, 286→r11 (nuestro r9,r9,r11;
objetivo r11,r9,r11). Para r11,r9,r11 haría falta pre-sched2
`lis6,lis22,lfs6,lis86,lfs22,lfs86`: **imposible con bar1** (fuerza
517<519<522) e imposible sin bar1 (otro horario + cambia el pool `.rodata`,
falla audit). Cero ediciones necesarias; la vía está cerrada con prueba.
