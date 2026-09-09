#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/OldSoundTemplates.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"

int SPEW_HYBRID_MIX = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 20

int DISPLAY_HYBRID_MIX = 0;               // size: 0x4, address: 0xFFFFFFFF, Decl: 25
static const int SPEW_GINSU_MIX_INFO = 0; // size: 0x4, Decl: 26

static const float SMOOTHNESS_FACTOR = 0.2f; // size: 0x4, Decl: 38

static const float GINSU_SHIFT_ATTACK_PUNCH = 1.0f; // size: 0x4, Decl: 40 // TODO use

int HARDCODE_RPM = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 43

static const int LPFSmoothing = 6000; // size: 0x4, Decl: 46

DEFINE_CREATABLE(0x20050, SFXCTL_HybridMotor, SFXCTL);

SFXCTL_HybridMotor::SFXCTL_HybridMotor()
    : DecelCrossfadeMix(CrossFadesPoints, 5), //
      m_AvgDeltaRPM(4) {
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
#ifndef SANE_CODE
    this->CrossFadesPoints[5] = bVector2(0.0f, 0.0f);
#endif
    this->SteadyFrameCnt = 0;
    this->mPrevDeltaRPM = 0;
}

SFXCTL_HybridMotor::~SFXCTL_HybridMotor() {}

void SFXCTL_HybridMotor::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = this->m_pEAXCar->GetEngineUpgradeLevel();

    int MaxMinDif = this->m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM() - this->m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM();

    this->CrossFadesPoints[1].x = this->m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM();
    this->CrossFadesPoints[1].y = 0.0f;

    this->CrossFadesPoints[2].x =
        this->m_pEAXCar->GetAttributes().GINSU_Decel_MinRPM() + MaxMinDif * this->m_pEAXCar->GetAttributes().GINSU_DECEL_FADE_OUT();
    this->CrossFadesPoints[2].y = 1.0f;

    this->CrossFadesPoints[3].x =
        this->m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM() - MaxMinDif * this->m_pEAXCar->GetAttributes().GINSU_DECEL_FADE_IN();
    this->CrossFadesPoints[3].y = 1.0f;

    this->CrossFadesPoints[4].x = this->m_pEAXCar->GetAttributes().GINSU_Decel_MaxRPM();
    this->CrossFadesPoints[4].y = 0.0f;
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

    this->m_CurPhyDeltaRPMVal = this->GetPhysRPM() - this->PrevRPM;

    this->m_CurAudDeltaRPMVal = this->m_pEngineCtl->m_fEng_RPM - this->m_pEngineCtl->m_fPrevRPM;
    this->PrevRPM = this->GetPhysRPM();

    if (this->m_pEAXCar->GetPhysicsCTL()->NISRevingEnabled) {
        this->m_AvgDeltaRPM.Record(this->m_CurAudDeltaRPMVal);
    } else if (this->m_pAccelTranCtl->IsActive()) {
        if (this->m_pAccelTranCtl->eAccelTransFxState == FX_ACCEL_STATE_ATTACK) {
            this->m_AvgDeltaRPM.Flush(this->m_CurPhyDeltaRPMVal);
        } else {
            this->m_AvgDeltaRPM.Flush(this->m_CurAudDeltaRPMVal);
        }
    } else if (this->m_pShiftingCtl->IsActive()) {
        if (this->m_pShiftingCtl->eShiftState == SHFT_DOWN_ENGAGING_RISE) {
            this->m_AvgDeltaRPM.Flush(this->m_CurAudDeltaRPMVal);
        } else if (this->m_pShiftingCtl->eShiftStageChanged == SHFT_UP_ENGAGING) {
            this->m_AvgDeltaRPM.Flush(bAbs(this->m_CurPhyDeltaRPMVal));
        } else {
            if (this->m_pShiftingCtl->eShiftState == SHFT_UP_ENGAGING || this->m_pShiftingCtl->eShiftState == SHFT_UP_LFO) {
                this->m_AvgDeltaRPM.Record(bAbs(this->m_CurAudDeltaRPMVal));
            } else {
                this->m_AvgDeltaRPM.Record(this->m_CurAudDeltaRPMVal);
            }
        }
    } else {
        this->m_AvgDeltaRPM.Record(this->m_CurPhyDeltaRPMVal);
    }

    this->m_AvgDeltaRPM.Recalculate();
}

// Decl: 233
// I had to add m_pEAXCar-> to them
#define THRESHOLDMIX(Threshold, ENG)                                                                                                                 \
    Threshold *(this->m_pEAXCar->GetAttributes().##ENG##Mix_L_RPM() - this->m_pEAXCar->GetAttributes().##ENG##Mix_S_RPM()) +                         \
        this->m_pEAXCar->GetAttributes().##ENG##Mix_S_RPM();

// Decl: 237
#define THRESHOLDMIX_DECEL(Threshold, ENG)                                                                                                           \
    Threshold *(this->m_pEAXCar->GetAttributes().DECEL_##ENG##Mix_L_RPM() - this->m_pEAXCar->GetAttributes().DECEL_##ENG##Mix_S_RPM()) +             \
        this->m_pEAXCar->GetAttributes().DECEL_##ENG##Mix_S_RPM()

#define INTERPOLATE(start, finish, tee) ((finish - start) * tee + start) // Decl: 259

void SFXCTL_HybridMotor::UpdateParams(float t) {
    if (!this->m_pEAXCar->GetAttributes().IsValid()) {
        return;
    }

    SFXCTL::UpdateParams(t);
    this->UpdateDeltaRPM();

    float AemsRPM = this->m_pEngineCtl->GetEngRPM();
    AemsRPM = bClamp(AemsRPM, 1000.0f, 10000.0f);

    this->m_GinsuScaledRPM =
        ((AemsRPM - 1000.0f) / 9000.0f) * (this->m_pEAXCar->GetAttributes().MaxRPM() - this->m_pEAXCar->GetAttributes().MinRPM()) +
        this->m_pEAXCar->GetAttributes().MinRPM();

    if (this->m_pEAXCar->m_EngineType == eGINSU_ENG_SINGLE) {
        this->UpdateSingleMixEng(t);
    } else {
        this->UpdateDualMixEng(t);
    }

    this->UpdateVolumeRedlining();
}

#define FILTER_SCALE(factor, value) (1.0f - factor) + factor *value; // Decl: 338

static const int MAX_VOL_CHANGE = 7000;      // size: 0x4, Decl: 353
static const float DELTA_RPM_OFFSET = 10.0f; // size: 0x4, Decl: 354
static const float TRQ_THRESHOLD = 35.0f;    // size: 0x4, Decl: 355

void SFXCTL_HybridMotor::UpdateSingleMixEng(float t) {
    Slope TrqThreshold(0.0f, 1.0f, 0.0f, TRQ_THRESHOLD);
    float AccelDecelMix = TrqThreshold.GetValue(this->m_pEngineCtl->GetEngTorque());
    bool USE_SMOOTHING = true;
    EngineMix AccelMix;
    EngineMix DecelMix;

    float DeltaRPM = this->m_AvgDeltaRPM.GetValue();
    float adt = this->m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();

    this->PercentOfAccelThreshold = 1.0f - (adt - bAbs(DeltaRPM + DELTA_RPM_OFFSET)) / adt;
    this->PercentOfAccelThreshold = bClamp(this->PercentOfAccelThreshold, 0.0f, 1.0f);
    this->PercentOfDecelThreshold = 1.0f - (this->m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold() - bAbs(DeltaRPM + DELTA_RPM_OFFSET)) /
                                               this->m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold();
    this->PercentOfDecelThreshold = bClamp(this->PercentOfDecelThreshold, 0.0f, 1.0f);

    if (this->GetPhysCar()->IsShifting() || this->m_pShiftingCtl->IsActive()) {
        switch (this->m_pShiftingCtl->eShiftState) {
            case SHFT_UP_ENGAGING:
                USE_SMOOTHING = false;
                break;
            case SHFT_UP_LFO:
                break;
            case SHFT_DOWN_ENGAGING_RISE:
                break;
            case SHFT_DOWN_ENGAGING_REATTACH:
                if (this->m_pEAXCar->IsAccelerating()) {
                    USE_SMOOTHING = false;
                }
                break;
            default:
                break;
        }
    } else if (this->m_pAccelTranCtl->IsActive()) {
        if (this->m_pAccelTranCtl->eAccelTransFxState == FX_ACCEL_STATE_ATTACK) {
            USE_SMOOTHING = false;
        }
    }

    AccelMix.Aems = THRESHOLDMIX(this->PercentOfAccelThreshold, AEMS);
    AccelMix.AccelGinsu = THRESHOLDMIX(this->PercentOfAccelThreshold, GINSU);
    AccelMix.DecelGinsu = 0.0f;
    AccelMix.Cutoff = this->m_pEngineCtl->m_DistanceFltr;

    DecelMix.Aems = THRESHOLDMIX_DECEL(this->PercentOfDecelThreshold, AEMS);
    DecelMix.AccelGinsu = this->PercentOfDecelThreshold *
                              (this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_L_RPM() - this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_L_RPM()) +
                          this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_S_RPM();
    AccelMix.DecelGinsu = 0.0f;
    DecelMix.Cutoff = bMin(this->m_pEAXCar->GetAttributes().GINSU_LowPassCutoff(), this->m_pEngineCtl->m_DistanceFltr);

    EngineMix newmix;
    newmix.Aems = INTERPOLATE(DecelMix.Aems, AccelMix.Aems, AccelDecelMix);
    newmix.DecelGinsu = INTERPOLATE(DecelMix.DecelGinsu, AccelMix.DecelGinsu, AccelDecelMix);
    newmix.AccelGinsu = INTERPOLATE(DecelMix.AccelGinsu, AccelMix.AccelGinsu, AccelDecelMix);
    newmix.Cutoff = static_cast<int>(INTERPOLATE(static_cast<float>(DecelMix.Cutoff), static_cast<float>(AccelMix.Cutoff), AccelDecelMix));

    if (USE_SMOOTHING) {
        this->m_EngineMix.Aems = smooth(newmix.Aems, newmix.Aems, SMOOTHNESS_FACTOR);
        this->m_EngineMix.AccelGinsu = smooth(newmix.AccelGinsu, newmix.AccelGinsu, SMOOTHNESS_FACTOR);
        this->m_EngineMix.DecelGinsu = smooth(newmix.DecelGinsu, newmix.DecelGinsu, SMOOTHNESS_FACTOR);
        this->m_EngineMix.Cutoff = smooth(newmix.Cutoff, newmix.Cutoff, LPFSmoothing);
    } else {
        this->m_EngineMix = newmix;
    }

    this->m_GinsuLPFVal = static_cast<float>(
        25000 - NFSMixShape::GetCurveOutput(SHAPE_UP_ONE_MIN_EQPWR_SQ, static_cast<int>(this->m_EngineMix.Cutoff * 1.3106799f), true));

    float EngineCtlVolFactor = static_cast<float>(this->m_pEngineCtl->m_iEngineVol) / 32767.0f;

    int VolAEMS = static_cast<int>(this->m_EngineMix.Aems * this->m_pEAXCar->GetAttributes().AEMSVol() * EngineCtlVolFactor);
    int VolAccelGinsu = static_cast<int>(this->m_EngineMix.AccelGinsu * this->m_pEAXCar->GetAttributes().GINSUAccelVol() * EngineCtlVolFactor);
    int VolDecelGinsu = static_cast<int>(this->m_EngineMix.DecelGinsu * this->m_pEAXCar->GetAttributes().GinsuDecelVol() * EngineCtlVolFactor);

    if (USE_SMOOTHING) {
        this->m_EngVolAEMS = smooth(this->m_EngVolAEMS, VolAEMS, MAX_VOL_CHANGE);
        this->m_EngVolAccelGinsu = smooth(this->m_EngVolAccelGinsu, VolAccelGinsu, MAX_VOL_CHANGE);
        this->m_EngVolDecelGinsu = smooth(this->m_EngVolDecelGinsu, VolDecelGinsu, MAX_VOL_CHANGE);
    } else {
        this->m_EngVolAEMS = VolAEMS;
        this->m_EngVolAccelGinsu = VolAccelGinsu;
        this->m_EngVolDecelGinsu = VolDecelGinsu;
    }

    this->m_EngVolRedLine = static_cast<int>(this->m_pEAXCar->GetAttributes().AEMSVol() * EngineCtlVolFactor);
}

void SFXCTL_HybridMotor::UpdateDualMixEng(float t) {
    Slope TrqThreshold(0.0f, 1.0f, 0.0f, TRQ_THRESHOLD);
    float AccelDecelMix = TrqThreshold.GetValue(this->m_pEngineCtl->GetEngTorque());
    bool USE_SMOOTHING = true;
    EngineMix AccelMix;
    EngineMix DecelMix;

    float DeltaRPM = this->m_AvgDeltaRPM.GetValue();
    float adt = this->m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();
    this->PercentOfAccelThreshold = 1.0f - (adt - bAbs(DeltaRPM + DELTA_RPM_OFFSET)) / adt;
    this->PercentOfAccelThreshold = bClamp(this->PercentOfAccelThreshold, 0.0f, 1.0f);
    this->PercentOfDecelThreshold = 1.0f - (this->m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold() - bAbs(DeltaRPM + DELTA_RPM_OFFSET)) /
                                               this->m_pEAXCar->GetAttributes().DecelDeltaRPMThreshold();
    this->PercentOfDecelThreshold = bClamp(this->PercentOfDecelThreshold, 0.0f, 1.0f);

    bool UseAccelMix;

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

    AccelMix.Aems = THRESHOLDMIX(this->PercentOfAccelThreshold, AEMS);
    AccelMix.AccelGinsu = THRESHOLDMIX(this->PercentOfAccelThreshold, GINSU);
    AccelMix.DecelGinsu = 0.0f;
    AccelMix.Cutoff = 25000;

    DecelMix.Aems = THRESHOLDMIX_DECEL(this->PercentOfDecelThreshold, AEMS);
    DecelMix.DecelGinsu = THRESHOLDMIX_DECEL(this->PercentOfDecelThreshold, GINSU);
    DecelMix.Aems = FILTER_SCALE(this->DecelCrossfadeMix.GetValue(this->m_GinsuScaledRPM), DecelMix.Aems);
    DecelMix.DecelGinsu = this->DecelCrossfadeMix.GetValue(this->m_GinsuScaledRPM) * DecelMix.DecelGinsu;
    DecelMix.AccelGinsu = this->PercentOfDecelThreshold *
                              (this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_L_RPM() - this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_L_RPM()) +
                          this->m_pEAXCar->GetAttributes().Ginsu_ACL_Neg_S_RPM();
    DecelMix.Cutoff = bMin(static_cast<int>(this->m_pEAXCar->GetAttributes().GINSU_LowPassCutoff()), this->m_pEngineCtl->m_DistanceFltr);

    this->m_bAEMSLPF = false;
    EngineMix newmix;
    float EngineCtlVolFactor;
    newmix.Aems = INTERPOLATE(DecelMix.Aems, AccelMix.Aems, AccelDecelMix);
    newmix.DecelGinsu = INTERPOLATE(DecelMix.DecelGinsu, AccelMix.DecelGinsu, AccelDecelMix);
    newmix.AccelGinsu = INTERPOLATE(DecelMix.AccelGinsu, AccelMix.AccelGinsu, AccelDecelMix);
    newmix.Cutoff = static_cast<int>(INTERPOLATE(static_cast<float>(DecelMix.Cutoff), static_cast<float>(AccelMix.Cutoff), AccelDecelMix));

    if (USE_SMOOTHING) {
        this->m_EngineMix.Aems = smooth(this->m_EngineMix.Aems, newmix.Aems, SMOOTHNESS_FACTOR);
        this->m_EngineMix.AccelGinsu = smooth(this->m_EngineMix.AccelGinsu, newmix.AccelGinsu, SMOOTHNESS_FACTOR);
        this->m_EngineMix.DecelGinsu = smooth(this->m_EngineMix.DecelGinsu, newmix.DecelGinsu, SMOOTHNESS_FACTOR);
        this->m_EngineMix.Cutoff = smooth(this->m_EngineMix.Cutoff, newmix.Cutoff, LPFSmoothing);
    } else {
        this->m_EngineMix = newmix;
    }

    this->m_GinsuLPFVal = static_cast<float>(NFSMixShape::GetCurveOutput(SHAPE_UP_ONE_MIN_EQPWR_SQ, this->m_EngineMix.Cutoff, true));
    this->m_GinsuLPFVal = static_cast<float>(bMin(this->m_EngineMix.Cutoff, this->m_pEngineCtl->m_DistanceFltr));

    EngineCtlVolFactor = static_cast<float>(this->m_pEngineCtl->m_iEngineVol) / 32767.0f;

    int VolAEMS;
    int VolAccelGinsu;
    int VolDecelGinsu;
    int tempAEMVol = static_cast<int>(INTERPOLATE(static_cast<float>(this->m_pEAXCar->GetAttributes().DECEL_AEMSVol()),
                                                  static_cast<float>(this->m_pEAXCar->GetAttributes().AEMSVol()), AccelDecelMix));
    VolAEMS = static_cast<int>(this->m_EngineMix.Aems * tempAEMVol * EngineCtlVolFactor);
    VolAccelGinsu = static_cast<int>(this->m_EngineMix.AccelGinsu * this->m_pEAXCar->GetAttributes().GINSUAccelVol() * EngineCtlVolFactor);
    VolDecelGinsu = static_cast<int>(this->m_EngineMix.DecelGinsu * this->m_pEAXCar->GetAttributes().GinsuDecelVol() * EngineCtlVolFactor);

    if (USE_SMOOTHING) {
        this->m_EngVolAEMS = smooth(this->m_EngVolAEMS, VolAEMS, MAX_VOL_CHANGE);
        this->m_EngVolAccelGinsu = smooth(this->m_EngVolAccelGinsu, VolAccelGinsu, MAX_VOL_CHANGE);
        this->m_EngVolDecelGinsu = smooth(this->m_EngVolDecelGinsu, VolDecelGinsu, MAX_VOL_CHANGE);
    } else {
        this->m_EngVolAEMS = VolAEMS;
        this->m_EngVolAccelGinsu = VolAccelGinsu;
        this->m_EngVolDecelGinsu = VolDecelGinsu;
    }

    this->m_EngVolRedLine = static_cast<int>(static_cast<float>(this->m_pEAXCar->GetAttributes().AEMSVol()) * EngineCtlVolFactor);
}

static const int DEBUG_REDLINING = 0; // size: 0x4, Decl: 708

void SFXCTL_HybridMotor::UpdateVolumeRedlining() {
    this->m_EngVolRedLine = static_cast<int>(this->m_EngVolRedLine * this->m_pEngineCtl->RedLineSampFactor.GetValue());
    this->m_EngVolAEMS = static_cast<int>(this->m_EngVolAEMS * this->m_pEngineCtl->RedLineEngFactor.GetValue());
    this->m_EngVolAccelGinsu = static_cast<int>(this->m_EngVolAccelGinsu * this->m_pEngineCtl->RedLineEngFactor.GetValue());
    this->m_EngVolDecelGinsu = static_cast<int>(this->m_EngVolDecelGinsu * this->m_pEngineCtl->RedLineEngFactor.GetValue());
}

static int Smoother_Up_Steady_State;   // size: 0x4, Decl: 733
static int Smoother_Down_Steady_State; // size: 0x4, Decl: 734
static int Smoother_DeltaRPM;          // size: 0x4, Decl: 735

static int MAX_NUM_FRAME_BETWEEN_COMPRESS; // size: 0x4, Decl: 738

static int MIN_NUM_FRAME_BETWEEN_COMPRESS; // size: 0x4, Decl: 739

void SFXCTL_HybridMotor::UpdateMixerOutputs() {
    bool bOutputOn = false;
    if ((this->m_pEAXCar->GetVelocityMagnitudeMPH() > 30.0f) && (bAbs(this->m_AvgDeltaRPM.GetValue()) < 30.0f)) {
        bOutputOn = SndBase::m_fRunningTime > this->tSteadyDuration + 3.0f;
    } else {
        this->tSteadyDuration = SndBase::m_fRunningTime;
    }

    if (bOutputOn) {
        if (this->SteadyFrameCnt == 0) {
            this->SteadyFrameCnt = static_cast<unsigned short>(g_pEAXSound->Random(150) + 60);
            this->m_pEngineCtl->bPlayCompression = true;
        }

        this->SteadyFrameCnt--;
    } else {
        this->tSteadyDuration = SndBase::m_fRunningTime;
    }

    this->SetDMIX_Input(0, smooth(this->GetDMIX_InputValue(0), bOutputOn ? 0x7FFF : 0, 983, 196));

    float AvgDeltaRPM = this->m_AvgDeltaRPM.GetLastRecordedValue();
    float PercentOfThreshold = 1.0f - (this->m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold() - bAbs(AvgDeltaRPM)) /
                                          this->m_pEAXCar->GetAttributes().AccelDeltaRPMThreshold();
    PercentOfThreshold = bClamp(PercentOfThreshold, 0.0f, 1.0f);

    int output = smooth(this->GetDMIX_InputValue(1), static_cast<int>(PercentOfThreshold * 32767.0f), 3000);

    if (!this->m_pEngineCtl->bWasRedlining && this->m_pEngineCtl->bIsRedlining) {
        this->mPrevDeltaRPM = output / 2;
    }

    if (!this->m_pShiftingCtl->IsActive()) {
        if (0.0f < static_cast<float>(this->GetPhysCar()->GetWheelsOnGround())) {
            this->SetDMIX_Input(1, output);
        }
    }

    if (this->m_pEngineCtl->bIsRedlining) {
        this->SetDMIX_Input(1, this->mPrevDeltaRPM);
    }
}
