#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyVehicleDestroyed.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern float lbl_803D726C;
extern float lbl_803D72D0;
extern float lbl_803D72D4;
extern float lbl_803D72D8;
extern float lbl_803D72DC;
extern float lbl_803D72E0;
extern float lbl_803D72E4;
extern float lbl_803D72E8;
extern float lbl_803D72EC;
extern float lbl_803D72F0;
extern float lbl_803D72F4;
extern float lbl_803D72F8;
extern Slope RedLineDelayPerGear;

static const float REDLINE_ENG_FADE[2] = {450.0f, 50.0f};
static const float REDLINE_REDSAMP_FADE[2] = {50.0f, 120.0f};

struct HSIMABLE__;

SFXCTL_Engine::SFXCTL_Engine()
    : m_pShiftCtl(nullptr) //
    , m_pAccelTransitionCtl(nullptr) //
    , m_pPhysicsCtl(nullptr) //
    , m_p3DCarPosCtl(nullptr) //
    , tMergeWithPhysicsOffStart(0.0f) //
    , bPreRace(false) //
    , m_iEngineVol(0) //
    , bIsRedlining(false) //
    , bWasRedlining(false) //
    , bRedliningBounce(false) //
    , RedlineingVisualOffset(0.0f) //
    , m_fEng_RPM(0.0f) //
    , m_fPrevRPM(0.0f) //
    , m_fSmoothedEng_RPM(0.0f) //
    , m_fEng_Trq(0.0f) //
    , m_fSmoothedEng_Trq(0.0f) //
    , m_Rotation(0) //
    , m_bIsEngineBlown(false) //
    , m_DistanceFltr(0) //
    , bClutchStateOn(false) //
    , m_VOL_LFO(0.0f) //
    , m_RPM_LFO(0.0f) //
    , m_TRQ_LFO(0.0f) //
    , m_aglRPM_LFO(0x4097) //
    , m_aglTRQ_LFO(0x4097) //
    , m_aglVOL_LFO(0x4097) //
    , bPlayCompression(false) //
    , mmsgMVehicleDestroyed(nullptr) //
    , mmsgMVehicleDestroyed2(nullptr) {}

SndBase *SFXCTL_Engine::CreateObject(unsigned int allocator) {
    (void)allocator;
    return new SFXCTL_Engine();
}

SFXCTL_Engine::~SFXCTL_Engine() {
    if (mmsgMVehicleDestroyed) {
        Hermes::Handler::Destroy(mmsgMVehicleDestroyed);
    }
    if (mmsgMVehicleDestroyed2) {
        Hermes::Handler::Destroy(mmsgMVehicleDestroyed2);
    }
}

SndBase::TypeInfo *SFXCTL_Engine::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Engine::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Engine::InitSFX() {
    SFXCTL::InitSFX();
    m_iEngineVol = 0;
    bIsRedlining = false;
    bWasRedlining = false;
    bRedliningBounce = false;
    RedlineingVisualOffset = 0.0f;
    m_fEng_RPM = 0.0f;
    m_fPrevRPM = 0.0f;
    m_fSmoothedEng_RPM = 0.0f;
    m_fEng_Trq = 0.0f;
    m_fSmoothedEng_Trq = 0.0f;
}

void SFXCTL_Engine::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);

    EAXCar *carOwner = m_pEAXCar;
    EAX_CarState *car = carOwner->GetPhysCar();

    const Attrib::Gen::engineaudio &attributes = carOwner->GetAttributes();
    SetDMIX_Input(2, static_cast<int>(attributes.Master_Vol()));
    const bVector3 *forward = static_cast<const bVector3 *>(static_cast<const void *>(&car->mMatrix.v0));
    m_p3DCarPosCtl->AssignDirectionVector(forward);

    const bVector3 *cur3dPos = static_cast<const bVector3 *>(static_cast<const void *>(&car->mMatrix.v3));
    const bVector2 *cur2dPos = static_cast<const bVector2 *>(static_cast<const void *>(cur3dPos));
    (void)cur2dPos;
    vCarPos = *cur3dPos;

    bVector3 vOffset = bNormalize(*forward);
    if (carOwner->m_PovType == 1) {
        vOffset = bScale(vOffset, 2.0f);
    } else {
        vOffset = bScale(vOffset, lbl_803D726C);
    }
    vCarPos = bAdd(vCarPos, vOffset);

    UpdateClutchState();
    UpdateTorque(t);
    UpdateCompression(t);
    UpdateRPM(t);
    UpdateRedlining(t);
    UpdateVolume(t);
    UpdateFilterFX();
    UpdateEngineLFO_FX(t);

    car->mVisualRPM = carOwner->m_fAudioRPM;

    int blownState = car->mEngine.mBlownFlag;
    if ((blownState == 1 || blownState == 2) && !m_bIsEngineBlown) {
        m_bIsEngineBlown = true;
        unsigned int key = 0;
        if (blownState == 1) {
            key = 0xbc2dfa2f;
        }
        if (blownState == 2) {
            key = 0xbae41d1b;
        }
        MGamePlayMoment moment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, key);
        moment.Send(UCrc32("MomentStrm"));
    }
}

void SFXCTL_Engine::MessageVehicleDestroyed(const MNotifyVehicleDestroyed &message) {
    (void)message;
    m_bIsEngineBlown = true;
}

float SFXCTL_Engine::GetEngRPM() { return m_fEng_RPM; }

float SFXCTL_Engine::GetSmoothedEngRPM() { return m_fSmoothedEng_RPM; }

float SFXCTL_Engine::GetEngTorque() { return m_fEng_Trq; }

float SFXCTL_Engine::GetSmoothedEngTorque() { return m_fSmoothedEng_Trq; }

void SFXCTL_Engine::MsgCountdownDone(const MCountdownDone &message) {
    tMergeWithPhysicsOffStart = 0.7f;
    bPreRace = 0;
}

void SFXCTL_Engine::UpdateClutchState() {
    bClutchStateOn = ShouldTurnOnClutch();
}

bool SFXCTL_Engine::ShouldTurnOnClutch() {
    return m_pPhysicsCtl != nullptr && m_pPhysicsCtl->m_CurGear <= 0;
}

void SFXCTL_Engine::UpdateFilterFX() {
    m_DistanceFltr = (m_p3DCarPosCtl != nullptr) ? 1 : 0;
}

void SFXCTL_Engine::UpdateCompression(float t) {
    (void)t;
}

void SFXCTL_Engine::UpdateRedlining(float t) {
    bWasRedlining = bIsRedlining;
    if (m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        return;
    }

    if (!bIsRedlining) {
        if (!m_pShiftCtl->IsActive() && GetEngRPM() > 9800.0f) {
            bIsRedlining = true;
            float TimeScaleValue = RedLineDelayPerGear.GetValue(static_cast<float>(m_pEAXCar->GetCurGear()));
            RedLineEngFactor.Initialize(
                RedLineEngFactor.GetValue(),
                0.14999998f,
                static_cast<int>(REDLINE_ENG_FADE[0] * RedLineEngFactor.GetValue() * TimeScaleValue),
                LINEAR);
            RedLineSampFactor.Initialize(
                RedLineSampFactor.GetValue(),
                0.85f,
                static_cast<int>(REDLINE_REDSAMP_FADE[1] * (1.0f - RedLineSampFactor.GetValue()) * TimeScaleValue),
                LINEAR);
            bRedliningBounce = true;
            RedlineingVisualOffset = 0.0f;
        }
    } else if (GetEngRPM() < 9800.0f || m_pShiftCtl->IsActive()) {
        bIsRedlining = false;
        RedLineEngFactor.Initialize(
            RedLineEngFactor.GetValue(),
            1.0f,
            static_cast<int>(REDLINE_ENG_FADE[1] * (1.0f - RedLineEngFactor.GetValue())),
            LINEAR);
        RedLineSampFactor.Initialize(
            RedLineSampFactor.GetValue(),
            0.0f,
            static_cast<int>(REDLINE_REDSAMP_FADE[0] * RedLineSampFactor.GetValue()),
            LINEAR);
    }

    RedLineEngFactor.Update(t);
    RedLineSampFactor.Update(t);
}

void SFXCTL_Engine::UpdateVolume(float t) {
    (void)t;
    m_iEngineVol = static_cast< int >(m_fSmoothedEng_RPM * 32767.0f);
    if (m_iEngineVol < 0) {
        m_iEngineVol = 0;
    } else if (m_iEngineVol > 32767) {
        m_iEngineVol = 32767;
    }
    SetDMIX_Input(DMX_VOL, m_iEngineVol);
}

void SFXCTL_Engine::UpdateRPM(float t) {
    float VisualRPM;
    float NormalRPM;
    float PhysicsNewAudioRPM;

    if (m_pShiftCtl != nullptr) {
        int shiftActive = 1;
        if (m_pShiftCtl->eShiftState == SHFT_NONE) {
            shiftActive = 0;
        }
        if (shiftActive != 0) {
            VisualRPM = m_pShiftCtl->GetShiftingRPM();
            goto have_cur_rpm;
        }
    }

    if (m_pAccelTransitionCtl != nullptr) {
        int accelActive = 1;
        if (m_pAccelTransitionCtl->eAccelTransFxState == 0) {
            accelActive = 0;
        }
        if (accelActive != 0) {
            VisualRPM = m_pAccelTransitionCtl->m_InterpEngRPM.GetValue();
            goto have_cur_rpm;
        }
    }

    VisualRPM = m_pEAXCar->PhysRPM;

have_cur_rpm:
    if (*static_cast<int *>(static_cast<void *>(&bClutchStateOn)) != 0) {
        int shiftActive = 1;
        if (m_pShiftCtl->eShiftState == SHFT_NONE) {
            shiftActive = 0;
        }
        if (shiftActive == 0) {
            VisualRPM =
                smooth(GetEngRPM(),
                       m_pEAXCar->PhysRPM,
                       lbl_803D72D4, lbl_803D72D8);
        }
    }

    NormalRPM = VisualRPM + m_RPM_LFO + m_ComppressionRPM.CurValue + m_RPM_LFO;
    m_fPrevRPM = m_fEng_RPM;
    m_fEng_RPM = NormalRPM;
    m_fSmoothedEng_RPM = m_fSmoothedEng_RPM * lbl_803D72DC + NormalRPM * lbl_803D72E0;

    if (static_cast< unsigned int >(m_pShiftCtl->eShiftState - SHFT_UP_DISENGAGE) < 2u) {
        VisualRPM = m_pShiftCtl->m_VisualRPM.GetValue();
    } else if (m_pAccelTransitionCtl->eAccelTransFxState == 1) {
        VisualRPM = m_pEAXCar->PhysRPM;
    } else {
        if (bIsRedlining) {
            float target = lbl_803D72E4;
            if (bRedliningBounce) {
                float offset = smooth(RedlineingVisualOffset, target, lbl_803D72E8);
                RedlineingVisualOffset = offset;
                if (offset == target) {
                    bRedliningBounce = false;
                }
            } else {
                float offset = smooth(RedlineingVisualOffset, lbl_803D72D0, lbl_803D72E8);
                RedlineingVisualOffset = offset;
                if (offset == lbl_803D72D0) {
                    bRedliningBounce = true;
                }
            }
            VisualRPM = VisualRPM - RedlineingVisualOffset;
        }
    }

    VisRpmAvg.Record(VisualRPM);
    VisRpmAvg.Recalculate();

    PhysicsNewAudioRPM = (VisRpmAvg.GetValue() - lbl_803D72EC) * lbl_803D72F0;
    EAX_CarState *car = m_pStateBase->GetPhysCar();
    if (car->mContext == 0) {
        if (bPreRace != 0) {
            PhysicsNewAudioRPM = car->mVisualRPM;
        } else if (tMergeWithPhysicsOffStart > lbl_803D72D0) {
            tMergeWithPhysicsOffStart -= t;
            if (tMergeWithPhysicsOffStart < lbl_803D72D0) {
                tMergeWithPhysicsOffStart = lbl_803D72D0;
            }
            float PercentInterp = (lbl_803D72F4 - tMergeWithPhysicsOffStart) * lbl_803D72F8;
            PhysicsNewAudioRPM =
                (PhysicsNewAudioRPM - car->mVisualRPM) * PercentInterp + car->mVisualRPM;
        }
    }

    m_pEAXCar->m_fAudioRPM = PhysicsNewAudioRPM;
}

void SFXCTL_Engine::UpdateTorque(float t) {
    (void)t;
    if (m_pPhysicsCtl != nullptr) {
        m_fEng_Trq = m_pPhysicsCtl->PhysicsTRQ;
    }
    m_fSmoothedEng_Trq = m_fEng_Trq;
}

void SFXCTL_Engine::UpdateEngineLFO_FX(float t) {
    SFXCTL_Shifting *shiftCtl = m_pShiftCtl;
    int volLfoAmp = 0;
    int volLfoFreq = 0;
    int trqLfoAmp = 0;
    int trqLfoFreq = 0;
    int rpmLfoAmp = 0;
    int rpmLfoFreq = 0;
    float updateTime = t;

    if (shiftCtl != nullptr) {
        int hasShiftData = 1;
        if (shiftCtl->eShiftState == SHFT_NONE) {
            hasShiftData = 0;
        }
        if (hasShiftData != 0) {
            rpmLfoFreq = shiftCtl->m_RPM_LFO_FRQ;
            volLfoAmp = shiftCtl->m_VOL_LFO_AMP;
            volLfoFreq = shiftCtl->m_VOL_LFO_FRQ;
            trqLfoAmp = shiftCtl->m_TRQ_LFO_AMP;
            trqLfoFreq = shiftCtl->m_TRQ_LFO_FRQ;
            rpmLfoAmp = shiftCtl->m_RPM_LFO_AMP;
        } else {
            m_aglTRQ_LFO = 0x4097;
            m_TRQ_LFO = 0.0f;
            m_VOL_LFO = 0.0f;
            m_aglVOL_LFO = 0x4097;
            m_RPM_LFO = 0.0f;
            m_aglRPM_LFO = 0x4097;
        }
    } else {
        m_aglTRQ_LFO = 0x4097;
        m_TRQ_LFO = 0.0f;
        m_VOL_LFO = 0.0f;
        m_aglVOL_LFO = 0x4097;
        m_RPM_LFO = 0.0f;
        m_aglRPM_LFO = 0x4097;
    }

    int rpmFreq = 1;
    if (rpmLfoFreq > 1) {
        rpmFreq = rpmLfoFreq;
    }
    if (rpmFreq > 10000) {
        rpmFreq = 10000;
    }

    int trqFreq = 1;
    if (trqLfoFreq > 1) {
        trqFreq = trqLfoFreq;
    }
    if (trqFreq > 10000) {
        trqFreq = 10000;
    }

    int volFreq = 1;
    if (volLfoFreq > 1) {
        volFreq = volLfoFreq;
    }
    if (volFreq > 10000) {
        volFreq = 10000;
    }

    if (rpmLfoAmp != 0) {
        unsigned int angle = static_cast<unsigned int>(m_aglRPM_LFO) +
                             static_cast<unsigned int>((updateTime / (static_cast<float>(rpmFreq) * 0.001f)) * 65535.0f);
        angle = static_cast<unsigned short>(angle);
        angle = angle % 0xFFFF;
        m_aglRPM_LFO = static_cast<unsigned short>(angle);
        m_RPM_LFO = static_cast<float>(rpmLfoAmp) * bSin(m_aglRPM_LFO);
    }

    if (trqLfoAmp != 0) {
        unsigned int angle = static_cast<unsigned int>(m_aglTRQ_LFO) +
                             static_cast<unsigned int>((updateTime / (static_cast<float>(trqFreq) * 0.001f)) * 65535.0f);
        angle = static_cast<unsigned short>(angle);
        angle = angle % 0xFFFF;
        m_aglTRQ_LFO = static_cast<unsigned short>(angle);
        m_TRQ_LFO = static_cast<float>(trqLfoAmp) * bSin(m_aglTRQ_LFO);
    }

    if (volLfoAmp != 0) {
        unsigned int angle = static_cast<unsigned int>(m_aglVOL_LFO) +
                             static_cast<unsigned int>((updateTime / (static_cast<float>(volFreq) * 0.001f)) * 65535.0f);
        angle = static_cast<unsigned short>(angle);
        angle = angle % 0xFFFF;
        m_aglVOL_LFO = static_cast<unsigned short>(angle);
        m_VOL_LFO = static_cast<float>(volLfoAmp) * bSin(m_aglVOL_LFO);
    }
}

void SFXCTL_Engine::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = static_cast<eAemsUpgradeLevel>(m_pEAXCar->GetEngineUpgradeLevel());
}

int SFXCTL_Engine::GetController(int Index) {
    switch (Index) {
    case 0:
        return 2;
    case 1:
        return 3;
    case 2:
        return 0;
    case 3:
        return 7;
    default:
        return -1;
    }
}

void SFXCTL_Engine::AttachController(SFXCTL *ctrl) {
    int id = (ctrl->GetTypeInfo()->ObjectID >> 4) & 0xFFF;
    switch (id) {
    case 2:
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(ctrl);
        break;
    case 3:
        m_pAccelTransitionCtl = static_cast<SFXCTL_AccelTrans *>(ctrl);
        break;
    case 0:
        m_pPhysicsCtl = static_cast<SFXCTL_Physics *>(ctrl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(ctrl);
        break;
    }
}
