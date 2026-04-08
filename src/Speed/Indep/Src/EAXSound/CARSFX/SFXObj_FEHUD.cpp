#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_FEHUD.hpp"

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

extern int GameFlowSndState[];

SndBase::TypeInfo SFXObj_FEHUD::s_TypeInfo = {
    0x00000070,
    "SFXObj_FEHUD",
    &SndBase::s_TypeInfo,
    SFXObj_FEHUD::CreateObject,
};

SndBase::TypeInfo *SFXObj_FEHUD::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_FEHUD::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_FEHUD::CreateObject(unsigned int allocator) {
    if (allocator == 0) {
        return new (GetStaticTypeInfo()->typeName, false) SFXObj_FEHUD();
    }
    return new (GetStaticTypeInfo()->typeName, true) SFXObj_FEHUD();
}

SFXObj_FEHUD::SFXObj_FEHUD()
    : CARSFX() {}

SFXObj_FEHUD::~SFXObj_FEHUD() {
    g_pEAXSound->SetSFXBaseObject(nullptr, eMM_MAIN, 7, 0);
}

void SFXObj_FEHUD::InitSFX() {
    SndBase::InitSFX();
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 7, 0);
}

void SFXObj_FEHUD::Destroy() {}

void SFXObj_FEHUD::UpdateMixerOutputs() {
    bMemSet(GetOutputBlockPtr(), '\0', 0x3C);
    bMemCpy(GetOutputBlockPtr(), GameFlowSndState, 0x3C);
}
int SFXObj_FEHUD::GetController(int Index) {
    return -1;
}

void SFXObj_FEHUD::AttachController(SFXCTL *psfxctl) {}

