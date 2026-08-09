#ifndef SFXOBJ_FEHUD_HPP
#define SFXOBJ_FEHUD_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"

// total size: 0x28
// Decl: 7
class SFXObj_FEHUD : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_FEHUD();
    ~SFXObj_FEHUD() override;

    // Overrides: SndBase
    int GetController(int Index) override {
        return -1;
    }                                                  // Decl: 13
    void AttachController(SFXCTL *psfxctl) override {} // Decl: 14
    void InitSFX() override;
    void Destroy() override;
    void UpdateMixerOutputs() override;
};

#endif
