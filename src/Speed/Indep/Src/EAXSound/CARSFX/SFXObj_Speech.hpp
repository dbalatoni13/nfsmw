#ifndef SFXOBJ_SPEECH_HPP
#define SFXOBJ_SPEECH_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

// total size: 0x44
// Decl: 9
class SFXObj_Speech : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_Speech();
    ~SFXObj_Speech() override;

    // Overrides: SndBase
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override {} // Decl: 21
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;

    SFXCTL_3DObjPos *m_pActorPos; // offset 0x28, size 0x4, Decl: 26
    bVector3 fPosition;           // offset 0x2C, size 0x10, Decl: 27
    bool bFresh;                  // offset 0x3C, size 0x1, Decl: 28
    EAX_CarState *m_pClosestCar;  // offset 0x40, size 0x4, Decl: 29
};

// total size: 0x64
// Decl: 34
class SFXCTL_3DVoiceActorPos : SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DVoiceActorPos() {} // Decl: 34
};

#endif
