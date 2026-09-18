#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/GameCube/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/GameCube/Src/Ecstasy/eLightPlat.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include <dolphin.h>
#include "Speed/GameCube/Src/Ecstasy/DmaE.hpp"
SlotPool *eLightMaterialPlatInfoSlotPool = 0;
extern eLightMaterial *TweakLightMaterial;

bVector4 eLamb(128.0f, 128.0f, 128.0f, 0.0f);
bVector4 eLdir[3];
bVector4 eLpos[3];
bVector4 eLspec[3];
bVector4 eLdiff[3] = {bVector4(128.0f, 128.0f, 128.0f, 0.0f), bVector4(192.0f, 192.0f, 192.0f, 0.0f), bVector4(96.0f, 96.0f, 96.0f, 0.0f)};

GXColor gx_LightColour(const bVector4 &v4);
GXLightID GetLightID(int id);

float arnMinA = 0.2f;
float arnMaxA = 0.6f;
float arnEnvMax = 0.5f;

void eLightMaterialPlatInterface::CreatePlatInfo() {
    PlatInfo = new eLightMaterialPlatInfo;

    PlatInfo->pLightMaterialScaleTextureInfo = 0;
    PlatInfo->pLightMaterialScalePixels = 0;
}

void eLightMaterialPlatInterface::UpdatePlatInfo() {
    eLightMaterial *light_material = static_cast<eLightMaterial *>(this);
    eLightMaterialPlatInfo *plat_info = light_material->PlatInfo;
    eLightMaterialData *material_data = &light_material->feData;
    const float OneAsFloat = 1.0f;

    float diffuse_min_scale = material_data->DiffuseMinScale;
    float diffuse_min_r = diffuse_min_scale * material_data->DiffuseMinR;
    float diffuse_min_g = diffuse_min_scale * material_data->DiffuseMinG;
    float diffuse_min_b = diffuse_min_scale * material_data->DiffuseMinB;
    float diffuse_min_a = material_data->DiffuseMinA;
    float diffuse_max_scale = material_data->DiffuseMaxScale;
    float diffuse_rng_r = diffuse_max_scale * material_data->DiffuseMaxR - diffuse_min_r;
    float diffuse_rng_g = diffuse_max_scale * material_data->DiffuseMaxG - diffuse_min_g;
    float diffuse_rng_b = diffuse_max_scale * material_data->DiffuseMaxB - diffuse_min_b;
    float diffuse_rng_a = material_data->DiffuseMaxA - diffuse_min_a;

    float specular_power = material_data->SpecularPower;
    float specular_magic = FloatAsInteger(OneAsFloat) - specular_power * 1065353200.0f;
    float specular_min_scale = material_data->SpecularMinScale;
    float specular_min_r = specular_min_scale * material_data->SpecularMinR;
    float specular_min_g = specular_min_scale * material_data->SpecularMinG;
    float specular_min_b = specular_min_scale * material_data->SpecularMinB;
    float specular_max_scale = material_data->SpecularMaxScale;
    float specular_rng_r = specular_max_scale * material_data->SpecularMaxR - specular_min_r;
    float specular_rng_g = specular_max_scale * material_data->SpecularMaxG - specular_min_g;
    float specular_rng_b = specular_max_scale * material_data->SpecularMaxB - specular_min_b;

    float envmap_power = material_data->EnvmapPower;
    float envmap_magic = FloatAsInteger(OneAsFloat) - envmap_power * 1065353200.0f;
    float envmap_min_scale = material_data->EnvmapMinScale;
    float envmap_min_r = envmap_min_scale * material_data->EnvmapMinR;
    float envmap_min_g = envmap_min_scale * material_data->EnvmapMinG;
    float envmap_min_b = material_data->EnvmapMinB * envmap_min_scale;
    float envmap_max_scale = material_data->EnvmapMaxScale;
    float envmap_max_r = envmap_max_scale * material_data->EnvmapMaxR;
    float envmap_max_g = envmap_max_scale * material_data->EnvmapMaxG;
    float envmap_max_b = envmap_max_scale * material_data->EnvmapMaxB;

    switch (light_material->NameHash) {

    default:

        envmap_min_r *= 0.1f; envmap_min_g *= 0.1f; envmap_min_b *= 0.1f;
        envmap_max_r *= 0.1f; envmap_max_g *= 0.1f; envmap_max_b *= 0.1f;
        break;

    case 0x33A26CB6:
        diffuse_min_a *= 0.25f;
        break;

    case 0xA6348EE3:

        diffuse_min_a = arnMinA; diffuse_rng_a = arnMaxA - diffuse_min_a;
        envmap_max_b = envmap_max_r = envmap_max_g = arnEnvMax;
        break;

    case 0x12C9453C:
    case 0x3ED70C43:

        envmap_min_b = envmap_min_r = envmap_min_g = 0.4f;
        diffuse_min_a *= 0.25f;
        break;

    case 0x471A1DCA:

        envmap_power = 2.0f;

        diffuse_min_a *= 0.25f;
        diffuse_rng_a = material_data->DiffuseMaxA * 1.18f - diffuse_min_a;

        break;

    case 0x89946400:
    case 0x89F23A14:
        envmap_power = 2.0f;

        diffuse_min_a *= 0.25f;
        diffuse_rng_a = material_data->DiffuseMaxA * 1.18f - diffuse_min_a;
        break;

    case 0x8812634B:
    case 0x8A5626D8:
    case 0xD51AACCD:
    case 0xD57882E1:

        envmap_power = 8.0f;
        diffuse_rng_a = 0.0f;
        diffuse_min_a *= 0.25f;
        diffuse_rng_a = material_data->DiffuseMaxA * 1.18f - diffuse_min_a;
        break;

    case 0x68E97F75:
    case 0x8050CA3F:
    case 0x8A60F52C:
    case 0x8A616C4C:
    case 0x8A6435CA:
    case 0x8A66DA4B:
    case 0x8A690BF6:
    case 0xC7228AA0:
    case 0xD68EA0C1:
    case 0xD6EC6733:
    case 0xD6EC76D5:
    case 0xDD90E320:
        envmap_power = 6.0f;

        diffuse_min_a = 0.22f;
        diffuse_rng_a = 0.86f;

        break;

    case 0x2388DD82:
    case 0x23E6A3F4:
    case 0x23E6B396:
    case 0x6CF41096:
    case 0x8062E2C0:
    case 0x8CB61DCD:
    case 0x8CB694ED:
    case 0x8CB95E6B:
    case 0x8CBC02EC:
    case 0x8CBE3497:
    case 0xB3645F81:
    case 0xC9D2B801:
    {

        envmap_power = 6.0f;

        diffuse_rng_a = 0.0f;

        diffuse_max_scale = 0.3f;
        if (light_material->NameHash == 0x2388DD82) {
            diffuse_max_scale = 0.0f;
        }
        diffuse_min_a = 0.22f;
        diffuse_rng_a = 0.86f;

        float diffuse_max_r = material_data->DiffuseMaxR * diffuse_max_scale;
        float diffuse_max_g = material_data->DiffuseMaxG * diffuse_max_scale;
        float diffuse_max_b = material_data->DiffuseMaxB * diffuse_max_scale;

        float diff_r = diffuse_min_r - diffuse_max_r;
        float diff_g = diffuse_min_g - diffuse_max_g;
        float diff_b = diffuse_min_b - diffuse_max_b;
        float diff_scale = bAbs(diff_r);

        diffuse_min_r = diffuse_max_r;
        diffuse_min_g = diffuse_max_g;
        diffuse_min_b = diffuse_max_b;

        diffuse_rng_r = diff_r;
        diffuse_rng_g = diff_g;
        diffuse_rng_b = diff_b;

        break;
    }

    case 0x33310077:
    {
        float diffuse_max_r = diffuse_max_scale * material_data->DiffuseMaxR;
        float diffuse_max_g = diffuse_max_scale * material_data->DiffuseMaxG;
        float diffuse_max_b = diffuse_max_scale * material_data->DiffuseMaxB;
        float diff_r = diffuse_min_r - diffuse_max_r;
        float diff_g = diffuse_min_g - diffuse_max_g;
        float diff_b = diffuse_min_b - diffuse_max_b;
        float diff_scale = bAbs(diff_r);

        diffuse_min_r = diffuse_max_r;
        diffuse_min_g = diffuse_max_g;
        diffuse_min_b = diffuse_max_b;

        diffuse_rng_r = diff_r;
        diffuse_rng_g = diff_g;
        diffuse_rng_b = diff_b;

        break;
    }
    }
    unsigned int envmap_min_ir = static_cast<int>(envmap_min_r * 128.0f);
    unsigned int envmap_min_ig = static_cast<int>(envmap_min_g * 128.0f);
    unsigned int envmap_min_ib = static_cast<int>(envmap_min_b * 128.0f);
    unsigned int envmap_max_ir = static_cast<int>(envmap_max_r * 128.0f);
    unsigned int envmap_max_ig = static_cast<int>(envmap_max_g * 128.0f);
    unsigned int envmap_max_ib = static_cast<int>(envmap_max_b * 128.0f);

    envmap_min_ir = bClamp(envmap_min_ir, 0, 255);
    envmap_min_ig = bClamp(envmap_min_ig, 0, 255);
    envmap_min_ib = bClamp(envmap_min_ib, 0, 255);
    envmap_max_ir = bClamp(envmap_max_ir, 0, 255);
    envmap_max_ig = bClamp(envmap_max_ig, 0, 255);
    envmap_max_ib = bClamp(envmap_max_ib, 0, 255);

    plat_info->DiffuseMinR = diffuse_min_r;
    plat_info->DiffuseMinG = diffuse_min_g;
    plat_info->DiffuseMinB = diffuse_min_b;
    plat_info->DiffuseMinA = diffuse_min_a;
    plat_info->DiffuseRngR = diffuse_rng_r;
    plat_info->DiffuseRngG = diffuse_rng_g;
    plat_info->DiffuseRngB = diffuse_rng_b;
    plat_info->DiffuseRngA = diffuse_rng_a;

    plat_info->SpecularMinR = specular_min_r;
    plat_info->SpecularMinG = specular_min_g;
    plat_info->SpecularMinB = specular_min_b;
    plat_info->SpecularRngR = specular_rng_r;
    plat_info->SpecularRngG = specular_rng_g;
    plat_info->SpecularRngB = specular_rng_b;

    plat_info->SpecularPower = specular_power;
    plat_info->EnvmapPower = envmap_power;
    plat_info->SpecularPowerBias = specular_magic;
    plat_info->EnvmapPowerBias = envmap_magic;

    plat_info->EnvmapMinRGB = (envmap_min_ir << 24) | (envmap_min_ig << 16) | (envmap_min_ib << 8);
    plat_info->EnvmapMaxRGB = (envmap_max_ir << 24) | (envmap_max_ig << 16) | (envmap_max_ib << 8);
}

void elInitPlat() {
    eLightMaterialPlatInfoSlotPool = bNewSlotPool(sizeof(eLightMaterialPlatInfo), 160, "eLightMaterialPlatInfoSlotPool", 0);
}

GXLightID GetLightID(int id) {

    GXLightID lid = GX_LIGHT_NULL;
    switch (id) {

    case 0: lid = GX_LIGHT0; break;
    case 1: lid = GX_LIGHT1; break;
    case 2: lid = GX_LIGHT2; break;
    case 3: lid = GX_LIGHT3; break;
    case 4: lid = GX_LIGHT4; break;
    case 5: lid = GX_LIGHT5; break;
    case 6: lid = GX_LIGHT6; break;
    case 7: lid = GX_LIGHT7; break;
    }
    return lid;
}

// Recorta a byte: el valor por defecto es 0 y solo se toca si n cabe.
static inline int _c(int n) {
    if (n < 0)
        return 0;
    if (n > 255)
        return 255;
    return n;
}

GXColor gx_LightColour(const bVector4 &v4) {

    int r = (int)v4.x;
    int g = (int)v4.y;
    int b = (int)v4.z;
    int a = 255;

    GXColor clr;
    clr.r = _c(r);
    clr.g = _c(g);
    clr.b = _c(b);
    clr.a = _c(a);

    return clr;
}

void gx_LightAmbient(int opt) {
    if (opt == 0) {

        GXColor black = { 0, 0, 0, 255 };
        GXSetChanAmbColor(GX_COLOR0A0, black);

    } else {

        GXColor amb = gx_LightColour(eLamb);
        GXSetChanAmbColor(GX_COLOR0A0, amb);
    }
}

void gx_Lighting() {
    int l;
    GXLightObj light[3];
    bVector4 dir;
    GXColor clr;
    static float LARGE_NUMBER = -1048576.0f;

    for (l = 0; l < 3; l++) {

        dir = eLdir[l];
        dir.x *= LARGE_NUMBER;
        dir.y *= LARGE_NUMBER;
        dir.z *= LARGE_NUMBER;

        clr = gx_LightColour(eLdiff[l]);

        GXInitLightPos(&light[l], eLpos[l].x, eLpos[l].y, eLpos[l].z);
        GXInitLightDir(&light[l], dir.x, dir.y, dir.z);
        GXInitLightColor(&light[l], clr);

        GXLoadLightObjImm(&light[l], GetLightID(l));
    }
}

unsigned int PlatConvertColor(unsigned int colour) {

    unsigned int a = colour >> 24;
    unsigned int r = (colour >> 16) & 0xFF;
    unsigned int g = (colour >> 8) & 0xFF;
    unsigned int b = colour & 0xFF;

    unsigned int out_colour = a | (r << 8) | (g << 16) | (b << 24);
    return out_colour;
}
