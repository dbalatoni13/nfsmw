#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"
#include "Speed/Indep/Src/Main/Event.h"

// POSICION: esta lista va en orden alfabetico SIN distinguir mayusculas
// (MEnteringGameplay antes que MEnterPostRaceFlow), que es el que da el
// objetivo. NO la reordenes con un sort ASCII.
#include "Speed/Indep/Src/Generated/Messages/MAcceptEnterCareerEvent.h"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"
#include "Speed/Indep/Src/Generated/Messages/MAudioReflection.h"
#include "Speed/Indep/Src/Generated/Messages/MBreakerStopCops.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MCountdownDone.h"
#include "Speed/Indep/Src/Generated/Messages/MDeclineEnterCareerEvent.h"
#include "Speed/Indep/Src/Generated/Messages/MEnterFreeRoam.h"
#include "Speed/Indep/Src/Generated/Messages/MEnteringGameplay.h"
#include "Speed/Indep/Src/Generated/Messages/MEnterPostRaceFlow.h"
#include "Speed/Indep/Src/Generated/Messages/MEnterRaceOverFlow.h"
#include "Speed/Indep/Src/Generated/Messages/MEnterSafeHouse.h"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"
#include "Speed/Indep/Src/Generated/Messages/MForcePursuitStart.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MICECameraFinished.h"
#include "Speed/Indep/Src/Generated/Messages/MJackKnife.h"
#include "Speed/Indep/Src/Generated/Messages/MJumpCut.h"
#include "Speed/Indep/Src/Generated/Messages/MLoadingComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MNISComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallComplete.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyCellCallStarted.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyChallengePassed.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyEngineBlown.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyFinished.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyKnockedOut.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMessageDone.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMilestoneProgress.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMilestoneReached.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyOnlineRaceOver.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyPlayerRep.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyPursuitLength.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceAbandoned.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRacePlacement.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceTime.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceTimeExpired.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyRaceTimeSecTick.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySimTick.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeedTrap.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyTimer.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyVehicleDestroyed.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpEscaped.h"
#include "Speed/Indep/Src/Generated/Messages/MPlayerEnterPursuit.h"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"
#include "Speed/Indep/Src/Generated/Messages/MPursuitOver.h"
#include "Speed/Indep/Src/Generated/Messages/MQuitToFrontEnd.h"
#include "Speed/Indep/Src/Generated/Messages/MReqBackup.h"
#include "Speed/Indep/Src/Generated/Messages/MReqRoadBlock.h"
#include "Speed/Indep/Src/Generated/Messages/MRestartRace.h"
#include "Speed/Indep/Src/Generated/Messages/MSetCopAutoSpawnMode.h"
#include "Speed/Indep/Src/Generated/Messages/MSetCopsEnabled.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Generated/Messages/MSpawnCop.h"
#include "Speed/Indep/Src/Generated/Messages/MSpawnTraffic.h"
#include "Speed/Indep/Src/Generated/Messages/MStateEnter.h"
#include "Speed/Indep/Src/Generated/Messages/MStateExit.h"
#include "Speed/Indep/Src/Generated/Messages/MTriggerEnter.h"
#include "Speed/Indep/Src/Generated/Messages/MTriggerExit.h"
#include "Speed/Indep/Src/Generated/Messages/MTriggerInside.h"
#include "Speed/Indep/Src/Generated/Messages/MUnspawnCop.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/PackedBinaryTree.h"

void EScheduleEvent_MakeEvent_Callback(const void *staticData);
void EFadeScreenOn_MakeEvent_Callback(const void *staticData);
void ENISStopEffects_MakeEvent_Callback(const void *staticData);
void EAutoSave_MakeEvent_Callback(const void *staticData);
void EProcessAreaStimulus_MakeEvent_Callback(const void *staticData);
void EResetSequencer_MakeEvent_Callback(const void *staticData);
void ETireBlown_MakeEvent_Callback(const void *staticData);
void EResetPlayerCar_MakeEvent_Callback(const void *staticData);
void EGPSLost_MakeEvent_Callback(const void *staticData);
void ENISBurnout_MakeEvent_Callback(const void *staticData);
void ENISFreeze_MakeEvent_Callback(const void *staticData);
void EWorldMapOn_MakeEvent_Callback(const void *staticData);
void ERaceSheetOn_MakeEvent_Callback(const void *staticData);
void EShowTimeExtension_MakeEvent_Callback(const void *staticData);
void EPlayRaceMovie_MakeEvent_Callback(const void *staticData);
void EDisablePursuitVehicle_MakeEvent_Callback(const void *staticData);
void ENISDetach_MakeEvent_Callback(const void *staticData);
void EKillJoint_MakeEvent_Callback(const void *staticData);
void EPlayEndNIS_MakeEvent_Callback(const void *staticData);
void EEnableTrigger_MakeEvent_Callback(const void *staticData);
void ENISSteering_MakeEvent_Callback(const void *staticData);
void EEndCarStop_MakeEvent_Callback(const void *staticData);
void EJointDetached_MakeEvent_Callback(const void *staticData);
void EJumpToStrategyFlow_MakeEvent_Callback(const void *staticData);
void ENISCarDamageReset_MakeEvent_Callback(const void *staticData);
void EDDaySpeech_MakeEvent_Callback(const void *staticData);
void ENISRoadNoise_MakeEvent_Callback(const void *staticData);
void EStopObjectEffects_MakeEvent_Callback(const void *staticData);
void ECameraShake_MakeEvent_Callback(const void *staticData);
void EMomentStrm_MakeEvent_Callback(const void *staticData);
void EExitEngagableTrigger_MakeEvent_Callback(const void *staticData);
void ESetPlayerCarReset_MakeEvent_Callback(const void *staticData);
void EUnPause_MakeEvent_Callback(const void *staticData);
void ECameraPhotoFinish_MakeEvent_Callback(const void *staticData);
void ELoadingScreenOn_MakeEvent_Callback(const void *staticData);
void ESpawnExplosion_MakeEvent_Callback(const void *staticData);
void ENISCopCarDoors_MakeEvent_Callback(const void *staticData);
void ENISFakeFar_MakeEvent_Callback(const void *staticData);
void ENISHideCharacter_MakeEvent_Callback(const void *staticData);
void EBailPursuit_MakeEvent_Callback(const void *staticData);
void EShowMilestones_MakeEvent_Callback(const void *staticData);
void EGPSFinished_MakeEvent_Callback(const void *staticData);
void ENISDetail_MakeEvent_Callback(const void *staticData);
void EFadeScreenOff_MakeEvent_Callback(const void *staticData);
void EEnableCollisionElement_MakeEvent_Callback(const void *staticData);
void ENISCopLights_MakeEvent_Callback(const void *staticData);
void ENISPixelate_MakeEvent_Callback(const void *staticData);
void E911Call_MakeEvent_Callback(const void *staticData);
void EAudioSmackableTest_MakeEvent_Callback(const void *staticData);
void EQuitToFE_MakeEvent_Callback(const void *staticData);
void ENISBrakelock_MakeEvent_Callback(const void *staticData);
void EHideRaceOverMessage_MakeEvent_Callback(const void *staticData);
void EDynamicRegion_MakeEvent_Callback(const void *staticData);
void ETirePunctured_MakeEvent_Callback(const void *staticData);
void ERandomEventList_MakeEvent_Callback(const void *staticData);
void EGTriggerInternal_MakeEvent_Callback(const void *staticData);
void ENISReattach_MakeEvent_Callback(const void *staticData);
void EBreakerStopCops_MakeEvent_Callback(const void *staticData);
void ERequestEventInfoDialog_MakeEvent_Callback(const void *staticData);
void ESpawnSmackable_MakeEvent_Callback(const void *staticData);
void EFireRandomTrigger_MakeEvent_Callback(const void *staticData);
void ENISTimeOfDay_MakeEvent_Callback(const void *staticData);
void ENISCarShake_MakeEvent_Callback(const void *staticData);
void EForceCarStop_MakeEvent_Callback(const void *staticData);
void EBecomeAiCar_MakeEvent_Callback(const void *staticData);
void ETuneVehicle_MakeEvent_Callback(const void *staticData);
void EAccelerate_MakeEvent_Callback(const void *staticData);
void EShowRaceOverMessage_MakeEvent_Callback(const void *staticData);
void ENISConstraint_MakeEvent_Callback(const void *staticData);
void EQuitDemo_MakeEvent_Callback(const void *staticData);
void EEnableModeling_MakeEvent_Callback(const void *staticData);
void ERestartRace_MakeEvent_Callback(const void *staticData);
void EStopObjectEffect_MakeEvent_Callback(const void *staticData);
void ELoadingScreenOff_MakeEvent_Callback(const void *staticData);
void EReloadHud_MakeEvent_Callback(const void *staticData);
void EPursuitBreaker_MakeEvent_Callback(const void *staticData);
void EResetSystem_MakeEvent_Callback(const void *staticData);
void ENISWolrdGeometry_MakeEvent_Callback(const void *staticData);
void ENISPlayEffect_MakeEvent_Callback(const void *staticData);
void ESetSimRate_MakeEvent_Callback(const void *staticData);
void EPlayRaceNIS_MakeEvent_Callback(const void *staticData);
void ENISRain_MakeEvent_Callback(const void *staticData);
void EAIEngineRev_MakeEvent_Callback(const void *staticData);
void EReloadGame_MakeEvent_Callback(const void *staticData);
void EPerfectLaunch_MakeEvent_Callback(const void *staticData);
void ESndGameState_MakeEvent_Callback(const void *staticData);
void EPerfectShift_MakeEvent_Callback(const void *staticData);
void EFadeScreenNoLoadingBarOff_MakeEvent_Callback(const void *staticData);
void ECinematicMoment_MakeEvent_Callback(const void *staticData);
void ETriggerMomentNIS_MakeEvent_Callback(const void *staticData);
void ESetCopAutoSpawnMode_MakeEvent_Callback(const void *staticData);
void EAwardUpgrade_MakeEvent_Callback(const void *staticData);
void EShowSMS_MakeEvent_Callback(const void *staticData);
void EChangeState_MakeEvent_Callback(const void *staticData);
void ECellCall_MakeEvent_Callback(const void *staticData);
void EResetProps_MakeEvent_Callback(const void *staticData);
void ENISAeroDynamics_MakeEvent_Callback(const void *staticData);
void EWakeObject_MakeEvent_Callback(const void *staticData);
void EEnableAIPhysics_MakeEvent_Callback(const void *staticData);
void EAddSMS_MakeEvent_Callback(const void *staticData);
void ENISScreenFlash_MakeEvent_Callback(const void *staticData);
void ENISCarPitch_MakeEvent_Callback(const void *staticData);
void EFireEventList_MakeEvent_Callback(const void *staticData);
void ELoadLost_MakeEvent_Callback(const void *staticData);
void ENISNukeSmack_MakeEvent_Callback(const void *staticData);
void ETerminateMusic_MakeEvent_Callback(const void *staticData);
void EEngineBlown_MakeEvent_Callback(const void *staticData);
void EPause_MakeEvent_Callback(const void *staticData);
void EBecomePlayerCar_MakeEvent_Callback(const void *staticData);
void EKillObject_MakeEvent_Callback(const void *staticData);
void EPlayObjectEffect_MakeEvent_Callback(const void *staticData);
void EDispIntroRace_MakeEvent_Callback(const void *staticData);
void EScheduleEventUpdate_MakeEvent_Callback(const void *staticData);
void EFadeScreenNoLoadingBarOn_MakeEvent_Callback(const void *staticData);
void EShockObject_MakeEvent_Callback(const void *staticData);
void EShowMessageScreen_MakeEvent_Callback(const void *staticData);
void EReportInfraction_MakeEvent_Callback(const void *staticData);
void EProcessStimulus_MakeEvent_Callback(const void *staticData);
void ENISNitro_MakeEvent_Callback(const void *staticData);
void EHidePart_MakeEvent_Callback(const void *staticData);
void EDebugPrint_MakeEvent_Callback(const void *staticData);
void EAudioWorldTest_MakeEvent_Callback(const void *staticData);
void EDestroyVehicle_MakeEvent_Callback(const void *staticData);
void EMissShift_MakeEvent_Callback(const void *staticData);
void EDisableTrigger_MakeEvent_Callback(const void *staticData);
void ENISMotionBlur_MakeEvent_Callback(const void *staticData);
void ENISNeutralRev_MakeEvent_Callback(const void *staticData);
void EPlayerTriggeredNOS_MakeEvent_Callback(const void *staticData);
void EHideObject_MakeEvent_Callback(const void *staticData);
void EVehicleDestroyed_MakeEvent_Callback(const void *staticData);
void ERandomExplosion_MakeEvent_Callback(const void *staticData);
void EAudioRigidBodyTest_MakeEvent_Callback(const void *staticData);
void EDebugScreenMessage_MakeEvent_Callback(const void *staticData);
void EEnterEngagableTrigger_MakeEvent_Callback(const void *staticData);
void ESetPlayerCollisionCache_MakeEvent_Callback(const void *staticData);
void ERaceSheetOff_MakeEvent_Callback(const void *staticData);
void ECommitRenderAssets_MakeEvent_Callback(const void *staticData);
void ESpawnFragment_MakeEvent_Callback(const void *staticData);
void EVehicleReset_MakeEvent_Callback(const void *staticData);
void EDeliverMessage_MakeEvent_Callback(const void *staticData);
void EEnterBin_MakeEvent_Callback(const void *staticData);
void ECommitAudioAssets_MakeEvent_Callback(const void *staticData);
void ENISCarRoll_MakeEvent_Callback(const void *staticData);
void EShowMarketingScreen_MakeEvent_Callback(const void *staticData);
void EReportMilestoneAtStake_MakeEvent_Callback(const void *staticData);
void EFireTriggerSpeedCondition_MakeEvent_Callback(const void *staticData);
void ENISOverlayMessage_MakeEvent_Callback(const void *staticData);
void EBecomePursuitCar_MakeEvent_Callback(const void *staticData);
void ENISVisualLook_MakeEvent_Callback(const void *staticData);
void EPlayerShift_MakeEvent_Callback(const void *staticData);
void ETips_MakeEvent_Callback(const void *staticData);
void EWorldMapOff_MakeEvent_Callback(const void *staticData);
void EShowRaceCountdown_MakeEvent_Callback(const void *staticData);
void EKnockoutRacer_MakeEvent_Callback(const void *staticData);
void EDamageLights_MakeEvent_Callback(const void *staticData);
void ESimulate_MakeEvent_Callback(const void *staticData);
void EPlayerAirborne_MakeEvent_Callback(const void *staticData);
void ENISLights_MakeEvent_Callback(const void *staticData);
void EShowResults_MakeEvent_Callback(const void *staticData);
void ENISWorldAnimTrigger_MakeEvent_Callback(const void *staticData);
void EScheduleEvent_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISStopEffects_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EAutoSave_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EProcessAreaStimulus_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EResetSequencer_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EResetPlayerCar_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISBurnout_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISFreeze_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EWorldMapOn_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EShowTimeExtension_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EPlayRaceMovie_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDisablePursuitVehicle_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISDetach_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EKillJoint_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EPlayEndNIS_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EEnableTrigger_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISSteering_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EEndCarStop_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EJumpToStrategyFlow_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISCarDamageReset_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDDaySpeech_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISRoadNoise_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EStopObjectEffects_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ECameraShake_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EMomentStrm_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EExitEngagableTrigger_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ESetPlayerCarReset_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ECameraPhotoFinish_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ESpawnExplosion_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISCopCarDoors_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISFakeFar_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISHideCharacter_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EBailPursuit_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISDetail_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EEnableCollisionElement_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISCopLights_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISPixelate_ResolveEvent_Callback(void *staticData, const UGroup *context);
void E911Call_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISBrakelock_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EHideRaceOverMessage_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDynamicRegion_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ERandomEventList_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISReattach_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EBreakerStopCops_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ERequestEventInfoDialog_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EFireRandomTrigger_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISTimeOfDay_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISCarShake_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EForceCarStop_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EBecomeAiCar_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EAccelerate_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EShowRaceOverMessage_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISConstraint_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EEnableModeling_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EStopObjectEffect_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EReloadHud_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EResetSystem_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISWolrdGeometry_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISPlayEffect_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ESetSimRate_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EPlayRaceNIS_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISRain_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EAIEngineRev_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EReloadGame_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ESndGameState_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EFadeScreenNoLoadingBarOff_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ECinematicMoment_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ETriggerMomentNIS_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ESetCopAutoSpawnMode_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EShowSMS_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ECellCall_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EResetProps_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISAeroDynamics_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EWakeObject_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EAddSMS_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISScreenFlash_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISCarPitch_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EFireEventList_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ELoadLost_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISNukeSmack_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ETerminateMusic_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EBecomePlayerCar_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EKillObject_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EPlayObjectEffect_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDispIntroRace_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EFadeScreenNoLoadingBarOn_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EShockObject_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EShowMessageScreen_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EProcessStimulus_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISNitro_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EHidePart_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDebugPrint_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDestroyVehicle_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDisableTrigger_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISMotionBlur_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISNeutralRev_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EHideObject_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ERandomExplosion_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDebugScreenMessage_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EEnterEngagableTrigger_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ESetPlayerCollisionCache_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ESpawnFragment_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EEnterBin_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISCarRoll_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EShowMarketingScreen_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EReportMilestoneAtStake_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EFireTriggerSpeedCondition_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISOverlayMessage_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EBecomePursuitCar_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISVisualLook_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ETips_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EShowRaceCountdown_ResolveEvent_Callback(void *staticData, const UGroup *context);
void EDamageLights_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISLights_ResolveEvent_Callback(void *staticData, const UGroup *context);
void ENISWorldAnimTrigger_ResolveEvent_Callback(void *staticData, const UGroup *context);
int EScheduleEvent_MakeEvent_LuaBinding(lua_State *L);
int ENISStopEffects_MakeEvent_LuaBinding(lua_State *L);
int EAutoSave_MakeEvent_LuaBinding(lua_State *L);
int EProcessAreaStimulus_MakeEvent_LuaBinding(lua_State *L);
int EResetSequencer_MakeEvent_LuaBinding(lua_State *L);
int EResetPlayerCar_MakeEvent_LuaBinding(lua_State *L);
int ENISBurnout_MakeEvent_LuaBinding(lua_State *L);
int ENISFreeze_MakeEvent_LuaBinding(lua_State *L);
int EWorldMapOn_MakeEvent_LuaBinding(lua_State *L);
int EShowTimeExtension_MakeEvent_LuaBinding(lua_State *L);
int EPlayRaceMovie_MakeEvent_LuaBinding(lua_State *L);
int EDisablePursuitVehicle_MakeEvent_LuaBinding(lua_State *L);
int ENISDetach_MakeEvent_LuaBinding(lua_State *L);
int EKillJoint_MakeEvent_LuaBinding(lua_State *L);
int EPlayEndNIS_MakeEvent_LuaBinding(lua_State *L);
int EEnableTrigger_MakeEvent_LuaBinding(lua_State *L);
int ENISSteering_MakeEvent_LuaBinding(lua_State *L);
int EEndCarStop_MakeEvent_LuaBinding(lua_State *L);
int EJumpToStrategyFlow_MakeEvent_LuaBinding(lua_State *L);
int ENISCarDamageReset_MakeEvent_LuaBinding(lua_State *L);
int EDDaySpeech_MakeEvent_LuaBinding(lua_State *L);
int ENISRoadNoise_MakeEvent_LuaBinding(lua_State *L);
int EStopObjectEffects_MakeEvent_LuaBinding(lua_State *L);
int ECameraShake_MakeEvent_LuaBinding(lua_State *L);
int EMomentStrm_MakeEvent_LuaBinding(lua_State *L);
int EExitEngagableTrigger_MakeEvent_LuaBinding(lua_State *L);
int ESetPlayerCarReset_MakeEvent_LuaBinding(lua_State *L);
int ECameraPhotoFinish_MakeEvent_LuaBinding(lua_State *L);
int ESpawnExplosion_MakeEvent_LuaBinding(lua_State *L);
int ENISCopCarDoors_MakeEvent_LuaBinding(lua_State *L);
int ENISFakeFar_MakeEvent_LuaBinding(lua_State *L);
int ENISHideCharacter_MakeEvent_LuaBinding(lua_State *L);
int EBailPursuit_MakeEvent_LuaBinding(lua_State *L);
int ENISDetail_MakeEvent_LuaBinding(lua_State *L);
int EEnableCollisionElement_MakeEvent_LuaBinding(lua_State *L);
int ENISCopLights_MakeEvent_LuaBinding(lua_State *L);
int ENISPixelate_MakeEvent_LuaBinding(lua_State *L);
int E911Call_MakeEvent_LuaBinding(lua_State *L);
int ENISBrakelock_MakeEvent_LuaBinding(lua_State *L);
int EHideRaceOverMessage_MakeEvent_LuaBinding(lua_State *L);
int EDynamicRegion_MakeEvent_LuaBinding(lua_State *L);
int ERandomEventList_MakeEvent_LuaBinding(lua_State *L);
int ENISReattach_MakeEvent_LuaBinding(lua_State *L);
int EBreakerStopCops_MakeEvent_LuaBinding(lua_State *L);
int ERequestEventInfoDialog_MakeEvent_LuaBinding(lua_State *L);
int EFireRandomTrigger_MakeEvent_LuaBinding(lua_State *L);
int ENISTimeOfDay_MakeEvent_LuaBinding(lua_State *L);
int ENISCarShake_MakeEvent_LuaBinding(lua_State *L);
int EForceCarStop_MakeEvent_LuaBinding(lua_State *L);
int EBecomeAiCar_MakeEvent_LuaBinding(lua_State *L);
int EAccelerate_MakeEvent_LuaBinding(lua_State *L);
int EShowRaceOverMessage_MakeEvent_LuaBinding(lua_State *L);
int ENISConstraint_MakeEvent_LuaBinding(lua_State *L);
int EEnableModeling_MakeEvent_LuaBinding(lua_State *L);
int EStopObjectEffect_MakeEvent_LuaBinding(lua_State *L);
int EReloadHud_MakeEvent_LuaBinding(lua_State *L);
int EResetSystem_MakeEvent_LuaBinding(lua_State *L);
int ENISWolrdGeometry_MakeEvent_LuaBinding(lua_State *L);
int ENISPlayEffect_MakeEvent_LuaBinding(lua_State *L);
int ESetSimRate_MakeEvent_LuaBinding(lua_State *L);
int EPlayRaceNIS_MakeEvent_LuaBinding(lua_State *L);
int ENISRain_MakeEvent_LuaBinding(lua_State *L);
int EAIEngineRev_MakeEvent_LuaBinding(lua_State *L);
int EReloadGame_MakeEvent_LuaBinding(lua_State *L);
int ESndGameState_MakeEvent_LuaBinding(lua_State *L);
int EFadeScreenNoLoadingBarOff_MakeEvent_LuaBinding(lua_State *L);
int ECinematicMoment_MakeEvent_LuaBinding(lua_State *L);
int ETriggerMomentNIS_MakeEvent_LuaBinding(lua_State *L);
int ESetCopAutoSpawnMode_MakeEvent_LuaBinding(lua_State *L);
int EShowSMS_MakeEvent_LuaBinding(lua_State *L);
int ECellCall_MakeEvent_LuaBinding(lua_State *L);
int EResetProps_MakeEvent_LuaBinding(lua_State *L);
int ENISAeroDynamics_MakeEvent_LuaBinding(lua_State *L);
int EWakeObject_MakeEvent_LuaBinding(lua_State *L);
int EAddSMS_MakeEvent_LuaBinding(lua_State *L);
int ENISScreenFlash_MakeEvent_LuaBinding(lua_State *L);
int ENISCarPitch_MakeEvent_LuaBinding(lua_State *L);
int EFireEventList_MakeEvent_LuaBinding(lua_State *L);
int ELoadLost_MakeEvent_LuaBinding(lua_State *L);
int ENISNukeSmack_MakeEvent_LuaBinding(lua_State *L);
int ETerminateMusic_MakeEvent_LuaBinding(lua_State *L);
int EBecomePlayerCar_MakeEvent_LuaBinding(lua_State *L);
int EKillObject_MakeEvent_LuaBinding(lua_State *L);
int EPlayObjectEffect_MakeEvent_LuaBinding(lua_State *L);
int EDispIntroRace_MakeEvent_LuaBinding(lua_State *L);
int EFadeScreenNoLoadingBarOn_MakeEvent_LuaBinding(lua_State *L);
int EShockObject_MakeEvent_LuaBinding(lua_State *L);
int EShowMessageScreen_MakeEvent_LuaBinding(lua_State *L);
int EProcessStimulus_MakeEvent_LuaBinding(lua_State *L);
int ENISNitro_MakeEvent_LuaBinding(lua_State *L);
int EHidePart_MakeEvent_LuaBinding(lua_State *L);
int EDebugPrint_MakeEvent_LuaBinding(lua_State *L);
int EDestroyVehicle_MakeEvent_LuaBinding(lua_State *L);
int EDisableTrigger_MakeEvent_LuaBinding(lua_State *L);
int ENISMotionBlur_MakeEvent_LuaBinding(lua_State *L);
int ENISNeutralRev_MakeEvent_LuaBinding(lua_State *L);
int EHideObject_MakeEvent_LuaBinding(lua_State *L);
int ERandomExplosion_MakeEvent_LuaBinding(lua_State *L);
int EDebugScreenMessage_MakeEvent_LuaBinding(lua_State *L);
int EEnterEngagableTrigger_MakeEvent_LuaBinding(lua_State *L);
int ESetPlayerCollisionCache_MakeEvent_LuaBinding(lua_State *L);
int ESpawnFragment_MakeEvent_LuaBinding(lua_State *L);
int EEnterBin_MakeEvent_LuaBinding(lua_State *L);
int ENISCarRoll_MakeEvent_LuaBinding(lua_State *L);
int EShowMarketingScreen_MakeEvent_LuaBinding(lua_State *L);
int EReportMilestoneAtStake_MakeEvent_LuaBinding(lua_State *L);
int EFireTriggerSpeedCondition_MakeEvent_LuaBinding(lua_State *L);
int ENISOverlayMessage_MakeEvent_LuaBinding(lua_State *L);
int EBecomePursuitCar_MakeEvent_LuaBinding(lua_State *L);
int ENISVisualLook_MakeEvent_LuaBinding(lua_State *L);
int ETips_MakeEvent_LuaBinding(lua_State *L);
int EShowRaceCountdown_MakeEvent_LuaBinding(lua_State *L);
int EDamageLights_MakeEvent_LuaBinding(lua_State *L);
int ENISLights_MakeEvent_LuaBinding(lua_State *L);
int ENISWorldAnimTrigger_MakeEvent_LuaBinding(lua_State *L);

// Tablas generadas: el arbol binario empaquetado de eventos y sus tres tablas de callbacks.

static const unsigned int gEventKeyOrderTable[161] = {
    0x88281A34u, 0x5F8C3748u, 0xC0582354u, 0x2EC2700Du,
    0x762442F3u, 0xA20E0BF5u, 0xE998FAA4u, 0x16A9A8DFu,
    0x473C46C4u, 0x6A5265B2u, 0x7E045D1Fu, 0x8E73AD1Au,
    0xB0F01146u, 0xD44ED7B5u, 0xF4A034F9u, 0x09A11738u,
    0x257A10A8u, 0x35F52C01u, 0x4FB8AE5Cu, 0x663056FDu,
    0x7282975Du, 0x7C48A840u, 0x8610085Fu, 0x8CE4705Au,
    0x991DD250u, 0xA67E8286u, 0xB9A02FA8u, 0xC7389F39u,
    0xDEDC722Au, 0xF07A31ACu, 0xF82046BBu, 0x05F022EEu,
    0x0F9F8F67u, 0x2096C371u, 0x28F13822u, 0x31BBBAE2u,
    0x3F56A04Eu, 0x49D34FF1u, 0x534BEE88u, 0x643BCCA0u,
    0x6A15C6C7u, 0x6C3EC469u, 0x74E96511u, 0x77803D76u,
    0x7D8AD548u, 0x83E731C7u, 0x86D368CBu, 0x8BA7F63Du,
    0x8E396A7Du, 0x919AA63Fu, 0xA01A259Bu, 0xA537E70Fu,
    0xAA6E2CDBu, 0xB34E3015u, 0xBCF19D46u, 0xC34649C0u,
    0xCF01B27Eu, 0xDDA7829Bu, 0xE33BBBCAu, 0xEB626F77u,
    0xF122FCB9u, 0xF699BF04u, 0xFB3DAD79u, 0x0181B5C6u,
    0x079E93AEu, 0x0EE7940Fu, 0x148F983Eu, 0x1CCA882Cu,
    0x22AA36CEu, 0x25C6F1CBu, 0x2BA00F54u, 0x308383A4u,
    0x3338EFB8u, 0x396E0F75u, 0x42DFDB2Fu, 0x47C1D36Du,
    0x4DCA3442u, 0x509649B1u, 0x58AAE7A8u, 0x6209F42Du,
    0x65012BC5u, 0x66666DC8u, 0x6A465054u, 0x6AD404FFu,
    0x719CCA1Du, 0x73A12483u, 0x75B0D22Bu, 0x766C6A6Eu,
    0x7B129988u, 0x7D2B68BDu, 0x7DF7F613u, 0x8212EEB0u,
    0x83ED76FEu, 0x86D32FBBu, 0x87DB3D2Cu, 0x885F756Eu,
    0x8C424B17u, 0x8DA05B3Du, 0x8E60A642u, 0x8E8B60DEu,
    0x97EB86D4u, 0x9F79A4EDu, 0xA18E1896u, 0xA3CFBFECu,
    0xA629B821u, 0xA87E176Du, 0xAE37AF4Au, 0xB27B8608u,
    0xB680DFD9u, 0xBBE323C4u, 0xBD2F6FF1u, 0xC1AF4FDDu,
    0xC6D64D12u, 0xC945BC1Fu, 0xD1703F59u, 0xD84AB50Fu,
    0xDE59505Au, 0xE2A5B82Au, 0xE3C86F4Cu, 0xEB348F62u,
    0xEFFECDD6u, 0xF0D05F9Au, 0xF2D10992u, 0xF650FA09u,
    0xF6E5EBDAu, 0xFA777CFAu, 0xFC382D92u, 0x00EB198Bu,
    0x03419522u, 0x0713D2FAu, 0x08EEAA09u, 0x0A6B1ABDu,
    0x0F803EFEu, 0x10CCA629u, 0x1670C5CBu, 0x16AF830Du,
    0x1CEC643Eu, 0x216AC2FAu, 0x2554C48Au, 0x25B87D1Fu,
    0x26522D72u, 0x2AF7332Bu, 0x2D04B9E8u, 0x3081837Du,
    0x312E1E33u, 0x3223154Cu, 0x34E26D9Eu, 0x37A3EE72u,
    0x3B6225C9u, 0x41D652BBu, 0x45CFC754u, 0x479BEC5Cu,
    0x49A7AFEFu, 0x4C7A247Au, 0x4DD829EAu, 0x4FD8F6DCu,
    0x522D67DBu, 0x54E8FC2Bu, 0x5E565165u, 0x60B06742u,
    0x62F10875u,
};

static RegisterEvent::MakeEventCallback gMakeEventCallbacks[161] = {
    EScheduleEvent_MakeEvent_Callback,
    EFadeScreenOn_MakeEvent_Callback,
    ENISStopEffects_MakeEvent_Callback,
    EAutoSave_MakeEvent_Callback,
    EProcessAreaStimulus_MakeEvent_Callback,
    EResetSequencer_MakeEvent_Callback,
    ETireBlown_MakeEvent_Callback,
    EResetPlayerCar_MakeEvent_Callback,
    EGPSLost_MakeEvent_Callback,
    ENISBurnout_MakeEvent_Callback,
    ENISFreeze_MakeEvent_Callback,
    EWorldMapOn_MakeEvent_Callback,
    ERaceSheetOn_MakeEvent_Callback,
    EShowTimeExtension_MakeEvent_Callback,
    EPlayRaceMovie_MakeEvent_Callback,
    EDisablePursuitVehicle_MakeEvent_Callback,
    ENISDetach_MakeEvent_Callback,
    EKillJoint_MakeEvent_Callback,
    EPlayEndNIS_MakeEvent_Callback,
    EEnableTrigger_MakeEvent_Callback,
    ENISSteering_MakeEvent_Callback,
    EEndCarStop_MakeEvent_Callback,
    EJointDetached_MakeEvent_Callback,
    EJumpToStrategyFlow_MakeEvent_Callback,
    ENISCarDamageReset_MakeEvent_Callback,
    EDDaySpeech_MakeEvent_Callback,
    ENISRoadNoise_MakeEvent_Callback,
    EStopObjectEffects_MakeEvent_Callback,
    ECameraShake_MakeEvent_Callback,
    EMomentStrm_MakeEvent_Callback,
    EExitEngagableTrigger_MakeEvent_Callback,
    ESetPlayerCarReset_MakeEvent_Callback,
    EUnPause_MakeEvent_Callback,
    ECameraPhotoFinish_MakeEvent_Callback,
    ELoadingScreenOn_MakeEvent_Callback,
    ESpawnExplosion_MakeEvent_Callback,
    ENISCopCarDoors_MakeEvent_Callback,
    ENISFakeFar_MakeEvent_Callback,
    ENISHideCharacter_MakeEvent_Callback,
    EBailPursuit_MakeEvent_Callback,
    EShowMilestones_MakeEvent_Callback,
    EGPSFinished_MakeEvent_Callback,
    ENISDetail_MakeEvent_Callback,
    EFadeScreenOff_MakeEvent_Callback,
    EEnableCollisionElement_MakeEvent_Callback,
    ENISCopLights_MakeEvent_Callback,
    ENISPixelate_MakeEvent_Callback,
    E911Call_MakeEvent_Callback,
    EAudioSmackableTest_MakeEvent_Callback,
    EQuitToFE_MakeEvent_Callback,
    ENISBrakelock_MakeEvent_Callback,
    EHideRaceOverMessage_MakeEvent_Callback,
    EDynamicRegion_MakeEvent_Callback,
    ETirePunctured_MakeEvent_Callback,
    ERandomEventList_MakeEvent_Callback,
    EGTriggerInternal_MakeEvent_Callback,
    ENISReattach_MakeEvent_Callback,
    EBreakerStopCops_MakeEvent_Callback,
    ERequestEventInfoDialog_MakeEvent_Callback,
    ESpawnSmackable_MakeEvent_Callback,
    EFireRandomTrigger_MakeEvent_Callback,
    ENISTimeOfDay_MakeEvent_Callback,
    ENISCarShake_MakeEvent_Callback,
    EForceCarStop_MakeEvent_Callback,
    EBecomeAiCar_MakeEvent_Callback,
    ETuneVehicle_MakeEvent_Callback,
    EAccelerate_MakeEvent_Callback,
    EShowRaceOverMessage_MakeEvent_Callback,
    ENISConstraint_MakeEvent_Callback,
    EQuitDemo_MakeEvent_Callback,
    EEnableModeling_MakeEvent_Callback,
    ERestartRace_MakeEvent_Callback,
    EStopObjectEffect_MakeEvent_Callback,
    ELoadingScreenOff_MakeEvent_Callback,
    EReloadHud_MakeEvent_Callback,
    EPursuitBreaker_MakeEvent_Callback,
    EResetSystem_MakeEvent_Callback,
    ENISWolrdGeometry_MakeEvent_Callback,
    ENISPlayEffect_MakeEvent_Callback,
    ESetSimRate_MakeEvent_Callback,
    NULL,
    EPlayRaceNIS_MakeEvent_Callback,
    ENISRain_MakeEvent_Callback,
    EAIEngineRev_MakeEvent_Callback,
    EReloadGame_MakeEvent_Callback,
    EPerfectLaunch_MakeEvent_Callback,
    ESndGameState_MakeEvent_Callback,
    EPerfectShift_MakeEvent_Callback,
    EFadeScreenNoLoadingBarOff_MakeEvent_Callback,
    ECinematicMoment_MakeEvent_Callback,
    ETriggerMomentNIS_MakeEvent_Callback,
    ESetCopAutoSpawnMode_MakeEvent_Callback,
    EAwardUpgrade_MakeEvent_Callback,
    EShowSMS_MakeEvent_Callback,
    EChangeState_MakeEvent_Callback,
    ECellCall_MakeEvent_Callback,
    EResetProps_MakeEvent_Callback,
    ENISAeroDynamics_MakeEvent_Callback,
    EWakeObject_MakeEvent_Callback,
    EEnableAIPhysics_MakeEvent_Callback,
    EAddSMS_MakeEvent_Callback,
    ENISScreenFlash_MakeEvent_Callback,
    ENISCarPitch_MakeEvent_Callback,
    EFireEventList_MakeEvent_Callback,
    ELoadLost_MakeEvent_Callback,
    ENISNukeSmack_MakeEvent_Callback,
    ETerminateMusic_MakeEvent_Callback,
    EEngineBlown_MakeEvent_Callback,
    EPause_MakeEvent_Callback,
    EBecomePlayerCar_MakeEvent_Callback,
    EKillObject_MakeEvent_Callback,
    EPlayObjectEffect_MakeEvent_Callback,
    EDispIntroRace_MakeEvent_Callback,
    EScheduleEventUpdate_MakeEvent_Callback,
    EFadeScreenNoLoadingBarOn_MakeEvent_Callback,
    EShockObject_MakeEvent_Callback,
    EShowMessageScreen_MakeEvent_Callback,
    EReportInfraction_MakeEvent_Callback,
    EProcessStimulus_MakeEvent_Callback,
    ENISNitro_MakeEvent_Callback,
    EHidePart_MakeEvent_Callback,
    EDebugPrint_MakeEvent_Callback,
    EAudioWorldTest_MakeEvent_Callback,
    EDestroyVehicle_MakeEvent_Callback,
    EMissShift_MakeEvent_Callback,
    EDisableTrigger_MakeEvent_Callback,
    ENISMotionBlur_MakeEvent_Callback,
    ENISNeutralRev_MakeEvent_Callback,
    EPlayerTriggeredNOS_MakeEvent_Callback,
    EHideObject_MakeEvent_Callback,
    EVehicleDestroyed_MakeEvent_Callback,
    ERandomExplosion_MakeEvent_Callback,
    EAudioRigidBodyTest_MakeEvent_Callback,
    EDebugScreenMessage_MakeEvent_Callback,
    EEnterEngagableTrigger_MakeEvent_Callback,
    ESetPlayerCollisionCache_MakeEvent_Callback,
    ERaceSheetOff_MakeEvent_Callback,
    ECommitRenderAssets_MakeEvent_Callback,
    ESpawnFragment_MakeEvent_Callback,
    EVehicleReset_MakeEvent_Callback,
    EDeliverMessage_MakeEvent_Callback,
    EEnterBin_MakeEvent_Callback,
    ECommitAudioAssets_MakeEvent_Callback,
    ENISCarRoll_MakeEvent_Callback,
    EShowMarketingScreen_MakeEvent_Callback,
    EReportMilestoneAtStake_MakeEvent_Callback,
    EFireTriggerSpeedCondition_MakeEvent_Callback,
    ENISOverlayMessage_MakeEvent_Callback,
    EBecomePursuitCar_MakeEvent_Callback,
    ENISVisualLook_MakeEvent_Callback,
    EPlayerShift_MakeEvent_Callback,
    ETips_MakeEvent_Callback,
    EWorldMapOff_MakeEvent_Callback,
    EShowRaceCountdown_MakeEvent_Callback,
    EKnockoutRacer_MakeEvent_Callback,
    EDamageLights_MakeEvent_Callback,
    ESimulate_MakeEvent_Callback,
    EPlayerAirborne_MakeEvent_Callback,
    ENISLights_MakeEvent_Callback,
    EShowResults_MakeEvent_Callback,
    ENISWorldAnimTrigger_MakeEvent_Callback,
};

static RegisterEvent::ResolveEventCallback gResolveEventCallbacks[161] = {
    EScheduleEvent_ResolveEvent_Callback,
    NULL,
    ENISStopEffects_ResolveEvent_Callback,
    EAutoSave_ResolveEvent_Callback,
    EProcessAreaStimulus_ResolveEvent_Callback,
    EResetSequencer_ResolveEvent_Callback,
    NULL,
    EResetPlayerCar_ResolveEvent_Callback,
    NULL,
    ENISBurnout_ResolveEvent_Callback,
    ENISFreeze_ResolveEvent_Callback,
    EWorldMapOn_ResolveEvent_Callback,
    NULL,
    EShowTimeExtension_ResolveEvent_Callback,
    EPlayRaceMovie_ResolveEvent_Callback,
    EDisablePursuitVehicle_ResolveEvent_Callback,
    ENISDetach_ResolveEvent_Callback,
    EKillJoint_ResolveEvent_Callback,
    EPlayEndNIS_ResolveEvent_Callback,
    EEnableTrigger_ResolveEvent_Callback,
    ENISSteering_ResolveEvent_Callback,
    EEndCarStop_ResolveEvent_Callback,
    NULL,
    EJumpToStrategyFlow_ResolveEvent_Callback,
    ENISCarDamageReset_ResolveEvent_Callback,
    EDDaySpeech_ResolveEvent_Callback,
    ENISRoadNoise_ResolveEvent_Callback,
    EStopObjectEffects_ResolveEvent_Callback,
    ECameraShake_ResolveEvent_Callback,
    EMomentStrm_ResolveEvent_Callback,
    EExitEngagableTrigger_ResolveEvent_Callback,
    ESetPlayerCarReset_ResolveEvent_Callback,
    NULL,
    ECameraPhotoFinish_ResolveEvent_Callback,
    NULL,
    ESpawnExplosion_ResolveEvent_Callback,
    ENISCopCarDoors_ResolveEvent_Callback,
    ENISFakeFar_ResolveEvent_Callback,
    ENISHideCharacter_ResolveEvent_Callback,
    EBailPursuit_ResolveEvent_Callback,
    NULL,
    NULL,
    ENISDetail_ResolveEvent_Callback,
    NULL,
    EEnableCollisionElement_ResolveEvent_Callback,
    ENISCopLights_ResolveEvent_Callback,
    ENISPixelate_ResolveEvent_Callback,
    E911Call_ResolveEvent_Callback,
    NULL,
    NULL,
    ENISBrakelock_ResolveEvent_Callback,
    EHideRaceOverMessage_ResolveEvent_Callback,
    EDynamicRegion_ResolveEvent_Callback,
    NULL,
    ERandomEventList_ResolveEvent_Callback,
    NULL,
    ENISReattach_ResolveEvent_Callback,
    EBreakerStopCops_ResolveEvent_Callback,
    ERequestEventInfoDialog_ResolveEvent_Callback,
    NULL,
    EFireRandomTrigger_ResolveEvent_Callback,
    ENISTimeOfDay_ResolveEvent_Callback,
    ENISCarShake_ResolveEvent_Callback,
    EForceCarStop_ResolveEvent_Callback,
    EBecomeAiCar_ResolveEvent_Callback,
    NULL,
    EAccelerate_ResolveEvent_Callback,
    EShowRaceOverMessage_ResolveEvent_Callback,
    ENISConstraint_ResolveEvent_Callback,
    NULL,
    EEnableModeling_ResolveEvent_Callback,
    NULL,
    EStopObjectEffect_ResolveEvent_Callback,
    NULL,
    EReloadHud_ResolveEvent_Callback,
    NULL,
    EResetSystem_ResolveEvent_Callback,
    ENISWolrdGeometry_ResolveEvent_Callback,
    ENISPlayEffect_ResolveEvent_Callback,
    ESetSimRate_ResolveEvent_Callback,
    NULL,
    EPlayRaceNIS_ResolveEvent_Callback,
    ENISRain_ResolveEvent_Callback,
    EAIEngineRev_ResolveEvent_Callback,
    EReloadGame_ResolveEvent_Callback,
    NULL,
    ESndGameState_ResolveEvent_Callback,
    NULL,
    EFadeScreenNoLoadingBarOff_ResolveEvent_Callback,
    ECinematicMoment_ResolveEvent_Callback,
    ETriggerMomentNIS_ResolveEvent_Callback,
    ESetCopAutoSpawnMode_ResolveEvent_Callback,
    NULL,
    EShowSMS_ResolveEvent_Callback,
    NULL,
    ECellCall_ResolveEvent_Callback,
    EResetProps_ResolveEvent_Callback,
    ENISAeroDynamics_ResolveEvent_Callback,
    EWakeObject_ResolveEvent_Callback,
    NULL,
    EAddSMS_ResolveEvent_Callback,
    ENISScreenFlash_ResolveEvent_Callback,
    ENISCarPitch_ResolveEvent_Callback,
    EFireEventList_ResolveEvent_Callback,
    ELoadLost_ResolveEvent_Callback,
    ENISNukeSmack_ResolveEvent_Callback,
    ETerminateMusic_ResolveEvent_Callback,
    NULL,
    NULL,
    EBecomePlayerCar_ResolveEvent_Callback,
    EKillObject_ResolveEvent_Callback,
    EPlayObjectEffect_ResolveEvent_Callback,
    EDispIntroRace_ResolveEvent_Callback,
    NULL,
    EFadeScreenNoLoadingBarOn_ResolveEvent_Callback,
    EShockObject_ResolveEvent_Callback,
    EShowMessageScreen_ResolveEvent_Callback,
    NULL,
    EProcessStimulus_ResolveEvent_Callback,
    ENISNitro_ResolveEvent_Callback,
    EHidePart_ResolveEvent_Callback,
    EDebugPrint_ResolveEvent_Callback,
    NULL,
    EDestroyVehicle_ResolveEvent_Callback,
    NULL,
    EDisableTrigger_ResolveEvent_Callback,
    ENISMotionBlur_ResolveEvent_Callback,
    ENISNeutralRev_ResolveEvent_Callback,
    NULL,
    EHideObject_ResolveEvent_Callback,
    NULL,
    ERandomExplosion_ResolveEvent_Callback,
    NULL,
    EDebugScreenMessage_ResolveEvent_Callback,
    EEnterEngagableTrigger_ResolveEvent_Callback,
    ESetPlayerCollisionCache_ResolveEvent_Callback,
    NULL,
    NULL,
    ESpawnFragment_ResolveEvent_Callback,
    NULL,
    NULL,
    EEnterBin_ResolveEvent_Callback,
    NULL,
    ENISCarRoll_ResolveEvent_Callback,
    EShowMarketingScreen_ResolveEvent_Callback,
    EReportMilestoneAtStake_ResolveEvent_Callback,
    EFireTriggerSpeedCondition_ResolveEvent_Callback,
    ENISOverlayMessage_ResolveEvent_Callback,
    EBecomePursuitCar_ResolveEvent_Callback,
    ENISVisualLook_ResolveEvent_Callback,
    NULL,
    ETips_ResolveEvent_Callback,
    NULL,
    EShowRaceCountdown_ResolveEvent_Callback,
    NULL,
    EDamageLights_ResolveEvent_Callback,
    NULL,
    NULL,
    ENISLights_ResolveEvent_Callback,
    NULL,
    ENISWorldAnimTrigger_ResolveEvent_Callback,
};

static RegisterEvent::LuaBinding gEventLuaBindings[161] = {
    EScheduleEvent_MakeEvent_LuaBinding,
    NULL,
    ENISStopEffects_MakeEvent_LuaBinding,
    EAutoSave_MakeEvent_LuaBinding,
    EProcessAreaStimulus_MakeEvent_LuaBinding,
    EResetSequencer_MakeEvent_LuaBinding,
    NULL,
    EResetPlayerCar_MakeEvent_LuaBinding,
    NULL,
    ENISBurnout_MakeEvent_LuaBinding,
    ENISFreeze_MakeEvent_LuaBinding,
    EWorldMapOn_MakeEvent_LuaBinding,
    NULL,
    EShowTimeExtension_MakeEvent_LuaBinding,
    EPlayRaceMovie_MakeEvent_LuaBinding,
    EDisablePursuitVehicle_MakeEvent_LuaBinding,
    ENISDetach_MakeEvent_LuaBinding,
    EKillJoint_MakeEvent_LuaBinding,
    EPlayEndNIS_MakeEvent_LuaBinding,
    EEnableTrigger_MakeEvent_LuaBinding,
    ENISSteering_MakeEvent_LuaBinding,
    EEndCarStop_MakeEvent_LuaBinding,
    NULL,
    EJumpToStrategyFlow_MakeEvent_LuaBinding,
    ENISCarDamageReset_MakeEvent_LuaBinding,
    EDDaySpeech_MakeEvent_LuaBinding,
    ENISRoadNoise_MakeEvent_LuaBinding,
    EStopObjectEffects_MakeEvent_LuaBinding,
    ECameraShake_MakeEvent_LuaBinding,
    EMomentStrm_MakeEvent_LuaBinding,
    EExitEngagableTrigger_MakeEvent_LuaBinding,
    ESetPlayerCarReset_MakeEvent_LuaBinding,
    NULL,
    ECameraPhotoFinish_MakeEvent_LuaBinding,
    NULL,
    ESpawnExplosion_MakeEvent_LuaBinding,
    ENISCopCarDoors_MakeEvent_LuaBinding,
    ENISFakeFar_MakeEvent_LuaBinding,
    ENISHideCharacter_MakeEvent_LuaBinding,
    EBailPursuit_MakeEvent_LuaBinding,
    NULL,
    NULL,
    ENISDetail_MakeEvent_LuaBinding,
    NULL,
    EEnableCollisionElement_MakeEvent_LuaBinding,
    ENISCopLights_MakeEvent_LuaBinding,
    ENISPixelate_MakeEvent_LuaBinding,
    E911Call_MakeEvent_LuaBinding,
    NULL,
    NULL,
    ENISBrakelock_MakeEvent_LuaBinding,
    EHideRaceOverMessage_MakeEvent_LuaBinding,
    EDynamicRegion_MakeEvent_LuaBinding,
    NULL,
    ERandomEventList_MakeEvent_LuaBinding,
    NULL,
    ENISReattach_MakeEvent_LuaBinding,
    EBreakerStopCops_MakeEvent_LuaBinding,
    ERequestEventInfoDialog_MakeEvent_LuaBinding,
    NULL,
    EFireRandomTrigger_MakeEvent_LuaBinding,
    ENISTimeOfDay_MakeEvent_LuaBinding,
    ENISCarShake_MakeEvent_LuaBinding,
    EForceCarStop_MakeEvent_LuaBinding,
    EBecomeAiCar_MakeEvent_LuaBinding,
    NULL,
    EAccelerate_MakeEvent_LuaBinding,
    EShowRaceOverMessage_MakeEvent_LuaBinding,
    ENISConstraint_MakeEvent_LuaBinding,
    NULL,
    EEnableModeling_MakeEvent_LuaBinding,
    NULL,
    EStopObjectEffect_MakeEvent_LuaBinding,
    NULL,
    EReloadHud_MakeEvent_LuaBinding,
    NULL,
    EResetSystem_MakeEvent_LuaBinding,
    ENISWolrdGeometry_MakeEvent_LuaBinding,
    ENISPlayEffect_MakeEvent_LuaBinding,
    ESetSimRate_MakeEvent_LuaBinding,
    NULL,
    EPlayRaceNIS_MakeEvent_LuaBinding,
    ENISRain_MakeEvent_LuaBinding,
    EAIEngineRev_MakeEvent_LuaBinding,
    EReloadGame_MakeEvent_LuaBinding,
    NULL,
    ESndGameState_MakeEvent_LuaBinding,
    NULL,
    EFadeScreenNoLoadingBarOff_MakeEvent_LuaBinding,
    ECinematicMoment_MakeEvent_LuaBinding,
    ETriggerMomentNIS_MakeEvent_LuaBinding,
    ESetCopAutoSpawnMode_MakeEvent_LuaBinding,
    NULL,
    EShowSMS_MakeEvent_LuaBinding,
    NULL,
    ECellCall_MakeEvent_LuaBinding,
    EResetProps_MakeEvent_LuaBinding,
    ENISAeroDynamics_MakeEvent_LuaBinding,
    EWakeObject_MakeEvent_LuaBinding,
    NULL,
    EAddSMS_MakeEvent_LuaBinding,
    ENISScreenFlash_MakeEvent_LuaBinding,
    ENISCarPitch_MakeEvent_LuaBinding,
    EFireEventList_MakeEvent_LuaBinding,
    ELoadLost_MakeEvent_LuaBinding,
    ENISNukeSmack_MakeEvent_LuaBinding,
    ETerminateMusic_MakeEvent_LuaBinding,
    NULL,
    NULL,
    EBecomePlayerCar_MakeEvent_LuaBinding,
    EKillObject_MakeEvent_LuaBinding,
    EPlayObjectEffect_MakeEvent_LuaBinding,
    EDispIntroRace_MakeEvent_LuaBinding,
    NULL,
    EFadeScreenNoLoadingBarOn_MakeEvent_LuaBinding,
    EShockObject_MakeEvent_LuaBinding,
    EShowMessageScreen_MakeEvent_LuaBinding,
    NULL,
    EProcessStimulus_MakeEvent_LuaBinding,
    ENISNitro_MakeEvent_LuaBinding,
    EHidePart_MakeEvent_LuaBinding,
    EDebugPrint_MakeEvent_LuaBinding,
    NULL,
    EDestroyVehicle_MakeEvent_LuaBinding,
    NULL,
    EDisableTrigger_MakeEvent_LuaBinding,
    ENISMotionBlur_MakeEvent_LuaBinding,
    ENISNeutralRev_MakeEvent_LuaBinding,
    NULL,
    EHideObject_MakeEvent_LuaBinding,
    NULL,
    ERandomExplosion_MakeEvent_LuaBinding,
    NULL,
    EDebugScreenMessage_MakeEvent_LuaBinding,
    EEnterEngagableTrigger_MakeEvent_LuaBinding,
    ESetPlayerCollisionCache_MakeEvent_LuaBinding,
    NULL,
    NULL,
    ESpawnFragment_MakeEvent_LuaBinding,
    NULL,
    NULL,
    EEnterBin_MakeEvent_LuaBinding,
    NULL,
    ENISCarRoll_MakeEvent_LuaBinding,
    EShowMarketingScreen_MakeEvent_LuaBinding,
    EReportMilestoneAtStake_MakeEvent_LuaBinding,
    EFireTriggerSpeedCondition_MakeEvent_LuaBinding,
    ENISOverlayMessage_MakeEvent_LuaBinding,
    EBecomePursuitCar_MakeEvent_LuaBinding,
    ENISVisualLook_MakeEvent_LuaBinding,
    NULL,
    ETips_MakeEvent_LuaBinding,
    NULL,
    EShowRaceCountdown_MakeEvent_LuaBinding,
    NULL,
    EDamageLights_MakeEvent_LuaBinding,
    NULL,
    NULL,
    ENISLights_MakeEvent_LuaBinding,
    NULL,
    ENISWorldAnimTrigger_MakeEvent_LuaBinding,
};

RegisterEvent::MakeEventCallback RegisterEvent::LookupEvent(unsigned int tag) {
    unsigned int index = SearchPackedBinaryTree<const unsigned int, unsigned int>(161, gEventKeyOrderTable, tag);

    if (index > 160) {
        return NULL;
    }

    return gMakeEventCallbacks[index];
}

RegisterEvent::ResolveEventCallback RegisterEvent::ResolveEvent(unsigned int tag) {
    unsigned int index = SearchPackedBinaryTree<const unsigned int, unsigned int>(161, gEventKeyOrderTable, tag);

    if (index > 160) {
        return NULL;
    }

    return gResolveEventCallbacks[index];
}

RegisterEvent::LuaBinding RegisterEvent::GetLuaBinding(unsigned int tag) {
    unsigned int index = SearchPackedBinaryTree<const unsigned int, unsigned int>(161, gEventKeyOrderTable, tag);

    if (index > 160) {
        return NULL;
    }

    return gEventLuaBindings[index];
}



void EventManager::BindMessagesToLua(UTL::Std::vector<Hermes::HHANDLER, Hermes::_type_ID_HermesHandlerVector> &handlers) {
    Hermes::HHANDLER bindings[] = {
        Hermes::Handler::Create(MEnterFreeRoam::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyRaceTimeSecTick::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyMilestoneProgress::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyPlayerRep::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyMilestoneReached::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MTriggerExit::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MICECameraFinished::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyCellCallComplete::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MEnteringGameplay::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyEngineBlown::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MJackKnife::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyMusicFlow::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MTriggerInside::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MPerpBusted::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MUnspawnCop::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyFinished::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyChallengePassed::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MAudioReflection::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MJumpCut::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MFlowReadyForOutro::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyCellCallStarted::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNISComplete::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MQuitToFrontEnd::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyMovieFinished::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyRaceTime::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MAcceptEnterCareerEvent::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyRaceAbandoned::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MSetCopsEnabled::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MEnterSafeHouse::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MEnterRaceOverFlow::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MAIEngineRev::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyVehicleDestroyed::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyRacePlacement::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifySpeechStatus::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MReqRoadBlock::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifySimTick::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MLoadingComplete::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MBreakerStopCops::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MSpawnTraffic::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MTriggerEnter::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyMessageDone::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MStateEnter::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifySpeedTrap::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MEnterPostRaceFlow::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MRestartRace::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MSetTrafficSpeed::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MControlPathfinder::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MDeclineEnterCareerEvent::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MPerpEscaped::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MPursuitOver::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MStateExit::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyRaceTimeExpired::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyOnlineRaceOver::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MGamePlayMoment::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MPlayerEnterPursuit::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MPursuitBreaker::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MSetCopAutoSpawnMode::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MMiscSound::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MCountdownDone::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyPursuitLength::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MForcePursuitStart::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MSpawnCop::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MReqBackup::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyTimer::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
        Hermes::Handler::Create(MNotifyKnockedOut::HandleMessage_LuaBinding, UCrc32(0x20D60DBF), 0),
    };

    handlers.reserve(sizeof(bindings) / sizeof(bindings[0]));

    for (unsigned int onBinding = 0; onBinding < sizeof(bindings) / sizeof(bindings[0]); onBinding++) {
        handlers.push_back(bindings[onBinding]);
    }
}
