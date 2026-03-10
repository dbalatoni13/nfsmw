#include "Speed/Indep/Src/Camera/CameraMover.hpp"

static float CrossXY(const bVector3 *v1, const bVector3 *v2) {
    return v1->x * v2->y - v1->y * v2->x;
}
