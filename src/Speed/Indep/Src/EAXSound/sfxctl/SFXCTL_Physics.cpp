#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
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
    SFXCTL_Physics::UpdateParams(t);
    GenDeltaRPM();
    UpdateRPM(t);
    UpdateTorque(t);
    UpdateAccel(t);
    UpdateGear();
}

void SFXCTL_AIPhysics::GenDeltaRPM() {
    float prev = m_fDeltaRPM;
    m_fDeltaRPM = PhysicsRPM - prev;
}

void SFXCTL_AIPhysics::UpdateRPM(float t) {
    (void)t;
    PhysicsRPM = smooth(PhysicsRPM, m_fThrottle, 10.0f);
}

void SFXCTL_AIPhysics::UpdateAccel(float t) {
    (void)t;
    IsAccelerating = (m_fThrottle > 0.2f);
}

int SFXCTL_AIPhysics::SuggestGear() {
    return static_cast< int >(m_CurGear);
}

void SFXCTL_AIPhysics::UpdateGear() {
    m_LastGear = m_CurGear;
    m_CurGear = static_cast< Gear >(SuggestGear());
}

SndBase::TypeInfo *SFXCTL_TruckPhysics::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_TruckPhysics::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_TruckPhysics::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_TruckPhysics();
}

void SFXCTL_Physics::UpdateNIS(float TotalTime, float deltaTime) {
    NIS_ENGINE_REVING_STATE nisState = eCurNisRevingState;
    if (nisState == NIS_OFF) {
        if (*static_cast<int *>(static_cast<void *>(&PattternPlay)) == 0) {
            eCurNisRevingState = NIS_OFF;
            TimeIntoRev = TotalTime;
            EAX_CarState *stateCar =
                m_pStateBase != nullptr
                    ? *static_cast<EAX_CarState **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pStateBase)) + 0x34))
                    : nullptr;
            CarID = stateCar != nullptr
                        ? *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(stateCar)) + 0x23C))
                        : -1;
            if (CarID < 0) {
                return;
            }
            if (*static_cast<int *>(static_cast<void *>(&g_pNISRevMgr->IsInitialized)) == 0) {
                return;
            }

            int numPoints = g_pNISRevMgr->m_EngineDataSet[CarID].NumPoints;
            if (numPoints < 2) {
                return;
            }

            pRevData = g_pNISRevMgr->m_EngineDataSet[CarID].DataPoints;
            eCurNisRevingState = NIS_PATTERN_ON;
            nisState = NIS_PATTERN_ON;
            NumDataPoints = numPoints - 1;
        } else {
            *static_cast<int *>(static_cast<void *>(&PattternPlay)) = 0;
            pRevData = nullptr;

            EngRevDataPoint *patternData = RevPat5;
            int patternLength = 0x1B;
            switch (PatternNumber) {
            case 6:
                patternLength = 0x16;
                patternData = RevPat6;
                break;
            case 7:
                patternLength = 0x13;
                patternData = RevPat7;
                break;
            case 8:
                patternLength = 0x43;
                patternData = RevPat8;
                break;
            case 9:
                patternLength = 0x38;
                patternData = RevPat9;
                break;
            case 10:
                patternLength = 0x22;
                patternData = RevPat10;
                break;
            case 11:
                patternLength = 0x1E;
                patternData = RevPat11;
                break;
            case 12:
                patternLength = 0x1D;
                patternData = RevPat12;
                break;
            default:
                break;
            }

            NumDataPoints = patternLength;
            pRevData = patternData;
            if (pRevData != nullptr) {
                eCurNisRevingState = NIS_PATTERN_ON;
                Slope rpmSlope(static_cast< float >(pRevData->RPM), static_cast< float >(pRevData[1].RPM), pRevData->time,
                               pRevData[1].time);
                Slope trqSlope(static_cast< float >(pRevData->Trq), static_cast< float >(pRevData[1].Trq), pRevData->time,
                               pRevData[1].time);
                NISRPM = rpmSlope.GetValue(TimeIntoRev);
                NISTRQ = trqSlope.GetValue(TimeIntoRev);
                nisState = eCurNisRevingState;
            } else {
                nisState = eCurNisRevingState;
            }
        }
    }

    if (nisState == NIS_PATTERN_ON) {
        if (TotalTime < 0.001f) {
            TimeIntoRev = TimeIntoRev + deltaTime;
        } else {
            TimeIntoRev = TotalTime;
        }

        EngRevDataPoint *revData = pRevData;
        int stateValue = 2;
        if (revData[1].time < TimeIntoRev) {
            int remainingDataPoints = NumDataPoints;
            while (true) {
                NumDataPoints = remainingDataPoints - 1;
                if ((remainingDataPoints - 1) == 0) {
                    eCurNisRevingState = NIS_OFF;
                    stateValue = 1;
                } else {
                    revData = revData + 1;
                    pRevData = revData;
                }
                if ((TimeIntoRev <= revData[1].time) || (stateValue == 1)) {
                    break;
                }
                remainingDataPoints = NumDataPoints;
            }
        }

        if (stateValue != 1) {
            Slope rpmSlope(static_cast< float >(revData->RPM), static_cast< float >(revData[1].RPM), revData->time,
                           revData[1].time);
            revData = pRevData;
            Slope trqSlope(static_cast< float >(revData->Trq), static_cast< float >(revData[1].Trq), revData->time,
                           revData[1].time);
            NISRPM = rpmSlope.GetValue(TimeIntoRev);
            NISTRQ = trqSlope.GetValue(TimeIntoRev);
        }

        float secondsBeforeRaceStart = -1.0f;
        if (secondsBeforeRaceStart < 1.0f && secondsBeforeRaceStart > 0.0f) {
            eCurNisRevingState = NIS_MERGE_WITH_PHYSICS;
        }
    } else if (nisState == NIS_MERGE_WITH_PHYSICS) {
        float carPhysRPM =
            *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0x64));
        if (carPhysRPM < PhysicsRPM) {
            NISTRQ = 100.0f;
        } else {
            NISTRQ = 0.0f;
        }
        NISRPM = smooth(carPhysRPM, PhysicsRPM, 500.0f);
    } else {
        NISRPM = NISRPM - 500.0f;
        NISTRQ = NISTRQ - 15.0f;
    }

    float nisTrq = NISTRQ;
    if (nisTrq < 0.0f) {
        nisTrq = 0.0f;
    }
    if (nisTrq > 100.0f) {
        nisTrq = 100.0f;
    }

    float nisRpm = NISRPM;
    if (nisRpm < 1000.0f) {
        nisRpm = 1000.0f;
    }
    if (nisRpm > 10000.0f) {
        nisRpm = 10000.0f;
    }

    NISTRQ = nisTrq;
    PhysicsRPM = nisRpm;
    NISRPM = nisRpm;
    PhysicsTRQ = nisTrq;
    m_fThrottle = nisTrq;
    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0x60)) = nisTrq;
    *static_cast<int *>(static_cast<void *>(&IsAccelerating)) = (nisTrq > 30.0f);
    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0x64)) = PhysicsRPM;
    *static_cast<float *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0x70)) = m_fThrottle;
    *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0x68)) =
        static_cast<int>(static_cast<float>(*static_cast<int *>(static_cast<void *>(&IsAccelerating))) != 0.0f);
    *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEAXCar)) + 0x6C)) = static_cast<int>(m_CurGear);
}
