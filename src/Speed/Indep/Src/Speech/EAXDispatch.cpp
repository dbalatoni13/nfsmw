#include "EAXDispatch.h"
#include "ScheduleSpeech.hpp"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace MiscSpeech {
bool GetLocation(RoadNames road, int &region, int &location);
}

enum SpeechObservations {
    None = 0,
    Collision_Cop_Cop = 1,
    Collision_Cop_Traffic = 2,
    Collision_Cop_Suspect = 3,
    Collision_Cop_World = 4,
    Collision_Suspect_World = 5,
    Collision_Suspect_Suspect = 6,
    Collision_Suspect_Traffic = 7,
    Collision_Suspect_Structure = 8,
    Collision_Suspect_Tree = 9,
    Collision_Suspect_Guardrail = 10,
    Collision_Suspect_Train = 11,
    Collision_Suspect_Semi = 12,
    Collision_Suspect_GasStation = 13,
    Collision_Suspect_Spikebelt = 14,
};

namespace Csis {
enum Type_yes_no {
    Type_yes_no_Yes_True = 1,
    Type_yes_no_No_False = 2,
};

enum Type_num_suspects {
    Type_num_suspects_one_suspect = 1,
    Type_num_suspects_multiple_suspects = 2,
};

enum Type_address_group_type {
    Type_address_group_type_college_town = 1,
    Type_address_group_type_city = 2,
    Type_address_group_type_coastal = 4,
    Type_address_group_type_alpine = 8,
    Type_address_group_type_generic_any_ = 16,
};

enum Type_location_region {
    Type_location_region_college_town = 1,
    Type_location_region_coastal = 2,
    Type_location_region_coastal_extra = 4,
    Type_location_region_city = 8,
};

enum Type_roadblock_type {
    Type_roadblock_type_Roadblock_Generic_ = 1,
    Type_roadblock_type_Spikes = 2,
    Type_roadblock_type_Heli_Roadblock_disp_only_ = 4,
    Type_roadblock_type_Multiple_Roadblocks_disp_only_ = 8,
};

enum Type_disp_backup_eta {
    Type_disp_backup_eta_15sec = 1,
    Type_disp_backup_eta_30sec = 2,
    Type_disp_backup_eta_1min = 4,
    Type_disp_backup_eta_1min30sec = 8,
    Type_disp_backup_eta_2min = 16,
    Type_disp_backup_eta_2min_ = 32,
};

enum Type_encounter {
    Type_encounter_first_encounter = 1,
    Type_encounter_subsequent_encounter = 2,
};

struct Backup_DispBackupReplyStruct {
    int speaker_id;
    int yes_no;
    int subject_battalion;
    int subject_call_sign_id;
    int code;
    int disp_backup_type;
};

struct Arrest_DispArrestReplyStruct {
    int speaker_id;
};

struct AnytimeEvents_DispPursuitUpdateStruct {
    int speaker_id;
    int subject_battalion;
    int subject_call_sign_id;
};

struct AnytimeEvents_DispPursEscGenStruct {
    int speaker_id;
    int num_suspects;
};

struct AnytimeEvents_DispPursuitEscalationStruct {
    int speaker_id;
    int address_group_type;
    int pursuit_type;
    int num_suspects;
    int direction;
    int location;
    int location_region;
};

struct Backup_DispBackupUpdateStruct {
    int speaker_id;
    int yes_no;
};

struct AnytimeEvents_DispBreakAwayStruct {
    int speaker_id;
    int location_region;
    int location;
    int direction;
};

struct Setup_DispGoAheadStruct {
    int speaker_id;
};

struct AnytimeEvents_DispTimeExpiredStruct {
    int speaker_id;
};

struct AnytimeEvents_Disp911ReportStruct {
    int speaker_id;
    int pursuit_type;
    int num_suspects;
    int encounter;
    int direction;
    int location;
    int location_region;
    int address_group_type;
};

struct StaticRoadblock_DispRBUpdateStruct {
    int speaker_id;
    int code;
    int yes_no;
    int roadblock_type;
};

struct StaticRoadblock_DispRBReplyStruct {
    int speaker_id;
    int code;
    int roadblock_type;
    int yes_no;
    int subject_battalion;
    int subject_call_sign_id;
};

struct AnytimeEvents_DispJurisShiftStruct {
    int speaker_id;
    int jurisdiction;
};

struct Backup_DispBUETAStruct {
    int speaker_id;
    int disp_backup_eta;
};

struct Setup_DispVehDescripStruct {
    int speaker_id;
    int car_color;
    int car_type;
};

struct Setup_DispNoVehDescripStruct {
    int speaker_id;
};

struct StaticRoadblock_DispSubRBStruct {
    int speaker_id;
};

extern InterfaceId Backup_DispBackupReplyId;
extern InterfaceId Arrest_DispArrestReplyId;
extern InterfaceId AnytimeEvents_DispPursuitUpdateId;
extern InterfaceId AnytimeEvents_DispPursEscGenId;
extern InterfaceId AnytimeEvents_DispPursuitEscalationId;
extern InterfaceId Backup_DispBackupUpdateId;
extern InterfaceId AnytimeEvents_DispBreakAwayId;
extern InterfaceId Setup_DispGoAheadId;
extern InterfaceId AnytimeEvents_DispTimeExpiredId;
extern InterfaceId AnytimeEvents_Disp911ReportId;
extern InterfaceId StaticRoadblock_DispRBUpdateId;
extern InterfaceId StaticRoadblock_DispRBReplyId;
extern InterfaceId AnytimeEvents_DispJurisShiftId;
extern InterfaceId Backup_DispBUETAId;
extern InterfaceId Setup_DispVehDescripId;
extern InterfaceId Setup_DispNoVehDescripId;
extern InterfaceId StaticRoadblock_DispSubRBId;

extern FunctionHandle gBackup_DispBackupReplyHandle;
extern FunctionHandle gArrest_DispArrestReplyHandle;
extern FunctionHandle gAnytimeEvents_DispPursuitUpdateHandle;
extern FunctionHandle gAnytimeEvents_DispPursEscGenHandle;
extern FunctionHandle gAnytimeEvents_DispPursuitEscalationHandle;
extern FunctionHandle gBackup_DispBackupUpdateHandle;
extern FunctionHandle gAnytimeEvents_DispBreakAwayHandle;
extern FunctionHandle gSetup_DispGoAheadHandle;
extern FunctionHandle gAnytimeEvents_DispTimeExpiredHandle;
extern FunctionHandle gAnytimeEvents_Disp911ReportHandle;
extern FunctionHandle gStaticRoadblock_DispRBUpdateHandle;
extern FunctionHandle gStaticRoadblock_DispRBReplyHandle;
extern FunctionHandle gAnytimeEvents_DispJurisShiftHandle;
extern FunctionHandle gBackup_DispBUETAHandle;
extern FunctionHandle gSetup_DispVehDescripHandle;
extern FunctionHandle gSetup_DispNoVehDescripHandle;
extern FunctionHandle gStaticRoadblock_DispSubRBHandle;
}


EAXDispatch::~EAXDispatch() {}

void EAXDispatch::Update() {
    EAXCharacter::Update();
    *reinterpret_cast<unsigned int *>(&mSuspectLOS) = 1;
}

void EAXDispatch::BackupReply(EAXCop *cop, int yes, int type) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::Backup_DispBackupReplyStruct data;

    if (ai) {
        data.speaker_id = mSpeakerID;
        data.yes_no = Csis::Type_yes_no_Yes_True;
        if (yes == 0) {
            data.yes_no = Csis::Type_yes_no_No_False;
        }
        data.code = Type_code_dont_use_10_code;
        data.subject_battalion = cop->GetCallsign();
        data.subject_call_sign_id = cop->GetUnitNumber();
        data.disp_backup_type = type;
        ScheduleSpeech(data, Csis::Backup_DispBackupReplyId, Csis::gBackup_DispBackupReplyHandle, this);
    }
}

void EAXDispatch::ArrestReply() {
    Csis::Arrest_DispArrestReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Arrest_DispArrestReplyId, Csis::gArrest_DispArrestReplyHandle, this);
}

void EAXDispatch::PursuitUpdate(EAXCop *cop) {
    Csis::AnytimeEvents_DispPursuitUpdateStruct data;
    data.speaker_id = mSpeakerID;
    data.subject_battalion = cop->GetCallsign();
    data.subject_call_sign_id = cop->GetUnitNumber();
    ScheduleSpeech(data, Csis::AnytimeEvents_DispPursuitUpdateId, Csis::gAnytimeEvents_DispPursuitUpdateHandle, this);
}

void EAXDispatch::PursuitEscalationGeneric() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_DispPursEscGenStruct data;

    if (ai) {
        data.speaker_id = mSpeakerID;
        register int num_suspects = Csis::Type_num_suspects_one_suspect;
        if (ai->AreRacersNearby()) {
            num_suspects = Csis::Type_num_suspects_multiple_suspects;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech(data, Csis::AnytimeEvents_DispPursEscGenId, Csis::gAnytimeEvents_DispPursEscGenHandle, this);
    }
}

void EAXDispatch::PursuitEscalation() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    bool result;
    EAXCop *furthest;
    int dir;
    int region;
    int location;
    int last;
    register int num_suspects;
    register unsigned int direction;
    Csis::AnytimeEvents_DispPursuitEscalationStruct data;

    if (!ai) {
        return;
    }
    if (!ai->IsHeadingValid()) {
        PursuitEscalationGeneric();
        return;
    }
    data.speaker_id = mSpeakerID;
    furthest = ai->FindFurthestCop(true);
    if (furthest) {
        switch (furthest->GetCallsign()) {
        case Csis::Type_address_group_type_college_town:
            data.address_group_type = Csis::Type_address_group_type_college_town;
            break;
        case Csis::Type_address_group_type_city:
            data.address_group_type = Csis::Type_address_group_type_coastal;
            break;
        case Csis::Type_address_group_type_coastal:
            data.address_group_type = Csis::Type_address_group_type_city;
            break;
        case Csis::Type_address_group_type_alpine:
            data.address_group_type = Csis::Type_address_group_type_alpine;
            break;
        default:
            data.address_group_type = Csis::Type_address_group_type_generic_any_;
            break;
        }
    } else {
        data.address_group_type = Csis::Type_address_group_type_generic_any_;
    }
    last = ai->GetLastObservation();
    switch (last) {
    case Collision_Cop_Suspect:
        data.pursuit_type = Csis::Type_pursuit_type_Unit_Rammed;
        break;
    case Collision_Suspect_Suspect:
    case Collision_Suspect_Spikebelt:
        data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
        break;
    case Collision_Suspect_Traffic:
    case Collision_Suspect_Train:
    case Collision_Suspect_Semi:
        data.pursuit_type = Csis::Type_pursuit_type_Hit_and_Run;
        break;
    case Collision_Suspect_World:
    case Collision_Suspect_Structure:
    case Collision_Suspect_Tree:
    case Collision_Suspect_Guardrail:
    case Collision_Suspect_GasStation:
        data.pursuit_type = Csis::Type_pursuit_type_Reckless;
        break;
    default:
        data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
        break;
    }
    num_suspects = Csis::Type_num_suspects_one_suspect;
    if (ai->AreRacersNearby()) {
        num_suspects = Csis::Type_num_suspects_multiple_suspects;
    }
    data.num_suspects = num_suspects;
    result = MiscSpeech::GetLocation(ai->GetLastKnownRoad(), region, location);
    if (!result) {
        result = MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location);
        if (!result) {
            MiscSpeech::GetLocation(ai->GetPlayerRoadID(1), region, location);
        }
    }
    data.location_region = region;
    data.location = location;
    direction = ai->GetLastKnownDirection();
    if (direction == 0) {
        direction = ai->GetPlayerDirection(0);
        if (direction == 0) {
            direction = ai->GetPlayerDirection(1);
            if (direction == 0) {
                dir = bRandom(4);
                direction = 1 << dir;
            }
        }
    }
    data.direction = direction;
    ScheduleSpeech(data, Csis::AnytimeEvents_DispPursuitEscalationId, Csis::gAnytimeEvents_DispPursuitEscalationHandle, this);
}

void EAXDispatch::BackupUpdate(EAXCop *, int yes) {
    Csis::Backup_DispBackupUpdateStruct data;
    data.yes_no = Csis::Type_yes_no_No_False;
    data.speaker_id = mSpeakerID;
    if (yes != 0) {
        data.yes_no = Csis::Type_yes_no_Yes_True;
    }
    ScheduleSpeech(data, Csis::Backup_DispBackupUpdateId, Csis::gBackup_DispBackupUpdateHandle, this);
}

void EAXDispatch::BreakAway() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    bool result;
    unsigned int dir;
    int location_region;
    int location;
    Csis::AnytimeEvents_DispBreakAwayStruct data;
    unsigned int last_known_direction;
    unsigned int player_direction_0;
    unsigned int player_direction_1;
    RoadNames last_known_road;
    RoadNames player_road_0;
    RoadNames player_road_1;

    if (ai) {
        last_known_direction = ai->GetLastKnownDirection();
        player_direction_0 = ai->GetPlayerDirection(0);
        player_direction_1 = ai->GetPlayerDirection(1);
        last_known_road = ai->GetLastKnownRoad();
        player_road_0 = ai->GetPlayerRoadID(0);
        player_road_1 = ai->GetPlayerRoadID(1);
        result = MiscSpeech::GetLocation(last_known_road, location_region, location);
        if (!result) {
            result = MiscSpeech::GetLocation(player_road_0, location_region, location);
            if (!result) {
                MiscSpeech::GetLocation(player_road_1, location_region, location);
            }
        }
        data.location_region = location_region;
        data.location = location;
        data.direction = last_known_direction;
        if (data.direction == 0) {
            data.direction = player_direction_0;
            if (data.direction == 0) {
                data.direction = player_direction_1;
                if (data.direction == 0) {
                    dir = bRandom(4);
                    data.direction = 1 << (dir & 0x3F);
                }
            }
        }
        data.speaker_id = mSpeakerID;
        data.location_region = location_region;
        data.location = location;
        ScheduleSpeech(data, Csis::AnytimeEvents_DispBreakAwayId, Csis::gAnytimeEvents_DispBreakAwayHandle, this);
    }
}

void EAXDispatch::GoAhead() {
    Csis::Setup_DispGoAheadStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Setup_DispGoAheadId, Csis::gSetup_DispGoAheadHandle, this);
}

void EAXDispatch::TimeExpired() {
    Csis::AnytimeEvents_DispTimeExpiredStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::AnytimeEvents_DispTimeExpiredId, Csis::gAnytimeEvents_DispTimeExpiredHandle, this);
}

void EAXDispatch::Report911(Csis::Type_pursuit_type infraction) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_Disp911ReportStruct data;
    int location;
    int location_region;
    bool result;
    unsigned int dir;
    register int num_suspects;
    register int address_group_type;

    if (!ai) {
        return;
    }
    data.speaker_id = mSpeakerID;
    data.pursuit_type = infraction;
    num_suspects = Csis::Type_num_suspects_one_suspect;
    if (ai->AreRacersNearby()) {
        num_suspects = Csis::Type_num_suspects_multiple_suspects;
    }
    data.num_suspects = num_suspects;
    data.encounter = Csis::Type_encounter_first_encounter;
    result = MiscSpeech::GetLocation(ai->GetLastKnownRoad(), location_region, location);
    if (!result) {
        result = MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), location_region, location);
        if (!result) {
            MiscSpeech::GetLocation(ai->GetPlayerRoadID(1), location_region, location);
        }
    }
    data.location_region = location_region;
    data.location = location;
    {
        register unsigned int direction = ai->GetLastKnownDirection();
        if ((direction == 0) && ((direction = ai->GetPlayerDirection(0)) == 0) &&
            ((direction = ai->GetPlayerDirection(1)) == 0)) {
            dir = bRandom(4);
            direction = 1 << dir;
        }
        data.direction = direction;
    }
    address_group_type = location_region;
    switch (address_group_type) {
    case Csis::Type_location_region_coastal:
    case Csis::Type_location_region_coastal_extra:
        address_group_type = Csis::Type_address_group_type_coastal;
        break;
    case Csis::Type_location_region_city:
        address_group_type = Csis::Type_address_group_type_city;
        break;
    case Csis::Type_location_region_college_town:
        break;
    default:
        address_group_type = Csis::Type_address_group_type_generic_any_;
        break;
    }
    data.address_group_type = address_group_type;
    dir = bRandom(3);
    if (dir == 2) {
        data.address_group_type = Csis::Type_address_group_type_generic_any_;
    }
    ScheduleSpeech(data, Csis::AnytimeEvents_Disp911ReportId, Csis::gAnytimeEvents_Disp911ReportHandle, this);
}

void EAXDispatch::RBUpdate(EAXCop *, signed char true_false) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::StaticRoadblock_DispRBUpdateStruct data;

    data.speaker_id = mSpeakerID;
    data.code = GetRandomizedCode();
    data.roadblock_type = Csis::Type_roadblock_type_Roadblock_Generic_;
    if (ai->NumRoadBlocks() > 1) {
        data.roadblock_type = Csis::Type_roadblock_type_Multiple_Roadblocks_disp_only_;
    }
    data.yes_no = Csis::Type_yes_no_No_False;
    if (true_false > 0) {
        data.yes_no = Csis::Type_yes_no_Yes_True;
    }
    ScheduleSpeech(data, Csis::StaticRoadblock_DispRBUpdateId, Csis::gStaticRoadblock_DispRBUpdateHandle, this);
}

void EAXDispatch::RBReply(EAXCop *cop, signed char true_false, unsigned int type) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::StaticRoadblock_DispRBReplyStruct data;

    data.speaker_id = mSpeakerID;
    data.code = GetRandomizedCode();
    data.roadblock_type = type;
    if (type == 0) {
        if (!ai->SpikesEnabled()) {
            data.roadblock_type = Csis::Type_roadblock_type_Roadblock_Generic_;
            if (ai->NumRoadBlocks() > 1) {
                data.roadblock_type = Csis::Type_roadblock_type_Multiple_Roadblocks_disp_only_;
            }
        } else {
            data.roadblock_type = Csis::Type_roadblock_type_Spikes;
        }
    }
    data.yes_no = Csis::Type_yes_no_No_False;
    if (true_false > 0) {
        data.yes_no = Csis::Type_yes_no_Yes_True;
    }
    data.subject_battalion = cop->GetCallsign();
    data.subject_call_sign_id = cop->GetUnitNumber();
    ScheduleSpeech(data, Csis::StaticRoadblock_DispRBReplyId, Csis::gStaticRoadblock_DispRBReplyHandle, this);
}

void EAXDispatch::JurisShift(Csis::Type_jurisdiction jurisdiction) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_DispJurisShiftStruct data;

    if (ai) {
        data.speaker_id = mSpeakerID;
        data.jurisdiction = jurisdiction;
        ScheduleSpeech(data, Csis::AnytimeEvents_DispJurisShiftId, Csis::gAnytimeEvents_DispJurisShiftHandle, this);
    }
}

void EAXDispatch::BackupETA() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    IPursuit *pursuit;
    float eta;
    Csis::Backup_DispBUETAStruct data;

    if (ai && (pursuit = ai->GetPursuit(), pursuit)) {
        data.speaker_id = mSpeakerID;
        eta = pursuit->GetBackupETA();
        if ((eta > 10.0f) && (eta < 20.0f)) {
            data.disp_backup_eta = Csis::Type_disp_backup_eta_15sec;
        } else if ((eta > 20.0f) && (eta < 40.0f)) {
            data.disp_backup_eta = Csis::Type_disp_backup_eta_30sec;
        } else if ((eta > 40.0f) && (eta < 75.0f)) {
            data.disp_backup_eta = Csis::Type_disp_backup_eta_1min;
        } else if ((eta > 75.0f) && (eta < 100.0f)) {
            data.disp_backup_eta = Csis::Type_disp_backup_eta_1min30sec;
        } else if ((eta > 100.0f) && (eta < 140.0f)) {
            data.disp_backup_eta = Csis::Type_disp_backup_eta_2min;
        } else if (eta > 140.0f) {
            data.disp_backup_eta = Csis::Type_disp_backup_eta_2min_;
        } else {
            return;
        }
        ScheduleSpeech(data, Csis::Backup_DispBUETAId, Csis::gBackup_DispBUETAHandle, this);
    }
}

void EAXDispatch::VehicleDescription() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::Setup_DispVehDescripStruct data;
    if (ai) {
        data.speaker_id = mSpeakerID;
        data.car_color = ai->GetPlayerCarColor();
        if (data.car_color != 0) {
            data.car_type = static_cast<int>(ai->GetPlayerSpecs().VerbalType());
            ScheduleSpeech(data, Csis::Setup_DispVehDescripId, Csis::gSetup_DispVehDescripHandle, this);
        }
    }
}

void EAXDispatch::NoVehicleDescription() {
    Csis::Setup_DispNoVehDescripStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Setup_DispNoVehDescripId, Csis::gSetup_DispNoVehDescripHandle, this);
}

void EAXDispatch::SubRBReply() {
    Csis::StaticRoadblock_DispSubRBStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::StaticRoadblock_DispSubRBId, Csis::gStaticRoadblock_DispSubRBHandle, this);
}
