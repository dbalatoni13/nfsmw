#include "./STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

bPList<SndBase::TypeInfo> CSTATEMGR_Base::m_SFXCTRLClassList;
bPList<SndBase::TypeInfo> CSTATEMGR_Base::m_SFXClassList;
bPList<CSTATE_Base::StateInfo> CSTATEMGR_Base::m_STATEClassList;

void *CSTATEMGR_Base::operator new(unsigned int size, const char *debug_name) {
    return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
}

CSTATEMGR_Base::CSTATEMGR_Base() {
    m_pHeadStateObj = nullptr;
    m_eStateType = eMM_MAIN;
    m_CurNumStates = 0;
    m_CurTime = 0.0f;
    bIsInitialized = false;
    m_DeltaTime = 0.0f;
}

CSTATEMGR_Base::~CSTATEMGR_Base() {}

void CSTATEMGR_Base::DisconnectMixMap() { // thanks chippy
    if (!m_pHeadStateObj) {
        return;
    }

    CSTATE_Base *obj = m_pHeadStateObj;
    obj->DisconnectMixMap();

    while (obj->m_pNextState) {
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

    while (obj->m_pNextState) {
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
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj) {
        if (!obj->IsDataLoaded())
            return false;
        obj = obj->m_pNextState;
    }
    return true;
}

SFX_Base *CSTATEMGR_Base::CreateSFX(int Instance, int SFXObjID) {
    SndBase::TypeInfo *FoundTypeInfo = nullptr;
    bPNode *CurNode = m_SFXClassList.GetHead();
    while (CurNode != m_SFXClassList.EndOfList()) {
        SndBase::TypeInfo *CurTypeInfo = reinterpret_cast<SndBase::TypeInfo *>(CurNode->GetObject());
        CurNode = CurNode->GetNext();
        if (((CurTypeInfo->ObjectID >> 4) & 0xFFF) == SFXObjID) {
            if ((((m_eStateType == eMM_AIRACECAR) || (m_eStateType == eMM_TRAFFIC) || (m_eStateType == eMM_TRUCK) ||
                  (m_eStateType == eMM_COPCAR)) &&
                 (static_cast<unsigned char>(CurTypeInfo->ObjectID >> 16) == eMM_PLAYERCAR)) ||
                (static_cast<unsigned char>(CurTypeInfo->ObjectID >> 16) == m_eStateType)) {
                if (!FoundTypeInfo) {
                    FoundTypeInfo = CurTypeInfo;
                }

                if (static_cast<unsigned char>(CurTypeInfo->ObjectID >> 16) == m_eStateType) {
                    if (!FoundTypeInfo) {
                        FoundTypeInfo = CurTypeInfo;
                    }

                    if (static_cast<unsigned char>(FoundTypeInfo->ObjectID >> 16) == m_eStateType) {
                        SndBase::TypeInfo *BaseClassInfo = CurTypeInfo->baseTypeInfo;
                        while (BaseClassInfo) {
                            if (BaseClassInfo == FoundTypeInfo) {
                                break;
                            }
                            BaseClassInfo = BaseClassInfo->baseTypeInfo;
                        }

                        if (BaseClassInfo != FoundTypeInfo) {
                            FoundTypeInfo = CurTypeInfo;
                        }
                    } else {
                        FoundTypeInfo = CurTypeInfo;
                    }
                }
            }
        }
    }

    if (!FoundTypeInfo) {
        return nullptr;
    }

    {
        SFX_Base *theObject = static_cast<SFX_Base *>(FoundTypeInfo->CreateObject((FoundTypeInfo->ObjectID >> 28) & 0xF));
        theObject->SetObjectID((m_eStateType << 16) | (Instance << 11) | (SFXObjID << 4));
        return theObject;
    }
}

SFXCTL *CSTATEMGR_Base::CreateSFXCTL(int Instance, int SFXCtrlID) {
    SndBase::TypeInfo *FoundTypeInfo = nullptr;
    bPNode *CurNode = m_SFXCTRLClassList.GetHead();
    while (CurNode != m_SFXCTRLClassList.EndOfList()) {
        SndBase::TypeInfo *CurTypeInfo = reinterpret_cast<SndBase::TypeInfo *>(CurNode->GetObject());
        CurNode = CurNode->GetNext();
        if (((CurTypeInfo->ObjectID >> 4) & 0xFFF) == SFXCtrlID) {
            if ((((m_eStateType == eMM_AIRACECAR) || (m_eStateType == eMM_TRAFFIC) || (m_eStateType == eMM_TRUCK) ||
                  (m_eStateType == eMM_COPCAR)) &&
                 (static_cast<unsigned char>(CurTypeInfo->ObjectID >> 16) == eMM_PLAYERCAR)) ||
                (static_cast<unsigned char>(CurTypeInfo->ObjectID >> 16) == m_eStateType)) {
                if (!FoundTypeInfo) {
                    FoundTypeInfo = CurTypeInfo;
                }

                if (static_cast<unsigned char>(CurTypeInfo->ObjectID >> 16) == m_eStateType) {
                    if (!FoundTypeInfo) {
                        FoundTypeInfo = CurTypeInfo;
                    }

                    if (static_cast<unsigned char>(FoundTypeInfo->ObjectID >> 16) == m_eStateType) {
                        SndBase::TypeInfo *BaseClassInfo = CurTypeInfo->baseTypeInfo;
                        while (BaseClassInfo) {
                            if (BaseClassInfo == FoundTypeInfo) {
                                break;
                            }
                            BaseClassInfo = BaseClassInfo->baseTypeInfo;
                        }

                        if (BaseClassInfo != FoundTypeInfo) {
                            FoundTypeInfo = CurTypeInfo;
                        }
                    } else {
                        FoundTypeInfo = CurTypeInfo;
                    }
                }
            }
        }
    }

    if (!FoundTypeInfo) {
        return nullptr;
    }

    {
        SFXCTL *theObject = static_cast<SFXCTL *>(FoundTypeInfo->CreateObject((FoundTypeInfo->ObjectID >> 28) & 0xF));
        theObject->SetObjectID((m_eStateType << 16) | (Instance << 11) | (SFXCtrlID << 4));
        return theObject;
    }
}

CSTATE_Base *CSTATEMGR_Base::CreateState(int StateInstType, int _SFXFlags) {
    CSTATE_Base::StateInfo *FoundStateInfo = nullptr;
    for (bPNode *CurNode = m_STATEClassList.GetHead(); CurNode != m_STATEClassList.EndOfList(); CurNode = CurNode->GetNext()) {
        CSTATE_Base::StateInfo *CurStateInfo = reinterpret_cast<CSTATE_Base::StateInfo *>(CurNode->GetObject());
        if (((CurStateInfo->StateID >> 16) & 0xFF) == m_eStateType) {
            if (!FoundStateInfo) {
                FoundStateInfo = CurStateInfo;
            }

            if ((CurStateInfo->StateID & 0xFFFF) == StateInstType) {
                if (static_cast<unsigned short>(FoundStateInfo->StateID) == StateInstType) {
                    CSTATE_Base::StateInfo *BaseStateInfo = CurStateInfo->baseStateInfo;
                    while (BaseStateInfo) {
                        if (BaseStateInfo == FoundStateInfo) {
                            break;
                        }
                        BaseStateInfo = BaseStateInfo->baseStateInfo;
                    }

                    if (BaseStateInfo != FoundStateInfo) {
                        FoundStateInfo = CurStateInfo;
                    }
                } else {
                    FoundStateInfo = CurStateInfo;
                }
            }
        }
    }

    if (!FoundStateInfo) {
        return nullptr;
    }

    CSTATE_Base *NewStateObj = FoundStateInfo->CreateState((FoundStateInfo->StateID >> 28) & 0xF);
    NewStateObj->m_pStateMgr = this;
    NewStateObj->m_InstNum = m_CurNumStates;
    NewStateObj->m_StateInstType = StateInstType;
    NewStateObj->m_eStateType = m_eStateType;
    if (!m_pHeadStateObj) {
        m_pHeadStateObj = NewStateObj;
    } else {
        CSTATE_Base *CurStateObj = m_pHeadStateObj;
        while (CurStateObj->m_pNextState) {
            CurStateObj = CurStateObj->m_pNextState;
        }

        NewStateObj->m_pPreviousState = CurStateObj;
        CurStateObj->m_pNextState = NewStateObj;
    }

    m_CurNumStates++;
    return NewStateObj;
}

void CSTATEMGR_Base::EnterWorld(eSndGameMode esgm) {
    bIsInitialized = true;
}

CSTATE_Base *CSTATEMGR_Base::GetFreeState(void *ObjectPtr) {
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj) {
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
        while (obj) {
            obj->UpdateParams(t);
            obj = obj->m_pNextState;
        }
    }
}

void CSTATEMGR_Base::ProcessUpdate() {
    if (bIsInitialized) {
        CSTATE_Base *obj = m_pHeadStateObj;
        while (obj) {
            obj->ProcessUpdate();
            obj = obj->m_pNextState;
        }
    }
}

CSTATE_Base *CSTATEMGR_Base::GetStateObj(int nInstance) {
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj) {
        if (obj->m_InstNum == nInstance)
            return obj;
        obj = obj->m_pNextState;
    }
    return nullptr;
}

CSTATE_Base *CSTATEMGR_Base::GetStateObj(void *testattachment) {
    CSTATE_Base *obj = m_pHeadStateObj;
    while (obj) {
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
    while (obj) {
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
    while (obj) {
        if (obj->bIsAttached)
            i++;
        obj = obj->m_pNextState;
    }
    return i;
}
