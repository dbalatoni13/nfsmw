#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"

SFXCTL_Shifting::~SFXCTL_Shifting() {}

SndBase::TypeInfo *SFXCTL_Shifting::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_Shifting::GetTypeName() const { return s_TypeInfo.typeName; }

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
    m_pShiftingPatternData = reinterpret_cast<shiftpattern *>(reinterpret_cast<char *>(m_pEAXCar) + 0xd8);
    m_nPostShiftFXLevel = ugl % 2;
}

void SFXCTL_Shifting::AttachController(SFXCTL *ctrl) {
    if (((ctrl->GetTypeInfo()->ObjectID >> 4) & 0xFFF) == 4) {
        m_pEngineCtl = static_cast<SFXCTL_Engine *>(ctrl);
    }
}

void SFXCTL_Shifting::UpdateMixerOutputs() {
    int upDisengage = (eShiftState == SHFT_UP_DISENGAGE) ? 0x7FFF : 0;
    SetDMIX_Input(0, upDisengage);
    int upEngaging = (eShiftStageChanged == SHFT_UP_ENGAGING) ? 0x7FFF : 0;
    SetDMIX_Input(1, upEngaging);
    int downShifting = IsDownShifting() ? 0x7FFF : 0;
    SetDMIX_Input(2, downShifting);
}

void SFXCTL_Shifting::UpdateGearShiftState(float t) {
    if (m_pEngineCtl == nullptr || m_pEngineCtl->m_pPhysicsCtl == nullptr) {
        CleanUpShiftFX();
        return;
    }

    Gear curGear = GetCurGear();
    Gear lastGear = GetLastGear();
    if (curGear != lastGear) {
        eShiftState = (curGear > lastGear) ? SHFT_UP_DISENGAGE : SHFT_DOWN_DISENGAGE;
        eShiftStageChanged = eShiftState;
        m_bPendingNeedShiftSound = true;
        tShiftDelay = 0.1f;
        RPM_AtShift = m_pEngineCtl->m_pPhysicsCtl->PhysicsRPM;
    }

    if (tShiftDelay > 0.0f) {
        tShiftDelay -= t;
        if (tShiftDelay <= 0.0f) {
            eShiftState = IsDownShifting() ? SHFT_DOWN_ENGAGING_RISE : SHFT_UP_ENGAGING;
            eShiftStageChanged = eShiftState;
        }
    }

    m_InterpShiftRPM.Update(t, m_pEngineCtl->m_fEng_RPM);
    m_InterpShiftTorque.Update(t, IsDownShifting() ? 0.5f : 1.0f);
    m_InterpShiftVol.Update(t, IsDownShifting() ? 0.6f : 1.0f);

    if (m_bPendingNeedShiftSound && tShiftDelay <= 0.0f) {
        m_bNeed_ShiftGearSnd = true;
        m_bPendingNeedShiftSound = false;
    }

    if (eShiftState == SHFT_UP_ENGAGING || eShiftState == SHFT_DOWN_ENGAGING_REATTACH) {
        eShiftState = SHFT_NONE;
        eShiftStageChanged = SHFT_NONE;
    }
}
