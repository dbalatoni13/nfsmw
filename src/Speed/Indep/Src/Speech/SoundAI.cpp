#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Generated/Messages/MRestartRace.h"

namespace MiscSpeech {
bool GetLocation(RoadNames road, int &region, int &location) asm("GetLocation__10MiscSpeech9RoadNamesRQ24Csis20Type_location_regionRQ24Csis13Type_location");
void LostSuspect(int speaker) asm("LostSuspect__10MiscSpeechi");
void Bailout(int speaker) asm("Bailout__10MiscSpeechi");
void QuadrantForming() asm("QuadrantForming__10MiscSpeech");
void SuspectPossiblyGone() asm("SuspectPossiblyGone__10MiscSpeech");
void QuadrantMoving() asm("QuadrantMoving__10MiscSpeech");
void OtherLead() asm("OtherLead__10MiscSpeech");
void PossibleSuspect() asm("PossibleSuspect__10MiscSpeech");
void WrongSuspect() asm("WrongSuspect__10MiscSpeech");
}

namespace Speech {
void Observer_Observe(void *observer, int observation, int speaker, float score) asm("Observe__Q26Speech8Observeriif");
void RoadblockFlow_NailedSomethingInRB(void *rbflow, unsigned int key) asm("NailedSomethingInRB__Q26Speech13RoadblockFlowUi");
bool Manager_IsCopSpeechBusy() asm("IsCopSpeechBusy__Q26Speech7Manager");
Timer Manager_GetTimeSinceLastEvent(SpeechModuleIndex module) asm("GetTimeSinceLastEvent__Q26Speech7Manager17SpeechModuleIndex");
void SpeechFlow_ChangeStateTo(void *flow, int state) asm("ChangeStateTo__Q26Speech10SpeechFlowi");
int SpeechFlow_GetState(void *flow) asm("GetState__Q26Speech10SpeechFlow");
void PursuitFlow_Update(void *flow) asm("Update__Q26Speech11PursuitFlow");
int PursuitFlow_IsTransitionable(void *flow) asm("IsTransitionable__Q26Speech11PursuitFlow");
void StrategyFlow_Update(void *flow) asm("Update__Q26Speech12StrategyFlow");
int StrategyFlow_IsTransitionable(void *flow) asm("IsTransitionable__Q26Speech12StrategyFlow");
void MusicFlow_Update(void *flow) asm("Update__Q26Speech9MusicFlow");
void Observer_Update(void *observer) asm("Update__Q26Speech8Observer");
void RoadblockFlow_Update(void *flow) asm("Update__Q26Speech13RoadblockFlow");
void PursuitFlow_Reset(void *flow) asm("Reset__Q26Speech11PursuitFlow");
void StrategyFlow_Reset(void *flow) asm("Reset__Q26Speech12StrategyFlow");
void RoadblockFlow_Reset(void *flow) asm("Reset__Q26Speech13RoadblockFlow");
void PursuitFlow_OnCopRemoved(void *flow, EAXCop *cop) asm("OnCopRemoved__Q26Speech11PursuitFlowP6EAXCop");
void PursuitFlow_Reacquire(void *flow) asm("Reacquire__Q26Speech11PursuitFlow");
void Observer_Reset(void *observer) asm("Reset__Q26Speech8Observer");
void MusicFlow_Reset(void *flow) asm("Reset__Q26Speech9MusicFlow");
void MusicFlow_Reacquire(void *flow) asm("Reacquire__Q26Speech9MusicFlow");
void Manager_ResetGlobalHistory() asm("ResetGlobalHistory__Q26Speech7Manager");
void *PursuitFlow_Ctor(void *flow) asm("__Q26Speech11PursuitFlow");
void *StrategyFlow_Ctor(void *flow) asm("__Q26Speech12StrategyFlow");
void *Observer_Ctor(void *observer) asm("__Q26Speech8Observer");
void *RoadblockFlow_Ctor(void *flow) asm("__Q26Speech13RoadblockFlow");
void *MusicFlow_Ctor(void *flow) asm("__Q26Speech9MusicFlow");
void PursuitFlow_Dtor(void *flow, int in_chrg) asm("_._Q26Speech11PursuitFlow");
void StrategyFlow_Dtor(void *flow, int in_chrg) asm("_._Q26Speech12StrategyFlow");
void Observer_Dtor(void *observer, int in_chrg) asm("_._Q26Speech8Observer");
void RoadblockFlow_Dtor(void *flow, int in_chrg) asm("_._Q26Speech13RoadblockFlow");
void MusicFlow_Dtor(void *flow, int in_chrg) asm("_._Q26Speech9MusicFlow");
bool Observer_WeatherExists(void *observer) asm("WeatherExists__Q26Speech8Observer");
}

extern float lbl_80407BA8;
extern float lbl_80407BA4;
extern int FORCE_VOICE_RANDOMIZATION;
extern bool IsSpeechEnabled;
extern short Speech_Manager_mLastSpeakerID asm("_Q26Speech7Manager.mLastSpeakerID");
extern ParameterAccessor SPAMAccessorSpeech;

void EAXDispatch_Report911(void *dispatch, int infraction) asm("Report911__11EAXDispatchQ24Csis17Type_pursuit_type");
void EAXDispatch_JurisShift(void *dispatch, int jurisdiction) asm("JurisShift__11EAXDispatchQ24Csis17Type_jurisdiction");

static float prev_heat_30802 = 0.0f;

SoundAI::SoundAI()
    : Sim::Activity(1),
      mMainUpdate(0),
      mProcessObservations(0),
      mFlags(0),
      mActors(10),
      mUsage(),
      mDispatch(0),
      mLeader(0),
      mHeli(0),
      mCopsInFormation(),
      mDeadAir(0.0f),
      mLastCopInFormation(0),
      mLatestCop(0),
      mPlayerHeat(0),
      mPlayerSpeed(0.0f),
      mPlayerPos(UMath::Vector3::kZero),
      mSmoothedFWRoad(UMath::Vector3::kZero),
      mPursuit(0),
      mAIPursuit(0),
      mFocus(kPursuitFlow),
      mPursuitDist(0.0f),
      mT_PerpLastSeen(0.0f),
      mLOSCount(0),
      mTrafficHits911(0),
      mCTS911(0),
      mHavoc(0),
      mPursuitCount(0),
      mNumRoadBlocks(0),
      mRacerCount(0),
      mClosestRacerDist(0.0f),
      mTimeSinceLastChase(0.0f),
      mPVehicle(static_cast<const Attrib::Collection *>(0), 0, 0),
      mTune(static_cast<const Attrib::Collection *>(0), 0, 0),
      mPursuitLevel(static_cast<const Attrib::Collection *>(0), 0, 0),
      mPursuitState(kInactive),
      mQuadrantState(kReset),
      mRecentBlowby(),
      mInfraction(-1),
      mNumCopsInWave(0),
      mNumActiveCopCars(0),
      mPlayerOffroadID(-1),
      mCopsInView(0),
      mPursuitFlow(0),
      mStrategyFlow(0),
      mObserver(0),
      mRoadblockFlow(0),
      mMusicFlow(0),
      mT_outofFormation(0),
      mT_reallylowspeed(0),
      mT_noLOS(0),
      mT_LOS(0),
      mT_lastCrashed(0),
      mT_lastCopNailed(0),
      mT_pursuitStart(WorldTimer),
      mT_sinceLastPursuit(0),
      mPlayerCarCustom(0),
      mAICarCustom(0),
      mActorPool(0),
      mMsgPerpBusted(0),
      mMsgAIPerpBusted(0),
      mMsgForcePursuitStart(0),
      mMsgRestartRace(0),
      mMsgInfraction(0),
      mMsgUnspawnCop(0),
      mMsgTireBlown(0) {
    mMainUpdate = AddTask("Speech", 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);
    mProcessObservations = AddTask("Comment", 1.0f, 0.0f, Sim::TASK_FRAME_FIXED);

    mUsage.voices.reserve(8);
    mUsage.cs_Rhino.reserve(6);
    mUsage.cs_SuperPursuit.reserve(6);
    mUsage.cs_City.reserve(20);
    mUsage.cs_Coastal.reserve(10);
    mUsage.cs_Rosewood.reserve(10);
    mUsage.cs_Alpine.reserve(10);

    mMsgPerpBusted = Hermes::Handler::Create<MPerpBusted, SoundAI, SoundAI>(this, &SoundAI::MessagePerpBusted, "Speech", 0);
    mMsgAIPerpBusted = Hermes::Handler::Create<MPerpBusted, SoundAI, SoundAI>(this, &SoundAI::MessageAIPerpBusted, "AIRacerBusted", 0);
    mMsgRestartRace = Hermes::Handler::Create<MRestartRace, SoundAI, SoundAI>(this, &SoundAI::MessageRestart, "Speech", 0);
    mMsgInfraction = Hermes::Handler::Create<MMiscSound, SoundAI, SoundAI>(this, &SoundAI::MessageInfraction, "Infraction", 0);
    mMsgUnspawnCop = Hermes::Handler::Create<MUnspawnCop, SoundAI, SoundAI>(this, &SoundAI::MessageUnspawnCop, "SoundAI", 0);
    mMsgTireBlown = Hermes::Handler::Create<MGamePlayMoment, SoundAI, SoundAI>(this, &SoundAI::MessageTireBlown, "TireBlo", 0);

    SoundAI::mRefCount = 1;
    mRecentBlowby.distance = lbl_80407BA4;
    mRecentBlowby.speed = 0.0f;
    mRecentBlowby.timestamp = Timer(0);

    Sim::ProfileTask(mMainUpdate, "Speech");
    Sim::ProfileTask(mProcessObservations, "Speech");

    mTune.Change(Attrib::FindCollectionWithDefault(Attrib::Gen::speechtune::ClassKey(), 0));
    mPursuitLevel.Change(Attrib::FindCollectionWithDefault(Attrib::Gen::pursuitlevels::ClassKey(), 0));

    mActorPool = bNewSlotPool(0xA0, 10, "VoiceActors slotpool", AudioMemoryPool);
    if (mActorPool) {
        mActorPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
        mActorPool->ClearFlag(SLOTPOOL_FLAG_WARN_IF_NONEMPTY_DELETE);
    }

    mDispatch = reinterpret_cast<EAXDispatch *>(new EAXCharacter(1, 0, 0, 0));
    mPursuitFlow = reinterpret_cast<PursuitFlow *>(Speech::PursuitFlow_Ctor(::operator new(0x28)));
    mStrategyFlow = reinterpret_cast<StrategyFlow *>(Speech::StrategyFlow_Ctor(::operator new(0x48)));
    mObserver = reinterpret_cast<Observer *>(Speech::Observer_Ctor(::operator new(0xA0)));
    mRoadblockFlow = reinterpret_cast<RoadblockFlow *>(Speech::RoadblockFlow_Ctor(::operator new(0x44)));
    mMusicFlow = reinterpret_cast<MusicFlow *>(Speech::MusicFlow_Ctor(::operator new(0x68)));

    for (int i = 0; i < 2; i++) {
        mPlayerCurrent[i].direction = CalcPlayerDirection(false);
        mPlayerCurrent[i].roadID = untagged;
        mAICurrent[i].direction = 0;
        mAICurrent[i].roadID = untagged;
    }

    mLastKnown.direction = 0;
    mLastKnown.roadID = untagged;
    mAILastKnown.direction = 0;
    mAILastKnown.roadID = untagged;

    for (int i = 3; i < 10; i++) {
        mUsage.voices.push_back(i);
    }
    for (unsigned int ndx = 0; ndx < mUsage.voices.size(); ndx++) {
        int rand = bRandom(static_cast<int>(mUsage.voices.size()));
        int rand_voice = mUsage.voices[rand];
        int curr_voice = mUsage.voices[ndx];
        if (rand_voice != curr_voice) {
            mUsage.voices[ndx] = rand_voice;
            mUsage.voices[rand] = curr_voice;
        }
    }

    ModifyTask(mMainUpdate, 1.0f);
    ModifyTask(mProcessObservations, 1.0f);
}

SoundAI::~SoundAI() {
    SoundAI::mRefCount = 0;
    RemoveTask(mMainUpdate);
    RemoveTask(mProcessObservations);

    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop) {
            delete cop;
        }
        ++iter;
    }
    mActors.clear();
    mCopsInFormation.clear();

    if (mActorPool) {
        bDeleteSlotPool(mActorPool);
        mActorPool = 0;
    }

    Sim::Collision::RemoveListener(this);

    if (mPursuitFlow) {
        Speech::PursuitFlow_Dtor(mPursuitFlow, 3);
        mPursuitFlow = 0;
    }
    if (mStrategyFlow) {
        Speech::StrategyFlow_Dtor(mStrategyFlow, 3);
        mStrategyFlow = 0;
    }
    if (mObserver) {
        Speech::Observer_Dtor(mObserver, 3);
        mObserver = 0;
    }
    if (mRoadblockFlow) {
        Speech::RoadblockFlow_Dtor(mRoadblockFlow, 3);
        mRoadblockFlow = 0;
    }
    if (mMusicFlow) {
        Speech::MusicFlow_Dtor(mMusicFlow, 3);
        mMusicFlow = 0;
    }

    if (mPlayerCarCustom) {
        delete mPlayerCarCustom;
        mPlayerCarCustom = 0;
    }
    if (mAICarCustom) {
        delete mAICarCustom;
        mAICarCustom = 0;
    }

    if (mMsgPerpBusted) {
        Hermes::Handler::Destroy(mMsgPerpBusted);
        mMsgPerpBusted = 0;
    }
    if (mMsgAIPerpBusted) {
        Hermes::Handler::Destroy(mMsgAIPerpBusted);
        mMsgAIPerpBusted = 0;
    }
    if (mMsgForcePursuitStart) {
        Hermes::Handler::Destroy(mMsgForcePursuitStart);
        mMsgForcePursuitStart = 0;
    }
    if (mMsgRestartRace) {
        Hermes::Handler::Destroy(mMsgRestartRace);
        mMsgRestartRace = 0;
    }
    if (mMsgInfraction) {
        Hermes::Handler::Destroy(mMsgInfraction);
        mMsgInfraction = 0;
    }
    if (mMsgUnspawnCop) {
        Hermes::Handler::Destroy(mMsgUnspawnCop);
        mMsgUnspawnCop = 0;
    }
    if (mMsgTireBlown) {
        Hermes::Handler::Destroy(mMsgTireBlown);
        mMsgTireBlown = 0;
    }
}

void SoundAI::MessagePerpBusted(const MPerpBusted &) {
    mFocus = kTerminal;
    if (mMusicFlow) {
        MControlPathfinder msg(false, 0xF, 0, 0);
        msg.Send(UCrc32("Event"));
    }
    mFlags |= BUSTED;
}

void SoundAI::MessageAIPerpBusted(const MPerpBusted &) {
    if (mPursuitState != kInactive) {
        EAXCop *cop = FindFurthestCop(true);
        if (cop) {
            cop->Arrest();
        }
    }
}

void SoundAI::MessageInfraction(const MMiscSound &message) {
    mInfraction = message.GetSoundID();
}

void SoundAI::MessageRestart(const MRestartRace &) {
    ResetPursuit(true);
    Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
    if (cop_speech) {
        cop_speech->ReleaseResource();
    }
    Speech::Manager::ClearPlayback();
}

void SoundAI::MessageUnspawnCop(const MUnspawnCop &message) {
    EAXCop *cop = mActors.Find(message.GetCopHandle());
    int param;

    if (!cop) {
        return;
    }
    param = message.GetParam();
    if (param != 2) {
        if (param < 3) {
            if (param == 0) {
                goto remove_cop;
            }
            if (param == 1) {
                goto deactivate_cop;
            }
            goto remove_cop;
        }
        if (param < 7) {
deactivate_cop:
            cop->SetActive(false);
            return;
        }
    }
remove_cop:
    RemoveCop(message.GetCopHandle());
}

void SoundAI::MessageTireBlown(const MGamePlayMoment &) {
    EAXCop *spkr;
    void *observer;
    int speaker;

    if (mRoadblockFlow) {
        Speech::RoadblockFlow_NailedSomethingInRB(mRoadblockFlow, 0x10);
    }
    spkr = FindClosestCop(true, true);
    observer = mObserver;
    if (observer && spkr) {
        speaker = spkr->GetSpeakerID();
        Speech::Observer_Observe(observer, 0xE, speaker, lbl_80407BA8);
    }
}

void SoundAI::OnVehicleAdded(IVehicle *ivehicle) {
    Sim::Collision::AddListener(this, ivehicle, "SoundAI");
}

void SoundAI::OnCollision(const COLLISION_INFO &cinfo) {
    if (VU0_v3lengthsquare(cinfo.closingVel) < mTune.CollisionMinClosingVelSq()) {
        return;
    }

    ISimable *objA = ISimable::FindInstance(cinfo.objA);
    ISimable *objB = ISimable::FindInstance(cinfo.objB);
    if (!objA && !objB) {
        return;
    }

    EAXCop *actorA = mActors.Find(cinfo.objA);
    EAXCop *actorB = mActors.Find(cinfo.objB);

    int cops_involved = 0;
    if (actorA) {
        cops_involved++;
    }
    if (actorB) {
        cops_involved++;
    }

    int visible_count = 0;
    if (objA) {
        IRenderable *renderable = 0;
        if (objA->QueryInterface(&renderable) && renderable->InView()) {
            visible_count++;
        }
    }
    if (objB) {
        IRenderable *renderable = 0;
        if (objB->QueryInterface(&renderable) && renderable->InView()) {
            visible_count++;
        }
    }

    float min_smash = mTune.PlayerSmashSpeedRange(0) * 0.44703f;
    float max_smash = mTune.PlayerSmashSpeedRange(1) * 0.44703f;
    float impact = 0.0f;
    if (max_smash > min_smash) {
        impact = (cinfo.impulseA + cinfo.impulseB - min_smash) / (max_smash - min_smash);
    }
    if (impact < 0.0f) {
        impact = 0.0f;
    } else if (impact > 1.0f) {
        impact = 1.0f;
    }

    EAXCop *cop_actor = actorA ? actorA : actorB;

    if (cinfo.type == Sim::Collision::Info::WORLD) {
        if ((cops_involved != 1) || !cop_actor || (visible_count == 0) || !cop_actor->IsActive()) {
            return;
        }

        if (impact >= mTune.MinIntensityCopSmash()) {
            Csis::Type_intensity intensity = (impact > 0.75f) ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
            if (mObserver && Speech::Observer_WeatherExists(mObserver)) {
                cop_actor->HeadOn(intensity);
            } else {
                cop_actor->RearEnded(intensity);
            }
        }

        if (cop_actor->IsPrimary() && (mCopsInFormation.size() > 1)) {
            RandomBailoutDeny(cop_actor);
        }
        if (mObserver) {
            Speech::Observer_Observe(mObserver, 4, cop_actor->GetSpeakerID(), impact);
        }
        return;
    }

    if (cops_involved == 0) {
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);
        if (!player) {
            return;
        }

        ISimable *player_sim = player->GetSimable();
        ISimable *other = 0;
        const UMath::Vector3 *player_vel = 0;
        if (player_sim && objA && (objA->GetOwnerHandle() == player_sim->GetOwnerHandle())) {
            other = objB;
            player_vel = &cinfo.objAVel;
        } else if (player_sim && objB && (objB->GetOwnerHandle() == player_sim->GetOwnerHandle())) {
            other = objA;
            player_vel = &cinfo.objBVel;
        }
        if (!other) {
            return;
        }

        IVehicle *other_vehicle = 0;
        other->QueryInterface(&other_vehicle);
        if (!other_vehicle) {
            return;
        }

        if (player_vel) {
            IVehicle *player_vehicle = 0;
            if (player_sim) {
                player_sim->QueryInterface(&player_vehicle);
            }
            if (!player_vehicle) {
                return;
            }
            float player_speed = player_vehicle->GetSpeed();
            float vel_mag = VU0_sqrt(VU0_v3lengthsquare(*player_vel));
            if ((vel_mag > 0.0f) && ((player_speed / vel_mag) < (1.0f - mTune.CrashSlowdownPct()))) {
                mT_lastCrashed = WorldTimer;
            }
        }

        if (other_vehicle->GetDriverClass() == DRIVER_TRAFFIC) {
            int speaker = -1;
            EAXCop *spkr = FindClosestCop(true, true);
            if (spkr) {
                speaker = spkr->GetSpeakerID();
            }
            if (mObserver) {
                Speech::Observer_Observe(mObserver, 7, speaker, impact);
            }
            if (impact >= mTune.MinIntensityTrafficSmash()) {
                mTrafficHits911++;
            }
            return;
        }

        if (other_vehicle->GetDriverClass() == DRIVER_HUMAN) {
            if (mObserver) {
                Speech::Observer_Observe(mObserver, 6, -1, impact);
            }
        } else if (mObserver) {
            Speech::Observer_Observe(mObserver, 7, -1, impact);
        }
        return;
    }

    if (cops_involved != 1 || !cop_actor) {
        return;
    }

    ISimable *cop_sim = actorA ? objA : objB;
    ISimable *other_sim = actorA ? objB : objA;
    if (!cop_sim || !other_sim) {
        return;
    }

    IVehicle *cop_vehicle = 0;
    IVehicle *other_vehicle = 0;
    cop_sim->QueryInterface(&cop_vehicle);
    other_sim->QueryInterface(&other_vehicle);
    if (!cop_vehicle || !other_vehicle) {
        return;
    }

    if (other_vehicle->GetDriverClass() == DRIVER_TRAFFIC) {
        if (mObserver) {
            Speech::Observer_Observe(mObserver, 2, -1, impact);
        }
        cop_actor->JustHitTraffic();
        if ((mPursuitState == kActive) && (mFocus == kStrategyFlow) && cop_actor->IsActive()) {
            cop_actor->BailoutTraffic();
            RandomBailoutDeny(cop_actor);
        }
        return;
    }

    bool cop_is_in_rb = false;
    IRoadBlock *roadblock = GetRoadblock();
    if (roadblock) {
        if (roadblock->IsComprisedOf(other_sim->GetOwnerHandle()) == other_vehicle) {
            cop_is_in_rb = true;
            if (mRoadblockFlow) {
                Speech::RoadblockFlow_NailedSomethingInRB(mRoadblockFlow, 0x40);
            }
        }
    } else if (mObserver) {
        Speech::Observer_Observe(mObserver, 3, -1, impact);
    }

    bool cop_is_braking = false;
    IInput *input = 0;
    if (other_sim->QueryInterface(&input)) {
        InputControls &controls = input->GetControls();
        cop_is_braking = (controls.fBrake > 0.0f) || (controls.fHandBrake > 0.0f);
    }

    UMath::Vector3 fwCop;
    UMath::Vector3 fwRacer;
    UMath::Vector3 dimCop;
    UMath::Vector3 dimRacer;
    UMath::Vector3 armCop = actorA ? cinfo.armA : cinfo.armB;
    UMath::Vector3 armRacer = actorA ? cinfo.armB : cinfo.armA;
    UMath::Vector3 velCop = (cop_sim->GetOwnerHandle() == cinfo.objA) ? cinfo.objAVel : cinfo.objBVel;
    UMath::Vector3 velRacer = (other_sim->GetOwnerHandle() == cinfo.objA) ? cinfo.objAVel : cinfo.objBVel;
    UMath::Vector3 cnormal = cinfo.normal;

    IRigidBody *cop_body = cop_sim->GetRigidBody();
    IRigidBody *racer_body = other_sim->GetRigidBody();
    if (!cop_body || !racer_body) {
        return;
    }

    cop_body->GetForwardVector(fwCop);
    racer_body->GetForwardVector(fwRacer);
    cop_body->GetDimension(dimCop);
    racer_body->GetDimension(dimRacer);

    bool process = false;
    int collision_type = 8;
    float dot_heading = UMath::Dot(fwRacer, fwCop);
    float dot_vel_cop = UMath::Dot(velCop, cnormal);
    float dot_vel_racer = UMath::Dot(velRacer, cnormal);

    if (dot_heading >= 0.7f) {
        float arm_cop_x = (armCop.x < 0.0f) ? -armCop.x : armCop.x;
        float arm_racer_x = (armRacer.x < 0.0f) ? -armRacer.x : armRacer.x;
        if ((arm_cop_x <= dimCop.x * 0.75f) || (arm_racer_x <= dimRacer.x * 0.75f)) {
            if ((dimCop.z * 0.75f <= armCop.z) && (armRacer.z <= dimRacer.z * -0.75f)) {
                collision_type = 0;
                process = true;
            } else if ((dimRacer.z * 0.75f <= armRacer.z) && (armCop.z <= dimCop.z * -0.75f)) {
                collision_type = 1;
                process = true;
            }
        } else if ((dot_vel_cop < 0.0f ? -dot_vel_cop : dot_vel_cop) <= (dot_vel_racer < 0.0f ? -dot_vel_racer : dot_vel_racer)) {
            collision_type = 7;
            process = true;
        } else {
            collision_type = 6;
            process = true;
        }
    } else if (dot_heading <= -0.7f) {
        if ((dot_vel_cop < 0.0f ? -dot_vel_cop : dot_vel_cop) <= (dot_vel_racer < 0.0f ? -dot_vel_racer : dot_vel_racer)) {
            collision_type = 5;
        } else {
            collision_type = 4;
        }
        process = true;
    } else {
        if (dimRacer.z * 0.7f <= armRacer.z) {
            collision_type = 3;
            process = true;
        }
        if (dimCop.z * 0.7f <= armCop.z) {
            collision_type = 2;
            process = true;
        }
    }

    if (!process) {
        return;
    }

    mT_lastCopNailed = WorldTimer;
    if (cop_is_in_rb || cop_is_braking || ((mFlags & COPS_IMMUNE) != 0)) {
        return;
    }

    cop_actor->WasRammed();
    if ((mFocus == kPursuitFlow) || (mFocus == kTerminal) || cop_actor->IsDead() || !cop_actor->IsActive() || (impact <= 0.15f)) {
        return;
    }

    Csis::Type_intensity intensity = (impact > 0.75f) ? Csis::Type_intensity_High : Csis::Type_intensity_Normal;
    if ((impact <= 0.5f) || (impact > 0.75f)) {
        switch (collision_type) {
        case 1:
            cop_actor->RearEnded(intensity);
            break;
        case 3:
            cop_actor->TBoned(intensity);
            break;
        case 5:
            cop_actor->HeadOn(intensity);
            break;
        case 7:
            cop_actor->SideSwiped(intensity);
            break;
        default:
            cop_actor->SuspectSpunout(intensity);
            break;
        }
    } else {
        float chance = bRandom(1.0f);
        if (0.5f <= chance) {
            cop_actor->Deny();
        } else {
            cop_actor->Ack();
        }
    }
}

void SoundAI::OnAttached(IAttachable *pOther) {
    bool attached;
    IVehicle *ivehicle = 0;

    attached = pOther->QueryInterface(&ivehicle);
    if (attached) {
        OnVehicleAdded(ivehicle);
    }
}

void SoundAI::OnVehicleRemoved(IVehicle *ivehicle) {
    Sim::Collision::RemoveListener(this, ivehicle);

    EAXCop *cop = *reinterpret_cast<EAXCop **>(reinterpret_cast<char *>(mObserver) + 0x60);
    if (cop) {
        EAXCop *actor = mActors.Find(ivehicle->GetSimable()->GetOwnerHandle());
        if (actor == cop) {
            *reinterpret_cast<EAXCop **>(reinterpret_cast<char *>(mObserver) + 0x60) = 0;
        }
    }
}

void SoundAI::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle = 0;
    IPursuit *pursuit;
    bool detached;
    Timer t = WorldTimer;

    detached = pOther->QueryInterface(&ivehicle);
    if (detached) {
        OnVehicleRemoved(ivehicle);
    }

    detached = UTL::COM::ComparePtr(mPursuit, pOther);
    if (detached) {
        PursuitState state = kInactive;
        pursuit = mAIPursuit;
        mPursuit = 0;
        if (pursuit) {
            state = kOtherTarget;
        }
        mT_pursuitStart = t;
        mPursuitState = state;
    } else {
        pursuit = mAIPursuit;
    }

    detached = UTL::COM::ComparePtr(pursuit, pOther);
    if (detached) {
        mAIPursuit = 0;
        if ((mPursuitState == kOtherTarget) && !mPursuit) {
            mPursuitState = kInactive;
        }
    }
}

Sim::IActivity *SoundAI::Construct(Sim::Param) {
    Sim::IActivity *activity;

    if ((IsSoundEnabled != 0) && (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN)) {
        SoundAI *instance = UTL::Collections::Singleton<SoundAI>::Get();
        if (instance) {
            activity = static_cast<Sim::IActivity *>(instance);
            SoundAI::mRefCount = SoundAI::mRefCount + 1;
        } else {
            SoundAI *result = new SoundAI;
            activity = 0;
            if (result) {
                activity = static_cast<Sim::IActivity *>(result);
            }
        }
    } else {
        activity = 0;
    }
    return activity;
}

IRoadBlock *SoundAI::GetRoadblock() {
    IPursuit *pursuit = mPursuit;

    if (pursuit) {
        return pursuit->GetRoadBlock();
    }

    const UTL::Collections::Listable<IRoadBlock, 8>::List &blocks = UTL::Collections::Listable<IRoadBlock, 8>::GetList();
    UTL::Collections::Listable<IRoadBlock, 8>::List::const_iterator i = blocks.begin();
    while (i != blocks.end()) {
        IRoadBlock *rb = *i;
        if (rb) {
            pursuit = rb->GetPursuit();
            if (pursuit) {
                int active = (**reinterpret_cast<int (**)(void *)>(*reinterpret_cast<int **>(reinterpret_cast<char *>(pursuit) + 4) + 0x124 / 4))(
                    reinterpret_cast<char *>(pursuit) + *reinterpret_cast<short *>(*reinterpret_cast<int *>(reinterpret_cast<char *>(pursuit) + 4) + 0x120)
                );
                if (active) {
                    return rb;
                }
            }
        }
        ++i;
    }
    return 0;
}

bool SoundAI::IsMusicActive() {
    bool result;
    int i;
    char *music = reinterpret_cast<char *>(mMusicFlow);

    if (music) {
        i = (**reinterpret_cast<int (**)(void *)>(*reinterpret_cast<int **>(music + 0xC) + 0x2C / 4))(music + *reinterpret_cast<short *>(*reinterpret_cast<int *>(music + 0xC) + 0x28));
        result = true;
        if (i == -1) {
            result = false;
        }
    } else {
        result = false;
    }
    return result;
}

EAXCop *SoundAI::GetCopInRB() {
    IRoadBlock *block = GetRoadblock();

    if (block) {
        Speech::copMap::iterator iter = mActors.begin();
        while (iter != mActors.end()) {
            EAXCop *cop = iter->cop;
            if ((cop->GetInFormation() != 0) && !cop->IsPrimary()) {
                if (block->IsComprisedOf(cop->GetHandle())) {
                    return cop;
                }
            }
            ++iter;
        }
    }
    return 0;
}

EAXCop *SoundAI::GetCop(int speaker) {
    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop && cop->GetSpeakerID() == speaker) {
            return iter->cop;
        }
        ++iter;
    }
    return 0;
}

void SoundAI::RandomBailoutDeny(EAXCop *wimp) {
    if (wimp->GetInFormation() == 1) {
        if ((mActors.size() > 1) && (mPursuitState != kInactive)) {
            Speech::copMap::iterator iter = mActors.begin();
            while (iter != mActors.end()) {
                EAXCop *cop = iter->cop;
                if (cop->IsPrimary()) {
                    if (cop->GetSpeakerID() != wimp->GetSpeakerID()) {
                        cop->DenyBailout();
                    }
                }
                ++iter;
            }
        }
    }
}

int SoundAI::GetVoice(int type) {
    int return_voice;
    Speech::voiceIDs::iterator i = mUsage.voices.begin();
    Speech::voiceIDs::iterator end = mUsage.voices.end();

    if (i == end) {
        return -1;
    }

    if (type == 2) {
        while (i != end) {
            return_voice = *i;
            if (static_cast<unsigned int>(return_voice - 6) < 3) {
                mUsage.voices.erase(i);
                return return_voice;
            }
            ++i;
            end = mUsage.voices.end();
        }
        return -1;
    }

    if (type < 3) {
        if (type == 1) {
            while (i != end) {
                return_voice = *i;
                if (static_cast<unsigned int>(return_voice - 3) < 3) {
                    mUsage.voices.erase(i);
                    return return_voice;
                }
                ++i;
                end = mUsage.voices.end();
            }
            return -1;
        }
    } else if (type == 3) {
        while (i != end) {
            return_voice = *i;
            if (return_voice == 9) {
                mUsage.voices.erase(i);
                return 9;
            }
            ++i;
            end = mUsage.voices.end();
        }
        return -1;
    }

    while (i != end) {
        return_voice = *i;
        if (static_cast<unsigned int>(return_voice - 3) < 6) {
            mUsage.voices.erase(i);
            return return_voice;
        }
        ++i;
        end = mUsage.voices.end();
    }
    return -1;
}

void SoundAI::Release() {
    if ((mRefCount != 0) && ((mRefCount = mRefCount - 1) == 0)) {
        Sim::Activity::Release();
    }
}

bool SoundAI::IsHeadingValid() {
    if ((static_cast<unsigned int>(mPlayerCurrent[0].roadID - on_Highway99) < 0x50) && (mPlayerCurrent[0].direction != 0)) {
        return true;
    }
    return false;
}

void SoundAI::SyncPlayers() {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    if (!player) {
        return;
    }

    IVehicle *vehicle = 0;
    IPerpetrator *perp = 0;
    IVehicleAI *vai = 0;
    player->GetSimable()->QueryInterface(&vehicle);
    player->GetSimable()->QueryInterface(&perp);
    if (vehicle) {
        vehicle->QueryInterface(&vai);
    }
    if (!vehicle || !vai) {
        return;
    }

    WRoadNav *nav = vai->GetDriveToNav();
    if (!nav) {
        return;
    }

    mPlayerCurrent[0].roadID = static_cast<RoadNames>(nav->GetRoadSpeechId());
    mPlayerCurrent[0].direction = CalcPlayerDirection(false);

    if (IsHeadingValid()) {
        RoadNames last_road = mPlayerCurrent[1].roadID;
        RoadNames curr_road = mPlayerCurrent[0].roadID;
        unsigned int curr_dir = mPlayerCurrent[0].direction;
        unsigned int known_dir = mLastKnown.direction;
        mPlayerCurrent[1].roadID = curr_road;
        mPlayerCurrent[1].direction = curr_dir;
        if ((known_dir != curr_dir) && mPursuit && mPursuit->IsPerpInSight() && (last_road != curr_road)) {
            if ((mPursuitState == kActive) && (mFocus != kPursuitFlow)) {
                EAXCop *cop = FindClosestCop(true, true);
                if (cop) {
                    cop->DirectionChange();
                }
            }
            mLastKnown.direction = curr_dir;
            mLastKnown.roadID = curr_road;
        }
    }

    if (!perp) {
        return;
    }

    const Attrib::Instance &vehicle_attrib = player->GetSimable()->GetAttributes();
    if (mPVehicle.GetCollection() != vehicle_attrib.GetCollection()) {
        mPVehicle.Change(Attrib::FindCollectionWithDefault(Attrib::Gen::pvehicle::ClassKey(), vehicle_attrib.GetCollection()));
    }

    if (!mPlayerCarCustom) {
        mPlayerCarCustom = new CarCustomizations;
        if (!GetCustomized(vehicle, *mPlayerCarCustom)) {
            delete mPlayerCarCustom;
            mPlayerCarCustom = 0;
        }
    }

    mPlayerSpeed = MPS2MPH(vehicle->GetSpeed());
    if ((mTune.MinTimeConsideredStopped() <= mPlayerSpeed) || ((mFlags & LOWSPEEDTIMER) != 0)) {
        if (mTune.MinSpeedConsideredStopped() <= mPlayerSpeed) {
            mFlags &= ~LOWSPEEDTIMER;
            mT_reallylowspeed = WorldTimer;
        }
    } else {
        mFlags |= LOWSPEEDTIMER;
        mT_reallylowspeed = WorldTimer;
    }

    mPlayerPos = player->GetPosition();
    if (SPAMAccessorSpeech.Layer) {
        SPAMAccessorSpeech.CaptureData(mPlayerPos.z, -mPlayerPos.x);
        mPlayerOffroadID = SPAMAccessorSpeech.GetDataInt(1);
    }

    float heat = perp->GetHeat();
    mPlayerHeat = static_cast<int>(heat);

    if ((mFocus != kPursuitFlow) && (60.0f < mPursuitDuration) && (prev_heat_30802 < heat)) {
        bool crossed = false;
        int cutoff_index = 3;
        while (cutoff_index > -1) {
            float cutoff = heat_cutoffs[cutoff_index].value;
            if ((prev_heat_30802 < cutoff) && (cutoff <= heat)) {
                crossed = true;
                prev_heat_30802 = heat;
                break;
            }
            cutoff_index--;
        }

        if (crossed && (mPursuitState < kInactive)) {
            if (bRandom(1.0f) <= 0.5f) {
                EAXCop *cop = GetRandomActiveCop(1, false);
                if (mHeli && (bRandom(1.0f) > 0.5f)) {
                    mHeli->HeatJump(heat_cutoffs[cutoff_index].heat_level);
                } else if (cop) {
                    cop->HeatJump(heat_cutoffs[cutoff_index].heat_level);
                }
            } else if (mDispatch) {
                reinterpret_cast<EAXCharacter *>(mDispatch)->HeatJump(heat_cutoffs[cutoff_index].heat_level);
            }

            if (bRandom(1.0f) > 0.5f && mDispatch) {
                if ((3.0f <= heat) && (heat < 6.0f)) {
                    EAXDispatch_JurisShift(mDispatch, 1);
                } else if (6.0f <= heat) {
                    EAXDispatch_JurisShift(mDispatch, 2);
                }
            }
        }
    }

    Attrib::Gen::pursuitlevels *pursuitatr = perp->GetPursuitLevelAttrib();
    if (pursuitatr && (mPursuitLevel.GetCollection() != pursuitatr->GetCollection())) {
        mPursuitLevel.Change(Attrib::FindCollectionWithDefault(Attrib::Gen::pursuitlevels::ClassKey(), pursuitatr->GetCollection()));
    }

    if (mPursuitLevel.GetCollection()) {
        if (mPursuitLevel.roadblockprobability() <= 0.0f) {
            mFlags &= ~RB_ENABLED;
        } else {
            mFlags |= RB_ENABLED;
        }
        if (mPursuitLevel.roadblockhelichance() <= 0.0f) {
            mFlags &= ~HELIRB_ENABLED;
        } else {
            mFlags |= HELIRB_ENABLED;
        }
        if (mPursuitLevel.roadblockspikechance(0) <= 0.0f) {
            mFlags &= ~SPIKES_ENABLED;
        } else {
            mFlags |= SPIKES_ENABLED;
        }

        mNumCopsInWave = mPursuitLevel.NumCopsToTriggerBackup();

        if ((mPursuitState == kInactive) && ((mFlags & DISP911_ACTIVE) == 0)) {
            bool is_DDay = false;
            bool is_pursuit_race = false;
            bool is_roaming = false;
            if (GRaceStatus::Exists()) {
                is_roaming = (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming);
                GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
                if (parms) {
                    is_DDay = parms->GetIsDDayRace();
                    is_pursuit_race = parms->GetIsPursuitRace();
                }
            }

            ICopMgr *copmgr = UTL::Collections::Singleton<ICopMgr>::Get();
            if (copmgr && ICopMgr::AreCopsEnabled()) {
                float t_lockout = copmgr->GetLockoutTimeRemaining();
                bool scripted_911 = ((mPursuitLevel.Lifetime911(0) < t_lockout) && (t_lockout < mPursuitLevel.Lifetime911(1)) && !is_pursuit_race &&
                                     !is_roaming);
                bool req911_met = false;
                if ((mCTS911 >= mPursuitLevel.CTSFor911()) || (mTrafficHits911 >= mPursuitLevel.NumCiviHitsFor911(0))) {
                    req911_met = true;
                }

                if ((req911_met || scripted_911) && !is_DDay && mDispatch) {
                    EAXDispatch_Report911(mDispatch, IsHighIntensity() ? 2 : 1);
                }
            }
        }
    }

    UMath::Vector3 player_fw;
    player->GetSimable()->GetRigidBody()->GetForwardVector(player_fw);
    UMath::Vector3 unit_fw;
    UMath::Unit(player_fw, unit_fw);
    mPlayerFW = unit_fw;
    mSmoothedFWRoad.x = (mSmoothedFWRoad.x * 0.8f) + (unit_fw.x * 0.2f);
    mSmoothedFWRoad.y = (mSmoothedFWRoad.y * 0.8f) + (unit_fw.y * 0.2f);
    mSmoothedFWRoad.z = (mSmoothedFWRoad.z * 0.8f) + (unit_fw.z * 0.2f);
}

int SoundAI::GetBattalionFromRoadID(int roadID) {
    int region;
    int location;
    bool result = MiscSpeech::GetLocation(static_cast<RoadNames>(roadID), region, location);

    if (!result) {
        return -1;
    }
    if (region != 2) {
        if (region < 3) {
            if (region == 1) {
                return 1;
            }
            return -1;
        }
        if (region != 4) {
            if (region != 8) {
                return -1;
            }
            return 4;
        } else {
            return 2;
        }
    }
    return 2;
}

void SoundAI::AddNewHeli(IVehicle *heli) {
    HSIMABLE handle = heli->GetSimable()->GetOwnerHandle();
    EAXAirSupport *chopper = new EAXAirSupport(2, handle);
    int focus;

    mActors.Add(handle, chopper);
    focus = mFocus;
    mHeli = chopper;

    if (focus != 1) {
        if ((focus != 999) && (focus != 0)) {
            chopper->BackupArrives();
            return;
        }
        if (mFocus != 1) {
            return;
        }
    }
    mFlags |= HELI_INTRO_REQ;
}

int SoundAI::GetBattalionFromKey(unsigned int theKey) {
    if ((theKey == 0x38B38226) || (theKey == 0x54B10E38) || (theKey == 0x2E149EAC)) {
        return 0x20;
    }
    if ((theKey == 0x7A49CCCB) || (theKey == 0xA4EB6688) || (theKey == 0xB2F32FE2) || (theKey == 0xD37C806D)) {
        return 0x10;
    }
    return -1;
}

bool SoundAI::IsHighIntensity() {
    if ((mPlayerHeat >= static_cast<int>(mTune.HighIntensityMark())) || (mPursuitDuration >= mTune.PursuitDurationHighIntensity())) {
        return true;
    }
    return false;
}

bool SoundAI::OnTask(HSIMTASK htask, float) {
    float tout = 0.0f;

    if (!Speech::Manager_IsCopSpeechBusy()) {
        tout = (WorldTimer - Speech::Manager_GetTimeSinceLastEvent(COPSPEECH_MODULE)).GetSeconds();
    }
    mDeadAir = tout;

    if (htask == mMainUpdate) {
        if (FORCE_VOICE_RANDOMIZATION != 0) {
            ForceGlobalVoiceChange();
            FORCE_VOICE_RANDOMIZATION = 0;
        }
        if ((mFlags & BUSTED) == 0) {
            SyncPursuit();
            SyncCarsToActors();
            SyncPlayers();
            SyncFormations();
            ShuffleActors();
            UpdateStateMachines();
            if ((IsSpeechEnabled != 0) && (mDeadAir > 0.0f)) {
                DealWithDeadAir();
            }
        }
    }

    if (htask == mProcessObservations) {
        if ((mFlags & BUSTED) == 0) {
            Speech::Observer_Update(mObserver);
            Speech::RoadblockFlow_Update(mRoadblockFlow);
        }
        Speech::Manager::Deduce();
    }
    return true;
}

void SoundAI::UpdateStateMachines() {
    if ((IsSpeechEnabled == 0) || ((mFlags & BUSTED) != 0)) {
        Speech::MusicFlow_Update(mMusicFlow);
        return;
    }

    int focus = mFocus;
    if (focus == kPursuitFlow) {
        Speech::PursuitFlow_Update(mPursuitFlow);
        if (Speech::PursuitFlow_IsTransitionable(mPursuitFlow)) {
            mFocus = kStrategyFlow;
            Speech::SpeechFlow_ChangeStateTo(mStrategyFlow, 0);
        }
    } else if ((focus == kStrategyFlow) || (focus == kRoadblockFlow)) {
        if (Speech::StrategyFlow_IsTransitionable(mStrategyFlow)) {
            Speech::SpeechFlow_ChangeStateTo(mStrategyFlow, kWaiting);
        }
        Speech::StrategyFlow_Update(mStrategyFlow);
    } else if (focus == kWaiting) {
        if ((mPursuit != 0) && (mPursuitState == kSearching)) {
            if (Speech::SpeechFlow_GetState(mStrategyFlow) != kOtherTarget) {
                Speech::SpeechFlow_ChangeStateTo(mStrategyFlow, kOtherTarget);
            }
            Speech::StrategyFlow_Update(mStrategyFlow);
        }
    } else if (!mPursuit) {
        ResetPursuit(false);
    }

    Speech::MusicFlow_Update(mMusicFlow);
}

void SoundAI::AttemptReattachPursuit() {
    bool playerfound = false;
    bool aifound = false;

    if (mPursuit && mPursuit->IsPlayerPursuit()) {
        playerfound = true;
    }
    if (mAIPursuit && !mAIPursuit->IsPlayerPursuit()) {
        aifound = true;
    }

    const UTL::Collections::Listable<IPursuit, 8>::List &pursuits = UTL::Collections::Listable<IPursuit, 8>::GetList();
    UTL::Collections::Listable<IPursuit, 8>::List::const_iterator i = pursuits.begin();
    while (i != pursuits.end()) {
        IPursuit *pursuit = *i;
        if (pursuit->IsPlayerPursuit()) {
            playerfound = true;
            if (mPursuit != pursuit) {
                mPursuitCount++;
                if (!mPursuit) {
                    mT_pursuitStart = WorldTimer;
                }
                mPursuit = pursuit;
            }
        } else {
            if (!aifound) {
                aifound = true;
                mAIPursuit = pursuit;
            }
        }
        ++i;
    }

    if (!playerfound) {
        mPursuit = 0;
        mPursuitState = kInactive;
        mT_pursuitStart = WorldTimer;
    }

    if (!aifound) {
        mAIPursuit = 0;
        if ((mPursuitState == kOtherTarget) && !mPursuit) {
            mPursuitState = kInactive;
        }
    }
}

void SoundAI::SyncPursuit() {
    if (!mPursuit || !mPursuit->IsPlayerPursuit() || (mAIPursuit && mAIPursuit->IsPlayerPursuit())) {
        AttemptReattachPursuit();
    }

    Timer t = WorldTimer;
    if (!mPursuit) {
        if ((mPursuitState == kActive) || (mPursuitState == kSearching)) {
            if (!mAIPursuit || (mRacerCount < 1)) {
                mPursuitState = kInactive;
                mT_sinceLastPursuit = t;
            } else {
                mPursuitState = kOtherTarget;
            }
        }
    } else {
        bool ai_searching = false;
        if (!mPursuit->IsPerpInSight() || !mPursuit->PursuitMeterCanShowBusted() || (mPursuit->GetFormationType() == 2)) {
            ai_searching = true;
        }

        bool heli_los = false;
        if (mHeli && mHeli->HasLOS()) {
            heli_los = mHeli->GetInFormation() != 0;
        }

        if (ai_searching || ((mLOSCount == 0) && !heli_los)) {
            mPursuitState = kSearching;
        } else {
            if (mPursuitState != kActive) {
                mCTS911 = 0;
                mTrafficHits911 = 0;
            }
            mPursuitState = kActive;
        }
    }

    float no_los_time = (WorldTimer - mT_noLOS).GetSeconds();
    if (mPursuitState == kActive) {
        mT_noLOS = WorldTimer;
        mT_sinceLastPursuit = t;
    } else {
        mT_LOS = WorldTimer;
    }

    float inactivity_cutoff[2];
    inactivity_cutoff[0] = mTune.PursuitInactivityTimer(0);
    inactivity_cutoff[1] = mTune.PursuitInactivityTimer(1);

    if ((mFocus == kLost) && (mPursuitState == kActive) && (mTimeSinceLastChase < inactivity_cutoff[1])) {
        mFocus = kPursuitFlow;
        mQuadrantState = kReset;
        mFlags &= ~PURSUIT_EXPIRED;
        Speech::Manager::ClearPlayback();
        Speech::PursuitFlow_Reacquire(mPursuitFlow);
        Speech::MusicFlow_Reacquire(mMusicFlow);
    }

    if (mFocus == kLost) {
        if (((mPursuitState == kSearching) || (mPursuitState == kInactive)) && mPursuit && (mPursuit->GetFormationType() == 4) &&
            ((mFlags & PURSUIT_EXPIRED) == 0)) {
            Speech::Manager::ClearPlayback();
            mDispatch->TimeExpired();
            mQuadrantState = kReset;
            mFlags |= PURSUIT_EXPIRED;
        } else if (no_los_time > inactivity_cutoff[1]) {
            ResetPursuit(false);
        } else if (((mPursuitState == kSearching) || (mPursuitState == kInactive)) && !Speech::Manager_IsCopSpeechBusy()) {
            switch (mQuadrantState) {
            case kInitial:
                MiscSpeech::QuadrantForming();
                mQuadrantState = kForming;
                break;
            case kForming:
                if (bRandom(1.0f) <= 0.4f) {
                    mQuadrantState = kFiction1;
                    switch (bRandom(3)) {
                    case 0:
                        MiscSpeech::SuspectPossiblyGone();
                        break;
                    case 1:
                        MiscSpeech::QuadrantMoving();
                        break;
                    default:
                        MiscSpeech::OtherLead();
                        break;
                    }
                } else {
                    mQuadrantState = kFiction2;
                    MiscSpeech::PossibleSuspect();
                }
                break;
            case kFiction2:
                MiscSpeech::WrongSuspect();
                mQuadrantState = kExpired;
                break;
            case kFiction1:
                mQuadrantState = kExpired;
                break;
            default:
                break;
            }
        }
    }
}

void SoundAI::DealWithDeadAir() {
    if (mPursuitState != kActive) {
        return;
    }
    if (!mLeader) {
        return;
    }
    if (mFocus == kPursuitFlow) {
        return;
    }

    if (mPursuitDuration > 60.0f) {
        mDispatch->PursuitUpdate(mLeader);
    }

    if ((WorldTimer - mT_noLOS).GetSeconds() < mTune.NoLOSCommentaryTime()) {
        if (mPursuitDuration > 60.0f) {
            if (IsHeadingValid() && (mPlayerOffroadID > -1) && (bRandom(1.0f) <= 0.5f)) {
                mLeader->LocationReport();
            } else {
                mLeader->PursuitUpdateReply();
            }
        }
        return;
    }

    if (!mHeli) {
        mLeader->LostVisual();
        return;
    }

    if (!mHeli->HasLOS()) {
        mHeli->LostVisual();
        return;
    }

    if (IsHeadingValid() && (mPlayerOffroadID > -1) && (bRandom(1.0f) <= 0.5f)) {
        mHeli->LocationReport();
    } else {
        mHeli->PursuitUpdateReply();
    }
}

EAXCop *SoundAI::GetRandomCop(int type) {
    EAXCop *spkr = 0;
    unsigned int actor_count = mActors.size();

    if ((type == 0) && (actor_count > 1)) {
        unsigned int idx = bRandom(static_cast<int>(actor_count));
        return mActors[idx].cop;
    }

    if (actor_count == 1) {
        EAXCop *cop = mActors.begin()->cop;
        if (type == 1) {
            if (!cop->IsPrimary()) {
                return 0;
            }
        } else if (type == 2) {
            if (cop->IsPrimary()) {
                return 0;
            }
        } else if (type != 0) {
            return 0;
        }
        return cop;
    }

    if ((type != 1) && (type != 2)) {
        return 0;
    }

    Speech::copList secondaries;
    secondaries.reserve(actor_count);

    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if ((type == 1 && cop->IsPrimary()) || (type == 2 && !cop->IsPrimary())) {
            secondaries.push_back(cop);
        }
        ++iter;
    }

    if (!secondaries.empty()) {
        spkr = secondaries[bRandom(static_cast<int>(secondaries.size()))];
    }
    return spkr;
}

EAXCop *SoundAI::GetRandomActiveCop(int type, bool reqLOS) {
    Speech::copList active;
    active.reserve(mActors.size());

    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive() && (!reqLOS || cop->HasLOS())) {
            active.push_back(cop);
        }
        ++iter;
    }

    if (active.empty()) {
        return 0;
    }

    if ((type == 0) && (active.size() > 1)) {
        return active[bRandom(static_cast<int>(active.size()))];
    }

    if (active.size() == 1) {
        EAXCop *cop = active.front();
        if (type == 1) {
            if (!cop->IsPrimary()) {
                return 0;
            }
        } else if (type == 2) {
            if (cop->IsPrimary()) {
                return 0;
            }
        } else if (type != 0) {
            return 0;
        }
        return cop;
    }

    if ((type != 1) && (type != 2)) {
        return 0;
    }

    Speech::copList secondaries;
    secondaries.reserve(active.size());
    Speech::copList::iterator i = active.begin();
    while (i != active.end()) {
        EAXCop *cop = *i;
        if ((type == 1 && cop->IsPrimary()) || (type == 2 && !cop->IsPrimary())) {
            secondaries.push_back(cop);
        }
        ++i;
    }

    if (!secondaries.empty()) {
        return secondaries[bRandom(static_cast<int>(secondaries.size()))];
    }
    return 0;
}

void SoundAI::RandomizeCallsign(Speech::voiceIDs &cs, Csis::Type_speaker_call_sign_id start, Csis::Type_speaker_call_sign_id finish) {
    if (cs.empty()) {
        int i = start;
        while (i <= finish) {
            cs.push_back(i);
            i <<= 1;
        }
        for (unsigned int ndx = 0; ndx < cs.size(); ndx++) {
            int rand = bRandom(static_cast<int>(cs.size()));
            int rand_cs = cs[rand];
            int curr_cs = cs[ndx];
            if (rand_cs != curr_cs) {
                cs[ndx] = rand_cs;
                cs[rand] = curr_cs;
            }
        }
    }
}

int SoundAI::GetCallsign(Csis::Type_speaker_battalion battalion) {
    Speech::voiceIDs *cs_pool = 0;

    switch (battalion) {
    case Csis::Type_speaker_battalion_City:
        cs_pool = &mUsage.cs_City;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign20);
        }
        break;
    case Csis::Type_speaker_battalion_Rosewood:
        cs_pool = &mUsage.cs_Rosewood;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
        }
        break;
    case Csis::Type_speaker_battalion_Coastal:
        cs_pool = &mUsage.cs_Coastal;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
        }
        break;
    case Csis::Type_speaker_battalion_Super_Pursuit:
        cs_pool = &mUsage.cs_SuperPursuit;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign05);
        }
        break;
    case Csis::Type_speaker_battalion_Alpine:
        cs_pool = &mUsage.cs_Alpine;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
        }
        break;
    case Csis::Type_speaker_battalion_Rhino_Units:
        cs_pool = &mUsage.cs_Rhino;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign06);
        }
        break;
    default:
        break;
    }

    if (!cs_pool) {
        return -1;
    }

    Speech::voiceIDs::iterator iter = cs_pool->begin();
    int id = *iter;
    cs_pool->erase(iter);
    return id;
}

const float SoundAI::GetTimeLastNailedCop() {
    Speech::copList nailed;

    nailed.reserve(mActors.size());
    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop->GetTimesRammed() > 0) {
            nailed.push_back(cop);
        }
        ++iter;
    }

    if (nailed.empty()) {
        return 0.0f;
    }

    float t_mostrecent = nailed.front()->GetTimeLastRammed();
    Speech::copList::iterator i = nailed.begin();
    while (i != nailed.end()) {
        EAXCop *unfortunate = *i;
        if (unfortunate->GetTimeLastRammed() < t_mostrecent) {
            t_mostrecent = unfortunate->GetTimeLastRammed();
        }
        ++i;
    }
    return t_mostrecent;
}

void SoundAI::SyncCarsToActors() {
    IVehicles new_cop_cars;
    new_cop_cars.reserve(30);
    new_cop_cars.clear();

    float closest = lbl_80407BA4;
    mRacerCount = static_cast<char>(UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AIRACERS).size());

    {
        const UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List &vehicles =
            UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AIRACERS);
        UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List::const_iterator i = vehicles.begin();
        while (i != vehicles.end()) {
            IVehicle *vehicle = *i;
            IRenderable *renderable = 0;
            if (vehicle && vehicle->QueryInterface(&renderable)) {
                float dist2cam = renderable->DistanceToView();
                if (dist2cam < closest) {
                    closest = dist2cam;
                }
            }
            ++i;
        }
    }

    mClosestRacerDist = closest;
    if (closest > mTune.AIRacerProximity()) {
        if ((mFlags & RACERS_PROXIMAL) != 0) {
            EAXCop *cop = GetRandomActiveCop(0, false);
            if (cop) {
                cop->FocusChange();
            }
        }
        mFlags &= ~RACERS_PROXIMAL;
    } else {
        mFlags |= RACERS_PROXIMAL;
    }

    unsigned char cops_in_view = 0;
    unsigned char cops_with_los = 0;
    unsigned char cops_ahead = 0;
    unsigned char num_active = 0;
    float pursuit_distance = lbl_80407BA4;

    const UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List &vehicles =
        UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AICOPS);
    UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List::const_iterator i = vehicles.begin();
    while (i != vehicles.end()) {
        IVehicle *vehicle = *i;
        if (vehicle) {
            IRenderable *renderable = 0;
            vehicle->QueryInterface(&renderable);

            ISimable *simable = vehicle->GetSimable();
            HSIMABLE thisObj = simable->GetOwnerHandle();

            bool is_in_rb = false;
            IRoadBlock *irb = GetRoadblock();
            if (irb) {
                is_in_rb = (irb->IsComprisedOf(thisObj) == vehicle);
            }

            if (vehicle->GetDriverClass() == DRIVER_COP) {
                IPursuitAI *ai = 0;
                vehicle->QueryInterface(&ai);

                if (vehicle->GetVehicleClass() == VehicleClass::CAR) {
                    bool has_visual = false;
                    bool is_ahead = false;
                    bool in_view = false;

                    if (renderable && renderable->InView()) {
                        cops_in_view++;
                        in_view = true;
                    }

                    if (vehicle->IsActive() && ai && ((ai->GetTimeSinceTargetSeen() < 0.05f) || (is_in_rb && in_view))) {
                        has_visual = true;
                        cops_with_los++;
                    }

                    UMath::Vector3 player_pos = mPlayerPos;
                    UMath::Vector3 copcar_pos = vehicle->GetPosition();
                    UMath::Vector3 player_fw = mPlayerFW;
                    if (vehicle->IsActive() || in_view) {
                        UMath::Vector3 playerToCop;
                        UMath::Sub(copcar_pos, player_pos, playerToCop);
                        UMath::Unit(playerToCop, playerToCop);
                        float dot = UMath::Dot(playerToCop, player_fw);
                        if (0.0f < dot) {
                            is_ahead = true;
                        }
                    }

                    if (vehicle->IsActive()) {
                        num_active++;
                        float cop_dist_to_car = UMath::Distance(copcar_pos, player_pos);
                        if (cop_dist_to_car < pursuit_distance) {
                            pursuit_distance = cop_dist_to_car;
                        }
                        if (is_ahead) {
                            cops_ahead++;
                        }
                    }

                    EAXCop *actor = mActors.Find(thisObj);
                    if (!actor) {
                        new_cop_cars.push_back(vehicle);
                    } else {
                        if (!vehicle->IsActive()) {
                            actor->SetActive(false);
                        } else if (!actor->IsActive()) {
                            actor->SetActive(true);
                        }

                        actor->SetLOS(has_visual);
                        actor->SetAhead(is_ahead);
                        actor->Update();

                        float t_lastblowby = (WorldTimer - mRecentBlowby.timestamp).GetSeconds();
                        if ((mTune.BlowbyInterval() < t_lastblowby) && (actor->GetSpeed() < mPlayerSpeed) && !is_ahead) {
                            mRecentBlowby.distance = actor->GetDistance();
                            mRecentBlowby.speed = mPlayerSpeed - actor->GetSpeed();
                            mRecentBlowby.timestamp = WorldTimer;
                        }

                        if ((actor->GetDistance() < mRecentBlowby.distance) && (actor->GetSpeed() < mPlayerSpeed) && !is_ahead &&
                            ((mPlayerSpeed * 0.75f) < (mPlayerSpeed - actor->GetSpeed()))) {
                            mRecentBlowby.distance = actor->GetDistance();
                            mRecentBlowby.speed = mPlayerSpeed - actor->GetSpeed();
                            mRecentBlowby.timestamp = WorldTimer;
                        }
                    }
                }
            } else if (vehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                if (!mHeli) {
                    AddNewHeli(vehicle);
                } else {
                    if (mHeli->GetHandle() != thisObj) {
                        mHeli->SetHandle(thisObj);
                    }

                    if (vehicle->IsActive()) {
                        if (!mHeli->IsActive()) {
                            mHeli->SetActive(true);
                        }
                    } else if (mHeli->IsActive()) {
                        mHeli->SetActive(false);
                    }

                    IPursuitAI *ai = 0;
                    vehicle->QueryInterface(&ai);
                    if (vehicle->IsActive() && ai) {
                        mHeli->SetLOS(ai->GetTimeSinceTargetSeen() < 0.05f);
                    }
                    mHeli->Update();
                }
            }
        }
        ++i;
    }

    mCopsInView = cops_in_view;
    mLOSCount = cops_with_los;
    mPursuitDist = pursuit_distance;
    mNumActiveCopCars = num_active;
    if (num_active == cops_ahead) {
        mFlags |= COPS_ARE_AHEAD;
    } else {
        mFlags &= ~COPS_ARE_AHEAD;
    }

    IVehicles::iterator add_it = new_cop_cars.begin();
    while (add_it != new_cop_cars.end()) {
        AddNewCop(*add_it);
        ++add_it;
    }
    new_cop_cars.clear();

    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *actor = iter->cop;
        if (actor && actor->IsActive() && !ISimable::FindInstance(actor->GetHandle())) {
            RemoveCop(actor->GetHandle());
            break;
        }
        ++iter;
    }

    if (mDispatch) {
        mDispatch->Update();
    }
}

void SoundAI::SyncFormations() {
    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (!cop->GetInFormation() || cop->IsHeli()) {
            Speech::copList::iterator i = mCopsInFormation.begin();
            while (i != mCopsInFormation.end()) {
                EAXCop *copInFormation = *i;
                if (copInFormation && copInFormation->GetHandle() == cop->GetHandle()) {
                    mCopsInFormation.erase(i);
                    break;
                }
                ++i;
            }
        } else {
            bool found = false;
            Speech::copList::iterator i = mCopsInFormation.begin();
            while (i != mCopsInFormation.end()) {
                EAXCop *copInFormation = *i;
                if (copInFormation && copInFormation->GetHandle() == cop->GetHandle()) {
                    found = true;
                    break;
                }
                ++i;
            }
            if (!found && !cop->IsHeli()) {
                mCopsInFormation.push_back(cop);
            }
        }
        ++iter;
    }

    if (mCopsInFormation.size() == 1) {
        mLastCopInFormation = mCopsInFormation.front();
    }
    if (!mCopsInFormation.empty()) {
        mT_outofFormation = WorldTimer;
    }
}

void SoundAI::ResetPursuit(bool including_music) {
    Speech::PursuitFlow_Reset(mPursuitFlow);
    Speech::StrategyFlow_Reset(mStrategyFlow);
    Speech::RoadblockFlow_Reset(mRoadblockFlow);
    Speech::Observer_Reset(mObserver);
    if (including_music) {
        Speech::MusicFlow_Reset(mMusicFlow);
    }
    Speech::Manager_ResetGlobalHistory();

    mFocus = kPursuitFlow;
    mQuadrantState = kReset;
    mRacerCount = 0;

    while (!mActors.empty()) {
        RemoveCop(mActors.begin()->hsimable);
    }

    mLeader = 0;
    mCopsInFormation.clear();
    mPursuit = 0;
    mAIPursuit = 0;
    mLastCopInFormation = 0;
    mLatestCop = 0;
    mHavoc = 0;
    mT_pursuitStart = WorldTimer;
    mPursuitState = kInactive;
    mPursuitDist = 0.0f;
    mT_lastCopNailed = Timer(0);
    mT_lastCrashed = Timer(0);
    mT_noLOS = Timer(0);
    mT_reallylowspeed = Timer(0);
    mT_outofFormation = Timer(0);
    mT_LOS = Timer(0);
    mCopsInView = 0;
    mInfraction = 0;
    mFlags = 0;
    mTrafficHits911 = 0;
    mCTS911 = 0;
}

void SoundAI::TerminatePursuit(BailoutType type) {
    if ((type == kOutrunBail) || (type != kForcedBail)) {
        bool cops_visible = false;
        const UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List &vehicles =
            UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AICOPS);
        UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List::const_iterator i = vehicles.begin();
        while (i != vehicles.end()) {
            IVehicle *vehicle = *i;
            IRenderable *renderable = 0;
            if (vehicle->QueryInterface(&renderable) && renderable->InView()) {
                cops_visible = true;
            }
            ++i;
        }
        Speech::Manager::ClearPlayback();
        if (!cops_visible) {
            MiscSpeech::LostSuspect(static_cast<int>(Speech_Manager_mLastSpeakerID));
        }
        mDispatch->BreakAway();
        mFocus = kLost;
        mQuadrantState = kInitial;
        mFlags &= ~DISP911_ACTIVE;
        return;
    }

    if (mMusicFlow) {
        Speech::SpeechFlow_ChangeStateTo(mMusicFlow, kTerminal);
    }

    bool is_DDay = false;
    GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
    if (parms) {
        is_DDay = parms->GetIsDDayRace();
    }

    if (!is_DDay) {
        Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
        if (cop_speech) {
            cop_speech->ReleaseResource();
        }
        Speech::Manager::ClearPlayback();
    }

    EAXCop *bailer = GetRandomActiveCop(0, false);
    if (bailer) {
        MiscSpeech::Bailout(bailer->GetSpeakerID());
    } else {
        MiscSpeech::Bailout(0);
    }
    mFocus = kTerminal;
}

void SoundAI::RemoveCop(HSIMABLE seeya) {
    if (mActors.size() == 0) {
        return;
    }

    EAXCop *cop = mActors.Remove(seeya);
    if (!cop) {
        return;
    }

    Speech::copList::iterator i = mCopsInFormation.begin();
    while (i != mCopsInFormation.end()) {
        EAXCop *copInFormation = *i;
        if (copInFormation && copInFormation->GetHandle() == cop->GetHandle()) {
            mCopsInFormation.erase(i);
            break;
        }
        ++i;
    }

    if (mLastCopInFormation && (mLastCopInFormation->GetHandle() == cop->GetHandle())) {
        mLastCopInFormation = 0;
    }
    if (mLeader == cop) {
        mLeader = 0;
    }
    if (mHeli == cop) {
        mHeli = 0;
    }
    if (mLatestCop == cop) {
        mLatestCop = 0;
    }

    Speech::PursuitFlow_OnCopRemoved(mPursuitFlow, cop);
    mUsage.voices.push_back(cop->GetSpeakerID());
    cop->SetActive(false);
}

void SoundAI::AddNewCop(IVehicle *newcop) {
    HSIMABLE newbie = newcop->GetSimable()->GetOwnerHandle();
    IVehicleAI *vai = newcop->GetAIVehiclePtr();
    int roadID = -1;
    bool is_rb_cop = false;

    if (vai) {
        WRoadNav *nav = vai->GetDriveToNav();
        if (nav) {
            roadID = nav->GetRoadSpeechId();
        }
    }

    bool is_cross = (newcop->GetVehicleKey() == static_cast<unsigned int>(-0x2c837f93));
    IRoadBlock *block = GetRoadblock();
    if (block) {
        is_rb_cop = (block->IsComprisedOf(newbie) == newcop);
    }

    int bID = GetBattalionFromKey(newcop->GetVehicleKey());
    if (bID < 1) {
        if (roadID == 0x6b) {
            return;
        }
        bID = GetBattalionFromRoadID(roadID);
        if (bID < 1) {
            bID = 1 << (bRandom(4) & 0x3F);
        }
    }

    EAXCop *latest_cop = 0;
    if (mUsage.voices.empty() || !is_cross) {
        UMath::Vector3 cop_pos = newcop->GetPosition();
        UMath::Vector3 delta;
        VU0_v3sub(mPlayerPos, cop_pos, delta);
        float distance = VU0_sqrt(VU0_v3lengthsquare(delta));

        Speech::copMap::iterator i = mActors.begin();
        while (i != mActors.end()) {
            EAXCop *cop = i->cop;
            if (!cop->IsActive() && !is_rb_cop && (distance < cop->GetDistance()) && !cop->IsHeli()) {
                if (cop->GetCallsign() != bID) {
                    int keyedID = GetBattalionFromKey(newcop->GetVehicleKey());
                    if (keyedID > 0) {
                        bID = keyedID;
                    } else if (roadID != 0x6b) {
                        keyedID = GetBattalionFromRoadID(roadID);
                        if (keyedID > 0) {
                            bID = keyedID;
                        } else {
                            bID = 1 << (bRandom(4) & 0x3F);
                        }
                    } else {
                        return;
                    }
                }

                mActors.ModifyHandle(cop->GetHandle(), newbie);
                cop->SetHandle(newbie);
                cop->SetCallsign(bID);
                cop->SetUnitNumber(GetCallsign(static_cast<Csis::Type_speaker_battalion>(bID)));
                cop->Reset();
                latest_cop = cop;
                break;
            }
            ++i;
        }
    }

    if (!latest_cop) {
        if (!mUsage.voices.empty() || !is_cross) {
            int type = 0;
            if (is_cross) {
                type = 3;
            } else if (is_rb_cop) {
                type = 2;
            }

            int voice = GetVoice(type);
            if (voice > 0) {
                int cID = GetCallsign(static_cast<Csis::Type_speaker_battalion>(bID));
                latest_cop = new EAXCop(voice, newbie, bID, cID);
                latest_cop->SetRank(mActors.size());
                mActors.Add(newbie, latest_cop);
                latest_cop->Reset();
            }
        }
        if (!latest_cop) {
            return;
        }
    }

    mLatestCop = latest_cop;
    if ((mPursuitState < kInactive) && !is_rb_cop && (mFocus == kStrategyFlow)) {
        float random = bRandom(1.0f);
        if (random <= 0.5f) {
            mLatestCop->PrimaryEngage();
        } else {
            mLatestCop->BackupArrives();
        }
    }
}

void SoundAI::ShuffleActors() {
    if (mActors.empty()) {
        return;
    }

    Speech::copList active;
    Speech::copList inactive;
    active.reserve(mActors.size());
    inactive.reserve(mActors.size());

    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive()) {
            active.push_back(cop);
        } else {
            inactive.push_back(cop);
        }
        ++iter;
    }

    Speech::copList::iterator i = inactive.begin();
    while (i != inactive.end()) {
        EAXCop *inact = *i;
        Speech::copList::iterator j = active.begin();
        while (j != active.end()) {
            EAXCop *act = *j;
            if (inact->GetSpeakerID() == act->GetSpeakerID()) {
                RemoveCop(inact->GetHandle());
                break;
            }
            ++j;
        }
        ++i;
    }

    bool changed = true;
    while (changed) {
        changed = false;
        active.clear();
        Speech::copMap::iterator it = mActors.begin();
        while (it != mActors.end()) {
            EAXCop *cop = it->cop;
            if (cop->IsActive()) {
                active.push_back(cop);
            }
            ++it;
        }

        Speech::copList::iterator a = active.begin();
        while (a != active.end()) {
            Speech::copList::iterator b = a;
            ++b;
            while (b != active.end()) {
                if ((*a)->GetSpeakerID() == (*b)->GetSpeakerID() && ((*a)->GetHandle() != (*b)->GetHandle())) {
                    if ((*b)->GetDistance() <= (*a)->GetDistance()) {
                        RemoveCop((*a)->GetHandle());
                    } else {
                        RemoveCop((*b)->GetHandle());
                    }
                    changed = true;
                    break;
                }
                ++b;
            }
            if (changed) {
                break;
            }
            ++a;
        }
    }

    active.clear();
    Speech::copMap::iterator final_iter = mActors.begin();
    while (final_iter != mActors.end()) {
        EAXCop *cop = final_iter->cop;
        if (cop->IsActive()) {
            active.push_back(cop);
        }
        ++final_iter;
    }

    if (!active.empty()) {
        if (!mLeader || !mLeader->IsActive() || mLeader->IsHeli()) {
            Speech::copList::iterator c = active.begin();
            while (c != active.end()) {
                EAXCop *cop = *c;
                if (cop->IsPrimary() && cop->HasLOS()) {
                    mLeader = cop;
                    if ((mFocus == kStrategyFlow) && !mLeader->HasLOS()) {
                        mLeader->SpotterWanted();
                    }
                    break;
                }
                ++c;
            }
        }
        if (mLeader && !mLeader->IsPrimary()) {
            MakeLeader(mLeader);
        }
    }
}
