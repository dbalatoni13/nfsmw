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
    this->m_eStateType = eMM_MAIN;
    this->m_CurNumStates = 0;
    this->m_CurTime = 0.0f;
    this->m_DeltaTime = 0.0f;
    this->bIsInitialized = false;
    this->m_pHeadStateObj = nullptr;
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

// STRIPPED
void CSTATEMGR_Base::Start() {}

// STRIPPED
void CSTATEMGR_Base::Stop() {}

bool CSTATEMGR_Base::IsDataLoaded() {
    CSTATE_Base *CurStateObj = this->m_pHeadStateObj;
    while (CurStateObj != nullptr) {
        if (!CurStateObj->IsDataLoaded())
            return false;
        CurStateObj = CurStateObj->m_pNextState;
    }
    return true;
}

SFX_Base *CSTATEMGR_Base::CreateSFX(int Instance, int SFXObjID) {
    SndBase::TypeInfo *FoundTypeInfo = nullptr;

    for (bPNode *CurNode = this->m_SFXClassList.GetHead(); CurNode != this->m_SFXClassList.EndOfList(); CurNode = CurNode->GetNext()) {
        SndBase::TypeInfo *CurTypeInfo = static_cast<SndBase::TypeInfo *>(CurNode->GetObject());

        if (MASK_OBJIDX(CurTypeInfo->ObjectID) == SFXObjID) {
            if ((((this->m_eStateType == eMM_AIRACECAR) || (this->m_eStateType == eMM_TRAFFIC) || (this->m_eStateType == eMM_TRUCK) ||
                  (this->m_eStateType == eMM_COPCAR)) &&
                 (MASK_GRPID(CurTypeInfo->ObjectID) == eMM_PLAYERCAR)) ||
                (MASK_GRPID(CurTypeInfo->ObjectID) == this->m_eStateType)) {
                if (FoundTypeInfo == nullptr) {
                    FoundTypeInfo = CurTypeInfo;
                }

                if (MASK_GRPID(CurTypeInfo->ObjectID) == this->m_eStateType) {
                    if (FoundTypeInfo == nullptr) {
                        FoundTypeInfo = CurTypeInfo;
                    }

                    if (MASK_GRPID(FoundTypeInfo->ObjectID) != this->m_eStateType) {
                        FoundTypeInfo = CurTypeInfo;
                    } else {
                        SndBase::TypeInfo *BaseClassInfo = CurTypeInfo->baseTypeInfo;

                        while (BaseClassInfo != nullptr) {
                            if (BaseClassInfo == FoundTypeInfo) {
                                FoundTypeInfo = CurTypeInfo;
                                break;
                            }

                            BaseClassInfo = BaseClassInfo->baseTypeInfo;
                        }
                    }
                }
            }
        }
    }

    if (FoundTypeInfo != nullptr) {
        SFX_Base *theObject = static_cast<SFX_Base *>(FoundTypeInfo->CreateObject(0));

        theObject->SetObjectID(SFT_GRPID(this->m_eStateType) | (Instance << 11) | SFT_OBJIDX(SFXObjID));

        return theObject;
    }

    return nullptr;
}

SFXCTL *CSTATEMGR_Base::CreateSFXCTL(int Instance, int SFXCtrlID) {
    SndBase::TypeInfo *FoundTypeInfo = nullptr;

    for (bPNode *CurNode = this->m_SFXCTRLClassList.GetHead(); CurNode != this->m_SFXCTRLClassList.EndOfList(); CurNode = CurNode->GetNext()) {
        SndBase::TypeInfo *CurTypeInfo = static_cast<SndBase::TypeInfo *>(CurNode->GetObject());

        if (MASK_OBJIDX(CurTypeInfo->ObjectID) == SFXCtrlID) {
            if ((((this->m_eStateType == eMM_AIRACECAR) || (this->m_eStateType == eMM_TRAFFIC) || (this->m_eStateType == eMM_TRUCK) ||
                  (this->m_eStateType == eMM_COPCAR)) &&
                 (MASK_GRPID(CurTypeInfo->ObjectID) == eMM_PLAYERCAR)) ||
                (MASK_GRPID(CurTypeInfo->ObjectID) == this->m_eStateType)) {
                if (FoundTypeInfo == nullptr) {
                    FoundTypeInfo = CurTypeInfo;
                }

                if (MASK_GRPID(CurTypeInfo->ObjectID) == this->m_eStateType) {
                    if (FoundTypeInfo == nullptr) {
                        FoundTypeInfo = CurTypeInfo;
                    }

                    if (MASK_GRPID(FoundTypeInfo->ObjectID) != this->m_eStateType) {
                        FoundTypeInfo = CurTypeInfo;
                    } else {
                        SndBase::TypeInfo *BaseClassInfo = CurTypeInfo->baseTypeInfo;

                        while (BaseClassInfo != nullptr) {
                            if (BaseClassInfo == FoundTypeInfo) {
                                FoundTypeInfo = CurTypeInfo;
                                break;
                            }

                            BaseClassInfo = BaseClassInfo->baseTypeInfo;
                        }
                    }
                }
            }
        }
    }

    if (FoundTypeInfo != nullptr) {
        SFXCTL *theObject = static_cast<SFXCTL *>(FoundTypeInfo->CreateObject(0));

        theObject->SetObjectID(SFT_GRPID(this->m_eStateType) | (Instance << 11) | SFT_OBJIDX(SFXCtrlID));

        return theObject;
    }

    return nullptr;
}

CSTATE_Base *CSTATEMGR_Base::CreateState(int StateInstType, int _SFXFlags) {
    CSTATE_Base::StateInfo *FoundStateInfo = nullptr;

    for (bPNode *CurNode = this->m_STATEClassList.GetHead(); CurNode != this->m_STATEClassList.EndOfList(); CurNode = CurNode->GetNext()) {
        CSTATE_Base::StateInfo *CurStateInfo = static_cast<CSTATE_Base::StateInfo *>(CurNode->GetObject());

        if (MASK_GRPID(CurStateInfo->StateID) == this->m_eStateType) {
            if (FoundStateInfo == nullptr) {
                FoundStateInfo = CurStateInfo;
            }

            if ((CurStateInfo->StateID & 0xFFFF) == StateInstType) {
                if (static_cast<unsigned short>(FoundStateInfo->StateID) != StateInstType) {
                    FoundStateInfo = CurStateInfo;
                } else {
                    CSTATE_Base::StateInfo *BaseStateInfo = CurStateInfo->baseStateInfo;

                    while (BaseStateInfo != nullptr) {
                        if (BaseStateInfo == FoundStateInfo) {
                            FoundStateInfo = CurStateInfo;
                            break;
                        }

                        BaseStateInfo = BaseStateInfo->baseStateInfo;
                    }
                }
            }
        }
    }

    if (FoundStateInfo == nullptr) {
        return nullptr;
    }

    CSTATE_Base *NewStateObj = FoundStateInfo->CreateState(0);

    NewStateObj->m_pStateMgr = this;
    NewStateObj->m_InstNum = this->m_CurNumStates;
    NewStateObj->m_eStateType = this->m_eStateType;
    NewStateObj->m_StateInstType = StateInstType;

    if (this->m_pHeadStateObj == nullptr) {
        this->m_pHeadStateObj = NewStateObj;
    } else {
        CSTATE_Base *CurStateObj = this->m_pHeadStateObj;

        while (CurStateObj->m_pNextState != nullptr) {
            CurStateObj = CurStateObj->m_pNextState;
        }

        NewStateObj->m_pPreviousState = CurStateObj;
        CurStateObj->m_pNextState = NewStateObj;
    }

    this->m_CurNumStates++;
    return NewStateObj;
}

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
