#include "./SndBase.hpp"
#include "Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

SndBase::TypeInfo SndBase::s_TypeInfo = {(int)0xFFFFFFFF, "SndBase", nullptr, nullptr};

float SndBase::m_fDeltaTime = 0.0f;   // Decl: 18
float SndBase::m_fRunningTime = 0.0f; //  Decl: 19

SndBase::TypeInfo *SndBase::GetTypeInfo() const {
    return &s_TypeInfo;
}

SndBase::SndBase() {
    for (int n = 0; n < 16; n++) {
        this->m_refCount = 0;
    }
    this->m_pEAXCar = nullptr;
    this->m_pStateBase = nullptr;
    this->m_pOutPutBlock = nullptr;
    this->m_pInputBlock = nullptr;
    this->m_pNextSFX = nullptr;
    this->m_bIsEnabled = false;
    this->m_fDeltaTime = 0.0f;
    this->m_fRunningTime = 0.0f;
}

SndBase::~SndBase() {
    gAEMSMgr.mWaitForResolve.DeleteRefToAsset(this);
}

int SndBase::GetDMixOutput(int idx, DMX_PRESET_TYPE etype) {
    if (this->m_pInputBlock != nullptr) {
        int shift = (idx & 1) << 4;
        int out = this->m_pInputBlock[idx >> 1];

        switch (etype) {
            case DMX_PITCH:
                return NFSMixShape::GetIntPitchMultFromCents((out >> shift) & 0xFFFF);
            case DMX_AZIM:
                if (g_pEAXSound->GetCurAudioSettings()->AudioMode != 0) {
                    return (out >> shift) & 0xFFFF;
                }
                break;
            case DMX_VOL:
            case DMX_FREQ:
            case DMX_DEPTH:
                return (out >> shift) & 0x7FFF;
            default:
                break;
        }
    }
    return 0;
}

const char *SndBase::GetTypeName(void) const {
    return s_TypeInfo.typeName;
}

// STRIPPED
bool SndBase::IsOfExactType(const TypeInfo *typeInfo) const {
    return false;
}

// STRIPPED
bool SndBase::IsOfType(const TypeInfo *typeInfo) const {
    return false;
}

void SndBase::SetupSFX(CSTATE_Base *_StateBase) {
    if (_StateBase->m_eStateType == eMM_PLAYERCAR || _StateBase->m_eStateType == eMM_AIRACECAR || _StateBase->m_eStateType == eMM_COPCAR ||
        _StateBase->m_eStateType == eMM_TRUCK || _StateBase->m_eStateType == eMM_TRAFFIC) {
        m_pEAXCar = reinterpret_cast<EAXCar *>(_StateBase);
    } else {
        m_pEAXCar = nullptr;
    }

    this->SetStateBase(_StateBase);
}

// UNSOLVED
void SndBase::LoadAsset(Attrib::StringKey filename, eSNDDATAPATH path, eSNDDATATYPE datatype, eBANK_SLOT_TYPE SlotType, bool LoadToTop) {
    stSndAssetQueue requeststruct;

    requeststruct.pThis = this;
    requeststruct.pCar = this->GetPhysCar();
    requeststruct.Asset.FileName = filename;
    requeststruct.Asset.DataPath = path;
    requeststruct.Asset.eDataType = datatype;
    requeststruct.Asset.bLoadToTop = LoadToTop;

    if ((requeststruct.pCar == nullptr) && this->GetGroupID() == eMM_PLAYERCAR) {
        requeststruct.pThis = nullptr;
    }
    this->LoadAsset(requeststruct, SlotType);
}

void SndBase::LoadAsset(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType) {
    gAEMSMgr.QueueFileLoad(queueitem, SlotType);
}
