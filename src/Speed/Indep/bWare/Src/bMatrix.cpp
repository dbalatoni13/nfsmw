

#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#if defined(EA_PLATFORM_WIN32)
extern "C" bMatrix4 *__stdcall D3DXMatrixTranspose(bMatrix4 *dest, const bMatrix4 *src);
#endif

#ifdef EA_PLATFORM_PLAYSTATION2
// Retail EE affine inverse; both PS2 versions use this same instruction body.
asm(
    ".text\n\t"
    ".set noreorder\n\t"
    ".set nomacro\n\t"
    ".globl bInvertMatrix__FP8bMatrix4PC8bMatrix4\n\t"
    ".ent bInvertMatrix__FP8bMatrix4PC8bMatrix4\n\t"
    "bInvertMatrix__FP8bMatrix4PC8bMatrix4:\n\t"
    "addiu $29, $29, -16\n\t"
    "lwc1 $f7, 0($5)\n\t"
    "swc1 $f20, 0($29)\n\t"
    "lui $1, 0x3f80\n\t"
    "mtc1 $1, $f9\n\t"
    "lwc1 $f14, 16($5)\n\t"
    "lwc1 $f0, 24($5)\n\t"
    "lwc1 $f16, 32($5)\n\t"
    "lwc1 $f8, 40($5)\n\t"
    "lwc1 $f10, 20($5)\n\t"
    "mul.s $f4, $f0, $f16\n\t"
    "lwc1 $f15, 36($5)\n\t"
    "mul.s $f12, $f14, $f8\n\t"
    "mul.s $f3, $f10, $f8\n\t"
    "lwc1 $f5, 4($5)\n\t"
    "mul.s $f1, $f0, $f15\n\t"
    "lwc1 $f2, 8($5)\n\t"
    "sub.s $f12, $f12, $f4\n\t"
    "lwc1 $f11, 48($5)\n\t"
    "mul.s $f4, $f10, $f16\n\t"
    "lwc1 $f17, 52($5)\n\t"
    "mul.s $f19, $f14, $f15\n\t"
    "lwc1 $f18, 56($5)\n\t"
    "sub.s $f3, $f3, $f1\n\t"
    "swc1 $f9, 60($4)\n\t"
    "mul.s $f6, $f5, $f12\n\t"
    "sw $0, 12($4)\n\t"
    "sub.s $f19, $f19, $f4\n\t"
    "sw $0, 28($4)\n\t"
    "mul.s $f1, $f7, $f3\n\t"
    "sw $0, 44($4)\n\t"
    "mul.s $f20, $f2, $f14\n\t"
    "mul.s $f4, $f2, $f19\n\t"
    "sub.s $f1, $f1, $f6\n\t"
    "mul.s $f6, $f7, $f0\n\t"
    "mul.s $f0, $f5, $f0\n\t"
    "add.s $f1, $f1, $f4\n\t"
    "mul.s $f13, $f7, $f8\n\t"
    "mul.s $f4, $f2, $f15\n\t"
    "div.s $f9, $f9, $f1\n\t"
    "mul.s $f1, $f2, $f10\n\t"
    "mul.s $f14, $f5, $f14\n\t"
    "mul.s $f8, $f5, $f8\n\t"
    "mul.s $f2, $f2, $f16\n\t"
    "sub.s $f0, $f0, $f1\n\t"
    "mul.s $f10, $f7, $f10\n\t"
    "neg.s $f1, $f9\n\t"
    "sub.s $f6, $f6, $f20\n\t"
    "sub.s $f8, $f8, $f4\n\t"
    "lwc1 $f20, 0($29)\n\t"
    "sub.s $f13, $f13, $f2\n\t"
    "mul.s $f7, $f7, $f15\n\t"
    "mul.s $f5, $f5, $f16\n\t"
    "sub.s $f10, $f10, $f14\n\t"
    "mul.s $f3, $f9, $f3\n\t"
    "mul.s $f0, $f9, $f0\n\t"
    "mul.s $f12, $f1, $f12\n\t"
    "mul.s $f6, $f1, $f6\n\t"
    "swc1 $f3, 0($4)\n\t"
    "mul.s $f8, $f1, $f8\n\t"
    "swc1 $f0, 8($4)\n\t"
    "mul.s $f13, $f9, $f13\n\t"
    "swc1 $f12, 16($4)\n\t"
    "mul.s $f10, $f9, $f10\n\t"
    "swc1 $f6, 24($4)\n\t"
    "sub.s $f7, $f7, $f5\n\t"
    "swc1 $f8, 4($4)\n\t"
    "mul.s $f9, $f9, $f19\n\t"
    "swc1 $f13, 20($4)\n\t"
    "mul.s $f6, $f17, $f6\n\t"
    "swc1 $f10, 40($4)\n\t"
    "mul.s $f12, $f17, $f12\n\t"
    "mul.s $f0, $f11, $f0\n\t"
    "swc1 $f9, 32($4)\n\t"
    "mul.s $f3, $f11, $f3\n\t"
    "mul.s $f1, $f1, $f7\n\t"
    "mul.s $f11, $f11, $f8\n\t"
    "mul.s $f17, $f17, $f13\n\t"
    "mul.s $f10, $f18, $f10\n\t"
    "swc1 $f1, 36($4)\n\t"
    "mul.s $f9, $f18, $f9\n\t"
    "add.s $f3, $f3, $f12\n\t"
    "add.s $f0, $f0, $f6\n\t"
    "add.s $f11, $f11, $f17\n\t"
    "mul.s $f18, $f18, $f1\n\t"
    "add.s $f0, $f0, $f10\n\t"
    "add.s $f3, $f3, $f9\n\t"
    "add.s $f11, $f11, $f18\n\t"
    "neg.s $f0, $f0\n\t"
    "neg.s $f3, $f3\n\t"
    "neg.s $f11, $f11\n\t"
    "swc1 $f0, 56($4)\n\t"
    "swc1 $f3, 48($4)\n\t"
    "swc1 $f11, 52($4)\n\t"
    "jr $31\n\t"
    "addiu $29, $29, 16\n\t"
    ".end bInvertMatrix__FP8bMatrix4PC8bMatrix4\n\t"
    ".set macro\n\t"
    ".set reorder\n\t");
#else
void bInvertMatrix(bMatrix4 *dest, const bMatrix4 *src) {
#ifdef EA_PLATFORM_WIN32
    float a = src->v0.x;
    float b = src->v0.y;
    float c = src->v0.z;
    float g = src->v1.x;
    float f = src->v1.y;
    float d = src->v1.z;
    float i = src->v2.x;
    float h = src->v2.y;
    float e = src->v2.z;
    float j = src->v3.x;
    float k = src->v3.y;
    float l = src->v3.z;

    float scale = 1.0f / (a * (f * e - d * h) - b * (g * e - d * i) + c * (g * h - f * i));

    dest->v0.x = scale * (f * e - d * h);
    dest->v0.y = scale * (c * h - b * e);
    dest->v0.z = scale * (b * d - c * f);
    dest->v0.w = 0.0f;
    dest->v1.x = scale * (d * i - g * e);
    dest->v1.y = scale * (a * e - c * i);
    dest->v1.z = scale * (c * g - a * d);
    dest->v1.w = 0.0f;
    dest->v2.x = scale * (g * h - f * i);
    dest->v2.y = scale * (b * i - a * h);
    dest->v2.z = scale * (a * f - b * g);
    dest->v2.w = 0.0f;
    dest->v3.x = -(dest->v0.x * j + dest->v1.x * k + dest->v2.x * l);
    dest->v3.y = -(dest->v0.y * j + dest->v1.y * k + dest->v2.y * l);
    dest->v3.z = -(dest->v0.z * j + dest->v1.z * k + dest->v2.z * l);
    dest->v3.w = 1.0f;
#else
    eInvertMatrix(dest, const_cast<bMatrix4 *>(src));
#endif
}
#endif

// Preserve the EE floating-point operation order from both PS2 retail builds.
#ifdef EA_PLATFORM_PLAYSTATION2
float fDeterminant(bMatrix4 *m);
asm(
    ".text\n\t"
    ".set noreorder\n\t"
    ".set nomacro\n\t"
    ".globl fDeterminant__FP8bMatrix4\n\t"
    ".ent fDeterminant__FP8bMatrix4\n\t"
    "fDeterminant__FP8bMatrix4:\n\t"
    "addiu $29, $29, -48\n\t"
    "lwc1 $f9, 0($4)\n\t"
    "swc1 $f25, 40($29)\n\t"
    "swc1 $f24, 32($29)\n\t"
    "swc1 $f23, 24($29)\n\t"
    "swc1 $f22, 16($29)\n\t"
    "swc1 $f20, 0($29)\n\t"
    "swc1 $f21, 8($29)\n\t"
    "lwc1 $f21, 60($4)\n\t"
    "lwc1 $f10, 12($4)\n\t"
    "lwc1 $f2, 48($4)\n\t"
    "lwc1 $f19, 36($4)\n\t"
    "lwc1 $f14, 24($4)\n\t"
    "mul.s $f4, $f10, $f2\n\t"
    "lwc1 $f13, 8($4)\n\t"
    "lwc1 $f7, 28($4)\n\t"
    "mul.s $f23, $f19, $f14\n\t"
    "mul.s $f6, $f13, $f2\n\t"
    "lwc1 $f18, 40($4)\n\t"
    "mul.s $f20, $f19, $f7\n\t"
    "lwc1 $f17, 20($4)\n\t"
    "mul.s $f0, $f4, $f23\n\t"
    "lwc1 $f11, 4($4)\n\t"
    "mul.s $f25, $f18, $f17\n\t"
    "lwc1 $f15, 44($4)\n\t"
    "mul.s $f1, $f6, $f20\n\t"
    "lwc1 $f3, 52($4)\n\t"
    "mul.s $f12, $f18, $f7\n\t"
    "lwc1 $f8, 32($4)\n\t"
    "mul.s $f2, $f11, $f2\n\t"
    "lwc1 $f16, 16($4)\n\t"
    "mul.s $f4, $f4, $f25\n\t"
    "lwc1 $f5, 56($4)\n\t"
    "sub.s $f0, $f0, $f1\n\t"
    "mul.s $f24, $f15, $f17\n\t"
    "mul.s $f1, $f2, $f12\n\t"
    "sub.s $f0, $f0, $f4\n\t"
    "mul.s $f22, $f15, $f14\n\t"
    "mul.s $f6, $f6, $f24\n\t"
    "add.s $f0, $f0, $f1\n\t"
    "mul.s $f2, $f2, $f22\n\t"
    "mul.s $f1, $f10, $f3\n\t"
    "add.s $f0, $f0, $f6\n\t"
    "mul.s $f14, $f8, $f14\n\t"
    "mul.s $f4, $f13, $f3\n\t"
    "sub.s $f0, $f0, $f2\n\t"
    "mul.s $f2, $f1, $f14\n\t"
    "mul.s $f7, $f8, $f7\n\t"
    "mul.s $f18, $f18, $f16\n\t"
    "sub.s $f0, $f0, $f2\n\t"
    "mul.s $f6, $f4, $f7\n\t"
    "mul.s $f3, $f9, $f3\n\t"
    "mul.s $f1, $f1, $f18\n\t"
    "add.s $f0, $f0, $f6\n\t"
    "mul.s $f15, $f15, $f16\n\t"
    "mul.s $f12, $f3, $f12\n\t"
    "add.s $f0, $f0, $f1\n\t"
    "mul.s $f4, $f4, $f15\n\t"
    "mul.s $f8, $f8, $f17\n\t"
    "sub.s $f0, $f0, $f12\n\t"
    "mul.s $f3, $f3, $f22\n\t"
    "mul.s $f10, $f10, $f5\n\t"
    "lwc1 $f22, 16($29)\n\t"
    "sub.s $f0, $f0, $f4\n\t"
    "mul.s $f1, $f11, $f5\n\t"
    "mul.s $f2, $f10, $f8\n\t"
    "add.s $f0, $f0, $f3\n\t"
    "mul.s $f19, $f19, $f16\n\t"
    "mul.s $f7, $f1, $f7\n\t"
    "add.s $f0, $f0, $f2\n\t"
    "mul.s $f5, $f9, $f5\n\t"
    "mul.s $f10, $f10, $f19\n\t"
    "sub.s $f0, $f0, $f7\n\t"
    "mul.s $f20, $f5, $f20\n\t"
    "mul.s $f1, $f1, $f15\n\t"
    "sub.s $f0, $f0, $f10\n\t"
    "mul.s $f5, $f5, $f24\n\t"
    "mul.s $f13, $f13, $f21\n\t"
    "lwc1 $f24, 32($29)\n\t"
    "add.s $f0, $f0, $f20\n\t"
    "mul.s $f11, $f11, $f21\n\t"
    "lwc1 $f20, 0($29)\n\t"
    "mul.s $f8, $f13, $f8\n\t"
    "add.s $f0, $f0, $f1\n\t"
    "mul.s $f14, $f11, $f14\n\t"
    "mul.s $f9, $f9, $f21\n\t"
    "sub.s $f0, $f0, $f5\n\t"
    "lwc1 $f21, 8($29)\n\t"
    "mul.s $f13, $f13, $f19\n\t"
    "mul.s $f23, $f9, $f23\n\t"
    "sub.s $f0, $f0, $f8\n\t"
    "mul.s $f11, $f11, $f18\n\t"
    "mul.s $f9, $f9, $f25\n\t"
    "add.s $f0, $f0, $f14\n\t"
    "lwc1 $f25, 40($29)\n\t"
    "add.s $f0, $f0, $f13\n\t"
    "sub.s $f0, $f0, $f23\n\t"
    "lwc1 $f23, 24($29)\n\t"
    "sub.s $f0, $f0, $f11\n\t"
    "add.s $f0, $f0, $f9\n\t"
    "jr $31\n\t"
    "addiu $29, $29, 48\n\t"
    ".end fDeterminant__FP8bMatrix4\n\t"
    ".set macro\n\t"
    ".set reorder\n\t");
#else
float fDeterminant(bMatrix4 *m) {
    float value =
        m->v0.x * m->v1.y * m->v2.z * m->v3.w +
        (((m->v0.z * m->v1.x * m->v2.y * m->v3.w + m->v0.y * m->v1.z * m->v2.x * m->v3.w +
           (((m->v0.y * m->v1.x * m->v2.w * m->v3.z + m->v0.x * m->v1.w * m->v2.y * m->v3.z +
              (((m->v0.w * m->v1.y * m->v2.x * m->v3.z + m->v0.x * m->v1.z * m->v2.w * m->v3.y +
                 (((m->v0.w * m->v1.x * m->v2.z * m->v3.y + m->v0.z * m->v1.w * m->v2.x * m->v3.y +
                    (((m->v0.z * m->v1.y * m->v2.w * m->v3.x + m->v0.y * m->v1.w * m->v2.z * m->v3.x +
                       ((m->v0.w * m->v1.z * m->v2.y * m->v3.x - m->v0.z * m->v1.w * m->v2.y * m->v3.x) - m->v0.w * m->v1.y * m->v2.z * m->v3.x)) -
                      m->v0.y * m->v1.z * m->v2.w * m->v3.x) -
                     m->v0.w * m->v1.z * m->v2.x * m->v3.y)) -
                   m->v0.x * m->v1.w * m->v2.z * m->v3.y) -
                  m->v0.z * m->v1.x * m->v2.w * m->v3.y)) -
                m->v0.y * m->v1.w * m->v2.x * m->v3.z) -
               m->v0.w * m->v1.x * m->v2.y * m->v3.z)) -
             m->v0.x * m->v1.y * m->v2.w * m->v3.z) -
            m->v0.z * m->v1.y * m->v2.x * m->v3.w)) -
          m->v0.x * m->v1.z * m->v2.y * m->v3.w) -
         m->v0.y * m->v1.x * m->v2.z * m->v3.w);

    return value;
}
#endif

void fInvertMatrix(bMatrix4 *d, bMatrix4 *s) {
#ifdef EA_PLATFORM_WIN32
    float scale = 1.0f / fDeterminant(s);

    d->v0.x = (s->v1.y * s->v3.w * s->v2.z +
               (((s->v2.y * s->v3.z * s->v1.w +
                  (s->v2.w * s->v3.y * s->v1.z - s->v3.y * s->v2.z * s->v1.w)) -
                 s->v1.y * s->v2.w * s->v3.z) -
                s->v2.y * s->v3.w * s->v1.z)) *
              scale;
    d->v0.y = scale * (((((s->v0.w * (s->v2.z * s->v3.y) - s->v0.z * (s->v3.y * s->v2.w)) - s->v0.w * (s->v3.z * s->v2.y)) + s->v0.z * (s->v3.w * s->v2.y)) + s->v0.y * (s->v3.z * s->v2.w)) - s->v2.z * (s->v3.w * s->v0.y));
    d->v0.z = scale * (((((s->v0.z * (s->v1.w * s->v3.y) - s->v0.w * (s->v1.z * s->v3.y)) + s->v0.w * (s->v3.z * s->v1.y)) - s->v1.w * (s->v0.y * s->v3.z)) - s->v0.z * (s->v3.w * s->v1.y)) + s->v1.z * (s->v3.w * s->v0.y));
    d->v0.w = scale * (((((s->v0.w * (s->v1.z * s->v2.y) - s->v0.z * (s->v1.w * s->v2.y)) - s->v0.w * (s->v2.z * s->v1.y)) + s->v0.z * (s->v2.w * s->v1.y)) + s->v1.w * (s->v2.z * s->v0.y)) - s->v1.z * (s->v0.y * s->v2.w));
    d->v1.x = scale * (((((s->v1.w * (s->v2.z * s->v3.x) - s->v1.z * (s->v2.w * s->v3.x)) - s->v1.w * (s->v3.z * s->v2.x)) + s->v3.z * (s->v2.w * s->v1.x)) + s->v1.z * (s->v3.w * s->v2.x)) - s->v2.z * (s->v3.w * s->v1.x));
    d->v1.y = scale * (((((s->v0.z * (s->v2.w * s->v3.x) - s->v0.w * (s->v2.z * s->v3.x)) + s->v0.w * (s->v3.z * s->v2.x)) - s->v0.x * (s->v3.z * s->v2.w)) - s->v0.z * (s->v3.w * s->v2.x)) + s->v2.z * (s->v3.w * s->v0.x));
    d->v1.z = scale * (((((s->v0.w * (s->v1.z * s->v3.x) - s->v0.z * (s->v1.w * s->v3.x)) - s->v0.w * (s->v3.z * s->v1.x)) + s->v0.z * (s->v3.w * s->v1.x)) + s->v1.w * (s->v0.x * s->v3.z)) - s->v1.z * (s->v3.w * s->v0.x));
    d->v1.w = scale * (((((s->v0.z * (s->v1.w * s->v2.x) - s->v0.w * (s->v1.z * s->v2.x)) + s->v0.w * (s->v2.z * s->v1.x)) - s->v1.w * (s->v2.z * s->v0.x)) - s->v0.z * (s->v2.w * s->v1.x)) + s->v1.z * (s->v0.x * s->v2.w));
    d->v2.x = scale * (((((s->v2.w * (s->v3.x * s->v1.y) - s->v1.w * (s->v2.y * s->v3.x)) + s->v1.w * (s->v3.y * s->v2.x)) - s->v3.y * (s->v2.w * s->v1.x)) - s->v3.w * (s->v2.x * s->v1.y)) + s->v3.w * (s->v2.y * s->v1.x));
    d->v2.y = scale * (((((s->v0.w * (s->v2.y * s->v3.x) - s->v0.y * (s->v2.w * s->v3.x)) - s->v0.w * (s->v3.y * s->v2.x)) + s->v0.x * (s->v3.y * s->v2.w)) + s->v3.w * (s->v0.y * s->v2.x)) - s->v3.w * (s->v0.x * s->v2.y));
    d->v2.z = scale * (((((s->v1.w * (s->v0.y * s->v3.x) - s->v0.w * (s->v3.x * s->v1.y)) + s->v0.w * (s->v3.y * s->v1.x)) - s->v1.w * (s->v0.x * s->v3.y)) - s->v3.w * (s->v0.y * s->v1.x)) + s->v3.w * (s->v0.x * s->v1.y));
    d->v2.w = scale * (((((s->v0.w * (s->v2.x * s->v1.y) - s->v1.w * (s->v0.y * s->v2.x)) - s->v0.w * (s->v2.y * s->v1.x)) + s->v1.w * (s->v0.x * s->v2.y)) + s->v0.y * (s->v2.w * s->v1.x)) - s->v0.x * (s->v2.w * s->v1.y));
    d->v3.x = scale * (((((s->v1.z * (s->v2.y * s->v3.x) - s->v2.z * (s->v3.x * s->v1.y)) - s->v1.z * (s->v3.y * s->v2.x)) + s->v3.z * (s->v2.x * s->v1.y)) + s->v2.z * (s->v3.y * s->v1.x)) - s->v3.z * (s->v2.y * s->v1.x));
    d->v3.y = scale * (((((s->v2.z * (s->v0.y * s->v3.x) - s->v0.z * (s->v2.y * s->v3.x)) + s->v0.z * (s->v3.y * s->v2.x)) - s->v2.z * (s->v0.x * s->v3.y)) - s->v0.y * (s->v3.z * s->v2.x)) + s->v0.x * (s->v3.z * s->v2.y));
    d->v3.z = scale * (((((s->v0.z * (s->v3.x * s->v1.y) - s->v1.z * (s->v0.y * s->v3.x)) - s->v0.z * (s->v3.y * s->v1.x)) + s->v1.z * (s->v0.x * s->v3.y)) + s->v0.y * (s->v3.z * s->v1.x)) - s->v0.x * (s->v3.z * s->v1.y));
    d->v3.w = scale * (((((s->v1.z * (s->v0.y * s->v2.x) - s->v0.z * (s->v2.x * s->v1.y)) + s->v0.z * (s->v2.y * s->v1.x)) - s->v1.z * (s->v0.x * s->v2.y)) - s->v2.z * (s->v0.y * s->v1.x)) + s->v2.z * (s->v0.x * s->v1.y));
#else
    float scale = 1.0f / fDeterminant(s);

    d->v0.x = scale * (((((s->v1.z * s->v2.w * s->v3.y - s->v1.w * s->v2.z * s->v3.y) + s->v1.w * s->v2.y * s->v3.z) - s->v1.y * s->v2.w * s->v3.z) -
                        s->v1.z * s->v2.y * s->v3.w) +
                       s->v1.y * s->v2.z * s->v3.w);
    d->v0.y = scale * ((((s->v0.w * s->v2.z * s->v3.y - s->v0.z * s->v2.w * s->v3.y) - s->v0.w * s->v2.y * s->v3.z) + s->v0.y * s->v2.w * s->v3.z +
                        s->v0.z * s->v2.y * s->v3.w) -
                       s->v0.y * s->v2.z * s->v3.w);
    d->v0.z = scale * (((((s->v0.z * s->v1.w * s->v3.y - s->v0.w * s->v1.z * s->v3.y) + s->v0.w * s->v1.y * s->v3.z) - s->v0.y * s->v1.w * s->v3.z) -
                        s->v0.z * s->v1.y * s->v3.w) +
                       s->v0.y * s->v1.z * s->v3.w);
    d->v0.w = scale * ((((s->v0.w * s->v1.z * s->v2.y - s->v0.z * s->v1.w * s->v2.y) - s->v0.w * s->v1.y * s->v2.z) + s->v0.y * s->v1.w * s->v2.z +
                        s->v0.z * s->v1.y * s->v2.w) -
                       s->v0.y * s->v1.z * s->v2.w);
    d->v1.x = scale * ((((s->v1.w * s->v2.z * s->v3.x - s->v1.z * s->v2.w * s->v3.x) - s->v1.w * s->v2.x * s->v3.z) + s->v1.x * s->v2.w * s->v3.z +
                        s->v1.z * s->v2.x * s->v3.w) -
                       s->v1.x * s->v2.z * s->v3.w);
    d->v1.y = scale * (((((s->v0.z * s->v2.w * s->v3.x - s->v0.w * s->v2.z * s->v3.x) + s->v0.w * s->v2.x * s->v3.z) - s->v0.x * s->v2.w * s->v3.z) -
                        s->v0.z * s->v2.x * s->v3.w) +
                       s->v0.x * s->v2.z * s->v3.w);
    d->v1.z = scale * ((((s->v0.w * s->v1.z * s->v3.x - s->v0.z * s->v1.w * s->v3.x) - s->v0.w * s->v1.x * s->v3.z) + s->v0.x * s->v1.w * s->v3.z +
                        s->v0.z * s->v1.x * s->v3.w) -
                       s->v0.x * s->v1.z * s->v3.w);
    d->v1.w = scale * (((((s->v0.z * s->v1.w * s->v2.x - s->v0.w * s->v1.z * s->v2.x) + s->v0.w * s->v1.x * s->v2.z) - s->v0.x * s->v1.w * s->v2.z) -
                        s->v0.z * s->v1.x * s->v2.w) +
                       s->v0.x * s->v1.z * s->v2.w);
    d->v2.x = scale * (((((s->v1.y * s->v2.w * s->v3.x - s->v1.w * s->v2.y * s->v3.x) + s->v1.w * s->v2.x * s->v3.y) - s->v1.x * s->v2.w * s->v3.y) -
                        s->v1.y * s->v2.x * s->v3.w) +
                       s->v1.x * s->v2.y * s->v3.w);
    d->v2.y = scale * ((((s->v0.w * s->v2.y * s->v3.x - s->v0.y * s->v2.w * s->v3.x) - s->v0.w * s->v2.x * s->v3.y) + s->v0.x * s->v2.w * s->v3.y +
                        s->v0.y * s->v2.x * s->v3.w) -
                       s->v0.x * s->v2.y * s->v3.w);
    d->v2.z = scale * (((((s->v0.y * s->v1.w * s->v3.x - s->v0.w * s->v1.y * s->v3.x) + s->v0.w * s->v1.x * s->v3.y) - s->v0.x * s->v1.w * s->v3.y) -
                        s->v0.y * s->v1.x * s->v3.w) +
                       s->v0.x * s->v1.y * s->v3.w);
    d->v2.w = scale * ((((s->v0.w * s->v1.y * s->v2.x - s->v0.y * s->v1.w * s->v2.x) - s->v0.w * s->v1.x * s->v2.y) + s->v0.x * s->v1.w * s->v2.y +
                        s->v0.y * s->v1.x * s->v2.w) -
                       s->v0.x * s->v1.y * s->v2.w);
    d->v3.x = scale * ((((s->v1.z * s->v2.y * s->v3.x - s->v1.y * s->v2.z * s->v3.x) - s->v1.z * s->v2.x * s->v3.y) + s->v1.x * s->v2.z * s->v3.y +
                        s->v1.y * s->v2.x * s->v3.z) -
                       s->v1.x * s->v2.y * s->v3.z);
    d->v3.y = scale * (((((s->v0.y * s->v2.z * s->v3.x - s->v0.z * s->v2.y * s->v3.x) + s->v0.z * s->v2.x * s->v3.y) - s->v0.x * s->v2.z * s->v3.y) -
                        s->v0.y * s->v2.x * s->v3.z) +
                       s->v0.x * s->v2.y * s->v3.z);
    d->v3.z = scale * ((((s->v0.z * s->v1.y * s->v3.x - s->v0.y * s->v1.z * s->v3.x) - s->v0.z * s->v1.x * s->v3.y) + s->v0.x * s->v1.z * s->v3.y +
                        s->v0.y * s->v1.x * s->v3.z) -
                       s->v0.x * s->v1.y * s->v3.z);
    d->v3.w = scale * (((((s->v0.y * s->v1.z * s->v2.x - s->v0.z * s->v1.y * s->v2.x) + s->v0.z * s->v1.x * s->v2.y) - s->v0.x * s->v1.z * s->v2.y) -
                        s->v0.y * s->v1.x * s->v2.z) +
                       s->v0.x * s->v1.y * s->v2.z);
#endif
}

void hermite_basis(bMatrix4 *b, bMatrix4 *p, float u1, float u2, float u3, float u4) {
#ifdef EA_PLATFORM_WIN32
    // These matrices are completely populated before they are read.  The retail
    // PC function reserves five aligned matrices and does not run the identity
    // constructor for them.
    struct ATTRIBUTE_ALIGN(16) RawMatrix4 {
        bVector4 v0;
        bVector4 v1;
        bVector4 v2;
        bVector4 v3;
    };

    RawMatrix4 raw_U;
    RawMatrix4 raw_iU;
    RawMatrix4 raw_Mf;
    RawMatrix4 raw_iMf;
    RawMatrix4 raw_Nf;
    bMatrix4 &U = *reinterpret_cast<bMatrix4 *>(&raw_U);
    bMatrix4 &iU = *reinterpret_cast<bMatrix4 *>(&raw_iU);
    bMatrix4 &Mf = *reinterpret_cast<bMatrix4 *>(&raw_Mf);
    bMatrix4 &iMf = *reinterpret_cast<bMatrix4 *>(&raw_iMf);
    bMatrix4 &Nf = *reinterpret_cast<bMatrix4 *>(&raw_Nf);
#else
    bMatrix4 U;
    bMatrix4 iU;
    bMatrix4 Mf;
    bMatrix4 iMf;
    bMatrix4 K;
    bMatrix4 Nf;
#endif

    Mf.v0.x = 2.0f;
    Mf.v0.y = -2.0f;
    Mf.v0.z = 1.0f;
    Mf.v0.w = 1.0f;
    Mf.v1.x = -3.0f;
    Mf.v1.y = 3.0f;
    Mf.v1.z = -2.0f;
    Mf.v1.w = -1.0f;
    Mf.v2.x = 0.0f;
    Mf.v2.y = 0.0f;
    Mf.v2.z = 1.0f;
    Mf.v2.w = 0.0f;
    Mf.v3.x = 1.0f;
    Mf.v3.y = 0.0f;
    Mf.v3.z = 0.0f;
    Mf.v3.w = 0.0f;

    iMf.v0.x = 0.0f;
    iMf.v0.y = 0.0f;
    iMf.v0.z = 0.0f;
    iMf.v0.w = 1.0f;
    iMf.v1.x = 1.0f;
    iMf.v1.y = 1.0f;
    iMf.v1.z = 1.0f;
    iMf.v1.w = 1.0f;
    iMf.v2.x = 0.0f;
    iMf.v2.y = 0.0f;
    iMf.v2.z = 1.0f;
    iMf.v2.w = 0.0f;
    iMf.v3.x = 3.0f;
    iMf.v3.y = 2.0f;
    iMf.v3.z = 1.0f;
    iMf.v3.w = 0.0f;

    U.v0.x = u1 * u1 * u1;
    U.v0.y = u1 * u1;
    U.v0.z = u1;
    U.v0.w = 1.0f;
    U.v1.x = u2 * u2 * u2;
    U.v1.y = u2 * u2;
    U.v1.z = u2;
    U.v1.w = 1.0f;
    U.v2.x = u3 * u3 * u3;
    U.v2.y = u3 * u3;
    U.v2.z = u3;
    U.v2.w = 1.0f;
    U.v3.x = u4 * u4 * u4;
    U.v3.y = u4 * u4;
    U.v3.z = u4;
    U.v3.w = 1.0f;

    fInvertMatrix(&iU, &U);
#ifdef EA_PLATFORM_WIN32
    eMulMatrix(&U, &iMf, &iU);
    eMulMatrix(&Nf, &Mf, &U);
#else
    eMulMatrix(&K, &iMf, &iU);
    eMulMatrix(&Nf, &Mf, &K);
#endif
    eMulMatrix(b, &Nf, p);
}

void hermite_parameter(bVector4 *dest, const bMatrix4 *b, float t) {
    ATTRIBUTE_ALIGN(16) bVector4 u;

    u.x = t * t * t;
    u.y = t * t;
    u.z = t;
    u.w = 1.0f;
    eMulVector(dest, b, &u);
}

#ifdef EA_PLATFORM_PLAYSTATION2
void bMulMatrix(bVector4 *dest, const bMatrix4 *m, const bVector4 *v);

// Retail EE/VU0 paths. Load every input before storing, including when dest
// aliases an input. The 3D path uses vf0.w == 1 and only updates xyz; its
// quadword store deliberately retains vf12's unspecified padding lane.
asm(
    ".text\n\t"
    ".set noreorder\n\t"
    ".set nomacro\n\t"
    ".align 3\n\t"
    ".globl bMulMatrix__FP8bMatrix4PC8bMatrix4T1\n\t"
    ".ent bMulMatrix__FP8bMatrix4PC8bMatrix4T1\n\t"
    "bMulMatrix__FP8bMatrix4PC8bMatrix4T1:\n\t"
    "lqc2 vf4, 0($6)\n\t"
    "lqc2 vf5, 16($6)\n\t"
    "lqc2 vf6, 32($6)\n\t"
    "lqc2 vf7, 48($6)\n\t"
    "lqc2 vf8, 0($5)\n\t"
    "lqc2 vf9, 16($5)\n\t"
    "lqc2 vf10, 32($5)\n\t"
    "lqc2 vf11, 48($5)\n\t"
    "vmulax.xyzw ACC, vf8, vf4x\n\t"
    "vmadday.xyzw ACC, vf9, vf4y\n\t"
    "vmaddaz.xyzw ACC, vf10, vf4z\n\t"
    "vmaddw.xyzw vf12, vf11, vf4w\n\t"
    "vmulax.xyzw ACC, vf8, vf5x\n\t"
    "vmadday.xyzw ACC, vf9, vf5y\n\t"
    "vmaddaz.xyzw ACC, vf10, vf5z\n\t"
    "vmaddw.xyzw vf13, vf11, vf5w\n\t"
    "vmulax.xyzw ACC, vf8, vf6x\n\t"
    "vmadday.xyzw ACC, vf9, vf6y\n\t"
    "vmaddaz.xyzw ACC, vf10, vf6z\n\t"
    "vmaddw.xyzw vf14, vf11, vf6w\n\t"
    "vmulax.xyzw ACC, vf8, vf7x\n\t"
    "vmadday.xyzw ACC, vf9, vf7y\n\t"
    "vmaddaz.xyzw ACC, vf10, vf7z\n\t"
    "vmaddw.xyzw vf15, vf11, vf7w\n\t"
    "sqc2 vf12, 0($4)\n\t"
    "sqc2 vf13, 16($4)\n\t"
    "sqc2 vf14, 32($4)\n\t"
    "sqc2 vf15, 48($4)\n\t"
    "jr $31\n\t"
    "nop\n\t"
    ".end bMulMatrix__FP8bMatrix4PC8bMatrix4T1\n\t"
    ".align 3\n\t"
    ".globl bMulMatrix__FP8bVector4PC8bMatrix4PC8bVector4\n\t"
    ".ent bMulMatrix__FP8bVector4PC8bMatrix4PC8bVector4\n\t"
    "bMulMatrix__FP8bVector4PC8bMatrix4PC8bVector4:\n\t"
    "lqc2 vf4, 0($6)\n\t"
    "lqc2 vf8, 0($5)\n\t"
    "lqc2 vf9, 16($5)\n\t"
    "lqc2 vf10, 32($5)\n\t"
    "lqc2 vf11, 48($5)\n\t"
    "vmulax.xyzw ACC, vf8, vf4x\n\t"
    "vmadday.xyzw ACC, vf9, vf4y\n\t"
    "vmaddaz.xyzw ACC, vf10, vf4z\n\t"
    "vmaddw.xyzw vf12, vf11, vf4w\n\t"
    "vnop\n\t"
    "vnop\n\t"
    "vnop\n\t"
    "sqc2 vf12, 0($4)\n\t"
    "jr $31\n\t"
    "nop\n\t"
    ".end bMulMatrix__FP8bVector4PC8bMatrix4PC8bVector4\n\t"
    ".align 3\n\t"
    ".globl bMulMatrix__FP8bVector3PC8bMatrix4PC8bVector3\n\t"
    ".ent bMulMatrix__FP8bVector3PC8bMatrix4PC8bVector3\n\t"
    "bMulMatrix__FP8bVector3PC8bMatrix4PC8bVector3:\n\t"
    "lqc2 vf4, 0($6)\n\t"
    "lqc2 vf8, 0($5)\n\t"
    "lqc2 vf9, 16($5)\n\t"
    "lqc2 vf10, 32($5)\n\t"
    "lqc2 vf11, 48($5)\n\t"
    "vmulax.xyz ACC, vf8, vf4x\n\t"
    "vmadday.xyz ACC, vf9, vf4y\n\t"
    "vmaddaz.xyz ACC, vf10, vf4z\n\t"
    "vmaddw.xyz vf12, vf11, vf0w\n\t"
    "vnop\n\t"
    "vnop\n\t"
    "vnop\n\t"
    "sqc2 vf12, 0($4)\n\t"
    "jr $31\n\t"
    "nop\n\t"
    ".end bMulMatrix__FP8bVector3PC8bMatrix4PC8bVector3\n\t"
    ".align 3\n\t"
    ".globl bTransposeMatrix__FP8bMatrix4PC8bMatrix4\n\t"
    ".ent bTransposeMatrix__FP8bMatrix4PC8bMatrix4\n\t"
    "bTransposeMatrix__FP8bMatrix4PC8bMatrix4:\n\t"
    "lq $7, 0($5)\n\t"
    "lq $2, 16($5)\n\t"
    "lq $9, 48($5)\n\t"
    "lq $8, 32($5)\n\t"
    "pextuw $6, $2, $7\n\t"
    "pextlw $3, $9, $8\n\t"
    "pextlw $2, $2, $7\n\t"
    "pextuw $5, $9, $8\n\t"
    "pcpyld $7, $3, $2\n\t"
    "pcpyld $8, $5, $6\n\t"
    "sq $7, 0($4)\n\t"
    "pcpyud $2, $2, $3\n\t"
    "sq $2, 16($4)\n\t"
    "pcpyud $9, $6, $5\n\t"
    "sq $8, 32($4)\n\t"
    "daddu $2, $4, $0\n\t"
    "sq $9, 48($4)\n\t"
    "jr $31\n\t"
    "nop\n\t"
    ".end bTransposeMatrix__FP8bMatrix4PC8bMatrix4\n\t"
    ".set macro\n\t"
    ".set reorder\n\t");
#else
void bMulMatrix(bMatrix4 *dest, const bMatrix4 *a, const bMatrix4 *b) {
    eMulMatrix(dest, const_cast<bMatrix4 *>(b), const_cast<bMatrix4 *>(a));
}

void bMulMatrix(bVector4 *dest, const bMatrix4 *m, const bVector4 *v) {
    eMulVector(dest, m, v);
}

void bMulMatrix(bVector3 *dest, const bMatrix4 *m, const bVector3 *v) {
    eMulVector(dest, m, v);
}

bMatrix4 *bTransposeMatrix(bMatrix4 *dest, const bMatrix4 *m) {
#ifdef EA_PLATFORM_GAMECUBE
    MTX44Transpose(*reinterpret_cast<const Mtx44 *>(m), *reinterpret_cast<Mtx44 *>(dest));
#elif defined(EA_PLATFORM_WIN32)
    D3DXMatrixTranspose(dest, m);
#else
    float transposed[4][4];
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            transposed[row][column] = (*m)[column][row];
        }
    }
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            (*dest)[row][column] = transposed[row][column];
        }
    }
#endif
    return dest;
}
#endif
