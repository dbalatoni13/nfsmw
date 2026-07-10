#include "Observer.h"
#include "EAXDispatch.h"
#include "RoadblockFlow.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace UTL {
namespace COM {
template <> inline bool IUnknown::QueryInterface<IInput>(IInput **out) {
    HINTERFACE handle = IInput::_IHandle();

    *out = (IInput *)_mCOMObject->_mInterfaces.Find(handle);
    return *out;
}
}
}

namespace MiscSpeech {
bool IsVehicleTypeOK();
int MoreDetails(int spkrID);
int Unit911Reply(int spkrID);
bool GetSPAMLocation(int SPAMID, Csis::Type_offroad_moment_id &id);
}

namespace Speech {

struct ComComparator {
    const UTL::COM::IUnknown *Me;

    ComComparator(const UTL::COM::IUnknown *me) : Me(me) {}

    bool operator()(IVehicle *vehicle) const {
        return UTL::COM::ComparePtr(vehicle, Me);
    }
};

Observer::Observer()
    : mObservations(), //
      mObserveMask(Braking | Outcome), //
      mLastEvent(kNone), //
      mNumCopsWithLOS(0), //
      mT_bullhorn(WorldTimer), //
      mT_unstable(WorldTimer), //
      mT_airborne(WorldTimer), //
      mT_flipped(WorldTimer), //
      mT_trackingOutcome(WorldTimer), //
      mDotTrack(0.0f), //
      mOffroadHistory(0), //
      mCurrOffroadID(-1), //
      mTracking(0), //
      mT_gasstationexpl(WorldTimer), //
      mPrevPursuitState(2), //
      mWeather(false), //
      mTunnel(false), //
      mOutcomeIntensity(Csis::Type_intensity_Normal), //
      mRamCop(0), //
      mAirborneHeight(0.0f), //
      mAirborneLength(0.0f), //
      mGasStationPos(UMath::Vector3::kZero), //
      mFwPlayer(UMath::Vector3::kZero), //
      mFwRoad(UMath::Vector3::kZero), //
      mMsgBlewPastCop(0), //
      mMsgGamePlayMoment(0), //
      mMsgNotifyEventCompletion(0), //
      mMsgTunnelUpdate(0) {
    mMsgBlewPastCop = Hermes::Handler::Create<MGamePlayMoment, Observer, Observer>(this, &Observer::MessageBlewPastCop, "BlewByCop", 0);
    mMsgGamePlayMoment = Hermes::Handler::Create<MGamePlayMoment, Observer, Observer>(this, &Observer::MessageGamePlayMoment, "MomentStrm", 0);
    mMsgNotifyEventCompletion =
        Hermes::Handler::Create<MNotifySpeechStatus, Observer, Observer>(this, &Observer::MessageEventComplete, UCrc32(0x20d60dbf), 0);
    mMsgTunnelUpdate = Hermes::Handler::Create<MMiscSound, Observer, Observer>(this, &Observer::MessageTunnelUpdate, "TunnelUpdate", 0);
}

Observer::~Observer() {
    if (mMsgBlewPastCop) {
        Hermes::Handler::Destroy(mMsgBlewPastCop);
        mMsgBlewPastCop = 0;
    }
    if (mMsgGamePlayMoment) {
        Hermes::Handler::Destroy(mMsgGamePlayMoment);
        mMsgGamePlayMoment = 0;
    }
    if (mMsgNotifyEventCompletion) {
        Hermes::Handler::Destroy(mMsgNotifyEventCompletion);
        mMsgNotifyEventCompletion = 0;
    }
    if (mMsgTunnelUpdate) {
        Hermes::Handler::Destroy(mMsgTunnelUpdate);
        mMsgTunnelUpdate = 0;
    }
}

void Observer::Update() {
    switch (mState) {
    case kTransition:
        return;
    case kCullCheck:
        CullCheck();
        return;
    default:
        ChangeStateTo(kCullCheck);
        return;
    }
}

void Observer::CullCheck() {
    Process();
}

bool Observer::IsTransitionable() {
    return mState == kTransition;
}

void Observer::Reset() {
    mObservations.clear();
    mObserveMask = Braking | Outcome;
    mLastEvent = kNone;
    mNumCopsWithLOS = 0;
    mT_bullhorn = WorldTimer;
    mT_unstable = WorldTimer;
    mT_airborne = WorldTimer;
    mT_flipped = WorldTimer;
    mT_trackingOutcome = WorldTimer;
    mDotTrack = 0.0f;
    mOffroadHistory = 0;
    mCurrOffroadID = -1;
    mTracking = 0;
    mT_gasstationexpl = WorldTimer;
    mPrevPursuitState = 2;
    mWeather = false;
    mTunnel = false;
    mOutcomeIntensity = Csis::Type_intensity_Normal;
    mRamCop = 0;
    mAirborneHeight = 0.0f;
    mAirborneLength = 0.0f;
    mGasStationPos = UMath::Vector3::kZero;
    mFwPlayer = UMath::Vector3::kZero;
    mFwRoad = UMath::Vector3::kZero;
    mBusy = 0;
    mState = kCullCheck;
    mLastState = kTransition;
}

void Observer::Observe(int currobsrvation, int speaker, float score) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->GetPursuit() && (ai->GetFocus() != SoundAI::kPursuitFlow) && (ai->GetFocus() != SoundAI::kTerminal)) {
        bool found = false;
        for (observations::iterator i = mObservations.begin(); i != mObservations.end(); i++) {
            SpeechObservation &curr = *i;
            if (curr.observation == currobsrvation) {
                curr.time = WorldTimer;
                curr.speakerID = speaker;
                float force = curr.force;
                if (force < score) {
                    force = score;
                }
                curr.force = force;
                found = true;
            }
        }
        if (!found) {
            SpeechObservation newObs;
            newObs.speakerID = speaker;
            newObs.time = WorldTimer;
            newObs.force = score;
            newObs.observation = static_cast<SpeechObservations>(currobsrvation);
            mObservations.push_back(newObs);
        }
        mLastEvent = static_cast<SpeechObservations>(currobsrvation);
    }
}

void Observer::Process() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->GetPursuit() && (((ai->GetFocus() != SoundAI::kPursuitFlow) || (ai->GetPursuit()->TimeUntilBusted() >= 0.5f)) &&
                             (ai->GetFocus() != SoundAI::kTerminal))) {
        observations::iterator i = mObservations.begin();
        while (i != mObservations.end()) {
            SpeechObservation &obs = *i;
            EAXCop *speaker = ai->GetCop(obs.speakerID);
            if (speaker) {
                speaker->Collision(obs.observation, obs.force, ai->GetRandomCop(0));
            }
            mObservations.erase(i);
            if (mObservations.empty()) {
                break;
            }
            i = mObservations.begin();
        }
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);
        if (player) {
            player->GetSimable()->GetRigidBody()->GetForwardVector(mFwPlayer);
        }
        AssessArrest();
        if ((mTracking & Arrest) == 0) {
            AssessLOS();
            AssessFlippage();
            Assess180();
            AssessOutcome();
            AssessBraking();
            AssessOutrun();
            AssessOffroad();
            GasStationAftermath();
        }
        mNumCopsWithLOS = ai->NumCopsWithLOS();
    }
}

float Observer::CalcFWVec_Road_Car() {
    float rval = 0.0f;
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (player) {
        IVehicleAI *vai = 0;
        player->GetSimable()->QueryInterface(&vai);
        if (vai) {
            WRoadNav *nav = vai->GetDriveToNav();
            UMath::Vector3 fwRoad = nav->GetForwardVector();
            UMath::Vector3 fwCar;
            player->GetSimable()->GetRigidBody()->GetForwardVector(fwCar);
            rval = UMath::Dot(fwCar, fwRoad);
            mFwPlayer = fwCar;
            mFwRoad = fwRoad;
        }
    }
    return rval;
}

void Observer::GasStationAftermath() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    if ((WorldTimer - mT_gasstationexpl).GetSeconds() < 15.0f) {
        copMap::const_iterator iter = ai->GetActors().begin();
        while (iter != ai->GetActors().end()) {
            EAXCop *cop = iter->cop;
            UMath::Vector3 cop_pos = cop->GetPosition();
            float rel_dist_expl = UMath::Distance(mGasStationPos, cop_pos);
            if (rel_dist_expl < 50.0f) {
                cop->CallforEV(2);
            }
            ++iter;
        }
    }
}

void Observer::AssessArrest() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    float t_busted = 0.0f;
    Speech::Module *cop_speech = nullptr;
    if (ai->GetPursuit()) {
        t_busted = ai->GetPursuit()->TimeUntilBusted();
        if ((t_busted > 0.8f) && !Manager::IsQueued(kSPCH1_EventID_HeliBullhornArrest, 4) &&
            !Manager::IsCopSpeechPlaying(kSPCH1_EventID_HeliBullhornArrest) && !Manager::IsQueued(kSPCH1_EventID_BullhornArrest, 4) &&
            !Manager::IsCopSpeechPlaying(kSPCH1_EventID_BullhornArrest)) {
            GRaceParameters *parms = GRaceStatus::Exists() ? GRaceStatus::Get().GetRaceParameters() : nullptr;
            bool isEpicPursuit = false;
            if (parms && parms->GetIsEpicPursuitRace()) {
                isEpicPursuit = true;
            }
            bool isEpicCellCallQueued = false;
            if (isEpicPursuit && (Manager::IsQueued(kSPCH1_EventID_CellCall, 4) || Manager::IsCopSpeechPlaying(kSPCH1_EventID_CellCall))) {
                isEpicCellCallQueued = true;
            }

            if (!isEpicCellCallQueued) {
                mTracking |= Arrest;
                EAXCop *cop = ai->GetRandomActiveCop(0, true);
                if (cop) {
                    cop_speech = Manager::GetSpeechModule(COPSPEECH_MODULE);
                    if (cop_speech) {
                        cop_speech->ReleaseResource();
                    }
                    Manager::ClearPlayback();
                    if (ai->GetHeli() && (ai->GetHeli()->GetDistance() < 100.0f) && ai->GetHeli()->IsActive()) {
                        ai->GetHeli()->BullhornArrest();
                    } else {
                        cop->BullhornArrest();
                    }
                }
            }
        } else {
            mTracking &= ~Arrest;
        }
    }

    if (!cop_speech) {
        cop_speech = Manager::GetSpeechModule(COPSPEECH_MODULE);
        if (!cop_speech) {
            return;
        }
    }

    if (cop_speech->GetSFXOBJ_Speech()) {
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
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (((ai->GetFocus() == SoundAI::kPursuitFlow) || (ai->GetFocus() == SoundAI::kLost)) && (mTracking & Lost)) {
        mTracking &= ~Lost;
    }

    if ((ai->GetTimeInView() > 0.0f) && (mTracking & Lost)) {
        copList losList;
        losList.reserve(ai->GetActors().size());
        {
            copMap::const_iterator iter = ai->GetActors().begin();
            while (iter != ai->GetActors().end()) {
                EAXCop *cop = iter->cop;
                if (cop->IsActive() && cop->HasLOS()) {
                    losList.push_back(cop);
                }
                ++iter;
            }
        }

        if (losList.size() > 1) {
            _STL::sort(losList.begin(), losList.end());
        }

        if (!losList.empty()) {
            EAXCop *closest = losList.front();
            closest->RegainVisual();
            mTracking &= ~Lost;
        }
    }
}

void Observer::NotifyAirborne(float alt, float t) {
    SoundAI *ai = SoundAI::Get();

    if (ai->NumCopsWithLOS() > 0 && alt >= ai->GetTune().MinHeightAirborne()) {
        if (t >= ai->GetTune().HangTimeForCommentary()) {
            EAXCop *cop = ai->FindClosestCop(true, true);
            if (cop) {
                cop->SuspectAirborne(ai->GetPlayerSpeed() < ai->GetTune().SpeedThreshFlyFlipIntensity()
                                         ? Csis::Type_intensity_Normal
                                         : Csis::Type_intensity_High);
            }
        }
    }
}

void Observer::AssessFlippage() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->NumCopsWithLOS() > 0) {
        bool flipped = false;
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);
        if (player) {
            ISimable *simable = player->GetSimable();
            IRigidBody *irb = simable->GetRigidBody();
            UMath::Vector3 upVec;
            irb->GetUpVector(upVec);
            flipped = upVec.y < 0.0f;
            if (!flipped) {
                mT_flipped = WorldTimer;
            }
        }

        float t_flipped = (WorldTimer - mT_flipped).GetSeconds();
        if (flipped && (t_flipped >= ai->GetTune().FlipTimeForCommentary())) {
            EAXCop *cop = ai->FindClosestCop(true, true);
            if (cop) {
                cop->SuspectRollover(ai->GetPlayerSpeed() > ai->GetTune().SpeedThreshFlyFlipIntensity() ? Csis::Type_intensity_High : Csis::Type_intensity_Normal);
            }
        }
    }
}

void Observer::Assess180() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->NumCopsWithLOS() > 0) {
        float fwDot = CalcFWVec_Road_Car();
        if ((fwDot >= -0.2f) && ((mTracking & UTurnResult) == 0)) {
            mT_unstable = WorldTimer;
        }

        static UMath::Vector3 fw0 = UMath::Vector3::kZero;
        static UMath::Vector3 startpos = UMath::Vector3::kZero;
        static char datapts = 0;

        if (fwDot < -0.2f) {
            if ((mTracking ^ UTurn) & UTurn) {
                if ((mTracking & UTurnResult) == 0) {
                    mDotTrack = fwDot;
                    fw0 = mFwRoad;
                    mTracking |= UTurn;
                    datapts = 0;
                    startpos = ai->GetPlayerPos();
                }
            }
        }

        {
            unsigned int tracking = mTracking;
            if ((tracking & UTurn) == 0) {
                return;
            }
            if (fwDot > 0.2f) {
                if ((tracking & UTurnResult) == 0) {
                    if (fwDot > mDotTrack) {
                        mTracking = tracking & ~UTurn;
                        mT_unstable = WorldTimer;
                        datapts = 0;
                        startpos = UMath::Vector3::kZero;
                    } else {
                        mDotTrack = fwDot;
                        datapts++;
                    }
                }
            }

            if (fwDot <= 0.2f) {
                if (datapts >= ai->GetTune().MinContigFramesFor180()) {
                    if ((mTracking & UTurnResult) == 0) {
                        UMath::Vector3 endpos = ai->GetPlayerPos();
                        float dist_from_start = UMath::Distance(startpos, endpos);
                        if (dist_from_start > ai->GetTune().MaxRangeFor180()) {
                            mTracking &= ~UTurn;
                            mT_unstable = WorldTimer;
                            datapts = 0;
                            startpos = UMath::Vector3::kZero;
                        } else {
                            mTracking |= UTurnResult;
                            mT_unstable = WorldTimer;
                        }
                    }
                }
            }

            if ((mTracking & UTurnResult) != 0) {
                float delta_dot = UMath::Dot(mFwPlayer, fw0);
                float t_unstable = (WorldTimer - mT_unstable).GetSeconds();
                if (delta_dot <= 0.2f) {
                    if (t_unstable > ai->GetTune().MaxTimeFor180()) {
                        EAXCop *cop = ai->FindClosestCop(true, true);
                        if (cop) {
                            cop->SuspectUTurn();
                        } else {
                            return;
                        }
                    } else {
                        return;
                    }
                }
                mTracking &= ~UTurn;
                mTracking &= ~UTurnResult;
                mT_unstable = WorldTimer;
                datapts = 0;
            }
        }
    } else {
        mTracking &= ~UTurn;
        mTracking &= ~UTurnResult;
        mT_unstable = WorldTimer;
    }
}

void Observer::AssessOutcome() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->GetFocus() != SoundAI::kStrategyFlow) {
        return;
    }
    if (ai->GetPursuitState() != SoundAI::kActive) {
        return;
    }

    {
        float t_busted = ai->GetPursuit()->TimeUntilBusted();
        if ((t_busted > 0.0f) && (t_busted < 2.0f) || ai->GetPursuit()->IsCollapseActive()) {
            EAXCop *cop = ai->GetRandomActiveCop(1, false);
            if (cop && !cop->IsHeli() && !Manager::IsQueued(static_cast<SPCHType_1_EventID>(0x5A), 4)) {
                cop->AnticipateSuccess();
            }
        }

        if ((mTracking & Outcome) == 0) {
            return;
        }
        if (t_busted >= 2.0f) {
            return;
        }

        {
            float t_tracking = (WorldTimer - mT_trackingOutcome).GetSeconds();
            if (t_tracking < ai->GetTune().OutcomeTrackTime()) {
                return;
            }

            {
                copList primvisual;
                primvisual.reserve(ai->GetActors().size());
                {
                    copMap::const_iterator iter = ai->GetActors().begin();
                    while (iter != ai->GetActors().end()) {
                        EAXCop *cop = iter->cop;
                        if (cop->IsActive() && cop->HasLOS() && cop->IsPrimary()) {
                            primvisual.push_back(cop);
                        }
                        ++iter;
                    }
                }

                if (primvisual.empty()) {
                    return;
                }

                {
                    bool say_outcome = true;
                    EAXCop *cop;
                    if (primvisual.size() > 1) {
                        _STL::sort(primvisual.begin(), primvisual.end());
                    }

                    cop = *primvisual.begin();
                    if (mTracking & CarRam) {
                        if (mRamCop) {
                            say_outcome = false;
                            if (mRamCop->GetTimeLastRammed() <= t_tracking) {
                                mRamCop->AnticipateSuccess();
                            } else {
                                mRamCop->AnticipateFail();
                            }
                        }
                        mTracking &= ~CarRam;
                        ai->ClearImmunity();
                    }

                    if (say_outcome) {
                        if (ai->GetPlayerSpeed() >= ai->GetTune().OutcomeFailSpeed()) {
                            cop->OutcomeFail(-1);
                        } else if (!cop->IsHeli()) {
                            cop->AnticipateSuccess();
                        }
                    }

                    mTracking &= ~Outcome;
                }
            }
        }
    }
}

void Observer::AssessBraking() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->NumCopsWithLOS() <= 0) {
        return;
    }

    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (!player) {
        return;
    }

    IInput *input;
    player->GetSimable()->QueryInterface(&input);
    if (!input) {
        return;
    }

    float steer = CalcFWVec_Road_Car();
    float brake = UMath::Clamp(input->GetControls().fBrake, 0.0f, 1.0f);
    float ebrake = UMath::Clamp(input->GetControls().fHandBrake, 0.0f, 1.0f);

    static float speed0 = ai->GetPlayerSpeed();
    static Timer t_brake_start = WorldTimer;

    float t_last_crashed = bMin(ai->GetTimeLastCrashed(), ai->GetTimeLastNailedCop());
    if ((mTracking & Braking) == 0) {
        if ((steer >= 0.7f) && ((brake >= 1.0f) || (ebrake >= 1.0f)) && (ai->GetPlayerSpeed() > 60.0f) && (t_last_crashed > 5.0f)) {
            mTracking |= Braking;
            speed0 = ai->GetPlayerSpeed();
            t_brake_start = WorldTimer;
        }

        if ((mTracking & Braking) == 0) {
            return;
        }
    }

    if ((steer < 0.7f) || ((brake < 0.5f) && (ebrake < 0.5f)) || (t_last_crashed < 5.0f)) {
        mTracking &= ~Braking;
        return;
    }

    {
        float curr_speed = ai->GetPlayerSpeed();
        float pct_decrease = curr_speed / speed0;
        if (pct_decrease < (1.0f - ai->GetTune().CrashSlowdownPct())) {
            EAXCop *cop = ai->FindClosestCop(true, true);
            if (cop) {
                cop->SuspectBrake();
                mTracking &= ~Braking;
            }
        }
    }
}

void Observer::AssessOutrun() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai->NumCopsWithLOS() <= 0) {
        return;
    }

    if (ai->GetTimeInView() < 2.0f) {
        return;
    }

    copList visual;
    visual.reserve(ai->GetActors().size());

    copMap::const_iterator iter = ai->GetActors().begin();
    while (iter != ai->GetActors().end()) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive() && cop->HasLOS()) {
            visual.push_back(cop);
        }
        ++iter;
    }

    if (visual.empty()) {
        return;
    }

    unsigned char num_being_outrun = 0;
    copList::iterator i = visual.begin();
    while (i != visual.end()) {
        EAXCop *cop = *i;
        if (cop->GetTimeLastClosing() > 8.0f) {
            num_being_outrun++;
        }
        ++i;
    }

    if ((num_being_outrun == visual.size()) && !ai->AreCopsAhead()) {
        EAXCop *cop = ai->GetRandomActiveCop(0, false);
        if (cop) {
            cop->SuspectOutrun();
        }
    }
}

void Observer::AssessOffroad() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if ((ai->NumCopsWithLOS() > 0) && (ai->GetTimeInView() >= 2.0f)) {
        unsigned int id = ai->GetPlayerOffroadID();
        if (mCurrOffroadID != static_cast<int>(id)) {
            EAXCop *cop = ai->FindClosestCop(true, true);
            if (cop) {
                mCurrOffroadID = static_cast<int>(id);
                Csis::Type_offroad_moment_id speech_id;
                if (MiscSpeech::GetSPAMLocation(id, speech_id)) {
                    cop->Offroad(speech_id, (mOffroadHistory & speech_id) != 0);
                }
            }
        }

    }
}

void Observer::MessageEventComplete(const MNotifySpeechStatus &message) {
    ScheduledSpeechEvent *speech = message.GetEvent();
    if (speech == 0) {
        return;
    }

    static int spkrID911;
    bool outcome_req = false;
    mOutcomeIntensity = Csis::Type_intensity_High;

    switch (speech->ID) {
    case kSPCH1_EventID_OffroadMoment: {
        Csis::AnytimeEvents_OffroadMomentStruct *data =
            static_cast<Csis::AnytimeEvents_OffroadMomentStruct *>(speech->GetData(0));
        if (data != 0) {
            unsigned int offroad_id = data->offroad_id;
            if ((mOffroadHistory & offroad_id) == 0) {
                mOffroadHistory |= offroad_id;
            }
        }
        break;
    }
    case kSPCH1_EventID_CollisionWorld: {
        Csis::AnytimeEvents_CollisionWorldStruct *data =
            static_cast<Csis::AnytimeEvents_CollisionWorldStruct *>(speech->GetData(0));
        outcome_req = true;
        if (data != 0 && data->world_object_type == 0x40) {
            mOutcomeIntensity = Csis::Type_intensity_Normal;
        }
        break;
    }
    case kSPCH1_EventID_RBEngage:
    case kSPCH1_EventID_ExtraRBEngage:
    case kSPCH1_EventID_HeliBullhornArrest: {
        outcome_req = true;
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if (ai != 0 && ai->GetRBFlow() != 0) {
            outcome_req = ai->GetRBFlow()->HasNailedSpikes() == 0;
        }
        break;
    }
    case kSPCH1_EventID_StrategyExecute: {
        Csis::RollingStrategy_StrategyExecuteStruct *data =
            static_cast<Csis::RollingStrategy_StrategyExecuteStruct *>(speech->GetData(0));
        outcome_req = true;
        if (data != 0) {
            mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
        }
        break;
    }
    case kSPCH1_EventID_Arrest: {
        Csis::Arrest_ArrestStruct *data = static_cast<Csis::Arrest_ArrestStruct *>(speech->GetData(0));
        outcome_req = true;
        if (data != 0) {
            mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
        }
        break;
    }
    case kSPCH1_EventID_BullhornArrest: {
        Csis::Arrest_BullhornArrestStruct *data = static_cast<Csis::Arrest_BullhornArrestStruct *>(speech->GetData(0));
        outcome_req = true;
        if (data != 0) {
            mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
        }
        break;
    }
    case kSPCH1_EventID_CollWorld_Flip: {
        Csis::AnytimeEvents_CollWorld_FlipStruct *data =
            static_cast<Csis::AnytimeEvents_CollWorld_FlipStruct *>(speech->GetData(0));
        outcome_req = true;
        if (data != 0) {
            mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
        }
        break;
    }
    case kSPCH1_EventID_IntentToRam: {
        Csis::AnytimeEvents_IntentToRamStruct *data = static_cast<Csis::AnytimeEvents_IntentToRamStruct *>(speech->GetData(0));
        if (data != 0) {
            if (data->speaker_id == 2) {
                mTracking &= ~CarRam;
            } else {
                SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
                mTracking |= CarRam;
                if (ai != 0) {
                    mRamCop = ai->GetCop(data->speaker_id);
                    if (mRamCop != 0) {
                        ai->MakeCopsImmune();
                    }
                }
            }
            outcome_req = true;
            mOutcomeIntensity = static_cast<Csis::Type_intensity>(data->intensity);
        }
        break;
    }
    case kSPCH1_EventID_LostVisual:
    case kSPCH1_EventID_LostSuspect:
    case kSPCH1_EventID_HeliLostVisual:
        if ((mTracking & Lost) == 0) {
            mTracking |= Lost;
        }
        break;
    case kSPCH1_EventID_ReInitPursuit:
        if ((mTracking & Lost) != 0) {
            mTracking &= ~Lost;
        }
        if (Manager::IsQueued(kSPCH1_EventID_RegainVisual, 4)) {
            Manager::RecallSpeechEvent(kSPCH1_EventID_RegainVisual);
        }
        break;
    case kSPCH1_EventID_Disp911Report:
        spkrID911 = MiscSpeech::MoreDetails(0);
        if (UTL::Collections::Singleton<SoundAI>::Get() != 0) {
            UTL::Collections::Singleton<SoundAI>::Get()->Force911State();
        }
        break;
    case kSPCH1_EventID_MoreDetails: {
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if (ai != 0 && ai->GetPursuitState() != SoundAI::kActive) {
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
        mTracking |= Outcome;
        mT_trackingOutcome = WorldTimer;
    }
}

void Observer::MessageBlewPastCop(const MGamePlayMoment &message) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if ((ai->GetFocus() == SoundAI::kStrategyFlow) &&
        ((ai->GetPursuitState() == SoundAI::kActive) || (ai->GetPursuitState() == SoundAI::kSearching)) && (ai->NumCopsWithLOS() < 2)) {
        unsigned int copID = message.GethSimable();
        copMap::const_iterator i = ai->GetActors().begin();
        while (i != ai->GetActors().end()) {
            EAXCop *cop = i->cop;
            ISimable *simable = ISimable::FindInstance(cop->GetHandle());
            if (simable && (simable->GetWorldID() == copID)) {
                IVehicle *vehicle = nullptr;
                if (simable->QueryInterface(&vehicle)) {
                    float cop_speed = MPS2MPH(vehicle->GetSpeed());
                    if (ai->GetPlayerSpeed() > (cop_speed + 50.0f)) {
                        bool cop_in_rb = false;
                        IRoadBlock *block = ai->GetRoadblock();
                        if (block) {
                            IVehicle *car = block->IsComprisedOf(simable->GetOwnerHandle());
                            if (car) {
                                cop_in_rb = true;
                            }
                        }

                        if (!cop_in_rb) {
                            cop->Spotted();
                        }
                    }
                }
            }
            ++i;
        }
    }
}

void Observer::MessageTunnelUpdate(const MMiscSound &message) {
    mTunnel = message.GetSoundID() > 0;
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (ai && mTunnel && ai->GetHeli() && ai->GetHeli()->IsActive() && ai->GetHeli()->HasLOS()) {
        ai->GetHeli()->HazardAlert(Csis::Type_heli_hazard_alert_type_approaching_tunnel);
    }
}

void Observer::MessageGamePlayMoment(const MGamePlayMoment &message) {
    if (message.GetAttribKey() == 0xa1794c23) {
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        if (ai->GetFocus() != SoundAI::kStrategyFlow) {
            return;
        }

        bool player_started = false;
        ISimable *simable = ISimable::FindInstance(reinterpret_cast<HSIMABLE>(message.GethSimable()));

        if (simable) {
            ICause *causer = ICause::FindInstance(simable->GetCausality());
            if (IVehicle::FindIf(VEHICLE_PLAYERS, ComComparator(causer)) != nullptr) {
                player_started = true;
            }
        }

        if (player_started) {
            EAXCop *closest = ai->FindClosestCop(false, true);
            if (closest && closest->HasLOS()) {
                closest->Collision(0xd, 1.0f, closest);
            }
        }

        mT_gasstationexpl = WorldTimer;
        mGasStationPos = UMath::Vector4To3(message.GetPosition());
    }
}

void Observer::DetachRamCop() {
    mRamCop = 0;
}

void Observer::SetMask(unsigned int mask) {
    mObserveMask = mask;
}

} // namespace Speech
