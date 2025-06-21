#pragma once

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct Car : public bTNode<Car> {
    // total size: 0x8
};

struct VinylLayerInfo {
    // total size: 0x34
    unsigned int m_LayerHash;               // offset 0x0, size 0x4
    struct TextureInfo *m_LayerTexture;     // offset 0x4, size 0x4
    struct TextureInfo *m_LayerMaskTexture; // offset 0x8, size 0x4
    unsigned char *m_LayerImageData;        // offset 0xC, size 0x4
    unsigned int *m_LayerImagePaletteData;  // offset 0x10, size 0x4
    unsigned char *m_LayerMaskData;         // offset 0x14, size 0x4
    unsigned int *m_LayerMaskPaletteData;   // offset 0x18, size 0x4
    int m_NumColours;                       // offset 0x1C, size 0x4
    int m_RemapPalette;                     // offset 0x20, size 0x4
    unsigned int m_RemapColours[4];         // offset 0x24, size 0x10
};

struct SkinCompositeParams {
    // total size: 0x50
    struct TextureInfo *DestTexture;   // offset 0x0, size 0x4
    unsigned int BaseColour;           // offset 0x4, size 0x4
    unsigned int SwatchColours[4];     // offset 0x8, size 0x10
    VinylLayerInfo VinylLayerInfos[1]; // offset 0x18, size 0x34
    int NumLayers;                     // offset 0x4C, size 0x4
};

void UpdateSkinCompositeCache(SkinCompositeParams *skin_composite_params);
void FlushFromSkinCompositeCache(unsigned int texture_name_hash);
