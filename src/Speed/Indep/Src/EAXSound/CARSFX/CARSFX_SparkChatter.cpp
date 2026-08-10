#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_SparkChatter.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"

DEFINE_CREATABLE(0x20060, CARSFX_SparkChatter, SndBase);

int SparkChatVol = 32000; // size: 0x4, address: 0xFFFFFFFF, Decl: 27

// UNSOLVED
CARSFX_SparkChatter::CARSFX_SparkChatter() : CARSFX() {
    this->m_pSparkChatterControl = nullptr;
    this->m_pSparkChatterOutput = nullptr;
    this->m_pShiftingCTL = nullptr;

    this->SparkChatOutputClients.m_pThis = this;
    this->SparkChatOutputClients.CreateClient.pClientData = &this->SparkChatOutputClients;
    this->SparkChatOutputClients.DestroyClient.pClientData = &this->SparkChatOutputClients;
    this->SparkChatOutputClients.UpdateClient.pClientData = &this->SparkChatOutputClients;
    this->SparkChatOutputClients.CreateClient.pClientFunc = this->SparkChatCreateCallBack;
    this->SparkChatOutputClients.DestroyClient.pClientFunc = this->SparkChatDestroyCallBack;
    this->m_pSweetnersData = nullptr;
    this->BlipVol = 0;
}

CARSFX_SparkChatter::~CARSFX_SparkChatter() {
    this->Destroy();
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
            this->m_pShiftingCTL = static_cast<SFXCTL_Shifting *>(psfxctl);
            break;
        case 7:
            this->m_p3DCarPosCtl = static_cast<SFXCTL_3DCarPos *>(psfxctl);
            break;
    }
}

void CARSFX_SparkChatter::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_UGL = this->m_pEAXCar->GetEngineUpgradeLevel();
    this->m_pSweetnersData = &this->m_pEAXCar->GetAttributes();
}

void CARSFX_SparkChatter::InitSFX() {
    CARSFX_SparkChatter *pThisPtr = this;
    int iThisPtr = reinterpret_cast<int>(pThisPtr);

    SndBase::InitSFX();
    g_pEAXSound->SetCsisName(this);
    m_pSparkChatterControl = new Csis::CAR_Sputter(static_cast<int>(m_pEAXCar->GetAttributes().CarID()), iThisPtr, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    static int tmp_refCnt = m_pSparkChatterControl->GetRefCount();
}

// TODO wtf?
void CARSFX_SparkChatter::SparkChatCreateCallBack(Csis::Class *pSparkChatClass, Csis::Parameter *pParameters, void *pClientData) {
    SparkChatOutputInstance *pSparkChatInstance = static_cast<SparkChatOutputInstance *>(pClientData);
    Csis::CAR_SputOutputStruct *pSparkData = reinterpret_cast<Csis::CAR_SputOutputStruct *>(pParameters);

    if (reinterpret_cast<Csis::Parameter *>(pSparkData)[2].iVal == reinterpret_cast<intptr_t>(pSparkChatInstance->m_pThis)) {
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

    pSparkChatInstance->m_pThis->UpdateMixerOutputs();
    pSparkChatClass->UnsubscribeMemberData(&pSparkChatInstance->UpdateClient);
    pSparkChatClass->UnsubscribeDestructor(&pSparkChatInstance->DestroyClient);
}

void CARSFX_SparkChatter::UpdateMixerOutputs() {
    if (this->BlipVol != 0) {
        this->SetDMIX_Input(2, 0x7FFF);
        this->SetDMIX_Input(0, this->BlipVol);
    } else {
        this->SetDMIX_Input(2, 0);
    }
}

void CARSFX_SparkChatter::ReceiveSparkChatterInputs(Csis::CAR_SputOutputStruct *pInputs) {
    this->BlipVol = pInputs->volume;
}

void CARSFX_SparkChatter::Destroy() {
    if (this->m_pSparkChatterControl != nullptr) {
        delete this->m_pSparkChatterControl;
        this->m_pSparkChatterControl = nullptr;
    }
    this->m_pShiftingCTL = nullptr;
    this->m_pSparkChatterOutput = nullptr;
}

void CARSFX_SparkChatter::UpdateParams(float t) {
    if (!this->IsEnabled()) {
        return;
    }
    this->SndBase::UpdateParams(t);

    if (this->m_pStateBase->m_eStateType == eMM_PLAYERCAR) {
        EAXTunerCar *CurCar = static_cast<EAXTunerCar *>(this->m_pEAXCar);

        if (CurCar->PlayBackFire && this->m_pSparkChatterControl != nullptr &&
            (CurCar->GetCurGear() == FIRST_GEAR || CurCar->GetCurGear() == NEUTRAL)) {
            if (this->m_pSparkChatterControl->GetForce_Trigger() == 1) {
                this->m_pSparkChatterControl->SetForce_Trigger(0);
                this->m_pSparkChatterControl->CommitMemberData();
            } else {
                CurCar->PlayBackFire = false;
                this->m_pSparkChatterControl->SetForce_Trigger(1);
                this->m_pSparkChatterControl->CommitMemberData();
            }
        }
    }
}

void CARSFX_SparkChatter::ProcessUpdate() {
    if (this->m_pSparkChatterControl != nullptr) {
        int TmpVol;

        TmpVol = this->GetDMixOutput(1, DMX_VOL) * this->m_pSweetnersData->Vol_Sputters() >> 15;
        this->m_pSparkChatterControl->SetCOMMON_PARAMETERS_AZIMUTH(this->GetDMixOutput(0, DMX_AZIM));
        this->m_pSparkChatterControl->SetCOMMON_PARAMETERS_PITCH_OFFSET(0);
        this->m_pSparkChatterControl->SetCOMMON_PARAMETERS_ROTATION(0);
        this->m_pSparkChatterControl->SetForce_Trigger(0);
        this->m_pSparkChatterControl->SetRPM(static_cast<int>(this->GetPhysRPM()));
        this->m_pSparkChatterControl->SetTORQUE(static_cast<int>(this->m_pEAXCar->GetPhysTRQ() * 10.24f));
        this->m_pSparkChatterControl->SetVOL(TmpVol);
        this->m_pSparkChatterControl->SetAccel_true(static_cast<int>(this->m_pEAXCar->IsAccelerating()));
        this->m_pSparkChatterControl->SetShifting_true(static_cast<int>(this->m_pShiftingCTL->IsActive()));
        this->m_pSparkChatterControl->CommitMemberData();
    }
}
