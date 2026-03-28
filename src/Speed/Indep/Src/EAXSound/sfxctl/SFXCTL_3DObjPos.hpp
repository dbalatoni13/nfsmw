#ifndef EAXSOUND_SFXCTL_SFXCTL_3DOBJPOS_H
#define EAXSOUND_SFXCTL_SFXCTL_3DOBJPOS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct bVector2;
struct bVector3;

struct SFXCTL_3DObjPos : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }
    static bVector2 m_v2ObjPosCopy;
    static bVector2 *m_pv2AzimRefDir;
    static bVector2 *m_pv2AzimRefPos;
    static unsigned short m_CameraAngle;
    /* 0x28 */ bVector3 *m_pV3ObjPos;
    /* 0x2c */ bVector3 *m_pV3ObjDir;
    /* 0x30 */ bVector3 *m_pV3ObjVel;
    /* 0x34 */ float m_fDistToRef[2][2];

  public:
    /* 0x44 */ int m_PlayerRef;

  protected:
    /* 0x48 */ bool m_bIsInTwoPlayerTransitionZone;
    /* 0x4c */ bool m_bDirectionClockwise;
    /* 0x50 */ bool m_bIsOtherCamRightSide;
    /* 0x54 */ float m_fdvelmag_car[2];
    /* 0x5c */ float m_fdvelmag_cam[2];

  public:
    SFXCTL_3DObjPos();
    ~SFXCTL_3DObjPos() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int);
    void Detach() override;
    void SetCameraAngle();
    virtual void Generate3DParams(int);
    void UpdateParams(float t) override;
    void UpdateDoppler(int PlayerNum, float t);

    void GenerateSinglePlayerMix();
    virtual void AssignPositionVector(bVector3 *pV3ObjPos);
    virtual void AssignDirectionVector(const bVector3 *pV3ObjDir);
    virtual void AssignVelocityVector(const bVector3 *pV3ObjVel);

    void SetPlayerRef(int i);
};

#endif
