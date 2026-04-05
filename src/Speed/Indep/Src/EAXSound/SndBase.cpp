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

SndBase::~SndBase() {}

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
