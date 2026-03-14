#include "Speed/Indep/Src/AI/AIVehiclePursuit.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

float AIVehiclePursuit::mStagger = 0.0f;

AIVehiclePursuit::AIVehiclePursuit(const BehaviorParams &bp)
    : AIVehiclePid(bp, 0.125f, mStagger, Sim::TASK_FRAME_FIXED), //
      IPursuitAI(bp.fowner),                                     //
      mInPursuit(false),                                         //
      mBreaker(false),                                           //
      mChicken(false),                                           //
      mDamagedByPerp(false),                                     //
      mSirenState(SIREN_OFF),                                    //
      mSirenInit(false),                                         //
      mInFormation(false),                                       //
      mInPosition(false),                                        //
      mWithinEngagementRadius(false),                            //
      mPursuitOffset(UMath::Vector3::kZero) {
    mStagger += 0.125f;
    if (mStagger >= 1.0f) {
        mStagger = 0.0f;
    }
    mVisibiltyTestTimer = 0.0f;
    mTimeSinceTargetSeen = 99.0f;
    mSupportGoal = (const char *)nullptr;
    mT_siren[0] = mT_siren[1] = mT_siren[2] = WorldTimer;
}

AIVehiclePursuit::~AIVehiclePursuit() {}

void AIVehiclePursuit::ResetInternals() {
    AIVehicle::ResetInternals();
    mInPursuit = false;
    mBreaker = false;
    mChicken = false;
    mDamagedByPerp = false;
    mInFormation = false;
    mInPosition = false;

    mPursuitOffset = UMath::Vector3::kZero;

    mTimeSinceTargetSeen = 99.0f;
    mVisibiltyTestTimer = 0.25f;

    mWithinEngagementRadius = false;

    mT_siren[0] = mT_siren[1] = mT_siren[2] = WorldTimer;
    mSirenState = SIREN_OFF;
    mSirenInit = false;
}

void AIVehiclePursuit::StartPatrol() {
    SetInPursuit(false);
    GetTarget()->Clear();
    SetGoal("AIGoalPatrol");
}

void AIVehiclePursuit::StartFlee() {
    IVehicle *ivehicle;
    GetVehicle()->GlareOff(LIGHT_COPS);

    UCrc32 goal("AIGoalFleePursuit");
    if (GetSimable()->QueryInterface(&ivehicle) && ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
        goal = "AIGoalHeliExit";
    }
    if (GetGoalName() != goal) {
        ClearGoal();
        SetGoal(goal);
    }
}

void AIVehiclePursuit::StartRoadBlock() {
    IVehicle *ivehicle;
    GetVehicle()->GlareOn(LIGHT_COPS);
    SetInPursuit(true);
    GetTarget()->Clear();
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        SetGoal("AIGoalHeliRoadBlock");
    } else {
        SetGoal("AIGoalStaticRoadBlock");
    }
}

void AIVehiclePursuit::StartPursuit(AITarget *target, ISimable *itargetSimable) {
    GetVehicle()->GlareOn(LIGHT_COPS);
    if (target) {
        GetTarget()->Aquire(target);
    } else if (itargetSimable) {
        GetTarget()->Aquire(itargetSimable);
    }
    UpdateTargeting();
    SetInPursuit(true);
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        SetGoal("AIGoalHeliPursuit");
    } else {
        SetGoal("AIGoalPursuit");
    }
}

void AIVehiclePursuit::DoInPositionGoal() {
    SetGoal(mInPositionGoal);
}

void AIVehiclePursuit::EndPursuit() {
    SetInPursuit(false);
    GetVehicle()->GlareOff(LIGHT_COPS);
}

bool AIVehiclePursuit::StartSupportGoal() {
    if (mSupportGoal != (const char *)nullptr) {
        SetGoal(mSupportGoal);
        return true;
    }
    return false;
}

void AIVehiclePursuit::SetSupportGoal(UCrc32 sg) {
    mSupportGoal = sg;
}

AITarget *AIVehiclePursuit::GetPursuitTarget() {
    if (GetTarget()->IsValid()) {
        return GetTarget();
    } else {
        return nullptr;
    }
}

AITarget *AIVehiclePursuit::PursuitRequest() {
    if (!GetInPursuit() && GetTarget()->IsValid()) {
        return GetTarget();
    } else {
        return nullptr;
    }
}

void AIVehiclePursuit::Update(float dT) {
    ProfileNode profile_node("TODO", 0);
    AIVehicle::Update(dT);
    UpdateSiren(dT);

    if (!mInPursuit || !GetTarget()->IsValid()) {
        mTimeSinceTargetSeen = 0.25f;
        mVisibiltyTestTimer = 0.25f;
        return;
    }

    mVisibiltyTestTimer += dT;
    mTimeSinceTargetSeen += dT;
    if (mVisibiltyTestTimer >= 0.25f) {
        mVisibiltyTestTimer -= 0.25f;
        if (CanSeeTarget(GetTarget())) {
            mTimeSinceTargetSeen = -0.25f;
        }
    }
}

void AIVehiclePursuit::UpdateSiren(float dT) {
    IPursuit *ipursuit = GetPursuit();
    SoundAI *soundai = SoundAI::Get();
    IVehicle *cop_car = GetVehicle();

    bool pursuitRace = false;
    bool is_dday = false;
    bool is_in_rb = false;
    bool doAI = true;

    if (GRaceStatus::Get().GetRaceParameters()) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
        is_dday = GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace();
    }

    const UTL::Collections::Listable<IRoadBlock, 8>::List &blocks =
        UTL::Collections::Listable<IRoadBlock, 8>::GetList();
    for (IRoadBlock *const *i = blocks.begin(); i != blocks.end(); ++i) {
        IRoadBlock *rb = *i;
        if (rb) {
            IVehicle *vehicle = rb->IsComprisedOf(GetSimable()->GetOwnerHandle());
            if (GetVehicle() == vehicle) {
                is_in_rb = true;
                break;
            }
        }
    }

    if (!soundai || !ipursuit || !mInPursuit || !soundai->GetPursuitSpecs().IsValid()) {
        mSirenState = SIREN_OFF;
    } else {
        const Attrib::Gen::pursuitlevels &pursuitatr = soundai->GetPursuitSpecs();

        if (ipursuit->IsPlayerPursuit() && !is_dday && soundai->NumPursuits() < 2) {
            if (soundai->GetFocus() != SoundAI::kPursuitFlow ||
                soundai->GetPursuitDuration() >= 5.0f) {
                if (soundai->GetFocus() == SoundAI::kLost) {
                    mSirenState = SIREN_OFF;
                    doAI = false;
                    mT_siren[0] = WorldTimer;
                }
            } else {
                doAI = false;
                float dt = (WorldTimer - mT_siren[0]).GetSeconds();
                {
                    static float variation;
                    static bool __tmp_40;
                    if (!__tmp_40) {
                        variation = pursuitatr.SirenInitMinPeriod() +
                                    bRandom(pursuitatr.SirenInitVariation());
                        __tmp_40 = true;
                    }
                    if (variation < dt && mSirenState == SIREN_OFF) {
                        mSirenState = SIREN_YELP;
                        mT_siren[1] = WorldTimer;
                    }
                    if (mSirenState == SIREN_YELP) {
                        float t_bleeping = (WorldTimer - mT_siren[1]).GetSeconds();
                        if (t_bleeping > pursuitatr.SirenInitMinPeriod()) {
                            mSirenState = SIREN_OFF;
                            variation = pursuitatr.SirenInitMinPeriod() +
                                        bRandom(pursuitatr.SirenInitVariation());
                            doAI = false;
                            mT_siren[0] = WorldTimer;
                        }
                    }
                }
            }
        }

        if (!cop_car->IsGlareOn(LIGHT_COPS)) {
            if (!cop_car->IsDestroyed()) {
                mSirenState = SIREN_OFF;
            } else {
                mSirenState = SIREN_DIE;
            }
        }

        if (doAI) {
            float t_start = (WorldTimer - mT_siren[0]).GetSeconds();
            bool high_heat = soundai->IsHighIntensity();
            bool siren_initializing = t_start < 10.0f;

            if (!siren_initializing && mSirenInit) {
                mT_siren[1] = WorldTimer;
                if (!high_heat) {
                    mSirenState = SIREN_WAIL;
                } else {
                    mSirenState = SIREN_YELP;
                }
            }
            mSirenInit = siren_initializing;

            if (siren_initializing) {
                if (!high_heat) {
                    mSirenState = SIREN_YELP;
                } else {
                    mSirenState = SIREN_SCREAM;
                    if (t_start >= 5.0f) {
                        mSirenState = SIREN_YELP;
                    }
                }
            } else {
                if ((ipursuit->IsCollapseActive() ||
                     ipursuit->IsFinisherActive() ||
                     Speech::Manager::IsCopSpeechPlaying(
                         160) ||
                     Speech::Manager::IsCopSpeechPlaying(
                         214) ||
                     Speech::Manager::IsCopSpeechPlaying(
                         222) ||
                     Speech::Manager::IsCopSpeechPlaying(
                         64)) &&
                    mInFormation) {
                    if (!high_heat) {
                        mSirenState = SIREN_YELP;
                    } else {
                        float t_screaming = (WorldTimer - mT_siren[2]).GetSeconds();
                        if (mSirenState == SIREN_SCREAM ||
                            t_screaming <= pursuitatr.SirenScreamPeriod()) {
                            if (pursuitatr.SirenMaxScreamTime() < t_screaming) {
                                mSirenState = SIREN_YELP;
                                mT_siren[2] = WorldTimer;
                            }
                        } else {
                            mSirenState = SIREN_SCREAM;
                            mT_siren[2] = WorldTimer;
                        }
                    }
                    mT_siren[1] = WorldTimer;
                } else {
                    float t_constant = (WorldTimer - mT_siren[1]).GetSeconds();
                    float sirenWailPeriod = pursuitatr.SirenWailPeriod();
                    if (sirenWailPeriod + pursuitatr.SirenMaxYelpTime() < t_constant) {
                        mSirenState = SIREN_WAIL;
                        mT_siren[1] = WorldTimer;
                    } else if (t_constant > sirenWailPeriod) {
                        mSirenState = SIREN_YELP;
                    }
                }
            }
        }
    }

    if (is_in_rb) {
        mSirenState = SIREN_WAIL;
    }
}
