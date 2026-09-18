
#include "UVectorMath.h"
#include "UMath.h"
#include "UEALibs.hpp"

// Paired-single support (SN Systems). r77: el modo `PS` es de GameCube.
#ifdef EA_PLATFORM_GAMECUBE
typedef float sn_ps __attribute__((mode(PS)));

#define V2(v) (*(sn_ps *)&(v))
#endif

// Decl: GC MW: UVectorMath.cpp: 312
// De aqui al `#endif` esta la implementacion de GameCube (paired-singles: modo
// `PS` y __builtin_ps_muls0). PS2 tiene la suya en UVectorMath.hpp, con
// ensamblador VU0, y compilar las dos da 25 redefiniciones. VU0_Atan2 y los dos
// estaticos del final SI son comunes (en PS2 tambien sale de este .cpp) y quedan
// fuera de la guarda.
#ifdef EA_PLATFORM_GAMECUBE
void VU0_v3crossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest) {
    float t0, t1, t2, t3, t4, t5, t6;
    asm volatile (
        "psq_l %1, 0(%8), 0, 0\n\t"
        "lfs %2, 8(%7)\n\t"
        "psq_l %0, 0(%7), 0, 0\n\t"
        "ps_merge10 %6, %1, %1\n\t"
        "ps_mul %4, %1, %2\n\t"
        "lfs %3, 8(%8)\n\t"
        "ps_muls0 %1, %1, %0\n\t"
        "ps_msub %2, %0, %3, %4\n\t"
        "ps_msub %3, %0, %6, %1\n\t"
        "ps_merge11 %0, %2, %2\n\t"
        "ps_merge01 %1, %2, %3\n\t"
        "stfs %0, 0(%9)\n\t"
        "ps_neg %1, %1\n\t"
        "psq_st %1, 4(%9), 0, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6)
        : "b"(&a), "b"(&b), "b"(&dest));
}

// Decl: GC MW: UVectorMath.cpp: 333
void VU0_v4crossprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &dest) {
    float t0, t1, t2, t3, t4, t5, t6;
    asm volatile (
        "psq_l %1, 0(%8), 0, 0\n\t"
        "lfs %2, 8(%7)\n\t"
        "psq_l %0, 0(%7), 0, 0\n\t"
        "ps_merge10 %6, %1, %1\n\t"
        "ps_mul %4, %1, %2\n\t"
        "lfs %3, 8(%8)\n\t"
        "ps_muls0 %1, %1, %0\n\t"
        "ps_msub %2, %0, %3, %4\n\t"
        "ps_msub %3, %0, %6, %1\n\t"
        "ps_merge11 %0, %2, %2\n\t"
        "ps_merge01 %1, %2, %3\n\t"
        "stfs %0, 0(%9)\n\t"
        "ps_neg %1, %1\n\t"
        "psq_st %1, 4(%9), 0, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6)
        : "b"(&a), "b"(&b), "b"(&dest));
}

// Decl: GC MW: UVectorMath.cpp: 390
float VU0_v3dotprod(const UMath::Vector3 &a, const UMath::Vector3 &b) {
    float t0, t1, t2, t3, result;
    asm (
        "psq_l %1, 0(%5), 0, 0\n\t"
        "psq_l %2, 0(%6), 0, 0\n\t"
        "ps_mul %1, %1, %2\n\t"
        "lfs %4, 8(%5)\n\t"
        "lfs %3, 8(%6)\n\t"
        "ps_sum0 %1, %1, %1, %1\n\t"
        "ps_madd %0, %4, %3, %1"
        : "=f"(result), "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3)
        : "b"(&a), "b"(&b));
    return result;
}

// Decl: GC MW: UVectorMath.cpp: 407
float VU0_v4dotprod(const UMath::Vector4 &a, const UMath::Vector4 &b) {
    float t0, t1, t2, t3, result;
    asm (
        "psq_l %1, 0(%5), 0, 0\n\t"
        "psq_l %2, 0(%6), 0, 0\n\t"
        "ps_mul %1, %1, %2\n\t"
        "psq_l %4, 8(%5), 0, 0\n\t"
        "psq_l %3, 8(%6), 0, 0\n\t"
        "ps_madd %2, %4, %3, %1\n\t"
        "ps_sum0 %0, %2, %2, %2"
        : "=f"(result), "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3)
        : "b"(&a), "b"(&b));
    return result;
}

// Decl: GC MW: UVectorMath.cpp: 428
float VU0_v4dotprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b) {
    float t0, t1, t2, t3, result;
    asm (
        "psq_l %1, 0(%5), 0, 0\n\t"
        "psq_l %2, 0(%6), 0, 0\n\t"
        "ps_mul %1, %1, %2\n\t"
        "lfs %4, 8(%5)\n\t"
        "lfs %3, 8(%6)\n\t"
        "ps_sum0 %1, %1, %1, %1\n\t"
        "ps_madd %0, %4, %3, %1"
        : "=f"(result), "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3)
        : "b"(&a), "b"(&b));
    return result;
}

// Decl: GC MW: UVectorMath.cpp: 453
void VU0_v3add(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    float t0, t1, t2, t3, t4, t5;
    asm volatile (
        "psq_l %0, 0(%6), 0, 0\n\t"
        "psq_l %1, 0(%7), 0, 0\n\t"
        "lfs %3, 8(%6)\n\t"
        "ps_add %2, %0, %1\n\t"
        "lfs %4, 8(%7)\n\t"
        "psq_st %2, 0(%8), 0, 0\n\t"
        "ps_add %5, %3, %4\n\t"
        "psq_st %5, 8(%8), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5)
        : "b"(&a), "b"(&b), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 468
void VU0_v4add(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    float t0, t1, t2, t3, t4, t5;
    asm volatile (
        "psq_l %0, 0(%6), 0, 0\n\t"
        "psq_l %1, 0(%7), 0, 0\n\t"
        "psq_l %3, 8(%6), 0, 0\n\t"
        "ps_add %2, %0, %1\n\t"
        "psq_l %4, 8(%7), 0, 0\n\t"
        "psq_st %2, 0(%8), 0, 0\n\t"
        "ps_add %5, %3, %4\n\t"
        "psq_st %5, 8(%8), 0, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5)
        : "b"(&a), "b"(&b), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 487
void VU0_v4addxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    float t0, t1, t2, t3, t4, t5;
    asm volatile (
        "psq_l %0, 0(%6), 0, 0\n\t"
        "psq_l %1, 0(%7), 0, 0\n\t"
        "lfs %3, 8(%6)\n\t"
        "ps_add %2, %0, %1\n\t"
        "lfs %4, 8(%7)\n\t"
        "psq_st %2, 0(%8), 0, 0\n\t"
        "ps_add %5, %3, %4\n\t"
        "psq_st %5, 8(%8), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5)
        : "b"(&a), "b"(&b), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 510
void VU0_v3sub(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    float t0, t1, t2, t3, t4, t5;
    asm volatile (
        "psq_l %0, 0(%6), 0, 0\n\t"
        "psq_l %1, 0(%7), 0, 0\n\t"
        "lfs %3, 8(%6)\n\t"
        "ps_sub %2, %0, %1\n\t"
        "lfs %4, 8(%7)\n\t"
        "psq_st %2, 0(%8), 0, 0\n\t"
        "ps_sub %5, %3, %4\n\t"
        "psq_st %5, 8(%8), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5)
        : "b"(&a), "b"(&b), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 525
void VU0_v4sub(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    float t0, t1, t2, t3, t4, t5;
    asm volatile (
        "psq_l %0, 0(%6), 0, 0\n\t"
        "psq_l %1, 0(%7), 0, 0\n\t"
        "psq_l %3, 8(%6), 0, 0\n\t"
        "ps_sub %2, %0, %1\n\t"
        "psq_l %4, 8(%7), 0, 0\n\t"
        "psq_st %2, 0(%8), 0, 0\n\t"
        "ps_sub %5, %3, %4\n\t"
        "psq_st %5, 8(%8), 0, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5)
        : "b"(&a), "b"(&b), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 544
void VU0_v4subxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    float t0, t1, t2, t3, t4, t5;
    asm volatile (
        "psq_l %0, 0(%6), 0, 0\n\t"
        "psq_l %1, 0(%7), 0, 0\n\t"
        "lfs %3, 8(%6)\n\t"
        "ps_sub %2, %0, %1\n\t"
        "lfs %4, 8(%7)\n\t"
        "psq_st %2, 0(%8), 0, 0\n\t"
        "ps_sub %5, %3, %4\n\t"
        "psq_st %5, 8(%8), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5)
        : "b"(&a), "b"(&b), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 567
void VU0_v3scale(const UMath::Vector3 &a, const float scaleby, UMath::Vector3 &result) {
    float t0, t1;
    asm volatile (
        "psq_l %0, 0(%2), 0, 0\n\t"
        "lfs %1, 8(%2)\n\t"
        "ps_muls0 %0, %0, %4\n\t"
        "ps_muls0 %1, %1, %4\n\t"
        "psq_st %0, 0(%3), 0, 0\n\t"
        "psq_st %1, 8(%3), 1, 0"
        : "=&f"(t0), "=&f"(t1)
        : "b"(&a), "b"(&result), "f"(scaleby));
}

// Decl: GC MW: UVectorMath.cpp: 580
void VU0_v3scale(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    float t0, t1, t2, t3;
    asm volatile (
        "psq_l %0, 0(%4), 0, 0\n\t"
        "psq_l %1, 0(%5), 0, 0\n\t"
        "lfs %2, 8(%4)\n\t"
        "lfs %3, 8(%5)\n\t"
        "ps_mul %0, %0, %1\n\t"
        "ps_muls0 %2, %2, %3\n\t"
        "psq_st %0, 0(%6), 0, 0\n\t"
        "psq_st %2, 8(%6), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3)
        : "b"(&a), "b"(&b), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 599
void VU0_v4scale(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result) {
    sn_ps zw = __builtin_ps_muls0(V2(a.z), (sn_ps)scaleby);
    sn_ps xy = __builtin_ps_muls0(V2(a.x), (sn_ps)scaleby);
    V2(result.x) = xy;
    V2(result.z) = zw;
}

// Decl: GC MW: UVectorMath.cpp: 615
void VU0_v4scalexyz(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result) {
    float t0, t1;
    asm volatile (
        "psq_l %0, 0(%2), 0, 0\n\t"
        "lfs %1, 8(%2)\n\t"
        "ps_muls0 %0, %0, %4\n\t"
        "ps_muls0 %1, %1, %4\n\t"
        "psq_st %0, 0(%3), 0, 0\n\t"
        "psq_st %1, 8(%3), 1, 0"
        : "=&f"(t0), "=&f"(t1)
        : "b"(&a), "b"(&result), "f"(scaleby));
}

// Decl: GC MW: UVectorMath.cpp: 634
void VU0_v4scalexyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {
    float t0, t1, t2, t3;
    asm volatile (
        "psq_l %0, 0(%4), 0, 0\n\t"
        "psq_l %1, 0(%5), 0, 0\n\t"
        "lfs %2, 8(%4)\n\t"
        "lfs %3, 8(%5)\n\t"
        "ps_mul %0, %0, %1\n\t"
        "ps_muls0 %2, %2, %3\n\t"
        "psq_st %0, 0(%6), 0, 0\n\t"
        "psq_st %2, 8(%6), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3)
        : "b"(&a), "b"(&b), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 676
void VU0_v3addscale(const UMath::Vector3 &a, const UMath::Vector3 &b, const float scaleby, UMath::Vector3 &result) {
    float t0, t1, t2, t3;
    asm volatile (
        "psq_l %0, 0(%4), 0, 0\n\t"
        "lfs %1, 8(%4)\n\t"
        "psq_l %2, 0(%5), 0, 0\n\t"
        "lfs %3, 8(%5)\n\t"
        "ps_add %0, %0, %2\n\t"
        "ps_add %1, %1, %3\n\t"
        "ps_muls0 %0, %0, %7\n\t"
        "ps_muls0 %1, %1, %7\n\t"
        "psq_st %0, 0(%6), 0, 0\n\t"
        "psq_st %1, 8(%6), 1, 0"
        : "=&f"(t0), "=&f"(t1), "=&f"(t2), "=&f"(t3)
        : "b"(&a), "b"(&b), "b"(&result), "f"(scaleby));
}

// Decl: GC MW: UVectorMath.cpp: 693
void VU0_v4addscale(const UMath::Vector4 &a, const UMath::Vector4 &b, const float scaleby, UMath::Vector4 &result) {
    float t0, t1, t2, t3;
    asm volatile (
        "psq_l %0, 0(%4), 0, 0\n\t"
        "psq_l %1, 8(%4), 0, 0\n\t"
        "psq_l %2, 0(%5), 0, 0\n\t"
        "psq_l %3, 8(%5), 0, 0\n\t"
        "ps_add %0, %0, %2\n\t"
        "ps_add %1, %1, %3\n\t"
        "ps_muls0 %0, %0, %7\n\t"
        "ps_muls0 %1, %1, %7\n\t"
        "psq_st %0, 0(%6), 0, 0\n\t"
        "psq_st %1, 8(%6), 0, 0"
        : "=&f"(t0), "=&f"(t1), "=&f"(t2), "=&f"(t3)
        : "b"(&a), "b"(&b), "b"(&result), "f"(scaleby));
}

// Decl: GC MW: UVectorMath.cpp: 715
void VU0_v4addscalexyz(const UMath::Vector4 &a, const UMath::Vector4 &b, const float scaleby, UMath::Vector4 &result) {
    float t0, t1, t2, t3;
    asm volatile (
        "psq_l %0, 0(%4), 0, 0\n\t"
        "lfs %1, 8(%4)\n\t"
        "psq_l %2, 0(%5), 0, 0\n\t"
        "lfs %3, 8(%5)\n\t"
        "ps_add %0, %0, %2\n\t"
        "ps_add %1, %1, %3\n\t"
        "ps_muls0 %0, %0, %7\n\t"
        "ps_muls0 %1, %1, %7\n\t"
        "psq_st %0, 0(%6), 0, 0\n\t"
        "psq_st %1, 8(%6), 1, 0"
        : "=&f"(t0), "=&f"(t1), "=&f"(t2), "=&f"(t3)
        : "b"(&a), "b"(&b), "b"(&result), "f"(scaleby));
}

// Decl: GC MW: UVectorMath.cpp: 742
void VU0_v3scaleadd(const UMath::Vector3 &a, const float scaleby, const UMath::Vector3 &b, UMath::Vector3 &result) {
    float t0, t1, t2, t3;
    asm volatile (
        "psq_l %0, 0(%4), 0, 0\n\t"
        "lfs %1, 8(%4)\n\t"
        "psq_l %2, 0(%5), 0, 0\n\t"
        "ps_muls0 %0, %0, %7\n\t"
        "ps_muls0 %1, %1, %7\n\t"
        "lfs %3, 8(%5)\n\t"
        "ps_add %0, %0, %2\n\t"
        "ps_add %1, %1, %3\n\t"
        "psq_st %0, 0(%6), 0, 0\n\t"
        "psq_st %1, 8(%6), 1, 0"
        : "=&f"(t0), "=&f"(t1), "=&f"(t2), "=&f"(t3)
        : "b"(&a), "b"(&b), "b"(&result), "f"(scaleby));
}

// Decl: GC MW: UVectorMath.cpp: 759
void VU0_v4scaleaddxyz(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result) {
    float t0, t1, t2, t3;
    asm volatile (
        "psq_l %0, 0(%4), 0, 0\n\t"
        "lfs %1, 8(%4)\n\t"
        "psq_l %2, 0(%5), 0, 0\n\t"
        "ps_muls0 %0, %0, %7\n\t"
        "ps_muls0 %1, %1, %7\n\t"
        "lfs %3, 8(%5)\n\t"
        "ps_add %0, %0, %2\n\t"
        "ps_add %1, %1, %3\n\t"
        "psq_st %0, 0(%6), 0, 0\n\t"
        "psq_st %1, 8(%6), 1, 0"
        : "=&f"(t0), "=&f"(t1), "=&f"(t2), "=&f"(t3)
        : "b"(&a), "b"(&b), "b"(&result), "f"(scaleby));
}

// Decl: GC MW: UVectorMath.cpp: 781
void VU0_v4scaleadd(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result) {
    float t0, t1, t2, t3;
    asm volatile (
        "psq_l %0, 0(%4), 0, 0\n\t"
        "psq_l %1, 8(%4), 0, 0\n\t"
        "psq_l %2, 0(%5), 0, 0\n\t"
        "ps_muls0 %0, %0, %7\n\t"
        "ps_muls0 %1, %1, %7\n\t"
        "psq_l %3, 8(%5), 0, 0\n\t"
        "ps_add %0, %0, %2\n\t"
        "ps_add %1, %1, %3\n\t"
        "psq_st %0, 0(%6), 0, 0\n\t"
        "psq_st %1, 8(%6), 0, 0"
        : "=&f"(t0), "=&f"(t1), "=&f"(t2), "=&f"(t3)
        : "b"(&a), "b"(&b), "b"(&result), "f"(scaleby));
}

// Decl: GC MW: UVectorMath.cpp: 807
float VU0_v3distancesquare(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    float result, t1, t2, t3, t4, t5, t6, t7;
    asm (
        "psq_l %1, 0(%8), 0, 0\n\t"
        "psq_l %2, 0(%9), 0, 0\n\t"
        "lfs %3, 8(%8)\n\t"
        "lfs %4, 8(%9)\n\t"
        "ps_sub %5, %1, %2\n\t"
        "ps_sub %6, %3, %4\n\t"
        "ps_mul %5, %5, %5\n\t"
        "ps_sum0 %5, %5, %5, %5\n\t"
        "ps_madd %0, %6, %6, %5"
        : "=f"(result), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6), "=f"(t7)
        : "b"(&p1), "b"(&p2));
    return result;
}

// Decl: GC MW: UVectorMath.cpp: 851
float VU0_v4distancesquarexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2) {
    float result, t1, t2, t3, t4, t5, t6, t7;
    asm (
        "psq_l %1, 0(%8), 0, 0\n\t"
        "psq_l %2, 0(%9), 0, 0\n\t"
        "lfs %3, 8(%8)\n\t"
        "lfs %4, 8(%9)\n\t"
        "ps_sub %5, %1, %2\n\t"
        "ps_sub %6, %3, %4\n\t"
        "ps_mul %5, %5, %5\n\t"
        "ps_sum0 %5, %5, %5, %5\n\t"
        "ps_madd %0, %6, %6, %5"
        : "=f"(result), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6), "=f"(t7)
        : "b"(&p1), "b"(&p2));
    return result;
}

// Decl: GC MW: UVectorMath.cpp: 893
float VU0_v3distancesquarexz(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    float result, t1, t2, t3, t4, t5, t6, t7;
    asm (
        "psq_l %1, 0(%8), 0, 0\n\t"
        "psq_l %2, 0(%9), 0, 0\n\t"
        "lfs %3, 8(%8)\n\t"
        "lfs %4, 8(%9)\n\t"
        "ps_sub %5, %1, %2\n\t"
        "ps_sub %6, %3, %4\n\t"
        "ps_mul %5, %5, %5\n\t"
        "ps_madd %0, %6, %6, %5"
        : "=f"(result), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6), "=f"(t7)
        : "b"(&p1), "b"(&p2));
    return result;
}

// Decl: GC MW: UVectorMath.cpp: 925
float VU0_v3lengthsquare(const UMath::Vector3 &a) {
    float result, t1, t2, t3, t4;
    asm (
        "psq_l %1, 0(%5), 0, 0\n\t"
        "ps_mul %3, %1, %1\n\t"
        "lfs %2, 8(%5)\n\t"
        "ps_sum0 %3, %3, %3, %3\n\t"
        "ps_madd %0, %2, %2, %3"
        : "=f"(result), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4)
        : "b"(&a));
    return result;
}

// Decl: GC MW: UVectorMath.cpp: 948
float VU0_v4lengthsquare(const UMath::Vector4 &a) {
    float result, t1, t2, t3, t4;
    asm (
        "psq_l %1, 0(%5), 0, 0\n\t"
        "ps_mul %3, %1, %1\n\t"
        "psq_l %2, 8(%5), 0, 0\n\t"
        "ps_madd %3, %2, %2, %3\n\t"
        "ps_sum0 %0, %3, %3, %3"
        : "=f"(result), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4)
        : "b"(&a));
    return result;
}

// Decl: GC MW: UVectorMath.cpp: 966
float VU0_v4lengthsquarexyz(const UMath::Vector4 &a) {
    float result, t1, t2, t3, t4;
    asm (
        "psq_l %1, 0(%5), 0, 0\n\t"
        "ps_mul %3, %1, %1\n\t"
        "lfs %2, 8(%5)\n\t"
        "ps_sum0 %3, %3, %3, %3\n\t"
        "ps_madd %0, %2, %2, %3"
        : "=f"(result), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4)
        : "b"(&a));
    return result;
}

#ifndef EA_PLATFORM_GAMECUBE
// Decl: Carbon: UVectorMath.cpp: 1137, GC MW: UVectorMathGC.hpp: 352, PS2 MW: UVectorMath.hpp: TODO
void VU0_ExtractXAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float yy = scale * (quat.y * quat.y);
    float zz = scale * (quat.z * quat.z);
    float xy = scale * (quat.x * quat.y);
    float xz = scale * (quat.x * quat.z);
    float yw = scale * (quat.y * quat.w);
    float zw = scale * (quat.z * quat.w);

    result.x = 1.0f - (yy + zz);
    result.y = xy + zw;
    result.z = xz - yw;
}

// Decl: Carbon: UVectorMath.cpp: 1169, GC MW: UVectorMathGC.hpp: 386, PS2 MW: UVectorMath.hpp: TODO
void VU0_ExtractYAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float xx = scale * (quat.x * quat.x);
    float zz = scale * (quat.z * quat.z);
    float xy = scale * (quat.x * quat.y);
    float xw = scale * (quat.x * quat.w);
    float yz = scale * (quat.y * quat.z);
    float zw = scale * (quat.z * quat.w);

    result.x = xy - zw;
    result.y = 1.0f - (xx + zz);
    result.z = yz + xw;
}

// Decl: Carbon: UVectorMath.cpp: 1202, GC MW: UVectorMathGC.hpp: 423, PS2 MW: UVectorMath.hpp: TODO
void VU0_ExtractZAxis3FromQuat(const RQUAT &quat, UMath::Vector3 &result) {
    const float scale = 2.0f;
    float xx = scale * (quat.x * quat.x);
    float yy = scale * (quat.y * quat.y);
    float xz = scale * (quat.x * quat.z);
    float xw = scale * (quat.x * quat.w);
    float yz = scale * (quat.y * quat.z);
    float yw = scale * (quat.y * quat.w);

    result.x = xz + yw;
    result.y = yz - xw;
    result.z = 1.0f - (xx + yy);
}

// Decl: Carbon: UVectorMath.cpp: 1235, GC MW: UVectorMathGC.hpp: 472, PS2 MW: UVectorMath.hpp: TODO
void VU0_quattom4(const RQUAT &quat, UMath::Matrix4 &result) {
    const float scale = 2.0f;
    float xx = quat.x * quat.x * scale;
    float yy = quat.y * quat.y * scale;
    float zz = quat.z * quat.z * scale;

    float xy = quat.x * quat.y * scale;
    float xz = quat.x * quat.z * scale;
    float xw = quat.x * quat.w * scale;

    float yz = quat.y * quat.z * scale;
    float yw = quat.y * quat.w * scale;
    float zw = quat.z * quat.w * scale;

    result[0][0] = 1.0f - (yy + zz);
    result[0][1] = (xy + zw);
    result[0][2] = (xz - yw);
    result[0][3] = 0.0f;

    result[1][0] = (xy - zw);
    result[1][1] = 1.0f - (xx + zz);
    result[1][2] = (yz + xw);
    result[1][3] = 0.0f;

    result[2][0] = (xz + yw);
    result[2][1] = (yz - xw);
    result[2][2] = 1.0f - (xx + yy);
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;
}

// Decl: Carbon: UVectorMath.cpp: 1313, GC MW: UVectorMathGC.hpp: 538, PS2 MW: UVectorMath.hpp: 1891
void VU0_MATRIX4_mult(const UMath::Matrix4 &m1, const UMath::Matrix4 &m2, UMath::Matrix4 &result) {
    UMath::Matrix4 temp;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            temp[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
        }
    }
    result = temp;
}
#endif

// Decl: GC MW: UVectorMath.cpp: 948
float VU0_sqrt(const float a) {
    // La forma sale del DWARF del original (mw_dwarfdump.nothpp): el parametro
    // `a` vive en f12 y la funcion tiene UN bloque anonimo cuya unica local es
    // `guess`, en f1, que es el destino del frsqrte. El `volatile` del asm es lo
    // que mantiene el frsqrte delante de los dos `lis` del pool: sin el, el
    // planificador sube uno de los `lis` por encima y el orden r9/r11 se cambia.
    if (a != 0.0f) {
        float guess;
        asm volatile ("frsqrte %0, %1" : "=f"(guess) : "f"(a));
        return (a * guess + 1.0f / guess) * 0.5f;
    }
    return a;
}

// Decl: GC MW: UVectorMath.cpp: 970
float VU0_rsqrt(const float a) {
    float temp = VU0_sqrt(a);
    float zero = 0.0f;
    if (temp == zero)
        return zero;
    return 1.0f / temp;
}

// Decl: GC MW: UVectorMath.cpp: 987
void VU0_MATRIX4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result) {
    float t0, t1, t2, t3, t4, t5, t6;
    asm volatile (
        "psq_l %6, 0(%7), 0, 0\n\t"
        "psq_l %2, 0(%8), 0, 0\n\t"
        "psq_l %3, 8(%8), 0, 0\n\t"
        "psq_l %4, 16(%8), 0, 0\n\t"
        "ps_muls0 %0, %2, %6\n\t"
        "psq_l %5, 24(%8), 0, 0\n\t"
        "ps_muls0 %1, %3, %6\n\t"
        "psq_l %3, 8(%7), 1, 0\n\t"
        "ps_madds1 %0, %4, %6, %0\n\t"
        "psq_l %2, 32(%8), 0, 0\n\t"
        "ps_madds1 %1, %5, %6, %1\n\t"
        "psq_l %6, 40(%8), 0, 0\n\t"
        "ps_madds0 %0, %2, %3, %0\n\t"
        "ps_madds0 %1, %6, %3, %1\n\t"
        "psq_l %2, 48(%8), 0, 0\n\t"
        "psq_l %3, 56(%8), 0, 0\n\t"
        "ps_add %0, %0, %2\n\t"
        "ps_add %1, %1, %3\n\t"
        "psq_st %0, 0(%9), 0, 0\n\t"
        "psq_st %1, 8(%9), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6)
        : "b"(&v), "b"(&m), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 1029
void VU0_MATRIX4_vect4mult(const UMath::Vector4 &v, const UMath::Matrix4 &m, UMath::Vector4 &result) {
    float t0, t1, t2, t3, t4, t5, t6;
    asm volatile (
        "psq_l %6, 0(%7), 0, 0\n\t"
        "psq_l %2, 0(%8), 0, 0\n\t"
        "psq_l %3, 8(%8), 0, 0\n\t"
        "psq_l %4, 16(%8), 0, 0\n\t"
        "ps_muls0 %0, %2, %6\n\t"
        "psq_l %5, 24(%8), 0, 0\n\t"
        "ps_muls0 %1, %3, %6\n\t"
        "psq_l %3, 8(%7), 0, 0\n\t"
        "ps_madds1 %0, %4, %6, %0\n\t"
        "psq_l %2, 32(%8), 0, 0\n\t"
        "ps_madds1 %1, %5, %6, %1\n\t"
        "psq_l %6, 40(%8), 0, 0\n\t"
        "ps_madds0 %0, %2, %3, %0\n\t"
        "ps_madds0 %1, %6, %3, %1\n\t"
        "psq_l %4, 48(%8), 0, 0\n\t"
        "psq_l %5, 56(%8), 0, 0\n\t"
        "ps_madds1 %0, %4, %3, %0\n\t"
        "ps_madds1 %1, %5, %3, %1\n\t"
        "psq_st %0, 0(%9), 0, 0\n\t"
        "psq_st %1, 8(%9), 0, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6)
        : "b"(&v), "b"(&m), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 1060
void VU0_MATRIX4_vect4mult(const UMath::Vector4 *v, const UMath::Matrix4 &m, UMath::Vector4 *result, int count) {
    int i;
    for (i = 0; i < count; i++) {
        VU0_MATRIX4_vect4mult(*v, m, *result);
        v++;
        result++;
    }
}

// Decl: GC MW: UVectorMath.cpp: 1070
void VU0_MATRIX3x4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result) {
    float t0, t1, t2, t3, t4, t5, t6;
    asm volatile (
        "psq_l %6, 0(%7), 0, 0\n\t"
        "psq_l %2, 0(%8), 0, 0\n\t"
        "psq_l %3, 8(%8), 0, 0\n\t"
        "psq_l %4, 16(%8), 0, 0\n\t"
        "ps_muls0 %0, %2, %6\n\t"
        "psq_l %5, 24(%8), 0, 0\n\t"
        "ps_muls0 %1, %3, %6\n\t"
        "psq_l %3, 8(%7), 1, 0\n\t"
        "ps_madds1 %0, %4, %6, %0\n\t"
        "psq_l %2, 32(%8), 0, 0\n\t"
        "ps_madds1 %1, %5, %6, %1\n\t"
        "psq_l %6, 40(%8), 0, 0\n\t"
        "ps_madds0 %0, %2, %3, %0\n\t"
        "ps_madds0 %1, %6, %3, %1\n\t"
        "psq_st %0, 0(%9), 0, 0\n\t"
        "psq_st %1, 8(%9), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6)
        : "b"(&v), "b"(&m), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 1097
void VU0_MATRIX3x4_vect4mult(const UMath::Vector4 &v, const UMath::Matrix4 &m, UMath::Vector4 &result) {
    float t0, t1, t2, t3, t4, t5, t6;
    asm volatile (
        "psq_l %6, 0(%7), 0, 0\n\t"
        "psq_l %2, 0(%8), 0, 0\n\t"
        "psq_l %3, 8(%8), 0, 0\n\t"
        "psq_l %4, 16(%8), 0, 0\n\t"
        "ps_muls0 %0, %2, %6\n\t"
        "psq_l %5, 24(%8), 0, 0\n\t"
        "ps_muls0 %1, %3, %6\n\t"
        "psq_l %3, 8(%7), 1, 0\n\t"
        "ps_madds1 %0, %4, %6, %0\n\t"
        "psq_l %2, 32(%8), 0, 0\n\t"
        "ps_madds1 %1, %5, %6, %1\n\t"
        "psq_l %6, 40(%8), 0, 0\n\t"
        "ps_madds0 %0, %2, %3, %0\n\t"
        "ps_madds0 %1, %6, %3, %1\n\t"
        "psq_st %0, 0(%9), 0, 0\n\t"
        "psq_st %1, 8(%9), 1, 0"
        : "=f"(t0), "=f"(t1), "=f"(t2), "=f"(t3), "=f"(t4), "=f"(t5), "=f"(t6)
        : "b"(&v), "b"(&m), "b"(&result));
}

// Decl: GC MW: UVectorMath.cpp: 3078
void VU0_v3quatrotate(const UMath::Vector4 &q, const UMath::Vector3 &v, UMath::Vector3 &result) {
    UMath::Vector3 temp0, temp1, temp2;
    VU0_v3crossprod((const UMath::Vector3 &)q, v, temp1);
    VU0_v3scaleadd(v, q.w, temp1, temp1);
    VU0_v3crossprod(temp1, (const UMath::Vector3 &)q, temp2);
    VU0_v3scale((const UMath::Vector3 &)q, VU0_v3dotprod((const UMath::Vector3 &)q, v), temp0);
    VU0_v3scaleadd(temp1, q.w, temp0, temp0);
    VU0_v3sub(temp0, temp2, result);
}

// Decl: GC MW: UVectorMath.cpp: 3112
void VU0_v3quatrotate_xlate(const UMath::Vector4 &q, const UMath::Vector3 &v, const UMath::Vector3 &trans, UMath::Vector3 &result) {
    UMath::Vector3 temp0, temp1, temp2;
    VU0_v3crossprod((const UMath::Vector3 &)q, v, temp1);
    VU0_v3scaleadd(v, q.w, temp1, temp1);
    VU0_v3crossprod(temp1, (const UMath::Vector3 &)q, temp2);
    VU0_v3scale((const UMath::Vector3 &)q, VU0_v3dotprod((const UMath::Vector3 &)q, v), temp0);
    VU0_v3scaleadd(temp1, q.w, temp0, temp0);
    VU0_v3sub(temp0, temp2, result);
    VU0_v3add(result, trans, result);
}

// Decl: GC MW: UVectorMath.cpp: 4028
void VU0_MATRIX4setyrot(UMath::Matrix4 &dest, const float yangle) {
    float ycos = VU0_Cos(yangle * UMath::TWOPI);
    float ysin = VU0_Sin(yangle * UMath::TWOPI);
    for (int i = 0; i < 16; i++) {
        *((int *)&dest.GetElements()[i]) = 0;
    }
    dest.GetElements()[10] = dest.GetElements()[0] = ycos;
    dest.GetElements()[2] = -ysin;
    dest.GetElements()[8] = ysin;
    *((int *)&dest.GetElements()[5]) = PS2_FLOAT_ONE_AS_INT;
    *((int *)&dest.GetElements()[15]) = PS2_FLOAT_ONE_AS_INT;
}

// Decl: GC MW: UVectorMath.cpp: 4088
void VU0_m4toquat(const UMath::Matrix4 &mat, UMath::Vector4 &result) {
    // La forma sale del DWARF del original (mw_dwarfdump.nothpp), que en esta
    // funcion solo declara DOS locales: `float s` en f11 y `int i` en r9. No hay
    // ni `t`, ni `tr`, ni `d`, ni `m00/m11/m22`: son subexpresiones que CSE
    // comparte. Y la clave del reparto es que `s` es UNA SOLA variable que
    // primero guarda la traza y luego se reutiliza para la raiz -- por eso f11
    // lleva los dos valores en el objetivo.
    const float *e = mat.GetElements();
    float s = e[0] + e[5] + e[10];
    int i;
    if (s > 0.0f) {
        s = VU0_sqrt(s + 1.0f);
        result.w = s * 0.5f;
        s = 0.5f / s;
        result.x = (e[6] - e[9]) * s;
        result.y = (e[8] - e[2]) * s;
        result.z = (e[1] - e[4]) * s;
    } else {
        if (e[5] > e[0])
            i = 5;
        else
            i = 0;
        if (e[10] > e[i]) {
            s = VU0_sqrt((e[10] - (e[0] + e[5])) + 1.0f);
            result.z = s * 0.5f;
            if (s != 0.0f)
                s = 0.5f / s;
            result.w = (e[1] - e[4]) * s;
            result.x = (e[8] + e[2]) * s;
            result.y = (e[9] + e[6]) * s;
        } else if (i != 0) {
            s = VU0_sqrt((e[5] - (e[10] + e[0])) + 1.0f);
            result.y = s * 0.5f;
            if (s != 0.0f)
                s = 0.5f / s;
            result.w = (e[8] - e[2]) * s;
            result.z = (e[6] + e[9]) * s;
            result.x = (e[4] + e[1]) * s;
        } else {
            s = VU0_sqrt((e[0] - (e[5] + e[10])) + 1.0f);
            result.x = s * 0.5f;
            if (s != 0.0f)
                s = 0.5f / s;
            result.w = (e[6] - e[9]) * s;
            result.y = (e[1] + e[4]) * s;
            result.z = (e[2] + e[8]) * s;
        }
    }
}

// Decl: GC MW: UVectorMath.cpp: 4896
void VU0_Matrix4ToEuler(const UMath::Matrix4 &m, UMath::Vector3 &e) {
    float sy = m[0][2];
    if (sy < -1.0f)
        sy = -1.0f;
    if (sy > 1.0f)
        sy = 1.0f;
    float cy = sqrtf(1.0f - sy * sy);
    if ((m[0][0] < 0.0f) && (m[2][2] < 0.0f))
        cy = -cy;
    float z = 0.0f;
    float abs_cy = cy;
    if (cy < z)
        abs_cy = -cy;
    if (abs_cy > 0.001f) {
        e.x = UMath::Atan2a(m[1][2] / cy, m[2][2] / cy);
        e.y = -UMath::Atan2a(sy, cy);
        e.z = UMath::Atan2a(m[0][1] / cy, m[0][0] / cy);
    } else {
        e.x = UMath::Atan2a(-m[2][1], m[1][1]);
        e.y = -rasin(sy);
        e.z = z;
    }
}

#endif // EA_PLATFORM_GAMECUBE

// Decl: Carbon: UVectorMath.cpp: 4934, GC MW: UVectorMath.cpp: 4935, PS2 MW: UVectorMath.cpp: 4936
float VU0_Atan2(const float opposite, const float adjacent) {
    float x = UMath::Epsilon < VU0_fabs(adjacent) ? adjacent : 0.0f;
    float y = UMath::Epsilon < VU0_fabs(opposite) ? opposite : 0.0f;

    short bangle = bATan(x, y);
    return bangle / 65536.0f;
}

// Los dos estaticos que el static-init del objetivo inicializa desde este .cpp
// (UVectorMath.h:311 y :350 en el mapa de lineas son los CONSTRUCTORES en
// linea, no el sitio de la definicion).
const UTransform UTransform::fgIdentityTransform(UMath::Matrix4::kIdentity);

const USphere USphere::fgNullSphere(UMath::Vector4::kZero);
