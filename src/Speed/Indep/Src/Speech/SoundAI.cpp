#include "SoundAI.h"
#include "MusicFlow.h"
#include "Observer.h"
#include "PursuitFlow.h"
#include "RoadblockFlow.h"
#include "StrategyFlow.h"
#include "Speed/Indep/Src/Speech/EAXDispatch.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
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
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
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
#include "Speed/Indep/Src/Speech/SpeechCache.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"

namespace MiscSpeech {
bool GetLocation(RoadNames road, int &region, int &location);
int LostSuspect(int speaker);
int Bailout(int speaker);
void QuadrantForming();
void SuspectPossiblyGone();
void QuadrantMoving();
void OtherLead();
void PossibleSuspect();
void WrongSuspect();
}

extern int FORCE_VOICE_RANDOMIZATION;
extern bool IsSpeechEnabled;
extern ParameterAccessor SPAMAccessorSpeech;
struct CarPart;
struct CarPartDatabase;
struct ColourHashToSoundColour {
    unsigned int Hash;
    unsigned int SoundColour;
};
extern CarPartDatabase CarPartDB;
extern CarTypeInfo *CarTypeInfoArray;
extern ColourHashToSoundColour ColourHashToSoundColourMap[];
extern int NumberOfColourHashToSoundColourMaps;

extern "C" CarPart *GetInstalledPart__C21FECustomizationRecord7CarTypei(const FECustomizationRecord *record, CarType car_type, int car_slot_id);
extern "C" unsigned int GetAppliedAttributeUParam__7CarPartUiUi(CarPart *part, unsigned int name_hash, unsigned int default_value);
extern "C" CarPart *NewGetCarPart__15CarPartDatabase7CarTypeiUiP7CarParti(
    CarPartDatabase *db,
    CarType car_type,
    int car_slot_id,
    unsigned int car_part_namehash,
    CarPart *prev_part,
    int upg_level);

static float prev_heat_30802 = 0.0f;
static unsigned int dir_tracking_30959 = 0;
static Timer t_currdir_30960 = 0;
static unsigned int dir_init_30961 = 0;
static float CopMinClosingVelSq;
static int DESTROY_COPS_ON_INACTIVITY;

SoundAI::SoundAI()
    : Sim::Activity(1),
      mMainUpdate(AddTask("Speech", 1.0f, 0.0f, Sim::TASK_FRAME_FIXED)),
      mProcessObservations(AddTask("Comment", 1.0f, 0.0f, Sim::TASK_FRAME_FIXED)),
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
      mTrafficHits911(0),
      mCTS911(0),
      mHavoc(0),
      mPursuitCount(0),
      mNumRoadBlocks(0),
      mRacerCount(0),
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
      mMsgPerpBusted(Hermes::Handler::Create<MPerpBusted, SoundAI, SoundAI>(
          this, &SoundAI::MessagePerpBusted, UCrc32(0x20D60DBF), 0)),
      mMsgAIPerpBusted(Hermes::Handler::Create<MPerpBusted, SoundAI, SoundAI>(
          this, &SoundAI::MessageAIPerpBusted, "AIRacerBusted", 0)),
      mMsgRestartRace(Hermes::Handler::Create<MRestartRace, SoundAI, SoundAI>(
          this, &SoundAI::MessageRestart, UCrc32(0x20D60DBF), 0)),
      mMsgInfraction(Hermes::Handler::Create<MMiscSound, SoundAI, SoundAI>(
          this, &SoundAI::MessageInfraction, "Infraction", 0)),
      mMsgUnspawnCop(Hermes::Handler::Create<MUnspawnCop, SoundAI, SoundAI>(
          this, &SoundAI::MessageUnspawnCop, "SoundAI", 0)),
      mMsgTireBlown(Hermes::Handler::Create<MGamePlayMoment, SoundAI, SoundAI>(
          this, &SoundAI::MessageTireBlown, "TireBlo", 0)) {

    SoundAI::mRefCount = 1;
    mCopsInFormation.clear();
    mRecentBlowby.distance = 32767.0f;
    mRecentBlowby.speed = 0.0f;
    mRecentBlowby.timestamp = Timer(0);

    Sim::ProfileTask(mMainUpdate, "Speech");
    Sim::ProfileTask(mProcessObservations, "Speech");

    mTune.ChangeWithDefault(0);
    mPursuitLevel.ChangeWithDefault(0);

    mActorPool = bNewSlotPool(0xA0, 10, "VoiceActors slotpool", AudioMemoryPool);
    mActorPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    mActorPool->ClearFlag(SLOTPOOL_FLAG_WARN_IF_NONEMPTY_DELETE);

    mDispatch = new EAXDispatch(1);
    mPursuitFlow = new ("SpeechFlow: Pursuit", 0) Speech::PursuitFlow;
    mStrategyFlow = new ("SpeechFlow: Strategy", 0) Speech::StrategyFlow;
    mObserver = new ("SpeechFlow: Observer", 0) Speech::Observer;
    mRoadblockFlow = new ("SpeechFlow: Roadblock", 0) Speech::RoadblockFlow;
    mMusicFlow = new ("MusicFlow: Interactive", 0) Speech::MusicFlow;

    for (int i = 0; i < 2; i++) {
        mPlayerCurrent[i].direction = CalcPlayerDirection(false);
        mAICurrent[i].direction = 0;
        mAICurrent[i].roadID = untagged;
        mPlayerCurrent[i].roadID = untagged;
    }

    mLastKnown.roadID = untagged;
    mAILastKnown.direction = 0;
    mLastKnown.direction = 0;
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
    }

    Sim::Collision::RemoveListener(this);
    mLeader = 0;
    mHeli = 0;

    if (mPursuitFlow) {
        delete mPursuitFlow;
    }
    mPursuitFlow = 0;
    if (mStrategyFlow) {
        delete mStrategyFlow;
    }
    mStrategyFlow = 0;
    if (mObserver) {
        delete mObserver;
    }
    mObserver = 0;
    if (mRoadblockFlow) {
        delete mRoadblockFlow;
    }
    mRoadblockFlow = 0;
    if (mMusicFlow) {
        delete mMusicFlow;
    }
    mMusicFlow = 0;

    if (mPlayerCarCustom) {
        delete mPlayerCarCustom;
    }
    mPlayerCarCustom = 0;
    if (mAICarCustom) {
        delete mAICarCustom;
    }
    mAICarCustom = 0;

    if (mMsgPerpBusted) {
        Hermes::Handler::Destroy(mMsgPerpBusted);
    }
    if (mMsgAIPerpBusted) {
        Hermes::Handler::Destroy(mMsgAIPerpBusted);
    }
    if (mMsgForcePursuitStart) {
        Hermes::Handler::Destroy(mMsgForcePursuitStart);
    }
    if (mMsgRestartRace) {
        Hermes::Handler::Destroy(mMsgRestartRace);
    }
    if (mMsgUnspawnCop) {
        Hermes::Handler::Destroy(mMsgUnspawnCop);
    }
    if (mMsgInfraction) {
        Hermes::Handler::Destroy(mMsgInfraction);
    }
    if (mMsgTireBlown) {
        Hermes::Handler::Destroy(mMsgTireBlown);
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
    Speech::Observer *observer;
    int speaker;

    if (mRoadblockFlow) {
        mRoadblockFlow->NailedSomethingInRB(0x10);
    }
    spkr = FindClosestCop(true, true);
    observer = mObserver;
    if (observer && spkr) {
        speaker = spkr->GetSpeakerID();
        observer->Observe(0xE, speaker, 1.0f);
    }
}

void SoundAI::OnVehicleAdded(IVehicle *ivehicle) {
    Sim::Collision::AddListener(this, ivehicle, "SoundAI");
}

void SoundAI::OnCollision(const COLLISION_INFO &cinfo) {
    if (VU0_v3lengthsquare(cinfo.closingVel) < CopMinClosingVelSq) {
        return;
    }

    short actors_involved = 0;
    short objects_visible = 0;
    IRenderable *renderA;
    IRenderable *renderB;
    EAXCop *actorA = 0;
    EAXCop *actorB = 0;
    ISimable *simableA;
    if (cinfo.objA) {
        simableA = ISimable::FindInstance(cinfo.objA);
    } else {
        simableA = 0;
    }
    ISimable *simableB;
    if (cinfo.objB) {
        simableB = ISimable::FindInstance(cinfo.objB);
    } else {
        simableB = 0;
    }

    if (simableA) {
        actorA = mActors.Find(cinfo.objA);
        if (actorA) {
            actors_involved++;
        }
        renderA = 0;
        if (simableA->QueryInterface(&renderA) && renderA->InView()) {
            objects_visible++;
        }
    }
    if (simableB) {
        actorB = mActors.Find(cinfo.objB);
        if (actorB) {
            actors_involved++;
        }
        renderB = 0;
        if (simableB->QueryInterface(&renderB) && renderB->InView()) {
            objects_visible++;
        }
    }
    if (!simableA && !simableB) {
        return;
    }

    float collisionspeed = cinfo.impulseA + cinfo.impulseB;
    float minspeed = mTune.PlayerSmashSpeedRange(0) * 0.44703f;
    float maxspeed = mTune.PlayerSmashSpeedRange(1) * 0.44703f;
    float intensity = UMath::Clamp((collisionspeed - minspeed) / (maxspeed - minspeed), 0.0f, 1.0f);

    if (cinfo.type != Sim::Collision::Info::OBJECT) {
        if (cinfo.type < Sim::Collision::Info::WORLD) {
            return;
        }
        if (cinfo.type == Sim::Collision::Info::WORLD) {
            goto collision_world;
        }
        return;
    }

    {
        if (actors_involved == 2) {
            return;
        }

        if (actors_involved == 1) {
            IVehicle *theOtherCar;
            ISimable *theOtherObj = simableA ? simableA : simableB;
            EAXCop *actor = actorA ? actorA : actorB;

            if (!theOtherObj->QueryInterface(&theOtherCar)) {
                return;
            }

            switch (theOtherCar->GetDriverClass()) {
            case DRIVER_HUMAN: {

            bool cop_is_in_rb = false;
            bool cop_rammed = false;
            bool cop_is_suv = false;
            bool cop_is_braking = false;
            IRigidBody *rbCop;
            IRigidBody *rbRacer;
            IVehicle *vehicleCop;
            IInput *inputcop;
            ISimable *simableCop = simableA;
            IRoadBlock *block;
            if (theOtherObj == simableA) {
                simableCop = simableB;
            }
            if (!simableCop) {
                return;
            }

            inputcop = 0;
            if (simableCop->QueryInterface(&inputcop)) {
                float brake = UMath::Clamp(inputcop->GetControls().fBrake, 0.0f, 1.0f);
                float ebrake = UMath::Clamp(inputcop->GetControls().fHandBrake, 0.0f, 1.0f);
                if ((brake > 0.0f) || (ebrake > 0.0f)) {
                    cop_is_braking = true;
                }
            }

            if ((mFocus == kPursuitFlow) && ((mFlags & SETUP_RESTARTED) == 0)) {
                Speech::PursuitFlow::PursuitCause cause = mPursuitFlow->GetPursuitCause();
                if ((cause != Speech::PursuitFlow::kScripted) && (cause != Speech::PursuitFlow::kCopAssaulted) &&
                    (cause != Speech::PursuitFlow::kCopAssaultedScripted) && mPursuitFlow->RequiresRestart()) {
                    Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(COPSPEECH_MODULE);
                    if (cop_speech) {
                        Speech::Manager::ClearPlayback();
                        cop_speech->ReleaseResource();
                        mT_lastCopNailed = WorldTimer;
                        actor->WasRammed();
                        if (mPursuitFlow->GetPursuitCause() == Speech::PursuitFlow::kScripted) {
                            mPursuitFlow->SetPursuitCause(Speech::PursuitFlow::kCopAssaultedScripted);
                        } else {
                            mPursuitFlow->SetPursuitCause(Speech::PursuitFlow::kCopAssaulted);
                        }
                        mPursuitFlow->ChangeStateTo(Speech::PursuitFlow::kPrimaryBranch);
                        mPursuitFlow->Update();
                        mFlags |= SETUP_RESTARTED;
                    }
                }
            }

            block = GetRoadblock();
            if (block) {
                IVehicle *car = block->IsComprisedOf(simableCop->GetOwnerHandle());
                if (car) {
                    cop_is_in_rb = true;
                    if (UTL::COM::ComparePtr(car, simableCop)) {
                        mRoadblockFlow->NailedSomethingInRB(0x40);
                    }
                }
            }
            if (!cop_is_in_rb) {
                mObserver->Observe(3, -1, intensity);
            }

            vehicleCop = 0;
            simableCop->QueryInterface(&vehicleCop);
            if (vehicleCop) {
                unsigned int vtype = vehicleCop->GetVehicleAttributes().GetCollection();
                if ((vtype == 0x38b38226) || (vtype == 0x54b10e38) || (vtype == 0x2e149eac)) {
                    cop_is_suv = true;
                }
            }

            rbCop = simableCop->GetRigidBody();
            rbRacer = theOtherObj->GetRigidBody();

            UMath::Vector3 armCop = actorA ? cinfo.armB : cinfo.armA;
            UMath::Vector3 armRacer = actorA ? cinfo.armA : cinfo.armB;
            UMath::Vector3 fwCop;
            UMath::Vector3 fwRacer;
            UMath::Vector3 dimCop;
            UMath::Vector3 dimRacer;
            UMath::Vector3 velRacer;
            UMath::Vector3 velCop;
            UMath::Vector3 cnormal = cinfo.normal;
            rbCop->GetForwardVector(fwCop);
            rbCop->GetDimension(dimCop);
            velRacer = (theOtherObj->GetOwnerHandle() == cinfo.objA) ? cinfo.objAVel : cinfo.objBVel;
            rbRacer->GetForwardVector(fwRacer);
            rbRacer->GetDimension(dimRacer);
            velCop = (simableCop->GetOwnerHandle() == cinfo.objA) ? cinfo.objAVel : cinfo.objBVel;

            VehicleImpactType coll_type = kUnknown;
            float fwDot = UMath::Dot(fwRacer, fwCop);
            float vel_norm_dotRacer = bAbs(UMath::Dot(velRacer, cnormal));
            float vel_norm_dotCop = bAbs(UMath::Dot(velCop, cnormal));

            if (fwDot >= 0.7f) {
                if ((bAbs(armCop.x) > dimCop.x * 0.75f) && (bAbs(armRacer.x) > dimRacer.x * 0.75f)) {
                    coll_type = kCopSSPerp;
                    if (vel_norm_dotRacer <= vel_norm_dotCop) {
                        coll_type = kPerpSSCop;
                        cop_rammed = true;
                    }
                } else {
                    if ((armCop.z >= dimCop.z * 0.75f) && (armRacer.z <= armRacer.z * -0.75f)) {
                        coll_type = kCopREperp;
                    }
                    if ((armRacer.z >= dimRacer.z * 0.75f) && (armCop.z <= dimCop.z * -0.75f)) {
                        float speedCop = VU0_v3length(velCop);
                        float speedRacer = VU0_v3length(velRacer);
                        if (speedCop <= speedRacer) {
                        } else {
                            cop_rammed = true;
                        }
                        coll_type = kPerpRECop;
                    }
                }
            } else if (fwDot > -0.7f) {
                if (armRacer.z >= dimRacer.z * 0.7f) {
                    coll_type = kPerpTBCop;
                    cop_rammed = true;
                }
                if (armCop.z >= dimCop.z * 0.7f) {
                    coll_type = kCopTBPerp;
                }
            } else {
                if (vel_norm_dotCop <= vel_norm_dotRacer) {
                    coll_type = kCopHOPerp;
                } else {
                    coll_type = kPerpHOCop;
                    cop_rammed = true;
                }
            }

            if (!cop_rammed) {
                return;
            }
            mT_lastCopNailed = WorldTimer;
            if (cop_is_in_rb || cop_is_suv || cop_is_braking || ((mFlags & COPS_IMMUNE) != 0)) {
                return;
            }
            actor->WasRammed();
            if ((mFocus == kPursuitFlow) || (mFocus == kTerminal) || actor->IsDead() || !actor->IsActive()) {
                return;
            }

            if (intensity > 0.15f) {
                if (intensity <= 0.5f) {
                    Csis::Type_intensity csis_intensity = Csis::Type_intensity_Normal;
                    if (coll_type == kPerpRECop) {
                        actor->RearEnded(csis_intensity);
                    } else if (coll_type == kPerpTBCop) {
                        actor->TBoned(csis_intensity);
                    } else if (coll_type == kPerpHOCop) {
                        actor->HeadOn(csis_intensity);
                    } else if (coll_type == kPerpSSCop) {
                        actor->SideSwiped(csis_intensity);
                    }
                    return;
                }
            }
            if (intensity <= 0.5f) {
                return;
            }
            if (intensity <= 0.75f) {
                float rand_select = bRandom(1.0f);
                if (rand_select < 0.67f) {
                    Csis::Type_intensity csis_intensity = Csis::Type_intensity_High;
                    if (coll_type == kPerpRECop) {
                        actor->RearEnded(csis_intensity);
                    } else if (coll_type == kPerpTBCop) {
                        actor->TBoned(csis_intensity);
                    } else if ((coll_type == kPerpHOCop) && !cop_is_suv) {
                        actor->HeadOn(csis_intensity);
                    } else if (coll_type == kPerpSSCop) {
                        actor->SideSwiped(csis_intensity);
                    } else {
                        actor->InterruptExpletive();
                    }
                } else {
                    actor->InterruptExpletive();
                }
            } else {
                actor->InterruptViolent();
            }
            return;
            }

            case DRIVER_TRAFFIC:
                mObserver->Observe(2, -1, intensity);
                actor->JustHitTraffic();
                if ((mPursuitState == kActive) && (mFocus == kStrategyFlow) && actor->IsActive()) {
                    actor->BailoutTraffic();
                    RandomBailoutDeny(actor);
                }
                return;

            default:
                return;
            }
        }

        IVehicle *pvehicle = 0;
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);
        if (!player) {
            return;
        }

        if (UTL::COM::ComparePtr(player->GetSimable(), simableA)) {
            simableA->QueryInterface(&pvehicle);
        } else if (UTL::COM::ComparePtr(player->GetSimable(), simableB)) {
            simableB->QueryInterface(&pvehicle);
        }
        if (!pvehicle) {
            return;
        }

        ISimable *otherObj = simableA;
        if (pvehicle->GetSimable()->GetOwnerHandle() == cinfo.objA) {
            otherObj = simableB;
        }
        IModel *model;
        if (otherObj) {
            model = otherObj->GetModel();
        } else {
            model = 0;
        }
        IVehicle *otherVehicle = 0;
        if (otherObj) {
            otherObj->QueryInterface(&otherVehicle);
        }

        float collisionspeed = cinfo.impulseA + cinfo.impulseB;
        float minspeed = mTune.PlayerSmashSpeedRange(0) * 0.44703f;
        float maxspeed = mTune.PlayerSmashSpeedRange(1) * 0.44703f;
        float intensity = UMath::Clamp((collisionspeed - minspeed) / (maxspeed - minspeed), 0.0f, 1.0f);

        const UMath::Vector3 &player_vel =
            (pvehicle->GetSimable()->GetOwnerHandle() == cinfo.objA) ? cinfo.objAVel : cinfo.objBVel;
        float speed_b4_impact = VU0_v3length(player_vel);
        float curr_speed = pvehicle->GetSimable()->GetRigidBody()->GetSpeed();
        float pct_decrease = 1.0f;
        if (speed_b4_impact > 0.0f) {
            pct_decrease = curr_speed / speed_b4_impact;
        }
        if (pct_decrease < (1.0f - mTune.CrashSlowdownPct())) {
            mT_lastCrashed = WorldTimer;
        }

        if (model) {
            model->GetAttributes().GetCollection();
            if (model->IsRootModel() && (mPursuitState == kInactive)) {
                Attrib::Gen::smackable obj_atr(model->GetAttributes());
                mCTS911 += obj_atr.COST_TO_STATE();
            }
        }

        if (GetRoadblock()) {
            const IRoadBlock::Smackables &objects = GetRoadblock()->GetSmackables();
            if (objects.size() != 0) {
                for (IRoadBlock::Smackables::const_iterator i = objects.begin(); i != objects.end(); ++i) {
                    IPlaceableScenery *object = *i;
                    if (UTL::COM::ComparePtr(object, model) && model) {
                        if (model->GetAttributes().GetCollection() == 0xca89ef8f) {
                            mRoadblockFlow->NailedSomethingInRB(0x10);
                            EAXCop *spkr = FindClosestCop(false, true);
                            int spkrID;
                            if (spkr) {
                                spkrID = spkr->GetSpeakerID();
                            } else {
                                spkrID = -1;
                            }
                            mObserver->Observe(0xe, spkrID, intensity);
                        } else {
                            mRoadblockFlow->NailedSomethingInRB(0x20);
                        }
                    }
                }
            }
        }

        if (!otherVehicle) {
            return;
        }
        switch (otherVehicle->GetDriverClass()) {
        case DRIVER_HUMAN:
            mObserver->Observe(6, -1, intensity);
            return;
        case DRIVER_TRAFFIC:
        case DRIVER_NONE: {
            mObserver->Observe(7, -1, intensity);
            int spkrID = -1;
            EAXCop *spkr = FindClosestCop(true, true);
            if (spkr) {
                spkrID = spkr->GetSpeakerID();
            }
            if ((otherVehicle->GetVehicleClass() == VehicleClass::TRACTOR) || (otherVehicle->GetVehicleClass() == VehicleClass::TRAILER)) {
                if (intensity >= mTune.MinIntensityTrafficSmash()) {
                    mObserver->Observe(0xc, spkrID, intensity);
                }
            } else {
                mObserver->Observe(7, spkrID, intensity);
            }
            if (intensity >= mTune.MinIntensityTrafficSmash()) {
                mTrafficHits911++;
            }
            return;
        }
        }
        return;
    }

collision_world: {
    ISimable *isimable = simableA;
    IVehicle *ivehicle;
    float collisionspeed = cinfo.impulseA + cinfo.impulseB;
    float minspeed = mTune.PlayerSmashSpeedRange(0) * 0.44703f;
    float maxspeed = mTune.PlayerSmashSpeedRange(1) * 0.44703f;
    float intensity = UMath::Clamp((collisionspeed - minspeed) / (maxspeed - minspeed), 0.0f, 1.0f);
    if (!isimable->QueryInterface(&ivehicle)) {
        return;
    }
    switch (ivehicle->GetDriverClass()) {
    case DRIVER_HUMAN: {
        const UMath::Vector3 &player_vel = cinfo.objAVel;
        float speed_b4_impact = VU0_v3length(player_vel);
        float curr_speed = isimable->GetRigidBody()->GetSpeed();
        float pct_decrease = 1.0f;
        if (speed_b4_impact > 0.0f) {
            pct_decrease = curr_speed / speed_b4_impact;
        }
        if (pct_decrease >= (1.0f - mTune.CrashSlowdownPct())) {
            return;
        }
        mT_lastCrashed = WorldTimer;
        if ((mPursuitState == kActive) && (mFocus == kStrategyFlow)) {
            EAXCop *actor = GetRandomActiveCop(0, true);
            if (actor) {
                mObserver->Observe(5, actor->GetSpeakerID(), intensity);
            }
        }
        return;
    }
    case DRIVER_COP:
        break;
    default:
        return;
    }

    EAXCop *actor = actorA ? actorA : actorB;
    if (!actor || (objects_visible <= 0) || !actor->IsActive()) {
        return;
    }
    if (intensity >= mTune.MinIntensityCopSmash()) {
        if (mObserver && mObserver->WeatherExists()) {
            actor->BailoutBadRoad();
        } else {
            actor->Bailout();
        }
    }
    if (actor->GetInFormation() && (mCopsInFormation.size() > 1)) {
        RandomBailoutDeny(actor);
    }
    mObserver->Observe(4, actor ? actor->GetSpeakerID() : -1, intensity);
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

    (void)ivehicle;
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
                int active = pursuit->ContingentHasActiveCops();
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
    if (mMusicFlow) {
        result = mMusicFlow->GetState() != -1;
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
    int return_voice = -1;

    if (mUsage.voices.empty()) {
        return -1;
    }

    switch (type) {
    case 1:
        for (int *i = mUsage.voices.begin(); i < mUsage.voices.end(); ++i) {
            if (static_cast<unsigned int>(*i - 3) < 3) {
                return_voice = *i;
                mUsage.voices.erase(i);
                break;
            }
        }
        break;
    case 2:
        for (int *i = mUsage.voices.begin(); i < mUsage.voices.end(); ++i) {
            if (static_cast<unsigned int>(*i - 6) < 3) {
                return_voice = *i;
                mUsage.voices.erase(i);
                break;
            }
        }
        break;
    case 3:
        for (int *i = mUsage.voices.begin(); i < mUsage.voices.end(); ++i) {
            if (*i == 9) {
                return_voice = 9;
                mUsage.voices.erase(i);
                break;
            }
        }
        break;
    default:
        for (int *i = mUsage.voices.begin(); i < mUsage.voices.end(); ++i) {
            if (static_cast<unsigned int>(*i - 3) < 6) {
                return_voice = *i;
                mUsage.voices.erase(i);
                break;
            }
        }
        break;
    }
    return return_voice;
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

    IVehicle *vehicle = 0;
    IPerpetrator *perp = 0;
    IVehicleAI *vai = 0;
    player->GetSimable()->QueryInterface(&vehicle);
    player->GetSimable()->QueryInterface(&perp);
    vehicle->QueryInterface(&vai);

    WRoadNav *nav = vai->GetDriveToNav();

    mPlayerCurrent[0].roadID = static_cast<RoadNames>(nav->GetRoadSpeechId());
    unsigned int dir = CalcPlayerDirection(false);

    mPlayerCurrent[0].direction = dir;

    if (IsHeadingValid()) {
        bool road_changed = (mPlayerCurrent[1].roadID != mPlayerCurrent[0].roadID);
        mPlayerCurrent[1].roadID = mPlayerCurrent[0].roadID;
        mPlayerCurrent[1].direction = mPlayerCurrent[0].direction;
        if ((mLastKnown.direction != dir) && mPursuit && mPursuit->IsPerpInSight() && road_changed) {
            if ((mPursuitState == kActive) && (mFocus != kPursuitFlow)) {
                EAXCop *cop = FindClosestCop(true, true);
                if (cop) {
                    cop->DirectionChange();
                }
            }
            mLastKnown.direction = dir;
            mLastKnown.roadID = mPlayerCurrent[0].roadID;
        }
    }

    if (!perp) {
        return;
    }

    if (mPVehicle.GetCollection() != player->GetSimable()->GetAttributes().GetCollection()) {
        mPVehicle.ChangeWithDefault(player->GetSimable()->GetAttributes().GetCollection());
    }

    if (!mPlayerCarCustom || (mPlayerCarCustom->color == 0)) {
        if (mPlayerCarCustom) {
            delete mPlayerCarCustom;
        }
        mPlayerCarCustom = new ("SoundAI CarCustomization", 0) CarCustomizations;
        if (!GetCustomized(vehicle, *mPlayerCarCustom)) {
            delete mPlayerCarCustom;
            mPlayerCarCustom = 0;
        }
    }

    mPlayerSpeed = MPS2MPH(vehicle->GetSpeed());
    if ((mPlayerSpeed < mTune.MinSpeedConsideredStopped()) && ((mFlags & LOWSPEEDTIMER) == 0)) {
        mFlags |= LOWSPEEDTIMER;
        mT_reallylowspeed = WorldTimer;
    } else if (mTune.MinSpeedConsideredStopped() <= mPlayerSpeed) {
        mFlags &= ~LOWSPEEDTIMER;
        mT_reallylowspeed = WorldTimer;
    }

    UMath::Vector3 vel;

    mPlayerPos = player->GetPosition();
    if (SPAMAccessorSpeech.IsValid()) {
        const bVector2 ppos(mPlayerPos.z, -mPlayerPos.x);
        SPAMAccessorSpeech.CaptureData(ppos.x, ppos.y);
        mPlayerOffroadID = SPAMAccessorSpeech.GetDataInt(1);
    }

    int heat = static_cast<int>(perp->GetHeat());
    mPlayerHeat = heat;

    if ((mFocus != kPursuitFlow) && (60.0f < mPursuitDuration) && (prev_heat_30802 < perp->GetHeat())) {
        bool jump = false;
        int i = 3;
        while (i > -1) {
            if ((prev_heat_30802 < heat_cutoffs[i].value) && (heat_cutoffs[i].value <= perp->GetHeat())) {
                jump = true;
                prev_heat_30802 = perp->GetHeat();
                break;
            }
            i--;
        }

        if (jump && (mPursuitState < kInactive)) {
            if (bRandom(1.0f) > 0.5f) {
                EAXCop *cop = GetRandomActiveCop(1, false);
                if (mHeli && (bRandom(1.0f) > 0.5f)) {
                    mHeli->HeatJump(heat_cutoffs[i].heat_level);
                } else if (cop) {
                    cop->HeatJump(heat_cutoffs[i].heat_level);
                }
            } else {
                mDispatch->HeatJump(heat_cutoffs[i].heat_level);
            }

            if (bRandom(1.0f) > 0.5f) {
                if ((3.0f <= perp->GetHeat()) && (perp->GetHeat() < 6.0f)) {
                    mDispatch->JurisShift(Csis::Type_jurisdiction_state);
                } else if (6.0f <= perp->GetHeat()) {
                    mDispatch->JurisShift(Csis::Type_jurisdiction_federal);
                }
            }
        }
    }

    Attrib::Gen::pursuitlevels *pursuitatr = perp->GetPursuitLevelAttrib();
    if (pursuitatr && pursuitatr->IsValid()) {
        if (mPursuitLevel.GetCollection() != pursuitatr->GetCollection()) {
            mPursuitLevel.ChangeWithDefault(pursuitatr->GetCollection());
        }
    }

    if (mPursuitLevel.IsValid()) {
        if (0.0f < mPursuitLevel.roadblockprobability()) {
            mFlags |= RB_ENABLED;
        } else {
            mFlags &= ~RB_ENABLED;
        }
        if (0.0f < mPursuitLevel.roadblockhelichance()) {
            mFlags |= HELIRB_ENABLED;
        } else {
            mFlags &= ~HELIRB_ENABLED;
        }
        if (0.0f < mPursuitLevel.roadblockspikechance(0)) {
            mFlags |= SPIKES_ENABLED;
        } else {
            mFlags &= ~SPIKES_ENABLED;
        }

        mNumCopsInWave = mPursuitLevel.NumCopsToTriggerBackup();

        if ((mPursuitState == kInactive) && ((mFlags & DISP911_ACTIVE) == 0)) {
            bool is_DDay = false;
            bool is_Race = false;
            bool is_Roaming = false;
            if ((GRaceStatus::Exists() && (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming)) ||
                (GRaceDatabase::Exists() && !GRaceDatabase::Get().GetStartupRace())) {
                is_Roaming = true;
            }

            ICopMgr *copmgr = UTL::Collections::Singleton<ICopMgr>::Get();
            if (copmgr && ICopMgr::AreCopsEnabled()) {
                float t_lockout = copmgr->GetLockoutTimeRemaining();
                if (GRaceStatus::Get().GetRaceParameters()) {
                    if (GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
                        is_DDay = true;
                    }
                    if (GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {
                        is_Race = true;
                    }
                }
                bool scripted_911 = ((mPursuitLevel.Lifetime911(0) < t_lockout) && (t_lockout < mPursuitLevel.Lifetime911(1)) && !is_Race &&
                                     !is_Roaming);
                bool req911_met = false;
                if ((mCTS911 >= mPursuitLevel.CTSFor911()) || (mTrafficHits911 >= mPursuitLevel.NumCiviHitsFor911(0))) {
                    req911_met = true;
                }

                if ((req911_met || scripted_911) && !is_DDay && mDispatch) {
                    mDispatch->Report911(IsHighIntensity() ? Csis::Type_pursuit_type_Possible_Wanted : Csis::Type_pursuit_type_Generic_Speeder);
                }
            }
        }
    }

    UMath::Unit(player->GetSimable()->GetRigidBody()->GetLinearVelocity(), vel);
    mSmoothedFWRoad.x = (mSmoothedFWRoad.x * 0.9f) + (vel.x * 0.1f);
    mSmoothedFWRoad.y = (mSmoothedFWRoad.y * 0.9f) + (vel.y * 0.1f);
    mSmoothedFWRoad.z = (mSmoothedFWRoad.z * 0.9f) + (vel.z * 0.1f);
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

    if (!Speech::Manager::IsCopSpeechBusy()) {
        tout = (WorldTimer - Speech::Manager::GetTimeSinceLastEvent(COPSPEECH_MODULE)).GetSeconds();
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
            mObserver->Update();
            mRoadblockFlow->Update();
        }
        Speech::Manager::Deduce();
    }
    return true;
}

void SoundAI::UpdateStateMachines() {
    if ((IsSpeechEnabled != 0) && ((mFlags & BUSTED) == 0)) {
        switch (mFocus) {
        case kRoadblockFlow:
            goto update_strategy;

        case kPursuitFlow:
            mPursuitFlow->Update();
            if (mPursuitFlow->IsTransitionable()) {
                mFocus = kStrategyFlow;
                mStrategyFlow->ChangeStateTo(0);
            }
            break;

        case kStrategyFlow:
        update_strategy:
            if (mStrategyFlow->IsTransitionable()) {
                mStrategyFlow->ChangeStateTo(kWaiting);
            }
            mStrategyFlow->Update();
            break;

        case kLost:
            if ((mPursuit != 0) && (mPursuitState == kSearching)) {
                if (mStrategyFlow->GetState() != kOtherTarget) {
                    mStrategyFlow->ChangeStateTo(kOtherTarget);
                }
                mStrategyFlow->Update();
            }
            break;

        default:
            break;
        }
    }

    mMusicFlow->Update();
}

void SoundAI::AttemptReattachPursuit() {
    short playerfound = false;
    short aifound = false;

    if (mPursuit && mPursuit->IsPlayerPursuit()) {
        playerfound = true;
    }
    aifound = mAIPursuit && !mAIPursuit->IsPlayerPursuit();

    const UTL::Collections::Listable<IPursuit, 8>::List &pursuits = UTL::Collections::Listable<IPursuit, 8>::GetList();
    if (pursuits.size() != 0) {
        UTL::Collections::Listable<IPursuit, 8>::List::const_iterator i = pursuits.begin();
        if (!playerfound) {
            while (i != pursuits.end()) {
                IPursuit *pursuit = *i;
                if (pursuit->IsPlayerPursuit() && !playerfound) {
                    playerfound = true;
                    if (mPursuit != pursuit) {
                        mPursuitCount++;
                        if (!mPursuit) {
                            mT_pursuitStart = WorldTimer;
                        } else {
                            if (IsAttached(mPursuit)) {
                                Detach(mPursuit);
                            }
                            mPursuit = 0;
                        }
                        if (!IsAttached(pursuit)) {
                            Attach(pursuit);
                        }
                        mPursuit = pursuit;
                    }
                } else if (!aifound) {
                    aifound = true;
                    if (mAIPursuit != pursuit) {
                        if (mAIPursuit) {
                            if (IsAttached(mAIPursuit)) {
                                Detach(mAIPursuit);
                            }
                            mAIPursuit = 0;
                        }
                        if (!IsAttached(pursuit)) {
                            if (Attach(pursuit)) {
                                mAIPursuit = pursuit;
                            }
                        }
                        mAIPursuit = pursuit;
                    }
                }
                ++i;
            }
        }
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

    if (!mPursuit) {
        if ((mPursuitState != kOtherTarget) && (mPursuitState != kInactive)) {
            if (mAIPursuit && (mRacerCount > 0)) {
                mPursuitState = kOtherTarget;
            } else {
                mPursuitState = kInactive;
                mT_sinceLastPursuit = WorldTimer;
            }
        }
    } else {
        bool ai_searching = false;
        if (mPursuit->AttemptingToReAquire() || !mPursuit->IsPerpInSight() || (mPursuit->GetPursuitStatus() == PS_COOL_DOWN)) {
            ai_searching = true;
        }

        bool heli_los = false;
        if (mHeli && mHeli->HasLOS()) {
            heli_los = mHeli->GetInFormation() != 0;
        }

        if (ai_searching || ((mLOSCount == 0) && !heli_los)) {
            mPursuitState = kSearching;
        } else if ((mLOSCount > 0) || heli_los || GetRoadblock()) {
            if (mPursuitState != kActive) {
                mTimeSinceLastChase = (WorldTimer - mT_sinceLastPursuit).GetSeconds();
            }
            mPursuitState = kActive;
        }

        if (mPursuitState <= kSearching) {
            mTrafficHits911 = 0;
            mCTS911 = 0;
        }
    }

    float t_lost = GetPerpLostTime();
    if (mPursuitState == kActive) {
        mT_noLOS = WorldTimer;
        mT_sinceLastPursuit = WorldTimer;
    } else {
        mT_LOS = WorldTimer;
    }

    float inactivity_cutoff[2];
    inactivity_cutoff[0] = mTune.PursuitInactivityTimer(0) + mTune.TimeConsideredLostNoLOS();
    inactivity_cutoff[1] = mTune.PursuitInactivityTimer(1) + mTune.TimeConsideredLostNoLOS();

    if ((mFocus == kLost) && (mPursuitState == kActive) && (mTimeSinceLastChase < inactivity_cutoff[1])) {
        mFocus = kPursuitFlow;
        mQuadrantState = kReset;
        mFlags &= ~SETUP_RESTARTED;
        mFlags &= ~PURSUIT_EXPIRED;
        Speech::Manager::ClearPlayback();
        mPursuitFlow->Reacquire();
        mMusicFlow->Reacquire();
    }

    if ((mFocus == kStrategyFlow) && ((mPursuitState == kSearching) || (mPursuitState == kInactive)) &&
        (t_lost > inactivity_cutoff[0])) {
        if (!mPursuit || ((mPursuit->GetPursuitStatus() == PS_COOL_DOWN) && (mPursuit->GetCoolDownTimeRemaining() > 0.0f))) {
            TerminatePursuit(kOutrunBail);
        }
    }

    if (mFocus == kLost) {
        if (((mPursuitState == kSearching) || (mPursuitState == kInactive)) && (t_lost > inactivity_cutoff[0]) &&
            (t_lost < inactivity_cutoff[1]) &&
            !Speech::Manager::IsCopSpeechBusy() &&
            (!mPursuit || ((mPursuit->GetPursuitStatus() == PS_COOL_DOWN) && (mPursuit->GetCoolDownTimeRemaining() > 0.0f)))) {
            switch (mQuadrantState) {
            case kInitial:
                MiscSpeech::QuadrantForming();
                mQuadrantState = kForming;
                break;
            case kForming:
                if (bRandom(1.0f) > 0.5f) {
                    mQuadrantState = kFiction2;
                    MiscSpeech::PossibleSuspect();
                } else {
                    mQuadrantState = kFiction1;
                    int rand = bRandom(3);
                    if (rand != 1) {
                        if (rand == 0) {
                            MiscSpeech::SuspectPossiblyGone();
                        } else {
                            MiscSpeech::OtherLead();
                        }
                    } else {
                        MiscSpeech::QuadrantMoving();
                    }
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
        } else if (((mPursuitState == kSearching) || (mPursuitState == kInactive)) && (t_lost > inactivity_cutoff[0]) &&
                   (!mPursuit || ((mPursuit->GetPursuitStatus() == PS_COOL_DOWN) && (mPursuit->GetCoolDownTimeRemaining() == 0.0f)))) {
            if ((mQuadrantState == kExpired) &&
                (!mPursuit || (mPursuit->GetPursuitStatus() == PS_EVADED)) && ((mFlags & PURSUIT_EXPIRED) == 0)) {
                Speech::Manager::ClearPlayback();
                mDispatch->TimeExpired();
                mQuadrantState = kReset;
                mFlags |= PURSUIT_EXPIRED;
            } else if (t_lost > inactivity_cutoff[1]) {
                ResetPursuit(false);
                while (!mActors.empty()) {
                    RemoveCop(mActors.begin()->hsimable);
                }
            }
        }
    }

    if (mFocus == kTerminal) {
        mPursuitState = kInactive;
    }

    if ((static_cast<unsigned int>(mPursuitState) < static_cast<unsigned int>(kInactive)) && mPursuit) {
        mPursuitDuration = (WorldTimer - mT_pursuitStart).GetSeconds();
    } else {
        mPursuitDuration = -1.0f;
    }

    if (((mFlags & HELI_INTRO_REQ) != 0) && mHeli && (mFocus == kStrategyFlow) && (mPursuitState != kInactive)) {
        mHeli->BackupArrives();
        mFlags &= ~HELI_INTRO_REQ;
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

    if (!Speech::Manager::IsQueued(static_cast<SPCHType_1_EventID>(0x63), 4) && (mPursuitDuration > 60.0f)) {
        mDispatch->PursuitUpdate(mLeader);
    }

    if ((WorldTimer - mT_noLOS).GetSeconds() >= mTune.NoLOSCommentaryTime()) {
        if (!mHeli) {
            mLeader->LostVisual();
        } else if (!mHeli->HasLOS()) {
            mHeli->LostVisual();
        } else if (IsHeadingValid() && (mPlayerOffroadID >= 0) && (bRandom(1.0f) <= 0.5f)) {
            mHeli->LocationReport();
        } else {
            mHeli->PursuitUpdateReply();
        }
    } else {
        if (!Speech::Manager::IsQueued(static_cast<SPCHType_1_EventID>(0x64), 4) &&
            !Speech::Manager::IsQueued(static_cast<SPCHType_1_EventID>(0x9e), 4) && (mPursuitDuration > 60.0f)) {
            if (IsHeadingValid() && (mPlayerOffroadID >= 0) && (bRandom(1.0f) <= 0.5f)) {
                mLeader->LocationReport();
            } else {
                mLeader->PursuitUpdateReply();
            }
        }
    }

    if (Speech::Manager::IsQueued(static_cast<SPCHType_1_EventID>(0x9e), 4) &&
        Speech::Manager::HasBeenSaid(static_cast<SPCHType_1_EventID>(0x63))) {
        mLeader->PursuitUpdateReply();
    }
}

EAXCop *SoundAI::GetRandomCop(int type) {
    EAXCop *spkr = 0;

    if ((type == 0) && (mActors.size() > 1)) {
        spkr = mActors[bRandom(static_cast<int>(mActors.size()))].cop;
    } else {
        unsigned int actor_count = mActors.size();
        if (actor_count == 1) {
            switch (type) {
            case 1:
                if (!mActors[0].cop->IsPrimary()) {
                    return spkr;
                }
                spkr = mActors[0].cop;
                break;
            case 2:
                if (mActors[0].cop->IsPrimary()) {
                    return spkr;
                }
                spkr = mActors[0].cop;
                break;
            case 0:
                spkr = mActors[0].cop;
                break;
            default:
                return spkr;
            }
        } else if (type == 1) {
            UTL::Std::vector<EAXCop *, _type_vector> primaries;
            primaries.reserve(actor_count);

            Speech::copMap::iterator i = mActors.begin();
            while (i != mActors.end()) {
                if (i->cop->IsPrimary()) {
                    primaries.push_back(i->cop);
                }
                ++i;
            }

            if (!primaries.empty()) {
                spkr = primaries[bRandom(static_cast<int>(primaries.size()))];
            }
            primaries.clear();
        } else if (type == 2) {
            UTL::Std::vector<EAXCop *, _type_vector> secondaries;
            secondaries.reserve(actor_count);

            Speech::copMap::iterator i = mActors.begin();
            while (i != mActors.end()) {
                if (!i->cop->IsPrimary()) {
                    secondaries.push_back(i->cop);
                }
                ++i;
            }

            if (!secondaries.empty()) {
                spkr = secondaries[bRandom(static_cast<int>(secondaries.size()))];
            }
            secondaries.clear();
        }
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
        active.clear();
        return 0;
    }

    EAXCop *spkr = 0;
    if ((type == 0) && (active.size() > 1)) {
        spkr = active[bRandom(static_cast<int>(active.size()))];
    } else if (active.size() == 1) {
        switch (type) {
        case 1:
            if (active[0]->IsPrimary()) {
                spkr = active[0];
            }
            break;
        case 2:
            if (!active[0]->IsPrimary()) {
                spkr = active[0];
            }
            break;
        case 0:
            spkr = active[0];
            break;
        default:
            break;
        }
    } else if (type == 1) {
        UTL::Std::vector<EAXCop *, _type_vector> primaries;
        primaries.reserve(active.size());

        Speech::copList::iterator i = active.begin();
        while (i != active.end()) {
            if ((*i)->IsPrimary()) {
                primaries.push_back(*i);
            }
            ++i;
        }

        if (!primaries.empty()) {
            spkr = primaries[bRandom(static_cast<int>(primaries.size()))];
        }
        primaries.clear();
    } else if (type == 2) {
        UTL::Std::vector<EAXCop *, _type_vector> secondaries;
        secondaries.reserve(active.size());

        Speech::copList::iterator i = active.begin();
        while (i != active.end()) {
            if (!(*i)->IsPrimary()) {
                secondaries.push_back(*i);
            }
            ++i;
        }

        if (!secondaries.empty()) {
            spkr = secondaries[bRandom(static_cast<int>(secondaries.size()))];
        }
        secondaries.clear();
    }

    active.clear();
    return spkr;
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
    case Csis::Type_speaker_battalion_City:
        cs_pool = &mUsage.cs_City;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign20);
        }
        break;
    case Csis::Type_speaker_battalion_Alpine:
        cs_pool = &mUsage.cs_Alpine;
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
    case Csis::Type_speaker_battalion_Rhino_Units:
        cs_pool = &mUsage.cs_Rhino;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign06);
        }
        break;
    default:
        break;
    }

    if (cs_pool != nullptr) {
        Speech::voiceIDs::iterator iter = cs_pool->begin();
        int id = *iter;
        cs_pool->erase(iter);
        return id;
    } else {
        return -1;
    }
}

void SoundAI::Force911State() {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    IPerpetrator *perp = 0;
    ICopMgr *copmgr = UTL::Collections::Singleton<ICopMgr>::Get();

    if (!player) {
        return;
    }

    player->GetSimable()->QueryInterface(&perp);
    if (copmgr && ICopMgr::AreCopsEnabled() && perp && mPursuitLevel.GetCollection()) {
        mFlags |= DISP911_ACTIVE;
        perp->Set911CallTime(mPursuitLevel.Lifetime911(0));
        copmgr->LockoutCops(false);
    }
}

EAXCop *SoundAI::FindFurthestCop(bool includeHeli) {
    EAXCop *furthest = 0;

    if (!mActors.size()) {
        return 0;
    }

    Speech::copPair *iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop) {
            if (!furthest || (furthest->GetDistance() < cop->GetDistance())) {
                if ((cop->IsHeli() && includeHeli) || !cop->IsHeli()) {
                    furthest = cop;
                }
            }
        }
        ++iter;
    }
    return furthest;
}

EAXCop *SoundAI::FindClosestCop(bool enforceLOS, bool includeHeli) {
    EAXCop *closest = 0;

    if (!mActors.size()) {
        return 0;
    }

    Speech::copPair *iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop && (!enforceLOS || cop->HasLOS()) && (includeHeli || !cop->IsHeli())) {
            if (!closest || (cop->GetDistance() < closest->GetDistance())) {
                closest = cop;
            }
        }
        ++iter;
    }
    return closest;
}

bool SoundAI::MakeLeader(EAXCop *newprim) {
    EAXCop *wannab;
    Speech::copPair *primary;

    if (mLeader && newprim && (mLeader->GetSpeakerID() == newprim->GetSpeakerID())) {
        return true;
    }

    if (newprim->IsActive()) {
        if ((newprim->GetSpeakerID() == 5) || (newprim->GetSpeakerID() == 4) || (newprim->GetSpeakerID() == 3) ||
            (newprim->GetSpeakerID() == 2) || (newprim->GetSpeakerID() == 9)) {
            if (mLeader != newprim) {
                mLeader = newprim;
                if (mFocus == kStrategyFlow) {
                    newprim->PrimaryEngage();
                }
            }
            return true;
        }
    }

    wannab = mActors.Find(newprim->GetHandle());
    if (!wannab) {
        return false;
    }

    primary = mActors.begin();
    while (primary != mActors.end()) {
        EAXCop *cop = primary->cop;
        if ((cop->GetSpeakerID() == 5) || (cop->GetSpeakerID() == 4) || (cop->GetSpeakerID() == 3) ||
            (cop->GetSpeakerID() == 9)) {
            break;
        }
        ++primary;
    }

    if (primary == mActors.end()) {
        int rand = bRandom(3);
        switch (rand) {
        case 0:
            newprim->SetSpeakerID(3);
            break;
        case 1:
            newprim->SetSpeakerID(4);
            break;
        case 2:
            newprim->SetSpeakerID(5);
            break;
        default:
            newprim->SetSpeakerID(3);
            break;
        }

        int *i = mUsage.voices.begin();
        while (i != mUsage.voices.end()) {
            if (*i == newprim->GetSpeakerID()) {
                mUsage.voices.erase(i);
                break;
            }
            ++i;
        }

        if (mLeader != newprim) {
            mLeader = newprim;
            if (mFocus == kStrategyFlow) {
                newprim->PrimaryEngage();
            }
        }
        gSpeechCache.AddSpeaker(mLeader->GetSpeakerID());
        return true;
    } else {
        int spkrA = wannab->GetSpeakerID();
        int spkrB = primary->cop->GetSpeakerID();
        wannab->SetSpeakerID(spkrB);
        primary->cop->SetSpeakerID(spkrA);
    }

    if (mLeader != newprim) {
        mLeader = newprim;
        if (mFocus == kStrategyFlow) {
            newprim->PrimaryEngage();
        }
    }
    return true;
}

unsigned char SoundAI::GetCustomized(IVehicle *vehicle, CarCustomizations &custrec) {
    const FECustomizationRecord *record;
    bool has_vinyls;
    bool has_custom_paint;
    bool has_racing_numbers;
    bool has_decals;

    has_vinyls = false;
    has_racing_numbers = false;
    has_decals = false;
    has_custom_paint = has_decals;

    record = vehicle->GetCustomizations();
    custrec.flags = 0;
    if (record) {
        CarPart *paint_part =
            GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x4c);
        if (paint_part) {
            unsigned int colour_hash = GetAppliedAttributeUParam__7CarPartUiUi(paint_part, 0xd68a7bab, 0);
            for (int i = 0; i < NumberOfColourHashToSoundColourMaps; i++) {
                if (ColourHashToSoundColourMap[i].Hash == colour_hash) {
                    custrec.color = ColourHashToSoundColourMap[i].SoundColour;
                    break;
                }
            }

            CarPart *vinyls =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x4d);
            if (vinyls) {
                has_vinyls = true;
            }

            CarPart *left_number1 =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x69);
            CarPart *left_number2 =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x6a);
            CarPart *right_number1 =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x71);
            CarPart *right_number2 =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x72);
            if (left_number1 || left_number2 || right_number1 || right_number2) {
                has_racing_numbers = true;
            }

            CarPart *left_door_decal =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x48);
            CarPart *right_door_decal =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x49);
            CarPart *left_quarter =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x4a);
            CarPart *right_quarter =
                GetInstalledPart__C21FECustomizationRecord7CarTypei(record, vehicle->GetModelType(), 0x4b);
            if (left_door_decal || right_door_decal || left_quarter || right_quarter) {
                has_decals = true;
            }
        }
    } else {
        CarTypeInfo *type_info = &CarTypeInfoArray[vehicle->GetModelType()];
        CarPart *paint_part = NewGetCarPart__15CarPartDatabase7CarTypeiUiP7CarParti(
            &CarPartDB,
            vehicle->GetModelType(),
            0x4c,
            static_cast<unsigned int>(type_info->DefaultBasePaint),
            0,
            -1);
        if (paint_part) {
            unsigned int colour_hash = GetAppliedAttributeUParam__7CarPartUiUi(paint_part, 0xd68a7bab, 0);
            for (int i = 0; i < NumberOfColourHashToSoundColourMaps; i++) {
                if (ColourHashToSoundColourMap[i].Hash == colour_hash) {
                    custrec.color = ColourHashToSoundColourMap[i].SoundColour;
                    break;
                }
            }
        }
    }

    if (has_vinyls) {
        custrec.flags |= VINYLS;
    }
    if (has_custom_paint) {
        custrec.flags |= PAINT;
    }
    if (has_racing_numbers) {
        custrec.flags |= RACING_NUMS;
    }
    if (has_decals) {
        custrec.flags |= DECALS;
    }
    return 1;
}

unsigned int SoundAI::CalcPlayerDirection(bool force_set) {
    if (!dir_init_30961) {
        t_currdir_30960 = Timer(0);
        dir_init_30961 = 1;
    }

    float zmag = UMath::Abs(mSmoothedFWRoad.z);
    float xmag = UMath::Abs(mSmoothedFWRoad.x);
    unsigned int dir;
    if (xmag <= zmag) {
        dir = 1;
        if (0.0f < mSmoothedFWRoad.z) {
            dir = 2;
        }
    } else {
        dir = 8;
        if (0.0f < mSmoothedFWRoad.x) {
            dir = 4;
        }
    }

    if ((dir != 0) && (dir != dir_tracking_30959)) {
        t_currdir_30960 = WorldTimer;
        dir_tracking_30959 = dir;
    }

    float t_samedir = (WorldTimer - t_currdir_30960).GetSeconds();
    if ((t_samedir <= 3.0f) && !force_set) {
        dir = 0;
    }
    return dir;
}

void SoundAI::ForceGlobalVoiceChange() {
    Speech::copPair *iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop) {
            cop->SetSpeakerID(3);
        }
        ++iter;
    }

    mActors.clear();
    mCopsInFormation.clear();
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

    float closest = 32767.0f;
    mRacerCount = static_cast<char>(UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AIRACERS).size());

    {
        const UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List &vehicles =
            UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AIRACERS);
        UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List::const_iterator i = vehicles.begin();
        while (i != vehicles.end()) {
            IVehicle *vehicle = *i;
            IRenderable *renderable = 0;
            if (vehicle->QueryInterface(&renderable)) {
                float dist2cam = renderable->DistanceToView();
                if (dist2cam < closest) {
                    closest = dist2cam;
                }
            }
            ++i;
        }
    }

    if (closest <= mTune.AIRacerProximity()) {
        mFlags |= RACERS_PROXIMAL;
    } else {
        if ((mFlags & RACERS_PROXIMAL) != 0) {
            EAXCop *cop = GetRandomActiveCop(0, false);
            if (cop) {
                cop->FocusChange();
            }
        }
        mFlags &= ~RACERS_PROXIMAL;
    }

    unsigned char cops_in_view = 0;
    unsigned char cops_with_los = 0;
    unsigned char cops_ahead = 0;
    unsigned char num_active = 0;
    float pursuit_distance = 32767.0f;

    const UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List &vehicles =
        UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AICOPS);
    UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List::const_iterator i = vehicles.begin();
    while (i != vehicles.end()) {
        IVehicle *vehicle = *i;
        IRenderable *renderable = 0;
        vehicle->QueryInterface(&renderable);
        if (vehicle) {
            bool has_visual = false;
            bool is_ahead = false;
            bool in_view = false;
            bool is_in_rb = false;

            ISimable *simable = vehicle->GetSimable();
            HSIMABLE thisObj = simable->GetOwnerHandle();

            IRoadBlock *irb = GetRoadblock();
            if (irb) {
                IVehicle *car_in_rb = irb->IsComprisedOf(thisObj);
                is_in_rb = (car_in_rb == vehicle);
            }

            if (vehicle->GetDriverClass() == DRIVER_COP) {
                IPursuitAI *ai = 0;
                vehicle->QueryInterface(&ai);

                if (vehicle->GetVehicleClass() == VehicleClass::CAR) {
                    if (renderable && renderable->InView()) {
                        cops_in_view++;
                        in_view = true;
                    }

                    if (vehicle->IsActive() && ai) {
                        float t_tgt_last_seen = ai->GetTimeSinceTargetSeen();
                        if ((t_tgt_last_seen < 0.05f) || (is_in_rb && in_view)) {
                            has_visual = true;
                            cops_with_los++;
                        }
                    }

                    UMath::Vector3 player_pos = mPlayerPos;
                    UMath::Vector3 copcar_pos = vehicle->GetPosition();
                    UMath::Vector3 player_fw = mPlayerFW;
                    if (vehicle->IsActive() || (renderable && renderable->InView())) {
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
                    if (actor) {
                        if (vehicle->IsActive()) {
                            if (!actor->IsActive()) {
                                actor->SetActive(true);
                            }
                        } else if (DESTROY_COPS_ON_INACTIVITY) {
                            RemoveCop(actor->GetHandle());
                        } else {
                            actor->SetActive(false);
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
                            float speed_diff = mPlayerSpeed - actor->GetSpeed();
                            mRecentBlowby.distance = actor->GetDistance();
                            mRecentBlowby.speed = speed_diff;
                            mRecentBlowby.timestamp = WorldTimer;
                        }
                    } else {
                        new_cop_cars.push_back(vehicle);
                    }
                }
            } else if (vehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                if (mHeli) {
                    if (mHeli->GetHandle() != vehicle->GetSimable()->GetOwnerHandle()) {
                        mHeli->SetHandle(vehicle->GetSimable()->GetOwnerHandle());
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
                } else {
                    AddNewHeli(vehicle);
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

    if (new_cop_cars.size() != 0) {
        IVehicles::iterator add_it = new_cop_cars.begin();
        while (add_it != new_cop_cars.end()) {
            AddNewCop(*add_it);
            ++add_it;
        }
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
        if (cop->GetInFormation() && !cop->IsHeli()) {
            bool found = false;
            Speech::copList::iterator i = mCopsInFormation.begin();
            while (i != mCopsInFormation.end()) {
                EAXCop *copInFormation = *i;
                if (copInFormation && copInFormation->GetHandle() == cop->GetHandle()) {
                    found = true;
                }
                ++i;
            }
            if (!found && !cop->IsHeli()) {
                mCopsInFormation.push_back(cop);
            }
        } else if (!mCopsInFormation.empty()) {
            Speech::copList::iterator i = mCopsInFormation.begin();
            while (i != mCopsInFormation.end()) {
                EAXCop *copInFormation = *i;
                if (copInFormation && copInFormation->GetHandle() == cop->GetHandle()) {
                    mCopsInFormation.erase(i);
                    break;
                }
                ++i;
            }
        }
        ++iter;
    }

    if (mCopsInFormation.size() == 1) {
        mLastCopInFormation = mCopsInFormation.front();
    }
    if (mCopsInFormation.size() != 0) {
        mT_outofFormation = WorldTimer;
    }
}

void SoundAI::ResetPursuit(bool including_music) {
    mPursuitFlow->Reset();
    mStrategyFlow->Reset();
    mRoadblockFlow->Reset();
    mObserver->Reset();
    if (including_music) {
        mMusicFlow->Reset();
    }
    Speech::Manager::ResetGlobalHistory();

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
    if ((type != kOutrunBail) && (type == kForcedBail)) {
        mMusicFlow->ChangeStateTo(kTerminal);

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
    } else {
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
            MiscSpeech::LostSuspect(static_cast<int>(Speech::Manager::mLastSpeakerID));
        }
        mDispatch->BreakAway();
        mFocus = kLost;
        mQuadrantState = kInitial;
        mFlags &= ~SETUP_RESTARTED;
    }
}

void SoundAI::RemoveCop(HSIMABLE seeya) {
    if (mActors.size() == 0) {
        return;
    }

    EAXCop *cop = mActors.Remove(seeya);
    if (!cop) {
        return;
    }

    gSpeechCache.RemoveSpeaker(cop->GetSpeakerID());

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

    mPursuitFlow->OnCopRemoved(cop);
    mUsage.voices.push_back(cop->GetSpeakerID());
    delete cop;
}

void SoundAI::AddNewCop(IVehicle *newcop) {
    IVehicleAI *vai = 0;
    newcop->QueryInterface(&vai);
    HSIMABLE newbie = newcop->GetSimable()->GetOwnerHandle();
    vai->GetAttributes();
    WRoadNav *nav = vai->GetDriveToNav();
    int roadID = nav->GetRoadSpeechId();
    bool is_rb_cop = false;

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
        UMath::Vector3 pPos = mPlayerPos;
        UMath::Vector3 delta;
        VU0_v3sub(pPos, cop_pos, delta);
        float distance = VU0_sqrt(VU0_v3lengthsquare(delta));

        Speech::copMap::iterator i = mActors.begin();
        while (i != mActors.end()) {
            EAXCop *cop = i->cop;
            if (!cop->IsHeli() && !is_rb_cop && (distance < cop->GetDistance()) && !cop->IsHeli()) {
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
                cop->Update();
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
                gSpeechCache.AddSpeaker(voice);
                latest_cop->Update();
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
            mLatestCop->BackupArrives();
        } else {
            mLatestCop->UnitBackupReply();
        }
    }
}

void SoundAI::ShuffleActors() {
    if (mActors.size() == 0) {
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
        Speech::copList::iterator j = active.begin();
        while (j != active.end()) {
            if ((*i)->GetSpeakerID() == (*j)->GetSpeakerID()) {
                RemoveCop((*i)->GetHandle());
                break;
            }
            ++j;
        }
        ++i;
    }

    if (active.size() > 1) {
        bool item_removed = false;
        Speech::copList::iterator i = active.begin();
        while (i != active.end()) {
            if (active.size() <= 1) {
                break;
            }
            Speech::copList::iterator j = i + 1;
            while (j != active.end()) {
                EAXCop *a = *i;
                EAXCop *b = *j;
                if ((a->GetSpeakerID() == b->GetSpeakerID()) && (a->GetHandle() != b->GetHandle())) {
                    if (b->GetDistance() <= a->GetDistance()) {
                        RemoveCop(a->GetHandle());
                        active.erase(i);
                    } else {
                        RemoveCop(b->GetHandle());
                        active.erase(j);
                    }
                    item_removed = true;
                    break;
                }
                ++j;
            }
            if (!item_removed) {
                ++i;
            }
            item_removed = false;
        }
    }

    if (active.size() != 0) {
        if (!mLeader || !mLeader->IsActive() || mLeader->IsHeli()) {
            Speech::copList::iterator i = active.begin();
            while (i != active.end()) {
                EAXCop *activecop = *i;
                if (activecop->IsPrimary() && activecop->HasLOS()) {
                    mLeader = activecop;
                    if ((mFocus == kStrategyFlow) && !mLeader->IsHeli()) {
                        mLeader->PrimaryEngage();
                    }
                    break;
                }
                ++i;
            }
        }
        if (mLeader && !mLeader->IsActive() && !MakeLeader(mLeader)) {
            return;
        }
    }
}
