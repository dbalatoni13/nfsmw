#ifndef EAXSOUND_CARSFX_CARSFX_SHIFTING_H
#define EAXSOUND_CARSFX_CARSFX_SHIFTING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/ENGINES_AEMS2.h"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/shiftpattern.h"

class CARSFX_Shift : public CARSFX {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    CARSFX_Shift();
    ~CARSFX_Shift() override;

    static SndBase *CreateObject(unsigned int allocator);

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    void UpdateMixerOutputs() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void SetupLoadData() override;

    void PlayGearWhine();
    void PlayDisengageSnd();
    void PlayEngageSnd();
    void PlayAccelSnd();
    void PlayDecelSnd();
    void PlayShiftSnd();
    void PlayBrakesMashed();

  private:
    FX_SHIFTING_01 *m_ShiftGear;                       // offset 0x28, size 0x4
    FX_SHIFTING_01 *m_BrakePedal;                      // offset 0x2C, size 0x4
    CAR_SWTN *m_DeccelSnd;                             // offset 0x30, size 0x4
    CAR_SWTN *m_AccelSnd;                              // offset 0x34, size 0x4
    CAR_SWTN *m_Disengage;                             // offset 0x38, size 0x4
    CAR_SWTN *m_Engage;                                // offset 0x3C, size 0x4
    CAR_WHINE *m_Whine;                                // offset 0x40, size 0x4
    SFXCTL_3DCarPos *m_p3DCarPosCtl;                   // offset 0x44, size 0x4
    SFXCTL_Shifting *m_pShiftCtl;                      // offset 0x48, size 0x4
    SFXCTL_AccelTrans *m_pAccelTransCtl;               // offset 0x4C, size 0x4
    Attrib::Gen::shiftpattern *m_pShiftingPatternData; // offset 0x50, size 0x4
    Attrib::Gen::engineaudio *m_pSweetnersData;        // offset 0x54, size 0x4
};

#endif
