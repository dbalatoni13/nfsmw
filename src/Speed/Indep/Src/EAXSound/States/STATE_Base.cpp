#include "./STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

ROOTSTATETYPE_IMPLEMENT(0, CSTATE_Base);

CSTATE_Base::CSTATE_Base() {
    this->m_pNextState = nullptr;
    this->m_pPreviousState = nullptr;
    this->m_pStateMgr = nullptr;
    this->m_InstNum = 0;
    this->m_eStateType = eMM_MAIN;
    this->m_pHeadSFXCTL = nullptr;
    this->m_pHeadSFXObj = nullptr;
    this->m_NumLoadedSFXObj = 0;
    this->m_NumLoadedSFXCTL = 0;
    this->m_pCar = nullptr;
    this->t_CurTime = 0;
    this->t_DeltaTime = 0;
}

void CSTATE_Base::Setup(int _m_SFXFlags) {
    this->m_SFXFlags = _m_SFXFlags;
    this->m_pAttachment = nullptr;
    this->bIsAttached = false;
    this->CreateSFXObjs();
    this->CreateSFXCtrls();
}

CSTATE_Base::~CSTATE_Base() {
    this->Destroy();
}

void CSTATE_Base::DisconnectMixMap() {}

void CSTATE_Base::SafeConnectOrphanObjects() {}

void CSTATE_Base::CreateSFXObjs() {
    for (int n = 0; n < 32; n++) {
        if ((this->m_SFXFlags >> n) & 1) {
            this->NewSFXObj(n);
        }
    }
}

void CSTATE_Base::CreateSFXCtrls() {
    this->m_NumLoadedSFXCTL = 0;
}

void CSTATE_Base::SortSFXCtl() {}

SFXCTL *CSTATE_Base::HasCtrlBeenAdded(int esfxctrl) {
    SndBase *CurSFXCtl = m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        if (CurSFXCtl->GetObjectIndex() == esfxctrl) {
            return static_cast<SFXCTL *>(CurSFXCtl);
        }

        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }
    return nullptr;
}

bool CSTATE_Base::IsDataLoaded(void) {}
