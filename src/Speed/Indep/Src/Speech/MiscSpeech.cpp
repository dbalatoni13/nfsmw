#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle_hash.h"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

void MiscSpeech::SMSCellCall(int SMS_ID) {
    Csis::CellCallStruct data;
    if (MapSMSToSPCHEnums(SMS_ID, data)) {
        Speech::Manager::ScheduleSpeech(data, Csis::CellCallId, Csis::gCellCallHandle, static_cast<EAXCharacter *>(nullptr));
    }
}

bool MiscSpeech::IsVehicleTypeOK() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return false;
    }

    Attrib::Gen::pvehicle pcar(ai->GetPlayerSpecs());
    bool is_commentable = true;

    if (pcar.VerbalType() == 0 || pcar.VerbalType() == Csis::Type_car_type_Pagani) {
        is_commentable = false;
    }

    while ((pcar.GetCollection() != Attrib::key_default) && is_commentable) {
        if ((pcar.GetCollection() == Attrib::Hash::pvehicle::key_traffic) || (pcar.GetCollection() == Attrib::Hash::pvehicle::key_cops) ||
            (pcar.GetCollection() == Attrib::Hash::pvehicle::key_tractors)) {
            is_commentable = false;
        }

        if (!is_commentable) {
            break;
        }

        pcar.Change(pcar.GetParentCollection());
    }

    return is_commentable;
}

// STRIPPED
bool MiscSpeech::IsSMSValid(int SMS_ID) {}

// TODO magic
bool MiscSpeech::MapSMSToSPCHEnums(int SMS_ID, Csis::CellCallStruct &data) {
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

    data.cell_call_bucket = static_cast<Csis::Type_cell_call_bucket>(1 << bucket);
    data.cell_call_number = static_cast<Csis::Type_cell_call_number>(1 << (SMS_ID - base));
    return true;
}

// STRIPPED
void MiscSpeech::ForcedRhinoArrival(int speakerid) {}

// STRIPPED
void MiscSpeech::CrossDialog(int ID) {}

// STRIPPED
void MiscSpeech::PlayE3Specific() {}

bool MiscSpeech::GetSPAMLocation(int SPAMID, Csis::Type_offroad_moment_id &id) {
    switch (SPAMID) {
        case 0:
            id = Csis::Type_offroad_moment_id_gas_station;
            break;
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

bool MiscSpeech::GetLocation(RoadNames id, Csis::Type_location_region &region, Csis::Type_location &location) {
    switch (id) {
        case on_Highway99:
            region = Csis::Type_location_region_college_town;
            location = static_cast<Csis::Type_location>(static_cast<int>(region));
            break;
        case thru_Campus_Interchange:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_2;
            break;
        case on_Kilgore_Bridge:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_3;
            break;
        case thru_Camden_Interchange:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_4;
            break;
        case thru_Petersburg_Interchange:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_5;
            break;
        case thru_Stadium_Interchange:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_6;
            break;
        case past_Ironwood_Estates:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_7;
            break;
        case past_Hillcrest:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_8;
            break;
        case on_Hillcrest_Drive:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_9;
            break;
        case on_Union_Row:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_10;
            break;
        case past_Rosewood_Park:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_11;
            break;
        case on_Clubhouse_Road:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_12;
            break;
        case on_Campus_Circle:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_13;
            break;
        case on_Campus_Way:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_14;
            break;
        case on_Chancellor_Way:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_15;
            break;
        case past_Rosewood_College_Hospital:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_16;
            break;
        case past_Rosewood_College_Research_Center:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_17;
            break;
        case past_Diamond_Park:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_18;
            break;
        case on_State_Street:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_19;
            break;
        case on_Hollis_Blvd:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_20;
            break;
        case on_Rockridge_Drive:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_21;
            break;
        case past_Heritage_Heights:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_22;
            break;
        case past_Highlander_Stadium:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_23;
            break;
        case past_Hickley_Field:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_24;
            break;
        case on_Stadium_Blvd:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_25;
            break;
        case past_Forest_Green_Country_Club:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_26;
            break;
        case past_Rosewood_Bus_Station:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_27;
            break;
        case on_Boundary_Road:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_28;
            break;
        case past_Boundary_Mall:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_29;
            break;
        case on_Riverside_Drive:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_30;
            break;
        case on_Bristol_Road:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_1;
            break;
        case on_North_Bay_Road:
            region = Csis::Type_location_region_coastal;
            location = static_cast<Csis::Type_location>(static_cast<int>(region));
            break;
        case on_Cannery_Way:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_3;
            break;
        case through_Dunwich_Village:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_4;
            break;
        case on_Fisher_Road:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_5;
            break;
        case through_North_Bay:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_6;
            break;
        case past_North_Bay_Cannery:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_7;
            break;
        case over_Seagate_Bridge:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_8;
            break;
        case on_Route_55:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_9;
            break;
        case on_Chase_Road:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_10;
            break;
        case past_Asylum:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_11;
            break;
        case past_Horn:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_12;
            break;
        case past_Camden_Beach:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_13;
            break;
        case on_Coast_Road:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_14;
            break;
        case past_Ocean_Hills:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_15;
            break;
        case on_Ocean_Hills_Drive:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_16;
            break;
        case thru_Boardwalk:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_17;
            break;
        case over_Seaside_bridge:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_18;
            break;
        case on_Seaside_Highway:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_19;
            break;
        case past_Beacon_Point:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_20;
            break;
        case past_Beacon_Point_Marina:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_21;
            break;
        case on_Harbour_Road:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_22;
            break;
        case past_Shipyard:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_23;
            break;
        case thru_Camden_Tunnel:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_24;
            break;
        case over_Bay_Bridge:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_25;
            break;
        case on_Camden_Tunnel_Road:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_26;
            break;
        case on_Bayshore_Blvd:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_27;
            break;
        case over_Terminal_Bridge:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_28;
            break;
        case past_Omega_Power_Station:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_29;
            break;
        case past_Penitentiary:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_30;
            break;
        case by_Smugglers_Wharf:
            region = Csis::Type_location_region_coastal;
            location = Csis::Type_location_Location_31;
            break;
        case past_Omega_Industries:
            region = Csis::Type_location_region_coastal_extra;
            location = static_cast<Csis::Type_location>(static_cast<int>(region));
            break;
        case past_Point_Camden_trainyard:
            region = Csis::Type_location_region_coastal_extra;
            location = Csis::Type_location_Location_1;
            break;
        case on_Highway_201:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_1;
            break;
        case thru_Valley_Interchange:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_3;
            break;
        case thru_Lyons_Tunnel:
            region = Csis::Type_location_region_city;
            location = static_cast<Csis::Type_location>(static_cast<int>(region));
            break;
        case over_Ironhorse_Bridge:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_5;
            break;
        case on_Hastings_Road:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_6;
            break;
        case on_Warrant_Road:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_7;
            break;
        case on_Lennox_Road:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_8;
            break;
        case near_Grand_Terrace:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_9;
            break;
        case past_Projects:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_10;
            break;
        case on_Bond_Blvd:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_11;
            break;
        case past_City_Park:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_12;
            break;
        case past_Fairmount_Bowl:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_13;
            break;
        case past_Riverfront_Stadium:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_14;
            break;
        case through_Little_Italy:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_15;
            break;
        case thru_Financial_District:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_16;
            break;
        case through_Downtown:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_17;
            break;
        case through_Century_Square:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_18;
            break;
        case Seaside_Interchange:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_19;
            break;
        case on_I17:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_20;
            break;
        case thru_Student_Housing:
            region = Csis::Type_location_region_college_town;
            location = Csis::Type_location_Location_31;
            break;
        case over_Beacon_Bridge:
            region = Csis::Type_location_region_coastal_extra;
            location = Csis::Type_location_Location_4;
            break;
        case past_Cascade_Park:
            region = Csis::Type_location_region_city;
            location = Csis::Type_location_Location_21;
            break;
        default:
            return false;
    }

    return true;
}

int MiscSpeech::Bailout(int spkrID) {
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr) {
        Csis::AnytimeEvents_BailoutStruct data;
        if (spkrID > 0) {
            data.speaker_id = spkrID;
        } else {
            data.speaker_id = static_cast<int>(bRandom(6.0f) + 3.0f);
        }
        data.bailout_type = Csis::Type_bailout_type_Generic_high_intensity;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, nullptr);
        return data.speaker_id;
    }
    return 0;
}

int MiscSpeech::LostSuspect(int spkrID) {
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr) {
        Csis::AnytimeEvents_LostSuspectStruct data;
        if (spkrID > 0) {
            data.speaker_id = spkrID;
        } else {
            data.speaker_id = static_cast<int>(bRandom(6.0f) + 3.0f);
        }
        data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_LostSuspectId, Csis::gAnytimeEvents_LostSuspectHandle, nullptr);
        return data.speaker_id;
    }
    return 0;
}

int MiscSpeech::Unit911Reply(int spkrID) {
    Csis::AnytimeEvents_Unit911ReplyStruct data;
    if (spkrID > 0) {
        data.speaker_id = spkrID;
    } else {
        data.speaker_id = static_cast<int>(bRandom(6.0f) + 3.0f);
    }
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_Unit911ReplyId, Csis::gAnytimeEvents_Unit911ReplyHandle, nullptr);
    return data.speaker_id;
}

int MiscSpeech::MoreDetails(int spkrID) {
    Csis::Setup_MoreDetailsStruct data;
    if (spkrID > 0) {
        data.speaker_id = spkrID;
    } else {
        data.speaker_id = static_cast<int>(bRandom(4.0f) + 3.0f);
    }
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_MoreDetailsId, Csis::gSetup_MoreDetailsHandle, nullptr);
    return data.speaker_id;
}

void MiscSpeech::RBWarning() {
    Csis::ExtraCops_RBWarningStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_RBWarningId, Csis::gExtraCops_RBWarningHandle, nullptr);
}

void MiscSpeech::RBPosition(int pos) {
    Csis::ExtraCops_RBPositionStruct data;
    data.spikebelt_position = pos <= -4 ? Csis::Type_spikebelt_position_left
                              : pos > 4 ? Csis::Type_spikebelt_position_right
                                        : Csis::Type_spikebelt_position_center;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_RBPositionId, Csis::gExtraCops_RBPositionHandle, nullptr);
}

void MiscSpeech::RBEngaged(bool spikes_hit) {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    IRoadBlock *block = ai->GetRoadblock();
    if (block == nullptr) {
        return;
    }

    Csis::ExtraCops_ExtraRBEngageStruct data;
    data.roadblock_engage_type = spikes_hit ? Csis::Type_roadblock_engage_type_spikes : Csis::Type_roadblock_engage_type_roadblock;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_ExtraRBEngageId, Csis::gExtraCops_ExtraRBEngageHandle, nullptr);
}

void MiscSpeech::RBAverted() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    IRoadBlock *block = ai->GetRoadblock();
    if (block == nullptr) {
        return;
    }

    Csis::ExtraCops_ExtraRBAvertedStruct data;
    data.roadblock_engage_type =
        block->GetNumSpikeStrips() > 0 ? Csis::Type_roadblock_engage_type_spikes : Csis::Type_roadblock_engage_type_roadblock;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_ExtraRBAvertedId, Csis::gExtraCops_ExtraRBAvertedHandle, nullptr);
}

void MiscSpeech::SwarmingReply() {
    Csis::ExtraCops_SwarmingReplyStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_SwarmingReplyId, Csis::gExtraCops_SwarmingReplyHandle, nullptr);
}

void MiscSpeech::SwarmingReplyFollow() {
    Csis::ExtraCops_SwarmingReplyFollowStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_SwarmingReplyFollowId, Csis::gExtraCops_SwarmingReplyFollowHandle, nullptr);
}

void MiscSpeech::QuadrantForming() {
    Csis::ExtraCops_QuadrentFormingStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_QuadrentFormingId, Csis::gExtraCops_QuadrentFormingHandle, nullptr);
}

void MiscSpeech::SuspectPossiblyGone() {
    Csis::ExtraCops_SuspectPossiblyGoneStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_SuspectPossiblyGoneId, Csis::gExtraCops_SuspectPossiblyGoneHandle, nullptr);
}

void MiscSpeech::QuadrantMoving() {
    Csis::ExtraCops_QuadrentMovingStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_QuadrentMovingId, Csis::gExtraCops_QuadrentMovingHandle, nullptr);
}

void MiscSpeech::OtherLead() {
    Csis::ExtraCops_OtherLeadStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_OtherLeadId, Csis::gExtraCops_OtherLeadHandle, nullptr);
}

void MiscSpeech::PossibleSuspect() {
    Csis::ExtraCops_PossibleSuspectStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_PossibleSuspectId, Csis::gExtraCops_PossibleSuspectHandle, nullptr);
}

void MiscSpeech::WrongSuspect() {
    Csis::ExtraCops_WrongSuspectStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_WrongSuspectId, Csis::gExtraCops_WrongSuspectHandle, nullptr);
}

// STRIPPED
void MiscSpeech::SuspectGone() {}

void MiscSpeech::D_Day() {
    Csis::D_DayStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::D_DayId, Csis::gD_DayHandle, nullptr);
}

void MiscSpeech::DispIntroRace() {
    Csis::DispIntroRaceStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::DispIntroRaceId, Csis::gDispIntroRaceHandle, nullptr);
}
