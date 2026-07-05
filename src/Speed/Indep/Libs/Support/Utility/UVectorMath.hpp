#include "UVectorMath.h"
#include "eetypes.h"

// TODO reorder the rest

// Decl: 159
inline float VU0_v3dotprod(const UMath::Vector3 &a, const UMath::Vector3 &b) {
    float result;
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "vmul vf3, vf1, vf2\n"
                     "vaddy vf3, vf3, vf3y\n"
                     "vaddz vf3, vf3, vf3z\n"
                     "qmfc2.ni %0, vf3"
                     : "=r"(result)
                     : "o"(a), "o"(b));
    return result;
}

// Decl: 263
inline void VU0_v3crossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "lqc2 vf3, %0\n"
                     "vopmula ACC, vf1, vf2\n"
                     "vopmsub vf3, vf2, vf1\n"
                     "sqc2 vf3, %0"
                     : "=o"(dest)
                     : "o"(a), "o"(b));
}

// Decl: Carbon: UVectorMathCPU.hpp: 94, GC MW: UVectorMathGC.hpp: 26, PS2 MW: UVectorMath.hpp: 354
inline void VU0_v3unitcrossprod(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &dest) {
    u_long128 _t0;
    asm __volatile__("lui %3, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "lqc2 vf3, %0\n"
                     "vopmula ACC, vf1, vf2\n"
                     "vopmsub vf3, vf2, vf1\n"
                     "qmtc2.ni %3, vf5\n"
                     "vmul vf4, vf3, vf3\n"
                     "vmulax ACC, vf3, vf3x\n"
                     "vmadday ACC, vf5, vf4y\n"
                     "vmaddz vf4, vf5, vf4z\n"
                     "vrsqrt Q, vf0w, vf4x\n"
                     "vwaitq\n"
                     "vmulq vf3, vf3, Q\n"
                     "sqc2 vf3, %0"
                     : "=o"(dest)
                     : "o"(a), "o"(b), "r"(_t0));
}

// TODO order
inline float VU0_sqrt(const float a) {
    float result;
    asm __volatile__("vsqrt Q, vf2x\n"
                     "vwaitq\n"
                     "vaddq vf3, vf0, Q\n"
                     : "=f"(result)
                     : "o"(a));
    return result;
}

inline float VU0_v3distancesquarexz(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {}

inline void VU0_v4scaleadd(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result) {}

inline void VU0_v4scaleaddxyz(const UMath::Vector4 &a, const float scaleby, const UMath::Vector4 &b, UMath::Vector4 &result) {}

inline float VU0_v4lengthsquare(const UMath::Vector4 &a) {}

inline float VU0_v4lengthsquarexyz(const UMath::Vector4 &a) {}

inline void VU0_v4subxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {}

inline float VU0_v4dotprodxyz(const UMath::Vector4 &a, const UMath::Vector4 &b) {}

inline void VU0_v4scalexyz(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result) {}

inline float VU0_v4distancesquarexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2) {}

inline void VU0_v4addxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result) {}

// Decl: Carbon: UVectorMathCPU.hpp: 129, GC MW: UVectorMathGC.hpp: 220, PS2 MW: UVectorMath.hpp: 621
inline void VU0_v3unit(const UMath::Vector3 &a, UMath::Vector3 &result) {
    u_long128 _t0;
    asm __volatile__("lui %2, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "lqc2 vf2, %0\n"
                     "vmul vf2, vf1, vf1\n"
                     "qmtc2.ni %2, vf3\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmadday ACC, vf3, vf2y\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "vrsqrt Q, vf0w, vf2x\n"
                     "vwaitq\n"
                     "vmulq vf2, vf1, Q\n"
                     "sqc2 vf2, %0"
                     : "=o"(result)
                     : "o"(a), "r"(_t0));
}

// Decl: Carbon: UVectorMathCPU.hpp: 146, GC MW: UVectorMathGC.hpp: 236, PS2 MW: UVectorMath.hpp: 677
inline void VU0_v4unitxyz(const UMath::Vector4 &a, UMath::Vector4 &result) {
    float rlen = VU0_rsqrt(VU0_v4lengthsquarexyz(a));
    VU0_v4scalexyz(a, rlen, result);
}

// Decl: 706
inline void VU0_v3add(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "lqc2 vf3, %0\n"
                     "vadd vf3, vf1, vf2\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "o"(b));
}

// Decl: 789
inline void VU0_v3sub(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "lqc2 vf3, %0\n"
                     "vsub vf3, vf1, vf2\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "o"(b));
}

// Decl: Carbon: UVectorMathCPU.hpp: 333, GC MW: UVectorMathGC.hpp: 255, PS2 MW: UVectorMath.hpp: 842
inline float VU0_v4distancexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2) {
    UMath::Vector4 temp;
    VU0_v4subxyz(p1, p2, temp);
    return VU0_sqrt(VU0_v4lengthsquarexyz(temp));
}

// Decl: 869
inline void VU0_v3scale(const UMath::Vector3 &a, const float scaleby, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf3, %0\n"
                     "qmtc2.ni %2, vf2\n"
                     "vmulx vf3, vf1, vf2x\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "r"(scaleby));
}

// Decl: 896
inline void VU0_v3scale(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf3, %0\n"
                     "lqc2 vf1, %2\n"
                     "lqc2 vf2, %1\n"
                     "vmul vf3, vf1, vf2\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "o"(b));
}

// Decl: 922
inline void VU0_v4scale(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "qmtc2.ni %2, vf2\n"
                     "vmulx vf3, vf1, vf2x\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "r"(scaleby));
}

// Decl: Carbon: UVectorMathCPU.hpp: 354, GC MW: UVectorMathGC.hpp: 274, PS2 MW: UVectorMath.hpp: 1014
inline float VU0_v3length(const struct UMath::Vector3 &a) {
    u_long128 _t0; // TODO uint128 instead of u_long128
    float result;
    asm __volatile__("lui %2, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "vmul vf2, vf1, vf1\n"
                     "qmtc2.ni %2, vf3\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmadday ACC, vf3, vf2y\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "vsqrt Q, vf2x\n"
                     "vwaitq\n"
                     "vaddq vf3, vf0, Q\n"
                     "qmfc2.ni %0, vf3"
                     : "=r"(result)
                     : "o"(a), "r"(_t0));
    return result;
}

// Decl: Carbon: UVectorMathCPU.hpp: 538, GC MW: UVectorMathGC.hpp: 125, PS2 MW: UVectorMath.hpp: TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
inline void VU0_qmul(const RQUAT &b, const RQUAT &a, RQUAT &dest) {}

// Decl: 1136
inline void VU0_v3scaleadd(const UMath::Vector3 &a, const float scaleby, const UMath::Vector3 &b, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, %3\n"
                     "lqc2 vf3, %0\n"
                     "qmtc2.ni %2, vf4\n"
                     "vmulx vf3, vf1, vf4x\n"
                     "vadd vf3, vf2, vf3\n"
                     "sqc2 vf3, %0"
                     : "=o"(result)
                     : "o"(a), "r"(scaleby), "o"(b));
}

// Decl: Carbon: UVectorMathCPU.hpp: 139, GC MW: UVectorMathGC.hpp: 228, PS2 MW: UVectorMath.hpp: 1203
inline void VU0_v4unit(const UMath::Vector4 &a, UMath::Vector4 &result) {
    float rlen = VU0_rsqrt(VU0_v4lengthsquare(a));
    VU0_v4scale(a, rlen, result);
}

// Decl: Carbon: UVectorMathCPU.hpp: 293, GC MW: UVectorMathGC.hpp: 62, PS2 MW: UVectorMath.hpp: 1223
inline void VU0_v3negate(UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %0\n"
                     "vsub vf1, vf0, vf1\n"
                     "sqc2 vf1, %0"
                     : "=o"(result));
}

// Decl: Carbon: UVectorMathCPU.hpp: 300, GC MW: UVectorMathGC.hpp: 69, PS2 MW: UVectorMath.hpp: 1243
inline void VU0_v4negatexyz(UMath::Vector4 &result) {}

// Decl: Carbon: UVectorMathCPU.hpp: 324, GC MW: UVectorMathGC.hpp: 243, PS2 MW: UVectorMath.hpp: 1323
inline float VU0_v3distance(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    float result;
    u_long128 _t0;
    asm __volatile__("lui %3, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "vsub vf1, vf1, vf2\n"
                     "qmtc2.ni %3, vf3\n"
                     "vmul vf2, vf1, vf1\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmadday ACC, vf3, vf2y\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "vsqrt Q, vf2x\n"
                     "vwaitq\n"
                     "vaddq vf3, vf0, Q\n"
                     "qmfc2.ni %0, vf3"
                     : "=r"(result)
                     : "o"(p1), "o"(p2), "r"(_t0));
    return result;
}

// Decl: 1351
inline float VU0_v3distancesquare(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    u_long128 _t0;
    float result;
    asm __volatile__("lui %3, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "lqc2 vf2, %2\n"
                     "vsub vf1, vf1, vf2\n"
                     "qmtc2.ni %3, vf3\n"
                     "vmul vf2, vf1, vf1\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmadday ACC, vf3, vf2y\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "qmfc2.ni %0, vf2\n"
                     : "=r"(result)
                     : "o"(p1), "o"(p2), "r"(_t0));
    return result;
}

// Decl: Carbon: UVectorMathCPU.hpp: 342, GC MW: UVectorMathGC.hpp: 263, PS2 MW: UVectorMath.hpp: 1437
inline float VU0_v3distancexz(const UMath::Vector3 &p1, const UMath::Vector3 &p2) {
    float result;
    u_long128 _t0;
    asm __volatile__("lui %3, 0x3f80\n"
                     "lqc2 vf1, %2\n"
                     "lqc2 vf2, %1\n"
                     "vsub vf1, vf1, vf2\n"
                     "qmtc2.ni %3, vf3\n"
                     "vmul vf2, vf1, vf1\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "vsqrt Q, vf2x\n"
                     "vwaitq\n"
                     "vaddq vf3, vf0, Q\n"
                     "qmfc2.ni %0, vf3"
                     : "=r"(result)
                     : "o"(p1), "o"(p2), "r"(_t0));
    return result;
}

// Decl: 1545
inline float VU0_v3lengthsquare(const UMath::Vector3 &a) {
    u_long128 _t0;
    float result;
    asm __volatile__("lui %2, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "vmul vf2, vf1, vf1\n"
                     "qmtc2.ni %2, vf3\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmadday ACC, vf3, vf2y\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "qmfc2.ni %0, vf2"
                     : "=r"(result)
                     : "o"(a), "r"(_t0));
    return result;
}

// Decl: Carbon: UVectorMathCPU.hpp: 359, GC MW: UVectorMathGC.hpp: 279, PS2 MW: UVectorMath.hpp: 1571
inline float VU0_v3lengthxz(const UMath::Vector3 &a) {
    u_long128 _t0;
    float result;
    asm __volatile__("lui %2, 0x3f80\n"
                     "lqc2 vf1, %1\n"
                     "vmul vf2, vf1, vf1\n"
                     "qmtc2.ni %2, vf3\n"
                     "vmulax ACC, vf1, vf1x\n"
                     "vmaddz vf2, vf3, vf2z\n"
                     "vsqrt Q, vf2x\n"
                     "vwaitq\n"
                     "vaddq vf3, vf0, Q\n"
                     "qmfc2.ni %0, vf3"
                     : "=r"(result)
                     : "o"(a), "r"(_t0));
    return result;
}

// Decl: Carbon: UVectorMathCPU.hpp: 364, GC MW: UVectorMathGC.hpp: 284, PS2 MW: UVectorMath.hpp: TODO
inline float VU0_v4length(const UMath::Vector4 &a) {
    return VU0_sqrt(VU0_v4lengthsquare(a));
}

// Decl: Carbon: UVectorMathCPU.hpp: 374, GC MW: UVectorMathGC.hpp: 294, PS2 MW: UVectorMath.hpp: 1653
inline float VU0_v4lengthxyz(const UMath::Vector4 &a) {
    return VU0_sqrt(VU0_v4lengthsquarexyz(a));
}

// Decl: 1812
inline void VU0_MATRIX4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, 0x0(%2)\n"
                     "lqc2 vf3, 0x10(%2)\n"
                     "lqc2 vf4, 0x20(%2)\n"
                     "lqc2 vf5, 0x30(%2)\n"
                     "lqc2 vf6, %1\n"
                     "vmulax ACC, vf2, vf1x\n"
                     "vmadday ACC, vf3, vf1y\n"
                     "vmaddaz ACC, vf4, vf1z\n"
                     "vmaddw vf6, vf5, vf0w\n"
                     "sqc2 vf6, %0"
                     : "=o"(result)
                     : "o"(v), "r"(&m));
}

// Decl: 1968
inline void VU0_MATRIX3x4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result) {
    asm __volatile__("lqc2 vf1, %1\n"
                     "lqc2 vf2, 0x0(%2)\n"
                     "lqc2 vf3, 0x10(%2)\n"
                     "lqc2 vf4, 0x20(%2)\n"
                     "lqc2 vf5, %0\n"
                     "vmulax ACC, vf2, vf1x\n"
                     "vmadday ACC, vf3, vf1y\n"
                     "vmaddz vf5, vf4, vf1z\n"
                     "sqc2 vf5, %0"
                     : "=o"(result)
                     : "o"(v), "r"(&m));
}

// Decl: Carbon: UVectorMathCPU.hpp: 506, GC MW: UVectorMathGC.hpp: 92, PS2 MW: UVectorMath.hpp: 2061
inline void VU0_MATRIX4_transpose(const UMath::Matrix4 &m, UMath::Matrix4 &result) {}

// Decl: Carbon: UVectorMathCPU.hpp: 530, GC MW: UVectorMathGC.hpp: 115, PS2 MW: UVectorMath.hpp: 2086
inline void VU0_qtranspose(const RQUAT &a, RQUAT &result) {}

// Decl: Carbon: UVectorMathCPU.hpp: 584, GC MW: UVectorMathGC.hpp: 169, PS2 MW: UVectorMath.hpp: 2188
inline void VU0_MATRIX4Init(UMath::Matrix4 &dest, const float xx, const float yy, const float zz) {}

// Decl: Carbon: UVectorMathCPU.hpp: 607, GC MW: UVectorMathGC.hpp: 193, PS2 MW: UVectorMath.hpp: 2234
inline void VU0_v4Copy(const UMath::Vector4 &a, UMath::Vector4 &b) {
    // u_long128 _t0;
    // u_long128 _t1;
    // asm __volatile__("lq %0, 0x0(%2)\n"
    //                  "lq %1, 0x10(%2)\n"
    //                  "sq %0, %3\n"
    //                  "sq %1, %4\n"
    //                  "lq %0, 0x20(%2)\n"
    //                  "lq %1, 0x30(%2)\n"
    //                  "sq %0, %5\n"
    //                  "sq %1, %6\n"
    //                  :
    //                  : "r"(_t0), "r"(_t1), "r"(&a), "o"(b.v0), "o"(b.v1), "o"(b.v2), "o"(b.v3));
}

// Decl: Carbon: UVectorMathCPU.hpp: 612, GC MW: UVectorMathGC.hpp: 198, PS2 MW: UVectorMath.hpp: 2268
inline void VU0_MATRIX4Copy(const UMath::Matrix4 &a, UMath::Matrix4 &b) {
    u_long128 _t0;
    u_long128 _t1;
    asm __volatile__("lq %0, 0x0(%2)\n"
                     "lq %1, 0x10(%2)\n"
                     "sq %0, %3\n"
                     "sq %1, %4\n"
                     "lq %0, 0x20(%2)\n"
                     "lq %1, 0x30(%2)\n"
                     "sq %0, %5\n"
                     "sq %1, %6\n"
                     :
                     : "r"(_t0), "r"(_t1), "r"(&a), "o"(b.v0), "o"(b.v1), "o"(b.v2), "o"(b.v3));
}

// Decl: Carbon: UVectorMathCPU.hpp: 644, GC MW: UVectorMathGC.hpp: 311, PS2 MW: UVectorMath.hpp: 2466
inline void VU0_v3lerp(const UMath::Vector3 &v1, const UMath::Vector3 &v2, const float t, UMath::Vector3 &target) {
    target.x = v1.x + (v2.x - v1.x) * t;
    target.y = v1.y + (v2.y - v1.y) * t;
    target.z = v1.z + (v2.z - v1.z) * t;
}
