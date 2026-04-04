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
    bVector3 *m_pV3ObjPos;                       // offset 0x28, size 0x4
    bVector3 *m_pV3ObjDir;                       // offset 0x2C, size 0x4
    bVector3 *m_pV3ObjVel;                       // offset 0x30, size 0x4
    float m_fDistToRef[2][2];                    // offset 0x34, size 0x10
    int m_PlayerRef;                             // offset 0x44, size 0x4
    bool m_bIsInTwoPlayerTransitionZone;         // offset 0x48, size 0x1
    bool m_bDirectionClockwise;                  // offset 0x4C, size 0x1
    bool m_bIsOtherCamRightSide;                 // offset 0x50, size 0x1
    float m_fdvelmag_car[2];                     // offset 0x54, size 0x8
    float m_fdvelmag_cam[2];                     // offset 0x5C, size 0x8

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
