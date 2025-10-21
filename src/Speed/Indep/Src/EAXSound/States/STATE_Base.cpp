#include "./STATE_Base.hpp"

CSTATE_Base::StateInfo *CSTATE_Base::GetStateInfo(void) const {
    return &s_StateInfo;
}
char *CSTATE_Base::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *CSTATE_Base::CreateState(unsigned int allocator) {
    return new CSTATE_Base;
}

CSTATE_Base::CSTATE_Base() {
    m_pNextState = nullptr;
    m_pPreviousState = nullptr;
    m_pStateMgr = nullptr;
    m_InstNum = 0;
    m_eStateType = eMM_MAIN;
    m_pHeadSFXCTL = nullptr;
    m_pHeadSFXObj = nullptr;
    m_NumLoadedSFXObj = 0;
    m_NumLoadedSFXCTL = 0;
    m_pCar = nullptr;
    t_CurTime = 0;
    t_DeltaTime = 0;
}

void CSTATE_Base::Setup(int _m_SFXFlags) {
    m_SFXFlags = _m_SFXFlags;
    m_pAttachment = nullptr;
    bIsAttached = false;
    CreateSFXObjs();
    CreateSFXCtrls();
}

CSTATE_Base::~CSTATE_Base() {
    Destroy();
}

void CSTATE_Base::DisconnectMixMap() {}

void CSTATE_Base::SafeConnectOrphanObjects() {}

void CSTATE_Base::CreateSFXObjs() {
    for (int i = 0; i < 32; i++) {
        if ((m_SFXFlags >> i) & 1) {
            NewSFXObj(i);
        }
    }
}

void CSTATE_Base::CreateSFXCtrls() {
    m_NumLoadedSFXCTL = 0;
}

void CSTATE_Base::SortSFXCtl() {}

SFXCTL *CSTATE_Base::HasCtrlBeenAdded(int esfxctrl) {
    SndBase *obj = m_pHeadSFXCTL;
    while (obj != nullptr) {
    }
    return nullptr;
}

bool CSTATE_Base::IsDataLoaded(void) {}
