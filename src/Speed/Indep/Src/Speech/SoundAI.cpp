#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle_hash.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/smackable.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/MusicFlow.h"
#include "Speed/Indep/Src/Speech/SpeechCache.h"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/Src/Speech/SoundAI.h"

extern int IsSoundEnabled; // Decl: 62

extern ParameterAccessor SPAMAccessorSpeech; // Decl: 64

int SoundAI::mRefCount = 0;     // size: 0x4, Decl: 66
BIND_ACTIVITY_FACTORY(SoundAI); // Decl: 67
IMPLEMENT_SINGLETON(SoundAI);   // Decl: 68

const bool SPEECHFLOW_DISPLAY = false; // Decl: 71
int SPEECHFLOW_DISPLAY_DITCHED;        // Decl: 72

static int DESTROY_COPS_ON_INACTIVITY = 0; // Decl: 74
int FORCE_VOICE_RANDOMIZATION = 0;         // Decl: 75

// size: 0x20, Decl: 77
const SoundAI::HeatCutoffs SoundAI::heat_cutoffs[4] = {
    {2.0f, Csis::Type_heat_level_2},
    {3.0f, Csis::Type_heat_level_3},
    {4.0f, Csis::Type_heat_level_4},
    {5.0f, Csis::Type_heat_level_5},
};

SoundAI::SoundAI()
    : Sim::Activity(1),                                                                   //
      mMainUpdate(this->AddTask("Speech", 0.1f, 0.0f, Sim::TASK_FRAME_FIXED)),            //
      mProcessObservations(this->AddTask("Comment", 0.25f, 0.0f, Sim::TASK_FRAME_FIXED)), //
      mFlags(0),                                                                          //
      mActors(10),                                                                        //
      mDispatch(nullptr),                                                                 //
      mLeader(nullptr),                                                                   //
      mHeli(nullptr),                                                                     //
      mDeadAir(0.0f),                                                                     //
      mLastCopInFormation(nullptr),                                                       //
      mLatestCop(nullptr),                                                                //
      mPlayerHeat(0),                                                                     //
      mPlayerSpeed(0.0f),                                                                 //
      mPlayerPos(UMath::Vector3::kZero),                                                  //
      mSmoothedFWRoad(UMath::Vector3::kZero),                                             //
      mPursuit(nullptr),                                                                  //
      mAIPursuit(nullptr),                                                                //
      mFocus(kPursuitFlow),                                                               //
      mPursuitDist(0.0f),                                                                 //
      mT_PerpLastSeen(0.0f),                                                              //
      mTrafficHits911(0),                                                                 //
      mCTS911(0),                                                                         //
      mHavoc(0),                                                                          //
      mPursuitCount(0),                                                                   //
      mNumRoadBlocks(0),                                                                  //
      mRacerCount(0),                                                                     //
      mTimeSinceLastChase(0.0f),                                                          //
      mPVehicle(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),                  //
      mTune(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),                      //
      mPursuitLevel(static_cast<Attrib::Collection *>(nullptr), 0, nullptr),              //
      mPursuitState(kInactive),                                                           //
      mQuadrantState(kReset),                                                             //
      mInfraction(-1),                                                                    //
      mNumCopsInWave(0),                                                                  //
      mNumActiveCopCars(0),                                                               //
      mPlayerOffroadID(-1),                                                               //
      mCopsInView(0),                                                                     //
      mPursuitFlow(nullptr),                                                              //
      mStrategyFlow(nullptr),                                                             //
      mObserver(nullptr),                                                                 //
      mRoadblockFlow(nullptr),                                                            //
      mMusicFlow(nullptr),                                                                //
      mT_outofFormation(0),                                                               //
      mT_reallylowspeed(0),                                                               //
      mT_noLOS(0),                                                                        //
      mT_LOS(0),                                                                          //
      mT_lastCrashed(0),                                                                  //
      mT_lastCopNailed(0),                                                                //
      mT_pursuitStart(WorldTimer),                                                        //
      mT_sinceLastPursuit(0),                                                             //
      mPlayerCarCustom(nullptr),                                                          //
      mAICarCustom(nullptr),                                                              //
      mActorPool(nullptr),                                                                //
      mMsgPerpBusted(Hermes::Handler::Create<MPerpBusted, SoundAI, SoundAI>(this, &SoundAI::MessagePerpBusted, UCrc32(UCRC32_Gameplay), 0)),
      mMsgAIPerpBusted(Hermes::Handler::Create<MPerpBusted, SoundAI, SoundAI>(this, &SoundAI::MessageAIPerpBusted, "AIRacerBusted", 0)),
      mMsgRestartRace(Hermes::Handler::Create<MRestartRace, SoundAI, SoundAI>(this, &SoundAI::MessageRestart, UCrc32(UCRC32_Gameplay), 0)),
      mMsgInfraction(Hermes::Handler::Create<MMiscSound, SoundAI, SoundAI>(this, &SoundAI::MessageInfraction, "Infraction", 0)),
      mMsgUnspawnCop(Hermes::Handler::Create<MUnspawnCop, SoundAI, SoundAI>(this, &SoundAI::MessageUnspawnCop, "SoundAI", 0)),
      mMsgTireBlown(Hermes::Handler::Create<MGamePlayMoment, SoundAI, SoundAI>(this, &SoundAI::MessageTireBlown, "TireBlo", 0)) {

    SoundAI::mRefCount = 1;
    this->mCopsInFormation.clear();
    this->mRecentBlowby.Reset();

    Sim::ProfileTask(this->mMainUpdate, "Speech");
    Sim::ProfileTask(this->mProcessObservations, "Speech");

    this->mTune.ChangeWithDefault(0);
    this->mPursuitLevel.ChangeWithDefault(0);

    this->mActorPool = bNewSlotPool(0xA0, 10, "VoiceActors slotpool", AudioMemoryPool);
    this->mActorPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    this->mActorPool->ClearFlag(SLOTPOOL_FLAG_WARN_IF_NONEMPTY_DELETE);

    this->mDispatch = new EAXDispatch(1);
    this->mPursuitFlow = new ("SpeechFlow: Pursuit", 0) Speech::PursuitFlow();
    this->mStrategyFlow = new ("SpeechFlow: Strategy", 0) Speech::StrategyFlow();
    this->mObserver = new ("SpeechFlow: Observer", 0) Speech::Observer();
    this->mRoadblockFlow = new ("SpeechFlow: Roadblock", 0) Speech::RoadblockFlow();
    this->mMusicFlow = new ("MusicFlow: Interactive", 0) Speech::MusicFlow();

    for (int i = 0; i < 2; i++) {
        this->mPlayerCurrent[i].direction = this->CalcPlayerDirection(false);
        this->mAICurrent[i].direction = 0;
        this->mPlayerCurrent[i].roadID = this->mAICurrent[i].roadID = untagged;
    }

    this->mAILastKnown.direction = 0;
    this->mLastKnown.direction = 0;
    this->mAILastKnown.roadID = untagged;
    this->mLastKnown.roadID = untagged;

    for (int i = 3; i < 10; i++) {
        mUsage.voices.push_back(i);
    }
    for (unsigned int i = 0; i < mUsage.voices.size(); i++) {
        int rand = bRandom(static_cast<int>(mUsage.voices.size()));
        int rand_voice = mUsage.voices[rand];
        int curr_voice = mUsage.voices[i];
        if (rand_voice != curr_voice) {
            mUsage.voices[i] = rand_voice;
            mUsage.voices[rand] = curr_voice;
        }
    }

    this->Enable();
}

SoundAI::~SoundAI() {
    SoundAI::mRefCount = 0;
    this->RemoveTask(this->mMainUpdate);
    this->RemoveTask(this->mProcessObservations);

    for (Speech::copMap::iterator iter = this->mActors.begin(); iter != this->mActors.end(); ++iter) {
        if (iter->cop != nullptr) {
            delete iter->cop;
        }
    }
    this->mActors.clear();
    this->mCopsInFormation.clear();

    if (this->mActorPool != nullptr) {
        bDeleteSlotPool(this->mActorPool);
    }

    Sim::Collision::RemoveListener(this);
    this->mLeader = nullptr;
    this->mHeli = nullptr;

    if (this->mPursuitFlow != nullptr) {
        delete this->mPursuitFlow;
    }
    this->mPursuitFlow = nullptr;
    if (this->mStrategyFlow != nullptr) {
        delete this->mStrategyFlow;
    }
    this->mStrategyFlow = nullptr;
    if (this->mObserver != nullptr) {
        delete this->mObserver;
    }
    this->mObserver = nullptr;
    if (this->mRoadblockFlow != nullptr) {
        delete this->mRoadblockFlow;
    }
    this->mRoadblockFlow = nullptr;
    if (this->mMusicFlow != nullptr) {
        delete this->mMusicFlow;
    }
    this->mMusicFlow = nullptr;

    if (this->mPlayerCarCustom != nullptr) {
        delete this->mPlayerCarCustom;
    }
    this->mPlayerCarCustom = nullptr;
    if (this->mAICarCustom != nullptr) {
        delete this->mAICarCustom;
    }
    this->mAICarCustom = nullptr;

    if (this->mMsgPerpBusted != nullptr) {
        Hermes::Handler::Destroy(this->mMsgPerpBusted);
    }
    if (this->mMsgAIPerpBusted != nullptr) {
        Hermes::Handler::Destroy(this->mMsgAIPerpBusted);
    }
    if (this->mMsgForcePursuitStart != nullptr) {
        Hermes::Handler::Destroy(this->mMsgForcePursuitStart);
    }
    if (this->mMsgRestartRace != nullptr) {
        Hermes::Handler::Destroy(this->mMsgRestartRace);
    }
    if (this->mMsgUnspawnCop != nullptr) {
        Hermes::Handler::Destroy(this->mMsgUnspawnCop);
    }
    if (this->mMsgInfraction != nullptr) {
        Hermes::Handler::Destroy(this->mMsgInfraction);
    }
    if (this->mMsgTireBlown != nullptr) {
        Hermes::Handler::Destroy(this->mMsgTireBlown);
    }
}

void SoundAI::MessagePerpBusted(const MPerpBusted &message) {
    this->mFocus = kTerminal;
    if (this->mMusicFlow != nullptr) {
        MControlPathfinder(false, 15, 0, 0).Send(UCrc32("Event"));
    }
    this->mFlags |= BUSTED;
}

void SoundAI::MessageAIPerpBusted(const MPerpBusted &message) {
    if (this->mPursuitState != kInactive) {
        EAXCop *cop = this->FindFurthestCop(true);
        if (cop != nullptr) {
            cop->Arrest();
        }
    }
}

void SoundAI::MessageInfraction(const MMiscSound &message) {
    this->mInfraction = message.GetSoundID();
}

void SoundAI::MessageRestart(const MRestartRace &message) {
    this->ResetPursuit(true);
    Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
    if (cop_speech != nullptr) {
        cop_speech->ReleaseResource();
    }
    Speech::Manager::ClearPlayback();
}

void SoundAI::MessageUnspawnCop(const MUnspawnCop &message) {
    EAXCop *j = this->mActors.Find(message.GetCopHandle());
    if (j == nullptr) {
        return;
    }

    switch (message.GetParam()) {
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            j->SetActive(false);
            return;
        case 0:
        case 2:
        default:
            this->RemoveCop(message.GetCopHandle());
            return;
    }
}

void SoundAI::MessageTireBlown(const MGamePlayMoment &message) {
    if (this->mRoadblockFlow != nullptr) {
        this->mRoadblockFlow->NailedSomethingInRB(Speech::RoadblockFlow::NAILED_SPIKES);
    }
    EAXCop *spkr = this->FindClosestCop(true, true);
    if (this->mObserver != nullptr && spkr != nullptr) {
        this->mObserver->Observe(Speech::Collision_Suspect_Spikebelt, spkr->GetSpeakerID(), 1.0f);
    }
}

void SoundAI::OnVehicleAdded(IVehicle *ivehicle) {
    Sim::Collision::AddListener(this, ivehicle, "SoundAI");
}

void SoundAI::OnVehicleRemoved(IVehicle *ivehicle) {
    Sim::Collision::RemoveListener(this, ivehicle);
    EAXCop *cop = this->mObserver->GetRamCop();
    if (cop != nullptr) {
        EAXCop *actor = this->mActors.Find(ivehicle->GetSimable()->GetOwnerHandle());
        if (actor == cop) {
            this->mObserver->DetachRamCop();
        }
    }
}

static const float CopMinClosingVelSq = MPH2MPS(25.0f); // Decl: 440

// TODO improve on the control flow
void SoundAI::OnCollision(const COLLISION_INFO &cinfo) {
    if (UMath::LengthSquare(cinfo.closingVel) < CopMinClosingVelSq) {
        return;
    }

    short actors_involved = 0;
    short objects_visible = 0;
    IRenderable *renderA;
    IRenderable *renderB;
    EAXCop *actorA = nullptr;
    EAXCop *actorB = nullptr;
    ISimable *simableA;
    if (cinfo.objA != nullptr) {
        simableA = ISimable::FindInstance(cinfo.objA);
    } else {
        simableA = nullptr;
    }
    ISimable *simableB;
    if (cinfo.objB != nullptr) {
        simableB = ISimable::FindInstance(cinfo.objB);
    } else {
        simableB = nullptr;
    }

    if (simableA != nullptr) {
        actorA = this->mActors.Find(cinfo.objA);
        if (actorA != nullptr) {
            actors_involved++;
        }
        renderA = nullptr;
        UTL::COM::ValidatePtr(simableA);
        if (simableA->QueryInterface(&renderA) && renderA->InView()) {
            objects_visible++;
        }
    }
    if (simableB != nullptr) {
        actorB = this->mActors.Find(cinfo.objB);
        if (actorB != nullptr) {
            actors_involved++;
        }
        renderB = nullptr;
        UTL::COM::ValidatePtr(simableB);
        if (simableB->QueryInterface(&renderB) && renderB->InView()) {
            objects_visible++;
        }
    }
    if (simableA == nullptr && simableB == nullptr) {
        return;
    }

    float collisionspeed = cinfo.impulseA + cinfo.impulseB;
    float minspeed = MPH2MPS(this->mTune.PlayerSmashSpeedRange(0));
    float maxspeed = MPH2MPS(this->mTune.PlayerSmashSpeedRange(1));
    float intensity = UMath::Clamp((collisionspeed - minspeed) / (maxspeed - minspeed), 0.0f, 1.0f);

    if (cinfo.type != Sim::Collision::Info::OBJECT) {
        if (cinfo.type < Sim::Collision::Info::WORLD) {
            return;
        }
        switch (cinfo.type) {
            default:
                return;
            case Sim::Collision::Info::WORLD:
                break;
        }
    } else {
        if (actors_involved == 2) {
            return;
        }

        if (actors_involved == 1) {
            IVehicle *theOtherCar;
            ISimable *theOtherObj = simableA != nullptr ? simableA : simableB;
            EAXCop *actor = actorA != nullptr ? actorA : actorB;

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
                    ISimable *simableCop;
                    if (theOtherObj == simableA) {
                        simableCop = simableB;
                    } else {
                        simableCop = simableA;
                    }
                    if (simableCop == nullptr) {
                        return;
                    }

                    inputcop = nullptr;
                    if (simableCop->QueryInterface(&inputcop)) {
                        float brake = UMath::Clamp(inputcop->GetControls().fBrake, 0.0f, 1.0f);
                        float ebrake = UMath::Clamp(inputcop->GetControls().fHandBrake, 0.0f, 1.0f);
                        if ((brake > 0.0f) || (ebrake > 0.0f)) {
                            cop_is_braking = true;
                        }
                    }

                    if ((this->mFocus == kPursuitFlow) && ((this->mFlags & SETUP_RESTARTED) == 0)) {
                        if ((this->mPursuitFlow->GetPursuitCause() != Speech::PursuitFlow::kScripted) &&
                            (this->mPursuitFlow->GetPursuitCause() != Speech::PursuitFlow::kCopAssaulted) &&
                            (this->mPursuitFlow->GetPursuitCause() != Speech::PursuitFlow::kCopAssaultedScripted) &&
                            this->mPursuitFlow->RequiresRestart()) {
                            Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(COPSPEECH_MODULE);
                            if (cop_speech != nullptr) {
                                Speech::Manager::ClearPlayback();
                                cop_speech->ReleaseResource();
                                this->mT_lastCopNailed = WorldTimer;
                                actor->WasRammed();
                                if (this->mPursuitFlow->GetPursuitCause() == Speech::PursuitFlow::kScripted) {
                                    this->mPursuitFlow->SetPursuitCause(Speech::PursuitFlow::kCopAssaultedScripted);
                                } else {
                                    this->mPursuitFlow->SetPursuitCause(Speech::PursuitFlow::kCopAssaulted);
                                }
                                this->mPursuitFlow->ChangeStateTo(Speech::PursuitFlow::kPrimaryBranch);
                                this->mPursuitFlow->Update();
                                this->mFlags |= SETUP_RESTARTED;
                            }
                        }
                    }

                    IRoadBlock *block = this->GetRoadblock();
                    if (block != nullptr) {
                        IVehicle *car = block->IsComprisedOf(simableCop->GetOwnerHandle());
                        if (car != nullptr) {
                            cop_is_in_rb = true;
                            if (UTL::COM::ComparePtr(car, simableCop)) {
                                this->mRoadblockFlow->NailedSomethingInRB(0x40);
                            }
                        }
                    }
                    if (!cop_is_in_rb) {
                        this->mObserver->Observe(Speech::Collision_Cop_Suspect, -1, intensity);
                    }

                    vehicleCop = nullptr;
                    simableCop->QueryInterface(&vehicleCop);
                    if (vehicleCop != nullptr) {
                        unsigned int vtype = vehicleCop->GetVehicleAttributes().GetCollection();
                        if ((vtype == 0x38b38226) || (vtype == 0x54b10e38) || (vtype == 0x2e149eac)) {
                            cop_is_suv = true;
                        }
                    }

                    rbRacer = theOtherObj->GetRigidBody();
                    rbCop = simableCop->GetRigidBody();

                    UMath::Vector3 armCop = actorA == nullptr ? cinfo.armB : cinfo.armA;
                    UMath::Vector3 armRacer = actorA == nullptr ? cinfo.armA : cinfo.armB;
                    UMath::Vector3 fwCop;
                    UMath::Vector3 fwRacer;
                    UMath::Vector3 dimCop;
                    UMath::Vector3 dimRacer;
                    UMath::Vector3 velCop;
                    UMath::Vector3 velRacer;
                    UMath::Vector3 cnormal = cinfo.normal;
                    rbCop->GetForwardVector(fwCop);
                    rbCop->GetDimension(dimCop);
                    velCop = (simableCop->GetOwnerHandle() == cinfo.objA) ? cinfo.objAVel : cinfo.objBVel;
                    rbRacer->GetForwardVector(fwRacer);
                    rbRacer->GetDimension(dimRacer);
                    velRacer = (theOtherObj->GetOwnerHandle() == cinfo.objA) ? cinfo.objAVel : cinfo.objBVel;

                    float fwDot = VU0_v3dotprod(fwRacer, fwCop);
                    float vel_norm_dotCop = bAbs(VU0_v3dotprod(velCop, cnormal));
                    float vel_norm_dotRacer = bAbs(VU0_v3dotprod(velRacer, cnormal));
                    VehicleImpactType coll_type = kUnknown;

                    if (fwDot >= 0.7f) {
                        if ((bAbs(armCop.x) > dimCop.x * 0.75f) && (bAbs(armRacer.x) > dimRacer.x * 0.75f)) {
                            coll_type = kCopSSPerp;
                            if (vel_norm_dotCop <= vel_norm_dotRacer) {
                                coll_type = kPerpSSCop;
                                cop_rammed = true;
                            }
                        } else {
                            if ((armCop.z >= dimCop.z * 0.75f) && (armRacer.z <= armRacer.z * -0.75f)) {
                                coll_type = kCopREperp;
                            }
                            if ((armRacer.z >= dimRacer.z * 0.75f) && (armCop.z <= dimCop.z * -0.75f)) {
                                if (VU0_v3length(velRacer) <= VU0_v3length(velCop)) {
                                } else {
                                    cop_rammed = true;
                                }
                                coll_type = kPerpRECop;
                            }
                        }
                    } else if ((fwDot > -0.7f) && (fwDot < 0.7f)) {
                        if (armRacer.z >= dimRacer.z * 0.7f) {
                            cop_rammed = true;
                            coll_type = kPerpTBCop;
                        }
                        if (armCop.z >= dimCop.z * 0.7f) {
                            coll_type = kCopTBPerp;
                        }
                    } else if (fwDot <= -0.7f) {
                        if (vel_norm_dotRacer > vel_norm_dotCop) {
                            cop_rammed = true;
                            coll_type = kPerpHOCop;
                        } else {
                            coll_type = kCopHOPerp;
                        }
                    }

                    if (!cop_rammed) {
                        break;
                    }
                    this->mT_lastCopNailed = WorldTimer;
                    if (actor == nullptr) {
                        break;
                    }
                    if (cop_is_in_rb || cop_is_suv || cop_is_braking || ((this->mFlags & COPS_IMMUNE) != 0)) {
                        break;
                    }
                    actor->WasRammed();
                    if ((this->mFocus == kPursuitFlow) || (this->mFocus == kTerminal) || actor->IsDead() || !actor->IsActive()) {
                        break;
                    }

                    if (intensity > 0.15f) {
                        if (intensity <= 0.5f) {
                            if (coll_type == kPerpRECop) {
                                actor->RearEnded(Csis::Type_intensity_Normal);
                            } else if (coll_type == kPerpTBCop) {
                                actor->TBoned(Csis::Type_intensity_Normal);
                            } else if (coll_type == kPerpHOCop) {
                                actor->HeadOn(Csis::Type_intensity_Normal);
                            } else if (coll_type == kPerpSSCop) {
                                actor->SideSwiped(Csis::Type_intensity_Normal);
                            }
                            break;
                        }
                    }
                    if ((intensity > 0.5f) && (intensity <= 0.75f)) {
                        float rand_select = bRandom(1.0f);
                        if (rand_select < 0.67f) {
                            if (coll_type == kPerpRECop) {
                                actor->RearEnded(Csis::Type_intensity_High);
                            } else if (coll_type == kPerpTBCop) {
                                actor->TBoned(Csis::Type_intensity_High);
                            } else if ((coll_type == kPerpHOCop) && !cop_is_suv) {
                                actor->HeadOn(Csis::Type_intensity_High);
                            } else if (coll_type == kPerpSSCop) {
                                actor->SideSwiped(Csis::Type_intensity_High);
                            } else {
                                actor->InterruptExpletive();
                            }
                        } else {
                            actor->InterruptExpletive();
                        }
                    } else if (intensity > 0.75f) {
                        actor->InterruptViolent();
                    }
                    break;
                }

                case DRIVER_TRAFFIC:
                    this->mObserver->Observe(Speech::Collision_Cop_Traffic, -1, intensity);
                    actor->JustHitTraffic();
                    if ((this->mPursuitState == kActive) && (this->mFocus == kStrategyFlow) && actor->IsActive()) {
                        actor->BailoutTraffic();
                        this->RandomBailoutDeny(actor);
                    }
                    break;

                default:
                    break;
            }
        } else {
            IPlayer *player = IPlayer::First(PLAYER_LOCAL);
            IVehicle *pvehicle = nullptr;
            if (player != nullptr) {
                if (UTL::COM::ComparePtr(player->GetSimable(), simableA)) {
                    simableA->QueryInterface(&pvehicle);
                } else if (UTL::COM::ComparePtr(player->GetSimable(), simableB)) {
                    simableB->QueryInterface(&pvehicle);
                }
            }
            if (pvehicle != nullptr) {
                ISimable *otherObj = simableA;
                if (pvehicle->GetSimable()->GetOwnerHandle() == cinfo.objA) {
                    otherObj = simableB;
                }
                IModel *model;
                if (otherObj != nullptr) {
                    model = otherObj->GetModel();
                } else {
                    model = nullptr;
                }
                IVehicle *otherVehicle = nullptr;
                if (otherObj != nullptr) {
                    otherObj->QueryInterface(&otherVehicle);
                }

                float collisionspeed = cinfo.impulseA + cinfo.impulseB;
                float minspeed = MPH2MPS(this->mTune.PlayerSmashSpeedRange(0));
                float maxspeed = MPH2MPS(this->mTune.PlayerSmashSpeedRange(1));
                float intensity = UMath::Clamp((collisionspeed - minspeed) / (maxspeed - minspeed), 0.0f, 1.0f);

                const UMath::Vector3 &player_vel = (pvehicle->GetSimable()->GetOwnerHandle() == cinfo.objA) ? cinfo.objAVel : cinfo.objBVel;
                float speed_b4_impact = UMath::Length(player_vel);
                float curr_speed = pvehicle->GetSimable()->GetRigidBody()->GetSpeed();
                float pct_decrease;
                if (speed_b4_impact > 0.0f) {
                    pct_decrease = curr_speed / speed_b4_impact;
                } else {
                    pct_decrease = 1.0f;
                }
                if (pct_decrease < (1.0f - this->mTune.CrashSlowdownPct())) {
                    this->mT_lastCrashed = WorldTimer;
                }

                if (model != nullptr) {
                    unsigned int model_atr = model->GetAttributes().GetCollection();
                    if (model->IsRootModel() && (this->mPursuitState == kInactive)) {
                        Attrib::Gen::smackable obj_atr(model->GetAttributes());
                        int cost_to_state = obj_atr.COST_TO_STATE();
                        this->mCTS911 += cost_to_state;
                    }
                }

                if (this->GetRoadblock() != nullptr) {
                    const IRoadBlock::Smackables &objects = this->GetRoadblock()->GetSmackables();
                    if (objects.size() > 0) {
                        for (IRoadBlock::Smackables::const_iterator i = objects.begin(); i != objects.end(); ++i) {
                            IPlaceableScenery *object = *i;
                            if (UTL::COM::ComparePtr(object, model) && model != nullptr) {
                                if (model->GetAttributes().GetCollection() == 0xca89ef8f) {
                                    this->mRoadblockFlow->NailedSomethingInRB(0x10);
                                    EAXCop *spkr = this->FindClosestCop(false, true);
                                    this->mObserver->Observe(Speech::Collision_Suspect_Spikebelt, spkr != nullptr ? spkr->GetSpeakerID() : -1,
                                                             intensity);
                                } else {
                                    this->mRoadblockFlow->NailedSomethingInRB(0x20);
                                }
                            }
                        }
                    }
                }

                if (otherVehicle != nullptr) {
                    switch (otherVehicle->GetDriverClass()) {
                        case DRIVER_HUMAN:
                            this->mObserver->Observe(Speech::Collision_Suspect_Suspect, -1, intensity);
                            break;
                        case DRIVER_TRAFFIC:
                        case DRIVER_NONE: {
                            this->mObserver->Observe(Speech::Collision_Suspect_Traffic, -1, intensity);
                            {
                                EAXCop *spkr = this->FindClosestCop(true, true);
                                int spkrID = -1;
                                if (spkr != nullptr) {
                                    spkrID = spkr->GetSpeakerID();
                                }
                                if ((otherVehicle->GetVehicleClass() == VehicleClass::TRACTOR) ||
                                    (otherVehicle->GetVehicleClass() == VehicleClass::TRAILER)) {
                                    if (intensity >= this->mTune.MinIntensityTrafficSmash()) {
                                        this->mObserver->Observe(Speech::Collision_Suspect_Semi, spkrID, intensity);
                                    }
                                } else {
                                    this->mObserver->Observe(Speech::Collision_Suspect_Traffic, spkrID, intensity);
                                }
                            }
                            if (intensity >= this->mTune.MinIntensityTrafficSmash()) {
                                this->mTrafficHits911++;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }
        return;
    }
    {
        ISimable *isimable = simableA;
        IVehicle *ivehicle;
        float collisionspeed = cinfo.impulseA + cinfo.impulseB;
        float minspeed = MPH2MPS(this->mTune.PlayerSmashSpeedRange(0));
        float maxspeed = MPH2MPS(this->mTune.PlayerSmashSpeedRange(1));
        float intensity = UMath::Clamp((collisionspeed - minspeed) / (maxspeed - minspeed), 0.0f, 1.0f);
        if (isimable->QueryInterface(&ivehicle)) {
            switch (ivehicle->GetDriverClass()) {
                case DRIVER_HUMAN: {
                    {
                        const UMath::Vector3 &player_vel = cinfo.objAVel;
                        float speed_b4_impact = UMath::Length(player_vel);
                        float curr_speed = isimable->GetRigidBody()->GetSpeed();
                        float pct_decrease;
                        if (speed_b4_impact > 0.0f) {
                            pct_decrease = curr_speed / speed_b4_impact;
                        } else {
                            pct_decrease = 1.0f;
                        }
                        if (pct_decrease >= (1.0f - this->mTune.CrashSlowdownPct())) {
                            return;
                        }
                        this->mT_lastCrashed = WorldTimer;
                    }
                    if ((this->mPursuitState == kActive) && (this->mFocus == kStrategyFlow)) {
                        EAXCop *spkr = this->GetRandomActiveCop(0, true);
                        if (spkr != nullptr) {
                            this->mObserver->Observe(Speech::Collision_Suspect_World, spkr->GetSpeakerID(), intensity);
                        }
                    }
                    return;
                }
                case DRIVER_COP:
                    break;
                default:
                    return;
            }

            {
                EAXCop *actor = actorA != nullptr ? actorA : actorB;
                if (actor == nullptr || (objects_visible <= 0) || !actor->IsActive()) {
                    return;
                }
                if (intensity >= this->mTune.MinIntensityCopSmash()) {
                    if (this->mObserver != nullptr && this->mObserver->WeatherExists()) {
                        actor->BailoutBadRoad();
                    } else {
                        actor->Bailout();
                    }
                }
                if (actor->GetInFormation() && (this->mCopsInFormation.size() > 1)) {
                    this->RandomBailoutDeny(actor);
                }
                this->mObserver->Observe(Speech::Collision_Cop_World, actor != nullptr ? actor->GetSpeakerID() : -1, intensity);
            }
        }
    }
}

EAXCop *SoundAI::GetCopInRB() {
    IRoadBlock *block = this->GetRoadblock();

    if (block != nullptr) {
        for (Speech::copMap::const_iterator iter = this->mActors.begin(); iter != this->mActors.end(); ++iter) {
            EAXCop *cop = iter->cop;
            if (cop->IsActive() && !cop->IsPrimary()) {
                IVehicle *car = block->IsComprisedOf(cop->GetHandle());
                if (car != nullptr) {
                    return cop;
                }
            }
        }
    }
    return nullptr;
}

EAXCop *SoundAI::GetRandomActiveCop(int type, bool reqLOS) {
    Speech::copList active;
    active.reserve(this->mActors.size());

    for (Speech::copMap::const_iterator iter = this->mActors.begin(); iter != this->mActors.end(); ++iter) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive() && (!reqLOS || cop->HasLOS())) {
            active.push_back(cop);
        }
    }

    if (active.empty()) {
        return nullptr;
    }

    EAXCop *spkr = nullptr;
    if ((type == 0) && (active.size() > 1)) {
        spkr = active[bRandom(static_cast<int>(active.size()))];
    } else if (active.size() == 1) {
        switch (type) {
            case 0:
                spkr = active[0];
                break;
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
            default:
                break;
        }
    } else {
        switch (type) {
            case 1: {
                UTL::Std::vector<EAXCop *, _type_vector> primaries;
                primaries.reserve(active.size());

                for (Speech::copList::iterator i = active.begin(); i != active.end(); ++i) {
                    if ((*i)->IsPrimary()) {
                        primaries.push_back(*i);
                    }
                }

                if (!primaries.empty()) {
                    int rand = bRandom(static_cast<int>(primaries.size()));
                    spkr = primaries[rand];
                }
                primaries.clear();
                break;
            }
            case 2: {
                UTL::Std::vector<EAXCop *, _type_vector> secondaries;
                secondaries.reserve(active.size());

                for (Speech::copList::iterator i = active.begin(); i != active.end(); ++i) {
                    if (!(*i)->IsPrimary()) {
                        secondaries.push_back(*i);
                    }
                }

                if (!secondaries.empty()) {
                    int rand = bRandom(static_cast<int>(secondaries.size()));
                    spkr = secondaries[rand];
                }
                secondaries.clear();
                break;
            }
            default:
                break;
        }
    }

    active.clear();
    return spkr;
}

EAXCop *SoundAI::GetRandomCop(int type) {
    EAXCop *spkr = nullptr;

    if ((type == 0) && (this->mActors.size() > 1)) {
        spkr = this->mActors[bRandom(static_cast<int>(this->mActors.size()))].cop;
    } else if (this->mActors.size() == 1) {
        switch (type) {
            case 1:
                if (this->mActors[0].cop->IsPrimary()) {
                    spkr = this->mActors[0].cop;
                }
                break;
            case 2:
                if (!this->mActors[0].cop->IsPrimary()) {
                    spkr = this->mActors[0].cop;
                }
                break;
            case 0:
                spkr = this->mActors[0].cop;
                break;
            default:
                break;
        }
    } else {
        switch (type) {
            case 1: {
                UTL::Std::vector<EAXCop *, _type_vector> primaries;
                primaries.reserve(this->mActors.size());

                for (Speech::copMap::iterator i = this->mActors.begin(); i != this->mActors.end(); ++i) {
                    if (i->cop->IsPrimary()) {
                        primaries.push_back(i->cop);
                    }
                }

                if (!primaries.empty()) {
                    int rand = bRandom(static_cast<int>(primaries.size()));
                    spkr = primaries[rand];
                }
                primaries.clear();
                break;
            }
            case 2: {
                UTL::Std::vector<EAXCop *, _type_vector> secondaries;
                secondaries.reserve(this->mActors.size());

                for (Speech::copMap::iterator i = this->mActors.begin(); i != this->mActors.end(); ++i) {
                    if (!i->cop->IsPrimary()) {
                        secondaries.push_back(i->cop);
                    }
                }

                if (!secondaries.empty()) {
                    int rand = bRandom(static_cast<int>(secondaries.size()));
                    spkr = secondaries[rand];
                }
                secondaries.clear();
                break;
            }
        }
    }

    return spkr;
}

void SoundAI::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle = nullptr;

    if (pOther->QueryInterface(&ivehicle)) {
        this->OnVehicleAdded(ivehicle);
    }

    this->Sim::Activity::OnAttached(pOther);
}

void SoundAI::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle = nullptr;

    if (pOther->QueryInterface(&ivehicle)) {
        this->OnVehicleRemoved(ivehicle);
    }

    if (UTL::COM::ComparePtr(this->mPursuit, pOther)) {
        this->mPursuit = nullptr;
        this->mT_pursuitStart = WorldTimer;
        this->mPursuitState = this->mAIPursuit != nullptr ? kOtherTarget : kInactive;
    }

    if (UTL::COM::ComparePtr(this->mAIPursuit, pOther)) {
        this->mAIPursuit = nullptr;
        if ((this->mPursuitState == kOtherTarget) && this->mPursuit == nullptr) {
            this->mPursuitState = kInactive;
        }
    }

    this->Sim::Activity::OnDetached(pOther);
}

Sim::IActivity *SoundAI::Construct(Sim::Param params) {
    if ((IsSoundEnabled == 0) || Sim::IsSplitScreen()) {
        return nullptr;
    }

    SoundAI *result = SoundAI::Get();
    if (result != nullptr) {
        SoundAI::mRefCount = SoundAI::mRefCount + 1;
        return static_cast<Sim::IActivity *>(result);
    }

    return static_cast<Sim::IActivity *>(new SoundAI);
}

IRoadBlock *SoundAI::GetRoadblock() {
    if (this->mPursuit != nullptr) {
        return this->mPursuit->GetRoadBlock();
    } else {
        const IRoadBlock::List &blocks = IRoadBlock::GetList();
        for (IRoadBlock::List::const_iterator i = blocks.begin(); i != blocks.end(); ++i) {
            IRoadBlock *rb = *i;
            if (rb != nullptr) {
                IPursuit *pursuit = rb->GetPursuit();
                if (pursuit != nullptr && pursuit->IsPlayerPursuit()) {
                    return rb;
                }
            }
        }
        return nullptr;
    }
}

bool SoundAI::IsMusicActive() {
    if (this->mMusicFlow == nullptr) {
        return false;
    }
    return this->mMusicFlow->GetState() != -1;
}

bool SoundAI::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("SoundAI::OnTask", 0);
    float tout = (WorldTimer - WorldTimer).GetSeconds();

    this->mDeadAir = Speech::Manager::IsCopSpeechBusy() ? 0.0f : (WorldTimer - Speech::Manager::GetTimeSinceLastEvent(COPSPEECH_MODULE)).GetSeconds();

    if (htask == this->mMainUpdate) {
        if (FORCE_VOICE_RANDOMIZATION != 0) {
            this->ForceGlobalVoiceChange();
            FORCE_VOICE_RANDOMIZATION = 0;
        }
        if ((this->mFlags & BUSTED) == 0) {
            this->SyncPursuit();
            this->SyncCarsToActors();
            this->SyncPlayers();
            this->SyncFormations();
            this->ShuffleActors();
            this->UpdateStateMachines();
            if ((IsSpeechEnabled != 0) && (this->mDeadAir > 0.0f)) {
                this->DealWithDeadAir();
            }
        }
    }

    if (htask == this->mProcessObservations) {
        if ((this->mFlags & BUSTED) == 0) {
            this->mObserver->Update();
            this->mRoadblockFlow->Update();
        }
        Speech::Manager::Deduce();
    }
    return true;
}

void SoundAI::DealWithDeadAir() {
    if (this->mPursuitState != kActive) {
        return;
    }
    if (this->mLeader == nullptr) {
        return;
    }
    if (this->mFocus == kPursuitFlow) {
        return;
    }

    if (!Speech::Manager::IsQueued(kSPCH1_EventID_DispPursuitUpdate, 4) && (this->mPursuitDuration > 60.0f)) {
        this->mDispatch->PursuitUpdate(this->mLeader);
    }

    if (this->GetPerpLostTime() >= this->mTune.NoLOSCommentaryTime()) {
        if (this->mHeli == nullptr) {
            this->mLeader->LostVisual();
        } else {
            if (this->mHeli->HasLOS()) {
                if (!this->IsHeadingValid() || (this->mPlayerOffroadID < 0) || (bRandom(1.0f) > 0.5f)) {
                    this->mHeli->PursuitUpdateReply();
                } else {
                    this->mHeli->LocationReport();
                }
            } else {
                this->mHeli->LostVisual();
            }
        }
    } else {
        if (!Speech::Manager::IsQueued(kSPCH1_EventID_PursuitUpdateRep, 4) && !Speech::Manager::IsQueued(kSPCH1_EventID_LocationReport, 4) &&
            (this->mPursuitDuration > 60.0f)) {
            if (!this->IsHeadingValid() || (this->mPlayerOffroadID < 0) || (bRandom(1.0f) > 0.5f)) {
                this->mLeader->PursuitUpdateReply();
            } else {
                this->mLeader->LocationReport();
            }
        }
    }

    if (Speech::Manager::IsQueued(kSPCH1_EventID_LocationReport, 4) && Speech::Manager::HasBeenSaid(kSPCH1_EventID_DispPursuitUpdate)) {
        this->mLeader->PursuitUpdateReply();
    }
}

void SoundAI::UpdateStateMachines() {
    if ((IsSpeechEnabled != 0) && ((this->mFlags & BUSTED) == 0)) {
        switch (this->mFocus) {
            case kPursuitFlow:
                this->mPursuitFlow->Update();
                if (this->mPursuitFlow->IsTransitionable()) {
                    this->mFocus = kStrategyFlow;
                    this->mStrategyFlow->ChangeStateTo(0);
                }
                break;

            case kRoadblockFlow:
            case kStrategyFlow:
                if (this->mStrategyFlow->IsTransitionable()) {
                    this->mStrategyFlow->ChangeStateTo(kWaiting);
                }
                this->mStrategyFlow->Update();
                break;

            case kLost:
                if ((this->mPursuit != nullptr) && (this->mPursuitState == kSearching)) {
                    if (this->mStrategyFlow->GetState() != kOtherTarget) {
                        this->mStrategyFlow->ChangeStateTo(kOtherTarget);
                    }
                    this->mStrategyFlow->Update();
                }
                break;

            case kTerminal + 1:
                break;

            default:
                break;
        }
    }

    this->mMusicFlow->Update();
}

void SoundAI::AttemptReattachPursuit() {
    short playerfound = this->mPursuit != nullptr && this->mPursuit->IsPlayerPursuit();
    short aifound = this->mAIPursuit != nullptr && !this->mAIPursuit->IsPlayerPursuit();

    const IPursuit::List &pursuits = IPursuit::GetList();
    if (pursuits.size() > 0) {
        IPursuit::List::const_iterator i = pursuits.begin();
        if (!playerfound) {
            for (IPursuit::List::const_iterator i = pursuits.begin(); i != pursuits.end(); ++i) {
                IPursuit *pursuit = *i;
                if (pursuit->IsPlayerPursuit() && !playerfound) {
                    playerfound = 1;
                    if (this->mPursuit != pursuit) {
                        this->mPursuitCount++;
                        if (this->mPursuit == nullptr) {
                            this->mT_pursuitStart = WorldTimer;
                        } else {
                            if (this->IsAttached(this->mPursuit)) {
                                this->Detach(this->mPursuit);
                            }
                            this->mPursuit = nullptr;
                        }
                        if (!this->IsAttached(pursuit)) {
                            this->Attach(pursuit);
                        }
                        this->mPursuit = pursuit;
                    }
                } else if (!aifound) {
                    aifound = 1;
                    if (this->mAIPursuit != pursuit) {
                        if (this->mAIPursuit != nullptr) {
                            if (this->IsAttached(this->mAIPursuit)) {
                                this->Detach(this->mAIPursuit);
                            }
                            this->mAIPursuit = nullptr;
                        }
                        if (!this->IsAttached(pursuit)) {
                            if (this->Attach(pursuit)) {
                                this->mAIPursuit = pursuit;
                            }
                        }
                        this->mAIPursuit = pursuit;
                    }
                }
            }
        }
    }

    if (!playerfound) {
        this->mPursuit = nullptr;
        this->mT_pursuitStart = WorldTimer;
        this->mPursuitState = kInactive;
    }

    if (!aifound) {
        this->mAIPursuit = nullptr;
        if ((this->mPursuitState == kOtherTarget) && this->mPursuit == nullptr) {
            this->mPursuitState = kInactive;
        }
    }
}

void SoundAI::SyncPursuit() {
    if (this->mPursuit == nullptr || !this->mPursuit->IsPlayerPursuit() || (this->mAIPursuit == nullptr || this->mAIPursuit->IsPlayerPursuit())) {
        this->AttemptReattachPursuit();
    }

    if (this->mPursuit == nullptr) {
        if ((this->mPursuitState != kOtherTarget) && (this->mPursuitState != kInactive)) {
            if (this->mAIPursuit != nullptr && (this->mRacerCount > 0)) {
                this->mPursuitState = kOtherTarget;
            } else {
                this->mT_sinceLastPursuit = WorldTimer;
                this->mPursuitState = kInactive;
            }
        }
    } else {
        bool ai_searching = false;
        if (this->mPursuit->AttemptingToReAquire() || !this->mPursuit->IsPerpInSight() || (this->mPursuit->GetPursuitStatus() == PS_COOL_DOWN)) {
            ai_searching = true;
        }

        bool heli_LOS;
        if (this->mHeli != nullptr && this->mHeli->IsActive()) {
            heli_LOS = this->mHeli->HasLOS();
        } else {
            heli_LOS = false;
        }

        if (ai_searching || ((this->NumCopsWithLOS() == 0) && !heli_LOS)) {
            this->mPursuitState = kSearching;
        } else if (((!ai_searching) && ((this->NumCopsWithLOS() > 0) || (heli_LOS == true))) || this->GetRoadblock() != nullptr) {
            if (this->mPursuitState != kActive) {
                this->mTimeSinceLastChase = (WorldTimer - this->mT_sinceLastPursuit).GetSeconds();
            }
            this->mPursuitState = kActive;
        }

        if (static_cast<unsigned int>(this->mPursuitState) <= static_cast<unsigned int>(kSearching)) {
            this->mTrafficHits911 = 0;
            this->mCTS911 = 0;
        }
    }

    float t_lost = this->GetPerpLostTime();
    if (this->mPursuitState == kActive) {
        this->mT_noLOS = this->mT_sinceLastPursuit = WorldTimer;
    } else {
        this->mT_LOS = WorldTimer;
    }

    float inactivity_cutoff[2];
    inactivity_cutoff[0] = this->mTune.PursuitInactivityTimer(0) + this->mTune.TimeConsideredLostNoLOS();
    inactivity_cutoff[1] = this->mTune.PursuitInactivityTimer(1) + this->mTune.TimeConsideredLostNoLOS();

    if ((this->mFocus == kLost) && (this->mPursuitState == kActive) && (this->mTimeSinceLastChase < inactivity_cutoff[1])) {
        this->mFocus = kPursuitFlow;
        this->mQuadrantState = kReset;
        this->mFlags &= ~SETUP_RESTARTED;
        this->mFlags &= ~PURSUIT_EXPIRED;
        Speech::Manager::ClearPlayback();
        this->mPursuitFlow->Reacquire();
        this->mMusicFlow->Reacquire();
    }

    if ((this->mFocus == kStrategyFlow) && ((this->mPursuitState == kSearching) || (this->mPursuitState == kInactive)) &&
        (t_lost > inactivity_cutoff[0]) &&
        (this->mPursuit == nullptr ||
         ((this->mPursuit->GetPursuitStatus() == PS_COOL_DOWN) && (this->mPursuit->GetCoolDownTimeRemaining() > 0.0f)))) {
        this->TerminatePursuit(kOutrunBail);
    }

    else if (this->mFocus == kLost) {
        if (((this->mPursuitState == kSearching) || (this->mPursuitState == kInactive)) && (t_lost > inactivity_cutoff[0]) &&
            (t_lost < inactivity_cutoff[1]) && !Speech::Manager::IsCopSpeechBusy() &&
            (this->mPursuit == nullptr ||
             ((this->mPursuit->GetPursuitStatus() == PS_COOL_DOWN) && (this->mPursuit->GetCoolDownTimeRemaining() > 0.0f)))) {
            switch (this->mQuadrantState) {
                case kInitial:
                    MiscSpeech::QuadrantForming();
                    this->mQuadrantState = kForming;
                    break;
                case kForming:
                    if (bRandom(1.0f) > 0.5f) {
                        this->mQuadrantState = kFiction2;
                        MiscSpeech::PossibleSuspect();
                    } else {
                        this->mQuadrantState = kFiction1;
                        int rand = bRandom(3);
                        if (rand != 1) {
                            if (rand <= 1) {
                                if (rand == 0) {
                                    MiscSpeech::SuspectPossiblyGone();
                                    break;
                                }
                            }
                        } else {
                            MiscSpeech::QuadrantMoving();
                            break;
                        }
                        MiscSpeech::OtherLead();
                    }
                    break;
                case kFiction2:
                    MiscSpeech::WrongSuspect();
                    this->mQuadrantState = kExpired;
                    break;
                case kFiction1:
                default:
                    this->mQuadrantState = kExpired;
                    break;
            }
        } else if ((this->mFocus == kLost) && ((this->mPursuitState == kSearching) || (this->mPursuitState == kInactive)) &&
                   (this->mPursuit == nullptr ||
                    ((this->mPursuit->GetPursuitStatus() == PS_COOL_DOWN) && (this->mPursuit->GetCoolDownTimeRemaining() == 0.0f)))) {
            if ((this->mQuadrantState == kExpired) && (this->mPursuit == nullptr || (this->mPursuit->GetPursuitStatus() == PS_EVADED)) &&
                ((this->mFlags & PURSUIT_EXPIRED) == 0)) {
                Speech::Manager::ClearPlayback();
                this->mDispatch->TimeExpired();
                this->mQuadrantState = kReset;
                this->mFlags |= PURSUIT_EXPIRED;
            } else if (t_lost > inactivity_cutoff[1]) {
                this->ResetPursuit(false);
            }
        }
    }

    if (this->mFocus == kTerminal) {
        this->mPursuitState = kInactive;
    }

    this->mPursuitDuration = ((static_cast<unsigned int>(this->mPursuitState) < static_cast<unsigned int>(kInactive)) && this->mPursuit != nullptr)
                                 ? (WorldTimer - this->mT_pursuitStart).GetSeconds()
                                 : -1.0f;

    if (((this->mFlags & HELI_INTRO_REQ) != 0) && this->mHeli != nullptr && (this->mFocus == kStrategyFlow) && (this->mPursuitState != kInactive)) {
        this->mHeli->BackupArrives();
        this->mFlags &= ~HELI_INTRO_REQ;
    }
}

void SoundAI::TerminatePursuit(BailoutType type) {
    if (type != kOutrunBail) {
        if (type == kForcedBail) {
            this->mMusicFlow->ChangeStateTo(kTerminal);

            bool is_DDay = GRaceStatus::Get().GetRaceParameters() != nullptr ? GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace() : false;

            if (!is_DDay) {
                Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
                if (cop_speech != nullptr) {
                    cop_speech->ReleaseResource();
                }
                Speech::Manager::ClearPlayback();
            }

            EAXCop *bailer = this->GetRandomActiveCop(0, false);
            if (bailer != nullptr) {
                MiscSpeech::Bailout(bailer->GetSpeakerID());
            } else {
                MiscSpeech::Bailout(0);
            }
            this->mFocus = kTerminal;
            return;
        }
    }

    {
        bool cops_visible = false;
        for (IVehicle::List::const_iterator i = IVehicle::GetList(VEHICLE_AICOPS).begin(); i != IVehicle::GetList(VEHICLE_AICOPS).end(); ++i) {
            IVehicle *vehicle = *i;
            IRenderable *renderable;
            if (vehicle->QueryInterface(&renderable) && renderable->InView()) {
                cops_visible = true;
            }
        }
        Speech::Manager::ClearPlayback();
        if (!cops_visible) {
            MiscSpeech::LostSuspect(static_cast<int>(Speech::Manager::GetLastSpeakerID()));
        }
        this->mDispatch->BreakAway();
        this->mFocus = kLost;
        this->mQuadrantState = kInitial;
        this->mFlags &= ~SETUP_RESTARTED;
    }
}

void SoundAI::ResetPursuit(bool including_music) {
    this->mPursuitFlow->Reset();
    this->mStrategyFlow->Reset();
    this->mRoadblockFlow->Reset();
    this->mObserver->Reset();
    if (including_music) {
        this->mMusicFlow->Reset();
    }
    Speech::Manager::ResetGlobalHistory();

    this->mFocus = kPursuitFlow;
    this->mQuadrantState = kReset;
    this->mRacerCount = 0;

    if (this->mActors.size() > 0) {
        Speech::copMap::iterator i = this->mActors.begin();
        while (this->mActors.size() > 0) {
            RemoveCop(i->hsimable);
        }
    }

    this->mLeader = nullptr;
    this->mCopsInFormation.clear();
    this->mPursuit = nullptr;
    this->mAIPursuit = nullptr;
    this->mLastCopInFormation = nullptr;
    this->mLatestCop = nullptr;
    this->mT_pursuitStart = WorldTimer;
    this->mPursuitState = kInactive;
    this->mPursuitDist = 0.0f;
    this->mT_lastCopNailed = Timer(0);
    this->mT_lastCrashed = Timer(0);
    this->mT_noLOS = Timer(0);
    this->mT_reallylowspeed = Timer(0);
    this->mT_outofFormation = Timer(0);
    this->mT_LOS = Timer(0);
    this->mCopsInView = 0;
    this->mInfraction = 0;
    this->mFlags = 0;
    this->mTrafficHits911 = 0;
    this->mCTS911 = 0;
    this->mHavoc = 0;
}

void SoundAI::ShuffleActors() {
    if (this->mActors.size() == 0) {
        return;
    }

    Speech::copList active;
    Speech::copList inactive;
    active.reserve(this->mActors.size());
    inactive.reserve(this->mActors.size());

    for (Speech::copMap::const_iterator iter = this->mActors.begin(); iter != this->mActors.end(); iter++) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive()) {
            active.push_back(cop);
        } else {
            inactive.push_back(cop);
        }
    }

    for (Speech::copList::iterator i = inactive.begin(); i != inactive.end(); ++i) {
        for (Speech::copList::iterator j = active.begin(); j != active.end(); ++j) {
            if ((*i)->GetSpeakerID() == (*j)->GetSpeakerID()) {
                this->RemoveCop((*i)->GetHandle());
                break;
            }
        }
    }

    if (active.size() > 1) {
        bool item_removed = false;
        Speech::copList::iterator i = active.begin();
        while ((i != active.end()) && (active.size() > 1)) {
            Speech::copList::iterator j = i + 1;
            while (j != active.end()) {
                if (((*i)->GetSpeakerID() == (*j)->GetSpeakerID()) && ((*i)->GetHandle() != (*j)->GetHandle())) {
                    EAXCop *a = *i;
                    EAXCop *b = *j;
                    if (a->GetDistance() < b->GetDistance()) {
                        this->RemoveCop(b->GetHandle());
                        active.erase(j);
                        item_removed = true;
                    } else {
                        this->RemoveCop(a->GetHandle());
                        active.erase(i);
                        item_removed = true;
                    }
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

    if (active.size() > 0) {
        if ((this->mLeader == nullptr || !this->mLeader->IsActive()) || (this->mLeader == nullptr || this->mLeader->IsHeli())) {
            for (Speech::copList::iterator i = active.begin(); i != active.end(); ++i) {
                EAXCop *activecop = *i;
                if (activecop->IsPrimary() && activecop->HasLOS()) {
                    this->mLeader = activecop;
                    if ((this->mFocus == kStrategyFlow) && !this->mLeader->IsHeli()) {
                        this->mLeader->PrimaryEngage();
                    }
                    break;
                }
            }
        }
        if (this->mLeader != nullptr && !this->mLeader->IsActive() && !this->MakeLeader(this->mLeader)) {
            return;
        }
    }
}

bool SoundAI::IsHeadingValid() {
    if (this->mPlayerCurrent[0].roadID > untagged && this->mPlayerCurrent[0].roadID < Seaside_Interchange &&
        (this->mPlayerCurrent[0].direction > 0)) {
        return true;
    }
    return false;
}

void SoundAI::SyncPlayers() {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    IPerpetrator *perp = nullptr;
    IVehicle *vehicle = nullptr;
    IVehicleAI *vai = nullptr;
    player->GetSimable()->QueryInterface(&vehicle);
    player->GetSimable()->QueryInterface(&perp);
    vehicle->QueryInterface(&vai);

    WRoadNav *nav = vai->GetDriveToNav();

    unsigned int roadID = nav->GetRoadSpeechId();
    this->mPlayerCurrent[0].roadID = static_cast<RoadNames>(roadID);
    unsigned int dir = this->CalcPlayerDirection(false);

    this->mPlayerCurrent[0].direction = dir;

    if (this->IsHeadingValid()) {
        bool road_changed = (this->mPlayerCurrent[1].roadID != this->mPlayerCurrent[0].roadID);
        this->mPlayerCurrent[1].roadID = this->mPlayerCurrent[0].roadID;
        this->mPlayerCurrent[1].direction = this->mPlayerCurrent[0].direction;
        if ((this->mLastKnown.direction != dir) && this->mPursuit != nullptr && this->mPursuit->IsPerpInSight() && road_changed) {
            if ((this->mPursuitState == kActive) && (this->mFocus != kPursuitFlow)) {
                EAXCop *cop = this->FindClosestCop(true, true);
                if (cop != nullptr) {
                    cop->DirectionChange();
                }
            }
            this->mLastKnown.direction = dir;
            this->mLastKnown.roadID = this->mPlayerCurrent[0].roadID;
        }
    }

    if (perp != nullptr) {
        int heat;
        static float prev_heat = 1.0f;

        if (this->mPVehicle.GetCollection() != player->GetSimable()->GetAttributes().GetCollection()) {
            this->mPVehicle.ChangeWithDefault(player->GetSimable()->GetAttributes().GetCollection());
        }

        if (this->mPlayerCarCustom == nullptr || (this->mPlayerCarCustom->color == 0)) {
            if (this->mPlayerCarCustom != nullptr) {
                delete this->mPlayerCarCustom;
            }
            this->mPlayerCarCustom = new ("SoundAI CarCustomization", 0) CarCustomizations;
            if (!this->GetCustomized(vehicle, *this->mPlayerCarCustom)) {
                delete this->mPlayerCarCustom;
                this->mPlayerCarCustom = nullptr;
            }
        }

        this->mPlayerSpeed = MPS2MPH(vehicle->GetSpeed());
        if ((this->mPlayerSpeed < this->mTune.MinSpeedConsideredStopped()) && ((this->mFlags & LOWSPEEDTIMER) == 0)) {
            this->mT_reallylowspeed = WorldTimer;
            this->mFlags |= LOWSPEEDTIMER;
        } else if (this->mPlayerSpeed >= this->mTune.MinSpeedConsideredStopped()) {
            this->mFlags &= ~LOWSPEEDTIMER;
            this->mT_reallylowspeed = WorldTimer;
        }

        this->mPlayerPos = player->GetPosition();
        if (SPAMAccessorSpeech.IsValid()) {
            const bVector2 ppos(this->mPlayerPos.z, -this->mPlayerPos.x);
            SPAMAccessorSpeech.CaptureData(ppos.x, ppos.y);
            this->mPlayerOffroadID = SPAMAccessorSpeech.GetDataInt(1);
        }

        heat = static_cast<int>(perp->GetHeat());
        this->mPlayerHeat = heat;

        if ((this->mFocus != kPursuitFlow) && (5.0f < this->mPursuitDuration) && (perp->GetHeat() > prev_heat)) {
            bool jump = false;
            int i = 3;
            while (i > -1) {
                if ((prev_heat < heat_cutoffs[i].value) && (perp->GetHeat() >= heat_cutoffs[i].value)) {
                    jump = true;
                    prev_heat = perp->GetHeat();
                    break;
                }
                i--;
            }

            if (jump && ((this->mPursuitState == kActive) || (this->mPursuitState == kSearching))) {
                if (bRandom(1.0f) > 0.5f) {
                    this->mDispatch->HeatJump(heat_cutoffs[i].heat_level);
                } else {
                    EAXCop *cop = this->GetRandomActiveCop(1, false);
                    if (this->mHeli != nullptr) {
                        if (bRandom(1.0f) > 0.5f) {
                            this->mHeli->HeatJump(heat_cutoffs[i].heat_level);
                        } else if (cop != nullptr) {
                            cop->HeatJump(heat_cutoffs[i].heat_level);
                        }
                    } else if (cop != nullptr) {
                        cop->HeatJump(heat_cutoffs[i].heat_level);
                    }
                }

                if (bRandom(1.0f) > 0.5f) {
                    if ((3.0f <= perp->GetHeat()) && (perp->GetHeat() < 4.0f)) {
                        this->mDispatch->JurisShift(Csis::Type_jurisdiction_state);
                    } else if (5.0f <= perp->GetHeat()) {
                        this->mDispatch->JurisShift(Csis::Type_jurisdiction_federal);
                    }
                }
            }
        }

        Attrib::Gen::pursuitlevels *pursuitatr = perp->GetPursuitLevelAttrib();
        if (pursuitatr != nullptr && pursuitatr->IsValid()) {
            if (this->mPursuitLevel.GetCollection() != pursuitatr->GetCollection()) {
                this->mPursuitLevel.ChangeWithDefault(pursuitatr->GetCollection());
            }
        }

        if (this->mPursuitLevel.IsValid()) {
            if (0.0f < this->mPursuitLevel.roadblockprobability()) {
                this->mFlags |= RB_ENABLED;
            } else {
                this->mFlags &= ~RB_ENABLED;
            }
            if (0.0f < this->mPursuitLevel.roadblockhelichance()) {
                this->mFlags |= HELIRB_ENABLED;
            } else {
                this->mFlags &= ~HELIRB_ENABLED;
            }
            if (0.0f < this->mPursuitLevel.roadblockspikechance()) {
                this->mFlags |= SPIKES_ENABLED;
            } else {
                this->mFlags &= ~SPIKES_ENABLED;
            }

            this->mNumCopsInWave = this->mPursuitLevel.NumCopsToTriggerBackup();

            if ((this->mPursuitState == kInactive) && ((this->mFlags & DISP911_ACTIVE) == 0)) {
                bool is_DDay = false;
                bool is_Race = false;
                bool is_Roaming = false;
                if ((GRaceStatus::Exists() && (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming)) ||
                    (GRaceDatabase::Exists() && GRaceDatabase::Get().GetStartupRace() == nullptr)) {
                    is_Roaming = true;
                }

                bool copsEnabled;
                float t_lockout;
                ICopMgr *copmgr = ICopMgr::Get();
                if (copmgr != nullptr) {
                    copsEnabled = ICopMgr::AreCopsEnabled();
                }
                if (copmgr != nullptr && copsEnabled) {
                    // TODO temporary A124 bhack to get it to compile
#ifndef EA_BUILD_A124
                    t_lockout = copmgr->GetLockoutTimeRemaining();
#endif
                    if (GRaceStatus::Get().GetRaceParameters() != nullptr) {
                        if (GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
                            is_DDay = true;
                        }
                        if (GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {
                            is_Race = true;
                        }
                    }
                    bool scripted_911 = ((0.0f < t_lockout) && (t_lockout < 20.0f) && !is_Race && !is_Roaming);
                    bool req911_met = false;
                    if (((this->mCTS911 >= this->mPursuitLevel.CTSFor911()) || (this->mTrafficHits911 >= this->mPursuitLevel.NumCiviHitsFor911())) &&
                        is_Roaming) {
                        req911_met = true;
                    }

                    if ((req911_met || scripted_911) && !is_DDay) {
                        this->mDispatch->Report911(this->IsHighIntensity() ? Csis::Type_pursuit_type_Possible_Wanted
                                                                           : Csis::Type_pursuit_type_Generic_Speeder);
                    }
                }
            }
        }

        UMath::Vector3 vel;
        UMath::Unit(player->GetSimable()->GetRigidBody()->GetLinearVelocity(), vel);
        this->mSmoothedFWRoad.x = (this->mSmoothedFWRoad.x * 0.9f) + (vel.x * 0.1f);
        this->mSmoothedFWRoad.y = (this->mSmoothedFWRoad.y * 0.9f) + (vel.y * 0.1f);
        this->mSmoothedFWRoad.z = (this->mSmoothedFWRoad.z * 0.9f) + (vel.z * 0.1f);
    }
}

void SoundAI::Force911State() {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    IPerpetrator *perp = nullptr;

    player->GetSimable()->QueryInterface(&perp);
    ICopMgr *copmgr = ICopMgr::Get();
    if (copmgr != nullptr && ICopMgr::AreCopsEnabled() && perp != nullptr && this->mPursuitLevel.IsValid()) {
        this->mFlags |= DISP911_ACTIVE;
        perp->Set911CallTime(this->mPursuitLevel.Lifetime911());
        copmgr->LockoutCops(false);
    }
}

void SoundAI::SyncCarsToActors() {
    IVehicles new_cop_cars;
    new_cop_cars.reserve(30);
    new_cop_cars.clear();

    float closest = 65535.0f;
    this->mRacerCount = static_cast<char>(IVehicle::GetList(VEHICLE_AIRACERS).size());

    for (IVehicle::List::const_iterator i = IVehicle::GetList(VEHICLE_AIRACERS).begin(); i != IVehicle::GetList(VEHICLE_AIRACERS).end(); ++i) {
        IVehicle *vehicle = *i;
        IRenderable *renderable = nullptr;
        if (vehicle->QueryInterface(&renderable)) {
            float dist2cam = renderable->DistanceToView();
            if (dist2cam < closest) {
                closest = dist2cam;
            }
        }
    }

    if (closest <= this->mTune.AIRacerProximity()) {
        this->mFlags |= RACERS_PROXIMAL;
    } else {
        if ((this->mFlags & RACERS_PROXIMAL) != 0) {
            EAXCop *cop = this->GetRandomActiveCop(0, false);
            if (cop != nullptr) {
                cop->FocusChange();
            }
        }
        this->mFlags &= ~RACERS_PROXIMAL;
    }

    unsigned char cops_in_view = 0;
    unsigned char cops_with_los = 0;
    unsigned char cops_ahead = 0;
    unsigned char num_active = 0;
    float pursuit_distance = 65535.0f;

    for (IVehicle::List::const_iterator i = IVehicle::GetList(VEHICLE_AICOPS).begin(); i != IVehicle::GetList(VEHICLE_AICOPS).end(); ++i) {
        IVehicle *vehicle = *i;
        IRenderable *renderable = nullptr;
        HSIMABLE thisObj;
        vehicle->QueryInterface(&renderable);
        if (vehicle != nullptr) {
            bool has_visual = false;
            bool is_ahead = false;
            bool in_view = false;
            bool is_in_rb = false;

            thisObj = vehicle->GetSimable()->GetOwnerHandle();

            IRoadBlock *irb = this->GetRoadblock();
            if (irb != nullptr) {
                IVehicle *car_in_rb = irb->IsComprisedOf(thisObj);
                is_in_rb = (car_in_rb == vehicle);
            }

            if (vehicle->GetDriverClass() == DRIVER_COP) {
                IPursuitAI *ai = nullptr;
                vehicle->QueryInterface(&ai);

                if (vehicle->GetVehicleClass() == VehicleClass::CAR) {
                    if (renderable != nullptr && renderable->InView()) {
                        cops_in_view++;
                        in_view = true;
                    }

                    if (vehicle->IsActive() && ai != nullptr) {
                        float t_tgt_last_seen = ai->GetTimeSinceTargetSeen();
                        if ((t_tgt_last_seen < 0.05f) || (is_in_rb && in_view)) {
                            has_visual = true;
                            cops_with_los++;
                        }
                    }

                    UMath::Vector3 player_pos = this->mPlayerPos;
                    UMath::Vector3 copcar_pos = vehicle->GetPosition();
                    UMath::Vector3 player_fw = this->mPlayerFW;
                    UMath::Vector3 playerToCop;
                    if (vehicle->IsActive() || (renderable != nullptr && renderable->InView())) {
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

                    EAXCop *actor = this->mActors.Find(thisObj);
                    if (actor != nullptr) {
                        if (vehicle->IsActive()) {
                            if (!actor->IsActive()) {
                                actor->SetActive(true);
                            }
                        } else if (DESTROY_COPS_ON_INACTIVITY) {
                            this->RemoveCop(actor->GetHandle());
                        } else {
                            actor->SetActive(false);
                        }

                        actor->SetLOS(has_visual);
                        actor->SetAhead(is_ahead);
                        actor->Update();

                        float t_lastblowby = (WorldTimer - this->mRecentBlowby.timestamp).GetSeconds();
                        if ((t_lastblowby > this->mTune.BlowbyInterval()) && (this->mPlayerSpeed > actor->GetSpeed()) && !is_ahead) {
                            this->mRecentBlowby.Set(actor->GetDistance(), this->mPlayerSpeed - actor->GetSpeed());
                        }

                        if ((actor->GetDistance() < this->mRecentBlowby.distance) && (this->mPlayerSpeed > actor->GetSpeed()) && !is_ahead) {
                            float speed_diff = this->mPlayerSpeed - actor->GetSpeed();
                            if (speed_diff > (this->mPlayerSpeed * 0.75f)) {
                                this->mRecentBlowby.Set(actor->GetDistance(), this->mPlayerSpeed - actor->GetSpeed());
                            }
                        }
                    } else {
                        new_cop_cars.push_back(vehicle);
                    }
                } else if (vehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                    if (this->mHeli != nullptr) {
                        if (this->mHeli->GetHandle() != vehicle->GetSimable()->GetOwnerHandle()) {
                            this->mHeli->SetHandle(vehicle->GetSimable()->GetOwnerHandle());
                        }

                        if (vehicle->IsActive() && !this->mHeli->IsActive()) {
                            this->mHeli->SetActive(true);
                        } else if (!vehicle->IsActive() && this->mHeli->IsActive()) {
                            this->mHeli->SetActive(false);
                        }

                        if (vehicle->IsActive() && ai != nullptr) {
                            float t_tgt_last_seen = ai->GetTimeSinceTargetSeen();
                            this->mHeli->SetLOS(t_tgt_last_seen < 0.05f);
                        }
                        this->mHeli->Update();
                    } else {
                        this->AddNewHeli(vehicle);
                    }
                }
            }
        }
    }

    this->mCopsInView = cops_in_view;
    this->mLOSCount = cops_with_los;
    this->mPursuitDist = pursuit_distance;
    this->mNumActiveCopCars = num_active;
    if (num_active == cops_ahead) {
        this->mFlags |= COPS_ARE_AHEAD;
    } else {
        this->mFlags &= ~COPS_ARE_AHEAD;
    }

    if (new_cop_cars.size() > 0) {
        for (IVehicles::iterator i = new_cop_cars.begin(); i != new_cop_cars.end(); ++i) {
            this->AddNewCop(*i);
        }
        new_cop_cars.clear();
    }

    for (Speech::copMap::const_iterator iter = this->mActors.begin(); iter != this->mActors.end(); ++iter) {
        EAXCop *actor = iter->cop;
        if (actor != nullptr && actor->IsActive()) {
            ISimable *simable = ISimable::FindInstance(actor->GetHandle());
            if (simable == nullptr) {
                this->RemoveCop(actor->GetHandle());
                break;
            }
        }
    }

    if (this->mDispatch != nullptr) {
        this->mDispatch->Update();
    }
}

void SoundAI::SyncFormations() {
    unsigned int numPrimedCops = this->mCopsInFormation.size();

    for (Speech::copMap::const_iterator iter = this->mActors.begin(); iter != this->mActors.end(); ++iter) {
        EAXCop *cop = iter->cop;
        if (cop->GetInFormation() && !cop->IsHeli()) {
            bool found = false;
            for (Speech::copList::iterator i = this->mCopsInFormation.begin(); i != this->mCopsInFormation.end(); ++i) {
                EAXCop *copInFormation = *i;
                if (copInFormation != nullptr && copInFormation->GetHandle() == cop->GetHandle()) {
                    found = true;
                }
            }
            if (!found && !cop->IsHeli()) {
                this->mCopsInFormation.push_back(cop);
            }
        } else if (!this->mCopsInFormation.empty()) {
            for (Speech::copList::iterator i = this->mCopsInFormation.begin(); i != this->mCopsInFormation.end(); ++i) {
                EAXCop *copInFormation = *i;
                if (copInFormation != nullptr && copInFormation->GetHandle() == cop->GetHandle()) {
                    this->mCopsInFormation.erase(i);
                    break;
                }
            }
        }
    }

    if (this->mCopsInFormation.size() == 1) {
        mLastCopInFormation = *this->mCopsInFormation.begin();
    }
    if (this->mCopsInFormation.size() > 0) {
        this->mT_outofFormation = WorldTimer;
    }
}

EAXCop *SoundAI::FindFurthestCop(bool includeHeli) {
    if (!this->mActors.size()) {
        return nullptr;
    }

    EAXCop *furthest = nullptr;
    for (Speech::copMap::const_iterator iter = this->mActors.begin(); iter != this->mActors.end(); ++iter) {
        EAXCop *cop = iter->cop;
        if (cop != nullptr) {
            if (furthest == nullptr || (cop->GetDistance() > furthest->GetDistance())) {
                if ((cop->IsHeli() && includeHeli) || !cop->IsHeli()) {
                    furthest = cop;
                }
            }
        }
    }
    return furthest;
}

EAXCop *SoundAI::FindClosestCop(bool enforceLOS, bool includeHeli) {
    if (!this->mActors.size()) {
        return nullptr;
    }

    EAXCop *closest = nullptr;
    for (Speech::copMap::const_iterator iter = this->mActors.begin(); iter != this->mActors.end(); ++iter) {
        EAXCop *cop = iter->cop;
        if (cop != nullptr && (!enforceLOS || cop->HasLOS()) && (includeHeli || !cop->IsHeli())) {
            if (closest == nullptr) {
                closest = cop;
            }
            if (cop->GetDistance() < closest->GetDistance()) {
                closest = cop;
            }
        }
    }
    return closest;
}

void SoundAI::RemoveCop(HSIMABLE seeya) {
    if (this->mActors.size() == 0) {
        return;
    }

    EAXCop *cop = this->mActors.Remove(seeya);
    if (cop == nullptr) {
        return;
    }

    gSpeechCache.RemoveSpeaker(cop->GetSpeakerID());

    if (!this->mCopsInFormation.empty()) {
        for (Speech::copList::iterator i = this->mCopsInFormation.begin(); i != this->mCopsInFormation.end(); ++i) {
            EAXCop *copInFormation = *i;
            if (copInFormation != nullptr && copInFormation->GetHandle() == cop->GetHandle()) {
                this->mCopsInFormation.erase(i);
                break;
            }
        }
    }

    if (this->mLastCopInFormation != nullptr && (this->mLastCopInFormation->GetHandle() == cop->GetHandle())) {
        this->mLastCopInFormation = nullptr;
    }
    if (this->mLeader == cop) {
        this->mLeader = nullptr;
    }
    if (this->mHeli == cop) {
        this->mHeli = nullptr;
    }
    if (this->mLatestCop == cop) {
        this->mLatestCop = nullptr;
    }

    this->mPursuitFlow->OnCopRemoved(cop);
    mUsage.voices.push_back(cop->GetSpeakerID());
    delete cop;
}

void SoundAI::AddNewHeli(IVehicle *heli) {
    HSIMABLE handle = heli->GetSimable()->GetOwnerHandle();
    EAXAirSupport *chopper = new EAXAirSupport(2, handle);

    this->mActors.Add(handle, chopper);
    this->mHeli = chopper;

    if (this->mFocus != 1) {
        if ((this->mFocus != 999) && (this->mFocus != 0)) {
            chopper->BackupArrives();
            return;
        }
        if (this->mFocus != 1) {
            return;
        }
    }
    this->mFlags |= HELI_INTRO_REQ;
}

int SoundAI::GetBattalionFromRoadID(int roadID) {
    Csis::Type_location_region region;
    Csis::Type_location location;
    bool result = MiscSpeech::GetLocation(static_cast<RoadNames>(roadID), region, location);

    if (!result) {
        return -1;
    }
    switch (region) {
        case Csis::Type_location_region_city:
            return Csis::Type_speaker_battalion_City;
        case Csis::Type_location_region_coastal:
        case Csis::Type_location_region_coastal_extra:
            return Csis::Type_speaker_battalion_Coastal;
        case Csis::Type_location_region_college_town:
            return Csis::Type_speaker_battalion_Rosewood;
        default:
            return -1;
    }
}

int SoundAI::GetBattalionFromKey(unsigned int theKey) {
    if ((theKey == Attrib::Hash::pvehicle::key_copsuv) || (theKey == Attrib::Hash::pvehicle::key_copsuvl) ||
        (theKey == Attrib::Hash::pvehicle::key_copsuvpatrol)) {
        return Csis::Type_speaker_battalion_Rhino_Units;
    }
    if ((theKey == Attrib::Hash::pvehicle::key_copsport) || (theKey == Attrib::Hash::pvehicle::key_copsportghost) || (theKey == 0xB2F32FE2) ||
        (theKey == Attrib::Hash::pvehicle::key_copcross)) {
        return Csis::Type_speaker_battalion_Super_Pursuit;
    }
    return -1;
}

void SoundAI::AddNewCop(IVehicle *newcop) {
    IVehicleAI *vai = nullptr;
    newcop->QueryInterface(&vai);
    HSIMABLE newbie = newcop->GetSimable()->GetOwnerHandle();
    int bID = static_cast<int>(vai->GetAttributes().DetachmentID());
    WRoadNav *nav = vai->GetDriveToNav();
    int roadID = nav->GetRoadSpeechId();
    bool is_rb_cop = false;
    bool is_cross = (newcop->GetVehicleKey() == Attrib::Hash::pvehicle::key_copcross);

    IRoadBlock *block = this->GetRoadblock();
    if (block != nullptr) {
        is_rb_cop = (block->IsComprisedOf(newbie) == newcop);
    }

    EAXCop *latest_cop = nullptr;
    int keyedID = this->GetBattalionFromKey(newcop->GetVehicleKey());
    if (keyedID > 0) {
        bID = keyedID;
    } else {
        if (roadID == MAX_ROADNAMES) {
            return;
        }
        int bid = this->GetBattalionFromRoadID(roadID);
        if (bid > 0) {
            bID = bid;
        } else {
            bID = 1 << bRandom(4);
        }
    }

    if (mUsage.voices.empty() || !is_cross) {
        UMath::Vector3 cop_pos = newcop->GetPosition();
        UMath::Vector3 pPos = this->mPlayerPos;
        float distance = UMath::Distance(pPos, cop_pos);

        for (Speech::copMap::const_iterator i = this->mActors.begin(); i != this->mActors.end(); ++i) {
            EAXCop *cop = i->cop;
            if (!cop->IsHeli() && !is_rb_cop && (distance < cop->GetDistance()) && !cop->IsHeli()) {
                if (cop->GetCallsign() != bID) {
                    int keyedID = this->GetBattalionFromKey(newcop->GetVehicleKey());
                    if (keyedID > 0) {
                        bID = keyedID;
                    } else if (roadID != MAX_ROADNAMES) {
                        int bid = this->GetBattalionFromRoadID(roadID);
                        if (bid > 0) {
                            bID = bid;
                        } else {
                            bID = 1 << bRandom(4);
                        }
                    } else {
                        return;
                    }
                }

                this->mActors.ModifyHandle(cop->GetHandle(), newbie);
                cop->SetHandle(newbie);
                int cID = this->GetCallsign(static_cast<Csis::Type_speaker_battalion>(bID));
                cop->SetCallsign(bID);
                cop->SetUnitNumber(cID);
                cop->Update();
                latest_cop = cop;
                break;
            }
        }
    }

    if (latest_cop == nullptr) {
        if (!mUsage.voices.empty() || is_cross) {
            int voice = this->GetVoice(is_cross ? 3 : (is_rb_cop ? 2 : 0)); // TODO magic
            if (voice > 0) {
                int cID = this->GetCallsign(static_cast<Csis::Type_speaker_battalion>(bID));
                EAXCop *primary = new EAXCop(voice, newbie, bID, cID);
                primary->SetRank(this->mActors.size());
                this->mActors.Add(newbie, primary);
                gSpeechCache.AddSpeaker(voice);
                latest_cop = primary;
                latest_cop->Update();
            }
        }
        if (latest_cop == nullptr) {
            return;
        }
    }

    this->mLatestCop = latest_cop;
    if ((this->mPursuitState == kActive || this->mPursuitState == kSearching) && !is_rb_cop && (this->mFocus == kStrategyFlow)) {
        if (bRandom(1.0f) > 0.5f) {
            this->mLatestCop->BackupArrives();
        } else {
            this->mLatestCop->UnitBackupReply();
        }
    }
}

bool SoundAI::MakeLeader(EAXCop *newprim) {
    if (this->mLeader != nullptr && newprim != nullptr && (this->mLeader->GetSpeakerID() == newprim->GetSpeakerID())) {
        return true;
    }

    if (newprim->IsActive()) {
        if ((newprim->GetSpeakerID() == Speech::Primary3) || (newprim->GetSpeakerID() == Speech::Primary2) ||
            (newprim->GetSpeakerID() == Speech::Primary1) || (newprim->GetSpeakerID() == Speech::Heli) ||
            (newprim->GetSpeakerID() == Speech::Cross)) {
            if (this->mLeader != newprim) {
                this->mLeader = newprim;
                if (this->mFocus == kStrategyFlow) {
                    newprim->PrimaryEngage();
                }
            }
            return true;
        }
    }

    EAXCop *wannab = this->mActors.Find(newprim->GetHandle());
    if (wannab == nullptr) {
        return false;
    }

    Speech::copMap::iterator primary = this->mActors.begin();
    while (primary != this->mActors.end()) {
        EAXCop *cop = primary->cop;
        if ((cop->GetSpeakerID() == Speech::Primary3) || (cop->GetSpeakerID() == Speech::Primary2) || (cop->GetSpeakerID() == Speech::Primary1) ||
            (cop->GetSpeakerID() == Speech::Cross)) {
            break;
        }
        ++primary;
    }

    if (primary == this->mActors.end()) {
        int rand = bRandom(3);
        switch (rand) {
            case 0:
                newprim->SetSpeakerID(Speech::Primary1);
                break;
            case 1:
                newprim->SetSpeakerID(Speech::Primary2);
                break;
            case 2:
                newprim->SetSpeakerID(Speech::Primary3);
                break;
            default:
                newprim->SetSpeakerID(Speech::Primary1);
                break;
        }

        Speech::voiceIDs::iterator i = mUsage.voices.begin();
        while (i != mUsage.voices.end()) {
            if (*i == newprim->GetSpeakerID()) {
                mUsage.voices.erase(i);
                break;
            }
            ++i;
        }

        if (this->mLeader != newprim) {
            this->mLeader = newprim;
            if (this->mFocus == kStrategyFlow) {
                newprim->PrimaryEngage();
            }
        }
        gSpeechCache.AddSpeaker(this->mLeader->GetSpeakerID());
        return true;
    } else {
        int spkrA = wannab->GetSpeakerID();
        int spkrB = primary->cop->GetSpeakerID();
        wannab->SetSpeakerID(spkrB);
        primary->cop->SetSpeakerID(spkrA);
    }

    if (this->mLeader != newprim) {
        this->mLeader = newprim;
        if (this->mFocus == kStrategyFlow) {
            newprim->PrimaryEngage();
        }
    }
    return true;
}

int SoundAI::GetCallsign(Csis::Type_speaker_battalion battalion) {
    Speech::voiceIDs *cs_pool = nullptr;

    switch (battalion) {
        case Csis::Type_speaker_battalion_Rosewood:
            if (mUsage.cs_Rosewood.empty()) {
                this->RandomizeCallsign(mUsage.cs_Rosewood, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
            }
            cs_pool = &mUsage.cs_Rosewood;
            break;
        case Csis::Type_speaker_battalion_Coastal:
            if (mUsage.cs_Coastal.empty()) {
                this->RandomizeCallsign(mUsage.cs_Coastal, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
            }
            cs_pool = &mUsage.cs_Coastal;
            break;
        case Csis::Type_speaker_battalion_City:
            if (mUsage.cs_City.empty()) {
                this->RandomizeCallsign(mUsage.cs_City, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign20);
            }
            cs_pool = &mUsage.cs_City;
            break;
        case Csis::Type_speaker_battalion_Alpine:
            if (mUsage.cs_Alpine.empty()) {
                this->RandomizeCallsign(mUsage.cs_Alpine, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
            }
            cs_pool = &mUsage.cs_Alpine;
            break;
        case Csis::Type_speaker_battalion_Super_Pursuit:
            if (mUsage.cs_SuperPursuit.empty()) {
                this->RandomizeCallsign(mUsage.cs_SuperPursuit, Csis::Type_speaker_call_sign_id_CallSign01,
                                        Csis::Type_speaker_call_sign_id_CallSign05);
            }
            cs_pool = &mUsage.cs_SuperPursuit;
            break;
        case Csis::Type_speaker_battalion_Rhino_Units:
            if (mUsage.cs_Rhino.empty()) {
                this->RandomizeCallsign(mUsage.cs_Rhino, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign06);
            }
            cs_pool = &mUsage.cs_Rhino;
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

void SoundAI::RandomizeCallsign(Speech::voiceIDs &cs, Csis::Type_speaker_call_sign_id start, Csis::Type_speaker_call_sign_id finish) {
    if (cs.empty()) {
        for (int i = start; i <= finish; i += i) {
            cs.push_back(i);
        }
        for (unsigned int i = 0; i < cs.size(); i++) {
            int rand = bRandom(static_cast<int>(cs.size()));
            int rand_cs = cs[rand];
            int curr_cs = cs[i];
            if (rand_cs != curr_cs) {
                cs[i] = rand_cs;
                cs[rand] = curr_cs;
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
            for (Speech::voiceIDs::iterator i = mUsage.voices.begin(); i < mUsage.voices.end(); ++i) {
                if (*i == Speech::Primary1 || *i == Speech::Primary2 || *i == Speech::Primary3) {
                    return_voice = *i;
                    mUsage.voices.erase(i);
                    break;
                }
            }
            break;
        case 2:
            for (Speech::voiceIDs::iterator i = mUsage.voices.begin(); i < mUsage.voices.end(); ++i) {
                if (*i == Speech::Secondary1 || *i == Speech::Secondary2 || *i == Speech::Secondary3) {
                    return_voice = *i;
                    mUsage.voices.erase(i);
                    break;
                }
            }
            break;
        case 3:
            for (Speech::voiceIDs::iterator i = mUsage.voices.begin(); i < mUsage.voices.end(); ++i) {
                if (*i == Speech::Cross) {
                    return_voice = Speech::Cross;
                    mUsage.voices.erase(i);
                    break;
                }
            }
            break;
        default:
            for (Speech::voiceIDs::iterator i = mUsage.voices.begin(); i < mUsage.voices.end(); ++i) {
                if (*i >= Speech::Primary1 && *i <= Speech::Secondary3) {
                    return_voice = *i;
                    mUsage.voices.erase(i);
                    break;
                }
            }
            break;
    }
    return return_voice;
}

EAXCop *SoundAI::GetCop(int speaker) {
    for (Speech::copMap::iterator iter = mActors.begin(); iter != mActors.end(); ++iter) {
        if (iter->cop != nullptr && iter->cop->GetSpeakerID() == speaker) {
            return iter->cop;
        }
    }
    return nullptr;
}

void SoundAI::RandomBailoutDeny(EAXCop *wimp) {
    if (wimp->GetInFormation() == true && (mActors.size() > 1) && (mPursuitState != kInactive)) {
        for (Speech::copMap::iterator iter = mActors.begin(); iter != mActors.end(); ++iter) {
            if (iter->cop->IsPrimary()) {
                if (iter->cop->GetSpeakerID() != wimp->GetSpeakerID()) {
                    iter->cop->DenyBailout();
                }
            }
        }
    }
}

unsigned int SoundAI::CalcPlayerDirection(bool force_set) {
    static unsigned int dir_tracking = 0;
    static Timer t_currdir(0);

    unsigned int dir;
    float zmag = UMath::Abs(this->mSmoothedFWRoad.z);
    float xmag = UMath::Abs(this->mSmoothedFWRoad.x);
    if (xmag > zmag) {
        if (this->mSmoothedFWRoad.x > 0.0f) {
            dir = 4;
        } else {
            dir = 8;
        }
    } else {
        if (this->mSmoothedFWRoad.z > 0.0f) {
            dir = 2;
        } else {
            dir = 1;
        }
    }

    if ((dir != 0) && (dir != dir_tracking)) {
        dir_tracking = dir;
        t_currdir = WorldTimer;
    }

    float t_samedir = (WorldTimer - t_currdir).GetSeconds();
    if ((t_samedir > 3.0f) || force_set) {
        return dir;
    } else {
        return 0;
    }
}

void SoundAI::ForceGlobalVoiceChange() {
    for (Speech::copMap::iterator iter = this->mActors.begin(); iter != this->mActors.end(); ++iter) {
        delete iter->cop;
    }

    this->mActors.clear();
    this->mCopsInFormation.clear();
}

void SoundAI::Release() {
    if ((mRefCount != 0) && (--mRefCount == 0)) {
        this->Sim::Activity::Release();
    }
}

// total size: 0x8
// Decl: 3716
struct ColourHashToSoundColour {
    uint32 Hash;                      // offset 0x0, size 0x4, Decl: 3717
    Csis::Type_car_color SoundColour; // offset 0x4, size 0x4, Decl: 3718
};

// Decl: 3721
ColourHashToSoundColour ColourHashToSoundColourMap[] = {
    {STRINGHASH_RED, Csis::Type_car_color_Red},       {STRINGHASH_BLACK, Csis::Type_car_color_Black},
    {STRINGHASH_WHITE, Csis::Type_car_color_White},   {STRINGHASH_BLUE, Csis::Type_car_color_Blue},
    {STRINGHASH_GREEN, Csis::Type_car_color_Green},   {STRINGHASH_ORANGE, Csis::Type_car_color_Orange},
    {STRINGHASH_SILVER, Csis::Type_car_color_Silver}, {STRINGHASH_GOLD, Csis::Type_car_color_Gold},
    {STRINGHASH_PURPLE, Csis::Type_car_color_Purple}, {STRINGHASH_BROWN, Csis::Type_car_color_Brown},
    {STRINGHASH_YELLOW, Csis::Type_car_color_Yellow}, {STRINGHASH_PINK, Csis::Type_car_color_Pink},
    {STRINGHASH_BEIGE, Csis::Type_car_color_Beige},
};

int NumberOfColourHashToSoundColourMaps = NUM_ELEMENTS(ColourHashToSoundColourMap); //  Decl: 3737

uint8 SoundAI::GetCustomized(IVehicle *vehicle, CarCustomizations &custrec) {
    const FECustomizationRecord *record = vehicle->GetCustomizations();

    custrec.flags = 0;
    bool has_vinyls = false;
    bool has_custom_paint;
    bool has_racing_numbers = false;
    bool has_decals = false;
    has_custom_paint = has_decals;

    if (record != nullptr) {
        CarPart *paint_part = record->GetInstalledPart(vehicle->GetModelType(), CARSLOTID_BASE_PAINT);
        if (paint_part != nullptr) {
            unsigned int colour_hash = paint_part->GetAppliedAttributeUParam(STRINGHASH_SPEECHCOLOUR, 0);
            for (int i = 0; i < NumberOfColourHashToSoundColourMaps; i++) {
                if (ColourHashToSoundColourMap[i].Hash == colour_hash) {
                    custrec.color = ColourHashToSoundColourMap[i].SoundColour;
                    break;
                }
            }

            CarPart *vinyls = record->GetInstalledPart(vehicle->GetModelType(), CARSLOTID_VINYL_LAYER0);
            if (vinyls != nullptr) {
                has_vinyls = true;
            }

            CarPart *left_number1 = record->GetInstalledPart(vehicle->GetModelType(), CARSLOTID_DECAL_LEFT_DOOR_TEX6);
            CarPart *left_number2 = record->GetInstalledPart(vehicle->GetModelType(), CARSLOTID_DECAL_LEFT_DOOR_TEX7);
            CarPart *right_number1 = record->GetInstalledPart(vehicle->GetModelType(), CARSLOTID_DECAL_RIGHT_DOOR_TEX6);
            CarPart *right_number2 = record->GetInstalledPart(vehicle->GetModelType(), CARSLOTID_DECAL_RIGHT_DOOR_TEX7);
            if (left_number1 != nullptr || left_number2 != nullptr || right_number1 != nullptr || right_number2 != nullptr) {
                has_racing_numbers = true;
            }

            CarPart *left_door_decal = record->GetInstalledPart(vehicle->GetModelType(), CARPARTID_DECAL_LEFT_DOOR);
            CarPart *right_door_decal = record->GetInstalledPart(vehicle->GetModelType(), CARPARTID_DECAL_RIGHT_DOOR);
            CarPart *left_quarter = record->GetInstalledPart(vehicle->GetModelType(), CARPARTID_DECAL_LEFT_QUARTER);
            CarPart *right_quarter = record->GetInstalledPart(vehicle->GetModelType(), CARPARTID_DECAL_RIGHT_QUARTER);
            if (left_door_decal != nullptr || right_door_decal != nullptr || left_quarter != nullptr || right_quarter != nullptr) {
                has_decals = true;
            }
        }
    } else {
        CarTypeInfo *type_info = GetCarTypeInfo(vehicle->GetModelType());
        CarPart *paint_part = CarPartDB.NewGetCarPart(vehicle->GetModelType(), CARSLOTID_BASE_PAINT, type_info->GetDefaultBasePaint(), nullptr, -1);
        if (paint_part != nullptr) {
            unsigned int colour_hash = paint_part->GetAppliedAttributeUParam(STRINGHASH_SPEECHCOLOUR, 0);
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

bool SoundAI::IsHighIntensity() {
    if ((this->mPlayerHeat >= static_cast<int>(this->mTune.HighIntensityMark())) ||
        (this->mPursuitDuration >= this->mTune.PursuitDurationHighIntensity())) {
        return true;
    }
    return false;
}

const float SoundAI::GetTimeLastNailedCop() {
    Speech::copList nailed;
    nailed.reserve(mActors.size());
    for (Speech::copMap::const_iterator iter = mActors.begin(); iter != mActors.end(); ++iter) {
        EAXCop *cop = iter->cop;
        if (cop->GetTimesRammed() > 0) {
            nailed.push_back(cop);
        }
    }

    if (nailed.empty()) {
        return 65535.0f;
    }

    float t_mostrecent = nailed.front()->GetTimeLastRammed();
    for (Speech::copList::iterator i = nailed.begin(); i != nailed.end(); ++i) {
        EAXCop *unfortunate = *i;
        if (unfortunate->GetTimeLastRammed() < t_mostrecent) {
            t_mostrecent = unfortunate->GetTimeLastRammed();
        }
    }
    return t_mostrecent;
}
