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

    if (eCurNisRevingState == NIS_OFF) {
        if (PattternPlay) {
            int patternLength;

            PattternPlay = false;
            pRevData = nullptr;
            if (PatternNumber == 8) {
                goto L_Pattern8;
            }
            if (PatternNumber > 8) {
                goto L_PatternGreaterThan8;
            }
            if (PatternNumber == 6) {
                goto L_Pattern6;
            }
            if (PatternNumber > 6) {
                goto L_Pattern7;
            }
            goto L_Pattern5;
L_PatternGreaterThan8:
            if (PatternNumber == 10) {
                goto L_Pattern10;
            }
            if (PatternNumber < 10) {
                goto L_Pattern9;
            }
            if (PatternNumber == 11) {
                goto L_Pattern11;
            }
            if (PatternNumber == 12) {
                goto L_Pattern12;
            }
L_Pattern5:
            patternLength = 0x1B;
            pRevData = RevPat5;
            goto L_PatternDone;
L_Pattern6:
            patternLength = 0x16;
            pRevData = RevPat6;
            goto L_PatternDone;
L_Pattern7:
            patternLength = 0x13;
            pRevData = RevPat7;
            goto L_PatternDone;
L_Pattern8:
            patternLength = 0x43;
            pRevData = RevPat8;
            goto L_PatternDone;
L_Pattern9:
            patternLength = 0x38;
            pRevData = RevPat9;
            goto L_PatternDone;
L_Pattern10:
            patternLength = 0x22;
            pRevData = RevPat10;
            goto L_PatternDone;
L_Pattern11:
            patternLength = 0x1E;
            pRevData = RevPat11;
            goto L_PatternDone;
L_Pattern12:
            patternLength = 0x1D;
            pRevData = RevPat12;

L_PatternDone:
            NumDataPoints = patternLength;
            if (pRevData) {
                eCurNisRevingState = NIS_PATTERN_ON;
                Slope RPMSlope(static_cast<float>(pRevData->RPM), static_cast<float>(pRevData[1].RPM), pRevData->time, pRevData[1].time);
                Slope TRQSlope(static_cast<float>(pRevData->Trq), static_cast<float>(pRevData[1].Trq), pRevData->time, pRevData[1].time);
                NISRPM = RPMSlope.GetValue(TimeIntoRev);
                NISTRQ = TRQSlope.GetValue(TimeIntoRev);
            }
        } else {
            eCurNisRevingState = NIS_OFF;
            TimeIntoRev = TotalTime;
            CarID = m_pStateBase->GetPhysCar()->GetNISCarID();
            if (CarID < 0) {
                return;
            }
            if (!g_pNISRevMgr->IsInitialized) {
                return;
            }
            if (g_pNISRevMgr->m_EngineDataSet[CarID].NumPoints < 2) {
                return;
            }
            eCurNisRevingState = NIS_PATTERN_ON;
            NumDataPoints = g_pNISRevMgr->m_EngineDataSet[CarID].NumPoints - 1;
            pRevData = g_pNISRevMgr->m_EngineDataSet[CarID].DataPoints;
        }
    }

    switch (eCurNisRevingState) {
    case NIS_PATTERN_ON:
        if (TotalTime < 0.001f) {
            TimeIntoRev = TimeIntoRev + deltaTime;
        } else {
            TimeIntoRev = TotalTime;
        }

        if (TimeIntoRev > pRevData[1].time) {
            while (eCurNisRevingState != NIS_OFF) {
                NumDataPoints = NumDataPoints - 1;
                if (NumDataPoints == 0) {
                    eCurNisRevingState = NIS_OFF;
                } else {
                    pRevData = pRevData + 1;
                }
                if (TimeIntoRev <= pRevData[1].time) {
                    break;
                }
            }
        }

        if (eCurNisRevingState != NIS_OFF) {
            Slope RPMSlope(static_cast<float>(pRevData->RPM), static_cast<float>(pRevData[1].RPM), pRevData->time, pRevData[1].time);
            Slope TRQSlope(static_cast<float>(pRevData->Trq), static_cast<float>(pRevData[1].Trq), pRevData->time, pRevData[1].time);
            NISRPM = RPMSlope.GetValue(TimeIntoRev);
            NISTRQ = TRQSlope.GetValue(TimeIntoRev);
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
            eCurNisRevingState = NIS_MERGE_WITH_PHYSICS;
        }
        break;
    case NIS_MERGE_WITH_PHYSICS:
        NISTRQ = m_pEAXCar->GetPhysRPM() < PhysicsRPM ? 100.0f : 0.0f;
        NISRPM = smooth(m_pEAXCar->GetPhysRPM(), PhysicsRPM, 500.0f);
        goto ClampAndStore;
    default:
        NISRPM = NISRPM - 500.0f;
        NISTRQ = NISTRQ - 15.0f;
        goto ClampAndStore;
    }

ClampAndStore:
    {
        float clampedRPM = bClamp(NISRPM, 1000.0f, 10000.0f);
        float clampedTRQ = bClamp(NISTRQ, 0.0f, 100.0f);

        PhysicsRPM = clampedRPM;
        IsAccelerating = clampedTRQ > 30.0f;
        m_fThrottle = clampedTRQ;
        NISRPM = clampedRPM;
        NISTRQ = clampedTRQ;
        PhysicsTRQ = clampedTRQ;
        m_pEAXCar->SetPhysTRQ(clampedTRQ);
        m_pEAXCar->SetPhysRPM(PhysicsRPM);
        m_pEAXCar->SetIsAccelerating(static_cast<float>(IsAccelerating));
        m_pEAXCar->SetCurGear(static_cast<Sound::Gear>(m_CurGear));
        m_pEAXCar->SetThrottle(m_fThrottle);
    }
}

void SFXCTL_Physics::MsgRevEngine(const MAIEngineRev &message) {
    (void)message;
    eCurNisRevingState = NIS_OFF;
    if (m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        PattternPlay = true;
        PatternNumber = bRandom(7) + 5;
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
    mMsgRevEngine =
        Hermes::Handler::Create<MAIEngineRev, SFXCTL_Physics, SFXCTL_Physics>(this, &SFXCTL_Physics::MsgRevEngine, UCrc32("QRev"), 0);
    mMsgRevOff =
        Hermes::Handler::Create<MAIEngineRev, SFXCTL_Physics, SFXCTL_Physics>(this, &SFXCTL_Physics::MsgRevOff, UCrc32("RevOFF"), 0);

    m_fDeltaDesiredSpeed.Flush(0.0f);

    eCurNisRevingState = NIS_OFF;
    fMaxPhysRPM = 1.0f;
    NISRPM = 0.0f;
    PatternNumber = 0;
    TimeIntoRev = 0.0f;
    CarID = -1;
    m_OldThrottle = 0.0f;
    m_fThrottle = 0.0f;
    IsAccelerating = false;
    PhysicsRPM = 0.0f;
    PhysicsTRQ = 0.0f;
    fMaxPhysTRQ = 0.0f;
    m_CurGear = static_cast<Gear>(Sound::NEUTRAL);
    m_LastGear = static_cast<Gear>(Sound::NEUTRAL);
    bPlayerEngEnable = false;
    NISRevingEnabled = false;
    RevFramesRemaining = 0;
    NISTRQ = 0.0f;
    mRPMCurve = nullptr;
    PattternPlay = false;
}

SFXCTL_Physics::~SFXCTL_Physics() {
    if (mMsgRevEngine) {
        Hermes::Handler::Destroy(mMsgRevEngine);
    }
    if (mMsgRevOff) {
        Hermes::Handler::Destroy(mMsgRevOff);
    }
}

void SFXCTL_Physics::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);

    m_LastGear = m_CurGear;
    EAX_CarState *car = GetPhysCar();

    m_CurGear = static_cast<Gear>(static_cast<int>(car->mDriveline.mGear));
    m_OldThrottle = m_fThrottle;

    if (car->mControlSource == Sound::CONTROL_AI) {
        m_fDeltaDesiredSpeed.Record(car->mDesiredSpeed - m_OldDesiredSpeed);
        m_OldDesiredSpeed = car->mDesiredSpeed;
        m_fDeltaDesiredSpeed.Recalculate();

        if (m_fDeltaDesiredSpeed.GetValue() > -1.0f && m_tHoldDecel < 0.0f) {
            m_fThrottle = smooth(m_fThrottle, 100.0f, 50.0f);
        } else {
            if (m_fDeltaDesiredSpeed.GetValue() < 0.0f) {
                m_tHoldDecel = 0.5f;
            }
            m_tHoldDecel -= t;
            m_fThrottle = smooth(m_fThrottle, 0.0f, 50.0f);
        }
    } else {
        m_fThrottle = car->mEngine.mThrottle * 100.0f;
    }

    if (m_fThrottle > 30.0f) {
        if (!IsAccelerating) {
            IsAccelerating = true;
            t_Last_Accel = ReadStateCurTime(m_pEAXCar);
        }
    } else if (IsAccelerating) {
        IsAccelerating = false;
        t_Last_Deccel = ReadStateCurTime(m_pEAXCar);
    }

    PhysicsTRQ = smooth(PhysicsTRQ, m_fThrottle, 100.0f);

    float rpm = car->mEngine.mRPMPct;
    if (m_pStateBase->m_eStateType == eMM_PLAYERCAR) {
        const Attrib::Gen::engineaudio *engineInfo = &m_pEAXCar->mEngineInfo;
        const bMatrix4 *curve = static_cast<const bMatrix4 *>(
            static_cast<const void *>(engineInfo->GetAttributePointer(0x07E3C833, 0)));
        if (!curve) {
            curve = static_cast<const bMatrix4 *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(bMatrix4))));
        }
        mRPMCurve = const_cast<bMatrix4 *>(curve);
        if (curve) {
            rpm = GetValueFromSpline(rpm, mRPMCurve);
        }
    }

    PhysicsRPM = rpm * 9000.0f + 1000.0f;

    INIS *nis = INIS::Get();
    if (nis && nis->IsPlaying()) {
        if (nis->GetAnimScene()) {
            float sceneTime = nis->GetAnimScene()->GetTimeElapsed();
            UpdateNIS(sceneTime, 0.0f);
            return;
        }
    } else if (GameFlowSndState[3] != 0 && m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        UpdateNIS(0.0f, t);
        return;
    }

    m_pEAXCar->SetPhysTRQ(PhysicsTRQ);
    m_pEAXCar->SetPhysRPM(PhysicsRPM);
    m_pEAXCar->SetIsAccelerating(static_cast<float>(IsAccelerating));
    m_pEAXCar->SetCurGear(static_cast<Sound::Gear>(m_CurGear));
    m_pEAXCar->SetThrottle(m_fThrottle);
}

void SFXCTL_Physics::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pEAXCar->SetPhysicsCTLPtr(this);
}

void SFXCTL_Physics::InitSFX() {
    SFXCTL::InitSFX();
    EAX_CarState *car = GetPhysCar();
    fMaxPhysTRQ = car->GetMaxEngineTorque();
    fMaxPhysRPM = car->GetMaxRPM();
    fMinPhysRPM = car->GetIdleRPM();
    fRedLinePhysRPM = car->GetRedlineRPM();
    m_tHoldDecel = 0.0f;
    RedLineRPM = (fRedLinePhysRPM / fMaxPhysRPM) * 10000.0f;
    if (car->GetAttributes()->TruckSndFX()) {
        SetDMIX_Input(11, 0x7fff);
    }
}

void SFXCTL_Physics::UpdateMixerOutputs() {
    int TargeVal;

    TargeVal = bClamp(static_cast<int>(bAbs(GetPhysCar()->GetVelocityMagnitudeMPH()) * 1092.2334f), 0, 0x7FFF);
    SetDMIX_Input(0, TargeVal);
    TargeVal = bClamp(static_cast<int>(bAbs(GetPhysCar()->GetVelocityMagnitudeMPH()) * 546.1167f), 0, 0x7FFF);
    SetDMIX_Input(1, TargeVal);
    TargeVal = bClamp(static_cast<int>(bAbs(GetPhysCar()->GetVelocityMagnitudeMPH()) * 327.66998f), 0, 0x7FFF);
    SetDMIX_Input(2, TargeVal);
    TargeVal = bClamp(static_cast<int>(bAbs(GetPhysCar()->GetVelocityMagnitudeMPH()) * 234.05f), 0, 0x7FFF);
    SetDMIX_Input(3, TargeVal);
    TargeVal = bClamp(static_cast<int>((10000.0f - m_pEAXCar->GetPhysRPM()) * 3.6407778f), 0, 0x7FFF);
    SetDMIX_Input(4, TargeVal);

    TargeVal = 0;
    if (IsAccelerating) {
        TargeVal = 0x7FFF;
    }
    SetDMIX_Input(10, TargeVal);
    SetDMIX_Input(5, static_cast<int>(static_cast<float>(GetPhysCar()->GetWheelsOnGround()) * 8191.75f));

    TargeVal = m_pEAXCar->GetPOV();
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
    TargeVal = smooth(GetDMIX_InputValue(6), TargeVal, 0x3FFF);
    SetDMIX_Input(6, TargeVal);
    SetDMIX_Input(8, 0);
    SetDMIX_Input(7, 0);
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
    m_LastGear = Sound::FIRST_GEAR;
    SteadyVelocityFactor = 0.0f;
    IsCornering = false;
    m_CurGear = Sound::FIRST_GEAR;
    m_pShiftCtl = nullptr;
    m_DeltaRPM_LFO_Offset = 0.0f;
    m_AngleDeltaRPM_LFO = 0;
    DownShiftSameGearCount = 0;
    UpShiftSameGearCount = 0;
    IsDrifting = false;
}

SFXCTL_AIPhysics::~SFXCTL_AIPhysics() {}

void SFXCTL_AIPhysics::SetupSFX(CSTATE_Base *_StateBase) {
    SFXCTL_Physics::SetupSFX(_StateBase);
}

void SFXCTL_AIPhysics::InitSFX() {
    SFXCTL_Physics::InitSFX();
    AIStateManager.Initialize(static_cast<SFXCTL_Physics *>(this));
    Zero60Time = GetPhysCar()->GetZero60Time();
}

void SFXCTL_AIPhysics::UpdateParams(float t) {
    bool IsRacing;

    SFXCTL::UpdateParams(t);
    AIStateManager.Update(t);
    IsCornering = AIStateManager.GetState() == SND_AI_STATE_CORNER_LEFT || AIStateManager.GetState() == SND_AI_STATE_CORNER_RIGHT;
    m_fDeltaDesiredSpeed.Record(GetPhysCar()->GetDriver()->GetThrottle() * 100.0f);
    m_fDeltaDesiredSpeed.Recalculate();
    m_OldThrottle = m_fThrottle;
    m_fDeltaRPM = GenDeltaRPM();
    UpdateAccel(t);
    m_fThrottle = static_cast<float>(static_cast<int>(IsAccelerating)) * 100.0f;
    UpdateRPM(t);
    UpdateTorque(t);
    m_LastGear = m_CurGear;
    UpdateGear();

    m_LastGear = m_CurGear;
    UpdateGear();

    if (INIS::Get() && INIS::Get()->IsPlaying()) {
        if (INIS::Get()->GetAnimScene()) {
            UpdateNIS(INIS::Get()->GetAnimScene()->GetTimeElapsed(), 0.0f);
            return;
        }
    } else if (GameFlowSndState[3] != 0) {
        UpdateNIS(0.0f, t);
        return;
    }

    m_pEAXCar->SetPhysTRQ(PhysicsTRQ);
    m_pEAXCar->SetPhysRPM(PhysicsRPM);
    m_pEAXCar->SetIsAccelerating(static_cast<float>(IsAccelerating));
    m_pEAXCar->SetCurGear(m_CurGear);
    m_pEAXCar->SetThrottle(m_fThrottle);
    GetPhysCar()->SetVisualRPM(m_pEAXCar->GetFinalAudioRPM());
}

float SFXCTL_AIPhysics::GenDeltaRPM() {
    float RPM_LengthScale;

    if (TheRaceParameters.IsDriftRace()) {
        RPM_LengthScale = SND_AI_DRIFT_RPM_Lengths_FINE[m_CurGear] * 3.0f;
        if (!IsAccelerating) {
            RPM_LengthScale *= 1.5f;
        }
    } else if (TheRaceParameters.IsShortTrackRace()) {
        RPM_LengthScale = SND_AI_SHORT_TRACK_RPM_Lengths_FINE[m_CurGear] * 4.0f;
        if (!IsAccelerating) {
            RPM_LengthScale *= 1.5f;
        }
    } else {
        RPM_LengthScale = SND_AI_RPM_Lengths_FINE[m_CurGear] * 3.02f;
        if (!IsAccelerating) {
            RPM_LengthScale *= 2.5f;
        }
    }

    if (m_CurGear < Sound::FOURTH_GEAR) {
        unsigned int angle = static_cast<unsigned int>(m_AngleDeltaRPM_LFO + static_cast<int>(SndBase::m_fDeltaTime * 43689.99609375f));
        float DeltaRPM_LFO_Offset;

        angle = static_cast<unsigned short>(angle);
        angle = angle % 0xFFFF;
        m_AngleDeltaRPM_LFO = static_cast<unsigned short>(angle);
        DeltaRPM_LFO_Offset = bSin(m_AngleDeltaRPM_LFO);
        RPM_LengthScale *= SteadyVelocityFactor;
        m_DeltaRPM_LFO_Offset = DeltaRPM_LFO_Offset * 15.0f;
    }
    float DeltaRPM = 33.333336f;

    DeltaRPM *= RPM_LengthScale;
    return DeltaRPM * SndBase::m_fDeltaTime;
}

void SFXCTL_AIPhysics::UpdateRPM(float t) {
    (void)t;
    if (!m_pShiftCtl->IsActive()) {
        if (IsAccelerating) {
            if (AIStateManager.AccelMonitor.AvgMonitor.GetValue() < 1.0f) {
                if (m_pEAXCar->GetVelocityMagnitudeMPH() < 5.0f) {
                    PhysicsRPM = smooth(PhysicsRPM, 9000.0f, m_fDeltaRPM);
                    return;
                }
            }
            PhysicsRPM = smooth(PhysicsRPM, 10000.0f, m_fDeltaRPM);
        } else if (m_pEAXCar->GetVelocityMagnitudeMPH() < 3.0f) {
            PhysicsRPM = smooth(PhysicsRPM, 1000.0f, 50.0f);
        } else {
            PhysicsRPM = smooth(PhysicsRPM, 8000.0f, m_fDeltaRPM);
        }
    } else if (m_pShiftCtl->IsDownShifting()) {
        PhysicsRPM = 5000.0f;
    } else if (fMaxPhysRPM != 0.0f) {
        float ratio = 3000.0f / fMaxPhysRPM;
        float rpm = ratio * 10000.0f;

        rpm += 100.0f;
        rpm += TargetRPMOffset;
        PhysicsRPM = rpm;
    }
}

void SFXCTL_AIPhysics::UpdateAccel(float t) {
    (void)t;
    SteadyVelocityFactor =
        bClamp(bAbs(static_cast<const Average &>(GetAIStateManager()->AccelMonitor.AvgMonitor).GetValue()) * 5.5555553f,
               0.1f, 1.0f);
    if (m_pShiftCtl->IsActive()) {
        return;
    }
    if (GetPhysCar()->GetNitroFlag()) {
        IsAccelerating = true;
        return;
    }
    IsAccelerating = AIStateManager.GetState() == SND_AI_STATE_ACCEL;
}

void SFXCTL_AIPhysics::UpdateTorque(float t) {
    float target;
    if (IsAccelerating) {
        target = 100.0f;
    } else {
        target = 0.0f;
    }
    PhysicsTRQ = smooth(PhysicsTRQ, target, 50.0f);
}

Gear SFXCTL_AIPhysics::SuggestGear() {
    float PercentOfMaxSpeed = GetPhysCar()->GetVelocityMagnitude();
    int GearNumber;
    int Result = Sound::FIRST_GEAR;
    float Spread = 1.22f;

    PercentOfMaxSpeed /= GetPhysCar()->GetTheoreticalTopSpeed();
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
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
    }
}

void SFXCTL_AIPhysics::UpdateGear() {
    Gear SuggestedGear;
    int GearDiff;

    if (GetPhysCar()->GetGear() == Sound::REVERSE) {
        m_CurGear = Sound::FIRST_GEAR;
    } else {
        SuggestedGear = SuggestGear();
        GearDiff = (m_CurGear - SuggestedGear) + 1;
        if (static_cast<unsigned int>(GearDiff) > 2u) {
            m_LastGear = SuggestGear();
            m_CurGear = m_LastGear;
        }
        if (GetPhysRPM() > 9300.0f) {
            SuggestedGear = SuggestGear();
            if (SuggestedGear < Sound::SECOND_GEAR) {
                SuggestedGear = Sound::SECOND_GEAR;
            }
            if (SuggestedGear > Sound::SIXTH_GEAR) {
                SuggestedGear = Sound::SIXTH_GEAR;
            }
            m_CurGear = SuggestedGear;
            TargetRPMOffset = g_pEAXSound->Random(700.0f);
            SuggestedGear = static_cast<Gear>(m_CurGear + Sound::AUTOMATIC);
            UpShiftSameGearCount = 0;
        } else {
            if (GetPhysRPM() >= SND_AI_DOWNSHIFT_RPMS[m_CurGear]) {
                return;
            }
            if (m_CurGear < Sound::SECOND_GEAR) {
                return;
            }
            m_CurGear = SuggestGear();
            if (m_CurGear == m_LastGear) {
                DownShiftSameGearCount++;
            } else {
                DownShiftSameGearCount = 0;
            }
            if (m_CurGear == Sound::FIRST_GEAR && m_pEAXCar->GetVelocityMagnitudeMPH() > 10.0f) {
                m_CurGear = Sound::SECOND_GEAR;
            }
            SuggestedGear = static_cast<Gear>(m_CurGear + Sound::NEUTRAL);
        }
        m_LastGear = SuggestedGear;
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
