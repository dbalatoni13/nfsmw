/* fdlibm.h de newlib, reducido a lo que usan estas unidades: los macros de
 * acceso a los bits de un flotante y los prototipos.
 *
 * Las constantes de cada unidad son `static const` como en newlib: el
 * front-end las pliega y salen como entradas del pool del compilador, que con
 * -msafe-sda van a .sdata por @sda21 en el orden de primer uso (el del
 * original).  Las tablas llevan su tamano escrito porque con `[]` este
 * compilador no las trata como dato pequeno; las mayores piden el -G de su
 * unidad (configure.py).
 */
#ifndef _FDLIBM_H_
#define _FDLIBM_H_

typedef union {
    double value;
    struct {
        unsigned int msw;
        unsigned int lsw;
    } parts;
} ieee_double_shape_type;

typedef union {
    float value;
    unsigned int word;
} ieee_float_shape_type;

#define EXTRACT_WORDS(ix0, ix1, d)                                             \
    do {                                                                       \
        ieee_double_shape_type ew_u;                                           \
        ew_u.value = (d);                                                      \
        (ix0) = ew_u.parts.msw;                                                \
        (ix1) = ew_u.parts.lsw;                                                \
    } while (0)

#define GET_HIGH_WORD(i, d)                                                    \
    do {                                                                       \
        ieee_double_shape_type gh_u;                                           \
        gh_u.value = (d);                                                      \
        (i) = gh_u.parts.msw;                                                  \
    } while (0)

#define GET_LOW_WORD(i, d)                                                     \
    do {                                                                       \
        ieee_double_shape_type gl_u;                                           \
        gl_u.value = (d);                                                      \
        (i) = gl_u.parts.lsw;                                                  \
    } while (0)

#define INSERT_WORDS(d, ix0, ix1)                                              \
    do {                                                                       \
        ieee_double_shape_type iw_u;                                           \
        iw_u.parts.msw = (ix0);                                                \
        iw_u.parts.lsw = (ix1);                                                \
        (d) = iw_u.value;                                                      \
    } while (0)

#define SET_HIGH_WORD(d, v)                                                    \
    do {                                                                       \
        ieee_double_shape_type sh_u;                                           \
        sh_u.value = (d);                                                      \
        sh_u.parts.msw = (v);                                                  \
        (d) = sh_u.value;                                                      \
    } while (0)

#define SET_LOW_WORD(d, v)                                                     \
    do {                                                                       \
        ieee_double_shape_type sl_u;                                           \
        sl_u.value = (d);                                                      \
        sl_u.parts.lsw = (v);                                                  \
        (d) = sl_u.value;                                                      \
    } while (0)

#define GET_FLOAT_WORD(i, d)                                                   \
    do {                                                                       \
        ieee_float_shape_type gf_u;                                            \
        gf_u.value = (d);                                                      \
        (i) = gf_u.word;                                                       \
    } while (0)

#define SET_FLOAT_WORD(d, i)                                                   \
    do {                                                                       \
        ieee_float_shape_type sf_u;                                            \
        sf_u.word = (i);                                                       \
        (d) = sf_u.value;                                                      \
    } while (0)

/* Lo que newlib saca de <math.h> y de su propio fdlibm.h. */
extern double fabs(double x);
extern double sqrt(double x);
extern double floor(double x);
extern double copysign(double x, double y);
extern double scalbn(double x, int n);
extern float fabsf(float x);
extern float sqrtf(float x);
extern float floorf(float x);
extern float copysignf(float x, float y);
extern float scalbnf(float x, int n);
extern float atanf(float x);
extern float expf(float x);
extern float expm1f(float x);
extern float logf(float x);

extern int __ieee754_rem_pio2(double x, double *y);
extern double __kernel_sin(double x, double y, int iy);
extern double __kernel_cos(double x, double y);
extern double __kernel_tan(double x, double y, int iy);
extern int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec, const int *ipio2);
extern int __ieee754_rem_pio2f(float x, float *y);
extern float __kernel_sinf(float x, float y, int iy);
extern float __kernel_cosf(float x, float y);
extern float __kernel_tanf(float x, float y, int iy);
extern int __kernel_rem_pio2f(float *x, float *y, int e0, int nx, int prec, const int *ipio2);

#endif
