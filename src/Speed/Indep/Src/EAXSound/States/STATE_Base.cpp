#include "./STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMap.hpp"

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

int DEBUGPRINT_MIXERCONNECT; // Decl: 46

void CSTATE_Base::DisconnectMixMap() {
#ifdef EA_BUILD_A124
    SndBase *CurSFXOBj = this->m_pHeadSFXObj;
    while (CurSFXOBj != nullptr) {
        CurSFXOBj->SetOutputsPtr(nullptr);
        CurSFXOBj->SetInputsPtr(nullptr);
        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }

    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        CurSFXCtl->SetInputsPtr(nullptr);
        CurSFXCtl->SetOutputsPtr(nullptr);
        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }
#else
    SndBase *CurSFXOBj = this->m_pHeadSFXObj;
    while (CurSFXOBj != nullptr) {
        int *pIn = CurSFXOBj->GetInputBlockPtr();
        if (pIn != nullptr) {
            for (int n = 0; n < 16; n++) {
                *pIn++ = 0;
            }
        }

        int *pout = CurSFXOBj->GetOutputBlockPtr();
        if (pout != nullptr) {
            for (int n = 0; n < 16; n++) {
                *pout++ = 0;
            }
        }

        CurSFXOBj->SetOutputsPtr(nullptr);
        CurSFXOBj->SetInputsPtr(nullptr);

        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }

    SndBase *CurSFXCtl = m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        int *pIn = CurSFXCtl->GetInputBlockPtr();
        if (pIn != nullptr) {
            for (int n = 0; n < 16; n++) {
                *pIn++ = 0;
            }
        }

        int *pout = CurSFXCtl->GetOutputBlockPtr();
        if (pout != nullptr) {
            for (int n = 0; n < 16; n++) {
                *pout++ = 0;
            }
        }

        CurSFXCtl->SetInputsPtr(nullptr);
        CurSFXCtl->SetOutputsPtr(nullptr);

        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }
#endif
}

void CSTATE_Base::SafeConnectOrphanObjects() {
    SndBase *CurSFXOBj = this->m_pHeadSFXObj;
    while (CurSFXOBj != nullptr) {
        if (CurSFXOBj->GetOutputBlockPtr() == nullptr) {
            CurSFXOBj->SetOutputsPtr(g_DMIX_DummyOutputBlock);
        }

        if (CurSFXOBj->GetInputBlockPtr() == nullptr) {
            CurSFXOBj->SetInputsPtr(g_DMIX_DummyInputBlock);
        }

        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }

    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        if (CurSFXCtl->GetOutputBlockPtr() == nullptr) {
            CurSFXCtl->SetOutputsPtr(g_DMIX_DummyOutputBlock);
        }

        if (CurSFXCtl->GetInputBlockPtr() == nullptr) {
            CurSFXCtl->SetInputsPtr(g_DMIX_DummyInputBlock);
        }

        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }
}

void CSTATE_Base::CreateSFXObjs() {
    for (int n = 0; n < 32; n++) {
        if ((this->m_SFXFlags >> n) & 1) {
            this->NewSFXObj(n);
        }
    }
}

void CSTATE_Base::ForceCreateSFXCtrls(int iSFXCtrls) {
    for (int n = 0; n < 32; n++) {
        if ((iSFXCtrls >> n) & 1) {
            this->NewSFXCtrl(n);
        }
    }
}

void CSTATE_Base::CreateSFXCtrls() {
    SndBase *CurSFXOBj = this->m_pHeadSFXObj;
    this->m_NumLoadedSFXCTL = 0;

    while (CurSFXOBj != nullptr) {
        int Index = 0;
        while (CurSFXOBj->GetController(Index) != -1) {
            CurSFXOBj->AttachController(this->NewSFXCtrl(CurSFXOBj->GetController(Index)));
            Index++;
        }
        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }

    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        int Index = 0;
        while (CurSFXCtl->GetController(Index) != -1) {
            CurSFXCtl->AttachController(this->NewSFXCtrl(CurSFXCtl->GetController(Index)));
            Index++;
        }
        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }

    this->SortSFXCtl();
}

void CSTATE_Base::SortSFXCtl() {
    SndBase *TmpSFXCTLArray[64];

    bMemSet(TmpSFXCTLArray, 0, sizeof(TmpSFXCTLArray));

    int Cnt = 0;
    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    if (CurSFXCtl != nullptr) {
        do {
            TmpSFXCTLArray[Cnt] = CurSFXCtl;
            Cnt++;
            CurSFXCtl = CurSFXCtl->m_pNextSFX;
        } while (CurSFXCtl != nullptr);
    }

    this->m_pHeadSFXCTL = nullptr;
    SndBase *CurEndElement = nullptr;
    bool bFound;

    do {
        bFound = false;
        int UsedIndex = 0;
        int LastSmalledID = 64;

        for (int n = 0; n < this->m_NumLoadedSFXCTL; n++) {
            if (TmpSFXCTLArray[n] != nullptr) {
                bFound = true;

                if (TmpSFXCTLArray[n]->GetSFX_ID() < LastSmalledID) {
                    UsedIndex = n;
                    LastSmalledID = TmpSFXCTLArray[n]->GetSFX_ID();
                }
            }
        }

        if (bFound) {
            if (this->m_pHeadSFXCTL == nullptr) {
                this->m_pHeadSFXCTL = TmpSFXCTLArray[UsedIndex];
                CurEndElement = this->m_pHeadSFXCTL;
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
    SndBase *NewlyCreatedSFXObj = this->m_pStateMgr->CreateSFX(groupID, ecarsfx);
    if (NewlyCreatedSFXObj != nullptr) {
        NewlyCreatedSFXObj->SetupSFX(this);
        this->m_NumLoadedSFXObj++;
        if (this->m_pHeadSFXObj == nullptr) {
            this->m_pHeadSFXObj = NewlyCreatedSFXObj;
        } else {
            SndBase *LastSFXObj = this->m_pHeadSFXObj;
            while (LastSFXObj->m_pNextSFX != nullptr) {
                LastSFXObj = LastSFXObj->m_pNextSFX;
            }

            LastSFXObj->m_pNextSFX = NewlyCreatedSFXObj;
        }
    }
}

SFXCTL *CSTATE_Base::NewSFXCtrl(int esfxctl) {
    int groupID;
    SndBase *NewSFXCTL = this->HasCtrlBeenAdded(esfxctl);
    if (NewSFXCTL == nullptr) {
        if (esfxctl == -1) {
            return nullptr;
        }

        groupID = m_InstNum;
        NewSFXCTL = this->m_pStateMgr->CreateSFXCTL(groupID, esfxctl);
        NewSFXCTL->SetupSFX(this);
        if (this->m_pHeadSFXCTL == nullptr) {
            this->m_pHeadSFXCTL = NewSFXCTL;
        } else {
            SndBase *LastSFX = this->m_pHeadSFXCTL;
            while (LastSFX->m_pNextSFX != nullptr) {
                LastSFX = LastSFX->m_pNextSFX;
            }

            LastSFX->m_pNextSFX = NewSFXCTL;
        }

        this->m_NumLoadedSFXCTL++;
    }
    return static_cast<SFXCTL *>(NewSFXCTL);
}

SFXCTL *CSTATE_Base::HasCtrlBeenAdded(int esfxctrl) {
    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        if (CurSFXCtl->GetObjectIndex() == esfxctrl) {
            return static_cast<SFXCTL *>(CurSFXCtl);
        }

        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }
    return nullptr;
}

void CSTATE_Base::LoadData() {
    SndBase *CurSFXOBj = this->m_pHeadSFXObj;

    while (CurSFXOBj != nullptr) {
        CurSFXOBj->SetupLoadData();
        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }
}

void CSTATE_Base::ProcessUpdate() {
    SndBase *CurSFXOBj = this->m_pHeadSFXObj;

    while (CurSFXOBj != nullptr) {
        CurSFXOBj->ProcessUpdate();
        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }
}

void CSTATE_Base::UpdateParams(float t) {
    if (!g_EAXIsPaused()) {
        this->t_DeltaTime = t;
        this->t_CurTime += t;
    } else {
        this->t_DeltaTime = 0.0f;
    }

    if (!IsAttached()) {
        return;
    }

    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        CurSFXCtl->UpdateParams(t);
        CurSFXCtl->UpdateMixerOutputs();
        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }

    SndBase *CurSFXOBj = this->m_pHeadSFXObj;
    while (CurSFXOBj != nullptr) {
        CurSFXOBj->UpdateParams(t);
        CurSFXOBj->UpdateMixerOutputs();
        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }
}

void CSTATE_Base::Destroy() {
    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        SndBase *TmpSFXCtl = CurSFXCtl->m_pNextSFX;
        delete CurSFXCtl;
        CurSFXCtl = TmpSFXCtl;
    }

    SndBase *CurSFXOBj = this->m_pHeadSFXObj;
    while (CurSFXOBj != nullptr) {
        SndBase *TmpSFXOBj = CurSFXOBj->m_pNextSFX;
        delete CurSFXOBj;
        CurSFXOBj = TmpSFXOBj;
    }

    this->m_pHeadSFXCTL = nullptr;
    this->m_pHeadSFXObj = nullptr;
    this->m_NumLoadedSFXObj = 0;
    this->m_NumLoadedSFXCTL = 0;
}

SndBase *CSTATE_Base::GetSFXObject(int SFXId) {
    SndBase *CurSFXOBj = this->m_pHeadSFXObj;

    while (CurSFXOBj != nullptr) {
        if (CurSFXOBj->GetSFX_ID() == SFXId) {
            return CurSFXOBj;
        }
        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }

    return nullptr;
}

SndBase *CSTATE_Base::GetSFXCTLObject(int SFXId) {
    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;

    while (CurSFXCtl != nullptr) {
        if (CurSFXCtl->GetSFX_ID() == SFXId) {
            return CurSFXCtl;
        }
        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }

    return CurSFXCtl;
}

bool CSTATE_Base::IsDataLoaded(void) {
    SndAssetQueue &cbs = gAEMSMgr.mWaitForResolve;
    for (SndAssetQueue::iterator i = cbs.begin(); i != cbs.end(); i++) {
        stSndAssetQueue &data = *i;
        if (data.pThis != nullptr && data.pThis->GetStateBase() == this) {
            return false;
        }
    }
    return true;
}

void CSTATE_Base::Attach(void *pAttachment) {
    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;

    this->m_pAttachment = pAttachment;
    this->bIsAttached = true;

    while (CurSFXCtl != nullptr) {
        CurSFXCtl->InitSFX();
        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }

    this->LoadData();
}

bool CSTATE_Base::Detach() {
    this->m_pAttachment = nullptr;
    this->bIsAttached = false;

    SndBase *CurSFXOBj = this->m_pHeadSFXObj;
    while (CurSFXOBj != nullptr) {
        CurSFXOBj->Detach();
        CurSFXOBj->TurnOffMixer();
        gAEMSMgr.mWaitForResolve.DeleteRefToAsset(CurSFXOBj);

        if (CurSFXOBj->GetOutputBlockPtr() != nullptr) {
            // TODO size?
            bMemSet(CurSFXOBj->GetOutputBlockPtr(), 0, 0x40);
        }

        CurSFXOBj = CurSFXOBj->m_pNextSFX;
    }

    SndBase *CurSFXCtl = this->m_pHeadSFXCTL;
    while (CurSFXCtl != nullptr) {
        CurSFXCtl->Detach();

        if (CurSFXCtl->GetOutputBlockPtr() != nullptr) {
            // TODO size?
            bMemSet(CurSFXCtl->GetOutputBlockPtr(), 0, 0x40);
        }

        CurSFXCtl = CurSFXCtl->m_pNextSFX;
    }

    return true;
}
