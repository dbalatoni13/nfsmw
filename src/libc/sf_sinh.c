/* sf_sinh.c -- float version of e_sinh.c */
#include "fdlibm.h"


static const float one = 1.0, shuge = 1.0e37;

float sinhf(float x) {
    float t, w, h;
    int ix, jx;

    GET_FLOAT_WORD(jx, x);
    ix = jx & 0x7fffffff;

    /* x is INF or NaN */
    if (ix >= 0x7f800000)
        return x + x;

    h = 0.5;
    if (jx < 0)
        h = -h;
    /* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
    if (ix < 0x41b00000) {   /* |x|<22 */
        if (ix < 0x31800000) /* |x|<2**-28 */
            if (shuge + x > one)
                return x; /* sinh(tiny) = tiny with inexact */
        t = expm1f(fabsf(x));
        if (ix < 0x3f800000)
            return h * ((float)2.0 * t - t * t / (t + one));
        return h * (t + t / (t + one));
    }

    /* |x| in [22, log(maxfloat)] return 0.5*exp(|x|) */
    if (ix < 0x42b17180)
        return h * expf(fabsf(x));

    /* |x| in [log(maxfloat), overflowthresold] */
    if (ix <= 0x42b2d4fc) {
        w = expf((float)0.5 * fabsf(x));
        t = h * w;
        return t * w;
    }

    /* |x| > overflowthresold, sinh(x) overflow */
    return x * shuge;
}
