#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/Messages/MCountdownDone.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyVehicleDestroyed.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

static const float CLUTCH_MODEL_RPM_DELTA = 60.0f;
static const float CLUTCH_MODEL_RPM_THRESHOLD = 2500.0f;
static const int MIN_EXHVIEW_ROTATION = 150;
static const float LP_CUTOFF_FREQ_AT_MAX_DIST = 725.0f;
static const float TimeToMergeWithPhysicsOffStart = 0.7f;
static const float MIN_COMPRESSION_RPM = 25.0f;
static const float MAX_COMPRESSION_RPM = 100.0f;
static const int MIN_COMPRESSION_DURATION = 25;
static const int MAX_COMPRESSION_DURATION = 100;
static const float REDLINING_MIX_PERCENT = 0.85f;
static const float REDLINING_RPM = 9800.0f;
static const float REDLINE_SCALE_PER_GEAR = 5.0f;

extern Slope RedLineDelayPerGear;
extern "C" int GetQ15FromHundredthsdB__11NFSMixShapei(int ndB);

static const float REDLINE_ENG_FADE[2] = {450.0f, 50.0f};
static const float REDLINE_REDSAMP_FADE[2] = {50.0f, 120.0f};

Slope RedLineDelayPerGear(1.0f, REDLINE_SCALE_PER_GEAR, 1.0f, REDLINE_SCALE_PER_GEAR);

SndBase::TypeInfo SFXCTL_Engine::s_TypeInfo = {(int)0x00020040, "SFXCTL_Engine", &SFXCTL::s_TypeInfo, SFXCTL_Engine::CreateObject};

SndBase::TypeInfo *SFXCTL_Engine::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_Engine::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_Engine::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_Engine::s_TypeInfo.typeName, false) SFXCTL_Engine();
    }
    return new (SFXCTL_Engine::s_TypeInfo.typeName, true) SFXCTL_Engine();
}

SFXCTL_Engine::SFXCTL_Engine()
    : Trq(3), //
      VisRpmAvg(2), //
      m_fSmoothedEng_RPM(0.0f), //
      m_fSmoothedEng_Trq(0.0f), //
      mmsgMVehicleDestroyed(Hermes::Handler::Create<MNotifyVehicleDestroyed, SFXCTL_Engine, SFXCTL_Engine>(
          this, &SFXCTL_Engine::MessageVehicleDestroyed, UCrc32(0x20D60DBF), 0)), //
      mmsgMCoundown(Hermes::Handler::Create<MCountdownDone, SFXCTL_Engine, SFXCTL_Engine>(
          this, &SFXCTL_Engine::MsgCountdownDone, UCrc32(0x20D60DBF), 0)) {
    this->m_pAccelTransitionCtl = nullptr;
    this->m_pShiftCtl = nullptr;
    this->m_pPhysicsCtl = nullptr;
    this->bClutchStateOn = false;
    this->bIsRedlining = false;
    this->RedLineEngFactor.Initialize(1.0f, 1.0f, 1, LINEAR);
    this->RedLineSampFactor.Initialize(0.0f, 0.0f, 1, LINEAR);
    this->bPlayCompression = true;
    this->m_ComppressionRPM.Initialize(0.0f, 0.0f, 1);
    bVector3 v3CarPos(0.0f, 0.0f, 0.0f);

    this->vCarPos = v3CarPos;
    this->bWasRedlining = false;
}

SFXCTL_Engine::~SFXCTL_Engine() {
    if (this->mmsgMVehicleDestroyed) {
        Hermes::Handler::Destroy(this->mmsgMVehicleDestroyed);
    }
    if (this->mmsgMCoundown) {
        Hermes::Handler::Destroy(this->mmsgMCoundown);
    }
}

void SFXCTL_Engine::MessageVehicleDestroyed(const MNotifyVehicleDestroyed &message) {
    UMath::Vector4 vpos;

    if (this->GetPhysCar() && this->GetPhysCar()->mHandle == message.GetRacer() && this->GetPhysCar()->IsLocalPlayerCar()) {
        vpos.z = this->GetPhysCar()->GetPosition()->x;
        vpos.x = -this->GetPhysCar()->GetPosition()->y;
        vpos.y = this->GetPhysCar()->GetPosition()->z;

        MGamePlayMoment(vpos, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0xC565AC30)
            .Send(UCrc32("MomentStrm"));
    }
}

void SFXCTL_Engine::MsgCountdownDone(const MCountdownDone &message) {
    this->tMergeWithPhysicsOffStart = TimeToMergeWithPhysicsOffStart;
    this->bPreRace = false;
}

void SFXCTL_Engine::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = static_cast<eAemsUpgradeLevel>(this->m_pEAXCar->GetEngineUpgradeLevel());
}

void SFXCTL_Engine::InitSFX() {
    GRaceParameters *race;

    SFXCTL::InitSFX();
    this->m_bIsEngineBlown = false;
    this->m_Rotation = MIN_EXHVIEW_ROTATION;
    this->Trq.Flush(0.0f);
    this->VisRpmAvg.Flush(0.0f);
    this->Rpm.Flush(0.0f);
    this->m_DistanceFltr = 24000;
    this->m_fPrevRPM = 0.0f;
    this->m_VOL_LFO = 0.0f;
    this->m_RPM_LFO = 0.0f;
    this->m_TRQ_LFO = 0.0f;
    this->bClutchStateOn = false;
    this->m_aglRPM_LFO = 0;
    this->m_aglTRQ_LFO = 0;
    this->m_aglVOL_LFO = 0;
    this->m_p3DCarPosCtl->AssignPositionVector(&this->vCarPos);
    this->m_p3DCarPosCtl->AssignVelocityVector(this->GetPhysCar()->GetVelocity());
    this->m_p3DCarPosCtl->AssignDirectionVector(this->GetPhysCar()->GetForwardVector());

    switch (this->m_pEAXCar->GetEngineUpgradeLevel()) {
    case AEMS_LEVEL0:
    default:
        this->SetDMIX_Input(0, 0);
        break;
    case AEMS_LEVEL1:
        this->SetDMIX_Input(0, 0x2AAA);
        break;
    case AEMS_LEVEL2:
        this->SetDMIX_Input(0, 0x5554);
        break;
    case AEMS_LEVEL3:
        this->SetDMIX_Input(0, 0x7FFF);
        break;
    }

    this->tMergeWithPhysicsOffStart = 0.0f;
    if (GRaceStatus::Exists()) {
        race = GRaceStatus::Get().GetRaceParameters();
    } else {
        race = GRaceDatabase::Get().GetStartupRace();
    }

    if (!race) {
        this->bPreRace = false;
    } else if (race->GetIsRollingStart()) {
        this->bPreRace = false;
    } else {
        this->bPreRace = true;
    }
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
        this->m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    case 3:
        this->m_pAccelTransitionCtl = static_cast<SFXCTL_AccelTrans *>(psfxctl);
        break;
    case 0:
        this->m_pPhysicsCtl = static_cast<SFXCTL_Physics *>(psfxctl);
        break;
    case 7:
        this->m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    }
}

void SFXCTL_Engine::UpdateParams(float t) {
    SFXCTL::UpdateParams(t);

    const bVector3 *Cur3dPos;
    const bVector2 *Cur2dPos;
    bVector3 vOffset;

    this->SetDMIX_Input(2, static_cast<int>(this->m_pEAXCar->GetAttributes().Master_Vol()));
    this->m_p3DCarPosCtl->AssignDirectionVector(this->GetPhysCar()->GetForwardVector());

    Cur3dPos = this->GetPhysCar()->GetPosition();
    Cur2dPos = this->GetPhysCar()->GetPosition2D();
    (void)Cur2dPos;
    this->vCarPos = *Cur3dPos;
    {
        vOffset = *this->GetPhysCar()->GetForwardVector();
        vOffset = bNormalize(vOffset);
    }

    if (this->m_pEAXCar->GetPOV() != 1) {
        vOffset = bScale(vOffset, 0.2f);
    } else {
        vOffset = bScale(vOffset, 2.0f);
    }
    this->vCarPos = bAdd(this->vCarPos, vOffset);

    this->UpdateClutchState();
    this->UpdateEngineLFO_FX(t);
    this->UpdateCompression(t);
    this->UpdateRPM(t);
    this->UpdateTorque(t);
    this->UpdateVolume(t);
    this->UpdateFilterFX();
    this->UpdateRedlining(t);

    this->GetPhysCar()->SetVisualRPM(this->m_pEAXCar->GetFinalAudioRPM());

    if ((this->GetPhysCar()->IsEngineBlown() || this->GetPhysCar()->IsEngineSabotaged()) && !this->m_bIsEngineBlown) {
        this->m_bIsEngineBlown = true;
        unsigned int key;
        if (this->GetPhysCar()->IsEngineBlown()) {
            key = 0xbc2dfa2f;
        }
        if (this->GetPhysCar()->IsEngineSabotaged()) {
            key = 0xbae41d1b;
        }
        MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, key).Send(UCrc32("MomentStrm"));
    }
}

void SFXCTL_Engine::UpdateFilterFX() {
    float DistanceToUse;
    float fdist;
    float DistanceRolloffFilterFActor;
    int DBResult;
    int Q15Result;
    int DistanceFilter;

    fdist = static_cast<float>(this->m_p3DCarPosCtl->GetDMIX_InputValue(1));
    float DistanceRolloffRange = 44.0f;
    DistanceRolloffFilterFActor = ((fdist * 0.01f) - 6.5f) / DistanceRolloffRange;
    DistanceToUse = bClamp(DistanceRolloffFilterFActor, 0.0f, 1.0f);
    DBResult = NFSMixShape::GetCurveOutput(
        static_cast<eMIXTABLEID>(6), static_cast<int>(DistanceToUse * 32766.0f), true);
    Q15Result = GetQ15FromHundredthsdB__11NFSMixShapei(DBResult);
    this->m_DistanceFltr = static_cast<int>(static_cast<float>(Q15Result) * 0.7103183f + LP_CUTOFF_FREQ_AT_MAX_DIST);

    DistanceFilter = bClamp(this->m_DistanceFltr, 0, 0x7FFF);
    this->m_DistanceFltr = DistanceFilter;
}

void SFXCTL_Engine::UpdateCompression(float t) {
    this->SetDMIX_Input(1, 0);
    this->m_ComppressionRPM.Update(t);

    if (this->bPlayCompression) {
        int CompDuration = g_pEAXSound->Random(MAX_COMPRESSION_DURATION) + 0x19;
        float DeltaRPM = g_pEAXSound->Random(MAX_COMPRESSION_RPM) + MIN_COMPRESSION_RPM;

        this->m_ComppressionRPM.ClearStages();
        this->m_ComppressionRPM.AddStage(0.0f, DeltaRPM, CompDuration, EQ_PWR_SQ);
        this->m_ComppressionRPM.AddStage(DeltaRPM, 0.0f, CompDuration, EQ_PWR_SQ);
        this->bPlayCompression = false;
        this->SetDMIX_Input(1, 0x7fff);
    }
}

void SFXCTL_Engine::UpdateRedlining(float t) {
    this->bWasRedlining = this->bIsRedlining;
    if (this->m_pStateBase->m_eStateType == eMM_AIRACECAR) {
        return;
    }

    if (!this->bIsRedlining) {
        if (!this->m_pShiftCtl->IsActive() && this->GetEngRPM() > REDLINING_RPM) {
            this->bIsRedlining = true;
            float TimeScaleValue = RedLineDelayPerGear.GetValue(static_cast<float>(this->m_pEAXCar->GetCurGear()));
            this->RedLineEngFactor.Initialize(
                this->RedLineEngFactor.GetValue(),
                0.14999998f,
                static_cast<int>(REDLINE_ENG_FADE[0] * this->RedLineEngFactor.GetValue() * TimeScaleValue),
                LINEAR);
            this->RedLineSampFactor.Initialize(
                this->RedLineSampFactor.GetValue(),
                REDLINING_MIX_PERCENT,
                static_cast<int>(REDLINE_REDSAMP_FADE[1] * (1.0f - this->RedLineSampFactor.GetValue()) * TimeScaleValue),
                LINEAR);
            this->bRedliningBounce = true;
            this->RedlineingVisualOffset = 0.0f;
        }
    } else if ((this->GetEngRPM() < REDLINING_RPM || this->m_pShiftCtl->IsActive()) && this->bIsRedlining) {
        this->bIsRedlining = false;
        this->RedLineEngFactor.Initialize(
            this->RedLineEngFactor.GetValue(),
            1.0f,
            static_cast<int>(REDLINE_ENG_FADE[1] * (1.0f - this->RedLineEngFactor.GetValue())),
            LINEAR);
        this->RedLineSampFactor.Initialize(
            this->RedLineSampFactor.GetValue(),
            0.0f,
            static_cast<int>(REDLINE_REDSAMP_FADE[0] * this->RedLineSampFactor.GetValue()),
            LINEAR);
    }

    this->RedLineEngFactor.Update(t);
    this->RedLineSampFactor.Update(t);
}

void SFXCTL_Engine::UpdateVolume(float t) {
    this->m_iEngineVol = 0x7fff;
    if (this->m_pShiftCtl->IsActive()) {
        this->m_iEngineVol += static_cast<int>(static_cast<float>(this->m_iEngineVol) * this->m_pShiftCtl->GetShiftingVOL());
    }
    this->m_iEngineVol += static_cast<int>(this->m_VOL_LFO);
}

void SFXCTL_Engine::UpdateRPM(float t) {
    float Cur_RPM;
    float VisualRPM;
    float NormalRPM;
    float PhysicsNewAudioRPM;

    Cur_RPM = this->m_pShiftCtl && this->m_pShiftCtl->IsActive()
                  ? this->m_pShiftCtl->GetShiftingRPM()
                  : this->m_pAccelTransitionCtl && this->m_pAccelTransitionCtl->IsActive()
                        ? this->m_pAccelTransitionCtl->m_InterpEngRPM.GetValue()
                        : this->GetPhysRPM();

    if (this->bClutchStateOn && !this->m_pShiftCtl->IsActive()) {
        Cur_RPM = smooth(this->GetEngRPM(), this->GetPhysRPM(), 999.0f, CLUTCH_MODEL_RPM_DELTA);
    }

    VisualRPM = Cur_RPM;
    Cur_RPM = VisualRPM + this->m_RPM_LFO;
    Cur_RPM = Cur_RPM + this->m_ComppressionRPM.GetValue();
    NormalRPM = Cur_RPM + this->m_RPM_LFO;
    this->SetEngRPM(NormalRPM);

    if (static_cast<unsigned int>(this->m_pShiftCtl->eShiftState - SHFT_UP_DISENGAGE) < 2u) {
        VisualRPM = this->m_pShiftCtl->m_VisualRPM.GetValue();
    } else if (this->m_pAccelTransitionCtl->eAccelTransFxState == 1) {
        VisualRPM = this->GetPhysRPM();
    } else {
        if (this->bIsRedlining) {
            float Target = 200.0f;
            if (this->bRedliningBounce) {
                Cur_RPM = smooth(this->RedlineingVisualOffset, Target, 50.0f);
                this->RedlineingVisualOffset = Cur_RPM;
                if (Cur_RPM == Target) {
                    this->bRedliningBounce = false;
                }
            } else {
                Cur_RPM = smooth(this->RedlineingVisualOffset, 0.0f, 50.0f);
                this->RedlineingVisualOffset = Cur_RPM;
                if (Cur_RPM == 0.0f) {
                    this->bRedliningBounce = true;
                }
            }
            VisualRPM = VisualRPM - this->RedlineingVisualOffset;
        }
    }

    this->VisRpmAvg.Record(VisualRPM);
    this->VisRpmAvg.Recalculate();

    PhysicsNewAudioRPM = (this->VisRpmAvg.GetValue() - 1000.0f) * 0.00011111111f;
    if (this->GetPhysCar()->IsLocalPlayerCar()) {
        if (this->bPreRace != 0) {
            PhysicsNewAudioRPM = this->GetPhysCar()->GetRPMPct();
        } else if (this->tMergeWithPhysicsOffStart > 0.0f) {
            this->tMergeWithPhysicsOffStart -= t;
            if (this->tMergeWithPhysicsOffStart < 0.0f) {
                this->tMergeWithPhysicsOffStart = 0.0f;
            }
            float PercentInterp = (TimeToMergeWithPhysicsOffStart - this->tMergeWithPhysicsOffStart) * 1.4285715f;
            PhysicsNewAudioRPM = (PhysicsNewAudioRPM - this->GetPhysCar()->GetRPMPct()) * PercentInterp +
                                 this->GetPhysCar()->GetRPMPct();
        }
    }

    this->m_pEAXCar->SetFinalAudioRPM(PhysicsNewAudioRPM);
}

void SFXCTL_Engine::UpdateTorque(float t) {
    (void)t;
    if (this->m_pShiftCtl && this->m_pShiftCtl->IsActive()) {
        this->Trq.Flush(this->m_pShiftCtl->GetShiftingTRQ());
    } else if (this->m_pAccelTransitionCtl->IsActive()) {
        this->Trq.Flush(this->m_pAccelTransitionCtl->m_InterpEngTorque.GetValue());
    } else {
        this->Trq.Record(this->GetPhysTRQ());
    }

    this->Trq.Recalculate();

    this->SetEngTorque(this->Trq.GetValue());
}

void SFXCTL_Engine::UpdateEngineLFO_FX(float t) {
    int tmp_VOL_LFO_AMP = 0;
    int tmp_VOL_LFO_FRQ = 0;
    int tmp_TRQ_LFO_AMP = 0;
    int tmp_TRQ_LFO_FRQ = 0;
    int tmp_RPM_LFO_AMP = 0;
    int tmp_RPM_LFO_FRQ = 0;

    if (this->m_pShiftCtl && this->m_pShiftCtl->IsActive()) {
        tmp_VOL_LFO_AMP = this->m_pShiftCtl->m_VOL_LFO_AMP;
        tmp_VOL_LFO_FRQ = this->m_pShiftCtl->m_VOL_LFO_FRQ;
        tmp_TRQ_LFO_AMP = this->m_pShiftCtl->m_TRQ_LFO_AMP;
        tmp_TRQ_LFO_FRQ = this->m_pShiftCtl->m_TRQ_LFO_FRQ;
        tmp_RPM_LFO_AMP = this->m_pShiftCtl->m_RPM_LFO_AMP;
        tmp_RPM_LFO_FRQ = this->m_pShiftCtl->m_RPM_LFO_FRQ;
    } else {
        this->m_VOL_LFO = 0.0f;
        this->m_aglVOL_LFO = 0x4097;
        this->m_RPM_LFO = 0.0f;
        this->m_aglRPM_LFO = 0x4097;
        this->m_TRQ_LFO = 0.0f;
        this->m_aglTRQ_LFO = 0x4097;
    }

    tmp_RPM_LFO_FRQ = bClamp(tmp_RPM_LFO_FRQ, 1, 10000);
    tmp_TRQ_LFO_FRQ = bClamp(tmp_TRQ_LFO_FRQ, 1, 10000);
    tmp_VOL_LFO_FRQ = bClamp(tmp_VOL_LFO_FRQ, 1, 10000);

    if (tmp_RPM_LFO_AMP != 0) {
        this->m_aglRPM_LFO = static_cast<unsigned short>(this->m_aglRPM_LFO +
                                                        static_cast<int>((t / (static_cast<float>(tmp_RPM_LFO_FRQ) * 0.001f)) * 65535.0f)) % 0xFFFF;
        this->m_RPM_LFO = static_cast<float>(tmp_RPM_LFO_AMP) * bSin(this->m_aglRPM_LFO);
    }

    if (tmp_TRQ_LFO_AMP != 0) {
        this->m_aglTRQ_LFO = static_cast<unsigned short>(this->m_aglTRQ_LFO +
                                                        static_cast<int>((t / (static_cast<float>(tmp_TRQ_LFO_FRQ) * 0.001f)) * 65535.0f)) % 0xFFFF;
        this->m_TRQ_LFO = static_cast<float>(tmp_TRQ_LFO_AMP) * bSin(this->m_aglTRQ_LFO);
    }

    if (tmp_VOL_LFO_AMP != 0) {
        this->m_aglVOL_LFO = static_cast<unsigned short>(this->m_aglVOL_LFO +
                                                        static_cast<int>((t / (static_cast<float>(tmp_VOL_LFO_FRQ) * 0.001f)) * 65535.0f)) % 0xFFFF;
        this->m_VOL_LFO = static_cast<float>(tmp_VOL_LFO_AMP) * bSin(this->m_aglVOL_LFO);
    }
}

bool SFXCTL_Engine::ShouldTurnOnClutch() {
    if (!this->GetPhysCar()->IsLocalPlayerCar()) {
        return false;
    }
    if (this->m_pAccelTransitionCtl->IsActive()) {
        return false;
    }
    if (this->GetEngRPM() > CLUTCH_MODEL_RPM_THRESHOLD) {
        return false;
    }

    return true;
}

void SFXCTL_Engine::UpdateClutchState() {
    this->bClutchStateOn = this->ShouldTurnOnClutch();
}
