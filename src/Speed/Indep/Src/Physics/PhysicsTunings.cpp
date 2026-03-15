#include "PhysicsTunings.h"

namespace Physics {

static const float TuningLimits[Tunings::MAX_TUNINGS][2] = {
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
    {-1.0f, 1.0f},
};

void Tunings::Default() {
    for (int i = 0; i < MAX_TUNINGS; i++) {
        Value[i] = 0.0f;
    }
}

float Tunings::LowerLimit(Path path) { return TuningLimits[path][0]; }

float Tunings::UpperLimit(Path path) { return TuningLimits[path][1]; }

} // namespace Physics