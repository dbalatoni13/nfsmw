#ifndef SPEECH_CSIS_SPEECH_EVENTS_H
#define SPEECH_CSIS_SPEECH_EVENTS_H

#include "Speed/Indep/Src/EAXSound/SND_GEN/copspeech.hpp"
#include "csis/csis.h"

// Generated Csis speech event interface declarations (from DWARF).
// Variables are defined in EAXSound/Stream/SpeechManager.cpp.

namespace Csis {

// total size: 0xC
struct AcknowledgeStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
    Type_yes_no yes_no; // offset 0x8, size 0x4
};

// total size: 0x8
struct Setup_SpotterStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

// total size: 0x4
struct Setup_SpotterWantedStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct Setup_SpotterReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

// total size: 0x14
struct Setup_AttmptVehStpStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_pursuit_type pursuit_type; // offset 0x4, size 0x4
    Type_num_suspects num_suspects; // offset 0x8, size 0x4
    Type_speaker_battalion speaker_battalion; // offset 0xC, size 0x4
    Type_speaker_call_sign_id speaker_call_sign_id; // offset 0x10, size 0x4
};

// total size: 0x4
struct Setup_DispGoAheadStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0xC
struct Setup_PrimaryEngageStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_speaker_battalion speaker_battalion; // offset 0x4, size 0x4
    Type_speaker_call_sign_id speaker_call_sign_id; // offset 0x8, size 0x4
};

// total size: 0x8
struct Setup_InitPursuitStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

// total size: 0x8
struct Setup_SuspectConfirmedStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

// total size: 0xC
struct Setup_ReInitPursuitStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_time_since_lost time_since_lost; // offset 0x4, size 0x4
    Type_num_suspects num_suspects; // offset 0x8, size 0x4
};

// total size: 0x14
struct Setup_VehicleReportStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_car_color car_color; // offset 0x4, size 0x4
    Type_car_type car_type; // offset 0x8, size 0x4
    Type_speed speed; // offset 0xC, size 0x4
    Type_measurement measurement; // offset 0x10, size 0x4
};

// total size: 0xC
struct Setup_VehicleReportTagStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_car_color car_color; // offset 0x4, size 0x4
    Type_car_type car_type; // offset 0x8, size 0x4
};

// total size: 0xC
struct Setup_DispVehDescripStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_car_color car_color; // offset 0x4, size 0x4
    Type_car_type car_type; // offset 0x8, size 0x4
};

// total size: 0xC
struct Setup_DispVehDescripVinylsStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_car_color car_color; // offset 0x4, size 0x4
    Type_car_type car_type; // offset 0x8, size 0x4
};

// total size: 0x4
struct Setup_DispNoVehDescripStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct Setup_DispCustPaintStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_car_type car_type; // offset 0x4, size 0x4
};

// total size: 0x4
struct Setup_MoreDetailsStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x18
struct Setup_LocationReportStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
    Type_direction direction; // offset 0x8, size 0x4
    Type_encounter encounter; // offset 0xC, size 0x4
    Type_location_region location_region; // offset 0x10, size 0x4
    Type_location location; // offset 0x14, size 0x4
};

// total size: 0x4
struct Setup_BullhornPrefixStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct Setup_BullhornStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0xC
struct Setup_SelfStrategyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
    Type_self_strategy_type self_strategy_type; // offset 0x8, size 0x4
};

// total size: 0x8
struct Setup_InitialCallForBUStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
};

// total size: 0x4
struct Setup_InitialCallForBU_MSStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0xC
struct Backup_CallForBUStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
    Type_backup_type backup_type; // offset 0x8, size 0x4
};

// total size: 0x4
struct Backup_UnitBUReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x18
struct Backup_DispBackupReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_yes_no yes_no; // offset 0x4, size 0x4
    Type_subject_battalion subject_battalion; // offset 0x8, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0xC, size 0x4
    Type_code code; // offset 0x10, size 0x4
    Type_disp_backup_type disp_backup_type; // offset 0x14, size 0x4
};

// total size: 0x4
struct Backup_CallForSwarmingStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct Backup_DispBUETAStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_disp_backup_eta disp_backup_eta; // offset 0x4, size 0x4
};

// total size: 0x8
struct Backup_DispHeliBUETAStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_disp_backup_eta disp_backup_eta; // offset 0x4, size 0x4
};

// total size: 0xC
struct Backup_BUReminderStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
    Type_backup_type backup_type; // offset 0x8, size 0x4
};

// total size: 0x10
struct Backup_NegativeBUReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_yes_no yes_no; // offset 0x4, size 0x4
    Type_subject_battalion subject_battalion; // offset 0x8, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0xC, size 0x4
};

// total size: 0x8
struct Backup_DispBackupUpdateStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_yes_no yes_no; // offset 0x4, size 0x4
};

// total size: 0xC
struct Backup_BUArrivesStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_speaker_battalion speaker_battalion; // offset 0x4, size 0x4
    Type_speaker_call_sign_id speaker_call_sign_id; // offset 0x8, size 0x4
};

// total size: 0xC
struct StaticRoadblock_CallForRBStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
    Type_roadblock_type roadblock_type; // offset 0x8, size 0x4
};

// total size: 0x8
struct StaticRoadblock_RBReminderStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
};

// total size: 0x4
struct StaticRoadblock_NegativeRBReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x18
struct StaticRoadblock_DispRBReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
    Type_roadblock_type roadblock_type; // offset 0x8, size 0x4
    Type_yes_no yes_no; // offset 0xC, size 0x4
    Type_subject_battalion subject_battalion; // offset 0x10, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0x14, size 0x4
};

// total size: 0x10
struct StaticRoadblock_DispRBUpdateStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
    Type_yes_no yes_no; // offset 0x8, size 0x4
    Type_roadblock_type roadblock_type; // offset 0xC, size 0x4
};

// total size: 0x8
struct StaticRoadblock_PursuitApproachingStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

// total size: 0x8
struct StaticRoadblock_RBApproachStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_roadblock_type roadblock_type; // offset 0x4, size 0x4
};

// total size: 0x8
struct StaticRoadblock_RBEngageStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_roadblock_engage_type roadblock_engage_type; // offset 0x4, size 0x4
};

// total size: 0x4
struct StaticRoadblock_RBAvertedStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct StaticRoadblock_CallForRB_subStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct StaticRoadblock_DispSubRBStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0xC
struct Projectile_CallForSafetyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
    Type_projectile_type projectile_type; // offset 0x8, size 0x4
};

// total size: 0x8
struct Projectile_ProjectileLaunchStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_projectile_type projectile_type; // offset 0x4, size 0x4
};

// total size: 0x8
struct Projectile_ProjectileHitStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_units num_units; // offset 0x4, size 0x4
};

// total size: 0xC
struct Projectile_ProjectileMissStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_projectile_type projectile_type; // offset 0x4, size 0x4
    Type_num_units num_units; // offset 0x8, size 0x4
};

// total size: 0xC
struct RollingStrategy_InitStrategyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
    Type_rolling_strategy_type rolling_strategy_type; // offset 0x8, size 0x4
};

// total size: 0x14
struct RollingStrategy_CallToPositionStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
    Type_position position; // offset 0x8, size 0x4
    Type_subject_battalion subject_battalion; // offset 0xC, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0x10, size 0x4
};

// total size: 0x8
struct RollingStrategy_CallToPositionRemStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct RollingStrategy_StrategyExecuteStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x4
struct Outcome_AnticipateFailStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct Outcome_AnticipateSuccessStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct Outcome_OutcomeFailStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0xC
struct Outcome_StrategyResetStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_same_new same_new; // offset 0x4, size 0x4
    Type_intensity intensity; // offset 0x8, size 0x4
};

// total size: 0x8
struct Arrest_BullhornArrestStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct Arrest_ArrestStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x4
struct Arrest_DispArrestReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0xC
struct AnytimeEvents_CollisionWorldStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_world_object_type world_object_type; // offset 0x4, size 0x4
    Type_num_units num_units; // offset 0x8, size 0x4
};

// total size: 0x8
struct AnytimeEvents_CollWorld_CiviStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_CollWorld_SpinStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_CollWorld_AirStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_CollWorld_FlipStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0xC
struct AnytimeEvents_DispPursuitUpdateStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_subject_battalion subject_battalion; // offset 0x4, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0x8, size 0x4
};

// total size: 0x4
struct AnytimeEvents_PursuitUpdateRepStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x20
struct AnytimeEvents_Disp911ReportStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_pursuit_type pursuit_type; // offset 0x4, size 0x4
    Type_num_suspects num_suspects; // offset 0x8, size 0x4
    Type_encounter encounter; // offset 0xC, size 0x4
    Type_direction direction; // offset 0x10, size 0x4
    Type_location location; // offset 0x14, size 0x4
    Type_location_region location_region; // offset 0x18, size 0x4
    Type_address_group_type address_group_type; // offset 0x1C, size 0x4
};

// total size: 0x24
struct AnytimeEvents_Disp911CsPntStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_address_group_type address_group_type; // offset 0x4, size 0x4
    Type_pursuit_type pursuit_type; // offset 0x8, size 0x4
    Type_num_suspects num_suspects; // offset 0xC, size 0x4
    Type_encounter encounter; // offset 0x10, size 0x4
    Type_direction direction; // offset 0x14, size 0x4
    Type_location_region location_region; // offset 0x18, size 0x4
    Type_location location; // offset 0x1C, size 0x4
    Type_car_type car_type; // offset 0x20, size 0x4
};

// total size: 0x24
struct AnytimeEvents_Disp911NoDescripStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_address_group_type address_group_type; // offset 0x4, size 0x4
    Type_pursuit_type pursuit_type; // offset 0x8, size 0x4
    Type_num_suspects num_suspects; // offset 0xC, size 0x4
    Type_encounter encounter; // offset 0x10, size 0x4
    Type_direction direction; // offset 0x14, size 0x4
    Type_location_region location_region; // offset 0x18, size 0x4
    Type_location location; // offset 0x1C, size 0x4
    Type_car_type car_type; // offset 0x20, size 0x4
};

// total size: 0x4
struct AnytimeEvents_Unit911ReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct AnytimeEvents_SuspectUTurnStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_SuspectOutrunStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_LostVisualStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_RegainVisualStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_LostSuspectStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x10
struct AnytimeEvents_DispBreakAwayStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_location_region location_region; // offset 0x4, size 0x4
    Type_location location; // offset 0x8, size 0x4
    Type_direction direction; // offset 0xC, size 0x4
};

// total size: 0x4
struct AnytimeEvents_DispTimeExpiredStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x1C
struct AnytimeEvents_DispPursuitEscalationStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_address_group_type address_group_type; // offset 0x4, size 0x4
    Type_pursuit_type pursuit_type; // offset 0x8, size 0x4
    Type_num_suspects num_suspects; // offset 0xC, size 0x4
    Type_direction direction; // offset 0x10, size 0x4
    Type_location location; // offset 0x14, size 0x4
    Type_location_region location_region; // offset 0x18, size 0x4
};

// total size: 0x8
struct AnytimeEvents_DispPursEscGenStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

// total size: 0xC
struct AnytimeEvents_UnitDisabledStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
    Type_self_other self_other; // offset 0x8, size 0x4
};

// total size: 0x8
struct AnytimeEvents_CallForEVStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_ev_type ev_type; // offset 0x4, size 0x4
};

// total size: 0x4
struct AnytimeEvents_DispEVReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct AnytimeEvents_IntentToRamStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_BailoutStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_bailout_type bailout_type; // offset 0x4, size 0x4
};

// total size: 0x4
struct AnytimeEvents_BailoutDenyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct AnytimeEvents_FocusChangeStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_SuspectBehaviourStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_DriverHistoryStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_region region; // offset 0x4, size 0x4
};

// total size: 0xC
struct AnytimeEvents_OffroadMomentStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_offroad_moment_id offroad_moment_id; // offset 0x4, size 0x4
    Type_first_subsequent first_subsequent; // offset 0x8, size 0x4
};

// total size: 0x8
struct AnytimeEvents_SpottedStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x4
struct AnytimeEvents_SuspectBrakeStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct AnytimeEvents_WeatherReportStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct AnytimeEvents_HeatJumpStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_heat_level heat_level; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_DirectionHighStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_direction direction; // offset 0x4, size 0x4
};

// total size: 0x8
struct AnytimeEvents_DispJurisShiftStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_jurisdiction jurisdiction; // offset 0x4, size 0x4
};

// total size: 0x8
struct HeliSpecific_HeliSelfStrategyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_heli_self_strategy_type heli_self_strategy_type; // offset 0x4, size 0x4
};

// total size: 0x8
struct HeliSpecific_HeliLostVisualStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_heli_lost_visual heli_lost_visual; // offset 0x4, size 0x4
};

// total size: 0x8
struct HeliSpecific_HeliIntentToBailStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_heli_bailout_type heli_bailout_type; // offset 0x4, size 0x4
};

// total size: 0x8
struct HeliSpecific_HeliBailoutStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_heli_bailout_type heli_bailout_type; // offset 0x4, size 0x4
};

// total size: 0x4
struct HeliSpecific_HeliSwarmingStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct HeliSpecific_HeliSpotterStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct HeliSpecific_HeliHazardAlertStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_heli_hazard_alert_type heli_hazard_alert_type; // offset 0x4, size 0x4
};

// total size: 0x4
struct HeliSpecific_HeliQuadrentStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct HeliSpecific_HeliQuadrentMovingStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct HeliSpecific_HeliBullhornArrestStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x0
struct E3_Events_E3_SetupStruct {
};

// total size: 0x8
struct Interrupts_InterruptStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x4
struct Interrupts_InterruptRamStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct Interrupts_InterruptRam_REStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct Interrupts_InterruptRam_HOStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct Interrupts_InterruptRam_SSStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct Interrupts_InterruptRam_TBStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x4
struct Interrupts_InterruptRamHighStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x0
struct Interrupts_StaticInterruptStruct {
};

// total size: 0x8
struct Interrupts_RegainVisualInterruptStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

// total size: 0x8
struct CellCallStruct {
    Type_cell_call_bucket cell_call_bucket; // offset 0x0, size 0x4
    Type_cell_call_number cell_call_number; // offset 0x4, size 0x4
};

// total size: 0x0
struct ExtraCops_SwarmingReplyStruct {
};

// total size: 0x0
struct ExtraCops_SuperPursuitReplyStruct {
};

// total size: 0x0
struct ExtraCops_SwarmingReplyFollowStruct {
};

// total size: 0x0
struct ExtraCops_QuadrentFormingStruct {
};

// total size: 0x0
struct ExtraCops_SuspectPossiblyGoneStruct {
};

// total size: 0x0
struct ExtraCops_QuadrentMovingStruct {
};

// total size: 0x0
struct ExtraCops_OtherLeadStruct {
};

// total size: 0x0
struct ExtraCops_PossibleSuspectStruct {
};

// total size: 0x0
struct ExtraCops_WrongSuspectStruct {
};

// total size: 0x0
struct ExtraCops_SuspectGoneStruct {
};

// total size: 0x0
struct ExtraCops_RBWarningStruct {
};

// total size: 0x4
struct ExtraCops_RBPositionStruct {
    Type_spikebelt_position spikebelt_position; // offset 0x0, size 0x4
};

// total size: 0x4
struct ExtraCops_ExtraRBEngageStruct {
    Type_roadblock_engage_type roadblock_engage_type; // offset 0x0, size 0x4
};

// total size: 0x4
struct ExtraCops_ExtraRBAvertedStruct {
    Type_roadblock_engage_type roadblock_engage_type; // offset 0x0, size 0x4
};

// total size: 0x4
struct Cross_CrossBUReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x8
struct Cross_CrossFailReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_on_off_scene on_off_scene; // offset 0x4, size 0x4
};

// total size: 0x8
struct Cross_CrossRBFailReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_on_off_scene on_off_scene; // offset 0x4, size 0x4
};

// total size: 0x4
struct Cross_CrossPursuitEscStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct Cross_CrossSelfStrategyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct Cross_CrossMultiStrategyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x4
struct Cross_CrossBailoutDeny_subStruct {
    int speaker_id; // offset 0x0, size 0x4
};

// total size: 0x0
struct D_DayStruct {
};

// total size: 0x0
struct DispIntroRaceStruct {
};

// Interface ids / handles
extern InterfaceId AcknowledgeId;
extern FunctionHandle gAcknowledgeHandle;
extern InterfaceId Setup_SpotterId;
extern FunctionHandle gSetup_SpotterHandle;
extern InterfaceId Setup_SpotterWantedId;
extern FunctionHandle gSetup_SpotterWantedHandle;
extern InterfaceId Setup_SpotterReplyId;
extern FunctionHandle gSetup_SpotterReplyHandle;
extern InterfaceId Setup_AttmptVehStpId;
extern FunctionHandle gSetup_AttmptVehStpHandle;
extern InterfaceId Setup_DispGoAheadId;
extern FunctionHandle gSetup_DispGoAheadHandle;
extern InterfaceId Setup_PrimaryEngageId;
extern FunctionHandle gSetup_PrimaryEngageHandle;
extern InterfaceId Setup_InitPursuitId;
extern FunctionHandle gSetup_InitPursuitHandle;
extern InterfaceId Setup_SuspectConfirmedId;
extern FunctionHandle gSetup_SuspectConfirmedHandle;
extern InterfaceId Setup_ReInitPursuitId;
extern FunctionHandle gSetup_ReInitPursuitHandle;
extern InterfaceId Setup_VehicleReportId;
extern FunctionHandle gSetup_VehicleReportHandle;
extern InterfaceId Setup_VehicleReportTagId;
extern FunctionHandle gSetup_VehicleReportTagHandle;
extern InterfaceId Setup_DispVehDescripId;
extern FunctionHandle gSetup_DispVehDescripHandle;
extern InterfaceId Setup_DispVehDescripVinylsId;
extern FunctionHandle gSetup_DispVehDescripVinylsHandle;
extern InterfaceId Setup_DispNoVehDescripId;
extern FunctionHandle gSetup_DispNoVehDescripHandle;
extern InterfaceId Setup_DispCustPaintId;
extern FunctionHandle gSetup_DispCustPaintHandle;
extern InterfaceId Setup_MoreDetailsId;
extern FunctionHandle gSetup_MoreDetailsHandle;
extern InterfaceId Setup_LocationReportId;
extern FunctionHandle gSetup_LocationReportHandle;
extern InterfaceId Setup_BullhornPrefixId;
extern FunctionHandle gSetup_BullhornPrefixHandle;
extern InterfaceId Setup_BullhornId;
extern FunctionHandle gSetup_BullhornHandle;
extern InterfaceId Setup_SelfStrategyId;
extern FunctionHandle gSetup_SelfStrategyHandle;
extern InterfaceId Setup_InitialCallForBUId;
extern FunctionHandle gSetup_InitialCallForBUHandle;
extern InterfaceId Setup_InitialCallForBU_MSId;
extern FunctionHandle gSetup_InitialCallForBU_MSHandle;
extern InterfaceId Backup_CallForBUId;
extern FunctionHandle gBackup_CallForBUHandle;
extern InterfaceId Backup_UnitBUReplyId;
extern FunctionHandle gBackup_UnitBUReplyHandle;
extern InterfaceId Backup_DispBackupReplyId;
extern FunctionHandle gBackup_DispBackupReplyHandle;
extern InterfaceId Backup_CallForSwarmingId;
extern FunctionHandle gBackup_CallForSwarmingHandle;
extern InterfaceId Backup_DispBUETAId;
extern FunctionHandle gBackup_DispBUETAHandle;
extern InterfaceId Backup_DispHeliBUETAId;
extern FunctionHandle gBackup_DispHeliBUETAHandle;
extern InterfaceId Backup_BUReminderId;
extern FunctionHandle gBackup_BUReminderHandle;
extern InterfaceId Backup_NegativeBUReplyId;
extern FunctionHandle gBackup_NegativeBUReplyHandle;
extern InterfaceId Backup_DispBackupUpdateId;
extern FunctionHandle gBackup_DispBackupUpdateHandle;
extern InterfaceId Backup_BUArrivesId;
extern FunctionHandle gBackup_BUArrivesHandle;
extern InterfaceId StaticRoadblock_CallForRBId;
extern FunctionHandle gStaticRoadblock_CallForRBHandle;
extern InterfaceId StaticRoadblock_RBReminderId;
extern FunctionHandle gStaticRoadblock_RBReminderHandle;
extern InterfaceId StaticRoadblock_NegativeRBReplyId;
extern FunctionHandle gStaticRoadblock_NegativeRBReplyHandle;
extern InterfaceId StaticRoadblock_DispRBReplyId;
extern FunctionHandle gStaticRoadblock_DispRBReplyHandle;
extern InterfaceId StaticRoadblock_DispRBUpdateId;
extern FunctionHandle gStaticRoadblock_DispRBUpdateHandle;
extern InterfaceId StaticRoadblock_PursuitApproachingId;
extern FunctionHandle gStaticRoadblock_PursuitApproachingHandle;
extern InterfaceId StaticRoadblock_RBApproachId;
extern FunctionHandle gStaticRoadblock_RBApproachHandle;
extern InterfaceId StaticRoadblock_RBEngageId;
extern FunctionHandle gStaticRoadblock_RBEngageHandle;
extern InterfaceId StaticRoadblock_RBAvertedId;
extern FunctionHandle gStaticRoadblock_RBAvertedHandle;
extern InterfaceId StaticRoadblock_CallForRB_subId;
extern FunctionHandle gStaticRoadblock_CallForRB_subHandle;
extern InterfaceId StaticRoadblock_DispSubRBId;
extern FunctionHandle gStaticRoadblock_DispSubRBHandle;
extern InterfaceId Projectile_CallForSafetyId;
extern FunctionHandle gProjectile_CallForSafetyHandle;
extern InterfaceId Projectile_ProjectileLaunchId;
extern FunctionHandle gProjectile_ProjectileLaunchHandle;
extern InterfaceId Projectile_ProjectileHitId;
extern FunctionHandle gProjectile_ProjectileHitHandle;
extern InterfaceId Projectile_ProjectileMissId;
extern FunctionHandle gProjectile_ProjectileMissHandle;
extern InterfaceId RollingStrategy_InitStrategyId;
extern FunctionHandle gRollingStrategy_InitStrategyHandle;
extern InterfaceId RollingStrategy_CallToPositionId;
extern FunctionHandle gRollingStrategy_CallToPositionHandle;
extern InterfaceId RollingStrategy_CallToPositionRemId;
extern FunctionHandle gRollingStrategy_CallToPositionRemHandle;
extern InterfaceId RollingStrategy_StrategyExecuteId;
extern FunctionHandle gRollingStrategy_StrategyExecuteHandle;
extern InterfaceId Outcome_AnticipateFailId;
extern FunctionHandle gOutcome_AnticipateFailHandle;
extern InterfaceId Outcome_AnticipateSuccessId;
extern FunctionHandle gOutcome_AnticipateSuccessHandle;
extern InterfaceId Outcome_OutcomeFailId;
extern FunctionHandle gOutcome_OutcomeFailHandle;
extern InterfaceId Outcome_StrategyResetId;
extern FunctionHandle gOutcome_StrategyResetHandle;
extern InterfaceId Arrest_BullhornArrestId;
extern FunctionHandle gArrest_BullhornArrestHandle;
extern InterfaceId Arrest_ArrestId;
extern FunctionHandle gArrest_ArrestHandle;
extern InterfaceId Arrest_DispArrestReplyId;
extern FunctionHandle gArrest_DispArrestReplyHandle;
extern InterfaceId AnytimeEvents_CollisionWorldId;
extern FunctionHandle gAnytimeEvents_CollisionWorldHandle;
extern InterfaceId AnytimeEvents_CollWorld_CiviId;
extern FunctionHandle gAnytimeEvents_CollWorld_CiviHandle;
extern InterfaceId AnytimeEvents_CollWorld_SpinId;
extern FunctionHandle gAnytimeEvents_CollWorld_SpinHandle;
extern InterfaceId AnytimeEvents_CollWorld_AirId;
extern FunctionHandle gAnytimeEvents_CollWorld_AirHandle;
extern InterfaceId AnytimeEvents_CollWorld_FlipId;
extern FunctionHandle gAnytimeEvents_CollWorld_FlipHandle;
extern InterfaceId AnytimeEvents_DispPursuitUpdateId;
extern FunctionHandle gAnytimeEvents_DispPursuitUpdateHandle;
extern InterfaceId AnytimeEvents_PursuitUpdateRepId;
extern FunctionHandle gAnytimeEvents_PursuitUpdateRepHandle;
extern InterfaceId AnytimeEvents_Disp911ReportId;
extern FunctionHandle gAnytimeEvents_Disp911ReportHandle;
extern InterfaceId AnytimeEvents_Disp911CsPntId;
extern FunctionHandle gAnytimeEvents_Disp911CsPntHandle;
extern InterfaceId AnytimeEvents_Disp911NoDescripId;
extern FunctionHandle gAnytimeEvents_Disp911NoDescripHandle;
extern InterfaceId AnytimeEvents_Unit911ReplyId;
extern FunctionHandle gAnytimeEvents_Unit911ReplyHandle;
extern InterfaceId AnytimeEvents_SuspectUTurnId;
extern FunctionHandle gAnytimeEvents_SuspectUTurnHandle;
extern InterfaceId AnytimeEvents_SuspectOutrunId;
extern FunctionHandle gAnytimeEvents_SuspectOutrunHandle;
extern InterfaceId AnytimeEvents_LostVisualId;
extern FunctionHandle gAnytimeEvents_LostVisualHandle;
extern InterfaceId AnytimeEvents_RegainVisualId;
extern FunctionHandle gAnytimeEvents_RegainVisualHandle;
extern InterfaceId AnytimeEvents_LostSuspectId;
extern FunctionHandle gAnytimeEvents_LostSuspectHandle;
extern InterfaceId AnytimeEvents_DispBreakAwayId;
extern FunctionHandle gAnytimeEvents_DispBreakAwayHandle;
extern InterfaceId AnytimeEvents_DispTimeExpiredId;
extern FunctionHandle gAnytimeEvents_DispTimeExpiredHandle;
extern InterfaceId AnytimeEvents_DispPursuitEscalationId;
extern FunctionHandle gAnytimeEvents_DispPursuitEscalationHandle;
extern InterfaceId AnytimeEvents_DispPursEscGenId;
extern FunctionHandle gAnytimeEvents_DispPursEscGenHandle;
extern InterfaceId AnytimeEvents_UnitDisabledId;
extern FunctionHandle gAnytimeEvents_UnitDisabledHandle;
extern InterfaceId AnytimeEvents_CallForEVId;
extern FunctionHandle gAnytimeEvents_CallForEVHandle;
extern InterfaceId AnytimeEvents_DispEVReplyId;
extern FunctionHandle gAnytimeEvents_DispEVReplyHandle;
extern InterfaceId AnytimeEvents_IntentToRamId;
extern FunctionHandle gAnytimeEvents_IntentToRamHandle;
extern InterfaceId AnytimeEvents_BailoutId;
extern FunctionHandle gAnytimeEvents_BailoutHandle;
extern InterfaceId AnytimeEvents_BailoutDenyId;
extern FunctionHandle gAnytimeEvents_BailoutDenyHandle;
extern InterfaceId AnytimeEvents_FocusChangeId;
extern FunctionHandle gAnytimeEvents_FocusChangeHandle;
extern InterfaceId AnytimeEvents_SuspectBehaviourId;
extern FunctionHandle gAnytimeEvents_SuspectBehaviourHandle;
extern InterfaceId AnytimeEvents_DriverHistoryId;
extern FunctionHandle gAnytimeEvents_DriverHistoryHandle;
extern InterfaceId AnytimeEvents_OffroadMomentId;
extern FunctionHandle gAnytimeEvents_OffroadMomentHandle;
extern InterfaceId AnytimeEvents_SpottedId;
extern FunctionHandle gAnytimeEvents_SpottedHandle;
extern InterfaceId AnytimeEvents_SuspectBrakeId;
extern FunctionHandle gAnytimeEvents_SuspectBrakeHandle;
extern InterfaceId AnytimeEvents_WeatherReportId;
extern FunctionHandle gAnytimeEvents_WeatherReportHandle;
extern InterfaceId AnytimeEvents_HeatJumpId;
extern FunctionHandle gAnytimeEvents_HeatJumpHandle;
extern InterfaceId AnytimeEvents_DirectionHighId;
extern FunctionHandle gAnytimeEvents_DirectionHighHandle;
extern InterfaceId AnytimeEvents_DispJurisShiftId;
extern FunctionHandle gAnytimeEvents_DispJurisShiftHandle;
extern InterfaceId HeliSpecific_HeliSelfStrategyId;
extern FunctionHandle gHeliSpecific_HeliSelfStrategyHandle;
extern InterfaceId HeliSpecific_HeliLostVisualId;
extern FunctionHandle gHeliSpecific_HeliLostVisualHandle;
extern InterfaceId HeliSpecific_HeliIntentToBailId;
extern FunctionHandle gHeliSpecific_HeliIntentToBailHandle;
extern InterfaceId HeliSpecific_HeliBailoutId;
extern FunctionHandle gHeliSpecific_HeliBailoutHandle;
extern InterfaceId HeliSpecific_HeliSwarmingId;
extern FunctionHandle gHeliSpecific_HeliSwarmingHandle;
extern InterfaceId HeliSpecific_HeliSpotterId;
extern FunctionHandle gHeliSpecific_HeliSpotterHandle;
extern InterfaceId HeliSpecific_HeliHazardAlertId;
extern FunctionHandle gHeliSpecific_HeliHazardAlertHandle;
extern InterfaceId HeliSpecific_HeliQuadrentId;
extern FunctionHandle gHeliSpecific_HeliQuadrentHandle;
extern InterfaceId HeliSpecific_HeliQuadrentMovingId;
extern FunctionHandle gHeliSpecific_HeliQuadrentMovingHandle;
extern InterfaceId HeliSpecific_HeliBullhornArrestId;
extern FunctionHandle gHeliSpecific_HeliBullhornArrestHandle;
extern InterfaceId E3_Events_E3_SetupId;
extern FunctionHandle gE3_Events_E3_SetupHandle;
extern InterfaceId Interrupts_InterruptId;
extern FunctionHandle gInterrupts_InterruptHandle;
extern InterfaceId Interrupts_InterruptRamId;
extern FunctionHandle gInterrupts_InterruptRamHandle;
extern InterfaceId Interrupts_InterruptRam_REId;
extern FunctionHandle gInterrupts_InterruptRam_REHandle;
extern InterfaceId Interrupts_InterruptRam_HOId;
extern FunctionHandle gInterrupts_InterruptRam_HOHandle;
extern InterfaceId Interrupts_InterruptRam_SSId;
extern FunctionHandle gInterrupts_InterruptRam_SSHandle;
extern InterfaceId Interrupts_InterruptRam_TBId;
extern FunctionHandle gInterrupts_InterruptRam_TBHandle;
extern InterfaceId Interrupts_InterruptRamHighId;
extern FunctionHandle gInterrupts_InterruptRamHighHandle;
extern InterfaceId Interrupts_StaticInterruptId;
extern FunctionHandle gInterrupts_StaticInterruptHandle;
extern InterfaceId Interrupts_RegainVisualInterruptId;
extern FunctionHandle gInterrupts_RegainVisualInterruptHandle;
extern InterfaceId CellCallId;
extern FunctionHandle gCellCallHandle;
extern InterfaceId ExtraCops_SwarmingReplyId;
extern FunctionHandle gExtraCops_SwarmingReplyHandle;
extern InterfaceId ExtraCops_SuperPursuitReplyId;
extern FunctionHandle gExtraCops_SuperPursuitReplyHandle;
extern InterfaceId ExtraCops_SwarmingReplyFollowId;
extern FunctionHandle gExtraCops_SwarmingReplyFollowHandle;
extern InterfaceId ExtraCops_QuadrentFormingId;
extern FunctionHandle gExtraCops_QuadrentFormingHandle;
extern InterfaceId ExtraCops_SuspectPossiblyGoneId;
extern FunctionHandle gExtraCops_SuspectPossiblyGoneHandle;
extern InterfaceId ExtraCops_QuadrentMovingId;
extern FunctionHandle gExtraCops_QuadrentMovingHandle;
extern InterfaceId ExtraCops_OtherLeadId;
extern FunctionHandle gExtraCops_OtherLeadHandle;
extern InterfaceId ExtraCops_PossibleSuspectId;
extern FunctionHandle gExtraCops_PossibleSuspectHandle;
extern InterfaceId ExtraCops_WrongSuspectId;
extern FunctionHandle gExtraCops_WrongSuspectHandle;
extern InterfaceId ExtraCops_SuspectGoneId;
extern FunctionHandle gExtraCops_SuspectGoneHandle;
extern InterfaceId ExtraCops_RBWarningId;
extern FunctionHandle gExtraCops_RBWarningHandle;
extern InterfaceId ExtraCops_RBPositionId;
extern FunctionHandle gExtraCops_RBPositionHandle;
extern InterfaceId ExtraCops_ExtraRBEngageId;
extern FunctionHandle gExtraCops_ExtraRBEngageHandle;
extern InterfaceId ExtraCops_ExtraRBAvertedId;
extern FunctionHandle gExtraCops_ExtraRBAvertedHandle;
extern InterfaceId Cross_CrossBUReplyId;
extern FunctionHandle gCross_CrossBUReplyHandle;
extern InterfaceId Cross_CrossFailReplyId;
extern FunctionHandle gCross_CrossFailReplyHandle;
extern InterfaceId Cross_CrossRBFailReplyId;
extern FunctionHandle gCross_CrossRBFailReplyHandle;
extern InterfaceId Cross_CrossPursuitEscId;
extern FunctionHandle gCross_CrossPursuitEscHandle;
extern InterfaceId Cross_CrossSelfStrategyId;
extern FunctionHandle gCross_CrossSelfStrategyHandle;
extern InterfaceId Cross_CrossMultiStrategyId;
extern FunctionHandle gCross_CrossMultiStrategyHandle;
extern InterfaceId Cross_CrossBailoutDeny_subId;
extern FunctionHandle gCross_CrossBailoutDeny_subHandle;
extern InterfaceId D_DayId;
extern FunctionHandle gD_DayHandle;
extern InterfaceId DispIntroRaceId;
extern FunctionHandle gDispIntroRaceHandle;
static inline Result CacheHandlesEvents() {
    gAcknowledgeHandle.Set(&AcknowledgeId);
    gSetup_SpotterHandle.Set(&Setup_SpotterId);
    gSetup_SpotterWantedHandle.Set(&Setup_SpotterWantedId);
    gSetup_SpotterReplyHandle.Set(&Setup_SpotterReplyId);
    gSetup_AttmptVehStpHandle.Set(&Setup_AttmptVehStpId);
    gSetup_DispGoAheadHandle.Set(&Setup_DispGoAheadId);
    gSetup_PrimaryEngageHandle.Set(&Setup_PrimaryEngageId);
    gSetup_InitPursuitHandle.Set(&Setup_InitPursuitId);
    gSetup_SuspectConfirmedHandle.Set(&Setup_SuspectConfirmedId);
    gSetup_ReInitPursuitHandle.Set(&Setup_ReInitPursuitId);
    gSetup_VehicleReportHandle.Set(&Setup_VehicleReportId);
    gSetup_VehicleReportTagHandle.Set(&Setup_VehicleReportTagId);
    gSetup_DispVehDescripHandle.Set(&Setup_DispVehDescripId);
    gSetup_DispVehDescripVinylsHandle.Set(&Setup_DispVehDescripVinylsId);
    gSetup_DispNoVehDescripHandle.Set(&Setup_DispNoVehDescripId);
    gSetup_DispCustPaintHandle.Set(&Setup_DispCustPaintId);
    gSetup_MoreDetailsHandle.Set(&Setup_MoreDetailsId);
    gSetup_LocationReportHandle.Set(&Setup_LocationReportId);
    gSetup_BullhornPrefixHandle.Set(&Setup_BullhornPrefixId);
    gSetup_BullhornHandle.Set(&Setup_BullhornId);
    gSetup_SelfStrategyHandle.Set(&Setup_SelfStrategyId);
    gSetup_InitialCallForBUHandle.Set(&Setup_InitialCallForBUId);
    gSetup_InitialCallForBU_MSHandle.Set(&Setup_InitialCallForBU_MSId);
    gBackup_CallForBUHandle.Set(&Backup_CallForBUId);
    gBackup_UnitBUReplyHandle.Set(&Backup_UnitBUReplyId);
    gBackup_DispBackupReplyHandle.Set(&Backup_DispBackupReplyId);
    gBackup_CallForSwarmingHandle.Set(&Backup_CallForSwarmingId);
    gBackup_DispBUETAHandle.Set(&Backup_DispBUETAId);
    gBackup_DispHeliBUETAHandle.Set(&Backup_DispHeliBUETAId);
    gBackup_BUReminderHandle.Set(&Backup_BUReminderId);
    gBackup_NegativeBUReplyHandle.Set(&Backup_NegativeBUReplyId);
    gBackup_DispBackupUpdateHandle.Set(&Backup_DispBackupUpdateId);
    gBackup_BUArrivesHandle.Set(&Backup_BUArrivesId);
    gStaticRoadblock_CallForRBHandle.Set(&StaticRoadblock_CallForRBId);
    gStaticRoadblock_RBReminderHandle.Set(&StaticRoadblock_RBReminderId);
    gStaticRoadblock_NegativeRBReplyHandle.Set(&StaticRoadblock_NegativeRBReplyId);
    gStaticRoadblock_DispRBReplyHandle.Set(&StaticRoadblock_DispRBReplyId);
    gStaticRoadblock_DispRBUpdateHandle.Set(&StaticRoadblock_DispRBUpdateId);
    gStaticRoadblock_PursuitApproachingHandle.Set(&StaticRoadblock_PursuitApproachingId);
    gStaticRoadblock_RBApproachHandle.Set(&StaticRoadblock_RBApproachId);
    gStaticRoadblock_RBEngageHandle.Set(&StaticRoadblock_RBEngageId);
    gStaticRoadblock_RBAvertedHandle.Set(&StaticRoadblock_RBAvertedId);
    gStaticRoadblock_CallForRB_subHandle.Set(&StaticRoadblock_CallForRB_subId);
    gStaticRoadblock_DispSubRBHandle.Set(&StaticRoadblock_DispSubRBId);
    gProjectile_CallForSafetyHandle.Set(&Projectile_CallForSafetyId);
    gProjectile_ProjectileLaunchHandle.Set(&Projectile_ProjectileLaunchId);
    gProjectile_ProjectileHitHandle.Set(&Projectile_ProjectileHitId);
    gProjectile_ProjectileMissHandle.Set(&Projectile_ProjectileMissId);
    gRollingStrategy_InitStrategyHandle.Set(&RollingStrategy_InitStrategyId);
    gRollingStrategy_CallToPositionHandle.Set(&RollingStrategy_CallToPositionId);
    gRollingStrategy_CallToPositionRemHandle.Set(&RollingStrategy_CallToPositionRemId);
    gRollingStrategy_StrategyExecuteHandle.Set(&RollingStrategy_StrategyExecuteId);
    gOutcome_AnticipateFailHandle.Set(&Outcome_AnticipateFailId);
    gOutcome_AnticipateSuccessHandle.Set(&Outcome_AnticipateSuccessId);
    gOutcome_OutcomeFailHandle.Set(&Outcome_OutcomeFailId);
    gOutcome_StrategyResetHandle.Set(&Outcome_StrategyResetId);
    gArrest_BullhornArrestHandle.Set(&Arrest_BullhornArrestId);
    gArrest_ArrestHandle.Set(&Arrest_ArrestId);
    gArrest_DispArrestReplyHandle.Set(&Arrest_DispArrestReplyId);
    gAnytimeEvents_CollisionWorldHandle.Set(&AnytimeEvents_CollisionWorldId);
    gAnytimeEvents_CollWorld_CiviHandle.Set(&AnytimeEvents_CollWorld_CiviId);
    gAnytimeEvents_CollWorld_SpinHandle.Set(&AnytimeEvents_CollWorld_SpinId);
    gAnytimeEvents_CollWorld_AirHandle.Set(&AnytimeEvents_CollWorld_AirId);
    gAnytimeEvents_CollWorld_FlipHandle.Set(&AnytimeEvents_CollWorld_FlipId);
    gAnytimeEvents_DispPursuitUpdateHandle.Set(&AnytimeEvents_DispPursuitUpdateId);
    gAnytimeEvents_PursuitUpdateRepHandle.Set(&AnytimeEvents_PursuitUpdateRepId);
    gAnytimeEvents_Disp911ReportHandle.Set(&AnytimeEvents_Disp911ReportId);
    gAnytimeEvents_Disp911CsPntHandle.Set(&AnytimeEvents_Disp911CsPntId);
    gAnytimeEvents_Disp911NoDescripHandle.Set(&AnytimeEvents_Disp911NoDescripId);
    gAnytimeEvents_Unit911ReplyHandle.Set(&AnytimeEvents_Unit911ReplyId);
    gAnytimeEvents_SuspectUTurnHandle.Set(&AnytimeEvents_SuspectUTurnId);
    gAnytimeEvents_SuspectOutrunHandle.Set(&AnytimeEvents_SuspectOutrunId);
    gAnytimeEvents_LostVisualHandle.Set(&AnytimeEvents_LostVisualId);
    gAnytimeEvents_RegainVisualHandle.Set(&AnytimeEvents_RegainVisualId);
    gAnytimeEvents_LostSuspectHandle.Set(&AnytimeEvents_LostSuspectId);
    gAnytimeEvents_DispBreakAwayHandle.Set(&AnytimeEvents_DispBreakAwayId);
    gAnytimeEvents_DispTimeExpiredHandle.Set(&AnytimeEvents_DispTimeExpiredId);
    gAnytimeEvents_DispPursuitEscalationHandle.Set(&AnytimeEvents_DispPursuitEscalationId);
    gAnytimeEvents_DispPursEscGenHandle.Set(&AnytimeEvents_DispPursEscGenId);
    gAnytimeEvents_UnitDisabledHandle.Set(&AnytimeEvents_UnitDisabledId);
    gAnytimeEvents_CallForEVHandle.Set(&AnytimeEvents_CallForEVId);
    gAnytimeEvents_DispEVReplyHandle.Set(&AnytimeEvents_DispEVReplyId);
    gAnytimeEvents_IntentToRamHandle.Set(&AnytimeEvents_IntentToRamId);
    gAnytimeEvents_BailoutHandle.Set(&AnytimeEvents_BailoutId);
    gAnytimeEvents_BailoutDenyHandle.Set(&AnytimeEvents_BailoutDenyId);
    gAnytimeEvents_FocusChangeHandle.Set(&AnytimeEvents_FocusChangeId);
    gAnytimeEvents_SuspectBehaviourHandle.Set(&AnytimeEvents_SuspectBehaviourId);
    gAnytimeEvents_DriverHistoryHandle.Set(&AnytimeEvents_DriverHistoryId);
    gAnytimeEvents_OffroadMomentHandle.Set(&AnytimeEvents_OffroadMomentId);
    gAnytimeEvents_SpottedHandle.Set(&AnytimeEvents_SpottedId);
    gAnytimeEvents_SuspectBrakeHandle.Set(&AnytimeEvents_SuspectBrakeId);
    gAnytimeEvents_WeatherReportHandle.Set(&AnytimeEvents_WeatherReportId);
    gAnytimeEvents_HeatJumpHandle.Set(&AnytimeEvents_HeatJumpId);
    gAnytimeEvents_DirectionHighHandle.Set(&AnytimeEvents_DirectionHighId);
    gAnytimeEvents_DispJurisShiftHandle.Set(&AnytimeEvents_DispJurisShiftId);
    gHeliSpecific_HeliSelfStrategyHandle.Set(&HeliSpecific_HeliSelfStrategyId);
    gHeliSpecific_HeliLostVisualHandle.Set(&HeliSpecific_HeliLostVisualId);
    gHeliSpecific_HeliIntentToBailHandle.Set(&HeliSpecific_HeliIntentToBailId);
    gHeliSpecific_HeliBailoutHandle.Set(&HeliSpecific_HeliBailoutId);
    gHeliSpecific_HeliSwarmingHandle.Set(&HeliSpecific_HeliSwarmingId);
    gHeliSpecific_HeliSpotterHandle.Set(&HeliSpecific_HeliSpotterId);
    gHeliSpecific_HeliHazardAlertHandle.Set(&HeliSpecific_HeliHazardAlertId);
    gHeliSpecific_HeliQuadrentHandle.Set(&HeliSpecific_HeliQuadrentId);
    gHeliSpecific_HeliQuadrentMovingHandle.Set(&HeliSpecific_HeliQuadrentMovingId);
    gHeliSpecific_HeliBullhornArrestHandle.Set(&HeliSpecific_HeliBullhornArrestId);
    gE3_Events_E3_SetupHandle.Set(&E3_Events_E3_SetupId);
    gInterrupts_InterruptHandle.Set(&Interrupts_InterruptId);
    gInterrupts_InterruptRamHandle.Set(&Interrupts_InterruptRamId);
    gInterrupts_InterruptRam_REHandle.Set(&Interrupts_InterruptRam_REId);
    gInterrupts_InterruptRam_HOHandle.Set(&Interrupts_InterruptRam_HOId);
    gInterrupts_InterruptRam_SSHandle.Set(&Interrupts_InterruptRam_SSId);
    gInterrupts_InterruptRam_TBHandle.Set(&Interrupts_InterruptRam_TBId);
    gInterrupts_InterruptRamHighHandle.Set(&Interrupts_InterruptRamHighId);
    gInterrupts_StaticInterruptHandle.Set(&Interrupts_StaticInterruptId);
    gInterrupts_RegainVisualInterruptHandle.Set(&Interrupts_RegainVisualInterruptId);
    gCellCallHandle.Set(&CellCallId);
    gExtraCops_SwarmingReplyHandle.Set(&ExtraCops_SwarmingReplyId);
    gExtraCops_SuperPursuitReplyHandle.Set(&ExtraCops_SuperPursuitReplyId);
    gExtraCops_SwarmingReplyFollowHandle.Set(&ExtraCops_SwarmingReplyFollowId);
    gExtraCops_QuadrentFormingHandle.Set(&ExtraCops_QuadrentFormingId);
    gExtraCops_SuspectPossiblyGoneHandle.Set(&ExtraCops_SuspectPossiblyGoneId);
    gExtraCops_QuadrentMovingHandle.Set(&ExtraCops_QuadrentMovingId);
    gExtraCops_OtherLeadHandle.Set(&ExtraCops_OtherLeadId);
    gExtraCops_PossibleSuspectHandle.Set(&ExtraCops_PossibleSuspectId);
    gExtraCops_WrongSuspectHandle.Set(&ExtraCops_WrongSuspectId);
    gExtraCops_SuspectGoneHandle.Set(&ExtraCops_SuspectGoneId);
    gExtraCops_RBWarningHandle.Set(&ExtraCops_RBWarningId);
    gExtraCops_RBPositionHandle.Set(&ExtraCops_RBPositionId);
    gExtraCops_ExtraRBEngageHandle.Set(&ExtraCops_ExtraRBEngageId);
    gExtraCops_ExtraRBAvertedHandle.Set(&ExtraCops_ExtraRBAvertedId);
    gCross_CrossBUReplyHandle.Set(&Cross_CrossBUReplyId);
    gCross_CrossFailReplyHandle.Set(&Cross_CrossFailReplyId);
    gCross_CrossRBFailReplyHandle.Set(&Cross_CrossRBFailReplyId);
    gCross_CrossPursuitEscHandle.Set(&Cross_CrossPursuitEscId);
    gCross_CrossSelfStrategyHandle.Set(&Cross_CrossSelfStrategyId);
    gCross_CrossMultiStrategyHandle.Set(&Cross_CrossMultiStrategyId);
    gCross_CrossBailoutDeny_subHandle.Set(&Cross_CrossBailoutDeny_subId);
    gD_DayHandle.Set(&D_DayId);
    gDispIntroRaceHandle.Set(&DispIntroRaceId);
    return RESULT_OK;
}

static inline Result Acknowledge(int speaker_id, enum Type_intensity intensity, enum Type_yes_no yes_no) {
    AcknowledgeStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    data.yes_no = yes_no;
    result = Function::Call(&gAcknowledgeHandle, &data);
    return result;
}

static inline Result Setup_Spotter(int speaker_id, enum Type_num_suspects num_suspects) {
    Setup_SpotterStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_suspects = num_suspects;
    result = Function::Call(&gSetup_SpotterHandle, &data);
    return result;
}

static inline Result Setup_SpotterWanted(int speaker_id) {
    Setup_SpotterWantedStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gSetup_SpotterWantedHandle, &data);
    return result;
}

static inline Result Setup_SpotterReply(int speaker_id, enum Type_num_suspects num_suspects) {
    Setup_SpotterReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_suspects = num_suspects;
    result = Function::Call(&gSetup_SpotterReplyHandle, &data);
    return result;
}

static inline Result Setup_AttmptVehStp(int speaker_id, enum Type_pursuit_type pursuit_type, enum Type_num_suspects num_suspects, enum Type_speaker_battalion speaker_battalion, enum Type_speaker_call_sign_id speaker_call_sign_id) {
    Setup_AttmptVehStpStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.pursuit_type = pursuit_type;
    data.num_suspects = num_suspects;
    data.speaker_battalion = speaker_battalion;
    data.speaker_call_sign_id = speaker_call_sign_id;
    result = Function::Call(&gSetup_AttmptVehStpHandle, &data);
    return result;
}

static inline Result Setup_DispGoAhead(int speaker_id) {
    Setup_DispGoAheadStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gSetup_DispGoAheadHandle, &data);
    return result;
}

static inline Result Setup_PrimaryEngage(int speaker_id, enum Type_speaker_battalion speaker_battalion, enum Type_speaker_call_sign_id speaker_call_sign_id) {
    Setup_PrimaryEngageStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.speaker_battalion = speaker_battalion;
    data.speaker_call_sign_id = speaker_call_sign_id;
    result = Function::Call(&gSetup_PrimaryEngageHandle, &data);
    return result;
}

static inline Result Setup_InitPursuit(int speaker_id, enum Type_num_suspects num_suspects) {
    Setup_InitPursuitStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_suspects = num_suspects;
    result = Function::Call(&gSetup_InitPursuitHandle, &data);
    return result;
}

static inline Result Setup_SuspectConfirmed(int speaker_id, enum Type_num_suspects num_suspects) {
    Setup_SuspectConfirmedStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_suspects = num_suspects;
    result = Function::Call(&gSetup_SuspectConfirmedHandle, &data);
    return result;
}

static inline Result Setup_ReInitPursuit(int speaker_id, enum Type_time_since_lost time_since_lost, enum Type_num_suspects num_suspects) {
    Setup_ReInitPursuitStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.time_since_lost = time_since_lost;
    data.num_suspects = num_suspects;
    result = Function::Call(&gSetup_ReInitPursuitHandle, &data);
    return result;
}

static inline Result Setup_VehicleReport(int speaker_id, enum Type_car_color car_color, enum Type_car_type car_type, enum Type_speed speed, enum Type_measurement measurement) {
    Setup_VehicleReportStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.car_color = car_color;
    data.car_type = car_type;
    data.speed = speed;
    data.measurement = measurement;
    result = Function::Call(&gSetup_VehicleReportHandle, &data);
    return result;
}

static inline Result Setup_VehicleReportTag(int speaker_id, enum Type_car_color car_color, enum Type_car_type car_type) {
    Setup_VehicleReportTagStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.car_color = car_color;
    data.car_type = car_type;
    result = Function::Call(&gSetup_VehicleReportTagHandle, &data);
    return result;
}

static inline Result Setup_DispVehDescrip(int speaker_id, enum Type_car_color car_color, enum Type_car_type car_type) {
    Setup_DispVehDescripStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.car_color = car_color;
    data.car_type = car_type;
    result = Function::Call(&gSetup_DispVehDescripHandle, &data);
    return result;
}

static inline Result Setup_DispVehDescripVinyls(int speaker_id, enum Type_car_color car_color, enum Type_car_type car_type) {
    Setup_DispVehDescripVinylsStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.car_color = car_color;
    data.car_type = car_type;
    result = Function::Call(&gSetup_DispVehDescripVinylsHandle, &data);
    return result;
}

static inline Result Setup_DispNoVehDescrip(int speaker_id) {
    Setup_DispNoVehDescripStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gSetup_DispNoVehDescripHandle, &data);
    return result;
}

static inline Result Setup_DispCustPaint(int speaker_id, enum Type_car_type car_type) {
    Setup_DispCustPaintStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.car_type = car_type;
    result = Function::Call(&gSetup_DispCustPaintHandle, &data);
    return result;
}

static inline Result Setup_MoreDetails(int speaker_id) {
    Setup_MoreDetailsStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gSetup_MoreDetailsHandle, &data);
    return result;
}

static inline Result Setup_LocationReport(int speaker_id, enum Type_num_suspects num_suspects, enum Type_direction direction, enum Type_encounter encounter, enum Type_location_region location_region, enum Type_location location) {
    Setup_LocationReportStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_suspects = num_suspects;
    data.direction = direction;
    data.encounter = encounter;
    data.location_region = location_region;
    data.location = location;
    result = Function::Call(&gSetup_LocationReportHandle, &data);
    return result;
}

static inline Result Setup_BullhornPrefix(int speaker_id) {
    Setup_BullhornPrefixStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gSetup_BullhornPrefixHandle, &data);
    return result;
}

static inline Result Setup_Bullhorn(int speaker_id) {
    Setup_BullhornStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gSetup_BullhornHandle, &data);
    return result;
}

static inline Result Setup_SelfStrategy(int speaker_id, enum Type_code code, enum Type_self_strategy_type self_strategy_type) {
    Setup_SelfStrategyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    data.self_strategy_type = self_strategy_type;
    result = Function::Call(&gSetup_SelfStrategyHandle, &data);
    return result;
}

static inline Result Setup_InitialCallForBU(int speaker_id, enum Type_code code) {
    Setup_InitialCallForBUStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    result = Function::Call(&gSetup_InitialCallForBUHandle, &data);
    return result;
}

static inline Result Setup_InitialCallForBU_MS(int speaker_id) {
    Setup_InitialCallForBU_MSStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gSetup_InitialCallForBU_MSHandle, &data);
    return result;
}

static inline Result Backup_CallForBU(int speaker_id, enum Type_code code, enum Type_backup_type backup_type) {
    Backup_CallForBUStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    data.backup_type = backup_type;
    result = Function::Call(&gBackup_CallForBUHandle, &data);
    return result;
}

static inline Result Backup_UnitBUReply(int speaker_id) {
    Backup_UnitBUReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gBackup_UnitBUReplyHandle, &data);
    return result;
}

static inline Result Backup_DispBackupReply(int speaker_id, enum Type_yes_no yes_no, enum Type_subject_battalion subject_battalion, enum Type_subject_call_sign_id subject_call_sign_id, enum Type_code code, enum Type_disp_backup_type disp_backup_type) {
    Backup_DispBackupReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.yes_no = yes_no;
    data.subject_battalion = subject_battalion;
    data.subject_call_sign_id = subject_call_sign_id;
    data.code = code;
    data.disp_backup_type = disp_backup_type;
    result = Function::Call(&gBackup_DispBackupReplyHandle, &data);
    return result;
}

static inline Result Backup_CallForSwarming(int speaker_id) {
    Backup_CallForSwarmingStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gBackup_CallForSwarmingHandle, &data);
    return result;
}

static inline Result Backup_DispBUETA(int speaker_id, enum Type_disp_backup_eta disp_backup_eta) {
    Backup_DispBUETAStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.disp_backup_eta = disp_backup_eta;
    result = Function::Call(&gBackup_DispBUETAHandle, &data);
    return result;
}

static inline Result Backup_DispHeliBUETA(int speaker_id, enum Type_disp_backup_eta disp_backup_eta) {
    Backup_DispHeliBUETAStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.disp_backup_eta = disp_backup_eta;
    result = Function::Call(&gBackup_DispHeliBUETAHandle, &data);
    return result;
}

static inline Result Backup_BUReminder(int speaker_id, enum Type_code code, enum Type_backup_type backup_type) {
    Backup_BUReminderStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    data.backup_type = backup_type;
    result = Function::Call(&gBackup_BUReminderHandle, &data);
    return result;
}

static inline Result Backup_NegativeBUReply(int speaker_id, enum Type_yes_no yes_no, enum Type_subject_battalion subject_battalion, enum Type_subject_call_sign_id subject_call_sign_id) {
    Backup_NegativeBUReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.yes_no = yes_no;
    data.subject_battalion = subject_battalion;
    data.subject_call_sign_id = subject_call_sign_id;
    result = Function::Call(&gBackup_NegativeBUReplyHandle, &data);
    return result;
}

static inline Result Backup_DispBackupUpdate(int speaker_id, enum Type_yes_no yes_no) {
    Backup_DispBackupUpdateStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.yes_no = yes_no;
    result = Function::Call(&gBackup_DispBackupUpdateHandle, &data);
    return result;
}

static inline Result Backup_BUArrives(int speaker_id, enum Type_speaker_battalion speaker_battalion, enum Type_speaker_call_sign_id speaker_call_sign_id) {
    Backup_BUArrivesStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.speaker_battalion = speaker_battalion;
    data.speaker_call_sign_id = speaker_call_sign_id;
    result = Function::Call(&gBackup_BUArrivesHandle, &data);
    return result;
}

static inline Result StaticRoadblock_CallForRB(int speaker_id, enum Type_code code, enum Type_roadblock_type roadblock_type) {
    StaticRoadblock_CallForRBStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    data.roadblock_type = roadblock_type;
    result = Function::Call(&gStaticRoadblock_CallForRBHandle, &data);
    return result;
}

static inline Result StaticRoadblock_RBReminder(int speaker_id, enum Type_code code) {
    StaticRoadblock_RBReminderStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    result = Function::Call(&gStaticRoadblock_RBReminderHandle, &data);
    return result;
}

static inline Result StaticRoadblock_NegativeRBReply(int speaker_id) {
    StaticRoadblock_NegativeRBReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gStaticRoadblock_NegativeRBReplyHandle, &data);
    return result;
}

static inline Result StaticRoadblock_DispRBReply(int speaker_id, enum Type_code code, enum Type_roadblock_type roadblock_type, enum Type_yes_no yes_no, enum Type_subject_battalion subject_battalion, enum Type_subject_call_sign_id subject_call_sign_id) {
    StaticRoadblock_DispRBReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    data.roadblock_type = roadblock_type;
    data.yes_no = yes_no;
    data.subject_battalion = subject_battalion;
    data.subject_call_sign_id = subject_call_sign_id;
    result = Function::Call(&gStaticRoadblock_DispRBReplyHandle, &data);
    return result;
}

static inline Result StaticRoadblock_DispRBUpdate(int speaker_id, enum Type_code code, enum Type_yes_no yes_no, enum Type_roadblock_type roadblock_type) {
    StaticRoadblock_DispRBUpdateStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    data.yes_no = yes_no;
    data.roadblock_type = roadblock_type;
    result = Function::Call(&gStaticRoadblock_DispRBUpdateHandle, &data);
    return result;
}

static inline Result StaticRoadblock_PursuitApproaching(int speaker_id, enum Type_num_suspects num_suspects) {
    StaticRoadblock_PursuitApproachingStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_suspects = num_suspects;
    result = Function::Call(&gStaticRoadblock_PursuitApproachingHandle, &data);
    return result;
}

static inline Result StaticRoadblock_RBApproach(int speaker_id, enum Type_roadblock_type roadblock_type) {
    StaticRoadblock_RBApproachStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.roadblock_type = roadblock_type;
    result = Function::Call(&gStaticRoadblock_RBApproachHandle, &data);
    return result;
}

static inline Result StaticRoadblock_RBEngage(int speaker_id, enum Type_roadblock_engage_type roadblock_engage_type) {
    StaticRoadblock_RBEngageStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.roadblock_engage_type = roadblock_engage_type;
    result = Function::Call(&gStaticRoadblock_RBEngageHandle, &data);
    return result;
}

static inline Result StaticRoadblock_RBAverted(int speaker_id) {
    StaticRoadblock_RBAvertedStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gStaticRoadblock_RBAvertedHandle, &data);
    return result;
}

static inline Result StaticRoadblock_CallForRB_sub(int speaker_id) {
    StaticRoadblock_CallForRB_subStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gStaticRoadblock_CallForRB_subHandle, &data);
    return result;
}

static inline Result StaticRoadblock_DispSubRB(int speaker_id) {
    StaticRoadblock_DispSubRBStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gStaticRoadblock_DispSubRBHandle, &data);
    return result;
}

static inline Result Projectile_CallForSafety(int speaker_id, enum Type_code code, enum Type_projectile_type projectile_type) {
    Projectile_CallForSafetyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    data.projectile_type = projectile_type;
    result = Function::Call(&gProjectile_CallForSafetyHandle, &data);
    return result;
}

static inline Result Projectile_ProjectileLaunch(int speaker_id, enum Type_projectile_type projectile_type) {
    Projectile_ProjectileLaunchStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.projectile_type = projectile_type;
    result = Function::Call(&gProjectile_ProjectileLaunchHandle, &data);
    return result;
}

static inline Result Projectile_ProjectileHit(int speaker_id, enum Type_num_units num_units) {
    Projectile_ProjectileHitStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_units = num_units;
    result = Function::Call(&gProjectile_ProjectileHitHandle, &data);
    return result;
}

static inline Result Projectile_ProjectileMiss(int speaker_id, enum Type_projectile_type projectile_type, enum Type_num_units num_units) {
    Projectile_ProjectileMissStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.projectile_type = projectile_type;
    data.num_units = num_units;
    result = Function::Call(&gProjectile_ProjectileMissHandle, &data);
    return result;
}

static inline Result RollingStrategy_InitStrategy(int speaker_id, enum Type_code code, enum Type_rolling_strategy_type rolling_strategy_type) {
    RollingStrategy_InitStrategyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.code = code;
    data.rolling_strategy_type = rolling_strategy_type;
    result = Function::Call(&gRollingStrategy_InitStrategyHandle, &data);
    return result;
}

static inline Result RollingStrategy_CallToPosition(int speaker_id, enum Type_intensity intensity, enum Type_position position, enum Type_subject_battalion subject_battalion, enum Type_subject_call_sign_id subject_call_sign_id) {
    RollingStrategy_CallToPositionStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    data.position = position;
    data.subject_battalion = subject_battalion;
    data.subject_call_sign_id = subject_call_sign_id;
    result = Function::Call(&gRollingStrategy_CallToPositionHandle, &data);
    return result;
}

static inline Result RollingStrategy_CallToPositionRem(int speaker_id, enum Type_intensity intensity) {
    RollingStrategy_CallToPositionRemStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gRollingStrategy_CallToPositionRemHandle, &data);
    return result;
}

static inline Result RollingStrategy_StrategyExecute(int speaker_id, enum Type_intensity intensity) {
    RollingStrategy_StrategyExecuteStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gRollingStrategy_StrategyExecuteHandle, &data);
    return result;
}

static inline Result Outcome_AnticipateFail(int speaker_id) {
    Outcome_AnticipateFailStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gOutcome_AnticipateFailHandle, &data);
    return result;
}

static inline Result Outcome_AnticipateSuccess(int speaker_id) {
    Outcome_AnticipateSuccessStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gOutcome_AnticipateSuccessHandle, &data);
    return result;
}

static inline Result Outcome_OutcomeFail(int speaker_id, enum Type_intensity intensity) {
    Outcome_OutcomeFailStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gOutcome_OutcomeFailHandle, &data);
    return result;
}

static inline Result Outcome_StrategyReset(int speaker_id, enum Type_same_new same_new, enum Type_intensity intensity) {
    Outcome_StrategyResetStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.same_new = same_new;
    data.intensity = intensity;
    result = Function::Call(&gOutcome_StrategyResetHandle, &data);
    return result;
}

static inline Result Arrest_BullhornArrest(int speaker_id, enum Type_intensity intensity) {
    Arrest_BullhornArrestStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gArrest_BullhornArrestHandle, &data);
    return result;
}

static inline Result Arrest_Arrest(int speaker_id, enum Type_intensity intensity) {
    Arrest_ArrestStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gArrest_ArrestHandle, &data);
    return result;
}

static inline Result Arrest_DispArrestReply(int speaker_id) {
    Arrest_DispArrestReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gArrest_DispArrestReplyHandle, &data);
    return result;
}

static inline Result AnytimeEvents_CollisionWorld(int speaker_id, enum Type_world_object_type world_object_type, enum Type_num_units num_units) {
    AnytimeEvents_CollisionWorldStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.world_object_type = world_object_type;
    data.num_units = num_units;
    result = Function::Call(&gAnytimeEvents_CollisionWorldHandle, &data);
    return result;
}

static inline Result AnytimeEvents_CollWorld_Civi(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_CollWorld_CiviStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_CollWorld_CiviHandle, &data);
    return result;
}

static inline Result AnytimeEvents_CollWorld_Spin(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_CollWorld_SpinStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_CollWorld_SpinHandle, &data);
    return result;
}

static inline Result AnytimeEvents_CollWorld_Air(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_CollWorld_AirStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_CollWorld_AirHandle, &data);
    return result;
}

static inline Result AnytimeEvents_CollWorld_Flip(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_CollWorld_FlipStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_CollWorld_FlipHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DispPursuitUpdate(int speaker_id, enum Type_subject_battalion subject_battalion, enum Type_subject_call_sign_id subject_call_sign_id) {
    AnytimeEvents_DispPursuitUpdateStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.subject_battalion = subject_battalion;
    data.subject_call_sign_id = subject_call_sign_id;
    result = Function::Call(&gAnytimeEvents_DispPursuitUpdateHandle, &data);
    return result;
}

static inline Result AnytimeEvents_PursuitUpdateRep(int speaker_id) {
    AnytimeEvents_PursuitUpdateRepStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gAnytimeEvents_PursuitUpdateRepHandle, &data);
    return result;
}

static inline Result AnytimeEvents_Disp911Report(int speaker_id, enum Type_pursuit_type pursuit_type, enum Type_num_suspects num_suspects, enum Type_encounter encounter, enum Type_direction direction, enum Type_location location, enum Type_location_region location_region, enum Type_address_group_type address_group_type) {
    AnytimeEvents_Disp911ReportStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.pursuit_type = pursuit_type;
    data.num_suspects = num_suspects;
    data.encounter = encounter;
    data.direction = direction;
    data.location = location;
    data.location_region = location_region;
    data.address_group_type = address_group_type;
    result = Function::Call(&gAnytimeEvents_Disp911ReportHandle, &data);
    return result;
}

static inline Result AnytimeEvents_Disp911CsPnt(int speaker_id, enum Type_address_group_type address_group_type, enum Type_pursuit_type pursuit_type, enum Type_num_suspects num_suspects, enum Type_encounter encounter, enum Type_direction direction, enum Type_location_region location_region, enum Type_location location, enum Type_car_type car_type) {
    AnytimeEvents_Disp911CsPntStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.address_group_type = address_group_type;
    data.pursuit_type = pursuit_type;
    data.num_suspects = num_suspects;
    data.encounter = encounter;
    data.direction = direction;
    data.location_region = location_region;
    data.location = location;
    data.car_type = car_type;
    result = Function::Call(&gAnytimeEvents_Disp911CsPntHandle, &data);
    return result;
}

static inline Result AnytimeEvents_Disp911NoDescrip(int speaker_id, enum Type_address_group_type address_group_type, enum Type_pursuit_type pursuit_type, enum Type_num_suspects num_suspects, enum Type_encounter encounter, enum Type_direction direction, enum Type_location_region location_region, enum Type_location location, enum Type_car_type car_type) {
    AnytimeEvents_Disp911NoDescripStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.address_group_type = address_group_type;
    data.pursuit_type = pursuit_type;
    data.num_suspects = num_suspects;
    data.encounter = encounter;
    data.direction = direction;
    data.location_region = location_region;
    data.location = location;
    data.car_type = car_type;
    result = Function::Call(&gAnytimeEvents_Disp911NoDescripHandle, &data);
    return result;
}

static inline Result AnytimeEvents_Unit911Reply(int speaker_id) {
    AnytimeEvents_Unit911ReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gAnytimeEvents_Unit911ReplyHandle, &data);
    return result;
}

static inline Result AnytimeEvents_SuspectUTurn(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_SuspectUTurnStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_SuspectUTurnHandle, &data);
    return result;
}

static inline Result AnytimeEvents_SuspectOutrun(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_SuspectOutrunStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_SuspectOutrunHandle, &data);
    return result;
}

static inline Result AnytimeEvents_LostVisual(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_LostVisualStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_LostVisualHandle, &data);
    return result;
}

static inline Result AnytimeEvents_RegainVisual(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_RegainVisualStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_RegainVisualHandle, &data);
    return result;
}

static inline Result AnytimeEvents_LostSuspect(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_LostSuspectStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_LostSuspectHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DispBreakAway(int speaker_id, enum Type_location_region location_region, enum Type_location location, enum Type_direction direction) {
    AnytimeEvents_DispBreakAwayStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.location_region = location_region;
    data.location = location;
    data.direction = direction;
    result = Function::Call(&gAnytimeEvents_DispBreakAwayHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DispTimeExpired(int speaker_id) {
    AnytimeEvents_DispTimeExpiredStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gAnytimeEvents_DispTimeExpiredHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DispPursuitEscalation(int speaker_id, enum Type_address_group_type address_group_type, enum Type_pursuit_type pursuit_type, enum Type_num_suspects num_suspects, enum Type_direction direction, enum Type_location location, enum Type_location_region location_region) {
    AnytimeEvents_DispPursuitEscalationStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.address_group_type = address_group_type;
    data.pursuit_type = pursuit_type;
    data.num_suspects = num_suspects;
    data.direction = direction;
    data.location = location;
    data.location_region = location_region;
    result = Function::Call(&gAnytimeEvents_DispPursuitEscalationHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DispPursEscGen(int speaker_id, enum Type_num_suspects num_suspects) {
    AnytimeEvents_DispPursEscGenStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_suspects = num_suspects;
    result = Function::Call(&gAnytimeEvents_DispPursEscGenHandle, &data);
    return result;
}

static inline Result AnytimeEvents_UnitDisabled(int speaker_id, enum Type_intensity intensity, enum Type_self_other self_other) {
    AnytimeEvents_UnitDisabledStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    data.self_other = self_other;
    result = Function::Call(&gAnytimeEvents_UnitDisabledHandle, &data);
    return result;
}

static inline Result AnytimeEvents_CallForEV(int speaker_id, enum Type_ev_type ev_type) {
    AnytimeEvents_CallForEVStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.ev_type = ev_type;
    result = Function::Call(&gAnytimeEvents_CallForEVHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DispEVReply(int speaker_id) {
    AnytimeEvents_DispEVReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gAnytimeEvents_DispEVReplyHandle, &data);
    return result;
}

static inline Result AnytimeEvents_IntentToRam(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_IntentToRamStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_IntentToRamHandle, &data);
    return result;
}

static inline Result AnytimeEvents_Bailout(int speaker_id, enum Type_bailout_type bailout_type) {
    AnytimeEvents_BailoutStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.bailout_type = bailout_type;
    result = Function::Call(&gAnytimeEvents_BailoutHandle, &data);
    return result;
}

static inline Result AnytimeEvents_BailoutDeny(int speaker_id) {
    AnytimeEvents_BailoutDenyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gAnytimeEvents_BailoutDenyHandle, &data);
    return result;
}

static inline Result AnytimeEvents_FocusChange(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_FocusChangeStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_FocusChangeHandle, &data);
    return result;
}

static inline Result AnytimeEvents_SuspectBehaviour(int speaker_id, enum Type_num_suspects num_suspects) {
    AnytimeEvents_SuspectBehaviourStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.num_suspects = num_suspects;
    result = Function::Call(&gAnytimeEvents_SuspectBehaviourHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DriverHistory(int speaker_id, enum Type_region region) {
    AnytimeEvents_DriverHistoryStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.region = region;
    result = Function::Call(&gAnytimeEvents_DriverHistoryHandle, &data);
    return result;
}

static inline Result AnytimeEvents_OffroadMoment(int speaker_id, enum Type_offroad_moment_id offroad_moment_id, enum Type_first_subsequent first_subsequent) {
    AnytimeEvents_OffroadMomentStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.offroad_moment_id = offroad_moment_id;
    data.first_subsequent = first_subsequent;
    result = Function::Call(&gAnytimeEvents_OffroadMomentHandle, &data);
    return result;
}

static inline Result AnytimeEvents_Spotted(int speaker_id, enum Type_intensity intensity) {
    AnytimeEvents_SpottedStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gAnytimeEvents_SpottedHandle, &data);
    return result;
}

static inline Result AnytimeEvents_SuspectBrake(int speaker_id) {
    AnytimeEvents_SuspectBrakeStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gAnytimeEvents_SuspectBrakeHandle, &data);
    return result;
}

static inline Result AnytimeEvents_WeatherReport(int speaker_id) {
    AnytimeEvents_WeatherReportStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gAnytimeEvents_WeatherReportHandle, &data);
    return result;
}

static inline Result AnytimeEvents_HeatJump(int speaker_id, enum Type_heat_level heat_level) {
    AnytimeEvents_HeatJumpStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.heat_level = heat_level;
    result = Function::Call(&gAnytimeEvents_HeatJumpHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DirectionHigh(int speaker_id, enum Type_direction direction) {
    AnytimeEvents_DirectionHighStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.direction = direction;
    result = Function::Call(&gAnytimeEvents_DirectionHighHandle, &data);
    return result;
}

static inline Result AnytimeEvents_DispJurisShift(int speaker_id, enum Type_jurisdiction jurisdiction) {
    AnytimeEvents_DispJurisShiftStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.jurisdiction = jurisdiction;
    result = Function::Call(&gAnytimeEvents_DispJurisShiftHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliSelfStrategy(int speaker_id, enum Type_heli_self_strategy_type heli_self_strategy_type) {
    HeliSpecific_HeliSelfStrategyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.heli_self_strategy_type = heli_self_strategy_type;
    result = Function::Call(&gHeliSpecific_HeliSelfStrategyHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliLostVisual(int speaker_id, enum Type_heli_lost_visual heli_lost_visual) {
    HeliSpecific_HeliLostVisualStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.heli_lost_visual = heli_lost_visual;
    result = Function::Call(&gHeliSpecific_HeliLostVisualHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliIntentToBail(int speaker_id, enum Type_heli_bailout_type heli_bailout_type) {
    HeliSpecific_HeliIntentToBailStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.heli_bailout_type = heli_bailout_type;
    result = Function::Call(&gHeliSpecific_HeliIntentToBailHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliBailout(int speaker_id, enum Type_heli_bailout_type heli_bailout_type) {
    HeliSpecific_HeliBailoutStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.heli_bailout_type = heli_bailout_type;
    result = Function::Call(&gHeliSpecific_HeliBailoutHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliSwarming(int speaker_id) {
    HeliSpecific_HeliSwarmingStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gHeliSpecific_HeliSwarmingHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliSpotter(int speaker_id) {
    HeliSpecific_HeliSpotterStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gHeliSpecific_HeliSpotterHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliHazardAlert(int speaker_id, enum Type_heli_hazard_alert_type heli_hazard_alert_type) {
    HeliSpecific_HeliHazardAlertStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.heli_hazard_alert_type = heli_hazard_alert_type;
    result = Function::Call(&gHeliSpecific_HeliHazardAlertHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliQuadrent(int speaker_id) {
    HeliSpecific_HeliQuadrentStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gHeliSpecific_HeliQuadrentHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliQuadrentMoving(int speaker_id) {
    HeliSpecific_HeliQuadrentMovingStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gHeliSpecific_HeliQuadrentMovingHandle, &data);
    return result;
}

static inline Result HeliSpecific_HeliBullhornArrest(int speaker_id) {
    HeliSpecific_HeliBullhornArrestStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gHeliSpecific_HeliBullhornArrestHandle, &data);
    return result;
}

static inline Result E3_Events_E3_Setup() {
    E3_Events_E3_SetupStruct data;
    Result result;
    result = Function::Call(&gE3_Events_E3_SetupHandle, &data);
    return result;
}

static inline Result Interrupts_Interrupt(int speaker_id, enum Type_intensity intensity) {
    Interrupts_InterruptStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gInterrupts_InterruptHandle, &data);
    return result;
}

static inline Result Interrupts_InterruptRam(int speaker_id) {
    Interrupts_InterruptRamStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gInterrupts_InterruptRamHandle, &data);
    return result;
}

static inline Result Interrupts_InterruptRam_RE(int speaker_id, enum Type_intensity intensity) {
    Interrupts_InterruptRam_REStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gInterrupts_InterruptRam_REHandle, &data);
    return result;
}

static inline Result Interrupts_InterruptRam_HO(int speaker_id, enum Type_intensity intensity) {
    Interrupts_InterruptRam_HOStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gInterrupts_InterruptRam_HOHandle, &data);
    return result;
}

static inline Result Interrupts_InterruptRam_SS(int speaker_id, enum Type_intensity intensity) {
    Interrupts_InterruptRam_SSStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gInterrupts_InterruptRam_SSHandle, &data);
    return result;
}

static inline Result Interrupts_InterruptRam_TB(int speaker_id, enum Type_intensity intensity) {
    Interrupts_InterruptRam_TBStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gInterrupts_InterruptRam_TBHandle, &data);
    return result;
}

static inline Result Interrupts_InterruptRamHigh(int speaker_id) {
    Interrupts_InterruptRamHighStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gInterrupts_InterruptRamHighHandle, &data);
    return result;
}

static inline Result Interrupts_StaticInterrupt() {
    Interrupts_StaticInterruptStruct data;
    Result result;
    result = Function::Call(&gInterrupts_StaticInterruptHandle, &data);
    return result;
}

static inline Result Interrupts_RegainVisualInterrupt(int speaker_id, enum Type_intensity intensity) {
    Interrupts_RegainVisualInterruptStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.intensity = intensity;
    result = Function::Call(&gInterrupts_RegainVisualInterruptHandle, &data);
    return result;
}

static inline Result CellCall(enum Type_cell_call_bucket cell_call_bucket, enum Type_cell_call_number cell_call_number) {
    CellCallStruct data;
    Result result;
    data.cell_call_bucket = cell_call_bucket;
    data.cell_call_number = cell_call_number;
    result = Function::Call(&gCellCallHandle, &data);
    return result;
}

static inline Result ExtraCops_SwarmingReply() {
    ExtraCops_SwarmingReplyStruct data;
    Result result;
    result = Function::Call(&gExtraCops_SwarmingReplyHandle, &data);
    return result;
}

static inline Result ExtraCops_SuperPursuitReply() {
    ExtraCops_SuperPursuitReplyStruct data;
    Result result;
    result = Function::Call(&gExtraCops_SuperPursuitReplyHandle, &data);
    return result;
}

static inline Result ExtraCops_SwarmingReplyFollow() {
    ExtraCops_SwarmingReplyFollowStruct data;
    Result result;
    result = Function::Call(&gExtraCops_SwarmingReplyFollowHandle, &data);
    return result;
}

static inline Result ExtraCops_QuadrentForming() {
    ExtraCops_QuadrentFormingStruct data;
    Result result;
    result = Function::Call(&gExtraCops_QuadrentFormingHandle, &data);
    return result;
}

static inline Result ExtraCops_SuspectPossiblyGone() {
    ExtraCops_SuspectPossiblyGoneStruct data;
    Result result;
    result = Function::Call(&gExtraCops_SuspectPossiblyGoneHandle, &data);
    return result;
}

static inline Result ExtraCops_QuadrentMoving() {
    ExtraCops_QuadrentMovingStruct data;
    Result result;
    result = Function::Call(&gExtraCops_QuadrentMovingHandle, &data);
    return result;
}

static inline Result ExtraCops_OtherLead() {
    ExtraCops_OtherLeadStruct data;
    Result result;
    result = Function::Call(&gExtraCops_OtherLeadHandle, &data);
    return result;
}

static inline Result ExtraCops_PossibleSuspect() {
    ExtraCops_PossibleSuspectStruct data;
    Result result;
    result = Function::Call(&gExtraCops_PossibleSuspectHandle, &data);
    return result;
}

static inline Result ExtraCops_WrongSuspect() {
    ExtraCops_WrongSuspectStruct data;
    Result result;
    result = Function::Call(&gExtraCops_WrongSuspectHandle, &data);
    return result;
}

static inline Result ExtraCops_SuspectGone() {
    ExtraCops_SuspectGoneStruct data;
    Result result;
    result = Function::Call(&gExtraCops_SuspectGoneHandle, &data);
    return result;
}

static inline Result ExtraCops_RBWarning() {
    ExtraCops_RBWarningStruct data;
    Result result;
    result = Function::Call(&gExtraCops_RBWarningHandle, &data);
    return result;
}

static inline Result ExtraCops_RBPosition(enum Type_spikebelt_position spikebelt_position) {
    ExtraCops_RBPositionStruct data;
    Result result;
    data.spikebelt_position = spikebelt_position;
    result = Function::Call(&gExtraCops_RBPositionHandle, &data);
    return result;
}

static inline Result ExtraCops_ExtraRBEngage(enum Type_roadblock_engage_type roadblock_engage_type) {
    ExtraCops_ExtraRBEngageStruct data;
    Result result;
    data.roadblock_engage_type = roadblock_engage_type;
    result = Function::Call(&gExtraCops_ExtraRBEngageHandle, &data);
    return result;
}

static inline Result ExtraCops_ExtraRBAverted(enum Type_roadblock_engage_type roadblock_engage_type) {
    ExtraCops_ExtraRBAvertedStruct data;
    Result result;
    data.roadblock_engage_type = roadblock_engage_type;
    result = Function::Call(&gExtraCops_ExtraRBAvertedHandle, &data);
    return result;
}

static inline Result Cross_CrossBUReply(int speaker_id) {
    Cross_CrossBUReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gCross_CrossBUReplyHandle, &data);
    return result;
}

static inline Result Cross_CrossFailReply(int speaker_id, enum Type_on_off_scene on_off_scene) {
    Cross_CrossFailReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.on_off_scene = on_off_scene;
    result = Function::Call(&gCross_CrossFailReplyHandle, &data);
    return result;
}

static inline Result Cross_CrossRBFailReply(int speaker_id, enum Type_on_off_scene on_off_scene) {
    Cross_CrossRBFailReplyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    data.on_off_scene = on_off_scene;
    result = Function::Call(&gCross_CrossRBFailReplyHandle, &data);
    return result;
}

static inline Result Cross_CrossPursuitEsc(int speaker_id) {
    Cross_CrossPursuitEscStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gCross_CrossPursuitEscHandle, &data);
    return result;
}

static inline Result Cross_CrossSelfStrategy(int speaker_id) {
    Cross_CrossSelfStrategyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gCross_CrossSelfStrategyHandle, &data);
    return result;
}

static inline Result Cross_CrossMultiStrategy(int speaker_id) {
    Cross_CrossMultiStrategyStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gCross_CrossMultiStrategyHandle, &data);
    return result;
}

static inline Result Cross_CrossBailoutDeny_sub(int speaker_id) {
    Cross_CrossBailoutDeny_subStruct data;
    Result result;
    data.speaker_id = speaker_id;
    result = Function::Call(&gCross_CrossBailoutDeny_subHandle, &data);
    return result;
}

static inline Result D_Day() {
    D_DayStruct data;
    Result result;
    result = Function::Call(&gD_DayHandle, &data);
    return result;
}

static inline Result DispIntroRace() {
    DispIntroRaceStruct data;
    Result result;
    result = Function::Call(&gDispIntroRaceHandle, &data);
    return result;
}

}; // namespace Csis

#endif
