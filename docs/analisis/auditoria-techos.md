# Auditoría de techos: cuáles son falsos

`scripts/auditecho.py --todas`, 1-sep-2026.

Un techo del playbook dice *«no cede a estas formas»* y se acaba leyendo como
*«no cede»*. Ya salió caro: **`Moment::React(Plane)`** llevaba anotado «8 variantes
a mano más ~1.650 del permutador, cero mejora» y **se cerró entera, 5.668 B**, con
cuatro cambios de fuente. Y **un `= false` de más disolvió 58 de 60 diffs** que
parecían rotación pura del asignador.

El discriminante es barato: el **multiconjunto de mnemónicos**.

* **Mismo multiconjunto** → las mismas instrucciones en otro orden o con otros
  registros. Asignador o planificador: el techo es probablemente real y leer la
  fuente no dará nada.
* **Delta de mnemónicos** (`stfs +1`, `mr −9`, `bl −159`…) → **falta o sobra
  código**, y el opcode dice cuál. **Ese techo es falso.**

## El resultado

    129 funciones con delta   ->  240.112 B donde FALTA CODIGO
    149 con mismo multiconjunto  ->  asignador/planificador

O sea que **de las 278 funciones near-miss del juego, casi la mitad no son un
problema de registros**: les falta o les sobra una sentencia, y el opcode la
nombra.

## Techos documentados que la auditoría desmiente

| función | B | delta | lectura |
|---|---:|---|---|
| `Smackable::Smackable` | 3.120 | `lwz−2, mr+2` | **32 variantes gastadas** y es la firma exacta de «un `mr` del objetivo contra un `lwz` nuestro = miembro que hay que LIGAR a un local», descubierta después y nunca probada ahí |
| `UpdateOccludedPosition` | 4.368 | `mr−1` | 465 variantes del permutador; **nos sobra un `mr`** → `regmove`, borrar el local |
| `HolePunchAvoidables` | 2.980 | `fsel−2, fsubs−1` | 460 variantes; sobran dos `fsel` |
| `ProcessPadsForPackage` | 3.080 | `lis−3, li−3, addi−2` | sobran seis materializaciones: es el izado al preencabezado ya diagnosticado |
| `RenderFlaresOnCar` | 2.908 | `lis−2, lwz+1, stw+1` | 14 variantes |
| `CookieTrailCurvature` | 1.284 | `fmr+1` | 164 variantes, y falta **un** `fmr` |
| `CullParts` | 836 | `mr+1` | |
| `DrawAmbientShadow` | 2.548 | `lis−1` | |

## Lo que NADIE había mirado, y es lo gordo

| función | unidad | B | delta |
|---|---|---:|---|
| `ResolveCarBanks` | zEAXSound2 | 7.508 | `bgt+1, ble−1, b−1` — polaridad/estructura de rama |
| `_bOutput` | zBWare | 5.180 | `lwz−9, stw−3` — nueve cargas de más: locales sin ligar |
| `NotificationMessage__13UIQRCarSelect` | zFeOverlay | 5.096 | `mr+2, bne+1, **bl+1**` — **falta una llamada** |
| `__static_init` | zPhysicsBehaviors | 4.464 | **`bl−159, addi−153, mr−96`** |
| `FindNodes__C5WGrid` | zWorld2 | 3.456 | `b+4, cmpw+4` — faltan cuatro comparaciones |
| `__static_init` | zCamera | 3.604 | `stfs+141, lis+116, lfs+63` |

**El static-init de zPhysicsBehaviors emite 159 `bl` y 153 `addi` DE MÁS.** Eso no
es un near-miss: son ~4,4 kB de inicialización mal escrita, y estaba clasificado
como «49,5%» sin que nadie mirase qué le pasaba.

## Reparto por unidad

```
== zAI             16 near-miss:  12 con DELTA ( 15928 B, falta codigo),   4 mismo multiconjunto
        2908 B  97.0261%  UpdateAllAvoidables__11AIAvoidablef          blt-4, stw-4, mr+3, bne-2
        2448 B  94.1062%  Update__13AIPerpVehiclef                     addi-2, lis-2, mr-1
        1960 B  93.5204%  GetPotentialSpeed__C12AIActionRaceffb        lis+3, lfs+1, fmr+1
        1776 B  91.3739%  UpdatePatrols__12AICopManager                b+2
        1684 B  98.4323%  AssignClosestOffsets__9AIPursuitRQ33UTL3Stdt beq+1, mr+1, bne-1, blt-1
        1236 B  97.0065%  UpdateRoadBlocks__12AICopManager             mr-1
== zAnim            6 near-miss:   4 con DELTA (  1972 B, falta codigo),   2 mismo multiconjunto
         660 B  97.9455%  AdvanceAnimTime__9CAnimCtrlf                 psq_l+1, psq_st+1
         588 B  46.8639%  __static_initialization_and_destruction_0    stfs+3, li-1, stw-1, lfs+1
         496 B  96.6532%  Load__11CAnimPlayerUiib                      li-1, stw-1, beq+1, mr+1
         228 B  95.7895%  LoaderAnimSceneData__FP6bChunk               ble+1, cmplw+1
== zAttribSys       0 near-miss:   0 con DELTA (     0 B, falta codigo),   0 mismo multiconjunto
== zBWare           6 near-miss:   2 con DELTA (  5432 B, falta codigo),   4 mismo multiconjunto
        5180 B  92.5313%  _bOutput__FP11bOutputInfoPCcP13__va_list_tag lwz-9, stw-3, fmr+2, lfd-1
         252 B  97.7778%  bStrNICmp__FPCcT0i                           mr+1
== zCamera         23 near-miss:  15 con DELTA ( 15856 B, falta codigo),   8 mismo multiconjunto
        3604 B  26.4029%  __static_initialization_and_destruction_0    stfs+141, lis+116, lfs+63, addi+45
        2060 B  75.2233%  Update__21DebugWorldCameraMoverf             lfs+24, stfs+23, fmuls+12, fadds+9
        2036 B  65.6365%  AverageAir__FP8ISimablefPfT2                 lwz+37, stw+34, addi+6, beq-6
        1264 B  82.1392%  UpdateCameraMovers__Ff                       lfs+3, stfs+3, mr-2, fmuls+1
        1156 B  59.5640%  __Q33UTL11Collectionst8_Storage2ZPQ28CameraA lwz+20, mr+13, add+9, mtlr+7
        1100 B  95.9491%  IsUnderVehicle__16CubicCameraMover           mr-4, addi+1
== zComms         (sin objetos o objdiff fallo)
== zDebug           0 near-miss:   0 con DELTA (     0 B, falta codigo),   0 mismo multiconjunto
== zDynamics        3 near-miss:   2 con DELTA (  2832 B, falta codigo),   1 mismo multiconjunto
        1896 B  97.7110%  SphereVsBox__Q38Dynamics9Collision8GeometryP addi+2, mr-2
         936 B  99.1239%  React__Q38Dynamics12Articulation10Constraint mtlr-1, blrl-1
== zEAXSound       41 near-miss:  27 con DELTA ( 15456 B, falta codigo),  14 mismo multiconjunto
        1488 B  98.1452%  GenerateSinglePlayerMix__15SFXCTL_3DObjPos   lfs+2, lis+2
        1212 B  97.8713%  UpdateMixerOutputs__14SFXCTL_Physics         bgt+1, cmpwi-1, lis-1
        1008 B  92.7064%  SetupNextLoad__14EAXAemsManager              lwz+4, stw+3, mfcr-1, mr.+1
         908 B  99.3392%  InitAsImpact__Q25Sound14CollisionEventRCQ36A mr-1
         896 B  91.2098%  UpdateParams__16SFXCTL_MasterVolf            mr+3, stw-2, lwz-2
         736 B  90.5652%  QueueFileLoad__14EAXAemsManagerR15stSndAsset li+2, stw-2, lwz-2, addi-1
== zEAXSound2      13 near-miss:   5 con DELTA ( 10356 B, falta codigo),   8 mismo multiconjunto
        7508 B  98.4443%  ResolveCarBanks__18CSTATEMGR_CarState        bgt+1, ble-1, b-1
        1240 B  93.9645%  GenerateRoadNoise__16CARSFX_RoadNoise        b-1
         876 B  98.2648%  TestToLicensed__15SFXObj_PFEATraxb           li-1, lis-1, bne-1
         392 B  94.9388%  Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi  li+1
         340 B  95.8706%  BindToData__14GinsuSynthDataPv               mr-1
== zEagl4Anim      12 near-miss:   7 con DELTA ( 10108 B, falta codigo),   5 mismo multiconjunto
        2784 B  98.4138%  EvalSQT__Q29EAGL4Anim12FnDeltaQFastfPfPCQ29E addi-2, lfs-2
        2352 B  79.6599%  Initialize__Q25EAGL413DynamicLoaderPFPCcRb_P lwz-9, ori+2, lis+2, ble+2
        1480 B  95.3892%  EvalSQT__Q29EAGL4Anim12FnStatelessQfPfPCQ29E clrlwi-2, mr+2
        1468 B  99.3461%  EvalSQT__Q29EAGL4Anim13FnStatelessF3fPfPCQ29 mr+2
        1284 B  98.8162%  EvalPose__Q29EAGL4Anim10FnPoseAnimfPCQ29EAGL mr+3
         456 B  89.8684%  EvalState__Q29EAGL4Anim14FnRawStateChanfPQ29 mr+3, lwz-2, add-2
== zEcstasy        19 near-miss:   9 con DELTA (  8856 B, falta codigo),  10 mismo multiconjunto
        2072 B  83.0579%  epCalculateLocalDirectionalPOS16__FPUiT0iPUs addi-2, mullw+2, bgt-1, ble+1
        1848 B  97.2511%  eProgressiveScan_EURGB60DialogBox__Fi        cmpwi+2
        1412 B  95.0935%  Render__18eViewPlatInterfaceP5ePolyP11Textur addi+3, ori-2, mr-2
        1252 B  97.8626%  elSetupLights__FP20eDynamicLightContextP15eS blt+2, ble-2, lwz-1, stw-1
         812 B  87.7094%  TestLoadStreamingEntry__17eStreamPackLoaderP beq-4, bne+3, b-2, mfcr+1
         696 B  94.5402%  Render__13EmitterSystemP5eView               li-1, ori+1, stw+1, lwz+1
== zFEng            9 near-miss:   6 con DELTA (  7204 B, falta codigo),   3 mismo multiconjunto
        3080 B  50.6662%  ProcessPadsForPackage__7FEngineP9FEPackage   lis-3, li-3, addi-2, mfcr+1
        1640 B  95.2854%  FEInterpLinear__FP10FEKeyTracklPv            mr+2, addi+1, lfs+1, lis+1
        1104 B  99.1667%  ScrollSelection__9FEListBoxll                cmpwi-1
         776 B  98.2474%  Update__7FEnginelUi                          li+1, mr.+1, bne+1
         460 B  93.7826%  RenderGroup__7FEngineP7FEGroupR9FEMatrix4T2U mr-2
         144 B  66.8056%  FindConditionBranchTarget__C17FEMessageRespo bgelr-1, blt+1, blr-1, beqlr+1
== zFe             19 near-miss:   8 con DELTA (  3036 B, falta codigo),  11 mismo multiconjunto
        1204 B  99.2857%  NotificationMessage__9PauseMenuUlP8FEObjectU beq-1, bne+1
         460 B  94.4956%  CheckUnplugged__13cFEngJoyInput              lbz-1, beq-1, mr-1, b+1
         356 B  87.5056%  ScrollTrackPlayability__14UIEATraxScreenUl   li+1, lwz-1
         332 B  96.8675%  PushErrorPackage__5cFEngPCciUl               li+2
         276 B  84.8551%  ScrollRival__14uiRepSheetMain10eScrollDir    lbz+2, stw-1, cmplw-1, cmpw+1
         220 B  84.5455%  __static_initialization_and_destruction_0    li+1, stw+1, addi+1
== zFe2           116 near-miss: 101 con DELTA ( 38356 B, falta codigo),  15 mismo multiconjunto
        1780 B  96.1663%  ClipGeneral__14FERenderObjectP10FEClipInfoP8 lwz-2, beq-1, bgt+1, bne+1
        1668 B  98.0671%  SetupMinimap__7MinimapP7IPlayer              mr-1, cmplwi+1, cmpwi-1
        1572 B  62.0254%  RenderString__8FEngFontRC7FEColorPCsP8FEStri lha-4, lhz+4, xoris+3, extsh+2
        1496 B  81.5348%  IsCarUnlocked__17QuickRaceUnlocker14eUnlockF bgt+14, subfic-14, cmplwi+14, adde-14
        1436 B  90.9025%  Update__13TimeExtensionP7IPlayer             lwz+6, li+5, addi+2, lha+2
        1248 B  94.7917%  NotificationMessage__16IconScrollerMenuUlP8F lwz-5, lha-3, li-2, mtlr-1
== zFeOverlay      37 near-miss:  31 con DELTA ( 18400 B, falta codigo),   6 mismo multiconjunto
        5096 B  98.0424%  NotificationMessage__13UIQRCarSelectUlP8FEOb mr+2, bne+1, bl+1
        1364 B  92.8211%  RefreshHeader__19UIQRChallengeSeries         addi+2, subi-1, beq+1, slwi+1
        1144 B  85.2238%  GetCarPartList__19CarCustomizeManageriRt6bTL li-6, mr-4, beq+3, bgt+2
         996 B  95.5623%  HandleTick__16GarageMainScreenUl             li+1, stw+1, lwz+1, stfd+1
         844 B  99.5024%  HandleJoyEvents__16GarageMainScreen          lis+1
         776 B  94.9948%  __8ShowcaseP21ScreenConstructorData          subi+1, li+1, mcrf-1, addi+1
== zFoundation     21 near-miss:  13 con DELTA (  4712 B, falta codigo),   8 mismo multiconjunto
        1780 B  72.1371%  ExpressionEvaluator__4CARPPCQ24CARP10Express lwz+15, lfs+6, lis+6, li+5
         512 B  86.5938%  __Q24CARP12TagReferencePC6UGroup             blrl+2, beq+2, bne-2
         388 B  39.7423%  __static_initialization_and_destruction_0    stw+14, lwz+10, lis+8, addi+7
         352 B  89.7727%  ResolveOffsets__C6UGroupRC18UGroupResolverDa mr-1
         288 B  90.4028%  ProcessBreadthFirst__C6UGroupRQ26UGroup9Proc mr.-2, b+1, bne-1
         284 B  74.4930%  __adjust_heap__H4ZPQ24CARP15TagResolverNodeZ mr-3, lwzu+1, srawi+1
== zGameModes       0 near-miss:   0 con DELTA (     0 B, falta codigo),   0 mismo multiconjunto
== zGameplay       18 near-miss:  11 con DELTA ( 11256 B, falta codigo),   7 mismo multiconjunto
        2304 B  87.5260%  UpdateAdaptiveDifficulty__11GRaceStatusQ211G fsubs+7, mr+6, fsel+6, lfs+3
        1680 B  97.9000%  GenerateIndex__15GRaceParametersP14GRaceInde addi+1, lwz+1, mr+1
        1500 B  66.6907%  DetermineRaceSegmentLength__11GRaceStatusPCQ beq+4, lwzx-4, fmr-4, li+3
        1336 B  82.3683%  Update__11GRaceStatusf                       lwz-2, mr-2, lis+2, ori+1
        1216 B  98.7368%  AttemptSpawn__10GCharacter                   li-1, beq+1, mr+1, b-1
         916 B  95.1747%  DetermineRaceLength__11GRaceStatus           addi+1, mr+1, subi+1, add-1
== zLua             8 near-miss:   3 con DELTA (  1332 B, falta codigo),   5 mismo multiconjunto
         676 B  98.0473%  Game_NotifySpeedTrapTriggered__FP16GRuntimeI fmr+1
         412 B  98.0097%  Game_PlayTutorial__Fv                        li-1, b-1
         244 B  96.3934%  Game_GetPlayerBounty__Fv                     li+1, mr+1
== zMain           20 near-miss:  12 con DELTA (  8028 B, falta codigo),   8 mismo multiconjunto
        2176 B  98.3548%  __12EPlayRaceNISP7GMarkerPCcT2iiT2T2         bgt-1, cror+1, bso+1, b+1
        2140 B  96.6019%  UpdateForces__19SteeringWheelDeviceP7IPlayer lis-2
        1316 B  98.1246%  _._17EPlayObjectEffect                       mr-4, addi+3
        1304 B  66.9325%  __static_initialization_and_destruction_0    lis+31, addi+18, stw+15, bl+14
         488 B  98.1557%  FetchCurrentValues__11ActionQueueP11InputDev cror+1, bso+1
         268 B  83.8358%  ExecuteFilter__CQ214EventSequencer6SystemPCQ slwi-2, addic.-1, addi+1, lwzx+1
== zMisc            3 near-miss:   3 con DELTA (  1456 B, falta codigo),   0 mismo multiconjunto
        1444 B  44.0055%  __static_initialization_and_destruction_0    stw+44, lis+44, addi+43, lwz+17
           8 B   0.0000%  GetOptimalReadSize__Q28RealFile12DeviceDrive li+1, blr+1
           4 B   0.0000%  GetName__Q28RealFile12DeviceDriver           blr+1
== zMiscSmall       0 near-miss:   0 con DELTA (     0 B, falta codigo),   0 mismo multiconjunto
== zMission         0 near-miss:   0 con DELTA (     0 B, falta codigo),   0 mismo multiconjunto
== zOnline          0 near-miss:   0 con DELTA (     0 B, falta codigo),   0 mismo multiconjunto
== zPhysics        11 near-miss:   8 con DELTA (  7352 B, falta codigo),   3 mismo multiconjunto
        3120 B  98.0154%  __9SmackableRCQ25UMath7Matrix4RCQ36Attrib3Ge lwz-2, mr+2, li+1, stw-1
         928 B  95.4957%  MatchPerformance__Q27Physics8UpgradesRQ36Att fmr-2, beq-1, lis-1, ble+1
         824 B  90.4126%  ShiftPoints__Q27Physics4InfoRCQ36Attrib3Gen1 clrlwi.-1, beq-1, mtcrf+1, bso+1
         736 B  84.3750%  __10PUPartNodeRCQ26Attrib7RefSpecT1f         li+2, bl+2, beq+1, lwz+1
         600 B  91.4933%  Place__16PlaceableSceneryRCQ25UMath7Matrix4b mr-4, addi+1
         548 B  94.7372%  OnBehaviorChange__9SmackableRC6UCrc32        mr+1
== zPhysicsBehaviors   7 near-miss:   4 con DELTA (  6968 B, falta codigo),   3 mismo multiconjunto
        4464 B  49.5125%  __static_initialization_and_destruction_0    bl-159, addi-153, mr-96, stw+22
        1488 B  92.9301%  Add__6RBGridUiR9RigidBodyRCQ25UMath7Vector3f stw-8, blt-4, beq-4, b+4
         856 B  97.2103%  UpdateLoaded__Q217SuspensionTraffic4Tireffff stw-1, stmw+1, lwz-1, lmw+1
         160 B  91.8750%  __Q29RigidBody4MeshRC10SimSurfacePCQ25UMath7 mr+1
== zPlatform       10 near-miss:   5 con DELTA (  3800 B, falta codigo),   5 mismo multiconjunto
        1588 B  82.8917%  ActualReadJoystickData__Fv                   rlwinm+6, or+6, mr+4, stw-3
        1276 B  86.0846%  SpawnParticles__9CGEmitterff                 mr-6, addi+4, fmr+2, fsubs+1
         440 B  95.5727%  PlayFrontalCollisionForce__8LGWheelslUc      mr-1
         320 B  87.1875%  AgeParticles__12ParticleListf                mr+1
         176 B  97.1591%  LockPalette__24TextureInfoPlatInterface15Tex mr.-1, mr+1, cmpwi+1
== zRender          0 near-miss:   0 con DELTA (     0 B, falta codigo),   0 mismo multiconjunto
== zSim             5 near-miss:   3 con DELTA (  4428 B, falta codigo),   2 mismo multiconjunto
        1956 B  89.8650%  __static_initialization_and_destruction_0    lis+16, stw+8, addi+8, bl+8
        1908 B  99.7484%  CreateCars__9QuickGameRCQ25UMath7Vector3     mr-1
         564 B  94.0142%  OnManageTime__9QuickGameff                   lfs+2, lis+2, psq_l-1, psq_st-1
== zSpeech         15 near-miss:   9 con DELTA (  9452 B, falta codigo),   6 mismo multiconjunto
        2236 B  96.7406%  PostValidate__Q26Speech7ManagerPQ26Speech20S extsh+2
        1904 B  95.4853%  Waiting__Q26Speech12StrategyFlow             blrl-2, lwz-1, li-1
        1704 B  96.9132%  ReqBackup__Q26Speech12StrategyFlow           lwz+3, beq+1, lha+1, mtlr+1
        1472 B  95.6957%  ScheduleSpeechPartII__Q26Speech7ManagerUiPvR beq+2, lwz-2, bne-2, addi+1
         804 B  97.0000%  Update__Q26Speech9MusicFlow                  beq-1, bgt+1, cmpwi-1
         596 B  92.5168%  Setup__Q26Speech13RoadblockFlow              mtlr+3, blrl+3, add+1
== zTrack           7 near-miss:   2 con DELTA (  2412 B, falta codigo),   5 mismo multiconjunto
        1992 B  89.5402%  BuildHoleMovements__13TrackStreamerP12HoleMo lwz+7, stw+4, blt+1, bgt-1
         420 B  81.6381%  DetermineStreamingSections__13TrackStreamer  sth-4, mr+4, li+3, sthx+3
== zWorld          11 near-miss:   7 con DELTA (  9924 B, falta codigo),   4 mismo multiconjunto
        2908 B  98.0536%  RenderFlaresOnCar__13CarRenderInfoP5eViewPC8 lis-2, lwz+1, stw+1
        2548 B  98.8728%  DrawAmbientShadow__13CarRenderInfoP5eViewPC8 lis-1
        1232 B  96.6396%  CompositeSkin__FP8RideInfo                   beq+1, bne-1
        1068 B  96.2884%  UpdateLightStateTextures__13CarRenderInfo    lwzx+4, li+2, lwz-2, addi+2
         876 B  96.7808%  UpdateWheelYRenderOffset__13CarRenderInfo    lis+1, stfsx-1, stfs+1, add+1
         836 B  99.3302%  CullParts__13CarPartCullerP8bVector3Us       mr+1
== zWorld2          9 near-miss:   7 con DELTA ( 15200 B, falta codigo),   2 mismo multiconjunto
        4368 B  98.1227%  UpdateOccludedPosition__8WRoadNavb           mr-1
        3456 B  93.0093%  FindNodes__C5WGridPCQ25UMath7Vector4RQ23UTLt b+4, cmpw+4, addi+2, mr-2
        2980 B  94.4456%  HolePunchAvoidables__8WRoadNavP9NavCookieiff fsel-2, addi+1, fsubs-1
        1284 B  99.5327%  CookieTrailCurvature__8WRoadNavRCQ25UMath7Ve fmr+1
        1100 B  96.9745%  GetRightMostTrafficEntrance__12WRoadNetworki mr-2, lwz+1, stw+1
        1044 B  96.8506%  CheckCollideSRB__C15WTriggerManagerPC10IRigi li-1

TOTAL: 129 funciones con delta de mnemonicos (240112 B) -> ahi FALTA CODIGO
       149 con el mismo multiconjunto -> asignador/planificador

LAS 12 MAYORES, por bytes:
      7508 B  zEAXSound2   ResolveCarBanks__18CSTATEMGR_CarState          bgt+1, ble-1, b-1
      5180 B  zBWare       _bOutput__FP11bOutputInfoPCcP13__va_list_tag   lwz-9, stw-3, fmr+2, lfd-1
      5096 B  zFeOverlay   NotificationMessage__13UIQRCarSelectUlP8FEObje mr+2, bne+1, bl+1
      4464 B  zPhysicsBehaviors __static_initialization_and_destruction_0      bl-159, addi-153, mr-96, stw+22
      4368 B  zWorld2      UpdateOccludedPosition__8WRoadNavb             mr-1
      3604 B  zCamera      __static_initialization_and_destruction_0      stfs+141, lis+116, lfs+63, addi+45
      3456 B  zWorld2      FindNodes__C5WGridPCQ25UMath7Vector4RQ23UTLt6V b+4, cmpw+4, addi+2, mr-2
      3120 B  zPhysics     __9SmackableRCQ25UMath7Matrix4RCQ36Attrib3Gen9 lwz-2, mr+2, li+1, stw-1
      3080 B  zFEng        ProcessPadsForPackage__7FEngineP9FEPackage     lis-3, li-3, addi-2, mfcr+1
      2980 B  zWorld2      HolePunchAvoidables__8WRoadNavP9NavCookieiff   fsel-2, addi+1, fsubs-1
      2908 B  zWorld       RenderFlaresOnCar__13CarRenderInfoP5eViewPC8bV lis-2, lwz+1, stw+1
      2908 B  zAI          UpdateAllAvoidables__11AIAvoidablef            blt-4, stw-4, mr+3, bne-2
```
