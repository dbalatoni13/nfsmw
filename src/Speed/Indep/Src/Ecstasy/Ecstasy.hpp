#ifndef ECSTASY_ECSTASY_H
#define ECSTASY_ECSTASY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#ifdef TARGET_GC
#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#elif defined(TARGET_X360)
#include "Speed/Xenon/Src/Ecstasy/eViewPlat.hpp"
#endif

#include "EcstasyData.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "eModel.hpp"

extern eView eViews[22];

struct eView : public eViewPlatInterface {
    // total size: 0x68
    EVIEW_ID ID;                                    // offset 0x4, size 0x4
    char Active;                                    // offset 0x8, size 0x1
    char LetterBox;                                 // offset 0x9, size 0x1
    char pad0;                                      // offset 0xA, size 0x1
    char pad1;                                      // offset 0xB, size 0x1
    float H;                                        // offset 0xC, size 0x4
    float NearZ;                                    // offset 0x10, size 0x4
    float FarZ;                                     // offset 0x14, size 0x4
    float FovBias;                                  // offset 0x18, size 0x4
    float FovDegrees;                               // offset 0x1C, size 0x4
    int BlackAndWhiteMode;                          // offset 0x20, size 0x4
    int PixelMinSize;                               // offset 0x24, size 0x4
    bVector3 ViewDirection;                         // offset 0x28, size 0x10
    Camera *pCamera;                                // offset 0x38, size 0x4
    bTList<CameraMover> CameraMoverList;            // offset 0x3C, size 0x8
    unsigned int NumCopsInView;                     // offset 0x44, size 0x4
    unsigned int NumCopsTotal;                      // offset 0x48, size 0x4
    unsigned int NumCopsCherry;                     // offset 0x4C, size 0x4
    TextureInfo *pBlendMask;                        // offset 0x50, size 0x4
    struct eDynamicLightContext *WorldLightContext; // offset 0x54, size 0x4
    eRenderTarget *RenderTargetTable[1];            // offset 0x58, size 0x4
    struct ScreenEffectDB *ScreenEffects;           // offset 0x5C, size 0x4
    struct Rain *Precipitation;                     // offset 0x60, size 0x4
    struct FacePixelation *facePixelation;          // offset 0x64, size 0x4

    eView();
    ~eView();

    eVisibleState GetVisibleState(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world);
    eVisibleState GetVisibleState(eModel *model, bMatrix4 *local_world);
    int GetPixelSize(float radius, float distance);
    int GetPixelSize(const bVector3 *position, float radius);
    int GetPixelSize(const bVector3 *bbox_min, const bVector3 *bbox_max);
    void BiasMatrixForZSorting(bMatrix4 *pL2W, float zBias);
    void AttachCameraMover(CameraMover *camera_mover);
    void UnattachCameraMover(CameraMover *camera_mover);

    int GetID() {
        return this->ID;
    }

    void SetID(int id) {
        ID = static_cast<EVIEW_ID>(id);
    }

    void SetRenderTarget(eRenderTarget *target, int index) {
        this->RenderTargetTable[index] = target;
    }

    void SetRenderTarget0(eRenderTarget *target) {
        this->SetRenderTarget(target, 0);
    }

    Camera *GetCamera() {
        return this->pCamera;
    }

    void SetCamera(Camera *camera) {
        this->pCamera = camera;
    }

    void SetActive(int state) {
        int prev_state = this->Active;
        this->Active = state;
    }

    CameraMover *GetCameraMover() {
        if (!this->CameraMoverList.IsEmpty()) {
            return this->CameraMoverList.GetHead();
        }
        return nullptr;
    }
};

eView *eGetView(int view_id);

inline eView *eGetView(int view_id, bool doAssert) {
    // if (doAssert) {
    //     // ?
    // }
    return &eViews[view_id];
}

inline void eSwizzleWorldVector(const bVector3 &inVec, bVector3 &outVec) {
    bConvertFromBond(outVec, inVec);
}

inline void eUnSwizzleWorldVector(const bVector3 &inVec, bVector3 &outVec) {
    bConvertToBond(outVec, inVec);
}

eRenderTarget *eGetRenderTarget(int render_target);
void eUpdateViewMode(void);

enum ScreenEffectType {
    SE_NUM_TYPES = 5,
    SE_FE_BLUR = 4,
    SE_GLARE = 3,
    SE_VISUAL_SIG = 2,
    SE_MOTION_BLUR = 1,
    SE_TINT = 0,
};

enum ScreenEffectControl {
    SEC_FUNCTION = 2,
    SEC_BOOLEAN = 1,
    SEC_FRAME = 0,
};

struct ScreenEffectInf {
    // total size: 0xC
    ScreenEffectControl Controller; // offset 0x0, size 0x4
    unsigned int frameNum;          // offset 0x4, size 0x4
    unsigned int active;            // offset 0x8, size 0x4
};
struct ScreenEffectDef {
    // total size: 0x50
    float r;         // offset 0x0, size 0x4
    float g;         // offset 0x4, size 0x4
    float b;         // offset 0x8, size 0x4
    float a;         // offset 0xC, size 0x4
    float intensity; // offset 0x10, size 0x4
    float data[14];  // offset 0x14, size 0x38
    void (*UpdateFnc)(ScreenEffectType,
                      struct ScreenEffectDB *); // offset 0x4C, size 0x4
};

struct ScreenEffectDB {
    // total size: 0x1E8
    eView *MyView;              // offset 0x0, size 0x4
    ScreenEffectInf SE_inf[5];  // offset 0x4, size 0x3C
    ScreenEffectDef SE_data[5]; // offset 0x40, size 0x190
    unsigned int numType[5];    // offset 0x1D0, size 0x14
    float SE_time;              // offset 0x1E4, size 0x4

    ScreenEffectDB();

    void SetMyView(eView *view) {
        MyView = view;
    }
};

struct ePoly {
    // total size: 0x94
    bVector3 Vertices[4];        // offset 0x0, size 0x40
    float UVs[2][4];             // offset 0x40, size 0x20
    float UVsMask[2][4];         // offset 0x60, size 0x20
    unsigned char Colours[4][4]; // offset 0x80, size 0x10
    unsigned char flags;         // offset 0x90, size 0x1
    unsigned char Flailer;       // offset 0x91, size 0x1

    void *operator new(size_t size) {}

    void operator delete(void *ptr) {}

    void SetFlags(unsigned char i) {}

    void SetFlailer(unsigned char i) {}

    unsigned char GetFlags() {}

    unsigned char GetFlailer() {}
};

struct OnScreenRain {
    // total size: 0x4
    int NumOnScreen; // offset 0x0, size 0x4

    int GetNumOnScreen() {
        return NumOnScreen;
    }
};

enum CurtainStatus {
    CT_OVERIDE = 3,
    CT_TURNON = 2,
    CT_ACTIVE = 1,
    CT_INACTIVE = 0,
};

enum RainType {
    NUMTYPES = 2,
    INACTIVE = 1,
    RAIN = 0,
};

enum RainWindType {
    NUMWINDTYPES = 2,
    VECTOR_WIND = 1,
    POINT_WIND = 0,
};

struct Rain {
    // total size: 0x47C
    OnScreenRain OSrain;                 // offset 0x0, size 0x4
    BOOL NoRain;                         // offset 0x4, size 0x4
    BOOL NoRainAhead;                    // offset 0x8, size 0x4
    BOOL inTunnel;                       // offset 0xC, size 0x4
    BOOL inOverpass;                     // offset 0x10, size 0x4
    void *the_zone;                      // offset 0x14, size 0x4
    bVector3 outvex;                     // offset 0x18, size 0x10
    bVector2 twoDpos;                    // offset 0x28, size 0x8
    bVector3 CamVelLOCAL;                // offset 0x30, size 0x10
    CurtainStatus IsValidRainCurtainPos; // offset 0x40, size 0x4
    int renderCount;                     // offset 0x44, size 0x4
    eView *MyView;                       // offset 0x48, size 0x4
    float intensity;                     // offset 0x4C, size 0x4
    float CloudIntensity;                // offset 0x50, size 0x4
    unsigned int DesiredActive;          // offset 0x54, size 0x4
    unsigned int NewSwapBuffer;          // offset 0x58, size 0x4
    unsigned int OldSwapBuffer;          // offset 0x5C, size 0x4
    int NumRainPoints;                   // offset 0x60, size 0x4
    unsigned int NumOfType[2];           // offset 0x64, size 0x8
    unsigned int DesiredNumOfType[2];    // offset 0x6C, size 0x8
    float Percentages[2];                // offset 0x74, size 0x8
    float precipWindEffect[2][2];        // offset 0x7C, size 0x10
    TextureInfo *texture_info[2];        // offset 0x8C, size 0x8
    bVector3 OldCarDirection;            // offset 0x94, size 0x10
    bVector3 precipRadius[2];            // offset 0xA4, size 0x20
    bVector3 precipSpeedRange[2];        // offset 0xC4, size 0x20
    float precipZconstant[2];            // offset 0xE4, size 0x8
    RainWindType windType[2];            // offset 0xEC, size 0x8
    float CameraSpeed;                   // offset 0xF4, size 0x4
    bVector3 windSpeed;                  // offset 0xF8, size 0x10
    bVector3 DesiredwindSpeed;           // offset 0x108, size 0x10
    float DesiredWindTime;               // offset 0x118, size 0x4
    float windTime;                      // offset 0x11C, size 0x4
    unsigned int fogR;                   // offset 0x120, size 0x4
    unsigned int fogG;                   // offset 0x124, size 0x4
    unsigned int fogB;                   // offset 0x128, size 0x4
    bVector2 aabbMax;                    // offset 0x12C, size 0x8
    bVector2 aabbMin;                    // offset 0x134, size 0x8
    ePoly PRECIPpoly[2];                 // offset 0x13C, size 0x128
    bMatrix4 local2world;                // offset 0x264, size 0x40
    bMatrix4 world2localrot;             // offset 0x2A4, size 0x40
    float LenModifier;                   // offset 0x2E4, size 0x4
    float DesiredIntensity;              // offset 0x2E8, size 0x4
    float DesiredCloudyness;             // offset 0x2EC, size 0x4
    float DesiredRoadDampness;           // offset 0x2F0, size 0x4
    float RoadDampness;                  // offset 0x2F4, size 0x4
    float percentPrecip[2];              // offset 0x2F8, size 0x8
    bVector3 PrevailingWindSpeed;        // offset 0x300, size 0x10
    float WeatherTime;                   // offset 0x310, size 0x4
    float DesiredWeatherTime;            // offset 0x314, size 0x4
    bVector3 Velocities[10][2];          // offset 0x318, size 0x140
    bVector2 ent0;                       // offset 0x458, size 0x8
    bVector2 ent1;                       // offset 0x460, size 0x8
    bVector2 ext0;                       // offset 0x468, size 0x8
    bVector2 ext1;                       // offset 0x470, size 0x8
    unsigned char entFLAG;               // offset 0x478, size 0x1
    unsigned char extFLAG;               // offset 0x479, size 0x1

    Rain(eView *view, RainType StartType);
    void Init(RainType type, float percent);

    float GetRainIntensity() {}

    float GetCloudIntensity() {
        return this->CloudIntensity;
    }

    float GetRoadDampness() {}

    void GetPrecipFogColour(unsigned int *r, unsigned int *g, unsigned int *b) {}

    void SetPrecipFogColour(unsigned int r, unsigned int g, unsigned int b) {}

    float GetAmount(RainType type) {}

    void SetRoadDampness(float damp) {}

    bVector3 *GetWind() {}
};

struct FacePixelation {
    // total size: 0xC
    struct eView *MyView; // offset 0x0, size 0x4
    float mScreenX;       // offset 0x4, size 0x4
    float mScreenY;       // offset 0x8, size 0x4

    FacePixelation(eView *view);
    ~FacePixelation();
    static void SetLocation(bVector3 &worldPos);
    void GetData(float *x, float *y, float *width, float *height);
    void Render();

    bool IsEnabled() {}

    void Update(float x, float y) {}

    static void SetDimensions(float width, float height) {}

    static void Enable() {}

    static void Disable() {}
};

struct LoadedTable {
    // total size: 0x2004
    int NumLoaded;              // offset 0x0, size 0x4
    unsigned char Counts[8192]; // offset 0x4, size 0x2000

    LoadedTable() {}

    int IsLoaded(unsigned int hash) {
        return *this->GetPtr(hash) == 0;
    }

    void SetLoaded(unsigned int hash) {
        this->NumLoaded++;
        unsigned char *p = this->GetPtr(hash);
        *p = *p + 1;
    }

    void SetUnloaded(unsigned int hash) {
        this->NumLoaded--;
        unsigned char *p = this->GetPtr(hash);
        *p = *p - 1;
    }

    unsigned char *GetPtr(unsigned int hash) {
        return &this->Counts[hash & 0x1FFF];
    }
};

struct eSolidPlatInfo {
    // total size: 0x24
    unsigned short Version;              // offset 0x0, size 0x2
    unsigned short StripFlags;           // offset 0x2, size 0x2
    unsigned short NumStrips;            // offset 0x4, size 0x2
    unsigned short NumIdxClrTable;       // offset 0x6, size 0x2
    unsigned int SizeofStripData;        // offset 0x8, size 0x4
    unsigned int DataOffset0;            // offset 0xC, size 0x4
    unsigned int DataOffset1;            // offset 0x10, size 0x4
    unsigned int DataOffset2;            // offset 0x14, size 0x4
    unsigned int DataOffset3;            // offset 0x18, size 0x4
    struct eStripEntry *StripEntryTable; // offset 0x1C, size 0x4
    unsigned char *StripDataStart;       // offset 0x20, size 0x4
};

class eSolidPlatInterface {
    // total size: 0x4
    eSolidPlatInfo *PlatInfo; // offset 0x0, size 0x4

  public:
    int UnloaderPlatChunks(bChunk *chunk);
    int FixPlatInfo();
    int UnFixPlatInfo();
    void SetSmoothVertex(unsigned int vertex_offset, float nx, float ny, float nz);

    eSolidPlatInfo *GetPlatInfo() {
        return this->PlatInfo;
    }
};

struct eReplacementTextureTable {
    // total size: 0xC
    unsigned int hOldNameHash; // offset 0x0, size 0x4
    unsigned int hNewNameHash; // offset 0x4, size 0x4
    TextureInfo *pTextureInfo; // offset 0x8, size 0x4

    eReplacementTextureTable() {}

    void InvalidateTexture() {
        this->pTextureInfo = reinterpret_cast<TextureInfo *>(-1);
    }

    unsigned int GetNewNameHash() {}

    unsigned int GetOldNameHash() {}

    void SetOldNameHash(unsigned int name_hash) {}

    void SetNewNameHash(unsigned int name_hash) {}

    void SetExplicit(unsigned int name_hash, TextureInfo *pRepTextureInfo) {}

    TextureInfo *GetCurrentTexture() {
        return pTextureInfo;
    }

    void SetCurrentTexture(TextureInfo *texture_info) {
        this->pTextureInfo = texture_info;
    }
};

void eFixupReplacementTexturesAfterUnloading(TextureInfo *texture_info);
void eNotifyTextureLoading(TexturePack *texture_pack, TextureInfo *texture_info, bool loading);
TextureInfo *eGetRenderTargetTextureInfo(int name_hash);
TextureInfo *eGetOtherEcstacyTexture(unsigned int name_hash);
int eLoadSolidListPlatChunks(bChunk *chunk);
float GetSunIntensity(eView *view);

#endif
