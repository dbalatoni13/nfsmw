#include "EAXDispatch.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Speech/EAXCop.h"
#include "Speed/Indep/Src/Speech/MiscSpeech.h"
#include "Speed/Indep/Src/Speech/Observer.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

EAXDispatch::~EAXDispatch() {}

void EAXDispatch::Update() {
    EAXCharacter::Update();
    mSuspectLOS = true;
}

void EAXDispatch::BackupReply(EAXCop *cop, int yes, int type) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::Backup_DispBackupReplyStruct data;
        data.speaker_id = mSpeakerID;
        data.code = Csis::Type_code_dont_use_10_code;
        data.yes_no = yes ? Csis::Type_yes_no_Yes_True : Csis::Type_yes_no_No_False;
        data.subject_battalion = static_cast<Csis::Type_subject_battalion>(cop->GetCallsign());
        data.subject_call_sign_id = static_cast<Csis::Type_subject_call_sign_id>(cop->GetUnitNumber());
        data.disp_backup_type = static_cast<Csis::Type_disp_backup_type>(type);
        SCHEDULE_SPEECH(Backup_DispBackupReply, data, this);
    }
}

void EAXDispatch::ArrestReply() {
    Csis::Arrest_DispArrestReplyStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Arrest_DispArrestReply, data, this);
}

void EAXDispatch::PursuitUpdate(EAXCop *cop) {
    Csis::AnytimeEvents_DispPursuitUpdateStruct data;
    data.speaker_id = mSpeakerID;
    data.subject_battalion = static_cast<Csis::Type_subject_battalion>(cop->GetCallsign());
    data.subject_call_sign_id = static_cast<Csis::Type_subject_call_sign_id>(cop->GetUnitNumber());
    SCHEDULE_SPEECH(AnytimeEvents_DispPursuitUpdate, data, this);
}

void EAXDispatch::PursuitEscalationGeneric() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    if (ai == nullptr) {
        return;
    }

    Csis::AnytimeEvents_DispPursEscGenStruct data;
    data.speaker_id = mSpeakerID;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    SCHEDULE_SPEECH(AnytimeEvents_DispPursEscGen, data, this);
}

void EAXDispatch::PursuitEscalation() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    if (!ai->IsHeadingValid()) {
        PursuitEscalationGeneric();
        return;
    }
    Csis::AnytimeEvents_DispPursuitEscalationStruct data;
    data.speaker_id = mSpeakerID;
    EAXCop *furthest = ai->FindFurthestCop(true);
    if (furthest != nullptr) {
        switch (furthest->GetCallsign()) {
        case Csis::Type_speaker_battalion_Rosewood:
            data.address_group_type = Csis::Type_address_group_type_college_town;
            break;
        case Csis::Type_speaker_battalion_Coastal:
            data.address_group_type = Csis::Type_address_group_type_coastal;
            break;
        case Csis::Type_speaker_battalion_City:
            data.address_group_type = Csis::Type_address_group_type_city;
            break;
        case Csis::Type_speaker_battalion_Alpine:
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
        if (!MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location)) {
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
    // Barrera: sin ella el planificador hunde el `stw data.direction` por
    // detras de los dos `lis` de los argumentos (prioridad 4 contra 3) y
    // `dir` deja de nacer en r0.
    // r67 (juego-world-snd): IRREDUCIBLE. Sin esta barrera y la de BreakAway (zSpeech
    // 39d1580dcbc5228a -> ea7cbfc609d2b519): aqui 5 filas (el stw de direction una
    // ranura tarde) y en BreakAway 15. El DWARF tiene las mismas locales que la
    // fuente. El mapa de lineas del original pone direction en la 218 y la llamada
    // en la 220: UNA linea sin codigo entre las dos, justo donde va la barrera (en
    // BreakAway, 284 y 286). Lo que hubiera ahi no deja ni bytes ni locales.
    // r76: barrera fuera; el hueco del mapa de lineas puede ser una macro apagada.
    SCHEDULE_SPEECH(AnytimeEvents_DispPursuitEscalation, data, this);
}

void EAXDispatch::BackupUpdate(EAXCop *cop, int yes) {
    Csis::Backup_DispBackupUpdateStruct data;
    data.speaker_id = mSpeakerID;
    data.yes_no = yes ? Csis::Type_yes_no_Yes_True : Csis::Type_yes_no_No_False;
    SCHEDULE_SPEECH(Backup_DispBackupUpdate, data, this);
}

void EAXDispatch::BreakAway() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::AnytimeEvents_DispBreakAwayStruct data;
    Csis::Type_location_region region;
    Csis::Type_location location;
    bool result = MiscSpeech::GetLocation(ai->GetLastKnownRoad(), region, location);
    if (!result) {
        if (!MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location)) {
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
    // Barrera: sin ella el planificador hunde el `stw data.direction` por
    // detras de los dos `lis` de los argumentos (prioridad 4 contra 3) y
    // `dir` deja de nacer en r0.
    // r67 (juego-world-snd): IRREDUCIBLE. El mapa de lineas del original reasigna
    // `result` (259, 261, 266) y encadena `dir` con ifs SUCESIVOS (274-281), no
    // anidados. Escrito asi y sin barrera -> 06054b8f502f6448: las MISMAS 15 filas
    // que la forma anidada sin barrera (dir r0 -> r10 y el stw de direction detras
    // de los lis); el anidamiento no llega al planificador. Entre direction (284) y
    // speaker_id (286) el original tiene una linea sin codigo.
    // r76: barrera fuera; el hueco del mapa de lineas puede ser una macro apagada.
    data.speaker_id = mSpeakerID;
    data.location_region = region;
    data.location = location;
    SCHEDULE_SPEECH(AnytimeEvents_DispBreakAway, data, this);
}

void EAXDispatch::GoAhead() {
    Csis::Setup_DispGoAheadStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Setup_DispGoAhead, data, this);
}

void EAXDispatch::TimeExpired() {
    Csis::AnytimeEvents_DispTimeExpiredStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_DispTimeExpired, data, this);
}

void EAXDispatch::Report911(Csis::Type_pursuit_type infraction) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::AnytimeEvents_Disp911ReportStruct data;
    data.speaker_id = mSpeakerID;
    data.pursuit_type = infraction;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    data.encounter = Csis::Type_encounter_first_encounter;
    Csis::Type_location_region region;
    Csis::Type_location location;
    bool result = MiscSpeech::GetLocation(ai->GetLastKnownRoad(), region, location);
    if (!result) {
        if (!MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location)) {
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
        data.address_group_type = Csis::Type_address_group_type_coastal;
        break;
    case Csis::Type_location_region_city:
        data.address_group_type = Csis::Type_address_group_type_city;
        break;
    case Csis::Type_location_region_college_town:
        data.address_group_type = Csis::Type_address_group_type_college_town;
        break;
    default:
        data.address_group_type = Csis::Type_address_group_type_generic_any_;
        break;
    }
    if (bRandom(3) == 2) {
        data.address_group_type = Csis::Type_address_group_type_generic_any_;
    }
    SCHEDULE_SPEECH(AnytimeEvents_Disp911Report, data, this);
}

void EAXDispatch::RBUpdate(EAXCop *cop, signed char true_false) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::StaticRoadblock_DispRBUpdateStruct data;
    data.speaker_id = mSpeakerID;
    data.code = GetRandomizedCode();
    data.roadblock_type = ai->NumRoadBlocks() > 1 ? static_cast<Csis::Type_roadblock_type>(8)
                                                  : static_cast<Csis::Type_roadblock_type>(1);
    data.yes_no = true_false > 0 ? Csis::Type_yes_no_Yes_True : Csis::Type_yes_no_No_False;
    SCHEDULE_SPEECH(StaticRoadblock_DispRBUpdate, data, this);
}

void EAXDispatch::RBReply(EAXCop *cop, signed char true_false, unsigned int type) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::StaticRoadblock_DispRBReplyStruct data;
    data.speaker_id = mSpeakerID;
    data.code = GetRandomizedCode();
    if (type != 0) {
        data.roadblock_type = static_cast<Csis::Type_roadblock_type>(type);
    } else if (ai->SpikesEnabled()) {
        data.roadblock_type = static_cast<Csis::Type_roadblock_type>(2);
    } else if (ai->NumRoadBlocks() > 1) {
        data.roadblock_type = static_cast<Csis::Type_roadblock_type>(8);
    } else {
        data.roadblock_type = static_cast<Csis::Type_roadblock_type>(1);
    }
    data.yes_no = true_false > 0 ? Csis::Type_yes_no_Yes_True : Csis::Type_yes_no_No_False;
    data.subject_battalion = static_cast<Csis::Type_subject_battalion>(cop->GetCallsign());
    data.subject_call_sign_id = static_cast<Csis::Type_subject_call_sign_id>(cop->GetUnitNumber());
    SCHEDULE_SPEECH(StaticRoadblock_DispRBReply, data, this);
}

void EAXDispatch::JurisShift(Csis::Type_jurisdiction jurisdiction) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::AnytimeEvents_DispJurisShiftStruct data;
        data.speaker_id = mSpeakerID;
        data.jurisdiction = jurisdiction;
        SCHEDULE_SPEECH(AnytimeEvents_DispJurisShift, data, this);
    }
}

void EAXDispatch::BackupETA() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetPursuit() != nullptr) {
        IPursuit *pursuit = ai->GetPursuit();
        Csis::Backup_DispBUETAStruct data;
        data.speaker_id = mSpeakerID;
        float eta = pursuit->GetBackupETA();
        if (eta > 10.0f && eta < 20.0f) {
            data.disp_backup_eta = static_cast<Csis::Type_disp_backup_eta>(1);
        } else if (eta > 20.0f && eta < 40.0f) {
            data.disp_backup_eta = static_cast<Csis::Type_disp_backup_eta>(2);
        } else if (eta > 40.0f && eta < 75.0f) {
            data.disp_backup_eta = static_cast<Csis::Type_disp_backup_eta>(4);
        } else if (eta > 75.0f && eta < 100.0f) {
            data.disp_backup_eta = static_cast<Csis::Type_disp_backup_eta>(8);
        } else if (eta > 100.0f && eta < 140.0f) {
            data.disp_backup_eta = static_cast<Csis::Type_disp_backup_eta>(16);
        } else if (eta > 140.0f) {
            data.disp_backup_eta = static_cast<Csis::Type_disp_backup_eta>(32);
        } else {
            return;
        }
        SCHEDULE_SPEECH(Backup_DispBUETA, data, this);
    }
}

void EAXDispatch::VehicleDescription() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::Setup_DispVehDescripStruct data;
        data.speaker_id = mSpeakerID;
        unsigned int color = ai->GetPlayerCarColor();
        if (color != 0) {
            data.car_color = static_cast<Csis::Type_car_color>(color);
            data.car_type = ai->GetPlayerSpecs().VerbalType();
            SCHEDULE_SPEECH(Setup_DispVehDescrip, data, this);
        }
    }
}

void EAXDispatch::NoVehicleDescription() {
    Csis::Setup_DispNoVehDescripStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Setup_DispNoVehDescrip, data, this);
}

void EAXDispatch::SubRBReply() {
    Csis::StaticRoadblock_DispSubRBStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(StaticRoadblock_DispSubRB, data, this);
}
