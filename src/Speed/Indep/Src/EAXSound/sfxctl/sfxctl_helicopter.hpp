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
    ~SFXCTL_3DHeliPos() override {}
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
    EAX_HeliState *m_pHeliState;                 // offset 0x28, size 0x4
    SFXCTL_3DHeliPos *m_p3DHeliPosCtl;           // offset 0x2C, size 0x4
    bVector3 vHeliPos;                           // offset 0x30, size 0x10
    bVector3 vHeliFwd;                           // offset 0x40, size 0x10
    bVector3 vHeliVel;                           // offset 0x50, size 0x10
    float m_fspeed;                              // offset 0x60, size 0x4
    float m_fdist;                               // offset 0x64, size 0x4
    int m_Rotation;                              // offset 0x68, size 0x4

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
