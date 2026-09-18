// Stub portable del VU0_rsqrt de Gekko (Newton-Raphson en paired singles).
// El resto de VU0_* ya son C portable via UVectorMathCPU.hpp.
#include <cmath>

float bFMod(float a, float b) { return fmodf(a, b); }
float VU0_rsqrt(float a) { return 1.0f / sqrtf(a); }

extern const float lbl_803EB444 = 1.0f;
extern const float lbl_803EB448 = 0.0f;
extern const float lbl_803EB44C = 1.0f;
extern const float lbl_803EB450 = 1.0f;
