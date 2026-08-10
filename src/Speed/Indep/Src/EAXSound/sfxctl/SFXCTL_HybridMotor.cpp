#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"

SFXCTL_HybridMotor::SFXCTL_HybridMotor()
    : DecelCrossfadeMix(CrossFadesPoints, 5) //
    , m_AvgDeltaRPM(4) {
    this->m_AvgDeltaRPM.Flush(0.0f);
    this->m_GinsuLPFVal = 24000.0f;
    this->PrevRPM = 0.0f;
    this->m_CurPhyDeltaRPMVal = 0.0f;
    this->m_CurAudDeltaRPMVal = 0.0f;
    this->PercentOfAccelThreshold = 0.0f;
    this->tSteadyDuration = 0.0f;
    this->CrossFadesPoints[0] = bVector2(0.0f, 0.0f);
    this->CrossFadesPoints[1] = bVector2(0.0f, 0.0f);
    this->CrossFadesPoints[2] = bVector2(0.0f, 0.0f);
    this->CrossFadesPoints[3] = bVector2(0.0f, 0.0f);
    this->CrossFadesPoints[4] = bVector2(0.0f, 0.0f);
    this->CrossFadesPoints[5] = bVector2(0.0f, 0.0f);
    this->SteadyFrameCnt = 0;
    this->mPrevDeltaRPM = 0;
}

SndBase::TypeInfo *SFXCTL_HybridMotor::GetTypeInfo() const { return &s_TypeInfo; }

const char *SFXCTL_HybridMotor::GetTypeName() const { return s_TypeInfo.typeName; }

SndBase *SFXCTL_HybridMotor::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (SFXCTL_HybridMotor::GetStaticTypeInfo()->typeName, false) SFXCTL_HybridMotor();
    }
    return new (SFXCTL_HybridMotor::GetStaticTypeInfo()->typeName, true) SFXCTL_HybridMotor();
}

SFXCTL_HybridMotor::~SFXCTL_HybridMotor() {}

void SFXCTL_HybridMotor::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = static_cast<eAemsUpgradeLevel>(this->m_pEAXCar->GetEngineUpgradeLevel());

    int MaxMinDif = static_cast<int>(this->m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM()) -
                    static_cast<int>(this->m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM());

    this->CrossFadesPoints[1].y = 0.0f;
    this->CrossFadesPoints[1].x = static_cast<float>(this->m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM());
    this->CrossFadesPoints[2].y = 1.0f;
    this->CrossFadesPoints[2].x = static_cast<float>(this->m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM()) +
                            static_cast<float>(MaxMinDif) * this->m_pEAXCar->GetAttributes().GINSU_DECEL_FADE_OUT();
    this->CrossFadesPoints[3].y = 1.0f;
    this->CrossFadesPoints[3].x = static_cast<float>(this->m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM()) -
                            static_cast<float>(MaxMinDif) * this->m_pEAXCar->GetAttributes().GINSU_DECEL_FADE_IN();
    this->CrossFadesPoints[4].y = 0.0f;
    this->CrossFadesPoints[4].x = static_cast<float>(this->m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM());
}

void SFXCTL_HybridMotor::InitSFX() {
    SFXCTL::InitSFX();
    this->m_EngVolAEMS = 0;
    this->m_EngVolAccelGinsu = 0;
    this->m_EngVolDecelGinsu = 0;
}

int SFXCTL_HybridMotor::GetController(int Index) {
    switch (Index) {
    case 0:
        return 4;
    case 1:
        return 2;
    case 2:
        return 3;
    default:
        return -1;
    }
}

void SFXCTL_HybridMotor::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
    case 4:
        this->m_pEngineCtl = static_cast<SFXCTL_Engine *>(psfxctl);
        break;
    case 2:
        this->m_pShiftingCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    case 3:
        this->m_pAccelTranCtl = static_cast<SFXCTL_AccelTrans *>(psfxctl);
        break;
    }
}

void SFXCTL_HybridMotor::UpdateDeltaRPM() {
    this->mPrevPhyDeltaRPM = this->m_CurPhyDeltaRPMVal;

    float PhyDeltaRPM = this->GetPhysRPM() - this->PrevRPM;
    this->m_CurPhyDeltaRPMVal = PhyDeltaRPM;

    float AudDeltaRPM = this->m_pEngineCtl->m_fEng_RPM - this->m_pEngineCtl->m_fPrevRPM;
    this->m_CurAudDeltaRPMVal = AudDeltaRPM;
    this->PrevRPM = this->GetPhysRPM();

    if (this->m_pEAXCar->GetPhysicsCTL()->NISRevingEnabled) {
        this->m_AvgDeltaRPM.Record(AudDeltaRPM);
    } else if (this->m_pAccelTranCtl->IsActive()) {
        if (this->m_pAccelTranCtl->eAccelTransFxState == FX_ACCEL_STATE_ATTACK) {
            this->m_AvgDeltaRPM.Flush(PhyDeltaRPM);
        } else {
            this->m_AvgDeltaRPM.Flush(AudDeltaRPM);
        }
    } else if (this->m_pShiftingCtl->IsActive()) {
        SHIFT_STAGE ShiftState = this->m_pShiftingCtl->eShiftState;

        if (ShiftState == SHFT_DOWN_ENGAGING_RISE) {
            this->m_AvgDeltaRPM.Flush(AudDeltaRPM);
        } else if (this->m_pShiftingCtl->eShiftStageChanged == SHFT_UP_ENGAGING) {
            this->m_AvgDeltaRPM.Flush(bAbs(PhyDeltaRPM));
        } else {
            if (static_cast<unsigned int>(ShiftState - SHFT_UP_ENGAGING) < 2u) {
                this->m_AvgDeltaRPM.Record(bAbs(AudDeltaRPM));
            } else {
                this->m_AvgDeltaRPM.Record(AudDeltaRPM);
            }
        }
    } else {
        this->m_AvgDeltaRPM.Record(PhyDeltaRPM);
    }

    this->m_AvgDeltaRPM.Recalculate();
}

void SFXCTL_HybridMotor::UpdateParams(float t) {
    if (!this->m_pEAXCar->GetAttributes().IsValid()) {
        return;
    }
    SFXCTL::UpdateParams(t);
    this->UpdateDeltaRPM();
    float AemsRPM = this->m_pEngineCtl->GetEngRPM();
    AemsRPM = bClamp(AemsRPM, 1000.0f, 10000.0f);
    this->m_GinsuScaledRPM = (AemsRPM - 1000.0f) * (this->m_pEAXCar->GetAttributes().MaxRPM() - this->m_pEAXCar->GetAttributes().MinRPM()) * (1.0f / 9000.0f) + this->m_pEAXCar->GetAttributes().MinRPM();
    if (this->m_pEAXCar->m_EngineType == eGINSU_ENG_SINGLE) {
        this->UpdateSingleMixEng(t);
    } else {
        this->UpdateDualMixEng(t);
    }
    this->UpdateVolumeRedlining();
}

void SFXCTL_HybridMotor::UpdateSingleMixEng(float t) {
    (void)t;
    Slope TrqThreshold(0.0f, 1.0f, 0.0f, 35.0f);
    float AccelDecelMix = TrqThreshold.GetValue(this->m_pEngineCtl->GetEngTorque());
    bool USE_SMOOTHING = true;
    EngineMix AccelMix;
    EngineMix DecelMix;
    float DeltaRPM;
    float adt;

    DeltaRPM = static_cast<const Average &>(this->m_AvgDeltaRPM).GetValue();
    adt = this->m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();

    this->PercentOfAccelThreshold = 1.0f - (adt - bAbs(DeltaRPM + 10.0f)) / adt;
    this->PercentOfAccelThreshold = bClamp(this->PercentOfAccelThreshold, 0.0f, 1.0f);
    adt = this->m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold();
    this->PercentOfDecelThreshold =
        1.0f - (adt - bAbs(DeltaRPM + 10.0f)) / this->m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold();
    this->PercentOfDecelThreshold = bClamp(this->PercentOfDecelThreshold, 0.0f, 1.0f);

    if (this->GetPhysCar()->IsShifting() || this->m_pShiftingCtl->IsActive()) {
        if (this->m_pShiftingCtl->eShiftState != SHFT_UP_LFO) {
            if (this->m_pShiftingCtl->eShiftState < SHFT_UP_LFO) {
                if (this->m_pShiftingCtl->eShiftState == SHFT_UP_ENGAGING) {
                    USE_SMOOTHING = false;
                }
            } else if (this->m_pShiftingCtl->eShiftState != SHFT_DOWN_ENGAGING_RISE) {
                if (this->m_pShiftingCtl->eShiftState == SHFT_DOWN_ENGAGING_REATTACH && this->m_pEAXCar->IsAccelerating()) {
                    USE_SMOOTHING = false;
                }
            }
        }
    } else if (this->m_pAccelTranCtl->IsActive()) {
        if (this->m_pAccelTranCtl->eAccelTransFxState == FX_ACCEL_STATE_ATTACK) {
            USE_SMOOTHING = false;
        }
    }
    AccelMix.Aems = this->PercentOfAccelThreshold *
                        (this->m_pEAXCar->GetAttributes().AEMSMix_L_RPM() - this->m_pEAXCar->GetAttributes().AEMSMix_S_RPM()) +
                    this->m_pEAXCar->GetAttributes().AEMSMix_S_RPM();
    AccelMix.AccelGinsu =
        this->PercentOfAccelThreshold *
            (this->m_pEAXCar->GetAttributes().GINSUMix_L_RPM() - this->m_pEAXCar->GetAttributes().GINSUMix_S_RPM()) +
        this->m_pEAXCar->GetAttributes().GINSUMix_S_RPM();
    AccelMix.DecelGinsu = 0.0f;
    AccelMix.Cutoff = this->m_pEngineCtl->m_DistanceFltr;
    DecelMix.Aems = this->PercentOfDecelThreshold * (this->m_pEAXCar->GetAttributes().DECEL_AEMSMix_L_RPM() -
                                               this->m_pEAXCar->GetAttributes().DECEL_AEMSMix_S_RPM()) +
                    this->m_pEAXCar->GetAttributes().DECEL_AEMSMix_S_RPM();
    DecelMix.AccelGinsu =
        this->PercentOfDecelThreshold * (this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_L_RPM() -
                                   this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_L_RPM()) +
        this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_S_RPM();
    AccelMix.DecelGinsu = 0.0f;
    DecelMix.Cutoff = bMin(static_cast<int>(this->m_pEAXCar->GetAttributes().GINSU_LowPassCutoff()),
                           this->m_pEngineCtl->m_DistanceFltr);

    EngineMix newmix;
    float EngineCtlVolFactor;
    int VolAEMS;
    int VolAccelGinsu;
    int VolDecelGinsu;
    newmix.Aems = (AccelMix.Aems - DecelMix.Aems) * AccelDecelMix + DecelMix.Aems;
    newmix.DecelGinsu = (AccelMix.DecelGinsu - DecelMix.DecelGinsu) * AccelDecelMix + DecelMix.DecelGinsu;
    newmix.AccelGinsu = (AccelMix.AccelGinsu - DecelMix.AccelGinsu) * AccelDecelMix + DecelMix.AccelGinsu;
    newmix.Cutoff = static_cast<int>((static_cast<float>(AccelMix.Cutoff) - static_cast<float>(DecelMix.Cutoff)) *
                                         AccelDecelMix +
                                     static_cast<float>(DecelMix.Cutoff));

    if (USE_SMOOTHING) {
        this->m_EngineMix.Aems = smooth(newmix.Aems, newmix.Aems, 0.2f);
        this->m_EngineMix.AccelGinsu = smooth(newmix.AccelGinsu, newmix.AccelGinsu, 0.2f);
        this->m_EngineMix.DecelGinsu = smooth(newmix.DecelGinsu, newmix.DecelGinsu, 0.2f);
        this->m_EngineMix.Cutoff = smooth(newmix.Cutoff, newmix.Cutoff, 6000);
    } else {
        this->m_EngineMix = newmix;
    }

    this->m_GinsuLPFVal = static_cast<float>(
        25000 - NFSMixShape::GetCurveOutput(static_cast<NFSMixShape::eMIXTABLEID>(7),
                                            static_cast<int>(static_cast<float>(this->m_EngineMix.Cutoff) * 1.3106799f),
                                            true));

    EngineCtlVolFactor = static_cast<float>(this->m_pEngineCtl->m_iEngineVol) * 3.051851e-05f;
    VolAEMS = static_cast<int>(
        this->m_EngineMix.Aems * static_cast<float>(this->m_pEAXCar->GetAttributes().AEMSVol()) * EngineCtlVolFactor);
    VolAccelGinsu = static_cast<int>(
        this->m_EngineMix.AccelGinsu * static_cast<float>(this->m_pEAXCar->GetAttributes().GINSUAccelVol()) * EngineCtlVolFactor);
    VolDecelGinsu = static_cast<int>(
        this->m_EngineMix.DecelGinsu * static_cast<float>(this->m_pEAXCar->GetAttributes().GinsuDecelVol()) * EngineCtlVolFactor);

    if (USE_SMOOTHING) {
        this->m_EngVolAEMS = smooth(this->m_EngVolAEMS, VolAEMS, 7000);
        this->m_EngVolAccelGinsu = smooth(this->m_EngVolAccelGinsu, VolAccelGinsu, 7000);
        this->m_EngVolDecelGinsu = smooth(this->m_EngVolDecelGinsu, VolDecelGinsu, 7000);
    } else {
        this->m_EngVolAEMS = VolAEMS;
        this->m_EngVolAccelGinsu = VolAccelGinsu;
        this->m_EngVolDecelGinsu = VolDecelGinsu;
    }

    this->m_EngVolRedLine = static_cast<int>(static_cast<float>(this->m_pEAXCar->GetAttributes().AEMSVol()) * EngineCtlVolFactor);
}

void SFXCTL_HybridMotor::UpdateDualMixEng(float t) {
    (void)t;
    Slope TrqThreshold(0.0f, 1.0f, 0.0f, 35.0f);
    float AccelDecelMix = TrqThreshold.GetValue(this->m_pEngineCtl->GetEngTorque());
    bool USE_SMOOTHING = true;
    EngineMix AccelMix;
    EngineMix DecelMix;
    float DeltaRPM;
    float adt;
    bool UseAccelMix;

    DeltaRPM = static_cast<const Average &>(this->m_AvgDeltaRPM).GetValue();
    adt = this->m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();
    this->PercentOfAccelThreshold = 1.0f - (adt - bAbs(DeltaRPM + 10.0f)) / adt;
    this->PercentOfAccelThreshold = bClamp(this->PercentOfAccelThreshold, 0.0f, 1.0f);
    adt = this->m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold();
    this->PercentOfDecelThreshold =
        1.0f - (adt - bAbs(DeltaRPM + 10.0f)) / this->m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold();
    this->PercentOfDecelThreshold = bClamp(this->PercentOfDecelThreshold, 0.0f, 1.0f);

    if (!this->m_pEAXCar->GetPhysicsCTL()->NISRevingEnabled) {
        if (this->GetPhysCar()->IsShifting() || this->m_pShiftingCtl->IsActive()) {
            if (this->m_pShiftingCtl->eShiftState == SHFT_UP_ENGAGING) {
                USE_SMOOTHING = false;
            }
        } else if (this->m_pAccelTranCtl->IsActive()) {
            if (this->m_pAccelTranCtl->eAccelTransFxState == FX_ACCEL_STATE_ATTACK) {
                USE_SMOOTHING = false;
            }
        }
    }

    AccelMix.Aems = this->PercentOfAccelThreshold *
                        (this->m_pEAXCar->GetAttributes().AEMSMix_L_RPM() - this->m_pEAXCar->GetAttributes().AEMSMix_S_RPM()) +
                    this->m_pEAXCar->GetAttributes().AEMSMix_S_RPM();
    AccelMix.AccelGinsu =
        this->PercentOfAccelThreshold *
            (this->m_pEAXCar->GetAttributes().GINSUMix_L_RPM() - this->m_pEAXCar->GetAttributes().GINSUMix_S_RPM()) +
        this->m_pEAXCar->GetAttributes().GINSUMix_S_RPM();
    AccelMix.DecelGinsu = 0.0f;
    AccelMix.Cutoff = 25000;
    DecelMix.Aems = this->PercentOfDecelThreshold * (this->m_pEAXCar->GetAttributes().DECEL_AEMSMix_L_RPM() -
                                               this->m_pEAXCar->GetAttributes().DECEL_AEMSMix_S_RPM()) +
                    this->m_pEAXCar->GetAttributes().DECEL_AEMSMix_S_RPM();
    DecelMix.DecelGinsu =
        this->PercentOfDecelThreshold *
            (this->m_pEAXCar->GetAttributes().DECEL_GINSUMix_L_RPM() - this->m_pEAXCar->GetAttributes().DECEL_GINSUMix_S_RPM()) +
        this->m_pEAXCar->GetAttributes().DECEL_GINSUMix_S_RPM();
    DecelMix.Aems = this->DecelCrossfadeMix.GetValue(this->m_GinsuScaledRPM) * DecelMix.Aems +
                    (1.0f - this->DecelCrossfadeMix.GetValue(this->m_GinsuScaledRPM));
    DecelMix.DecelGinsu = this->DecelCrossfadeMix.GetValue(this->m_GinsuScaledRPM) * DecelMix.DecelGinsu;
    DecelMix.AccelGinsu =
        this->PercentOfDecelThreshold * (this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_L_RPM() -
                                   this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_L_RPM()) +
        this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_S_RPM();
    DecelMix.Cutoff = bMin(static_cast<int>(this->m_pEAXCar->GetAttributes().GINSU_LowPassCutoff()),
                           this->m_pEngineCtl->m_DistanceFltr);

    this->m_bAEMSLPF = false;
    EngineMix newmix;
    float EngineCtlVolFactor;
    int VolAEMS;
    int VolAccelGinsu;
    int VolDecelGinsu;
    int tempAEMVol;
    newmix.Aems = (AccelMix.Aems - DecelMix.Aems) * AccelDecelMix + DecelMix.Aems;
    newmix.DecelGinsu = (AccelMix.DecelGinsu - DecelMix.DecelGinsu) * AccelDecelMix + DecelMix.DecelGinsu;
    newmix.AccelGinsu = (AccelMix.AccelGinsu - DecelMix.AccelGinsu) * AccelDecelMix + DecelMix.AccelGinsu;
    newmix.Cutoff = static_cast<int>((static_cast<float>(AccelMix.Cutoff) - static_cast<float>(DecelMix.Cutoff)) *
                                         AccelDecelMix +
                                     static_cast<float>(DecelMix.Cutoff));

    if (USE_SMOOTHING) {
        this->m_EngineMix.Aems = smooth(this->m_EngineMix.Aems, newmix.Aems, 0.2f);
        this->m_EngineMix.AccelGinsu = smooth(this->m_EngineMix.AccelGinsu, newmix.AccelGinsu, 0.2f);
        this->m_EngineMix.DecelGinsu = smooth(this->m_EngineMix.DecelGinsu, newmix.DecelGinsu, 0.2f);
        this->m_EngineMix.Cutoff = smooth(this->m_EngineMix.Cutoff, newmix.Cutoff, 6000);
    } else {
        this->m_EngineMix = newmix;
    }

    this->m_GinsuLPFVal = static_cast<float>(NFSMixShape::GetCurveOutput(
        static_cast<NFSMixShape::eMIXTABLEID>(7), this->m_EngineMix.Cutoff, true));
    this->m_GinsuLPFVal = static_cast<float>(bMin(this->m_pEngineCtl->m_DistanceFltr, this->m_EngineMix.Cutoff));

    EngineCtlVolFactor = static_cast<float>(this->m_pEngineCtl->m_iEngineVol) * 3.051851e-05f;
    tempAEMVol = static_cast<int>(
        (static_cast<float>(this->m_pEAXCar->GetAttributes().AEMSVol()) -
         static_cast<float>(this->m_pEAXCar->GetAttributes().DECEL_AEMSVol())) * AccelDecelMix +
        static_cast<float>(this->m_pEAXCar->GetAttributes().DECEL_AEMSVol()));
    VolAEMS = static_cast<int>(this->m_EngineMix.Aems * static_cast<float>(tempAEMVol) * EngineCtlVolFactor);
    VolAccelGinsu = static_cast<int>(
        this->m_EngineMix.AccelGinsu * static_cast<float>(this->m_pEAXCar->GetAttributes().GINSUAccelVol()) * EngineCtlVolFactor);
    VolDecelGinsu = static_cast<int>(
        this->m_EngineMix.DecelGinsu * static_cast<float>(this->m_pEAXCar->GetAttributes().GinsuDecelVol()) * EngineCtlVolFactor);

    if (USE_SMOOTHING) {
        this->m_EngVolAEMS = smooth(this->m_EngVolAEMS, VolAEMS, 7000);
        this->m_EngVolAccelGinsu = smooth(this->m_EngVolAccelGinsu, VolAccelGinsu, 7000);
        this->m_EngVolDecelGinsu = smooth(this->m_EngVolDecelGinsu, VolDecelGinsu, 7000);
    } else {
        this->m_EngVolAEMS = VolAEMS;
        this->m_EngVolAccelGinsu = VolAccelGinsu;
        this->m_EngVolDecelGinsu = VolDecelGinsu;
    }

    this->m_EngVolRedLine = static_cast<int>(static_cast<float>(this->m_pEAXCar->GetAttributes().AEMSVol()) * EngineCtlVolFactor);
}

void SFXCTL_HybridMotor::UpdateVolumeRedlining() {
    this->m_EngVolRedLine =
        static_cast<int>(static_cast<float>(this->m_EngVolRedLine) * this->m_pEngineCtl->RedLineSampFactor.GetValue());
    this->m_EngVolAEMS =
        static_cast<int>(static_cast<float>(this->m_EngVolAEMS) * this->m_pEngineCtl->RedLineEngFactor.GetValue());
    this->m_EngVolAccelGinsu =
        static_cast<int>(static_cast<float>(this->m_EngVolAccelGinsu) * this->m_pEngineCtl->RedLineEngFactor.GetValue());
    this->m_EngVolDecelGinsu =
        static_cast<int>(static_cast<float>(this->m_EngVolDecelGinsu) * this->m_pEngineCtl->RedLineEngFactor.GetValue());
}

void SFXCTL_HybridMotor::UpdateMixerOutputs() {
    bool bOutputOn = false;
    float AvgDeltaRPM;
    float PercentOfThreshold;
    int output;
    float speedMph = this->m_pEAXCar->GetVelocityMagnitudeMPH();

    if (speedMph <= 30.0f || 30.0f <= bAbs(this->m_AvgDeltaRPM.GetValue())) {
        this->tSteadyDuration = SndBase::m_fRunningTime;
    } else {
        bOutputOn = this->tSteadyDuration + 3.0f < SndBase::m_fRunningTime;
    }

    if (!bOutputOn) {
        this->tSteadyDuration = SndBase::m_fRunningTime;
    } else {
        if (this->SteadyFrameCnt == 0) {
            this->SteadyFrameCnt = static_cast<unsigned short>(g_pEAXSound->Random(0x96) + 0x3C);
            this->m_pEngineCtl->bPlayCompression = true;
        }
        this->SteadyFrameCnt = static_cast<unsigned short>(this->SteadyFrameCnt - 1);
    }

    output = smooth(this->GetDMIX_InputValue(0), bOutputOn ? 0x7FFF : 0, 0x3D7, 0xC4);
    this->SetDMIX_Input(0, output);

    AvgDeltaRPM = this->m_AvgDeltaRPM.GetLastRecordedValue();
    float accelThreshold = this->m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();
    PercentOfThreshold = 1.0f - (accelThreshold - bAbs(AvgDeltaRPM)) / accelThreshold;
    PercentOfThreshold = bClamp(PercentOfThreshold, 0.0f, 1.0f);
    output = smooth(this->GetDMIX_InputValue(1), static_cast<int>(PercentOfThreshold * 32767.0f), 3000);

    if (!this->m_pEngineCtl->bWasRedlining &&
        this->m_pEngineCtl->bIsRedlining) {
        this->mPrevDeltaRPM = output / 2;
    }

    {
        int shiftActive = 1;

        if (this->m_pShiftingCtl->eShiftState == SHFT_NONE) {
            shiftActive = 0;
        }

        if (shiftActive == 0) {
        EAX_CarState *physCar = this->m_pStateBase->GetPhysCar();
        int wheelsOnGround = 0;

            for (int i = 0; i < 4; i++) {
                if (physCar->mWheel[i].mWheelOnGround != 0) {
                    wheelsOnGround++;
                }
            }

            if (0.0f < static_cast<float>(wheelsOnGround)) {
                this->SetDMIX_Input(1, output);
            }
        }
    }

    if (this->m_pEngineCtl->bIsRedlining) {
        this->SetDMIX_Input(1, this->mPrevDeltaRPM);
    }
}
