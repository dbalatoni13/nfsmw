#ifndef ECSTASY_ECSTASY_DATA_H
#define ECSTASY_ECSTASY_DATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

enum EVIEW_ID {
    NUM_RVM_VIEWS = 1,
    NUM_PLAYER_VIEWS = 3,
    NUM_RENDER_VIEWS = 21,
    NUM_EVIEWS = 22,
    EVIEW_LAST_ENVMAP = 21,
    EVIEW_FIRST_ENVMAP = 16,
    EVIEW_LAST_RVM = 3,
    EVIEW_FIRST_RVM = 3,
    EVIEW_LAST_PLAYER = 3,
    EVIEW_FIRST_PLAYER = 1,
    EVIEW_LAST_RENDER = 21,
    EVIEW_FIRST_RENDER = 1,
    EVIEW_LAST = 21,
    EVIEW_FIRST = 0,
    EVIEW_ENVMAP0D = 21,
    EVIEW_ENVMAP0U = 20,
    EVIEW_ENVMAP0L = 19,
    EVIEW_ENVMAP0B = 18,
    EVIEW_ENVMAP0R = 17,
    EVIEW_ENVMAP0F = 16,
    EVIEW_SHADOWMATTE = 15,
    EVIEW_SHADOWMAP2 = 14,
    EVIEW_SHADOWMAP1 = 13,
    EVIEW_HDR_SCENE = 12,
    EVIEW_QUADRANT_BOTTOM_RIGHT = 11,
    EVIEW_QUADRANT_BOTTOM_LEFT = 10,
    EVIEW_QUADRANT_TOP_RIGHT = 9,
    EVIEW_QUADRANT_TOP_LEFT = 8,
    EVIEW_PLAYER2_SPECULAR = 7,
    EVIEW_PLAYER1_SPECULAR = 6,
    EVIEW_PLAYER2_GLOW = 5,
    EVIEW_PLAYER1_GLOW = 4,
    EVIEW_PLAYER1_RVM = 3,
    EVIEW_PLAYER2 = 2,
    EVIEW_PLAYER1 = 1,
    EVIEW_FLAILER = 0,
    EVIEW_NONE = -1,
};

enum eVisibleState {
    EVISIBLESTATE_FULL = 2,
    EVISIBLESTATE_PARTIAL = 1,
    EVISIBLESTATE_NOT = 0,
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

struct eTextureEntry {
    // total size: 0x8
    unsigned int NameHash;            // offset 0x0, size 0x4
    struct TextureInfo *pTextureInfo; // offset 0x4, size 0x4

    void EndianSwap() {}
};

struct eSolidListHeader : public bTNode<eSolidListHeader> {
    // total size: 0x90
    int Version;                                   // offset 0x8, size 0x4
    int NumSolids;                                 // offset 0xC, size 0x4
    char Filename[56];                             // offset 0x10, size 0x38
    char GroupName[32];                            // offset 0x48, size 0x20
    unsigned int PermChunkByteOffset;              // offset 0x68, size 0x4
    unsigned int PermChunkByteSize;                // offset 0x6C, size 0x4
    short MaxSolidChunkByteAlignment;              // offset 0x70, size 0x2
    short EndianSwapped;                           // offset 0x72, size 0x2
    struct eSolidIndexEntry *SolidIndexEntryTable; // offset 0x74, size 0x4
    struct eStreamingEntry *SolidStreamEntryTable; // offset 0x78, size 0x4
    short NumTexturePacks;                         // offset 0x7C, size 0x2
    short NumDefaultTextures;                      // offset 0x7E, size 0x2
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

struct eSolidIndexEntry {
    // total size: 0x8
    unsigned int NameHash; // offset 0x0, size 0x4
    struct eSolid *Solid;  // offset 0x4, size 0x4

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
    }
};

struct eLightMaterialEntry {
    // total size: 0x8
    unsigned int NameHash;                // offset 0x0, size 0x4
    struct eLightMaterial *LightMaterial; // offset 0x4, size 0x4

    void EndianSwap() {}
};

struct eSmoothVertex {
    // total size: 0x8
    unsigned int VertexHash;            // offset 0x0, size 0x4
    unsigned char SmoothingGroupNumber; // offset 0x4, size 0x1
    char NX;                            // offset 0x5, size 0x1
    char NY;                            // offset 0x6, size 0x1
    char NZ;                            // offset 0x7, size 0x1

    void EndianSwap() {}
};

struct eSmoothVertexPlat {
    // total size: 0xC
    unsigned int VertexHash;     // offset 0x0, size 0x4
    unsigned int SmoothingGroup; // offset 0x4, size 0x4
    unsigned int VertexOffset;   // offset 0x8, size 0x4

    void EndianSwap() {}
};

struct eNormalSmoother {
    // total size: 0xC
    eSmoothVertex *SmoothVertexTable;         // offset 0x0, size 0x4
    eSmoothVertexPlat *SmoothVertexPlatTable; // offset 0x4, size 0x4
    short NumSmoothVertex;                    // offset 0x8, size 0x2
    short NumSmoothVertexPlat;                // offset 0xA, size 0x2

    void EndianSwap() {}
};

struct ePositionMarker {
    // total size: 0x50
    unsigned int NameHash; // offset 0x0, size 0x4
    int iParam0;           // offset 0x4, size 0x4
    float fParam0;         // offset 0x8, size 0x4
    float fParam1;         // offset 0xC, size 0x4
    bMatrix4 Matrix;       // offset 0x10, size 0x40

    void EndianSwap() {}
};

#endif
