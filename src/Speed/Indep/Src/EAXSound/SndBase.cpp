#include "./SndBase.hpp"

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

int SndBase::GetDMixOutput(int idx, DMX_PRESET_TYPE etype) {}

char *SndBase::GetTypeName(void) const {
    return s_TypeInfo.typeName;
}
