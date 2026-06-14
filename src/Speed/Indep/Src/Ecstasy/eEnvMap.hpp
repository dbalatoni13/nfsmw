#ifndef __ECSTASY_ENGINE__ENVMAP
#define __ECSTASY_ENGINE__ENVMAP

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

class eEnvMap {
  public:
    eEnvMap() {}

    void UpdateCameras(bVector3 *viewer_world_position, bVector3 *envmap_world_position);

    eView *Views[6];   // offset 0x0, size 0x18,
    Camera Cameras[6]; // offset 0x18, size 0xF60,
};

eEnvMap *eGetEnvMap();

#endif
