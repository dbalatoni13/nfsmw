#ifndef Events_H
#define Events_H

namespace Csis {

enum Type_code {
    Type_code_Use_10_code = 1,
    Type_code_dont_use_10_code = 2,
};

enum Type_backup_type {
    Type_backup_type_all_available_units = 1,
    Type_backup_type_Air_support = 2,
    Type_backup_type_Rhino_Unit = 4,
    Type_backup_type_generic_more_backup = 8,
};

enum Type_bailout_type {
    Type_bailout_type_Generic_low_intensity = 1,
    Type_bailout_type_Heavy_traffic = 2,
    Type_bailout_type_Road_conditions = 4,
    Type_bailout_type_Damage_Sustained = 8,
    Type_bailout_type_Generic_high_intensity = 16,
};

enum Type_disp_backup_type {
    Type_disp_backup_type_No_available_units = 1,
    Type_disp_backup_type_1_unit = 2,
    Type_disp_backup_type_all_available_units = 4,
    Type_disp_backup_type_Air_Support = 8,
    Type_disp_backup_type_Rhino_Unit = 16,
    Type_disp_backup_type_Generic_ground_units = 32,
    Type_disp_backup_type_Super_Pursuit = 64,
};

enum Type_encounter {
    Type_encounter_first_encounter = 1,
    Type_encounter_subsequent_encounter = 2,
};

enum Type_ev_type {
    Type_ev_type_multiple_units_down = 1,
    Type_ev_type_gas_station_fire = 2,
    Type_ev_type_heli_down = 4,
};

enum Type_heli_lost_visual {
    Type_heli_lost_visual_In_tunnel = 1,
    Type_heli_lost_visual_behind_building = 2,
    Type_heli_lost_visual_Under_trees = 4,
    Type_heli_lost_visual_Generic = 8,
    Type_heli_lost_visual_Restricted_Airspace = 16,
};

enum Type_intensity {
    Type_intensity_Normal = 1,
    Type_intensity_High = 2,
};

enum Type_num_suspects {
    Type_num_suspects_one_suspect = 1,
    Type_num_suspects_multiple_suspects = 2,
};

enum Type_num_units {
    Type_num_units_one_unit_in_pursuit = 1,
    Type_num_units_multiple_units_in_pursuit = 2,
};

enum Type_position {
    Type_position_Right_Side = 1,
    Type_position_Left_Side = 2,
    Type_position_Ahead = 4,
    Type_position_Behind = 8,
};

enum Type_pursuit_type {
    Type_pursuit_type_Generic_Speeder = 1,
    Type_pursuit_type_Possible_Wanted = 2,
    Type_pursuit_type_Hit_and_Run = 4,
    Type_pursuit_type_Reckless = 8,
    Type_pursuit_type_Unit_Rammed = 16,
};

enum Type_roadblock_type {
    Type_roadblock_type_Roadblock_Generic_ = 1,
    Type_roadblock_type_Spikes = 2,
    Type_roadblock_type_Heli_Roadblock_disp_only_ = 4,
    Type_roadblock_type_Multiple_Roadblocks_disp_only_ = 8,
};

enum Type_roadblock_engage_type {
    Type_roadblock_engage_type_roadblock = 1,
    Type_roadblock_engage_type_spikes = 2,
};

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

enum Type_same_new {
    Type_same_new_same_strategy = 1,
    Type_same_new_new_strategy = 2,
};

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

enum Type_world_object_type {
    Type_world_object_type_generic = 1,
    Type_world_object_type_guardrail = 2,
    Type_world_object_type_train = 4,
    Type_world_object_type_semi = 8,
    Type_world_object_type_gas_station = 16,
    Type_world_object_type_spike_belt = 32,
    Type_world_object_type_low_heat_collision = 64,
};

enum Type_yes_no {
    Type_yes_no_Yes_True = 1,
    Type_yes_no_No_False = 2,
};

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

enum Type_time_since_lost {
    Type_time_since_lost_lost_recently = 1,
    Type_time_since_lost_lost_for_X_time = 2,
};

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

enum Type_location_region {
    Type_location_region_college_town = 1,
    Type_location_region_coastal = 2,
    Type_location_region_coastal_extra = 4,
    Type_location_region_city = 8,
};

enum Type_measurement {
    Type_measurement_generic = 1,
    Type_measurement_imperial_only = 2,
    Type_measurement_metric_only = 4,
};

//

// TODO get the order of these right

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

enum Type_direction {
    Type_direction_Northbound = 1,
    Type_direction_Southbound = 2,
    Type_direction_Eastbound = 4,
    Type_direction_Westbound = 8,
};

enum Type_disp_backup_eta {
    Type_disp_backup_eta_15sec = 1,
    Type_disp_backup_eta_30sec = 2,
    Type_disp_backup_eta_1min = 4,
    Type_disp_backup_eta_1min30sec = 8,
    Type_disp_backup_eta_2min = 16,
    Type_disp_backup_eta_2min_ = 32,
};

enum Type_projectile_type {
    Type_projectile_type_Smokescreen = 1,
    Type_projectile_type_Rolling_Spikes = 2,
};

enum Type_address_group_type {
    Type_address_group_type_college_town = 1,
    Type_address_group_type_city = 2,
    Type_address_group_type_coastal = 4,
    Type_address_group_type_alpine = 8,
    Type_address_group_type_generic_any_ = 16,
};

enum Type_self_other {
    Type_self_other_context_about_self = 1,
    Type_self_other_context_about_other_unit = 2,
};

enum Type_region {
    Type_region_College_Town = 1,
    Type_region_Coastal = 2,
    Type_region_City = 4,
    Type_region_Alpine = 8,
};

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

enum Type_first_subsequent {
    Type_first_subsequent_first_time = 1,
    Type_first_subsequent_subsequent_time = 2,
};

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

enum Type_jurisdiction {
    Type_jurisdiction_state = 1,
    Type_jurisdiction_federal = 2,
};

enum Type_heli_self_strategy_type {
    Type_heli_self_strategy_type_offset_folloe = 1,
    Type_heli_self_strategy_type_heli_roadblock = 2,
    Type_heli_self_strategy_type_ram = 4,
    Type_heli_self_strategy_type_smokescreen = 8,
    Type_heli_self_strategy_type_rolling_spikes = 16,
};

enum Type_heli_bailout_type {
    Type_heli_bailout_type_flight_conditions = 1,
    Type_heli_bailout_type_low_ammo = 2,
    Type_heli_bailout_type_fuel_low = 4,
    Type_heli_bailout_type_damage_sustained = 8,
};

enum Type_heli_hazard_alert_type {
    Type_heli_hazard_alert_type_windy_roads = 1,
    Type_heli_hazard_alert_type_approaching_highway = 2,
    Type_heli_hazard_alert_type_approaching_tunnel = 4,
    Type_heli_hazard_alert_type_approaching_city = 8,
    Type_heli_hazard_alert_type_approaching_airport = 16,
    Type_heli_hazard_alert_type_approaching_blimp = 32,
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

enum Type_spikebelt_position {
    Type_spikebelt_position_left = 1,
    Type_spikebelt_position_center = 2,
    Type_spikebelt_position_right = 4,
};

enum Type_on_off_scene {
    Type_on_off_scene_on_scene = 1,
    Type_on_off_scene_off_scene = 2,
};

}; // namespace Csis

#endif
