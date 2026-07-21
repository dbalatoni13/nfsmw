//
#ifndef SFXObj_Woosh_HPP
#define SFXObj_Woosh_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/STICH_Struct.h"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

// total size: 0x50
// Decl: 9
class SFXObj_Woosh : public CARSFX {
  public:
    DECLARE_CREATABLE();

    SFXObj_Woosh();
    ~SFXObj_Woosh() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Destroy() override;
    void Detach() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;

    SND_Stich *m_pStitchData;     // offset 0x28, size 0x4, Decl: 28
    SND_Params m_SndParams;       // offset 0x2C, size 0x18, Decl: 29
    cStichWrapper *m_pWooshStich; // offset 0x44, size 0x4, Decl: 30

    CSTATE_DriveBy *m_pDriveByState; // offset 0x48, size 0x4, Decl: 32

    SFXCTL_3DObjPos *m_p3DPos; // offset 0x4C, size 0x4, Decl: 34
};

// total size: 0x64
// Decl: 40
class SFXCTL_3DWooshPos : public SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DWooshPos() {}           // Decl: 42
    ~SFXCTL_3DWooshPos() override {} // Decl: 43
};

#endif
