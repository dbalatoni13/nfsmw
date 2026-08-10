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

namespace {

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
    2.0f,
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
    1.5f,
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
    2.0f,
    1.0f,
};

inline float ReadStateCurTime(EAXCar *carOwner) {
    return carOwner->GetCurTime();
}
} // namespace

extern int GameFlowSndState[];
extern float GetValueFromSpline(float value, bMatrix4 *curve);

void SFXCTL_Physics::UpdateNIS(float TotalTime, float deltaTime) {
    float timeLeft;
    IPlayer *player;
    IHud *hud;
    ICountdown *icountdown;

    if (this->eCurNisRevingState == NIS_OFF) {
        if (this->PattternPlay) {
            int patternLength;

            this->PattternPlay = false;
            this->pRevData = nullptr;
            if (this->PatternNumber == 8) {
                goto L_Pattern8;
            }
            if (this->PatternNumber > 8) {
                goto L_PatternGreaterThan8;
            }
            if (this->PatternNumber == 6) {
                goto L_Pattern6;
            }
            if (this->PatternNumber > 6) {
                goto L_Pattern7;
            }
            goto L_Pattern5;
L_PatternGreaterThan8:
            if (this->PatternNumber == 10) {
                goto L_Pattern10;
            }
            if (this->PatternNumber < 10) {
                goto L_Pattern9;
            }
            if (this->PatternNumber == 11) {
                goto L_Pattern11;
            }
            if (this->PatternNumber == 12) {
                goto L_Pattern12;
            }
L_Pattern5:
            patternLength = 0x1B;
            this->pRevData = RevPat5;
            goto L_PatternDone;
L_Pattern6:
            patternLength = 0x16;
            this->pRevData = RevPat6;
            goto L_PatternDone;
L_Pattern7:
            patternLength = 0x13;
            this->pRevData = RevPat7;
            goto L_PatternDone;
L_Pattern8:
            patternLength = 0x43;
            this->pRevData = RevPat8;
            goto L_PatternDone;
L_Pattern9:
            patternLength = 0x38;
            this->pRevData = RevPat9;
            goto L_PatternDone;
L_Pattern10:
            patternLength = 0x22;
            this->pRevData = RevPat10;
            goto L_PatternDone;
L_Pattern11:
            patternLength = 0x1E;
            this->pRevData = RevPat11;
            goto L_PatternDone;
L_Pattern12:
            patternLength = 0x1D;
            this->pRevData = RevPat12;

L_PatternDone:
            this->NumDataPoints = patternLength;
            if (this->pRevData) {
                this->eCurNisRevingState = NIS_PATTERN_ON;
                Slope RPMSlope(static_cast<float>(this->pRevData->RPM), static_cast<float>(this->pRevData[1].RPM), this->pRevData->time, this->pRevData[1].time);
                Slope TRQSlope(static_cast<float>(this->pRevData->Trq), static_cast<float>(this->pRevData[1].Trq), this->pRevData->time, this->pRevData[1].time);
                this->NISRPM = RPMSlope.GetValue(this->TimeIntoRev);
                this->NISTRQ = TRQSlope.GetValue(this->TimeIntoRev);
            }
        } else {
            this->eCurNisRevingState = NIS_OFF;
            this->TimeIntoRev = TotalTime;
            this->CarID = this->m_pStateBase->GetPhysCar()->GetNISCarID();
            if (this->CarID < 0) {
                return;
            }
            if (!g_pNISRevMgr->IsInitialized) {
                return;
            }
            if (g_pNISRevMgr->m_EngineDataSet[this->CarID].NumPoints < 2) {
                return;
            }
            this->eCurNisRevingState = NIS_PATTERN_ON;
            this->NumDataPoints = g_pNISRevMgr->m_EngineDataSet[this->CarID].NumPoints - 1;
            this->pRevData = g_pNISRevMgr->m_EngineDataSet[this->CarID].DataPoints;
        }
    }

    switch (this->eCurNisRevingState) {
    case NIS_PATTERN_ON:
        if (TotalTime < 0.001f) {
            this->TimeIntoRev = this->TimeIntoRev + deltaTime;
        } else {
            this->TimeIntoRev = TotalTime;
        }

        if (this->TimeIntoRev > this->pRevData[1].time) {
            while (this->eCurNisRevingState != NIS_OFF) {
                this->NumDataPoints = this->NumDataPoints - 1;
                if (this->NumDataPoints == 0) {
                    this->eCurNisRevingState = NIS_OFF;
                } else {
                    this->pRevData = this->pRevData + 1;
                }
                if (this->TimeIntoRev <= this->pRevData[1].time) {
                    break;
                }
            }
        }

        if (this->eCurNisRevingState != NIS_OFF) {
            Slope RPMSlope(static_cast<float>(this->pRevData->RPM), static_cast<float>(this->pRevData[1].RPM), this->pRevData->time, this->pRevData[1].time);
            Slope TRQSlope(static_cast<float>(this->pRevData->Trq), static_cast<float>(this->pRevData[1].Trq), this->pRevData->time, this->pRevData[1].time);
            this->NISRPM = RPMSlope.GetValue(this->TimeIntoRev);
            this->NISTRQ = TRQSlope.GetValue(this->TimeIntoRev);
        }

        timeLeft = -1.0f;
        player = IPlayer::First(PLAYER_LOCAL);
        if (player) {
            hud = player->GetHud();
            if (hud) {
                icountdown = nullptr;
                if (hud->QueryInterface(&icountdown)) {
                    timeLeft = icountdown->GetSecondsBeforeRaceStart();
                }
            }
        }

        if (timeLeft < 1.0f && 0.0f < timeLeft) {
            this->eCurNisRevingState = NIS_MERGE_WITH_PHYSICS;
        }
        break;
    case NIS_MERGE_WITH_PHYSICS:
        this->NISTRQ = this->m_pEAXCar->GetPhysRPM() < this->PhysicsRPM ? 100.0f : 0.0f;
        this->NISRPM = smooth(this->m_pEAXCar->GetPhysRPM(), this->PhysicsRPM, 500.0f);
        goto ClampAndStore;
    default:
        this->NISRPM = this->NISRPM - 500.0f;
        this->NISTRQ = this->NISTRQ - 15.0f;
        goto ClampAndStore;
    }

ClampAndStore:
    {
        float clampedRPM = bClamp(this->NISRPM, 1000.0f, 10000.0f);
        float clampedTRQ = bClamp(this->NISTRQ, 0.0f, 100.0f);

        this->PhysicsRPM = clampedRPM;
        this->IsAccelerating = clampedTRQ > 30.0f;
        this->m_fThrottle = clampedTRQ;
        this->NISRPM = clampedRPM;
        this->NISTRQ = clampedTRQ;
        this->PhysicsTRQ = clampedTRQ;
        this->m_pEAXCar->SetPhysTRQ(clampedTRQ);
        this->m_pEAXCar->SetPhysRPM(this->PhysicsRPM);
        this->m_pEAXCar->SetIsAccelerating(static_cast<float>(this->IsAccelerating));
        this->m_pEAXCar->SetCurGear(static_cast<Sound::Gear>(this->m_CurGear));
        this->m_pEAXCar->SetThrottle(this->m_fThrottle);
    }
}

void SFXCTL_Physics::MsgRevEngine(const MAIEngineRev &message) {
    (void)message;
    this->eCurNisRevingState = NIS_OFF;
    if (this->m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        this->PattternPlay = true;
        this->PatternNumber = bRandom(7) + 5;
    }
}

SndBase::TypeInfo *SFXCTL_Physics::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_Physics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Physics::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Physics::GetStaticTypeInfo()->typeName, false) SFXCTL_Physics();
    }
    return new (SFXCTL_Physics::GetStaticTypeInfo()->typeName, true) SFXCTL_Physics();
}

SFXCTL_Physics::SFXCTL_Physics()
    : m_fDeltaDesiredSpeed(2) {
    this->mMsgRevEngine =
        Hermes::Handler::Create<MAIEngineRev, SFXCTL_Physics, SFXCTL_Physics>(this, &SFXCTL_Physics::MsgRevEngine, UCrc32("QRev"), 0);
    this->mMsgRevOff =
        Hermes::Handler::Create<MAIEngineRev, SFXCTL_Physics, SFXCTL_Physics>(this, &SFXCTL_Physics::MsgRevOff, UCrc32("RevOFF"), 0);

    this->m_fDeltaDesiredSpeed.Flush(0.0f);

    this->eCurNisRevingState = NIS_OFF;
    this->fMaxPhysRPM = 1.0f;
    this->NISRPM = 0.0f;
    this->PatternNumber = 0;
    this->TimeIntoRev = 0.0f;
    this->CarID = -1;
    this->m_OldThrottle = 0.0f;
    this->m_fThrottle = 0.0f;
    this->IsAccelerating = false;
    this->PhysicsRPM = 0.0f;
    this->PhysicsTRQ = 0.0f;
    this->fMaxPhysTRQ = 0.0f;
    this->m_CurGear = static_cast<Gear>(Sound::NEUTRAL);
    this->m_LastGear = static_cast<Gear>(Sound::NEUTRAL);
    this->bPlayerEngEnable = false;
    this->NISRevingEnabled = false;
    this->RevFramesRemaining = 0;
    this->NISTRQ = 0.0f;
    this->mRPMCurve = nullptr;
    this->PattternPlay = false;
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
    EAX_CarState *car = this->GetPhysCar();

    this->m_CurGear = static_cast<Gear>(static_cast<int>(car->mDriveline.mGear));
    this->m_OldThrottle = this->m_fThrottle;

    if (car->mControlSource == Sound::CONTROL_AI) {
        this->m_fDeltaDesiredSpeed.Record(car->mDesiredSpeed - this->m_OldDesiredSpeed);
        this->m_OldDesiredSpeed = car->mDesiredSpeed;
        this->m_fDeltaDesiredSpeed.Recalculate();

        if (this->m_fDeltaDesiredSpeed.GetValue() > -1.0f && this->m_tHoldDecel < 0.0f) {
            this->m_fThrottle = smooth(this->m_fThrottle, 100.0f, 50.0f);
        } else {
            if (this->m_fDeltaDesiredSpeed.GetValue() < 0.0f) {
                this->m_tHoldDecel = 0.5f;
            }
            this->m_tHoldDecel -= t;
            this->m_fThrottle = smooth(this->m_fThrottle, 0.0f, 50.0f);
        }
    } else {
        this->m_fThrottle = car->mEngine.mThrottle * 100.0f;
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

    float rpm = car->mEngine.mRPMPct;
    if (this->m_pStateBase->m_eStateType == eMM_PLAYERCAR) {
        const Attrib::Gen::engineaudio *engineInfo = &this->m_pEAXCar->mEngineInfo;
        const bMatrix4 *curve = static_cast<const bMatrix4 *>(
            static_cast<const void *>(engineInfo->GetAttributePointer(0x07E3C833, 0)));
        if (!curve) {
            curve = static_cast<const bMatrix4 *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(bMatrix4))));
        }
        this->mRPMCurve = const_cast<bMatrix4 *>(curve);
        if (curve) {
            rpm = GetValueFromSpline(rpm, this->mRPMCurve);
        }
    }

    this->PhysicsRPM = rpm * 9000.0f + 1000.0f;

    INIS *nis = INIS::Get();
    if (nis && nis->IsPlaying()) {
        if (nis->GetAnimScene()) {
            float sceneTime = nis->GetAnimScene()->GetTimeElapsed();
            this->UpdateNIS(sceneTime, 0.0f);
            return;
        }
    } else if (GameFlowSndState[3] != 0 && this->m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        this->UpdateNIS(0.0f, t);
        return;
    }

    this->m_pEAXCar->SetPhysTRQ(this->PhysicsTRQ);
    this->m_pEAXCar->SetPhysRPM(this->PhysicsRPM);
    this->m_pEAXCar->SetIsAccelerating(static_cast<float>(this->IsAccelerating));
    this->m_pEAXCar->SetCurGear(static_cast<Sound::Gear>(this->m_CurGear));
    this->m_pEAXCar->SetThrottle(this->m_fThrottle);
}

void SFXCTL_Physics::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_pEAXCar->SetPhysicsCTLPtr(this);
}

void SFXCTL_Physics::InitSFX() {
    SFXCTL::InitSFX();
    EAX_CarState *car = this->GetPhysCar();
    this->fMaxPhysTRQ = car->GetMaxEngineTorque();
    this->fMaxPhysRPM = car->GetMaxRPM();
    this->fMinPhysRPM = car->GetIdleRPM();
    this->fRedLinePhysRPM = car->GetRedlineRPM();
    this->m_tHoldDecel = 0.0f;
    this->RedLineRPM = (this->fRedLinePhysRPM / this->fMaxPhysRPM) * 10000.0f;
    if (car->GetAttributes()->TruckSndFX()) {
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

    TargeVal = 0;
    if (this->IsAccelerating) {
        TargeVal = 0x7FFF;
    }
    this->SetDMIX_Input(10, TargeVal);
    this->SetDMIX_Input(5, static_cast<int>(static_cast<float>(this->GetPhysCar()->GetWheelsOnGround()) * 8191.75f));

    TargeVal = this->m_pEAXCar->GetPOV();
    if (TargeVal == 3) {
        TargeVal = 0x7FFF;
    } else {
        if (TargeVal < 4) {
            if (TargeVal == 1) {
                TargeVal = 4000;
                goto L_SET_POV;
            }
            if (TargeVal > 1) {
                TargeVal = 0x7FFF;
                goto L_SET_POV;
            }
        } else if ((TargeVal == 5) || (TargeVal < 5) || (TargeVal == 6)) {
            TargeVal = 0x7FFF;
            goto L_SET_POV;
        }
        TargeVal = 0;
    }

L_SET_POV:
    TargeVal = smooth(this->GetDMIX_InputValue(6), TargeVal, 0x3FFF);
    this->SetDMIX_Input(6, TargeVal);
    this->SetDMIX_Input(8, 0);
    this->SetDMIX_Input(7, 0);
}

SndBase::TypeInfo *SFXCTL_AIPhysics::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_AIPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_AIPhysics::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_AIPhysics::GetStaticTypeInfo()->typeName, false) SFXCTL_AIPhysics();
    }
    return new (SFXCTL_AIPhysics::GetStaticTypeInfo()->typeName, true) SFXCTL_AIPhysics();
}

SFXCTL_AIPhysics::SFXCTL_AIPhysics() {
    this->m_LastGear = Sound::FIRST_GEAR;
    this->SteadyVelocityFactor = 0.0f;
    this->IsCornering = false;
    this->m_CurGear = Sound::FIRST_GEAR;
    this->m_pShiftCtl = nullptr;
    this->m_DeltaRPM_LFO_Offset = 0.0f;
    this->m_AngleDeltaRPM_LFO = 0;
    this->DownShiftSameGearCount = 0;
    this->UpShiftSameGearCount = 0;
    this->IsDrifting = false;
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
        RPM_LengthScale = SND_AI_DRIFT_RPM_Lengths_FINE[this->m_CurGear] * 3.0f;
        if (!this->IsAccelerating) {
            RPM_LengthScale *= 1.5f;
        }
    } else if (TheRaceParameters.IsShortTrackRace()) {
        RPM_LengthScale = SND_AI_SHORT_TRACK_RPM_Lengths_FINE[this->m_CurGear] * 4.0f;
        if (!this->IsAccelerating) {
            RPM_LengthScale *= 1.5f;
        }
    } else {
        RPM_LengthScale = SND_AI_RPM_Lengths_FINE[this->m_CurGear] * 3.02f;
        if (!this->IsAccelerating) {
            RPM_LengthScale *= 2.5f;
        }
    }

    if (this->m_CurGear < Sound::FOURTH_GEAR) {
        unsigned int angle = static_cast<unsigned int>(this->m_AngleDeltaRPM_LFO + static_cast<int>(SndBase::m_fDeltaTime * 43689.99609375f));
        float DeltaRPM_LFO_Offset;

        angle = static_cast<unsigned short>(angle);
        angle = angle % 0xFFFF;
        this->m_AngleDeltaRPM_LFO = static_cast<unsigned short>(angle);
        DeltaRPM_LFO_Offset = bSin(this->m_AngleDeltaRPM_LFO);
        RPM_LengthScale *= this->SteadyVelocityFactor;
        this->m_DeltaRPM_LFO_Offset = DeltaRPM_LFO_Offset * 15.0f;
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
        } else if (this->m_pEAXCar->GetVelocityMagnitudeMPH() < 3.0f) {
            this->PhysicsRPM = smooth(this->PhysicsRPM, 1000.0f, 50.0f);
        } else {
            this->PhysicsRPM = smooth(this->PhysicsRPM, 8000.0f, this->m_fDeltaRPM);
        }
    } else if (this->m_pShiftCtl->IsDownShifting()) {
        this->PhysicsRPM = 5000.0f;
    } else if (this->fMaxPhysRPM != 0.0f) {
        float ratio = 3000.0f / this->fMaxPhysRPM;
        float rpm = ratio * 10000.0f;

        rpm += 100.0f;
        rpm += this->TargetRPMOffset;
        this->PhysicsRPM = rpm;
    }
}

void SFXCTL_AIPhysics::UpdateAccel(float t) {
    (void)t;
    this->SteadyVelocityFactor =
        bClamp(bAbs(static_cast<const Average &>(this->GetAIStateManager()->AccelMonitor.AvgMonitor).GetValue()) * 5.5555553f,
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
    float target;
    if (this->IsAccelerating) {
        target = 100.0f;
    } else {
        target = 0.0f;
    }
    this->PhysicsTRQ = smooth(this->PhysicsTRQ, target, 50.0f);
}

Gear SFXCTL_AIPhysics::SuggestGear() {
    float PercentOfMaxSpeed = this->GetPhysCar()->GetVelocityMagnitude();
    int GearNumber;
    int Result = Sound::FIRST_GEAR;
    float Spread = 1.22f;

    PercentOfMaxSpeed /= this->GetPhysCar()->GetTheoreticalTopSpeed();
    GearNumber = static_cast<int>(PercentOfMaxSpeed * Spread * 8.0f);
    if (GearNumber > Sound::FIRST_GEAR) {
        Result = GearNumber;
    }
    GearNumber = Sound::SEVENTH_GEAR;
    if (GearNumber < Result) {
        Result = GearNumber;
    }
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

    if (this->GetPhysCar()->GetGear() == Sound::REVERSE) {
        this->m_CurGear = Sound::FIRST_GEAR;
    } else {
        SuggestedGear = this->SuggestGear();
        GearDiff = (this->m_CurGear - SuggestedGear) + 1;
        if (static_cast<unsigned int>(GearDiff) > 2u) {
            this->m_LastGear = this->SuggestGear();
            this->m_CurGear = this->m_LastGear;
        }
        if (this->GetPhysRPM() > 9300.0f) {
            SuggestedGear = this->SuggestGear();
            if (SuggestedGear < Sound::SECOND_GEAR) {
                SuggestedGear = Sound::SECOND_GEAR;
            }
            if (SuggestedGear > Sound::SIXTH_GEAR) {
                SuggestedGear = Sound::SIXTH_GEAR;
            }
            this->m_CurGear = SuggestedGear;
            this->TargetRPMOffset = g_pEAXSound->Random(700.0f);
            SuggestedGear = static_cast<Gear>(this->m_CurGear + Sound::AUTOMATIC);
            this->UpShiftSameGearCount = 0;
        } else {
            if (this->GetPhysRPM() >= SND_AI_DOWNSHIFT_RPMS[this->m_CurGear]) {
                return;
            }
            if (this->m_CurGear < Sound::SECOND_GEAR) {
                return;
            }
            this->m_CurGear = this->SuggestGear();
            if (this->m_CurGear == this->m_LastGear) {
                this->DownShiftSameGearCount++;
            } else {
                this->DownShiftSameGearCount = 0;
            }
            if (this->m_CurGear == Sound::FIRST_GEAR && this->m_pEAXCar->GetVelocityMagnitudeMPH() > 10.0f) {
                this->m_CurGear = Sound::SECOND_GEAR;
            }
            SuggestedGear = static_cast<Gear>(this->m_CurGear + Sound::NEUTRAL);
        }
        this->m_LastGear = SuggestedGear;
    }
}

void SFXCTL_AIPhysics::Destroy() {}

SndBase::TypeInfo *SFXCTL_TruckPhysics::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_TruckPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_TruckPhysics::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_TruckPhysics::GetStaticTypeInfo()->typeName, false) SFXCTL_TruckPhysics();
    }
    return new (SFXCTL_TruckPhysics::GetStaticTypeInfo()->typeName, true) SFXCTL_TruckPhysics();
}
