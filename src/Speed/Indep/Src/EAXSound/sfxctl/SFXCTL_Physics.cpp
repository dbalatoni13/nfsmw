#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
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
} // namespace

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
    m_OldThrottle = m_fThrottle;
    m_fThrottle = smooth(m_fThrottle, 1.0f, 10.0f);
    PhysicsTRQ = smooth(PhysicsTRQ, m_fThrottle, 10.0f);
    PhysicsRPM = smooth(PhysicsRPM, m_fThrottle, 10.0f);
    UpdateNIS(t, SndBase::m_fDeltaTime);
}

void SFXCTL_Physics::UpdateMixerOutputs() {
    int *outputs = GetOutputBlockPtr();
    EAX_CarState *pCar =
        m_pEAXCar != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pEAXCar) + 0x34) : nullptr;

    float fVelY = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x58);
    float fVelX = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x54);
    float fVelZ = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x5C);
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

    pCar = m_pEAXCar != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pEAXCar) + 0x34) : nullptr;
    fVelY = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x58);
    fVelX = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x54);
    fVelZ = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x5C);
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

    pCar = m_pEAXCar != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pEAXCar) + 0x34) : nullptr;
    fVelY = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x58);
    fVelX = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x54);
    fVelZ = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x5C);
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
    pCar = m_pEAXCar != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pEAXCar) + 0x34) : nullptr;
    fVelY = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x58);
    fVelX = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x54);
    fVelZ = *reinterpret_cast<float *>(reinterpret_cast<char *>(pCar) + 0x5C);
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

    iOut = static_cast< int >((fMaxPhysRPM - *reinterpret_cast<float *>(reinterpret_cast<char *>(m_pEAXCar) + 0x64)) * 3.6407778f);
    iClamp = 0;
    if (iOut > 0) {
        iClamp = iOut;
    }
    if (iClamp > 0x7FFF) {
        iClamp = 0x7FFF;
    }
    outputs[4] = iClamp;

    int iAccelOutput = 0;
    if (*reinterpret_cast<int *>(&IsAccelerating) != 0) {
        iAccelOutput = 0x7FFF;
    }
    outputs[10] = iAccelOutput;

    unsigned int wheelsOnGround = 0;
    int wheelIndex = 0;
    do {
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(pCar) + 0xB8 + wheelIndex * 0x44) != 0) {
            wheelsOnGround += 1;
        }
        wheelIndex += 1;
    } while (wheelIndex < 4);
    outputs[5] = static_cast< int >(static_cast<float>(wheelsOnGround) * 8191.75f);

    int pov = *reinterpret_cast<int *>(reinterpret_cast<char *>(m_pEAXCar) + 0xB8);
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
        if (*reinterpret_cast<int *>(&PattternPlay) == 0) {
            eCurNisRevingState = NIS_OFF;
            TimeIntoRev = TotalTime;
            EAX_CarState *stateCar =
                m_pStateBase != nullptr ? *reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(m_pStateBase) + 0x34)
                                        : nullptr;
            CarID = stateCar != nullptr ? *reinterpret_cast<int *>(reinterpret_cast<char *>(stateCar) + 0x23C) : -1;
            if (CarID < 0) {
                return;
            }
            if (*reinterpret_cast<int *>(&g_pNISRevMgr->IsInitialized) == 0) {
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
            *reinterpret_cast<int *>(&PattternPlay) = 0;
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
        float carPhysRPM = *reinterpret_cast<float *>(reinterpret_cast<char *>(m_pEAXCar) + 0x64);
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
    *reinterpret_cast<float *>(reinterpret_cast<char *>(m_pEAXCar) + 0x60) = nisTrq;
    *reinterpret_cast<int *>(&IsAccelerating) = (nisTrq > 30.0f);
    *reinterpret_cast<float *>(reinterpret_cast<char *>(m_pEAXCar) + 0x64) = PhysicsRPM;
    *reinterpret_cast<float *>(reinterpret_cast<char *>(m_pEAXCar) + 0x70) = m_fThrottle;
    *reinterpret_cast<int *>(reinterpret_cast<char *>(m_pEAXCar) + 0x68) =
        static_cast<int>(static_cast< float >(*reinterpret_cast<int *>(&IsAccelerating)) != 0.0f);
    *reinterpret_cast<int *>(reinterpret_cast<char *>(m_pEAXCar) + 0x6C) = static_cast< int >(m_CurGear);
}
