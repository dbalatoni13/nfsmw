#include "EAXCop.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"

namespace Csis {
struct AnytimeEvents_Unit911ReplyStruct {
    int speaker_id;
};

struct Backup_UnitBUReplyStruct {
    int speaker_id;
};

struct Backup_NegativeBUReplyStruct {
    int speaker_id;
    int yes_no;
    int subject_battalion;
    int subject_call_sign_id;
};

struct Outcome_AnticipateSuccessStruct {
    int speaker_id;
};

struct Outcome_AnticipateFailStruct {
    int speaker_id;
};

struct Setup_BullhornPrefixStruct {
    int speaker_id;
};

struct ExtraCops_SwarmingReplyStruct {};

struct AnytimeEvents_PursuitUpdateRepStruct {
    int speaker_id;
};

struct AnytimeEvents_BailoutDenyStruct {
    int speaker_id;
};

struct StaticRoadblock_RBAvertedStruct {
    int speaker_id;
};

struct Backup_CallForSwarmingStruct {
    int speaker_id;
};

struct Setup_SpotterWantedStruct {
    int speaker_id;
};

struct AnytimeEvents_WeatherReportStruct {
    int speaker_id;
};

struct AnytimeEvents_SuspectBrakeStruct {
    int speaker_id;
};

struct Setup_SpotterStruct {
    int speaker_id;
    int num_suspects;
};

struct Setup_SpotterReplyStruct {
    int speaker_id;
    int num_suspects;
};

struct Setup_InitPursuitStruct {
    int speaker_id;
    int num_suspects;
};

struct AnytimeEvents_LostSuspectStruct {
    int speaker_id;
    int intensity;
};

struct Arrest_ArrestStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_LostVisualStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_RegainVisualStruct {
    int speaker_id;
    int intensity;
};

struct RollingStrategy_CallToPositionRemStruct {
    int speaker_id;
    int intensity;
};

struct RollingStrategy_StrategyExecuteStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_IntentToRamStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_SuspectBehaviourStruct {
    int speaker_id;
    int num_suspects;
};

struct Setup_SuspectConfirmedStruct {
    int speaker_id;
    int num_suspects;
};

struct AnytimeEvents_FocusChangeStruct {
    int speaker_id;
    int intensity;
};

struct StaticRoadblock_PursuitApproachingStruct {
    int speaker_id;
    int num_suspects;
};

extern InterfaceId AnytimeEvents_Unit911ReplyId;
extern InterfaceId Backup_UnitBUReplyId;
extern InterfaceId Backup_NegativeBUReplyId;
extern InterfaceId Outcome_AnticipateSuccessId;
extern InterfaceId Outcome_AnticipateFailId;
extern InterfaceId Setup_BullhornPrefixId;
extern InterfaceId ExtraCops_SwarmingReplyId;
extern InterfaceId AnytimeEvents_PursuitUpdateRepId;
extern InterfaceId AnytimeEvents_BailoutDenyId;
extern InterfaceId StaticRoadblock_RBAvertedId;
extern InterfaceId Backup_CallForSwarmingId;
extern InterfaceId Setup_SpotterWantedId;
extern InterfaceId AnytimeEvents_WeatherReportId;
extern InterfaceId AnytimeEvents_SuspectBrakeId;
extern InterfaceId Setup_SpotterId;
extern InterfaceId Setup_SpotterReplyId;
extern InterfaceId Setup_InitPursuitId;
extern InterfaceId AnytimeEvents_LostSuspectId;
extern InterfaceId Arrest_ArrestId;
extern InterfaceId AnytimeEvents_LostVisualId;
extern InterfaceId AnytimeEvents_RegainVisualId;
extern InterfaceId RollingStrategy_CallToPositionRemId;
extern InterfaceId RollingStrategy_StrategyExecuteId;
extern InterfaceId AnytimeEvents_IntentToRamId;
extern InterfaceId AnytimeEvents_SuspectBehaviourId;
extern InterfaceId Setup_SuspectConfirmedId;
extern InterfaceId AnytimeEvents_FocusChangeId;
extern InterfaceId StaticRoadblock_PursuitApproachingId;

extern FunctionHandle gAnytimeEvents_Unit911ReplyHandle;
extern FunctionHandle gBackup_UnitBUReplyHandle;
extern FunctionHandle gBackup_NegativeBUReplyHandle;
extern FunctionHandle gOutcome_AnticipateSuccessHandle;
extern FunctionHandle gOutcome_AnticipateFailHandle;
extern FunctionHandle gSetup_BullhornPrefixHandle;
extern FunctionHandle gExtraCops_SwarmingReplyHandle;
extern FunctionHandle gAnytimeEvents_PursuitUpdateRepHandle;
extern FunctionHandle gAnytimeEvents_BailoutDenyHandle;
extern FunctionHandle gStaticRoadblock_RBAvertedHandle;
extern FunctionHandle gBackup_CallForSwarmingHandle;
extern FunctionHandle gSetup_SpotterWantedHandle;
extern FunctionHandle gAnytimeEvents_WeatherReportHandle;
extern FunctionHandle gAnytimeEvents_SuspectBrakeHandle;
extern FunctionHandle gSetup_SpotterHandle;
extern FunctionHandle gSetup_SpotterReplyHandle;
extern FunctionHandle gSetup_InitPursuitHandle;
extern FunctionHandle gAnytimeEvents_LostSuspectHandle;
extern FunctionHandle gArrest_ArrestHandle;
extern FunctionHandle gAnytimeEvents_LostVisualHandle;
extern FunctionHandle gAnytimeEvents_RegainVisualHandle;
extern FunctionHandle gRollingStrategy_CallToPositionRemHandle;
extern FunctionHandle gRollingStrategy_StrategyExecuteHandle;
extern FunctionHandle gAnytimeEvents_IntentToRamHandle;
extern FunctionHandle gAnytimeEvents_SuspectBehaviourHandle;
extern FunctionHandle gSetup_SuspectConfirmedHandle;
extern FunctionHandle gAnytimeEvents_FocusChangeHandle;
extern FunctionHandle gStaticRoadblock_PursuitApproachingHandle;
}

extern void ScheduleSpeech_AnytimeEvents_Unit911Reply(Csis::AnytimeEvents_Unit911ReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32AnytimeEvents_Unit911ReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_UnitBUReply(Csis::Backup_UnitBUReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis24Backup_UnitBUReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_NegativeBUReply(Csis::Backup_NegativeBUReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Backup_NegativeBUReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Outcome_AnticipateSuccess(Csis::Outcome_AnticipateSuccessStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31Outcome_AnticipateSuccessStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Outcome_AnticipateFail(Csis::Outcome_AnticipateFailStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Outcome_AnticipateFailStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_BullhornPrefix(Csis::Setup_BullhornPrefixStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis26Setup_BullhornPrefixStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_ExtraCops_SwarmingReply(Csis::ExtraCops_SwarmingReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis29ExtraCops_SwarmingReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_PursuitUpdateRep(Csis::AnytimeEvents_PursuitUpdateRepStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis36AnytimeEvents_PursuitUpdateRepStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_BailoutDeny(Csis::AnytimeEvents_BailoutDenyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31AnytimeEvents_BailoutDenyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_RBAverted(Csis::StaticRoadblock_RBAvertedStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31StaticRoadblock_RBAvertedStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Backup_CallForSwarming(Csis::Backup_CallForSwarmingStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Backup_CallForSwarmingStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_SpotterWanted(Csis::Setup_SpotterWantedStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis25Setup_SpotterWantedStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_WeatherReport(Csis::AnytimeEvents_WeatherReportStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_WeatherReportStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_SuspectBrake(Csis::AnytimeEvents_SuspectBrakeStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32AnytimeEvents_SuspectBrakeStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_Spotter(Csis::Setup_SpotterStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis19Setup_SpotterStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_SpotterReply(Csis::Setup_SpotterReplyStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis24Setup_SpotterReplyStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_InitPursuit(Csis::Setup_InitPursuitStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis23Setup_InitPursuitStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_LostSuspect(Csis::AnytimeEvents_LostSuspectStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31AnytimeEvents_LostSuspectStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Arrest_Arrest(Csis::Arrest_ArrestStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis19Arrest_ArrestStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_LostVisual(Csis::AnytimeEvents_LostVisualStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis30AnytimeEvents_LostVisualStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_RegainVisual(Csis::AnytimeEvents_RegainVisualStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32AnytimeEvents_RegainVisualStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_RollingStrategy_CallToPositionRem(Csis::RollingStrategy_CallToPositionRemStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis39RollingStrategy_CallToPositionRemStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_RollingStrategy_StrategyExecute(Csis::RollingStrategy_StrategyExecuteStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis37RollingStrategy_StrategyExecuteStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_IntentToRam(Csis::AnytimeEvents_IntentToRamStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31AnytimeEvents_IntentToRamStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_SuspectBehaviour(Csis::AnytimeEvents_SuspectBehaviourStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis36AnytimeEvents_SuspectBehaviourStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Setup_SuspectConfirmed(Csis::Setup_SuspectConfirmedStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28Setup_SuspectConfirmedStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_FocusChange(Csis::AnytimeEvents_FocusChangeStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis31AnytimeEvents_FocusChangeStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_StaticRoadblock_PursuitApproaching(Csis::StaticRoadblock_PursuitApproachingStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis40StaticRoadblock_PursuitApproachingStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");

EAXCop::EAXCop(int speakerID, HSIMABLE handle, int bID, int cID)
    : EAXCharacter(speakerID, handle, bID, cID) {
    ISimable *simable;

    mPctTractiveTires = 0.0f;
    mRank = 0;
    *reinterpret_cast<unsigned int *>(&mInFormation) = 0;
    *reinterpret_cast<unsigned int *>(&mInPosition) = 0;
    *reinterpret_cast<unsigned int *>(&mAhead) = 0;
    mTimeAirborne = WorldTimer;
    mTrafficHitCount = 0;
    mTimeNoLOS = WorldTimer;
    mT_closingDist = 0;
    mCurrRoad = MAX_ROADNAMES;
    mTgtOffset.x = UMath::Vector3::kZero.x;
    mNumRammed = 0;
    mLastRammedTime = 0;
    mT_lastactivity = 0;
    mOrigin = MAX_ROADNAMES;
    mTgtOffset.z = UMath::Vector3::kZero.z;
    mTgtOffset.y = UMath::Vector3::kZero.y;
    simable = ISimable::FindInstance(handle);
    if (simable) {
        IVehicleAI *vai;
        if (simable->QueryInterface(&vai) && vai) {
            WRoadNav *nav = vai->GetDriveToNav();
            if (nav) {
                unsigned int road_id = nav->GetRoadSpeechId();
                if (road_id != 0x6B) {
                    mOrigin = static_cast<RoadNames>(road_id);
                }
            }
        }
    }
}

EAXCop::~EAXCop() {}

int EAXCop::GetBackupTypeFromDispatch(int type) {
    return type;
}

void EAXCop::Update() {
    EAXCharacter::Update();
}

void EAXCop::SetActive(bool activity) {
    EAXCharacter::SetActive(activity);
}

void EAXCop::Reset() {
    EAXCharacter::Reset();
    mInFormation = false;
    mInPosition = false;
    mAhead = false;
    mPctTractiveTires = 0.0f;
    mTrafficHitCount = 0;
    mNumRammed = 0;
}

void EAXCop::AttemptVehicleStop() {}

void EAXCop::VehicleReport() {}

void EAXCop::InitiatePursuit() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::Setup_InitPursuitStruct data;
        int num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech_Setup_InitPursuit(data, Csis::Setup_InitPursuitId, Csis::gSetup_InitPursuitHandle, this);
    }
}

void EAXCop::LocationReport() {}

void EAXCop::SelfStrategy(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::InitialCallForBackup() {}

void EAXCop::CallForBackup(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::UnitBackupReply() {
    Csis::Backup_UnitBUReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Backup_UnitBUReply(data, Csis::Backup_UnitBUReplyId, Csis::gBackup_UnitBUReplyHandle, this);
}

void EAXCop::InitiateStrategy(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::CallToPosition(EAXCop *cop) {
    if (cop) {
        mTgtOffset = cop->mTgtOffset;
    }
}

void EAXCop::CallToPositionReminder() {
    Csis::RollingStrategy_CallToPositionRemStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_RollingStrategy_CallToPositionRem(data, Csis::RollingStrategy_CallToPositionRemId, Csis::gRollingStrategy_CallToPositionRemHandle, this);
}

void EAXCop::StrategyExecute() {
    Csis::RollingStrategy_StrategyExecuteStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_RollingStrategy_StrategyExecute(data, Csis::RollingStrategy_StrategyExecuteId, Csis::gRollingStrategy_StrategyExecuteHandle, this);
}

void EAXCop::Collision(int collisionType, float force, EAXCop *spkr) {
    if (force > 0.0f || spkr) {
        mLastEvent = collisionType;
    }
}

void EAXCop::AnticipateSuccess() {
    Csis::Outcome_AnticipateSuccessStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Outcome_AnticipateSuccess(data, Csis::Outcome_AnticipateSuccessId, Csis::gOutcome_AnticipateSuccessHandle, this);
}

void EAXCop::AnticipateFail() {
    Csis::Outcome_AnticipateFailStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Outcome_AnticipateFail(data, Csis::Outcome_AnticipateFailId, Csis::gOutcome_AnticipateFailHandle, this);
}

void EAXCop::OutcomeFail(short intensity) {
    if (intensity) {
        mLastEvent = intensity;
    }
}

void EAXCop::StrategyReset(bool new_strategy) {
    if (new_strategy) {
        mLastEvent = 0;
    }
}

void EAXCop::SuspectBehavior() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::AnytimeEvents_SuspectBehaviourStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_AnytimeEvents_SuspectBehaviour(data, Csis::AnytimeEvents_SuspectBehaviourId, Csis::gAnytimeEvents_SuspectBehaviourHandle, this);
    }
}

void EAXCop::SuspectOutrun() {}

void EAXCop::SuspectUTurn() {}

void EAXCop::LostSuspect() {
    Csis::AnytimeEvents_LostSuspectStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_LostSuspect(data, Csis::AnytimeEvents_LostSuspectId, Csis::gAnytimeEvents_LostSuspectHandle, this);
}

void EAXCop::LostVisual() {
    Csis::AnytimeEvents_LostVisualStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_LostVisual(data, Csis::AnytimeEvents_LostVisualId, Csis::gAnytimeEvents_LostVisualHandle, this);
}

void EAXCop::RegainVisual() {
    Csis::AnytimeEvents_RegainVisualStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_RegainVisual(data, Csis::AnytimeEvents_RegainVisualId, Csis::gAnytimeEvents_RegainVisualHandle, this);
}

void EAXCop::SwarmingReply() {
    Csis::ExtraCops_SwarmingReplyStruct data;
    ScheduleSpeech_ExtraCops_SwarmingReply(data, Csis::ExtraCops_SwarmingReplyId, Csis::gExtraCops_SwarmingReplyHandle, this);
}

void EAXCop::Arrest() {
    Csis::Arrest_ArrestStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Arrest_Arrest(data, Csis::Arrest_ArrestId, Csis::gArrest_ArrestHandle, this);
}

void EAXCop::PursuitUpdateReply() {
    Csis::AnytimeEvents_PursuitUpdateRepStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_PursuitUpdateRep(data, Csis::AnytimeEvents_PursuitUpdateRepId, Csis::gAnytimeEvents_PursuitUpdateRepHandle, this);
}

void EAXCop::ReinitiatePursuit() {}

void EAXCop::NegativeBackupReply() {
    Csis::Backup_NegativeBUReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Backup_NegativeBUReply(data, Csis::Backup_NegativeBUReplyId, Csis::gBackup_NegativeBUReplyHandle, this);
}

void EAXCop::BackupReminder(int type) {
    if (type) {
        mLastEvent = type;
    }
}

void EAXCop::BackupArrives() {}

void EAXCop::IntentToRam() {
    Csis::AnytimeEvents_IntentToRamStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_IntentToRam(data, Csis::AnytimeEvents_IntentToRamId, Csis::gAnytimeEvents_IntentToRamHandle, this);
}

void EAXCop::CallforEV(unsigned int type) {
    if (type) {
        mLastEvent = static_cast<int>(type);
    }
}

void EAXCop::UnitDisabled(int other) {
    mLastEvent = other;
}

void EAXCop::Bailout() {}

void EAXCop::LoBailout() {}

void EAXCop::HiBailout() {}

void EAXCop::BailoutTraffic() {}

void EAXCop::BailoutBadRoad() {}

void EAXCop::Spotter() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::Setup_SpotterStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_Setup_Spotter(data, Csis::Setup_SpotterId, Csis::gSetup_SpotterHandle, this);
    }
}

void EAXCop::SpotterReply() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::Setup_SpotterReplyStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_Setup_SpotterReply(data, Csis::Setup_SpotterReplyId, Csis::gSetup_SpotterReplyHandle, this);
    }
}

void EAXCop::Reply911() {
    Csis::AnytimeEvents_Unit911ReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_Unit911Reply(data, Csis::AnytimeEvents_Unit911ReplyId, Csis::gAnytimeEvents_Unit911ReplyHandle, this);
}

void EAXCop::DenyBailout() {
    Csis::AnytimeEvents_BailoutDenyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_BailoutDeny(data, Csis::AnytimeEvents_BailoutDenyId, Csis::gAnytimeEvents_BailoutDenyHandle, this);
}

void EAXCop::PrimaryEngage() {}

void EAXCop::Bullhorn() {}

void EAXCop::PreBullhorn() {
    Csis::Setup_BullhornPrefixStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Setup_BullhornPrefix(data, Csis::Setup_BullhornPrefixId, Csis::gSetup_BullhornPrefixHandle, this);
}

void EAXCop::BullhornArrest() {}

void EAXCop::SuspectConfirmed() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::Setup_SuspectConfirmedStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_Setup_SuspectConfirmed(data, Csis::Setup_SuspectConfirmedId, Csis::gSetup_SuspectConfirmedHandle, this);
    }
}

void EAXCop::FocusChange() {
    Csis::AnytimeEvents_FocusChangeStruct data;
    int intensity;
    data.speaker_id = mSpeakerID;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    ScheduleSpeech_AnytimeEvents_FocusChange(data, Csis::AnytimeEvents_FocusChangeId, Csis::gAnytimeEvents_FocusChangeHandle, this);
}

void EAXCop::Spotted() {}

void EAXCop::DirectionChange() {}

void EAXCop::CallForSwarming() {
    Csis::Backup_CallForSwarmingStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Backup_CallForSwarming(data, Csis::Backup_CallForSwarmingId, Csis::gBackup_CallForSwarmingHandle, this);
}

void EAXCop::SpotterWanted() {
    Csis::Setup_SpotterWantedStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Setup_SpotterWanted(data, Csis::Setup_SpotterWantedId, Csis::gSetup_SpotterWantedHandle, this);
}

void EAXCop::Offroad(unsigned int id, bool subsequent) {
    if (subsequent) {
        mLastEvent = static_cast<int>(id);
    }
}

void EAXCop::WeatherReport() {
    Csis::AnytimeEvents_WeatherReportStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_WeatherReport(data, Csis::AnytimeEvents_WeatherReportId, Csis::gAnytimeEvents_WeatherReportHandle, this);
}

void EAXCop::CallForRB() {}

void EAXCop::RBReminder() {}

void EAXCop::NegRBReply() {}

void EAXCop::RBApproach() {}

void EAXCop::RBEngage(bool spikes_hit) {
    if (spikes_hit) {
        mLastEvent = 1;
    }
}

void EAXCop::PursuitApproaching() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::StaticRoadblock_PursuitApproachingStruct data;
        int num_suspects;
        data.speaker_id = mSpeakerID;
        num_suspects = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        ScheduleSpeech_StaticRoadblock_PursuitApproaching(data, Csis::StaticRoadblock_PursuitApproachingId, Csis::gStaticRoadblock_PursuitApproachingHandle, this);
    }
}

void EAXCop::RBAverted() {
    Csis::StaticRoadblock_RBAvertedStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_StaticRoadblock_RBAverted(data, Csis::StaticRoadblock_RBAvertedId, Csis::gStaticRoadblock_RBAvertedHandle, this);
}

void EAXCop::CallForSubRB() {}

void EAXCop::RearEnded(Csis::Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::HeadOn(Csis::Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SideSwiped(Csis::Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::TBoned(Csis::Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SuspectRollover(Csis::Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SuspectAirborne(Csis::Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SuspectSpunout(Csis::Type_intensity intensity) {
    mLastEvent = intensity;
}

void EAXCop::SuspectBrake() {
    Csis::AnytimeEvents_SuspectBrakeStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_AnytimeEvents_SuspectBrake(data, Csis::AnytimeEvents_SuspectBrakeId, Csis::gAnytimeEvents_SuspectBrakeHandle, this);
}

void EAXCop::SwapVoices(EAXCop *cop) {
    if (cop) {
        int callsign = mCallsign.name;
        int unit = mCallsign.number;
        mCallsign.name = cop->mCallsign.name;
        mCallsign.number = cop->mCallsign.number;
        cop->mCallsign.name = callsign;
        cop->mCallsign.number = unit;
    }
}

bool EAXCop::IsPrimary() {
    return mRank == 0;
}

bool EAXCop::SetRank(int newrank) {
    mRank = newrank;
    return true;
}

void EAXCop::Impact_Suspect_World() {}

void EAXCop::Impact_Suspect_Semi() {}

void EAXCop::Impact_Suspect_Train() {}

void EAXCop::Impact_Suspect_Guardrail() {}

void EAXCop::Impact_Suspect_GasStation() {}

void EAXCop::Impact_Suspect_Spikebelt() {}

void EAXCop::Impact_Suspect_Traffic(Csis::Type_intensity intensity) {
    mLastEvent = intensity;
}
