# Censo R36: SDK, STD y Library Code

Instantánea de las funciones con coincidencia inferior al 100% en `build/GOWE69/report.json` (generado el 3 de septiembre de 2026). Se excluye Game Code. La prioridad ordena primero el código con unidad fuente disponible y, dentro de él, por bytes objetivo recuperables.

## Resumen

| Categoría | Funciones no exactas | Bytes objetivo | Bytes alcanzables desde fuente |
|---|---:|---:|---:|
| SDK | 16 | 5008 | 5008 |
| STD | 20 | 28184 | 28184 |
| Library | 254 | 68036 | 62004 |
| **Total** | **290** | **101228** | **95196** |

Disponibilidad: SDK 16/16 con fuente; STD 20/20 con fuente libc; Library 208 con fuente y 46 linked/sin unidad de traducción. No quedan funciones no exactas clasificadas como auto-generated en esta instantánea. Los 6.032 B linked de Library no se consideran alcanzables hasta recuperar o registrar su TU.

## Prioridad por bytes alcanzables

| # | Categoría | Unidad | B | % | Estado | Procedencia | Función |
|---:|---|---|---:|---:|---|---|---|
| 1 | STD | `libc/vfprintf` | 6128 | 83.78068 | libc con fuente | `libc/vfprintf.c` | `_vfprintf_r` |
| 2 | STD | `libc/vfprintf_1` | 5760 | 83.95903 | libc con fuente | `libc/vfprintf_1.c` | `_vfiprintf_r` |
| 3 | Library | `Packages/vp6/1.0.6/source/decode/gc/criticalpath` | 4728 | 91.04907 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/gc/criticalpath.c` | `VP6_DecodeBlock` |
| 4 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathaction` | 3252 | 67.77859 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathaction.cpp` | `PATHI_serviceaction__FP9PATHEVENTP10PATHACTION` |
| 5 | Library | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface` | 2456 | 0.00000 | fuente reconstruible | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface.cpp` | `__static_initialization_and_destruction_0_803906C0` |
| 6 | STD | `libc/k_rem_pio2` | 2184 | 93.46154 | libc con fuente | `libc/k_rem_pio2.c` | `__kernel_rem_pio2` |
| 7 | STD | `libc/kf_rem_pio2` | 2180 | 93.51376 | libc con fuente | `libc/kf_rem_pio2.c` | `__kernel_rem_pio2f` |
| 8 | STD | `libc/e_pow` | 2000 | 0.00000 | libc con fuente | `libc/e_pow.c` | `pow` |
| 9 | Library | `Packages/vp6/1.0.6/source/decode/cmn/dering` | 1808 | 75.03761 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/dering.c` | `DeringBlockStrong_C` |
| 10 | STD | `libc/ef_pow` | 1780 | 0.00000 | libc con fuente | `libc/ef_pow.c` | `powf` |
| 11 | Library | `Packages/vp6/1.0.6/source/decode/cmn/deblock` | 1768 | 95.55430 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/deblock.c` | `DeblockNonFilteredBandNewFilter_C` |
| 12 | STD | `libc/itoa` | 1256 | 97.75478 | libc con fuente | `libc/itoa.c` | `fftoa` |
| 13 | STD | `libc/fseek` | 996 | 98.27309 | libc con fuente | `libc/fseek.c` | `fseek` |
| 14 | Library | `LibSN/steering` | 992 | 99.43549 | fuente reconstruible | `LibSN/steering.c` | `Effect_Update` |
| 15 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathaction` | 980 | 96.61224 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathaction.cpp` | `PATHI_getvalue__FiiP9PATHTRACKP9PATHEVENT` |
| 16 | Library | `Speed/Indep/Libs/snd/9/source/library/mix/sfir` | 936 | 76.41880 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/mix/sfir.c` | `calcFIRCoeffs__FP11SNDFIRSTATEi` |
| 17 | Library | `LibSN/steering` | 924 | 92.35931 | fuente reconstruible | `LibSN/steering.c` | `SimThread_Step` |
| 18 | Library | `LibSN/metrotrk` | 920 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310750` |
| 19 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode` | 896 | 97.54464 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp` | `PATHI_queuenode__FP9PATHTRACK` |
| 20 | STD | `libc/e_rem_pio2` | 868 | 84.61290 | libc con fuente | `libc/e_rem_pio2.c` | `__ieee754_rem_pio2` |
| 21 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick` | 848 | 69.76415 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c` | `iSPCH_ChooseSentence__FPUi` |
| 22 | STD | `libc/ef_rem_pio2` | 848 | 0.00000 | libc con fuente | `libc/ef_rem_pio2.c` | `__ieee754_rem_pio2f` |
| 23 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys` | 824 | 90.85922 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp` | `FILE_init__FPvi` |
| 24 | Library | `LibSN/steering` | 808 | 69.07921 | fuente reconstruible | `LibSN/steering.c` | `Effect_UpdateEffect` |
| 25 | STD | `libc/sf_expm1` | 804 | 0.00000 | libc con fuente | `libc/sf_expm1.c` | `expm1f` |
| 26 | Library | `Packages/vp6/1.0.6/source/decode/gc/criticalpath` | 740 | 93.23243 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/gc/criticalpath.c` | `VP6_PredictFilteredBlock` |
| 27 | Library | `Packages/vp6/1.0.6/source/decode/cmn/borders` | 704 | 90.41477 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/borders.c` | `UpdateUMVBorder` |
| 28 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 700 | 94.15429 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `EXI2_ReadN` |
| 29 | Library | `Packages/vp6/1.0.6/source/decode/cmn/decodemode` | 696 | 79.01150 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/decodemode.c` | `VP6_BuildModeTree` |
| 30 | Library | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver` | 692 | 99.45087 | fuente reconstruible | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver.cpp` | `OpenFile__Q26Realmc8GCDriverRCQ26Realmc6CardIDPCQ26Realmc8FileInfoQ26Realmc12FileOpenModePPQ26Realmc18OpenFileDescriptor` |
| 31 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack` | 680 | 81.10000 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack.cpp` | `PATHI_inittrack__FiPc` |
| 32 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd` | 672 | 98.80952 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd.cpp` | `CheckStatus__Q24Path18PathTrackSndStream` |
| 33 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 664 | 91.68675 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `DBGEXIImm` |
| 34 | Library | `egami/rcmp/dev/source/av/cmn/avplayer` | 656 | 98.78049 | fuente reconstruible | `egami/rcmp/dev/source/av/cmn/avplayer.cpp` | `GetFirstFrame__Q24RCMP9AV_PLAYERUii` |
| 35 | Library | `Packages/vp6/1.0.6/source/decode/cmn/dering` | 644 | 94.96273 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/dering.c` | `DeringBlockWeak_C` |
| 36 | STD | `libc/sf_log` | 640 | 0.00000 | libc con fuente | `libc/sf_log.c` | `logf` |
| 37 | STD | `libc/e_exp` | 636 | 0.00000 | libc con fuente | `libc/e_exp.c` | `exp` |
| 38 | Library | `egami/rcmp/dev/source/decoder/cmn/madidct` | 632 | 52.15823 | fuente reconstruible | `egami/rcmp/dev/source/decoder/cmn/madidct.cpp` | `IdctColumn` |
| 39 | Library | `LibSN/sndvd` | 628 | 98.43949 | fuente reconstruible | `LibSN/sndvd.c` | `DSIHandler` |
| 40 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 608 | 91.09210 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `DBWrite` |
| 41 | Library | `LibSN/metrotrk` | 592 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030EC5C` |
| 42 | Library | `LibSN/steering` | 588 | 94.89796 | fuente reconstruible | `LibSN/steering.c` | `HandleTriggers` |
| 43 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 580 | 94.06896 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `AmcEXIImm` |
| 44 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathinit` | 580 | 84.00000 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathinit.cpp` | `PATH_addmapfile__FPc` |
| 45 | Library | `Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_utilities` | 576 | 87.25694 | fuente reconstruible | `Packages/realmemcard/3.04.01-layer2/source/lib/cmn/memcard_utilities.cpp` | `Wildcard__11RealmcUtilsPcT1` |
| 46 | STD | `libc/sf_exp` | 556 | 0.00000 | libc con fuente | `libc/sf_exp.c` | `expf` |
| 47 | Library | `Packages/vp6/1.0.6/source/decode/cmn/simpledeblocker` | 540 | 98.53333 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/simpledeblocker.c` | `SimpleDeblockFrame` |
| 48 | Library | `LibSN/metrotrk` | 532 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030FBF0` |
| 49 | Library | `Packages/vp6/1.0.6/source/decode/cmn/FrameIni` | 532 | 88.80451 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/FrameIni.c` | `VP6_InitFrameDetails` |
| 50 | STD | `libc/kf_tan` | 532 | 97.70676 | libc con fuente | `libc/kf_tan.c` | `__kernel_tanf` |
| 51 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick` | 524 | 98.32061 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c` | `iSPCH_MakeSampleRequests__FP8VoxEventP11VoxSentenceP9EventSpec` |
| 52 | Library | `egami/rcmp/dev/source/decoder/cmn/madidct` | 516 | 50.40310 | fuente reconstruible | `egami/rcmp/dev/source/decoder/cmn/madidct.cpp` | `IdctRow` |
| 53 | Library | `LibSN/metrotrk` | 512 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310AF0` |
| 54 | Library | `LibSN/metrotrk` | 512 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `lbl_8030FFDC` |
| 55 | Library | `Packages/vp6/1.0.6/source/decode/cmn/deblock` | 504 | 95.95238 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/deblock.c` | `DeblockPlane` |
| 56 | Library | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver` | 464 | 95.88793 | fuente reconstruible | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver.cpp` | `ReadFile__Q26Realmc8GCDriverPQ26Realmc18OpenFileDescriptorPviPi` |
| 57 | Library | `Packages/vp6/1.0.6/source/decode/cmn/decodemv` | 464 | 81.96551 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/decodemv.c` | `VP6_ConfigureMvEntropyDecoder` |
| 58 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd` | 456 | 96.35965 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd.cpp` | `GetPathStatus__Q24Path12PathTrackSndP10PATHSTATUS` |
| 59 | Library | `LibSN/metrotrk` | 448 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F870` |
| 60 | Library | `LibSN/metrotrk` | 448 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030FA30` |
| 61 | Library | `LibSN/metrotrk` | 444 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311064` |
| 62 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/file/gc/dvd_device` | 440 | 92.54546 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/file/gc/dvd_device.cpp` | `StartNonAlignedAyncRead__FP11DVDFileInfoPvll` |
| 63 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 432 | 89.68519 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `EXI2_WriteN` |
| 64 | Library | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver` | 424 | 94.96227 | fuente reconstruible | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver.cpp` | `WriteHeaderData__Q26Realmc8GCDriverPQ26Realmc16GcFileDescriptor` |
| 65 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack` | 424 | 99.92453 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack.cpp` | `PATH_createstreamimp__Fiif` |
| 66 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick` | 412 | 98.00000 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c` | `iSPCH_ChooseSamples__FP16SentencePickInfoP11VoxSentenceP14PhrasePickInfoP9VoxPhrasePUi` |
| 67 | Library | `LibSN/metrotrk` | 408 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311220` |
| 68 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 408 | 74.41177 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `AmcEXISync` |
| 69 | Library | `LibSN/metrotrk` | 404 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803101DC` |
| 70 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd` | 404 | 99.85149 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd.cpp` | `CreateStreamTrack__Q24Path9PathToSndPPQ24Path10IPathTrackifi` |
| 71 | Library | `Speed/Indep/Libs/snd/9/source/library/gc/snddrv` | 400 | 99.56000 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/gc/snddrv.c` | `SNDDRV_audiocallback__FPvT0` |
| 72 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank` | 396 | 88.84849 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank.cpp` | `PATHI_loadbankdata__FP9PATHTRACKii` |
| 73 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/input/gc/gc_interface` | 396 | 87.55556 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/input/gc/gc_interface.cpp` | `EnumerateDevices__Q29RealInput11GcInterface` |
| 74 | Library | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis` | 392 | 98.95918 | fuente reconstruible | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis.cpp` | `Subscribe__Q24Csis6SystemPv` |
| 75 | STD | `libc/vfprintf` | 392 | 96.32653 | libc con fuente | `libc/vfprintf.c` | `add_separators` |
| 76 | Library | `LibSN/vmbase` | 380 | 90.47369 | fuente reconstruible | `LibSN/vmbase.c` | `__VMBASESetupExceptionHandlers` |
| 77 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchevnt` | 352 | 74.73864 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchevnt.c` | `iSPCH_ChooseEvent__FUi` |
| 78 | Library | `LibSN/metrotrk` | 340 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E40` |
| 79 | Library | `Packages/vp6/1.0.6/source/decode/cmn/decodemode` | 336 | 92.38095 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/decodemode.c` | `VP6_DecodeModeProbs` |
| 80 | Library | `Packages/vp6/1.0.6/source/decode/gc/criticalpath` | 336 | 97.71429 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/gc/criticalpath.c` | `VP6_FindNearestandNextNearest` |
| 81 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode` | 336 | 98.51190 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp` | `PATHI_calcwaitbeat__FiiiP12PATHBEATINFO` |
| 82 | Library | `LibSN/steering` | 324 | 90.74074 | fuente reconstruible | `LibSN/steering.c` | `SimThread_Init` |
| 83 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys` | 316 | 83.84810 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp` | `FILE_nametodevice__FPCc` |
| 84 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode` | 308 | 93.05195 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp` | `PATHI_nextnode__Fiii` |
| 85 | Library | `LibSN/metrotrk` | 300 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803105EC` |
| 86 | Library | `LibSN/metrotrk` | 296 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F31C` |
| 87 | Library | `LibSN/metrotrk` | 296 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803104C4` |
| 88 | Library | `Speed/Indep/Libs/snd/9/source/library/cmn/sst` | 280 | 98.71429 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/cmn/sst.c` | `SNDSTRMI_parsedata__FP16SNDSTREAMCHANNELP14STREAMCHUNKHDR` |
| 89 | Library | `LibSN/steering` | 276 | 29.23188 | fuente reconstruible | `LibSN/steering.c` | `Effect_Init` |
| 90 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd` | 276 | 94.05797 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd.cpp` | `Play__Q24Path16PathTrackSndBankiUiiiUi` |
| 91 | Library | `LibSN/metrotrk` | 268 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311AD8` |
| 92 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr` | 268 | 97.01492 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr.cpp` | `TIMER_init__Fi` |
| 93 | STD | `libc/sf_log10` | 268 | 93.28359 | libc con fuente | `libc/sf_log10.c` | `log10f` |
| 94 | Library | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver` | 264 | 93.03030 | fuente reconstruible | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver.cpp` | `RecordIplDataChecksum__Q26Realmc8GCDriverPQ26Realmc16GcFileDescriptor` |
| 95 | Library | `Packages/vp6/1.0.6/source/decode/cmn/simpledeblocker` | 264 | 88.80303 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/simpledeblocker.c` | `FilterVert_Simple_C` |
| 96 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 264 | 99.18182 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `EXI2_Poll` |
| 97 | Library | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis` | 264 | 95.72727 | fuente reconstruible | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis.cpp` | `Init__Q24Csis6System` |
| 98 | Library | `LibSN/steering` | 260 | 95.76923 | fuente reconstruible | `LibSN/steering.c` | `CookValues` |
| 99 | Library | `Speed/Indep/Libs/snd/9/source/library/mix/sfsplit` | 256 | 99.53125 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/mix/sfsplit.c` | `SFILTER_splitter__FPviT0T0i` |
| 100 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys` | 252 | 84.60317 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp` | `AddToQueue__13FILEOPERATION` |
| 101 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys` | 252 | 84.44444 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp` | `Find__18FileOperationQueueib` |
| 102 | Library | `Speed/Indep/Libs/snd/9/source/library/cmn/sstfxlev` | 252 | 98.80952 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/cmn/sstfxlev.c` | `SNDSTRM_fxlevel` |
| 103 | Library | `Packages/vp6/1.0.6/source/decode/cmn/postproc` | 248 | 69.38710 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/postproc.c` | `ChangePostProcConfiguration` |
| 104 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick` | 248 | 32.90322 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c` | `iSPCH_SentenceIsContextMatch__FP8VoxEventP11VoxSentencePUi` |
| 105 | Library | `LibSN/metrotrk` | 240 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `DBcallback` |
| 106 | Library | `LibSN/metrotrk` | 240 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F144` |
| 107 | Library | `LibSN/metrotrk` | 236 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030EEC0` |
| 108 | Library | `Packages/vp6/1.0.6/source/decode/cmn/postproc` | 232 | 96.29311 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/postproc.c` | `InitPostProcessing` |
| 109 | Library | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis` | 232 | 99.01724 | fuente reconstruible | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis.cpp` | `Unsubscribe__Q24Csis6SystemPv` |
| 110 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys` | 232 | 80.20689 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp` | `SetName__13FILEOPERATIONPCc` |
| 111 | Library | `LibSN/metrotrk` | 228 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311870` |
| 112 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/systask` | 224 | 87.94643 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/systask.cpp` | `SYNCTASK_add__FPFPvi_viiPv` |
| 113 | Library | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver` | 220 | 92.54546 | fuente reconstruible | `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_driver.cpp` | `VerifyIplDataChecksum__Q26Realmc8GCDriverPQ26Realmc16GcFileDescriptor` |
| 114 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 220 | 94.03636 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `DBGRead` |
| 115 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 220 | 94.03636 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `DBGWrite` |
| 116 | Library | `LibSN/metrotrk` | 216 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F614` |
| 117 | Library | `LibSN/steering` | 216 | 95.90741 | fuente reconstruible | `LibSN/steering.c` | `VDevice_RecalcGammaTable` |
| 118 | Library | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter` | 216 | 98.61111 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter.c` | `FilteringVert_12_C` |
| 119 | Library | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter` | 216 | 98.61111 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter.c` | `FilteringVert_8_C` |
| 120 | Library | `LibSN/steering` | 204 | 99.50980 | fuente reconstruible | `LibSN/steering.c` | `LGDownloadForceEffect` |
| 121 | Library | `LibSN/metrotrk` | 196 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F4C8` |
| 122 | Library | `LibSN/metrotrk` | 196 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310CF0` |
| 123 | Library | `LibSN/steering` | 196 | 93.26530 | fuente reconstruible | `LibSN/steering.c` | `Effect_PerformEnvelope` |
| 124 | Library | `LibSN/steering` | 188 | 98.51063 | fuente reconstruible | `LibSN/steering.c` | `VDevice_DownloadEffect` |
| 125 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 188 | 94.80851 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `EXI2_Init` |
| 126 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack` | 188 | 69.48937 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathtrack.cpp` | `PATHI_getmastertrack__Fv` |
| 127 | Library | `Speed/Indep/Libs/snd/9/source/library/cmn/srender` | 188 | 69.46809 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/cmn/srender.c` | `SNDI_validrendermode__FPiP15SNDIPATCHHEADER` |
| 128 | Library | `LibSN/metrotrk` | 184 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803103FC` |
| 129 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathevent` | 184 | 70.97826 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathevent.cpp` | `PATHI_serviceevent__Fi` |
| 130 | Library | `LibSN/steering` | 180 | 99.44444 | fuente reconstruible | `LibSN/steering.c` | `Effect_PolarToRect` |
| 131 | Library | `LibSN/metrotrk` | 176 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F6EC` |
| 132 | Library | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter` | 176 | 98.75000 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter.c` | `FilteringHoriz_12_C` |
| 133 | Library | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter` | 176 | 98.75000 | fuente reconstruible | `Packages/vp6/1.0.6/source/decode/cmn/loopfilter.c` | `FilteringHoriz_8_C` |
| 134 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 172 | 89.86047 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `DBGReadMailbox` |
| 135 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 172 | 89.86047 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `DBGReadStatus` |
| 136 | Library | `LibSN/vm` | 168 | 96.07143 | fuente reconstruible | `LibSN/vm.c` | `__VMAllocVirtualToARAMLUT` |
| 137 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchrule` | 168 | 86.50000 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchrule.c` | `iSPCH_GetSentenceRuleSettings__FP8VoxEventiPUiT2` |
| 138 | Library | `LibSN/metrotrk` | 164 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803117C0` |
| 139 | Library | `LibSN/metrotrk` | 160 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F27C` |
| 140 | Library | `LibSN/steering` | 160 | 95.50000 | fuente reconstruible | `LibSN/steering.c` | `Effect_UpdateSpring` |
| 141 | Library | `LibSN/metrotrk` | 156 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `cmdRecvMem` |
| 142 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchbank` | 156 | 98.33334 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchbank.c` | `iSPCHBank_GetSampleTimeInQueue__FP10VOXBANKHDRi` |
| 143 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick` | 152 | 90.52631 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c` | `iSPCH_SentenceLength__FP11VoxSentenceP16SentencePickInfo` |
| 144 | Library | `LibSN/metrotrk` | 148 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311954` |
| 145 | STD | `libc/memset` | 148 | 82.97298 | libc con fuente | `libc/memset.c` | `memset` |
| 146 | Library | `Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_root1x` | 144 | 89.02778 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/cmn/SNDI_root1x.c` | `SNDI_rootof1plusx__Ff` |
| 147 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd` | 140 | 88.57143 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd.cpp` | `__Q24Path12PathTrackSnd` |
| 148 | Library | `LibSN/metrotrk` | 136 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310FDC` |
| 149 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchbank` | 136 | 95.29412 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchbank.c` | `iSPCHBank_AddToQueue__FP10VOXBANKHDRi` |
| 150 | Library | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp` | 136 | 91.76471 | fuente reconstruible | `Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp.c` | `iSPCH_GetSampleSizeData__FP10VOXBANKHDRiPUiT2` |
| 151 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 128 | 98.71875 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `AmcEXISelect` |
| 152 | Library | `Speed/Indep/Libs/snd/9/source/library/cmn/sgetdata` | 128 | 93.75000 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/cmn/sgetdata.c` | `SNDI_getb__FPvi` |
| 153 | Library | `LibSN/sndvd` | 124 | 91.45161 | fuente reconstruible | `LibSN/sndvd.c` | `SNDVDEmuInitDSIHandler` |
| 154 | Library | `LibSN/metrotrk` | 120 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030EA80` |
| 155 | Library | `LibSN/metrotrk` | 120 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311A50` |
| 156 | Library | `Speed/Indep/Libs/snd/9/source/library/cmn/saemstimupdt` | 120 | 68.00000 | fuente reconstruible | `Speed/Indep/Libs/snd/9/source/library/cmn/saemstimupdt.c` | `SNDAEMSI_timerupdate__FPv` |
| 157 | Library | `LibSN/steering` | 116 | 77.06896 | fuente reconstruible | `LibSN/steering.c` | `VDevice_GetFreeEffect` |
| 158 | Library | `LibSN/metrotrk` | 112 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803115AC` |
| 159 | Library | `LibSN/metrotrk` | 112 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803116E8` |
| 160 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 108 | 85.18519 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `AmcEXISetExiCallback` |
| 161 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/exit` | 108 | 68.33334 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/system/cmn/exit.cpp` | `SYSTEM_addexit__FPFv_v` |
| 162 | Library | `LibSN/metrotrk` | 104 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8031161C` |
| 163 | Library | `LibSN/metrotrk` | 104 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311758` |
| 164 | STD | `libc/strstr` | 104 | 92.11539 | libc con fuente | `libc/strstr.c` | `strstr` |
| 165 | STD | `libc/wcstring` | 104 | 99.61539 | libc con fuente | `libc/wcstring.c` | `wcscat` |
| 166 | Library | `LibSN/metrotrk` | 100 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `EnableMetroTRKInterrupts` |
| 167 | Library | `LibSN/metrotrk` | 100 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F7C8` |
| 168 | Library | `LibSN/metrotrk` | 100 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311684` |
| 169 | Library | `LibSN/steering` | 96 | 98.33334 | fuente reconstruible | `LibSN/steering.c` | `Effect_UpdateDamper` |
| 170 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathserv` | 96 | 94.16666 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathserv.cpp` | `PATHI_switchproject__Fii` |
| 171 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathevent` | 92 | 93.04348 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathevent.cpp` | `PATHI_clearalleventfilters__Fv` |
| 172 | Library | `LibSN/metrotrk` | 88 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F58C` |
| 173 | Library | `LibSN/metrotrk` | 84 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803113C0` |
| 174 | Library | `LibSN/vmbase` | 84 | 98.80952 | fuente reconstruible | `LibSN/vmbase.c` | `__VMBASESetupVMRegisters` |
| 175 | Library | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis` | 84 | 98.23810 | fuente reconstruible | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis.cpp` | `Alloc__Q24Csis24ICoreToIAllocatorAdaptorUiPCcUi` |
| 176 | Library | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis` | 84 | 98.23810 | fuente reconstruible | `Speed/Indep/Libs/csis/dev/source/library/cmn/csis.cpp` | `Alloc__Q24Csis24ICoreToIAllocatorAdaptorUiPCcUiUiUi` |
| 177 | Library | `LibSN/metrotrk` | 80 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F058` |
| 178 | Library | `LibSN/metrotrk` | 80 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8031155C` |
| 179 | Library | `LibSN/syscalls` | 80 | 79.75000 | fuente reconstruible | `LibSN/syscalls.c` | `_write` |
| 180 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 80 | 99.25000 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `AmcDebugIntHandler` |
| 181 | Library | `LibSN/metrotrk` | 72 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F234` |
| 182 | Library | `LibSN/metrotrk` | 72 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310F94` |
| 183 | Library | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys` | 72 | 83.88889 | fuente reconstruible | `Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp` | `FILE_overhead__Fv` |
| 184 | Library | `LibSN/metrotrk` | 68 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F82C` |
| 185 | Library | `LibSN/metrotrk` | 64 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030EFD8` |
| 186 | Library | `LibSN/metrotrk` | 64 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F018` |
| 187 | Library | `LibSN/metrotrk` | 64 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8031147C` |
| 188 | SDK | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver` | 64 | 99.37500 | fuente reconstruible | `Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c` | `DBGHandler` |
| 189 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathreal` | 64 | 52.81250 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathreal.cpp` | `PATH_setallocator__FPQ32EA9Allocator10IAllocatorRCQ22EA12TagValuePair` |
| 190 | Library | `LibSN/metrotrk` | 60 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803114F0` |
| 191 | Library | `LibSN/metrotrk` | 56 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310718` |
| 192 | Library | `LibSN/metrotrk` | 52 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803114BC` |
| 193 | Library | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd` | 52 | 0.00000 | fuente reconstruible | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathsnd.cpp` | `_._Q24Path10IPathToSnd` |
| 194 | Library | `LibSN/metrotrk` | 48 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8031152C` |
| 195 | Library | `LibSN/metrotrk` | 44 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F79C` |
| 196 | Library | `LibSN/metrotrk` | 44 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DB4` |
| 197 | Library | `LibSN/metrotrk` | 40 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030EFAC` |
| 198 | Library | `LibSN/metrotrk` | 40 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F474` |
| 199 | Library | `LibSN/metrotrk` | 40 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311440` |
| 200 | Library | `Packages/eathread/1.1.0/source/eathread_thread` | 40 | 82.40000 | fuente reconstruible | `Packages/eathread/1.1.0/source/eathread_thread.cpp` | `__static_initialization_and_destruction_0` |
| 201 | Library | `LibSN/metrotrk` | 28 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `DSIentry` |
| 202 | Library | `LibSN/metrotrk` | 28 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `cmdReset` |
| 203 | Library | `LibSN/metrotrk` | 28 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311A28` |
| 204 | Library | `LibSN/metrotrk` | 24 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `ISIentry` |
| 205 | Library | `LibSN/metrotrk` | 24 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F49C` |
| 206 | Library | `LibSN/metrotrk` | 24 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311414` |
| 207 | Library | `LibSN/metrotrk` | 24 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803119E8` |
| 208 | Library | `LibSN/metrotrk` | 24 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311A10` |
| 209 | Library | `LibSN/metrotrk` | 20 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `SNDebugBoot` |
| 210 | Library | `LibSN/metrotrk` | 20 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F460` |
| 211 | Library | `LibSN/metrotrk` | 20 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030F4B4` |
| 212 | Library | `LibSN/metrotrk` | 20 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8031142C` |
| 213 | Library | `LibSN/metrotrk` | 20 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311468` |
| 214 | Library | `LibSN/metrotrk` | 16 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030EAF8` |
| 215 | Library | `LibSN/metrotrk` | 16 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803104B4` |
| 216 | Library | `LibSN/metrotrk` | 16 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80311A00` |
| 217 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_8030EFD4` |
| 218 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DE0` |
| 219 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DE4` |
| 220 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DE8` |
| 221 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DEC` |
| 222 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DF0` |
| 223 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DF4` |
| 224 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DF8` |
| 225 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310DFC` |
| 226 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E00` |
| 227 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E04` |
| 228 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E08` |
| 229 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E0C` |
| 230 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E10` |
| 231 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E14` |
| 232 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E18` |
| 233 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E1C` |
| 234 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E20` |
| 235 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E24` |
| 236 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E28` |
| 237 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E2C` |
| 238 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E30` |
| 239 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E34` |
| 240 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E38` |
| 241 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_80310E3C` |
| 242 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803113B8` |
| 243 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `fn_803113BC` |
| 244 | Library | `LibSN/metrotrk` | 4 | 0.00000 | fuente reconstruible | `LibSN/metrotrk.c` | `pad_01_8030EA7C_text` |
| 245 | Library | `Speed/Indep/Libs/snd/9/source/library/mix/sfrsf` | 728 | 0.00000 | linked/sin TU | `-` | `SFILTER_rsf__FPviT0T0i` |
| 246 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 440 | 0.00000 | linked/sin TU | `-` | `Decode__Q23Snd12CEAXABLKDecfPPfi` |
| 247 | Library | `ppc2D2` | 388 | 0.00000 | linked/sin TU | `-` | `CompletePCreadAsync` |
| 248 | Library | `Speed/Indep/Libs/snd/9/source/library/cmn/slib` | 360 | 0.00000 | linked/sin TU | `-` | `iSNDcalcvol__Fi` |
| 249 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 336 | 0.00000 | linked/sin TU | `-` | `decodexac__3SndPQ23Snd10MXAPACKETF` |
| 250 | Library | `ppc2D2` | 316 | 0.00000 | linked/sin TU | `-` | `EXI2TCHandler` |
| 251 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 284 | 0.00000 | linked/sin TU | `-` | `process_raw_block__3SndPQ23Snd10MXAPACKETF` |
| 252 | Library | `ppc2D2` | 244 | 0.00000 | linked/sin TU | `-` | `__va_arg` |
| 253 | Library | `ppc2D2` | 216 | 0.00000 | linked/sin TU | `-` | `PCrwSyncFSACK` |
| 254 | Library | `Speed/Indep/Libs/snd/9/source/library/gc/ssdfx` | 196 | 0.00000 | linked/sin TU | `-` | `SNDPLATFORM_setfxlevel__Fii` |
| 255 | Library | `ppc2D2` | 168 | 0.00000 | linked/sin TU | `-` | `PCrwAsyncFSACK` |
| 256 | Library | `ppc2D2` | 164 | 0.00000 | linked/sin TU | `-` | `PCreadAsyncInit` |
| 257 | Library | `ppc2D2` | 160 | 0.00000 | linked/sin TU | `-` | `ReadSyncNext` |
| 258 | Library | `atexit` | 152 | 0.00000 | linked/sin TU | `-` | `atexit` |
| 259 | Library | `crt2D1` | 144 | 0.00000 | linked/sin TU | `-` | `__do_global_ctors` |
| 260 | Library | `ppc2D2` | 140 | 0.00000 | linked/sin TU | `-` | `DoFSReadHeader` |
| 261 | Library | `ppc2D2` | 140 | 0.00000 | linked/sin TU | `-` | `PCreadAsyncNext` |
| 262 | Library | `ppc2D2` | 116 | 0.00000 | linked/sin TU | `-` | `PCrwAsyncNextPh` |
| 263 | Library | `Speed/Indep/Libs/snd/9/source/library/cmn/sdownmix` | 112 | 0.00000 | linked/sin TU | `-` | `SetDefaultAzimuths__Q23Snd4UtilP7CHANPUB` |
| 264 | Library | `ppc2D2` | 112 | 0.00000 | linked/sin TU | `-` | `SNInitEXI2TCHandler` |
| 265 | Library | `Speed/Indep/Libs/snd/9/source/library/mix/sfrsf` | 92 | 0.00000 | linked/sin TU | `-` | `SFILTER_rsfinit__FP8RSFSTATEii` |
| 266 | Library | `ppc2D2` | 92 | 0.00000 | linked/sin TU | `-` | `InitReadCounts` |
| 267 | Library | `ppc2D2` | 84 | 0.00000 | linked/sin TU | `-` | `CompleteAsync` |
| 268 | Library | `Speed/Indep/Libs/snd/9/source/library/gc/ssdfx` | 80 | 0.00000 | linked/sin TU | `-` | `SetCustomFx__Q23Snd3HalPvT1` |
| 269 | Library | `pro2D4` | 76 | 0.00000 | linked/sin TU | `-` | `__restore_gpr` |
| 270 | Library | `pro2D4` | 76 | 0.00000 | linked/sin TU | `-` | `__save_gpr` |
| 271 | Library | `ppc2D2` | 72 | 0.00000 | linked/sin TU | `-` | `__cvt_fp2unsigned` |
| 272 | Library | `Speed/Indep/Libs/snd/9/source/library/gc/ssdfx` | 68 | 0.00000 | linked/sin TU | `-` | `Reset__Q23Snd3HalPv` |
| 273 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 60 | 0.00000 | linked/sin TU | `-` | `Feed__Q23Snd12CEAXABLKDecfPvii` |
| 274 | Library | `sn_malloc` | 60 | 0.00000 | linked/sin TU | `-` | `free` |
| 275 | Library | `sn_malloc` | 60 | 0.00000 | linked/sin TU | `-` | `malloc` |
| 276 | Library | `crt2D1` | 56 | 0.00000 | linked/sin TU | `-` | `__main` |
| 277 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 48 | 0.00000 | linked/sin TU | `-` | `GetState__Q23Snd12CEAXABLKDecf` |
| 278 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 44 | 0.00000 | linked/sin TU | `-` | `__dl__Q23Snd12CEAXABLKDecfPv` |
| 279 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 44 | 0.00000 | linked/sin TU | `-` | `__nw__Q23Snd12CEAXABLKDecfUi` |
| 280 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 40 | 0.00000 | linked/sin TU | `-` | `__Q23Snd12CEAXABLKDecf` |
| 281 | Library | `Speed/Indep/Libs/snd/9/source/library/extern/coda/cmn/eaxadecf` | 20 | 0.00000 | linked/sin TU | `-` | `SetState__Q23Snd12CEAXABLKDecfPQ23Snd8XAFSTATE` |
| 282 | Library | `Speed/Indep/Libs/snd/9/source/library/mix/sfrsf` | 8 | 0.00000 | linked/sin TU | `-` | `SFILTER_rsfsetpitch__FP8RSFSTATEi` |
| 283 | Library | `ppc2D2` | 8 | 0.00000 | linked/sin TU | `-` | `PCwriteAsyncInit` |
| 284 | Library | `ppc2D2` | 4 | 0.00000 | linked/sin TU | `-` | `__div2i` |
| 285 | Library | `ppc2D2` | 4 | 0.00000 | linked/sin TU | `-` | `__div2u` |
| 286 | Library | `ppc2D2` | 4 | 0.00000 | linked/sin TU | `-` | `__mod2i` |
| 287 | Library | `ppc2D2` | 4 | 0.00000 | linked/sin TU | `-` | `__mod2u` |
| 288 | Library | `ppc2D2` | 4 | 0.00000 | linked/sin TU | `-` | `__shl2i` |
| 289 | Library | `ppc2D2` | 4 | 0.00000 | linked/sin TU | `-` | `__shr2i` |
| 290 | Library | `ppc2D2` | 4 | 0.00000 | linked/sin TU | `-` | `__shr2u` |

## Lectura operativa

- **Alcanzable** significa que `objdiff.json` o el reporte señalan un `source_path` existente; no garantiza que la función esté agotada ni que el compilador correcto produzca un A/B útil.
- **linked/sin TU** identifica símbolos presentes en el objeto enlazado pero sin fuente configurada. Su prioridad efectiva es cero hasta recuperar o dar de alta la unidad; no deben atacarse cambiando SourceLists.
- **STD** se mantiene separado aunque sus 20 funciones tengan fuente: requieren reproducir exactamente la variante de libc y sus opciones, no estilo de Game Code.
- El porcentaje es el `fuzzy_match_percent` por función del reporte; los bytes son el tamaño del objetivo. Para cierres se debe volver a medir el objeto recién compilado con `libdiff.py` u objdiff.

## Reproducción

- Entrada: `build/GOWE69/report.json`, SHA-256 `9d42f190c0fc1017218096c2d6d990c6e5fb1b37af134bd20d03428a7912448b`.
- Generador: `python scratchpad/r36_gen_census.py` (emite esta tabla por stdout).
- Control: la tabla debe contener 290 filas; las sumas de resumen se calculan desde las mismas filas, no se introducen a mano.

## Sonda de cierre R36

Se eligió `SNDDRV_audiocallback__FPvT0` (`snddrv.c`), 400 B y 99,56000%. El residuo son dos pares independientes de `lwz r3/r4` invertidos en los `MemCpy` de los buses 2 y 3; no hay diferencias de opcode ni tamaño.

Se probaron tres formas acotadas: alias previo de la fuente, asignación mediante expresión coma y barrera de memoria después del alias. Las dos primeras fueron neutras; la barrera empeoró el mapa de registros de la función. Las tres se revirtieron. El estado final conserva 400 B, 99,56000%, y el objeto completo queda en 11.520/11.920 B (96,6443%) con 49 funciones exactas.
