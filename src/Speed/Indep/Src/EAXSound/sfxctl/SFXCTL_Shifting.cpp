#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Libs/Support/Utility/UBezierLite.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

struct stShiftPair {
    short RPM;
    short Time;
};

struct stShiftPatternLayout {
    Attrib::StringKey BankName;
    float Down_Shift_Sound_Delay;
    float Up_Engaging_Attack_Vol;
    unsigned int LFO_RPM_Amp;
    unsigned int LFO_Vol_Amp;
    unsigned int Down_Engaging_Rise_T;
    unsigned int Up_Engaging_Attack_T;
    unsigned int LFO_RPM_Decay_Time;
    float Down_Reattach_Scale;
    unsigned int Down_Engaging_Fall_T;
    float Up_Shift_Sound_Delay;
    unsigned int Up_Vol_Shift;
    unsigned int Down_Disengage_Fall_T;
    unsigned int LFO_Vol_Freq;
    unsigned int Down_Disengage_Fall_RPM;
    unsigned int Down_Vol_Shift;
    unsigned int Down_Engaging_Rise_RPM;
    unsigned int LFO_RPM_Freq;
    unsigned int LFO_Vol_Decay_Time;
};

struct EAXCarShiftView {
    char _pad_state[0x34];
    EAX_CarState *mCarState;
    char _pad_38[0x34];
    int mCurGear;
    char _pad_70[0x68];
    char mShiftInfo[0x14];
};

struct EAXCarStateShiftView {
    char _pad0[0x54];
    bVector3 mVel0;
    char _pad64[0x14];
    float mBrake;
    char _pad7C[0x194];
    int mContext;
};

#define SHIFT_EAXCAR_VIEW(ptr) (*static_cast<EAXCarShiftView *>(static_cast<void *>(ptr)))
#define SHIFT_CAR_VIEW(ptr) (*static_cast<EAXCarStateShiftView *>(static_cast<void *>(ptr)))

static inline const stShiftPatternLayout *GetShiftingPatternLayout(const shiftpattern *pattern) {
    return reinterpret_cast<const stShiftPatternLayout *>(
        static_cast<const Attrib::Instance *>(static_cast<const void *>(pattern))->GetLayoutPointer());
}

void FillGraphFromSpline(const UMath::Matrix4 &matrix, bVector2 *points, int num_points, float XScale, float YScale);

SFXCTL_Shifting::SFXCTL_Shifting()
    : m_pEngineCtl(nullptr) //
    , m_bNeed_ShiftGearSnd(false) //
    , m_bNeed_DisengageSnd(false) //
    , m_bNeed_EngageSnd(false) //
    , m_bNeed_AccelSnd(false) //
    , m_bNeed_DeccelSnd(false) //
    , m_bShouldBeWhining(false) //
    , m_bBrakePedalMashed(false) //
    , m_pShiftingPatternData(nullptr) //
    , eShiftState(SHFT_NONE) //
    , eShiftStageChanged(SHFT_NONE) //
    , m_VOL_LFO_AMP(0) //
    , m_VOL_LFO_FRQ(0) //
    , m_TRQ_LFO_AMP(0) //
    , m_TRQ_LFO_FRQ(0) //
    , m_RPM_LFO_AMP(0) //
    , m_RPM_LFO_FRQ(0) //
    , m_bPendingNeedShiftSound(false) //
    , ShiftType(static_cast< AEMS_SHIFTING_SAMPLES >(0)) //
    , tShiftDelay(0.0f) //
    , t_Last_Shift(0.0f) //
    , RPM_AtShift(0.0f) //
    , m_RPMGraph(m_RPMPoints, 7) //
    , t_CurStage(0.0f) //
    , m_CurStage(0) //
    , RPMOffset(0) //
    , m_timeBrakeLastMashed(0) //
    , m_nPostShiftFXLevel(0) //
    , eShift_LFO(SHIFT_LFO_NONE) {}

SndBase *SFXCTL_Shifting::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Shifting();
}

SFXCTL_Shifting::~SFXCTL_Shifting() {}

SndBase::TypeInfo *SFXCTL_Shifting::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Shifting::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Shifting::InitSFX() {
    SFXCTL::InitSFX();
    CleanUpShiftFX();
    m_bNeed_ShiftGearSnd = false;
    m_bNeed_DisengageSnd = false;
    m_bNeed_EngageSnd = false;
    m_bNeed_AccelSnd = false;
    m_bNeed_DeccelSnd = false;
    m_bPendingNeedShiftSound = false;
    tShiftDelay = 0.0f;
}

void SFXCTL_Shifting::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);
    UpdateGearShiftState(t);
    UpdateRPM(t);
    UpdateTorque(t);
    PostShiftFX_Update(t);
}

int SFXCTL_Shifting::GetController(int Index) {
    if (Index != 0) {
        return -1;
    }
    return 4;
}

float SFXCTL_Shifting::GetShiftingRPM() { return m_InterpShiftRPM.CurValue; }

float SFXCTL_Shifting::GetShiftingTRQ() { return m_InterpShiftTorque.CurValue; }

float SFXCTL_Shifting::GetShiftingVOL() { return m_InterpShiftVol.CurValue; }

Gear SFXCTL_Shifting::GetCurGear() { return m_pEngineCtl->m_pPhysicsCtl->m_CurGear; }

Gear SFXCTL_Shifting::GetLastGear() { return m_pEngineCtl->m_pPhysicsCtl->m_LastGear; }

void SFXCTL_Shifting::PostShiftFX_End() {
    eShift_LFO = SHIFT_LFO_NONE;
    m_VOL_LFO_AMP = 0;
    m_VOL_LFO_FRQ = 0;
    m_TRQ_LFO_AMP = 0;
    m_TRQ_LFO_FRQ = 0;
    m_RPM_LFO_AMP = 0;
    m_RPM_LFO_FRQ = 0;
}

void SFXCTL_Shifting::CleanUpShiftFX() {
    PostShiftFX_End();
    eShiftState = static_cast<SHIFT_STAGE>(0);
    eShiftStageChanged = static_cast<SHIFT_STAGE>(0);
}

void SFXCTL_Shifting::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    eAemsUpgradeLevel ugl = m_pEAXCar->m_TurboUGL;
    m_UGL = ugl;
    m_pShiftingPatternData = static_cast<shiftpattern *>(static_cast<void *>(SHIFT_EAXCAR_VIEW(m_pEAXCar).mShiftInfo));
    m_nPostShiftFXLevel = ugl % 2;
}

void SFXCTL_Shifting::AttachController(SFXCTL *ctrl) {
    if (((ctrl->GetTypeInfo()->ObjectID >> 4) & 0xFFF) == 4) {
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(ctrl);
    }
}

void SFXCTL_Shifting::UpdateMixerOutputs() {
    SetDMIX_Input(0, (eShiftState == SHFT_UP_DISENGAGE) ? 0x7FFF : 0);
    SetDMIX_Input(1, (eShiftStageChanged == SHFT_UP_ENGAGING) ? 0x7FFF : 0);
    SetDMIX_Input(2, IsDownShifting() ? 0x7FFF : 0);
}

void SFXCTL_Shifting::UpdateGearShiftState(float t) {
    static int _brakeInit = 0;
    static float _prevBrakeState = 0.0f;
    static const float kDownShiftingRevPercent = 0.5f;
    static const int kDownShiftingRevRampTime = 0x32;
    static const int kUpShiftTrqAttackTime[4] = {0x64, 0x64, 0x64, 0x64};
    static const float kUpShiftTrqAttachInitialPercent[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    static const int kSportShift = -1;

    if (SndBase::m_fRunningTime > tShiftDelay &&
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) != 0) {
        *static_cast<int *>(static_cast<void *>(&m_bNeed_ShiftGearSnd)) = 1;
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 0;
    }

    int isWhining = static_cast<int>(GetCurGear());
    *static_cast<int *>(static_cast<void *>(&m_bShouldBeWhining)) = (isWhining == 0);

    EAXCarStateShiftView *carstate =
        static_cast<EAXCarStateShiftView *>(static_cast<void *>(SHIFT_EAXCAR_VIEW(m_pEAXCar).mCarState));
    if (static_cast<int>(carstate->mContext) == 0) {
        float t_last_mashed =
            static_cast<float>(WorldTimer.GetPackedTime() - m_timeBrakeLastMashed.GetPackedTime()) * 0.00025f;

        if (_brakeInit == 0) {
            _prevBrakeState = carstate->mBrake;
            _brakeInit = 1;
        }

        if (carstate->mBrake >= 1.0f) {
            if (*static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) == 0) {
                if (t_last_mashed > 1.0f && _prevBrakeState == 0.0f) {
                    bVector3 &vel = carstate->mVel0;
                    if (bLength(vel) * 2.23699f > 5.0f) {
                        *static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) = 1;
                    }
                }
            }
        }

        if (*static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) != 0) {
            m_timeBrakeLastMashed = WorldTimer;
            if (carstate->mBrake == 0.0f) {
                *static_cast<int *>(static_cast<void *>(&m_bBrakePedalMashed)) = 0;
            }
        }

        _prevBrakeState = carstate->mBrake;
    }

    eShiftStageChanged = SHFT_NONE;
    if (eShiftState == SHFT_NONE) {
        return;
    }

    UpdateRPM(t);
    UpdateTorque(t);
    m_InterpShiftVol.Update(t);
    PostShiftFX_Update(t);
    t_CurStage += t * 1000.0f;

    switch (eShiftState) {
    case SHFT_UP_DISENGAGE: {
        float curTime = t_CurStage;
        if (m_RPMPoints[6].x < curTime) {
            m_CurStage++;
            unsigned int numDisengage = static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData))
                                            ->Get(0xF2D90101)
                                            .GetLength();
            if (m_CurStage < numDisengage) {
                const UMath::Matrix4 *curve =
                    static_cast<const UMath::Matrix4 *>(static_cast<const void *>(
                        static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData))
                            ->GetAttributePointer(0xF040E6B0, m_CurStage)));
                if (curve == nullptr) {
                    curve = static_cast<const UMath::Matrix4 *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(UMath::Matrix4))));
                }
                const stShiftPair *pair =
                    static_cast<const stShiftPair *>(static_cast<const void *>(
                        static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData))
                            ->GetAttributePointer(0xF2D90101, m_CurStage)));
                if (pair == nullptr) {
                    pair = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
                }
                const stShiftPair *pair2 =
                    static_cast<const stShiftPair *>(static_cast<const void *>(
                        static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData))
                            ->GetAttributePointer(0xF2D90101, m_CurStage)));
                if (pair2 == nullptr) {
                    pair2 = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
                }

                FillGraphFromSpline(*curve, m_RPMPoints, 7, static_cast<float>(pair->Time), static_cast<float>(pair2->RPM));
                t_CurStage = t * 1000.0f;
                RPMOffset = static_cast<unsigned short>(static_cast<int>(m_InterpShiftRPM.CurValue));
                curTime = t_CurStage;
            } else {
                eShiftStageChanged = SHFT_UP_ENGAGING;
                eShiftState = SHFT_UP_ENGAGING;

                const UMath::Matrix4 *curve =
                    static_cast<const UMath::Matrix4 *>(static_cast<const void *>(
                        static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData))
                            ->GetAttributePointer(0x68DA6275, 0)));
                if (curve == nullptr) {
                    curve = static_cast<const UMath::Matrix4 *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(UMath::Matrix4))));
                }
                const stShiftPair *pair =
                    static_cast<const stShiftPair *>(static_cast<const void *>(
                        static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData))
                            ->GetAttributePointer(0xCB89B8C8, 0)));
                if (pair == nullptr) {
                    pair = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
                }
                const stShiftPair *pair2 =
                    static_cast<const stShiftPair *>(static_cast<const void *>(
                        static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData))
                            ->GetAttributePointer(0xCB89B8C8, 0)));
                if (pair2 == nullptr) {
                    pair2 = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
                }

                FillGraphFromSpline(*curve, m_RPMPoints, 7, static_cast<float>(pair->Time), static_cast<float>(pair2->RPM));
                t_CurStage = 0.0f;
                RPMOffset = static_cast<unsigned short>(static_cast<int>(m_pEngineCtl->m_pPhysicsCtl->PhysicsRPM));

                float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(t_CurStage), 1000.0f, 10000.0f);
                m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);

                int length = bClamp(SHIFT_EAXCAR_VIEW(m_pEAXCar).mCurGear + kSportShift, 0, 3);
                float physTRQ = m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ();
                m_InterpShiftTorque.Initialize(kUpShiftTrqAttachInitialPercent[length] * physTRQ, physTRQ,
                                               kUpShiftTrqAttackTime[length], LINEAR);

                const stShiftPatternLayout *layout = GetShiftingPatternLayout(m_pShiftingPatternData);
                m_InterpShiftVol.Initialize(layout->Up_Engaging_Attack_Vol, 0.0f,
                                            static_cast<int>(layout->Up_Engaging_Attack_T), LINEAR);

                PostShiftFX_Init();
                PostShiftFX_Update(t);
                *static_cast<int *>(static_cast<void *>(&m_bNeed_EngageSnd)) = 1;
                return;
            }
        }

        float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(curTime), 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
        break;
    }
    case SHFT_UP_ENGAGING: {
        float curTime = t_CurStage;
        if (m_RPMPoints[6].x < curTime) {
            eShiftStageChanged = SHFT_UP_LFO;
            eShiftState = SHFT_UP_LFO;
            return;
        }
        RPMOffset = static_cast<unsigned short>(static_cast<int>(m_pEngineCtl->m_pPhysicsCtl->PhysicsRPM));
        float currpm = bClamp(static_cast<float>(RPMOffset) + m_RPMGraph.GetValue(curTime), 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(currpm, currpm, 0, LINEAR);
        break;
    }
    case SHFT_UP_LFO:
        if ((!m_Shift_RPM_AMP_DECAY.bComplete || !m_Shift_VOL_AMP_DECAY.bComplete) && eShift_LFO != SHIFT_LFO_NONE) {
            return;
        }
        CleanUpShiftFX();
        break;
    case SHFT_DOWN_DISENGAGE: {
        if (!m_InterpShiftRPM.bComplete) {
            return;
        }

        eShiftStageChanged = SHFT_DOWN_ENGAGING_RISE;
        eShiftState = SHFT_DOWN_ENGAGING_RISE;

        const stShiftPatternLayout *layout = GetShiftingPatternLayout(m_pShiftingPatternData);
        int riseTime = static_cast<int>(layout->Down_Engaging_Rise_T);
        if (m_pEngineCtl->m_pPhysicsCtl->m_CurGear < static_cast<Gear>(2)) {
            riseTime = static_cast<int>(static_cast<float>(riseTime) * 0.7f);
        }

        float lowRpmScale = m_pEngineCtl->GetEngRPM() < 1500.0f ? 0.0f : 1.0f;
        float targetRpm = bClamp(m_pEngineCtl->m_pPhysicsCtl->PhysicsRPM +
                                     static_cast<float>(layout->Down_Engaging_Rise_RPM) *
                                         lowRpmScale,
                                 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(m_pEngineCtl->GetEngRPM(), targetRpm, riseTime, EQ_PWR_SQ);
        m_InterpShiftTorque.Initialize(0.0f, m_pEngineCtl->GetEngTorque(), kDownShiftingRevRampTime, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_RISE: {
        if (!m_InterpShiftRPM.bComplete) {
            return;
        }

        eShiftStageChanged = SHFT_DOWN_ENGAGING_FALL;
        eShiftState = SHFT_DOWN_ENGAGING_FALL;

        const stShiftPatternLayout *layout = GetShiftingPatternLayout(m_pShiftingPatternData);
        int fallTime = static_cast<int>(layout->Down_Engaging_Fall_T);
        if (GetCurGear() < static_cast<Gear>(2) && m_UGL < AEMS_LEVEL2) {
            fallTime = static_cast<int>(static_cast<float>(fallTime) * 0.7f);
        }

        float lowRpmScale = m_pEngineCtl->GetEngRPM() < 1500.0f ? 0.0f : 1.0f;
        const unsigned int *fallRpmPtr = static_cast<const unsigned int *>(static_cast<const void *>(
            static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData))->GetAttributePointer(0x3E1A0DB6, 0)));
        if (fallRpmPtr == nullptr) {
            fallRpmPtr = static_cast<const unsigned int *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(unsigned int))));
        }

        m_InterpShiftRPM.Initialize(m_pEngineCtl->GetEngRPM(),
                                    m_pEngineCtl->GetEngRPM() - static_cast<float>(*fallRpmPtr) * lowRpmScale, fallTime,
                                    LINEAR);
        m_InterpShiftTorque.Initialize(kDownShiftingRevPercent * lowRpmScale, 0.0f, kDownShiftingRevRampTime, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_FALL: {
        if (!m_InterpShiftRPM.bComplete) {
            return;
        }

        eShiftStageChanged = SHFT_DOWN_ENGAGING_REATTACH;
        eShiftState = SHFT_DOWN_ENGAGING_REATTACH;

        const stShiftPatternLayout *layout = GetShiftingPatternLayout(m_pShiftingPatternData);
        float attachTime =
            bClamp(bAbs((m_pEngineCtl->GetEngRPM() - m_pEngineCtl->m_pPhysicsCtl->PhysicsRPM) *
                        layout->Down_Reattach_Scale),
                   0.0f, 800.0f);
        m_InterpShiftRPM.Initialize(m_pEngineCtl->GetEngRPM(), m_pEngineCtl->m_pPhysicsCtl->PhysicsRPM,
                                    static_cast<int>(attachTime), EQ_PWR_SQ);
        m_InterpShiftTorque.Initialize(0.0f, m_pEngineCtl->m_pPhysicsCtl->GetPhysTRQ(), 0x3C, LINEAR);
        break;
    }
    case SHFT_DOWN_ENGAGING_REATTACH:
        if (m_InterpShiftRPM.bComplete) {
            CleanUpShiftFX();
        }
        break;
    default:
        break;
    }
}

void SFXCTL_Shifting::UpdateTorque(float t) {
    (void)t;
    if (m_pEngineCtl == nullptr) {
        return;
    }
    m_InterpShiftTorque.Update(SndBase::m_fDeltaTime, m_pEngineCtl->GetSmoothedEngTorque());
}

void SFXCTL_Shifting::UpdateRPM(float t) {
    (void)t;
    if (m_pEngineCtl == nullptr) {
        return;
    }
    m_InterpShiftRPM.Update(SndBase::m_fDeltaTime, m_pEngineCtl->GetSmoothedEngRPM());
}

void SFXCTL_Shifting::BeginUpShift() {
    CleanUpShiftFX();

    SFXCTL_Engine *engine = m_pEngineCtl;
    if (3000.0f <= engine->m_pPhysicsCtl->PhysicsRPM) {
        eShiftStageChanged = SHFT_UP_DISENGAGE;
        eShiftState = SHFT_UP_DISENGAGE;

        float runningTime = SndBase::m_fRunningTime;
        float prevRpm = engine->m_fPrevRPM;
        RPM_AtShift = prevRpm;
        float curRpm = engine->m_fEng_RPM;
        engine->m_fEng_RPM = prevRpm;
        engine->m_fPrevRPM = curRpm;
        engine->m_fSmoothedEng_RPM = engine->m_fSmoothedEng_RPM * 0.95f + prevRpm * 0.05f;
        t_Last_Shift = runningTime;

        Attrib::Instance *shiftPattern = static_cast<Attrib::Instance *>(static_cast<void *>(m_pShiftingPatternData));
        const UMath::Matrix4 *curve = static_cast<const UMath::Matrix4 *>(static_cast<const void *>(shiftPattern->GetAttributePointer(0xF040E6B0, 0)));
        if (curve == nullptr) {
            curve = static_cast<const UMath::Matrix4 *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(UMath::Matrix4))));
        }

        const stShiftPair *pair = static_cast<const stShiftPair *>(static_cast<const void *>(shiftPattern->GetAttributePointer(0xF2D90101, 0)));
        if (pair == nullptr) {
            pair = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
        }
        const stShiftPair *pair2 =
            static_cast<const stShiftPair *>(static_cast<const void *>(shiftPattern->GetAttributePointer(0xF2D90101, 0)));
        if (pair2 == nullptr) {
            pair2 = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
        }

        FillGraphFromSpline(*curve, m_RPMPoints, 7, static_cast<float>(pair->Time), static_cast<float>(pair2->RPM));

        float shiftRpm = RPM_AtShift;
        m_CurStage = 0;
        t_CurStage = 0.0f;
        RPMOffset = static_cast<unsigned short>(static_cast<int>(shiftRpm));

        m_InterpShiftRPM.Initialize(shiftRpm, shiftRpm, 0, LINEAR);
        m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);
        float physTrq = engine->m_pPhysicsCtl->GetPhysTRQ();
        m_InterpShiftTorque.Initialize(physTrq, 0.0f, 100, LINEAR);

        float totalDuration;
        unsigned int numDisengage = shiftPattern->Get(0xF2D90101).GetLength();
        if (numDisengage == 1) {
            const stShiftPair *first =
                static_cast<const stShiftPair *>(static_cast<const void *>(shiftPattern->GetAttributePointer(0xF2D90101, 0)));
            if (first == nullptr) {
                first = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
            }
            totalDuration = static_cast<float>(first->Time);
        } else {
            const stShiftPair *first =
                static_cast<const stShiftPair *>(static_cast<const void *>(shiftPattern->GetAttributePointer(0xF2D90101, 0)));
            if (first == nullptr) {
                first = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
            }
            const stShiftPair *second =
                static_cast<const stShiftPair *>(static_cast<const void *>(shiftPattern->GetAttributePointer(0xF2D90101, 1)));
            if (second == nullptr) {
                second = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
            }
            totalDuration = static_cast<float>(first->Time) + static_cast<float>(second->Time);
        }

        const stShiftPair *upEngage =
            static_cast<const stShiftPair *>(static_cast<const void *>(shiftPattern->GetAttributePointer(0xCB89B8C8, 0)));
        if (upEngage == nullptr) {
            upEngage = static_cast<const stShiftPair *>(static_cast<const void *>(Attrib::DefaultDataArea(sizeof(stShiftPair))));
        }
        int visualLength = static_cast<int>(totalDuration + static_cast<float>(upEngage->Time));
        m_VisualRPM.Initialize(RPM_AtShift, RPM_AtShift - 500.0f, visualLength, EQ_PWR_SQ);

        *static_cast<int *>(static_cast<void *>(&m_bNeed_DisengageSnd)) = 1;
        *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 1;
        tShiftDelay = SndBase::m_fRunningTime + GetShiftingPatternLayout(m_pShiftingPatternData)->Up_Shift_Sound_Delay;
        ShiftType = static_cast< AEMS_SHIFTING_SAMPLES >(1);
    }
}

void SFXCTL_Shifting::BeginDownShift() {
    if (GetCurGear() != static_cast< Gear >(1)) {
        CleanUpShiftFX();

        SFXCTL_Engine *engine = m_pEngineCtl;
        eShiftStageChanged = SHFT_DOWN_DISENGAGE;
        eShiftState = SHFT_DOWN_DISENGAGE;

        float prevRpm = engine->m_fPrevRPM;
        RPM_AtShift = prevRpm;
        float curRpm = engine->m_fEng_RPM;
        engine->m_fEng_RPM = prevRpm;
        engine->m_fPrevRPM = curRpm;
        engine->m_fSmoothedEng_RPM = engine->m_fSmoothedEng_RPM * 0.95f + prevRpm * 0.05f;
        t_Last_Shift = SndBase::m_fRunningTime;

        float physTrq = engine->m_pPhysicsCtl->GetPhysTRQ();
        m_InterpShiftTorque.Initialize(physTrq, 0.0f, 0x32, LINEAR);

        const stShiftPatternLayout *layout = GetShiftingPatternLayout(m_pShiftingPatternData);
        float targetRpm =
            RPM_AtShift - static_cast<float>(layout->Down_Disengage_Fall_RPM);
        targetRpm = bClamp(targetRpm, 1000.0f, 10000.0f);
        m_InterpShiftRPM.Initialize(RPM_AtShift, targetRpm, static_cast<int>(layout->Down_Disengage_Fall_T), LINEAR);
        m_InterpShiftVol.Initialize(0.0f, 0.0f, 1, LINEAR);

        tShiftDelay = SndBase::m_fRunningTime + layout->Down_Shift_Sound_Delay;
        if (3000.0f < engine->m_pPhysicsCtl->PhysicsRPM) {
            *static_cast<int *>(static_cast<void *>(&m_bPendingNeedShiftSound)) = 1;
        }

        ShiftType = static_cast< AEMS_SHIFTING_SAMPLES >(0);
    }
}

void SFXCTL_Shifting::PostShiftFX_Update(float t) {
    if (t <= 0.0f) {
        return;
    }
    m_Shift_VOL_AMP_DECAY.Update(t);
    m_Shift_RPM_AMP_DECAY.Update(t);
}

void SFXCTL_Shifting::PostShiftFX_Init() {
    PostShiftFX_End();
    m_Shift_VOL_AMP_DECAY.Initialize(0.0f, 0.0f, 0.0f, LINEAR);
    m_Shift_RPM_AMP_DECAY.Initialize(0.0f, 0.0f, 0.0f, LINEAR);
}

void FillGraphFromSpline(const UMath::Matrix4 &matrix, bVector2 *points, int num_points, float XScale, float YScale) {
    if (num_points > 0) {
        float denom = static_cast<float>(num_points - 1);
        for (int n = 0; n < num_points; ++n) {
            UMath::Vector4 point;
            UBezierLite::Evaluate(matrix, static_cast<float>(n) / denom, point);
            points[n].x = point.x * XScale;
            points[n].y = point.y * YScale;
        }
    }
}
