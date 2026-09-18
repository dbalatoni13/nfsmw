#ifndef XENON_ECSTASY_ELIGHT_PLAT_H
#define XENON_ECSTASY_ELIGHT_PLAT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct eLightMaterialPlatInterface {
    void CreatePlatInfo();
    void UpdatePlatInfo();
};

void elInitPlat();

// r71e: la capa GC declara PlatConvertColor (eLightPlat.hpp:50) y eLight.hpp
// (indep) la usa; el stub Xenon no la tenia. X360 es big-endian como GC.
unsigned int PlatConvertColor(unsigned int colour);

#endif
