#ifndef SFXOBJ_AMBIENCE_HPP
#define SFXOBJ_AMBIENCE_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"

// total size: 0x28
// Decl: 7
class SFXObj_Ambience : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_Ambience();
    ~SFXObj_Ambience() override;

    // Overrides: SndBase
    int GetController(int Index) override {
        return -1;
    } // Decl: 13
    void AttachController(struct SFXCTL *psfxctl) override {} // Decl: 14
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override {} // Decl: 20
};

#endif
