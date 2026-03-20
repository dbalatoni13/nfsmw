#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern SlotPool *eAnimTextureSlotPool;
extern TextureInfo *pTexPrev;

static inline unsigned int Convert16To32(unsigned short entry) {
    if (entry & 0x8000) {
        unsigned int r = (entry >> 10) & 0x1F;
        unsigned int g = (entry >> 5) & 0x1F;
        unsigned int b = entry & 0x1F;
        return 0xFF000000 | (r << 19) | (g << 11) | (b << 3);
    }

    unsigned int a = (entry >> 12) & 0x0F;
    unsigned int r = (entry >> 8) & 0x0F;
    unsigned int g = (entry >> 4) & 0x0F;
    unsigned int b = entry & 0x0F;
    return (a << 28) | (r << 20) | (g << 12) | (b << 4);
}

static inline unsigned short Convert32To16(unsigned int entry) {
    unsigned int a = entry >> 24;
    unsigned int r = (entry >> 16) & 0xFF;
    unsigned int g = (entry >> 8) & 0xFF;
    unsigned int b = entry & 0xFF;

    if (a > 0xEF) {
        return 0x8000 | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
    }

    return ((a >> 4) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4);
}

void TextureInfoPlatInterface::SetPlatInfo(TextureInfoPlatInfo *info) {
    this->PlatInfo = info;
}

void TextureInfoPlatInterface::Init() {
    TextureInfo *texture_info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();

    bMemSet(&plat_info->ImageInfos, 0, 0x2C);
    GXInvalidateTexAll();
    DCFlushRange(texture_info->ImageData, texture_info->BaseImageSize);
    DCFlushRange(texture_info->PaletteData, texture_info->PaletteSize);
    plat_info->SetImage(texture_info);
}

void TextureInfoPlatInterface::Close() {}

void *TextureInfoPlatInterface::LockImage(TextureLockType lock) {
    TextureInfo *texture_info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();
    return texture_info->ImageData;
}

void TextureInfoPlatInterface::UnlockImage(void *image_lock) {}

void *TextureInfoPlatInterface::LockPalette(TextureLockType lock) {
    TextureInfo *texture_info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();
    unsigned short *gcPal = static_cast<unsigned short *>(texture_info->PaletteData);
    unsigned int *Pal32 = nullptr;

    if (gcPal) {
        Pal32 = new unsigned int[256];
        if (Pal32) {
            for (int j = 0; j <= 0xFF; j++) {
                Pal32[j] = Convert16To32(gcPal[j]);
            }
        }
    }

    return Pal32;
}

void TextureInfoPlatInterface::UnlockPalette(void *palette_lock) {
    TextureInfo *texture_info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();

    if (palette_lock) {
        unsigned short *gcPal = static_cast<unsigned short *>(texture_info->PaletteData);
        unsigned int *Pal32 = reinterpret_cast<unsigned int *>(palette_lock);

        for (int j = 0; j <= 0xFF; j++) {
            gcPal[j] = Convert32To16(Pal32[j]);
        }

        delete[] Pal32;
    }
}

void *TextureInfoPlatInterface::CreateAnimData() {
    TextureInfo *info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();
    unsigned int *val = reinterpret_cast<unsigned int *>(bOMalloc(eAnimTextureSlotPool));

    val[0] = reinterpret_cast<unsigned int>(info->ImageData);
    val[1] = reinterpret_cast<unsigned int>(info->PaletteData);
    return val;
}

void TextureInfoPlatInterface::ReleaseAnimData(void *anim_data) {
    bFree(eAnimTextureSlotPool, anim_data);
}

void TextureInfoPlatInterface::SetAnimData(void *anim_data) {
    TextureInfo *info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();
    unsigned int *val = reinterpret_cast<unsigned int *>(anim_data);

    info->ImageData = reinterpret_cast<void *>(val[0]);
    info->PaletteData = reinterpret_cast<void *>(val[1]);
    plat_info->SetImage(info);
}

unsigned char TextureInfoPlatInfo::HasClut() {
    unsigned int texture_format = this->Format & 0x7FFFFFFF;
    return texture_format >= 8 && texture_format <= 10;
}

unsigned char TextureInfoPlatInfo::SetImage(TextureInfo *texture_info) {
    TextureInfoPlatInfo *plat_info = texture_info->GetPlatInfo();

    if (plat_info) {
        plat_info->SetImage(texture_info->Width, texture_info->Height, texture_info->NumMipMapLevels, plat_info->Format,
                            texture_info->ImageData, texture_info->PaletteData, texture_info->AlphaUsageType, texture_info->TilableUV);
        return 1;
    }

    return 0;
}

unsigned char TextureInfoPlatInfo::SetImage(int width, int height, int mip, int format, void *imageData, void *imagePal,
                                            int alphaUsageType, int clamp) {
    int wrap_s = 0;
    int wrap_t = 0;
    int texture_format = format & 0x7FFFFFFF;
    GXTlutFmt tlut_format = static_cast<GXTlutFmt>(format >= 0 ? GX_TL_RGB5A3 : GX_TL_IA8);

    if (clamp & 1) {
        int width_lsb = width & (~width + 1);

        if (width == width_lsb) {
            wrap_s = 1;
        }
    }

    if (clamp & 2) {
        int height_lsb = height & (~height + 1);

        if (height == height_lsb) {
            wrap_t = 1;
        }
    }

    if (HasClut()) {
        GXTexObj *obj = &ImageInfos.obj;

        GXInitTexObjCI(obj, imageData, static_cast<u16>(width), static_cast<u16>(height), static_cast<GXCITexFmt>(texture_format),
                       static_cast<GXTexWrapMode>(wrap_s), static_cast<GXTexWrapMode>(wrap_t), static_cast<u8>(mip), 0);
        GXInitTlutObj(&ImageInfos.objClut, imagePal, tlut_format, texture_format == GX_TF_C4 ? 0x10 : 0x100);
    } else {
        GXInitTexObj(&ImageInfos.obj, imageData, static_cast<u16>(width), static_cast<u16>(height), static_cast<GXTexFmt>(texture_format),
                     static_cast<GXTexWrapMode>(wrap_s), static_cast<GXTexWrapMode>(wrap_t), static_cast<u8>(mip));
    }

    if (mip) {
        float max_lod = static_cast<float>(mip - 1);

        if (alphaUsageType && max_lod > 1.0f) {
            max_lod -= 1.0f;
        }

        GXInitTexObjLOD(&ImageInfos.obj, GX_LIN_MIP_LIN, GX_LINEAR, 0.0f, max_lod, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    } else {
        GXInitTexObjLOD(&ImageInfos.obj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    }

    return 1;
}

int eSetTexture(TextureInfo *texture_info, int stage) {
    static int stagePrev;

    if (texture_info == pTexPrev && stage == stagePrev) {
        return 0;
    }

    if (texture_info->GetPlatInfo()->HasClut()) {
        GXLoadTlut(&texture_info->GetPlatInfo()->ImageInfos.objClut, 0);
    }

    GXLoadTexObj(&texture_info->GetPlatInfo()->ImageInfos.obj, static_cast<GXTexMapID>(stage));

    pTexPrev = texture_info;
    stagePrev = stage;
    return 1;
}
