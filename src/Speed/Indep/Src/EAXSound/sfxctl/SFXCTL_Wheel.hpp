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
  protected:
    static TypeInfo s_TypeInfo;

  public:
    SFXCTL_Wheel();
    /* 0x28 */ char _pad_whl0[0x78 - 0x28]; // bVector2/float arrays
    /* 0x78 */ Attrib::Gen::simsurface LeftSideTerrain;
    /* 0x8c */ Attrib::Gen::simsurface RightSideTerrain;
    /* 0xa0 */ Attrib::Gen::simsurface PrevLeftSideTerrain;
    /* 0xb4 */ Attrib::Gen::simsurface PrevRightSideTerrain;

    ~SFXCTL_Wheel() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void InitSFX() override;
    void UpdateParams(float t) override;
    bVector3 *GetWheelPos(int wheelID, int numtires);
    void GenerateWheelPosition();
    void GenerateTerrainTypes();

    void UpdateTireParams();
};

#endif
