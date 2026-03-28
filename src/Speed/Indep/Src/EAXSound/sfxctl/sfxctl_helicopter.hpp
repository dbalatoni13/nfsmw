#ifndef EAXSOUND_SFXCTL_SFXCTL_HELICOPTER_H
#define EAXSOUND_SFXCTL_SFXCTL_HELICOPTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

struct SFXCTL_3DHeliPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    ~SFXCTL_3DHeliPos() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
};

struct EAX_HeliState;

struct SFXCTL_Helicopter : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_Helicopter();
    /* 0x28 */ EAX_HeliState *m_pHeliState;
    /* 0x2c */ SFXCTL_3DHeliPos *m_p3DHeliPosCtl;
    /* 0x30 */ bVector3 vHeliPos;
    /* 0x40 */ bVector3 vHeliFwd;
    /* 0x50 */ bVector3 vHeliVel;
    /* 0x60 */ float m_fspeed;
    /* 0x64 */ float m_fdist;
    /* 0x68 */ int m_Rotation;

    ~SFXCTL_Helicopter() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    int GetController(int Index) override;
    void AttachController(SFXCTL *) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void Detach() override;
};

#endif
