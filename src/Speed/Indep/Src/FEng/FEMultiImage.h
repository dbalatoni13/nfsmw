#ifndef __FEMULTIIMAGE_H__
#define __FEMULTIIMAGE_H__

#include "FEImage.h"
#include "Speed/Indep/Src/FEng/FEObject.h"

enum eFEMultiImageUsage {
    FEMI_MASK = 1,
    FEMI_TILE_U = 2,
    FEMI_TILE_V = 4,
};

enum eFEMultiImageTexNum {
    FEMI_TEXTURE_1 = 0,
    FEMI_TEXTURE_2 = 1,
    FEMI_TEXTURE_3 = 2,
    FEMI_NUM_TEXTURES = 3,
};

// total size: 0x90
// Decl: 42
class FEMultiImageData : public FEImageData {
  public:
    FEVector2 TopLeftUV[3];     // offset 0x54, size 0x18, Decl: 44
    FEVector2 BottomRightUV[3]; // offset 0x6C, size 0x18, Decl: 45
    FEVector3 PivotRot;         // offset 0x84, size 0xC, Decl: 46
};

// total size: 0x78
// Decl: 52
class FEMultiImage : public FEImage {
  public:
    u32 hTexture[3];     // offset 0x60, size 0xC, Decl: 54
    u32 TextureFlags[3]; // offset 0x6C, size 0xC, Decl: 55

    FEMultiImage() : FEImage() { // Decl: 62
        Type = FE_MultiImage;
        for (int i = 0; i <= 2; i++) {
            hTexture[i] = 0;
            TextureFlags[i] = 1;
        }
    }

    FEMultiImage(const FEMultiImage &Object, bool bReference) : FEImage(Object, bReference) {}

    ~FEMultiImage() override {} // Decl: 77

    FEMultiImageData *GetMultiImageData() { // Decl: 79
        return reinterpret_cast<FEMultiImageData *>(GetObjData());
    }

    FEObject *Clone(bool bReference) override { // Decl: 81
        return FNEW FEMultiImage(*this, bReference);
    }

    u32 GetTexture(u32 tex_num);

    u32 GetTextureFlags(u32 tex_num);

    void SetTexture(u32 tex_num, u32 handle);

    void SetTextureFlag(u32 tex_num, u32 flag, bool on);

    void SetUVs(u32 tex_num, FEVector2 top_left, FEVector2 bottom_right);

    void GetUVs(u32 tex_num, FEVector2 &top_left, FEVector2 &bottom_right);
};

#endif
