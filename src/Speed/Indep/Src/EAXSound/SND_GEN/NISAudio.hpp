#ifndef NIS_H
#define NIS_H

#include "csis/csis.h"
namespace Csis {

enum Type_NIS_Scene_Start {
    Type_NIS_Scene_Start_IntroNis01 = 1,
    Type_NIS_Scene_Start_IntroNis02 = 2,
    Type_NIS_Scene_Start_IntroNis03 = 4,
    Type_NIS_Scene_Start_IntroNis04 = 8,
    Type_NIS_Scene_Start_IntroNis05 = 16,
    Type_NIS_Scene_Start_IntroNis06 = 32,
    Type_NIS_Scene_Start_IntroNis07 = 64,
    Type_NIS_Scene_Start_IntroNis08 = 128,
    Type_NIS_Scene_Start_IntroNis09 = 256,
    Type_NIS_Scene_Start_IntroNis10 = 512,
    Type_NIS_Scene_Start_IntroNis11 = 1024,
    Type_NIS_Scene_Start_dummy = 2048,
    Type_NIS_Scene_Start_flythrough = 4096,
    Type_NIS_Scene_Start_safehouse = 8192,
    Type_NIS_Scene_Start_dday = 16384,
    Type_NIS_Scene_Start_e3 = 32768,
    Type_NIS_Scene_Start_opm = 65536,
    Type_NIS_Scene_Start_toll = 131072,
    Type_NIS_Scene_Start_generic = 262144,
    Type_NIS_Scene_Start_ddayend = 524288,
};

enum Type_NIS_Track {
    Type_NIS_Track_Track01 = 1,
    Type_NIS_Track_Track02 = 2,
    Type_NIS_Track_Track03 = 4,
    Type_NIS_Track_Track04 = 8,
    Type_NIS_Track_Track05 = 16,
    Type_NIS_Track_Track06 = 32,
    Type_NIS_Track_Track07 = 64,
    Type_NIS_Track_Track08 = 128,
    Type_NIS_Track_Track09 = 256,
    Type_NIS_Track_Track10 = 512,
    Type_NIS_Track_Track11 = 1024,
    Type_NIS_Track_Track12 = 2048,
    Type_NIS_Track_Track13 = 4096,
    Type_NIS_Track_Track00 = 8192,
    Type_NIS_Track_Enter = 16384,
    Type_NIS_Track_Exit = 32768,
    Type_NIS_Track_Run = 65536,
    Type_NIS_Track_Generic = 131072,
    Type_NIS_Track_Track14 = 262144,
    Type_NIS_Track_Track15 = 524288,
};

// Decl: 41
enum Type_NIS_Scene_End {
    Type_NIS_Scene_End_Arrest_M01 = 1,
    Type_NIS_Scene_End_Arrest_M04 = 2,
    Type_NIS_Scene_End_Arrest_M06 = 4,
    Type_NIS_Scene_End_Arrest_M14 = 8,
    Type_NIS_Scene_End_Arrest_M16 = 16,
    Type_NIS_Scene_End_Arrest_M19 = 32,
    Type_NIS_Scene_End_Arrest_M23 = 64,
    Type_NIS_Scene_End_Arrest_F02 = 128,
    Type_NIS_Scene_End_Arrest_F06 = 256,
    Type_NIS_Scene_End_Arrest_F07 = 512,
    Type_NIS_Scene_End_Arrest_F14 = 1024,
    Type_NIS_Scene_End_Arrest_F23 = 2048,
    Type_NIS_Scene_End_E3 = 4096,
    Type_NIS_Scene_End_opm = 8192,
    Type_NIS_Scene_End_Arrest_F18 = 16384,
    Type_NIS_Scene_End_Arrest_M07 = 32768,
    Type_NIS_Scene_End_generic = 65536,
    Type_NIS_Scene_End_end01 = 131072,
    Type_NIS_Scene_End_end02 = 262144,
    Type_NIS_Scene_End_end03 = 524288,
    Type_NIS_Scene_End_end04 = 1048576,
};

// Decl: 69
enum Type_NIS_Section {
    Type_NIS_Section_Complete = 1,
    Type_NIS_Section_Start = 2,
    Type_NIS_Section_End = 4,
};

// Decl: 96
enum Type_SoundFX_Type {
    Type_SoundFX_Type_Pursbreaker = 1,
    Type_SoundFX_Type_Crash = 2,
    Type_SoundFX_Type_Thunder = 4,
    Type_SoundFX_Type_engine = 8,
    Type_SoundFX_Type_Car = 16,
    Type_SoundFX_Type_GamePlayMoment = 32,
    Type_SoundFX_Type_jump = 64,
    Type_SoundFX_Type_game = 128,
};

// Decl: 110
enum Type_SoundFX_Param {
    Type_SoundFX_Param_distant = 1,
    Type_SoundFX_Param_sweetener = 2,
    Type_SoundFX_Param_blow = 4,
    Type_SoundFX_Param_total = 8,
    Type_SoundFX_Param_expl_gas_station = 16,
    Type_SoundFX_Param_truck_jackknife = 32,
    Type_SoundFX_Param_boatfall = 64,
    Type_SoundFX_Param_trck_log_dump = 128,
    Type_SoundFX_Param_stripmall = 256,
    Type_SoundFX_Param_scaffolding_big = 512,
    Type_SoundFX_Param_drive_in = 1024,
    Type_SoundFX_Param_water_tower = 2048,
    Type_SoundFX_Param_porch_collapse = 4096,
    Type_SoundFX_Param_comm_tower = 8192,
    Type_SoundFX_Param_fish_market_sign = 16384,
    Type_SoundFX_Param_patio_collapse = 32768,
    Type_SoundFX_Param_start_music = 65536,
    Type_SoundFX_Param_end_music = 131072,
    Type_SoundFX_Param_start_effects = 262144,
    Type_SoundFX_Param_end_effects = 524288,
    Type_SoundFX_Param_cam = 1048576,
    Type_SoundFX_Param_break = 2097152,
    Type_SoundFX_Param_amphitheatre = 4194304,
    Type_SoundFX_Param_gate_signage = 8388608,
    Type_SoundFX_Param_trailer_park = 16777216,
    Type_SoundFX_Param_torus_shop = 33554432,
    Type_SoundFX_Param_gazebo = 67108864,
    Type_SoundFX_Param_uves = 134217728,
    Type_SoundFX_Param_zoom = 268435456,
    Type_SoundFX_Param_camera = 536870912,
    Type_SoundFX_Param_police_tower = 1073741824,
};

enum Type_NIS_Blacklist {
    Type_NIS_Blacklist_Black01 = 1,
    Type_NIS_Blacklist_Black02 = 2,
    Type_NIS_Blacklist_Black03 = 4,
    Type_NIS_Blacklist_Black04 = 8,
    Type_NIS_Blacklist_Black05 = 16,
    Type_NIS_Blacklist_Black06 = 32,
    Type_NIS_Blacklist_Black07 = 64,
    Type_NIS_Blacklist_Black08 = 128,
    Type_NIS_Blacklist_Black09 = 256,
    Type_NIS_Blacklist_Black10 = 512,
    Type_NIS_Blacklist_Black11 = 1024,
    Type_NIS_Blacklist_Black12 = 2048,
    Type_NIS_Blacklist_Black13 = 4096,
    Type_NIS_Blacklist_Black14 = 8192,
    Type_NIS_Blacklist_Black15 = 16384,
    Type_NIS_Blacklist_Black16 = 32768,
    Type_NIS_Blacklist_Black06end = 65536,
    Type_NIS_Blacklist_Black07end = 131072,
    Type_NIS_Blacklist_generic = 262144,
};

extern InterfaceId NIS_Select_StartId;
extern FunctionHandle gNIS_Select_StartHandle;

// total size: 0xC
typedef struct {
    Type_NIS_Scene_Start NIS_Scene_Start; // offset 0x0, size 0x4
    Type_NIS_Track NIS_Track;             // offset 0x4, size 0x4
    Type_NIS_Section NIS_Section;         // offset 0x8, size 0x4
} NIS_Select_StartStruct;

extern InterfaceId SoundFX_SelectId;
extern FunctionHandle gSoundFX_SelectHandle;

// total size: 0x8
// Decl: 196
typedef struct {
    Type_SoundFX_Type SoundFX_Type;   // offset 0x0, size 0x4
    Type_SoundFX_Param SoundFX_Param; // offset 0x4, size 0x4
} SoundFX_SelectStruct;

extern InterfaceId NIS_Select_BlacklistId;
extern FunctionHandle gNIS_Select_BlacklistHandle;

// total size: 0xC
typedef struct {
    Type_NIS_Track NIS_Track;         // offset 0x0, size 0x4
    Type_NIS_Section NIS_Section;     // offset 0x4, size 0x4
    Type_NIS_Blacklist NIS_Blacklist; // offset 0x8, size 0x4
} NIS_Select_BlacklistStruct;

extern InterfaceId NIS_Select_EndId;
extern FunctionHandle gNIS_Select_EndHandle;

// total size: 0xC
typedef struct {
    Type_NIS_Scene_End NIS_Scene_End; // offset 0x0, size 0x4
    Type_NIS_Track NIS_Track;         // offset 0x4, size 0x4
    Type_NIS_Section NIS_Section;     // offset 0x8, size 0x4
} NIS_Select_EndStruct;

inline Result NIS_Select_Start(Type_NIS_Scene_Start NIS_Scene_Start, Type_NIS_Track NIS_Track, Type_NIS_Section NIS_Section) {
    NIS_Select_StartStruct data;
    data.NIS_Scene_Start = NIS_Scene_Start;
    data.NIS_Track = NIS_Track;
    data.NIS_Section = NIS_Section;

    Result result = Function::Call(&gNIS_Select_StartHandle, &data);
    if (result < RESULT_OK) {
        gNIS_Select_StartHandle.Set(&NIS_Select_StartId);
        Function::Call(&gNIS_Select_StartHandle, &data);
    }

    return result;
}

inline Result SoundFX_Select(Type_SoundFX_Type SoundFX_Type, Type_SoundFX_Param SoundFX_Param) {
    SoundFX_SelectStruct data;
    data.SoundFX_Type = SoundFX_Type;
    data.SoundFX_Param = SoundFX_Param;

    Result result = Function::Call(&gSoundFX_SelectHandle, &data);
    if (result < RESULT_OK) {
        gSoundFX_SelectHandle.Set(&SoundFX_SelectId);
        Function::Call(&gSoundFX_SelectHandle, &data);
    }

    return result;
}

inline Result NIS_Select_Blacklist(Type_NIS_Track NIS_Track, Type_NIS_Section NIS_Section, Type_NIS_Blacklist NIS_Blacklist) {
    NIS_Select_BlacklistStruct data;
    data.NIS_Track = NIS_Track;
    data.NIS_Section = NIS_Section;
    data.NIS_Blacklist = NIS_Blacklist;

    Result result = Function::Call(&gNIS_Select_BlacklistHandle, &data);
    if (result < RESULT_OK) {
        gNIS_Select_BlacklistHandle.Set(&NIS_Select_BlacklistId);
        Function::Call(&gNIS_Select_BlacklistHandle, &data);
    }

    return result;
}

inline Result NIS_Select_End(Type_NIS_Scene_End NIS_Scene_End, Type_NIS_Track NIS_Track, Type_NIS_Section NIS_Section) {
    NIS_Select_EndStruct data;
    data.NIS_Scene_End = NIS_Scene_End;
    data.NIS_Track = NIS_Track;
    data.NIS_Section = NIS_Section;

    Result result = Function::Call(&gNIS_Select_EndHandle, &data);
    if (result < RESULT_OK) {
        gNIS_Select_EndHandle.Set(&NIS_Select_EndId);
        Function::Call(&gNIS_Select_EndHandle, &data);
    }

    return result;
}

}; // namespace Csis

#endif
