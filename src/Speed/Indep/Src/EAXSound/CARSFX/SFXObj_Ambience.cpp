#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Ambience.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

SndBase::TypeInfo SFXObj_Ambience::s_TypeInfo = {
    0x00000020,
    "SFXObj_Ambience",
    &SndBase::s_TypeInfo,
    SFXObj_Ambience::CreateObject,
};

SFXObj_Ambience::TypeInfo *SFXObj_Ambience::GetStaticTypeInfo() {
    return &s_TypeInfo;
}

SFXObj_Ambience::TypeInfo *SFXObj_Ambience::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_Ambience::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_Ambience::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) SFXObj_Ambience();
    }
    return new (GetStaticTypeInfo()->typeName, true) SFXObj_Ambience();
}

SFXObj_Ambience::SFXObj_Ambience()
    : CARSFX() {}

SFXObj_Ambience::~SFXObj_Ambience() {}

void SFXObj_Ambience::InitSFX() {
    int *inputBlock = GetInputBlockPtr();
    if (inputBlock) {
        inputBlock[15] = 1;
    }
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 2, 0);
}

void SFXObj_Ambience::Destroy() {}

void SFXObj_Ambience::UpdateParams(float t) {}
