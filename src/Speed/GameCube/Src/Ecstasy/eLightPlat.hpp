#ifndef GAMECUBE_ECSTASY_ELIGHT_PLAT_H
#define GAMECUBE_ECSTASY_ELIGHT_PLAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct eLightMaterialPlatInterface {
    void CreatePlatInfo();
    void UpdatePlatInfo();
};

void elInitPlat();

#endif
