#ifndef PSX2_ECSTASY_ELIGHTPLAT_H
#define PSX2_ECSTASY_ELIGHTPLAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct eLightMaterialPlatInterface {
    void CreatePlatInfo();
    void UpdatePlatInfo();
};

void elInitPlat();

#endif
