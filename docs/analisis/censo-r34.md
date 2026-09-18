# Censo R34 de funciones no exactas

Fecha de captura: 2026-09-03. Alcance: las 33 `SourceLists` no vacías que producen objeto; `zComms.cpp` está vacío y no tiene objeto/ASM que comparar.

## Resumen

- Código exacto: **3.197.864 / 3.333.632 B (95,9273%)**.
- Funciones emparejadas: **15.353 exactas + 140 no exactas = 15.493**.
- Censo de ausencia por símbolos del ASM: **1 función adicional, 16 B**, cuyo sufijo local no está emparejado.
- Total de pendientes enumerados abajo: **141 entradas** (140 con porcentaje objdiff y 1 ausente).

La clasificación es operativa, no semántica: `estructura` significa que cambia el multiconjunto normalizado de instrucciones; `asignador` significa que el multiconjunto coincide y sólo quedan registros/reorden local. Los signos del delta son **objetivo − nuestro**: positivo = falta en nuestro objeto; negativo = sobra.

| Grupo | Entradas | Bytes objetivo |
|---|---:|---:|
| Accionable por estructura o código faltante | 46 | 54.072 |
| Mismo multiconjunto / asignador | 92 | 76.920 |
| Plantilla o cabecera compartida | 1 | 1.156 |
| Cuerpo ausente o placeholder | 1 | 16 |
| Static init / datos | 1 | 3.604 |

## Método y reproducción

Se cruzaron tres vistas independientes:

1. `measure.py` para bytes y funciones exactas por unidad.
2. `pctsnap.py`/objdiff para el nombre, tamaño y porcentaje de cada símbolo emparejado.
3. `triage.py --muro` para el multiconjunto de instrucciones normalizadas, sustituciones, registros y distancia máxima de movimientos; `censo.py` para símbolos del objetivo ausentes en nuestro objeto.

Comandos reproducibles desde la raíz del repositorio:

```powershell
$units = Get-ChildItem src/Speed/Indep/SourceLists -Filter '*.cpp' | Where-Object Length -gt 0 | Sort-Object Name | ForEach-Object BaseName
python scripts/measure.py -o antes_r34_foundation.json @units
python scripts/pctsnap.py -o pct_despues_r34_foundation.json
python scripts/triage.py --muro
python scripts/censo.py
```

Los JSON y volcados intermedios de esta captura son `despues_r34_foundation.json`, `pct_despues_r34_foundation.json`, `scratchpad/censo_r34_foundation.json`, `scratchpad/triage_r34_foundation.txt` y `scratchpad/censo_r34_foundation_detail.txt`.

## Ranking de siguientes candidatos no marcados como agotados

Este ranking excluye los techos documentados en el playbook/briefs (entre otros `ResolveCarBanks`, `UpdateLightStateTextures`, `CookieTrailCurvature`, `TestToLicensed`, `CreateCars`, `AssignClosestOffsets`, `Smackable` y `InitAsImpact`). Antes de editar conviene releer el histórico de la unidad porque el censo sólo demuestra el delta actual.

| Prioridad | Unidad | Función | Tamaño | % | Motivo |
|---:|---|---|---:|---:|---|
| 1 | zEcstasy | `UpdatePlatInfo__27eLightMaterialPlatInterface` | 2.044 B | 98,8689% | reorden local de 2 filas, dmax 5; buen tamaño y sin barrido documentado |
| 2 | zEAXSound | `UpdateNIS__14SFXCTL_Physicsff` | 1.592 B | 99,9623% | sólo 3 registros; candidato para vida/ámbito de locales |
| 3 | zEAXSound2 | `InitSFX__16SFXObj_Collision` | 1.304 B | 99,9693% | sólo 2 registros; gran retorno potencial por una forma pequeña |
| 4 | zTrack | `CalculateRegionInfo__11RegionQueryP5eView10RegionTypei` | 1.208 B | 98,5464% | reorden local de 2 filas, dmax 5 |
| 5 | zEAXSound | `SetupNextLoad__14EAXAemsManager` | 1.008 B | 98,6706% | reorden local de 2 filas, dmax 2; revisar primero el avance local existente |

## Ensayo acotado de R34

Se seleccionó inicialmente `SFXObj_PFEATrax::TestToLicensed` (zEAXSound2, 876 B, 98,26484%, sobran `lis`, `li` y `bne`). Dos formas naturales del retorno redundante final redujeron el objeto a 844 B y **regresaron a 95,0685%**, por lo que se revirtieron. Al contrastarlo con `brief-r17` se confirmó que ya era un techo agotado (8 formas y el izado depende de que `r3` esté ocupado); no se retuvo ningún cambio.

## Accionable por estructura o código faltante

| Unidad | Función | Tamaño | % | Delta | Diagnóstico |
|---|---|---:|---:|---|---|
| zEAXSound2 | `ResolveCarBanks__18CSTATEMGR_CarState` | 7.508 B | 98,44433% | b-1, bgt+1, ble-1 | faltan 1, sobran 2, de ellas 1 SUST |
| zBWare | `_bOutput__FP11bOutputInfoPCcP13__va_list_tag` | 5.180 B | 95,46641% | fmr+2, lwz-2, mr+2, stw-2, beq+1, bne-1, lfd-1 | faltan 6, sobran 7, de ellas 15 SUST |
| zPhysics | `__9SmackableRCQ25UMath7Matrix4RCQ36Attrib3Gen9smackablePCQ217CollisionGeometry6BoundsbP6IModelT4T4` | 3.120 B | 99,26154% | beq+1, bne-1, li+1, mr+1 | faltan 3, sobran 1, de ellas 1 SUST |
| zAI | `UpdateAllAvoidables__11AIAvoidablef` | 2.908 B | 99,60110% | b+4, blt-4 | faltan 4, sobran 4, de ellas 4 SUST |
| zWorld | `RenderFlaresOnCar__13CarRenderInfoP5eViewPC8bVector3PC8bMatrix4iii` | 2.908 B | 98,61761% | lis-1 | faltan 0, sobran 1, de ellas 1 SUST |
| zEagl4Anim | `Initialize__Q25EAGL413DynamicLoaderPFPCcRb_Pv` | 2.352 B | 79,65987% | lwz-9, ble+2, cmplw+2, cmplwi+2, lis+2, ori+2, b-1, beq-1, bge+1, bgt+1, cmpwi-1, mfctr+1, mtctr+1, stw+1, subi+1 | faltan 16, sobran 12, de ellas 21 SUST |
| zSim | `CreateCars__9QuickGameRCQ25UMath7Vector3` | 1.908 B | 99,74843% | mr-1 | faltan 0, sobran 1, de ellas 0 SUST |
| zFeOverlay | `Setup__14CustomizeParts` | 1.708 B | 99,83607% | — | faltan 1, sobran 1, de ellas 0 SUST |
| zAI | `AssignClosestOffsets__9AIPursuitRQ33UTL3Stdt6vector2ZQ25UMath7Vector3Z19_type_AIVector3ListRQ33UTL3Stdt6vector2ZP10IPursuitAIZ16_type_AIPursuersRQ33UTL3Stdt6vector2ZQ29AIPursuit15FormationTargetZ27_type_AIFormationTargetListb` | 1.684 B | 99,66746% | cmpwi+1, mr+1, mr.-1 | faltan 2, sobran 1, de ellas 0 SUST |
| zPlatform | `ActualReadJoystickData__Fv` | 1.588 B | 97,35516% | andi.+1, mr+1 | faltan 2, sobran 0, de ellas 1 SUST |
| zPhysicsBehaviors | `Add__6RBGridUiR9RigidBodyRCQ25UMath7Vector3f` | 1.488 B | 99,30108% | b+4, blt-4 | faltan 4, sobran 4, de ellas 4 SUST |
| zEagl4Anim | `EvalSQT__Q29EAGL4Anim12FnStatelessQfPfPCQ29EAGL4Anim8BoneMask` | 1.480 B | 98,81081% | clrlwi-2, mr+2 | faltan 2, sobran 2, de ellas 0 SUST |
| zEagl4Anim | `EvalSQT__Q29EAGL4Anim13FnStatelessF3fPfPCQ29EAGL4Anim8BoneMask` | 1.468 B | 99,34605% | mr+2 | faltan 2, sobran 0, de ellas 0 SUST |
| zEagl4Anim | `EvalPose__Q29EAGL4Anim10FnPoseAnimfPCQ29EAGL4Anim15PosePaletteBankPf` | 1.284 B | 98,81620% | mr+3 | faltan 3, sobran 0, de ellas 0 SUST |
| zWorld2 | `CookieTrailCurvature__8WRoadNavRCQ25UMath7Vector3T1` | 1.284 B | 99,53271% | fmr+1 | faltan 1, sobran 0, de ellas 0 SUST |
| zWorld | `UpdateLightStateTextures__13CarRenderInfo` | 1.068 B | 98,62921% | slwi+1 | faltan 1, sobran 0, de ellas 0 SUST |
| zEAXSound | `InitiateLoad__14EAXAemsManager` | 1.004 B | 98,66534% | mr-1 | faltan 0, sobran 1, de ellas 0 SUST |
| zEAXSound | `InitAsImpact__Q25Sound14CollisionEventRCQ36Attrib3Gen11audioimpact` | 908 B | 99,33921% | mr-1 | faltan 0, sobran 1, de ellas 0 SUST |
| zEAXSound2 | `TestToLicensed__15SFXObj_PFEATraxb` | 876 B | 98,26484% | bne-1, li-1, lis-1 | faltan 0, sobran 3, de ellas 1 SUST |
| zWorld | `UpdateWheelYRenderOffset__13CarRenderInfo` | 876 B | 99,38356% | lis+1 | faltan 1, sobran 0, de ellas 0 SUST |
| zPhysicsBehaviors | `UpdateLoaded__Q217SuspensionTraffic4Tireffff` | 856 B | 97,21028% | lis-1, lmw+1, lwz-1, stmw+1, stw-1 | faltan 2, sobran 3, de ellas 1 SUST |
| zWorld | `CullParts__13CarPartCullerP8bVector3Us` | 836 B | 99,33015% | mr+1 | faltan 1, sobran 0, de ellas 0 SUST |
| zFeOverlay | `NotificationMessage__13CustomizeMainUlP8FEObjectUlUl` | 808 B | 97,89604% | — | faltan 1, sobran 1, de ellas 0 SUST |
| zFEng | `Update__7FEnginelUi` | 776 B | 99,14948% | cmpwi-1, li-1, mr.+1 | faltan 1, sobran 2, de ellas 1 SUST |
| zEAXSound | `UpdateRPM__13SFXCTL_Enginef` | 716 B | 96,45252% | fmr-1 | faltan 0, sobran 1, de ellas 1 SUST |
| zEcstasy | `Render__13EmitterSystemP5eView` | 696 B | 82,12069% | lwz+1, stw+1 | faltan 2, sobran 0, de ellas 0 SUST |
| zFe2 | `ScrollVer__13ArrayScroller10eScrollDir` | 620 B | 96,29032% | mr+1 | faltan 1, sobran 0, de ellas 0 SUST |
| zEAXSound | `Play__11EAXFrontEnd18eMenuSoundTriggers` | 612 B | 97,32026% | cmpwi-1, mr-1, mr.+1 | faltan 1, sobran 2, de ellas 1 SUST |
| zPhysics | `Place__16PlaceableSceneryRCQ25UMath7Matrix4b` | 600 B | 91,49333% | mr-4, addi+1 | faltan 1, sobran 4, de ellas 2 SUST |
| zSim | `OnManageTime__9QuickGameff` | 564 B | 94,01418% | lfs+2, lis+2, fmr-1, psq_l-1, psq_st-1 | faltan 4, sobran 3, de ellas 1 SUST |
| zEAXSound | `Play__9EAXCommon18eMenuSoundTriggers` | 492 B | 96,82927% | b+1, beq-1, bne+1, li+1, mr-1 | faltan 3, sobran 2, de ellas 1 SUST |
| zEagl4Anim | `EvalState__Q29EAGL4Anim14FnRawStateChanfPQ29EAGL4Anim5State` | 456 B | 93,50877% | add-2, mr+2 | faltan 2, sobran 2, de ellas 2 SUST |
| zFeOverlay | `SetScreenNames__13CustomizeMain` | 440 B | 91,72727% | — | faltan 1, sobran 1, de ellas 0 SUST |
| zEAXSound2 | `Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi` | 392 B | 94,93877% | li+1 | faltan 1, sobran 0, de ellas 1 SUST |
| zFe2 | `PackString__17WideCharHistogramPciPCUs` | 368 B | 95,81522% | add+1, addi-1, stb+1, stbx-1, subi+1 | faltan 3, sobran 2, de ellas 1 SUST |
| zSpeech | `Report911__11EAXDispatchQ24Csis17Type_pursuit_type` | 368 B | 95,21739% | b-1, stw-1 | faltan 0, sobran 2, de ellas 0 SUST |
| zEAXSound2 | `BindToData__14GinsuSynthDataPv` | 340 B | 95,87059% | mr-1 | faltan 0, sobran 1, de ellas 0 SUST |
| zFe | `PushErrorPackage__5cFEngPCciUl` | 332 B | 96,86747% | li+2 | faltan 2, sobran 0, de ellas 0 SUST |
| zSpeech | `LoadSpeechBank__Q26Speech7ManagerP17CLUMP_IDX_FILEtagRiT2PQ26Speech11SPEECH_BANK` | 316 B | 95,31645% | mr+1 | faltan 1, sobran 0, de ellas 0 SUST |
| zEAXSound | `Create__14SndStrmWrapperiii` | 176 B | 82,00000% | mr+1 | faltan 1, sobran 0, de ellas 1 SUST |
| zPlatform | `LockPalette__24TextureInfoPlatInterface15TextureLockType` | 176 B | 97,38636% | cmpwi+1, mr+1, mr.-1 | faltan 2, sobran 1, de ellas 0 SUST |
| zCamera | `LoadCameraShakes__10ICEManagerP6bChunk` | 168 B | 96,66666% | li+1 | faltan 1, sobran 0, de ellas 0 SUST |
| zFe2 | `ConvertBigBangMarkerAward__15FEMarkerManagerPCcT1` | 140 B | 98,28571% | — | faltan 2, sobran 2, de ellas 0 SUST |
| zFe | `GetElapsedTime__9SubTitler` | 116 B | 95,51724% | fmr+1 | faltan 1, sobran 0, de ellas 0 SUST |
| zEcstasy | `GetPostionMarker__6eSolidP15ePositionMarker` | 92 B | 88,91304% | mr+1 | faltan 1, sobran 0, de ellas 2 SUST |
| zEAXSound | `AssignAudioStreamHandle__FUi` | 16 B | 25,00000% | cmplw+1, lis+1, lwz+1 | faltan 3, sobran 0, de ellas 0 SUST |

## Mismo multiconjunto / asignador

| Unidad | Función | Tamaño | % | Delta | Diagnóstico |
|---|---|---:|---:|---|---|
| zCamera | `Update__8ICEMoverf` | 3.868 B | 99,90693% | — | 15 registros |
| zFEng | `ProcessPadsForPackage__7FEngineP9FEPackage` | 3.080 B | 97,89220% | — | reorden local, dmax 4 (10 filas crudas, CERO reales) |
| zWorld2 | `HolePunchAvoidables__8WRoadNavP9NavCookieiff` | 2.980 B | 95,44161% | — | reorden local, dmax 11 (26 filas crudas, CERO reales) |
| zGameplay | `__8GTriggerRCUi` | 2.544 B | 99,56761% | — | reorden local, dmax 6 (2 filas crudas, CERO reales) |
| zMain | `__12EPlayRaceNISP7GMarkerPCcT2iiT2T2` | 2.176 B | 99,48529% | — | 29 registros |
| zMain | `_._14ESpawnFragment` | 2.164 B | 98,86876% | — | reorden local, dmax 3 (4 filas crudas, CERO reales) |
| zEAXSound | `Play__14cSampleWarpperPC10SND_Params` | 2.152 B | 90,10780% | — | reorden local, dmax 7 (44 filas crudas, CERO reales) |
| zMain | `UpdateForces__19SteeringWheelDeviceP7IPlayer` | 2.140 B | 99,93458% | — | 6 registros |
| zEcstasy | `epCalculateLocalDirectionalPOS16__FPUiT0iPUsPiPUciiP14eLightMaterialP13eLightContext` | 2.072 B | 92,33784% | — | reorden local, dmax 20 (28 filas crudas, CERO reales) |
| zEcstasy | `UpdatePlatInfo__27eLightMaterialPlatInterface` | 2.044 B | 98,86888% | — | reorden local, dmax 5 (2 filas crudas, CERO reales) |
| zDynamics | `SphereVsBox__Q38Dynamics9Collision8GeometryPCQ38Dynamics9Collision8GeometryT1PQ38Dynamics9Collision8Geometry` | 1.896 B | 98,47890% | — | reorden local, dmax 4 (6 filas crudas, CERO reales) |
| zEcstasy | `eProgressiveScan_EURGB60DialogBox__Fi` | 1.848 B | 98,76623% | — | 94 registros |
| zEcstasy | `UpdateParticles__13EmitterSystemf` | 1.820 B | 99,95605% | — | 4 registros |
| zGameplay | `GenerateIndex__15GRaceParametersP14GRaceIndexData` | 1.680 B | 99,03571% | — | reorden local, dmax 2 (4 filas crudas, CERO reales) |
| zEAXSound | `UpdateNIS__14SFXCTL_Physicsff` | 1.592 B | 99,96231% | — | 3 registros |
| zFe2 | `RenderString__8FEngFontRC7FEColorPCsP8FEStringP8bMatrix4P14FERenderObjectP19FEPackageRenderInfo` | 1.572 B | 93,32316% | — | reorden local, dmax 5 (18 filas crudas, CERO reales) |
| zFe2 | `IsCarUnlocked__17QuickRaceUnlocker14eUnlockFiltersUii` | 1.496 B | 99,83957% | — | 12 registros |
| zPhysics | `__13PhysicsObjectPCcT111SimableTypeP10HSIMABLE__Ui` | 1.436 B | 99,24234% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zWorld2 | `FindFaceInCInst__13WCollisionMgrRCQ25UMath7Matrix4RCQ25UMath7Vector3RC18WCollisionInstanceR13WCollisionTriRf` | 1.436 B | 99,91644% | — | 6 registros |
| zEcstasy | `Render__18eViewPlatInterfaceP5ePolyP11TextureInfoi` | 1.412 B | 99,44759% | — | 31 registros |
| zEAXSound2 | `InitSFX__16SFXObj_Collision` | 1.304 B | 99,96932% | — | 2 registros |
| zEAXSound2 | `GenerateRoadNoise__16CARSFX_RoadNoise` | 1.240 B | 96,10968% | — | reorden local, dmax 6 (8 filas crudas, CERO reales) |
| zTrack | `CalculateRegionInfo__11RegionQueryP5eView10RegionTypei` | 1.208 B | 98,54636% | — | reorden local, dmax 5 (2 filas crudas, CERO reales) |
| zCamera | `TerrainVelocityNoise__11CameraMoverP8bMatrix4P12CameraAnchorff` | 1.192 B | 99,07718% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zWorld2 | `CheckCollideSRB__C15WTriggerManagerPC10IRigidBodyPC8WTriggerf` | 1.044 B | 99,13793% | — | 45 registros |
| zEAXSound | `SetupNextLoad__14EAXAemsManager` | 1.008 B | 98,67063% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zCamera | `Update__19TrackCarCameraMoverf` | 992 B | 99,56855% | — | 18 registros |
| zCamera | `Update__19TrackCopCameraMoverf` | 948 B | 99,15612% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zSim | `__Q23Sim6Entity` | 924 B | 99,86147% | — | 4 registros |
| zAnim | `Init__16CWorldAnimEntityPvP9SpaceNode` | 912 B | 97,98245% | — | reorden local, dmax 16 (4 filas crudas, CERO reales) |
| zEAXSound | `UpdateParams__16SFXCTL_MasterVolf` | 896 B | 98,12500% | — | 51 registros |
| zWorld2 | `InitAtSegment__8WRoadNavscf` | 816 B | 99,00491% | — | 49 registros |
| zEcstasy | `GenerateHorizonFogDisplayList__FPPvPUl9_GXVtxFmt` | 796 B | 98,99497% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zEAXSound2 | `MsgPlayMiscSound__10SFX_CommonRC10MMiscSound` | 780 B | 97,26154% | — | reorden local, dmax 2 (4 filas crudas, CERO reales) |
| zWorld | `CreateCarLightFlares__13CarRenderInfo` | 748 B | 99,73262% | — | 8 registros |
| zEAXSound | `QueueFileLoad__14EAXAemsManagerR15stSndAssetQueue15eBANK_SLOT_TYPE` | 736 B | 98,52718% | — | reorden local, dmax 4 (2 filas crudas, CERO reales) |
| zEagl4Anim | `FindMatchTime__CQ29EAGL4Anim12FnRunBlenderRCQ29EAGL4Anim15MatchPhaseInputRf` | 720 B | 97,36111% | — | reorden local, dmax 7 (4 filas crudas, CERO reales) |
| zTrack | `GetLoadingPriority__13TrackStreamerP21TrackStreamingSectionP22StreamingPositionEntryb` | 708 B | 97,18079% | — | 67 registros |
| zWorld | `DefragmentPool__9CarLoader` | 684 B | 99,26901% | — | 23 registros |
| zEAXSound2 | `ProcessUpdate__12CARSFX_Turbo` | 672 B | 97,50000% | — | reorden local, dmax 5 (4 filas crudas, CERO reales) |
| zPhysics | `Simplify__9Smackable` | 664 B | 99,87952% | — | 4 registros |
| zTrack | `FindZone__16TrackPathManagerPC8bVector218eTrackPathZoneTypeP13TrackPathZone` | 664 B | 99,84940% | — | 5 registros |
| zTrack | `GetPredictedZone__13TrackStreamerP22StreamingPositionEntry` | 660 B | 99,93939% | — | 2 registros |
| zEAXSound | `STREAM_create__FiiiPvi` | 616 B | 99,55195% | — | 15 registros |
| zAI | `UpdateRoadAffinity__22AIActionPursuitOffRoadRQ25UMath7Vector3` | 600 B | 93,73333% | — | reorden local, dmax 3 (6 filas crudas, CERO reales) |
| zSpeech | `Setup__Q26Speech13RoadblockFlow` | 596 B | 98,25504% | — | reorden local, dmax 4 (2 filas crudas, CERO reales) |
| zTrack | `HandleLoading__13TrackStreamer` | 592 B | 99,49324% | — | 12 registros |
| zEAXSound2 | `UpdateASREvent__9NFSMixMapP15stEvtMixCtlProc` | 556 B | 99,60432% | — | 9 registros |
| zSpeech | `PursuitEscalation__11EAXDispatch` | 552 B | 98,44203% | — | reorden local, dmax 3 (2 filas crudas, CERO reales) |
| zFEng | `IssueScriptMessages__9FEPackageP7FEngineP8FEObjectP8FEScriptll` | 540 B | 98,14815% | — | 33 registros |
| zFe | `FoundEntry__16MemcardCallbacksPCQ211RealmcIface9EntryInfo` | 512 B | 97,92969% | — | reorden local, dmax 5 (2 filas crudas, CERO reales) |
| zFe | `NotificationMessage__12uiCareerCribUlP8FEObjectUlUl` | 512 B | 99,76562% | — | 6 registros |
| zEAXSound | `RegisterSlots__14EAXAemsManager15eBANK_SLOT_TYPEiiib` | 508 B | 99,82677% | — | 2 registros |
| zSpeech | `NotifyEventCompletion__Q26Speech7ManagerPQ26Speech20ScheduledSpeechEventb` | 504 B | 98,37302% | — | reorden local, dmax 3 (2 filas crudas, CERO reales) |
| zEagl4Anim | `Eval__Q29EAGL4Anim18FnRawLinearChannelffPf` | 488 B | 98,85246% | — | 22 registros |
| zFe2 | `__21PostRacePursuitScreenP21ScreenConstructorData` | 460 B | 98,21739% | — | 16 registros |
| zEAXSound2 | `CreateMixCtls__14NFSMixMapState` | 448 B | 97,32143% | — | reorden local, dmax 5 (2 filas crudas, CERO reales) |
| zPlatform | `PlayFrontalCollisionForce__8LGWheelslUc` | 440 B | 99,45454% | — | 12 registros |
| zEAXSound | `Play__13cStichWrapperPC10SND_Params` | 384 B | 99,68750% | — | 4 registros |
| zFe2 | `__12IconScrollerPCcN31f` | 384 B | 95,47916% | — | reorden local, dmax 9 (4 filas crudas, CERO reales) |
| zEAXSound | `startnextrequest__FP15STREAMHEADERtagi` | 356 B | 94,30337% | — | reorden local, dmax 2 (4 filas crudas, CERO reales) |
| zEAXSound2 | `CreateSubMixChannels__14NFSMixMapState` | 336 B | 98,69048% | — | 16 registros |
| zBWare | `bATan__Fff` | 328 B | 97,43903% | — | 26 registros |
| zEAXSound | `DataLoadCB__14EAXAemsManagerii` | 320 B | 94,48750% | — | reorden local, dmax 2 (4 filas crudas, CERO reales) |
| zWorld | `SetMemoryPoolSize__9CarLoaderi` | 304 B | 97,36842% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zGameplay | `AllocateInstanceMap__8GManager` | 280 B | 98,07143% | — | 24 registros |
| zEAXSound | `MsgPlayMiscSound__9EAXCommonRC10MMiscSound` | 268 B | 96,76119% | — | reorden local, dmax 6 (2 filas crudas, CERO reales) |
| zEcstasy | `eProject__FfffPA3_fPfN44` | 268 B | 85,82089% | — | reorden local, dmax 6 (8 filas crudas, CERO reales) |
| zEAXSound | `UpdateParams__18SFXCTL_HybridMotorf` | 256 B | 85,76562% | — | reorden local, dmax 6 (8 filas crudas, CERO reales) |
| zFe | `FEngSetScaleX__FP8FEObjectf` | 256 B | 99,29688% | — | 8 registros |
| zFe | `FEngSetScaleY__FP8FEObjectf` | 256 B | 99,29688% | — | 8 registros |
| zBWare | `bStrNICmp__FPCcT0i` | 252 B | 98,80952% | — | 11 registros |
| zSpeech | `BreakAway__11EAXDispatch` | 252 B | 95,74603% | — | reorden local, dmax 13 (2 filas crudas, CERO reales) |
| zEAXSound | `UpdateParams__17SFXCTL_AccelTransf` | 232 B | 99,48276% | — | 6 registros |
| zEagl4Anim | `FindTime__Q29EAGL4Anim14FnRawStateChanRCQ29EAGL4Anim9StateTestfRf` | 232 B | 99,56896% | — | 4 registros |
| zGameplay | `PreloadTransientVaults__8GManagerP20AttribVaultPackImage` | 200 B | 93,40000% | — | reorden local, dmax 6 (2 filas crudas, CERO reales) |
| zBWare | `bStrNCmp__FPCcT0i` | 176 B | 99,09091% | — | 5 registros |
| zFe | `__17UIMemcardKeyboardP21ScreenConstructorData` | 172 B | 92,97675% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zMain | `ConvertWheelRotation__19SteeringWheelDevicei` | 172 B | 98,83721% | — | 8 registros |
| zPlatform | `UnlockPalette__24TextureInfoPlatInterfacePv` | 172 B | 89,41860% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zEAXSound | `ResolveCurrentDataMemory__14EAXAemsManager` | 168 B | 90,23810% | — | reorden local, dmax 2 (4 filas crudas, CERO reales) |
| zEAXSound | `Find__Q26Speech12EventHistory18SPCHType_1_EventID` | 148 B | 94,59460% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zFEng | `FindConditionBranchTarget__C17FEMessageResponseUl` | 144 B | 98,47222% | — | 9 registros |
| zSim | `Link__7SimTask` | 144 B | 83,19444% | — | reorden local, dmax 29 (6 filas crudas, CERO reales) |
| zEAXSound2 | `MsgBarrier__18CARSFX_PreColWooshRC16MAudioReflection` | 140 B | 94,28571% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zAI | `TimeUntilBusted__C9AIPursuit` | 120 B | 98,00000% | — | 7 registros |
| zBWare | `bFMod__Fff` | 108 B | 97,77778% | — | 8 registros |
| zEAXSound | `sort_nested_priority__Q26Speech20ScheduledSpeechEventPCQ26Speech20ScheduledSpeechEventT1` | 104 B | 99,23077% | — | 3 registros |
| zGameplay | `GetStrippedNameKey__8GManagerPCc` | 100 B | 98,80000% | — | 5 registros |
| zFe2 | `ToggleCapsLock__10FEKeyboard` | 92 B | 91,08696% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zEagl4Anim | `NewBlockAux__Q29EAGL4Anim17MemoryPoolManagerUi` | 84 B | 88,57143% | — | reorden local, dmax 2 (2 filas crudas, CERO reales) |
| zEAXSound | `inbetween__FPcN20` | 64 B | 87,50000% | — | reorden local, dmax 3 (2 filas crudas, CERO reales) |

## Plantilla o cabecera compartida

| Unidad | Función | Tamaño | % | Delta | Diagnóstico |
|---|---|---:|---:|---|---|
| zCamera | `__Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2RCQ33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2` | 1.156 B | 96,15917% | mr+2, bge-1, bgt+1, ble+1, blt-1, cmpwi-1, lwz-1, mr.+1 | faltan 5, sobran 4, de ellas 1 SUST |

## Cuerpo ausente o placeholder

| Unidad | Función | Tamaño | % | Delta | Diagnóstico |
|---|---|---:|---:|---|---|
| zGameplay | `Compare__Q38GManager48GatherVisibleIcons__8GManagerPP5GIconP7IPlayer.0_8IconSortPCvT1.35326` | 16 B | N/D | — | símbolo objetivo sin cuerpo emparejable |

## Static init / datos

| Unidad | Función | Tamaño | % | Delta | Diagnóstico |
|---|---|---:|---:|---|---|
| zCamera | `__static_initialization_and_destruction_0` | 3.604 B | 97,11210% | addi-4 | faltan 4, sobran 8, de ellas 2 SUST |
