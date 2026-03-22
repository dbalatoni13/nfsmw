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
unsigned int GetWheelTextureHash(RideInfo *ride_info);
unsigned int GetWheelTextureMaskHash(RideInfo *ride_info);
unsigned int GetVinylLayerHash(RideInfo *ride_info, int layer);
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

int IsInSkinCompositeCache(SkinCompositeParams *skin_composite_params) {
    SkinCompositeParams *cache_params = GetSkinCompositeParams(skin_composite_params->DestTexture->NameHash);

    if (cache_params != 0 && cache_params->DestTexture != 0) {
        return CompareCompositeParams(cache_params, skin_composite_params);
    }

    return 0;
}

int CompositeSkin32(SkinCompositeParams *composite_params) {
    TextureInfo *dest_texture = composite_params->DestTexture;
    int num_layers = composite_params->NumLayers;
    unsigned int base_colour = composite_params->BaseColour;

    if (dest_texture == 0) {
        return 0;
    }

    if (dest_texture->ImageCompressionType == TEXCOMP_32BIT) {
        unsigned int *dest_image_data = static_cast<unsigned int *>(TextureInfo_LockImage(dest_texture, TEXLOCK_WRITE));
        short dest_width = dest_texture->Width;
        short dest_height = dest_texture->Height;
        int num_pixels = dest_width * dest_height;

        if (swatch_offset_init == 0) {
            unsigned int swatch_lookup_colours[4];
            unsigned int *dest_pixel = dest_image_data;

            swatch_lookup_colours[0] = 0xBF0000FF;
            swatch_lookup_colours[1] = 0xBF00FF00;
            swatch_lookup_colours[2] = 0xBFFF0000;
            swatch_lookup_colours[3] = 0xBFFF00FF;
            bMemSet(swatch_offset_cache, 0, sizeof(swatch_offset_cache));

            while (dest_pixel < dest_image_data + num_pixels) {
                int pixel_offset = dest_pixel - dest_image_data;
                int i = 0;

                do {
                    if (*dest_pixel == swatch_lookup_colours[i]) {
                        int count = swatch_offset_count[i];

                        swatch_offset_count[i] = count + 1;
                        swatch_offset_cache[count + i * 16] = pixel_offset;
                        break;
                    }

                    i++;
                } while (i < 4);

                dest_pixel++;
            }

            swatch_offset_init = 1;
        }

        {
            unsigned int *dest_pixel = dest_image_data;

            for (; dest_pixel < dest_image_data + num_pixels; dest_pixel++) {
                *dest_pixel = base_colour;
            }
        }

        for (int i = 0; i < num_layers; i++) {
            VinylLayerInfo *info = &composite_params->VinylLayerInfos[i];

            if (info->m_LayerMaskData != 0) {
                unsigned int *src_pixel = reinterpret_cast<unsigned int *>(info->m_LayerImageData);
                unsigned int *dest_pixel = dest_image_data;
                unsigned int *src_mask_pixel = reinterpret_cast<unsigned int *>(info->m_LayerMaskData);
                unsigned int *src_end = src_pixel + num_pixels;

                for (; src_pixel < src_end; src_pixel++, src_mask_pixel++, dest_pixel++) {
                    unsigned int src_colour = *src_pixel;
                    unsigned int dest_colour = *dest_pixel;
                    unsigned int blend_value = reinterpret_cast<unsigned char *>(src_mask_pixel)[2];

                    if (info->m_RemapPalette != 0 && blend_value != 0) {
                        src_colour = RemapColour(src_colour, info->m_RemapColours);
                    }

                    if (blend_value < 0x80) {
                        if (blend_value != 0) {
                            unsigned int blend_colours[2];
                            float weights[2];
                            float blend = static_cast<float>(blend_value) / 255.0f;

                            if (blend > 1.0f) {
                                blend = 1.0f;
                            }

                            weights[0] = blend;
                            weights[1] = 1.0f - blend;

                            if (weights[1] < 0.0f) {
                                weights[1] = 0.0f;
                            }

                            blend_colours[0] = src_colour;
                            blend_colours[1] = dest_colour;
                            src_colour = GetBlendColour(blend_colours, weights, 2, false);
                            *dest_pixel = src_colour;
                        }
                    } else {
                        *dest_pixel = src_colour;
                    }
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < swatch_offset_count[i]; j++) {
                dest_image_data[swatch_offset_cache[j + i * 16]] = composite_params->SwatchColours[i];
            }
        }

        TextureInfo_UnlockImage(dest_texture, dest_image_data);
        return 1;
    }

    return 0;
}

int CompositeSkin(SkinCompositeParams *composite_params) {
    struct SemiTransPixel {
        short x;
        short y;
    };

    TextureInfo *dest_texture = composite_params->DestTexture;
    unsigned int base_colour = composite_params->BaseColour;
    int num_layers = composite_params->NumLayers;

    if (dest_texture == 0) {
        return 0;
    }

    if (dest_texture->ImageCompressionType == TEXCOMP_8BIT) {
        unsigned char *dest_image_data = static_cast<unsigned char *>(TextureInfo_LockImage(dest_texture, TEXLOCK_WRITE));
        unsigned int *dest_palette_data = static_cast<unsigned int *>(TextureInfo_LockPalette(dest_texture, TEXLOCK_WRITE));
        short dest_width = dest_texture->Width;
        short dest_height = dest_texture->Height;
        int allocation_params = (GetVirtualMemoryPoolNumber() & 0xF) | 0x40;
        SemiTransPixel *semi_trans_pixels = static_cast<SemiTransPixel *>(bMalloc(0x30000, allocation_params));
        unsigned int *semi_trans_colours = static_cast<unsigned int *>(bMalloc(0x30000, allocation_params));
        unsigned char *dest_end = dest_image_data + dest_width * dest_height;
        unsigned char *image_src[1];
        unsigned char *mask_src[1];
        int max_semi_trans_pixels = 0xC000;
        int cur_semi_trans_pixel = 0;
        int current_palette_base;

        eUnSwizzle8bitPalette(dest_palette_data);

        for (int i = 0; i < num_layers; i++) {
            VinylLayerInfo *info = &composite_params->VinylLayerInfos[i];

            if (info->m_LayerHash != 0) {
                eUnSwizzle8bitPalette(info->m_LayerImagePaletteData);
                eUnSwizzle8bitPalette(info->m_LayerMaskPaletteData);
                image_src[i] = info->m_LayerImageData;
                mask_src[i] = info->m_LayerMaskData;
            }
        }

        if (swatch_offset_init == 0) {
            unsigned int swatch_lookup_colours[4];
            int swatch_indices[4];

            swatch_lookup_colours[0] = 0xA00000F0;
            swatch_lookup_colours[1] = 0xA000F000;
            swatch_lookup_colours[2] = 0xA0F00000;
            swatch_lookup_colours[3] = 0xA0F000F0;
            swatch_indices[0] = -1;
            swatch_indices[1] = -1;
            swatch_indices[2] = -1;
            swatch_indices[3] = -1;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 0x100; j++) {
                    if (dest_palette_data[j] == swatch_lookup_colours[i]) {
                        swatch_indices[i] = j;
                        break;
                    }
                }
            }

            bMemSet(swatch_offset_cache, 0, sizeof(swatch_offset_cache));

            for (unsigned char *dest = dest_image_data; dest < dest_end; dest++) {
                int i = 0;

                do {
                    if (static_cast<unsigned int>(*dest) == static_cast<unsigned int>(swatch_indices[i])) {
                        int count = swatch_offset_count[i];

                        *dest = static_cast<unsigned char>(i + 1);
                        swatch_offset_count[i] = count + 1;
                        swatch_offset_cache[count + i * 16] = dest - dest_image_data;
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
            dest_palette_data[current_palette_base] = composite_params->SwatchColours[i];
            current_palette_base++;
        }

        for (unsigned char *dest = dest_image_data; dest < dest_end; dest++) {
            unsigned int dest_colour = dest_palette_data[*dest];

            for (int i = 0; i < num_layers; i++) {
                VinylLayerInfo *info = &composite_params->VinylLayerInfos[i];

                if (info->m_LayerHash != 0) {
                    unsigned int mask_colour = info->m_LayerMaskPaletteData[*mask_src[i]];
                    unsigned int blend_value = mask_colour & 0xFF;
                    unsigned int src_colour = info->m_LayerImagePaletteData[*image_src[i]];

                    if (info->m_RemapPalette != 0 && blend_value != 0) {
                        src_colour = RemapColour(src_colour, info->m_RemapColours);
                    }

                    if (blend_value < 0x80) {
                        if (blend_value != 0) {
                            unsigned int blend_colours[2];
                            float weights[2];
                            float blend = static_cast<float>(blend_value) / 255.0f;
                            int next_semi_trans_pixel = cur_semi_trans_pixel + 1;
                            int pixel_offset = dest - dest_image_data;

                            if (blend > 1.0f) {
                                blend = 1.0f;
                            }

                            weights[0] = blend;
                            weights[1] = 1.0f - blend;
                            blend_colours[0] = src_colour;
                            blend_colours[1] = dest_colour;
                            semi_trans_colours[cur_semi_trans_pixel] = GetBlendColour(blend_colours, weights, 2, false);
                            semi_trans_pixels[cur_semi_trans_pixel].x = pixel_offset - (pixel_offset / dest_width) * dest_width;
                            semi_trans_pixels[cur_semi_trans_pixel].y = pixel_offset / dest_width;

                            if (max_semi_trans_pixels <= next_semi_trans_pixel) {
                                next_semi_trans_pixel = cur_semi_trans_pixel;
                            }

                            *dest = 0xFF;
                            cur_semi_trans_pixel = next_semi_trans_pixel;
                        }
                    } else {
                        *dest = static_cast<unsigned char>(*image_src[i] + current_palette_base);
                        dest_colour = src_colour;
                    }

                    image_src[i]++;
                    mask_src[i]++;
                }
            }
        }

        for (int i = 0; i < num_layers; i++) {
            VinylLayerInfo *info = &composite_params->VinylLayerInfos[i];

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
            int remaining_palette_slots = 0xFF - current_palette_base;
            unsigned char *quantized_colours;

            if (bLargestMalloc(0) < (cur_semi_trans_pixel << 2)) {
                cur_semi_trans_pixel = 0;
            }

            quantized_colours = static_cast<unsigned char *>(bMalloc(cur_semi_trans_pixel << 2, 0x40));

            for (int i = 0; i < cur_semi_trans_pixel; i++) {
                unsigned int colour = semi_trans_colours[i];

                quantized_colours[i * 4] = static_cast<unsigned char>(colour);
                quantized_colours[i * 4 + 3] = static_cast<unsigned char>(colour >> 24);
                quantized_colours[i * 4 + 1] = static_cast<unsigned char>(colour >> 8);
                quantized_colours[i * 4 + 2] = static_cast<unsigned char>(colour >> 16);
            }

            if (cur_semi_trans_pixel < remaining_palette_slots) {
                for (int i = 0; i < cur_semi_trans_pixel; i++) {
                    int palette_index = current_palette_base + i;

                    dest_image_data[semi_trans_pixels[i].x + semi_trans_pixels[i].y * dest_width] =
                        static_cast<unsigned char>(palette_index);
                    dest_palette_data[palette_index] = semi_trans_colours[i];
                }
            } else {
                initnet(quantized_colours, cur_semi_trans_pixel << 2, remaining_palette_slots, 0x14);
                learn();
                unbiasnet();

                for (int i = 0; i < remaining_palette_slots; i++) {
                    unsigned char r;
                    unsigned char g;
                    unsigned char b;
                    unsigned char a;

                    nqGetPaletteEntry(i, r, g, b, a);
                    dest_palette_data[current_palette_base + i] =
                        (static_cast<unsigned int>(a) << 24) | (static_cast<unsigned int>(b) << 16) |
                        (static_cast<unsigned int>(g) << 8) | r;
                }

                inxbuild();

                for (int i = 0; i < cur_semi_trans_pixel; i++) {
                    int offset = semi_trans_pixels[i].x + semi_trans_pixels[i].y * dest_width;

                    if (dest_image_data[offset] == 0xFF) {
                        unsigned int colour = semi_trans_colours[i];
                        int palette_index = inxsearch(colour & 0xFF, (colour >> 8) & 0xFF, (colour >> 16) & 0xFF,
                                                      colour >> 24);

                        if (palette_index < remaining_palette_slots) {
                            dest_image_data[offset] = static_cast<unsigned char>(palette_index + current_palette_base);
                        } else {
                            dest_image_data[offset] = 0;
                        }
                    }
                }
            }

            bFree(quantized_colours);
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
    if (ride_info->IsUsingCompositeSkin() != 0) {
        TextureInfo *dest_texture = GetTextureInfo(ride_info->GetCompositeSkinNameHash(), false, false);

        if (dest_texture != 0) {
            bool use_palette = dest_texture->ImageCompressionType != TEXCOMP_32BIT;
            CarPart *base_paint = ride_info->GetPart(CARSLOTID_BASE_PAINT);

            if (base_paint != 0) {
                unsigned int base_colour = base_paint->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                int green = base_paint->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                int blue = base_paint->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                int gloss = base_paint->GetAppliedAttributeIParam(bStringHash("GLOSS"), 0);
                SkinCompositeParams composite_params;
                VinylLayerInfo *info = &composite_params.VinylLayerInfos[0];
                CarPart *vinyl_part;

                base_colour |= green << 8;
                base_colour |= blue << 16;
                base_colour |= gloss << 24;

                for (int i = 0; i < 4; i++) {
                    composite_params.SwatchColours[i] = base_colour;
                }

                bMemSet(&composite_params, 0, sizeof(composite_params));
                composite_params.DestTexture = dest_texture;
                composite_params.BaseColour = base_colour;

                for (int i = 0; i < 4; i++) {
                    composite_params.SwatchColours[i] = base_colour;
                }

                vinyl_part = ride_info->GetPart(CARSLOTID_VINYL_LAYER0);
                if (vinyl_part != 0) {
                    info->m_LayerHash = GetVinylLayerHash(ride_info, 0);
                    info->m_NumColours = vinyl_part->GetAppliedAttributeIParam(bStringHash("NUMCOLOURS"), 0);
                    if (info->m_NumColours == 0) {
                        return 0;
                    }
                }

                if (info->m_LayerHash != 0) {
                    info->m_LayerTexture = GetTextureInfo(info->m_LayerHash, false, false);

                    if (info->m_LayerTexture == 0) {
                        info->m_LayerHash = 0;
                    } else {
                        info->m_LayerImageData = static_cast<unsigned char *>(TextureInfo_LockImage(info->m_LayerTexture, TEXLOCK_READ));

                        if (use_palette) {
                            info->m_LayerImagePaletteData = static_cast<unsigned int *>(TextureInfo_LockPalette(info->m_LayerTexture, TEXLOCK_READ));
                        }

                        if (info->m_LayerImageData == 0) {
                            info->m_LayerHash = 0;
                        } else {
                            if (UsePrecompositeVinyls != 0 || ride_info->SkinType == 2) {
                                DumpPreComp(info, dest_texture);
                                return 1;
                            }

                            info->m_LayerMaskTexture = GetTextureInfo(bStringHash("_MASK", info->m_LayerHash), false, false);
                            if (info->m_LayerMaskTexture == 0) {
                                info->m_LayerHash = 0;
                            } else {
                                info->m_LayerMaskData = static_cast<unsigned char *>(TextureInfo_LockImage(info->m_LayerMaskTexture, TEXLOCK_READ));

                                if (use_palette) {
                                    info->m_LayerMaskPaletteData =
                                        static_cast<unsigned int *>(TextureInfo_LockPalette(info->m_LayerMaskTexture, TEXLOCK_READ));
                                }

                                if (info->m_LayerMaskData != 0) {
                                    composite_params.NumLayers = 1;
                                    if (vinyl_part != 0 && vinyl_part->HasAppliedAttribute(bStringHash("REMAP")) != 0) {
                                        info->m_RemapPalette = vinyl_part->GetAppliedAttributeIParam(bStringHash("REMAP"), 0);
                                        if (info->m_RemapPalette != 0) {
                                            for (int i = 0; i < 4; i++) {
                                                CarPart *vinyl_colour = ride_info->GetPart(CARSLOTID_VINYL_COLOUR0_0 + i);

                                                if (vinyl_colour == 0) {
                                                    info->m_RemapColours[i] = 0xFFu << ((i & 3) << 3);
                                                } else {
                                                    unsigned int remap_colour =
                                                        vinyl_colour->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                                                    int remap_green =
                                                        vinyl_colour->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                                                    int remap_blue =
                                                        vinyl_colour->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                                                    int remap_gloss =
                                                        vinyl_colour->GetAppliedAttributeIParam(bStringHash("GLOSS"), 0);

                                                    remap_colour |= remap_green << 8;
                                                    remap_colour |= remap_blue << 16;
                                                    remap_colour |= remap_gloss << 24;
                                                    info->m_RemapColours[i] = remap_colour;
                                                }
                                            }
                                        }
                                    }
                                } else {
                                    info->m_LayerHash = 0;
                                }
                            }
                        }
                    }
                }

                eWaitUntilRenderingDone();
                CompositeRim(ride_info);
                if (IsInSkinCompositeCache(&composite_params) == 0) {
                    UpdateSkinCompositeCache(&composite_params);
                    if (dest_texture->ImageCompressionType == TEXCOMP_32BIT) {
                        CompositeSkin32(&composite_params);
                    } else {
                        CompositeSkin(&composite_params);
                    }
                }

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
            }
        }
    }

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

unsigned int GetVinylLayerHash(CarPart *car_part, CarType car_type, int skin_type) {
    CarTypeInfo *type_info = &CarTypeInfoArray[car_type];
    const char *texture_name = car_part->GetAppliedAttributeString(bStringHash("TEXTURE"), 0);

    if (texture_name != 0) {
        char final_name[68];

        bStrCpy(final_name, type_info->BaseModelName);
        if (UsePrecompositeVinyls == 0 && skin_type != 2) {
            bStrCat(final_name, final_name, "_");
        } else {
            bStrCat(final_name, final_name, "_PRECOM_");
        }
        bStrCat(final_name, final_name, texture_name);
        return bStringHash(final_name);
    }

    return 0;
}

unsigned int GetVinylLayerHash(RideInfo *ride_info, int layer) {
    CarPart *vinyl = ride_info->GetPart(CARSLOTID_VINYL_LAYER0 + layer);

    if (vinyl == 0) {
        return 0;
    }

    return GetVinylLayerHash(vinyl, ride_info->Type, ride_info->SkinType);
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
