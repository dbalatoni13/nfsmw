#ifndef WORLD_SUN_H
#define WORLD_SUN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#define BCHUNK_SUN_INFOS 0x00034202

enum SUN_TEXTURE {
    NUM_SUN_TEXTURES = 5,
    SUNTEX_RING = 4,
    SUNTEX_MINORRAYS = 3,
    SUNTEX_MAJORRAYS = 2,
    SUNTEX_HALO = 1,
    SUNTEX_CENTER = 0,
};

enum SUN_ALPHA {
    SUNALPHA_ADD = 1,
    SUNALPHA_BLEND = 0,
};

struct SunLayer {
    // total size: 0x24
    SUN_TEXTURE Texture;     // offset 0x0, size 0x4
    SUN_ALPHA AlphaType;     // offset 0x4, size 0x4
    float IntensityScale;    // offset 0x8, size 0x4
    float Size;              // offset 0xC, size 0x4
    float OffsetX;           // offset 0x10, size 0x4
    float OffsetY;           // offset 0x14, size 0x4
    unsigned char Colour[4]; // offset 0x18, size 0x4
    int Angle;               // offset 0x1C, size 0x4
    float SweepAngleAmount;  // offset 0x20, size 0x4
};

struct SunChunkInfo {
    // total size: 0x110
    int Version;              // offset 0x0, size 0x4
    unsigned int NameHash;    // offset 0x4, size 0x4
    char Name[24];            // offset 0x8, size 0x18
    float PositionX;          // offset 0x20, size 0x4
    float PositionY;          // offset 0x24, size 0x4
    float PositionZ;          // offset 0x28, size 0x4
    float CarShadowPositionX; // offset 0x2C, size 0x4
    float CarShadowPositionY; // offset 0x30, size 0x4
    float CarShadowPositionZ; // offset 0x34, size 0x4
    SunLayer SunLayers[6];    // offset 0x38, size 0xD8
};

void RenderSunAsFlare();

extern SunChunkInfo *SunInfo;

#endif
