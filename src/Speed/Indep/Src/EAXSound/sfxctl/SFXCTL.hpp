//
#ifndef SFXCTL_HPP
#define SFXCTL_HPP

#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"

#define MAX_NUM_SFX_CTLS 64 // Decl: 8

class SFXCTL : public SndBase {
    DECLARE_TYPEINFO();

  public:
    SFXCTL();
    ~SFXCTL() override;
    void InitSFX() override;
    void UpdateParams(float t) override;

    float GetPhysTRQ() {}
    float GetPhysRPM() {}

    eAemsUpgradeLevel m_UGL; // Decl: 26
};

#endif
