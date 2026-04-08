#ifndef EAXSOUND_SFXCTL_SFXCTL_WHEEL_H
#define EAXSOUND_SFXCTL_SFXCTL_WHEEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"

struct bVector2;
struct bVector3;

struct SFXCTL_Wheel : public SFXCTL {
  public:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXCTL_Wheel();
    bVector2 m_NormWheelSlip[4];                 // offset 0x28, size 0x20
    bVector2 m_bvTotalRightWheelSlip;            // offset 0x48, size 0x8
    bVector2 m_bvTotalLeftWheelSlip;             // offset 0x50, size 0x8
    float m_fWheelTractionMag[4];                // offset 0x58, size 0x10
    float m_fLoad[4];                            // offset 0x68, size 0x10
    Attrib::Gen::simsurface LeftSideTerrain;     // offset 0x78, size 0x14
    Attrib::Gen::simsurface RightSideTerrain;    // offset 0x8C, size 0x14
    Attrib::Gen::simsurface PrevLeftSideTerrain; // offset 0xA0, size 0x14
    Attrib::Gen::simsurface PrevRightSideTerrain; // offset 0xB4, size 0x14
    bVector3 v3NewPosLeft;                       // offset 0xC8, size 0x10
    bVector3 v3NewPosRight;                      // offset 0xD8, size 0x10
    bool LeftSideTouchingGround;                 // offset 0xE8, size 0x1
    bool RightSideTouchingGround;                // offset 0xEC, size 0x1

    ~SFXCTL_Wheel() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void InitSFX() override;
    void UpdateParams(float t) override;
    bVector3 *GetWheelPos(int wheelID, int numtires);
    void GenerateWheelPosition();
    void GenerateTerrainTypes();

    void UpdateTireParams();
};

#endif
