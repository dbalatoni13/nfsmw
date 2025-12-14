#ifndef ANIMATION_ANIMLOCATOR_H
#define ANIMATION_ANIMLOCATOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

class CAnimLocator {
  public:
    static void SetAnimOriginPosition(const bVector3 &nis_scene_origin, unsigned short pnis_scene_angle);

    static bool GetAnimOriginPosition(bVector3 *nis_scene_origin, unsigned short *pnis_scene_angle, bool at_start_line);

    static bool GetInitialAnimMatricies(bMatrix4 *scene_rotation_matrix, bMatrix4 *scene_translation_matrix, bool at_start_line);
};

#endif
