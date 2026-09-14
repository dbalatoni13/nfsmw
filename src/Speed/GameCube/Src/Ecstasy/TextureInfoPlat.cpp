#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern SlotPool *eAnimTextureSlotPool;
extern TextureInfo *pTexPrev;

static inline unsigned int Convert16To32(unsigned short entry) {
    unsigned int a;
    unsigned int r;
    unsigned int g;
    unsigned int b;

    if (entry & 0x8000) {
        a = 0xFF;
        r = ((entry >> 10) & 0x1F) << 3;
        g = ((entry >> 5) & 0x1F) << 3;
        b = (entry & 0x1F) << 3;
    } else {
        a = ((entry >> 12) & 0x0F) << 5;
        r = ((entry >> 8) & 0x0F) << 4;
        g = ((entry >> 4) & 0x0F) << 4;
        b = (entry & 0x0F) << 4;
    }

    return (a << 24) | (b << 16) | (g << 8) | r;
}

static inline unsigned short Convert32To16(unsigned int entry) {
    unsigned int r = (entry >> 16) & 0xFF;
    unsigned int g = (entry >> 8) & 0xFF;
    unsigned int b = entry & 0xFF;
    unsigned int a = entry >> 24;
    unsigned int hi;

    if (a > 0xEF) {
        hi = 0xFFFF8000 | ((b >> 3) << 10);
        return hi | ((g >> 3) << 5) | (r >> 3);
    }

    return ((a << 7) & 0x7000) | ((b >> 4) << 8) | ((g >> 4) << 4) | (r >> 4);
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
    void *pTempPal = nullptr;
    unsigned short *gcPal = static_cast<unsigned short *>(texture_info->PaletteData);

    if (gcPal) {
        unsigned int *Pal32 = new unsigned int[256];

        pTempPal = Pal32;
        if (pTempPal) {
            for (int j = 0; j <= 0xFF; j++) {
                Pal32[j] = Convert16To32(gcPal[j]);
            }
        }
    }

    return pTempPal;
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
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();
    TextureInfo *info = static_cast<TextureInfo *>(this);
    unsigned int *val = reinterpret_cast<unsigned int *>(bOMalloc(eAnimTextureSlotPool));

    val[0] = reinterpret_cast<unsigned int>(info->ImageData);
    val[1] = reinterpret_cast<unsigned int>(info->PaletteData);
    return val;
}

void TextureInfoPlatInterface::ReleaseAnimData(void *anim_data) {
    bFree(eAnimTextureSlotPool, anim_data);
}

void TextureInfoPlatInterface::SetAnimData(void *anim_data) {
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();
    TextureInfo *info = static_cast<TextureInfo *>(this);

    info->ImageData = reinterpret_cast<void *>(reinterpret_cast<unsigned int *>(anim_data)[0]);
    info->PaletteData = reinterpret_cast<void *>(reinterpret_cast<unsigned int *>(anim_data)[1]);
    plat_info->SetImage(info);
}

static inline unsigned char IsPow2(int n) {
    return n == (n & (~n + 1));
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
    GXTexWrapMode wrap_s;
    GXTexWrapMode wrap_t;
    unsigned int texture_format;
    unsigned int texture_format_IA8;
    unsigned int palette_format;

    wrap_s = GX_CLAMP;
    wrap_t = GX_CLAMP;

    if (clamp & 1) {
        if (IsPow2(width)) {
            wrap_s = GX_REPEAT;
        }
    }

    texture_format_IA8 = GX_TL_IA8;
    if (clamp & 2) {
        if (IsPow2(height)) {
            wrap_t = GX_REPEAT;
        }
    }

    texture_format = format & 0x7FFFFFFF;
    palette_format = format >= static_cast<int>(texture_format_IA8) ? GX_TL_RGB5A3 : GX_TL_IA8;

    if (HasClut()) {
        GXInitTexObjCI(&ImageInfos.obj, imageData, static_cast<u16>(width), static_cast<u16>(height), static_cast<GXCITexFmt>(texture_format),
                       wrap_s, wrap_t, static_cast<u8>(mip), 0);
        GXInitTlutObj(&ImageInfos.objClut, imagePal, static_cast<GXTlutFmt>(palette_format), texture_format == GX_TF_C4 ? 0x10 : 0x100);
    } else {
        GXInitTexObj(&ImageInfos.obj, imageData, static_cast<u16>(width), static_cast<u16>(height), static_cast<GXTexFmt>(texture_format),
                     wrap_s, wrap_t, static_cast<u8>(mip));
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
