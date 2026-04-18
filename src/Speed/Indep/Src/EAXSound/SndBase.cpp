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
    if (static_cast<unsigned int>(_StateBase->m_eStateType - eMM_PLAYERCAR) < 3 ||
        _StateBase->m_eStateType == eMM_TRUCK || _StateBase->m_eStateType == eMM_TRAFFIC) {
        m_pEAXCar = reinterpret_cast<EAXCar *>(_StateBase);
    } else {
        m_pEAXCar = nullptr;
    }

    SetStateBase(_StateBase);
}

inline EAX_CarState *SndBase::GetPhysCar() {
    return m_pStateBase->GetPhysCar();
}

void SndBase::LoadAsset(Attrib::StringKey filename,
                        eSNDDATAPATH path,
                        eSNDDATATYPE datatype,
                        eBANK_SLOT_TYPE SlotType,
                        bool LoadToTop) {
    stSndAssetQueue requeststruct;

    requeststruct.Asset.Clear();
    requeststruct.pCar = nullptr;
    requeststruct.pThis = this;
    requeststruct.Asset.FileName = filename;
    requeststruct.Asset.DataPath = path;
    requeststruct.Asset.eDataType = datatype;
    requeststruct.Asset.bLoadToTop = LoadToTop;
    requeststruct.pCar = GetPhysCar();

    if (!requeststruct.pCar && reinterpret_cast<unsigned char *>(&objectID)[1] == eMM_PLAYERCAR) {
        requeststruct.pThis = nullptr;
    }
    LoadAsset(requeststruct, SlotType);
}

void SndBase::LoadAsset(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType) {
    gAEMSMgr.QueueFileLoad(queueitem, SlotType);
}
