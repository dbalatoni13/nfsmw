#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"

SFXCTL_AccelTrans::~SFXCTL_AccelTrans() {}

SndBase::TypeInfo *SFXCTL_AccelTrans::GetTypeInfo() const { return &s_TypeInfo; }

char *SFXCTL_AccelTrans::GetTypeName() const { return s_TypeInfo.typeName; }

void SFXCTL_AccelTrans::Destroy() {}

int SFXCTL_AccelTrans::GetController(int Index) {
    if (Index == 1) {
        goto ReturnTwo;
    }
    if (Index > 1) {
        goto ReturnNegOne;
    }
    if (Index != 0) {
        goto ReturnNegOne;
    }
    return 4;

ReturnTwo:
    return 2;

ReturnNegOne:
    return -1;
}

void SFXCTL_AccelTrans::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = static_cast<eAemsUpgradeLevel>(m_pEAXCar->GetEngineUpgradeLevel());
    m_pAccelTransDataSet = reinterpret_cast<acceltrans *>(reinterpret_cast<char *>(m_pEAXCar) + 0x100);
}

void SFXCTL_AccelTrans::InitSFX() {
    SFXCTL::InitSFX();
    eAccelTransFxState = 0;
    t_LastAccelTrans = 0.0f;
    IsAccelerating = false;
    OldIsAccelerating = false;
    PlayEngOffSweet = false;
}

void SFXCTL_AccelTrans::AttachController(SFXCTL *ctrl) {
    int id = (ctrl->GetTypeInfo()->ObjectID >> 4) & 0xFFF;
    if (id != 2) {
        if (id == 4) {
            m_pEngineCtl = static_cast<SFXCTL_Engine *>(ctrl);
        }
    } else {
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(ctrl);
    }
}
