//
//
//
//
//
//
//
//
//
#ifndef SFX_COMMON_H
#define SFX_COMMON_H // Decl: 11

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x38
// Decl: 23
class SFX_Common : public SFX_Base {
  public:
    DECLARE_CREATABLE();
    SFX_Common();
    ~SFX_Common() override;

    // Overrides: SndBase
    void AttachController(SFXCTL *psfxctl) override;
    void InitSFX() override {} // Decl: 32
    void Destroy() override;
    void SetupLoadData() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    void MsgPlayMiscSound(const MMiscSound &message);

    Csis::FX_Camera *m_pcsisCameraShot;  // offset 0x24, size 0x4, Decl: 39
    Csis::FX_UVES *m_pUves;              // offset 0x28, size 0x4, Decl: 40
#ifndef EA_BUILD_A124
    Csis::FX_UVES *m_pPursuitBreakStart; // offset 0x2C, size 0x4, Decl: 41
    Csis::FX_UVES *m_pPursuitBreakEnd;   // offset 0x30, size 0x4, Decl: 42
#endif

    Hermes::HHANDLER mMsgMiscSound; // offset 0x34, size 0x4, Decl: 45
};

#endif
