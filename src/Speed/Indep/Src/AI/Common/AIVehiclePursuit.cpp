#include "Speed/Indep/Src/AI/AIVehiclePursuit.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

static const float Pursuit_AIUpdateRate = 0.125f; // Decl: 26
static const float Pursuit_AIStagger = 0.125f;    // Decl: 27
float AIVehiclePursuit::mStagger = 0.0f;          // Decl: 28

AIVehiclePursuit::AIVehiclePursuit(const BehaviorParams &bp)
    : AIVehiclePid(bp, Pursuit_AIUpdateRate, mStagger, Sim::TASK_FRAME_FIXED), //
      IPursuitAI(bp.fowner),                                                   //
      mInPursuit(false),                                                       //
      mBreaker(false),                                                         //
      mChicken(false),                                                         //
      mDamagedByPerp(false),                                                   //
      mSirenState(Sound::SIREN_OFF),                                           //
      mSirenInit(false),                                                       //
      mInFormation(false),                                                     //
      mInPosition(false),                                                      //
      mWithinEngagementRadius(false),                                          //
      mPursuitOffset(UMath::Vector3::kZero) {
    mStagger += Pursuit_AIStagger;
    if (mStagger >= 1.0f) {
        mStagger = 0.0f;
    }
    this->mVisibiltyTestTimer = 0.0f;
    this->mTimeSinceTargetSeen = 99.0f;
    this->mSupportGoal = (const char *)nullptr;
    this->mT_siren[0] = this->mT_siren[1] = this->mT_siren[2] = WorldTimer;
}

AIVehiclePursuit::~AIVehiclePursuit() {}

void AIVehiclePursuit::ResetInternals() {
    this->AIVehicle::ResetInternals();
    this->mInPursuit = false;
    this->mBreaker = false;
    this->mChicken = false;
    this->mDamagedByPerp = false;
    this->mInFormation = false;
    this->mInPosition = false;

    this->mPursuitOffset = UMath::Vector3::kZero;

    this->mTimeSinceTargetSeen = 99.0f;
    this->mVisibiltyTestTimer = 0.25f;

    this->mWithinEngagementRadius = false;

    this->mT_siren[0] = this->mT_siren[1] = this->mT_siren[2] = WorldTimer;
    this->mSirenState = Sound::SIREN_OFF;
    this->mSirenInit = false;
}

void AIVehiclePursuit::StartPatrol() {
    this->SetInPursuit(false);
    this->GetTarget()->Clear();
    this->SetGoal(UCrc32("AIGoalPatrol"));
}

void AIVehiclePursuit::StartFlee() {
    IVehicle *ivehicle;
    this->GetVehicle()->GlareOff(VehicleFX::LIGHT_COPS);

    UCrc32 goal("AIGoalFleePursuit");
    if (GetSimable()->QueryInterface(&ivehicle) && ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
        goal = "AIGoalHeliExit";
    }
    if (this->GetGoalName() != goal) {
        this->ClearGoal();
        this->SetGoal(goal);
    }
}

void AIVehiclePursuit::StartRoadBlock() {
    IVehicle *ivehicle;
    this->GetVehicle()->GlareOn(VehicleFX::LIGHT_COPS);
    this->SetInPursuit(true);
    this->GetTarget()->Clear();
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        this->SetGoal(UCrc32("AIGoalHeliRoadBlock"));
    } else {
        this->SetGoal(UCrc32("AIGoalStaticRoadBlock"));
    }
}

void AIVehiclePursuit::StartPursuit(AITarget *target, ISimable *itargetSimable) {
    this->GetVehicle()->GlareOn(VehicleFX::LIGHT_COPS);
    if (target != nullptr) {
        this->GetTarget()->Aquire(target);
    } else if (itargetSimable != nullptr) {
        this->GetTarget()->Aquire(itargetSimable);
    }
    this->UpdateTargeting();
    this->SetInPursuit(true);
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        this->SetGoal(UCrc32("AIGoalHeliPursuit"));
    } else {
        this->SetGoal(UCrc32("AIGoalPursuit"));
    }
}

void AIVehiclePursuit::DoInPositionGoal() {
    this->SetGoal(this->mInPositionGoal);
}

void AIVehiclePursuit::EndPursuit() {
    this->SetInPursuit(false);
    this->GetVehicle()->GlareOff(VehicleFX::LIGHT_COPS);
}

bool AIVehiclePursuit::StartSupportGoal() {
    if (this->mSupportGoal != (const char *)nullptr) {
        this->SetGoal(this->mSupportGoal);
        return true;
    }
    return false;
}

void AIVehiclePursuit::SetSupportGoal(UCrc32 sg) {
    this->mSupportGoal = sg;
}

AITarget *AIVehiclePursuit::GetPursuitTarget() {
    if (this->GetTarget()->IsValid()) {
        return this->GetTarget();
    } else {
        return nullptr;
    }
}

AITarget *AIVehiclePursuit::PursuitRequest() {
    if (!this->GetInPursuit() && this->GetTarget()->IsValid()) {
        return this->GetTarget();
    } else {
        return nullptr;
    }
}

void AIVehiclePursuit::Update(float dT) {
    ProfileNode profile_node("TODO", 0);
    this->AIVehicle::Update(dT);
    this->UpdateSiren(dT);

    if (!this->mInPursuit || !this->GetTarget()->IsValid()) {
        this->mTimeSinceTargetSeen = 0.25f;
        this->mVisibiltyTestTimer = 0.25f;
        return;
    }

    this->mVisibiltyTestTimer += dT;
    this->mTimeSinceTargetSeen += dT;
    if (this->mVisibiltyTestTimer >= 0.25f) {
        this->mVisibiltyTestTimer -= 0.25f;
        if (this->CanSeeTarget(this->GetTarget())) {
            this->mTimeSinceTargetSeen = -0.25f;
        }
    }
}

void AIVehiclePursuit::UpdateSiren(float dT) {
    IPursuit *ipursuit = this->GetPursuit();
    SoundAI *soundai = SoundAI::Get();
    IVehicle *cop_car = this->GetVehicle();

    bool pursuitRace = false;
    bool is_dday = false;
    bool is_in_rb = false;
    bool doAI = true;

    if (GRaceStatus::Get().GetRaceParameters() != nullptr) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
        is_dday = GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace();
    }

    const IRoadBlock::List &blocks = IRoadBlock::GetList();
    for (IRoadBlock::List::const_iterator i = blocks.begin(); i != blocks.end(); ++i) {
        IRoadBlock *rb = *i;
        if (rb != nullptr) {
            IVehicle *vehicle = rb->IsComprisedOf(this->GetSimable()->GetOwnerHandle());
            if (this->GetVehicle() == vehicle) {
                is_in_rb = true;
                break;
            }
        }
    }

    if (soundai != nullptr && ipursuit != nullptr && this->mInPursuit && soundai->GetPursuitSpecs().IsValid()) {
        const Attrib::Gen::pursuitlevels &pursuitatr = soundai->GetPursuitSpecs();

        if (ipursuit->IsPlayerPursuit() && !is_dday && soundai->NumPursuits() < 2) {
            if (soundai->GetFocus() == SoundAI::kPursuitFlow && soundai->GetPursuitDuration() < 5.0f) {
                doAI = false;

                float dt = (WorldTimer - this->mT_siren[0]).GetSeconds();
                static float variation = pursuitatr.SirenInitMinPeriod() + bRandom(pursuitatr.SirenInitVariation());

                if (dt > variation && this->mSirenState == Sound::SIREN_OFF) {
                    this->mT_siren[1] = WorldTimer;
                    this->mSirenState = Sound::SIREN_YELP;
                }

                if (this->mSirenState == Sound::SIREN_YELP) {
                    float t_bleeping = (WorldTimer - this->mT_siren[1]).GetSeconds();

                    if (t_bleeping > 0.5f) {
                        this->mSirenState = Sound::SIREN_OFF;
                        variation = pursuitatr.SirenInitMinPeriod() + bRandom(pursuitatr.SirenInitVariation());
                        this->mT_siren[0] = WorldTimer;
                    }
                }
            } else if (soundai->GetFocus() == SoundAI::kLost) {
                this->mSirenState = Sound::SIREN_OFF;
                doAI = false;
                this->mT_siren[0] = WorldTimer;
            }
        }

        if (!cop_car->IsGlareOn(VehicleFX::LIGHT_COPS)) {
            this->mSirenState = cop_car->IsDestroyed() ? (this->mSirenState = Sound::SIREN_DIE) : Sound::SIREN_OFF;
        }

        if (doAI) {
            float t_start = (WorldTimer - this->mT_siren[0]).GetSeconds();
            bool high_heat = soundai->IsHighIntensity();
            bool siren_initializing = t_start < 6.0f;

            if (!siren_initializing && this->mSirenInit) {
                this->mT_siren[1] = WorldTimer;

                if (high_heat) {
                    this->mSirenState = Sound::SIREN_YELP;
                } else {
                    this->mSirenState = Sound::SIREN_WAIL;
                }
            }

            this->mSirenInit = siren_initializing;

            if (siren_initializing) {
                if (!high_heat) {
                    this->mSirenState = Sound::SIREN_YELP;
                } else {
                    if (t_start < 3.0f) {
                        this->mSirenState = Sound::SIREN_SCREAM;
                    } else {
                        this->mSirenState = Sound::SIREN_YELP;
                    }
                }
            } else {
                if ((ipursuit->IsCollapseActive() || ipursuit->IsFinisherActive() ||
                     Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_ReInitPursuit) ||
                     Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_Spotted) ||
                     Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_BullhornArrest) ||
                     Speech::Manager::IsCopSpeechPlaying(kSPCH1_EventID_Bullhorn)) &&
                    this->mInFormation) {
                    if (high_heat) {
                        float t_screaming = (WorldTimer - this->mT_siren[2]).GetSeconds();

                        if (this->mSirenState != Sound::SIREN_SCREAM && t_screaming > pursuitatr.SirenScreamPeriod()) {
                            this->mSirenState = Sound::SIREN_SCREAM;
                            this->mT_siren[2] = WorldTimer;
                        } else {
                            if (t_screaming > pursuitatr.SirenMaxScreamTime()) {
                                this->mSirenState = Sound::SIREN_YELP;
                                this->mT_siren[2] = WorldTimer;
                            }
                        }
                    } else {
                        this->mSirenState = Sound::SIREN_YELP;
                    }

                    this->mT_siren[1] = WorldTimer;
                } else {
                    float t_constant = (WorldTimer - this->mT_siren[1]).GetSeconds();

                    if (t_constant > pursuitatr.SirenWailPeriod() + pursuitatr.SirenMaxYelpTime()) {
                        this->mSirenState = Sound::SIREN_WAIL;
                        this->mT_siren[1] = WorldTimer;
                    } else if (t_constant > pursuitatr.SirenWailPeriod()) {
                        this->mSirenState = Sound::SIREN_YELP;
                    }
                }
            }
        }
    } else {
        this->mSirenState = Sound::SIREN_OFF;
    }

    if (is_in_rb) {
        this->mSirenState = Sound::SIREN_WAIL;
    }
}
