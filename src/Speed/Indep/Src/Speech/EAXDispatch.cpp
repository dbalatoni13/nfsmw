#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

EAXDispatch::~EAXDispatch() {}

void EAXDispatch::Update() {
    this->EAXCharacter::Update();
    this->mSuspectLOS = true;
}

void EAXDispatch::BackupReply(EAXCop *cop, int yes, int type) {
    SoundAI *ai = SoundAI::Get();
    Csis::Backup_DispBackupReplyStruct data;

    if (ai != nullptr) {
        data.speaker_id = this->mSpeakerID;
        data.code = Csis::Type_code_dont_use_10_code;
        if (yes != 0) {
            data.yes_no = Csis::Type_yes_no_Yes_True;
        } else {
            data.yes_no = Csis::Type_yes_no_No_False;
        }
        data.subject_battalion = static_cast<Csis::Type_subject_battalion>(cop->GetCallsign());
        data.subject_call_sign_id = static_cast<Csis::Type_subject_call_sign_id>(cop->GetUnitNumber());
        data.disp_backup_type = static_cast<Csis::Type_disp_backup_type>(type);
        Speech::Manager::ScheduleSpeech(data, Csis::Backup_DispBackupReplyId, Csis::gBackup_DispBackupReplyHandle, this);
    }
}

void EAXDispatch::ArrestReply() {
    Csis::Arrest_DispArrestReplyStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Arrest_DispArrestReplyId, Csis::gArrest_DispArrestReplyHandle, this);
}

void EAXDispatch::PursuitUpdate(EAXCop *cop) {
    Csis::AnytimeEvents_DispPursuitUpdateStruct data;
    data.speaker_id = this->mSpeakerID;
    data.subject_battalion = static_cast<Csis::Type_subject_battalion>(cop->GetCallsign());
    data.subject_call_sign_id = static_cast<Csis::Type_subject_call_sign_id>(cop->GetUnitNumber());
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_DispPursuitUpdateId, Csis::gAnytimeEvents_DispPursuitUpdateHandle, this);
}

void EAXDispatch::PursuitEscalationGeneric() {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_DispPursEscGenStruct data;

    if (ai != nullptr) {
        data.speaker_id = this->mSpeakerID;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_DispPursEscGenId, Csis::gAnytimeEvents_DispPursEscGenHandle, this);
    }
}

void EAXDispatch::PursuitEscalation() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    if (!ai->IsHeadingValid()) {
        this->PursuitEscalationGeneric();
        return;
    }

    Csis::AnytimeEvents_DispPursuitEscalationStruct data;
    data.speaker_id = this->mSpeakerID;
    EAXCop *furthest = ai->FindFurthestCop(true);

    if (furthest != nullptr) {
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
    Speech::SpeechObservations last = ai->GetLastObservation();
    switch (last) {
        case Speech::Collision_Cop_Suspect:
            data.pursuit_type = Csis::Type_pursuit_type_Unit_Rammed;
            break;
        case Speech::Collision_Suspect_Suspect:
        case Speech::Collision_Suspect_Spikebelt:
            data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
            break;
        case Speech::Collision_Suspect_Traffic:
        case Speech::Collision_Suspect_Train:
        case Speech::Collision_Suspect_Semi:
            data.pursuit_type = Csis::Type_pursuit_type_Hit_and_Run;
            break;
        case Speech::Collision_Suspect_World:
        case Speech::Collision_Suspect_Structure:
        case Speech::Collision_Suspect_Tree:
        case Speech::Collision_Suspect_Guardrail:
        case Speech::Collision_Suspect_GasStation:
            data.pursuit_type = Csis::Type_pursuit_type_Reckless;
            break;
        default:
            data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
            break;
    }
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;

    Csis::Type_location_region region;
    Csis::Type_location location;
    bool result = MiscSpeech::GetLocation(ai->GetLastKnownRoad(), region, location);
    if (!result) {
        result = MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location);
        if (!result) {
            MiscSpeech::GetLocation(ai->GetPlayerRoadID(1), region, location);
        }
    }
    data.location_region = region;
    data.location = location;

    unsigned int dir = ai->GetLastKnownDirection();
    if (dir == 0) {
        dir = ai->GetPlayerDirection(0);
        if (dir == 0) {
            dir = ai->GetPlayerDirection(1);
            if (dir == 0) {
                dir = 1 << bRandom(4);
            }
        }
        data.direction = static_cast<Csis::Type_direction>(dir);
    } else {
        data.direction = static_cast<Csis::Type_direction>(dir);
    }
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_DispPursuitEscalationId, Csis::gAnytimeEvents_DispPursuitEscalationHandle, this);
}

void EAXDispatch::BackupUpdate(EAXCop *cop, int yes) {
    Csis::Backup_DispBackupUpdateStruct data;
    data.speaker_id = this->mSpeakerID;
    data.yes_no = yes != 0 ? Csis::Type_yes_no_Yes_True : Csis::Type_yes_no_No_False;
    Speech::Manager::ScheduleSpeech(data, Csis::Backup_DispBackupUpdateId, Csis::gBackup_DispBackupUpdateHandle, this);
}

void EAXDispatch::BreakAway() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::AnytimeEvents_DispBreakAwayStruct data;
    Csis::Type_location_region region;
    Csis::Type_location location;
    bool result = MiscSpeech::GetLocation(ai->GetLastKnownRoad(), region, location);
    if (!result) {
        result = MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location);
        if (!result) {
            MiscSpeech::GetLocation(ai->GetPlayerRoadID(1), region, location);
        }
    }
    data.location_region = region;
    data.location = location;
    unsigned int dir = ai->GetLastKnownDirection();
    if (dir == 0) {
        dir = ai->GetPlayerDirection(0);
        if (dir == 0) {
            dir = ai->GetPlayerDirection(1);
            if (dir == 0) {
                dir = 1 << bRandom(4);
            }
        }
        data.direction = static_cast<Csis::Type_direction>(dir);
    } else {
        data.direction = static_cast<Csis::Type_direction>(dir);
    }
    data.speaker_id = this->mSpeakerID;
    data.location_region = region;
    data.location = location;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_DispBreakAwayId, Csis::gAnytimeEvents_DispBreakAwayHandle, this);
}

void EAXDispatch::GoAhead() {
    Csis::Setup_DispGoAheadStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_DispGoAheadId, Csis::gSetup_DispGoAheadHandle, this);
}

void EAXDispatch::TimeExpired() {
    Csis::AnytimeEvents_DispTimeExpiredStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_DispTimeExpiredId, Csis::gAnytimeEvents_DispTimeExpiredHandle, this);
}

void EAXDispatch::Report911(Csis::Type_pursuit_type infraction) {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::AnytimeEvents_Disp911ReportStruct data;
    Csis::Type_location_region region;
    Csis::Type_location location;

    data.speaker_id = this->mSpeakerID;
    data.pursuit_type = infraction;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    data.encounter = Csis::Type_encounter_first_encounter;

    bool result = MiscSpeech::GetLocation(ai->GetLastKnownRoad(), region, location);
    if (!result) {
        result = MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location);
        if (!result) {
            MiscSpeech::GetLocation(ai->GetPlayerRoadID(1), region, location);
        }
    }
    data.location_region = region;
    data.location = location;
    unsigned int dir = ai->GetLastKnownDirection();
    if (dir == 0) {
        dir = ai->GetPlayerDirection(0);
        if (dir == 0) {
            dir = ai->GetPlayerDirection(1);
            if (dir == 0) {
                dir = 1 << bRandom(4);
            }
        }
    }
    data.direction = static_cast<Csis::Type_direction>(dir);
    dir = region;
    switch (static_cast<Csis::Type_location_region>(dir)) {
        case Csis::Type_location_region_coastal:
        case Csis::Type_location_region_coastal_extra:
            dir = Csis::Type_address_group_type_coastal;
            break;
        case Csis::Type_location_region_city:
            dir = Csis::Type_address_group_type_city;
            break;
        case Csis::Type_location_region_college_town:
            break;
        default:
            dir = Csis::Type_address_group_type_generic_any_;
            break;
    }
    data.address_group_type = static_cast<Csis::Type_address_group_type>(dir);
    dir = bRandom(3);
    if (dir == 2) {
        data.address_group_type = Csis::Type_address_group_type_generic_any_;
    }
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_Disp911ReportId, Csis::gAnytimeEvents_Disp911ReportHandle, this);
}

void EAXDispatch::RBUpdate(EAXCop *cop, int8 true_false) {
    SoundAI *ai = SoundAI::Get();

    Csis::StaticRoadblock_DispRBUpdateStruct data;
    data.speaker_id = this->mSpeakerID;
    data.code = this->GetRandomizedCode();
    data.roadblock_type =
        ai->NumRoadBlocks() > 1 ? Csis::Type_roadblock_type_Multiple_Roadblocks_disp_only_ : Csis::Type_roadblock_type_Roadblock_Generic_;
    data.yes_no = true_false > 0 ? Csis::Type_yes_no_Yes_True : Csis::Type_yes_no_No_False;
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_DispRBUpdateId, Csis::gStaticRoadblock_DispRBUpdateHandle, this);
}

void EAXDispatch::RBReply(EAXCop *cop, int8 true_false, unsigned int type) {
    SoundAI *ai = SoundAI::Get();

    Csis::StaticRoadblock_DispRBReplyStruct data;
    data.speaker_id = this->mSpeakerID;
    data.code = this->GetRandomizedCode();
    if (type != 0) {
        data.roadblock_type = static_cast<Csis::Type_roadblock_type>(type);
    } else {
        if (ai->SpikesEnabled()) {
            data.roadblock_type = Csis::Type_roadblock_type_Spikes;
        } else if (ai->NumRoadBlocks() > 1) {
            data.roadblock_type = Csis::Type_roadblock_type_Multiple_Roadblocks_disp_only_;
        } else {
            data.roadblock_type = Csis::Type_roadblock_type_Roadblock_Generic_;
        }
    }
    data.yes_no = true_false > 0 ? Csis::Type_yes_no_Yes_True : Csis::Type_yes_no_No_False;
    data.subject_battalion = static_cast<Csis::Type_subject_battalion>(cop->GetCallsign());
    data.subject_call_sign_id = static_cast<Csis::Type_subject_call_sign_id>(cop->GetUnitNumber());
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_DispRBReplyId, Csis::gStaticRoadblock_DispRBReplyHandle, this);
}

// STRIPPED
void EAXDispatch::EVReply() {}

void EAXDispatch::JurisShift(Csis::Type_jurisdiction jurisdiction) {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::AnytimeEvents_DispJurisShiftStruct data;
    data.speaker_id = this->mSpeakerID;
    data.jurisdiction = jurisdiction;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_DispJurisShiftId, Csis::gAnytimeEvents_DispJurisShiftHandle, this);
}

void EAXDispatch::BackupETA() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    IPursuit *pursuit = ai->GetPursuit();
    if (pursuit == nullptr) {
        return;
    }

    Csis::Backup_DispBUETAStruct data;
    data.speaker_id = this->mSpeakerID;
    float eta = pursuit->GetBackupETA();
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
    Speech::Manager::ScheduleSpeech(data, Csis::Backup_DispBUETAId, Csis::gBackup_DispBUETAHandle, this);
}

void EAXDispatch::VehicleDescription() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::Setup_DispVehDescripStruct data;
    data.speaker_id = this->mSpeakerID;
    unsigned int color = ai->GetPlayerCarColor();
    if (color == 0) {
        return;
    }

    const Attrib::Gen::pvehicle &pcar = ai->GetPlayerSpecs();
    data.car_color = static_cast<Csis::Type_car_color>(color);
    data.car_type = static_cast<Csis::Type_car_type>(pcar.VerbalType());
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_DispVehDescripId, Csis::gSetup_DispVehDescripHandle, this);
}

void EAXDispatch::NoVehicleDescription() {
    Csis::Setup_DispNoVehDescripStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_DispNoVehDescripId, Csis::gSetup_DispNoVehDescripHandle, this);
}

// STRIPPED
void EAXDispatch::VehicleVinyls() {}

// STRIPPED
void EAXDispatch::VehicleCustomPaint() {}

void EAXDispatch::SubRBReply() {
    Csis::StaticRoadblock_DispSubRBStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_DispSubRBId, Csis::gStaticRoadblock_DispSubRBHandle, this);
}
