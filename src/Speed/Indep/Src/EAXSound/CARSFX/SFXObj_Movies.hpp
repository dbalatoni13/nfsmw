//
#ifndef SFXOBJ_MOVIES_HPP
#define SFXOBJ_MOVIES_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

// total size: 0x28
// Decl: 6
class SFXObj_Movies : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_Movies();
    ~SFXObj_Movies() override;

    // Overrides: SndBase
    int GetController(int Index) override {}           // Decl: 12
    void AttachController(SFXCTL *psfxctl) override {} // Decl: 13

    void SetupSFX(CSTATE_Base *_StateBase) override {} // Decl: 15
    void InitSFX() override {}                         // Decl: 16
    void Destroy() override;
    void UpdateParams(float t) override {} // Decl: 18
    void ProcessUpdate() override {}       // Decl: 19
};

#endif
