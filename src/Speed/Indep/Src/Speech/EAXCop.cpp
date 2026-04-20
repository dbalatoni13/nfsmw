#include "EAXCop.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"

namespace MiscSpeech {
bool IsVehicleTypeOK();
}

namespace Csis {
struct AnytimeEvents_Unit911ReplyStruct {
    int speaker_id;
};

struct Backup_UnitBUReplyStruct {
    int speaker_id;
};

struct Backup_NegativeBUReplyStruct {
    int speaker_id;
    int yes_no;
    int subject_battalion;
    int subject_call_sign_id;
};

struct Outcome_AnticipateSuccessStruct {
    int speaker_id;
};

struct Outcome_AnticipateFailStruct {
    int speaker_id;
};

struct Setup_BullhornPrefixStruct {
    int speaker_id;
};

struct ExtraCops_SwarmingReplyStruct {};

struct AnytimeEvents_PursuitUpdateRepStruct {
    int speaker_id;
};

struct AnytimeEvents_BailoutDenyStruct {
    int speaker_id;
};

struct StaticRoadblock_RBAvertedStruct {
    int speaker_id;
};

struct Backup_CallForSwarmingStruct {
    int speaker_id;
};

struct Setup_SpotterWantedStruct {
    int speaker_id;
};

struct AnytimeEvents_WeatherReportStruct {
    int speaker_id;
};

struct AnytimeEvents_BailoutStruct {
    int speaker_id;
    int bailout_type;
};

struct AnytimeEvents_SuspectBrakeStruct {
    int speaker_id;
};

struct Setup_SpotterStruct {
    int speaker_id;
    int num_suspects;
};

struct Setup_SpotterReplyStruct {
    int speaker_id;
    int num_suspects;
};

struct Setup_InitPursuitStruct {
    int speaker_id;
    int num_suspects;
};

struct AnytimeEvents_LostSuspectStruct {
    int speaker_id;
    int intensity;
};

struct Arrest_ArrestStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_LostVisualStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_RegainVisualStruct {
    int speaker_id;
    int intensity;
};

struct RollingStrategy_CallToPositionRemStruct {
    int speaker_id;
    int intensity;
};

struct RollingStrategy_StrategyExecuteStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_IntentToRamStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_SuspectBehaviourStruct {
    int speaker_id;
    int num_suspects;
};

struct Setup_SuspectConfirmedStruct {
    int speaker_id;
    int num_suspects;
};

struct AnytimeEvents_FocusChangeStruct {
    int speaker_id;
    int intensity;
};

struct StaticRoadblock_PursuitApproachingStruct {
    int speaker_id;
    int num_suspects;
};

struct StaticRoadblock_CallForRB_subStruct {
    int speaker_id;
};

struct AnytimeEvents_DirectionHighStruct {
    int speaker_id;
    int direction_type;
};

struct Setup_BullhornStruct {
    int speaker_id;
};

struct AnytimeEvents_SuspectOutrunStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_SuspectUTurnStruct {
    int speaker_id;
    int intensity;
};

struct StaticRoadblock_NegativeRBReplyStruct {
    int speaker_id;
};

struct StaticRoadblock_RBReminderStruct {
    int speaker_id;
    int code_type;
};

struct StaticRoadblock_CallForRBStruct {
    int speaker_id;
    int code_type;
    int roadblock_type;
};

struct Setup_PrimaryEngageStruct {
    int speaker_id;
    int callsign;
    int unit_number;
};

struct Backup_BUArrivesStruct {
    int speaker_id;
    int callsign;
    int unit_number;
};

struct AnytimeEvents_SpottedStruct {
    int speaker_id;
    int intensity;
};

struct Interrupts_InterruptRam_HOStruct {
    int speaker_id;
    int intensity;
};

struct Interrupts_InterruptRam_REStruct {
    int speaker_id;
    int intensity;
};

struct Interrupts_InterruptRam_SSStruct {
    int speaker_id;
    int intensity;
};

struct Interrupts_InterruptRam_TBStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_CollWorld_FlipStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_CollWorld_AirStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_CollWorld_SpinStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_CollWorld_CiviStruct {
    int speaker_id;
    int intensity;
};

struct Arrest_BullhornArrestStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_OffroadMomentStruct {
    int speaker_id;
    int offroad_id;
    int offroad_type;
};

struct AnytimeEvents_UnitDisabledStruct {
    int speaker_id;
    int intensity;
    int disabled_type;
};

struct Outcome_OutcomeFailStruct {
    int speaker_id;
    int intensity;
};

struct Backup_BUReminderStruct {
    int speaker_id;
    int code_type;
    int backup_type;
};

struct Backup_CallForBUStruct {
    int speaker_id;
    int code_type;
    int backup_type;
};

struct Outcome_StrategyResetStruct {
    int speaker_id;
    int strategy_type;
    int intensity;
};

struct StaticRoadblock_RBEngageStruct {
    int speaker_id;
    int engage_type;
};

extern InterfaceId AnytimeEvents_Unit911ReplyId;
extern InterfaceId Backup_UnitBUReplyId;
extern InterfaceId Backup_NegativeBUReplyId;
extern InterfaceId Outcome_AnticipateSuccessId;
extern InterfaceId Outcome_AnticipateFailId;
extern InterfaceId Setup_BullhornPrefixId;
extern InterfaceId ExtraCops_SwarmingReplyId;
extern InterfaceId AnytimeEvents_PursuitUpdateRepId;
extern InterfaceId AnytimeEvents_BailoutDenyId;
extern InterfaceId StaticRoadblock_RBAvertedId;
extern InterfaceId Backup_CallForSwarmingId;
extern InterfaceId Setup_SpotterWantedId;
extern InterfaceId AnytimeEvents_WeatherReportId;
extern InterfaceId AnytimeEvents_BailoutId;
extern InterfaceId AnytimeEvents_SuspectBrakeId;
extern InterfaceId Setup_SpotterId;
extern InterfaceId Setup_SpotterReplyId;
extern InterfaceId Setup_InitPursuitId;
extern InterfaceId AnytimeEvents_LostSuspectId;
extern InterfaceId Arrest_ArrestId;
extern InterfaceId AnytimeEvents_LostVisualId;
extern InterfaceId AnytimeEvents_RegainVisualId;
extern InterfaceId RollingStrategy_CallToPositionRemId;
extern InterfaceId RollingStrategy_StrategyExecuteId;
extern InterfaceId AnytimeEvents_IntentToRamId;
extern InterfaceId AnytimeEvents_SuspectBehaviourId;
extern InterfaceId Setup_SuspectConfirmedId;
extern InterfaceId AnytimeEvents_FocusChangeId;
extern InterfaceId StaticRoadblock_PursuitApproachingId;
extern InterfaceId StaticRoadblock_CallForRB_subId;
extern InterfaceId AnytimeEvents_DirectionHighId;
extern InterfaceId Setup_BullhornId;
extern InterfaceId AnytimeEvents_SuspectOutrunId;
extern InterfaceId AnytimeEvents_SuspectUTurnId;
extern InterfaceId StaticRoadblock_NegativeRBReplyId;
extern InterfaceId StaticRoadblock_RBReminderId;
extern InterfaceId StaticRoadblock_CallForRBId;
extern InterfaceId Setup_PrimaryEngageId;
extern InterfaceId Backup_BUArrivesId;
extern InterfaceId AnytimeEvents_SpottedId;
extern InterfaceId Interrupts_InterruptRam_HOId;
extern InterfaceId Interrupts_InterruptRam_REId;
extern InterfaceId Interrupts_InterruptRam_SSId;
extern InterfaceId Interrupts_InterruptRam_TBId;
extern InterfaceId AnytimeEvents_CollWorld_FlipId;
extern InterfaceId AnytimeEvents_CollWorld_AirId;
extern InterfaceId AnytimeEvents_CollWorld_SpinId;
extern InterfaceId AnytimeEvents_CollWorld_CiviId;
extern InterfaceId Arrest_BullhornArrestId;
extern InterfaceId AnytimeEvents_OffroadMomentId;
extern InterfaceId AnytimeEvents_UnitDisabledId;
extern InterfaceId Outcome_OutcomeFailId;
extern InterfaceId Backup_BUReminderId;
extern InterfaceId Backup_CallForBUId;
extern InterfaceId Outcome_StrategyResetId;
extern InterfaceId StaticRoadblock_RBEngageId;

extern FunctionHandle gAnytimeEvents_Unit911ReplyHandle;
extern FunctionHandle gBackup_UnitBUReplyHandle;
extern FunctionHandle gBackup_NegativeBUReplyHandle;
extern FunctionHandle gOutcome_AnticipateSuccessHandle;
extern FunctionHandle gOutcome_AnticipateFailHandle;
extern FunctionHandle gSetup_BullhornPrefixHandle;
extern FunctionHandle gExtraCops_SwarmingReplyHandle;
extern FunctionHandle gAnytimeEvents_PursuitUpdateRepHandle;
extern FunctionHandle gAnytimeEvents_BailoutDenyHandle;
extern FunctionHandle gStaticRoadblock_RBAvertedHandle;
extern FunctionHandle gBackup_CallForSwarmingHandle;
extern FunctionHandle gSetup_SpotterWantedHandle;
extern FunctionHandle gAnytimeEvents_WeatherReportHandle;
extern FunctionHandle gAnytimeEvents_BailoutHandle;
extern FunctionHandle gAnytimeEvents_SuspectBrakeHandle;
extern FunctionHandle gSetup_SpotterHandle;
extern FunctionHandle gSetup_SpotterReplyHandle;
extern FunctionHandle gSetup_InitPursuitHandle;
extern FunctionHandle gAnytimeEvents_LostSuspectHandle;
extern FunctionHandle gArrest_ArrestHandle;
extern FunctionHandle gAnytimeEvents_LostVisualHandle;
extern FunctionHandle gAnytimeEvents_RegainVisualHandle;
extern FunctionHandle gRollingStrategy_CallToPositionRemHandle;
extern FunctionHandle gRollingStrategy_StrategyExecuteHandle;
extern FunctionHandle gAnytimeEvents_IntentToRamHandle;
extern FunctionHandle gAnytimeEvents_SuspectBehaviourHandle;
extern FunctionHandle gSetup_SuspectConfirmedHandle;
extern FunctionHandle gAnytimeEvents_FocusChangeHandle;
extern FunctionHandle gStaticRoadblock_PursuitApproachingHandle;
extern FunctionHandle gStaticRoadblock_CallForRB_subHandle;
extern FunctionHandle gAnytimeEvents_DirectionHighHandle;
extern FunctionHandle gSetup_BullhornHandle;
extern FunctionHandle gAnytimeEvents_SuspectOutrunHandle;
extern FunctionHandle gAnytimeEvents_SuspectUTurnHandle;
extern FunctionHandle gStaticRoadblock_NegativeRBReplyHandle;
extern FunctionHandle gStaticRoadblock_RBReminderHandle;
extern FunctionHandle gStaticRoadblock_CallForRBHandle;
extern FunctionHandle gSetup_PrimaryEngageHandle;
extern FunctionHandle gBackup_BUArrivesHandle;
extern FunctionHandle gAnytimeEvents_SpottedHandle;
extern FunctionHandle gInterrupts_InterruptRam_HOHandle;
extern FunctionHandle gInterrupts_InterruptRam_REHandle;
extern FunctionHandle gInterrupts_InterruptRam_SSHandle;
extern FunctionHandle gInterrupts_InterruptRam_TBHandle;
extern FunctionHandle gAnytimeEvents_CollWorld_FlipHandle;
extern FunctionHandle gAnytimeEvents_CollWorld_AirHandle;
extern FunctionHandle gAnytimeEvents_CollWorld_SpinHandle;
extern FunctionHandle gAnytimeEvents_CollWorld_CiviHandle;
extern FunctionHandle gArrest_BullhornArrestHandle;
extern FunctionHandle gAnytimeEvents_OffroadMomentHandle;
extern FunctionHandle gAnytimeEvents_UnitDisabledHandle;
extern FunctionHandle gOutcome_OutcomeFailHandle;
extern FunctionHandle gBackup_BUReminderHandle;
extern FunctionHandle gBackup_CallForBUHandle;
extern FunctionHandle gOutcome_StrategyResetHandle;
extern FunctionHandle gStaticRoadblock_RBEngageHandle;
}

extern void ScheduleSpeech_AnytimeEvents_Unit911Reply(Csis::AnytimeEvents_Unit911ReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32AnytimeEvents_Unit911ReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_UnitBUReply(Csis::Backup_UnitBUReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis24Backup_UnitBUReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_NegativeBUReply(Csis::Backup_NegativeBUReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Backup_NegativeBUReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Outcome_AnticipateSuccess(Csis::Outcome_AnticipateSuccessStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31Outcome_AnticipateSuccessStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Outcome_AnticipateFail(Csis::Outcome_AnticipateFailStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Outcome_AnticipateFailStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_BullhornPrefix(Csis::Setup_BullhornPrefixStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis26Setup_BullhornPrefixStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_ExtraCops_SwarmingReply(Csis::ExtraCops_SwarmingReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis29ExtraCops_SwarmingReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_PursuitUpdateRep(Csis::AnytimeEvents_PursuitUpdateRepStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis36AnytimeEvents_PursuitUpdateRepStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_BailoutDeny(Csis::AnytimeEvents_BailoutDenyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31AnytimeEvents_BailoutDenyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_RBAverted(Csis::StaticRoadblock_RBAvertedStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31StaticRoadblock_RBAvertedStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_CallForSwarming(Csis::Backup_CallForSwarmingStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Backup_CallForSwarmingStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_SpotterWanted(Csis::Setup_SpotterWantedStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis25Setup_SpotterWantedStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_WeatherReport(Csis::AnytimeEvents_WeatherReportStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_WeatherReportStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_Bailout(Csis::AnytimeEvents_BailoutStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis27AnytimeEvents_BailoutStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_SuspectBrake(Csis::AnytimeEvents_SuspectBrakeStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32AnytimeEvents_SuspectBrakeStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_Spotter(Csis::Setup_SpotterStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis19Setup_SpotterStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_SpotterReply(Csis::Setup_SpotterReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis24Setup_SpotterReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_InitPursuit(Csis::Setup_InitPursuitStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis23Setup_InitPursuitStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_LostSuspect(Csis::AnytimeEvents_LostSuspectStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31AnytimeEvents_LostSuspectStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Arrest_Arrest(Csis::Arrest_ArrestStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis19Arrest_ArrestStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_LostVisual(Csis::AnytimeEvents_LostVisualStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis30AnytimeEvents_LostVisualStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_RegainVisual(Csis::AnytimeEvents_RegainVisualStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32AnytimeEvents_RegainVisualStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_RollingStrategy_CallToPositionRem(Csis::RollingStrategy_CallToPositionRemStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis39RollingStrategy_CallToPositionRemStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_RollingStrategy_StrategyExecute(Csis::RollingStrategy_StrategyExecuteStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis37RollingStrategy_StrategyExecuteStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_IntentToRam(Csis::AnytimeEvents_IntentToRamStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31AnytimeEvents_IntentToRamStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_SuspectBehaviour(Csis::AnytimeEvents_SuspectBehaviourStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis36AnytimeEvents_SuspectBehaviourStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_SuspectConfirmed(Csis::Setup_SuspectConfirmedStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Setup_SuspectConfirmedStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_FocusChange(Csis::AnytimeEvents_FocusChangeStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31AnytimeEvents_FocusChangeStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_PursuitApproaching(Csis::StaticRoadblock_PursuitApproachingStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis40StaticRoadblock_PursuitApproachingStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_CallForRB_sub(Csis::StaticRoadblock_CallForRB_subStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis34StaticRoadblock_CallForRB_subStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_DirectionHigh(Csis::AnytimeEvents_DirectionHighStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_DirectionHighStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_Bullhorn(Csis::Setup_BullhornStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis20Setup_BullhornStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_SuspectOutrun(Csis::AnytimeEvents_SuspectOutrunStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_SuspectOutrunStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_SuspectUTurn(Csis::AnytimeEvents_SuspectUTurnStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_SuspectUTurnStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_NegativeRBReply(Csis::StaticRoadblock_NegativeRBReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis37StaticRoadblock_NegativeRBReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_RBReminder(Csis::StaticRoadblock_RBReminderStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32StaticRoadblock_RBReminderStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_CallForRB(Csis::StaticRoadblock_CallForRBStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31StaticRoadblock_CallForRBStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_PrimaryEngage(Csis::Setup_PrimaryEngageStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis25Setup_PrimaryEngageStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_BUArrives(Csis::Backup_BUArrivesStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis22Backup_BUArrivesStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_Spotted(Csis::AnytimeEvents_SpottedStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis27AnytimeEvents_SpottedStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Interrupts_InterruptRam_HO(Csis::Interrupts_InterruptRam_HOStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32Interrupts_InterruptRam_HOStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Interrupts_InterruptRam_RE(Csis::Interrupts_InterruptRam_REStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32Interrupts_InterruptRam_REStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Interrupts_InterruptRam_SS(Csis::Interrupts_InterruptRam_SSStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32Interrupts_InterruptRam_SSStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Interrupts_InterruptRam_TB(Csis::Interrupts_InterruptRam_TBStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32Interrupts_InterruptRam_TBStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_CollWorld_Flip(Csis::AnytimeEvents_CollWorld_FlipStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis34AnytimeEvents_CollWorld_FlipStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_CollWorld_Air(Csis::AnytimeEvents_CollWorld_AirStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_CollWorld_AirStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_CollWorld_Spin(Csis::AnytimeEvents_CollWorld_SpinStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis34AnytimeEvents_CollWorld_SpinStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_CollWorld_Civi(Csis::AnytimeEvents_CollWorld_CiviStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis34AnytimeEvents_CollWorld_CiviStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Arrest_BullhornArrest(Csis::Arrest_BullhornArrestStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis27Arrest_BullhornArrestStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_OffroadMoment(Csis::AnytimeEvents_OffroadMomentStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_OffroadMomentStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_UnitDisabled(Csis::AnytimeEvents_UnitDisabledStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32AnytimeEvents_UnitDisabledStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Outcome_OutcomeFail(Csis::Outcome_OutcomeFailStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis25Outcome_OutcomeFailStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_BUReminder(Csis::Backup_BUReminderStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis23Backup_BUReminderStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_CallForBU(Csis::Backup_CallForBUStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis22Backup_CallForBUStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Outcome_StrategyReset(Csis::Outcome_StrategyResetStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis27Outcome_StrategyResetStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_RBEngage(Csis::StaticRoadblock_RBEngageStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis30StaticRoadblock_RBEngageStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");

EAXCop::EAXCop(int speakerID, HSIMABLE handle, int bID, int cID)
    : EAXCharacter(speakerID, handle, bID, cID) {
    ISimable *simable;

    mPctTractiveTires = 0.0f;
    mRank = 0;
    *reinterpret_cast<unsigned int *>(&mInFormation) = 0;
    *reinterpret_cast<unsigned int *>(&mInPosition) = 0;
    *reinterpret_cast<unsigned int *>(&mAhead) = 0;
    mTimeAirborne = WorldTimer;
    mTrafficHitCount = 0;
    mTimeNoLOS = WorldTimer;
    mT_closingDist = 0;
    mCurrRoad = MAX_ROADNAMES;
    mTgtOffset.x = UMath::Vector3::kZero.x;
    mNumRammed = 0;
    mLastRammedTime = 0;
    mT_lastactivity = 0;
    mOrigin = MAX_ROADNAMES;
    mTgtOffset.z = UMath::Vector3::kZero.z;
    mTgtOffset.y = UMath::Vector3::kZero.y;
    simable = ISimable::FindInstance(handle);
    if (simable) {
        IVehicleAI *vai;
        if (simable->QueryInterface(&vai) && vai) {
            WRoadNav *nav = vai->GetDriveToNav();
            if (nav) {
                unsigned int road_id = nav->GetRoadSpeechId();
                if (road_id != 0x6B) {
                    mOrigin = static_cast<RoadNames>(road_id);
                }
            }
        }
    }
}

EAXCop::~EAXCop() {}

int EAXCop::GetBackupTypeFromDispatch(int type) {
    return type;
}

void EAXCop::Update() {
    EAXCharacter::Update();
}

void EAXCop::SetActive(bool activity) {
    EAXCharacter::SetActive(activity);
}

void EAXCop::Reset() {
    mInFormation = false;
    mInPosition = false;
    mPctTractiveTires = 0.0f;
    *reinterpret_cast<unsigned int *>(&mTimeAirborne) = 0;
    mTrafficHitCount = 0;
    *reinterpret_cast<unsigned int *>(&mTimeNoLOS) = 0;
    mNumRammed = 0;
    *reinterpret_cast<unsigned int *>(&mLastRammedTime) = 0;
    *reinterpret_cast<unsigned int *>(&mT_lastactivity) = 0;
    *reinterpret_cast<unsigned int *>(&mT_closingDist) = 0;
    EAXCharacter::Reset();
}

void EAXCop::AttemptVehicleStop() {}

void EAXCop::VehicleReport() {}

void EAXCop::InitiatePursuit() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::Setup_InitPursuitStruct data;
        int num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech_Setup_InitPursuit(data, Csis::Setup_InitPursuitId, Csis::gSetup_InitPursuitHandle, this);
    }
}

void EAXCop::LocationReport() {}

void EAXCop::SelfStrategy(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::InitialCallForBackup() {}

void EAXCop::CallForBackup(int type) {
    register EAXCop *self = this;
    register int backup_type = type;
    if (!backup_type) {
        return;
    }
    Csis::Backup_CallForBUStruct data;
    data.code_type = self->GetRandomizedCode();
    data.backup_type = self->GetBackupTypeFromDispatch(backup_type);
    data.speaker_id = self->mSpeakerID;
    ScheduleSpeech_Backup_CallForBU(data, Csis::Backup_CallForBUId, Csis::gBackup_CallForBUHandle, self);
}

void EAXCop::UnitBackupReply() {
    Csis::Backup_UnitBUReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Backup_UnitBUReply(data, Csis::Backup_UnitBUReplyId, Csis::gBackup_UnitBUReplyHandle, this);
}

void EAXCop::InitiateStrategy(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::CallToPosition(EAXCop *cop) {
    if (cop) {
        mTgtOffset = cop->mTgtOffset;
    }
}

void EAXCop::CallToPositionReminder() {
    Csis::RollingStrategy_CallToPositionRemStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_RollingStrategy_CallToPositionRem(data, Csis::RollingStrategy_CallToPositionRemId, Csis::gRollingStrategy_CallToPositionRemHandle, this);
}

void EAXCop::StrategyExecute() {
    Csis::RollingStrategy_StrategyExecuteStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_RollingStrategy_StrategyExecute(data, Csis::RollingStrategy_StrategyExecuteId, Csis::gRollingStrategy_StrategyExecuteHandle, this);
}

void EAXCop::Collision(int collisionType, float force, EAXCop *spkr) {
    if (force > 0.0f || spkr) {
        mLastEvent = collisionType;
    }
}

void EAXCop::AnticipateSuccess() {
    Csis::Outcome_AnticipateSuccessStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Outcome_AnticipateSuccess(data, Csis::Outcome_AnticipateSuccessId, Csis::gOutcome_AnticipateSuccessHandle, this);
}

void EAXCop::AnticipateFail() {
    Csis::Outcome_AnticipateFailStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Outcome_AnticipateFail(data, Csis::Outcome_AnticipateFailId, Csis::gOutcome_AnticipateFailHandle, this);
}

void EAXCop::OutcomeFail(short intensity) {
    Csis::Outcome_OutcomeFailStruct data;
    SoundAI *ai = SoundAI::Get();
    if (intensity < 0) {
        register int new_intensity;
        if (ai->IsHighIntensity()) {
            new_intensity = 2;
        } else {
            new_intensity = 1;
        }
        data.intensity = new_intensity;
    } else {
        data.intensity = intensity;
    }
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Outcome_OutcomeFail(data, Csis::Outcome_OutcomeFailId, Csis::gOutcome_OutcomeFailHandle, this);
}

void EAXCop::StrategyReset(bool new_strategy) {
    Csis::Outcome_StrategyResetStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    if (new_strategy) {
        data.strategy_type = 2;
    } else {
        data.strategy_type = 1;
    }
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Outcome_StrategyReset(data, Csis::Outcome_StrategyResetId, Csis::gOutcome_StrategyResetHandle, this);
}

void EAXCop::SuspectBehavior() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::AnytimeEvents_SuspectBehaviourStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_AnytimeEvents_SuspectBehaviour(data, Csis::AnytimeEvents_SuspectBehaviourId, Csis::gAnytimeEvents_SuspectBehaviourHandle, this);
    }
}

void EAXCop::SuspectOutrun() {
    Csis::AnytimeEvents_SuspectOutrunStruct data;
    data.speaker_id = mSpeakerID;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_SuspectOutrun(data, Csis::AnytimeEvents_SuspectOutrunId, Csis::gAnytimeEvents_SuspectOutrunHandle, this);
}

void EAXCop::SuspectUTurn() {
    Csis::AnytimeEvents_SuspectUTurnStruct data;
    data.speaker_id = mSpeakerID;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_SuspectUTurn(data, Csis::AnytimeEvents_SuspectUTurnId, Csis::gAnytimeEvents_SuspectUTurnHandle, this);
}

void EAXCop::LostSuspect() {
    Csis::AnytimeEvents_LostSuspectStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_LostSuspect(data, Csis::AnytimeEvents_LostSuspectId, Csis::gAnytimeEvents_LostSuspectHandle, this);
}

void EAXCop::LostVisual() {
    Csis::AnytimeEvents_LostVisualStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_LostVisual(data, Csis::AnytimeEvents_LostVisualId, Csis::gAnytimeEvents_LostVisualHandle, this);
}

void EAXCop::RegainVisual() {
    Csis::AnytimeEvents_RegainVisualStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_RegainVisual(data, Csis::AnytimeEvents_RegainVisualId, Csis::gAnytimeEvents_RegainVisualHandle, this);
}

void EAXCop::SwarmingReply() {
    Csis::ExtraCops_SwarmingReplyStruct data;
    ScheduleSpeech_ExtraCops_SwarmingReply(data, Csis::ExtraCops_SwarmingReplyId, Csis::gExtraCops_SwarmingReplyHandle, this);
}

void EAXCop::Arrest() {
    Csis::Arrest_ArrestStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Arrest_Arrest(data, Csis::Arrest_ArrestId, Csis::gArrest_ArrestHandle, this);
}

void EAXCop::PursuitUpdateReply() {
    Csis::AnytimeEvents_PursuitUpdateRepStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_PursuitUpdateRep(data, Csis::AnytimeEvents_PursuitUpdateRepId, Csis::gAnytimeEvents_PursuitUpdateRepHandle, this);
}

void EAXCop::ReinitiatePursuit() {}

void EAXCop::NegativeBackupReply() {
    Csis::Backup_NegativeBUReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Backup_NegativeBUReply(data, Csis::Backup_NegativeBUReplyId, Csis::gBackup_NegativeBUReplyHandle, this);
}

void EAXCop::BackupReminder(int type) {
    register EAXCop *self = this;
    register int backup_type = type;
    if (!backup_type) {
        return;
    }
    Csis::Backup_BUReminderStruct data;
    data.code_type = self->GetRandomizedCode();
    data.backup_type = self->GetBackupTypeFromDispatch(backup_type);
    data.speaker_id = self->mSpeakerID;
    ScheduleSpeech_Backup_BUReminder(data, Csis::Backup_BUReminderId, Csis::gBackup_BUReminderHandle, self);
}

void EAXCop::BackupArrives() {
    Csis::Backup_BUArrivesStruct data;
    data.speaker_id = mSpeakerID;
    data.callsign = GetCallsign();
    data.unit_number = GetUnitNumber();
    ScheduleSpeech_Backup_BUArrives(data, Csis::Backup_BUArrivesId, Csis::gBackup_BUArrivesHandle, this);
}

void EAXCop::IntentToRam() {
    Csis::AnytimeEvents_IntentToRamStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_IntentToRam(data, Csis::AnytimeEvents_IntentToRamId, Csis::gAnytimeEvents_IntentToRamHandle, this);
}

void EAXCop::CallforEV(unsigned int type) {
    if (type) {
        mLastEvent = static_cast<int>(type);
    }
}

void EAXCop::UnitDisabled(int other) {
    Csis::AnytimeEvents_UnitDisabledStruct data;
    SoundAI *ai = SoundAI::Get();
    int disabled_type = 1;
    int intensity;
    if (other) {
        disabled_type = 2;
    }
    data.disabled_type = disabled_type;
    if (ai->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_UnitDisabled(data, Csis::AnytimeEvents_UnitDisabledId, Csis::gAnytimeEvents_UnitDisabledHandle, this);
}

void EAXCop::Bailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 8;
            ScheduleSpeech_AnytimeEvents_Bailout(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::LoBailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 1;
            ScheduleSpeech_AnytimeEvents_Bailout(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::HiBailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 0x10;
            ScheduleSpeech_AnytimeEvents_Bailout(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::BailoutTraffic() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 2;
            ScheduleSpeech_AnytimeEvents_Bailout(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::BailoutBadRoad() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 4;
            ScheduleSpeech_AnytimeEvents_Bailout(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::Spotter() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::Setup_SpotterStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_Setup_Spotter(data, Csis::Setup_SpotterId, Csis::gSetup_SpotterHandle, this);
    }
}

void EAXCop::SpotterReply() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::Setup_SpotterReplyStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_Setup_SpotterReply(data, Csis::Setup_SpotterReplyId, Csis::gSetup_SpotterReplyHandle, this);
    }
}

void EAXCop::Reply911() {
    Csis::AnytimeEvents_Unit911ReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_Unit911Reply(data, Csis::AnytimeEvents_Unit911ReplyId, Csis::gAnytimeEvents_Unit911ReplyHandle, this);
}

void EAXCop::DenyBailout() {
    Csis::AnytimeEvents_BailoutDenyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_BailoutDeny(data, Csis::AnytimeEvents_BailoutDenyId, Csis::gAnytimeEvents_BailoutDenyHandle, this);
}

void EAXCop::PrimaryEngage() {
    Csis::Setup_PrimaryEngageStruct data;
    data.speaker_id = mSpeakerID;
    data.callsign = GetCallsign();
    data.unit_number = GetUnitNumber();
    ScheduleSpeech_Setup_PrimaryEngage(data, Csis::Setup_PrimaryEngageId, Csis::gSetup_PrimaryEngageHandle, this);
}

void EAXCop::Bullhorn() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        Csis::Setup_BullhornStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech_Setup_Bullhorn(data, Csis::Setup_BullhornId, Csis::gSetup_BullhornHandle, this);
    }
}

void EAXCop::PreBullhorn() {
    Csis::Setup_BullhornPrefixStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Setup_BullhornPrefix(data, Csis::Setup_BullhornPrefixId, Csis::gSetup_BullhornPrefixHandle, this);
}

void EAXCop::BullhornArrest() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        SoundAI *ai = SoundAI::Get();
        if (ai) {
            Csis::Arrest_BullhornArrestStruct data;
            data.speaker_id = mSpeakerID;
            register int intensity;
            if (ai->IsHighIntensity()) {
                intensity = 2;
            } else {
                intensity = 1;
            }
            data.intensity = intensity;
            ScheduleSpeech_Arrest_BullhornArrest(data, Csis::Arrest_BullhornArrestId, Csis::gArrest_BullhornArrestHandle, this);
        }
    }
}

void EAXCop::SuspectConfirmed() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::Setup_SuspectConfirmedStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_Setup_SuspectConfirmed(data, Csis::Setup_SuspectConfirmedId, Csis::gSetup_SuspectConfirmedHandle, this);
    }
}

void EAXCop::FocusChange() {
    Csis::AnytimeEvents_FocusChangeStruct data;
    int intensity;
    data.speaker_id = mSpeakerID;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_FocusChange(data, Csis::AnytimeEvents_FocusChangeId, Csis::gAnytimeEvents_FocusChangeHandle, this);
}

extern "C" float lbl_804074E8;

void EAXCop::Spotted() {
    Csis::AnytimeEvents_SpottedStruct data;
    data.speaker_id = mSpeakerID;
    int intensity;
    if (GetSpeed() > lbl_804074E8) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_Spotted(data, Csis::AnytimeEvents_SpottedId, Csis::gAnytimeEvents_SpottedHandle, this);
}

void EAXCop::DirectionChange() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        int direction_type = *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x1AC);
        if (direction_type) {
            Csis::AnytimeEvents_DirectionHighStruct data;
            data.speaker_id = mSpeakerID;
            data.direction_type = direction_type;
            ScheduleSpeech_AnytimeEvents_DirectionHigh(data, Csis::AnytimeEvents_DirectionHighId, Csis::gAnytimeEvents_DirectionHighHandle, this);
        }
    }
}

void EAXCop::CallForSwarming() {
    Csis::Backup_CallForSwarmingStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Backup_CallForSwarming(data, Csis::Backup_CallForSwarmingId, Csis::gBackup_CallForSwarmingHandle, this);
}

void EAXCop::SpotterWanted() {
    Csis::Setup_SpotterWantedStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Setup_SpotterWanted(data, Csis::Setup_SpotterWantedId, Csis::gSetup_SpotterWantedHandle, this);
}

void EAXCop::Offroad(unsigned int id, bool subsequent) {
    Csis::AnytimeEvents_OffroadMomentStruct data;
    data.offroad_id = static_cast<int>(id);
    data.speaker_id = mSpeakerID;
    data.offroad_type = subsequent ? 2 : 1;
    ScheduleSpeech_AnytimeEvents_OffroadMoment(data, Csis::AnytimeEvents_OffroadMomentId, Csis::gAnytimeEvents_OffroadMomentHandle, this);
}

void EAXCop::WeatherReport() {
    Csis::AnytimeEvents_WeatherReportStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_WeatherReport(data, Csis::AnytimeEvents_WeatherReportId, Csis::gAnytimeEvents_WeatherReportHandle, this);
}

void EAXCop::CallForRB() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::StaticRoadblock_CallForRBStruct data;
        int roadblock_type;
        data.speaker_id = mSpeakerID;
        data.code_type = GetRandomizedCode();
        roadblock_type = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 4) != 0) {
            roadblock_type = 2;
        }
        data.roadblock_type = roadblock_type;
        ScheduleSpeech_StaticRoadblock_CallForRB(data, Csis::StaticRoadblock_CallForRBId, Csis::gStaticRoadblock_CallForRBHandle, this);
    }
}

void EAXCop::RBReminder() {
    Csis::StaticRoadblock_RBReminderStruct data;
    data.speaker_id = mSpeakerID;
    data.code_type = GetRandomizedCode();
    ScheduleSpeech_StaticRoadblock_RBReminder(data, Csis::StaticRoadblock_RBReminderId, Csis::gStaticRoadblock_RBReminderHandle, this);
}

void EAXCop::NegRBReply() {
    if (!IsHeli()) {
        Csis::StaticRoadblock_NegativeRBReplyStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech_StaticRoadblock_NegativeRBReply(data, Csis::StaticRoadblock_NegativeRBReplyId, Csis::gStaticRoadblock_NegativeRBReplyHandle, this);
    }
}

void EAXCop::RBApproach() {}

void EAXCop::RBEngage(bool spikes_hit) {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (ai->GetRoadblock()) {
            Csis::StaticRoadblock_RBEngageStruct data;
            int engage_type;
            data.speaker_id = mSpeakerID;
            engage_type = 1;
            if (spikes_hit) {
                engage_type = 2;
            }
            data.engage_type = engage_type;
            ScheduleSpeech_StaticRoadblock_RBEngage(data, Csis::StaticRoadblock_RBEngageId, Csis::gStaticRoadblock_RBEngageHandle, this);
        }
    }
}

void EAXCop::PursuitApproaching() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::StaticRoadblock_PursuitApproachingStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_StaticRoadblock_PursuitApproaching(data, Csis::StaticRoadblock_PursuitApproachingId, Csis::gStaticRoadblock_PursuitApproachingHandle, this);
    }
}

void EAXCop::RBAverted() {
    Csis::StaticRoadblock_RBAvertedStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_StaticRoadblock_RBAverted(data, Csis::StaticRoadblock_RBAvertedId, Csis::gStaticRoadblock_RBAvertedHandle, this);
}

void EAXCop::CallForSubRB() {
    if (SoundAI::Get()) {
        Csis::StaticRoadblock_CallForRB_subStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech_StaticRoadblock_CallForRB_sub(data, Csis::StaticRoadblock_CallForRB_subId, Csis::gStaticRoadblock_CallForRB_subHandle, this);
    }
}

void EAXCop::RearEnded(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_REStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech_Interrupts_InterruptRam_RE(data, Csis::Interrupts_InterruptRam_REId, Csis::gInterrupts_InterruptRam_REHandle, this);
}

void EAXCop::HeadOn(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_HOStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech_Interrupts_InterruptRam_HO(data, Csis::Interrupts_InterruptRam_HOId, Csis::gInterrupts_InterruptRam_HOHandle, this);
}

void EAXCop::SideSwiped(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_SSStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech_Interrupts_InterruptRam_SS(data, Csis::Interrupts_InterruptRam_SSId, Csis::gInterrupts_InterruptRam_SSHandle, this);
}

void EAXCop::TBoned(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_TBStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech_Interrupts_InterruptRam_TB(data, Csis::Interrupts_InterruptRam_TBId, Csis::gInterrupts_InterruptRam_TBHandle, this);
}

void EAXCop::SuspectRollover(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_FlipStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_CollWorld_Flip(data, Csis::AnytimeEvents_CollWorld_FlipId, Csis::gAnytimeEvents_CollWorld_FlipHandle, this);
}

void EAXCop::SuspectAirborne(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_AirStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_CollWorld_Air(data, Csis::AnytimeEvents_CollWorld_AirId, Csis::gAnytimeEvents_CollWorld_AirHandle, this);
}

void EAXCop::SuspectSpunout(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_SpinStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_CollWorld_Spin(data, Csis::AnytimeEvents_CollWorld_SpinId, Csis::gAnytimeEvents_CollWorld_SpinHandle, this);
}

void EAXCop::SuspectBrake() {
    Csis::AnytimeEvents_SuspectBrakeStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_SuspectBrake(data, Csis::AnytimeEvents_SuspectBrakeId, Csis::gAnytimeEvents_SuspectBrakeHandle, this);
}

void EAXCop::SwapVoices(EAXCop *cop) {
    if (cop) {
        int callsign = mCallsign.name;
        int unit = mCallsign.number;
        mCallsign.name = cop->mCallsign.name;
        mCallsign.number = cop->mCallsign.number;
        cop->mCallsign.name = callsign;
        cop->mCallsign.number = unit;
    }
}

bool EAXCop::IsPrimary() {
    return mRank == 0;
}

bool EAXCop::SetRank(int newrank) {
    bool did_reset = false;
    if (newrank == 0 && mRank > 0) {
        Csis::Setup_PrimaryEngageStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech_Setup_PrimaryEngage(data, Csis::Setup_PrimaryEngageId, Csis::gSetup_PrimaryEngageHandle, this);
        Reset();
        did_reset = true;
    }
    mRank = newrank;
    return did_reset;
}

void EAXCop::Impact_Suspect_World() {}

void EAXCop::Impact_Suspect_Semi() {}

void EAXCop::Impact_Suspect_Train() {}

void EAXCop::Impact_Suspect_Guardrail() {}

void EAXCop::Impact_Suspect_GasStation() {}

void EAXCop::Impact_Suspect_Spikebelt() {}

void EAXCop::Impact_Suspect_Traffic(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_CiviStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_CollWorld_Civi(data, Csis::AnytimeEvents_CollWorld_CiviId, Csis::gAnytimeEvents_CollWorld_CiviHandle, this);
}
