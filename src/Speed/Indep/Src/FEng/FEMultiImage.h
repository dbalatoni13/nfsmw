#ifndef _FEMULTIIMAGE
#define _FEMULTIIMAGE

#include "feimage.h"

struct FEMultiImageData;

// total size: 0x78
struct FEMultiImage : public FEImage {
    unsigned long hTexture[3];     // offset 0x60, size 0xC
    unsigned long TextureFlags[3]; // offset 0x6C, size 0xC

    inline FEMultiImage() : FEImage() {
        for (int i = 0; i <= 2; i++) {
            hTexture[i] = 0;
            TextureFlags[i] = 1;
        }
    }
    inline FEMultiImage(const FEMultiImage& Object, bool bReference);
    ~FEMultiImage() override;

    inline FEMultiImageData* GetMultiImageData();

    FEObject* Clone(bool bReference) override;

    unsigned long GetTexture(unsigned long tex_num);
    void SetTexture(unsigned long tex_num, unsigned long handle);
    unsigned long GetTextureFlags(unsigned long tex_num);
    void SetTextureFlag(unsigned long tex_num, unsigned long flag, bool on);
    void SetUVs(unsigned long tex_num, FEVector2 top_left, FEVector2 bottom_right);
    void GetUVs(unsigned long tex_num, FEVector2& top_left, FEVector2& bottom_right);
};

#endif
