
#include "Speed/Indep/Src/EAXSound/SND_GEN/COPSPEECH.hpp"

namespace Csis {

InterfaceId AcknowledgeId = {"Acknowledge", 0x5BA7, 0x2E86};
FunctionHandle gAcknowledgeHandle;

InterfaceId Setup_SpotterId = {"Setup_Spotter", 0x5BA7, 0x17C9};
FunctionHandle gSetup_SpotterHandle;

InterfaceId Setup_SpotterWantedId = {"Setup_SpotterWanted", 0x5BA7, 0x47C7};
FunctionHandle gSetup_SpotterWantedHandle;

InterfaceId Setup_SpotterReplyId = {"Setup_SpotterReply", 0x5BA7, 0x0369};
FunctionHandle gSetup_SpotterReplyHandle;

InterfaceId Setup_AttmptVehStpId = {"Setup_AttmptVehStp", 0x5BA7, 0x668C};
FunctionHandle gSetup_AttmptVehStpHandle;

InterfaceId Setup_DispGoAheadId = {"Setup_DispGoAhead", 0x5BA7, 0x17F9};
FunctionHandle gSetup_DispGoAheadHandle;

InterfaceId Setup_PrimaryEngageId = {"Setup_PrimaryEngage", 0x5BA7, 0x0A49};
FunctionHandle gSetup_PrimaryEngageHandle;

InterfaceId Setup_InitPursuitId = {"Setup_InitPursuit", 0x5BA7, 0x1418};
FunctionHandle gSetup_InitPursuitHandle;

InterfaceId Setup_SuspectConfirmedId = {"Setup_SuspectConfirmed", 0x5BA7, 0x4CBF};
FunctionHandle gSetup_SuspectConfirmedHandle;

InterfaceId Setup_ReInitPursuitId = {"Setup_ReInitPursuit", 0x5BA7, 0x4F90};
FunctionHandle gSetup_ReInitPursuitHandle;

InterfaceId Setup_VehicleReportId = {"Setup_VehicleReport", 0x5BA7, 0x466D};
FunctionHandle gSetup_VehicleReportHandle;

InterfaceId Setup_VehicleReportTagId = {"Setup_VehicleReportTag", 0x5BA7, 0x5E34};
FunctionHandle gSetup_VehicleReportTagHandle;

InterfaceId Setup_DispVehDescripId = {"Setup_DispVehDescrip", 0x5BA7, 0x2FDF};
FunctionHandle gSetup_DispVehDescripHandle;

InterfaceId Setup_DispVehDescripVinylsId = {"Setup_DispVehDescripVinyls", 0x5BA7, 0x73EC};
FunctionHandle gSetup_DispVehDescripVinylsHandle;

InterfaceId Setup_DispNoVehDescripId = {"Setup_DispNoVehDescrip", 0x5BA7, 0x6555};
FunctionHandle gSetup_DispNoVehDescripHandle;

InterfaceId Setup_DispCustPaintId = {"Setup_DispCustPaint", 0x5BA7, 0x05C4};
FunctionHandle gSetup_DispCustPaintHandle;

InterfaceId Setup_MoreDetailsId = {"Setup_MoreDetails", 0x5BA7, 0x40A4};
FunctionHandle gSetup_MoreDetailsHandle;

InterfaceId Setup_LocationReportId = {"Setup_LocationReport", 0x5BA7, 0x7048};
FunctionHandle gSetup_LocationReportHandle;

InterfaceId Setup_BullhornPrefixId = {"Setup_BullhornPrefix", 0x5BA7, 0x5893};
FunctionHandle gSetup_BullhornPrefixHandle;

InterfaceId Setup_BullhornId = {"Setup_Bullhorn", 0x5BA7, 0x5E0A};
FunctionHandle gSetup_BullhornHandle;

InterfaceId Setup_SelfStrategyId = {"Setup_SelfStrategy", 0x5BA7, 0x3A53};
FunctionHandle gSetup_SelfStrategyHandle;

InterfaceId Setup_InitialCallForBUId = {"Setup_InitialCallForBU", 0x5BA7, 0x154B};
FunctionHandle gSetup_InitialCallForBUHandle;

InterfaceId Setup_InitialCallForBU_MSId = {"Setup_InitialCallForBU_MS", 0x5BA7, 0x23E5};
FunctionHandle gSetup_InitialCallForBU_MSHandle;

InterfaceId Backup_CallForBUId = {"Backup_CallForBU", 0x5BA7, 0x398D};
FunctionHandle gBackup_CallForBUHandle;

InterfaceId Backup_UnitBUReplyId = {"Backup_UnitBUReply", 0x5BA7, 0x05D5};
FunctionHandle gBackup_UnitBUReplyHandle;

InterfaceId Backup_DispBackupReplyId = {"Backup_DispBackupReply", 0x5BA7, 0x5C2F};
FunctionHandle gBackup_DispBackupReplyHandle;

InterfaceId Backup_CallForSwarmingId = {"Backup_CallForSwarming", 0x5BA7, 0x5B23};
FunctionHandle gBackup_CallForSwarmingHandle;

InterfaceId Backup_DispBUETAId = {"Backup_DispBUETA", 0x5BA7, 0x3C9E};
FunctionHandle gBackup_DispBUETAHandle;

InterfaceId Backup_DispHeliBUETAId = {"Backup_DispHeliBUETA", 0x5BA7, 0x028D};
FunctionHandle gBackup_DispHeliBUETAHandle;

InterfaceId Backup_BUReminderId = {"Backup_BUReminder", 0x5BA7, 0x11A9};
FunctionHandle gBackup_BUReminderHandle;

InterfaceId Backup_NegativeBUReplyId = {"Backup_NegativeBUReply", 0x5BA7, 0x29E8};
FunctionHandle gBackup_NegativeBUReplyHandle;

InterfaceId Backup_DispBackupUpdateId = {"Backup_DispBackupUpdate", 0x5BA7, 0x70B2};
FunctionHandle gBackup_DispBackupUpdateHandle;

InterfaceId Backup_BUArrivesId = {"Backup_BUArrives", 0x5BA7, 0x4F76};
FunctionHandle gBackup_BUArrivesHandle;

InterfaceId StaticRoadblock_CallForRBId = {"StaticRoadblock_CallForRB", 0x5BA7, 0x7D0E};
FunctionHandle gStaticRoadblock_CallForRBHandle;

InterfaceId StaticRoadblock_RBReminderId = {"StaticRoadblock_RBReminder", 0x5BA7, 0x45D5};
FunctionHandle gStaticRoadblock_RBReminderHandle;

InterfaceId StaticRoadblock_NegativeRBReplyId = {"StaticRoadblock_NegativeRBReply", 0x5BA7, 0x0955};
FunctionHandle gStaticRoadblock_NegativeRBReplyHandle;

InterfaceId StaticRoadblock_DispRBReplyId = {"StaticRoadblock_DispRBReply", 0x5BA7, 0x1D9A};
FunctionHandle gStaticRoadblock_DispRBReplyHandle;

InterfaceId StaticRoadblock_DispRBUpdateId = {"StaticRoadblock_DispRBUpdate", 0x5BA7, 0x7B7F};
FunctionHandle gStaticRoadblock_DispRBUpdateHandle;

InterfaceId StaticRoadblock_PursuitApproachingId = {"StaticRoadblock_PursuitApproaching", 0x5BA7, 0x58B5};
FunctionHandle gStaticRoadblock_PursuitApproachingHandle;

InterfaceId StaticRoadblock_RBApproachId = {"StaticRoadblock_RBApproach", 0x5BA7, 0x6AF1};
FunctionHandle gStaticRoadblock_RBApproachHandle;

InterfaceId StaticRoadblock_RBEngageId = {"StaticRoadblock_RBEngage", 0x5BA7, 0x3E19};
FunctionHandle gStaticRoadblock_RBEngageHandle;

InterfaceId StaticRoadblock_RBAvertedId = {"StaticRoadblock_RBAverted", 0x5BA7, 0x5E18};
FunctionHandle gStaticRoadblock_RBAvertedHandle;

InterfaceId StaticRoadblock_CallForRB_subId = {"StaticRoadblock_CallForRB_sub", 0x5BA7, 0x58FF};
FunctionHandle gStaticRoadblock_CallForRB_subHandle;

InterfaceId StaticRoadblock_DispSubRBId = {"StaticRoadblock_DispSubRB", 0x5BA7, 0x1CEE};
FunctionHandle gStaticRoadblock_DispSubRBHandle;

InterfaceId Projectile_CallForSafetyId = {"Projectile_CallForSafety", 0x5BA7, 0x42C8};
FunctionHandle gProjectile_CallForSafetyHandle;

InterfaceId Projectile_ProjectileLaunchId = {"Projectile_ProjectileLaunch", 0x5BA7, 0x0AA7};
FunctionHandle gProjectile_ProjectileLaunchHandle;

InterfaceId Projectile_ProjectileHitId = {"Projectile_ProjectileHit", 0x5BA7, 0x6BED};
FunctionHandle gProjectile_ProjectileHitHandle;

InterfaceId Projectile_ProjectileMissId = {"Projectile_ProjectileMiss", 0x5BA7, 0x1DF2};
FunctionHandle gProjectile_ProjectileMissHandle;

InterfaceId RollingStrategy_InitStrategyId = {"RollingStrategy_InitStrategy", 0x5BA7, 0x3D7C};
FunctionHandle gRollingStrategy_InitStrategyHandle;

InterfaceId RollingStrategy_CallToPositionId = {"RollingStrategy_CallToPosition", 0x5BA7, 0x2BBC};
FunctionHandle gRollingStrategy_CallToPositionHandle;

InterfaceId RollingStrategy_CallToPositionRemId = {"RollingStrategy_CallToPositionRem", 0x5BA7, 0x4112};
FunctionHandle gRollingStrategy_CallToPositionRemHandle;

InterfaceId RollingStrategy_StrategyExecuteId = {"RollingStrategy_StrategyExecute", 0x5BA7, 0x6CB7};
FunctionHandle gRollingStrategy_StrategyExecuteHandle;

InterfaceId Outcome_AnticipateFailId = {"Outcome_AnticipateFail", 0x5BA7, 0x0AF9};
FunctionHandle gOutcome_AnticipateFailHandle;

InterfaceId Outcome_AnticipateSuccessId = {"Outcome_AnticipateSuccess", 0x5BA7, 0x4116};
FunctionHandle gOutcome_AnticipateSuccessHandle;

InterfaceId Outcome_OutcomeFailId = {"Outcome_OutcomeFail", 0x5BA7, 0x72D3};
FunctionHandle gOutcome_OutcomeFailHandle;

InterfaceId Outcome_StrategyResetId = {"Outcome_StrategyReset", 0x5BA7, 0x4EA0};
FunctionHandle gOutcome_StrategyResetHandle;

InterfaceId Arrest_BullhornArrestId = {"Arrest_BullhornArrest", 0x5BA7, 0x1AB5};
FunctionHandle gArrest_BullhornArrestHandle;

InterfaceId Arrest_ArrestId = {"Arrest_Arrest", 0x5BA7, 0x7925};
FunctionHandle gArrest_ArrestHandle;

InterfaceId Arrest_DispArrestReplyId = {"Arrest_DispArrestReply", 0x5BA7, 0x6C90};
FunctionHandle gArrest_DispArrestReplyHandle;

InterfaceId AnytimeEvents_CollisionWorldId = {"AnytimeEvents_CollisionWorld", 0x5BA7, 0x6D79};
FunctionHandle gAnytimeEvents_CollisionWorldHandle;

InterfaceId AnytimeEvents_CollWorld_CiviId = {"AnytimeEvents_CollWorld_Civi", 0x5BA7, 0x5569};
FunctionHandle gAnytimeEvents_CollWorld_CiviHandle;

InterfaceId AnytimeEvents_CollWorld_SpinId = {"AnytimeEvents_CollWorld_Spin", 0x5BA7, 0x6378};
FunctionHandle gAnytimeEvents_CollWorld_SpinHandle;

InterfaceId AnytimeEvents_CollWorld_AirId = {"AnytimeEvents_CollWorld_Air", 0x5BA7, 0x352F};
FunctionHandle gAnytimeEvents_CollWorld_AirHandle;

InterfaceId AnytimeEvents_CollWorld_FlipId = {"AnytimeEvents_CollWorld_Flip", 0x5BA7, 0x2DC0};
FunctionHandle gAnytimeEvents_CollWorld_FlipHandle;

InterfaceId AnytimeEvents_DispPursuitUpdateId = {"AnytimeEvents_DispPursuitUpdate", 0x5BA7, 0x18EA};
FunctionHandle gAnytimeEvents_DispPursuitUpdateHandle;

InterfaceId AnytimeEvents_PursuitUpdateRepId = {"AnytimeEvents_PursuitUpdateRep", 0x5BA7, 0x795D};
FunctionHandle gAnytimeEvents_PursuitUpdateRepHandle;

InterfaceId AnytimeEvents_Disp911ReportId = {"AnytimeEvents_Disp911Report", 0x5BA7, 0x5E7B};
FunctionHandle gAnytimeEvents_Disp911ReportHandle;

InterfaceId AnytimeEvents_Disp911CsPntId = {"AnytimeEvents_Disp911CsPnt", 0x5BA7, 0x19BC};
FunctionHandle gAnytimeEvents_Disp911CsPntHandle;

InterfaceId AnytimeEvents_Disp911NoDescripId = {"AnytimeEvents_Disp911NoDescrip", 0x5BA7, 0x1D71};
FunctionHandle gAnytimeEvents_Disp911NoDescripHandle;

InterfaceId AnytimeEvents_Unit911ReplyId = {"AnytimeEvents_Unit911Reply", 0x5BA7, 0x383D};
FunctionHandle gAnytimeEvents_Unit911ReplyHandle;

InterfaceId AnytimeEvents_SuspectUTurnId = {"AnytimeEvents_SuspectUTurn", 0x5BA7, 0x72C0};
FunctionHandle gAnytimeEvents_SuspectUTurnHandle;

InterfaceId AnytimeEvents_SuspectOutrunId = {"AnytimeEvents_SuspectOutrun", 0x5BA7, 0x4847};
FunctionHandle gAnytimeEvents_SuspectOutrunHandle;

InterfaceId AnytimeEvents_LostVisualId = {"AnytimeEvents_LostVisual", 0x5BA7, 0x3DFC};
FunctionHandle gAnytimeEvents_LostVisualHandle;

InterfaceId AnytimeEvents_RegainVisualId = {"AnytimeEvents_RegainVisual", 0x5BA7, 0x51D8};
FunctionHandle gAnytimeEvents_RegainVisualHandle;

InterfaceId AnytimeEvents_LostSuspectId = {"AnytimeEvents_LostSuspect", 0x5BA7, 0x34A4};
FunctionHandle gAnytimeEvents_LostSuspectHandle;

InterfaceId AnytimeEvents_DispBreakAwayId = {"AnytimeEvents_DispBreakAway", 0x5BA7, 0x54FC};
FunctionHandle gAnytimeEvents_DispBreakAwayHandle;

InterfaceId AnytimeEvents_DispTimeExpiredId = {"AnytimeEvents_DispTimeExpired", 0x5BA7, 0x784A};
FunctionHandle gAnytimeEvents_DispTimeExpiredHandle;

InterfaceId AnytimeEvents_DispPursuitEscalationId = {"AnytimeEvents_DispPursuitEscalation", 0x5BA7, 0x2701};
FunctionHandle gAnytimeEvents_DispPursuitEscalationHandle;

InterfaceId AnytimeEvents_DispPursEscGenId = {"AnytimeEvents_DispPursEscGen", 0x5BA7, 0x6BAC};
FunctionHandle gAnytimeEvents_DispPursEscGenHandle;

InterfaceId AnytimeEvents_UnitDisabledId = {"AnytimeEvents_UnitDisabled", 0x5BA7, 0x4002};
FunctionHandle gAnytimeEvents_UnitDisabledHandle;

InterfaceId AnytimeEvents_CallForEVId = {"AnytimeEvents_CallForEV", 0x5BA7, 0x216B};
FunctionHandle gAnytimeEvents_CallForEVHandle;

InterfaceId AnytimeEvents_DispEVReplyId = {"AnytimeEvents_DispEVReply", 0x5BA7, 0x7BCE};
FunctionHandle gAnytimeEvents_DispEVReplyHandle;

InterfaceId AnytimeEvents_IntentToRamId = {"AnytimeEvents_IntentToRam", 0x5BA7, 0x6C5E};
FunctionHandle gAnytimeEvents_IntentToRamHandle;

InterfaceId AnytimeEvents_BailoutId = {"AnytimeEvents_Bailout", 0x5BA7, 0x49F3};
FunctionHandle gAnytimeEvents_BailoutHandle;

InterfaceId AnytimeEvents_BailoutDenyId = {"AnytimeEvents_BailoutDeny", 0x5BA7, 0x416F};
FunctionHandle gAnytimeEvents_BailoutDenyHandle;

InterfaceId AnytimeEvents_FocusChangeId = {"AnytimeEvents_FocusChange", 0x5BA7, 0x39B1};
FunctionHandle gAnytimeEvents_FocusChangeHandle;

InterfaceId AnytimeEvents_SuspectBehaviourId = {"AnytimeEvents_SuspectBehaviour", 0x5BA7, 0x688B};
FunctionHandle gAnytimeEvents_SuspectBehaviourHandle;

InterfaceId AnytimeEvents_DriverHistoryId = {"AnytimeEvents_DriverHistory", 0x5BA7, 0x689E};
FunctionHandle gAnytimeEvents_DriverHistoryHandle;

InterfaceId AnytimeEvents_OffroadMomentId = {"AnytimeEvents_OffroadMoment", 0x5BA7, 0x7B7E};
FunctionHandle gAnytimeEvents_OffroadMomentHandle;

InterfaceId AnytimeEvents_SpottedId = {"AnytimeEvents_Spotted", 0x5BA7, 0x0CB3};
FunctionHandle gAnytimeEvents_SpottedHandle;

InterfaceId AnytimeEvents_SuspectBrakeId = {"AnytimeEvents_SuspectBrake", 0x5BA7, 0x54F4};
FunctionHandle gAnytimeEvents_SuspectBrakeHandle;

InterfaceId AnytimeEvents_WeatherReportId = {"AnytimeEvents_WeatherReport", 0x5BA7, 0x7F43};
FunctionHandle gAnytimeEvents_WeatherReportHandle;

InterfaceId AnytimeEvents_HeatJumpId = {"AnytimeEvents_HeatJump", 0x5BA7, 0x715F};
FunctionHandle gAnytimeEvents_HeatJumpHandle;

InterfaceId AnytimeEvents_DirectionHighId = {"AnytimeEvents_DirectionHigh", 0x5BA7, 0x6373};
FunctionHandle gAnytimeEvents_DirectionHighHandle;

InterfaceId AnytimeEvents_DispJurisShiftId = {"AnytimeEvents_DispJurisShift", 0x5BA7, 0x0C76};
FunctionHandle gAnytimeEvents_DispJurisShiftHandle;

InterfaceId HeliSpecific_HeliSelfStrategyId = {"HeliSpecific_HeliSelfStrategy", 0x5BA7, 0x2A4F};
FunctionHandle gHeliSpecific_HeliSelfStrategyHandle;

InterfaceId HeliSpecific_HeliLostVisualId = {"HeliSpecific_HeliLostVisual", 0x5BA7, 0x0744};
FunctionHandle gHeliSpecific_HeliLostVisualHandle;

InterfaceId HeliSpecific_HeliIntentToBailId = {"HeliSpecific_HeliIntentToBail", 0x5BA7, 0x4FED};
FunctionHandle gHeliSpecific_HeliIntentToBailHandle;

InterfaceId HeliSpecific_HeliBailoutId = {"HeliSpecific_HeliBailout", 0x5BA7, 0x2394};
FunctionHandle gHeliSpecific_HeliBailoutHandle;

InterfaceId HeliSpecific_HeliSwarmingId = {"HeliSpecific_HeliSwarming", 0x5BA7, 0x2274};
FunctionHandle gHeliSpecific_HeliSwarmingHandle;

InterfaceId HeliSpecific_HeliSpotterId = {"HeliSpecific_HeliSpotter", 0x5BA7, 0x35F5};
FunctionHandle gHeliSpecific_HeliSpotterHandle;

InterfaceId HeliSpecific_HeliHazardAlertId = {"HeliSpecific_HeliHazardAlert", 0x5BA7, 0x4AFB};
FunctionHandle gHeliSpecific_HeliHazardAlertHandle;

InterfaceId HeliSpecific_HeliQuadrentId = {"HeliSpecific_HeliQuadrent", 0x5BA7, 0x4F58};
FunctionHandle gHeliSpecific_HeliQuadrentHandle;

InterfaceId HeliSpecific_HeliQuadrentMovingId = {"HeliSpecific_HeliQuadrentMoving", 0x5BA7, 0x07F9};
FunctionHandle gHeliSpecific_HeliQuadrentMovingHandle;

InterfaceId HeliSpecific_HeliBullhornArrestId = {"HeliSpecific_HeliBullhornArrest", 0x5BA7, 0x0D4B};
FunctionHandle gHeliSpecific_HeliBullhornArrestHandle;

InterfaceId E3_Events_E3_SetupId = {"E3_Events_E3_Setup", 0x5BA7, 0x16B2};
FunctionHandle gE3_Events_E3_SetupHandle;

InterfaceId Interrupts_InterruptId = {"Interrupts_Interrupt", 0x5BA7, 0x1771};
FunctionHandle gInterrupts_InterruptHandle;

InterfaceId Interrupts_InterruptRamId = {"Interrupts_InterruptRam", 0x5BA7, 0x5E3B};
FunctionHandle gInterrupts_InterruptRamHandle;

InterfaceId Interrupts_InterruptRam_REId = {"Interrupts_InterruptRam_RE", 0x5BA7, 0x13B3};
FunctionHandle gInterrupts_InterruptRam_REHandle;

InterfaceId Interrupts_InterruptRam_HOId = {"Interrupts_InterruptRam_HO", 0x5BA7, 0x6FB0};
FunctionHandle gInterrupts_InterruptRam_HOHandle;

InterfaceId Interrupts_InterruptRam_SSId = {"Interrupts_InterruptRam_SS", 0x5BA7, 0x1AD3};
FunctionHandle gInterrupts_InterruptRam_SSHandle;

InterfaceId Interrupts_InterruptRam_TBId = {"Interrupts_InterruptRam_TB", 0x5BA7, 0x327B};
FunctionHandle gInterrupts_InterruptRam_TBHandle;

InterfaceId Interrupts_InterruptRamHighId = {"Interrupts_InterruptRamHigh", 0x5BA7, 0x3FD5};
FunctionHandle gInterrupts_InterruptRamHighHandle;

InterfaceId Interrupts_StaticInterruptId = {"Interrupts_StaticInterrupt", 0x5BA7, 0x5513};
FunctionHandle gInterrupts_StaticInterruptHandle;

InterfaceId Interrupts_RegainVisualInterruptId = {"Interrupts_RegainVisualInterrupt", 0x5BA7, 0x1F16};
FunctionHandle gInterrupts_RegainVisualInterruptHandle;

InterfaceId CellCallId = {"CellCall", 0x5BA7, 0x107B};
FunctionHandle gCellCallHandle;

InterfaceId ExtraCops_SwarmingReplyId = {"ExtraCops_SwarmingReply", 0x5BA7, 0x5B8C};
FunctionHandle gExtraCops_SwarmingReplyHandle;

InterfaceId ExtraCops_SuperPursuitReplyId = {"ExtraCops_SuperPursuitReply", 0x5BA7, 0x77F5};
FunctionHandle gExtraCops_SuperPursuitReplyHandle;

InterfaceId ExtraCops_SwarmingReplyFollowId = {"ExtraCops_SwarmingReplyFollow", 0x5BA7, 0x4A30};
FunctionHandle gExtraCops_SwarmingReplyFollowHandle;

InterfaceId ExtraCops_QuadrentFormingId = {"ExtraCops_QuadrentForming", 0x5BA7, 0x75EE};
FunctionHandle gExtraCops_QuadrentFormingHandle;

InterfaceId ExtraCops_SuspectPossiblyGoneId = {"ExtraCops_SuspectPossiblyGone", 0x5BA7, 0x58D4};
FunctionHandle gExtraCops_SuspectPossiblyGoneHandle;

InterfaceId ExtraCops_QuadrentMovingId = {"ExtraCops_QuadrentMoving", 0x5BA7, 0x1F0E};
FunctionHandle gExtraCops_QuadrentMovingHandle;

InterfaceId ExtraCops_OtherLeadId = {"ExtraCops_OtherLead", 0x5BA7, 0x148A};
FunctionHandle gExtraCops_OtherLeadHandle;

InterfaceId ExtraCops_PossibleSuspectId = {"ExtraCops_PossibleSuspect", 0x5BA7, 0x6AB8};
FunctionHandle gExtraCops_PossibleSuspectHandle;

InterfaceId ExtraCops_WrongSuspectId = {"ExtraCops_WrongSuspect", 0x5BA7, 0x25E7};
FunctionHandle gExtraCops_WrongSuspectHandle;

InterfaceId ExtraCops_SuspectGoneId = {"ExtraCops_SuspectGone", 0x5BA7, 0x3ED4};
FunctionHandle gExtraCops_SuspectGoneHandle;

InterfaceId ExtraCops_RBWarningId = {"ExtraCops_RBWarning", 0x5BA7, 0x0DF4};
FunctionHandle gExtraCops_RBWarningHandle;

InterfaceId ExtraCops_RBPositionId = {"ExtraCops_RBPosition", 0x5BA7, 0x1566};
FunctionHandle gExtraCops_RBPositionHandle;

InterfaceId ExtraCops_ExtraRBEngageId = {"ExtraCops_ExtraRBEngage", 0x5BA7, 0x63D7};
FunctionHandle gExtraCops_ExtraRBEngageHandle;

InterfaceId ExtraCops_ExtraRBAvertedId = {"ExtraCops_ExtraRBAverted", 0x5BA7, 0x149C};
FunctionHandle gExtraCops_ExtraRBAvertedHandle;

InterfaceId Cross_CrossBUReplyId = {"Cross_CrossBUReply", 0x5BA7, 0x4B6A};
FunctionHandle gCross_CrossBUReplyHandle;

InterfaceId Cross_CrossFailReplyId = {"Cross_CrossFailReply", 0x5BA7, 0x489E};
FunctionHandle gCross_CrossFailReplyHandle;

InterfaceId Cross_CrossRBFailReplyId = {"Cross_CrossRBFailReply", 0x5BA7, 0x5CF2};
FunctionHandle gCross_CrossRBFailReplyHandle;

InterfaceId Cross_CrossPursuitEscId = {"Cross_CrossPursuitEsc", 0x5BA7, 0x0677};
FunctionHandle gCross_CrossPursuitEscHandle;

InterfaceId Cross_CrossSelfStrategyId = {"Cross_CrossSelfStrategy", 0x5BA7, 0x7E65};
FunctionHandle gCross_CrossSelfStrategyHandle;

InterfaceId Cross_CrossMultiStrategyId = {"Cross_CrossMultiStrategy", 0x5BA7, 0x4A25};
FunctionHandle gCross_CrossMultiStrategyHandle;

InterfaceId Cross_CrossBailoutDeny_subId = {"Cross_CrossBailoutDeny_sub", 0x5BA7, 0x4C05};
FunctionHandle gCross_CrossBailoutDeny_subHandle;

InterfaceId D_DayId = {"D_Day", 0x5BA7, 0x1AB7};
FunctionHandle gD_DayHandle;

InterfaceId DispIntroRaceId = {"DispIntroRace", 0x5BA7, 0x3270};
FunctionHandle gDispIntroRaceHandle;

} // namespace Csis
