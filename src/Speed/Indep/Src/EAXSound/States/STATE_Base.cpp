#include "./STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int g_DMIX_DummyOutputBlock[];
extern int g_DMIX_DummyInputBlock[];

CSTATE_Base::StateInfo *CSTATE_Base::GetStateInfo(void) const {
    return &s_StateInfo;
}
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
                for (int n = 0; n < 16; n++) {
                    pIn[n] = 0;
                }
            }

            int *pout = CurSFXOBj->GetOutputBlockPtr();
            if (pout) {
                for (int n = 0; n < 16; n++) {
                    pout[n] = 0;
                }
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
                for (int n = 0; n < 16; n++) {
                    pIn[n] = 0;
                }
            }

            int *pout = CurSFXCtl->GetOutputBlockPtr();
            if (pout) {
                for (int n = 0; n < 16; n++) {
                    pout[n] = 0;
                }
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

void CSTATE_Base::NewSFXObj(int ecarsfx) {
    int groupID = m_InstNum;
    SndBase *NewlyCreatedSFXObj = m_pStateMgr->CreateSFX(groupID, ecarsfx);
    if (NewlyCreatedSFXObj) {
        NewlyCreatedSFXObj->SetupSFX(this);
        SndBase *LastSFXObj = m_pHeadSFXObj;
        m_NumLoadedSFXObj++;
        if (!LastSFXObj) {
            m_pHeadSFXObj = NewlyCreatedSFXObj;
        } else {
            while (LastSFXObj->m_pNextSFX) {
                LastSFXObj = LastSFXObj->m_pNextSFX;
            }

            LastSFXObj->m_pNextSFX = NewlyCreatedSFXObj;
        }
    }
}

SFXCTL *CSTATE_Base::NewSFXCtrl(int esfxctrl) {
    int groupID = m_InstNum;
    SndBase *NewSFXCTL = HasCtrlBeenAdded(esfxctrl);
    if (!NewSFXCTL) {
        if (esfxctrl == -1) {
            NewSFXCTL = nullptr;
        } else {
            NewSFXCTL = m_pStateMgr->CreateSFXCTL(groupID, esfxctrl);
            NewSFXCTL->SetupSFX(this);
            SndBase *LastSFX = m_pHeadSFXCTL;
            if (!LastSFX) {
                m_pHeadSFXCTL = NewSFXCTL;
            } else {
                while (LastSFX->m_pNextSFX) {
                    LastSFX = LastSFX->m_pNextSFX;
                }

                LastSFX->m_pNextSFX = NewSFXCTL;
            }

            m_NumLoadedSFXCTL++;
        }
    }
    return static_cast<SFXCTL *>(NewSFXCTL);
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
    m_NumLoadedSFXCTL = 0;
    if (CurSFXOBj) {
        int Index = 0;
        do {
            while (CurSFXOBj->GetController(Index) != -1) {
                SFXCTL *NewSFXCTL = NewSFXCtrl(CurSFXOBj->GetController(Index));
                CurSFXOBj->AttachController(NewSFXCTL);
                Index++;
            }

            CurSFXOBj = CurSFXOBj->m_pNextSFX;
            Index = 0;
        } while (CurSFXOBj);
    }

    SndBase *CurSFXCtl = m_pHeadSFXCTL;
    if (CurSFXCtl) {
        int Index = 0;
        do {
            while (CurSFXCtl->GetController(Index) != -1) {
                SFXCTL *NewSFXCTL = NewSFXCtrl(CurSFXCtl->GetController(Index));
                CurSFXCtl->AttachController(NewSFXCTL);
                Index++;
            }

            CurSFXCtl = CurSFXCtl->m_pNextSFX;
            Index = 0;
        } while (CurSFXCtl);
    }

    SortSFXCtl();
}

void CSTATE_Base::SortSFXCtl() {
    SndBase *TmpSFXCTLArray[64];
    int Cnt = 0;
    SndBase *CurSFXCtl = m_pHeadSFXCTL;
    SndBase *CurEndElement = nullptr;
    bool bFound;

    bMemSet(TmpSFXCTLArray, 0, 0x100);
    while (CurSFXCtl) {
        TmpSFXCTLArray[Cnt] = CurSFXCtl;
        Cnt++;
        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }

    m_pHeadSFXCTL = nullptr;
    while (true) {
        int UsedIndex = 0;
        int LastSmalledID = 0x40;

        bFound = false;
        for (int n = 0; n < m_NumLoadedSFXCTL; n++) {
            if (TmpSFXCTLArray[n]) {
                bFound = true;
                if (TmpSFXCTLArray[n]->GetSFX_ID() < LastSmalledID) {
                    UsedIndex = n;
                    LastSmalledID = TmpSFXCTLArray[n]->GetSFX_ID();
                }
            }
        }

        if (!bFound) {
            break;
        }

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
