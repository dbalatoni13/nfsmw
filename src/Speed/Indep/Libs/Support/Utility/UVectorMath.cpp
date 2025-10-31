#include "UVectorMath.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

float VU0_Atan2(const float opposite, const float adjacent) {
    float x = FLOAT_EPSILON < VU0_fabs(adjacent) ? adjacent : 0.0f;
    float y = FLOAT_EPSILON < VU0_fabs(opposite) ? opposite : 0.0f;

    short bangle = bATan(x, y);
    return bangle / 65536.0f;
}
