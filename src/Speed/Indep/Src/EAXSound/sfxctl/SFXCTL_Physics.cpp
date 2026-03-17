#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

namespace {
extern EngRevDataPoint RevPat5[];
extern EngRevDataPoint RevPat6[];
extern EngRevDataPoint RevPat7[];
extern EngRevDataPoint RevPat8[];
extern EngRevDataPoint RevPat9[];
extern EngRevDataPoint RevPat10[];
extern EngRevDataPoint RevPat11[];
extern EngRevDataPoint RevPat12[];

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

enum ControlSource {
    CONTROL_NONE = 0,
    CONTROL_HUMAN = 1,
    CONTROL_AI = 2,
    CONTROL_NIS = 3,
    CONTROL_ONLINE = 4,
};

inline EAX_CarState *ReadStateCar(EAXCar *carOwner) {
    return *static_cast<EAX_CarState **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0x34));
}

inline float &ReadStateCurTime(EAXCar *carOwner) {
    return *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0x3C));
}

inline float &ReadCarPhysTRQRef(EAXCar *carOwner) {
    return *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0x60));
}

inline float &ReadCarPhysRPMRef(EAXCar *carOwner) {
    return *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0x64));
}

inline int &ReadCarIsAcceleratingRef(EAXCar *carOwner) {
    return *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0x68));
}

inline int &ReadCarCurGearRef(EAXCar *carOwner) {
    return *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0x6C));
}

inline float &ReadCarTrottleRef(EAXCar *carOwner) {
    return *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0x70));
}

inline int &ReadCarPovTypeRef(EAXCar *carOwner) {
    return *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0xB8));
}

inline char *ReadCarEngineInfo(EAXCar *carOwner) {
    return static_cast<char *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(carOwner)) + 0xC4));
}

inline Sound::Wheel *ReadCarWheels(EAX_CarState *car) {
    return static_cast<Sound::Wheel *>(static_cast<void *>(car->mWheel));
}
} // namespace

extern int GameFlowSndState[];
extern "C" void Average_Record(Average *avg, float value) asm("Record__7Averagef");
extern "C" void AverageBase_Recalculate(AverageBase *avg) asm("Recalculate__11AverageBase");
extern "C" float GetValueFromSpline(float value, bMatrix4 *curve);

SFXCTL_Physics::SFXCTL_Physics()
    : m_bBlownEngineStreamQueued(false) //
    , PhysicsRPM(0.0f) //
    , RedLineRPM(0.0f) //
    , fMinPhysRPM(0.0f) //
    , fMaxPhysRPM(1.0f) //
    , fRedLinePhysRPM(0.0f) //
    , PhysicsTRQ(0.0f) //
    , fMaxPhysTRQ(0.0f) //
    , m_fThrottle(0.0f) //
    , m_OldThrottle(0.0f) //
    , m_OldDesiredSpeed(0.0f) //
    , m_tHoldDecel(0.0f) //
    , IsAccelerating(false) //
    , t_Last_Deccel(0.0f) //
    , t_Last_Accel(0.0f) //
    , m_CurGear(static_cast< Gear >(0)) //
    , m_LastGear(static_cast< Gear >(0)) //
    , mRPMCurve(nullptr) //
    , mMsgRevEngine(nullptr) //
    , mMsgRevOff(nullptr) //
    , PattternPlay(false) //
    , PatternNumber(0) //
    , CarID(-1) //
    , bPlayerEngEnable(false) //
    , RevFramesRemaining(0) //
    , NISRevingEnabled(false) //
    , eCurNisRevingState(NIS_OFF) //
    , TimeIntoRev(0.0f) //
    , NumDataPoints(0) //
    , pRevData(nullptr) //
    , NISRPM(0.0f) //
    , NISTRQ(0.0f) {}

SndBase *SFXCTL_Physics::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Physics();
}

SFXCTL_Physics::~SFXCTL_Physics() {
    if (mMsgRevEngine) {
        Hermes::Handler::Destroy(mMsgRevEngine);
    }
    if (mMsgRevOff) {
        Hermes::Handler::Destroy(mMsgRevOff);
    }
}

SFXCTL_AIPhysics::~SFXCTL_AIPhysics() {}

SFXCTL_AIPhysics::SFXCTL_AIPhysics()
    : m_pShiftCtl(nullptr) //
    , Zero60Time(0.0f) //
    , m_fDeltaRPM(0.0f) {}

SndBase *SFXCTL_AIPhysics::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_AIPhysics();
}

SndBase::TypeInfo *SFXCTL_Physics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Physics::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Physics::InitSFX() {
    SFXCTL::InitSFX();
    m_tHoldDecel = 0.0f;
    PhysicsTRQ = 0.0f;
    PhysicsRPM = 0.0f;
    NISRPM = 0.0f;
    NISTRQ = 0.0f;
    IsAccelerating = false;
    SetDMIX_Input(11, 0);
}

void SFXCTL_Physics::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);

    m_LastGear = m_CurGear;
    EAXCar *carOwner = m_pEAXCar;
    EAX_CarState *car = ReadStateCar(carOwner);

    m_CurGear = static_cast< Gear >(static_cast<int>(car->mDriveline.mGear));
    m_OldThrottle = m_fThrottle;

    if (car->mControlSource == CONTROL_AI) {
        Average_Record(&m_fDeltaDesiredSpeed, car->mDesiredSpeed - m_OldDesiredSpeed);
        m_OldDesiredSpeed = car->mDesiredSpeed;
        AverageBase_Recalculate(&m_fDeltaDesiredSpeed);

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
            t_Last_Accel = ReadStateCurTime(carOwner);
        }
    } else if (IsAccelerating) {
        IsAccelerating = false;
        t_Last_Deccel = ReadStateCurTime(carOwner);
    }

    PhysicsTRQ = smooth(PhysicsTRQ, m_fThrottle, 100.0f);

    float rpm = car->mEngine.mRPMPct;
    if (m_pStateBase->m_eStateType == eMM_PLAYERCAR) {
        const Attrib::Gen::engineaudio *engineInfo =
            static_cast<const Attrib::Gen::engineaudio *>(static_cast<const void *>(ReadCarEngineInfo(carOwner)));
        const bMatrix4 *curve = static_cast<const bMatrix4 *>(
            static_cast<const void *>(engineInfo->GetAttributePointer(0x07E3C833, 0)));
        if (curve == nullptr) {
            curve = static_cast<const bMatrix4 *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(bMatrix4))));
        }
        mRPMCurve = const_cast<bMatrix4 *>(curve);
        if (curve != nullptr) {
            rpm = GetValueFromSpline(rpm, mRPMCurve);
        }
    }

    PhysicsRPM = rpm * 9000.0f + 1000.0f;

    INIS *nis = INIS::Get();
    if (nis != nullptr && nis->IsPlaying()) {
        if (nis->GetAnimScene() != nullptr) {
            float sceneTime = nis->GetAnimScene()->GetTimeElapsed();
            UpdateNIS(sceneTime, 0.0f);
            return;
        }
    } else if (GameFlowSndState[3] != 0 && m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        UpdateNIS(0.0f, t);
        return;
    }

    ReadCarPhysTRQRef(carOwner) = PhysicsTRQ;
    ReadCarPhysRPMRef(carOwner) = PhysicsRPM;
    ReadCarTrottleRef(carOwner) = m_fThrottle;
    ReadCarIsAcceleratingRef(carOwner) = static_cast<int>(static_cast<float>(*static_cast<int *>(static_cast<void *>(&IsAccelerating))) != 0.0f);
    ReadCarCurGearRef(carOwner) = static_cast< int >(m_CurGear);
}

void SFXCTL_Physics::UpdateMixerOutputs() {
    int *outputs = GetOutputBlockPtr();
    EAXCar *carOwner = m_pEAXCar;
    EAX_CarState *pCar = carOwner != nullptr ? ReadStateCar(carOwner) : nullptr;

    float fVelY = pCar->mVel0.y;
    float fVelX = pCar->mVel0.x;
    float fVelZ = pCar->mVel0.z;
    float fVelLenSq = fVelZ * fVelZ + fVelX * fVelX + fVelY * fVelY;
    float fVelLen = 0.0f;
    if (fVelLenSq > 0.0f) {
        fVelLen = bSqrt(fVelLenSq);
    }

    int iOut = static_cast< int >(bAbs(fVelLen * 2.23699f) * 1092.2334f);
    int iClamp = 0;
    if (iOut > 0) {
        iClamp = iOut;
    }
    if (iClamp > 0x7FFF) {
        iClamp = 0x7FFF;
    }
    outputs[0] = iClamp;

    pCar = carOwner != nullptr ? ReadStateCar(carOwner) : nullptr;
    fVelY = pCar->mVel0.y;
    fVelX = pCar->mVel0.x;
    fVelZ = pCar->mVel0.z;
    fVelLenSq = fVelZ * fVelZ + fVelX * fVelX + fVelY * fVelY;
    fVelLen = 0.0f;
    if (fVelLenSq > 0.0f) {
        fVelLen = bSqrt(fVelLenSq);
    }

    iOut = static_cast< int >(bAbs(fVelLen * 2.23699f) * 546.1167f);
    iClamp = 0;
    if (iOut > 0) {
        iClamp = iOut;
    }
    if (iClamp > 0x7FFF) {
        iClamp = 0x7FFF;
    }
    outputs[1] = iClamp;

    pCar = carOwner != nullptr ? ReadStateCar(carOwner) : nullptr;
    fVelY = pCar->mVel0.y;
    fVelX = pCar->mVel0.x;
    fVelZ = pCar->mVel0.z;
    fVelLenSq = fVelZ * fVelZ + fVelX * fVelX + fVelY * fVelY;
    fVelLen = 0.0f;
    if (fVelLenSq > 0.0f) {
        fVelLen = bSqrt(fVelLenSq);
    }

    iOut = static_cast< int >(bAbs(fVelLen * 2.23699f) * 327.66998f);
    iClamp = 0;
    if (iOut > 0) {
        iClamp = iOut;
    }
    if (iClamp > 0x7FFF) {
        iClamp = 0x7FFF;
    }
    outputs[2] = iClamp;

    float fMaxPhysRPM = 10000.0f;
    pCar = carOwner != nullptr ? ReadStateCar(carOwner) : nullptr;
    fVelY = pCar->mVel0.y;
    fVelX = pCar->mVel0.x;
    fVelZ = pCar->mVel0.z;
    fVelLenSq = fVelZ * fVelZ + fVelX * fVelX + fVelY * fVelY;
    fVelLen = 0.0f;
    if (fVelLenSq > 0.0f) {
        fVelLen = bSqrt(fVelLenSq);
    }

    iOut = static_cast< int >(bAbs(fVelLen * 2.23699f) * 234.05f);
    iClamp = 0;
    if (iOut > 0) {
        iClamp = iOut;
    }
    if (iClamp > 0x7FFF) {
        iClamp = 0x7FFF;
    }
    outputs[3] = iClamp;

    iOut = static_cast<int>(
        (fMaxPhysRPM - ReadCarPhysRPMRef(carOwner)) *
        3.6407778f);
    iClamp = 0;
    if (iOut > 0) {
        iClamp = iOut;
    }
    if (iClamp > 0x7FFF) {
        iClamp = 0x7FFF;
    }
    outputs[4] = iClamp;

    int iAccelOutput = 0;
    if (*static_cast<int *>(static_cast<void *>(&IsAccelerating)) != 0) {
        iAccelOutput = 0x7FFF;
    }
    outputs[10] = iAccelOutput;

    unsigned int wheelsOnGround = 0;
    int wheelIndex = 0;
    Sound::Wheel *wheels = ReadCarWheels(pCar);
    do {
        if (wheels[wheelIndex].mWheelOnGround != 0) {
            wheelsOnGround += 1;
        }
        wheelIndex += 1;
    } while (wheelIndex < 4);
    outputs[5] = static_cast< int >(static_cast<float>(wheelsOnGround) * 8191.75f);

    int pov = ReadCarPovTypeRef(carOwner);
    if (pov == 3) {
        pov = 0x7FFF;
    } else {
        if (pov < 4) {
            if (pov == 1) {
                pov = 4000;
                goto L_POV_DONE;
            }
            if (pov > 1) {
                pov = 0x7FFF;
                goto L_POV_DONE;
            }
        } else if ((pov == 5) || (pov < 5) || (pov == 6)) {
            pov = 0x7FFF;
            goto L_POV_DONE;
        }
        pov = 0;
    }

L_POV_DONE:
    pov = smooth(outputs[6], pov, 0x3FFF);
    outputs[6] = pov;
    outputs[8] = 0;
    outputs[7] = 0;
}

void SFXCTL_Physics::MsgRevEngine(const MAIEngineRev &message) {
    (void)message;
    eCurNisRevingState = NIS_OFF;
    if (m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        PattternPlay = true;
        PatternNumber = 5;
    }
}

void SFXCTL_Physics::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pEAXCar->m_pPhysicsCTL = this;
}

SndBase::TypeInfo *SFXCTL_AIPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_AIPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

int SFXCTL_AIPhysics::GetController(int Index) {
    if (Index == 0) {
        return 2;
    }
    return -1;
}

void SFXCTL_AIPhysics::Destroy() {}

void SFXCTL_AIPhysics::UpdateMixerOutputs() {}

void SFXCTL_AIPhysics::SetupSFX(CSTATE_Base *_StateBase) {
    SFXCTL_Physics::SetupSFX(_StateBase);
}

void SFXCTL_AIPhysics::InitSFX() {
    SFXCTL_Physics::InitSFX();
    AIStateManager.Initialize(static_cast<SFXCTL_Physics *>(this));
    Zero60Time = 4.5f;
}

void SFXCTL_AIPhysics::AttachController(SFXCTL *psfxctl) {
    TypeInfo *pInfo = psfxctl->GetTypeInfo();
    if (((pInfo->ObjectID >> 4) & 0xFFF) == 2) {
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
    }
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

void SFXCTL_AIPhysics::UpdateParams(float t) {
    bool IsRacing;

    SFXCTL::UpdateParams(t);
    AIStateManager.Update(t);
    IsCornering = AIStateManager.GetState() == SND_AI_STATE_CORNER_LEFT || AIStateManager.GetState() == SND_AI_STATE_CORNER_RIGHT;
    Average_Record(&m_fDeltaDesiredSpeed, GetPhysCar()->GetDriver()->GetThrottle() * 100.0f);
    AverageBase_Recalculate(&m_fDeltaDesiredSpeed);
    m_OldThrottle = m_fThrottle;
    m_fDeltaRPM = GenDeltaRPM();
    UpdateAccel(t);
    m_fThrottle = static_cast<float>(*static_cast<int *>(static_cast<void *>(&IsAccelerating))) * 100.0f;
    UpdateRPM(t);
    UpdateTorque(t);
    m_LastGear = m_CurGear;
    UpdateGear();

    m_LastGear = m_CurGear;
    UpdateGear();

    if (INIS::Get() != nullptr && INIS::Get()->IsPlaying()) {
        if (INIS::Get()->GetAnimScene() != nullptr) {
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
    float prev = m_fDeltaRPM;
    m_fDeltaRPM = PhysicsRPM - prev;
    return m_fDeltaRPM;
}

void SFXCTL_AIPhysics::UpdateRPM(float t) {
    (void)t;
    PhysicsRPM = smooth(PhysicsRPM, m_fThrottle, 10.0f);
}

void SFXCTL_AIPhysics::UpdateAccel(float t) {
    (void)t;
    SteadyVelocityFactor = bClamp(bAbs(AIStateManager.AccelMonitor.AvgMonitor.GetValue()) * 5.5555553f, 0.1f, 1.0f);
    if (m_pShiftCtl->IsActive()) {
        return;
    }
    if (GetPhysCar()->GetNitroFlag()) {
        IsAccelerating = true;
        return;
    }
    IsAccelerating = AIStateManager.GetState() == SND_AI_STATE_ACCEL;
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

SndBase::TypeInfo *SFXCTL_TruckPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_TruckPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_TruckPhysics::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_TruckPhysics();
}

void SFXCTL_Physics::UpdateNIS(float TotalTime, float deltaTime) {
    float timeLeft;
    IPlayer *player;
    IHud *hud;
    ICountdown *icountdown;

    if (eCurNisRevingState == NIS_OFF) {
        if (*static_cast<int *>(static_cast<void *>(&PattternPlay)) != 0) {
            EngRevDataPoint *patternData;
            int patternLength;

            pRevData = nullptr;
            *static_cast<int *>(static_cast<void *>(&PattternPlay)) = 0;
            if (PatternNumber == 8) {
                patternLength = 0x43;
                patternData = RevPat8;
            } else if (PatternNumber < 9) {
                if (PatternNumber == 6) {
                    patternLength = 0x16;
                    patternData = RevPat6;
                } else if (PatternNumber < 7) {
                    patternLength = 0x1B;
                    patternData = RevPat5;
                } else {
                    patternLength = 0x13;
                    patternData = RevPat7;
                }
            } else if (PatternNumber == 10) {
                patternLength = 0x22;
                patternData = RevPat10;
            } else if (PatternNumber < 10) {
                patternLength = 0x38;
                patternData = RevPat9;
            } else if (PatternNumber == 11) {
                patternLength = 0x1E;
                patternData = RevPat11;
            } else {
                if (PatternNumber != 12) {
                    patternLength = 0x1B;
                    patternData = RevPat5;
                } else {
                    patternLength = 0x1D;
                    patternData = RevPat12;
                }
            }

            NumDataPoints = patternLength;
            pRevData = patternData;
            if (pRevData != nullptr) {
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
            if (*static_cast<int *>(static_cast<void *>(&g_pNISRevMgr->IsInitialized)) == 0) {
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

    if (eCurNisRevingState != NIS_PATTERN_ON) {
        if (eCurNisRevingState == NIS_MERGE_WITH_PHYSICS) {
            NISTRQ = 0.0f;
            if (m_pEAXCar->GetPhysRPM() < PhysicsRPM) {
                NISTRQ = 100.0f;
            }
            NISRPM = smooth(m_pEAXCar->GetPhysRPM(), PhysicsRPM, 500.0f);
        } else {
            NISTRQ = NISTRQ - 15.0f;
            NISRPM = NISRPM - 500.0f;
        }
        goto ClampAndStore;
    }

    if (TotalTime < 0.001f) {
        TimeIntoRev = TimeIntoRev + deltaTime;
    } else {
        TimeIntoRev = TotalTime;
    }

    if (TimeIntoRev <= pRevData[1].time) {
InterpolatePattern:
        if (eCurNisRevingState != NIS_OFF) {
            Slope RPMSlope(static_cast<float>(pRevData->RPM), static_cast<float>(pRevData[1].RPM), pRevData->time, pRevData[1].time);
            Slope TRQSlope(static_cast<float>(pRevData->Trq), static_cast<float>(pRevData[1].Trq), pRevData->time, pRevData[1].time);
            NISRPM = RPMSlope.GetValue(TimeIntoRev);
            NISTRQ = TRQSlope.GetValue(TimeIntoRev);
        }
    } else if (eCurNisRevingState != NIS_OFF) {
        do {
            NumDataPoints = NumDataPoints - 1;
            if (NumDataPoints == 0) {
                eCurNisRevingState = NIS_OFF;
            } else {
                pRevData = pRevData + 1;
            }
        } while (pRevData[1].time < TimeIntoRev && eCurNisRevingState != NIS_OFF);
        goto InterpolatePattern;
    }

    timeLeft = -1.0f;
    player = IPlayer::First(PLAYER_LOCAL);
    if (player != nullptr) {
        hud = player->GetHud();
        if (hud != nullptr) {
            icountdown = nullptr;
            if (hud->QueryInterface(&icountdown)) {
                timeLeft = icountdown->GetSecondsBeforeRaceStart();
            }
        }
    }

    if (timeLeft < 1.0f && 0.0f < timeLeft) {
        eCurNisRevingState = NIS_MERGE_WITH_PHYSICS;
    }

ClampAndStore:
    {
        float clampedTRQ = bClamp(NISTRQ, 0.0f, 100.0f);
        float clampedRPM = bClamp(NISRPM, 1000.0f, 10000.0f);

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
