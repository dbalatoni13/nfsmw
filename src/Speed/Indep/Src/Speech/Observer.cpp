#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_moment_strm_hash.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Speech/Observer.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

namespace Speech {

Observer::Observer()
    : mObservations(),                                                                                                                           //
      mObserveMask(CarRam | Outcome),                                                                                                            //
      mLastEvent(None),                                                                                                                          //
      mNumCopsWithLOS(0),                                                                                                                        //
      mT_bullhorn(WorldTimer),                                                                                                                   //
      mT_unstable(WorldTimer),                                                                                                                   //
      mT_airborne(WorldTimer),                                                                                                                   //
      mT_flipped(WorldTimer),                                                                                                                    //
      mT_trackingOutcome(WorldTimer),                                                                                                            //
      mDotTrack(0.0f),                                                                                                                           //
      mOffroadHistory(0),                                                                                                                        //
      mCurrOffroadID(-1),                                                                                                                        //
      mTracking(0),                                                                                                                              //
      mT_gasstationexpl(WorldTimer),                                                                                                             //
      mPrevPursuitState(2),                                                                                                                      //
      mWeather(false),                                                                                                                           //
      mTunnel(false),                                                                                                                            //
      mRamCop(nullptr),                                                                                                                          //
      mGasStationPos(UMath::Vector3::kZero),                                                                                                     //
      mFwPlayer(UMath::Vector3::kZero),                                                                                                          //
      mFwRoad(UMath::Vector3::kZero),                                                                                                            //
      mMsgBlewPastCop(Hermes::Handler::Create<MGamePlayMoment, Observer, Observer>(this, &Observer::MessageBlewPastCop, "BlewByCop", 0)),        //
      mMsgGamePlayMoment(Hermes::Handler::Create<MGamePlayMoment, Observer, Observer>(this, &Observer::MessageGamePlayMoment, "MomentStrm", 0)), //
      mMsgNotifyEventCompletion(
          Hermes::Handler::Create<MNotifySpeechStatus, Observer, Observer>(this, &Observer::MessageEventComplete, UCrc32(UCRC32_Gameplay), 0)), //
      mMsgTunnelUpdate(Hermes::Handler::Create<MMiscSound, Observer, Observer>(this, &Observer::MessageTunnelUpdate, "TunnelUpdate", 0)) {
    mObservations.clear();
}

Observer::~Observer() {
    mObservations.clear();
    if (this->mMsgBlewPastCop != nullptr) {
        Hermes::Handler::Destroy(this->mMsgBlewPastCop);
    }
    if (this->mMsgNotifyEventCompletion != nullptr) {
        Hermes::Handler::Destroy(this->mMsgNotifyEventCompletion);
    }
    if (this->mMsgGamePlayMoment != nullptr) {
        Hermes::Handler::Destroy(this->mMsgGamePlayMoment);
    }
    if (this->mMsgTunnelUpdate != nullptr) {
        Hermes::Handler::Destroy(this->mMsgTunnelUpdate);
    }
}

void Observer::Update() {
    int y;
    int x;
    if (this->mState != kTransition) {
        if (this->mState == kCullCheck) {
            this->CullCheck();
            return;
        }
    }
    this->ChangeStateTo(kCullCheck);
}

void Observer::CullCheck() {
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuitState() == SoundAI::kActive) {
        this->Process();
    }
}

bool Observer::IsTransitionable() {
    return true;
}

void Observer::Reset() {
    this->SpeechFlow::Reset();
    this->mPrevPursuitState = 2;
    this->mNumCopsWithLOS = 0;
    this->mT_bullhorn = Timer(0);
    this->mT_unstable = Timer(0);
    this->mT_airborne = Timer(0);
    this->mT_flipped = Timer(0);
    this->mT_trackingOutcome = Timer(0);
    this->mT_gasstationexpl = Timer(0);
    this->mDotTrack = 0.0f;
    this->mOffroadHistory = 0;
    this->mTracking = 0;
    this->mWeather = false;
    this->mTunnel = false;
    this->mAirborneHeight = 0.0f;
    this->mRamCop = nullptr;
    this->mAirborneLength = 0.0f;
    this->mGasStationPos = UMath::Vector3::kZero;
    this->mFwPlayer = UMath::Vector3::kZero;
    this->mFwRoad = UMath::Vector3::kZero;
}

void Observer::Observe(int currobsrvation, int speaker, float f) {
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuit() == nullptr) {
        return;
    }
    if (ai->GetFocus() == SoundAI::kPursuitFlow) {
        return;
    }
    if (ai->GetFocus() == SoundAI::kTerminal) {
        return;
    }

    bool found = false;
    for (observations::iterator i = mObservations.begin(); i != mObservations.end(); i++) {
        if ((*i).observation == currobsrvation) {
            SpeechObservation &curr = *i;
            curr.time = WorldTimer;
            curr.speakerID = speaker;
            curr.force = curr.force < f ? f : curr.force;
            found = true;
        }
    }

    if (!found) {
        SpeechObservation newObs(static_cast<SpeechObservations>(currobsrvation), speaker, f);
        mObservations.push_back(newObs);
    }
    this->mLastEvent = static_cast<SpeechObservations>(currobsrvation);
}

void Observer::Process() {
    SoundAI *ai = SoundAI::Get();
    if (ai->GetPursuit() == nullptr) {
        return;
    }
    if (ai->GetFocus() == SoundAI::kPursuitFlow && ai->GetPursuit()->TimeUntilBusted() < 0.5f) {
        return;
    }
    if (ai->GetFocus() == SoundAI::kTerminal) {
        return;
    }

    observations::iterator i = this->mObservations.begin();
    while (i != this->mObservations.end()) {
        SpeechObservation &obs = *i;
        EAXCop *speaker = ai->GetCop(obs.speakerID);
        if (speaker != nullptr) {
            speaker->Collision(obs.observation, obs.force, ai->GetRandomCop(0));
        }
        this->mObservations.erase(i);
        if (this->mObservations.empty()) {
            break;
        }
        i = this->mObservations.begin();
    }
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player != nullptr) {
        player->GetSimable()->GetRigidBody()->GetForwardVector(this->mFwPlayer);
    }
    this->AssessArrest();
    if ((this->mTracking & Arrest) == 0) {
        this->AssessLOS();
        this->AssessFlippage();
        this->Assess180();
        this->AssessOutcome();
        this->AssessBraking();
        this->AssessOutrun();
        this->AssessOffroad();
        this->GasStationAftermath();
    }
    this->mNumCopsWithLOS = ai->NumCopsWithLOS();
}

float Observer::CalcFWVec_Road_Car() {
    float rval = -666.6f;
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player != nullptr) {
        IVehicleAI *vai = nullptr;
        player->GetSimable()->QueryInterface(&vai);
        if (vai != nullptr) {
            WRoadNav *nav = vai->GetDriveToNav();
            UMath::Vector3 fwRoad = nav->GetForwardVector();
            UMath::Vector3 fwCar;
            player->GetSimable()->GetRigidBody()->GetForwardVector(fwCar);
            rval = VU0_v3dotprod(fwCar, fwRoad);
            this->mFwPlayer = fwCar;
            this->mFwRoad = fwRoad;
        }
    }
    return rval;
}

void Observer::GasStationAftermath() {
    SoundAI *ai = SoundAI::Get();

    if ((WorldTimer - this->mT_gasstationexpl).GetSeconds() < 15.0f) {
        for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
            EAXCop *cop = iter->cop;
            UMath::Vector3 cop_pos = cop->GetPosition();
            float rel_dist_expl = UMath::Distance(this->mGasStationPos, cop_pos);
            if (rel_dist_expl < 50.0f) {
                cop->CallforEV(2);
            }
        }
    }
}

void Observer::AssessArrest() {
    float t_busted = 0.0f;
    SoundAI *ai = SoundAI::Get();
    Speech::Module *cop_speech = nullptr;
    if (ai->GetPursuit() != nullptr) {
        t_busted = ai->GetPursuit()->TimeUntilBusted();
        bClamp(t_busted, 0.0f, 1.0f);
        if ((t_busted > 0.8f) && !Manager::IsQueued(kSPCH1_EventID_HeliBullhornArrest, 4) &&
            !Manager::IsCopSpeechPlaying(kSPCH1_EventID_HeliBullhornArrest) && !Manager::IsQueued(kSPCH1_EventID_BullhornArrest, 4) &&
            !Manager::IsCopSpeechPlaying(kSPCH1_EventID_BullhornArrest)) {
            GRaceParameters *parms = GRaceStatus::Exists() ? GRaceStatus::Get().GetRaceParameters() : nullptr;
            bool isEpicPursuit = false;
            if (parms != nullptr && parms->GetIsEpicPursuitRace()) {
                isEpicPursuit = true;
            }
            bool isEpicCellCallQueued = false;
            if (isEpicPursuit && (Manager::IsQueued(kSPCH1_EventID_CellCall, 4) || Manager::IsCopSpeechPlaying(kSPCH1_EventID_CellCall))) {
                isEpicCellCallQueued = true;
            }

            if (!isEpicCellCallQueued) {
                this->mTracking |= Arrest;
                EAXCop *cop = ai->GetRandomActiveCop(0, true);
                if (cop != nullptr) {
                    cop_speech = Manager::GetSpeechModule(COPSPEECH_MODULE);
                    if (cop_speech != nullptr) {
                        cop_speech->ReleaseResource();
                    }
                    Manager::ClearPlayback();
                    if (ai->GetHeli() != nullptr && (ai->GetHeli()->GetDistance() < 100.0f) && ai->GetHeli()->IsActive()) {
                        ai->GetHeli()->BullhornArrest();
                    } else {
                        cop->BullhornArrest();
                    }
                }
            }
        } else {
            this->mTracking &= ~Arrest;
        }
    }

    if (cop_speech == nullptr) {
        cop_speech = Manager::GetSpeechModule(COPSPEECH_MODULE);
        if (cop_speech == nullptr) {
            return;
        }
    }

    if (cop_speech->GetSFXOBJ_Speech() != nullptr) {
        float ftmp = t_busted * 32767.0f;
        if (ftmp > 32767.0f) {
            ftmp = 32767.0f;
        }
        if (ftmp < 0.0f) {
            ftmp = 0.0f;
        }
        int q15scale = static_cast<int>(ftmp);
        cop_speech->GetSFXOBJ_Speech()->SetDMIX_Input(5, q15scale);
    }
}

void Observer::AssessLOS() {
    SoundAI *ai = SoundAI::Get();
    if (((ai->GetFocus() == SoundAI::kPursuitFlow) || (ai->GetFocus() == SoundAI::kLost)) && (this->mTracking & Lost)) {
        this->mTracking &= ~Lost;
    }

    if ((ai->GetTimeInView() > 0.0f) && (this->mTracking & Lost)) {
        copList losList;
        losList.reserve(ai->GetActors().size());
        for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
            EAXCop *cop = iter->cop;
            if (cop->IsActive() && cop->HasLOS()) {
                losList.push_back(cop);
            }
        }

        if (losList.size() > 1) {
            std::sort(losList.begin(), losList.end());
        }

        if (!losList.empty()) {
            EAXCop *closest = losList.front();
            closest->RegainVisual();
            this->mTracking &= ~Lost;
        }
    }
}

void Observer::NotifyAirborne(float alt, float t) {
    SoundAI *ai = SoundAI::Get();

    if (ai->NumCopsWithLOS() > 0 && alt >= ai->GetTune().MinHeightAirborne()) {
        if (t >= ai->GetTune().HangTimeForCommentary()) {
            EAXCop *cop = ai->FindClosestCop(true, true);
            if (cop != nullptr) {
                cop->SuspectAirborne(ai->GetPlayerSpeed() >= ai->GetTune().SpeedThreshFlyFlipIntensity() ? Csis::Type_intensity_High
                                                                                                         : Csis::Type_intensity_Normal);
            }
        }
    }
}

void Observer::AssessFlippage() {
    SoundAI *ai = SoundAI::Get();
    if (ai->NumCopsWithLOS() > 0) {
        float pct_tractive_tires;
        bool flipped = false;
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);
        if (player != nullptr) {
            ISimable *simable = player->GetSimable();
            IRigidBody *irb = simable->GetRigidBody();
            UMath::Vector3 upVec;
            irb->GetUpVector(upVec);
            flipped = upVec.y < 0.0f;
            if (!flipped) {
                this->mT_flipped = WorldTimer;
            }
        }

        float t_flipped = (WorldTimer - this->mT_flipped).GetSeconds();
        if (flipped && (t_flipped >= ai->GetTune().FlipTimeForCommentary())) {
            EAXCop *cop = ai->FindClosestCop(true, true);
            if (cop != nullptr) {
                cop->SuspectRollover(ai->GetPlayerSpeed() > ai->GetTune().SpeedThreshFlyFlipIntensity() ? Csis::Type_intensity_High
                                                                                                        : Csis::Type_intensity_Normal);
            }
        }
    }
}

void Observer::Assess180() {
    SoundAI *ai = SoundAI::Get();
    if (ai->NumCopsWithLOS() > 0) {
        float fwDot = this->CalcFWVec_Road_Car();
        if ((fwDot >= 0.7f) && ((this->mTracking & UTurnResult) == 0)) {
            this->mT_unstable = WorldTimer;
        }

        static UMath::Vector3 fw0 = UMath::Vector3::kZero;
        static UMath::Vector3 startpos = UMath::Vector3::kZero;
        static char datapts = 0;

        if (fwDot < 0.7f) {
            if (!(this->mTracking & UTurn)) {
                if ((this->mTracking & UTurnResult) == 0) {
                    this->mDotTrack = fwDot;
                    fw0 = this->mFwRoad;
                    this->mTracking |= UTurn;
                    datapts = 0;
                    startpos = ai->GetPlayerPos();
                }
            }
        }

        if ((this->mTracking & UTurn) == 0) {
            return;
        }
        if (fwDot > -0.7f) {
            if ((this->mTracking & UTurnResult) == 0) {
                if (fwDot > this->mDotTrack) {
                    this->mTracking &= ~UTurn;
                    this->mT_unstable = WorldTimer;
                    datapts = 0;
                    startpos = UMath::Vector3::kZero;
                } else {
                    this->mDotTrack = fwDot;
                    datapts++;
                }
            }
        }

        if (fwDot <= -0.7f) {
            if (datapts >= ai->GetTune().MinContigFramesFor180()) {
                if ((this->mTracking & UTurnResult) == 0) {
                    UMath::Vector3 endpos = ai->GetPlayerPos();
                    float dist_from_start = UMath::Distance(startpos, endpos);
                    if (dist_from_start > ai->GetTune().MaxRangeFor180()) {
                        this->mTracking &= ~UTurn;
                        this->mT_unstable = WorldTimer;
                        datapts = 0;
                        startpos = UMath::Vector3::kZero;
                    } else {
                        this->mTracking |= UTurnResult;
                        this->mT_unstable = WorldTimer;
                    }
                }
            }
        }

        if ((this->mTracking & UTurnResult) != 0) {
            float delta_dot = VU0_v3dotprod(this->mFwPlayer, fw0);
            float t_unstable = (WorldTimer - this->mT_unstable).GetSeconds();
            if (delta_dot > -0.7f) {
                this->mTracking &= ~UTurn;
                this->mTracking &= ~UTurnResult;
                this->mT_unstable = WorldTimer;
                datapts = 0;
            } else {
                if (t_unstable > ai->GetTune().MaxTimeFor180()) {
                    EAXCop *cop = ai->FindClosestCop(true, true);
                    if (cop != nullptr) {
                        cop->SuspectUTurn();
                        this->mTracking &= ~UTurn;
                        this->mTracking &= ~UTurnResult;
                        this->mT_unstable = WorldTimer;
                        datapts = 0;
                    } else {
                        return;
                    }
                } else {
                    return;
                }
            }
        }
    } else {
        this->mTracking &= ~UTurn;
        this->mTracking &= ~UTurnResult;
        this->mT_unstable = WorldTimer;
    }
}

void Observer::AssessOutcome() {
    SoundAI *ai = SoundAI::Get();
    if (ai->GetFocus() != SoundAI::kStrategyFlow) {
        return;
    }
    if (ai->GetPursuitState() == SoundAI::kActive) {
        float t_busted = ai->GetPursuit()->TimeUntilBusted();
        if ((t_busted > 0.0f) && (t_busted < 1.0f) || ai->GetPursuit()->IsCollapseActive()) {
            EAXCop *cop = ai->GetRandomActiveCop(1, false);
            if (cop != nullptr && !cop->IsHeli() && !Manager::IsQueued(kSPCH1_EventID_AnticipateSuccess, 4)) {
                cop->AnticipateSuccess();
            }
        }

        if ((this->mTracking & Outcome) == 0) {
            return;
        }
        if (t_busted < 1.0f) {
            float t_tracking = (WorldTimer - this->mT_trackingOutcome).GetSeconds();
            if (t_tracking >= ai->GetTune().OutcomeTrackTime()) {
                copList primvisual;
                primvisual.reserve(ai->GetActors().size());
                for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
                    EAXCop *cop = iter->cop;
                    if (cop->IsActive() && cop->HasLOS() && cop->IsPrimary()) {
                        primvisual.push_back(cop);
                    }
                }

                if (primvisual.empty()) {
                    return;
                } else {
                    bool say_outcome = true;
                    if (primvisual.size() > 1) {
                        std::sort(primvisual.begin(), primvisual.end());
                    }

                    EAXCop *cop = *primvisual.begin();
                    if (this->mTracking & CarRam) {
                        if (this->mRamCop != nullptr) {
                            say_outcome = false;
                            if (this->mRamCop->GetTimeLastRammed() <= t_tracking) {
                                this->mRamCop->AnticipateSuccess();
                            } else {
                                this->mRamCop->AnticipateFail();
                            }
                        }
                        this->mTracking &= ~CarRam;
                        ai->ClearImmunity();
                    }

                    if (say_outcome) {
                        if (ai->GetPlayerSpeed() >= ai->GetTune().OutcomeFailSpeed()) {
                            cop->OutcomeFail(-1);
                        } else if (!cop->IsHeli()) {
                            cop->AnticipateSuccess();
                        }
                    }

                    this->mTracking &= ~Outcome;
                }
            }
        }
    }
}

void Observer::AssessBraking() {
    SoundAI *ai = SoundAI::Get();
    if (ai->NumCopsWithLOS() > 0) {
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);
        if (player != nullptr) {
            IInput *input;
            player->GetSimable()->QueryInterface(&input);
            if (input != nullptr) {
                float steer = this->CalcFWVec_Road_Car();
                float brake = UMath::Clamp(input->GetControls().fBrake, 0.0f, 1.0f);
                float ebrake = UMath::Clamp(input->GetControls().fHandBrake, 0.0f, 1.0f);

                static float speed0 = ai->GetPlayerSpeed();
                static Timer t_brake_start = WorldTimer;

                float t_last_crashed = bMin(ai->GetTimeLastCrashed(), ai->GetTimeLastNailedCop());
                if ((this->mTracking & Braking) == 0) {
                    if ((steer >= 0.7f) && ((brake >= 1.0f) || (ebrake >= 1.0f)) && (ai->GetPlayerSpeed() > 60.0f) && (t_last_crashed > 5.0f)) {
                        this->mTracking |= Braking;
                        speed0 = ai->GetPlayerSpeed();
                        t_brake_start = WorldTimer;
                    }

                    if ((this->mTracking & Braking) == 0) {
                        return;
                    }
                }

                if ((steer < 0.7f) || ((brake < 0.5f) && (ebrake < 0.5f)) || (t_last_crashed < 5.0f)) {
                    this->mTracking &= ~Braking;
                } else {
                    float curr_speed = ai->GetPlayerSpeed();
                    float pct_decrease = curr_speed / speed0;
                    if (pct_decrease < (1.0f - ai->GetTune().CrashSlowdownPct())) {
                        EAXCop *cop = ai->FindClosestCop(true, true);
                        if (cop != nullptr) {
                            cop->SuspectBrake();
                            this->mTracking &= ~Braking;
                        }
                    }
                }
            }
        }
    }
}

void Observer::AssessOutrun() {
    SoundAI *ai = SoundAI::Get();
    if (ai->NumCopsWithLOS() <= 0) {
        return;
    }

    if (ai->GetTimeInView() < 2.0f) {
        return;
    }

    copList visual;
    visual.reserve(ai->GetActors().size());

    for (copMap::const_iterator iter = ai->GetActors().begin(); iter != ai->GetActors().end(); ++iter) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive() && cop->HasLOS()) {
            visual.push_back(cop);
        }
    }

    if (visual.empty()) {
        return;
    }

    unsigned char num_being_outrun = 0;
    for (copList::iterator i = visual.begin(); i != visual.end(); ++i) {
        EAXCop *cop = *i;
        if (cop->GetTimeLastClosing() > 8.0f) {
            num_being_outrun++;
        }
    }

    if ((num_being_outrun == visual.size()) && !ai->AreCopsAhead()) {
        EAXCop *cop = ai->GetRandomActiveCop(0, false);
        if (cop != nullptr) {
            cop->SuspectOutrun();
        }
    }
}

void Observer::AssessOffroad() {
    SoundAI *ai = SoundAI::Get();

    if (ai->NumCopsWithLOS() <= 0) {
        return;
    }
    if (ai->GetTimeInView() < 2.0f) {
        return;
    }

    unsigned int id = ai->GetPlayerOffroadID();
    if (this->mCurrOffroadID == id) {
        return;
    }

    {
        EAXCop *cop;
    }

    {
        EAXCop *cop = ai->FindClosestCop(true, true);
        if (cop != nullptr) {
            this->mCurrOffroadID = static_cast<int>(id);
            Csis::Type_offroad_moment_id speech_id;
            if (MiscSpeech::GetSPAMLocation(id, speech_id)) {
                cop->Offroad(speech_id, (this->mOffroadHistory & speech_id) != 0);
            }
        }
    }
}

void Observer::MessageEventComplete(const MNotifySpeechStatus &message) {
    ScheduledSpeechEvent *speech = message.GetEvent();
    if (speech != nullptr) {
        static int spkrID911 = 0;
        bool outcome_req = false;
        this->mOutcomeIntensity = Csis::Type_intensity_High;

        switch (speech->ID) {
            case kSPCH1_EventID_OffroadMoment: {
                Csis::AnytimeEvents_OffroadMomentStruct *data = static_cast<Csis::AnytimeEvents_OffroadMomentStruct *>(speech->GetData(nullptr));
                if (data != nullptr) {
                    unsigned int offroad_id = data->offroad_moment_id;
                    if ((this->mOffroadHistory & offroad_id) == 0) {
                        this->mOffroadHistory |= offroad_id;
                    }
                }
                break;
            }
            case kSPCH1_EventID_CollisionWorld: {
                Csis::AnytimeEvents_CollisionWorldStruct *data = static_cast<Csis::AnytimeEvents_CollisionWorldStruct *>(speech->GetData(nullptr));
                outcome_req = true;
                if (data != nullptr && data->world_object_type == 0x40) {
                    this->mOutcomeIntensity = Csis::Type_intensity_Normal;
                }
                break;
            }
            case kSPCH1_EventID_CollWorld_Flip: {
                Csis::AnytimeEvents_CollWorld_FlipStruct *data = static_cast<Csis::AnytimeEvents_CollWorld_FlipStruct *>(speech->GetData(nullptr));
                outcome_req = true;
                if (data != nullptr) {
                    this->mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
                }
                break;
            }
            case kSPCH1_EventID_BullhornArrest: {
                Csis::Arrest_BullhornArrestStruct *data = static_cast<Csis::Arrest_BullhornArrestStruct *>(speech->GetData(nullptr));
                outcome_req = true;
                if (data != nullptr) {
                    this->mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
                }
                break;
            }
            case kSPCH1_EventID_Arrest: {
                Csis::Arrest_ArrestStruct *data = static_cast<Csis::Arrest_ArrestStruct *>(speech->GetData(nullptr));
                outcome_req = true;
                if (data != nullptr) {
                    this->mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
                }
                break;
            }
            case kSPCH1_EventID_RBEngage:
            case kSPCH1_EventID_ExtraRBEngage:
            case kSPCH1_EventID_HeliBullhornArrest: {
                outcome_req = true;
                SoundAI *ai = SoundAI::Get();
                if (ai != nullptr && ai->GetRBFlow() != nullptr) {
                    outcome_req = ai->GetRBFlow()->HasNailedSpikes() == 0;
                }
                break;
            }
            case kSPCH1_EventID_StrategyExecute: {
                Csis::RollingStrategy_StrategyExecuteStruct *data =
                    static_cast<Csis::RollingStrategy_StrategyExecuteStruct *>(speech->GetData(nullptr));
                outcome_req = true;
                if (data != nullptr) {
                    this->mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
                }
                break;
            }
            case kSPCH1_EventID_IntentToRam: {
                Csis::AnytimeEvents_IntentToRamStruct *data = static_cast<Csis::AnytimeEvents_IntentToRamStruct *>(speech->GetData(nullptr));
                if (data != nullptr) {
                    if (data->speaker_id == 2) {
                        this->mTracking &= ~CarRam;
                    } else {
                        SoundAI *ai = SoundAI::Get();
                        this->mTracking |= CarRam;
                        if (ai != nullptr) {
                            this->mRamCop = ai->GetCop(data->speaker_id);
                            if (this->mRamCop != nullptr) {
                                ai->MakeCopsImmune();
                            }
                        }
                    }
                    outcome_req = true;
                    this->mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
                }
                break;
            }
            case kSPCH1_EventID_LostVisual:
            case kSPCH1_EventID_LostSuspect:
            case kSPCH1_EventID_HeliLostVisual:
                if ((this->mTracking & Lost) == 0) {
                    this->mTracking |= Lost;
                }
                break;
            case kSPCH1_EventID_ReInitPursuit:
                if ((this->mTracking & Lost) != 0) {
                    this->mTracking &= ~Lost;
                }
                if (Manager::IsQueued(kSPCH1_EventID_RegainVisual, 4)) {
                    Manager::RecallSpeechEvent(kSPCH1_EventID_RegainVisual);
                }
                break;
            case kSPCH1_EventID_Disp911Report: {
                spkrID911 = MiscSpeech::MoreDetails(0);
                SoundAI *ai = SoundAI::Get();
                if (ai != nullptr) {
                    ai->Force911State();
                }
                break;
            }
            case kSPCH1_EventID_MoreDetails: {
                SoundAI *ai = SoundAI::Get();
                if (ai != nullptr && ai->GetPursuitState() != SoundAI::kActive) {
                    if (ai->GetPlayerCarColor() != 0 && MiscSpeech::IsVehicleTypeOK() && bRandom(1.0f) > 0.5f) {
                        ai->GetDispatch()->VehicleDescription();
                    } else {
                        ai->GetDispatch()->NoVehicleDescription();
                    }
                    if (spkrID911 > 0) {
                        MiscSpeech::Unit911Reply(spkrID911);
                    }
                }
                break;
            }
            default:
                break;
        }

        if (outcome_req) {
            this->mTracking |= Outcome;
            this->mT_trackingOutcome = WorldTimer;
        }
    }
}

void Observer::MessageBlewPastCop(const MGamePlayMoment &message) {
    SoundAI *ai = SoundAI::Get();
    if ((ai->GetFocus() == SoundAI::kStrategyFlow) &&
        ((ai->GetPursuitState() == SoundAI::kActive) || (ai->GetPursuitState() == SoundAI::kSearching)) && (ai->NumCopsWithLOS() < 2)) {
        unsigned int copID = message.GethSimable();
        for (copMap::const_iterator i = ai->GetActors().begin(); i != ai->GetActors().end(); ++i) {
            EAXCop *cop = i->cop;
            ISimable *simable = ISimable::FindInstance(cop->GetHandle());
            if (simable != nullptr && (simable->GetWorldID() == copID)) {
                IVehicle *vehicle = nullptr;
                if (simable->QueryInterface(&vehicle)) {
                    float cop_speed = MPS2MPH(vehicle->GetSpeed());
                    if (ai->GetPlayerSpeed() > (cop_speed + 50.0f)) {
                        bool cop_in_rb = false;
                        IRoadBlock *block = ai->GetRoadblock();
                        if (block != nullptr) {
                            IVehicle *car = block->IsComprisedOf(simable->GetOwnerHandle());
                            if (car != nullptr) {
                                cop_in_rb = true;
                            }
                        }

                        if (!cop_in_rb) {
                            cop->Spotted();
                        }
                    }
                }
            }
        }
    }
}

void Observer::MessageTunnelUpdate(const MMiscSound &message) {
    this->mTunnel = message.GetSoundID() > 0;
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr && this->mTunnel && ai->GetHeli() != nullptr && ai->GetHeli()->IsActive() && ai->GetHeli()->HasLOS()) {
        ai->GetHeli()->HazardAlert(Csis::Type_heli_hazard_alert_type_approaching_tunnel);
    }
}

// total size: 0x4
// Decl: 1350
struct ComComparator {
    // Decl: 1351
    ComComparator(const UTL::COM::IUnknown *me) : Me(me) {}
    // Decl: 1352
    bool operator()(const UTL::COM::IUnknown *them) const {
        return UTL::COM::ComparePtr(them, Me);
    }

    const UTL::COM::IUnknown *Me; // offset 0x0, size 0x4, Decl: 1353
};

void Observer::MessageGamePlayMoment(const MGamePlayMoment &message) {
    if (message.GetAttribKey() == Attrib::Hash::aud_moment_strm::key_expl_gas_station) {
        SoundAI *ai = SoundAI::Get();
        if (ai->GetFocus() == SoundAI::kStrategyFlow) {
            bool player_started = false;
            ISimable *simable = ISimable::FindInstance(reinterpret_cast<HSIMABLE>(message.GethSimable()));

            if (simable != nullptr) {
                ICause *causer = ICause::FindInstance(simable->GetCausality());
                if (IVehicle::FindIf(VEHICLE_PLAYERS, ComComparator(causer)) != nullptr) {
                    player_started = true;
                }
            }

            if (player_started) {
                EAXCop *closest = ai->FindClosestCop(false, true);
                if (closest != nullptr && closest->HasLOS()) {
                    closest->Collision(Collision_Suspect_GasStation, 1.0f, closest);
                }
            }

            this->mT_gasstationexpl = WorldTimer;
            this->mGasStationPos = UMath::Vector4To3(message.GetPosition());
        }
    }
}

}; // namespace Speech
