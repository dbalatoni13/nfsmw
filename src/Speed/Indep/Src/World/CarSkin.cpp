#include "./Car.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void *TextureInfo_LockImage(TextureInfoPlatInterface *texture_info, TextureLockType lock)
    asm("LockImage__24TextureInfoPlatInterface15TextureLockType");
void TextureInfo_UnlockImage(TextureInfoPlatInterface *texture_info, void *image_lock) asm("UnlockImage__24TextureInfoPlatInterfacePv");
void *TextureInfo_LockPalette(TextureInfoPlatInterface *texture_info, TextureLockType lock)
    asm("LockPalette__24TextureInfoPlatInterface15TextureLockType");
void TextureInfo_UnlockPalette(TextureInfoPlatInterface *texture_info, void *palette_lock) asm("UnlockPalette__24TextureInfoPlatInterfacePv");
void eUnSwizzle8bitPalette(unsigned int *palette);
void eSwizzle8bitPalette(unsigned int *palette);
unsigned int ScaleColours(unsigned int a, unsigned int b);
unsigned int GetBlendColour(unsigned int *colours, float *weights, int num_colours, bool max_alpha_blend);
unsigned int GetWheelTextureHash(RideInfo *ride_info);
unsigned int GetWheelTextureMaskHash(RideInfo *ride_info);
int CompositeWheel(RideInfo *ride_info, unsigned int dest_namehash, unsigned int src_namehash, unsigned int mask_namehash, CAR_SLOT_ID paint_slot);

SkinCompositeParams SkinCompositeParameterCache[4];

SkinCompositeParams *GetSkinCompositeParams(unsigned int dest_name_hash) {
    SkinCompositeParams *cache_params = 0;

    if (dest_name_hash == 0x530B82B1) {
        cache_params = &SkinCompositeParameterCache[1];
    } else if (dest_name_hash < 0x530B82B2) {
        if (dest_name_hash == 0x530B82B0) {
            cache_params = &SkinCompositeParameterCache[0];
        }
    } else if (dest_name_hash == 0x530B82B2) {
        cache_params = &SkinCompositeParameterCache[2];
    } else if (dest_name_hash == 0x530B82B3) {
        cache_params = &SkinCompositeParameterCache[3];
    }

    return cache_params;
}

bool CompareCompositeParams(SkinCompositeParams *a, SkinCompositeParams *b) {
    VinylLayerInfo *info_a;
    VinylLayerInfo *info_b;

    if (a->DestTexture != b->DestTexture || a->BaseColour != b->BaseColour) {
        return false;
    }

    for (int i = 0; i < 4; i++) {
        if (a->SwatchColours[i] != b->SwatchColours[i]) {
            return false;
        }
    }

    info_a = &a->VinylLayerInfos[0];
    info_b = &b->VinylLayerInfos[0];

    if (info_a->m_LayerHash != info_b->m_LayerHash || info_a->m_LayerTexture != info_b->m_LayerTexture ||
        info_a->m_LayerMaskTexture != info_b->m_LayerMaskTexture || info_a->m_NumColours != info_b->m_NumColours ||
        info_a->m_RemapPalette != info_b->m_RemapPalette || info_a->m_RemapColours[0] != info_b->m_RemapColours[0] ||
        info_a->m_RemapColours[1] != info_b->m_RemapColours[1] || info_a->m_RemapColours[2] != info_b->m_RemapColours[2] ||
        info_a->m_RemapColours[3] != info_b->m_RemapColours[3]) {
        return false;
    }

    return true;
}

void UpdateSkinCompositeCache(SkinCompositeParams *skin_composite_params) {
    SkinCompositeParams *cache_params = GetSkinCompositeParams(skin_composite_params->DestTexture->NameHash);

    if (cache_params != 0) {
        bMemCpy(cache_params, skin_composite_params, sizeof(*skin_composite_params));
    }
}

void FlushFromSkinCompositeCache(unsigned int texture_name_hash) {
    SkinCompositeParams *cache_params = GetSkinCompositeParams(texture_name_hash);

    if (cache_params != 0) {
        bMemSet(cache_params, 0, sizeof(*cache_params));
    }
}

int CompositeWheel32(TextureInfo *dest_texture, TextureInfo *src_texture, TextureInfo *src_mask, unsigned int remap_colour) {
    unsigned int *dest_image_data = static_cast<unsigned int *>(TextureInfo_LockImage(dest_texture, TEXLOCK_WRITE));
    unsigned int *src_image_data = static_cast<unsigned int *>(TextureInfo_LockImage(src_texture, TEXLOCK_READ));
    unsigned int *src_mask_data = static_cast<unsigned int *>(TextureInfo_LockImage(src_mask, TEXLOCK_READ));
    int num_pixels = dest_texture->Width * dest_texture->Height;
    unsigned int *dest_pixel = dest_image_data;
    unsigned int *end_pixel = dest_image_data + num_pixels;
    unsigned int *src_pixel = src_image_data;
    unsigned int *src_mask_pixel = src_mask_data;

    while (dest_pixel < end_pixel) {
        unsigned int colour_pixel = *src_pixel;
        unsigned int blend_colours[2];
        float blend = static_cast<float>(reinterpret_cast<unsigned char *>(src_mask_pixel)[2]) / 255.0f;
        float weights[2];

        blend_colours[0] = colour_pixel;
        blend_colours[1] = ScaleColours(colour_pixel, remap_colour);
        weights[0] = 1.0f - blend;
        weights[1] = blend;

        colour_pixel = GetBlendColour(blend_colours, weights, 2, false);
        *dest_pixel = colour_pixel & 0xFFFFFF;
        *dest_pixel = (colour_pixel & 0xFFFFFF) | (blend_colours[0] & 0xFF000000);

        dest_pixel++;
        src_pixel++;
        src_mask_pixel++;
    }

    TextureInfo_UnlockImage(src_mask, src_mask_data);
    TextureInfo_UnlockImage(src_texture, src_image_data);
    TextureInfo_UnlockImage(dest_texture, dest_image_data);

    return 1;
}

int CompositeWheel8(TextureInfo *dest_texture, TextureInfo *src_texture, TextureInfo *src_mask, unsigned int remap_colour) {
    unsigned char *dest_image_data = static_cast<unsigned char *>(TextureInfo_LockImage(dest_texture, TEXLOCK_WRITE));
    unsigned int *dest_palette_data = static_cast<unsigned int *>(TextureInfo_LockPalette(dest_texture, TEXLOCK_WRITE));
    unsigned char *src_image_data = static_cast<unsigned char *>(TextureInfo_LockImage(src_texture, TEXLOCK_READ));
    unsigned int *src_palette_data = static_cast<unsigned int *>(TextureInfo_LockPalette(src_texture, TEXLOCK_READ));
    unsigned char *src_mask_data = static_cast<unsigned char *>(TextureInfo_LockImage(src_mask, TEXLOCK_READ));
    unsigned int *src_mask_palette_data = static_cast<unsigned int *>(TextureInfo_LockPalette(src_mask, TEXLOCK_READ));
    unsigned int row = 0;

    eUnSwizzle8bitPalette(dest_palette_data);
    eUnSwizzle8bitPalette(src_palette_data);
    eUnSwizzle8bitPalette(src_mask_palette_data);

    do {
        float blend = static_cast<float>(row) / 15.0f;
        int i = 0;

        do {
            unsigned int blend_colours[2];
            float weights[2];
            unsigned int colour = src_palette_data[row * 16 + i];

            blend_colours[0] = colour;
            blend_colours[1] = ScaleColours(colour, remap_colour);
            weights[0] = 1.0f - blend;
            weights[1] = blend;

            colour = GetBlendColour(blend_colours, weights, 2, false);
            dest_palette_data[(row * 16) + i] = colour & 0xFFFFFF;
            dest_palette_data[(row * 16) + i] = (colour & 0xFFFFFF) | (blend_colours[0] & 0xFF000000);
            i++;
        } while (i < 16);

        row++;
    } while (row < 16);

    eUnSwizzle8bitPalette(dest_palette_data);
    eUnSwizzle8bitPalette(src_palette_data);
    TextureInfo_UnlockPalette(dest_texture, dest_palette_data);
    TextureInfo_UnlockPalette(src_texture, src_palette_data);

    {
        int num_pixels = dest_texture->Width * dest_texture->Height;
        unsigned char *dest_pixel = dest_image_data;
        unsigned char *end_pixel = dest_image_data + num_pixels;
        unsigned char *src_pixel = src_image_data;
        unsigned char *src_mask_pixel = src_mask_data;

        for (; dest_pixel < end_pixel; dest_pixel++) {
            unsigned int mask = src_mask_palette_data[*src_mask_pixel];

            *dest_pixel = *src_pixel + (static_cast<unsigned char>(mask) & 0xF0);
            src_pixel++;
            src_mask_pixel++;
        }
    }

    eSwizzle8bitPalette(src_mask_palette_data);
    TextureInfo_UnlockPalette(src_mask, src_mask_palette_data);
    TextureInfo_UnlockImage(src_mask, src_mask_data);
    TextureInfo_UnlockImage(src_texture, src_image_data);
    TextureInfo_UnlockImage(dest_texture, dest_image_data);

    return 1;
}

int CompositeWheel(RideInfo *ride_info, unsigned int dest_namehash, unsigned int src_namehash, unsigned int mask_namehash, CAR_SLOT_ID paint_slot) {
    TextureInfo *dest_texture = GetTextureInfo(dest_namehash, false, false);

    if (dest_texture == 0) {
        return 1;
    }

    int do_32bit_composite = dest_texture->ImageCompressionType == TEXCOMP_32BIT;
    TextureInfo *src_texture = GetTextureInfo(src_namehash, false, false);
    TextureInfo *src_mask = GetTextureInfo(mask_namehash, false, false);

    if (src_texture != 0 && src_mask != 0) {
        CarPart *car_part = ride_info->GetPart(paint_slot);
        unsigned int wheel_colour = 0xFFFFFFFF;

        if (car_part != 0) {
            int red = car_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            int green = car_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            int blue = car_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
            int gloss = car_part->GetAppliedAttributeIParam(bStringHash("GLOSS"), 0);

            wheel_colour = (gloss << 24) | (blue << 16) | (green << 8) | red;
        }

        if (do_32bit_composite != 0) {
            if (src_texture->ImageCompressionType != TEXCOMP_32BIT) {
                return 0;
            }

            if (src_mask->ImageCompressionType != TEXCOMP_32BIT) {
                return 0;
            }
        } else {
            if (src_texture->ImageCompressionType == TEXCOMP_32BIT) {
                return 0;
            }

            if (src_mask->ImageCompressionType == TEXCOMP_32BIT) {
                return 0;
            }
        }

        if (dest_texture->Width == src_texture->Width && dest_texture->Width == src_mask->Width &&
            dest_texture->Height == src_texture->Height && dest_texture->Height == src_mask->Height) {
            if (do_32bit_composite != 0) {
                return CompositeWheel32(dest_texture, src_texture, src_mask, wheel_colour);
            }

            return CompositeWheel8(dest_texture, src_texture, src_mask, wheel_colour);
        }
    }

    return 0;
}

int CompositeRim(RideInfo *ride_info) {
    unsigned int dest_namehash = ride_info->GetCompositeWheelNameHash();
    unsigned int src_namehash = GetWheelTextureHash(ride_info);
    unsigned int mask_namehash = GetWheelTextureMaskHash(ride_info);

    return CompositeWheel(ride_info, dest_namehash, src_namehash, mask_namehash, CARSLOTID_PAINT_RIM);
}
