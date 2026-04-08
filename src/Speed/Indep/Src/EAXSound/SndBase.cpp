#include "./SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

SndBase::TypeInfo *SndBase::GetTypeInfo() const {
    return &s_TypeInfo;
}

SndBase::SndBase() {
    int i = 0;
    do {
        i++;
        m_refCount = 0;
    } while (i < 16);
    m_pEAXCar = nullptr;
    m_pStateBase = nullptr;
    m_pOutPutBlock = nullptr;
    m_pInputBlock = nullptr;
    m_pNextSFX = nullptr;
    m_bIsEnabled = false;
    m_fDeltaTime = 0.0f;
    m_fRunningTime = 0.0f;
}

SndBase::~SndBase() {
ReprocessQueue:
    SndAssetQueue::iterator p_Var4 = gAEMSMgr.mWaitForResolve.begin();
    SndBase *pSVar8;
    int iVar5;
    int iVar7;

    while (true) {
        if (p_Var4 == gAEMSMgr.mWaitForResolve.end()) {
            return;
        }

        iVar5 = (*p_Var4).Asset.FileName.GetHash32();
        iVar7 = reinterpret_cast<int>((*p_Var4).Asset.FileName.GetString());
        pSVar8 = (*p_Var4).pThis;
        if (pSVar8 == this) {
            break;
        }
        ++p_Var4;
    }

    p_Var4 = gAEMSMgr.mWaitForResolve.begin();
    while (p_Var4 != gAEMSMgr.mWaitForResolve.end()) {
        SndAssetQueue::iterator ptr = p_Var4;
        ++p_Var4;
        if (pSVar8 == (*ptr).pThis &&
            iVar5 == (*ptr).Asset.FileName.GetHash32() &&
            iVar7 == reinterpret_cast<int>((*ptr).Asset.FileName.GetString())) {
            gAEMSMgr.mWaitForResolve.remove(*ptr);
        }
    }

    goto ReprocessQueue;
}

int SndBase::GetDMixOutput(int idx, DMX_PRESET_TYPE etype) {
    if (m_pInputBlock) {
        int shift = (idx & 1) << 4;
        int out = m_pInputBlock[idx >> 1];

        if (etype == DMX_FREQ) {
            return (out >> shift) & 0x7FFF;
        }

        if (etype < DMX_AZIM) {
            if (etype == DMX_VOL) {
                return (out >> shift) & 0x7FFF;
            }

            if (etype == DMX_PITCH) {
                return NFSMixShape::GetIntPitchMultFromCents((out >> shift) & 0xFFFF);
            }
        } else if (etype == DMX_AZIM) {
            if (g_pEAXSound->GetCurAudioSettings()->AudioMode != 0) {
                return (out >> shift) & 0xFFFF;
            }
        } else if (etype == DMX_DEPTH) {
            return (out >> shift) & 0x7FFF;
        }
    }

    return 0;
}

const char *SndBase::GetTypeName(void) const {
    return s_TypeInfo.typeName;
}

void SndBase::SetupSFX(CSTATE_Base *_StateBase) {
    eMAINMAPSTATES eVar1 = _StateBase->m_eStateType;

    if (static_cast<unsigned int>(eVar1 - eMM_PLAYERCAR) < 3 ||
        eVar1 == eMM_TRUCK || eVar1 == eMM_TRAFFIC) {
        m_pEAXCar = reinterpret_cast<EAXCar *>(_StateBase);
    } else {
        m_pEAXCar = nullptr;
    }

    m_pStateBase = _StateBase;
}

void SndBase::LoadAsset(Attrib::StringKey filename,
                        eSNDDATAPATH path,
                        eSNDDATATYPE datatype,
                        eBANK_SLOT_TYPE SlotType,
                        bool LoadToTop) {
    stSndAssetQueue requeststruct;

    requeststruct.Asset.Clear();
    requeststruct.Asset.FileName = filename;
    requeststruct.pCar = GetPhysCar();
    requeststruct.pThis = this;

    if (!requeststruct.pCar && static_cast<unsigned int>(objectID >> 16) == eMM_PLAYERCAR) {
        requeststruct.pThis = nullptr;
    }

    requeststruct.Asset.eDataType = datatype;
    requeststruct.Asset.DataPath = path;
    requeststruct.Asset.bLoadToTop = LoadToTop;
    LoadAsset(requeststruct, SlotType);
}

void SndBase::LoadAsset(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType) {
    gAEMSMgr.QueueFileLoad(queueitem, SlotType);
}
