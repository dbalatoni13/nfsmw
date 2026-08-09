//
//
#ifndef CARSFX_SHIFT_H
#define CARSFX_SHIFT_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENGINES_AEMS2.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"

// total size: 0x58
// Decl: 12
class CARSFX_Shift : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_Shift();
    ~CARSFX_Shift() override;

    // Overrides: SndBase
    void SetupLoadData() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void UpdateMixerOutputs() override;

  private:
    void PlayDisengageSnd();
    void PlayEngageSnd();
    void PlayAccelSnd();
    void PlayDecelSnd();
    void PlayShiftSnd();
    void PlayGearWhine();
    void PlayBrakesMashed();

    Csis::FX_SHIFTING_01 *m_ShiftGear;  // offset 0x28, size 0x4, Decl: 41
    Csis::FX_SHIFTING_01 *m_BrakePedal; // offset 0x2C, size 0x4, Decl: 42

    Csis::CAR_SWTN *m_DeccelSnd; // offset 0x30, size 0x4, Decl: 44
    Csis::CAR_SWTN *m_AccelSnd;  // offset 0x34, size 0x4, Decl: 45
    Csis::CAR_SWTN *m_Disengage; // offset 0x38, size 0x4, Decl: 46
    Csis::CAR_SWTN *m_Engage;    // offset 0x3C, size 0x4, Decl: 47
    Csis::CAR_WHINE *m_Whine;    // offset 0x40, size 0x4, Decl: 48

    SFXCTL_3DCarPos *m_p3DCarPosCtl;     // offset 0x44, size 0x4, Decl: 51
    SFXCTL_Shifting *m_pShiftCtl;        // offset 0x48, size 0x4, Decl: 52
    SFXCTL_AccelTrans *m_pAccelTransCtl; // offset 0x4C, size 0x4, Decl: 53

    Attrib::Gen::shiftpattern *m_pShiftingPatternData; // offset 0x50, size 0x4, Decl: 55
    Attrib::Gen::engineaudio *m_pSweetnersData;        // offset 0x54, size 0x4, Decl: 56
};

#endif
