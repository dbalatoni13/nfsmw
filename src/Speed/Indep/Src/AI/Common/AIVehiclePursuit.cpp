#include "Speed/Indep/Src/AI/AIVehiclePursuit.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/P2temp.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/VehicleFX.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

float AIVehiclePursuit::mStagger = 0.0f;

AIVehiclePursuit::AIVehiclePursuit(const BehaviorParams &bp)
    : AIVehiclePid(bp, 0.125f, mStagger, Sim::TASK_FRAME_FIXED), //
      IPursuitAI(bp.fowner),                                     //
      mInPursuit(false),                                         //
      mBreaker(false),                                           //
      mChicken(false),                                           //
      mDamagedByPerp(false),                                     //
      mSirenState(Sound::SIREN_OFF),                                    //
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
    mSirenState = Sound::SIREN_OFF;
    mSirenInit = false;
}

void AIVehiclePursuit::StartPatrol() {
    SetInPursuit(false);
    GetTarget()->Clear();
    SetGoal("AIGoalPatrol");
}

void AIVehiclePursuit::StartFlee() {
    IVehicle *ivehicle;
    GetVehicle()->GlareOff(VehicleFX::LIGHT_COPS);

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
    GetVehicle()->GlareOn(VehicleFX::LIGHT_COPS);
    SetInPursuit(true);
    GetTarget()->Clear();
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        SetGoal("AIGoalHeliRoadBlock");
    } else {
        SetGoal("AIGoalStaticRoadBlock");
    }
}

void AIVehiclePursuit::StartPursuit(AITarget *target, ISimable *itargetSimable) {
    GetVehicle()->GlareOn(VehicleFX::LIGHT_COPS);
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
    GetVehicle()->GlareOff(VehicleFX::LIGHT_COPS);
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
    ProfileNode profile_node;
    AIVehicle::Update(dT);
    UpdateSiren(dT); // TODO

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
    IPursuit *pursuit = GetPursuit();
    SoundAI *soundAI = UTL::Collections::Singleton<SoundAI>::Get();
    IVehicle *vehicle = GetVehicle();
    bool is_dday = false;
    bool in_roadblock = false;
    bool do_cycle = true;

    if (GRaceStatus::Get().GetRaceParameters()) {
        bool pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
        is_dday = GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace();
    }

    const IRoadBlock::List &blocks = IRoadBlock::GetList();
    for (IRoadBlock::List::const_iterator iter = blocks.begin(); iter != blocks.end(); ++iter) {
        IRoadBlock *rb = *iter;
        if (rb) {
            IVehicle *comprised = rb->IsComprisedOf(GetSimable()->GetOwnerHandle());
            if (GetVehicle() == comprised) {
                in_roadblock = true;
                break;
            }
        }
    }

    if (soundAI && pursuit && mInPursuit && soundAI->GetPursuitSpecs().IsValid()) {
        const Attrib::Gen::pursuitlevels &pl = soundAI->GetPursuitSpecs();

        if (pursuit->IsPlayerPursuit() && !is_dday && soundAI->NumPursuits() <= 1) {
            const Attrib::Gen::pursuitlevels &pursuitatr = soundAI->GetPursuitSpecs();
            if (soundAI->GetFocus() == SoundAI::kSearching && soundAI->GetPursuitDuration() < 5.0f) {
                do_cycle = false;
                // El objetivo trae `variation.36214` mas la bandera de guarda
                // `_.tmp_40.36215`: es un estatico local con inicializador
                // DINAMICO, no un par de estaticos con bandera a mano.
                float t0 = (WorldTimer - mT_siren[0]).GetSeconds();
                static float variation = pursuitatr.SirenInitMinPeriod() + bRandom(pursuitatr.SirenInitVariation());
                if (t0 > variation && mSirenState == Sound::SIREN_OFF) {
                    mT_siren[1] = WorldTimer;
                    mSirenState = Sound::SIREN_YELP;
                }
                if (mSirenState == Sound::SIREN_YELP) {
                    float t_bleeping = (WorldTimer - mT_siren[1]).GetSeconds();
                    if (t_bleeping > 0.5f) {
                        mSirenState = Sound::SIREN_OFF;
                        variation = pursuitatr.SirenInitMinPeriod() + bRandom(pursuitatr.SirenInitVariation());
                        mT_siren[0] = WorldTimer;
                    }
                }
            } else if (soundAI->GetFocus() == SoundAI::kLost) {
                mSirenState = Sound::SIREN_OFF;
                do_cycle = false;
                mT_siren[0] = WorldTimer;
            }
        }

        if (!vehicle->IsGlareOn(VehicleFX::LIGHT_COPS)) {
            mSirenState = vehicle->IsDestroyed() ? (mSirenState = Sound::SIREN_DIE) : Sound::SIREN_OFF;
        }

        if (do_cycle) {
            float t0 = (WorldTimer - mT_siren[0]).GetSeconds();
            bool high = soundAI->IsHighIntensity();
            bool past6 = t0 < 6.0f;
            if (!past6 && mSirenInit) {
                mT_siren[1] = WorldTimer;
                if (high) {
                    mSirenState = Sound::SIREN_YELP;
                } else {
                    mSirenState = Sound::SIREN_WAIL;
                }
            }
            mSirenInit = past6;
            if (past6) {
                if (!high) {
                    mSirenState = Sound::SIREN_YELP;
                } else if (t0 < 3.0f) {
                    mSirenState = Sound::SIREN_SCREAM;
                } else {
                    mSirenState = Sound::SIREN_YELP;
                }
            } else {
                if ((pursuit->IsCollapseActive() || pursuit->IsFinisherActive() ||
                     Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_ReInitPursuit) ||
                     Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_Spotted) ||
                     Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_BullhornArrest) ||
                     Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_Bullhorn)) &&
                    mInFormation) {
                    if (high) {
                        float t2 = (WorldTimer - mT_siren[2]).GetSeconds();
                        if (mSirenState != Sound::SIREN_SCREAM && t2 > pl.SirenScreamPeriod()) {
                            mSirenState = Sound::SIREN_SCREAM;
                            mT_siren[2] = WorldTimer;
                        } else if (t2 > pl.SirenMaxScreamTime()) {
                            mSirenState = Sound::SIREN_YELP;
                            mT_siren[2] = WorldTimer;
                        }
                    } else {
                        mSirenState = Sound::SIREN_YELP;
                    }
                    mT_siren[1] = WorldTimer;
                } else {
                    float t1 = (WorldTimer - mT_siren[1]).GetSeconds();
                    if (t1 > pl.SirenWailPeriod() + pl.SirenMaxYelpTime()) {
                        mSirenState = Sound::SIREN_WAIL;
                        mT_siren[1] = WorldTimer;
                    } else if (t1 > pl.SirenWailPeriod()) {
                        mSirenState = Sound::SIREN_YELP;
                    }
                }
            }
        }

    } else {
        mSirenState = Sound::SIREN_OFF;
    }

    if (in_roadblock) {
        mSirenState = Sound::SIREN_WAIL;
    }
}
