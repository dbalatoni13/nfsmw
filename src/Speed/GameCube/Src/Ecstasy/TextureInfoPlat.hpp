#pragma once

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include <dolphin.h>

struct TextureInfoPlatInfoOBJ {
  // total size: 0x2C
  GXTlutObj objClut; // offset 0x0, size 0xC
  GXTexObj obj;      // offset 0xC, size 0x20
};

struct TextureInfoPlatInfo : public bTNode<TextureInfoPlatInfo> {
  // total size: 0x3C
  struct TextureInfoPlatInfoOBJ ImageInfos; // offset 0x8, size 0x2C
  struct eTextureBucket *pActiveBucket;     // offset 0x34, size 0x4
  unsigned int Format;                      // offset 0x38, size 0x4
};

class TextureInfoPlatInterface {
  // total size: 0x4
  TextureInfoPlatInfo *PlatInfo; // offset 0x0, size 0x4

public:
  void *CreateAnimData();
  void ReleaseAnimData(void *anim_data);
  void SetPlatInfo(TextureInfoPlatInfo *info);

  TextureInfoPlatInfo *GetPlatInfo() {
    return this->PlatInfo;
  }
};
