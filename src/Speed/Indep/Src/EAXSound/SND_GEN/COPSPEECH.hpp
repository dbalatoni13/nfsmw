#ifndef Events_H
#define Events_H

#include "csis/csis.h"
namespace Csis {

// Decl: 349
enum Type_code {
    Type_code_Use_10_code = 1,
    Type_code_dont_use_10_code = 2,
};

// Decl: 356
enum Type_backup_type {
    Type_backup_type_all_available_units = 1,
    Type_backup_type_Air_support = 2,
    Type_backup_type_Rhino_Unit = 4,
    Type_backup_type_generic_more_backup = 8,
};

// Decl: 365
enum Type_bailout_type {
    Type_bailout_type_Generic_low_intensity = 1,
    Type_bailout_type_Heavy_traffic = 2,
    Type_bailout_type_Road_conditions = 4,
    Type_bailout_type_Damage_Sustained = 8,
    Type_bailout_type_Generic_high_intensity = 16,
};

// Decl: 375
enum Type_disp_backup_type {
    Type_disp_backup_type_No_available_units = 1,
    Type_disp_backup_type_1_unit = 2,
    Type_disp_backup_type_all_available_units = 4,
    Type_disp_backup_type_Air_Support = 8,
    Type_disp_backup_type_Rhino_Unit = 16,
    Type_disp_backup_type_Generic_ground_units = 32,
    Type_disp_backup_type_Super_Pursuit = 64,
};

// Decl: 387
enum Type_encounter {
    Type_encounter_first_encounter = 1,
    Type_encounter_subsequent_encounter = 2,
};

// Decl: 394
enum Type_ev_type {
    Type_ev_type_multiple_units_down = 1,
    Type_ev_type_gas_station_fire = 2,
    Type_ev_type_heli_down = 4,
};

// Decl: 402
enum Type_heli_lost_visual {
    Type_heli_lost_visual_In_tunnel = 1,
    Type_heli_lost_visual_behind_building = 2,
    Type_heli_lost_visual_Under_trees = 4,
    Type_heli_lost_visual_Generic = 8,
    Type_heli_lost_visual_Restricted_Airspace = 16,
};

// Decl: 412
enum Type_intensity {
    Type_intensity_Normal = 1,
    Type_intensity_High = 2,
};

// Decl: 419
enum Type_num_suspects {
    Type_num_suspects_one_suspect = 1,
    Type_num_suspects_multiple_suspects = 2,
};

// Decl: 426
enum Type_num_units {
    Type_num_units_one_unit_in_pursuit = 1,
    Type_num_units_multiple_units_in_pursuit = 2,
};

// Decl: 433
enum Type_position {
    Type_position_Right_Side = 1,
    Type_position_Left_Side = 2,
    Type_position_Ahead = 4,
    Type_position_Behind = 8,
};

// Decl: 449
enum Type_pursuit_type {
    Type_pursuit_type_Generic_Speeder = 1,
    Type_pursuit_type_Possible_Wanted = 2,
    Type_pursuit_type_Hit_and_Run = 4,
    Type_pursuit_type_Reckless = 8,
    Type_pursuit_type_Unit_Rammed = 16,
};

// Decl: 459
enum Type_region {
    Type_region_College_Town = 1,
    Type_region_Coastal = 2,
    Type_region_City = 4,
    Type_region_Alpine = 8,
};

// Decl: 468
enum Type_roadblock_type {
    Type_roadblock_type_Roadblock_Generic_ = 1,
    Type_roadblock_type_Spikes = 2,
    Type_roadblock_type_Heli_Roadblock_disp_only_ = 4,
    Type_roadblock_type_Multiple_Roadblocks_disp_only_ = 8,
};

// Decl: 477
enum Type_roadblock_engage_type {
    Type_roadblock_engage_type_roadblock = 1,
    Type_roadblock_engage_type_spikes = 2,
};

// Decl: 496
enum Type_rolling_strategy_type {
    Type_rolling_strategy_type_Offset = 1,
    Type_rolling_strategy_type_Rolling_Roadblock = 2,
    Type_rolling_strategy_type_Box = 4,
    Type_rolling_strategy_type_Rolling_Chicane = 8,
    Type_rolling_strategy_type_Herding = 16,
    Type_rolling_strategy_type_Pit = 32,
    Type_rolling_strategy_type_Smokescreen = 64,
    Type_rolling_strategy_type_Rolling_Spikes = 128,
};

// Decl: 509
enum Type_same_new {
    Type_same_new_same_strategy = 1,
    Type_same_new_new_strategy = 2,
};

// Decl: 516
enum Type_self_strategy_type {
    Type_self_strategy_type_Offset = 1,
    Type_self_strategy_type_Rolling_Roadblock = 2,
    Type_self_strategy_type_Herding = 4,
    Type_self_strategy_type_Ram = 8,
    Type_self_strategy_type_Side_Ram = 16,
    Type_self_strategy_type_Pit = 32,
    Type_self_strategy_type_Self_Pit = 64,
    Type_self_strategy_type_Rolling_Spikes = 128,
    Type_self_strategy_type_Smokescreen = 256,
};

// Decl: 530
enum Type_world_object_type {
    Type_world_object_type_generic = 1,
    Type_world_object_type_guardrail = 2,
    Type_world_object_type_train = 4,
    Type_world_object_type_semi = 8,
    Type_world_object_type_gas_station = 16,
    Type_world_object_type_spike_belt = 32,
    Type_world_object_type_low_heat_collision = 64,
};

// Decl: 542
enum Type_yes_no {
    Type_yes_no_Yes_True = 1,
    Type_yes_no_No_False = 2,
};

// Decl: 549
enum Type_speaker_battalion {
    Type_speaker_battalion_Rosewood = 1,
    Type_speaker_battalion_Coastal = 2,
    Type_speaker_battalion_City = 4,
    Type_speaker_battalion_Alpine = 8,
    Type_speaker_battalion_Super_Pursuit = 16,
    Type_speaker_battalion_Rhino_Units = 32,
    Type_speaker_battalion_Air_Support = 64,
    Type_speaker_battalion_All_Units = 128,
    Type_speaker_battalion_Dispatch = 256,
};

// Decl: 563
enum Type_subject_battalion {
    Type_subject_battalion_Rosewood = 1,
    Type_subject_battalion_Coastal = 2,
    Type_subject_battalion_City = 4,
    Type_subject_battalion_Alpine = 8,
    Type_subject_battalion_Super_Pursuit = 16,
    Type_subject_battalion_Rhino_Units = 32,
    Type_subject_battalion_Air_Support = 64,
    Type_subject_battalion_All_Units = 128,
};

// Decl: 576
enum Type_speaker_call_sign_id {
    Type_speaker_call_sign_id_CallSign01 = 1,
    Type_speaker_call_sign_id_CallSign02 = 2,
    Type_speaker_call_sign_id_CallSign03 = 4,
    Type_speaker_call_sign_id_CallSign04 = 8,
    Type_speaker_call_sign_id_CallSign05 = 16,
    Type_speaker_call_sign_id_CallSign06 = 32,
    Type_speaker_call_sign_id_CallSign07 = 64,
    Type_speaker_call_sign_id_CallSign08 = 128,
    Type_speaker_call_sign_id_CallSign09 = 256,
    Type_speaker_call_sign_id_CallSign10 = 512,
    Type_speaker_call_sign_id_CallSign11 = 1024,
    Type_speaker_call_sign_id_CallSign12 = 2048,
    Type_speaker_call_sign_id_CallSign13 = 4096,
    Type_speaker_call_sign_id_CallSign14 = 8192,
    Type_speaker_call_sign_id_CallSign15 = 16384,
    Type_speaker_call_sign_id_CallSign16 = 32768,
    Type_speaker_call_sign_id_CallSign17 = 65536,
    Type_speaker_call_sign_id_CallSign18 = 131072,
    Type_speaker_call_sign_id_CallSign19 = 262144,
    Type_speaker_call_sign_id_CallSign20 = 524288,
    Type_speaker_call_sign_id_All_units = 1048576,
    Type_speaker_call_sign_id_none = 2097152,
};

// Decl: 603
enum Type_subject_call_sign_id {
    Type_subject_call_sign_id_CallSign01 = 1,
    Type_subject_call_sign_id_CallSign02 = 2,
    Type_subject_call_sign_id_CallSign03 = 4,
    Type_subject_call_sign_id_CallSign04 = 8,
    Type_subject_call_sign_id_CallSign05 = 16,
    Type_subject_call_sign_id_CallSign06 = 32,
    Type_subject_call_sign_id_CallSign07 = 64,
    Type_subject_call_sign_id_CallSign08 = 128,
    Type_subject_call_sign_id_CallSign09 = 256,
    Type_subject_call_sign_id_CallSign10 = 512,
    Type_subject_call_sign_id_CallSign11 = 1024,
    Type_subject_call_sign_id_CallSign12 = 2048,
    Type_subject_call_sign_id_CallSign13 = 4096,
    Type_subject_call_sign_id_CallSign14 = 8192,
    Type_subject_call_sign_id_CallSign15 = 16384,
    Type_subject_call_sign_id_CallSign16 = 32768,
    Type_subject_call_sign_id_CallSign17 = 65536,
    Type_subject_call_sign_id_CallSign18 = 131072,
    Type_subject_call_sign_id_CallSign19 = 262144,
    Type_subject_call_sign_id_CallSign20 = 524288,
    Type_subject_call_sign_id_district_units = 1048576,
    Type_subject_call_sign_id_none = 2097152,
    Type_subject_call_sign_id_pursuit_units = 4194304,
    Type_subject_call_sign_id_cover_units = 8388608,
    Type_subject_call_sign_id_all_units = 16777216,
};

// Decl: 633
enum Type_time_since_lost {
    Type_time_since_lost_lost_recently = 1,
    Type_time_since_lost_lost_for_X_time = 2,
};

// Decl: 640
enum Type_self_other {
    Type_self_other_context_about_self = 1,
    Type_self_other_context_about_other_unit = 2,
};

// Decl: 647
enum Type_heli_bailout_type {
    Type_heli_bailout_type_flight_conditions = 1,
    Type_heli_bailout_type_low_ammo = 2,
    Type_heli_bailout_type_fuel_low = 4,
    Type_heli_bailout_type_damage_sustained = 8,
};

// Decl: 656
enum Type_offroad_moment_id {
    Type_offroad_moment_id_gas_station = 1,
    Type_offroad_moment_id_campus = 2,
    Type_offroad_moment_id_golf_course = 4,
    Type_offroad_moment_id_hospital = 8,
    Type_offroad_moment_id_strip_mall = 16,
    Type_offroad_moment_id_stadium = 32,
    Type_offroad_moment_id_park = 64,
    Type_offroad_moment_id_trailer_park = 128,
    Type_offroad_moment_id_junkyard = 256,
    Type_offroad_moment_id_boatyard = 512,
    Type_offroad_moment_id_refinery = 1024,
    Type_offroad_moment_id_trainyard = 2048,
    Type_offroad_moment_id_boardwalk = 4096,
    Type_offroad_moment_id_beach = 8192,
    Type_offroad_moment_id_subway = 16384,
    Type_offroad_moment_id_hotel = 32768,
    Type_offroad_moment_id_museum = 65536,
    Type_offroad_moment_id_police_station = 131072,
    Type_offroad_moment_id_hydro_plant = 262144,
    Type_offroad_moment_id_construction_yard = 524288,
    Type_offroad_moment_id_bus_station = 1048576,
    Type_offroad_moment_id_drive_in_theatre = 2097152,
    Type_offroad_moment_id_penitentiary = 4194304,
    Type_offroad_moment_id_fishery = 8388608,
};

// Decl: 685
enum Type_first_subsequent {
    Type_first_subsequent_first_time = 1,
    Type_first_subsequent_subsequent_time = 2,
};

// Decl: 692
enum Type_address_group_type {
    Type_address_group_type_college_town = 1,
    Type_address_group_type_city = 2,
    Type_address_group_type_coastal = 4,
    Type_address_group_type_alpine = 8,
    Type_address_group_type_generic_any_ = 16,
};

// Decl: 702
enum Type_heli_self_strategy_type {
    Type_heli_self_strategy_type_offset_folloe = 1,
    Type_heli_self_strategy_type_heli_roadblock = 2,
    Type_heli_self_strategy_type_ram = 4,
    Type_heli_self_strategy_type_smokescreen = 8,
    Type_heli_self_strategy_type_rolling_spikes = 16,
};

// Decl: 712
enum Type_heli_hazard_alert_type {
    Type_heli_hazard_alert_type_windy_roads = 1,
    Type_heli_hazard_alert_type_approaching_highway = 2,
    Type_heli_hazard_alert_type_approaching_tunnel = 4,
    Type_heli_hazard_alert_type_approaching_city = 8,
    Type_heli_hazard_alert_type_approaching_airport = 16,
    Type_heli_hazard_alert_type_approaching_blimp = 32,
};

// Decl: 760
enum Type_heat_level {
    Type_heat_level_2 = 1,
    Type_heat_level_3 = 2,
    Type_heat_level_4 = 4,
    Type_heat_level_5 = 8,
    Type_heat_level_6 = 16,
    Type_heat_level_7 = 32,
    Type_heat_level_8 = 64,
    Type_heat_level_9 = 128,
    Type_heat_level_10 = 256,
};

// Decl: 774
enum Type_jurisdiction {
    Type_jurisdiction_state = 1,
    Type_jurisdiction_federal = 2,
};

// Decl: 781
enum Type_disp_backup_eta {
    Type_disp_backup_eta_15sec = 1,
    Type_disp_backup_eta_30sec = 2,
    Type_disp_backup_eta_1min = 4,
    Type_disp_backup_eta_1min30sec = 8,
    Type_disp_backup_eta_2min = 16,
    Type_disp_backup_eta_2min_ = 32,
};

// Decl: 792
enum Type_spikebelt_position {
    Type_spikebelt_position_left = 1,
    Type_spikebelt_position_center = 2,
    Type_spikebelt_position_right = 4,
};

// Decl: 807
enum Type_car_type {
    Type_car_type_McLaren = 1,
    Type_car_type_Porsche = 2,
    Type_car_type_FordGT = 4,
    Type_car_type_Viper = 8,
    Type_car_type_AstonMartin = 16,
    Type_car_type_Corvette = 32,
    Type_car_type_BMW = 64,
    Type_car_type_Mercedes = 128,
    Type_car_type_Audi = 256,
    Type_car_type_Mitsubishi = 512,
    Type_car_type_Mustang = 1024,
    Type_car_type_Lotus = 2048,
    Type_car_type_Subaru = 4096,
    Type_car_type_Camaro = 8192,
    Type_car_type_GTO = 16384,
    Type_car_type_Nissan = 32768,
    Type_car_type_Mazda = 65536,
    Type_car_type_Renault = 131072,
    Type_car_type_Lexus = 262144,
    Type_car_type_Mini = 524288,
    Type_car_type_Volkswagen = 1048576,
    Type_car_type_SUV = 2097152,
    Type_car_type_Pickup = 4194304,
    Type_car_type_Pagani = 8388608,
    Type_car_type_Lamborghini = 16777216,
    Type_car_type_Chrysler = 33554432,
    Type_car_type_Opel = 67108864,
    Type_car_type_Sedan = 134217728,
    Type_car_type_SportsCar = 268435456,
    Type_car_type_Supra = 536870912,
    Type_car_type_Cadillac = 1073741824,
};

// Decl: 843
enum Type_direction {
    Type_direction_Northbound = 1,
    Type_direction_Southbound = 2,
    Type_direction_Eastbound = 4,
    Type_direction_Westbound = 8,
};

// Decl: 852
enum Type_car_color {
    Type_car_color_Red = 1,
    Type_car_color_Black = 2,
    Type_car_color_White = 4,
    Type_car_color_Blue = 8,
    Type_car_color_Green = 16,
    Type_car_color_Orange = 32,
    Type_car_color_Silver = 64,
    Type_car_color_Gold = 128,
    Type_car_color_Purple = 256,
    Type_car_color_Brown = 512,
    Type_car_color_Yellow = 1024,
    Type_car_color_Pink = 2048,
    Type_car_color_Beige = 4096,
};

// Decl: 870
enum Type_speed {
    Type_speed_over_speed_limit = 1,
    Type_speed_speeds_in_excess_of_100 = 2,
    Type_speed_speeds_in_excess_of_120 = 4,
    Type_speed_speeds_in_excess_of_140 = 8,
    Type_speed_speeds_in_excess_of_160 = 16,
    Type_speed_speeds_in_excess_of_180 = 32,
    Type_speed_speeds_in_excess_of_200 = 64,
    Type_speed_speeds_in_excess_of_220 = 128,
    Type_speed_speeds_in_excess_of_240 = 256,
    Type_speed_speeds_in_excess_of_260 = 512,
    Type_speed_speeds_in_excess_of_280 = 1024,
    Type_speed_speeds_in_excess_of_300 = 2048,
};

// Decl: 887
enum Type_location {
    Type_location_Location_1 = 1,
    Type_location_Location_2 = 2,
    Type_location_Location_3 = 4,
    Type_location_Location_4 = 8,
    Type_location_Location_5 = 16,
    Type_location_Location_6 = 32,
    Type_location_Location_7 = 64,
    Type_location_Location_8 = 128,
    Type_location_Location_9 = 256,
    Type_location_Location_10 = 512,
    Type_location_Location_11 = 1024,
    Type_location_Location_12 = 2048,
    Type_location_Location_13 = 4096,
    Type_location_Location_14 = 8192,
    Type_location_Location_15 = 16384,
    Type_location_Location_16 = 32768,
    Type_location_Location_17 = 65536,
    Type_location_Location_18 = 131072,
    Type_location_Location_19 = 262144,
    Type_location_Location_20 = 524288,
    Type_location_Location_21 = 1048576,
    Type_location_Location_22 = 2097152,
    Type_location_Location_23 = 4194304,
    Type_location_Location_24 = 8388608,
    Type_location_Location_25 = 16777216,
    Type_location_Location_26 = 33554432,
    Type_location_Location_27 = 67108864,
    Type_location_Location_28 = 134217728,
    Type_location_Location_29 = 268435456,
    Type_location_Location_30 = 536870912,
    Type_location_Location_31 = 1073741824,
};

// Decl: 923
enum Type_location_region {
    Type_location_region_college_town = 1,
    Type_location_region_coastal = 2,
    Type_location_region_coastal_extra = 4,
    Type_location_region_city = 8,
};

// Decl: 948
enum Type_measurement {
    Type_measurement_generic = 1,
    Type_measurement_imperial_only = 2,
    Type_measurement_metric_only = 4,
};

// TODO where are these in the order?
enum Type_projectile_type {
    Type_projectile_type_Smokescreen = 1,
    Type_projectile_type_Rolling_Spikes = 2,
};

enum Type_cell_call_bucket {
    Type_cell_call_bucket_situational_call = 1,
    Type_cell_call_bucket_bucket_01 = 2,
    Type_cell_call_bucket_bucket_02 = 4,
    Type_cell_call_bucket_bucket_03 = 8,
    Type_cell_call_bucket_bucket_04 = 16,
    Type_cell_call_bucket_bucket_05 = 32,
    Type_cell_call_bucket_bucket_06 = 64,
    Type_cell_call_bucket_bucket_07 = 128,
    Type_cell_call_bucket_bucket_08 = 256,
    Type_cell_call_bucket_bucket_09 = 512,
    Type_cell_call_bucket_bucket_10 = 1024,
    Type_cell_call_bucket_bucket_11 = 2048,
    Type_cell_call_bucket_bucket_12 = 4096,
    Type_cell_call_bucket_bucket_13 = 8192,
    Type_cell_call_bucket_bucket_14 = 16384,
    Type_cell_call_bucket_bucket_15 = 32768,
    Type_cell_call_bucket_bucket_16 = 65536,
};

enum Type_cell_call_number {
    Type_cell_call_number_call_01 = 1,
    Type_cell_call_number_call_02 = 2,
    Type_cell_call_number_call_03 = 4,
    Type_cell_call_number_call_04 = 8,
    Type_cell_call_number_call_05 = 16,
    Type_cell_call_number_call_06 = 32,
    Type_cell_call_number_call_07 = 64,
    Type_cell_call_number_call_08 = 128,
    Type_cell_call_number_call_09 = 256,
    Type_cell_call_number_call_10 = 512,
};

enum Type_on_off_scene {
    Type_on_off_scene_on_scene = 1,
    Type_on_off_scene_off_scene = 2,
};

extern InterfaceId AcknowledgeId;
extern FunctionHandle gAcknowledgeHandle;

// total size: 0xC
// Decl: 2081
typedef struct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
    Type_yes_no yes_no;       // offset 0x8, size 0x4
} AcknowledgeStruct;

extern InterfaceId Setup_SpotterId;
extern FunctionHandle gSetup_SpotterHandle;

// total size: 0x8
typedef struct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
} Setup_SpotterStruct;

extern InterfaceId Setup_SpotterWantedId;
extern FunctionHandle gSetup_SpotterWantedHandle;

// total size: 0x4
typedef struct {
    int speaker_id; // offset 0x0, size 0x4
} Setup_SpotterWantedStruct;

extern InterfaceId Setup_SpotterReplyId;
extern FunctionHandle gSetup_SpotterReplyHandle;

// total size: 0x8
typedef struct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
} Setup_SpotterReplyStruct;

extern InterfaceId Setup_AttmptVehStpId;
extern FunctionHandle gSetup_AttmptVehStpHandle;

// total size: 0x14
typedef struct {
    int speaker_id;                                 // offset 0x0, size 0x4
    Type_pursuit_type pursuit_type;                 // offset 0x4, size 0x4
    Type_num_suspects num_suspects;                 // offset 0x8, size 0x4
    Type_speaker_battalion speaker_battalion;       // offset 0xC, size 0x4
    Type_speaker_call_sign_id speaker_call_sign_id; // offset 0x10, size 0x4
} Setup_AttmptVehStpStruct;

extern InterfaceId Setup_DispGoAheadId;
extern FunctionHandle gSetup_DispGoAheadHandle;

// total size: 0x4
typedef struct {
    int speaker_id; // offset 0x0, size 0x4
} Setup_DispGoAheadStruct;

extern InterfaceId Setup_PrimaryEngageId;
extern FunctionHandle gSetup_PrimaryEngageHandle;

// total size: 0xC
typedef struct {
    int speaker_id;                                 // offset 0x0, size 0x4
    Type_speaker_battalion speaker_battalion;       // offset 0x4, size 0x4
    Type_speaker_call_sign_id speaker_call_sign_id; // offset 0x8, size 0x4
} Setup_PrimaryEngageStruct;

extern InterfaceId Setup_InitPursuitId;
extern FunctionHandle gSetup_InitPursuitHandle;

// total size: 0x8
struct Setup_InitPursuitStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

extern InterfaceId Setup_SuspectConfirmedId;
extern FunctionHandle gSetup_SuspectConfirmedHandle;

// total size: 0x8
struct Setup_SuspectConfirmedStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

extern InterfaceId Setup_ReInitPursuitId;
extern FunctionHandle gSetup_ReInitPursuitHandle;

// total size: 0xC
struct Setup_ReInitPursuitStruct {
    int speaker_id;                       // offset 0x0, size 0x4
    Type_time_since_lost time_since_lost; // offset 0x4, size 0x4
    Type_num_suspects num_suspects;       // offset 0x8, size 0x4
};

extern InterfaceId Setup_VehicleReportId;
extern FunctionHandle gSetup_VehicleReportHandle;

// total size: 0x14
struct Setup_VehicleReportStruct {
    int speaker_id;               // offset 0x0, size 0x4
    Type_car_color car_color;     // offset 0x4, size 0x4
    Type_car_type car_type;       // offset 0x8, size 0x4
    Type_speed speed;             // offset 0xC, size 0x4
    Type_measurement measurement; // offset 0x10, size 0x4
};

extern InterfaceId Setup_VehicleReportTagId;
extern FunctionHandle gSetup_VehicleReportTagHandle;

// total size: 0xC
struct Setup_VehicleReportTagStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_car_color car_color; // offset 0x4, size 0x4
    Type_car_type car_type;   // offset 0x8, size 0x4
};

extern InterfaceId Setup_DispVehDescripId;
extern FunctionHandle gSetup_DispVehDescripHandle;

// total size: 0xC
struct Setup_DispVehDescripStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_car_color car_color; // offset 0x4, size 0x4
    Type_car_type car_type;   // offset 0x8, size 0x4
};

extern InterfaceId Setup_DispVehDescripVinylsId;
extern FunctionHandle gSetup_DispVehDescripVinylsHandle;

// total size: 0xC
struct Setup_DispVehDescripVinylsStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_car_color car_color; // offset 0x4, size 0x4
    Type_car_type car_type;   // offset 0x8, size 0x4
};

extern InterfaceId Setup_DispNoVehDescripId;
extern FunctionHandle gSetup_DispNoVehDescripHandle;

// total size: 0x4
struct Setup_DispNoVehDescripStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Setup_DispCustPaintId;
extern FunctionHandle gSetup_DispCustPaintHandle;

// total size: 0x8
struct Setup_DispCustPaintStruct {
    int speaker_id;         // offset 0x0, size 0x4
    Type_car_type car_type; // offset 0x4, size 0x4
};

extern InterfaceId Setup_MoreDetailsId;
extern FunctionHandle gSetup_MoreDetailsHandle;

// total size: 0x4
struct Setup_MoreDetailsStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Setup_LocationReportId;
extern FunctionHandle gSetup_LocationReportHandle;

// total size: 0x18
struct Setup_LocationReportStruct {
    int speaker_id;                       // offset 0x0, size 0x4
    Type_num_suspects num_suspects;       // offset 0x4, size 0x4
    Type_direction direction;             // offset 0x8, size 0x4
    Type_encounter encounter;             // offset 0xC, size 0x4
    Type_location_region location_region; // offset 0x10, size 0x4
    Type_location location;               // offset 0x14, size 0x4
};

extern InterfaceId Setup_BullhornPrefixId;
extern FunctionHandle gSetup_BullhornPrefixHandle;

// total size: 0x4
struct Setup_BullhornPrefixStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Setup_BullhornId;
extern FunctionHandle gSetup_BullhornHandle;

// total size: 0x4
struct Setup_BullhornStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Setup_SelfStrategyId;
extern FunctionHandle gSetup_SelfStrategyHandle;

// total size: 0xC
struct Setup_SelfStrategyStruct {
    int speaker_id;                             // offset 0x0, size 0x4
    Type_code code;                             // offset 0x4, size 0x4
    Type_self_strategy_type self_strategy_type; // offset 0x8, size 0x4
};

extern InterfaceId Setup_InitialCallForBUId;
extern FunctionHandle gSetup_InitialCallForBUHandle;

// total size: 0x8
struct Setup_InitialCallForBUStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
};

extern InterfaceId Setup_InitialCallForBU_MSId;
extern FunctionHandle gSetup_InitialCallForBU_MSHandle;

// total size: 0x4
struct Setup_InitialCallForBU_MSStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Backup_CallForBUId;
extern FunctionHandle gBackup_CallForBUHandle;

// total size: 0xC
struct Backup_CallForBUStruct {
    int speaker_id;               // offset 0x0, size 0x4
    Type_code code;               // offset 0x4, size 0x4
    Type_backup_type backup_type; // offset 0x8, size 0x4
};

extern InterfaceId Backup_UnitBUReplyId;
extern FunctionHandle gBackup_UnitBUReplyHandle;

// total size: 0x4
struct Backup_UnitBUReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Backup_DispBackupReplyId;
extern FunctionHandle gBackup_DispBackupReplyHandle;

// total size: 0x18
struct Backup_DispBackupReplyStruct {
    int speaker_id;                                 // offset 0x0, size 0x4
    Type_yes_no yes_no;                             // offset 0x4, size 0x4
    Type_subject_battalion subject_battalion;       // offset 0x8, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0xC, size 0x4
    Type_code code;                                 // offset 0x10, size 0x4
    Type_disp_backup_type disp_backup_type;         // offset 0x14, size 0x4
};

extern InterfaceId Backup_CallForSwarmingId;
extern FunctionHandle gBackup_CallForSwarmingHandle;

// total size: 0x4
struct Backup_CallForSwarmingStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Backup_DispBUETAId;
extern FunctionHandle gBackup_DispBUETAHandle;

// total size: 0x8
struct Backup_DispBUETAStruct {
    int speaker_id;                       // offset 0x0, size 0x4
    Type_disp_backup_eta disp_backup_eta; // offset 0x4, size 0x4
};

extern InterfaceId Backup_DispHeliBUETAId;
extern FunctionHandle gBackup_DispHeliBUETAHandle;

// total size: 0x8
struct Backup_DispHeliBUETAStruct {
    int speaker_id;                       // offset 0x0, size 0x4
    Type_disp_backup_eta disp_backup_eta; // offset 0x4, size 0x4
};

extern InterfaceId Backup_BUReminderId;
extern FunctionHandle gBackup_BUReminderHandle;

// total size: 0xC
struct Backup_BUReminderStruct {
    int speaker_id;               // offset 0x0, size 0x4
    Type_code code;               // offset 0x4, size 0x4
    Type_backup_type backup_type; // offset 0x8, size 0x4
};

extern InterfaceId Backup_NegativeBUReplyId;
extern FunctionHandle gBackup_NegativeBUReplyHandle;

// total size: 0x10
struct Backup_NegativeBUReplyStruct {
    int speaker_id;                                 // offset 0x0, size 0x4
    Type_yes_no yes_no;                             // offset 0x4, size 0x4
    Type_subject_battalion subject_battalion;       // offset 0x8, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0xC, size 0x4
};

extern InterfaceId Backup_DispBackupUpdateId;
extern FunctionHandle gBackup_DispBackupUpdateHandle;

// total size: 0x8
struct Backup_DispBackupUpdateStruct {
    int speaker_id;     // offset 0x0, size 0x4
    Type_yes_no yes_no; // offset 0x4, size 0x4
};

extern InterfaceId Backup_BUArrivesId;
extern FunctionHandle gBackup_BUArrivesHandle;

// total size: 0xC
struct Backup_BUArrivesStruct {
    int speaker_id;                                 // offset 0x0, size 0x4
    Type_speaker_battalion speaker_battalion;       // offset 0x4, size 0x4
    Type_speaker_call_sign_id speaker_call_sign_id; // offset 0x8, size 0x4
};

extern InterfaceId StaticRoadblock_CallForRBId;
extern FunctionHandle gStaticRoadblock_CallForRBHandle;

// total size: 0xC
struct StaticRoadblock_CallForRBStruct {
    int speaker_id;                     // offset 0x0, size 0x4
    Type_code code;                     // offset 0x4, size 0x4
    Type_roadblock_type roadblock_type; // offset 0x8, size 0x4
};

extern InterfaceId StaticRoadblock_RBReminderId;
extern FunctionHandle gStaticRoadblock_RBReminderHandle;

// total size: 0x8
struct StaticRoadblock_RBReminderStruct {
    int speaker_id; // offset 0x0, size 0x4
    Type_code code; // offset 0x4, size 0x4
};

extern InterfaceId StaticRoadblock_NegativeRBReplyId;
extern FunctionHandle gStaticRoadblock_NegativeRBReplyHandle;

// total size: 0x4
struct StaticRoadblock_NegativeRBReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId StaticRoadblock_DispRBReplyId;
extern FunctionHandle gStaticRoadblock_DispRBReplyHandle;

// total size: 0x18
struct StaticRoadblock_DispRBReplyStruct {
    int speaker_id;                                 // offset 0x0, size 0x4
    Type_code code;                                 // offset 0x4, size 0x4
    Type_roadblock_type roadblock_type;             // offset 0x8, size 0x4
    Type_yes_no yes_no;                             // offset 0xC, size 0x4
    Type_subject_battalion subject_battalion;       // offset 0x10, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0x14, size 0x4
};

extern InterfaceId StaticRoadblock_DispRBUpdateId;
extern FunctionHandle gStaticRoadblock_DispRBUpdateHandle;

// total size: 0x10
struct StaticRoadblock_DispRBUpdateStruct {
    int speaker_id;                     // offset 0x0, size 0x4
    Type_code code;                     // offset 0x4, size 0x4
    Type_yes_no yes_no;                 // offset 0x8, size 0x4
    Type_roadblock_type roadblock_type; // offset 0xC, size 0x4
};

extern InterfaceId StaticRoadblock_PursuitApproachingId;
extern FunctionHandle gStaticRoadblock_PursuitApproachingHandle;

// total size: 0x8
struct StaticRoadblock_PursuitApproachingStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

extern InterfaceId StaticRoadblock_RBApproachId;
extern FunctionHandle gStaticRoadblock_RBApproachHandle;

// total size: 0x8
struct StaticRoadblock_RBApproachStruct {
    int speaker_id;                     // offset 0x0, size 0x4
    Type_roadblock_type roadblock_type; // offset 0x4, size 0x4
};

extern InterfaceId StaticRoadblock_RBEngageId;
extern FunctionHandle gStaticRoadblock_RBEngageHandle;

// total size: 0x8
struct StaticRoadblock_RBEngageStruct {
    int speaker_id;                                   // offset 0x0, size 0x4
    Type_roadblock_engage_type roadblock_engage_type; // offset 0x4, size 0x4
};

extern InterfaceId StaticRoadblock_RBAvertedId;
extern FunctionHandle gStaticRoadblock_RBAvertedHandle;

// total size: 0x4
struct StaticRoadblock_RBAvertedStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId StaticRoadblock_CallForRB_subId;
extern FunctionHandle gStaticRoadblock_CallForRB_subHandle;

// total size: 0x4
struct StaticRoadblock_CallForRB_subStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId StaticRoadblock_DispSubRBId;
extern FunctionHandle gStaticRoadblock_DispSubRBHandle;

// total size: 0x4
struct StaticRoadblock_DispSubRBStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Projectile_CallForSafetyId;
extern FunctionHandle gProjectile_CallForSafetyHandle;

// total size: 0xC
struct Projectile_CallForSafetyStruct {
    int speaker_id;                       // offset 0x0, size 0x4
    Type_code code;                       // offset 0x4, size 0x4
    Type_projectile_type projectile_type; // offset 0x8, size 0x4
};

extern InterfaceId Projectile_ProjectileLaunchId;
extern FunctionHandle gProjectile_ProjectileLaunchHandle;

// total size: 0x8
struct Projectile_ProjectileLaunchStruct {
    int speaker_id;                       // offset 0x0, size 0x4
    Type_projectile_type projectile_type; // offset 0x4, size 0x4
};

extern InterfaceId Projectile_ProjectileHitId;
extern FunctionHandle gProjectile_ProjectileHitHandle;

// total size: 0x8
struct Projectile_ProjectileHitStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_num_units num_units; // offset 0x4, size 0x4
};

extern InterfaceId Projectile_ProjectileMissId;
extern FunctionHandle gProjectile_ProjectileMissHandle;

// total size: 0xC
struct Projectile_ProjectileMissStruct {
    int speaker_id;                       // offset 0x0, size 0x4
    Type_projectile_type projectile_type; // offset 0x4, size 0x4
    Type_num_units num_units;             // offset 0x8, size 0x4
};

extern InterfaceId RollingStrategy_InitStrategyId;
extern FunctionHandle gRollingStrategy_InitStrategyHandle;

// total size: 0xC
struct RollingStrategy_InitStrategyStruct {
    int speaker_id;                                   // offset 0x0, size 0x4
    Type_code code;                                   // offset 0x4, size 0x4
    Type_rolling_strategy_type rolling_strategy_type; // offset 0x8, size 0x4
};

extern InterfaceId RollingStrategy_CallToPositionId;
extern FunctionHandle gRollingStrategy_CallToPositionHandle;

// total size: 0x14
struct RollingStrategy_CallToPositionStruct {
    int speaker_id;                                 // offset 0x0, size 0x4
    Type_intensity intensity;                       // offset 0x4, size 0x4
    Type_position position;                         // offset 0x8, size 0x4
    Type_subject_battalion subject_battalion;       // offset 0xC, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0x10, size 0x4
};

extern InterfaceId RollingStrategy_CallToPositionRemId;
extern FunctionHandle gRollingStrategy_CallToPositionRemHandle;

// total size: 0x8
struct RollingStrategy_CallToPositionRemStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId RollingStrategy_StrategyExecuteId;
extern FunctionHandle gRollingStrategy_StrategyExecuteHandle;

// total size: 0x8
struct RollingStrategy_StrategyExecuteStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Outcome_AnticipateFailId;
extern FunctionHandle gOutcome_AnticipateFailHandle;

// total size: 0x4
struct Outcome_AnticipateFailStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Outcome_AnticipateSuccessId;
extern FunctionHandle gOutcome_AnticipateSuccessHandle;

// total size: 0x4
struct Outcome_AnticipateSuccessStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Outcome_OutcomeFailId;
extern FunctionHandle gOutcome_OutcomeFailHandle;

// total size: 0x8
struct Outcome_OutcomeFailStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Outcome_StrategyResetId;
extern FunctionHandle gOutcome_StrategyResetHandle;

// total size: 0xC
struct Outcome_StrategyResetStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_same_new same_new;   // offset 0x4, size 0x4
    Type_intensity intensity; // offset 0x8, size 0x4
};

extern InterfaceId Arrest_BullhornArrestId;
extern FunctionHandle gArrest_BullhornArrestHandle;

// total size: 0x8
struct Arrest_BullhornArrestStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Arrest_ArrestId;
extern FunctionHandle gArrest_ArrestHandle;

// total size: 0x8
struct Arrest_ArrestStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Arrest_DispArrestReplyId;
extern FunctionHandle gArrest_DispArrestReplyHandle;

// total size: 0x4
struct Arrest_DispArrestReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId AnytimeEvents_CollisionWorldId;
extern FunctionHandle gAnytimeEvents_CollisionWorldHandle;

// total size: 0xC
struct AnytimeEvents_CollisionWorldStruct {
    int speaker_id;                           // offset 0x0, size 0x4
    Type_world_object_type world_object_type; // offset 0x4, size 0x4
    Type_num_units num_units;                 // offset 0x8, size 0x4
};

extern InterfaceId AnytimeEvents_CollWorld_CiviId;
extern FunctionHandle gAnytimeEvents_CollWorld_CiviHandle;

// total size: 0x8
struct AnytimeEvents_CollWorld_CiviStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_CollWorld_SpinId;
extern FunctionHandle gAnytimeEvents_CollWorld_SpinHandle;

// total size: 0x8
struct AnytimeEvents_CollWorld_SpinStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_CollWorld_AirId;
extern FunctionHandle gAnytimeEvents_CollWorld_AirHandle;

// total size: 0x8
struct AnytimeEvents_CollWorld_AirStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_CollWorld_FlipId;
extern FunctionHandle gAnytimeEvents_CollWorld_FlipHandle;

// total size: 0x8
struct AnytimeEvents_CollWorld_FlipStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_DispPursuitUpdateId;
extern FunctionHandle gAnytimeEvents_DispPursuitUpdateHandle;

// total size: 0xC
struct AnytimeEvents_DispPursuitUpdateStruct {
    int speaker_id;                                 // offset 0x0, size 0x4
    Type_subject_battalion subject_battalion;       // offset 0x4, size 0x4
    Type_subject_call_sign_id subject_call_sign_id; // offset 0x8, size 0x4
};

extern InterfaceId AnytimeEvents_PursuitUpdateRepId;
extern FunctionHandle gAnytimeEvents_PursuitUpdateRepHandle;

// total size: 0x4
struct AnytimeEvents_PursuitUpdateRepStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId AnytimeEvents_Disp911ReportId;
extern FunctionHandle gAnytimeEvents_Disp911ReportHandle;

// total size: 0x20
struct AnytimeEvents_Disp911ReportStruct {
    int speaker_id;                             // offset 0x0, size 0x4
    Type_pursuit_type pursuit_type;             // offset 0x4, size 0x4
    Type_num_suspects num_suspects;             // offset 0x8, size 0x4
    Type_encounter encounter;                   // offset 0xC, size 0x4
    Type_direction direction;                   // offset 0x10, size 0x4
    Type_location location;                     // offset 0x14, size 0x4
    Type_location_region location_region;       // offset 0x18, size 0x4
    Type_address_group_type address_group_type; // offset 0x1C, size 0x4
};

extern InterfaceId AnytimeEvents_Disp911CsPntId;
extern FunctionHandle gAnytimeEvents_Disp911CsPntHandle;

// total size: 0x24
struct AnytimeEvents_Disp911CsPntStruct {
    int speaker_id;                             // offset 0x0, size 0x4
    Type_address_group_type address_group_type; // offset 0x4, size 0x4
    Type_pursuit_type pursuit_type;             // offset 0x8, size 0x4
    Type_num_suspects num_suspects;             // offset 0xC, size 0x4
    Type_encounter encounter;                   // offset 0x10, size 0x4
    Type_direction direction;                   // offset 0x14, size 0x4
    Type_location_region location_region;       // offset 0x18, size 0x4
    Type_location location;                     // offset 0x1C, size 0x4
    Type_car_type car_type;                     // offset 0x20, size 0x4
};

extern InterfaceId AnytimeEvents_Disp911NoDescripId;
extern FunctionHandle gAnytimeEvents_Disp911NoDescripHandle;

// total size: 0x24
struct AnytimeEvents_Disp911NoDescripStruct {
    int speaker_id;                             // offset 0x0, size 0x4
    Type_address_group_type address_group_type; // offset 0x4, size 0x4
    Type_pursuit_type pursuit_type;             // offset 0x8, size 0x4
    Type_num_suspects num_suspects;             // offset 0xC, size 0x4
    Type_encounter encounter;                   // offset 0x10, size 0x4
    Type_direction direction;                   // offset 0x14, size 0x4
    Type_location_region location_region;       // offset 0x18, size 0x4
    Type_location location;                     // offset 0x1C, size 0x4
    Type_car_type car_type;                     // offset 0x20, size 0x4
};

extern InterfaceId AnytimeEvents_Unit911ReplyId;
extern FunctionHandle gAnytimeEvents_Unit911ReplyHandle;

// total size: 0x4
struct AnytimeEvents_Unit911ReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId AnytimeEvents_SuspectUTurnId;
extern FunctionHandle gAnytimeEvents_SuspectUTurnHandle;

// total size: 0x8
struct AnytimeEvents_SuspectUTurnStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_SuspectOutrunId;
extern FunctionHandle gAnytimeEvents_SuspectOutrunHandle;

// total size: 0x8
struct AnytimeEvents_SuspectOutrunStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_LostVisualId;
extern FunctionHandle gAnytimeEvents_LostVisualHandle;

// total size: 0x8
struct AnytimeEvents_LostVisualStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_RegainVisualId;
extern FunctionHandle gAnytimeEvents_RegainVisualHandle;

// total size: 0x8
struct AnytimeEvents_RegainVisualStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_LostSuspectId;
extern FunctionHandle gAnytimeEvents_LostSuspectHandle;

// total size: 0x8
struct AnytimeEvents_LostSuspectStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_DispBreakAwayId;
extern FunctionHandle gAnytimeEvents_DispBreakAwayHandle;

// total size: 0x10
struct AnytimeEvents_DispBreakAwayStruct {
    int speaker_id;                       // offset 0x0, size 0x4
    Type_location_region location_region; // offset 0x4, size 0x4
    Type_location location;               // offset 0x8, size 0x4
    Type_direction direction;             // offset 0xC, size 0x4
};

extern InterfaceId AnytimeEvents_DispTimeExpiredId;
extern FunctionHandle gAnytimeEvents_DispTimeExpiredHandle;

// total size: 0x4
struct AnytimeEvents_DispTimeExpiredStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId AnytimeEvents_DispPursuitEscalationId;
extern FunctionHandle gAnytimeEvents_DispPursuitEscalationHandle;

// total size: 0x1C
struct AnytimeEvents_DispPursuitEscalationStruct {
    int speaker_id;                             // offset 0x0, size 0x4
    Type_address_group_type address_group_type; // offset 0x4, size 0x4
    Type_pursuit_type pursuit_type;             // offset 0x8, size 0x4
    Type_num_suspects num_suspects;             // offset 0xC, size 0x4
    Type_direction direction;                   // offset 0x10, size 0x4
    Type_location location;                     // offset 0x14, size 0x4
    Type_location_region location_region;       // offset 0x18, size 0x4
};

extern InterfaceId AnytimeEvents_DispPursEscGenId;
extern FunctionHandle gAnytimeEvents_DispPursEscGenHandle;

// total size: 0x8
struct AnytimeEvents_DispPursEscGenStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_UnitDisabledId;
extern FunctionHandle gAnytimeEvents_UnitDisabledHandle;

// total size: 0xC
struct AnytimeEvents_UnitDisabledStruct {
    int speaker_id;             // offset 0x0, size 0x4
    Type_intensity intensity;   // offset 0x4, size 0x4
    Type_self_other self_other; // offset 0x8, size 0x4
};

extern InterfaceId AnytimeEvents_CallForEVId;
extern FunctionHandle gAnytimeEvents_CallForEVHandle;

// total size: 0x8
struct AnytimeEvents_CallForEVStruct {
    int speaker_id;       // offset 0x0, size 0x4
    Type_ev_type ev_type; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_DispEVReplyId;
extern FunctionHandle gAnytimeEvents_DispEVReplyHandle;

// total size: 0x4
struct AnytimeEvents_DispEVReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId AnytimeEvents_IntentToRamId;
extern FunctionHandle gAnytimeEvents_IntentToRamHandle;

// total size: 0x8
struct AnytimeEvents_IntentToRamStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_BailoutId;
extern FunctionHandle gAnytimeEvents_BailoutHandle;

// total size: 0x8
struct AnytimeEvents_BailoutStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_bailout_type bailout_type; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_BailoutDenyId;
extern FunctionHandle gAnytimeEvents_BailoutDenyHandle;

// total size: 0x4
struct AnytimeEvents_BailoutDenyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId AnytimeEvents_FocusChangeId;
extern FunctionHandle gAnytimeEvents_FocusChangeHandle;

// total size: 0x8
struct AnytimeEvents_FocusChangeStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_SuspectBehaviourId;
extern FunctionHandle gAnytimeEvents_SuspectBehaviourHandle;

// total size: 0x8
struct AnytimeEvents_SuspectBehaviourStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_num_suspects num_suspects; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_DriverHistoryId;
extern FunctionHandle gAnytimeEvents_DriverHistoryHandle;

// total size: 0x8
struct AnytimeEvents_DriverHistoryStruct {
    int speaker_id;     // offset 0x0, size 0x4
    Type_region region; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_OffroadMomentId;
extern FunctionHandle gAnytimeEvents_OffroadMomentHandle;

// total size: 0xC
struct AnytimeEvents_OffroadMomentStruct {
    int speaker_id;                           // offset 0x0, size 0x4
    Type_offroad_moment_id offroad_moment_id; // offset 0x4, size 0x4
    Type_first_subsequent first_subsequent;   // offset 0x8, size 0x4
};

extern InterfaceId AnytimeEvents_SpottedId;
extern FunctionHandle gAnytimeEvents_SpottedHandle;

// total size: 0x8
struct AnytimeEvents_SpottedStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_SuspectBrakeId;
extern FunctionHandle gAnytimeEvents_SuspectBrakeHandle;

// total size: 0x4
struct AnytimeEvents_SuspectBrakeStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId AnytimeEvents_WeatherReportId;
extern FunctionHandle gAnytimeEvents_WeatherReportHandle;

// total size: 0x4
struct AnytimeEvents_WeatherReportStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId AnytimeEvents_HeatJumpId;
extern FunctionHandle gAnytimeEvents_HeatJumpHandle;

// total size: 0x8
struct AnytimeEvents_HeatJumpStruct {
    int speaker_id;             // offset 0x0, size 0x4
    Type_heat_level heat_level; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_DirectionHighId;
extern FunctionHandle gAnytimeEvents_DirectionHighHandle;

// total size: 0x8
struct AnytimeEvents_DirectionHighStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_direction direction; // offset 0x4, size 0x4
};

extern InterfaceId AnytimeEvents_DispJurisShiftId;
extern FunctionHandle gAnytimeEvents_DispJurisShiftHandle;

// total size: 0x8
struct AnytimeEvents_DispJurisShiftStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_jurisdiction jurisdiction; // offset 0x4, size 0x4
};

extern InterfaceId HeliSpecific_HeliSelfStrategyId;
extern FunctionHandle gHeliSpecific_HeliSelfStrategyHandle;

// total size: 0x8
struct HeliSpecific_HeliSelfStrategyStruct {
    int speaker_id;                                       // offset 0x0, size 0x4
    Type_heli_self_strategy_type heli_self_strategy_type; // offset 0x4, size 0x4
};

extern InterfaceId HeliSpecific_HeliLostVisualId;
extern FunctionHandle gHeliSpecific_HeliLostVisualHandle;

// total size: 0x8
struct HeliSpecific_HeliLostVisualStruct {
    int speaker_id;                         // offset 0x0, size 0x4
    Type_heli_lost_visual heli_lost_visual; // offset 0x4, size 0x4
};

extern InterfaceId HeliSpecific_HeliIntentToBailId;
extern FunctionHandle gHeliSpecific_HeliIntentToBailHandle;

// total size: 0x8
struct HeliSpecific_HeliIntentToBailStruct {
    int speaker_id;                           // offset 0x0, size 0x4
    Type_heli_bailout_type heli_bailout_type; // offset 0x4, size 0x4
};

extern InterfaceId HeliSpecific_HeliBailoutId;
extern FunctionHandle gHeliSpecific_HeliBailoutHandle;

// total size: 0x8
struct HeliSpecific_HeliBailoutStruct {
    int speaker_id;                           // offset 0x0, size 0x4
    Type_heli_bailout_type heli_bailout_type; // offset 0x4, size 0x4
};

extern InterfaceId HeliSpecific_HeliSwarmingId;
extern FunctionHandle gHeliSpecific_HeliSwarmingHandle;

// total size: 0x4
struct HeliSpecific_HeliSwarmingStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId HeliSpecific_HeliSpotterId;
extern FunctionHandle gHeliSpecific_HeliSpotterHandle;

// total size: 0x4
struct HeliSpecific_HeliSpotterStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId HeliSpecific_HeliHazardAlertId;
extern FunctionHandle gHeliSpecific_HeliHazardAlertHandle;

// total size: 0x8
struct HeliSpecific_HeliHazardAlertStruct {
    int speaker_id;                                     // offset 0x0, size 0x4
    Type_heli_hazard_alert_type heli_hazard_alert_type; // offset 0x4, size 0x4
};

extern InterfaceId HeliSpecific_HeliQuadrentId;
extern FunctionHandle gHeliSpecific_HeliQuadrentHandle;

// total size: 0x4
struct HeliSpecific_HeliQuadrentStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId HeliSpecific_HeliQuadrentMovingId;
extern FunctionHandle gHeliSpecific_HeliQuadrentMovingHandle;

// total size: 0x4
struct HeliSpecific_HeliQuadrentMovingStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId HeliSpecific_HeliBullhornArrestId;
extern FunctionHandle gHeliSpecific_HeliBullhornArrestHandle;

// total size: 0x4
struct HeliSpecific_HeliBullhornArrestStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId E3_Events_E3_SetupId;
extern FunctionHandle gE3_Events_E3_SetupHandle;

// total size: 0x1
struct E3_Events_E3_SetupStruct {};

extern InterfaceId Interrupts_InterruptId;
extern FunctionHandle gInterrupts_InterruptHandle;

// total size: 0x8
struct Interrupts_InterruptStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Interrupts_InterruptRamId;
extern FunctionHandle gInterrupts_InterruptRamHandle;

// total size: 0x4
struct Interrupts_InterruptRamStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Interrupts_InterruptRam_REId;
extern FunctionHandle gInterrupts_InterruptRam_REHandle;

// total size: 0x8
struct Interrupts_InterruptRam_REStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Interrupts_InterruptRam_HOId;
extern FunctionHandle gInterrupts_InterruptRam_HOHandle;

// total size: 0x8
struct Interrupts_InterruptRam_HOStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Interrupts_InterruptRam_SSId;
extern FunctionHandle gInterrupts_InterruptRam_SSHandle;

// total size: 0x8
struct Interrupts_InterruptRam_SSStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Interrupts_InterruptRam_TBId;
extern FunctionHandle gInterrupts_InterruptRam_TBHandle;

// total size: 0x8
struct Interrupts_InterruptRam_TBStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId Interrupts_InterruptRamHighId;
extern FunctionHandle gInterrupts_InterruptRamHighHandle;

// total size: 0x4
struct Interrupts_InterruptRamHighStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Interrupts_StaticInterruptId;
extern FunctionHandle gInterrupts_StaticInterruptHandle;

// total size: 0x1
struct Interrupts_StaticInterruptStruct {};

extern InterfaceId Interrupts_RegainVisualInterruptId;
extern FunctionHandle gInterrupts_RegainVisualInterruptHandle;

// total size: 0x8
struct Interrupts_RegainVisualInterruptStruct {
    int speaker_id;           // offset 0x0, size 0x4
    Type_intensity intensity; // offset 0x4, size 0x4
};

extern InterfaceId CellCallId;
extern FunctionHandle gCellCallHandle;

// total size: 0x8
struct CellCallStruct {
    Type_cell_call_bucket cell_call_bucket; // offset 0x0, size 0x4
    Type_cell_call_number cell_call_number; // offset 0x4, size 0x4
};

extern InterfaceId ExtraCops_SwarmingReplyId;
extern FunctionHandle gExtraCops_SwarmingReplyHandle;

// total size: 0x1
struct ExtraCops_SwarmingReplyStruct {};

extern InterfaceId ExtraCops_SuperPursuitReplyId;
extern FunctionHandle gExtraCops_SuperPursuitReplyHandle;

// total size: 0x1
struct ExtraCops_SuperPursuitReplyStruct {};

extern InterfaceId ExtraCops_SwarmingReplyFollowId;
extern FunctionHandle gExtraCops_SwarmingReplyFollowHandle;

// total size: 0x1
struct ExtraCops_SwarmingReplyFollowStruct {};

extern InterfaceId ExtraCops_QuadrentFormingId;
extern FunctionHandle gExtraCops_QuadrentFormingHandle;

// total size: 0x1
struct ExtraCops_QuadrentFormingStruct {};

extern InterfaceId ExtraCops_SuspectPossiblyGoneId;
extern FunctionHandle gExtraCops_SuspectPossiblyGoneHandle;

// total size: 0x1
struct ExtraCops_SuspectPossiblyGoneStruct {};

extern InterfaceId ExtraCops_QuadrentMovingId;
extern FunctionHandle gExtraCops_QuadrentMovingHandle;

// total size: 0x1
struct ExtraCops_QuadrentMovingStruct {};

extern InterfaceId ExtraCops_OtherLeadId;
extern FunctionHandle gExtraCops_OtherLeadHandle;

// total size: 0x1
struct ExtraCops_OtherLeadStruct {};

extern InterfaceId ExtraCops_PossibleSuspectId;
extern FunctionHandle gExtraCops_PossibleSuspectHandle;

// total size: 0x1
struct ExtraCops_PossibleSuspectStruct {};

extern InterfaceId ExtraCops_WrongSuspectId;
extern FunctionHandle gExtraCops_WrongSuspectHandle;

// total size: 0x1
struct ExtraCops_WrongSuspectStruct {};

extern InterfaceId ExtraCops_SuspectGoneId;
extern FunctionHandle gExtraCops_SuspectGoneHandle;

// total size: 0x1
struct ExtraCops_SuspectGoneStruct {};

extern InterfaceId ExtraCops_RBWarningId;
extern FunctionHandle gExtraCops_RBWarningHandle;

// total size: 0x1
struct ExtraCops_RBWarningStruct {};

extern InterfaceId ExtraCops_RBPositionId;
extern FunctionHandle gExtraCops_RBPositionHandle;

// total size: 0x4
struct ExtraCops_RBPositionStruct {
    Type_spikebelt_position spikebelt_position; // offset 0x0, size 0x4
};

extern InterfaceId ExtraCops_ExtraRBEngageId;
extern FunctionHandle gExtraCops_ExtraRBEngageHandle;

// total size: 0x4
struct ExtraCops_ExtraRBEngageStruct {
    Type_roadblock_engage_type roadblock_engage_type; // offset 0x0, size 0x4
};

extern InterfaceId ExtraCops_ExtraRBAvertedId;
extern FunctionHandle gExtraCops_ExtraRBAvertedHandle;

// total size: 0x4
struct ExtraCops_ExtraRBAvertedStruct {
    Type_roadblock_engage_type roadblock_engage_type; // offset 0x0, size 0x4
};

extern InterfaceId Cross_CrossBUReplyId;
extern FunctionHandle gCross_CrossBUReplyHandle;

// total size: 0x4
struct Cross_CrossBUReplyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Cross_CrossFailReplyId;
extern FunctionHandle gCross_CrossFailReplyHandle;

// total size: 0x8
struct Cross_CrossFailReplyStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_on_off_scene on_off_scene; // offset 0x4, size 0x4
};

extern InterfaceId Cross_CrossRBFailReplyId;
extern FunctionHandle gCross_CrossRBFailReplyHandle;

// total size: 0x8
struct Cross_CrossRBFailReplyStruct {
    int speaker_id;                 // offset 0x0, size 0x4
    Type_on_off_scene on_off_scene; // offset 0x4, size 0x4
};

extern InterfaceId Cross_CrossPursuitEscId;
extern FunctionHandle gCross_CrossPursuitEscHandle;

// total size: 0x4
struct Cross_CrossPursuitEscStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Cross_CrossSelfStrategyId;
extern FunctionHandle gCross_CrossSelfStrategyHandle;

// total size: 0x4
struct Cross_CrossSelfStrategyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Cross_CrossMultiStrategyId;
extern FunctionHandle gCross_CrossMultiStrategyHandle;

// total size: 0x4
struct Cross_CrossMultiStrategyStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId Cross_CrossBailoutDeny_subId;
extern FunctionHandle gCross_CrossBailoutDeny_subHandle;

// total size: 0x4
struct Cross_CrossBailoutDeny_subStruct {
    int speaker_id; // offset 0x0, size 0x4
};

extern InterfaceId D_DayId;
extern FunctionHandle gD_DayHandle;

// total size: 0x1
struct D_DayStruct {};

extern InterfaceId DispIntroRaceId;
extern FunctionHandle gDispIntroRaceHandle;

// total size: 0x1
struct DispIntroRaceStruct {};

inline Result CacheHandlesEvents() {
    int result = RESULT_OK;
    result = gAcknowledgeHandle.Set(&AcknowledgeId);
    result = gSetup_SpotterHandle.Set(&Setup_SpotterId);
    result = gSetup_SpotterWantedHandle.Set(&Setup_SpotterWantedId);
    result = gSetup_SpotterReplyHandle.Set(&Setup_SpotterReplyId);
    result = gSetup_AttmptVehStpHandle.Set(&Setup_AttmptVehStpId);
    result = gSetup_DispGoAheadHandle.Set(&Setup_DispGoAheadId);
    result = gSetup_PrimaryEngageHandle.Set(&Setup_PrimaryEngageId);
    result = gSetup_InitPursuitHandle.Set(&Setup_InitPursuitId);
    result = gSetup_SuspectConfirmedHandle.Set(&Setup_SuspectConfirmedId);
    result = gSetup_ReInitPursuitHandle.Set(&Setup_ReInitPursuitId);
    result = gSetup_VehicleReportHandle.Set(&Setup_VehicleReportId);
    result = gSetup_VehicleReportTagHandle.Set(&Setup_VehicleReportTagId);
    result = gSetup_DispVehDescripHandle.Set(&Setup_DispVehDescripId);
    result = gSetup_DispVehDescripVinylsHandle.Set(&Setup_DispVehDescripVinylsId);
    result = gSetup_DispNoVehDescripHandle.Set(&Setup_DispNoVehDescripId);
    result = gSetup_DispCustPaintHandle.Set(&Setup_DispCustPaintId);
    result = gSetup_MoreDetailsHandle.Set(&Setup_MoreDetailsId);
    result = gSetup_LocationReportHandle.Set(&Setup_LocationReportId);
    result = gSetup_BullhornPrefixHandle.Set(&Setup_BullhornPrefixId);
    result = gSetup_BullhornHandle.Set(&Setup_BullhornId);
    result = gSetup_SelfStrategyHandle.Set(&Setup_SelfStrategyId);
    result = gSetup_InitialCallForBUHandle.Set(&Setup_InitialCallForBUId);
    result = gSetup_InitialCallForBU_MSHandle.Set(&Setup_InitialCallForBU_MSId);
    result = gBackup_CallForBUHandle.Set(&Backup_CallForBUId);
    result = gBackup_UnitBUReplyHandle.Set(&Backup_UnitBUReplyId);
    result = gBackup_DispBackupReplyHandle.Set(&Backup_DispBackupReplyId);
    result = gBackup_CallForSwarmingHandle.Set(&Backup_CallForSwarmingId);
    result = gBackup_DispBUETAHandle.Set(&Backup_DispBUETAId);
    result = gBackup_DispHeliBUETAHandle.Set(&Backup_DispHeliBUETAId);
    result = gBackup_BUReminderHandle.Set(&Backup_BUReminderId);
    result = gBackup_NegativeBUReplyHandle.Set(&Backup_NegativeBUReplyId);
    result = gBackup_DispBackupUpdateHandle.Set(&Backup_DispBackupUpdateId);
    result = gBackup_BUArrivesHandle.Set(&Backup_BUArrivesId);
    result = gStaticRoadblock_CallForRBHandle.Set(&StaticRoadblock_CallForRBId);
    result = gStaticRoadblock_RBReminderHandle.Set(&StaticRoadblock_RBReminderId);
    result = gStaticRoadblock_NegativeRBReplyHandle.Set(&StaticRoadblock_NegativeRBReplyId);
    result = gStaticRoadblock_DispRBReplyHandle.Set(&StaticRoadblock_DispRBReplyId);
    result = gStaticRoadblock_DispRBUpdateHandle.Set(&StaticRoadblock_DispRBUpdateId);
    result = gStaticRoadblock_PursuitApproachingHandle.Set(&StaticRoadblock_PursuitApproachingId);
    result = gStaticRoadblock_RBApproachHandle.Set(&StaticRoadblock_RBApproachId);
    result = gStaticRoadblock_RBEngageHandle.Set(&StaticRoadblock_RBEngageId);
    result = gStaticRoadblock_RBAvertedHandle.Set(&StaticRoadblock_RBAvertedId);
    result = gStaticRoadblock_CallForRB_subHandle.Set(&StaticRoadblock_CallForRB_subId);
    result = gStaticRoadblock_DispSubRBHandle.Set(&StaticRoadblock_DispSubRBId);
    result = gProjectile_CallForSafetyHandle.Set(&Projectile_CallForSafetyId);
    result = gProjectile_ProjectileLaunchHandle.Set(&Projectile_ProjectileLaunchId);
    result = gProjectile_ProjectileHitHandle.Set(&Projectile_ProjectileHitId);
    result = gProjectile_ProjectileMissHandle.Set(&Projectile_ProjectileMissId);
    result = gRollingStrategy_InitStrategyHandle.Set(&RollingStrategy_InitStrategyId);
    result = gRollingStrategy_CallToPositionHandle.Set(&RollingStrategy_CallToPositionId);
    result = gRollingStrategy_CallToPositionRemHandle.Set(&RollingStrategy_CallToPositionRemId);
    result = gRollingStrategy_StrategyExecuteHandle.Set(&RollingStrategy_StrategyExecuteId);
    result = gOutcome_AnticipateFailHandle.Set(&Outcome_AnticipateFailId);
    result = gOutcome_AnticipateSuccessHandle.Set(&Outcome_AnticipateSuccessId);
    result = gOutcome_OutcomeFailHandle.Set(&Outcome_OutcomeFailId);
    result = gOutcome_StrategyResetHandle.Set(&Outcome_StrategyResetId);
    result = gArrest_BullhornArrestHandle.Set(&Arrest_BullhornArrestId);
    result = gArrest_ArrestHandle.Set(&Arrest_ArrestId);
    result = gArrest_DispArrestReplyHandle.Set(&Arrest_DispArrestReplyId);
    result = gAnytimeEvents_CollisionWorldHandle.Set(&AnytimeEvents_CollisionWorldId);
    result = gAnytimeEvents_CollWorld_CiviHandle.Set(&AnytimeEvents_CollWorld_CiviId);
    result = gAnytimeEvents_CollWorld_SpinHandle.Set(&AnytimeEvents_CollWorld_SpinId);
    result = gAnytimeEvents_CollWorld_AirHandle.Set(&AnytimeEvents_CollWorld_AirId);
    result = gAnytimeEvents_CollWorld_FlipHandle.Set(&AnytimeEvents_CollWorld_FlipId);
    result = gAnytimeEvents_DispPursuitUpdateHandle.Set(&AnytimeEvents_DispPursuitUpdateId);
    result = gAnytimeEvents_PursuitUpdateRepHandle.Set(&AnytimeEvents_PursuitUpdateRepId);
    result = gAnytimeEvents_Disp911ReportHandle.Set(&AnytimeEvents_Disp911ReportId);
    result = gAnytimeEvents_Disp911CsPntHandle.Set(&AnytimeEvents_Disp911CsPntId);
    result = gAnytimeEvents_Disp911NoDescripHandle.Set(&AnytimeEvents_Disp911NoDescripId);
    result = gAnytimeEvents_Unit911ReplyHandle.Set(&AnytimeEvents_Unit911ReplyId);
    result = gAnytimeEvents_SuspectUTurnHandle.Set(&AnytimeEvents_SuspectUTurnId);
    result = gAnytimeEvents_SuspectOutrunHandle.Set(&AnytimeEvents_SuspectOutrunId);
    result = gAnytimeEvents_LostVisualHandle.Set(&AnytimeEvents_LostVisualId);
    result = gAnytimeEvents_RegainVisualHandle.Set(&AnytimeEvents_RegainVisualId);
    result = gAnytimeEvents_LostSuspectHandle.Set(&AnytimeEvents_LostSuspectId);
    result = gAnytimeEvents_DispBreakAwayHandle.Set(&AnytimeEvents_DispBreakAwayId);
    result = gAnytimeEvents_DispTimeExpiredHandle.Set(&AnytimeEvents_DispTimeExpiredId);
    result = gAnytimeEvents_DispPursuitEscalationHandle.Set(&AnytimeEvents_DispPursuitEscalationId);
    result = gAnytimeEvents_DispPursEscGenHandle.Set(&AnytimeEvents_DispPursEscGenId);
    result = gAnytimeEvents_UnitDisabledHandle.Set(&AnytimeEvents_UnitDisabledId);
    result = gAnytimeEvents_CallForEVHandle.Set(&AnytimeEvents_CallForEVId);
    result = gAnytimeEvents_DispEVReplyHandle.Set(&AnytimeEvents_DispEVReplyId);
    result = gAnytimeEvents_IntentToRamHandle.Set(&AnytimeEvents_IntentToRamId);
    result = gAnytimeEvents_BailoutHandle.Set(&AnytimeEvents_BailoutId);
    result = gAnytimeEvents_BailoutDenyHandle.Set(&AnytimeEvents_BailoutDenyId);
    result = gAnytimeEvents_FocusChangeHandle.Set(&AnytimeEvents_FocusChangeId);
    result = gAnytimeEvents_SuspectBehaviourHandle.Set(&AnytimeEvents_SuspectBehaviourId);
    result = gAnytimeEvents_DriverHistoryHandle.Set(&AnytimeEvents_DriverHistoryId);
    result = gAnytimeEvents_OffroadMomentHandle.Set(&AnytimeEvents_OffroadMomentId);
    result = gAnytimeEvents_SpottedHandle.Set(&AnytimeEvents_SpottedId);
    result = gAnytimeEvents_SuspectBrakeHandle.Set(&AnytimeEvents_SuspectBrakeId);
    result = gAnytimeEvents_WeatherReportHandle.Set(&AnytimeEvents_WeatherReportId);
    result = gAnytimeEvents_HeatJumpHandle.Set(&AnytimeEvents_HeatJumpId);
    result = gAnytimeEvents_DirectionHighHandle.Set(&AnytimeEvents_DirectionHighId);
    result = gAnytimeEvents_DispJurisShiftHandle.Set(&AnytimeEvents_DispJurisShiftId);
    result = gHeliSpecific_HeliSelfStrategyHandle.Set(&HeliSpecific_HeliSelfStrategyId);
    result = gHeliSpecific_HeliLostVisualHandle.Set(&HeliSpecific_HeliLostVisualId);
    result = gHeliSpecific_HeliIntentToBailHandle.Set(&HeliSpecific_HeliIntentToBailId);
    result = gHeliSpecific_HeliBailoutHandle.Set(&HeliSpecific_HeliBailoutId);
    result = gHeliSpecific_HeliSwarmingHandle.Set(&HeliSpecific_HeliSwarmingId);
    result = gHeliSpecific_HeliSpotterHandle.Set(&HeliSpecific_HeliSpotterId);
    result = gHeliSpecific_HeliHazardAlertHandle.Set(&HeliSpecific_HeliHazardAlertId);
    result = gHeliSpecific_HeliQuadrentHandle.Set(&HeliSpecific_HeliQuadrentId);
    result = gHeliSpecific_HeliQuadrentMovingHandle.Set(&HeliSpecific_HeliQuadrentMovingId);
    result = gHeliSpecific_HeliBullhornArrestHandle.Set(&HeliSpecific_HeliBullhornArrestId);
    result = gE3_Events_E3_SetupHandle.Set(&E3_Events_E3_SetupId);
    result = gInterrupts_InterruptHandle.Set(&Interrupts_InterruptId);
    result = gInterrupts_InterruptRamHandle.Set(&Interrupts_InterruptRamId);
    result = gInterrupts_InterruptRam_REHandle.Set(&Interrupts_InterruptRam_REId);
    result = gInterrupts_InterruptRam_HOHandle.Set(&Interrupts_InterruptRam_HOId);
    result = gInterrupts_InterruptRam_SSHandle.Set(&Interrupts_InterruptRam_SSId);
    result = gInterrupts_InterruptRam_TBHandle.Set(&Interrupts_InterruptRam_TBId);
    result = gInterrupts_InterruptRamHighHandle.Set(&Interrupts_InterruptRamHighId);
    result = gInterrupts_StaticInterruptHandle.Set(&Interrupts_StaticInterruptId);
    result = gInterrupts_RegainVisualInterruptHandle.Set(&Interrupts_RegainVisualInterruptId);
    result = gCellCallHandle.Set(&CellCallId);
    result = gExtraCops_SwarmingReplyHandle.Set(&ExtraCops_SwarmingReplyId);
    result = gExtraCops_SuperPursuitReplyHandle.Set(&ExtraCops_SuperPursuitReplyId);
    result = gExtraCops_SwarmingReplyFollowHandle.Set(&ExtraCops_SwarmingReplyFollowId);
    result = gExtraCops_QuadrentFormingHandle.Set(&ExtraCops_QuadrentFormingId);
    result = gExtraCops_SuspectPossiblyGoneHandle.Set(&ExtraCops_SuspectPossiblyGoneId);
    result = gExtraCops_QuadrentMovingHandle.Set(&ExtraCops_QuadrentMovingId);
    result = gExtraCops_OtherLeadHandle.Set(&ExtraCops_OtherLeadId);
    result = gExtraCops_PossibleSuspectHandle.Set(&ExtraCops_PossibleSuspectId);
    result = gExtraCops_WrongSuspectHandle.Set(&ExtraCops_WrongSuspectId);
    result = gExtraCops_SuspectGoneHandle.Set(&ExtraCops_SuspectGoneId);
    result = gExtraCops_RBWarningHandle.Set(&ExtraCops_RBWarningId);
    result = gExtraCops_RBPositionHandle.Set(&ExtraCops_RBPositionId);
    result = gExtraCops_ExtraRBEngageHandle.Set(&ExtraCops_ExtraRBEngageId);
    result = gExtraCops_ExtraRBAvertedHandle.Set(&ExtraCops_ExtraRBAvertedId);
    result = gCross_CrossBUReplyHandle.Set(&Cross_CrossBUReplyId);
    result = gCross_CrossFailReplyHandle.Set(&Cross_CrossFailReplyId);
    result = gCross_CrossRBFailReplyHandle.Set(&Cross_CrossRBFailReplyId);
    result = gCross_CrossPursuitEscHandle.Set(&Cross_CrossPursuitEscId);
    result = gCross_CrossSelfStrategyHandle.Set(&Cross_CrossSelfStrategyId);
    result = gCross_CrossMultiStrategyHandle.Set(&Cross_CrossMultiStrategyId);
    result = gCross_CrossBailoutDeny_subHandle.Set(&Cross_CrossBailoutDeny_subId);
    result = gD_DayHandle.Set(&D_DayId);
    result = gDispIntroRaceHandle.Set(&DispIntroRaceId);
    return static_cast<Result>(result);
}

}; // namespace Csis

#endif
