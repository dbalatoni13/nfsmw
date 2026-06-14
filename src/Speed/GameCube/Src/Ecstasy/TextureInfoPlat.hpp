#ifndef TEXTUREINFOPLAT_HPP
#define TEXTUREINFOPLAT_HPP

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include <dolphin.h>

class TextureInfo;
struct eTextureBucket; // TODO this exists only on GameCube, where is it defined?

// total size: 0x2C
struct TextureInfoPlatInfoOBJ {
    GXTlutObj objClut; // offset 0x0, size 0xC
    GXTexObj obj;      // offset 0xC, size 0x20
};

// total size: 0x3C
class TextureInfoPlatInfo : public bTNode<TextureInfoPlatInfo> {
  public:
    unsigned char SetImage(int width, int height, int mip, int format, void *imageData, void *imagePal, int alphaUsageType, int clamp);
    unsigned char SetImage(TextureInfo *texture_info);

    TextureInfoPlatInfoOBJ ImageInfos; // offset 0x8, size 0x2C
    eTextureBucket *pActiveBucket;     // offset 0x34, size 0x4
    unsigned int Format;               // offset 0x38, size 0x4
};

void eUnSwizzle8bitPalette(unsigned int *palette);
void eSwizzle8bitPalette(unsigned int *palette);

#endif
