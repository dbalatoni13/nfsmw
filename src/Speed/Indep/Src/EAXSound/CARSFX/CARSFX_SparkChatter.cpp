#include "./CARSFX_SparkChatter.hpp"

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"

namespace Csis {
InterfaceId CAR_SputterId = {"CAR_Sputter", 0x383F, 0x3686};
ClassHandle gCAR_SputterHandle;
InterfaceId CAR_SputOutputId = {"CAR_SputOutput", 0x383F, 0x1901};
ClassHandle gCAR_SputOutputHandle;
} // namespace Csis

CARSFX_SparkChatter::TypeInfo CARSFX_SparkChatter::s_TypeInfo = {
    0x00020060,
    "CARSFX_SparkChatter",
    &SndBase::s_TypeInfo,
    CARSFX_SparkChatter::CreateObject,
};

CARSFX_SparkChatter::TypeInfo *CARSFX_SparkChatter::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *CARSFX_SparkChatter::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *CARSFX_SparkChatter::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) CARSFX_SparkChatter();
    }
    return new (GetStaticTypeInfo()->typeName, true) CARSFX_SparkChatter();
}

CARSFX_SparkChatter::CARSFX_SparkChatter()
    : CARSFX() {
    SparkChatOutputInstance *pSVar1 = &SparkChatOutputClients;

    SparkChatOutputClients.DestroyClient.pClientData = pSVar1;
    SparkChatOutputClients.CreateClient.pClientFunc = SparkChatCreateCallBack;
    SparkChatOutputClients.DestroyClient.pClientFunc = SparkChatDestroyCallBack;
    BlipVol = 0;
    m_pSparkChatterControl = nullptr;
    m_pSparkChatterOutput = nullptr;
    m_pShiftingCTL = nullptr;
    SparkChatOutputClients.m_pThis = this;
    SparkChatOutputClients.CreateClient.pClientData = pSVar1;
    SparkChatOutputClients.UpdateClient.pClientData = pSVar1;
    m_pSweetnersData = nullptr;
}

CARSFX_SparkChatter::~CARSFX_SparkChatter() {
    Destroy();
}

int CARSFX_SparkChatter::GetController(int Index) {
    switch (Index) {
    case 0:
        return 2;
    case 1:
        return 7;
    default:
        return -1;
    }
}

void CARSFX_SparkChatter::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
    case 2:
        m_pShiftingCTL = static_cast<SFXCTL_Shifting *>(psfxctl);
        break;
    case 7:
        m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
        break;
    }
}

void CARSFX_SparkChatter::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_UGL = m_pEAXCar->GetEngineUpgradeLevel();
    m_pSweetnersData = &m_pEAXCar->GetAttributes();
}

void CARSFX_SparkChatter::InitSFX() {
    SndBase::InitSFX();
    g_pEAXSound->SetCsisName(this);
    m_pSparkChatterControl =
        new Csis::CAR_Sputter(static_cast<int>(m_pEAXCar->mEngineInfo.CarID()), reinterpret_cast<int>(this), 0, 0, 0, 0, 0, 0, 0, 0, 0);
    static int tmp_refCnt = m_pSparkChatterControl->GetRefCount();
}

void CARSFX_SparkChatter::SparkChatCreateCallBack(Csis::Class *pSparkChatClass, Csis::Parameter *pParameters, void *pClientData) {
    SparkChatOutputInstance *pSparkChatInstance = static_cast<SparkChatOutputInstance *>(pClientData);
    Csis::CAR_SputOutputStruct *pSparkData = reinterpret_cast<Csis::CAR_SputOutputStruct *>(pParameters);

    if (reinterpret_cast<Csis::Parameter *>(pSparkData)[2].iVal == reinterpret_cast<int>(pSparkChatInstance->m_pThis)) {
        Csis::Class::UnsubscribeConstructor(&Csis::gCAR_SputOutputHandle, &pSparkChatInstance->CreateClient);
        pSparkChatInstance->UpdateClient.pClientFunc = SparkChatUpdateCallBack;
        pSparkChatClass->SubscribeMemberData(&pSparkChatInstance->UpdateClient);
        pSparkChatClass->SubscribeDestructor(&pSparkChatInstance->DestroyClient);
    }
}

void CARSFX_SparkChatter::SparkChatUpdateCallBack(Csis::Parameter *pParameters, void *pClientData) {
    SparkChatOutputInstance *pSparkChatInstance = static_cast<SparkChatOutputInstance *>(pClientData);
    Csis::CAR_SputOutputStruct *pSparkData = reinterpret_cast<Csis::CAR_SputOutputStruct *>(pParameters);

    pSparkChatInstance->m_pThis->ReceiveSparkChatterInputs(pSparkData);
}

void CARSFX_SparkChatter::SparkChatDestroyCallBack(Csis::Class *pSparkChatClass, void *pClientData) {
    SparkChatOutputInstance *pSparkChatInstance = static_cast<SparkChatOutputInstance *>(pClientData);

    static_cast<SndBase *>(pSparkChatInstance->m_pThis)->Destroy();
    pSparkChatClass->UnsubscribeMemberData(&pSparkChatInstance->UpdateClient);
    pSparkChatClass->UnsubscribeDestructor(&pSparkChatInstance->DestroyClient);
}

void CARSFX_SparkChatter::UpdateMixerOutputs() {
    if (BlipVol != 0) {
        SetDMIX_Input(2, 0x7FFF);
        SetDMIX_Input(0, BlipVol);
    } else {
        SetDMIX_Input(2, 0);
    }
}

void CARSFX_SparkChatter::ReceiveSparkChatterInputs(Csis::CAR_SputOutputStruct *pInputs) {
    BlipVol = pInputs->volume;
}

void CARSFX_SparkChatter::Destroy() {
    if (m_pSparkChatterControl) {
        delete m_pSparkChatterControl;
        m_pSparkChatterControl = nullptr;
    }
    m_pShiftingCTL = nullptr;
    m_pSparkChatterOutput = nullptr;
}

void CARSFX_SparkChatter::UpdateParams(float t) {
    EAXTunerCar *CurCar;

    SndBase::UpdateParams(t);
    if (IsEnabled() && m_pStateBase->m_eStateType == eMM_PLAYERCAR &&
        (CurCar = static_cast<EAXTunerCar *>(m_pEAXCar), CurCar->PlayBackFire) && m_pSparkChatterControl &&
        (CurCar->CurGear == Sound::FIRST_GEAR || CurCar->CurGear == Sound::NEUTRAL)) {
        if (m_pSparkChatterControl->GetForce_Trigger() == 1) {
            m_pSparkChatterControl->SetForce_Trigger(0);
            m_pSparkChatterControl->CommitMemberData();
        } else {
            CurCar->PlayBackFire = false;
            m_pSparkChatterControl->SetForce_Trigger(1);
            m_pSparkChatterControl->CommitMemberData();
        }
    }
}

void CARSFX_SparkChatter::ProcessUpdate() {
    if (m_pSparkChatterControl) {
        int TmpVol;

        TmpVol = GetDMixOutput(1, DMX_VOL) * m_pSweetnersData->Vol_Sputters() >> 15;
        m_pSparkChatterControl->SetCOMMON_PARAMETERS_AZIMUTH(GetDMixOutput(0, DMX_AZIM));
        m_pSparkChatterControl->SetCOMMON_PARAMETERS_PITCH_OFFSET(0);
        m_pSparkChatterControl->SetCOMMON_PARAMETERS_ROTATION(0);
        m_pSparkChatterControl->SetForce_Trigger(0);
        m_pSparkChatterControl->SetRPM(static_cast<int>(GetPhysRPM()));
        m_pSparkChatterControl->SetTORQUE(static_cast<int>(m_pEAXCar->GetPhysTRQ() * 10.24f));
        m_pSparkChatterControl->SetVOL(TmpVol);
        m_pSparkChatterControl->SetAccel_true(m_pEAXCar->IsAccelerating());
        m_pSparkChatterControl->SetShifting_true(m_pShiftingCTL->IsActive());
        m_pSparkChatterControl->CommitMemberData();
    }
}

void CARSFX_SparkChatter::SetupLoadData() {
    LoadAsset(m_pEAXCar->GetEngineAttributes().SweetBank(0), SNDPATH_ENGINE, EAXSND_DT_AEMS_ASYNCSPUMEM, eBANK_SLOT_NONE, true);
}
