#ifndef SFXCTL_HELICOPTER_H
#define SFXCTL_HELICOPTER_H

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"

// total size: 0x64
// Decl: 11
class SFXCTL_3DHeliPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DHeliPos() {}           // Decl: 14
    ~SFXCTL_3DHeliPos() override {} // Decl: 15
};

// total size: 0x6C
// Decl: 21
class SFXCTL_Helicopter : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_Helicopter();
    ~SFXCTL_Helicopter() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void Detach() override;

    EAX_HeliState *m_pHeliState;       // offset 0x28, size 0x4, Decl: 34
    SFXCTL_3DHeliPos *m_p3DHeliPosCtl; // offset 0x2C, size 0x4, Decl: 35

    ALIGNVEC bVector3 vHeliPos; // offset 0x30, size 0x10, Decl: 37
    ALIGNVEC bVector3 vHeliFwd; // offset 0x40, size 0x10, Decl: 38
    ALIGNVEC bVector3 vHeliVel; // offset 0x50, size 0x10, Decl: 39

    float m_fspeed; // offset 0x60, size 0x4, Decl: 44
    float m_fdist;  // offset 0x64, size 0x4, Decl: 45
    int m_Rotation; // offset 0x68, size 0x4, Decl: 46
};

#endif
