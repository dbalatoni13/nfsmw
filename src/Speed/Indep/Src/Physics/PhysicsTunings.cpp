#include "PhysicsTunings.h"

// Fuera del namespace a proposito: en el original el simbolo es "TuningLimits"
// a secas (.rodata 0x803F9804, scope:local). Dentro de namespace Physics GCC lo
// emite como _7Physics.TuningLimits, que es otro simbolo.
static const float TuningLimits[7][2] = {
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
};

namespace Physics {

float Tunings::LowerLimit(Tunings::Path path) {
    return TuningLimits[path][0];
}

float Tunings::UpperLimit(Tunings::Path path) {
    return TuningLimits[path][1];
}

} // namespace Physics
