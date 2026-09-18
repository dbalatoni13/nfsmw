#ifndef PC_ECSTASY_ELIGHT_PLAT_H
#define PC_ECSTASY_ELIGHT_PLAT_H

struct eLightMaterialPlatInterface {
    void CreatePlatInfo();
    void UpdatePlatInfo();
};

void elInitPlat();

// eLight.hpp (common code) calls it, as on the other platforms.
unsigned int PlatConvertColor(unsigned int colour);

#endif
