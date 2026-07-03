#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/CarSkin.hpp"
#include "Speed/Indep/Src/World/NeuQant.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

int DoPixelClear = 1;
int DoCompositeBlend = 1;

// TODO use these
static const int PrintSkinComposite = 0;
static const int EnableWheelComposite = 1;
static const int EnableSpinnerComposite = 1;
static const int PrintCompositeWheel = 0;
static const int MaxSkinLayers = 0; // TODO value
static const int NumSwatches = 4;   // TODO value

SlotPool *carSkinSlotPool = nullptr;

struct CompColour {
    uint8 b;
    uint8 g;
    uint8 r;
    uint8 a;

    CompColour() {
        *reinterpret_cast<uint32 *>(this) = 0;
    }
};

SkinCompositeParams SkinCompositeParameterCache[4];

SkinCompositeParams *GetSkinCompositeParams(unsigned int dest_name_hash) {
    SkinCompositeParams *cache_params = nullptr;

    switch (dest_name_hash) {
        case STRINGHASH_DUMMY_SKIN1:
            cache_params = &SkinCompositeParameterCache[0];
            break;
        case STRINGHASH_DUMMY_SKIN2:
            cache_params = &SkinCompositeParameterCache[1];
            break;
        case STRINGHASH_DUMMY_SKIN3:
            cache_params = &SkinCompositeParameterCache[2];
            break;
        case STRINGHASH_DUMMY_SKIN4:
            cache_params = &SkinCompositeParameterCache[3];
            break;
    }

    return cache_params;
}

bool CompareCompositeParams(SkinCompositeParams *a, SkinCompositeParams *b) {
    if (a->DestTexture != b->DestTexture || a->BaseColour != b->BaseColour) {
        return false;
    }

    for (int i = 0; i < 4; i++) {
        if (a->SwatchColours[i] != b->SwatchColours[i]) {
            return false;
        }
    }

    for (int i = 0; i < 1; i++) {
        VinylLayerInfo *info_a = &a->VinylLayerInfos[i];
        VinylLayerInfo *info_b = &b->VinylLayerInfos[i];

        if (info_a->m_LayerHash != info_b->m_LayerHash || info_a->m_LayerTexture != info_b->m_LayerTexture ||
            info_a->m_LayerMaskTexture != info_b->m_LayerMaskTexture || info_a->m_NumColours != info_b->m_NumColours ||
            info_a->m_RemapPalette != info_b->m_RemapPalette || info_a->m_RemapColours[0] != info_b->m_RemapColours[0] ||
            info_a->m_RemapColours[1] != info_b->m_RemapColours[1] || info_a->m_RemapColours[2] != info_b->m_RemapColours[2] ||
            info_a->m_RemapColours[3] != info_b->m_RemapColours[3]) {
            return false;
        }
    }

    return true;
}

int IsInSkinCompositeCache(SkinCompositeParams *skin_composite_params) {
    SkinCompositeParams *cache_params = GetSkinCompositeParams(skin_composite_params->DestTexture->NameHash);
    bool match;

    for (int i = 0; i < 0; i++) {
    }

    for (int i = 0; i < 0; i++) {
        VinylLayerInfo *a;
        VinylLayerInfo *b;
    }

    if (cache_params != nullptr) {
        match = CompareCompositeParams(cache_params, skin_composite_params);
        return static_cast<int>(match);
    }

    return 0;
}

void UpdateSkinCompositeCache(SkinCompositeParams *skin_composite_params) {
    SkinCompositeParams *cache_params = GetSkinCompositeParams(skin_composite_params->DestTexture->NameHash);

    if (cache_params != nullptr) {
        bMemCpy(cache_params, skin_composite_params, sizeof(*skin_composite_params));
    }
}

void FlushFromSkinCompositeCache(uint32 texture_name_hash) {
    SkinCompositeParams *cache_params = GetSkinCompositeParams(texture_name_hash);

    if (cache_params != nullptr) {
        bMemSet(cache_params, 0, sizeof(*cache_params));
    }
}

uint32 ScaleColours(uint32 a, uint32 b) {
    CompColour *col_a = reinterpret_cast<CompColour *>(&a);
    CompColour *col_b = reinterpret_cast<CompColour *>(&b);
    CompColour result;

    result.r = static_cast<unsigned char>(static_cast<float>(static_cast<int>(col_a->r)) * (static_cast<float>(static_cast<int>(col_b->r)) / 255.0f));

    result.g = static_cast<unsigned char>(static_cast<float>(static_cast<int>(col_a->g)) * (static_cast<float>(static_cast<int>(col_b->g)) / 255.0f));

    result.b = static_cast<unsigned char>(static_cast<float>(static_cast<int>(col_a->b)) * (static_cast<float>(static_cast<int>(col_b->b)) / 255.0f));

    result.a = static_cast<unsigned char>(static_cast<float>(static_cast<int>(col_a->a)) * (static_cast<float>(static_cast<int>(col_b->a)) / 255.0f));

    return *reinterpret_cast<uint32 *>(&result);
}

uint32 GetBlendColour(uint32 *colours, float *weights, int num_colours, bool max_alpha_blend) {
    CompColour *comp_colours = reinterpret_cast<CompColour *>(colours);
    CompColour final_colour;
    int r, g, b;
    r = g = b = 0;
    int a = 0;

    for (int i = 0; i < num_colours; i++) {
        if (weights[i] > 1 / 255.0f) {
            r += static_cast<int>(weights[i] * static_cast<float>(comp_colours[i].r));
            g += static_cast<int>(weights[i] * static_cast<float>(comp_colours[i].g));
            b += static_cast<int>(weights[i] * static_cast<float>(comp_colours[i].b));

            if (max_alpha_blend) {
                int tempa = static_cast<int>(weights[i] * static_cast<float>(comp_colours[i].a)) & 0xFF;

                if (tempa > a) {
                    a = tempa;
                }
            } else {
                a += static_cast<int>(weights[i] * static_cast<float>(comp_colours[i].a));
            }
        }
    }

    final_colour.r = static_cast<uint8>(r > 255 ? 255 : r);
    final_colour.g = static_cast<uint8>(g > 255 ? 255 : g);
    final_colour.b = static_cast<uint8>(b > 255 ? 255 : b);
    final_colour.a = static_cast<uint8>(a > 255 ? 255 : a);

    return *reinterpret_cast<uint32 *>(&final_colour);
}

unsigned int RemapColour(unsigned int colour, unsigned int *colour_map) {
    CompColour col = *reinterpret_cast<CompColour *>(&colour);
    float weights[4];

    weights[0] = static_cast<float>(static_cast<int>(col.a)) / 255.0f;
    weights[1] = static_cast<float>(static_cast<int>(col.r)) / 255.0f;
    weights[2] = static_cast<float>(static_cast<int>(col.g)) / 255.0f;
    weights[3] = 0.0f;

    unsigned int result = GetBlendColour(colour_map, weights, 3, true);
    return result;
}

int swatch_offset_init = 0;
int swatch_offset_cache[4][16];
int swatch_offset_count[4] = {};

int CompositeSkin32(SkinCompositeParams *composite_params) {
    TextureInfo *dest_texture = composite_params->DestTexture;
    uint32 base_colour = composite_params->BaseColour;
    uint32 *swatch_colours = composite_params->SwatchColours;
    VinylLayerInfo *layer_infos = composite_params->VinylLayerInfos;
    int num_layers = composite_params->NumLayers;
    int debug_print;

    if (dest_texture == nullptr) {
        return 0;
    }

    if (dest_texture->ImageCompressionType != TEXCOMP_32BIT) {
        return 0;
    }

    uint32 *dest_image_data = static_cast<uint32 *>(dest_texture->LockImage(TEXLOCK_WRITE));
    int dest_width = dest_texture->Width;
    int dest_height = dest_texture->Height;

    if (swatch_offset_init == 0) {
        uint32 swatch_lookup_colours[4] = {
            0xBF0000FF,
            0xBF00FF00,
            0xBFFF0000,
            0xBFFF00FF,
        };

        bMemSet(swatch_offset_cache, 0, sizeof(swatch_offset_cache));

        uint32 *dest = dest_image_data;
        uint32 *dest_end = dest_image_data + dest_width * dest_height;

        while (dest < dest_end) {
            for (int i = 0; i < 4; i++) {
                if (*dest == swatch_lookup_colours[i]) {
                    swatch_offset_count[i] = swatch_offset_count[i] + 1;
                    swatch_offset_cache[i][swatch_offset_count[i] - 1] = dest - dest_image_data;
                    break;
                }
            };

            dest++;
        }

        swatch_offset_init = 1;
    }

    CompColour base = *reinterpret_cast<CompColour *>(&base_colour);

    int temp = (base_colour >> 8) & 0xFF;
    base.r = (base_colour >> 24) & 0xFF;
    base.b = temp;

    uint32 *dest_pixel;
    uint32 *end_pixel;
    for (dest_pixel = dest_image_data, end_pixel = dest_image_data + dest_width * dest_height; dest_pixel < end_pixel; dest_pixel++) {
        *dest_pixel = *reinterpret_cast<uint32 *>(&base);
    }

    int num_pixels = dest_width * dest_height;
    for (int i = 0; i < num_layers; i++) {
        VinylLayerInfo *info = &layer_infos[i];

        if (info->m_LayerHash == 0) {
            continue;
        }
        uint32 *dest = dest_image_data;
        uint32 *image_src = reinterpret_cast<uint32 *>(info->m_LayerImageData);
        uint32 *image_end = image_src + num_pixels;
        uint32 *mask_src = reinterpret_cast<uint32 *>(info->m_LayerMaskData);

        for (; image_src < image_end; image_src++, mask_src++, dest++) {
            uint32 src_pixel = *image_src;
            uint32 src_mask;
            uint32 dest_pixel = *dest;
            src_mask = (*mask_src >> 8) & 0xFF;

            if (info->m_RemapPalette != 0) {
                CompColour src_colour = *reinterpret_cast<CompColour *>(&src_pixel);

                if (src_mask != 0) {
                    // TODO remove fake var
                    int fake = (src_pixel >> 8) & 0xFF;
                    src_colour.b = fake;
                    int tmp = src_pixel >> 24;
                    src_colour.r = tmp & 0xFF;

                    src_pixel = RemapColour(*reinterpret_cast<uint32 *>(&src_colour), info->m_RemapColours);
                }
            }

            if (src_mask < 0x80) {
                if (src_mask != 0) {
                    uint32 colours[2];
                    float weights[2];

                    colours[0] = src_pixel;
                    colours[1] = dest_pixel;
                    weights[0] = static_cast<float>(src_mask) / 128.0f;
                    weights[1] = 1.0f - weights[0];

                    if (1.0f < weights[0]) {
                        weights[0] = 1.0f;
                    }

                    if (weights[1] < 0.0f) {
                        weights[1] = 0.0f;
                    }

                    src_pixel = GetBlendColour(colours, weights, 2, false);
                    *dest = src_pixel;
                }
            } else {
                *dest = src_pixel;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < swatch_offset_count[i]; j++) {
            dest_image_data[swatch_offset_cache[i][j]] = swatch_colours[i];
        }
    }

    dest_texture->UnlockImage(dest_image_data);
    return 1;
}

// total size: 0x4
struct SemiTransPixel {
    int16 x; // offset 0x0, size 0x2
    int16 y; // offset 0x2, size 0x2
};

// UNSOLVED
int CompositeSkin(SkinCompositeParams *composite_params) {
    TextureInfo *dest_texture = composite_params->DestTexture;
    uint32 base_colour = composite_params->BaseColour;
    uint32 *swatch_colours = composite_params->SwatchColours;
    VinylLayerInfo *layer_infos = composite_params->VinylLayerInfos;
    int num_layers = composite_params->NumLayers;
    int debug_print;

    if (dest_texture == nullptr) {
        return 0;
    }

    if (dest_texture->ImageCompressionType != TEXCOMP_8BIT) {
        return 0;
    }

    uint8 *dest_image_data = static_cast<uint8 *>(dest_texture->LockImage(TEXLOCK_WRITE));
    uint32 *dest_palette_data = static_cast<uint32 *>(dest_texture->LockPalette(TEXLOCK_WRITE));
    eUnSwizzle8bitPalette(dest_palette_data);

    int dest_width = dest_texture->Width;
    int dest_height = dest_texture->Height;
    int max_semi_trans_pixels = 0;
    max_semi_trans_pixels |= 0xC000;
    SemiTransPixel *semi_trans_pixels = static_cast<SemiTransPixel *>(bMalloc(0x30000, nullptr, 0, (GetVirtualMemoryPoolNumber() & 0xF) | 0x40));
    uint32 *semi_trans_colours = static_cast<unsigned int *>(bMalloc(0x30000, nullptr, 0, (GetVirtualMemoryPoolNumber() & 0xF) | 0x40));
    int semi_trans_pixels_buffer_size;
    int total_malloc_required;
    int cur_semi_trans_pixel = 0;
    int num_pixels = dest_texture->Width * dest_texture->Height;
    uint8 *dest = dest_image_data;
    uint8 *dest_end = dest_image_data + num_pixels;
    uint8 *image_src[1];
    uint8 *mask_src[1];

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
            for (int j = 0; j < 256; j++) {
                if (dest_palette_data[j] == swatch_lookup_colours[i]) {
                    swatch_indices[i] = j;
                    break;
                }
            }
        }

        bMemSet(swatch_offset_cache, 0, sizeof(swatch_offset_cache));

        for (; dest < dest_end; dest++) {
            int i;

            for (i = 0; i < 4; i++) {
                if (static_cast<unsigned int>(*dest) == static_cast<unsigned int>(swatch_indices[i])) {
                    *dest = static_cast<unsigned char>(i + 1);
                    swatch_offset_count[i] = swatch_offset_count[i] + 1;
                    swatch_offset_cache[i][swatch_offset_count[i] - 1] = dest - dest_image_data;
                    break;
                }
            }

            if (i == 4) {
                *dest = 0;
            }
        }

        swatch_offset_init = 1;
        dest = dest_image_data;
    } else {
        bMemSet(dest_image_data, 0, dest_width * dest_height);
    }

    dest_palette_data[0] = base_colour;
    int current_palette_base = 1;

    for (int i = 0; i < 4; i++) {
        dest_palette_data[current_palette_base] = swatch_colours[i];
        current_palette_base++;
    }

    for (; dest < dest_end; dest++) {
        uint32 dest_pixel = dest_palette_data[*dest];

        for (int i = 0; i < num_layers; i++) {
            VinylLayerInfo *info = &layer_infos[i];
            uint32 src_pixel;
            uint32 src_mask;

            if (info->m_LayerHash != 0) {
                src_mask = static_cast<uint8>(reinterpret_cast<unsigned short *>(info->m_LayerMaskPaletteData)[*mask_src[i] * 2]);
                src_pixel = info->m_LayerImagePaletteData[*image_src[i]];

                if (info->m_RemapPalette != 0 && src_mask != 0) {
                    src_pixel = RemapColour(src_pixel, info->m_RemapColours);
                }

                if (src_mask > 0x7F) {
                    *dest = static_cast<unsigned char>(*image_src[i] + current_palette_base);
                    dest_pixel = src_pixel;
                } else if (src_mask != 0) {
                    float weights[2];
                    unsigned int colours[2];

                    weights[0] = static_cast<float>(src_mask) / 128.0f;
                    if (weights[0] > 1.0f) {
                        weights[0] = 1.0f;
                    }

                    weights[1] = 1.0f - weights[0];
                    colours[0] = src_pixel;
                    colours[1] = dest_pixel;
                    unsigned int blend_colour = GetBlendColour(colours, weights, 2, false);
                    semi_trans_colours[cur_semi_trans_pixel] = blend_colour;

                    int x = image_src[i] - info->m_LayerImageData;
                    int y = (image_src[i] - info->m_LayerImageData) / dest_texture->Width;

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
                CompColour colour = *reinterpret_cast<CompColour *>(&info->m_LayerImagePaletteData[j]);
                dest_palette_data[current_palette_base + j] = RemapColour(*reinterpret_cast<unsigned int *>(&colour), info->m_RemapColours);
            }
        }

        eSwizzle8bitPalette(info->m_LayerImagePaletteData);
        eSwizzle8bitPalette(info->m_LayerMaskPaletteData);
        current_palette_base += info->m_NumColours;
    }

    if (cur_semi_trans_pixel != 0) {
        int max_remap_colours = 0xFF - current_palette_base;
        SemiTransPixel *pixel_list;
        int num_pixels = cur_semi_trans_pixel;

        if (bLargestMalloc(0) < (num_pixels << 2)) {
            num_pixels = 0;
        }

        uint8 *input = static_cast<uint8 *>(bMalloc(num_pixels << 2, nullptr, 0, 0x40));
        int p = 0;

        for (int i = 0; i < num_pixels; i++) {
            unsigned int pixel_colour = semi_trans_colours[i];

            input[p * 4] = static_cast<unsigned char>(pixel_colour);
            input[p * 4 + 1] = static_cast<unsigned char>(pixel_colour >> 8);
            input[p * 4 + 2] = static_cast<unsigned char>(pixel_colour >> 16);
            input[p * 4 + 3] = static_cast<unsigned char>(pixel_colour >> 24);
            p++;
        }

        if (num_pixels < max_remap_colours) {
            for (int i = 0; i < num_pixels; i++) {
                SemiTransPixel *pixel = &semi_trans_pixels[i];
                unsigned char *dest = &dest_image_data[pixel->x + pixel->y * dest_texture->Width];

                *dest = static_cast<unsigned char>(i + current_palette_base);
                dest_palette_data[i + current_palette_base] = semi_trans_colours[i];
            }
        } else {
            initnet(input, num_pixels << 2, max_remap_colours, 0x14);
            learn();
            unbiasnet();

            uint8 r, g, b, a;

            for (int i = 0; i < max_remap_colours; i++) {
                nqGetPaletteEntry(i, r, g, b, a);
                dest_palette_data[current_palette_base + i] =
                    static_cast<uint32>(b) | (static_cast<uint32>(g) << 8) | (static_cast<uint32>(r) << 16) | (static_cast<uint32>(a) << 24);
            }

            inxbuild();

            for (int i = 0; i < num_pixels; i++) {
                SemiTransPixel *pixel = &semi_trans_pixels[i];
                uint8 *dest = &dest_image_data[pixel->x + pixel->y * dest_texture->Width];

                if (*dest == 0xFF) {
                    uint32 colour = semi_trans_colours[i];
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
            dest_image_data[swatch_offset_cache[i][j]] = static_cast<uint8>(i + 1);
        }
    }

    eSwizzle8bitPalette(dest_palette_data);
    dest_texture->UnlockImage(dest_image_data);
    dest_texture->UnlockPalette(dest_palette_data);
    return 1;
}

int DumpPreComp(VinylLayerInfo *info, TextureInfo *dest_texture) {
    uint8 *dest_image_data = static_cast<unsigned char *>(dest_texture->LockImage(TEXLOCK_WRITE));
    uint8 *image_src = info->m_LayerImageData;
    int dest_width = dest_texture->Width;
    int dest_height = dest_texture->Height;
    int bpp = 1;

    if (dest_texture->ImageCompressionType == TEXCOMP_32BIT) {
        bpp = 4;
    }

    bMemCpy(dest_image_data, image_src, dest_width * dest_height * bpp);

    if (dest_texture->ImageCompressionType == TEXCOMP_8BIT) {
        uint32 *dest_palette_data = static_cast<unsigned int *>(dest_texture->LockPalette(TEXLOCK_WRITE));

        eUnSwizzle8bitPalette(dest_palette_data);
        eUnSwizzle8bitPalette(info->m_LayerImagePaletteData);
        bMemCpy(dest_palette_data, info->m_LayerImagePaletteData, info->m_NumColours << 2);
        eSwizzle8bitPalette(info->m_LayerImagePaletteData);
        eSwizzle8bitPalette(dest_palette_data);
        dest_texture->UnlockPalette(dest_palette_data);
    }

    dest_texture->UnlockImage(dest_image_data);
    return 1;
}

uint32 GetHoodSpoilerHash(RideInfo *ride_info) {
    return 0;
}

uint32 GetHoodSpoilerMaskHash(RideInfo *ride_info) {
    return 0;
}

uint32 GetWheelTextureHash(RideInfo *ride_info) {
    CarPart *rim_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

    if (rim_part == nullptr) {
        return 0;
    }

    return bStringHash("_WHEEL", rim_part->GetTextureNameHash());
}

unsigned int GetWheelTextureMaskHash(RideInfo *ride_info) {
    CarPart *rim_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

    if (rim_part == nullptr) {
        return 0;
    }

    return bStringHash("_WHEEL_INNER_MASK", rim_part->GetTextureNameHash());
}

unsigned int GetSpinnerTextureHash(RideInfo *ride_info) {
    CarPart *rim_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

    if (rim_part == nullptr) {
        return 0;
    }

    return rim_part->GetAppliedAttributeUParam(bStringHash("SPINNER_TEXTURE"), 0);
}

unsigned int GetSpinnerTextureMaskHash(RideInfo *ride_info) {
    CarPart *rim_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

    if (rim_part == nullptr) {
        return 0;
    }

    uint32 spinner_hash = rim_part->GetAppliedAttributeUParam(bStringHash("SPINNER_TEXTURE"), 0);
    if (spinner_hash == 0) {
        return 0;
    }

    return bStringHash("_MASK", spinner_hash);
}

uint32 GetVinylLayerHash(RideInfo *ride_info, int layer) {
    CarPart *car_part = ride_info->GetPart(CARSLOTID_VINYL_LAYER0 + layer);

    if (car_part == nullptr) {
        return 0;
    }

    return GetVinylLayerHash(car_part, ride_info->Type, ride_info->SkinType);
}

uint32 GetVinylLayerHash(CarPart *car_part, CarType car_type, int skin_type) {
    CarTypeInfo *car_type_info = GetCarTypeInfo(car_type);
    const char *texture_name = car_part->GetAppliedAttributeString(bStringHash("TEXTURE"), nullptr);

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

uint32 GetVinylLayerMaskHash(RideInfo *ride_info, int layer) {
    CarPart *car_part = ride_info->GetPart(CARSLOTID_VINYL_LAYER0 + layer);

    if (car_part == nullptr) {
        return 0;
    }

    CarType car_type = ride_info->Type;
    CarTypeInfo *car_type_info = GetCarTypeInfo(car_type);
    const char *texture_name = car_part->GetAppliedAttributeString(bStringHash("TEXTURE"), nullptr);

    if (texture_name == nullptr) {
        return 0;
    }

    char layer_name[64];
    bStrCpy(layer_name, car_type_info->GetBaseModelName());
    bStrCat(layer_name, layer_name, "_");
    bStrCat(layer_name, layer_name, texture_name);
    bStrCat(layer_name, layer_name, "_MASK");
    return bStringHash(layer_name);
}

int CompositeRim(RideInfo *ride_info);

// UNSOLVED branching issues
int CompositeSkin(RideInfo *ride_info) {
    if (ride_info->IsUsingCompositeSkin() == 0) {
        return 1;
    }

    unsigned int dest_namehash = ride_info->GetCompositeSkinNameHash();
    TextureInfo *dest_texture = GetTextureInfo(dest_namehash, FALSE, FALSE);
    if (dest_texture == nullptr) {
        return 1;
    }

    int do_32bit_composite = static_cast<int>(dest_texture->ImageCompressionType == TEXCOMP_32BIT);
    const int first_vinyl_layer = 0;
    CarPart *base_paint_part = ride_info->GetPart(CARSLOTID_BASE_PAINT);
    if (base_paint_part == nullptr) {
        return 1;
    }

    unsigned int base_paint_colour;
    int red = base_paint_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
    int green = base_paint_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
    int blue = base_paint_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
    int gloss = base_paint_part->GetAppliedAttributeIParam(bStringHash("GLOSS"), 0);
    base_paint_colour = (gloss << 24) | (blue << 16) | (green << 8) | red;

    unsigned int swatch_colours[4];
    for (int i = 0; i < 4; i++) {
        swatch_colours[i] = base_paint_colour;
    }

    VinylLayerInfo vinyl_layer_infos[1];
    int total_layer_colours = 0;
    bMemSet(vinyl_layer_infos, 0, sizeof(vinyl_layer_infos));
    const int max_layer_colours = 1;
    int cur_layer = first_vinyl_layer;

    for (int i = first_vinyl_layer; i < max_layer_colours; i++) {
        VinylLayerInfo *info = &vinyl_layer_infos[cur_layer];

        if (i == first_vinyl_layer) {
            int car_part_id = CARSLOTID_VINYL_LAYER0;
            CarPart *car_part = ride_info->GetPart(car_part_id);

            if (car_part != nullptr) {
                info->m_LayerHash = GetVinylLayerHash(ride_info, first_vinyl_layer);
                info->m_NumColours = car_part->GetAppliedAttributeIParam(bStringHash("NUMCOLOURS"), 0);
                if (info->m_NumColours == 0) {
                    return 0;
                }
            }
        }

        if (info->m_LayerHash == 0) {
            continue;
        }
        info->m_LayerTexture = GetTextureInfo(info->m_LayerHash, FALSE, FALSE);
        if (info->m_LayerTexture == nullptr) {
            info->m_LayerHash = 0;
            continue;
        }
        info->m_LayerImageData = static_cast<unsigned char *>(info->m_LayerTexture->LockImage(TEXLOCK_READ));
        if (do_32bit_composite == 0) {
            info->m_LayerImagePaletteData = static_cast<unsigned int *>(info->m_LayerTexture->LockPalette(TEXLOCK_READ));
        }

        if (info->m_LayerImageData == nullptr) {
            info->m_LayerHash = 0;
            continue;
        }
        unsigned int mask_hash;
        if (UsePrecompositeVinyls == 0 && ride_info->SkinType != 2) {
            mask_hash = bStringHash("_MASK", info->m_LayerHash);
            info->m_LayerMaskTexture = GetTextureInfo(mask_hash, FALSE, FALSE);
            if (info->m_LayerMaskTexture == nullptr) {
                info->m_LayerHash = 0;
                continue;
            }
            info->m_LayerMaskData = static_cast<unsigned char *>(info->m_LayerMaskTexture->LockImage(TEXLOCK_READ));
            if (do_32bit_composite == 0) {
                info->m_LayerMaskPaletteData = static_cast<unsigned int *>(info->m_LayerMaskTexture->LockPalette(TEXLOCK_READ));
            }

            if (info->m_LayerMaskData == nullptr) {
                info->m_LayerHash = 0;
                continue;
            }
            total_layer_colours = cur_layer + 1;

            if (i == first_vinyl_layer) {
                CarPart *car_part = ride_info->GetPart(CARSLOTID_VINYL_LAYER0);

                if (car_part != nullptr && car_part->HasAppliedAttribute(bStringHash("REMAP")) != 0) {
                    info->m_RemapPalette = car_part->GetAppliedAttributeIParam(bStringHash("REMAP"), 0);
                    if (info->m_RemapPalette != 0) {
                        int layer_id = CARSLOTID_VINYL_COLOUR0_0;

                        for (int j = 0; j < 4; j++) {
                            CarPart *colour_part = ride_info->GetPart(layer_id + j);

                            if (colour_part != nullptr) {
                                int red = colour_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                                int green = colour_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                                int blue = colour_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                                int gloss = colour_part->GetAppliedAttributeIParam(bStringHash("GLOSS"), 0);

                                info->m_RemapColours[j] = (gloss << 24) | (blue << 16) | (green << 8) | red;
                            } else {
                                info->m_RemapColours[j] = 0xFFu << (j << 3);
                            }
                        }
                    }
                }
            }

            cur_layer = total_layer_colours;
        } else {
            DumpPreComp(info, dest_texture);
            return 1;
        }
    }

    eWaitUntilRenderingDone();
    CompositeRim(ride_info);

    SkinCompositeParams composite_params;
    composite_params.BaseColour = base_paint_colour;
    composite_params.NumLayers = cur_layer;
    composite_params.DestTexture = dest_texture;
    bMemCpy(composite_params.SwatchColours, swatch_colours, sizeof(swatch_colours));
    bMemCpy(composite_params.VinylLayerInfos, vinyl_layer_infos, sizeof(vinyl_layer_infos));

    int success = 1;
    if (IsInSkinCompositeCache(&composite_params) == 0) {
        UpdateSkinCompositeCache(&composite_params);
        if (dest_texture->ImageCompressionType == TEXCOMP_8BIT) {
            success = CompositeSkin(&composite_params);
        } else {
            success = CompositeSkin32(&composite_params);
        }
    }

    for (int i = 0; i < max_layer_colours; i++) {
        VinylLayerInfo *info = &vinyl_layer_infos[i];

        if (info->m_LayerImageData != nullptr) {
            info->m_LayerTexture->UnlockImage(info->m_LayerImageData);
        }

        if (info->m_LayerImagePaletteData != nullptr) {
            info->m_LayerTexture->UnlockPalette(info->m_LayerImagePaletteData);
        }

        if (info->m_LayerMaskData != nullptr) {
            info->m_LayerMaskTexture->UnlockImage(info->m_LayerMaskData);
        }

        if (info->m_LayerMaskPaletteData != nullptr) {
            info->m_LayerMaskTexture->UnlockPalette(info->m_LayerMaskPaletteData);
        }
    }

    return success;
}

int GetTempCarSkinTextures(unsigned int *textures_to_load, int num_textures, int max_textures, RideInfo *ride) {
    CarType car_type = ride->Type;
    CarTypeInfo *car_type_info = GetCarTypeInfo(car_type);

    for (int car_part_id = CARSLOTID_VINYL_LAYER0; car_part_id < CARSLOTID_PAINT_RIM; car_part_id++) {
        CarPart *part = ride->GetPart(static_cast<CAR_SLOT_ID>(car_part_id));
        const char *name;
        unsigned int vinyl_hash;
        unsigned int mask_hash;

        if (part != nullptr) {
            name = part->GetName();
            vinyl_hash = GetVinylLayerHash(ride, car_part_id - CARSLOTID_VINYL_LAYER0);
            mask_hash = GetVinylLayerMaskHash(ride, car_part_id - CARSLOTID_VINYL_LAYER0);

            num_textures += UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, vinyl_hash);
            num_textures += UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, mask_hash);
        }
    }

    unsigned int hood_spoiler_hash = GetHoodSpoilerHash(ride);
    unsigned int hood_spoiler_mask_hash = GetHoodSpoilerMaskHash(ride);

    if (hood_spoiler_hash != 0 && hood_spoiler_mask_hash != 0) {
        num_textures += UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, hood_spoiler_hash);
        num_textures += UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, hood_spoiler_mask_hash);
    }

    unsigned int wheel_hash = GetWheelTextureHash(ride);
    unsigned int wheel_mask_hash = GetWheelTextureMaskHash(ride);

    if (wheel_hash != 0 && wheel_mask_hash != 0) {
        num_textures += UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, wheel_hash);
        num_textures += UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, wheel_mask_hash);
    }

    unsigned int spinner_hash = GetSpinnerTextureHash(ride);
    unsigned int spinner_mask_hash = GetSpinnerTextureMaskHash(ride);

    if (spinner_hash != 0 && spinner_mask_hash != 0) {
        num_textures += UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, spinner_hash);
        num_textures += UsedCarTextureAddToTable(textures_to_load, num_textures, max_textures, spinner_mask_hash);
    }

    return num_textures;
}

int CompositeWheel8(TextureInfo *dest_texture, TextureInfo *src_texture, TextureInfo *src_mask, uint32 remap_colour) {
    uint8 *dest_image_data = static_cast<uint8 *>(dest_texture->LockImage(TEXLOCK_WRITE));
    uint32 *dest_palette_data = static_cast<uint32 *>(dest_texture->LockPalette(TEXLOCK_WRITE));
    uint8 *src_image_data = static_cast<uint8 *>(src_texture->LockImage(TEXLOCK_READ));
    uint32 *src_palette_data = static_cast<uint32 *>(src_texture->LockPalette(TEXLOCK_READ));
    uint8 *src_mask_data = static_cast<uint8 *>(src_mask->LockImage(TEXLOCK_READ));
    uint32 *src_mask_palette_data = static_cast<uint32 *>(src_mask->LockPalette(TEXLOCK_READ));

    eUnSwizzle8bitPalette(dest_palette_data);
    eUnSwizzle8bitPalette(src_palette_data);
    eUnSwizzle8bitPalette(src_mask_palette_data);

    for (int i = 0; i < 16; i++) {
        float blend = static_cast<float>(i) / 15.0f;

        for (int j = 0; j < 16; j++) {
            unsigned int blend_colours[2];
            blend_colours[0] = src_palette_data[j];
            blend_colours[1] = ScaleColours(src_palette_data[j], remap_colour);

            float weights[2];
            weights[0] = 1.0f - blend;
            weights[1] = blend;

            dest_palette_data[i * 16 + j] = GetBlendColour(blend_colours, weights, 2, false) & 0xFFFFFF;
            dest_palette_data[i * 16 + j] |= blend_colours[0] & 0xFF000000;
        }
    }

    eUnSwizzle8bitPalette(dest_palette_data);
    eUnSwizzle8bitPalette(src_palette_data);
    dest_texture->UnlockPalette(dest_palette_data);
    src_texture->UnlockPalette(src_palette_data);

    int num_pixels = dest_texture->Width * dest_texture->Height;
    uint8 *dest_pixel = dest_image_data;
    uint8 *end_pixel = dest_image_data + num_pixels;
    uint8 *src_pixel = src_image_data;
    uint8 *src_mask_pixel = src_mask_data;

    for (; dest_pixel < end_pixel; dest_pixel++) {
        uint32 mask;
        unsigned int palette_offset = *src_mask_pixel << 2;

        mask = *reinterpret_cast<uint32 *>(reinterpret_cast<uint8 *>(src_mask_palette_data) + palette_offset);
        *dest_pixel = *src_pixel + (mask & 0xF0);
        src_pixel++;
        src_mask_pixel++;
    }

    eSwizzle8bitPalette(src_mask_palette_data);
    src_mask->UnlockPalette(src_mask_palette_data);
    src_mask->UnlockImage(src_mask_data);
    src_texture->UnlockImage(src_image_data);
    dest_texture->UnlockImage(dest_image_data);

    return 1;
}

int CompositeWheel32(TextureInfo *dest_texture, TextureInfo *src_texture, TextureInfo *src_mask, unsigned int remap_colour) {
    uint32 *dest_image_data = static_cast<unsigned int *>(dest_texture->LockImage(TEXLOCK_WRITE));
    uint32 *src_image_data = static_cast<unsigned int *>(src_texture->LockImage(TEXLOCK_READ));
    uint32 *src_mask_data = static_cast<unsigned int *>(src_mask->LockImage(TEXLOCK_READ));
    int num_pixels = dest_texture->Width * dest_texture->Height;
    uint32 *dest_pixel = dest_image_data;
    uint32 *end_pixel = dest_image_data + num_pixels;
    uint32 *src_pixel = src_image_data;
    uint32 *src_mask_pixel = src_mask_data;
    int pixel_count;

    while (dest_pixel < end_pixel) {
        uint32 src_maskI = reinterpret_cast<unsigned char *>(src_mask_pixel)[2];

        {
            float blend = static_cast<float>(src_maskI) / 255.0f;

            src_mask_pixel++;
            *dest_pixel = *src_pixel;
            uint32 colour_pixel = ScaleColours(*src_pixel, remap_colour);

            uint32 blend_colours[2];
            blend_colours[0] = *src_pixel;
            blend_colours[1] = colour_pixel;

            float weights[2];
            weights[0] = 1.0f - blend;
            weights[1] = blend;
            src_pixel++;
            *dest_pixel = colour_pixel;

            colour_pixel = GetBlendColour(blend_colours, weights, 2, false);
            *dest_pixel = colour_pixel & 0xFFFFFF;
            *dest_pixel = (colour_pixel & 0xFFFFFF) | (blend_colours[0] & 0xFF000000);
        }

        dest_pixel++;
    }

    src_mask->UnlockImage(src_mask_data);
    src_texture->UnlockImage(src_image_data);
    dest_texture->UnlockImage(dest_image_data);

    return 1;
}

int CompositeWheel(RideInfo *ride_info, unsigned int dest_namehash, unsigned int src_namehash, unsigned int mask_namehash, CAR_SLOT_ID paint_slot) {
    TextureInfo *dest_texture = GetTextureInfo(dest_namehash, FALSE, FALSE);

    if (dest_texture == nullptr) {
        return 1;
    }

    int do_32bit_composite = static_cast<int>(dest_texture->ImageCompressionType == TEXCOMP_32BIT);
    TextureInfo *src_texture = GetTextureInfo(src_namehash, FALSE, FALSE);
    TextureInfo *src_mask = GetTextureInfo(mask_namehash, FALSE, FALSE);

    if (src_texture == nullptr || src_mask == nullptr) {
        return 0;
    }
    CarPart *car_part = ride_info->GetPart(paint_slot);
    uint32 wheel_colour = 0xFFFFFFFF;

    if (car_part != nullptr) {
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

    if (dest_texture->Width != src_texture->Width || dest_texture->Width != src_mask->Width || dest_texture->Height != src_texture->Height ||
        dest_texture->Height != src_mask->Height) {
        return 0;
    }

    if (do_32bit_composite != 0) {
        return CompositeWheel8(dest_texture, src_texture, src_mask, wheel_colour);
    }

    return CompositeWheel32(dest_texture, src_texture, src_mask, wheel_colour);
}

int CompositeRim(RideInfo *ride_info) {
    uint32 dest_namehash = ride_info->GetCompositeWheelNameHash();
    uint32 src_namehash = GetWheelTextureHash(ride_info);
    uint32 mask_namehash = GetWheelTextureMaskHash(ride_info);

    return CompositeWheel(ride_info, dest_namehash, src_namehash, mask_namehash, CARSLOTID_PAINT_RIM);
}
