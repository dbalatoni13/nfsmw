#include "EAXCharacter.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

namespace Csis {
InterfaceId AcknowledgeId = {"Acknowledge", (short)0x5BA7, (short)0x2E86};
FunctionHandle gAcknowledgeHandle;
InterfaceId Setup_SpotterId = {"Setup_Spotter", (short)0x5BA7, (short)0x17C9};
FunctionHandle gSetup_SpotterHandle;
InterfaceId Setup_SpotterWantedId = {"Setup_SpotterWanted", (short)0x5BA7, (short)0x47C7};
FunctionHandle gSetup_SpotterWantedHandle;
InterfaceId Setup_SpotterReplyId = {"Setup_SpotterReply", (short)0x5BA7, (short)0x0369};
FunctionHandle gSetup_SpotterReplyHandle;
InterfaceId Setup_AttmptVehStpId = {"Setup_AttmptVehStp", (short)0x5BA7, (short)0x668C};
FunctionHandle gSetup_AttmptVehStpHandle;
InterfaceId Setup_DispGoAheadId = {"Setup_DispGoAhead", (short)0x5BA7, (short)0x17F9};
FunctionHandle gSetup_DispGoAheadHandle;
InterfaceId Setup_PrimaryEngageId = {"Setup_PrimaryEngage", (short)0x5BA7, (short)0x0A49};
FunctionHandle gSetup_PrimaryEngageHandle;
InterfaceId Setup_InitPursuitId = {"Setup_InitPursuit", (short)0x5BA7, (short)0x1418};
FunctionHandle gSetup_InitPursuitHandle;
InterfaceId Setup_SuspectConfirmedId = {"Setup_SuspectConfirmed", (short)0x5BA7, (short)0x4CBF};
FunctionHandle gSetup_SuspectConfirmedHandle;
InterfaceId Setup_ReInitPursuitId = {"Setup_ReInitPursuit", (short)0x5BA7, (short)0x4F90};
FunctionHandle gSetup_ReInitPursuitHandle;
InterfaceId Setup_VehicleReportId = {"Setup_VehicleReport", (short)0x5BA7, (short)0x466D};
FunctionHandle gSetup_VehicleReportHandle;
InterfaceId Setup_VehicleReportTagId = {"Setup_VehicleReportTag", (short)0x5BA7, (short)0x5E34};
FunctionHandle gSetup_VehicleReportTagHandle;
InterfaceId Setup_DispVehDescripId = {"Setup_DispVehDescrip", (short)0x5BA7, (short)0x2FDF};
FunctionHandle gSetup_DispVehDescripHandle;
InterfaceId Setup_DispVehDescripVinylsId = {"Setup_DispVehDescripVinyls", (short)0x5BA7, (short)0x73EC};
FunctionHandle gSetup_DispVehDescripVinylsHandle;
InterfaceId Setup_DispNoVehDescripId = {"Setup_DispNoVehDescrip", (short)0x5BA7, (short)0x6555};
FunctionHandle gSetup_DispNoVehDescripHandle;
InterfaceId Setup_DispCustPaintId = {"Setup_DispCustPaint", (short)0x5BA7, (short)0x05C4};
FunctionHandle gSetup_DispCustPaintHandle;
InterfaceId Setup_MoreDetailsId = {"Setup_MoreDetails", (short)0x5BA7, (short)0x40A4};
FunctionHandle gSetup_MoreDetailsHandle;
InterfaceId Setup_LocationReportId = {"Setup_LocationReport", (short)0x5BA7, (short)0x7048};
FunctionHandle gSetup_LocationReportHandle;
InterfaceId Setup_BullhornPrefixId = {"Setup_BullhornPrefix", (short)0x5BA7, (short)0x5893};
FunctionHandle gSetup_BullhornPrefixHandle;
InterfaceId Setup_BullhornId = {"Setup_Bullhorn", (short)0x5BA7, (short)0x5E0A};
FunctionHandle gSetup_BullhornHandle;
InterfaceId Setup_SelfStrategyId = {"Setup_SelfStrategy", (short)0x5BA7, (short)0x3A53};
FunctionHandle gSetup_SelfStrategyHandle;
InterfaceId Setup_InitialCallForBUId = {"Setup_InitialCallForBU", (short)0x5BA7, (short)0x154B};
FunctionHandle gSetup_InitialCallForBUHandle;
InterfaceId Setup_InitialCallForBU_MSId = {"Setup_InitialCallForBU_MS", (short)0x5BA7, (short)0x23E5};
FunctionHandle gSetup_InitialCallForBU_MSHandle;
InterfaceId Backup_CallForBUId = {"Backup_CallForBU", (short)0x5BA7, (short)0x398D};
FunctionHandle gBackup_CallForBUHandle;
InterfaceId Backup_UnitBUReplyId = {"Backup_UnitBUReply", (short)0x5BA7, (short)0x05D5};
FunctionHandle gBackup_UnitBUReplyHandle;
InterfaceId Backup_DispBackupReplyId = {"Backup_DispBackupReply", (short)0x5BA7, (short)0x5C2F};
FunctionHandle gBackup_DispBackupReplyHandle;
InterfaceId Backup_CallForSwarmingId = {"Backup_CallForSwarming", (short)0x5BA7, (short)0x5B23};
FunctionHandle gBackup_CallForSwarmingHandle;
InterfaceId Backup_DispBUETAId = {"Backup_DispBUETA", (short)0x5BA7, (short)0x3C9E};
FunctionHandle gBackup_DispBUETAHandle;
InterfaceId Backup_DispHeliBUETAId = {"Backup_DispHeliBUETA", (short)0x5BA7, (short)0x028D};
FunctionHandle gBackup_DispHeliBUETAHandle;
InterfaceId Backup_BUReminderId = {"Backup_BUReminder", (short)0x5BA7, (short)0x11A9};
FunctionHandle gBackup_BUReminderHandle;
InterfaceId Backup_NegativeBUReplyId = {"Backup_NegativeBUReply", (short)0x5BA7, (short)0x29E8};
FunctionHandle gBackup_NegativeBUReplyHandle;
InterfaceId Backup_DispBackupUpdateId = {"Backup_DispBackupUpdate", (short)0x5BA7, (short)0x70B2};
FunctionHandle gBackup_DispBackupUpdateHandle;
InterfaceId Backup_BUArrivesId = {"Backup_BUArrives", (short)0x5BA7, (short)0x4F76};
FunctionHandle gBackup_BUArrivesHandle;
InterfaceId StaticRoadblock_CallForRBId = {"StaticRoadblock_CallForRB", (short)0x5BA7, (short)0x7D0E};
FunctionHandle gStaticRoadblock_CallForRBHandle;
InterfaceId StaticRoadblock_RBReminderId = {"StaticRoadblock_RBReminder", (short)0x5BA7, (short)0x45D5};
FunctionHandle gStaticRoadblock_RBReminderHandle;
InterfaceId StaticRoadblock_NegativeRBReplyId = {"StaticRoadblock_NegativeRBReply", (short)0x5BA7, (short)0x0955};
FunctionHandle gStaticRoadblock_NegativeRBReplyHandle;
InterfaceId StaticRoadblock_DispRBReplyId = {"StaticRoadblock_DispRBReply", (short)0x5BA7, (short)0x1D9A};
FunctionHandle gStaticRoadblock_DispRBReplyHandle;
InterfaceId StaticRoadblock_DispRBUpdateId = {"StaticRoadblock_DispRBUpdate", (short)0x5BA7, (short)0x7B7F};
FunctionHandle gStaticRoadblock_DispRBUpdateHandle;
InterfaceId StaticRoadblock_PursuitApproachingId = {"StaticRoadblock_PursuitApproaching", (short)0x5BA7, (short)0x58B5};
FunctionHandle gStaticRoadblock_PursuitApproachingHandle;
InterfaceId StaticRoadblock_RBApproachId = {"StaticRoadblock_RBApproach", (short)0x5BA7, (short)0x6AF1};
FunctionHandle gStaticRoadblock_RBApproachHandle;
InterfaceId StaticRoadblock_RBEngageId = {"StaticRoadblock_RBEngage", (short)0x5BA7, (short)0x3E19};
FunctionHandle gStaticRoadblock_RBEngageHandle;
InterfaceId StaticRoadblock_RBAvertedId = {"StaticRoadblock_RBAverted", (short)0x5BA7, (short)0x5E18};
FunctionHandle gStaticRoadblock_RBAvertedHandle;
InterfaceId StaticRoadblock_CallForRB_subId = {"StaticRoadblock_CallForRB_sub", (short)0x5BA7, (short)0x58FF};
FunctionHandle gStaticRoadblock_CallForRB_subHandle;
InterfaceId StaticRoadblock_DispSubRBId = {"StaticRoadblock_DispSubRB", (short)0x5BA7, (short)0x1CEE};
FunctionHandle gStaticRoadblock_DispSubRBHandle;
InterfaceId Projectile_CallForSafetyId = {"Projectile_CallForSafety", (short)0x5BA7, (short)0x42C8};
FunctionHandle gProjectile_CallForSafetyHandle;
InterfaceId Projectile_ProjectileLaunchId = {"Projectile_ProjectileLaunch", (short)0x5BA7, (short)0x0AA7};
FunctionHandle gProjectile_ProjectileLaunchHandle;
InterfaceId Projectile_ProjectileHitId = {"Projectile_ProjectileHit", (short)0x5BA7, (short)0x6BED};
FunctionHandle gProjectile_ProjectileHitHandle;
InterfaceId Projectile_ProjectileMissId = {"Projectile_ProjectileMiss", (short)0x5BA7, (short)0x1DF2};
FunctionHandle gProjectile_ProjectileMissHandle;
InterfaceId RollingStrategy_InitStrategyId = {"RollingStrategy_InitStrategy", (short)0x5BA7, (short)0x3D7C};
FunctionHandle gRollingStrategy_InitStrategyHandle;
InterfaceId RollingStrategy_CallToPositionId = {"RollingStrategy_CallToPosition", (short)0x5BA7, (short)0x2BBC};
FunctionHandle gRollingStrategy_CallToPositionHandle;
InterfaceId RollingStrategy_CallToPositionRemId = {"RollingStrategy_CallToPositionRem", (short)0x5BA7, (short)0x4112};
FunctionHandle gRollingStrategy_CallToPositionRemHandle;
InterfaceId RollingStrategy_StrategyExecuteId = {"RollingStrategy_StrategyExecute", (short)0x5BA7, (short)0x6CB7};
FunctionHandle gRollingStrategy_StrategyExecuteHandle;
InterfaceId Outcome_AnticipateFailId = {"Outcome_AnticipateFail", (short)0x5BA7, (short)0x0AF9};
FunctionHandle gOutcome_AnticipateFailHandle;
InterfaceId Outcome_AnticipateSuccessId = {"Outcome_AnticipateSuccess", (short)0x5BA7, (short)0x4116};
FunctionHandle gOutcome_AnticipateSuccessHandle;
InterfaceId Outcome_OutcomeFailId = {"Outcome_OutcomeFail", (short)0x5BA7, (short)0x72D3};
FunctionHandle gOutcome_OutcomeFailHandle;
InterfaceId Outcome_StrategyResetId = {"Outcome_StrategyReset", (short)0x5BA7, (short)0x4EA0};
FunctionHandle gOutcome_StrategyResetHandle;
InterfaceId Arrest_BullhornArrestId = {"Arrest_BullhornArrest", (short)0x5BA7, (short)0x1AB5};
FunctionHandle gArrest_BullhornArrestHandle;
InterfaceId Arrest_ArrestId = {"Arrest_Arrest", (short)0x5BA7, (short)0x7925};
FunctionHandle gArrest_ArrestHandle;
InterfaceId Arrest_DispArrestReplyId = {"Arrest_DispArrestReply", (short)0x5BA7, (short)0x6C90};
FunctionHandle gArrest_DispArrestReplyHandle;
InterfaceId AnytimeEvents_CollisionWorldId = {"AnytimeEvents_CollisionWorld", (short)0x5BA7, (short)0x6D79};
FunctionHandle gAnytimeEvents_CollisionWorldHandle;
InterfaceId AnytimeEvents_CollWorld_CiviId = {"AnytimeEvents_CollWorld_Civi", (short)0x5BA7, (short)0x5569};
FunctionHandle gAnytimeEvents_CollWorld_CiviHandle;
InterfaceId AnytimeEvents_CollWorld_SpinId = {"AnytimeEvents_CollWorld_Spin", (short)0x5BA7, (short)0x6378};
FunctionHandle gAnytimeEvents_CollWorld_SpinHandle;
InterfaceId AnytimeEvents_CollWorld_AirId = {"AnytimeEvents_CollWorld_Air", (short)0x5BA7, (short)0x352F};
FunctionHandle gAnytimeEvents_CollWorld_AirHandle;
InterfaceId AnytimeEvents_CollWorld_FlipId = {"AnytimeEvents_CollWorld_Flip", (short)0x5BA7, (short)0x2DC0};
FunctionHandle gAnytimeEvents_CollWorld_FlipHandle;
InterfaceId AnytimeEvents_DispPursuitUpdateId = {"AnytimeEvents_DispPursuitUpdate", (short)0x5BA7, (short)0x18EA};
FunctionHandle gAnytimeEvents_DispPursuitUpdateHandle;
InterfaceId AnytimeEvents_PursuitUpdateRepId = {"AnytimeEvents_PursuitUpdateRep", (short)0x5BA7, (short)0x795D};
FunctionHandle gAnytimeEvents_PursuitUpdateRepHandle;
InterfaceId AnytimeEvents_Disp911ReportId = {"AnytimeEvents_Disp911Report", (short)0x5BA7, (short)0x5E7B};
FunctionHandle gAnytimeEvents_Disp911ReportHandle;
InterfaceId AnytimeEvents_Disp911CsPntId = {"AnytimeEvents_Disp911CsPnt", (short)0x5BA7, (short)0x19BC};
FunctionHandle gAnytimeEvents_Disp911CsPntHandle;
InterfaceId AnytimeEvents_Disp911NoDescripId = {"AnytimeEvents_Disp911NoDescrip", (short)0x5BA7, (short)0x1D71};
FunctionHandle gAnytimeEvents_Disp911NoDescripHandle;
InterfaceId AnytimeEvents_Unit911ReplyId = {"AnytimeEvents_Unit911Reply", (short)0x5BA7, (short)0x383D};
FunctionHandle gAnytimeEvents_Unit911ReplyHandle;
InterfaceId AnytimeEvents_SuspectUTurnId = {"AnytimeEvents_SuspectUTurn", (short)0x5BA7, (short)0x72C0};
FunctionHandle gAnytimeEvents_SuspectUTurnHandle;
InterfaceId AnytimeEvents_SuspectOutrunId = {"AnytimeEvents_SuspectOutrun", (short)0x5BA7, (short)0x4847};
FunctionHandle gAnytimeEvents_SuspectOutrunHandle;
InterfaceId AnytimeEvents_LostVisualId = {"AnytimeEvents_LostVisual", (short)0x5BA7, (short)0x3DFC};
FunctionHandle gAnytimeEvents_LostVisualHandle;
InterfaceId AnytimeEvents_RegainVisualId = {"AnytimeEvents_RegainVisual", (short)0x5BA7, (short)0x51D8};
FunctionHandle gAnytimeEvents_RegainVisualHandle;
InterfaceId AnytimeEvents_LostSuspectId = {"AnytimeEvents_LostSuspect", (short)0x5BA7, (short)0x34A4};
FunctionHandle gAnytimeEvents_LostSuspectHandle;
InterfaceId AnytimeEvents_DispBreakAwayId = {"AnytimeEvents_DispBreakAway", (short)0x5BA7, (short)0x54FC};
FunctionHandle gAnytimeEvents_DispBreakAwayHandle;
InterfaceId AnytimeEvents_DispTimeExpiredId = {"AnytimeEvents_DispTimeExpired", (short)0x5BA7, (short)0x784A};
FunctionHandle gAnytimeEvents_DispTimeExpiredHandle;
InterfaceId AnytimeEvents_DispPursuitEscalationId = {"AnytimeEvents_DispPursuitEscalation", (short)0x5BA7, (short)0x2701};
FunctionHandle gAnytimeEvents_DispPursuitEscalationHandle;
InterfaceId AnytimeEvents_DispPursEscGenId = {"AnytimeEvents_DispPursEscGen", (short)0x5BA7, (short)0x6BAC};
FunctionHandle gAnytimeEvents_DispPursEscGenHandle;
InterfaceId AnytimeEvents_UnitDisabledId = {"AnytimeEvents_UnitDisabled", (short)0x5BA7, (short)0x4002};
FunctionHandle gAnytimeEvents_UnitDisabledHandle;
InterfaceId AnytimeEvents_CallForEVId = {"AnytimeEvents_CallForEV", (short)0x5BA7, (short)0x216B};
FunctionHandle gAnytimeEvents_CallForEVHandle;
InterfaceId AnytimeEvents_DispEVReplyId = {"AnytimeEvents_DispEVReply", (short)0x5BA7, (short)0x7BCE};
FunctionHandle gAnytimeEvents_DispEVReplyHandle;
InterfaceId AnytimeEvents_IntentToRamId = {"AnytimeEvents_IntentToRam", (short)0x5BA7, (short)0x6C5E};
FunctionHandle gAnytimeEvents_IntentToRamHandle;
InterfaceId AnytimeEvents_BailoutId = {"AnytimeEvents_Bailout", (short)0x5BA7, (short)0x49F3};
FunctionHandle gAnytimeEvents_BailoutHandle;
InterfaceId AnytimeEvents_BailoutDenyId = {"AnytimeEvents_BailoutDeny", (short)0x5BA7, (short)0x416F};
FunctionHandle gAnytimeEvents_BailoutDenyHandle;
InterfaceId AnytimeEvents_FocusChangeId = {"AnytimeEvents_FocusChange", (short)0x5BA7, (short)0x39B1};
FunctionHandle gAnytimeEvents_FocusChangeHandle;
InterfaceId AnytimeEvents_SuspectBehaviourId = {"AnytimeEvents_SuspectBehaviour", (short)0x5BA7, (short)0x688B};
FunctionHandle gAnytimeEvents_SuspectBehaviourHandle;
InterfaceId AnytimeEvents_DriverHistoryId = {"AnytimeEvents_DriverHistory", (short)0x5BA7, (short)0x689E};
FunctionHandle gAnytimeEvents_DriverHistoryHandle;
InterfaceId AnytimeEvents_OffroadMomentId = {"AnytimeEvents_OffroadMoment", (short)0x5BA7, (short)0x7B7E};
FunctionHandle gAnytimeEvents_OffroadMomentHandle;
InterfaceId AnytimeEvents_SpottedId = {"AnytimeEvents_Spotted", (short)0x5BA7, (short)0x0CB3};
FunctionHandle gAnytimeEvents_SpottedHandle;
InterfaceId AnytimeEvents_SuspectBrakeId = {"AnytimeEvents_SuspectBrake", (short)0x5BA7, (short)0x54F4};
FunctionHandle gAnytimeEvents_SuspectBrakeHandle;
InterfaceId AnytimeEvents_WeatherReportId = {"AnytimeEvents_WeatherReport", (short)0x5BA7, (short)0x7F43};
FunctionHandle gAnytimeEvents_WeatherReportHandle;
InterfaceId AnytimeEvents_HeatJumpId = {"AnytimeEvents_HeatJump", (short)0x5BA7, (short)0x715F};
FunctionHandle gAnytimeEvents_HeatJumpHandle;
InterfaceId AnytimeEvents_DirectionHighId = {"AnytimeEvents_DirectionHigh", (short)0x5BA7, (short)0x6373};
FunctionHandle gAnytimeEvents_DirectionHighHandle;
InterfaceId AnytimeEvents_DispJurisShiftId = {"AnytimeEvents_DispJurisShift", (short)0x5BA7, (short)0x0C76};
FunctionHandle gAnytimeEvents_DispJurisShiftHandle;
InterfaceId HeliSpecific_HeliSelfStrategyId = {"HeliSpecific_HeliSelfStrategy", (short)0x5BA7, (short)0x2A4F};
FunctionHandle gHeliSpecific_HeliSelfStrategyHandle;
InterfaceId HeliSpecific_HeliLostVisualId = {"HeliSpecific_HeliLostVisual", (short)0x5BA7, (short)0x0744};
FunctionHandle gHeliSpecific_HeliLostVisualHandle;
InterfaceId HeliSpecific_HeliIntentToBailId = {"HeliSpecific_HeliIntentToBail", (short)0x5BA7, (short)0x4FED};
FunctionHandle gHeliSpecific_HeliIntentToBailHandle;
InterfaceId HeliSpecific_HeliBailoutId = {"HeliSpecific_HeliBailout", (short)0x5BA7, (short)0x2394};
FunctionHandle gHeliSpecific_HeliBailoutHandle;
InterfaceId HeliSpecific_HeliSwarmingId = {"HeliSpecific_HeliSwarming", (short)0x5BA7, (short)0x2274};
FunctionHandle gHeliSpecific_HeliSwarmingHandle;
InterfaceId HeliSpecific_HeliSpotterId = {"HeliSpecific_HeliSpotter", (short)0x5BA7, (short)0x35F5};
FunctionHandle gHeliSpecific_HeliSpotterHandle;
InterfaceId HeliSpecific_HeliHazardAlertId = {"HeliSpecific_HeliHazardAlert", (short)0x5BA7, (short)0x4AFB};
FunctionHandle gHeliSpecific_HeliHazardAlertHandle;
InterfaceId HeliSpecific_HeliQuadrentId = {"HeliSpecific_HeliQuadrent", (short)0x5BA7, (short)0x4F58};
FunctionHandle gHeliSpecific_HeliQuadrentHandle;
InterfaceId HeliSpecific_HeliQuadrentMovingId = {"HeliSpecific_HeliQuadrentMoving", (short)0x5BA7, (short)0x07F9};
FunctionHandle gHeliSpecific_HeliQuadrentMovingHandle;
InterfaceId HeliSpecific_HeliBullhornArrestId = {"HeliSpecific_HeliBullhornArrest", (short)0x5BA7, (short)0x0D4B};
FunctionHandle gHeliSpecific_HeliBullhornArrestHandle;
InterfaceId E3_Events_E3_SetupId = {"E3_Events_E3_Setup", (short)0x5BA7, (short)0x16B2};
FunctionHandle gE3_Events_E3_SetupHandle;
InterfaceId Interrupts_InterruptId = {"Interrupts_Interrupt", (short)0x5BA7, (short)0x1771};
FunctionHandle gInterrupts_InterruptHandle;
InterfaceId Interrupts_InterruptRamId = {"Interrupts_InterruptRam", (short)0x5BA7, (short)0x5E3B};
FunctionHandle gInterrupts_InterruptRamHandle;
InterfaceId Interrupts_InterruptRam_REId = {"Interrupts_InterruptRam_RE", (short)0x5BA7, (short)0x13B3};
FunctionHandle gInterrupts_InterruptRam_REHandle;
InterfaceId Interrupts_InterruptRam_HOId = {"Interrupts_InterruptRam_HO", (short)0x5BA7, (short)0x6FB0};
FunctionHandle gInterrupts_InterruptRam_HOHandle;
InterfaceId Interrupts_InterruptRam_SSId = {"Interrupts_InterruptRam_SS", (short)0x5BA7, (short)0x1AD3};
FunctionHandle gInterrupts_InterruptRam_SSHandle;
InterfaceId Interrupts_InterruptRam_TBId = {"Interrupts_InterruptRam_TB", (short)0x5BA7, (short)0x327B};
FunctionHandle gInterrupts_InterruptRam_TBHandle;
InterfaceId Interrupts_InterruptRamHighId = {"Interrupts_InterruptRamHigh", (short)0x5BA7, (short)0x3FD5};
FunctionHandle gInterrupts_InterruptRamHighHandle;
InterfaceId Interrupts_StaticInterruptId = {"Interrupts_StaticInterrupt", (short)0x5BA7, (short)0x5513};
FunctionHandle gInterrupts_StaticInterruptHandle;
InterfaceId Interrupts_RegainVisualInterruptId = {"Interrupts_RegainVisualInterrupt", (short)0x5BA7, (short)0x1F16};
FunctionHandle gInterrupts_RegainVisualInterruptHandle;
InterfaceId CellCallId = {"CellCall", (short)0x5BA7, (short)0x107B};
FunctionHandle gCellCallHandle;
InterfaceId ExtraCops_SwarmingReplyId = {"ExtraCops_SwarmingReply", (short)0x5BA7, (short)0x5B8C};
FunctionHandle gExtraCops_SwarmingReplyHandle;
InterfaceId ExtraCops_SuperPursuitReplyId = {"ExtraCops_SuperPursuitReply", (short)0x5BA7, (short)0x77F5};
FunctionHandle gExtraCops_SuperPursuitReplyHandle;
InterfaceId ExtraCops_SwarmingReplyFollowId = {"ExtraCops_SwarmingReplyFollow", (short)0x5BA7, (short)0x4A30};
FunctionHandle gExtraCops_SwarmingReplyFollowHandle;
InterfaceId ExtraCops_QuadrentFormingId = {"ExtraCops_QuadrentForming", (short)0x5BA7, (short)0x75EE};
FunctionHandle gExtraCops_QuadrentFormingHandle;
InterfaceId ExtraCops_SuspectPossiblyGoneId = {"ExtraCops_SuspectPossiblyGone", (short)0x5BA7, (short)0x58D4};
FunctionHandle gExtraCops_SuspectPossiblyGoneHandle;
InterfaceId ExtraCops_QuadrentMovingId = {"ExtraCops_QuadrentMoving", (short)0x5BA7, (short)0x1F0E};
FunctionHandle gExtraCops_QuadrentMovingHandle;
InterfaceId ExtraCops_OtherLeadId = {"ExtraCops_OtherLead", (short)0x5BA7, (short)0x148A};
FunctionHandle gExtraCops_OtherLeadHandle;
InterfaceId ExtraCops_PossibleSuspectId = {"ExtraCops_PossibleSuspect", (short)0x5BA7, (short)0x6AB8};
FunctionHandle gExtraCops_PossibleSuspectHandle;
InterfaceId ExtraCops_WrongSuspectId = {"ExtraCops_WrongSuspect", (short)0x5BA7, (short)0x25E7};
FunctionHandle gExtraCops_WrongSuspectHandle;
InterfaceId ExtraCops_SuspectGoneId = {"ExtraCops_SuspectGone", (short)0x5BA7, (short)0x3ED4};
FunctionHandle gExtraCops_SuspectGoneHandle;
InterfaceId ExtraCops_RBWarningId = {"ExtraCops_RBWarning", (short)0x5BA7, (short)0x0DF4};
FunctionHandle gExtraCops_RBWarningHandle;
InterfaceId ExtraCops_RBPositionId = {"ExtraCops_RBPosition", (short)0x5BA7, (short)0x1566};
FunctionHandle gExtraCops_RBPositionHandle;
InterfaceId ExtraCops_ExtraRBEngageId = {"ExtraCops_ExtraRBEngage", (short)0x5BA7, (short)0x63D7};
FunctionHandle gExtraCops_ExtraRBEngageHandle;
InterfaceId ExtraCops_ExtraRBAvertedId = {"ExtraCops_ExtraRBAverted", (short)0x5BA7, (short)0x149C};
FunctionHandle gExtraCops_ExtraRBAvertedHandle;
InterfaceId Cross_CrossBUReplyId = {"Cross_CrossBUReply", (short)0x5BA7, (short)0x4B6A};
FunctionHandle gCross_CrossBUReplyHandle;
InterfaceId Cross_CrossFailReplyId = {"Cross_CrossFailReply", (short)0x5BA7, (short)0x489E};
FunctionHandle gCross_CrossFailReplyHandle;
InterfaceId Cross_CrossRBFailReplyId = {"Cross_CrossRBFailReply", (short)0x5BA7, (short)0x5CF2};
FunctionHandle gCross_CrossRBFailReplyHandle;
InterfaceId Cross_CrossPursuitEscId = {"Cross_CrossPursuitEsc", (short)0x5BA7, (short)0x0677};
FunctionHandle gCross_CrossPursuitEscHandle;
InterfaceId Cross_CrossSelfStrategyId = {"Cross_CrossSelfStrategy", (short)0x5BA7, (short)0x7E65};
FunctionHandle gCross_CrossSelfStrategyHandle;
InterfaceId Cross_CrossMultiStrategyId = {"Cross_CrossMultiStrategy", (short)0x5BA7, (short)0x4A25};
FunctionHandle gCross_CrossMultiStrategyHandle;
InterfaceId Cross_CrossBailoutDeny_subId = {"Cross_CrossBailoutDeny_sub", (short)0x5BA7, (short)0x4C05};
FunctionHandle gCross_CrossBailoutDeny_subHandle;
InterfaceId D_DayId = {"D_Day", (short)0x5BA7, (short)0x1AB7};
FunctionHandle gD_DayHandle;
InterfaceId DispIntroRaceId = {"DispIntroRace", (short)0x5BA7, (short)0x3270};
FunctionHandle gDispIntroRaceHandle;
} // namespace Csis

EAXCharacter::EAXCharacter(int sID, HSIMABLE wID, int bID, int cID)
    : mSpeakerID(sID), mHandle(wID), mCallsign(bID, cID), mPos(UMath::Vector3::kZero), mSpeed(0.0f), mDistance(0.0f),
      mHealth(1.0f), mDestroyed(false), mActive(false), mSuspectLOS(false) {}

void *EAXCharacter::operator new(unsigned int obj_size) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr || ai->GetActorPool()->IsFull()) {
        return NullPointer;
    }
    return ai->GetActorPool()->Malloc(1, nullptr);
}

void EAXCharacter::operator delete(void *ptr) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        ai->GetActorPool()->Free(ptr);
    }
}

void EAXCharacter::Reset() {
    mActive = false;
    mDestroyed = false;
    mHealth = 0.0f;
    mDistance = 0.0f;
    mPos = UMath::Vector3::kZero;
    mSpeed = 0.0f;
    mSuspectLOS = false;
}

EAXCharacter::~EAXCharacter() {}

void EAXCharacter::Update() {
    ISimable *simable = ISimable::FindInstance(GetHandle());
    IVehicle *vehicle = nullptr;
    if (simable != nullptr) {
        simable->QueryInterface(&vehicle);
    } else {
        mActive = false;
        mHandle = nullptr;
    }
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && vehicle != nullptr && mActive) {
        mPos = vehicle->GetPosition();
        UMath::Vector3 pPos = ai->GetPlayerPos();
        UMath::Vector3 cPos = mPos;
        mSpeed = MPS2MPH(vehicle->GetAbsoluteSpeed());
        mDistance = UMath::Distance(cPos, pPos);
    }
}

void EAXCharacter::Ack() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AcknowledgeStruct ack;
    ack.speaker_id = mSpeakerID;
    ack.yes_no = mDestroyed ? Csis::Type_yes_no_No_False : Csis::Type_yes_no_Yes_True;
    ack.intensity = Csis::Type_intensity_Normal;
    SCHEDULE_SPEECH(Acknowledge, ack, this);
}

void EAXCharacter::Deny() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AcknowledgeStruct deny;
    deny.speaker_id = mSpeakerID;
    deny.yes_no = mDestroyed ? Csis::Type_yes_no_No_False : Csis::Type_yes_no_Yes_True;
    deny.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_Normal : Csis::Type_intensity_High;
    SCHEDULE_SPEECH(Acknowledge, deny, this);
}

void EAXCharacter::InterruptStatic() {
    Csis::Interrupts_StaticInterruptStruct data;
    SCHEDULE_SPEECH(Interrupts_StaticInterrupt, data, this);
}

void EAXCharacter::InterruptExpletive() {
    Csis::Interrupts_InterruptRamStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Interrupts_InterruptRam, data, this);
}

void EAXCharacter::InterruptViolent() {
    Csis::Interrupts_InterruptRamHighStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Interrupts_InterruptRamHigh, data, this);
}

void EAXCharacter::InterruptComposedLow() {
    Csis::Interrupts_InterruptStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = Csis::Type_intensity_Normal;
    SCHEDULE_SPEECH(Interrupts_Interrupt, data, this);
}

void EAXCharacter::InterruptComposedHigh() {
    Csis::Interrupts_InterruptStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = Csis::Type_intensity_High;
    SCHEDULE_SPEECH(Interrupts_Interrupt, data, this);
}

void EAXCharacter::DriverHistory() {
    Csis::AnytimeEvents_DriverHistoryStruct data;
    data.speaker_id = mSpeakerID;
    data.region = Csis::Type_region_Coastal;
    SCHEDULE_SPEECH(AnytimeEvents_DriverHistory, data, this);
}

void EAXCharacter::HeatJump(Csis::Type_heat_level heat) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::AnytimeEvents_HeatJumpStruct data;
        data.speaker_id = mSpeakerID;
        data.heat_level = heat;
        SCHEDULE_SPEECH(AnytimeEvents_HeatJump, data, this);
    }
}
