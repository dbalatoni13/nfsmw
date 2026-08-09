#ifndef SFXOBJ_HELICOPTER_H
#define SFXOBJ_HELICOPTER_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Helicopter.hpp"

// total size: 0x60
// Decl: 9
class SFXObj_Helicopter : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_Helicopter();
    ~SFXObj_Helicopter() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Detach() override;

    Csis::FX_HelicopterStruct m_HeliAemsData; // offset 0x28, size 0x30, Decl: 25
    Csis::FX_Helicopter *m_pCsisHeli;         // offset 0x58, size 0x4, Decl: 26
    SFXCTL_Helicopter *m_pHeliCtl;            // offset 0x5C, size 0x4, Decl: 27
};

#endif
