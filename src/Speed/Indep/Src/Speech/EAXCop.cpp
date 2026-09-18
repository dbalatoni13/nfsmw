#include "EAXCop.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Speech/MiscSpeech.h"
#include "Speed/Indep/Src/Speech/Observer.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

EAXCop::EAXCop(int speakerID, HSIMABLE handle, int bID, int cID)
    : EAXCharacter(speakerID, handle, bID, cID), mRank(0), mInFormation(false), mInPosition(false), mAhead(false), mPctTractiveTires(0.0f),
      mTimeAirborne(WorldTimer), mTrafficHitCount(0), mTimeNoLOS(WorldTimer), mNumRammed(0), mLastRammedTime(0), mT_lastactivity(0),
      mT_closingDist(0), mOrigin(static_cast<RoadNames>(0x6B)), mCurrRoad(static_cast<RoadNames>(0x6B)), mTgtOffset(UMath::Vector3::kZero) {
    ISimable *simable = ISimable::FindInstance(handle);
    if (simable != nullptr) {
        IVehicleAI *ivai;
        if (simable->QueryInterface(&ivai)) {
            WRoadNav *nav = ivai->GetDriveToNav();
            if (nav != nullptr) {
                if (nav->GetRoadSpeechId() != 0x6B) {
                    mOrigin = static_cast<RoadNames>(nav->GetRoadSpeechId());
                }
            }
        }
    }
}

EAXCop::~EAXCop() {
    Speech::Manager::FlushSpeechForActor(this);
}

void EAXCop::Update() {
    EAXCharacter::Update();
    if (GetHandle() == nullptr) {
        mInFormation = false;
        mInPosition = false;
        mPctTractiveTires = 1.0f;
        mTimeAirborne = WorldTimer;
        EAXCharacter::Reset();
        return;
    }
    ISimable *simable = ISimable::FindInstance(GetHandle());
    IDamageable *damage = nullptr;
    IVehicle *copcar = nullptr;
    ISuspension *suspension = nullptr;
    IPursuitAI *pursuitAI = nullptr;
    IVehicleAI *vai = nullptr;
    if (mSuspectLOS) {
        mTimeNoLOS = WorldTimer;
    }
    if (simable != nullptr) {
        simable->QueryInterface(&copcar);
        simable->QueryInterface(&damage);
        simable->QueryInterface(&pursuitAI);
        simable->QueryInterface(&suspension);
        simable->QueryInterface(&vai);
    } else {
        SetActive(false);
    }
    if (pursuitAI != nullptr && mActive) {
        SetInFormation(pursuitAI->GetInFormation());
        SetInPosition(pursuitAI->GetInPosition());
        SetTgtOffset(pursuitAI->GetPursuitOffset());
    } else {
        SetInFormation(false);
        SetInPosition(false);
        SetTgtOffset(UMath::Vector3::kZero);
    }
    if (suspension != nullptr && mActive) {
        mPctTractiveTires = suspension->GetNumWheels() != 0 ? suspension->GetNumWheelsOnGround() / suspension->GetNumWheels() : 1.0f;
        if (mPctTractiveTires > 0.25f) {
            mTimeAirborne = WorldTimer;
        }
    } else {
        mTimeAirborne = WorldTimer;
        mPctTractiveTires = 1.0f;
    }
    if (damage != nullptr) {
        mDestroyed = damage->IsDestroyed();
        mHealth = damage->GetHealth();
    }
    if (vai != nullptr && mActive) {
        WRoadNav *nav = vai->GetDriveToNav();
        unsigned int roadID = nav->GetRoadSpeechId();
        if (roadID != 0x6b) {
            mCurrRoad = static_cast<RoadNames>(roadID);
        }
    } else {
        mCurrRoad = static_cast<RoadNames>(0x6b);
    }
    if (mActive && mSuspectLOS && copcar != nullptr) {
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if (ai != nullptr) {
            UMath::Vector3 coppos = copcar->GetPosition();
            UMath::Vector3 ppos = ai->GetPlayerPos();
            float dist = UMath::Distance(coppos, ppos);
            if (dist <= mDistance) {
                mT_closingDist = WorldTimer;
            }
        }
    }
}

bool EAXCop::SetRank(int newrank) {
    bool rval = false;
    if (newrank == 0 && mRank > 0) {
        Csis::Setup_PrimaryEngageStruct data;
        data.speaker_id = mSpeakerID;
        SCHEDULE_SPEECH(Setup_PrimaryEngage, data, this);
        Reset();
        rval = true;
    }
    mRank = newrank;
    return rval;
}

void EAXCop::SwapVoices(EAXCop *cop) {
    int callsign = cop->GetCallsign();
    int unitnum = cop->GetUnitNumber();
    int speaker = cop->GetSpeakerID();

    cop->SetSpeakerID(mSpeakerID);
    cop->SetCallsign(mCallsign.name);
    cop->SetUnitNumber(mCallsign.number);

    mSpeakerID = speaker;
    mCallsign.name = callsign;
    mCallsign.number = unitnum;
}

void EAXCop::SetActive(bool activity) {
    if (mActive != activity) {
        mActive = activity;
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if (ai != nullptr) {
            if (activity) {
                if (ai->GetFocus() == 666 || ai->GetFocus() == 2) {
                    if (mSuspectLOS) {
                        if (bRandom(1.0f) > 0.5f) {
                            Spotted();
                        } else {
                            RegainVisual();
                        }
                    } else if (ai->GetFocus() == 2 && ai->GetPursuitState() != SoundAI::kInactive) {
                        IRoadBlock *block = ai->GetRoadblock();
                        if (block != nullptr && block->IsComprisedOf(GetHandle()) == nullptr) {
                            if (bRandom(1.0f) > 0.5f) {
                                UnitBackupReply();
                            } else {
                                BackupArrives();
                            }
                        }
                    }
                }
            } else {
                float t_lastactivity = (WorldTimer - mT_lastactivity).GetSeconds();
                if (t_lastactivity >= 2.0f) {
                    int num_events_flushed = Speech::Manager::FlushSpeechForActor(this);
                    if (mInFormation || mInPosition) {
                        mT_lastactivity = WorldTimer;
                        if (mTrafficHitCount > 1) {
                            BailoutTraffic();
                            mTrafficHitCount = 0;
                            if (ai->GetPursuitState() == SoundAI::kActive || ai->GetPursuitState() == SoundAI::kSearching) {
                                ai->RandomBailoutDeny(this);
                            }
                        } else if (IsHeli()) {
                            Bailout();
                        } else if (mDestroyed) {
                            EAXCop *rand_cop = ai->FindClosestCop(true, true);
                            if (rand_cop != nullptr) {
                                rand_cop->UnitDisabled(rand_cop->GetSpeakerID() == GetSpeakerID() ? 0 : 1);
                            } else {
                                UnitDisabled(0);
                            }
                        } else if (mHealth < ai->GetTune().MinHealthForCommentary()) {
                            HiBailout();
                        } else {
                            LoBailout();
                            if (ai->GetPursuitState() == SoundAI::kActive || ai->GetPursuitState() == SoundAI::kSearching) {
                                ai->RandomBailoutDeny(this);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool EAXCop::IsPrimary() {
    int speaker = mSpeakerID;
    if (static_cast<unsigned int>(speaker - 2) <= 3 || speaker == 9) {
        return true;
    }
    return false;
}

void EAXCop::Reset() {
    mInFormation = false;
    mInPosition = false;
    mPctTractiveTires = 1.0f;
    mTimeAirborne = 0;
    mTrafficHitCount = 0;
    mTimeNoLOS = 0;
    mNumRammed = 0;
    mLastRammedTime = 0;
    mT_lastactivity = 0;
    mT_closingDist = 0;
    EAXCharacter::Reset();
}

void EAXCop::Collision(int collisionType, float force, EAXCop *spkr) {
    Csis::Type_intensity intensity = Csis::Type_intensity_Normal;
    if (force > 0.75f) {
        intensity = Csis::Type_intensity_High;
    }
    switch (collisionType) {
        // El arbol del objetivo pivota en 0xb (nosotros en 0xc) y su subarbol
        // izquierdo pivota en 4: son DOS nodos vacios, un RANGO contiguo (0..2,
        // que resta 2 al indice del pivote) mas el `case 4` suelto (que resta 1).
        // 95,02% -> 100% (384 B). Un solo `case 4` deja el pivote en 0xc y el
        // rango 0..4 se funde con el nodo 5.
        case 0:
        case 1:
        case 2:
            break;
        case 4:
            break;
        case 5:
            Impact_Suspect_World();
            break;
        case 7:
            Impact_Suspect_Traffic(intensity);
            break;
        case 12:
            Impact_Suspect_Semi();
            break;
        case 10:
            Impact_Suspect_Guardrail();
            break;
        case 11:
            Impact_Suspect_Train();
            break;
        case 13:
            Impact_Suspect_GasStation();
            break;
        case 14:
            Impact_Suspect_Spikebelt();
            break;
        case 16:
            SuspectAirborne(intensity);
            break;
        case 17:
            SuspectRollover(intensity);
            break;
        case 15:
            SuspectSpunout(intensity);
            break;
    }
}

void EAXCop::AttemptVehicleStop() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::Setup_AttmptVehStpStruct data;
    Speech::EventHistory &g_hist = Speech::Manager::GetHistory();
    if (ai->GetTimeLastNailedCop() < 5.0f) {
        data.pursuit_type = Csis::Type_pursuit_type_Unit_Rammed;
    } else if (ai->GetLastInfraction() > 0) {
        switch (ai->GetLastInfraction()) {
            case 1:
            case 2:
                if (ai->IsHighIntensity()) {
                    data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
                } else {
                    data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
                }
                break;
            case 4:
            case 128:
                data.pursuit_type = Csis::Type_pursuit_type_Reckless;
                break;
            case 8:
                data.pursuit_type = Csis::Type_pursuit_type_Unit_Rammed;
                break;
            case 16:
            case 32:
                data.pursuit_type = Csis::Type_pursuit_type_Hit_and_Run;
                break;
            case 64:
                data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
                break;
        }
    } else {
        if (ai->IsHighIntensity()) {
            data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
        } else if (bRandom(1.0f) > 0.5f) {
            if (ai->NumTrafficHits() > 0) {
                data.pursuit_type = Csis::Type_pursuit_type_Hit_and_Run;
            } else if (ai->GetHavoc() > 1000) {
                data.pursuit_type = Csis::Type_pursuit_type_Reckless;
            } else if (g_hist.GetCount(kSPCH1_EventID_AttmptVehStp) > 0 || g_hist.GetCount(kSPCH1_EventID_Spotter) > 0 ||
                       g_hist.GetCount(kSPCH1_EventID_HeliSpotter) > 0) {
                data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
            } else {
                data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
            }
        } else if (g_hist.GetCount(kSPCH1_EventID_AttmptVehStp) > 0 || g_hist.GetCount(kSPCH1_EventID_Spotter) > 0 ||
                   g_hist.GetCount(kSPCH1_EventID_HeliSpotter) > 0) {
            data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
        } else {
            data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
        }
    }
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    data.speaker_battalion = static_cast<Csis::Type_speaker_battalion>(GetCallsign());
    data.speaker_call_sign_id = static_cast<Csis::Type_speaker_call_sign_id>(GetUnitNumber());
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Setup_AttmptVehStp, data, this);
}

void EAXCop::Spotter() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::Setup_SpotterStruct data;
        data.speaker_id = mSpeakerID;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        SCHEDULE_SPEECH(Setup_Spotter, data, this);
    }
}

void EAXCop::SpotterReply() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::Setup_SpotterReplyStruct data;
        data.speaker_id = mSpeakerID;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        SCHEDULE_SPEECH(Setup_SpotterReply, data, this);
    }
}

void EAXCop::Reply911() {
    Csis::AnytimeEvents_Unit911ReplyStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_Unit911Reply, data, this);
}

void EAXCop::ReinitiatePursuit() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    if (ai->GetTimeSinceLastChase() < ai->GetTune().TimeConsideredLostNoLOS()) {
        if (IsAhead() != 0.0f) {
            Spotted();
        } else {
            RegainVisual();
        }
        return;
    }
    Csis::Setup_ReInitPursuitStruct data;
    if (ai->GetTimeSinceLastChase() < 30.0f) {
        data.time_since_lost = Csis::Type_time_since_lost_lost_recently;
    } else {
        data.time_since_lost = Csis::Type_time_since_lost_lost_for_X_time;
    }
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Setup_ReInitPursuit, data, this);
}

void EAXCop::VehicleReport() {
    static const float speed_test[11] = {100.0f, 120.0f, 140.0f, 160.0f, 180.0f, 200.0f, 220.0f, 240.0f, 260.0f, 280.0f, 300.0f};
    unsigned int color;
    int ndx = 0;
    if (FEDatabase == nullptr) {
        return;
    }
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    color = ai->GetPlayerCarColor();
    if (color == 0) {
        return;
    }
    if (!MiscSpeech::IsVehicleTypeOK()) {
        return;
    }
    const Attrib::Gen::pvehicle &pcar = ai->GetPlayerSpecs();
    Csis::Setup_VehicleReportStruct data;
    data.speaker_id = mSpeakerID;
    data.car_type = pcar.VerbalType();
    data.car_color = static_cast<Csis::Type_car_color>(color);
    float speedo = ai->GetPlayerSpeed();
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speedo = speedo * 1.60931f;
        data.measurement = Csis::Type_measurement_metric_only;
    } else {
        data.measurement = Csis::Type_measurement_imperial_only;
    }
    while (ndx < 11 && speedo >= speed_test[ndx]) {
        ++ndx;
    }
    if (ndx == 0) {
        data.speed = Csis::Type_speed_over_speed_limit;
        data.measurement = Csis::Type_measurement_generic;
    } else {
        if (bRandom(1.0f) > 0.5f) {
            data.measurement = Csis::Type_measurement_generic;
        }
        data.speed = static_cast<Csis::Type_speed>(Csis::Type_speed_speeds_in_excess_of_100 << (ndx - 1));
    }
    SCHEDULE_SPEECH(Setup_VehicleReport, data, this);
}

void EAXCop::InitiatePursuit() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::Setup_InitPursuitStruct data;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        data.speaker_id = mSpeakerID;
        SCHEDULE_SPEECH(Setup_InitPursuit, data, this);
    }
}

void EAXCop::LocationReport() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    if (!ai->IsHeadingValid()) {
        return;
    }
    Csis::Setup_LocationReportStruct data;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    data.direction = static_cast<Csis::Type_direction>(ai->GetPlayerDirection(0));
    Csis::Type_location_region region;
    Csis::Type_location location;
    if (!MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location)) {
        return;
    }
    data.location_region = region;
    data.location = location;
    data.encounter = static_cast<Csis::Type_encounter>(Speech::Manager::GetHistory().GetCount(static_cast<SPCHType_1_EventID>(0x9E)) > 1
                                                            ? 2
                                                            : 1);
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Setup_LocationReport, data, this);
}

void EAXCop::SelfStrategy(int type) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    if (ai->GetPursuitState() != SoundAI::kActive) {
        return;
    }
    Csis::Setup_SelfStrategyStruct data;
    switch (type) {
    case 1:
        switch ((int) bRandom(4)) {
        case 0:
            data.self_strategy_type = static_cast<Csis::Type_self_strategy_type>(0x20);
            break;
        case 1:
            data.self_strategy_type = static_cast<Csis::Type_self_strategy_type>(0x40);
            break;
        case 2:
            data.self_strategy_type = static_cast<Csis::Type_self_strategy_type>(0x10);
            break;
        default:
            data.self_strategy_type = static_cast<Csis::Type_self_strategy_type>(0x8);
            break;
        }
        break;
    case 2:
    case 3:
    case 6:
        if (bRandom(1.0f) > 0.5f) {
            data.self_strategy_type = static_cast<Csis::Type_self_strategy_type>(2);
        } else {
            data.self_strategy_type = static_cast<Csis::Type_self_strategy_type>(4);
        }
        break;
    default:
        return;
    }
    data.code = GetRandomizedCode();
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Setup_SelfStrategy, data, this);
}

void EAXCop::CallforEV(unsigned int type) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr || ai->GetPursuitState() != SoundAI::kActive) {
        return;
    }
    if (ai->GetFocus() == 1) {
        return;
    }
    Csis::AnytimeEvents_CallForEVStruct data;
    if (type != 0) {
        data.ev_type = static_cast<Csis::Type_ev_type>(type);
    } else if (IsHeli()) {
        data.ev_type = static_cast<Csis::Type_ev_type>(4);
    } else if (ai->GetPursuit()->GetNumCopsDestroyed() > 1) {
        data.ev_type = static_cast<Csis::Type_ev_type>(1);
    } else {
        return;
    }
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_CallForEV, data, this);
}

void EAXCop::InitialCallForBackup() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    if (ai->AreRacersNearby()) {
        Csis::Setup_InitialCallForBU_MSStruct data;
        data.speaker_id = mSpeakerID;
        SCHEDULE_SPEECH(Setup_InitialCallForBU_MS, data, this);
    } else {
        Csis::Setup_InitialCallForBUStruct data;
        data.code = GetRandomizedCode();
        data.speaker_id = mSpeakerID;
        SCHEDULE_SPEECH(Setup_InitialCallForBU, data, this);
    }
}

int EAXCop::GetBackupTypeFromDispatch(int type) {
    switch (type) {
    case 8:
        return 2;
    case 16:
        return 4;
    case 1:
    case 2:
    case 4:
    case 32:
    case 64:
        return 1;
    default:
        return 0;
    }
}

void EAXCop::CallForBackup(int type) {
    Csis::Backup_CallForBUStruct data;
    data.code = GetRandomizedCode();
    data.backup_type = static_cast<Csis::Type_backup_type>(GetBackupTypeFromDispatch(type));
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Backup_CallForBU, data, this);
}

void EAXCop::BackupReminder(int type) {
    Csis::Backup_BUReminderStruct data;
    data.code = GetRandomizedCode();
    data.backup_type = static_cast<Csis::Type_backup_type>(GetBackupTypeFromDispatch(type));
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Backup_BUReminder, data, this);
}

void EAXCop::BackupArrives() {
    Csis::Backup_BUArrivesStruct data;
    data.speaker_id = mSpeakerID;
    data.speaker_battalion = static_cast<Csis::Type_speaker_battalion>(GetCallsign());
    data.speaker_call_sign_id = static_cast<Csis::Type_speaker_call_sign_id>(GetUnitNumber());
    SCHEDULE_SPEECH(Backup_BUArrives, data, this);
}

void EAXCop::PrimaryEngage() {
    Csis::Setup_PrimaryEngageStruct data;
    data.speaker_id = mSpeakerID;
    data.speaker_battalion = static_cast<Csis::Type_speaker_battalion>(GetCallsign());
    data.speaker_call_sign_id = static_cast<Csis::Type_speaker_call_sign_id>(GetUnitNumber());
    SCHEDULE_SPEECH(Setup_PrimaryEngage, data, this);
}

void EAXCop::UnitBackupReply() {
    Csis::Backup_UnitBUReplyStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Backup_UnitBUReply, data, this);
}

void EAXCop::NegativeBackupReply() {
    Csis::Backup_NegativeBUReplyStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Backup_NegativeBUReply, data, this);
}

void EAXCop::InitiateStrategy(int type) {
    Csis::RollingStrategy_InitStrategyStruct data;
    data.code = GetRandomizedCode();
    switch (type) {
    case E_BRAKE:
        data.rolling_strategy_type = static_cast<Csis::Type_rolling_strategy_type>(0x20);
        break;
    case COORDINATED_E_BRAKE:
        data.rolling_strategy_type = static_cast<Csis::Type_rolling_strategy_type>(4);
        break;
    case RAM:
        data.rolling_strategy_type = static_cast<Csis::Type_rolling_strategy_type>(2);
        break;
    case CROSS_BRAKE:
        data.rolling_strategy_type = static_cast<Csis::Type_rolling_strategy_type>(0x10);
        break;
    case CROSS_PLUS_V_BLOCK:
        return;
    default:
        return;
    }
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(RollingStrategy_InitStrategy, data, this);
}

void EAXCop::CallToPosition(EAXCop *cop) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::RollingStrategy_CallToPositionStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    data.subject_battalion = static_cast<Csis::Type_subject_battalion>(cop->GetCallsign());
    data.subject_call_sign_id = static_cast<Csis::Type_subject_call_sign_id>(cop->GetUnitNumber());
    float absz = UMath::Abs(mTgtOffset.z);
    if (UMath::Abs(mTgtOffset.x) > absz)
        data.position = static_cast<Csis::Type_position>((mTgtOffset.x > 0.0f) ? 1 : 2);
    else
        data.position = static_cast<Csis::Type_position>((mTgtOffset.z > 0.0f) ? 4 : 8);
    SCHEDULE_SPEECH(RollingStrategy_CallToPosition, data, this);
}

void EAXCop::CallToPositionReminder() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::RollingStrategy_CallToPositionRemStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(RollingStrategy_CallToPositionRem, data, this);
}

void EAXCop::StrategyExecute() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::RollingStrategy_StrategyExecuteStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(RollingStrategy_StrategyExecute, data, this);
}

void EAXCop::IntentToRam() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_IntentToRamStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_IntentToRam, data, this);
}

void EAXCop::AnticipateSuccess() {
    Csis::Outcome_AnticipateSuccessStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Outcome_AnticipateSuccess, data, this);
}

void EAXCop::AnticipateFail() {
    Csis::Outcome_AnticipateFailStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Outcome_AnticipateFail, data, this);
}

void EAXCop::LostSuspect() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_LostSuspectStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_LostSuspect, data, this);
}

void EAXCop::Arrest() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::Arrest_ArrestStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Arrest_Arrest, data, this);
}

void EAXCop::LostVisual() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_LostVisualStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_LostVisual, data, this);
}

void EAXCop::RegainVisual() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_RegainVisualStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_RegainVisual, data, this);
}

void EAXCop::OutcomeFail(short intensity) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::Outcome_OutcomeFailStruct data;
    if (intensity < 0)
        data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    else
        data.intensity = static_cast<Csis::Type_intensity>(intensity);
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Outcome_OutcomeFail, data, this);
}

void EAXCop::StrategyReset(bool new_strategy) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::Outcome_StrategyResetStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.same_new = new_strategy ? Csis::Type_same_new_new_strategy : Csis::Type_same_new_same_strategy;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Outcome_StrategyReset, data, this);
}

void EAXCop::SwarmingReply() {
    Csis::ExtraCops_SwarmingReplyStruct data;
    SCHEDULE_SPEECH(ExtraCops_SwarmingReply, data, this);
}

void EAXCop::Bullhorn() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        Csis::Setup_BullhornStruct data;
        data.speaker_id = mSpeakerID;
        SCHEDULE_SPEECH(Setup_Bullhorn, data, this);
    }
}

void EAXCop::PreBullhorn() {
    Csis::Setup_BullhornPrefixStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Setup_BullhornPrefix, data, this);
}

void EAXCop::BullhornArrest() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if (ai != nullptr) {
            Csis::Arrest_BullhornArrestStruct data;
            data.speaker_id = mSpeakerID;
            data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
            SCHEDULE_SPEECH(Arrest_BullhornArrest, data, this);
        }
    }
}

void EAXCop::SuspectBehavior() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::AnytimeEvents_SuspectBehaviourStruct data;
        data.speaker_id = mSpeakerID;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        SCHEDULE_SPEECH(AnytimeEvents_SuspectBehaviour, data, this);
    }
}

void EAXCop::SuspectConfirmed() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::Setup_SuspectConfirmedStruct data;
        data.speaker_id = mSpeakerID;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        SCHEDULE_SPEECH(Setup_SuspectConfirmed, data, this);
    }
}

void EAXCop::SuspectOutrun() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_SuspectOutrunStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    SCHEDULE_SPEECH(AnytimeEvents_SuspectOutrun, data, this);
}

void EAXCop::SuspectUTurn() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_SuspectUTurnStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    SCHEDULE_SPEECH(AnytimeEvents_SuspectUTurn, data, this);
}

void EAXCop::FocusChange() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_FocusChangeStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    SCHEDULE_SPEECH(AnytimeEvents_FocusChange, data, this);
}

void EAXCop::Impact_Suspect_World() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = mSpeakerID;
    data.world_object_type = static_cast<Csis::Type_world_object_type>(
        (Speech::Manager::GetHistory().GetCount(static_cast<SPCHType_1_EventID>(0x62)) > 0 || ai->IsHighIntensity()) ? 1 : 0x40);
    data.num_units = static_cast<Csis::Type_num_units>(1);
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = static_cast<Csis::Type_num_units>(2);
    }
    SCHEDULE_SPEECH(AnytimeEvents_CollisionWorld, data, this);
}

void EAXCop::Impact_Suspect_Semi() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = mSpeakerID;
    data.world_object_type = static_cast<Csis::Type_world_object_type>(8);
    data.num_units = static_cast<Csis::Type_num_units>(1);
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = static_cast<Csis::Type_num_units>(2);
    }
    SCHEDULE_SPEECH(AnytimeEvents_CollisionWorld, data, this);
}

void EAXCop::Impact_Suspect_Train() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = mSpeakerID;
    data.world_object_type = static_cast<Csis::Type_world_object_type>(4);
    data.num_units = static_cast<Csis::Type_num_units>(1);
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = static_cast<Csis::Type_num_units>(2);
    }
    SCHEDULE_SPEECH(AnytimeEvents_CollisionWorld, data, this);
}

void EAXCop::Impact_Suspect_Guardrail() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = mSpeakerID;
    data.world_object_type = static_cast<Csis::Type_world_object_type>(2);
    data.num_units = static_cast<Csis::Type_num_units>(1);
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = static_cast<Csis::Type_num_units>(2);
    }
    SCHEDULE_SPEECH(AnytimeEvents_CollisionWorld, data, this);
}

void EAXCop::Impact_Suspect_GasStation() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->GetPursuitState() == SoundAI::kActive) {
        Csis::AnytimeEvents_CollisionWorldStruct data;
        data.speaker_id = mSpeakerID;
        data.world_object_type = static_cast<Csis::Type_world_object_type>(0x10);
        data.num_units = static_cast<Csis::Type_num_units>(1);
        SCHEDULE_SPEECH(AnytimeEvents_CollisionWorld, data, this);
    }
}

void EAXCop::Impact_Suspect_Spikebelt() {
    Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
    if (cop_speech != nullptr) {
        cop_speech->ReleaseResource();
    }
    Speech::Manager::ClearPlayback();
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = mSpeakerID;
    data.world_object_type = static_cast<Csis::Type_world_object_type>(0x20);
    data.num_units = static_cast<Csis::Type_num_units>(1);
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = static_cast<Csis::Type_num_units>(2);
    }
    SCHEDULE_SPEECH(AnytimeEvents_CollisionWorld, data, this);
}

void EAXCop::Impact_Suspect_Traffic(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_CiviStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    SCHEDULE_SPEECH(AnytimeEvents_CollWorld_Civi, data, this);
}

void EAXCop::SuspectRollover(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_FlipStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    SCHEDULE_SPEECH(AnytimeEvents_CollWorld_Flip, data, this);
}

void EAXCop::SuspectAirborne(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_AirStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    SCHEDULE_SPEECH(AnytimeEvents_CollWorld_Air, data, this);
}

void EAXCop::SuspectSpunout(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_SpinStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    SCHEDULE_SPEECH(AnytimeEvents_CollWorld_Spin, data, this);
}

void EAXCop::SuspectBrake() {
    Csis::AnytimeEvents_SuspectBrakeStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_SuspectBrake, data, this);
}

void EAXCop::UnitDisabled(int other) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    Csis::AnytimeEvents_UnitDisabledStruct data;
    data.self_other = other != 0 ? static_cast<Csis::Type_self_other>(2) : static_cast<Csis::Type_self_other>(1);
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_UnitDisabled, data, this);
}

void EAXCop::PursuitUpdateReply() {
    Csis::AnytimeEvents_PursuitUpdateRepStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_PursuitUpdateRep, data, this);
}

void EAXCop::Bailout() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        Csis::AnytimeEvents_BailoutStruct data;
        data.speaker_id = mSpeakerID;
        data.bailout_type = static_cast<Csis::Type_bailout_type>(8);
        SCHEDULE_SPEECH(AnytimeEvents_Bailout, data, this);
    }
}

void EAXCop::DenyBailout() {
    Csis::AnytimeEvents_BailoutDenyStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_BailoutDeny, data, this);
}

void EAXCop::LoBailout() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        Csis::AnytimeEvents_BailoutStruct data;
        data.speaker_id = mSpeakerID;
        data.bailout_type = static_cast<Csis::Type_bailout_type>(1);
        SCHEDULE_SPEECH(AnytimeEvents_Bailout, data, this);
    }
}

void EAXCop::HiBailout() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        Csis::AnytimeEvents_BailoutStruct data;
        data.speaker_id = mSpeakerID;
        data.bailout_type = static_cast<Csis::Type_bailout_type>(0x10);
        SCHEDULE_SPEECH(AnytimeEvents_Bailout, data, this);
    }
}

void EAXCop::BailoutBadRoad() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        Csis::AnytimeEvents_BailoutStruct data;
        data.speaker_id = mSpeakerID;
        data.bailout_type = static_cast<Csis::Type_bailout_type>(4);
        SCHEDULE_SPEECH(AnytimeEvents_Bailout, data, this);
    }
}

void EAXCop::BailoutTraffic() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        Csis::AnytimeEvents_BailoutStruct data;
        data.speaker_id = mSpeakerID;
        data.bailout_type = static_cast<Csis::Type_bailout_type>(2);
        SCHEDULE_SPEECH(AnytimeEvents_Bailout, data, this);
    }
}

void EAXCop::CallForRB() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::StaticRoadblock_CallForRBStruct data;
        data.speaker_id = mSpeakerID;
        data.code = GetRandomizedCode();
        data.roadblock_type = static_cast<Csis::Type_roadblock_type>(ai->SpikesEnabled() ? 2 : 1);
        SCHEDULE_SPEECH(StaticRoadblock_CallForRB, data, this);
    }
}

void EAXCop::CallForSubRB() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::StaticRoadblock_CallForRB_subStruct data;
        data.speaker_id = mSpeakerID;
        SCHEDULE_SPEECH(StaticRoadblock_CallForRB_sub, data, this);
    }
}

void EAXCop::RBReminder() {
    Csis::StaticRoadblock_RBReminderStruct data;
    data.speaker_id = mSpeakerID;
    data.code = GetRandomizedCode();
    SCHEDULE_SPEECH(StaticRoadblock_RBReminder, data, this);
}

void EAXCop::NegRBReply() {
    if (!IsHeli()) {
        Csis::StaticRoadblock_NegativeRBReplyStruct data;
        data.speaker_id = mSpeakerID;
        SCHEDULE_SPEECH(StaticRoadblock_NegativeRBReply, data, this);
    }
}

void EAXCop::RBApproach() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai == nullptr) {
        return;
    }
    IRoadBlock *roadblock = ai->GetRoadblock();
    if (roadblock == nullptr) {
        return;
    }
    Csis::StaticRoadblock_RBApproachStruct data;
    data.speaker_id = mSpeakerID;
    data.roadblock_type = static_cast<Csis::Type_roadblock_type>(roadblock->GetNumSpikeStrips() > 0 ? 2 : 1);
    SCHEDULE_SPEECH(StaticRoadblock_RBApproach, data, this);
}

void EAXCop::RBEngage(bool spikes_hit) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr && ai->GetRoadblock() != nullptr) {
        Csis::StaticRoadblock_RBEngageStruct data;
        data.speaker_id = mSpeakerID;
        data.roadblock_engage_type = spikes_hit ? static_cast<Csis::Type_roadblock_engage_type>(2)
                                                : static_cast<Csis::Type_roadblock_engage_type>(1);
        SCHEDULE_SPEECH(StaticRoadblock_RBEngage, data, this);
    }
}

void EAXCop::RBAverted() {
    Csis::StaticRoadblock_RBAvertedStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(StaticRoadblock_RBAverted, data, this);
}

void EAXCop::PursuitApproaching() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        Csis::StaticRoadblock_PursuitApproachingStruct data;
        data.speaker_id = mSpeakerID;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        SCHEDULE_SPEECH(StaticRoadblock_PursuitApproaching, data, this);
    }
}

void EAXCop::HeadOn(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_HOStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    SCHEDULE_SPEECH(Interrupts_InterruptRam_HO, data, this);
}

void EAXCop::TBoned(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_TBStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    SCHEDULE_SPEECH(Interrupts_InterruptRam_TB, data, this);
}

void EAXCop::SideSwiped(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_SSStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    SCHEDULE_SPEECH(Interrupts_InterruptRam_SS, data, this);
}

void EAXCop::RearEnded(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_REStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = intensity;
    SCHEDULE_SPEECH(Interrupts_InterruptRam_RE, data, this);
}

void EAXCop::Spotted() {
    Csis::AnytimeEvents_SpottedStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = GetSpeed() > 80.0f ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    SCHEDULE_SPEECH(AnytimeEvents_Spotted, data, this);
}

void EAXCop::DirectionChange() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai != nullptr) {
        unsigned int dir = ai->GetPlayerDirection(0);
        if (dir != 0) {
            Csis::AnytimeEvents_DirectionHighStruct data;
            data.speaker_id = mSpeakerID;
            data.direction = static_cast<Csis::Type_direction>(dir);
            SCHEDULE_SPEECH(AnytimeEvents_DirectionHigh, data, this);
        }
    }
}

void EAXCop::CallForSwarming() {
    Csis::Backup_CallForSwarmingStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Backup_CallForSwarming, data, this);
}

void EAXCop::SpotterWanted() {
    Csis::Setup_SpotterWantedStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(Setup_SpotterWanted, data, this);
}

void EAXCop::Offroad(unsigned int id, bool subsequent) {
    Csis::AnytimeEvents_OffroadMomentStruct data;
    data.speaker_id = mSpeakerID;
    data.offroad_moment_id = static_cast<Csis::Type_offroad_moment_id>(id);
    data.first_subsequent = subsequent ? Csis::Type_first_subsequent_subsequent_time : Csis::Type_first_subsequent_first_time;
    SCHEDULE_SPEECH(AnytimeEvents_OffroadMoment, data, this);
}

void EAXCop::WeatherReport() {
    Csis::AnytimeEvents_WeatherReportStruct data;
    data.speaker_id = mSpeakerID;
    SCHEDULE_SPEECH(AnytimeEvents_WeatherReport, data, this);
}
