#include "./STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

bPList<SndBase::TypeInfo> CSTATEMGR_Base::m_SFXCTRLClassList;
bPList<SndBase::TypeInfo> CSTATEMGR_Base::m_SFXClassList;
bPList<CSTATE_Base::StateInfo> CSTATEMGR_Base::m_STATEClassList;

// UNSOLVED
CSTATEMGR_Base::CSTATEMGR_Base() {
    this->m_pHeadStateObj = nullptr;
    this->m_eStateType = eMM_MAIN;
    this->m_CurNumStates = 0;
    this->bIsInitialized = false;
    this->m_CurTime = 0.0f;
    this->m_DeltaTime = 0.0f;
}

CSTATEMGR_Base::~CSTATEMGR_Base() {}

// thanks chippy
void CSTATEMGR_Base::DisconnectMixMap() {
    if (this->m_pHeadStateObj != nullptr) {
        CSTATE_Base *CurStateObj = this->m_pHeadStateObj;
        CurStateObj->DisconnectMixMap();

        while (CurStateObj->m_pNextState != nullptr) {
            CurStateObj = CurStateObj->m_pNextState;
            CurStateObj->DisconnectMixMap();
        }
    }
}

// thanks chippy
void CSTATEMGR_Base::SafeConnectOrphanObjects() {
    if (this->m_pHeadStateObj != nullptr) {
        CSTATE_Base *CurStateObj = this->m_pHeadStateObj;
        CurStateObj->SafeConnectOrphanObjects();

        while (CurStateObj->m_pNextState != nullptr) {
            CurStateObj = CurStateObj->m_pNextState;
            CurStateObj->SafeConnectOrphanObjects();
        }
    }
}

void CSTATEMGR_Base::Initialize(eMAINMAPSTATES _m_eStateType) {
    this->m_eStateType = _m_eStateType;
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
    while (!m_SFXClassList.IsEmpty()) {
        m_SFXClassList.RemoveHead();
    }

    while (!m_SFXCTRLClassList.IsEmpty()) {
        m_SFXCTRLClassList.RemoveHead();
    }

    while (!m_STATEClassList.IsEmpty()) {
        m_STATEClassList.RemoveHead();
    }
}

bool CSTATEMGR_Base::IsDataLoaded() {
    CSTATE_Base *CurStateObj = this->m_pHeadStateObj;
    while (CurStateObj != nullptr) {
        if (!CurStateObj->IsDataLoaded())
            return false;
        CurStateObj = CurStateObj->m_pNextState;
    }
    return true;
}

SFX_Base *CSTATEMGR_Base::CreateSFX(int Instance, int SFXObjID) {}

SFXCTL *CSTATEMGR_Base::CreateSFXCTL(int Instance, int SFXCtrlID) {}

CSTATE_Base *CSTATEMGR_Base::CreateState(int StateInstType, int _SFXFlags) {}

void CSTATEMGR_Base::EnterWorld(eSndGameMode esgm) {
    this->bIsInitialized = true;
}

CSTATE_Base *CSTATEMGR_Base::GetFreeState(void *ObjectPtr) {
    CSTATE_Base *CurStateObj = this->m_pHeadStateObj;
    while (CurStateObj != nullptr) {
        if (!CurStateObj->IsAttached()) {
            return CurStateObj;
        }
        CurStateObj = CurStateObj->m_pNextState;
    }
    return nullptr;
}

void CSTATEMGR_Base::UpdateParams(float t) {
    ProfileNode profile_node("TODO", 0);
    CSTATE_Base *CurStateObj;

    if (bIsInitialized) {
        if (!g_EAXIsPaused()) {
            this->m_DeltaTime = t;
            this->m_CurTime += t;
        } else {
            this->m_DeltaTime = 0.0f;
        }

        CurStateObj = this->m_pHeadStateObj;
        while (CurStateObj != nullptr) {
            CurStateObj->UpdateParams(t);
            CurStateObj = CurStateObj->m_pNextState;
        }
    }
}

void CSTATEMGR_Base::ProcessUpdate() {
    CSTATE_Base *CurStateObj;

    if (bIsInitialized) {
        CurStateObj = this->m_pHeadStateObj;
        while (CurStateObj != nullptr) {
            CurStateObj->ProcessUpdate();
            CurStateObj = CurStateObj->m_pNextState;
        }
    }
}

CSTATE_Base *CSTATEMGR_Base::GetStateObj(int nInstance) {
    CSTATE_Base *CurStateObj = this->m_pHeadStateObj;
    while (CurStateObj != nullptr) {
        if (CurStateObj->m_InstNum == nInstance)
            return CurStateObj;
        CurStateObj = CurStateObj->m_pNextState;
    }
    return nullptr;
}

CSTATE_Base *CSTATEMGR_Base::GetStateObj(void *testattachment) {
    CSTATE_Base *CurStateObj = this->m_pHeadStateObj;
    while (CurStateObj != nullptr) {
        if (CurStateObj->IsAttachedToThis(testattachment))
            return CurStateObj;
        CurStateObj = CurStateObj->m_pNextState;
    }
    return nullptr;
}

void CSTATEMGR_Base::ExitWorld() {
    this->bIsInitialized = false;
    this->m_CurNumStates = 0;
    CSTATE_Base *CurStateObj = this->m_pHeadStateObj;
    while (CurStateObj != nullptr) {
        CSTATE_Base *DeletedStateObj = CurStateObj;
        CurStateObj = CurStateObj->m_pNextState;
        DeletedStateObj->Detach();
        delete DeletedStateObj;
    }
    this->m_pHeadStateObj = nullptr;
    this->m_CurNumStates = 0;
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
