#include "./STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

bPList<SndBase::TypeInfo> CSTATEMGR_Base::m_SFXCTRLClassList;
bPList<SndBase::TypeInfo> CSTATEMGR_Base::m_SFXClassList;
bPList<CSTATE_Base::StateInfo> CSTATEMGR_Base::m_STATEClassList;

CSTATEMGR_Base::CSTATEMGR_Base() {
    m_pHeadStateObj = nullptr;
    m_eStateType = eMM_MAIN;
    m_CurNumStates = 0;
    m_CurTime = 0.0f;
    bIsInitialized = 0;
    m_DeltaTime = 0.0f;
}

CSTATEMGR_Base::~CSTATEMGR_Base() {}

void CSTATEMGR_Base::DisconnectMixMap() { // thanks chippy
    if (!m_pHeadStateObj) {
        return;
    }

    CSTATE_Base *obj = m_pHeadStateObj;
    obj->DisconnectMixMap();

    while (obj->m_pNextState != nullptr) {
        obj = obj->m_pNextState;
        obj->DisconnectMixMap();
    }
}

void CSTATEMGR_Base::SafeConnectOrphanObjects() { // thanks chippy
    if (!m_pHeadStateObj) {
        return;
    }

    CSTATE_Base *obj = m_pHeadStateObj;
    obj->SafeConnectOrphanObjects();

    while (obj->m_pNextState != nullptr) {
        obj = obj->m_pNextState;
        obj->SafeConnectOrphanObjects();
    }
}

void CSTATEMGR_Base::Initialize(eMAINMAPSTATES _m_eStateType) {
    m_eStateType = _m_eStateType;
}

void CSTATEMGR_Base::RegisterSFXCTL(SndBase::TypeInfo *typeinfo) {
    m_SFXCTRLClassList.AddTail(typeinfo);
}

void CSTATEMGR_Base::RegisterSTATE(CSTATE_Base::StateInfo *stateinfo) {
    m_STATEClassList.AddTail(stateinfo);
}

void CSTATEMGR_Base::RegisterSFX(SndBase::TypeInfo *typeinfo) {
    m_SFXClassList.AddTail(typeinfo);
}

void CSTATEMGR_Base::ClearClassLists() {
    m_SFXClassList.DeleteAllElements();
    m_SFXCTRLClassList.DeleteAllElements();
    m_STATEClassList.DeleteAllElements();
}

bool CSTATEMGR_Base::IsDataLoaded() {
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj != nullptr) {
        if (!obj->IsDataLoaded())
            return false;
        obj = obj->m_pNextState;
    }
    return true;
}

SFX_Base *CSTATEMGR_Base::CreateSFX(int Instance, int SFXObjID) {}

SFXCTL *CSTATEMGR_Base::CreateSFXCTL(int Instance, int SFXCtrlID) {}

CSTATE_Base *CSTATEMGR_Base::CreateState(int StateInstType, int _SFXFlags) {}

void CSTATEMGR_Base::EnterWorld(eSndGameMode esgm) {
    bIsInitialized = true;
}

CSTATE_Base *CSTATEMGR_Base::GetFreeState(void *ObjectPtr) {
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj != nullptr) {
        if (!obj->bIsAttached)
            return obj;
        obj = obj->m_pNextState;
    }
    return nullptr;
}

void CSTATEMGR_Base::UpdateParams(float t) {
    if (bIsInitialized) {
        if (!g_EAXIsPaused()) {
            m_DeltaTime = t;
            m_CurTime += t;
        } else {
            m_DeltaTime = 0.0f;
        }
        CSTATE_Base *obj = m_pHeadStateObj;
        while (obj != nullptr) {
            obj->UpdateParams(t);
            obj = obj->m_pNextState;
        }
    }
}

void CSTATEMGR_Base::ProcessUpdate() {
    if (bIsInitialized) {
        CSTATE_Base *obj = m_pHeadStateObj;
        while (obj != nullptr) {
            obj->ProcessUpdate();
            obj = obj->m_pNextState;
        }
    }
}

CSTATE_Base *CSTATEMGR_Base::GetStateObj(int nInstance) {
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj != nullptr) {
        if (obj->m_InstNum == nInstance)
            return obj;
        obj = obj->m_pNextState;
    }
    return nullptr;
}

CSTATE_Base *CSTATEMGR_Base::GetStateObj(void *testattachment) {
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj != nullptr) {
        if (testattachment == obj->m_pAttachment)
            return obj;
        obj = obj->m_pNextState;
    }
    return nullptr;
}

void CSTATEMGR_Base::ExitWorld() {
    bIsInitialized = false;
    m_CurNumStates = 0;
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj != nullptr) {
        CSTATE_Base *del = obj;
        obj = obj->m_pNextState;
        del->Detach();
        delete del;
    }
    m_pHeadStateObj = nullptr;
    m_CurNumStates = 0;
}

int CSTATEMGR_Base::GetAttachedStateCount() {
    int i = 0;
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj != nullptr) {
        if (obj->bIsAttached)
            i++;
        obj = obj->m_pNextState;
    }
    return i;
}
