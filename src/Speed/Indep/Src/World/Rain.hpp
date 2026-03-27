#ifndef WORLD_RAIN_H
#define WORLD_RAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

// total size: 0x47C
struct Rain {
    OnScreenRain OSrain;                 // offset 0x0, size 0x4
    int NoRain;                          // offset 0x4, size 0x4
    int NoRainAhead;                     // offset 0x8, size 0x4
    int inTunnel;                        // offset 0xC, size 0x4
    int inOverpass;                      // offset 0x10, size 0x4
    void *the_zone;                      // offset 0x14, size 0x4
    bVector3 outvex;                     // offset 0x18, size 0x10
    bVector2 twoDpos;                    // offset 0x28, size 0x8
    bVector3 CamVelLOCAL;                // offset 0x30, size 0x10
    CurtainStatus IsValidRainCurtainPos; // offset 0x40, size 0x4
    int renderCount;                     // offset 0x44, size 0x4
  private:
    eView *MyView;                    // offset 0x48, size 0x4
    float intensity;                  // offset 0x4C, size 0x4
    float CloudIntensity;             // offset 0x50, size 0x4
    uint32 DesiredActive;             // offset 0x54, size 0x4
    uint32 NewSwapBuffer;             // offset 0x58, size 0x4
    uint32 OldSwapBuffer;             // offset 0x5C, size 0x4
    int32 NumRainPoints;              // offset 0x60, size 0x4
    unsigned int NumOfType[2];        // offset 0x64, size 0x8
    unsigned int DesiredNumOfType[2]; // offset 0x6C, size 0x8
    float Percentages[2];             // offset 0x74, size 0x8
    float precipWindEffect[2][2];     // offset 0x7C, size 0x10
    TextureInfo *texture_info[2];     // offset 0x8C, size 0x8
    bVector3 OldCarDirection;         // offset 0x94, size 0x10
    bVector3 precipRadius[2];         // offset 0xA4, size 0x20
    bVector3 precipSpeedRange[2];     // offset 0xC4, size 0x20
    float precipZconstant[2];         // offset 0xE4, size 0x8
    RainWindType windType[2];         // offset 0xEC, size 0x8
    float CameraSpeed;                // offset 0xF4, size 0x4
    bVector3 windSpeed;               // offset 0xF8, size 0x10
    bVector3 DesiredwindSpeed;        // offset 0x108, size 0x10
    float DesiredWindTime;            // offset 0x118, size 0x4
    float windTime;                   // offset 0x11C, size 0x4
    uint32 fogR;                      // offset 0x120, size 0x4
    uint32 fogG;                      // offset 0x124, size 0x4
    uint32 fogB;                      // offset 0x128, size 0x4
    bVector2 aabbMax;                 // offset 0x12C, size 0x8
    bVector2 aabbMin;                 // offset 0x134, size 0x8
    ePoly PRECIPpoly[2];              // offset 0x13C, size 0x128
    bMatrix4 local2world;             // offset 0x264, size 0x40
    bMatrix4 world2localrot;          // offset 0x2A4, size 0x40
    float LenModifier;                // offset 0x2E4, size 0x4
    float DesiredIntensity;           // offset 0x2E8, size 0x4
    float DesiredCloudyness;          // offset 0x2EC, size 0x4
    float DesiredRoadDampness;        // offset 0x2F0, size 0x4
    float RoadDampness;               // offset 0x2F4, size 0x4
    float percentPrecip[2];           // offset 0x2F8, size 0x8
    bVector3 PrevailingWindSpeed;     // offset 0x300, size 0x10
    float WeatherTime;                // offset 0x310, size 0x4
    float DesiredWeatherTime;         // offset 0x314, size 0x4
    bVector3 Velocities[10][2];       // offset 0x318, size 0x140
    bVector2 ent0;                    // offset 0x458, size 0x8
    bVector2 ent1;                    // offset 0x460, size 0x8
    bVector2 ext0;                    // offset 0x468, size 0x8
    bVector2 ext1;                    // offset 0x470, size 0x8
    uint8 entFLAG;                    // offset 0x478, size 0x1
    uint8 extFLAG;                    // offset 0x479, size 0x1

  public:
    Rain(eView *view, RainType StartType);
    void Init(RainType type, float percent);

    float GetRainIntensity() {
        return intensity;
    }

    float GetCloudIntensity() {
        return CloudIntensity;
    }

    float GetRoadDampness() {
        return RoadDampness;
    }

    void SetPrecipFogColour(unsigned int r, unsigned int g, unsigned int b) {}

    float GetAmount(RainType type) {}

    void SetRoadDampness(float damp) {}

    bVector3 *GetWind() {}

    void GetPrecipFogColour(unsigned int *r, unsigned int *g, unsigned int *b) {
        *r = this->fogR;
        *g = this->fogG;
        *b = this->fogB;
    }

    void AttachRainCurtain(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
};

int AmIinATunnel(eView *view, int CheckOverPass);
int AmIinATunnelSlow(eView *view, int CheckOverPass);
void SetRainBase();

#endif
