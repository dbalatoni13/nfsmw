#ifndef STATEMGR_PLAYERCAR_HPP
#define STATEMGR_PLAYERCAR_HPP

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"

// total size: 0x1C
// Decl: 10
class CSTATEMGR_PlayerCar : public CSTATEMGR_Base {
  public:
    CSTATEMGR_PlayerCar();
    ~CSTATEMGR_PlayerCar() override;

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
    void UpdateParams(float t) override;

    static bool IsTruck; // size: 0x1
};

// total size: 0x74
// Decl: 24
class SFXCTL_3DRearPos : public SFXCTL_3DCarPos {
  public:
    DECLARE_CREATABLE();
    SFXCTL_3DRearPos() {}           // Decl: 27
    ~SFXCTL_3DRearPos() override {} // Decl: 28

    // Overrides: SndBase
    void InitSFX() override;
    void UpdateParams(float t) override;

    ALIGNVEC bVector3 vRearPos; // offset 0x64, size 0x10, Decl: 34
};

#endif
