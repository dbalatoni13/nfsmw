#include "Speed/Indep/Src/EAXSound/SND_GEN/COPSPEECH.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

EAXCop::EAXCop(int speakerID, HSIMABLE handle, int bID, int cID)
    : EAXCharacter(speakerID, handle, bID, cID), //
      mRank(0),                                  //
      mInFormation(false),                       //
      mInPosition(false),                        //
      mAhead(false),                             //
      mPctTractiveTires(0.0f),                   //
      mTimeAirborne(WorldTimer),                 //
      mTrafficHitCount(0),                       //
      mTimeNoLOS(WorldTimer),                    //
      mNumRammed(0),                             //
      mLastRammedTime(0),                        //
      mT_lastactivity(0),                        //
      mT_closingDist(0),                         //
      mOrigin(MAX_ROADNAMES),                    //
      mCurrRoad(MAX_ROADNAMES),                  //
      mTgtOffset(UMath::Vector3::kZero) {
    ISimable *simable = ISimable::FindInstance(handle);
    if (simable != nullptr) {
        IVehicleAI *vai;
        if (simable->QueryInterface(&vai)) {
            WRoadNav *nav = vai->GetDriveToNav();
            if (nav != nullptr && nav->GetRoadSpeechId() != MAX_ROADNAMES) {
                this->mOrigin = static_cast<RoadNames>(nav->GetRoadSpeechId());
            }
        }
    }
}

EAXCop::~EAXCop() {
    Speech::Manager::FlushSpeechForActor(this);
}

#define MIN_DIST_BACKUP_ENROUTE 100.0f // Decl: 81

void EAXCop::Update() {
    this->EAXCharacter::Update();
    if (this->GetHandle() == nullptr) {
        this->mInFormation = false;
        this->mInPosition = false;
        this->mPctTractiveTires = 1.0f;
        this->mTimeAirborne = WorldTimer;
        this->EAXCharacter::Reset();
        return;
    }

    ISimable *simable = ISimable::FindInstance(this->GetHandle());
    IDamageable *damage = nullptr;
    IVehicle *copcar = nullptr;
    ISuspension *suspension = nullptr;
    IPursuitAI *pursuitAI = nullptr;
    IVehicleAI *vai = nullptr;
    if (this->mSuspectLOS) {
        this->mTimeNoLOS = WorldTimer;
    }

    if (simable != nullptr) {
        simable->QueryInterface(&copcar);
        simable->QueryInterface(&damage);
        simable->QueryInterface(&pursuitAI);
        simable->QueryInterface(&suspension);
        simable->QueryInterface(&vai);
    } else {
        this->SetActive(false);
    }

    if (pursuitAI != nullptr && (this->mActive)) {
        this->SetInFormation(pursuitAI->GetInFormation());
        this->SetInPosition(pursuitAI->GetInPosition());
        this->SetTgtOffset(pursuitAI->GetPursuitOffset());
    } else {
        this->SetInFormation(false);
        this->SetInPosition(false);
        this->SetTgtOffset(UMath::Vector3::kZero);
    }

    if (suspension != nullptr && (this->mActive)) {
        this->mPctTractiveTires =
            suspension->GetNumWheels() != 0 ? static_cast<float>(suspension->GetNumWheelsOnGround() / suspension->GetNumWheels()) : 1.0f;
        if (this->mPctTractiveTires > 0.25f) {
            this->mTimeAirborne = WorldTimer;
        }
    } else {
        this->mTimeAirborne = WorldTimer;
        this->mPctTractiveTires = 1.0f;
    }

    if (damage != nullptr) {
        this->mDestroyed = damage->IsDestroyed();
        this->mHealth = damage->GetHealth();
    }

    if (vai != nullptr) {
        if (!this->mActive) {
            this->mCurrRoad = MAX_ROADNAMES;
        } else {
            WRoadNav *nav = vai->GetDriveToNav();
            unsigned int roadID = nav->GetRoadSpeechId();
            if (roadID != MAX_ROADNAMES) {
                this->mCurrRoad = static_cast<RoadNames>(roadID);
            }
        }
    } else {
        this->mCurrRoad = MAX_ROADNAMES;
    }

    if ((this->mActive) && (this->mSuspectLOS) && copcar != nullptr) {
        SoundAI *ai = SoundAI::Get();
        if (ai != nullptr) {
            UMath::Vector3 coppos = copcar->GetPosition();
            UMath::Vector3 ppos = ai->GetPlayerPos();
            float dist = UMath::Distance(coppos, ppos);
            if (dist <= this->mDistance) {
                this->mT_closingDist = WorldTimer;
            }
        }
    }
}

bool EAXCop::SetRank(int newrank) {
    bool rval = false;
    if (newrank == 0 && this->mRank > 0) {
        Csis::Setup_PrimaryEngageStruct takeover;
        takeover.speaker_id = this->mSpeakerID;
        Speech::Manager::ScheduleSpeech(takeover, Csis::Setup_PrimaryEngageId, Csis::gSetup_PrimaryEngageHandle, this);
        this->Reset();
        rval = true;
    }
    this->mRank = newrank;
    return rval;
}

void EAXCop::SwapVoices(EAXCop *cop) {
    int callsign = cop->GetCallsign();
    int unitnum = cop->GetUnitNumber();
    int spkrID = cop->GetSpeakerID();
    cop->SetSpeakerID(this->mSpeakerID);
    cop->SetCallsign(this->mCallsign.name);
    cop->SetUnitNumber(this->mCallsign.number);
    this->mSpeakerID = spkrID;
    this->mCallsign.name = callsign;
    this->mCallsign.number = unitnum;
}

void EAXCop::SetActive(bool activity) {
    if (this->mActive != activity) {
        this->mActive = activity;
        SoundAI *ai = SoundAI::Get();
        if (ai == nullptr) {
            return;
        }

        if (activity) {
            if (ai->GetFocus() == 666 || ai->GetFocus() == 2) {
                if (this->mSuspectLOS) {
                    if (bRandom(1.0f) > 0.5f) {
                        this->Spotted();
                    } else {
                        this->RegainVisual();
                    }
                } else {
                    if (ai->GetFocus() == 2 && ai->GetPursuitState() != SoundAI::kInactive) {
                        IRoadBlock *block = ai->GetRoadblock();
                        if (block != nullptr && block->IsComprisedOf(this->GetHandle()) == nullptr) {
                            if (bRandom(1.0f) > 0.5f) {
                                this->UnitBackupReply();
                            } else {
                                this->BackupArrives();
                            }
                        }
                    }
                }
            }
        } else {
            float t_lastactivity = (WorldTimer - this->mT_lastactivity).GetSeconds();
            if (t_lastactivity < 2.0f) {
                return;
            }

            int num_events_flushed = Speech::Manager::FlushSpeechForActor(this);
            if (!this->mInFormation && !this->mInPosition) {
                return;
            }

            this->mT_lastactivity = WorldTimer;

            if (this->mTrafficHitCount > 1) {
                this->BailoutTraffic();
                this->mTrafficHitCount = 0;
                if (ai->GetPursuitState() != SoundAI::kActive && ai->GetPursuitState() != SoundAI::kSearching) {
                    return;
                }
                ai->RandomBailoutDeny(this);
                return;
            }

            if (this->IsHeli()) {
                this->Bailout();
                return;
            }

            if (this->mDestroyed) {
                EAXCop *rand_cop = ai->FindClosestCop(true, true);
                if (rand_cop != nullptr) {
                    rand_cop->UnitDisabled(static_cast<int>(rand_cop->GetSpeakerID() != this->GetSpeakerID()));
                } else {
                    this->UnitDisabled(0);
                }
                return;
            }

            if (this->mHealth < ai->GetTune().MinHealthForCommentary()) {
                this->HiBailout();
                return;
            }

            this->LoBailout();
            if (ai->GetPursuitState() != SoundAI::kActive && ai->GetPursuitState() != SoundAI::kSearching) {
                return;
            }
            ai->RandomBailoutDeny(this);
        }
    }
}

bool EAXCop::IsPrimary() {
    if ((this->mSpeakerID >= Speech::Heli && this->mSpeakerID <= Speech::Primary3) || (this->mSpeakerID == Speech::Cross)) {
        return true;
    }
    return false;
}

void EAXCop::Reset() {
    this->mInFormation = false;
    this->mInPosition = false;
    this->mPctTractiveTires = 1.0f;
    this->mTimeAirborne = Timer(0);
    this->mTrafficHitCount = 0;
    this->mTimeNoLOS = Timer(0);
    this->mNumRammed = 0;
    this->mLastRammedTime = Timer(0);
    this->mT_lastactivity = Timer(0);
    this->mT_closingDist = Timer(0);
    this->EAXCharacter::Reset();
}

void EAXCop::Collision(int collisionType, float force, EAXCop *spkr) {
    Csis::Type_intensity intensity = Csis::Type_intensity_Normal;
    if (force > 0.75f) {
        intensity = Csis::Type_intensity_High;
    }
    switch (collisionType) {
        case Speech::Collision_Cop_Cop:
        case Speech::Collision_Cop_Traffic:
        case Speech::Collision_Cop_Suspect:
            break;
        case Speech::Collision_Cop_World:
            return;
        case Speech::Collision_Suspect_World:
            this->Impact_Suspect_World();
            break;
        case Speech::Collision_Suspect_Traffic:
            this->Impact_Suspect_Traffic(intensity);
            break;
        case Speech::Collision_Suspect_Semi:
            this->Impact_Suspect_Semi();
            break;
        case Speech::Collision_Suspect_Guardrail:
            this->Impact_Suspect_Guardrail();
            break;
        case Speech::Collision_Suspect_Train:
            this->Impact_Suspect_Train();
            break;
        case Speech::Collision_Suspect_GasStation:
            this->Impact_Suspect_GasStation();
            break;
        case Speech::Collision_Suspect_Spikebelt:
            this->Impact_Suspect_Spikebelt();
            break;
        case Speech::Behavior_Suspect_Airborne:
            this->SuspectAirborne(intensity);
            break;
        case Speech::Behavior_Suspect_Rollover:
            this->SuspectRollover(intensity);
            break;
        case Speech::Behavior_Suspect_Spunout:
            this->SuspectSpunout(intensity);
            break;
        default:
            break;
    }
}

void EAXCop::AttemptVehicleStop() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::Setup_AttmptVehStpStruct data;
    Speech::EventHistory &g_hist = Speech::Manager::GetHistory();
    if (ai->GetTimeLastNailedCop() < 5.0f) {
        data.pursuit_type = Csis::Type_pursuit_type_Unit_Rammed;
    } else {
        if (ai->GetLastInfraction() > 0) {
            switch (ai->GetLastInfraction()) {
                case GInfractionManager::kInfraction_Speeding:
                case GInfractionManager::kInfraction_Racing:
                    if (ai->IsHighIntensity()) {
                        data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
                    } else {
                        data.pursuit_type = Csis::Type_pursuit_type_Generic_Speeder;
                    }
                    break;
                case GInfractionManager::kInfraction_Reckless:
                case GInfractionManager::kInfraction_OffRoad:
                    data.pursuit_type = Csis::Type_pursuit_type_Reckless;
                    break;
                case GInfractionManager::kInfraction_Assault:
                    data.pursuit_type = Csis::Type_pursuit_type_Unit_Rammed;
                    break;
                case GInfractionManager::kInfraction_HitAndRun:
                case GInfractionManager::kInfraction_Damage:
                    data.pursuit_type = Csis::Type_pursuit_type_Hit_and_Run;
                    break;
                case GInfractionManager::kInfraction_Resist:
                    data.pursuit_type = Csis::Type_pursuit_type_Possible_Wanted;
                    break;
                default:
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
    }

    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    data.speaker_battalion = static_cast<Csis::Type_speaker_battalion>(this->GetCallsign());
    data.speaker_call_sign_id = static_cast<Csis::Type_speaker_call_sign_id>(this->GetUnitNumber());
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_AttmptVehStpId, Csis::gSetup_AttmptVehStpHandle, this);
}

void EAXCop::Spotter() {
    SoundAI *ai = SoundAI::Get();
    Csis::Setup_SpotterStruct data;
    if (ai != nullptr) {
        data.speaker_id = this->mSpeakerID;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        Speech::Manager::ScheduleSpeech(data, Csis::Setup_SpotterId, Csis::gSetup_SpotterHandle, this);
    }
}

void EAXCop::SpotterReply() {
    SoundAI *ai = SoundAI::Get();
    Csis::Setup_SpotterReplyStruct data;
    if (ai != nullptr) {
        data.speaker_id = this->mSpeakerID;
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        Speech::Manager::ScheduleSpeech(data, Csis::Setup_SpotterReplyId, Csis::gSetup_SpotterReplyHandle, this);
    }
}

void EAXCop::Reply911() {
    Csis::AnytimeEvents_Unit911ReplyStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_Unit911ReplyId, Csis::gAnytimeEvents_Unit911ReplyHandle, this);
}

void EAXCop::ReinitiatePursuit() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::Setup_ReInitPursuitStruct data;
    if (ai->GetTimeSinceLastChase() < ai->GetTune().TimeConsideredLostNoLOS()) {
        if (this->IsAhead() != 0.0f) {
            this->Spotted();
        } else {
            this->RegainVisual();
        }
    } else {
        if (ai->GetTimeSinceLastChase() < 30.0f) {
            data.time_since_lost = Csis::Type_time_since_lost_lost_recently;
        } else {
            data.time_since_lost = Csis::Type_time_since_lost_lost_for_X_time;
        }
        data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
        data.speaker_id = this->mSpeakerID;
        Speech::Manager::ScheduleSpeech(data, Csis::Setup_ReInitPursuitId, Csis::gSetup_ReInitPursuitHandle, this);
    }
}

void EAXCop::VehicleReport() {
    static const float speed_test[11] = {100.0f, 120.0f, 140.0f, 160.0f, 180.0f, 200.0f, 220.0f, 240.0f, 260.0f, 280.0f, 300.0f};
    int ndx = 0;
    if (FEDatabase == nullptr) {
        return;
    }
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    unsigned int color = ai->GetPlayerCarColor();
    if (color == 0 || !MiscSpeech::IsVehicleTypeOK()) {
        return;
    }
    const Attrib::Gen::pvehicle &pcar = ai->GetPlayerSpecs();

    Csis::Setup_VehicleReportStruct data;
    data.speaker_id = this->mSpeakerID;
    data.car_type = pcar.VerbalType();
    data.car_color = static_cast<Csis::Type_car_color>(color);

    float speedo = ai->GetPlayerSpeed();
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        data.measurement = Csis::Type_measurement_metric_only;
        speedo = MPH2KPH(speedo);
    } else {
        data.measurement = Csis::Type_measurement_imperial_only;
    }
    while (ndx <= 10 && speedo >= speed_test[ndx]) {
        ndx++;
    }
    if (ndx == 0) {
        data.speed = Csis::Type_speed_over_speed_limit;
        data.measurement = Csis::Type_measurement_generic;
    } else {
        if (bRandom(1.0f) > 0.5f) {
            data.measurement = Csis::Type_measurement_generic;
        }
        data.speed = static_cast<Csis::Type_speed>(2 << (ndx - 1));
    }
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_VehicleReportId, Csis::gSetup_VehicleReportHandle, this);
}

void EAXCop::InitiatePursuit() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::Setup_InitPursuitStruct data;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_InitPursuitId, Csis::gSetup_InitPursuitHandle, this);
}

void EAXCop::LocationReport() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr || !ai->IsHeadingValid()) {
        return;
    }

    Csis::Setup_LocationReportStruct data;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    data.direction = static_cast<Csis::Type_direction>(ai->GetPlayerDirection(0));

    Csis::Type_location_region region;
    Csis::Type_location location;
    RoadNames road;
    bool result = MiscSpeech::GetLocation(ai->GetPlayerRoadID(0), region, location);
    if (result) {
        data.location_region = region;
        data.location = location;
        data.encounter = Speech::Manager::GetHistory().GetCount(kSPCH1_EventID_LocationReport) > 1 ? Csis::Type_encounter_subsequent_encounter
                                                                                                   : Csis::Type_encounter_first_encounter;
        data.speaker_id = this->mSpeakerID;
        Speech::Manager::ScheduleSpeech(data, Csis::Setup_LocationReportId, Csis::gSetup_LocationReportHandle, this);
    }
}

void EAXCop::SelfStrategy(int type) {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr || ai->GetPursuitState() != SoundAI::kActive) {
        return;
    }

    Csis::Setup_SelfStrategyStruct data;
    switch (type) {
        case 1: {
            int select = bRandom(4);
            switch (select) {
                case 0:
                    data.self_strategy_type = Csis::Type_self_strategy_type_Pit;
                    break;
                case 1:
                    data.self_strategy_type = Csis::Type_self_strategy_type_Self_Pit;
                    break;
                case 2:
                    data.self_strategy_type = Csis::Type_self_strategy_type_Side_Ram;
                    break;
                default:
                    data.self_strategy_type = Csis::Type_self_strategy_type_Ram;
                    break;
            }
            break;
        }
        case 2:
        case 3:
        case 6: {
            int select;
            if (bRandom(1.0f) > 0.5f) {
                data.self_strategy_type = Csis::Type_self_strategy_type_Rolling_Roadblock;
            } else {
                data.self_strategy_type = Csis::Type_self_strategy_type_Herding;
            }
            break;
        }
        default:
            return;
    }

    data.code = this->GetRandomizedCode();
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_SelfStrategyId, Csis::gSetup_SelfStrategyHandle, this);
}

void EAXCop::CallforEV(unsigned int type) {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr || ai->GetPursuitState() != SoundAI::kActive || ai->GetFocus() == 1) {
        return;
    }

    Csis::AnytimeEvents_CallForEVStruct data;
    if (type != 0) {
        data.ev_type = static_cast<Csis::Type_ev_type>(type);
    } else if (this->IsHeli()) {
        data.ev_type = Csis::Type_ev_type_heli_down;
    } else {
        if (ai->GetPursuit()->GetNumCopsDestroyed() < 2) {
            return;
        }
        data.ev_type = Csis::Type_ev_type_multiple_units_down;
    }
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CallForEVId, Csis::gAnytimeEvents_CallForEVHandle, this);
}

void EAXCop::InitialCallForBackup() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    if (ai->AreRacersNearby()) {
        Csis::Setup_InitialCallForBU_MSStruct data;
        data.speaker_id = this->mSpeakerID;
        Speech::Manager::ScheduleSpeech(data, Csis::Setup_InitialCallForBU_MSId, Csis::gSetup_InitialCallForBU_MSHandle, this);
    } else {
        Csis::Setup_InitialCallForBUStruct data;
        data.code = this->GetRandomizedCode();
        data.speaker_id = this->mSpeakerID;
        Speech::Manager::ScheduleSpeech(data, Csis::Setup_InitialCallForBUId, Csis::gSetup_InitialCallForBUHandle, this);
    }
}

// TODO magic
int EAXCop::GetBackupTypeFromDispatch(int type) {
    switch (type) {
        case 8:
            return Csis::Type_backup_type_Air_support;
        case 0x10:
            return Csis::Type_backup_type_Rhino_Unit;
        case 1:
        case 2:
        case 4:
        case 32:
        case 64:
            return Csis::Type_backup_type_all_available_units;
        default:
            return 0;
    }
}

void EAXCop::CallForBackup(int type) {
    Csis::Backup_CallForBUStruct data;
    data.code = static_cast<Csis::Type_code>(this->GetRandomizedCode());
    data.backup_type = static_cast<Csis::Type_backup_type>(this->GetBackupTypeFromDispatch(type));
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Backup_CallForBUId, Csis::gBackup_CallForBUHandle, this);
}

void EAXCop::BackupReminder(int type) {
    Csis::Backup_BUReminderStruct data;
    data.code = this->GetRandomizedCode();
    data.backup_type = static_cast<Csis::Type_backup_type>(this->GetBackupTypeFromDispatch(type));
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Backup_BUReminderId, Csis::gBackup_BUReminderHandle, this);
}

void EAXCop::BackupArrives() {
    Csis::Backup_BUArrivesStruct data;
    data.speaker_id = this->mSpeakerID;
    data.speaker_battalion = static_cast<Csis::Type_speaker_battalion>(this->GetCallsign());
    data.speaker_call_sign_id = static_cast<Csis::Type_speaker_call_sign_id>(this->GetUnitNumber());
    Speech::Manager::ScheduleSpeech(data, Csis::Backup_BUArrivesId, Csis::gBackup_BUArrivesHandle, this);
}

void EAXCop::PrimaryEngage() {
    Csis::Setup_PrimaryEngageStruct data;
    data.speaker_id = this->mSpeakerID;
    data.speaker_battalion = static_cast<Csis::Type_speaker_battalion>(this->GetCallsign());
    data.speaker_call_sign_id = static_cast<Csis::Type_speaker_call_sign_id>(this->GetUnitNumber());
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_PrimaryEngageId, Csis::gSetup_PrimaryEngageHandle, this);
}

void EAXCop::UnitBackupReply() {
    Csis::Backup_UnitBUReplyStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Backup_UnitBUReplyId, Csis::gBackup_UnitBUReplyHandle, this);
}

void EAXCop::NegativeBackupReply() {
    Csis::Backup_NegativeBUReplyStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Backup_NegativeBUReplyId, Csis::gBackup_NegativeBUReplyHandle, this);
}

void EAXCop::InitiateStrategy(int type) {
    Csis::RollingStrategy_InitStrategyStruct init;
    init.code = this->GetRandomizedCode();
    switch (type) {
        case 1:
            init.rolling_strategy_type = Csis::Type_rolling_strategy_type_Pit;
            break;
        case 2:
            init.rolling_strategy_type = Csis::Type_rolling_strategy_type_Box;
            break;
        case 3:
            init.rolling_strategy_type = Csis::Type_rolling_strategy_type_Rolling_Roadblock;
            break;
        case 6:
            init.rolling_strategy_type = Csis::Type_rolling_strategy_type_Herding;
            break;
        case 7:
        default:
            return;
    }
    init.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(init, Csis::RollingStrategy_InitStrategyId, Csis::gRollingStrategy_InitStrategyHandle, this);
}

void EAXCop::CallToPosition(EAXCop *cop) {
    SoundAI *ai = SoundAI::Get();

    Csis::RollingStrategy_CallToPositionStruct call;
    call.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    call.speaker_id = this->mSpeakerID;
    call.subject_battalion = static_cast<Csis::Type_subject_battalion>(cop->GetCallsign());
    call.subject_call_sign_id = static_cast<Csis::Type_subject_call_sign_id>(cop->GetUnitNumber());

    float zmag = UMath::Abs(this->mTgtOffset.z);
    float xmag = UMath::Abs(this->mTgtOffset.x);
    call.position = (xmag > zmag) ? ((0.0f < this->mTgtOffset.x) ? Csis::Type_position_Right_Side : Csis::Type_position_Left_Side)
                                  : ((0.0f < this->mTgtOffset.z) ? Csis::Type_position_Ahead : Csis::Type_position_Behind);
    Speech::Manager::ScheduleSpeech(call, Csis::RollingStrategy_CallToPositionId, Csis::gRollingStrategy_CallToPositionHandle, this);
}

void EAXCop::CallToPositionReminder() {
    SoundAI *ai = SoundAI::Get();
    Csis::RollingStrategy_CallToPositionRemStruct reminder;
    reminder.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    reminder.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(reminder, Csis::RollingStrategy_CallToPositionRemId, Csis::gRollingStrategy_CallToPositionRemHandle, this);
}

void EAXCop::StrategyExecute() {
    SoundAI *ai = SoundAI::Get();
    Csis::RollingStrategy_StrategyExecuteStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::RollingStrategy_StrategyExecuteId, Csis::gRollingStrategy_StrategyExecuteHandle, this);
}

void EAXCop::IntentToRam() {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_IntentToRamStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_IntentToRamId, Csis::gAnytimeEvents_IntentToRamHandle, this);
}

void EAXCop::AnticipateSuccess() {
    Csis::Outcome_AnticipateSuccessStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Outcome_AnticipateSuccessId, Csis::gOutcome_AnticipateSuccessHandle, this);
}

void EAXCop::AnticipateFail() {
    Csis::Outcome_AnticipateFailStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Outcome_AnticipateFailId, Csis::gOutcome_AnticipateFailHandle, this);
}

void EAXCop::LostSuspect() {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_LostSuspectStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_LostSuspectId, Csis::gAnytimeEvents_LostSuspectHandle, this);
}

void EAXCop::Arrest() {
    SoundAI *ai = SoundAI::Get();
    Csis::Arrest_ArrestStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Arrest_ArrestId, Csis::gArrest_ArrestHandle, this);
}

void EAXCop::LostVisual() {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_LostVisualStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_LostVisualId, Csis::gAnytimeEvents_LostVisualHandle, this);
}

void EAXCop::RegainVisual() {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_RegainVisualStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_RegainVisualId, Csis::gAnytimeEvents_RegainVisualHandle, this);
}

void EAXCop::OutcomeFail(short intensity) {
    SoundAI *ai = SoundAI::Get();
    Csis::Outcome_OutcomeFailStruct data;
    if (intensity < 0) {
        data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    } else {
        data.intensity = static_cast<Csis::Type_intensity>(intensity);
    }
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Outcome_OutcomeFailId, Csis::gOutcome_OutcomeFailHandle, this);
}

void EAXCop::StrategyReset(bool new_strategy) {
    SoundAI *ai = SoundAI::Get();
    Csis::Outcome_StrategyResetStruct data;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    if (new_strategy) {
        data.same_new = Csis::Type_same_new_new_strategy;
    } else {
        data.same_new = Csis::Type_same_new_same_strategy;
    }
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Outcome_StrategyResetId, Csis::gOutcome_StrategyResetHandle, this);
}

void EAXCop::SwarmingReply() {
    Csis::ExtraCops_SwarmingReplyStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::ExtraCops_SwarmingReplyId, Csis::gExtraCops_SwarmingReplyHandle, this);
}

void EAXCop::Bullhorn() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        Csis::Setup_BullhornStruct data;
        data.speaker_id = this->mSpeakerID;
        Speech::Manager::ScheduleSpeech(data, Csis::Setup_BullhornId, Csis::gSetup_BullhornHandle, this);
    }
}

void EAXCop::PreBullhorn() {
    Csis::Setup_BullhornPrefixStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_BullhornPrefixId, Csis::gSetup_BullhornPrefixHandle, this);
}

void EAXCop::BullhornArrest() {
    if (MiscSpeech::IsVehicleTypeOK()) {
        SoundAI *ai = SoundAI::Get();
        if (ai == nullptr) {
            return;
        }
        Csis::Arrest_BullhornArrestStruct data;
        data.speaker_id = this->mSpeakerID;
        data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
        Speech::Manager::ScheduleSpeech(data, Csis::Arrest_BullhornArrestId, Csis::gArrest_BullhornArrestHandle, this);
    }
}

void EAXCop::SuspectBehavior() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::AnytimeEvents_SuspectBehaviourStruct data;
    data.speaker_id = this->mSpeakerID;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_SuspectBehaviourId, Csis::gAnytimeEvents_SuspectBehaviourHandle, this);
}

void EAXCop::SuspectConfirmed() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::Setup_SuspectConfirmedStruct data;
    data.speaker_id = this->mSpeakerID;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_SuspectConfirmedId, Csis::gSetup_SuspectConfirmedHandle, this);
}

void EAXCop::SuspectOutrun() {
    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_SuspectOutrunStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_SuspectOutrunId, Csis::gAnytimeEvents_SuspectOutrunHandle, this);
}

void EAXCop::SuspectUTurn() {
    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_SuspectUTurnStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_SuspectUTurnId, Csis::gAnytimeEvents_SuspectUTurnHandle, this);
}

void EAXCop::FocusChange() {
    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_FocusChangeStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_FocusChangeId, Csis::gAnytimeEvents_FocusChangeHandle, this);
}

void EAXCop::Impact_Suspect_World() {
    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = this->mSpeakerID;
    data.world_object_type = (Speech::Manager::GetHistory().GetCount(kSPCH1_EventID_CollisionWorld) > 0) || ai->IsHighIntensity()
                                 ? Csis::Type_world_object_type_generic
                                 : Csis::Type_world_object_type_low_heat_collision;
    data.num_units = Csis::Type_num_units_one_unit_in_pursuit;
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = Csis::Type_num_units_multiple_units_in_pursuit;
    }
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollisionWorldId, Csis::gAnytimeEvents_CollisionWorldHandle, this);
}

void EAXCop::Impact_Suspect_Semi() {
    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = this->mSpeakerID;
    data.world_object_type = Csis::Type_world_object_type_semi;
    data.num_units = Csis::Type_num_units_one_unit_in_pursuit;
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = Csis::Type_num_units_multiple_units_in_pursuit;
    }
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollisionWorldId, Csis::gAnytimeEvents_CollisionWorldHandle, this);
}

void EAXCop::Impact_Suspect_Train() {
    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = this->mSpeakerID;
    data.world_object_type = Csis::Type_world_object_type_train;
    data.num_units = Csis::Type_num_units_one_unit_in_pursuit;
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = Csis::Type_num_units_multiple_units_in_pursuit;
    }
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollisionWorldId, Csis::gAnytimeEvents_CollisionWorldHandle, this);
}

void EAXCop::Impact_Suspect_Guardrail() {
    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = this->mSpeakerID;
    data.world_object_type = Csis::Type_world_object_type_guardrail;
    data.num_units = Csis::Type_num_units_one_unit_in_pursuit;
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = Csis::Type_num_units_multiple_units_in_pursuit;
    }
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollisionWorldId, Csis::gAnytimeEvents_CollisionWorldHandle, this);
}

void EAXCop::Impact_Suspect_GasStation() {
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuitState() != SoundAI::kActive) {
        return;
    }

    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = this->mSpeakerID;
    data.world_object_type = Csis::Type_world_object_type_gas_station;
    data.num_units = Csis::Type_num_units_one_unit_in_pursuit;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollisionWorldId, Csis::gAnytimeEvents_CollisionWorldHandle, this);
}

void EAXCop::Impact_Suspect_Spikebelt() {
    Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(COPSPEECH_MODULE);
    if (cop_speech != nullptr) {
        cop_speech->ReleaseResource();
    }
    Speech::Manager::ClearPlayback();

    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_CollisionWorldStruct data;
    data.speaker_id = this->mSpeakerID;
    data.world_object_type = Csis::Type_world_object_type_spike_belt;
    data.num_units = Csis::Type_num_units_one_unit_in_pursuit;
    if (ai->GetPursuit() != nullptr && ai->NumCopsWithLOS() > 1) {
        data.num_units = Csis::Type_num_units_multiple_units_in_pursuit;
    }
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollisionWorldId, Csis::gAnytimeEvents_CollisionWorldHandle, this);
}

void EAXCop::Impact_Suspect_Traffic(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_CiviStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = intensity;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollWorld_CiviId, Csis::gAnytimeEvents_CollWorld_CiviHandle, this);
}

void EAXCop::SuspectRollover(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_FlipStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = intensity;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollWorld_FlipId, Csis::gAnytimeEvents_CollWorld_FlipHandle, this);
}

void EAXCop::SuspectAirborne(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_AirStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = intensity;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollWorld_AirId, Csis::gAnytimeEvents_CollWorld_AirHandle, this);
}

void EAXCop::SuspectSpunout(Csis::Type_intensity intensity) {
    Csis::AnytimeEvents_CollWorld_SpinStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = intensity;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_CollWorld_SpinId, Csis::gAnytimeEvents_CollWorld_SpinHandle, this);
}

void EAXCop::SuspectBrake() {
    Csis::AnytimeEvents_SuspectBrakeStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_SuspectBrakeId, Csis::gAnytimeEvents_SuspectBrakeHandle, this);
}

void EAXCop::UnitDisabled(int other) {
    SoundAI *ai = SoundAI::Get();

    Csis::AnytimeEvents_UnitDisabledStruct data;
    data.self_other = other ? Csis::Type_self_other_context_about_other_unit : Csis::Type_self_other_context_about_self;
    data.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_UnitDisabledId, Csis::gAnytimeEvents_UnitDisabledHandle, this);
}

void EAXCop::PursuitUpdateReply() {
    Csis::AnytimeEvents_PursuitUpdateRepStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_PursuitUpdateRepId, Csis::gAnytimeEvents_PursuitUpdateRepHandle, this);
}

void EAXCop::Bailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::AnytimeEvents_BailoutStruct data;
    if (ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        data.speaker_id = this->mSpeakerID;
        data.bailout_type = Csis::Type_bailout_type_Damage_Sustained;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
    }
}

void EAXCop::DenyBailout() {
    Csis::AnytimeEvents_BailoutDenyStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_BailoutDenyId, Csis::gAnytimeEvents_BailoutDenyHandle, this);
}

void EAXCop::LoBailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::AnytimeEvents_BailoutStruct data;
    if (ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        data.speaker_id = this->mSpeakerID;
        data.bailout_type = Csis::Type_bailout_type_Generic_low_intensity;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
    }
}

void EAXCop::HiBailout() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::AnytimeEvents_BailoutStruct data;
    if (ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        data.speaker_id = this->mSpeakerID;
        data.bailout_type = Csis::Type_bailout_type_Generic_high_intensity;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
    }
}

void EAXCop::BailoutBadRoad() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    Csis::AnytimeEvents_BailoutStruct data;
    if (ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        data.speaker_id = this->mSpeakerID;
        data.bailout_type = Csis::Type_bailout_type_Road_conditions;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
    }
}

void EAXCop::BailoutTraffic() {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_BailoutStruct data;
    if (ai == nullptr) {
        return;
    }
    if (ai->GetPursuitDuration() >= ai->GetTune().MinPursuitDurationForBailouts()) {
        data.speaker_id = this->mSpeakerID;
        data.bailout_type = Csis::Type_bailout_type_Heavy_traffic;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_BailoutId, Csis::gAnytimeEvents_BailoutHandle, this);
    }
}

void EAXCop::CallForRB() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::StaticRoadblock_CallForRBStruct data;
    data.speaker_id = this->mSpeakerID;
    data.code = this->GetRandomizedCode();
    data.roadblock_type = ai->SpikesEnabled() ? Csis::Type_roadblock_type_Spikes : Csis::Type_roadblock_type_Roadblock_Generic_;
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_CallForRBId, Csis::gStaticRoadblock_CallForRBHandle, this);
}

void EAXCop::CallForSubRB() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::StaticRoadblock_CallForRB_subStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_CallForRB_subId, Csis::gStaticRoadblock_CallForRB_subHandle, this);
}

void EAXCop::RBReminder() {
    Csis::StaticRoadblock_RBReminderStruct data;
    data.speaker_id = this->mSpeakerID;
    data.code = this->GetRandomizedCode();
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_RBReminderId, Csis::gStaticRoadblock_RBReminderHandle, this);
}

void EAXCop::NegRBReply() {
    Csis::StaticRoadblock_NegativeRBReplyStruct data;
    if (!this->IsHeli()) {
        data.speaker_id = this->mSpeakerID;
        Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_NegativeRBReplyId, Csis::gStaticRoadblock_NegativeRBReplyHandle, this);
    }
}

void EAXCop::RBApproach() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    IRoadBlock *block = ai->GetRoadblock();
    if (block == nullptr) {
        return;
    }

    Csis::StaticRoadblock_RBApproachStruct data;
    data.speaker_id = this->mSpeakerID;
    data.roadblock_type = block->GetNumSpikeStrips() > 0 ? Csis::Type_roadblock_type_Spikes : Csis::Type_roadblock_type_Roadblock_Generic_;
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_RBApproachId, Csis::gStaticRoadblock_RBApproachHandle, this);
}

void EAXCop::RBEngage(bool spikes_hit) {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    IRoadBlock *block = ai->GetRoadblock();
    if (block == nullptr) {
        return;
    }

    Csis::StaticRoadblock_RBEngageStruct data;
    data.speaker_id = this->mSpeakerID;
    data.roadblock_engage_type = spikes_hit ? Csis::Type_roadblock_engage_type_spikes : Csis::Type_roadblock_engage_type_roadblock;
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_RBEngageId, Csis::gStaticRoadblock_RBEngageHandle, this);
}

void EAXCop::RBAverted() {
    Csis::StaticRoadblock_RBAvertedStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_RBAvertedId, Csis::gStaticRoadblock_RBAvertedHandle, this);
}

void EAXCop::PursuitApproaching() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }

    Csis::StaticRoadblock_PursuitApproachingStruct data;
    data.speaker_id = this->mSpeakerID;
    data.num_suspects = ai->AreRacersNearby() ? Csis::Type_num_suspects_multiple_suspects : Csis::Type_num_suspects_one_suspect;
    Speech::Manager::ScheduleSpeech(data, Csis::StaticRoadblock_PursuitApproachingId, Csis::gStaticRoadblock_PursuitApproachingHandle, this);
}

void EAXCop::HeadOn(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_HOStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = intensity;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_InterruptRam_HOId, Csis::gInterrupts_InterruptRam_HOHandle, this);
}

void EAXCop::TBoned(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_TBStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = intensity;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_InterruptRam_TBId, Csis::gInterrupts_InterruptRam_TBHandle, this);
}

void EAXCop::SideSwiped(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_SSStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = intensity;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_InterruptRam_SSId, Csis::gInterrupts_InterruptRam_SSHandle, this);
}

void EAXCop::RearEnded(Csis::Type_intensity intensity) {
    Csis::Interrupts_InterruptRam_REStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = intensity;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_InterruptRam_REId, Csis::gInterrupts_InterruptRam_REHandle, this);
}

void EAXCop::Spotted() {
    Csis::AnytimeEvents_SpottedStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = this->GetSpeed() > 80.0f ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_SpottedId, Csis::gAnytimeEvents_SpottedHandle, this);
}

void EAXCop::DirectionChange() {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr) {
        return;
    }
    unsigned int dir = ai->GetPlayerDirection(0);
    if (dir == 0) {
        return;
    }
    Csis::AnytimeEvents_DirectionHighStruct data;
    data.speaker_id = this->mSpeakerID;
    data.direction = static_cast<Csis::Type_direction>(dir);
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_DirectionHighId, Csis::gAnytimeEvents_DirectionHighHandle, this);
}

void EAXCop::CallForSwarming() {
    Csis::Backup_CallForSwarmingStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Backup_CallForSwarmingId, Csis::gBackup_CallForSwarmingHandle, this);
}

void EAXCop::SpotterWanted() {
    Csis::Setup_SpotterWantedStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Setup_SpotterWantedId, Csis::gSetup_SpotterWantedHandle, this);
}

void EAXCop::Offroad(unsigned int id, bool subsequent) {
    Csis::AnytimeEvents_OffroadMomentStruct data;
    data.offroad_moment_id = static_cast<Csis::Type_offroad_moment_id>(id);
    data.speaker_id = this->mSpeakerID;
    data.first_subsequent = subsequent ? Csis::Type_first_subsequent_subsequent_time : Csis::Type_first_subsequent_first_time;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_OffroadMomentId, Csis::gAnytimeEvents_OffroadMomentHandle, this);
}

void EAXCop::WeatherReport() {
    Csis::AnytimeEvents_WeatherReportStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_WeatherReportId, Csis::gAnytimeEvents_WeatherReportHandle, this);
}
