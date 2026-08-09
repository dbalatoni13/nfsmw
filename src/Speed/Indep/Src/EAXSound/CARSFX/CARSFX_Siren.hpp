//
//
//
//
#ifndef CARSFX_SIREN_H
#define CARSFX_SIREN_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/COP_SIREN_AEMS.h"

// total size: 0x40
// Decl: 14
class CARSFX_Siren : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_Siren();
    ~CARSFX_Siren() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void ProcessUpdate() override;
    void UpdateParams(float t) override;
    void Detach() override;

  private:
    Sound::SirenState UpdateSirenState(float t);

    Csis::SIREN *mSiren;                // offset 0x28, size 0x4, Decl: 37
    float tSirenState;                  // offset 0x2C, size 0x4, Decl: 40
    bool bIgnoreHealth;                 // offset 0x30, size 0x1, Decl: 41
    Timer mT_death;                     // offset 0x34, size 0x4, Decl: 43
    Sound::SirenState m_SirenState;     // offset 0x38, size 0x4, Decl: 45
    Sound::SirenState m_PrevSirenState; // offset 0x3C, size 0x4, Decl: 46
};

#endif
