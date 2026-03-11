#ifndef EAXSOUND_SFXCTL_SFXCTL_ENGINE_H
#define EAXSOUND_SFXCTL_SFXCTL_ENGINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_Shifting;
struct SFXCTL_AccelTrans;
struct SFXCTL_3DCarPos;
struct MCountdownDone;

struct SFXCTL_Engine : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    /* 0x28 */ SFXCTL_Shifting *m_pShiftCtl;
    /* 0x2c */ SFXCTL_AccelTrans *m_pAccelTransitionCtl;
    /* 0x30 */ struct SFXCTL_Physics *m_pPhysicsCtl;
    /* 0x34 */ SFXCTL_3DCarPos *m_p3DCarPosCtl;
    /* 0x38 */ char _pad_engine0[0x88]; // 0x38 to 0xc0
    /* 0xc0 */ float tMergeWithPhysicsOffStart;
    /* 0xc4 */ int bPreRace;
    /* 0xc8 */ char _pad_engine1[0x4C]; // 0xc8 to 0x114
    /* 0x114 */ float m_fEng_RPM;
    /* 0x118 */ float m_fPrevRPM;
    /* 0x11c */ float m_fSmoothedEng_RPM;
    /* 0x120 */ float m_fEng_Trq;
    /* 0x124 */ float m_fSmoothedEng_Trq;

    ~SFXCTL_Engine() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;

    virtual float GetEngRPM();
    virtual float GetSmoothedEngRPM();
    virtual float GetEngTorque();
    virtual float GetSmoothedEngTorque();

    void MsgCountdownDone(const MCountdownDone &message);
};

#endif
