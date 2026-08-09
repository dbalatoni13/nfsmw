#ifndef SFXCTL_MASTER_VOL_H
#define SFXCTL_MASTER_VOL_H

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

// total size: 0x28
// Decl: 10
class SFXCTL_MasterVol : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_MasterVol();
    ~SFXCTL_MasterVol() override;

    void SetupSFX();

    // Overrides: SndBase
    void InitSFX() override;
    void UpdateParams(float t) override;
};

// total size: 0x28
// Decl: 31
class SFXCTL_GameState : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_GameState() {}           // Decl: 31
    ~SFXCTL_GameState() override {} // Decl: 31

    // Overrides: SndBase
    void UpdateMixerOutputs() override;
};

#endif
