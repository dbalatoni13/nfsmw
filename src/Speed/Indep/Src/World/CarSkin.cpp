#include "./Car.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
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
unsigned int RemapColour(unsigned int colour, unsigned int *remap_colours);
char *bStrCat(char *dest, const char *source1, const char *source2);
void initnet(unsigned char *thepic, int len, int num_colours, int sample);
void learn();
void unbiasnet();
void nqGetPaletteEntry(int i, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a);
void inxbuild();
int inxsearch(int b, int g, int r, int aa);
int UsedCarTextureAddToTable(unsigned int *table, int num_used, int max_textures, unsigned int texture_hash);
unsigned int GetWheelTextureHash(RideInfo *ride_info);
unsigned int GetWheelTextureMaskHash(RideInfo *ride_info);
unsigned int GetVinylLayerHash(RideInfo *ride_info, int layer);
unsigned int GetVinylLayerMaskHash(RideInfo *ride_info, int layer);
unsigned int GetHoodSpoilerHash(RideInfo *ride_info);
unsigned int GetHoodSpoilerMaskHash(RideInfo *ride_info);
unsigned int GetSpinnerTextureHash(RideInfo *ride_info);
unsigned int GetSpinnerTextureMaskHash(RideInfo *ride_info);
int DumpPreComp(VinylLayerInfo *layer_info, TextureInfo *dest_texture);
extern int UsePrecompositeVinyls;
extern int swatch_offset_init;
extern int swatch_offset_count[4];
extern int swatch_offset_cache[64];
int CompositeSkin(SkinCompositeParams *composite_params);
int CompositeSkin32(SkinCompositeParams *composite_params);
int IsInSkinCompositeCache(SkinCompositeParams *skin_composite_params);
int CompositeWheel(RideInfo *ride_info, unsigned int dest_namehash, unsigned int src_namehash, unsigned int mask_namehash, CAR_SLOT_ID paint_slot);
int CompositeRim(RideInfo *ride_info);

SkinCompositeParams SkinCompositeParameterCache[4];

struct CompColour {
    unsigned char a;
    unsigned char b;
    unsigned char g;
    unsigned char r;

    CompColour() {}
};

inline char *CarTypeInfo::GetBaseModelName() {
    return BaseModelName;
}

SkinCompositeParams *GetSkinCompositeParams(unsigned int dest_name_hash) {
    SkinCompositeParams *cache_params = nullptr;

    switch (dest_name_hash) {
    case 0x530B82B0:
        cache_params = &SkinCompositeParameterCache[0];
        break;
    case 0x530B82B1:
        cache_params = &SkinCompositeParameterCache[1];
        break;
    case 0x530B82B2:
        cache_params = &SkinCompositeParameterCache[2];
        break;
    case 0x530B82B3:
        cache_params = &SkinCompositeParameterCache[3];
        break;
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

int IsInSkinCompositeCache(SkinCompositeParams *skin_composite_params) {
    SkinCompositeParams *cache_params = GetSkinCompositeParams(skin_composite_params->DestTexture->NameHash);
    bool match;

    if (cache_params) {
        match = CompareCompositeParams(cache_params, skin_composite_params);
        return match;
    }

    return 0;
}

int CompositeSkin32(SkinCompositeParams *composite_params) {
    TextureInfo *dest_texture;
    unsigned int base_colour;
    unsigned int *swatch_colours;
    VinylLayerInfo *layer_infos;
    int num_layers;
    int debug_print;

    dest_texture = composite_params->DestTexture;
    swatch_colours = composite_params->SwatchColours;
    layer_infos = composite_params->VinylLayerInfos;
    num_layers = composite_params->NumLayers;
    base_colour = composite_params->BaseColour;
    (void)debug_print;

    if (dest_texture == 0) {
        return 0;
    }

    if (dest_texture->ImageCompressionType == TEXCOMP_32BIT) {
        unsigned int *dest_image_data = static_cast<unsigned int *>(TextureInfo_LockImage(dest_texture, TEXLOCK_WRITE));
        int dest_width = dest_texture->Width;
        int dest_height = dest_texture->Height;
        CompColour base;
        unsigned int *dest_pixel;
        unsigned int *end_pixel;
        int num_pixels;

        if (swatch_offset_init == 0) {
            unsigned int swatch_lookup_colours[4] = {
                0xBF0000FF,
                0xBF00FF00,
                0xBFFF0000,
                0xBFFF00FF,
            };
            unsigned int *dest = dest_image_data;
            unsigned int *dest_end;

            bMemSet(swatch_offset_cache, 0, sizeof(swatch_offset_cache));

            dest_end = dest_image_data + dest_width * dest_height;
            while (dest < dest_end) {
                int pixel_offset = dest - dest_image_data;
                int i = 0;

                do {
                    if (*dest == swatch_lookup_colours[i]) {
                        int *swatch_offsets = swatch_offset_cache + i * 16;
                        int count = swatch_offset_count[i];

                        swatch_offset_count[i] = count + 1;
                        swatch_offsets[count] = pixel_offset;
                        break;
                    }

                    i++;
                } while (i < 4);

                dest++;
            }

            swatch_offset_init = 1;
        }

        num_pixels = dest_width * dest_height;
        *reinterpret_cast<unsigned int *>(&base) = base_colour;
        base.g = static_cast<unsigned char>(base_colour >> 24);
        base.a = static_cast<unsigned char>(base_colour >> 8);

        for (dest_pixel = dest_image_data, end_pixel = dest_image_data + num_pixels; dest_pixel < end_pixel; dest_pixel++) {
            *dest_pixel = *reinterpret_cast<unsigned int *>(&base);
        }

        for (int i = 0; i < num_layers; i++) {
            VinylLayerInfo *info = &layer_infos[i];

            if (info->m_LayerMaskData != 0) {
                unsigned int *image_src = reinterpret_cast<unsigned int *>(info->m_LayerImageData);
                unsigned int *dest = dest_image_data;
                unsigned int *mask_src = reinterpret_cast<unsigned int *>(info->m_LayerMaskData);
                unsigned int *image_end = image_src + num_pixels;

                for (; image_src < image_end; image_src++, mask_src++, dest++) {
                    unsigned int src_pixel = *image_src;
                    unsigned int src_mask = *mask_src;
                    unsigned int dest_pixel = *dest;
                    unsigned int blend_value = reinterpret_cast<unsigned char *>(&src_mask)[2];

                    if (info->m_RemapPalette != 0 && blend_value != 0) {
                        CompColour src_colour;

                        *reinterpret_cast<unsigned int *>(&src_colour) = src_pixel;
                        src_colour.g = static_cast<unsigned char>(src_pixel >> 24);
                        src_colour.a = static_cast<unsigned char>(src_pixel >> 8);
                        src_pixel = RemapColour(*reinterpret_cast<unsigned int *>(&src_colour), info->m_RemapColours);
                    }

                    if (blend_value < 0x80) {
                        if (blend_value != 0) {
                            unsigned int colours[2];
                            float weights[2];

                            weights[0] = static_cast<float>(blend_value) / 255.0f;
                            weights[1] = 1.0f - weights[0];

                            if (1.0f < weights[0]) {
                                weights[0] = 1.0f;
                            }

                            if (weights[1] < 0.0f) {
                                weights[1] = 0.0f;
                            }

                            colours[0] = src_pixel;
                            colours[1] = dest_pixel;
                            src_pixel = GetBlendColour(colours, weights, 2, false);
                            *dest = src_pixel;
                        }
                    } else {
                        *dest = src_pixel;
                    }
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            int *swatch_offsets = swatch_offset_cache + i * 16;
            unsigned int swatch_colour = swatch_colours[i];

            for (int j = 0; j < swatch_offset_count[i]; j++) {
                dest_image_data[swatch_offsets[j]] = swatch_colour;
            }
        }

        TextureInfo_UnlockImage(dest_texture, dest_image_data);
        return 1;
    }

    return 0;
}

unsigned int ScaleColours(unsigned int a, unsigned int b) {
    CompColour *colour_a = reinterpret_cast<CompColour *>(&a);
    CompColour *colour_b = reinterpret_cast<CompColour *>(&b);
    CompColour final_colour;

    final_colour.g = static_cast<unsigned char>(static_cast<float>(colour_a->g) * 0.003921569f * static_cast<float>(colour_b->g));
    final_colour.b = static_cast<unsigned char>(static_cast<float>(colour_a->b) * 0.003921569f * static_cast<float>(colour_b->b));
    final_colour.a = static_cast<unsigned char>(static_cast<float>(colour_a->a) * 0.003921569f * static_cast<float>(colour_b->a));
    final_colour.r = static_cast<unsigned char>(static_cast<float>(colour_a->r) * 0.003921569f * static_cast<float>(colour_b->r));
    return *reinterpret_cast<unsigned int *>(&final_colour);
}

unsigned int GetBlendColour(unsigned int *colours, float *weights, int num_colours, bool max_alpha_blend) {
    CompColour *comp_colours;
    CompColour final_colour;
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;

    for (int i = 0; i < num_colours; i++) {
        float weight = weights[i];

        if (weight > 0.003921569f) {
            comp_colours = reinterpret_cast<CompColour *>(&colours[i]);
            b += static_cast<int>(weight * static_cast<float>(comp_colours->b));
            g += static_cast<int>(weight * static_cast<float>(comp_colours->g));
            r += static_cast<int>(weight * static_cast<float>(comp_colours->r));

            if (!max_alpha_blend) {
                a += static_cast<int>(weight * static_cast<float>(comp_colours->a));
            } else {
                int tempa = static_cast<int>(weight * static_cast<float>(comp_colours->a)) & 0xFF;

                if (a < tempa) {
                    a = tempa;
                }
            }
        }
    }

    if (b > 0xFF) {
        b = 0xFF;
    }

    final_colour.b = static_cast<unsigned char>(b);

    if (g > 0xFF) {
        g = 0xFF;
    }

    if (r > 0xFF) {
        r = 0xFF;
    }

    final_colour.r = static_cast<unsigned char>(r);
    final_colour.g = static_cast<unsigned char>(g);

    if (a > 0xFF) {
        a = 0xFF;
    }

    final_colour.a = static_cast<unsigned char>(a);
    return *reinterpret_cast<unsigned int *>(&final_colour);
}

unsigned int RemapColour(unsigned int colour, unsigned int *colour_map) {
    CompColour col = *reinterpret_cast<CompColour *>(&colour);
    float weights[4];
    unsigned int result;

    weights[0] = static_cast<float>(col.r) * 0.003921569f;
    weights[1] = static_cast<float>(col.g) * 0.003921569f;
    weights[2] = static_cast<float>(col.b) * 0.003921569f;
    weights[3] = 0.0f;
    result = GetBlendColour(colour_map, weights, 3, true);
    return result;
}


int CompositeSkin(SkinCompositeParams *composite_params) {
    struct SemiTransPixel {
        short x;
        short y;
    };

    TextureInfo *dest_texture;
    unsigned int base_colour;
    unsigned int *swatch_colours;
    VinylLayerInfo *layer_infos;
    int num_layers;
    int debug_print;

    num_layers = composite_params->NumLayers;
    swatch_colours = composite_params->SwatchColours;
    layer_infos = composite_params->VinylLayerInfos;
    dest_texture = composite_params->DestTexture;
    base_colour = composite_params->BaseColour;
    (void)debug_print;

    if (dest_texture == 0) {
        return 0;
    }

    if (dest_texture->ImageCompressionType == TEXCOMP_8BIT) {
        unsigned char *dest_image_data = static_cast<unsigned char *>(TextureInfo_LockImage(dest_texture, TEXLOCK_WRITE));
        unsigned int *dest_palette_data = static_cast<unsigned int *>(TextureInfo_LockPalette(dest_texture, TEXLOCK_WRITE));
        eUnSwizzle8bitPalette(dest_palette_data);
        int dest_width = dest_texture->Width;
        int dest_height = dest_texture->Height;
        int max_semi_trans_pixels = 0xC000;
        SemiTransPixel *semi_trans_pixels;
        unsigned int *semi_trans_colours;
        int semi_trans_pixels_buffer_size = 0x30000;
        int total_malloc_required = semi_trans_pixels_buffer_size;
        int cur_semi_trans_pixel;
        int num_pixels;
        unsigned char *dest;
        unsigned char *dest_end;
        unsigned char *image_src[1];
        unsigned char *mask_src[1];
        int current_palette_base;

        cur_semi_trans_pixel = 0;
        semi_trans_pixels = static_cast<SemiTransPixel *>(bMalloc(total_malloc_required, 0, 0, (GetVirtualMemoryPoolNumber() & 0xF) | 0x40));
        semi_trans_colours = static_cast<unsigned int *>(bMalloc(total_malloc_required, 0, 0, (GetVirtualMemoryPoolNumber() & 0xF) | 0x40));
        num_pixels = dest_width * dest_height;
        dest_end = dest_image_data + num_pixels;

        for (int i = 0; i < num_layers; i++) {
            VinylLayerInfo *info = &layer_infos[i];

            if (info->m_LayerHash != 0) {
                eUnSwizzle8bitPalette(info->m_LayerImagePaletteData);
                eUnSwizzle8bitPalette(info->m_LayerMaskPaletteData);
                image_src[i] = info->m_LayerImageData;
                mask_src[i] = info->m_LayerMaskData;
            }
        }

        if (swatch_offset_init == 0) {
            unsigned int swatch_lookup_colours[4] = {
                0xA00000F0,
                0xA000F000,
                0xA0F00000,
                0xA0F000F0,
            };
            int swatch_indices[4] = {
                -1,
                -1,
                -1,
                -1,
            };

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 0x100; j++) {
                    if (dest_palette_data[j] == swatch_lookup_colours[i]) {
                        swatch_indices[i] = j;
                        break;
                    }
                }
            }

            bMemSet(swatch_offset_cache, 0, sizeof(swatch_offset_cache));

            for (dest = dest_image_data; dest < dest_end; dest++) {
                int i = 0;

                do {
                    if (static_cast<unsigned int>(*dest) == static_cast<unsigned int>(swatch_indices[i])) {
                        int *swatch_offsets = swatch_offset_cache + i * 16;

                        *dest = static_cast<unsigned char>(i + 1);
                        int count = swatch_offset_count[i];

                        swatch_offset_count[i] = count + 1;
                        swatch_offsets[count] = dest - dest_image_data;
                        break;
                    }

                    i++;
                } while (i < 4);

                if (i == 4) {
                    *dest = 0;
                }
            }

            swatch_offset_init = 1;
        } else {
            bMemSet(dest_image_data, 0, dest_width * dest_height);
        }

        dest_palette_data[0] = base_colour;
        current_palette_base = 1;
        for (int i = 0; i < 4; i++) {
            dest_palette_data[current_palette_base] = swatch_colours[i];
            current_palette_base++;
        }

        for (dest = dest_image_data; dest < dest_end; dest++) {
            unsigned int dest_pixel = dest_palette_data[*dest];

            for (int i = 0; i < num_layers; i++) {
                VinylLayerInfo *info = &layer_infos[i];

                if (info->m_LayerHash != 0) {
                    unsigned int src_pixel = info->m_LayerImagePaletteData[*image_src[i]];
                    unsigned int src_mask = info->m_LayerMaskPaletteData[*mask_src[i]];

                    if (info->m_RemapPalette != 0 && (src_mask & 0xFF) != 0) {
                        src_pixel = RemapColour(src_pixel, info->m_RemapColours);
                    }

                    if ((src_mask & 0xFF) > 0x7F) {
                        *dest = static_cast<unsigned char>(*image_src[i] + current_palette_base);
                        dest_pixel = src_pixel;
                    } else if ((src_mask & 0xFF) != 0) {
                        float weights[2];
                        unsigned int colours[2];
                        unsigned int blend_colour;
                        int x;
                        int y;

                        weights[0] = static_cast<float>(src_mask & 0xFF) / 255.0f;
                        if (weights[0] > 1.0f) {
                            weights[0] = 1.0f;
                        }

                        weights[1] = 1.0f - weights[0];
                        colours[0] = src_pixel;
                        colours[1] = dest_pixel;
                        blend_colour = GetBlendColour(colours, weights, 2, false);
                        semi_trans_colours[cur_semi_trans_pixel] = blend_colour;
                        x = dest - dest_image_data;
                        y = x / dest_texture->Width;
                        semi_trans_pixels[cur_semi_trans_pixel].x = x - y * dest_texture->Width;
                        semi_trans_pixels[cur_semi_trans_pixel].y = y;

                        cur_semi_trans_pixel++;
                        if (cur_semi_trans_pixel >= max_semi_trans_pixels) {
                            cur_semi_trans_pixel--;
                        }

                        *dest = 0xFF;
                    }

                    image_src[i]++;
                    mask_src[i]++;
                }
            }
        }

        for (int i = 0; i < num_layers; i++) {
            VinylLayerInfo *info = &layer_infos[i];

            if (info->m_RemapPalette == 0) {
                for (int j = 0; j < info->m_NumColours; j++) {
                    dest_palette_data[current_palette_base + j] = info->m_LayerImagePaletteData[j];
                }
            } else {
                for (int j = 0; j < info->m_NumColours; j++) {
                    dest_palette_data[current_palette_base + j] =
                        RemapColour(info->m_LayerImagePaletteData[j], info->m_RemapColours);
                }
            }

            eSwizzle8bitPalette(info->m_LayerImagePaletteData);
            eSwizzle8bitPalette(info->m_LayerMaskPaletteData);
            current_palette_base += info->m_NumColours;
        }

        if (cur_semi_trans_pixel != 0) {
            int max_remap_colours = 0xFF - current_palette_base;
            int num_pixels = cur_semi_trans_pixel;
            unsigned char *input;

            if (bLargestMalloc(0) < (num_pixels << 2)) {
                num_pixels = 0;
            }

            input = static_cast<unsigned char *>(bMalloc(num_pixels << 2, 0, 0, 0x40));

            for (int i = 0, p = 0; i < num_pixels; i++, p++) {
                unsigned int pixel_colour = semi_trans_colours[i];

                input[p * 4] = static_cast<unsigned char>(pixel_colour);
                input[p * 4 + 1] = static_cast<unsigned char>(pixel_colour >> 8);
                input[p * 4 + 2] = static_cast<unsigned char>(pixel_colour >> 16);
                input[p * 4 + 3] = static_cast<unsigned char>(pixel_colour >> 24);
            }

            if (num_pixels < max_remap_colours) {
                for (int i = 0; i < num_pixels; i++) {
                    SemiTransPixel *pixel = &semi_trans_pixels[i];
                    unsigned char *dest = &dest_image_data[pixel->x + pixel->y * dest_texture->Width];

                    *dest = static_cast<unsigned char>(current_palette_base + i);
                    dest_palette_data[current_palette_base + i] = semi_trans_colours[i];
                }
            } else {
                initnet(input, num_pixels << 2, max_remap_colours, 0x14);
                learn();
                unbiasnet();

                for (int i = 0; i < max_remap_colours; i++) {
                    unsigned char r;
                    unsigned char g;
                    unsigned char b;
                    unsigned char a;

                    nqGetPaletteEntry(i, r, g, b, a);
                    dest_palette_data[current_palette_base + i] = (((static_cast<unsigned int>(g) << 8) | b) |
                                                                    (static_cast<unsigned int>(r) << 16)) |
                                                                   (static_cast<unsigned int>(a) << 24);
                }

                inxbuild();

                for (int i = 0; i < num_pixels; i++) {
                    SemiTransPixel *pixel = &semi_trans_pixels[i];
                    unsigned char *dest = &dest_image_data[pixel->x + pixel->y * dest_texture->Width];

                    if (*dest == 0xFF) {
                        unsigned int colour = semi_trans_colours[i];
                        int index = inxsearch(colour & 0xFF, (colour >> 8) & 0xFF, (colour >> 16) & 0xFF, colour >> 24);

                        if (index < max_remap_colours) {
                            *dest = static_cast<unsigned char>(index + current_palette_base);
                        } else {
                            *dest = 0;
                        }
                    }
                }
            }

            bFree(input);
        }

        bFree(semi_trans_pixels);
        bFree(semi_trans_colours);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < swatch_offset_count[i]; j++) {
                dest_image_data[swatch_offset_cache[j + i * 16]] = static_cast<unsigned char>(i + 1);
            }
        }

        eSwizzle8bitPalette(dest_palette_data);
        TextureInfo_UnlockImage(dest_texture, dest_image_data);
        TextureInfo_UnlockPalette(dest_texture, dest_palette_data);
        return 1;
    }

    return 0;
}

int CompositeSkin(RideInfo *ride_info) {
    unsigned int dest_namehash;
    TextureInfo *dest_texture;
    int do_32bit_composite;
    const int first_vinyl_layer = 0;
    CarPart *base_paint_part;
    unsigned int base_paint_colour;
    int red;
    int green;
    int blue;
    int gloss;
    unsigned int swatch_colours[4];
    VinylLayerInfo vinyl_layer_infos[1];
    int total_layer_colours;
    const int max_layer_colours = 1;
    int cur_layer;
    SkinCompositeParams composite_params;
    int success;

    if (ride_info->IsUsingCompositeSkin() == 0) {
        return 1;
    }

    dest_namehash = ride_info->GetCompositeSkinNameHash();
    dest_texture = GetTextureInfo(dest_namehash, false, false);
    if (dest_texture == 0) {
        return 1;
    }

    do_32bit_composite = dest_texture->ImageCompressionType == TEXCOMP_32BIT;
    base_paint_part = ride_info->GetPart(CARSLOTID_BASE_PAINT);
    if (base_paint_part == 0) {
        return 1;
    }

    red = base_paint_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
    green = base_paint_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
    blue = base_paint_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
    gloss = base_paint_part->GetAppliedAttributeIParam(bStringHash("GLOSS"), 0);

    base_paint_colour = (gloss << 24) | (blue << 16) | (green << 8) | red;

    for (int i = 0; i < 4; i++) {
        swatch_colours[i] = base_paint_colour;
    }

    total_layer_colours = 0;
    bMemSet(vinyl_layer_infos, 0, sizeof(vinyl_layer_infos));
    cur_layer = first_vinyl_layer;

    do {
        VinylLayerInfo *info = &vinyl_layer_infos[cur_layer];
        unsigned int mask_hash;

        if (cur_layer == first_vinyl_layer) {
            int car_part_id = CARSLOTID_VINYL_LAYER0 + cur_layer;
            CarPart *car_part = ride_info->GetPart(car_part_id);

            if (car_part != 0) {
                info->m_LayerHash = GetVinylLayerHash(ride_info, cur_layer);
                info->m_NumColours = car_part->GetAppliedAttributeIParam(bStringHash("NUMCOLOURS"), 0);
                if (info->m_NumColours == 0) {
                    return 0;
                }
            }
        }

        if (info->m_LayerHash != 0) {
            info->m_LayerTexture = GetTextureInfo(info->m_LayerHash, false, false);
            if (info->m_LayerTexture == 0) {
                info->m_LayerHash = 0;
            } else {
                info->m_LayerImageData = static_cast<unsigned char *>(TextureInfo_LockImage(info->m_LayerTexture, TEXLOCK_READ));
                if (do_32bit_composite == 0) {
                    info->m_LayerImagePaletteData =
                        static_cast<unsigned int *>(TextureInfo_LockPalette(info->m_LayerTexture, TEXLOCK_READ));
                }

                if (info->m_LayerImageData != 0) {
                    if (UsePrecompositeVinyls == 0 && ride_info->SkinType != 2) {
                        mask_hash = bStringHash("_MASK", info->m_LayerHash);
                        info->m_LayerMaskTexture = GetTextureInfo(mask_hash, false, false);
                        if (info->m_LayerMaskTexture == 0) {
                            info->m_LayerHash = 0;
                        } else {
                            info->m_LayerMaskData =
                                static_cast<unsigned char *>(TextureInfo_LockImage(info->m_LayerMaskTexture, TEXLOCK_READ));
                            if (do_32bit_composite == 0) {
                                info->m_LayerMaskPaletteData =
                                    static_cast<unsigned int *>(TextureInfo_LockPalette(info->m_LayerMaskTexture, TEXLOCK_READ));
                            }

                            if (info->m_LayerMaskData != 0) {
                                int next_total_layer_colours = total_layer_colours + 1;

                                if (cur_layer == first_vinyl_layer) {
                                    CarPart *car_part = ride_info->GetPart(CARSLOTID_VINYL_LAYER0 + cur_layer);

                                    if (car_part != 0 && car_part->HasAppliedAttribute(bStringHash("REMAP")) != 0) {
                                        info->m_RemapPalette = car_part->GetAppliedAttributeIParam(bStringHash("REMAP"), 0);
                                        if (info->m_RemapPalette != 0) {
                                            for (int j = 0; j < 4; j++) {
                                                CarPart *colour_part = ride_info->GetPart(CARSLOTID_VINYL_COLOUR0_0 + j);

                                                if (colour_part != 0) {
                                                    unsigned int remap_colour =
                                                        colour_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                                                    int remap_green =
                                                        colour_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                                                    int remap_blue =
                                                        colour_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                                                    int remap_gloss =
                                                        colour_part->GetAppliedAttributeIParam(bStringHash("GLOSS"), 0);

                                                    info->m_RemapColours[j] = (remap_gloss << 24) | (remap_blue << 16) |
                                                                              (remap_green << 8) | remap_colour;
                                                } else {
                                                    info->m_RemapColours[j] = 0xFFu << (j << 3);
                                                }
                                            }
                                        }
                                    }
                                }

                                total_layer_colours = next_total_layer_colours;
                            } else {
                                info->m_LayerHash = 0;
                            }
                        }
                    } else {
                        DumpPreComp(info, dest_texture);
                        return 1;
                    }
                } else {
                    info->m_LayerHash = 0;
                }
            }
        }

        cur_layer++;

        if (cur_layer >= max_layer_colours) {
            success = 1;
            eWaitUntilRenderingDone();
            CompositeRim(ride_info);

            composite_params.BaseColour = base_paint_colour;
            composite_params.NumLayers = total_layer_colours;
            composite_params.DestTexture = dest_texture;
            bMemCpy(composite_params.SwatchColours, swatch_colours, sizeof(swatch_colours));
            bMemCpy(composite_params.VinylLayerInfos, vinyl_layer_infos, sizeof(vinyl_layer_infos));

            if (IsInSkinCompositeCache(&composite_params) == 0) {
                UpdateSkinCompositeCache(&composite_params);
                if (dest_texture->ImageCompressionType == TEXCOMP_8BIT) {
                    success = CompositeSkin(&composite_params);
                } else {
                    success = CompositeSkin32(&composite_params);
                }
            }

            {
                int i = 0;

                do {
                    VinylLayerInfo *info = &vinyl_layer_infos[i];

                    if (info->m_LayerImageData != 0) {
                        TextureInfo_UnlockImage(info->m_LayerTexture, info->m_LayerImageData);
                    }

                    if (info->m_LayerImagePaletteData != 0) {
                        TextureInfo_UnlockPalette(info->m_LayerTexture, info->m_LayerImagePaletteData);
                    }

                    if (info->m_LayerMaskData != 0) {
                        TextureInfo_UnlockImage(info->m_LayerMaskTexture, info->m_LayerMaskData);
                    }

                    if (info->m_LayerMaskPaletteData != 0) {
                        TextureInfo_UnlockPalette(info->m_LayerMaskTexture, info->m_LayerMaskPaletteData);
                    }

                    i++;
                } while (i < max_layer_colours);
            }

            return success;
        }
    } while (true);

    return 1;
}

int DumpPreComp(VinylLayerInfo *info, TextureInfo *dest_texture) {
    void *dest_image_data = TextureInfo_LockImage(dest_texture, TEXLOCK_WRITE);
    int pixel_size = 1;

    if (dest_texture->ImageCompressionType == TEXCOMP_32BIT) {
        pixel_size = 4;
    }

    bMemCpy(dest_image_data, info->m_LayerImageData, dest_texture->Width * dest_texture->Height * pixel_size);

    if (dest_texture->ImageCompressionType == TEXCOMP_8BIT) {
        unsigned int *dest_palette_data = static_cast<unsigned int *>(TextureInfo_LockPalette(dest_texture, TEXLOCK_WRITE));

        eUnSwizzle8bitPalette(dest_palette_data);
        eUnSwizzle8bitPalette(info->m_LayerImagePaletteData);
        bMemCpy(dest_palette_data, info->m_LayerImagePaletteData, info->m_NumColours << 2);
        eSwizzle8bitPalette(info->m_LayerImagePaletteData);
        eSwizzle8bitPalette(dest_palette_data);
        TextureInfo_UnlockPalette(dest_texture, dest_palette_data);
    }

    TextureInfo_UnlockImage(dest_texture, dest_image_data);
    return 1;
}

unsigned int GetWheelTextureHash(RideInfo *ride_info) {
    CarPart *wheel = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

    if (wheel == 0) {
        return 0;
    }

    return bStringHash("_WHEEL", wheel->GetAppliedAttributeUParam(0x10C98090, 0));
}

unsigned int GetWheelTextureMaskHash(RideInfo *ride_info) {
    CarPart *wheel = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

    if (wheel == 0) {
        return 0;
    }

    return bStringHash("_WHEEL_INNER_MASK", wheel->GetAppliedAttributeUParam(0x10C98090, 0));
}

unsigned int GetHoodSpoilerHash(RideInfo *ride_info) {
    return 0;
}

unsigned int GetHoodSpoilerMaskHash(RideInfo *ride_info) {
    return 0;
}

unsigned int GetSpinnerTextureHash(RideInfo *ride_info) {
    CarPart *rim_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

    if (rim_part == 0) {
        return 0;
    }

    return rim_part->GetAppliedAttributeUParam(bStringHash("SPINNER_TEXTURE"), 0);
}

unsigned int GetSpinnerTextureMaskHash(RideInfo *ride_info) {
    CarPart *rim_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);
    unsigned int spinner_hash;

    if (rim_part == 0) {
        return 0;
    }

    spinner_hash = rim_part->GetAppliedAttributeUParam(bStringHash("SPINNER_TEXTURE"), 0);
    if (spinner_hash == 0) {
        return 0;
    }

    return bStringHash("_MASK", spinner_hash);
}

unsigned int GetVinylLayerHash(CarPart *car_part, CarType car_type, int skin_type) {
    CarTypeInfo *car_type_info = GetCarTypeInfo(car_type);
    const char *texture_name = car_part->GetAppliedAttributeString(bStringHash("TEXTURE"), 0);

    if (texture_name == nullptr) {
        return 0;
    }

    char layer_name[64];

    bStrCpy(layer_name, car_type_info->GetBaseModelName());
    if (UsePrecompositeVinyls || skin_type == 2) {
        bStrCat(layer_name, layer_name, "_PRECOM_");
    } else {
        bStrCat(layer_name, layer_name, "_");
    }
    bStrCat(layer_name, layer_name, texture_name);
    return bStringHash(layer_name);
}

unsigned int GetVinylLayerHash(RideInfo *ride_info, int layer) {
    CarPart *vinyl = ride_info->GetPart(CARSLOTID_VINYL_LAYER0 + layer);

    if (vinyl == 0) {
        return 0;
    }

    return GetVinylLayerHash(vinyl, ride_info->Type, ride_info->SkinType);
}

unsigned int GetVinylLayerMaskHash(RideInfo *ride_info, int layer) {
    CarPart *car_part = ride_info->GetPart(CARSLOTID_VINYL_LAYER0 + layer);
    CarType car_type;
    CarTypeInfo *car_type_info;
    const char *texture_name;
    char layer_name[64];

    if (car_part != 0) {
        car_type = ride_info->Type;
        car_type_info = GetCarTypeInfo(car_type);
        texture_name = car_part->GetAppliedAttributeString(bStringHash("TEXTURE"), 0);

        if (texture_name != 0) {
            bStrCpy(layer_name, car_type_info->GetBaseModelName());
            bStrCat(layer_name, layer_name, "_");
            bStrCat(layer_name, layer_name, texture_name);
            bStrCat(layer_name, layer_name, "_MASK");
            return bStringHash(layer_name);
        }
    }

    return 0;
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
            if (!do_32bit_composite) {
                return CompositeWheel8(dest_texture, src_texture, src_mask, wheel_colour);
            }

            return CompositeWheel32(dest_texture, src_texture, src_mask, wheel_colour);
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

int GetTempCarSkinTextures(unsigned int *textures_to_load, int num_textures, int max_textures, RideInfo *ride) {
    for (int car_part_id = CARSLOTID_VINYL_LAYER0; car_part_id < CARSLOTID_PAINT_RIM; car_part_id++) {
        CarPart *part = ride->GetPart(static_cast<CAR_SLOT_ID>(car_part_id));

        if (part != 0) {
            const char *name = part->GetName();
            unsigned int vinyl_hash = GetVinylLayerHash(ride, car_part_id - CARSLOTID_VINYL_LAYER0);
            unsigned int mask_hash = GetVinylLayerMaskHash(ride, car_part_id - CARSLOTID_VINYL_LAYER0);
            int added_vinyls = UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, vinyl_hash);
            int added_masks = UsedCarTextureAddToTable(textures_to_load, num_textures + added_vinyls, max_textures, mask_hash);

            num_textures += added_vinyls + added_masks;
            (void)name;
        }
    }

    {
        unsigned int hood_spoiler_hash = GetHoodSpoilerHash(ride);
        unsigned int hood_spoiler_mask_hash = GetHoodSpoilerMaskHash(ride);

        if (hood_spoiler_hash != 0 && hood_spoiler_mask_hash != 0) {
            int added_spoilers = UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, hood_spoiler_hash);
            int added_masks = UsedCarTextureAddToTable(textures_to_load, num_textures + added_spoilers, max_textures, hood_spoiler_mask_hash);

            num_textures += added_spoilers + added_masks;
        }
    }

    {
        unsigned int wheel_hash = GetWheelTextureHash(ride);
        unsigned int wheel_mask_hash = GetWheelTextureMaskHash(ride);

        if (wheel_hash != 0 && wheel_mask_hash != 0) {
            int added_wheels = UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, wheel_hash);
            int added_masks = UsedCarTextureAddToTable(textures_to_load, num_textures + added_wheels, max_textures, wheel_mask_hash);

            num_textures += added_wheels + added_masks;
        }
    }

    {
        unsigned int spinner_hash = GetSpinnerTextureHash(ride);
        unsigned int spinner_mask_hash = GetSpinnerTextureMaskHash(ride);

        if (spinner_hash != 0 && spinner_mask_hash != 0) {
            int added_spinners = UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, spinner_hash);
            int added_masks = UsedCarTextureAddToTable(textures_to_load, num_textures + added_spinners, max_textures, spinner_mask_hash);

            num_textures += added_spinners + added_masks;
        }
    }

    return num_textures;
}
