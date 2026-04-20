#include "ScheduleSpeech.hpp"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace Csis {
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

namespace {
int ResolveSpeaker(int speaker_id) {
    if (speaker_id > 0) {
        return speaker_id;
    }
    return static_cast<int>(bRandom(3.0f) + 1.0f);
}

bool SetLocation(Csis::Type_location_region &region, Csis::Type_location &location, Csis::Type_location_region region_value, Csis::Type_location location_value) {
    region = region_value;
    location = location_value;
    return true;
}

bool SetRegionAsLocation(Csis::Type_location_region &region, Csis::Type_location &location, Csis::Type_location_region region_value) {
    region = region_value;
    location = (Csis::Type_location)region_value;
    return true;
}
} // namespace

namespace MiscSpeech {
bool MapSMSToSPCHEnums(int SMS_ID, Csis::CellCallStruct &data);

static inline bool IsVehicleTypeOK_Impl() {
    SoundAI *sound_ai = SoundAI::Get();
    if (!sound_ai) {
        return false;
    }

    Attrib::Gen::pvehicle pcar(sound_ai->GetPlayerSpecs());
    bool ok = true;

    if (pcar.VerbalType() == static_cast<Csis::Type_car_type>(0) || pcar.VerbalType() == static_cast<Csis::Type_car_type>(0x800000)) {
        ok = false;
    }

    while (ok && pcar.GetCollection() != 0xEEC2271A) {
        unsigned int collection = pcar.GetCollection();
        if (collection == 0xDF9C02AC || collection == 0xA9811B93 || collection == 0x230EB710) {
            ok = false;
            break;
        }

        const Attrib::Collection *parent = Attrib::GetCollectionParent(pcar.GetConstCollection());
        pcar.Change(parent);
    }

    return ok;
}

int LostSuspect(int spkrID) {
    SoundAI *sound_ai = SoundAI::Get();
    if (!sound_ai) {
        return 0;
    }

    Csis::AnytimeEvents_LostSuspectStruct data;
    data.speaker_id = ResolveSpeaker(spkrID);
    data.intensity = sound_ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    ScheduleSpeech(data, Csis::AnytimeEvents_LostSuspectId, Csis::gAnytimeEvents_LostSuspectHandle, static_cast<EAXCharacter *>(0));
    return data.speaker_id;
}

int Bailout(int spkrID) {
    SoundAI *sound_ai = SoundAI::Get();
    if (!sound_ai) {
        return 0;
    }

    Csis::AnytimeEvents_BailoutStruct data;
    data.speaker_id = ResolveSpeaker(spkrID);
    data.bailout_type = 8;
    ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, static_cast<EAXCharacter *>(0));
    return data.speaker_id;
}

static inline void RBAverted_Impl() {
    SoundAI *sound_ai = SoundAI::Get();
    if (!sound_ai) {
        return;
    }

    IRoadBlock *roadblock = sound_ai->GetRoadblock();
    if (!roadblock) {
        return;
    }

    Csis::ExtraCops_ExtraRBAvertedStruct data;
    data.roadblock_engage_type = 1;
    if (roadblock->GetNumSpikeStrips() > 0) {
        data.roadblock_engage_type = 2;
    }
    ScheduleSpeech(data, Csis::ExtraCops_ExtraRBAvertedId, Csis::gExtraCops_ExtraRBAvertedHandle, static_cast<EAXCharacter *>(0));
}

int Unit911Reply(int spkrID) {
    Csis::AnytimeEvents_Unit911ReplyStruct data;
    data.speaker_id = ResolveSpeaker(spkrID);
    ScheduleSpeech(data, Csis::AnytimeEvents_Unit911ReplyId, Csis::gAnytimeEvents_Unit911ReplyHandle, static_cast<EAXCharacter *>(0));
    return data.speaker_id;
}

void RBEngaged(bool spikes_hit) {
    SoundAI *sound_ai = SoundAI::Get();
    if (!sound_ai) {
        return;
    }

    IRoadBlock *roadblock = sound_ai->GetRoadblock();
    if (!roadblock) {
        return;
    }

    Csis::ExtraCops_ExtraRBEngageStruct data;
    data.roadblock_engage_type = spikes_hit ? 2 : 1;
    ScheduleSpeech(data, Csis::ExtraCops_ExtraRBEngageId, Csis::gExtraCops_ExtraRBEngageHandle, static_cast<EAXCharacter *>(0));
}

int MoreDetails(int spkrID) {
    Csis::Setup_MoreDetailsStruct data;
    data.speaker_id = ResolveSpeaker(spkrID);
    ScheduleSpeech(data, Csis::Setup_MoreDetailsId, Csis::gSetup_MoreDetailsHandle, static_cast<EAXCharacter *>(0));
    return data.speaker_id;
}

void RBPosition(int pos) {
    Csis::ExtraCops_RBPositionStruct data;
    if (pos < -3) {
        data.spikebelt_position = 1;
    } else if (pos > 4) {
        data.spikebelt_position = 4;
    } else {
        data.spikebelt_position = 2;
    }
    ScheduleSpeech(data, Csis::ExtraCops_RBPositionId, Csis::gExtraCops_RBPositionHandle, static_cast<EAXCharacter *>(0));
}

void SMSCellCall(int SMS_ID) {
    Csis::CellCallStruct data;
    if (MapSMSToSPCHEnums(SMS_ID, data)) {
        ScheduleSpeech(data, Csis::CellCallId, Csis::gCellCallHandle, static_cast<EAXCharacter *>(0));
    }
}

static inline void WrongSuspect_Impl() {
    Csis::ExtraCops_WrongSuspectStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_WrongSuspectId, Csis::gExtraCops_WrongSuspectHandle, static_cast<EAXCharacter *>(0));
}

static inline void SwarmingReplyFollow_Impl() {
    Csis::ExtraCops_SwarmingReplyFollowStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_SwarmingReplyFollowId, Csis::gExtraCops_SwarmingReplyFollowHandle, static_cast<EAXCharacter *>(0));
}

static inline void SwarmingReply_Impl() {
    Csis::ExtraCops_SwarmingReplyStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_SwarmingReplyId, Csis::gExtraCops_SwarmingReplyHandle, static_cast<EAXCharacter *>(0));
}

static inline void SuspectPossiblyGone_Impl() {
    Csis::ExtraCops_SuspectPossiblyGoneStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_SuspectPossiblyGoneId, Csis::gExtraCops_SuspectPossiblyGoneHandle, static_cast<EAXCharacter *>(0));
}

static inline void RBWarning_Impl() {
    Csis::ExtraCops_RBWarningStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_RBWarningId, Csis::gExtraCops_RBWarningHandle, static_cast<EAXCharacter *>(0));
}

static inline void QuadrantMoving_Impl() {
    Csis::ExtraCops_QuadrentMovingStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_QuadrentMovingId, Csis::gExtraCops_QuadrentMovingHandle, static_cast<EAXCharacter *>(0));
}

static inline void QuadrantForming_Impl() {
    Csis::ExtraCops_QuadrentFormingStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_QuadrentFormingId, Csis::gExtraCops_QuadrentFormingHandle, static_cast<EAXCharacter *>(0));
}

static inline void PossibleSuspect_Impl() {
    Csis::ExtraCops_PossibleSuspectStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_PossibleSuspectId, Csis::gExtraCops_PossibleSuspectHandle, static_cast<EAXCharacter *>(0));
}

static inline void OtherLead_Impl() {
    Csis::ExtraCops_OtherLeadStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_OtherLeadId, Csis::gExtraCops_OtherLeadHandle, static_cast<EAXCharacter *>(0));
}

static inline void DispIntroRace_Impl() {
    Csis::DispIntroRaceStruct data;
    ScheduleSpeech(data, Csis::DispIntroRaceId, Csis::gDispIntroRaceHandle, static_cast<EAXCharacter *>(0));
}

static inline void D_Day_Impl() {
    Csis::D_DayStruct data;
    ScheduleSpeech(data, Csis::D_DayId, Csis::gD_DayHandle, static_cast<EAXCharacter *>(0));
}

bool MapSMSToSPCHEnums(int SMS_ID, Csis::CellCallStruct &data) {
    int bucket = 0;
    int base = 0;

    if (SMS_ID - 10U < 7) {
        bucket = 0x10;
        base = 10;
    } else if (SMS_ID - 0x11U < 6) {
        bucket = 0xF;
        base = 0x11;
    } else if (SMS_ID - 0x17U < 7) {
        bucket = 0xE;
        base = 0x17;
    } else if (SMS_ID - 0x1EU < 7) {
        bucket = 0xD;
        base = 0x1E;
    } else if (SMS_ID - 0x25U < 5) {
        bucket = 0xC;
        base = 0x25;
    } else if (SMS_ID - 0x2AU < 5) {
        bucket = 0xB;
        base = 0x2A;
    } else if (SMS_ID - 0x2FU < 6) {
        bucket = 10;
        base = 0x2F;
    } else if (SMS_ID - 0x35U < 5) {
        bucket = 9;
        base = 0x35;
    } else if (SMS_ID - 0x3AU < 6) {
        bucket = 8;
        base = 0x3A;
    } else if (SMS_ID - 0x40U < 6) {
        bucket = 7;
        base = 0x40;
    } else if (SMS_ID - 0x46U < 3) {
        bucket = 6;
        base = 0x46;
    } else if (SMS_ID - 0x49U < 4) {
        bucket = 5;
        base = 0x49;
    } else if (SMS_ID - 0x4DU < 3) {
        bucket = 4;
        base = 0x4D;
    } else if (SMS_ID - 0x50U < 5) {
        bucket = 3;
        base = 0x50;
    } else if (SMS_ID - 0x55U < 4) {
        bucket = 2;
        base = 0x55;
    } else if (SMS_ID - 0x59U < 8) {
        bucket = 1;
        base = 0x59;
    } else {
        if (SMS_ID - 0x61U > 8) {
            return false;
        }
        bucket = 0;
        base = 0x61;
    }

    data.cell_call_number = static_cast<Csis::Type_cell_call_number>(1 << ((SMS_ID - base) & 0x1F));
    data.cell_call_bucket = static_cast<Csis::Type_cell_call_bucket>(1 << bucket);
    return true;
}

bool GetSPAMLocation(int SPAMID, Csis::Type_offroad_moment_id &id) {
    switch (SPAMID) {
    case 0:
        id = Csis::Type_offroad_moment_id_gas_station;
        return true;
    case 1:
        id = Csis::Type_offroad_moment_id_campus;
        break;
    case 2:
        id = Csis::Type_offroad_moment_id_golf_course;
        break;
    case 3:
        id = Csis::Type_offroad_moment_id_hospital;
        break;
    case 4:
        id = Csis::Type_offroad_moment_id_strip_mall;
        break;
    case 5:
        id = Csis::Type_offroad_moment_id_stadium;
        break;
    case 6:
        id = Csis::Type_offroad_moment_id_park;
        break;
    case 7:
        id = Csis::Type_offroad_moment_id_trailer_park;
        break;
    case 8:
        id = Csis::Type_offroad_moment_id_junkyard;
        break;
    case 9:
        id = Csis::Type_offroad_moment_id_boatyard;
        break;
    case 10:
        id = Csis::Type_offroad_moment_id_refinery;
        break;
    case 0xB:
        id = Csis::Type_offroad_moment_id_trainyard;
        break;
    case 0xC:
        id = Csis::Type_offroad_moment_id_boardwalk;
        break;
    case 0xD:
        id = Csis::Type_offroad_moment_id_beach;
        break;
    case 0xE:
        id = Csis::Type_offroad_moment_id_subway;
        break;
    case 0xF:
        id = Csis::Type_offroad_moment_id_hotel;
        break;
    case 0x10:
        id = Csis::Type_offroad_moment_id_museum;
        break;
    case 0x11:
        id = Csis::Type_offroad_moment_id_police_station;
        break;
    case 0x12:
        id = Csis::Type_offroad_moment_id_hydro_plant;
        break;
    case 0x13:
        id = Csis::Type_offroad_moment_id_construction_yard;
        break;
    case 0x14:
        id = Csis::Type_offroad_moment_id_bus_station;
        break;
    case 0x15:
        id = Csis::Type_offroad_moment_id_drive_in_theatre;
        break;
    case 0x16:
        id = Csis::Type_offroad_moment_id_penitentiary;
        break;
    case 0x17:
        id = Csis::Type_offroad_moment_id_fishery;
        break;
    default:
        return false;
    }
    return true;
}

bool GetLocation(RoadNames id, Csis::Type_location_region &region, Csis::Type_location &location) {
    switch (id) {
    case on_Highway99:
        return SetRegionAsLocation(region, location, Csis::Type_location_region_college_town);
    case thru_Campus_Interchange:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_2);
    case on_Kilgore_Bridge:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_3);
    case thru_Camden_Interchange:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_4);
    case thru_Petersburg_Interchange:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_5);
    case thru_Stadium_Interchange:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_6);
    case past_Ironwood_Estates:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_7);
    case past_Hillcrest:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_8);
    case on_Hillcrest_Drive:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_9);
    case on_Union_Row:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_10);
    case past_Rosewood_Park:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_11);
    case on_Clubhouse_Road:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_12);
    case on_Campus_Circle:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_13);
    case on_Campus_Way:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_14);
    case on_Chancellor_Way:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_15);
    case past_Rosewood_College_Hospital:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_16);
    case past_Rosewood_College_Research_Center:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_17);
    case past_Diamond_Park:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_18);
    case on_State_Street:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_19);
    case on_Hollis_Blvd:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_20);
    case on_Rockridge_Drive:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_21);
    case past_Heritage_Heights:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_22);
    case past_Highlander_Stadium:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_23);
    case past_Hickley_Field:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_24);
    case on_Stadium_Blvd:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_25);
    case past_Forest_Green_Country_Club:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_26);
    case past_Rosewood_Bus_Station:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_27);
    case on_Boundary_Road:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_28);
    case past_Boundary_Mall:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_29);
    case on_Riverside_Drive:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_30);
    case on_Bristol_Road:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_1);
    case on_North_Bay_Road:
        return SetRegionAsLocation(region, location, Csis::Type_location_region_coastal);
    case on_Cannery_Way:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_3);
    case through_Dunwich_Village:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_4);
    case on_Fisher_Road:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_5);
    case through_North_Bay:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_6);
    case past_North_Bay_Cannery:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_7);
    case over_Seagate_Bridge:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_8);
    case on_Route_55:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_9);
    case on_Chase_Road:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_10);
    case past_Asylum:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_11);
    case past_Horn:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_12);
    case past_Camden_Beach:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_13);
    case on_Coast_Road:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_14);
    case past_Ocean_Hills:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_15);
    case on_Ocean_Hills_Drive:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_16);
    case thru_Boardwalk:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_17);
    case over_Seaside_bridge:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_18);
    case on_Seaside_Highway:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_19);
    case past_Beacon_Point:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_20);
    case past_Beacon_Point_Marina:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_21);
    case on_Harbour_Road:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_22);
    case past_Shipyard:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_23);
    case thru_Camden_Tunnel:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_24);
    case over_Bay_Bridge:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_25);
    case on_Camden_Tunnel_Road:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_26);
    case on_Bayshore_Blvd:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_27);
    case over_Terminal_Bridge:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_28);
    case past_Omega_Power_Station:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_29);
    case past_Penitentiary:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_30);
    case by_Smugglers_Wharf:
        return SetLocation(region, location, Csis::Type_location_region_coastal, Csis::Type_location_Location_31);
    case past_Omega_Industries:
        return SetRegionAsLocation(region, location, Csis::Type_location_region_coastal_extra);
    case past_Point_Camden_trainyard:
        return SetLocation(region, location, Csis::Type_location_region_coastal_extra, Csis::Type_location_Location_1);
    case on_Highway_201:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_1);
    case thru_Valley_Interchange:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_3);
    case thru_Lyons_Tunnel:
        return SetRegionAsLocation(region, location, Csis::Type_location_region_city);
    case over_Ironhorse_Bridge:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_5);
    case on_Hastings_Road:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_6);
    case on_Warrant_Road:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_7);
    case on_Lennox_Road:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_8);
    case near_Grand_Terrace:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_9);
    case past_Projects:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_10);
    case on_Bond_Blvd:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_11);
    case past_City_Park:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_12);
    case past_Fairmount_Bowl:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_13);
    case past_Riverfront_Stadium:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_14);
    case through_Little_Italy:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_15);
    case thru_Financial_District:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_16);
    case through_Downtown:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_17);
    case through_Century_Square:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_18);
    case Seaside_Interchange:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_19);
    case on_I17:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_20);
    case thru_Student_Housing:
        return SetLocation(region, location, Csis::Type_location_region_college_town, Csis::Type_location_Location_31);
    case over_Beacon_Bridge:
        return SetLocation(region, location, Csis::Type_location_region_coastal_extra, Csis::Type_location_Location_4);
    case past_Cascade_Park:
        return SetLocation(region, location, Csis::Type_location_region_city, Csis::Type_location_Location_21);
    default:
        return false;
    }
}
} // namespace MiscSpeech

extern "C" bool IsVehicleTypeOK__10MiscSpeech() {
    return MiscSpeech::IsVehicleTypeOK_Impl();
}

extern "C" void RBAverted__10MiscSpeech() {
    MiscSpeech::RBAverted_Impl();
}

extern "C" void RBWarning__10MiscSpeech() {
    MiscSpeech::RBWarning_Impl();
}

extern "C" void SwarmingReply__10MiscSpeech() {
    MiscSpeech::SwarmingReply_Impl();
}

extern "C" void SwarmingReplyFollow__10MiscSpeech() {
    MiscSpeech::SwarmingReplyFollow_Impl();
}

extern "C" void QuadrantForming__10MiscSpeech() {
    MiscSpeech::QuadrantForming_Impl();
}

extern "C" void SuspectPossiblyGone__10MiscSpeech() {
    MiscSpeech::SuspectPossiblyGone_Impl();
}

extern "C" void QuadrantMoving__10MiscSpeech() {
    MiscSpeech::QuadrantMoving_Impl();
}

extern "C" void OtherLead__10MiscSpeech() {
    MiscSpeech::OtherLead_Impl();
}

extern "C" void PossibleSuspect__10MiscSpeech() {
    MiscSpeech::PossibleSuspect_Impl();
}

extern "C" void WrongSuspect__10MiscSpeech() {
    MiscSpeech::WrongSuspect_Impl();
}

extern "C" void D_Day__10MiscSpeech() {
    MiscSpeech::D_Day_Impl();
}

extern "C" void DispIntroRace__10MiscSpeech() {
    MiscSpeech::DispIntroRace_Impl();
}
