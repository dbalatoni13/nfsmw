#ifndef RAIN_HPP
#define RAIN_HPP

#ifdef EA_BUILD_A124
#define MAXRAINPOINTS 400
#else
#define MAXRAINPOINTS 350
#endif

#define MAX_OS_RAINPOINTS 20
#define MAXCURTAINRAINPOINTS 200

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

enum CurtainStatus {
    CT_INACTIVE = 0,
    CT_ACTIVE = 1,
    CT_TURNON = 2,
    CT_OVERIDE = 3,
};

enum RainType {
    RAIN = 0,
    INACTIVE = 1,
    NUMTYPES = 2,
};

enum RainSubType {
    LIGHT = 0,
    HEAVY = 1,
    NUMSUBTYPES = 2,
};

enum RainWindType {
    POINT_WIND = 0,
    VECTOR_WIND = 1,
    NUMWINDTYPES = 2,
};

enum ChangingStatus {
    STEADY = 0,
    CHANGE = 1,
    CHANGING = 2,
};

#define IN_LIGHT 0
#define IN_DARK 1

typedef struct RainPointsDef {
    bVector3 NormalizedPoint[2]; // offset 0x0, size 0x20
} RainPointsDef;

typedef struct RainPointsDef2 {
    uint8 status;       // offset 0x0, size 0x1
    uint8 type;         // offset 0x1, size 0x1
    uint8 subType;      // offset 0x2, size 0x1
    uint8 InHeadlights; // offset 0x3, size 0x1
} RainPointsDef2;

static const int NUM_DRIP_SHAPES = 4; // size: 0x4

// total size: 0x1C
typedef struct OnScreenRainPointsDef {
    float x;         // offset 0x0, size 0x4
    float y;         // offset 0x4, size 0x4
    float timer;     // offset 0x8, size 0x4
    float Maxtimer;  // offset 0xC, size 0x4
    float Size;      // offset 0x10, size 0x4
    float DripSpeed; // offset 0x14, size 0x4
    int DripShape;   // offset 0x18, size 0x4
} OnScreenRainPointsDef;

// total size: 0x4
class OnScreenRain {
  public:
    friend class Rain;

    OnScreenRain();

    int GetNumOnScreen() {
        return this->NumOnScreen;
    }

    void Update(eView *view);
    void GetData(int index, float *x, float *y, float *decay, float *size, int *dripShape);

  private:
    int NumOnScreen; // offset 0x0, size 0x4
#ifndef DISABLE_RAIN
    OnScreenRainPointsDef Points[MAX_OS_RAINPOINTS];
#endif
};

// total size: 0x47C
class Rain {
  public:
    Rain(eView *view, RainType StartType);
    ~Rain();
    void Init(RainType type, float percent);
    void Debug();

    void Wind(float time);
    void Update();
    void Reset();
    void SetRainIntensity(float percent);

    float GetRainIntensity() {
        return this->intensity;
    }

    float GetCloudIntensity() {
        return this->CloudIntensity;
    }

    float GetRoadDampness() {
        return this->RoadDampness;
    }

    void Change(RainType type, float percent);

    void GetPrecipFogColour(uint32 *r, uint32 *g, uint32 *b) {
        *r = this->fogR;
        *g = this->fogG;
        *b = this->fogB;
    }

    void SetPrecipFogColour(uint32 r, uint32 g, uint32 b) {
        this->fogR = r;
        this->fogG = g;
        this->fogB = b;
    }

    void Render();

    float GetAmount(RainType type) {}

    void SetRoadDampness(float damp) {
        this->RoadDampness = damp;
    }

    bVector3 *GetWind() {
        return &this->PrevailingWindSpeed;
    }

    void Init();
    void UpdateAndRender();
    void UpdateAndRenderCurtain();
    void AttachRainCurtain(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
    void FindCurtain();
    void FindCurtains();
    void SeedCurtainXZ(RainPointsDef *rainpoints);
    void SeedCurtainX(RainPointsDef *rainpoints);

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
    void Seed(RainPointsDef *rainpoints, bVector3 *CameraPosition, bVector3 *CameraDirection);

    eView *MyView;        // offset 0x48, size 0x4
    float intensity;      // offset 0x4C, size 0x4
    float CloudIntensity; // offset 0x50, size 0x4
    uint32 DesiredActive; // offset 0x54, size 0x4
    uint32 NewSwapBuffer; // offset 0x58, size 0x4
    uint32 OldSwapBuffer; // offset 0x5C, size 0x4
#ifndef DISABLE_RAIN
    RainPointsDef RainPoints[MAXRAINPOINTS];
    RainPointsDef2 RainPointsInf[MAXRAINPOINTS];
#endif
    int32 NumRainPoints;          // offset 0x60, size 0x4
    uint32 NumOfType[2];          // offset 0x64, size 0x8
    uint32 DesiredNumOfType[2];   // offset 0x6C, size 0x8
    float Percentages[2];         // offset 0x74, size 0x8
    float precipWindEffect[2][2]; // offset 0x7C, size 0x10
    TextureInfo *texture_info[2]; // offset 0x8C, size 0x8
    bVector3 OldCarDirection;     // offset 0x94, size 0x10
    bVector3 precipRadius[2];     // offset 0xA4, size 0x20
    bVector3 precipSpeedRange[2]; // offset 0xC4, size 0x20
    float precipZconstant[2];     // offset 0xE4, size 0x8
    RainWindType windType[2];     // offset 0xEC, size 0x8
    float CameraSpeed;            // offset 0xF4, size 0x4
    bVector3 windSpeed;           // offset 0xF8, size 0x10
    bVector3 DesiredwindSpeed;    // offset 0x108, size 0x10
    float DesiredWindTime;        // offset 0x118, size 0x4
    float windTime;               // offset 0x11C, size 0x4
    uint32 fogR;                  // offset 0x120, size 0x4
    uint32 fogG;                  // offset 0x124, size 0x4
    uint32 fogB;                  // offset 0x128, size 0x4
    bVector2 aabbMax;             // offset 0x12C, size 0x8
    bVector2 aabbMin;             // offset 0x134, size 0x8
    ePoly PRECIPpoly[2];          // offset 0x13C, size 0x128
    bMatrix4 local2world;         // offset 0x264, size 0x40
    bMatrix4 world2localrot;      // offset 0x2A4, size 0x40
    float LenModifier;            // offset 0x2E4, size 0x4
    float DesiredIntensity;       // offset 0x2E8, size 0x4
    float DesiredCloudyness;      // offset 0x2EC, size 0x4
    float DesiredRoadDampness;    // offset 0x2F0, size 0x4
    float RoadDampness;           // offset 0x2F4, size 0x4
    float percentPrecip[2];       // offset 0x2F8, size 0x8
    bVector3 PrevailingWindSpeed; // offset 0x300, size 0x10
    float WeatherTime;            // offset 0x310, size 0x4
    float DesiredWeatherTime;     // offset 0x314, size 0x4
    bVector3 Velocities[2][10];   // offset 0x318, size 0x140
    bVector2 ent0;                // offset 0x458, size 0x8
    bVector2 ent1;                // offset 0x460, size 0x8
    bVector2 ext0;                // offset 0x468, size 0x8
    bVector2 ext1;                // offset 0x470, size 0x8
    uint8 entFLAG;                // offset 0x478, size 0x1
    uint8 extFLAG;                // offset 0x479, size 0x1
#ifndef DISABLE_RAIN
    bVector3 RainCurtainPos[4];
    float CurtainLength;
    RainPointsDef CurtainRainPoints[MAXCURTAINRAINPOINTS];
#endif
};

class FogQuery {
  public:
    float FogFalloff;           // offset 0x0, size 0x4
    float FogFalloffX;          // offset 0x4, size 0x4
    float FogFalloffY;          // offset 0x8, size 0x4
    float DistFogStart;         // offset 0xC, size 0x4
    float DistFogPower;         // offset 0x10, size 0x4
    unsigned int DistFogColour; // offset 0x14, size 0x4

    int CalculateFogInfo(struct View *view, int InFE);
};

int AmIinATunnel(eView *view, int CheckOverPass);
int AmIinATunnelSlow(eView *view, int CheckOverPass);
void SetRainBase();
void CreateWindRotMatrix(eView *view, bMatrix4 *matrix, int x, bMatrix4 *world);

#endif
