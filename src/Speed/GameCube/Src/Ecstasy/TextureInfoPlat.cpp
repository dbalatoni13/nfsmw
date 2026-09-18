#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

// Pool de datos de texturas animadas: en el original vive en el .data de esta
// unidad (inicializado a cero) y lo crea EcstasyE en tiempo de ejecucion.
SlotPool *eAnimTextureSlotPool = 0;

// STRIPPED
void eInitTexture() {}

TextureInfo *pTexPrev = 0;
static int stagePrev = 0;

// Evita recargar la textura si es la misma que ya esta en esa etapa. La paleta
// solo se carga cuando el formato la lleva.
int eSetTexture(TextureInfo *texture_info, int stage) {
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

// STRIPPED
void eUnSwizzle8bitPalette(unsigned int *palette) {}

// STRIPPED
void eSwizzle8bitPalette(unsigned int *palette) {}

void TextureInfoPlatInterface::SetPlatInfo(TextureInfoPlatInfo *info) {
    this->PlatInfo = info;
}

void TextureInfoPlatInterface::Init() {
    TextureInfo *texture_info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->PlatInfo;

    bMemSet(&plat_info->ImageInfos, 0, sizeof(plat_info->ImageInfos));
    GXInvalidateTexAll();

    DCFlushRange(texture_info->ImageData, texture_info->BaseImageSize);
    DCFlushRange(texture_info->PaletteData, texture_info->PaletteSize);

    plat_info->SetImage(texture_info);
}

// STRIPPED
void TextureInfoPlatInterface::Close() {}

void *TextureInfoPlatInterface::LockImage(TextureLockType lock) {
    return static_cast<TextureInfo *>(this)->ImageData;
}

// STRIPPED
void TextureInfoPlatInterface::UnlockImage(void *image_lock) {}

// La paleta del GameCube es RGB5A3: si el bit alto esta puesto son cinco bits
// por componente y opaco; si no, tres de alfa y cuatro por componente. Se
// convierte a 32 bits en orden alfa-azul-verde-rojo.
static inline unsigned int Convert16To32(unsigned short data) {
    unsigned int a;
    unsigned int b;
    unsigned int g;
    unsigned int r;

    if (data & 0x8000) {
        r = (data >> 7) & 0xF8;
        a = 0xFF;
        g = (data >> 2) & 0xF8;
        b = (data << 3) & 0xF8;
    } else {
        b = (data << 4) & 0xF0;
        a = (data >> 12) << 5;
        r = (data >> 4) & 0xF0;
        g = data & 0xF0;
    }

    return (a << 24) | (b << 16) | (g << 8) | r;
}

void *TextureInfoPlatInterface::LockPalette(TextureLockType lock) {
    TextureInfo *texture_info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();
    void *pTempPal = 0;
    unsigned short *gcPal = static_cast<unsigned short *>(static_cast<TextureInfo *>(this)->PaletteData);

    if (gcPal == 0) {
        return pTempPal;
    }

    pTempPal = new unsigned int[256];

    if (pTempPal != 0) {
        unsigned int *Pal32 = static_cast<unsigned int *>(pTempPal);

        for (int j = 0; j <= 255; j++) {
            Pal32[j] = Convert16To32(gcPal[j]);
        }
    }

    return pTempPal;
}

// Inversa de LockPalette: reempaqueta a RGB5A3 y libera el buffer. Con alfa por
// encima de 0xEF la entrada se considera opaca y va en cinco bits por componente.
static inline unsigned short Convert32To16(unsigned int entry) {
    unsigned int a = entry >> 24;
    unsigned int b = (entry >> 16) & 0xFF;
    unsigned int g = (entry >> 8) & 0xFF;
    unsigned int r = entry & 0xFF;

    unsigned int result;

    if (a > 0xEF) {
        unsigned int t = 0xFFFF8000 | ((r >> 3) << 10);
        result = t | ((g >> 3) << 5) | (b >> 3);
    } else {
        result = (a & 0xE0) << 7;
        result |= (r >> 4) << 8;
        result |= (g >> 4) << 4;
        result |= b >> 4;
    }
    return result;
}

void TextureInfoPlatInterface::UnlockPalette(void *palette_lock) {
    TextureInfo *texture_info = static_cast<TextureInfo *>(this);
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();

    if (palette_lock != nullptr) {
        unsigned short *gcPal = static_cast<unsigned short *>(static_cast<TextureInfo *>(this)->PaletteData);
        unsigned int *Pal32 = static_cast<unsigned int *>(palette_lock);
        {
            int j;
            for (j = 0; j <= 255; j++) {
                gcPal[j] = Convert32To16(Pal32[j]);
            }
        }
    }
    if (palette_lock != nullptr) {
        delete[] static_cast<unsigned int *>(palette_lock);
    }
}

// El bloque de animacion guarda los dos punteros de imagen para poder
// restaurarlos luego.
void *TextureInfoPlatInterface::CreateAnimData() {
    TextureInfo *texture_info = static_cast<TextureInfo *>(this);
    void **anim_data = static_cast<void **>(bOMalloc(eAnimTextureSlotPool));

    anim_data[0] = texture_info->ImageData;
    anim_data[1] = texture_info->PaletteData;

    return anim_data;
}

void TextureInfoPlatInterface::ReleaseAnimData(void *anim_data) {
    bFree(eAnimTextureSlotPool, anim_data);
}

void TextureInfoPlatInterface::SetAnimData(void *anim_data) {
    TextureInfoPlatInfo *plat_info = this->GetPlatInfo();
    TextureInfo *info = static_cast<TextureInfo *>(this);

    info->ImageData = static_cast<void **>(anim_data)[0];
    info->PaletteData = static_cast<void **>(anim_data)[1];

    plat_info->SetImage(info);
}

// Los formatos con paleta de GX son C4, C8 y C14X2, o sea 8, 9 y 10; el original
// lo resuelve como un rango sobre el formato con el bit alto enmascarado.
bool TextureInfoPlatInfo::HasClut() {
    return (this->Format & 0x7FFFFFFF) - GX_TF_C4 <= 2;
}

// Los bits de clamp solo se respetan si la dimension es potencia de dos; el test
// es w == (w & -w). El bit de signo del formato elige el formato de la paleta, y
// los cuatro bits bajos, cuantas entradas tiene.
static inline unsigned char IsPow2(int n) {
    return n == (n & (~n + 1));
}

unsigned char TextureInfoPlatInfo::SetImage(int width, int height, int mip, int format, void *imageData, void *imagePal, int alphaUsageType,
                                            int clamp) {
    GXTexWrapMode wrap_s = GX_CLAMP;
    GXTexWrapMode wrap_t = GX_CLAMP;

    if (clamp & 1) {
        if (IsPow2(width)) {
            wrap_s = GX_REPEAT;
        }
    }

    if (clamp & 2) {
        if (IsPow2(height)) {
            wrap_t = GX_REPEAT;
        }
    }

    unsigned int texture_format = format & 0x7FFFFFFF;
    int palette_format = (format & 0x80000000) ? 0 : 2;

    if (this->HasClut()) {
        GXInitTexObjCI(&this->ImageInfos.obj, imageData, width, height, static_cast<GXCITexFmt>(texture_format),
                       wrap_s, wrap_t, mip, 0);
        GXInitTlutObj(&this->ImageInfos.objClut, imagePal, static_cast<GXTlutFmt>(palette_format), texture_format == 8 ? 0x10 : 0x100);
    } else {
        GXInitTexObj(&this->ImageInfos.obj, imageData, width, height, static_cast<GXTexFmt>(texture_format),
                     wrap_s, wrap_t, mip);
    }

    if (mip != 0) {
        float max_lod = static_cast<float>(mip - 1);

        if (alphaUsageType != 0) {
            if (max_lod > 1.0f) {
                max_lod -= 1.0f;
            }
        }

        GXInitTexObjLOD(&this->ImageInfos.obj, static_cast<GXTexFilter>(5), static_cast<GXTexFilter>(1), 0.0f, max_lod, 0.0f, 0, 0,
                        static_cast<GXAnisotropy>(0));
    } else {
        GXInitTexObjLOD(&this->ImageInfos.obj, static_cast<GXTexFilter>(1), static_cast<GXTexFilter>(1), 0.0f, 0.0f, 0.0f, 0, 0,
                        static_cast<GXAnisotropy>(0));
    }

    return 1;
}

unsigned char TextureInfoPlatInfo::SetImage(TextureInfo *texture_info) {
    TextureInfoPlatInfo *plat_info = texture_info->PlatInfo;

    if (plat_info != 0) {
        plat_info->SetImage(texture_info->Width, texture_info->Height, texture_info->NumMipMapLevels, plat_info->Format,
                            texture_info->ImageData, texture_info->PaletteData, texture_info->AlphaUsageType,
                            texture_info->TilableUV);
        return 1;
    }

    return 0;
}