#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/Messages/MCountdownDone.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyVehicleDestroyed.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern float lbl_803D726C;
extern float lbl_803D7270;
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
extern float lbl_803D72FC;
extern float lbl_803D7300;
extern Slope RedLineDelayPerGear;
extern "C" int GetQ15FromHundredthsdB__11NFSMixShapei(int ndB);

inline float SFXCTL::GetPhysRPM() { return m_pEAXCar->GetPhysRPM(); }
inline float SFXCTL::GetPhysTRQ() { return m_pEAXCar->GetPhysTRQ(); }

static const float REDLINE_ENG_FADE[2] = {450.0f, 50.0f};
static const float REDLINE_REDSAMP_FADE[2] = {50.0f, 120.0f};

Slope RedLineDelayPerGear(1.0f, 5.0f, 1.0f, 5.0f);

struct HSIMABLE__;

SFXCTL_Engine::SFXCTL_Engine()
    : Trq(3), //
      VisRpmAvg(2), //
      m_fSmoothedEng_RPM(0.0f), //
      m_fSmoothedEng_Trq(0.0f) {
    mmsgMVehicleDestroyed = Hermes::Handler::Create<MNotifyVehicleDestroyed, SFXCTL_Engine, SFXCTL_Engine>(
        this, &SFXCTL_Engine::MessageVehicleDestroyed, UCrc32(0x20D60DBF), 0);
    mmsgMVehicleDestroyed2 = Hermes::Handler::Create<MCountdownDone, SFXCTL_Engine, SFXCTL_Engine>(
        this, &SFXCTL_Engine::MsgCountdownDone, UCrc32(0x20D60DBF), 0);

    m_pAccelTransitionCtl = nullptr;
    m_pShiftCtl = nullptr;
    m_pPhysicsCtl = nullptr;
    bClutchStateOn = false;
    bIsRedlining = false;
    RedLineEngFactor.Initialize(1.0f, 1.0f, 1, LINEAR);
    RedLineSampFactor.Initialize(0.0f, 0.0f, 1, LINEAR);
    bPlayCompression = true;
    m_ComppressionRPM.Initialize(0.0f, 0.0f, 1);
    vCarPos = bVector3(0.0f, 0.0f, 0.0f);
    bWasRedlining = false;
}

SndBase *SFXCTL_Engine::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Engine::GetStaticTypeInfo()->typeName, false) SFXCTL_Engine();
    }
    return new (SFXCTL_Engine::GetStaticTypeInfo()->typeName, true) SFXCTL_Engine();
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

const char *SFXCTL_Engine::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_Engine::InitSFX() {
    GRaceParameters *race;

    SFXCTL::InitSFX();
    m_bIsEngineBlown = false;
    m_Rotation = 0x96;
    Trq.Flush(0.0f);
    VisRpmAvg.Flush(0.0f);
    Rpm.Flush(0.0f);
    m_DistanceFltr = 24000;
    m_fPrevRPM = 0.0f;
    m_VOL_LFO = 0.0f;
    m_RPM_LFO = 0.0f;
    m_TRQ_LFO = 0.0f;
    bClutchStateOn = false;
    m_aglVOL_LFO = 0;
    m_aglRPM_LFO = 0;
    m_aglTRQ_LFO = 0;
    m_p3DCarPosCtl->AssignPositionVector(&vCarPos);
    m_p3DCarPosCtl->AssignVelocityVector(GetPhysCar()->GetVelocity());
    m_p3DCarPosCtl->AssignDirectionVector(GetPhysCar()->GetForwardVector());

    switch (m_pEAXCar->GetEngineUpgradeLevel()) {
    case AEMS_LEVEL1:
        SetDMIX_Input(0, 0x2AAA);
        break;
    case AEMS_LEVEL2:
        SetDMIX_Input(0, 0x5554);
        break;
    case AEMS_LEVEL3:
        SetDMIX_Input(0, 0x7FFF);
        break;
    default:
        SetDMIX_Input(0, 0);
        break;
    }

    tMergeWithPhysicsOffStart = 0.0f;
    if (GRaceStatus::Exists()) {
        race = GRaceStatus::Get().GetRaceParameters();
    } else {
        race = GRaceDatabase::Get().GetStartupRace();
    }

    if (!race) {
        bPreRace = false;
    } else if (race->GetIsRollingStart()) {
        bPreRace = false;
    } else {
        bPreRace = true;
    }
}

void SFXCTL_Engine::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);

    const bVector3 *Cur3dPos = GetPhysCar()->GetPosition();
    const bVector2 *Cur2dPos = GetPhysCar()->GetPosition2D();
    bVector3 vOffset;

    SetDMIX_Input(2, static_cast<int>(m_pEAXCar->GetAttributes().Master_Vol()));
    m_p3DCarPosCtl->AssignDirectionVector(GetPhysCar()->GetForwardVector());

    (void)Cur2dPos;
    vCarPos = *Cur3dPos;
    vOffset = bNormalize(*GetPhysCar()->GetForwardVector());

    if (m_pEAXCar->GetPOV() == 1) {
        vOffset = bScale(vOffset, 2.0f);
    } else {
        vOffset = bScale(vOffset, lbl_803D726C);
    }
    vCarPos = bAdd(vCarPos, vOffset);

    UpdateClutchState();
    UpdateEngineLFO_FX(t);
    UpdateCompression(t);
    UpdateRPM(t);
    UpdateTorque(t);
    UpdateVolume(t);
    UpdateFilterFX();
    UpdateRedlining(t);

    GetPhysCar()->SetVisualRPM(m_pEAXCar->GetFinalAudioRPM());

    if ((GetPhysCar()->IsEngineBlown() || GetPhysCar()->IsEngineSabotaged()) && !m_bIsEngineBlown) {
        m_bIsEngineBlown = true;
        unsigned int key;
        if (GetPhysCar()->IsEngineBlown()) {
            key = 0xbc2dfa2f;
        }
        if (GetPhysCar()->IsEngineSabotaged()) {
            key = 0xbae41d1b;
        }
        MGamePlayMoment moment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, key);
        moment.Send(UCrc32("MomentStrm"));
    }
}

void SFXCTL_Engine::MessageVehicleDestroyed(const MNotifyVehicleDestroyed &message) {
    UMath::Vector4 vpos;

    if (GetPhysCar() != nullptr && GetPhysCar()->mHandle == message.GetRacer() && GetPhysCar()->IsLocalPlayerCar()) {
        vpos.z = GetPhysCar()->GetPosition()->x;
        vpos.x = -GetPhysCar()->GetPosition()->y;
        vpos.y = GetPhysCar()->GetPosition()->z;

        MGamePlayMoment(vpos, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0xC565AC30)
            .Send(UCrc32("MomentStrm"));
    }
}

float SFXCTL_Engine::GetEngRPM() { return m_fEng_RPM; }

float SFXCTL_Engine::GetSmoothedEngRPM() { return m_fSmoothedEng_RPM; }

float SFXCTL_Engine::GetEngTorque() { return m_fEng_Trq; }

float SFXCTL_Engine::GetSmoothedEngTorque() { return m_fSmoothedEng_Trq; }

inline void SFXCTL_Engine::SetEngTorque(float _torque) {
    _torque += m_TRQ_LFO;
    m_fEng_Trq = _torque;
    m_fSmoothedEng_Trq = m_fSmoothedEng_Trq * lbl_803D72FC + _torque * lbl_803D7300;
}

void SFXCTL_Engine::MsgCountdownDone(const MCountdownDone &message) {
    tMergeWithPhysicsOffStart = 0.7f;
    bPreRace = 0;
}

void SFXCTL_Engine::UpdateClutchState() {
    bClutchStateOn = ShouldTurnOnClutch();
}

bool SFXCTL_Engine::ShouldTurnOnClutch() {
    if (!GetPhysCar()->IsLocalPlayerCar()) {
        return false;
    }
    if (m_pAccelTransitionCtl->IsActive()) {
        return false;
    }
    return GetEngRPM() <= 2500.0f;
}

void SFXCTL_Engine::UpdateFilterFX() {
    float DistanceToUse;
    float fdist;
    float DistanceRolloffFilterFActor;
    int DBResult;
    int Q15Result;
    int DistanceFilter;

    fdist = static_cast<float>(m_p3DCarPosCtl->GetDMIX_InputValue(1));
    DistanceRolloffFilterFActor = ((fdist * 0.01f) - 6.5f) / lbl_803D7270;
    DistanceToUse = bClamp(DistanceRolloffFilterFActor, 0.0f, 1.0f);
    DBResult = NFSMixShape::GetCurveOutput(
        static_cast<NFSMixShape::eMIXTABLEID>(6), static_cast<int>(DistanceToUse * 32766.0f), true);
    Q15Result = GetQ15FromHundredthsdB__11NFSMixShapei(DBResult);
    m_DistanceFltr = static_cast<int>(static_cast<float>(Q15Result) * 0.7103183f + 725.0f);

    DistanceFilter = 0;
    if (m_DistanceFltr > 0) {
        DistanceFilter = m_DistanceFltr;
    }
    if (DistanceFilter > 0x7FFF) {
        DistanceFilter = 0x7FFF;
    }
    m_DistanceFltr = DistanceFilter;
}

void SFXCTL_Engine::UpdateCompression(float t) {
    SetDMIX_Input(1, 0);
    m_ComppressionRPM.Update(t);

    if (bPlayCompression) {
        int CompDuration = g_pEAXSound->Random(100) + 0x19;
        float DeltaRPM = g_pEAXSound->Random(100.0f) + 25.0f;

        m_ComppressionRPM.ClearStages();
        m_ComppressionRPM.AddStage(0.0f, DeltaRPM, CompDuration, EQ_PWR_SQ);
        m_ComppressionRPM.AddStage(DeltaRPM, 0.0f, CompDuration, EQ_PWR_SQ);
        bPlayCompression = false;
        SetDMIX_Input(1, 0x7fff);
    }
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
    m_iEngineVol = 0x7fff;
    if (m_pShiftCtl->IsActive()) {
        m_iEngineVol += static_cast<int>(static_cast<float>(m_iEngineVol) * m_pShiftCtl->GetShiftingVOL());
    }
    m_iEngineVol += static_cast<int>(m_VOL_LFO);
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
    if (bClutchStateOn) {
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
    if (m_pShiftCtl != nullptr && m_pShiftCtl->IsActive()) {
        Trq.Flush(m_pShiftCtl->GetShiftingTRQ());
    } else if (m_pAccelTransitionCtl->IsActive()) {
        Trq.Flush(m_pAccelTransitionCtl->m_InterpEngTorque.GetValue());
    } else {
        Trq.Record(GetPhysTRQ());
    }

    Trq.Recalculate();

    SetEngTorque(static_cast<const Average &>(Trq).GetValue());
}

void SFXCTL_Engine::UpdateEngineLFO_FX(float t) {
    int tmp_VOL_LFO_AMP = 0;
    int tmp_VOL_LFO_FRQ = 0;
    int tmp_TRQ_LFO_AMP = 0;
    int tmp_TRQ_LFO_FRQ = 0;
    int tmp_RPM_LFO_AMP = 0;
    int tmp_RPM_LFO_FRQ = 0;

    if (m_pShiftCtl != nullptr) {
        if (m_pShiftCtl->IsActive()) {
            tmp_RPM_LFO_FRQ = m_pShiftCtl->m_RPM_LFO_FRQ;
            tmp_VOL_LFO_AMP = m_pShiftCtl->m_VOL_LFO_AMP;
            tmp_VOL_LFO_FRQ = m_pShiftCtl->m_VOL_LFO_FRQ;
            tmp_TRQ_LFO_AMP = m_pShiftCtl->m_TRQ_LFO_AMP;
            tmp_TRQ_LFO_FRQ = m_pShiftCtl->m_TRQ_LFO_FRQ;
            tmp_RPM_LFO_AMP = m_pShiftCtl->m_RPM_LFO_AMP;
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

    tmp_RPM_LFO_FRQ = bClamp(tmp_RPM_LFO_FRQ, 1, 10000);
    tmp_TRQ_LFO_FRQ = bClamp(tmp_TRQ_LFO_FRQ, 1, 10000);
    tmp_VOL_LFO_FRQ = bClamp(tmp_VOL_LFO_FRQ, 1, 10000);

    if (tmp_RPM_LFO_AMP != 0) {
        unsigned int angle = static_cast<unsigned int>(m_aglRPM_LFO) +
                             static_cast<int>((t / (static_cast<float>(tmp_RPM_LFO_FRQ) * 0.001f)) * 65535.0f);
        m_aglRPM_LFO = static_cast<short>(angle) + static_cast<short>((angle & 0xFFFF) / 0xFFFF);
        m_RPM_LFO = static_cast<float>(tmp_RPM_LFO_AMP) * bSin(m_aglRPM_LFO);
    }

    if (tmp_TRQ_LFO_AMP != 0) {
        unsigned int angle = static_cast<unsigned int>(m_aglTRQ_LFO) +
                             static_cast<int>((t / (static_cast<float>(tmp_TRQ_LFO_FRQ) * 0.001f)) * 65535.0f);
        m_aglTRQ_LFO = static_cast<short>(angle) + static_cast<short>((angle & 0xFFFF) / 0xFFFF);
        m_TRQ_LFO = static_cast<float>(tmp_TRQ_LFO_AMP) * bSin(m_aglTRQ_LFO);
    }

    if (tmp_VOL_LFO_AMP != 0) {
        unsigned int angle = static_cast<unsigned int>(m_aglVOL_LFO) +
                             static_cast<int>((t / (static_cast<float>(tmp_VOL_LFO_FRQ) * 0.001f)) * 65535.0f);
        m_aglVOL_LFO = static_cast<short>(angle) + static_cast<short>((angle & 0xFFFF) / 0xFFFF);
        m_VOL_LFO = static_cast<float>(tmp_VOL_LFO_AMP) * bSin(m_aglVOL_LFO);
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

void SFXCTL_Engine::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
    case 2:
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    case 3:
        m_pAccelTransitionCtl = static_cast<SFXCTL_AccelTrans *>(psfxctl);
        break;
    case 0:
        m_pPhysicsCtl = static_cast<SFXCTL_Physics *>(psfxctl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    }
}
