#ifndef EAXSOUND_SFXCTL_SFXCTL_3DOBJPOS_H
#define EAXSOUND_SFXCTL_SFXCTL_3DOBJPOS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct bVector3;

struct SFXCTL_3DObjPos : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    /* 0x28 */ bVector3 *m_pV3ObjPos;
    /* 0x2c */ bVector3 *m_pV3ObjDir;
    /* 0x30 */ bVector3 *m_pV3ObjVel;
    /* 0x34 */ float m_fDistToRef[2][2];

  public:
    /* 0x44 */ int m_PlayerRef;

    ~SFXCTL_3DObjPos() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    void Detach() override;

    virtual void Generate3DParams() {}
    virtual void AssignPositionVector(bVector3 *pV3ObjPos);
    virtual void AssignDirectionVector(const bVector3 *pV3ObjDir);
    virtual void AssignVelocityVector(const bVector3 *pV3ObjVel);

    void SetPlayerRef(int i);
};

#endif
