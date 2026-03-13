#ifndef EAXSOUND_SFXCTL_SFXCTL_HELICOPTER_H
#define EAXSOUND_SFXCTL_SFXCTL_HELICOPTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

struct SFXCTL_3DHeliPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    ~SFXCTL_3DHeliPos() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
};

struct EAX_HeliState;

struct SFXCTL_Helicopter : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    SFXCTL_Helicopter();
    /* 0x28 */ EAX_HeliState *m_pHeliState;
    /* 0x2c */ SFXCTL_3DHeliPos *m_p3DHeliPosCtl;

    ~SFXCTL_Helicopter() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    int GetController(int Index) override;
    void AttachController(SFXCTL *) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void Detach() override;
};

#endif
