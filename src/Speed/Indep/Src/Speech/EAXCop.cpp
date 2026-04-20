#include "EAXCop.h"
#include "ScheduleSpeech.hpp"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"

namespace MiscSpeech {
bool IsVehicleTypeOK();
bool GetLocation(RoadNames road, int &region, int &location) asm("GetLocation__10MiscSpeech9RoadNamesRQ24Csis20Type_location_regionRQ24Csis13Type_location");
}

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

struct AnytimeEvents_BailoutStruct {
    int speaker_id;
    int bailout_type;
};

struct AnytimeEvents_CallForEVStruct {
    int speaker_id;
    int ev_type;
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

struct Setup_LocationReportStruct {
    int speaker_id;
    int num_suspects;
    int direction;
    int encounter;
    int location_region;
    int location;
};

struct Setup_ReInitPursuitStruct {
    int speaker_id;
    int time_since_lost;
    int num_suspects;
};

struct Setup_InitialCallForBU_MSStruct {
    int speaker_id;
};

struct Setup_InitialCallForBUStruct {
    int speaker_id;
    int code;
};

struct Setup_VehicleReportStruct {
    int speaker_id;
    int car_color;
    int car_type;
    int speed;
    int measurement;
};

struct Setup_AttmptVehStpStruct {
    int speaker_id;
    int pursuit_type;
    int num_suspects;
    int speaker_battalion;
    int speaker_call_sign_id;
};

struct Setup_SelfStrategyStruct {
    int speaker_id;
    int code;
    int self_strategy_type;
};

struct RollingStrategy_InitStrategyStruct {
    int speaker_id;
    int code;
    int rolling_strategy_type;
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

struct StaticRoadblock_CallForRB_subStruct {
    int speaker_id;
};

struct AnytimeEvents_DirectionHighStruct {
    int speaker_id;
    int direction_type;
};

struct Setup_BullhornStruct {
    int speaker_id;
};

struct AnytimeEvents_SuspectOutrunStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_SuspectUTurnStruct {
    int speaker_id;
    int intensity;
};

struct StaticRoadblock_NegativeRBReplyStruct {
    int speaker_id;
};

struct StaticRoadblock_RBReminderStruct {
    int speaker_id;
    int code_type;
};

struct StaticRoadblock_CallForRBStruct {
    int speaker_id;
    int code_type;
    int roadblock_type;
};

struct Setup_PrimaryEngageStruct {
    int speaker_id;
    int callsign;
    int unit_number;
};

struct Backup_BUArrivesStruct {
    int speaker_id;
    int callsign;
    int unit_number;
};

struct AnytimeEvents_SpottedStruct {
    int speaker_id;
    int intensity;
};

struct Interrupts_InterruptRam_HOStruct {
    int speaker_id;
    int intensity;
};

struct Interrupts_InterruptRam_REStruct {
    int speaker_id;
    int intensity;
};

struct Interrupts_InterruptRam_SSStruct {
    int speaker_id;
    int intensity;
};

struct Interrupts_InterruptRam_TBStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_CollWorld_FlipStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_CollWorld_AirStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_CollWorld_SpinStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_CollWorld_CiviStruct {
    int speaker_id;
    int intensity;
};

struct Arrest_BullhornArrestStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_OffroadMomentStruct {
    int speaker_id;
    int offroad_id;
    int offroad_type;
};

struct AnytimeEvents_UnitDisabledStruct {
    int speaker_id;
    int intensity;
    int disabled_type;
};

struct Outcome_OutcomeFailStruct {
    int speaker_id;
    int intensity;
};

struct Backup_BUReminderStruct {
    int speaker_id;
    int code_type;
    int backup_type;
};

struct Backup_CallForBUStruct {
    int speaker_id;
    int code_type;
    int backup_type;
};

struct Outcome_StrategyResetStruct {
    int speaker_id;
    int strategy_type;
    int intensity;
};

struct StaticRoadblock_RBEngageStruct {
    int speaker_id;
    int engage_type;
};

struct StaticRoadblock_RBApproachStruct {
    int speaker_id;
    int approach_type;
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
extern InterfaceId AnytimeEvents_BailoutId;
extern InterfaceId AnytimeEvents_SuspectBrakeId;
extern InterfaceId Setup_SpotterId;
extern InterfaceId Setup_SpotterReplyId;
extern InterfaceId Setup_InitPursuitId;
extern InterfaceId Setup_LocationReportId;
extern InterfaceId Setup_ReInitPursuitId;
extern InterfaceId Setup_InitialCallForBU_MSId;
extern InterfaceId Setup_InitialCallForBUId;
extern InterfaceId Setup_VehicleReportId;
extern InterfaceId Setup_AttmptVehStpId;
extern InterfaceId Setup_SelfStrategyId;
extern InterfaceId AnytimeEvents_LostSuspectId;
extern InterfaceId Arrest_ArrestId;
extern InterfaceId AnytimeEvents_LostVisualId;
extern InterfaceId AnytimeEvents_RegainVisualId;
extern InterfaceId RollingStrategy_CallToPositionRemId;
extern InterfaceId RollingStrategy_StrategyExecuteId;
extern InterfaceId RollingStrategy_InitStrategyId;
extern InterfaceId AnytimeEvents_IntentToRamId;
extern InterfaceId AnytimeEvents_SuspectBehaviourId;
extern InterfaceId Setup_SuspectConfirmedId;
extern InterfaceId AnytimeEvents_FocusChangeId;
extern InterfaceId StaticRoadblock_PursuitApproachingId;
extern InterfaceId StaticRoadblock_CallForRB_subId;
extern InterfaceId AnytimeEvents_DirectionHighId;
extern InterfaceId Setup_BullhornId;
extern InterfaceId AnytimeEvents_SuspectOutrunId;
extern InterfaceId AnytimeEvents_SuspectUTurnId;
extern InterfaceId AnytimeEvents_CallForEVId;
extern InterfaceId StaticRoadblock_NegativeRBReplyId;
extern InterfaceId StaticRoadblock_RBReminderId;
extern InterfaceId StaticRoadblock_CallForRBId;
extern InterfaceId Setup_PrimaryEngageId;
extern InterfaceId Backup_BUArrivesId;
extern InterfaceId AnytimeEvents_SpottedId;
extern InterfaceId Interrupts_InterruptRam_HOId;
extern InterfaceId Interrupts_InterruptRam_REId;
extern InterfaceId Interrupts_InterruptRam_SSId;
extern InterfaceId Interrupts_InterruptRam_TBId;
extern InterfaceId AnytimeEvents_CollWorld_FlipId;
extern InterfaceId AnytimeEvents_CollWorld_AirId;
extern InterfaceId AnytimeEvents_CollWorld_SpinId;
extern InterfaceId AnytimeEvents_CollWorld_CiviId;
extern InterfaceId Arrest_BullhornArrestId;
extern InterfaceId AnytimeEvents_OffroadMomentId;
extern InterfaceId AnytimeEvents_UnitDisabledId;
extern InterfaceId Outcome_OutcomeFailId;
extern InterfaceId Backup_BUReminderId;
extern InterfaceId Backup_CallForBUId;
extern InterfaceId Outcome_StrategyResetId;
extern InterfaceId StaticRoadblock_RBEngageId;
extern InterfaceId StaticRoadblock_RBApproachId;

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
extern FunctionHandle gAnytimeEvents_BailoutHandle;
extern FunctionHandle gAnytimeEvents_SuspectBrakeHandle;
extern FunctionHandle gSetup_SpotterHandle;
extern FunctionHandle gSetup_SpotterReplyHandle;
extern FunctionHandle gSetup_InitPursuitHandle;
extern FunctionHandle gSetup_LocationReportHandle;
extern FunctionHandle gSetup_ReInitPursuitHandle;
extern FunctionHandle gSetup_InitialCallForBU_MSHandle;
extern FunctionHandle gSetup_InitialCallForBUHandle;
extern FunctionHandle gSetup_VehicleReportHandle;
extern FunctionHandle gSetup_AttmptVehStpHandle;
extern FunctionHandle gSetup_SelfStrategyHandle;
extern FunctionHandle gAnytimeEvents_LostSuspectHandle;
extern FunctionHandle gArrest_ArrestHandle;
extern FunctionHandle gAnytimeEvents_LostVisualHandle;
extern FunctionHandle gAnytimeEvents_RegainVisualHandle;
extern FunctionHandle gRollingStrategy_CallToPositionRemHandle;
extern FunctionHandle gRollingStrategy_StrategyExecuteHandle;
extern FunctionHandle gRollingStrategy_InitStrategyHandle;
extern FunctionHandle gAnytimeEvents_IntentToRamHandle;
extern FunctionHandle gAnytimeEvents_SuspectBehaviourHandle;
extern FunctionHandle gSetup_SuspectConfirmedHandle;
extern FunctionHandle gAnytimeEvents_FocusChangeHandle;
extern FunctionHandle gStaticRoadblock_PursuitApproachingHandle;
extern FunctionHandle gStaticRoadblock_CallForRB_subHandle;
extern FunctionHandle gAnytimeEvents_DirectionHighHandle;
extern FunctionHandle gSetup_BullhornHandle;
extern FunctionHandle gAnytimeEvents_SuspectOutrunHandle;
extern FunctionHandle gAnytimeEvents_SuspectUTurnHandle;
extern FunctionHandle gAnytimeEvents_CallForEVHandle;
extern FunctionHandle gStaticRoadblock_NegativeRBReplyHandle;
extern FunctionHandle gStaticRoadblock_RBReminderHandle;
extern FunctionHandle gStaticRoadblock_CallForRBHandle;
extern FunctionHandle gSetup_PrimaryEngageHandle;
extern FunctionHandle gBackup_BUArrivesHandle;
extern FunctionHandle gAnytimeEvents_SpottedHandle;
extern FunctionHandle gInterrupts_InterruptRam_HOHandle;
extern FunctionHandle gInterrupts_InterruptRam_REHandle;
extern FunctionHandle gInterrupts_InterruptRam_SSHandle;
extern FunctionHandle gInterrupts_InterruptRam_TBHandle;
extern FunctionHandle gAnytimeEvents_CollWorld_FlipHandle;
extern FunctionHandle gAnytimeEvents_CollWorld_AirHandle;
extern FunctionHandle gAnytimeEvents_CollWorld_SpinHandle;
extern FunctionHandle gAnytimeEvents_CollWorld_CiviHandle;
extern FunctionHandle gArrest_BullhornArrestHandle;
extern FunctionHandle gAnytimeEvents_OffroadMomentHandle;
extern FunctionHandle gAnytimeEvents_UnitDisabledHandle;
extern FunctionHandle gOutcome_OutcomeFailHandle;
extern FunctionHandle gBackup_BUReminderHandle;
extern FunctionHandle gBackup_CallForBUHandle;
extern FunctionHandle gOutcome_StrategyResetHandle;
extern FunctionHandle gStaticRoadblock_RBEngageHandle;
extern FunctionHandle gStaticRoadblock_RBApproachHandle;
}

extern void FlushSpeechForActor(EAXCharacter *actor) asm("FlushSpeechForActor__Q26Speech7ManagerP12EAXCharacter");
extern int GetCount_EventHistory(void *history, int event_id) asm("GetCount__Q26Speech12EventHistory18SPCHType_1_EventID");
extern unsigned char gSpeechManagerGlobalHistory[] asm("_Q26Speech7Manager.mGlobalHistory");
extern "C" float lbl_804074DC;
extern "C" float lbl_804074E0;
extern "C" float lbl_804074D0;
extern "C" float lbl_804074D4;
extern "C" float lbl_804074D8;
extern "C" float lbl_80407484;
extern "C" float lbl_8040746C;
extern "C" float lbl_80407470;
extern "C" float lbl_80407468;
extern "C" float lbl_8040745C;
extern "C" float lbl_80407490;
extern "C" float lbl_80407494;
extern "C" float lbl_80407498;
extern "C" float speed_test_28362[] asm("speed_test.28362");

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

EAXCop::~EAXCop() {
    FlushSpeechForActor(this);
}

int EAXCop::GetBackupTypeFromDispatch(int type) {
    switch (type) {
    case 8:
        return 2;
    case 0x10:
        return 4;
    case 1:
    case 2:
    case 4:
    case 0x20:
    case 0x40:
        return 1;
    default:
        return 0;
    }
}

void EAXCop::Update() {
    EAXCharacter::Update();
    HSIMABLE handle = (*reinterpret_cast<HSIMABLE (**)(void *)>(*reinterpret_cast<char **>(this) + 0x5C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x58));
    if (!handle) {
        *reinterpret_cast<unsigned int *>(&mInPosition) = 0;
        *reinterpret_cast<unsigned int *>(&mInFormation) = 0;
        mPctTractiveTires = lbl_8040745C;
        mTimeAirborne = WorldTimer;
        EAXCharacter::Reset();
        return;
    }

    HSIMABLE find_handle = (*reinterpret_cast<HSIMABLE (**)(void *)>(*reinterpret_cast<char **>(this) + 0x5C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x58));
    ISimable *simable;
    if (find_handle) {
        simable = ISimable::FindInstance(find_handle);
    } else {
        simable = 0;
    }
    IDamageable *damageable = 0;
    IVehicle *vehicle = 0;
    ISuspension *suspension = 0;
    IPursuitAI *pursuit_ai = 0;
    IVehicleAI *vehicle_ai = 0;
    UMath::Vector3 cop_pos;
    UMath::Vector3 player_pos;
    UMath::Vector3 delta;

    if (*reinterpret_cast<unsigned int *>(&mSuspectLOS) != 0) {
        mTimeNoLOS = WorldTimer;
    }

    if (simable) {
        simable->QueryInterface(&vehicle);
        simable->QueryInterface(&damageable);
        simable->QueryInterface(&pursuit_ai);
        simable->QueryInterface(&suspension);
        simable->QueryInterface(&vehicle_ai);
    } else {
        (*reinterpret_cast<void (**)(void *, bool)>(*reinterpret_cast<char **>(this) + 0xD4))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0xD0), false);
    }

    if (pursuit_ai && (*reinterpret_cast<unsigned int *>(&mActive) != 0)) {
        (*reinterpret_cast<void (**)(void *, bool)>(*reinterpret_cast<char **>(this) + 0x35C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x358), pursuit_ai->GetInFormation());
        (*reinterpret_cast<void (**)(void *, bool)>(*reinterpret_cast<char **>(this) + 0x36C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x368), pursuit_ai->GetInPosition());
        (*reinterpret_cast<void (**)(void *, const UMath::Vector3 &)>(*reinterpret_cast<char **>(this) + 0x37C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x378), pursuit_ai->GetPursuitOffset());
    } else {
        (*reinterpret_cast<void (**)(void *, bool)>(*reinterpret_cast<char **>(this) + 0x35C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x358), false);
        (*reinterpret_cast<void (**)(void *, bool)>(*reinterpret_cast<char **>(this) + 0x36C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x368), false);
        (*reinterpret_cast<void (**)(void *, const UMath::Vector3 &)>(*reinterpret_cast<char **>(this) + 0x37C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x378), UMath::Vector3::kZero);
    }

    if (suspension) {
        if (*reinterpret_cast<unsigned int *>(&mActive) == 0) {
            mTimeAirborne = WorldTimer;
            mPctTractiveTires = lbl_8040745C;
        } else {
            mPctTractiveTires = lbl_8040745C;
            int num_wheels_on_ground = (*reinterpret_cast<int (**)(void *)>(*reinterpret_cast<char **>(suspension) + 0x1C))(reinterpret_cast<char *>(suspension) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(suspension) + 0x18));
            if (num_wheels_on_ground != 0) {
                unsigned int wheel_traction = (*reinterpret_cast<unsigned int (**)(void *)>(*reinterpret_cast<char **>(suspension) + 0xA4))(reinterpret_cast<char *>(suspension) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(suspension) + 0xA0));
                unsigned int num_wheels = (*reinterpret_cast<unsigned int (**)(void *)>(*reinterpret_cast<char **>(suspension) + 0x1C))(reinterpret_cast<char *>(suspension) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(suspension) + 0x18));
                wheel_traction = wheel_traction / num_wheels;
                mPctTractiveTires = static_cast<float>(wheel_traction);
            }
            if (mPctTractiveTires > lbl_80407468) {
                mTimeAirborne = WorldTimer;
            }
        }
    } else {
        mPctTractiveTires = lbl_8040745C;
        mTimeAirborne = WorldTimer;
    }

    if (damageable) {
        *reinterpret_cast<unsigned int *>(&mDestroyed) = damageable->IsDestroyed();
        mHealth = damageable->GetHealth();
    }

    if (vehicle_ai) {
        if (*reinterpret_cast<unsigned int *>(&mActive) == 0) {
            mCurrRoad = MAX_ROADNAMES;
        } else {
            WRoadNav *nav = (*reinterpret_cast<WRoadNav *(**)(void *)>(*reinterpret_cast<char **>(vehicle_ai) + 0x9C))(reinterpret_cast<char *>(vehicle_ai) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(vehicle_ai) + 0x98));
            RoadNames road = static_cast<RoadNames>(nav->GetRoadSpeechId());
            if (road != MAX_ROADNAMES) {
                mCurrRoad = road;
            }
        }
    } else {
        mCurrRoad = MAX_ROADNAMES;
    }

    if ((*reinterpret_cast<unsigned int *>(&mActive) != 0) && (*reinterpret_cast<unsigned int *>(&mSuspectLOS) != 0) && vehicle) {
        SoundAI *ai = SoundAI::Get();
        if (!ai) {
            return;
        }
        const UMath::Vector3 &v_pos = vehicle->GetPosition();
        *reinterpret_cast<unsigned int *>(&cop_pos.x) = *reinterpret_cast<const unsigned int *>(&v_pos.x);
        *reinterpret_cast<unsigned int *>(&cop_pos.y) = *reinterpret_cast<const unsigned int *>(&v_pos.y);
        *reinterpret_cast<unsigned int *>(&cop_pos.z) = *reinterpret_cast<const unsigned int *>(&v_pos.z);
        *reinterpret_cast<unsigned int *>(&player_pos.x) = *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x114);
        *reinterpret_cast<unsigned int *>(&player_pos.y) = *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x118);
        *reinterpret_cast<unsigned int *>(&player_pos.z) = *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x11C);
        VU0_v3sub(cop_pos, player_pos, delta);
        float dist = VU0_sqrt(VU0_v3lengthsquare(delta));
        if (dist <= mDistance) {
            mT_closingDist = WorldTimer;
        }
    }
}

void EAXCop::SetActive(bool activity) {
    unsigned int active = static_cast<unsigned int>(activity);
    if (*reinterpret_cast<unsigned int *>(&mActive) == active) {
        return;
    }

    *reinterpret_cast<unsigned int *>(&mActive) = active;
    SoundAI *ai = SoundAI::Get();
    if (!ai) {
        return;
    }

    if (active != 0) {
        int focus = *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x140);
        if (focus != 0x29A && focus != 2) {
            return;
        }

        if (*reinterpret_cast<unsigned int *>(&mSuspectLOS) != 0) {
            if (bRandom(lbl_8040746C) > lbl_80407470) {
                (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x28C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x288));
            } else {
                (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x1BC))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x1B8));
            }
            return;
        } else {
            if (focus != 2) {
                return;
            }
            if (*reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x1DC) == 2) {
                return;
            }

            IRoadBlock *roadblock = ai->GetRoadblock();
            if (!roadblock) {
                return;
            }

            HSIMABLE handle = (*reinterpret_cast<HSIMABLE (**)(void *)>(*reinterpret_cast<char **>(this) + 0x5C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x58));
            int includes = (*reinterpret_cast<int (**)(void *, HSIMABLE)>(*reinterpret_cast<char **>(roadblock) + 0xBC))(reinterpret_cast<char *>(roadblock) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(roadblock) + 0xB8), handle);
            if (includes != 0) {
                return;
            }

            if (bRandom(lbl_8040746C) > lbl_80407470) {
                (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x144))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x140));
            } else {
                (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x1F4))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x1F0));
            }
            return;
        }
    }

    if ((WorldTimer - mT_lastactivity).GetSeconds() < lbl_80407484) {
        return;
    }

    FlushSpeechForActor(this);
    if ((*reinterpret_cast<unsigned int *>(&mInFormation) == 0) && (*reinterpret_cast<unsigned int *>(&mInPosition) == 0)) {
        return;
    }

    mT_lastactivity = WorldTimer;

    if (mTrafficHitCount > 1) {
        (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x22C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x228));
        mTrafficHitCount = activity;
        int state = *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x1DC);
        if (state == 0 || state == 1) {
            ai->RandomBailoutDeny(this);
        }
        return;
    }

    if ((*reinterpret_cast<int (**)(void *)>(*reinterpret_cast<char **>(this) + 0x34C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x348))) {
        (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x214))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x210));
        return;
    }

    if (*reinterpret_cast<unsigned int *>(&mDestroyed) != 0) {
        EAXCop *rand_cop = ai->FindClosestCop(true, true);
        bool different_battalion = false;
        if (rand_cop) {
            int their_battalion = (*reinterpret_cast<int (**)(void *)>(*reinterpret_cast<char **>(rand_cop) + 0x6C))(reinterpret_cast<char *>(rand_cop) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(rand_cop) + 0x68));
            int my_battalion = (*reinterpret_cast<int (**)(void *)>(*reinterpret_cast<char **>(this) + 0x6C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x68));
            different_battalion = their_battalion != my_battalion;
        }
        (*reinterpret_cast<void (**)(void *, bool)>(*reinterpret_cast<char **>(this) + 0x20C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x208), different_battalion);
        return;
    }

    float min_health = *reinterpret_cast<float *>(*reinterpret_cast<char **>(reinterpret_cast<char *>(ai) + 0x18C) + 0x9C);
    if (mHealth < min_health) {
        (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x224))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x220));
        return;
    }

    (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x21C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x218));
    int state = *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x1DC);
    if (state == 0 || state == 1) {
        ai->RandomBailoutDeny(this);
    }
}

void EAXCop::Reset() {
    mInFormation = false;
    mInPosition = false;
    mPctTractiveTires = 0.0f;
    *reinterpret_cast<unsigned int *>(&mTimeAirborne) = 0;
    mTrafficHitCount = 0;
    *reinterpret_cast<unsigned int *>(&mTimeNoLOS) = 0;
    mNumRammed = 0;
    *reinterpret_cast<unsigned int *>(&mLastRammedTime) = 0;
    *reinterpret_cast<unsigned int *>(&mT_lastactivity) = 0;
    *reinterpret_cast<unsigned int *>(&mT_closingDist) = 0;
    EAXCharacter::Reset();
}

void EAXCop::AttemptVehicleStop() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        int pursuit_type = 0x10;
        float t_last_nailed = ai->GetTimeLastNailedCop();
        if (t_last_nailed >= lbl_80407490) {
            int infraction = *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x1F0);
            if (infraction > 0) {
                switch (infraction) {
                case 1:
                case 2:
                    if (ai->IsHighIntensity()) {
                        pursuit_type = 2;
                    } else {
                        pursuit_type = 1;
                    }
                    break;
                case 4:
                case 0x80:
                    pursuit_type = 8;
                    break;
                case 8:
                    pursuit_type = 0x10;
                    break;
                case 0x10:
                case 0x20:
                    pursuit_type = 4;
                    break;
                case 0x40:
                    pursuit_type = 2;
                    break;
                default:
                    break;
                }
            } else {
                if (ai->IsHighIntensity()) {
                    pursuit_type = 2;
                } else if (bRandom(lbl_80407494) > lbl_80407498) {
                    pursuit_type = 4;
                    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x154) > 0) {
                        pursuit_type = 4;
                    } else if (*reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x158) > 0x3E8) {
                        pursuit_type = 8;
                    } else if (GetCount_EventHistory(gSpeechManagerGlobalHistory, 0x9A) > 0 || GetCount_EventHistory(gSpeechManagerGlobalHistory, 0x3A) > 0 || GetCount_EventHistory(gSpeechManagerGlobalHistory, 0xB2) > 0) {
                        pursuit_type = 2;
                    } else {
                        pursuit_type = 1;
                    }
                } else if (GetCount_EventHistory(gSpeechManagerGlobalHistory, 0x9A) > 0 || GetCount_EventHistory(gSpeechManagerGlobalHistory, 0x3A) > 0 || GetCount_EventHistory(gSpeechManagerGlobalHistory, 0xB2) > 0) {
                    pursuit_type = 2;
                } else {
                    pursuit_type = 1;
                }
            }
        }

        Csis::Setup_AttmptVehStpStruct data;
        int num_suspects = 1;
        data.pursuit_type = pursuit_type;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            num_suspects = 2;
        }
        data.num_suspects = num_suspects;
        data.speaker_battalion = GetCallsign();
        data.speaker_call_sign_id = GetUnitNumber();
        data.speaker_id = mSpeakerID;
        ScheduleSpeech(data, Csis::Setup_AttmptVehStpId, Csis::gSetup_AttmptVehStpHandle, this);
    }
}

void EAXCop::VehicleReport() {
    int ndx = 0;
    if (FEDatabase) {
        SoundAI *ai = SoundAI::Get();
        if (ai) {
            int color = 0;
            void *custom = *reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x238);
            if (custom) {
                color = *reinterpret_cast<int *>(custom);
            }
            if (color && MiscSpeech::IsVehicleTypeOK()) {
                Csis::Setup_VehicleReportStruct data;
                float speedo;
                data.speaker_id = mSpeakerID;
                data.car_color = color;
                data.car_type = *reinterpret_cast<int *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x178)) + 0x40);
                speedo = *reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x110);
                if (*reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(FEDatabase) + 0x20)) + 0x44) == 1) {
                    data.measurement = 4;
                    speedo = speedo * lbl_804074D0;
                } else {
                    data.measurement = 2;
                }
                while (ndx <= 10 && speed_test_28362[ndx] <= speedo) {
                    ndx++;
                }
                if (ndx == 0) {
                    data.speed = 1;
                    data.measurement = 1;
                } else {
                    if (bRandom(lbl_804074D4) > lbl_804074D8) {
                        data.measurement = 1;
                    }
                    data.speed = 2 << (ndx - 1);
                }
                ScheduleSpeech(data, Csis::Setup_VehicleReportId, Csis::gSetup_VehicleReportHandle, this);
            }
        }
    }
}

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
        ScheduleSpeech(data, Csis::Setup_InitPursuitId, Csis::gSetup_InitPursuitHandle, this);
    }
}

void EAXCop::LocationReport() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (ai->IsHeadingValid()) {
            Csis::Setup_LocationReportStruct data;
            int num_suspects;
            int region;
            int location;
            bool result;
            num_suspects = 1;
            if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
                num_suspects = 2;
            }
            data.num_suspects = num_suspects;
            data.direction = *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x1AC);
            result = MiscSpeech::GetLocation(*reinterpret_cast<RoadNames *>(reinterpret_cast<char *>(ai) + 0x1B0), region, location);
            if (result) {
                int encounter = 1;
                data.location = location;
                data.location_region = region;
                if (GetCount_EventHistory(gSpeechManagerGlobalHistory, 0x9E) > 1) {
                    encounter = 2;
                }
                data.encounter = encounter;
                data.speaker_id = mSpeakerID;
                ScheduleSpeech(data, Csis::Setup_LocationReportId, Csis::gSetup_LocationReportHandle, this);
            }
        }
    }
}

void EAXCop::SelfStrategy(int type) {
    SoundAI *ai = SoundAI::Get();
    if (ai && ai->GetPursuitState() == SoundAI::kActive) {
        Csis::Setup_SelfStrategyStruct data;
        switch (type) {
        case 1: {
            unsigned int select = bRandom(4);
            if (select == 1) {
                data.self_strategy_type = 0x40;
            } else if (static_cast<int>(select) < 2) {
                if (select == 0) {
                    data.self_strategy_type = 0x20;
                } else {
                    data.self_strategy_type = 8;
                }
            } else if (select == 2) {
                data.self_strategy_type = 0x10;
            } else {
                data.self_strategy_type = 8;
            }
            break;
        }
        case 2:
        case 3:
        case 6:
            data.self_strategy_type = 4;
            if (bRandom(lbl_804074DC) > lbl_804074E0) {
                data.self_strategy_type = 2;
            }
            break;
        default:
            return;
        }
        data.code = GetRandomizedCode();
        data.speaker_id = mSpeakerID;
        ScheduleSpeech(data, Csis::Setup_SelfStrategyId, Csis::gSetup_SelfStrategyHandle, this);
    }
}

void EAXCop::InitialCallForBackup() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 0x800) != 0) {
            Csis::Setup_InitialCallForBU_MSStruct data;
            data.speaker_id = mSpeakerID;
            ScheduleSpeech(data, Csis::Setup_InitialCallForBU_MSId, Csis::gSetup_InitialCallForBU_MSHandle, this);
        } else {
            Csis::Setup_InitialCallForBUStruct data;
            data.code = GetRandomizedCode();
            data.speaker_id = mSpeakerID;
            ScheduleSpeech(data, Csis::Setup_InitialCallForBUId, Csis::gSetup_InitialCallForBUHandle, this);
        }
    }
}

void EAXCop::CallForBackup(int type) {
    Csis::Backup_CallForBUStruct data;
    data.code_type = GetRandomizedCode();
    data.backup_type = GetBackupTypeFromDispatch(type);
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Backup_CallForBUId, Csis::gBackup_CallForBUHandle, this);
}

void EAXCop::UnitBackupReply() {
    Csis::Backup_UnitBUReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Backup_UnitBUReplyId, Csis::gBackup_UnitBUReplyHandle, this);
}

void EAXCop::InitiateStrategy(int type) {
    Csis::RollingStrategy_InitStrategyStruct init;
    init.code = GetRandomizedCode();
    switch (type) {
    case 1:
        init.rolling_strategy_type = 0x20;
        break;
    case 3:
        init.rolling_strategy_type = 2;
        break;
    case 2:
        init.rolling_strategy_type = 4;
        break;
    case 6:
        init.rolling_strategy_type = 0x10;
        break;
    default:
        return;
    }
    init.speaker_id = mSpeakerID;
    ScheduleSpeech(init, Csis::RollingStrategy_InitStrategyId, Csis::gRollingStrategy_InitStrategyHandle, this);
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
    ScheduleSpeech(data, Csis::RollingStrategy_CallToPositionRemId, Csis::gRollingStrategy_CallToPositionRemHandle, this);
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
    ScheduleSpeech(data, Csis::RollingStrategy_StrategyExecuteId, Csis::gRollingStrategy_StrategyExecuteHandle, this);
}

void EAXCop::Collision(int collisionType, float force, EAXCop *spkr) {
    int intensity = 1;
    if (force > 0.75f) {
        intensity = 2;
    }
    switch (collisionType) {
    case 5:
        Impact_Suspect_World();
        break;
    case 7:
        Impact_Suspect_Traffic(static_cast<Csis::Type_intensity>(intensity));
        break;
    case 10:
        Impact_Suspect_Guardrail();
        break;
    case 11:
        Impact_Suspect_Train();
        break;
    case 12:
        Impact_Suspect_Semi();
        break;
    case 13:
        Impact_Suspect_GasStation();
        break;
    case 14:
        Impact_Suspect_Spikebelt();
        break;
    case 15:
        SuspectSpunout(static_cast<Csis::Type_intensity>(intensity));
        break;
    case 16:
        SuspectAirborne(static_cast<Csis::Type_intensity>(intensity));
        break;
    case 17:
        SuspectRollover(static_cast<Csis::Type_intensity>(intensity));
        break;
    default:
        break;
    }
}

void EAXCop::AnticipateSuccess() {
    Csis::Outcome_AnticipateSuccessStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Outcome_AnticipateSuccessId, Csis::gOutcome_AnticipateSuccessHandle, this);
}

void EAXCop::AnticipateFail() {
    Csis::Outcome_AnticipateFailStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Outcome_AnticipateFailId, Csis::gOutcome_AnticipateFailHandle, this);
}

void EAXCop::OutcomeFail(short intensity) {
    Csis::Outcome_OutcomeFailStruct data;
    SoundAI *ai = SoundAI::Get();
    if (intensity < 0) {
        register int new_intensity;
        if (ai->IsHighIntensity()) {
            new_intensity = 2;
        } else {
            new_intensity = 1;
        }
        data.intensity = new_intensity;
    } else {
        data.intensity = intensity;
    }
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Outcome_OutcomeFailId, Csis::gOutcome_OutcomeFailHandle, this);
}

void EAXCop::StrategyReset(bool new_strategy) {
    Csis::Outcome_StrategyResetStruct data;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    if (new_strategy) {
        data.strategy_type = 2;
    } else {
        data.strategy_type = 1;
    }
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Outcome_StrategyResetId, Csis::gOutcome_StrategyResetHandle, this);
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
        ScheduleSpeech(data, Csis::AnytimeEvents_SuspectBehaviourId, Csis::gAnytimeEvents_SuspectBehaviourHandle, this);
    }
}

void EAXCop::SuspectOutrun() {
    Csis::AnytimeEvents_SuspectOutrunStruct data;
    data.speaker_id = mSpeakerID;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::AnytimeEvents_SuspectOutrunId, Csis::gAnytimeEvents_SuspectOutrunHandle, this);
}

void EAXCop::SuspectUTurn() {
    Csis::AnytimeEvents_SuspectUTurnStruct data;
    data.speaker_id = mSpeakerID;
    register int intensity;
    if (SoundAI::Get()->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::AnytimeEvents_SuspectUTurnId, Csis::gAnytimeEvents_SuspectUTurnHandle, this);
}

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
    ScheduleSpeech(data, Csis::AnytimeEvents_LostSuspectId, Csis::gAnytimeEvents_LostSuspectHandle, this);
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
    ScheduleSpeech(data, Csis::AnytimeEvents_LostVisualId, Csis::gAnytimeEvents_LostVisualHandle, this);
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
    ScheduleSpeech(data, Csis::AnytimeEvents_RegainVisualId, Csis::gAnytimeEvents_RegainVisualHandle, this);
}

void EAXCop::SwarmingReply() {
    Csis::ExtraCops_SwarmingReplyStruct data;
    ScheduleSpeech(data, Csis::ExtraCops_SwarmingReplyId, Csis::gExtraCops_SwarmingReplyHandle, this);
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
    ScheduleSpeech(data, Csis::Arrest_ArrestId, Csis::gArrest_ArrestHandle, this);
}

void EAXCop::PursuitUpdateReply() {
    Csis::AnytimeEvents_PursuitUpdateRepStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::AnytimeEvents_PursuitUpdateRepId, Csis::gAnytimeEvents_PursuitUpdateRepHandle, this);
}

extern "C" float lbl_8040749C;
extern "C" float lbl_804074A0;

void EAXCop::ReinitiatePursuit() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x16C) < *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x7C)) {
            float speed = (*reinterpret_cast<float (**)(void *)>(*reinterpret_cast<char **>(this) + 0x3D4))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x3D0));
            if (speed != lbl_8040749C) {
                (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x28C))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x288));
            } else {
                (*reinterpret_cast<void (**)(void *)>(*reinterpret_cast<char **>(this) + 0x1BC))(reinterpret_cast<char *>(this) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(this) + 0x1B8));
            }
        } else {
            Csis::Setup_ReInitPursuitStruct data;
            int time_since_lost;
            int num_suspects;
            if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x16C) < lbl_804074A0) {
                time_since_lost = 1;
            } else {
                time_since_lost = 2;
            }
            data.time_since_lost = time_since_lost;
            unsigned int flags = *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C);
            num_suspects = 1;
            if ((flags & 0x800) != 0) {
                num_suspects = 2;
            }
            data.num_suspects = num_suspects;
            data.speaker_id = mSpeakerID;
            ScheduleSpeech(data, Csis::Setup_ReInitPursuitId, Csis::gSetup_ReInitPursuitHandle, this);
        }
    }
}

void EAXCop::NegativeBackupReply() {
    Csis::Backup_NegativeBUReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Backup_NegativeBUReplyId, Csis::gBackup_NegativeBUReplyHandle, this);
}

void EAXCop::BackupReminder(int type) {
    Csis::Backup_BUReminderStruct data;
    data.code_type = GetRandomizedCode();
    data.backup_type = GetBackupTypeFromDispatch(type);
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Backup_BUReminderId, Csis::gBackup_BUReminderHandle, this);
}

void EAXCop::BackupArrives() {
    Csis::Backup_BUArrivesStruct data;
    data.speaker_id = mSpeakerID;
    data.callsign = GetCallsign();
    data.unit_number = GetUnitNumber();
    ScheduleSpeech(data, Csis::Backup_BUArrivesId, Csis::gBackup_BUArrivesHandle, this);
}

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
    ScheduleSpeech(data, Csis::AnytimeEvents_IntentToRamId, Csis::gAnytimeEvents_IntentToRamHandle, this);
}

void EAXCop::CallforEV(unsigned int type) {
    SoundAI *ai = SoundAI::Get();
    if (ai && ai->GetPursuitState() == SoundAI::kActive && *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x140) != 1) {
        Csis::AnytimeEvents_CallForEVStruct data;
        data.ev_type = type;
        if (type == 0) {
            if (IsHeli()) {
                data.ev_type = 4;
            } else {
                IPursuit *pursuit = *reinterpret_cast<IPursuit **>(reinterpret_cast<char *>(ai) + 0x138);
                int destroyed = (*reinterpret_cast<int (**)(void *)>(*reinterpret_cast<char **>(reinterpret_cast<char *>(pursuit) + 4) + 0x13C))(reinterpret_cast<char *>(pursuit) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(reinterpret_cast<char *>(pursuit) + 4) + 0x138));
                if (destroyed < 2) {
                    return;
                }
                data.ev_type = 1;
            }
        }
        data.speaker_id = mSpeakerID;
        ScheduleSpeech(data, Csis::AnytimeEvents_CallForEVId, Csis::gAnytimeEvents_CallForEVHandle, this);
    }
}

void EAXCop::UnitDisabled(int other) {
    Csis::AnytimeEvents_UnitDisabledStruct data;
    SoundAI *ai = SoundAI::Get();
    int disabled_type = 1;
    int intensity;
    if (other) {
        disabled_type = 2;
    }
    data.disabled_type = disabled_type;
    if (ai->IsHighIntensity()) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::AnytimeEvents_UnitDisabledId, Csis::gAnytimeEvents_UnitDisabledHandle, this);
}

void EAXCop::Bailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 8;
            ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::LoBailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 1;
            ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::HiBailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 0x10;
            ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::BailoutTraffic() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 2;
            ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

void EAXCop::BailoutBadRoad() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (*reinterpret_cast<float *>(reinterpret_cast<char *>(ai) + 0x148) >= *reinterpret_cast<float *>(reinterpret_cast<char *>(*reinterpret_cast<void **>(reinterpret_cast<char *>(ai) + 0x18C)) + 0x58)) {
            Csis::AnytimeEvents_BailoutStruct data;
            data.speaker_id = mSpeakerID;
            data.bailout_type = 4;
            ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
        }
    }
}

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
        ScheduleSpeech(data, Csis::Setup_SpotterId, Csis::gSetup_SpotterHandle, this);
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
        ScheduleSpeech(data, Csis::Setup_SpotterReplyId, Csis::gSetup_SpotterReplyHandle, this);
    }
}

void EAXCop::Reply911() {
    Csis::AnytimeEvents_Unit911ReplyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::AnytimeEvents_Unit911ReplyId, Csis::gAnytimeEvents_Unit911ReplyHandle, this);
}

void EAXCop::DenyBailout() {
    Csis::AnytimeEvents_BailoutDenyStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::AnytimeEvents_BailoutDenyId, Csis::gAnytimeEvents_BailoutDenyHandle, this);
}

void EAXCop::PrimaryEngage() {
    Csis::Setup_PrimaryEngageStruct data;
    data.speaker_id = mSpeakerID;
    data.callsign = GetCallsign();
    data.unit_number = GetUnitNumber();
    ScheduleSpeech(data, Csis::Setup_PrimaryEngageId, Csis::gSetup_PrimaryEngageHandle, this);
}

void EAXCop::Bullhorn() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        Csis::Setup_BullhornStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech(data, Csis::Setup_BullhornId, Csis::gSetup_BullhornHandle, this);
    }
}

void EAXCop::PreBullhorn() {
    Csis::Setup_BullhornPrefixStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Setup_BullhornPrefixId, Csis::gSetup_BullhornPrefixHandle, this);
}

void EAXCop::BullhornArrest() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        SoundAI *ai = SoundAI::Get();
        if (ai) {
            Csis::Arrest_BullhornArrestStruct data;
            data.speaker_id = mSpeakerID;
            register int intensity;
            if (ai->IsHighIntensity()) {
                intensity = 2;
            } else {
                intensity = 1;
            }
            data.intensity = intensity;
            ScheduleSpeech(data, Csis::Arrest_BullhornArrestId, Csis::gArrest_BullhornArrestHandle, this);
        }
    }
}

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
        ScheduleSpeech(data, Csis::Setup_SuspectConfirmedId, Csis::gSetup_SuspectConfirmedHandle, this);
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
    ScheduleSpeech(data, Csis::AnytimeEvents_FocusChangeId, Csis::gAnytimeEvents_FocusChangeHandle, this);
}

extern "C" float lbl_804074E8;

void EAXCop::Spotted() {
    Csis::AnytimeEvents_SpottedStruct data;
    data.speaker_id = mSpeakerID;
    int intensity;
    if (GetSpeed() > lbl_804074E8) {
        intensity = 2;
    } else {
        intensity = 1;
    }
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::AnytimeEvents_SpottedId, Csis::gAnytimeEvents_SpottedHandle, this);
}

void EAXCop::DirectionChange() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        int direction_type = *reinterpret_cast<int *>(reinterpret_cast<char *>(ai) + 0x1AC);
        if (direction_type) {
            Csis::AnytimeEvents_DirectionHighStruct data;
            data.speaker_id = mSpeakerID;
            data.direction_type = direction_type;
            ScheduleSpeech(data, Csis::AnytimeEvents_DirectionHighId, Csis::gAnytimeEvents_DirectionHighHandle, this);
        }
    }
}

void EAXCop::CallForSwarming() {
    Csis::Backup_CallForSwarmingStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Backup_CallForSwarmingId, Csis::gBackup_CallForSwarmingHandle, this);
}

void EAXCop::SpotterWanted() {
    Csis::Setup_SpotterWantedStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Setup_SpotterWantedId, Csis::gSetup_SpotterWantedHandle, this);
}

void EAXCop::Offroad(unsigned int id, bool subsequent) {
    Csis::AnytimeEvents_OffroadMomentStruct data;
    data.offroad_id = static_cast<int>(id);
    data.speaker_id = mSpeakerID;
    data.offroad_type = subsequent ? 2 : 1;
    ScheduleSpeech(data, Csis::AnytimeEvents_OffroadMomentId, Csis::gAnytimeEvents_OffroadMomentHandle, this);
}

void EAXCop::WeatherReport() {
    Csis::AnytimeEvents_WeatherReportStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::AnytimeEvents_WeatherReportId, Csis::gAnytimeEvents_WeatherReportHandle, this);
}

void EAXCop::CallForRB() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        Csis::StaticRoadblock_CallForRBStruct data;
        int roadblock_type;
        data.speaker_id = mSpeakerID;
        data.code_type = GetRandomizedCode();
        roadblock_type = 1;
        if ((*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(ai) + 0x5C) & 4) != 0) {
            roadblock_type = 2;
        }
        data.roadblock_type = roadblock_type;
        ScheduleSpeech(data, Csis::StaticRoadblock_CallForRBId, Csis::gStaticRoadblock_CallForRBHandle, this);
    }
}

void EAXCop::RBReminder() {
    Csis::StaticRoadblock_RBReminderStruct data;
    data.speaker_id = mSpeakerID;
    data.code_type = GetRandomizedCode();
    ScheduleSpeech(data, Csis::StaticRoadblock_RBReminderId, Csis::gStaticRoadblock_RBReminderHandle, this);
}

void EAXCop::NegRBReply() {
    if (!IsHeli()) {
        Csis::StaticRoadblock_NegativeRBReplyStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech(data, Csis::StaticRoadblock_NegativeRBReplyId, Csis::gStaticRoadblock_NegativeRBReplyHandle, this);
    }
}

void EAXCop::RBApproach() {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        IRoadBlock *roadblock = ai->GetRoadblock();
        if (roadblock) {
            Csis::StaticRoadblock_RBApproachStruct data;
            int roadblock_type;
            data.speaker_id = mSpeakerID;
            roadblock_type = 1;
            if ((*reinterpret_cast<int (**)(void *)>(*reinterpret_cast<char **>(reinterpret_cast<char *>(roadblock) + 4) + 0x9C))(reinterpret_cast<char *>(roadblock) + *reinterpret_cast<short *>(*reinterpret_cast<char **>(reinterpret_cast<char *>(roadblock) + 4) + 0x98)) > 0) {
                roadblock_type = 2;
            }
            data.approach_type = roadblock_type;
            ScheduleSpeech(data, Csis::StaticRoadblock_RBApproachId, Csis::gStaticRoadblock_RBApproachHandle, this);
        }
    }
}

void EAXCop::RBEngage(bool spikes_hit) {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        if (ai->GetRoadblock()) {
            Csis::StaticRoadblock_RBEngageStruct data;
            int engage_type;
            data.speaker_id = mSpeakerID;
            engage_type = 1;
            if (spikes_hit) {
                engage_type = 2;
            }
            data.engage_type = engage_type;
            ScheduleSpeech(data, Csis::StaticRoadblock_RBEngageId, Csis::gStaticRoadblock_RBEngageHandle, this);
        }
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
        ScheduleSpeech(data, Csis::StaticRoadblock_PursuitApproachingId, Csis::gStaticRoadblock_PursuitApproachingHandle, this);
    }
}

void EAXCop::RBAverted() {
    Csis::StaticRoadblock_RBAvertedStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::StaticRoadblock_RBAvertedId, Csis::gStaticRoadblock_RBAvertedHandle, this);
}

void EAXCop::CallForSubRB() {
    if (SoundAI::Get()) {
        Csis::StaticRoadblock_CallForRB_subStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech(data, Csis::StaticRoadblock_CallForRB_subId, Csis::gStaticRoadblock_CallForRB_subHandle, this);
    }
}

void EAXCop::RearEnded(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_REStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::Interrupts_InterruptRam_REId, Csis::gInterrupts_InterruptRam_REHandle, this);
}

void EAXCop::HeadOn(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_HOStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::Interrupts_InterruptRam_HOId, Csis::gInterrupts_InterruptRam_HOHandle, this);
}

void EAXCop::SideSwiped(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_SSStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::Interrupts_InterruptRam_SSId, Csis::gInterrupts_InterruptRam_SSHandle, this);
}

void EAXCop::TBoned(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_TBStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::Interrupts_InterruptRam_TBId, Csis::gInterrupts_InterruptRam_TBHandle, this);
}

void EAXCop::SuspectRollover(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_FlipStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::AnytimeEvents_CollWorld_FlipId, Csis::gAnytimeEvents_CollWorld_FlipHandle, this);
}

void EAXCop::SuspectAirborne(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_AirStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::AnytimeEvents_CollWorld_AirId, Csis::gAnytimeEvents_CollWorld_AirHandle, this);
}

void EAXCop::SuspectSpunout(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_SpinStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::AnytimeEvents_CollWorld_SpinId, Csis::gAnytimeEvents_CollWorld_SpinHandle, this);
}

void EAXCop::SuspectBrake() {
    Csis::AnytimeEvents_SuspectBrakeStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::AnytimeEvents_SuspectBrakeId, Csis::gAnytimeEvents_SuspectBrakeHandle, this);
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
    int speaker_id = mSpeakerID;
    if ((speaker_id - 2U <= 3) || (speaker_id == 9)) {
        return true;
    }
    return false;
}

bool EAXCop::SetRank(int newrank) {
    bool did_reset = false;
    if (newrank == 0 && mRank > 0) {
        Csis::Setup_PrimaryEngageStruct data;
        data.speaker_id = mSpeakerID;
        ScheduleSpeech(data, Csis::Setup_PrimaryEngageId, Csis::gSetup_PrimaryEngageHandle, this);
        Reset();
        did_reset = true;
    }
    mRank = newrank;
    return did_reset;
}

void EAXCop::Impact_Suspect_World() {}

void EAXCop::Impact_Suspect_Semi() {}

void EAXCop::Impact_Suspect_Train() {}

void EAXCop::Impact_Suspect_Guardrail() {}

void EAXCop::Impact_Suspect_GasStation() {}

void EAXCop::Impact_Suspect_Spikebelt() {}

void EAXCop::Impact_Suspect_Traffic(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_CiviStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    ScheduleSpeech(data, Csis::AnytimeEvents_CollWorld_CiviId, Csis::gAnytimeEvents_CollWorld_CiviHandle, this);
}
