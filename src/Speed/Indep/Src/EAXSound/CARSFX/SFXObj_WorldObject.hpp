#ifndef SFXOBJ_FOUNTAIN_HPP
#define SFXOBJ_FOUNTAIN_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

// total size: 0x64
// Decl: 8
class SFXCTL_3DFountainPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();
    SFXCTL_3DFountainPos() {}           // Decl: 11
    ~SFXCTL_3DFountainPos() override {} // Decl: 12
};

// Decl: 19
class SFXObj_WorldObject : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_WorldObject();
    ~SFXObj_WorldObject() override;

    // Overrides: SndBase
    void InitSFX() override;
    void Destroy() override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void Detach() override;

    Csis::ENV_STATIC *m_pcsisSFX; // offset 0x28, size 0x4, Decl: 41
    ALIGNVEC bVector3 mObjPos;    // offset 0x30, size 0x10, Decl: 42

    SFXCTL_3DObjPos *m_p3DObjPos; // offset 0x40, size 0x4, Decl: 44
};

#endif
