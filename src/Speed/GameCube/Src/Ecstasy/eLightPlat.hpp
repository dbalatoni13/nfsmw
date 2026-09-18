#ifndef GAMECUBE_ECSTASY_ELIGHT_PLAT_H
#define GAMECUBE_ECSTASY_ELIGHT_PLAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct eLightMaterialPlatInfo {
    void *operator new(unsigned int size);

    float DiffuseMinR;
    float DiffuseMinG;
    float DiffuseMinB;
    float DiffuseMinA;
    float DiffuseRngR;
    float DiffuseRngG;
    float DiffuseRngB;
    float DiffuseRngA;
    float SpecularMinR;
    float SpecularMinG;
    float SpecularMinB;
    float SpecularRngR;
    float SpecularRngG;
    float SpecularRngB;
    float SpecularPower;
    float EnvmapPower;
    float SpecularPowerBias;
    float EnvmapPowerBias;
    unsigned int EnvmapMinRGB;
    unsigned int EnvmapMaxRGB;
    TextureInfo *pLightMaterialScaleTextureInfo;
    unsigned int *pLightMaterialScalePixels;
};

extern SlotPool *eLightMaterialPlatInfoSlotPool;

inline void *eLightMaterialPlatInfo::operator new(unsigned int size) {
    return bOMalloc(eLightMaterialPlatInfoSlotPool);
}

struct eLightMaterialPlatInterface {
    eLightMaterialPlatInfo *PlatInfo;

    void CreatePlatInfo();
    void UpdatePlatInfo();
};

void elInitPlat();

unsigned int PlatConvertColor(unsigned int colour);

#endif
