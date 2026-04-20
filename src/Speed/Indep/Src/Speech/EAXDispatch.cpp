#include "EAXCop.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace MiscSpeech {
bool GetLocation(RoadNames road, int &region, int &location) asm("GetLocation__10MiscSpeech9RoadNamesRQ24Csis20Type_location_regionRQ24Csis13Type_location");
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

enum Type_pursuit_type {
    Type_pursuit_type_Generic_Speeder = 1,
    Type_pursuit_type_Possible_Wanted = 2,
    Type_pursuit_type_Hit_and_Run = 4,
    Type_pursuit_type_Reckless = 8,
    Type_pursuit_type_Unit_Rammed = 16,
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

enum Type_jurisdiction {
    Type_jurisdiction_state = 1,
    Type_jurisdiction_federal = 2,
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

extern void ScheduleSpeech_Backup_DispBackupReply(Csis::Backup_DispBackupReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Backup_DispBackupReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Arrest_DispArrestReply(Csis::Arrest_DispArrestReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Arrest_DispArrestReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_DispPursuitUpdate(Csis::AnytimeEvents_DispPursuitUpdateStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis37AnytimeEvents_DispPursuitUpdateStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_DispPursEscGen(Csis::AnytimeEvents_DispPursEscGenStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis34AnytimeEvents_DispPursEscGenStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_DispPursuitEscalation(Csis::AnytimeEvents_DispPursuitEscalationStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis41AnytimeEvents_DispPursuitEscalationStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_DispBackupUpdate(Csis::Backup_DispBackupUpdateStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis29Backup_DispBackupUpdateStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_DispBreakAway(Csis::AnytimeEvents_DispBreakAwayStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_DispBreakAwayStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_DispGoAhead(Csis::Setup_DispGoAheadStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis23Setup_DispGoAheadStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_DispTimeExpired(Csis::AnytimeEvents_DispTimeExpiredStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis35AnytimeEvents_DispTimeExpiredStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_Disp911Report(Csis::AnytimeEvents_Disp911ReportStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_Disp911ReportStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_DispRBUpdate(Csis::StaticRoadblock_DispRBUpdateStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis34StaticRoadblock_DispRBUpdateStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_DispRBReply(Csis::StaticRoadblock_DispRBReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33StaticRoadblock_DispRBReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_DispJurisShift(Csis::AnytimeEvents_DispJurisShiftStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis34AnytimeEvents_DispJurisShiftStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_DispBUETA(Csis::Backup_DispBUETAStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis22Backup_DispBUETAStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_DispVehDescrip(Csis::Setup_DispVehDescripStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis26Setup_DispVehDescripStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_DispNoVehDescrip(Csis::Setup_DispNoVehDescripStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Setup_DispNoVehDescripStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_DispSubRB(Csis::StaticRoadblock_DispSubRBStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31StaticRoadblock_DispSubRBStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");

struct EAXDispatch : public EAXCharacter {
    EAXDispatch(int sID);

    ~EAXDispatch() override;
    void Update() override;
    void BackupReply(EAXCop *cop, int yes, int type);
    void ArrestReply();
    void PursuitUpdate(EAXCop *cop);
    void PursuitEscalationGeneric();
    void PursuitEscalation();
    void BackupUpdate(EAXCop *cop, int yes);
    void BreakAway();
    void GoAhead();
    void TimeExpired();
    void Report911(Csis::Type_pursuit_type infraction);
    void RBUpdate(EAXCop *cop, signed char true_false);
    void RBReply(EAXCop *cop, signed char true_false, unsigned int type);
    void JurisShift(Csis::Type_jurisdiction jurisdiction);
    void BackupETA();
    void VehicleDescription();
    void NoVehicleDescription();
    void SubRBReply();
};

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
        ScheduleSpeech_Backup_DispBackupReply(data, Csis::Backup_DispBackupReplyId, Csis::gBackup_DispBackupReplyHandle, this);
    }
}

void EAXDispatch::ArrestReply() {
    Csis::Arrest_DispArrestReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Arrest_DispArrestReply(data, Csis::Arrest_DispArrestReplyId, Csis::gArrest_DispArrestReplyHandle, this);
}

void EAXDispatch::PursuitUpdate(EAXCop *cop) {
    Csis::AnytimeEvents_DispPursuitUpdateStruct data;
    data.speaker_id = mSpeakerID;
    data.subject_battalion = cop->GetCallsign();
    data.subject_call_sign_id = cop->GetUnitNumber();
    ScheduleSpeech_AnytimeEvents_DispPursuitUpdate(data, Csis::AnytimeEvents_DispPursuitUpdateId, Csis::gAnytimeEvents_DispPursuitUpdateHandle, this);
}

void EAXDispatch::PursuitEscalationGeneric() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_DispPursEscGenStruct data;

    if (ai) {
        data.speaker_id = mSpeakerID;
        data.num_suspects = Csis::Type_num_suspects_one_suspect;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            data.num_suspects = Csis::Type_num_suspects_multiple_suspects;
        }
        ScheduleSpeech_AnytimeEvents_DispPursEscGen(data, Csis::AnytimeEvents_DispPursEscGenId, Csis::gAnytimeEvents_DispPursEscGenHandle, this);
    }
}

void EAXDispatch::PursuitEscalation() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    bool result;
    EAXCop *furthest;
    unsigned int dir;
    int location_region;
    int location;
    int last;
    Csis::AnytimeEvents_DispPursuitEscalationStruct data;
    SoundAI::CarHeading *last_known;
    SoundAI::CarHeading *player_current;
    void *observer;

    if (!ai) {
        return;
    }
    if (!ai->IsHeadingValid()) {
        PursuitEscalationGeneric();
        return;
    }
    data.speaker_id = mSpeakerID;
    data.address_group_type = Csis::Type_address_group_type_generic_any_;
    furthest = ai->FindFurthestCop(true);
    if (furthest) {
        data.address_group_type = furthest->GetCallsign();
        if (data.address_group_type == Csis::Type_address_group_type_city) {
            data.address_group_type = Csis::Type_address_group_type_coastal;
        } else if (data.address_group_type < 3) {
            if (data.address_group_type != Csis::Type_address_group_type_college_town) {
                data.address_group_type = Csis::Type_address_group_type_generic_any_;
            }
        } else if (data.address_group_type == Csis::Type_address_group_type_coastal) {
            data.address_group_type = Csis::Type_address_group_type_city;
        } else if (data.address_group_type != Csis::Type_address_group_type_alpine) {
            data.address_group_type = Csis::Type_address_group_type_generic_any_;
        }
    }
    last = 0;
    observer = *reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x20C);
    if (observer) {
        last = *reinterpret_cast<int *>(reinterpret_cast<char *>(observer) + 0x20);
    }
    if (last < Collision_Suspect_Train) {
        if ((last > Collision_Suspect_Structure) || (last == Collision_Suspect_World)) {
            data.pursuit_type = Csis::Type_pursuit_type_Reckless;
        } else if (last < Collision_Suspect_Suspect) {
            if (last == Collision_Cop_Suspect) {
                data.pursuit_type = Csis::Type_pursuit_type_Unit_Rammed;
            } else {
                data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
            }
        } else if (last == Collision_Suspect_Suspect || last == Collision_Suspect_Spikebelt) {
            data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
        } else if (last == Collision_Suspect_Traffic) {
            data.pursuit_type = Csis::Type_pursuit_type_Hit_and_Run;
        } else {
            data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
        }
    } else if (last == Collision_Suspect_GasStation || last == Collision_Suspect_Structure || last == Collision_Suspect_Tree || last == Collision_Suspect_Guardrail) {
        data.pursuit_type = Csis::Type_pursuit_type_Reckless;
    } else if (last < Collision_Suspect_GasStation) {
        data.pursuit_type = Csis::Type_pursuit_type_Hit_and_Run;
    } else if (last == Collision_Suspect_Spikebelt) {
        data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
    } else {
        data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
    }
    data.num_suspects = Csis::Type_num_suspects_one_suspect;
    if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
        data.num_suspects = Csis::Type_num_suspects_multiple_suspects;
    }
    last_known = reinterpret_cast<SoundAI::CarHeading *>(reinterpret_cast<char *>(ai) + 0x1CC);
    player_current = reinterpret_cast<SoundAI::CarHeading *>(reinterpret_cast<char *>(ai) + 0x1AC);
    result = MiscSpeech::GetLocation(last_known->roadID, location_region, location);
    if (!result) {
        result = MiscSpeech::GetLocation(player_current[0].roadID, location_region, location);
        if (!result) {
            MiscSpeech::GetLocation(player_current[1].roadID, location_region, location);
        }
    }
    data.location_region = location_region;
    data.location = location;
    data.direction = last_known->direction;
    if (data.direction == 0) {
        data.direction = player_current[0].direction;
        if (data.direction == 0) {
            data.direction = player_current[1].direction;
            if (data.direction == 0) {
                dir = bRandom(4);
                data.direction = 1 << (dir & 0x3F);
            }
        }
    }
    ScheduleSpeech_AnytimeEvents_DispPursuitEscalation(data, Csis::AnytimeEvents_DispPursuitEscalationId, Csis::gAnytimeEvents_DispPursuitEscalationHandle, this);
}

void EAXDispatch::BackupUpdate(EAXCop *, int yes) {
    Csis::Backup_DispBackupUpdateStruct data;
    data.yes_no = Csis::Type_yes_no_No_False;
    data.speaker_id = mSpeakerID;
    if (yes != 0) {
        data.yes_no = Csis::Type_yes_no_Yes_True;
    }
    ScheduleSpeech_Backup_DispBackupUpdate(data, Csis::Backup_DispBackupUpdateId, Csis::gBackup_DispBackupUpdateHandle, this);
}

void EAXDispatch::BreakAway() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    bool result;
    unsigned int dir;
    int location_region;
    int location;
    Csis::AnytimeEvents_DispBreakAwayStruct data;
    SoundAI::CarHeading *last_known;
    SoundAI::CarHeading *player_current;

    if (ai) {
        last_known = reinterpret_cast<SoundAI::CarHeading *>(reinterpret_cast<char *>(ai) + 0x1CC);
        player_current = reinterpret_cast<SoundAI::CarHeading *>(reinterpret_cast<char *>(ai) + 0x1AC);
        result = MiscSpeech::GetLocation(last_known->roadID, location_region, location);
        if (!result) {
            result = MiscSpeech::GetLocation(player_current[0].roadID, location_region, location);
            if (!result) {
                MiscSpeech::GetLocation(player_current[1].roadID, location_region, location);
            }
        }
        data.location_region = location_region;
        data.location = location;
        data.direction = last_known->direction;
        if (data.direction == 0) {
            data.direction = player_current[0].direction;
            if (data.direction == 0) {
                data.direction = player_current[1].direction;
                if (data.direction == 0) {
                    dir = bRandom(4);
                    data.direction = 1 << (dir & 0x3F);
                }
            }
        }
        data.speaker_id = mSpeakerID;
        data.location_region = location_region;
        data.location = location;
        ScheduleSpeech_AnytimeEvents_DispBreakAway(data, Csis::AnytimeEvents_DispBreakAwayId, Csis::gAnytimeEvents_DispBreakAwayHandle, this);
    }
}

void EAXDispatch::GoAhead() {
    Csis::Setup_DispGoAheadStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Setup_DispGoAhead(data, Csis::Setup_DispGoAheadId, Csis::gSetup_DispGoAheadHandle, this);
}

void EAXDispatch::TimeExpired() {
    Csis::AnytimeEvents_DispTimeExpiredStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_DispTimeExpired(data, Csis::AnytimeEvents_DispTimeExpiredId, Csis::gAnytimeEvents_DispTimeExpiredHandle, this);
}

void EAXDispatch::Report911(Csis::Type_pursuit_type infraction) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    bool result;
    unsigned int dir;
    int location_region;
    int location;
    Csis::AnytimeEvents_Disp911ReportStruct data;
    SoundAI::CarHeading *last_known;
    SoundAI::CarHeading *player_current;

    if (!ai) {
        return;
    }
    data.speaker_id = mSpeakerID;
    data.num_suspects = Csis::Type_num_suspects_one_suspect;
    if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
        data.num_suspects = Csis::Type_num_suspects_multiple_suspects;
    }
    data.encounter = Csis::Type_encounter_first_encounter;
    data.pursuit_type = infraction;
    last_known = reinterpret_cast<SoundAI::CarHeading *>(reinterpret_cast<char *>(ai) + 0x1CC);
    player_current = reinterpret_cast<SoundAI::CarHeading *>(reinterpret_cast<char *>(ai) + 0x1AC);
    result = MiscSpeech::GetLocation(last_known->roadID, location_region, location);
    if (!result) {
        result = MiscSpeech::GetLocation(player_current[0].roadID, location_region, location);
        if (!result) {
            MiscSpeech::GetLocation(player_current[1].roadID, location_region, location);
        }
    }
    data.location_region = location_region;
    data.location = location;
    data.direction = last_known->direction;
    if (data.direction == 0) {
        data.direction = player_current[0].direction;
        if (data.direction == 0) {
            data.direction = player_current[1].direction;
            if (data.direction == 0) {
                dir = bRandom(4);
                data.direction = 1 << (dir & 0x3F);
            }
        }
    }
    if (location_region != Csis::Type_location_region_coastal) {
        if (location_region < 3) {
            if (location_region == Csis::Type_location_region_college_town) {
                data.address_group_type = Csis::Type_address_group_type_college_town;
            } else {
                data.address_group_type = Csis::Type_address_group_type_generic_any_;
            }
        } else if (location_region != Csis::Type_location_region_coastal_extra) {
            if (location_region == Csis::Type_location_region_city) {
                data.address_group_type = Csis::Type_address_group_type_city;
            } else {
                data.address_group_type = Csis::Type_address_group_type_generic_any_;
            }
        } else {
            data.address_group_type = Csis::Type_address_group_type_coastal;
        }
    } else {
        data.address_group_type = Csis::Type_address_group_type_coastal;
    }
    dir = bRandom(3);
    if (dir == 2) {
        data.address_group_type = Csis::Type_address_group_type_generic_any_;
    }
    ScheduleSpeech_AnytimeEvents_Disp911Report(data, Csis::AnytimeEvents_Disp911ReportId, Csis::gAnytimeEvents_Disp911ReportHandle, this);
}

void EAXDispatch::RBUpdate(EAXCop *, signed char true_false) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::StaticRoadblock_DispRBUpdateStruct data;

    data.speaker_id = mSpeakerID;
    data.code = GetRandomizedCode();
    data.roadblock_type = Csis::Type_roadblock_type_Roadblock_Generic_;
    if (*reinterpret_cast<signed char *>(reinterpret_cast<char *>(ai) + 0x164) > 1) {
        data.roadblock_type = Csis::Type_roadblock_type_Multiple_Roadblocks_disp_only_;
    }
    data.yes_no = Csis::Type_yes_no_No_False;
    if (true_false > 0) {
        data.yes_no = Csis::Type_yes_no_Yes_True;
    }
    ScheduleSpeech_StaticRoadblock_DispRBUpdate(data, Csis::StaticRoadblock_DispRBUpdateId, Csis::gStaticRoadblock_DispRBUpdateHandle, this);
}

void EAXDispatch::RBReply(EAXCop *cop, signed char true_false, unsigned int type) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::StaticRoadblock_DispRBReplyStruct data;

    data.speaker_id = mSpeakerID;
    data.code = GetRandomizedCode();
    data.roadblock_type = type;
    if (type == 0) {
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 4) == 0) {
            data.roadblock_type = Csis::Type_roadblock_type_Roadblock_Generic_;
            if (*reinterpret_cast<signed char *>(reinterpret_cast<char *>(ai) + 0x164) > 1) {
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
    ScheduleSpeech_StaticRoadblock_DispRBReply(data, Csis::StaticRoadblock_DispRBReplyId, Csis::gStaticRoadblock_DispRBReplyHandle, this);
}

void EAXDispatch::JurisShift(Csis::Type_jurisdiction jurisdiction) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_DispJurisShiftStruct data;

    if (ai) {
        data.speaker_id = mSpeakerID;
        data.jurisdiction = jurisdiction;
        ScheduleSpeech_AnytimeEvents_DispJurisShift(data, Csis::AnytimeEvents_DispJurisShiftId, Csis::gAnytimeEvents_DispJurisShiftHandle, this);
    }
}

void EAXDispatch::BackupETA() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    IPursuit *pursuit;
    float eta;
    Csis::Backup_DispBUETAStruct data;

    if (ai && (pursuit = *reinterpret_cast<IPursuit **>(reinterpret_cast<char *>(ai) + 0x138), pursuit)) {
        data.speaker_id = mSpeakerID;
        eta = pursuit->GetBackupETA();
        if ((eta <= 10.0f) || (eta >= 20.0f)) {
            if ((eta <= 20.0f) || (eta >= 40.0f)) {
                if ((eta <= 40.0f) || (eta >= 75.0f)) {
                    if ((eta <= 75.0f) || (eta >= 100.0f)) {
                        if ((eta <= 100.0f) || (eta >= 140.0f)) {
                            if (eta <= 140.0f) {
                                return;
                            }
                            data.disp_backup_eta = Csis::Type_disp_backup_eta_2min_;
                        } else {
                            data.disp_backup_eta = Csis::Type_disp_backup_eta_2min;
                        }
                    } else {
                        data.disp_backup_eta = Csis::Type_disp_backup_eta_1min30sec;
                    }
                } else {
                    data.disp_backup_eta = Csis::Type_disp_backup_eta_1min;
                }
            } else {
                data.disp_backup_eta = Csis::Type_disp_backup_eta_30sec;
            }
        } else {
            data.disp_backup_eta = Csis::Type_disp_backup_eta_15sec;
        }
        ScheduleSpeech_Backup_DispBUETA(data, Csis::Backup_DispBUETAId, Csis::gBackup_DispBUETAHandle, this);
    }
}

void EAXDispatch::VehicleDescription() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::Setup_DispVehDescripStruct data;
    SoundAI::CarCustomizations *custrec;

    if (ai) {
        data.speaker_id = mSpeakerID;
        data.car_color = 0;
        custrec = *reinterpret_cast<SoundAI::CarCustomizations **>(reinterpret_cast<char *>(ai) + 0x238);
        if (custrec) {
            data.car_color = *reinterpret_cast<int *>(custrec);
        }
        if (data.car_color != 0) {
            data.car_type = *reinterpret_cast<int *>(*reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x178) + 0x40);
            ScheduleSpeech_Setup_DispVehDescrip(data, Csis::Setup_DispVehDescripId, Csis::gSetup_DispVehDescripHandle, this);
        }
    }
}

void EAXDispatch::NoVehicleDescription() {
    Csis::Setup_DispNoVehDescripStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Setup_DispNoVehDescrip(data, Csis::Setup_DispNoVehDescripId, Csis::gSetup_DispNoVehDescripHandle, this);
}

void EAXDispatch::SubRBReply() {
    Csis::StaticRoadblock_DispSubRBStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_StaticRoadblock_DispSubRB(data, Csis::StaticRoadblock_DispSubRBId, Csis::gStaticRoadblock_DispSubRBHandle, this);
}
