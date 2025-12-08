#include "../UTil.h"
#include "Speed/Indep/Src/Physics/Dynamics/Inertia.h"

// UMath::Matrix4 Util_GenerateMatrix(const UMath::Vector3 &vec, const UMath::Vector3 *InUp) {}

UMath::Vector3 Util_GenerateCarTensor(const float mass, const float width, const float height, const float length,
                                      const UMath::Vector3 &tensorScale) {
    Dynamics::Inertia::Tensor t = Dynamics::Inertia::Box(mass, 2 * width, 2 * height, 2 * length);
    UMath::Scale(t, tensorScale, t);
    return t;
}
