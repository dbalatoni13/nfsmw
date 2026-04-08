#include "./STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int g_DMIX_DummyOutputBlock[];
extern int g_DMIX_DummyInputBlock[];
const char *CSTATE_Base::GetStateName(void) const {
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
    t_CurTime = 0.0f;
    t_DeltaTime = 0.0f;
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

void CSTATE_Base::DisconnectMixMap() {
    SndBase *CurSFXOBj = m_pHeadSFXObj;
    while (CurSFXOBj) {
        {
            int *pIn = CurSFXOBj->GetInputBlockPtr();
            if (pIn) {
                int n = 0;
                do {
                    *pIn = 0;
                    n++;
                    pIn++;
                } while (n <= 15);
            }

            int *pout = CurSFXOBj->GetOutputBlockPtr();
            if (pout) {
                int n = 0;
                do {
                    *pout = 0;
                    n++;
                    pout++;
                } while (n <= 15);
            }

            CurSFXOBj->SetOutputsPtr(nullptr);
            CurSFXOBj->SetInputsPtr(nullptr);
        }

        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }

    SndBase *CurSFXCtl = m_pHeadSFXCTL;
    while (CurSFXCtl) {
        {
            int *pIn = CurSFXCtl->GetInputBlockPtr();
            if (pIn) {
                int n = 0;
                do {
                    *pIn = 0;
                    n++;
                    pIn++;
                } while (n <= 15);
            }

            int *pout = CurSFXCtl->GetOutputBlockPtr();
            if (pout) {
                int n = 0;
                do {
                    *pout = 0;
                    n++;
                    pout++;
                } while (n <= 15);
            }

            CurSFXCtl->SetInputsPtr(nullptr);
            CurSFXCtl->SetOutputsPtr(nullptr);
        }

        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }
}

void CSTATE_Base::SafeConnectOrphanObjects() {
    SndBase *CurSFXOBj = m_pHeadSFXObj;
    while (CurSFXOBj) {
        if (!CurSFXOBj->GetOutputBlockPtr()) {
            CurSFXOBj->SetOutputsPtr(g_DMIX_DummyOutputBlock);
        }

        if (!CurSFXOBj->GetInputBlockPtr()) {
            CurSFXOBj->SetInputsPtr(g_DMIX_DummyInputBlock);
        }

        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }

    SndBase *CurSFXCtl = m_pHeadSFXCTL;
    while (CurSFXCtl) {
        if (!CurSFXCtl->GetOutputBlockPtr()) {
            CurSFXCtl->SetOutputsPtr(g_DMIX_DummyOutputBlock);
        }

        if (!CurSFXCtl->GetInputBlockPtr()) {
            CurSFXCtl->SetInputsPtr(g_DMIX_DummyInputBlock);
        }

        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }
}

void CSTATE_Base::CreateSFXObjs() {
    for (int i = 0; i < 32; i++) {
        if ((m_SFXFlags >> i) & 1) {
            NewSFXObj(i);
        }
    }
}

void CSTATE_Base::ForceCreateSFXCtrls(int iSFXCtrls) {
    for (int n = 0; n < 32; n++) {
        if ((iSFXCtrls >> n) & 1) {
            NewSFXCtrl(n);
        }
    }
}

void CSTATE_Base::CreateSFXCtrls() {
    SndBase *CurSFXOBj = m_pHeadSFXObj;
    SndBase *CurSFXCtl;
    m_NumLoadedSFXCTL = 0;
    if (CurSFXOBj) {
        do {
            int Index = 0;
            while (CurSFXOBj->GetController(Index) != -1) {
                CurSFXOBj->AttachController(NewSFXCtrl(CurSFXOBj->GetController(Index)));
                Index++;
            }
            CurSFXOBj = CurSFXOBj->m_pNextSFX;
        } while (CurSFXOBj);
    }

    CurSFXCtl = m_pHeadSFXCTL;
    if (CurSFXCtl) {
        do {
            int Index = 0;
            while (CurSFXCtl->GetController(Index) != -1) {
                CurSFXCtl->AttachController(NewSFXCtrl(CurSFXCtl->GetController(Index)));
                Index++;
            }
            CurSFXCtl = CurSFXCtl->m_pNextSFX;
        } while (CurSFXCtl);
    }

    SortSFXCtl();
}

void CSTATE_Base::SortSFXCtl() {
    SndBase *TmpSFXCTLArray[64];
    int Cnt;
    SndBase *CurSFXCtl;
    SndBase *CurEndElement;
    bool bFound;

    bMemSet(TmpSFXCTLArray, 0, 0x100);
    Cnt = 0;
    CurSFXCtl = m_pHeadSFXCTL;
    if (CurSFXCtl) {
        do {
            TmpSFXCTLArray[Cnt] = CurSFXCtl;
            Cnt++;
            CurSFXCtl = CurSFXCtl->m_pNextSFX;
        } while (CurSFXCtl);
    }

    m_pHeadSFXCTL = nullptr;
    CurEndElement = nullptr;
    do {
        int UsedIndex = 0;
        bFound = false;
        int LastSmalledID = 0x40;
        for (int n = 0; n < m_NumLoadedSFXCTL; n++) {
            if (TmpSFXCTLArray[n]) {
                bFound = true;
                if (TmpSFXCTLArray[n]->GetSFX_ID() < LastSmalledID) {
                    UsedIndex = n;
                    LastSmalledID = TmpSFXCTLArray[n]->GetSFX_ID();
                }
            }
        }

        if (bFound) {
            if (!m_pHeadSFXCTL) {
                CurEndElement = TmpSFXCTLArray[UsedIndex];
                m_pHeadSFXCTL = CurEndElement;
            } else {
                CurEndElement->m_pNextSFX = TmpSFXCTLArray[UsedIndex];
                CurEndElement = TmpSFXCTLArray[UsedIndex];
            }

            CurEndElement->m_pNextSFX = nullptr;
            TmpSFXCTLArray[UsedIndex] = nullptr;
        }
    } while (bFound);
}

void CSTATE_Base::NewSFXObj(int ecarsfx) {
    int groupID = m_InstNum;
    SndBase *NewlyCreatedSFXObj = m_pStateMgr->CreateSFX(groupID, ecarsfx);
    if (NewlyCreatedSFXObj) {
        NewlyCreatedSFXObj->SetupSFX(this);
        m_NumLoadedSFXObj++;
        if (!m_pHeadSFXObj) {
            m_pHeadSFXObj = NewlyCreatedSFXObj;
        } else {
            SndBase *LastSFXObj = m_pHeadSFXObj;
            while (LastSFXObj->m_pNextSFX) {
                LastSFXObj = LastSFXObj->m_pNextSFX;
            }

            LastSFXObj->m_pNextSFX = NewlyCreatedSFXObj;
        }
    }
}

SFXCTL *CSTATE_Base::NewSFXCtrl(int esfxctl) {
    int groupID;
    SndBase *NewSFXCTL = HasCtrlBeenAdded(esfxctl);
    if (!NewSFXCTL) {
        if (esfxctl == -1) {
            return nullptr;
        }

        groupID = m_InstNum;
        NewSFXCTL = m_pStateMgr->CreateSFXCTL(groupID, esfxctl);
        NewSFXCTL->SetupSFX(this);
        if (!m_pHeadSFXCTL) {
            m_pHeadSFXCTL = NewSFXCTL;
        } else {
            SndBase *LastSFX = m_pHeadSFXCTL;
            while (LastSFX->m_pNextSFX) {
                LastSFX = LastSFX->m_pNextSFX;
            }

            LastSFX->m_pNextSFX = NewSFXCTL;
        }

        m_NumLoadedSFXCTL++;
    }
    return static_cast<SFXCTL *>(NewSFXCTL);
}

SFXCTL *CSTATE_Base::HasCtrlBeenAdded(int esfxctrl) {
    SndBase *CurSFXCtl = m_pHeadSFXCTL;
    while (CurSFXCtl) {
        if (CurSFXCtl->GetObjectIndex() == esfxctrl) {
            return static_cast<SFXCTL *>(CurSFXCtl);
        }

        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }
    return nullptr;
}

void CSTATE_Base::LoadData() {
    SndBase *CurSFXOBj = m_pHeadSFXObj;

    if (CurSFXOBj) {
        do {
            CurSFXOBj->SetupLoadData();
            CurSFXOBj = CurSFXOBj->m_pNextSFX;
        } while (CurSFXOBj);
    }
}

void CSTATE_Base::ProcessUpdate() {
    SndBase *CurSFXOBj = m_pHeadSFXObj;

    if (CurSFXOBj) {
        do {
            CurSFXOBj->ProcessUpdate();
            CurSFXOBj = CurSFXOBj->m_pNextSFX;
        } while (CurSFXOBj);
    }
}

void CSTATE_Base::UpdateParams(float t) {
    SndBase *CurSFXCtl;
    SndBase *CurSFXOBj;

    if (!g_EAXIsPaused()) {
        t_DeltaTime = t;
        t_CurTime = t_CurTime + t;
    } else {
        t_DeltaTime = 0.0f;
    }

    if (bIsAttached) {
        CurSFXCtl = m_pHeadSFXCTL;
        if (CurSFXCtl) {
            do {
                CurSFXCtl->UpdateParams(t);
                CurSFXCtl->UpdateMixerOutputs();
                CurSFXCtl = CurSFXCtl->m_pNextSFX;
            } while (CurSFXCtl);
        }

        CurSFXOBj = m_pHeadSFXObj;
        if (CurSFXOBj) {
            do {
                CurSFXOBj->UpdateParams(t);
                CurSFXOBj->UpdateMixerOutputs();
                CurSFXOBj = CurSFXOBj->m_pNextSFX;
            } while (CurSFXOBj);
        }
    }
}

void CSTATE_Base::Destroy() {
    SndBase *CurSFXCtl = m_pHeadSFXCTL;
    while (CurSFXCtl) {
        SndBase *TmpSFXCtl = CurSFXCtl->m_pNextSFX;
        delete CurSFXCtl;
        CurSFXCtl = TmpSFXCtl;
    }

    SndBase *CurSFXOBj = m_pHeadSFXObj;
    while (CurSFXOBj) {
        SndBase *TmpSFXOBj = CurSFXOBj->m_pNextSFX;
        delete CurSFXOBj;
        CurSFXOBj = TmpSFXOBj;
    }

    m_pHeadSFXCTL = nullptr;
    m_pHeadSFXObj = nullptr;
    m_NumLoadedSFXObj = 0;
    m_NumLoadedSFXCTL = 0;
}

SndBase *CSTATE_Base::GetSFXObject(int SFXId) {
    SndBase *CurSFXOBj = m_pHeadSFXObj;

    if (CurSFXOBj) {
        do {
            if (CurSFXOBj->GetSFX_ID() == SFXId) {
                return CurSFXOBj;
            }
            CurSFXOBj = CurSFXOBj->m_pNextSFX;
        } while (CurSFXOBj);
    }

    return nullptr;
}

SndBase *CSTATE_Base::GetSFXCTLObject(int SFXId) {
    SndBase *CurSFXCtl = m_pHeadSFXCTL;

    if (CurSFXCtl) {
        do {
            if (CurSFXCtl->GetSFX_ID() == SFXId) {
                return CurSFXCtl;
            }
            CurSFXCtl = CurSFXCtl->m_pNextSFX;
        } while (CurSFXCtl);
    }

    return CurSFXCtl;
}

bool CSTATE_Base::IsDataLoaded(void) {
    SndAssetQueue &cbs = gAEMSMgr.mWaitForResolve;
    for (SndAssetQueue::iterator i = cbs.begin(); i != cbs.end(); i++) {
        stSndAssetQueue &data = *i;
        if (data.pThis && data.pThis->GetStateBase() == this) {
            return false;
        }
    }
    return true;
}

void CSTATE_Base::Attach(void *pAttachment) {
    SndBase *CurSFXCtl = m_pHeadSFXCTL;

    m_pAttachment = pAttachment;
    bIsAttached = true;

    if (CurSFXCtl) {
        do {
            CurSFXCtl->InitSFX();
            CurSFXCtl = CurSFXCtl->m_pNextSFX;
        } while (CurSFXCtl);
    }

    LoadData();
}

bool CSTATE_Base::Detach() {
    SndBase *CurSFXObj;
    SndBase *CurSFXCtl;

    bIsAttached = false;
    m_pAttachment = nullptr;

    CurSFXObj = m_pHeadSFXObj;
    while (CurSFXObj) {
        int iVar7 = 0;
        int iVar9 = 0;
        SndAssetQueue::iterator ptr = gAEMSMgr.mWaitForResolve.begin();

        CurSFXObj->Detach();
        CurSFXObj->GetInputBlockPtr()[15] = 0;

        while (ptr != gAEMSMgr.mWaitForResolve.end()) {
            if ((*ptr).pThis == CurSFXObj) {
                iVar7 = (*ptr).Asset.FileName.GetHash32();
                iVar9 = reinterpret_cast<int>((*ptr).Asset.FileName.GetString());
                break;
            }
            ++ptr;
        }

        if (iVar7 != 0 || iVar9 != 0) {
ReprocessQueue:
            ptr = gAEMSMgr.mWaitForResolve.begin();
            while (ptr != gAEMSMgr.mWaitForResolve.end()) {
                if ((*ptr).Asset.FileName.GetHash32() == iVar7 &&
                    reinterpret_cast<int>((*ptr).Asset.FileName.GetString()) == iVar9) {
                    gAEMSMgr.mWaitForResolve.remove(*ptr);
                    goto ReprocessQueue;
                }
                ++ptr;
            }
        }

        bMemSet(CurSFXObj->GetOutputBlockPtr(), 0, 0x40);
        CurSFXObj = CurSFXObj->m_pNextSFX;
    }

    CurSFXCtl = m_pHeadSFXCTL;
    while (CurSFXCtl) {
        CurSFXCtl->Detach();
        bMemSet(CurSFXCtl->GetOutputBlockPtr(), 0, 0x40);
        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }

    return true;
}

void CSTATE_Base::PreLoadAssets() {}
extern bool g_EAXIsPaused(void);

CSTATE_Base::StateInfo *CSTATE_Base::GetStateInfo(void) const {
    return &s_StateInfo;
}
