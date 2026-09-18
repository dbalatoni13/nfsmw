#include "MiscSpeech.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

#include "Speed/Indep/Src/Main/AttribSupport.h"

void MiscSpeech::SMSCellCall(int SMS_ID) {
    Csis::CellCallStruct data;
    if (MapSMSToSPCHEnums(SMS_ID, data)) {
        SCHEDULE_SPEECH(CellCall, data, nullptr);
    }
}

bool MiscSpeech::IsVehicleTypeOK() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return false;
    }
    Attrib::Gen::pvehicle vehicle(ai->GetPlayerSpecs());
    bool ok = true;
    if (vehicle.VerbalType() == 0 || vehicle.VerbalType() == Csis::Type_car_type_Pagani) {
        ok = false;
    }
    while (vehicle.GetCollection() != 0xeec2271a && ok) {
        if (vehicle.GetCollection() == 0xdf9c02ac || vehicle.GetCollection() == 0xa9811b93 || vehicle.GetCollection() == 0x230eb710) {
            ok = false;
        }
        if (!ok) {
            break;
        }
        vehicle.Change(Attrib::GetCollectionParent(vehicle.GetConstCollection()));
    }
    return ok;
}


bool MiscSpeech::MapSMSToSPCHEnums(int SMS_ID, Csis::CellCallStruct &data) {
    int bucket;
    int base;
    if (SMS_ID >= 10 && SMS_ID <= 16) {
        bucket = 16;
        base = 10;
    } else if (SMS_ID >= 17 && SMS_ID <= 22) {
        bucket = 15;
        base = 17;
    } else if (SMS_ID >= 23 && SMS_ID <= 29) {
        bucket = 14;
        base = 23;
    } else if (SMS_ID >= 30 && SMS_ID <= 36) {
        bucket = 13;
        base = 30;
    } else if (SMS_ID >= 37 && SMS_ID <= 41) {
        bucket = 12;
        base = 37;
    } else if (SMS_ID >= 42 && SMS_ID <= 46) {
        bucket = 11;
        base = 42;
    } else if (SMS_ID >= 47 && SMS_ID <= 52) {
        bucket = 10;
        base = 47;
    } else if (SMS_ID >= 53 && SMS_ID <= 57) {
        bucket = 9;
        base = 53;
    } else if (SMS_ID >= 58 && SMS_ID <= 63) {
        bucket = 8;
        base = 58;
    } else if (SMS_ID >= 64 && SMS_ID <= 69) {
        bucket = 7;
        base = 64;
    } else if (SMS_ID >= 70 && SMS_ID <= 72) {
        bucket = 6;
        base = 70;
    } else if (SMS_ID >= 73 && SMS_ID <= 76) {
        bucket = 5;
        base = 73;
    } else if (SMS_ID >= 77 && SMS_ID <= 79) {
        bucket = 4;
        base = 77;
    } else if (SMS_ID >= 80 && SMS_ID <= 84) {
        bucket = 3;
        base = 80;
    } else if (SMS_ID >= 85 && SMS_ID <= 88) {
        bucket = 2;
        base = 85;
    } else if (SMS_ID >= 89 && SMS_ID <= 96) {
        bucket = 1;
        base = 89;
    } else if (SMS_ID >= 97 && SMS_ID <= 105) {
        bucket = 0;
        base = 97;
    } else {
        return false;
    }
    data.cell_call_bucket = static_cast<Csis::Type_cell_call_bucket>(1 << bucket);
    data.cell_call_number = static_cast<Csis::Type_cell_call_number>(1 << (SMS_ID - base));
    return true;
}

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
    case 11:
        id = Csis::Type_offroad_moment_id_trainyard;
        break;
    case 12:
        id = Csis::Type_offroad_moment_id_boardwalk;
        break;
    case 13:
        id = Csis::Type_offroad_moment_id_beach;
        break;
    case 14:
        id = Csis::Type_offroad_moment_id_subway;
        break;
    case 15:
        id = Csis::Type_offroad_moment_id_hotel;
        break;
    case 16:
        id = Csis::Type_offroad_moment_id_museum;
        break;
    case 17:
        id = Csis::Type_offroad_moment_id_police_station;
        break;
    case 18:
        id = Csis::Type_offroad_moment_id_hydro_plant;
        break;
    case 19:
        id = Csis::Type_offroad_moment_id_construction_yard;
        break;
    case 20:
        id = Csis::Type_offroad_moment_id_bus_station;
        break;
    case 21:
        id = Csis::Type_offroad_moment_id_drive_in_theatre;
        break;
    case 22:
        id = Csis::Type_offroad_moment_id_penitentiary;
        break;
    case 23:
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
        location = Csis::Type_location_Location_1;
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
        location = Csis::Type_location_Location_2;
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
        location = Csis::Type_location_Location_3;
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
        location = Csis::Type_location_Location_4;
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
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::AnytimeEvents_BailoutStruct data;
        if (spkrID > 0) {
            data.speaker_id = spkrID;
        } else {
            data.speaker_id = (int) (bRandom(6.0f) + 3.0f);
        }
        data.bailout_type = static_cast<Csis::Type_bailout_type>(16);
        SCHEDULE_SPEECH(AnytimeEvents_Bailout, data, nullptr);
        return data.speaker_id;
    }
    return 0;
}

int MiscSpeech::LostSuspect(int spkrID) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::AnytimeEvents_LostSuspectStruct data;
        if (spkrID > 0) {
            data.speaker_id = spkrID;
        } else {
            data.speaker_id = (int) (bRandom(6.0f) + 3.0f);
        }
        data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
        SCHEDULE_SPEECH(AnytimeEvents_LostSuspect, data, nullptr);
        return data.speaker_id;
    }
    return 0;
}

int MiscSpeech::Unit911Reply(int spkrID) {
    Csis::AnytimeEvents_Unit911ReplyStruct data;
    if (spkrID > 0) {
        data.speaker_id = spkrID;
    } else {
        data.speaker_id = (int) (bRandom(6.0f) + 3.0f);
    }
    SCHEDULE_SPEECH(AnytimeEvents_Unit911Reply, data, nullptr);
    return data.speaker_id;
}

int MiscSpeech::MoreDetails(int spkrID) {
    Csis::Setup_MoreDetailsStruct data;
    if (spkrID > 0) {
        data.speaker_id = spkrID;
    } else {
        data.speaker_id = (int) (bRandom(4.0f) + 3.0f);
    }
    SCHEDULE_SPEECH(Setup_MoreDetails, data, nullptr);
    return data.speaker_id;
}

void MiscSpeech::RBWarning() {
    Csis::ExtraCops_RBWarningStruct data;
    SCHEDULE_SPEECH(ExtraCops_RBWarning, data, nullptr);
}

void MiscSpeech::RBPosition(int pos) {
    Csis::ExtraCops_RBPositionStruct data;
    data.spikebelt_position =
        pos <= -4 ? Csis::Type_spikebelt_position_left
                  : (pos > 4 ? Csis::Type_spikebelt_position_right : Csis::Type_spikebelt_position_center);
    SCHEDULE_SPEECH(ExtraCops_RBPosition, data, nullptr);
}

void MiscSpeech::RBEngaged(bool spikes_hit) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetRoadblock() != nullptr) {
        Csis::ExtraCops_ExtraRBEngageStruct data;
        data.roadblock_engage_type = spikes_hit ? static_cast<Csis::Type_roadblock_engage_type>(2)
                                                : static_cast<Csis::Type_roadblock_engage_type>(1);
        SCHEDULE_SPEECH(ExtraCops_ExtraRBEngage, data, nullptr);
    }
}

void MiscSpeech::RBAverted() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    IRoadBlock *roadblock = ai->GetRoadblock();
    if (roadblock != nullptr) {
        Csis::ExtraCops_ExtraRBAvertedStruct data;
        data.roadblock_engage_type =
            roadblock->GetNumSpikeStrips() > 0 ? static_cast<Csis::Type_roadblock_engage_type>(2) : static_cast<Csis::Type_roadblock_engage_type>(1);
        SCHEDULE_SPEECH(ExtraCops_ExtraRBAverted, data, nullptr);
    }
}

void MiscSpeech::SwarmingReply() {
    Csis::ExtraCops_SwarmingReplyStruct data;
    SCHEDULE_SPEECH(ExtraCops_SwarmingReply, data, nullptr);
}

void MiscSpeech::SwarmingReplyFollow() {
    Csis::ExtraCops_SwarmingReplyFollowStruct data;
    SCHEDULE_SPEECH(ExtraCops_SwarmingReplyFollow, data, nullptr);
}

void MiscSpeech::QuadrantForming() {
    Csis::ExtraCops_QuadrentFormingStruct data;
    SCHEDULE_SPEECH(ExtraCops_QuadrentForming, data, nullptr);
}

void MiscSpeech::SuspectPossiblyGone() {
    Csis::ExtraCops_SuspectPossiblyGoneStruct data;
    SCHEDULE_SPEECH(ExtraCops_SuspectPossiblyGone, data, nullptr);
}

void MiscSpeech::QuadrantMoving() {
    Csis::ExtraCops_QuadrentMovingStruct data;
    SCHEDULE_SPEECH(ExtraCops_QuadrentMoving, data, nullptr);
}

void MiscSpeech::OtherLead() {
    Csis::ExtraCops_OtherLeadStruct data;
    SCHEDULE_SPEECH(ExtraCops_OtherLead, data, nullptr);
}

void MiscSpeech::PossibleSuspect() {
    Csis::ExtraCops_PossibleSuspectStruct data;
    SCHEDULE_SPEECH(ExtraCops_PossibleSuspect, data, nullptr);
}

void MiscSpeech::WrongSuspect() {
    Csis::ExtraCops_WrongSuspectStruct data;
    SCHEDULE_SPEECH(ExtraCops_WrongSuspect, data, nullptr);
}

void MiscSpeech::D_Day() {
    Csis::D_DayStruct data;
    SCHEDULE_SPEECH(D_Day, data, nullptr);
}

void MiscSpeech::DispIntroRace() {
    Csis::DispIntroRaceStruct data;
    SCHEDULE_SPEECH(DispIntroRace, data, nullptr);
}
