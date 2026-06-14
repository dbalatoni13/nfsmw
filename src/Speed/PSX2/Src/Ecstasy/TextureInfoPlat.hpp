#ifndef PSX2_ECSTASY_TEXTURE_INFO_PLAT_H
#define PSX2_ECSTASY_TEXTURE_INFO_PLAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x2C
struct TextureInfoPlatInfoOBJ {
    char dummy[0x2C];
};

struct TextureInfoPlatInfo : public bTNode<TextureInfoPlatInfo> {
    // total size: 0x3C
    struct TextureInfoPlatInfoOBJ ImageInfos; // offset 0x8, size 0x2C
    struct eTextureBucket *pActiveBucket;     // offset 0x34, size 0x4
    unsigned int Format;                      // offset 0x38, size 0x4

    unsigned char SetImage(int width, int height, int mip, int format, void *imageData, void *imagePal, int alphaUsageType, int clamp);
    unsigned char SetImage(struct TextureInfo *texture_info);
};

void eUnSwizzle8bitPalette(unsigned int *palette);
void eSwizzle8bitPalette(unsigned int *palette);

#endif
