#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_FEHUD.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

DEFINE_CREATABLE(0x70, SFXObj_FEHUD, SndBase);

SFXObj_FEHUD::SFXObj_FEHUD() : CARSFX() {}

SFXObj_FEHUD::~SFXObj_FEHUD() {
    g_pEAXSound->SetSFXBaseObject(nullptr, eMM_MAIN, 7, 0);
}

void SFXObj_FEHUD::InitSFX() {
    SndBase::InitSFX();
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 7, 0);
}

void SFXObj_FEHUD::Destroy() {}

void SFXObj_FEHUD::UpdateMixerOutputs() {
    // TODO what determines this size?
    bMemSet(GetOutputBlockPtr(), 0, sizeof(GameFlowSndState));
    bMemCpy(GetOutputBlockPtr(), GameFlowSndState, sizeof(GameFlowSndState));
}
