#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Ambience.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

DEFINE_CREATABLE(0x20, SFXObj_Ambience, SndBase);

SFXObj_Ambience::SFXObj_Ambience() : CARSFX() {}

SFXObj_Ambience::~SFXObj_Ambience() {}

void SFXObj_Ambience::InitSFX() {
    SndBase::InitSFX();
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 2, 0);
}

void SFXObj_Ambience::Destroy() {}

void SFXObj_Ambience::UpdateParams(float t) {}
