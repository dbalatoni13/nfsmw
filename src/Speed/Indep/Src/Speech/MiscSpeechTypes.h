#ifndef SPEECH_MISCSPEECHTYPES_H
#define SPEECH_MISCSPEECHTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"

#define SPEECH_MISC_TYPES_DEFINED
namespace Csis {
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

struct CellCallStruct {
    Type_cell_call_bucket cell_call_bucket;
    Type_cell_call_number cell_call_number;
};

struct Setup_MoreDetailsStruct {
    int speaker_id;
};

struct ExtraCops_OtherLeadStruct {};
struct ExtraCops_PossibleSuspectStruct {};
struct ExtraCops_QuadrentFormingStruct {};
struct ExtraCops_QuadrentMovingStruct {};
struct ExtraCops_ExtraRBAvertedStruct {
    int roadblock_engage_type;
};
struct ExtraCops_ExtraRBEngageStruct {
    int roadblock_engage_type;
};
struct ExtraCops_RBPositionStruct {
    int spikebelt_position;
};
struct ExtraCops_RBWarningStruct {};
struct ExtraCops_SuspectPossiblyGoneStruct {};
struct ExtraCops_SwarmingReplyFollowStruct {};
struct ExtraCops_WrongSuspectStruct {};
struct D_DayStruct {};
struct DispIntroRaceStruct {};

extern InterfaceId Setup_MoreDetailsId;
extern FunctionHandle gSetup_MoreDetailsHandle;
extern InterfaceId ExtraCops_OtherLeadId;
extern FunctionHandle gExtraCops_OtherLeadHandle;
extern InterfaceId ExtraCops_PossibleSuspectId;
extern FunctionHandle gExtraCops_PossibleSuspectHandle;
extern InterfaceId ExtraCops_QuadrentFormingId;
extern FunctionHandle gExtraCops_QuadrentFormingHandle;
extern InterfaceId ExtraCops_QuadrentMovingId;
extern FunctionHandle gExtraCops_QuadrentMovingHandle;
extern InterfaceId ExtraCops_ExtraRBAvertedId;
extern FunctionHandle gExtraCops_ExtraRBAvertedHandle;
extern InterfaceId ExtraCops_ExtraRBEngageId;
extern FunctionHandle gExtraCops_ExtraRBEngageHandle;
extern InterfaceId ExtraCops_RBPositionId;
extern FunctionHandle gExtraCops_RBPositionHandle;
extern InterfaceId ExtraCops_RBWarningId;
extern FunctionHandle gExtraCops_RBWarningHandle;
extern InterfaceId ExtraCops_SuspectPossiblyGoneId;
extern FunctionHandle gExtraCops_SuspectPossiblyGoneHandle;
extern InterfaceId ExtraCops_SwarmingReplyFollowId;
extern FunctionHandle gExtraCops_SwarmingReplyFollowHandle;
extern InterfaceId ExtraCops_WrongSuspectId;
extern FunctionHandle gExtraCops_WrongSuspectHandle;
extern InterfaceId CellCallId;
extern FunctionHandle gCellCallHandle;
extern InterfaceId DispIntroRaceId;
extern FunctionHandle gDispIntroRaceHandle;
extern InterfaceId D_DayId;
extern FunctionHandle gD_DayHandle;
} // namespace Csis

#endif
