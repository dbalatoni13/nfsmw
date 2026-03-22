#include "./Car.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

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
