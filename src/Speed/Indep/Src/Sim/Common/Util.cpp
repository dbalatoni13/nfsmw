#include "../UTil.h"
#include "Speed/Indep/Src/Physics/Dynamics/Inertia.h"

UMath::Matrix4 Util_GenerateMatrix(const UMath::Vector3 &vec, const UMath::Vector3 *InUp) {
    UMath::Matrix4 mat;
    UMath::Vector4 up = {0.0f, 1.0f, 0.0f, 0.0f};

    if (InUp) {
        up.x = InUp->x;
        up.y = InUp->y;
        up.z = InUp->z;
    }
    mat[2].x = vec.x;
    mat[2].y = vec.y;
    mat[2].z = vec.z;
    mat[2].w = 0.0f;

    UMath::Unitxyz(mat[2], mat[2]);
    UMath::UnitCross(UMath::Vector4To3(up), UMath::Vector4To3(mat[2]), UMath::Vector4To3(mat[0]));
    UMath::Crossxyz(mat[2], mat[0], mat[1]);
    mat[3] = UMath::Vector4::kIdentity;

    mat[0].w = 0.0f;
    mat[2].w = 0.0f;
    mat[1].w = 0.0f;

    return mat;
}

UMath::Vector3 Util_GenerateCarTensor(const float mass, const float width, const float height, const float length,
                                      const UMath::Vector3 &tensorScale) {
    Dynamics::Inertia::Tensor t = Dynamics::Inertia::Box(mass, 2 * width, 2 * height, 2 * length);
    UMath::Scale(t, tensorScale, t);
    return t;
}
