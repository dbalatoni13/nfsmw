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

class TextureInfoPlatInterface {
    // total size: 0x4
    TextureInfoPlatInfo *PlatInfo; // offset 0x0, size 0x4

  public:
    void *CreateAnimData();
    void SetAnimData(void *anim_data);
    void ReleaseAnimData(void *anim_data);
    void SetPlatInfo(TextureInfoPlatInfo *info);
    void Init();
    void Close();

    TextureInfoPlatInfo *GetPlatInfo() {
        return this->PlatInfo;
    }
};

#endif
