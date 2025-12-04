#include "UVectorMath.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

float VU0_Atan2(const float opposite, const float adjacent) {
    float x = FLOAT_EPSILON < VU0_fabs(adjacent) ? adjacent : 0.0f;
    float y = FLOAT_EPSILON < VU0_fabs(opposite) ? opposite : 0.0f;

    short bangle = bATan(x, y);
    return bangle / 65536.0f;
}

void VU0_v4addxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w;
}
