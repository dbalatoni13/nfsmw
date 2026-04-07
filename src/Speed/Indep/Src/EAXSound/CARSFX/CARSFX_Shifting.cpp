#include "./CARSFX_Shifting.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

namespace Csis {
InterfaceId CAR_SWTNId = {"CAR_SWTN", 0x383F, 0x7859};
ClassHandle gCAR_SWTNHandle;
InterfaceId CAR_WHINEId = {"CAR_WHINE", 0x383F, 0x603F};
ClassHandle gCAR_WHINEHandle;
InterfaceId FX_SHIFTING_01Id = {"FX_SHIFTING_01", 0x3EE6, 0x7937};
ClassHandle gFX_SHIFTING_01Handle;
} // namespace Csis

CARSFX_Shift::TypeInfo CARSFX_Shift::s_TypeInfo = {
    0x00020030,
    "CARSFX_Shift",
    &SndBase::s_TypeInfo,
    CARSFX_Shift::CreateObject,
};

CARSFX_Shift::TypeInfo *CARSFX_Shift::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_Shift::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_Shift::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (s_TypeInfo.typeName, false) CARSFX_Shift();
    }
    return new (s_TypeInfo.typeName, true) CARSFX_Shift();
}

CARSFX_Shift::CARSFX_Shift()
    : CARSFX() {
    m_pSweetnersData = nullptr;
    m_ShiftGear = nullptr;
    m_BrakePedal = nullptr;
    m_Disengage = nullptr;
    m_Engage = nullptr;
    m_AccelSnd = nullptr;
    m_DeccelSnd = nullptr;
    m_Whine = nullptr;
    m_pShiftCtl = nullptr;
    m_pShiftingPatternData = nullptr;
}

CARSFX_Shift::~CARSFX_Shift() {
    Destroy();
}

void CARSFX_Shift::UpdateMixerOutputs() {
    if (m_pShiftCtl->eShiftState == SHFT_UP_DISENGAGE) {
        SetDMIX_Input(7, 0x7FFF);
    } else {
        SetDMIX_Input(7, 0);
    }
}

void CARSFX_Shift::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pSweetnersData = &m_pEAXCar->GetAttributes();
    m_UGL = m_pEAXCar->m_TransmissionUGL;
    m_pShiftingPatternData = &m_pEAXCar->GetShiftInfo();
}

void CARSFX_Shift::InitSFX() {
    SndBase::InitSFX();
}

void CARSFX_Shift::Destroy() {
    if (m_DeccelSnd) {
        delete m_DeccelSnd;
        m_DeccelSnd = nullptr;
    }

    if (m_AccelSnd) {
        delete m_AccelSnd;
        m_AccelSnd = nullptr;
    }

    if (m_ShiftGear) {
        delete m_ShiftGear;
        m_ShiftGear = nullptr;
    }

    if (m_BrakePedal) {
        delete m_BrakePedal;
        m_BrakePedal = nullptr;
    }

    if (m_Disengage) {
        delete m_Disengage;
        m_Disengage = nullptr;
    }

    if (m_Engage) {
        delete m_Engage;
        m_Engage = nullptr;
    }

    if (m_Whine) {
        delete m_Whine;
        m_Whine = nullptr;
    }
}

int CARSFX_Shift::GetController(int Index) {
    switch (Index) {
    case 0:
        return 2;
    case 1:
        return 3;
    case 2:
        return 7;
    default:
        return -1;
    }
}

void CARSFX_Shift::AttachController(SFXCTL *psfxctl) {
    int nindex;

    nindex = psfxctl->GetObjectIndex();
    switch (nindex) {
    case 2:
        m_pShiftCtl = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    case 3:
        m_pAccelTransCtl = static_cast<SFXCTL_AccelTrans *>(psfxctl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    }
}

void CARSFX_Shift::SetupLoadData() {
    unsigned int i;

    if (EAXCar::g_ShiftInfo) {
        LoadAsset(EAXCar::g_ShiftInfo->BankName(), SNDPATH_SHIFT, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    } else {
        LoadAsset(m_pEAXCar->GetShiftInfo().BankName(), SNDPATH_SHIFT, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    }

    for (i = 0; i < m_pEAXCar->GetAttributes().Num_SweetBank(); i++) {
        LoadAsset(m_pEAXCar->GetAttributes().SweetBank(i), SNDPATH_ENGINE, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
    }
}
