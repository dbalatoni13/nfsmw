//
#ifndef SFXCTL_WHEEL_H
#define SFXCTL_WHEEL_H

#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

// total size: 0xF0
// Decl: 27
class SFXCTL_Wheel : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_Wheel();
    ~SFXCTL_Wheel() override;

    // Overrides: SndBase
    void UpdateParams(float t) override;
    void InitSFX() override;

    void UpdateTireParams();
    void GenerateWheelPosition();
    bVector3 *GetWheelPos(int wheelID, int numtires);
    void GenerateTerrainTypes();

    bVector2 m_NormWheelSlip[4]; // offset 0x28, size 0x20, Decl: 47

    bVector2 m_bvTotalRightWheelSlip; // offset 0x48, size 0x8, Decl: 49
    bVector2 m_bvTotalLeftWheelSlip;  // offset 0x50, size 0x8, Decl: 50

    float m_fWheelTractionMag[4]; // offset 0x58, size 0x10, Decl: 52
    float m_fLoad[4];             // offset 0x68, size 0x10, Decl: 53

    Attrib::Gen::simsurface LeftSideTerrain;  // offset 0x78, size 0x14, Decl: 57
    Attrib::Gen::simsurface RightSideTerrain; // offset 0x8C, size 0x14, Decl: 58

    Attrib::Gen::simsurface PrevLeftSideTerrain;  // offset 0xA0, size 0x14, Decl: 65
    Attrib::Gen::simsurface PrevRightSideTerrain; // offset 0xB4, size 0x14, Decl: 66
    bVector3 v3NewPosLeft;                        // offset 0xC8, size 0x10
    bVector3 v3NewPosRight;                       // offset 0xD8, size 0x10

    bool LeftSideTouchingGround;  // offset 0xE8, size 0x1, Decl: 81
    bool RightSideTouchingGround; // offset 0xEC, size 0x1, Decl: 82
};

#endif
