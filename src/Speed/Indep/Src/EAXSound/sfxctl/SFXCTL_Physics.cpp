#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

static const float DesiredVelDifThresh = -1.0f;
static const float HoldDecelDuration = 0.5f;
static const float SND_AI_RPM_Lengths_COARSE = 3.02f;
static const float SND_AI_DELTARPM_CRUISE_FACTOR = 2.5f;
static const float SND_AI_SHORT_TRACK_RPM_Lengths_COARSE = 4.0f;
static const float SND_AI_SHORT_TRACK_DELTARPM_CRUISE_FACTOR = 1.5f;
static const float SND_AI_DRIFT_RPM_Lengths_COARSE = 3.0f;
static const float SND_AI_DRIFT_DELTARPM_CRUISE_FACTOR = 1.5f;
static const float TIME_TO_WAIT_AFTER_SHIFT_T = 2.0f;
static const float SND_AI_PHYSICS_DRIFT_THRESHOLD = 2.0f;
static const float DeltaRPM_LFO_Amp = 15.0f;
static const float SND_AI_GearSpread = 1.22f;
static const float TwkTargeRPMOffset = 700.0f;

static const float SND_AI_DOWNSHIFT_RPMS[] = {
    6000.0f,
    6000.0f,
    5700.0f,
    6200.0f,
    6650.0f,
    7000.0f,
    7300.0f,
    7400.0f,
    7500.0f,
};

static const float SND_AI_RPM_Lengths_FINE[] = {
    40.0f,
    40.0f,
    40.5f,
    21.0f,
    12.0f,
    6.5f,
    3.6f,
    TIME_TO_WAIT_AFTER_SHIFT_T,
    1.0f,
};

static const float SND_AI_SHORT_TRACK_RPM_Lengths_FINE[] = {
    40.0f,
    40.0f,
    30.5f,
    12.5f,
    6.0f,
    3.5f,
    2.6f,
    SND_AI_SHORT_TRACK_DELTARPM_CRUISE_FACTOR,
    1.0f,
};

static const float SND_AI_DRIFT_RPM_Lengths_FINE[] = {
    40.0f,
    40.0f,
    40.5f,
    21.0f,
    12.0f,
    7.5f,
    3.6f,
    TIME_TO_WAIT_AFTER_SHIFT_T,
    1.0f,
};

inline float ReadStateCurTime(EAXCar *carOwner) {
    return carOwner->GetCurTime();
}
extern int GameFlowSndState[];
extern float GetValueFromSpline(float value, bMatrix4 *curve);

SndBase::TypeInfo SFXCTL_Physics::s_TypeInfo = {(int)0x00020000, "SFXCTL_Physics", &SFXCTL::s_TypeInfo, SFXCTL_Physics::CreateObject};

SndBase::TypeInfo *SFXCTL_Physics::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_Physics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Physics::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Physics::s_TypeInfo.typeName, false) SFXCTL_Physics();
    }
    return new (SFXCTL_Physics::s_TypeInfo.typeName, true) SFXCTL_Physics();
}

SFXCTL_Physics::SFXCTL_Physics()
    : m_fDeltaDesiredSpeed(2),
      mMsgRevEngine(Hermes::Handler::Create<MAIEngineRev, SFXCTL_Physics, SFXCTL_Physics>(this, &SFXCTL_Physics::MsgRevEngine,
                                                                                         UCrc32("QRev"), 0)),
      mMsgRevOff(Hermes::Handler::Create<MAIEngineRev, SFXCTL_Physics, SFXCTL_Physics>(this, &SFXCTL_Physics::MsgRevOff,
                                                                                      UCrc32("RevOFF"), 0)) {

    this->m_fDeltaDesiredSpeed.Flush(0.0f);

    this->m_OldThrottle = 0.0f;
    this->m_fThrottle = 0.0f;
    this->IsAccelerating = false;
    this->PhysicsRPM = 1000.0f;
    this->PhysicsTRQ = 0.0f;
    this->fMaxPhysTRQ = 0.0f;
    this->m_CurGear = static_cast<Gear>(NEUTRAL);
    this->m_LastGear = static_cast<Gear>(NEUTRAL);
    this->bPlayerEngEnable = false;
    this->NISRevingEnabled = false;
    this->RevFramesRemaining = 0;
    this->NISTRQ = 0.0f;
    this->mRPMCurve = nullptr;
    this->PattternPlay = false;
    this->eCurNisRevingState = NIS_OFF;
    this->fMaxPhysRPM = 10000.0f;
    this->NISRPM = 1000.0f;
    this->PatternNumber = 0;
    this->TimeIntoRev = 0.0f;
    this->CarID = -1;
}

SFXCTL_Physics::~SFXCTL_Physics() {
    if (this->mMsgRevEngine) {
        Hermes::Handler::Destroy(this->mMsgRevEngine);
    }
    if (this->mMsgRevOff) {
        Hermes::Handler::Destroy(this->mMsgRevOff);
    }
}

void SFXCTL_Physics::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);

    this->m_LastGear = this->m_CurGear;

    this->m_CurGear = static_cast<Gear>(static_cast<int>(this->GetPhysCar()->mDriveline.mGear));
    this->m_OldThrottle = this->m_fThrottle;

    if (this->GetPhysCar()->mControlSource == Sound::CONTROL_AI) {
        this->m_fDeltaDesiredSpeed.Record(this->GetPhysCar()->mDesiredSpeed - this->m_OldDesiredSpeed);
        this->m_OldDesiredSpeed = this->GetPhysCar()->mDesiredSpeed;
        this->m_fDeltaDesiredSpeed.Recalculate();

        if (this->m_fDeltaDesiredSpeed.GetValue() > DesiredVelDifThresh && this->m_tHoldDecel < 0.0f) {
            this->m_fThrottle = smooth(this->m_fThrottle, 100.0f, 50.0f);
        } else {
            if (this->m_fDeltaDesiredSpeed.GetValue() < 0.0f) {
                this->m_tHoldDecel = HoldDecelDuration;
            }
            this->m_tHoldDecel -= t;
            this->m_fThrottle = smooth(this->m_fThrottle, 0.0f, 50.0f);
        }
    } else {
        this->m_fThrottle = this->GetPhysCar()->mEngine.mThrottle * 100.0f;
    }

    if (this->m_fThrottle > 30.0f) {
        if (!this->IsAccelerating) {
            this->IsAccelerating = true;
            this->t_Last_Accel = ReadStateCurTime(this->m_pEAXCar);
        }
    } else if (this->IsAccelerating) {
        this->IsAccelerating = false;
        this->t_Last_Deccel = ReadStateCurTime(this->m_pEAXCar);
    }

    this->PhysicsTRQ = smooth(this->PhysicsTRQ, this->m_fThrottle, 100.0f);

    float physrpm = this->GetPhysCar()->GetRPMPct();
    float remapped_rpm = physrpm;

    if (this->m_pStateBase->m_eStateType == eMM_PLAYERCAR) {
        this->mRPMCurve = const_cast<bMatrix4 *>(
            reinterpret_cast<const bMatrix4 *>(&this->m_pEAXCar->GetAttributes().PhysicsRPM_Map()));
        if (this->mRPMCurve) {
            remapped_rpm = GetValueFromSpline(physrpm, this->mRPMCurve);
        }
    }

    this->PhysicsRPM = remapped_rpm * 9000.0f + 1000.0f;

    if (INIS::Get() && INIS::Get()->IsPlaying()) {
        if (INIS::Get()->GetAnimScene()) {
            this->UpdateNIS(INIS::Get()->GetAnimScene()->GetTimeElapsed(), 0.0f);
            return;
        }
    } else if (GameFlowSndState[3] != 0 && this->m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        this->UpdateNIS(0.0f, t);
        return;
    }

    this->m_pEAXCar->SetPhysTRQ(this->PhysicsTRQ);
    this->m_pEAXCar->SetPhysRPM(this->PhysicsRPM);
    this->m_pEAXCar->SetIsAccelerating(static_cast<float>(this->IsAccelerating));
    this->m_pEAXCar->SetCurGear(static_cast<Gear>(this->m_CurGear));
    this->m_pEAXCar->SetThrottle(this->m_fThrottle);
}

void SFXCTL_Physics::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_pEAXCar->SetPhysicsCTLPtr(this);
}

void SFXCTL_Physics::InitSFX() {
    SFXCTL::InitSFX();
    this->fMaxPhysTRQ = this->GetPhysCar()->GetMaxEngineTorque();
    this->fMaxPhysRPM = this->GetPhysCar()->GetMaxRPM();
    this->fMinPhysRPM = this->GetPhysCar()->GetIdleRPM();
    this->fRedLinePhysRPM = this->GetPhysCar()->GetRedlineRPM();
    this->m_tHoldDecel = 0.0f;
    this->RedLineRPM = (this->fRedLinePhysRPM / this->fMaxPhysRPM) * 10000.0f;
    if (this->GetPhysCar()->GetAttributes()->TruckSndFX()) {
        this->SetDMIX_Input(11, 0x7fff);
    }
}

void SFXCTL_Physics::UpdateMixerOutputs() {
    int TargeVal;

    TargeVal = bClamp(static_cast<int>(bAbs(this->GetPhysCar()->GetVelocityMagnitudeMPH()) * 1092.2334f), 0, 0x7FFF);
    this->SetDMIX_Input(0, TargeVal);
    TargeVal = bClamp(static_cast<int>(bAbs(this->GetPhysCar()->GetVelocityMagnitudeMPH()) * 546.1167f), 0, 0x7FFF);
    this->SetDMIX_Input(1, TargeVal);
    TargeVal = bClamp(static_cast<int>(bAbs(this->GetPhysCar()->GetVelocityMagnitudeMPH()) * 327.66998f), 0, 0x7FFF);
    this->SetDMIX_Input(2, TargeVal);
    TargeVal = bClamp(static_cast<int>(bAbs(this->GetPhysCar()->GetVelocityMagnitudeMPH()) * 234.05f), 0, 0x7FFF);
    this->SetDMIX_Input(3, TargeVal);
    TargeVal = bClamp(static_cast<int>((10000.0f - this->m_pEAXCar->GetPhysRPM()) * 3.6407778f), 0, 0x7FFF);
    this->SetDMIX_Input(4, TargeVal);

    this->SetDMIX_Input(10, this->IsAccelerating ? 0x7FFF : 0);
    this->SetDMIX_Input(5, static_cast<int>(static_cast<float>(this->GetPhysCar()->GetWheelsOnGround()) * 32767.0f));

    switch (this->m_pEAXCar->GetPOV()) {
    case 0:
    default:
        TargeVal = 0;
        break;
    case 1:
        TargeVal = 4000;
        break;
    case 2:
        TargeVal = 0x7FFF;
        break;
    case 3:
        TargeVal = 0x7FFF;
        break;
    case 4:
        TargeVal = 0x7FFF;
        break;
    case 5:
        TargeVal = 0x7FFF;
        break;
    case 6:
        TargeVal = 0x7FFF;
        break;
    }

    TargeVal = smooth(this->GetDMIX_InputValue(6), TargeVal, 0x3FFF);
    this->SetDMIX_Input(6, TargeVal);
    this->SetDMIX_Input(8, 0);
    this->SetDMIX_Input(7, 0);
}

SndBase::TypeInfo SFXCTL_AIPhysics::s_TypeInfo = {(int)0x00030000, "SFXCTL_AIPhysics", &SFXCTL_Physics::s_TypeInfo, SFXCTL_AIPhysics::CreateObject};

SndBase::TypeInfo *SFXCTL_AIPhysics::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_AIPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_AIPhysics::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_AIPhysics::s_TypeInfo.typeName, false) SFXCTL_AIPhysics();
    }
    return new (SFXCTL_AIPhysics::s_TypeInfo.typeName, true) SFXCTL_AIPhysics();
}

SFXCTL_AIPhysics::SFXCTL_AIPhysics() {
    this->m_CurGear = FIRST_GEAR;
    this->m_pShiftCtl = nullptr;
    this->m_DeltaRPM_LFO_Offset = 0.0f;
    this->m_LastGear = FIRST_GEAR;
    this->m_AngleDeltaRPM_LFO = 0;
    this->SteadyVelocityFactor = 0.0f;
    this->DownShiftSameGearCount = 0;
    this->UpShiftSameGearCount = 0;
    this->IsDrifting = false;
    this->IsCornering = false;
}

SFXCTL_AIPhysics::~SFXCTL_AIPhysics() {}

void SFXCTL_AIPhysics::SetupSFX(CSTATE_Base *_StateBase) {
    SFXCTL_Physics::SetupSFX(_StateBase);
}

void SFXCTL_AIPhysics::InitSFX() {
    SFXCTL_Physics::InitSFX();
    this->AIStateManager.Initialize(static_cast<SFXCTL_Physics *>(this));
    this->Zero60Time = this->GetPhysCar()->GetZero60Time();
}

void SFXCTL_AIPhysics::UpdateParams(float t) {
    bool IsRacing;

    SFXCTL::UpdateParams(t);
    this->AIStateManager.Update(t);
    this->IsCornering = this->AIStateManager.GetState() == SND_AI_STATE_CORNER_LEFT || this->AIStateManager.GetState() == SND_AI_STATE_CORNER_RIGHT;
    this->m_fDeltaDesiredSpeed.Record(this->GetPhysCar()->GetDriver()->GetThrottle() * 100.0f);
    this->m_fDeltaDesiredSpeed.Recalculate();
    this->m_OldThrottle = this->m_fThrottle;
    this->m_fDeltaRPM = this->GenDeltaRPM();
    this->UpdateAccel(t);
    this->m_fThrottle = static_cast<float>(static_cast<int>(this->IsAccelerating)) * 100.0f;
    this->UpdateRPM(t);
    this->UpdateTorque(t);
    this->m_LastGear = this->m_CurGear;
    this->UpdateGear();

    this->m_LastGear = this->m_CurGear;
    this->UpdateGear();

    if (INIS::Get() && INIS::Get()->IsPlaying()) {
        if (INIS::Get()->GetAnimScene()) {
            this->UpdateNIS(INIS::Get()->GetAnimScene()->GetTimeElapsed(), 0.0f);
            return;
        }
    } else if (GameFlowSndState[3] != 0) {
        this->UpdateNIS(0.0f, t);
        return;
    }

    this->m_pEAXCar->SetPhysTRQ(this->PhysicsTRQ);
    this->m_pEAXCar->SetPhysRPM(this->PhysicsRPM);
    this->m_pEAXCar->SetIsAccelerating(static_cast<float>(this->IsAccelerating));
    this->m_pEAXCar->SetCurGear(this->m_CurGear);
    this->m_pEAXCar->SetThrottle(this->m_fThrottle);
    this->GetPhysCar()->SetVisualRPM(this->m_pEAXCar->GetFinalAudioRPM());
}

float SFXCTL_AIPhysics::GenDeltaRPM() {
    float RPM_LengthScale;

    if (TheRaceParameters.IsDriftRace()) {
        RPM_LengthScale = SND_AI_DRIFT_RPM_Lengths_FINE[this->m_CurGear] * SND_AI_DRIFT_RPM_Lengths_COARSE;
        if (!this->IsAccelerating) {
            RPM_LengthScale *= SND_AI_SHORT_TRACK_DELTARPM_CRUISE_FACTOR;
        }
    } else if (TheRaceParameters.IsShortTrackRace()) {
        RPM_LengthScale = SND_AI_SHORT_TRACK_RPM_Lengths_FINE[this->m_CurGear] * SND_AI_SHORT_TRACK_RPM_Lengths_COARSE;
        if (!this->IsAccelerating) {
            RPM_LengthScale *= SND_AI_SHORT_TRACK_DELTARPM_CRUISE_FACTOR;
        }
    } else {
        RPM_LengthScale = SND_AI_RPM_Lengths_FINE[this->m_CurGear] * SND_AI_RPM_Lengths_COARSE;
        if (!this->IsAccelerating) {
            RPM_LengthScale *= SND_AI_DELTARPM_CRUISE_FACTOR;
        }
    }

    if (this->m_CurGear < FOURTH_GEAR) {
        unsigned int angle = static_cast<unsigned int>(this->m_AngleDeltaRPM_LFO + static_cast<int>(SndBase::m_fDeltaTime * 43689.99609375f));
        float DeltaRPM_LFO_Offset;

        angle = static_cast<unsigned short>(angle);
        angle = angle % 0xFFFF;
        this->m_AngleDeltaRPM_LFO = static_cast<unsigned short>(angle);
        DeltaRPM_LFO_Offset = bSin(this->m_AngleDeltaRPM_LFO);
        RPM_LengthScale *= this->SteadyVelocityFactor;
        this->m_DeltaRPM_LFO_Offset = DeltaRPM_LFO_Offset * DeltaRPM_LFO_Amp;
    }
    float DeltaRPM = 33.333336f;

    DeltaRPM *= RPM_LengthScale;
    return DeltaRPM * SndBase::m_fDeltaTime;
}

void SFXCTL_AIPhysics::UpdateRPM(float t) {
    (void)t;
    if (!this->m_pShiftCtl->IsActive()) {
        if (this->IsAccelerating) {
            if (this->AIStateManager.AccelMonitor.AvgMonitor.GetValue() < 1.0f) {
                if (this->m_pEAXCar->GetVelocityMagnitudeMPH() < 5.0f) {
                    this->PhysicsRPM = smooth(this->PhysicsRPM, 9000.0f, this->m_fDeltaRPM);
                    return;
                }
            }
            this->PhysicsRPM = smooth(this->PhysicsRPM, 10000.0f, this->m_fDeltaRPM);
        } else if (this->m_pEAXCar->GetVelocityMagnitudeMPH() < SND_AI_DRIFT_RPM_Lengths_COARSE) {
            this->PhysicsRPM = smooth(this->PhysicsRPM, 1000.0f, 50.0f);
        } else {
            this->PhysicsRPM = smooth(this->PhysicsRPM, 4000.0f, this->m_fDeltaRPM);
        }
    } else if (this->m_pShiftCtl->IsDownShifting()) {
        this->PhysicsRPM = 8000.0f;
    } else if (this->fMaxPhysRPM != 0.0f) {
        float ratio = 3000.0f / this->fMaxPhysRPM;

        this->PhysicsRPM = ratio * 10000.0f;
        this->PhysicsRPM += 100.0f;
        this->PhysicsRPM += this->TargetRPMOffset;
    }
}

void SFXCTL_AIPhysics::UpdateAccel(float t) {
    (void)t;
    this->SteadyVelocityFactor =
        bClamp(bAbs(this->GetAIStateManager()->AccelMonitor.AvgMonitor.GetValue()) * 5.5555553f,
               0.1f, 1.0f);
    if (this->m_pShiftCtl->IsActive()) {
        return;
    }
    if (this->GetPhysCar()->GetNitroFlag()) {
        this->IsAccelerating = true;
        return;
    }
    this->IsAccelerating = this->AIStateManager.GetState() == SND_AI_STATE_ACCEL;
}

void SFXCTL_AIPhysics::UpdateTorque(float t) {
    if (this->IsAccelerating) {
        this->PhysicsTRQ = smooth(this->PhysicsTRQ, 100.0f, 50.0f);
    } else {
        this->PhysicsTRQ = smooth(this->PhysicsTRQ, 0.0f, 50.0f);
    }
}

Gear SFXCTL_AIPhysics::SuggestGear() {
    float PercentOfMaxSpeed = this->GetPhysCar()->GetVelocityMagnitude();
    int GearNumber;
    int Result = FIRST_GEAR;
    float Spread = SND_AI_GearSpread;

    PercentOfMaxSpeed /= this->GetPhysCar()->GetTheoreticalTopSpeed();
    GearNumber = static_cast<int>(PercentOfMaxSpeed * Spread * 8.0f);
    Result = bClamp(GearNumber, FIRST_GEAR, SEVENTH_GEAR);
    return static_cast<Gear>(Result);
}

int SFXCTL_AIPhysics::GetController(int Index) {
    if (Index == 0) {
        return 2;
    }
    return -1;
}

void SFXCTL_AIPhysics::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 2) {
        this->m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
    }
}

void SFXCTL_AIPhysics::UpdateGear() {
    Gear SuggestedGear;
    int GearDiff;

    if (this->GetPhysCar()->GetGear() == REVERSE) {
        this->m_CurGear = FIRST_GEAR;
    } else {
        SuggestedGear = this->SuggestGear();
        GearDiff = (this->m_CurGear - SuggestedGear) + 1;
        if (static_cast<unsigned int>(GearDiff) > 2u) {
            this->m_CurGear = this->SuggestGear();
            this->m_LastGear = this->m_CurGear;
        }
        if (this->GetPhysRPM() > 9300.0f) {
            SuggestedGear = static_cast<Gear>(bClamp(this->SuggestGear(), SECOND_GEAR, SIXTH_GEAR));
            this->m_CurGear = SuggestedGear;
            this->TargetRPMOffset = g_pEAXSound->Random(TwkTargeRPMOffset);
            SuggestedGear = static_cast<Gear>(this->m_CurGear + AUTOMATIC);
            this->UpShiftSameGearCount = 0;
        } else {
            if (this->GetPhysRPM() >= SND_AI_DOWNSHIFT_RPMS[this->m_CurGear]) {
                return;
            }
            if (this->m_CurGear < SECOND_GEAR) {
                return;
            }
            this->m_CurGear = this->SuggestGear();
            if (this->m_CurGear == this->m_LastGear) {
                this->DownShiftSameGearCount++;
            } else {
                this->DownShiftSameGearCount = 0;
            }
            if (this->m_CurGear == FIRST_GEAR && this->m_pEAXCar->GetVelocityMagnitudeMPH() > 10.0f) {
                this->m_CurGear = SECOND_GEAR;
            }
            SuggestedGear = static_cast<Gear>(this->m_CurGear + NEUTRAL);
        }
        this->m_LastGear = SuggestedGear;
    }
}

void SFXCTL_AIPhysics::Destroy() {}

SndBase::TypeInfo SFXCTL_TruckPhysics::s_TypeInfo = {(int)0x000C0000, "SFXCTL_TruckPhysics", &SFXCTL_AIPhysics::s_TypeInfo, SFXCTL_TruckPhysics::CreateObject};

SndBase::TypeInfo *SFXCTL_TruckPhysics::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_TruckPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_TruckPhysics::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_TruckPhysics::s_TypeInfo.typeName, false) SFXCTL_TruckPhysics();
    }
    return new (SFXCTL_TruckPhysics::s_TypeInfo.typeName, true) SFXCTL_TruckPhysics();
}
