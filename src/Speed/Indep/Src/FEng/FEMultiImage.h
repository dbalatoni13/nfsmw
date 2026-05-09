#ifndef _FEMULTIIMAGE
#define _FEMULTIIMAGE

#include "Speed/Indep/Src/FEng/FETypes.h"
#include "feimage.h"

// total size: 0x90
class FEMultiImageData : public FEImageData {
  public:
    FEVector2 TopLeftUV[3];     // offset 0x54, size 0x18
    FEVector2 BottomRightUV[3]; // offset 0x6C, size 0x18
    FEVector3 PivotRot;         // offset 0x84, size 0xC
};

// total size: 0x78
class FEMultiImage : public FEImage {
  public:
    inline FEMultiImage() : FEImage() {
        for (int i = 0; i <= 2; i++) {
            hTexture[i] = 0;
            TextureFlags[i] = 1;
        }
    }
    inline FEMultiImage(const FEMultiImage &Object, bool bReference);
    ~FEMultiImage() override;

    inline FEMultiImageData *GetMultiImageData();

    FEObject *Clone(bool bReference) override;

    u32 GetTexture(u32 tex_num);
    u32 GetTextureFlags(u32 tex_num);
    void SetTexture(u32 tex_num, u32 handle);
    void SetTextureFlag(u32 tex_num, u32 flag, bool on);
    void SetUVs(u32 tex_num, FEVector2 top_left, FEVector2 bottom_right);
    void GetUVs(u32 tex_num, FEVector2 &top_left, FEVector2 &bottom_right);

    u32 hTexture[3];     // offset 0x60, size 0xC
    u32 TextureFlags[3]; // offset 0x6C, size 0xC
};

#endif
