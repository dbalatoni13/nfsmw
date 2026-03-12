#ifndef ECSTASY_ECSTASY_DATA_H
#define ECSTASY_ECSTASY_DATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct TextureInfo;
struct eLightContext;
struct eModel;
struct ePoly;

enum EVIEW_ID {
    EVIEW_NONE = -1,
    EVIEW_FLAILER = 0,
    EVIEW_PLAYER1 = 1,
    EVIEW_PLAYER2 = 2,
    EVIEW_PLAYER1_RVM = 3,
    EVIEW_PLAYER1_GLOW = 4,
    EVIEW_PLAYER2_GLOW = 5,
    EVIEW_PLAYER1_SPECULAR = 6,
    EVIEW_PLAYER2_SPECULAR = 7,
    EVIEW_QUADRANT_TOP_LEFT = 8,
    EVIEW_QUADRANT_TOP_RIGHT = 9,
    EVIEW_QUADRANT_BOTTOM_LEFT = 10,
    EVIEW_QUADRANT_BOTTOM_RIGHT = 11,
    EVIEW_HDR_SCENE = 12,
    EVIEW_SHADOWMAP1 = 13,
    EVIEW_SHADOWMAP2 = 14,
    EVIEW_SHADOWMATTE = 15,
    EVIEW_ENVMAP0F = 16,
    EVIEW_ENVMAP0R = 17,
    EVIEW_ENVMAP0B = 18,
    EVIEW_ENVMAP0L = 19,
    EVIEW_ENVMAP0U = 20,
    EVIEW_ENVMAP0D = 21,
    EVIEW_FIRST = 0,
    EVIEW_LAST = 21,
    EVIEW_FIRST_RENDER = 1,
    EVIEW_LAST_RENDER = 21,
    EVIEW_FIRST_PLAYER = 1,
    EVIEW_LAST_PLAYER = 3,
    EVIEW_FIRST_RVM = 3,
    EVIEW_LAST_RVM = 3,
    EVIEW_FIRST_ENVMAP = 16,
    EVIEW_LAST_ENVMAP = 21,
    NUM_EVIEWS = 22,
    NUM_RENDER_VIEWS = 21,
    NUM_PLAYER_VIEWS = 3,
    NUM_RVM_VIEWS = 1,
};

enum eVisibleState {
    EVISIBLESTATE_NOT = 0,
    EVISIBLESTATE_PARTIAL = 1,
    EVISIBLESTATE_FULL = 2,
};

enum TARGET_ID {
    NUM_RENDER_TARGETS = 17,
    LAST_RENDER_TARGET = 16,
    FIRST_RENDER_TARGET = 0,
    TOTAL_RENDER_TARGETS = 17,
    TARGET_ENVMAP0_FULL = 16,
    TARGET_ENVMAP0D = 15,
    TARGET_ENVMAP0U = 14,
    TARGET_ENVMAP0L = 13,
    TARGET_ENVMAP0B = 12,
    TARGET_ENVMAP0R = 11,
    TARGET_ENVMAP0F = 10,
    TARGET_QUADRANT_BOTTOM_RIGHT = 9,
    TARGET_QUADRANT_BOTTOM_LEFT = 8,
    TARGET_QUADRANT_TOP_RIGHT = 7,
    TARGET_QUADRANT_TOP_LEFT = 6,
    TARGET_PLAYER2_SPECULAR = 5,
    TARGET_PLAYER1_SPECULAR = 4,
    TARGET_PLAYER1_RVM = 3,
    TARGET_PLAYER2 = 2,
    TARGET_PLAYER1 = 1,
    TARGET_FLAYER = 0,
};

enum FILTER_ID {
    FILTER_TOTAL = 11,
    FILTER_PIXELATE = 10,
    FILTER_EFB_XFB_AA = 9,
    FILTER_CONTRAST_INTENSITY = 8,
    FILTER_GLOWBLOOM = 7,
    FILTER_MOTIONBLUR = 6,
    FILTER_REFLECTION = 5,
    FILTER_SPHERE_MAP = 4,
    FILTER_CUBE_FACES = 3,
    FILTER_EFB_XFB = 2,
    FILTER_DEFAULT = 1,
    FILTER_OFF = 0,
};

enum EVIEWMODE {
    EVIEWMODE_TWOV = 4,
    EVIEWMODE_TWOH = 3,
    EVIEWMODE_ONE_RVM = 2,
    EVIEWMODE_ONE = 1,
    EVIEWMODE_NONE = 0,
};

class eTextureEntry {
    // total size: 0x8
  public:
    uint32 NameHash;                  // offset 0x0, size 0x4
    struct TextureInfo *pTextureInfo; // offset 0x4, size 0x4

    void EndianSwap() {}
};

class eSolidListHeader : public bTNode<eSolidListHeader> {
    // total size: 0x90
  public:
    int32 Version;                                 // offset 0x8, size 0x4
    int32 NumSolids;                               // offset 0xC, size 0x4
    char Filename[56];                             // offset 0x10, size 0x38
    char GroupName[32];                            // offset 0x48, size 0x20
    uint32 PermChunkByteOffset;                    // offset 0x68, size 0x4
    uint32 PermChunkByteSize;                      // offset 0x6C, size 0x4
    int16 MaxSolidChunkByteAlignment;              // offset 0x70, size 0x2
    int16 EndianSwapped;                           // offset 0x72, size 0x2
    struct eSolidIndexEntry *SolidIndexEntryTable; // offset 0x74, size 0x4
    struct eStreamingEntry *SolidStreamEntryTable; // offset 0x78, size 0x4
    int16 NumTexturePacks;                         // offset 0x7C, size 0x2
    int16 NumDefaultTextures;                      // offset 0x7E, size 0x2
    bPList<struct TexturePack> TexturePackList;    // offset 0x80, size 0x8
    bPList<eTextureEntry> DefaultTextureList;      // offset 0x88, size 0x8

    void EndianSwap() {
        bPlatEndianSwap(&this->Version);
        bPlatEndianSwap(&this->NumSolids);
        bPlatEndianSwap(&this->PermChunkByteOffset);
        bPlatEndianSwap(&this->PermChunkByteSize);
        bPlatEndianSwap(&this->MaxSolidChunkByteAlignment);
    }
};

class eSolidIndexEntry {
    // total size: 0x8
  public:
    uint32 NameHash;      // offset 0x0, size 0x4
    struct eSolid *Solid; // offset 0x4, size 0x4

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
    }
};

class eLightMaterialEntry {
    // total size: 0x8
  public:
    uint32 NameHash;                      // offset 0x0, size 0x4
    struct eLightMaterial *LightMaterial; // offset 0x4, size 0x4

    void EndianSwap() {}
};

class eSmoothVertex {
    // total size: 0x8
  public:
    uint32 VertexHash;          // offset 0x0, size 0x4
    uint8 SmoothingGroupNumber; // offset 0x4, size 0x1
    int8 NX;                    // offset 0x5, size 0x1
    int8 NY;                    // offset 0x6, size 0x1
    int8 NZ;                    // offset 0x7, size 0x1

    void EndianSwap() {}
};

class eSmoothVertexPlat {
    // total size: 0xC
  public:
    uint32 VertexHash;     // offset 0x0, size 0x4
    uint32 SmoothingGroup; // offset 0x4, size 0x4
    uint32 VertexOffset;   // offset 0x8, size 0x4

    void EndianSwap() {}
};

class eNormalSmoother {
    // total size: 0xC
  public:
    eSmoothVertex *SmoothVertexTable;         // offset 0x0, size 0x4
    eSmoothVertexPlat *SmoothVertexPlatTable; // offset 0x4, size 0x4
    int16 NumSmoothVertex;                    // offset 0x8, size 0x2
    int16 NumSmoothVertexPlat;                // offset 0xA, size 0x2

    void EndianSwap() {}
};

class ePositionMarker {
    // total size: 0x50
  public:
    uint32 NameHash; // offset 0x0, size 0x4
    int32 iParam0;   // offset 0x4, size 0x4
    float fParam0;   // offset 0x8, size 0x4
    float fParam1;   // offset 0xC, size 0x4
    bMatrix4 Matrix; // offset 0x10, size 0x40

    void EndianSwap() {}
};

class eModel;
class ePoly;
class eLightContext;

class eViewPlatInfo;

// total size: 0x4
class eViewPlatInterface {
    eViewPlatInfo *PlatInfo; // offset 0x0, size 0x4

  public:
    eViewPlatInfo *GetPlatInfo() {
        return PlatInfo;
    }
    void SetPlatInfo(eViewPlatInfo *info) {
        PlatInfo = info;
    }

    static eViewPlatInfo *GimmeMyViewPlatInfo(int view_id);
    eVisibleState GetVisibleStateGB(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world);
    eVisibleState GetVisibleStateSB(const bVector3 *aabb_min, const bVector3 *aabb_max, bMatrix4 *local_world);

    void Render(eModel *model, bMatrix4 *local_to_world, eLightContext *light_context, uint32 flags, bMatrix4 *blending_matricies);
    void FERender(ePoly *poly, TextureInfo *texture_info, bMatrix4 *local_to_world, int use_previous_data, float bbRad);
    void Render(ePoly *poly, TextureInfo *texture_info, bMatrix4 *local_to_world, int use_previous_data, float bbRad);
    void FERender(ePoly *poly, TextureInfo *texture_info, TextureInfo *texture_info_mask, int use_previous_data);
    void Render(ePoly *poly, TextureInfo *texture_info, TextureInfo *texture_info_mask, int use_previous_data);
    void FERender(ePoly *poly, TextureInfo *texture_info, int use_previous_data);
    void Render(ePoly *poly, TextureInfo *texture_info, int use_previous_data);
    void Render(struct ePointSprite3D *sprite, TextureInfo *texture_info, bMatrix4 *local_world, int num_sprites);
};

class eLoadedSolidStats {
    // total size: 0x14
  public:
    uint32 NumLoadedLists;           // offset 0x0, size 0x4
    uint32 NumLoadedSolids;          // offset 0x4, size 0x4
    uint32 TotalSolidsByteSize;      // offset 0x8, size 0x4
    uint32 TotalNormalSmootherBytes; // offset 0xC, size 0x4
    uint32 TotalDamageBytes;         // offset 0x10, size 0x4
};

class eSolidPlatInfo {
    // total size: 0x24
  public:
    uint16 Version;    // offset 0x0, size 0x2
    uint16 StripFlags; // offset 0x2, size 0x2
    uint16 NumStrips;  // offset 0x4, size 0x2
#ifndef EA_BUILD_A124
    uint16 NumIdxClrTable; // offset 0x6, size 0x2
#else
    uint16 pad0;
#endif
    uint32 SizeofStripData; // offset 0x8, size 0x4
#ifndef EA_BUILD_A124
    uint32 DataOffset0; // offset 0xC, size 0x4
    uint32 DataOffset1; // offset 0x10, size 0x4
    uint32 DataOffset2; // offset 0x14, size 0x4
    uint32 DataOffset3; // offset 0x18, size 0x4
#endif
    struct eStripEntry *StripEntryTable; // offset 0x1C, size 0x4
    uint8 *StripDataStart;               // offset 0x20, size 0x4
};

#endif
