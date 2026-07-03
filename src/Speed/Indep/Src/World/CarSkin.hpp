#ifndef __CARSKIN_HPP_
#define __CARSKIN_HPP_

#include "Speed/Indep/Src/World/CarInfo.hpp"

// total size: 0x34
struct VinylLayerInfo {
    uint32 m_LayerHash;              // offset 0x0, size 0x4
    TextureInfo *m_LayerTexture;     // offset 0x4, size 0x4
    TextureInfo *m_LayerMaskTexture; // offset 0x8, size 0x4
    uint8 *m_LayerImageData;         // offset 0xC, size 0x4
    uint32 *m_LayerImagePaletteData; // offset 0x10, size 0x4
    uint8 *m_LayerMaskData;          // offset 0x14, size 0x4
    uint32 *m_LayerMaskPaletteData;  // offset 0x18, size 0x4
    int m_NumColours;                // offset 0x1C, size 0x4
    int m_RemapPalette;              // offset 0x20, size 0x4
    unsigned int m_RemapColours[4];  // offset 0x24, size 0x10
};

class TextureInfo;

// total size: 0x50
struct SkinCompositeParams {
    TextureInfo *DestTexture;          // offset 0x0, size 0x4
    uint32 BaseColour;                 // offset 0x4, size 0x4
    unsigned int SwatchColours[4];     // offset 0x8, size 0x10
    VinylLayerInfo VinylLayerInfos[1]; // offset 0x18, size 0x34
    int NumLayers;                     // offset 0x4C, size 0x4
};

uint32 GetVinylLayerHash(CarPart *car_part, CarType car_type, int skin_type);
int GetTempCarSkinTextures(unsigned int *textures_to_load, int num_textures, int max_textures, RideInfo *ride);
int CompositeSkin(RideInfo *ride_info);
void UpdateSkinCompositeCache(SkinCompositeParams *skin_composite_params);
void FlushFromSkinCompositeCache(unsigned int texture_name_hash);

#endif
