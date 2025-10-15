/*++

Copyright (c) Microsoft Corporation

Module Name:

    xboxmath.h

Abstract:

    Xbox general math library.

--*/

#pragma once

#ifndef __XBOXMATH_H__
#define __XBOXMATH_H__

#ifdef _X86_
#define _NO_INTRINSICS_
#else
#if defined(__VMX128_SUPPORTED)
#define _VMX128_INTRINSICS_
#else
#define _VMX32_INTRINSICS_
#endif // __VMX128_SUPPORTED
#endif // _X86_

#ifndef _NO_INTRINSICS_
#include <VectorIntrinsics.h>
#endif // _NO_INTRINSICS_
#include <math.h>

#ifndef _NO_INTRINSICS_
#ifdef _VMX1_INTRINSICS_
#ifndef _VMX32_INTRINSICS_
#define _VMX32_INTRINSICS_
#endif // !_VMX32_INTRINSICS_
#endif // _VMX1_INTRINSICS_
#endif // !_NO_INTRINSICS_

#ifndef _NO_INTRINSICS_
#ifndef _VMX32_INTRINSICS_
#ifndef _VMX128_INTRINSICS_
#define _VMX128_INTRINSICS_
#endif // !_VMX128_INTRINSICS_
#endif // !_VMX32_INTRINSICS_
#endif // !_VMX32_INTRINSICS_

#ifndef XMINLINE
#define XMINLINE __inline
#endif

#ifndef XMFINLINE
#define XMFINLINE __forceinline
#endif

#ifndef XMDEBUG
#ifdef _DEBUG
#define XMDEBUG
#endif
#endif // !XMDEBUG

#ifndef XMASSERT
#ifdef XMDEBUG
#define XMASSERT(Expression) ((VOID)(Expression || (XMAssert(#Expression, __FILE__, __LINE__), 0)))
#else // !XMDEBUG
#define XMASSERT(Expression) ((VOID)0)
#endif // !XMDEBUG
#endif // !XMASSERT

#ifndef _NOALIGNMENT_
#define _DECLSPEC_ALIGN_16_   __declspec(align(16))
#else // defined _NOALIGNMENT_
#define _DECLSPEC_ALIGN_16_
#endif //_NOALIGNMENT_

#define __vmaddcfp(VRA, VRC, VRB) __vmaddfp(VRA, VRC, VRB)

/****************************************************************************
 *
 * Constant definitions
 *
 ****************************************************************************/

#define XM_PI               3.141592654f
#define XM_2PI              6.283185307f
#define XM_1DIVPI           0.318309886f
#define XM_1DIV2PI          0.159154943f
#define XM_PIDIV2           1.570796327f
#define XM_PIDIV4           0.785398163f

#define XM_SELECT_0         0x00000000
#define XM_SELECT_1         0xFFFFFFFF

#define XM_PERMUTE_0X       0x00010203
#define XM_PERMUTE_0Y       0x04050607
#define XM_PERMUTE_0Z       0x08090A0B
#define XM_PERMUTE_0W       0x0C0D0E0F
#define XM_PERMUTE_1X       0x10111213
#define XM_PERMUTE_1Y       0x14151617
#define XM_PERMUTE_1Z       0x18191A1B
#define XM_PERMUTE_1W       0x1C1D1E1F

#define XM_CRMASK_CR6       0x000000F0
#define XM_CRMASK_CR6TRUE   0x00000080
#define XM_CRMASK_CR6FALSE  0x00000020
#define XM_CRMASK_CR6BOUNDS XM_CRMASK_CR6FALSE

/****************************************************************************
 *
 * Macros
 *
 ****************************************************************************/

// Unit conversion

#define XMConvertToRadians(Degrees)        ((Degrees) * (XM_PI / 180.0f))
#define XMConvertToDegrees(Radians)        ((Radians) * (180.0f / XM_PI))

// Condition register evaluation proceeding a recording (Rc) comparison

#define XMComparisonAllTrue(CR)            (((CR) & XM_CRMASK_CR6TRUE) == XM_CRMASK_CR6TRUE)
#define XMComparisonAnyTrue(CR)            (((CR) & XM_CRMASK_CR6FALSE) != XM_CRMASK_CR6FALSE)
#define XMComparisonAllFalse(CR)           (((CR) & XM_CRMASK_CR6FALSE) == XM_CRMASK_CR6FALSE)
#define XMComparisonAnyFalse(CR)           (((CR) & XM_CRMASK_CR6TRUE) != XM_CRMASK_CR6TRUE)
#define XMComparisonMixed(CR)              (((CR) & XM_CRMASK_CR6) == 0)
#define XMComparisonAllInBounds(CR)        (((CR) & XM_CRMASK_CR6BOUNDS) == XM_CRMASK_CR6BOUNDS)
#define XMComparisonAnyOutOfBounds(CR)     (((CR) & XM_CRMASK_CR6BOUNDS) != XM_CRMASK_CR6BOUNDS)

// Vector conversion operations.  Integer exponent values in these operations must be literal constants.

#if !defined(_NO_INTRINSICS_)

#define XMConvertVectorIntToFloat(VectorInt, DivExponent)    __vcfsx(VectorInt, DivExponent)
#define XMConvertVectorUIntToFloat(VectorUInt, DivExponent)  __vcfux(VectorUInt, DivExponent)
#define XMConvertVectorFloatToInt(VectorFloat, MulExponent)  __vctsxs(VectorFloat, MulExponent)
#define XMConvertVectorFloatToUInt(VectorFloat, MulExponent) __vctuxs(VectorFloat, MulExponent)

#endif

// Vector immediate operations.  Element integer values in these operations must be literal constants.

#if defined(_NO_INTRINSICS_)

#define XMVectorSetBinaryConstant(C0, C1, C2, C3)       XMConvertVectorUIntToFloat(XMVectorSetInt(C0, C1, C2, C3), 0)

#define XMVectorSplatConstant(IntConstant, DivExponent) XMConvertVectorIntToFloat(XMVectorSetInt((UINT)(IntConstant), \
                                                            (UINT)(IntConstant), (UINT)(IntConstant), (UINT)(IntConstant)), \
                                                            (DivExponent))

#define XMVectorSplatConstantInt(IntConstant)           XMVectorSetInt((UINT)(IntConstant), (UINT)(IntConstant), \
                                                            (UINT)(IntConstant), (UINT)(IntConstant))

#else

// Convert the given literal integer constants to floating point and set the results into the four elements of a vector:

#define XMVectorSetBinaryConstant(C0, C1, C2, C3)       __vpermwi(__vupkd3d(__vspltisw(0), VPACK_NORMSHORT2), \
                                                            ((((C0) & 1) + 2) << 6) | ((((C1) & 1) + 2) << 4) | \
                                                            ((((C2) & 1) + 2) << 2) | (((C3) & 1) + 2))

// Convert the given literal integer constant (ranging from -16 to 15) to floating point, 
// divide it by 2 ^ DivExponent, and replicate the result into four elements of a vector:

#define XMVectorSplatConstant(IntConstant, DivExponent) __vcfsx(__vspltisw((IntConstant)), (DivExponent))

// Replicate the given literal integer constant (ranging from -16 to 15) into four elements of a vector:

#define XMVectorSplatConstantInt(IntConstant)           __vspltisw((IntConstant))

#endif

#if defined(_NO_INTRINSICS_)

#define XMVectorShiftLeft(V1, V2, Elements) XMVectorPermute(V1, V2, XMVectorPermuteControl((Elements), \
                                                        ((Elements) + 1), ((Elements) + 2), ((Elements) + 3)))

#define XMVectorRotateLeft(V, Elements)     XMVectorSet((V).v[(Elements) & 3], (V).v[((Elements) + 1) & 3], \
                                                        (V).v[((Elements) + 2) & 3], (V).v[((Elements) + 3) & 3])

#define XMVectorRotateRight(V, Elements)    XMVectorSet((V).v[(4 - (Elements)) & 3], (V).v[(5 - (Elements)) & 3], \
                                                        (V).v[(6 - (Elements)) & 3], (V).v[(7 - (Elements)) & 3])

#else

// Shift the first vector left by the given number of 32-bit elements, filing vacated elements with elements from the second vector:

#define XMVectorShiftLeft(V1, V2, Elements) __vsldoi((V1), (V2), (Elements) << 2)

// Rotate the vector left by the given number of 32-bit elements:

#define XMVectorRotateLeft(V, Elements)     __vsldoi((V), (V), (Elements) << 2)

// Rotate the vector right by the given number of 32-bit elements:

#define XMVectorRotateRight(V, Elements)    __vsldoi((V), (V), ((4 - (Elements)) & 3) << 2)

#endif

#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

#define XMVectorSwizzle(V, E0, E1, E2, E3) XMVectorSet((V).v[(E0)], (V).v[(E1)], (V).v[(E2)], (V).v[(E3)])

#else

// Swizzle the vector such that the result contains 32-bit elements selected from the vector at the given element offsets:

#define XMVectorSwizzle(V, E0, E1, E2, E3) __vpermwi((V), (((E0) & 3) << 6) | (((E1) & 3) << 4) | \
                                                          (((E2) & 3) << 2) | ((E3) & 3))

#endif

#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

#define XMVectorInsert(VD, VS, VSLeftRotateElements, Select0, Select1, Select2, Select3) \
                                           XMVectorSelect((VD), XMVectorRotateLeft((VS), VSLeftRotateElements), \
                                                                XMVectorSelectControl((Select0) & 1, \
                                                                                      (Select1) & 1, \
                                                                                      (Select2) & 1, \
                                                                                      (Select3) & 1))

#else

// Rotate VS left by the given number of 32-bit elements and insert selected elements from the result into VD.
// For best performance, the result should be assigned back to VD:

#define XMVectorInsert(VD, VS, VSLeftRotateElements, Select0, Select1, Select2, Select3) \
                                           __vrlimi((VD), (VS), (((Select0) & 1) << 3) | \
                                                                (((Select1) & 1) << 2) | \
                                                                (((Select2) & 1) << 1) | \
                                                                ((Select3) & 1), (VSLeftRotateElements) & 3)

#endif

#define XMDUMMY_INITIALIZE_VECTOR(V)    (V = __vspltisw(0))
#define XMDUMMY_INITIALIZE_UINT(U)      (U = 0)

/****************************************************************************
 *
 * Data types
 *
 ****************************************************************************/

#pragma warning(push)
#pragma warning(disable:4201)

#ifndef _X86_
#pragma bitfield_order(push)
#pragma bitfield_order(lsb_to_msb)
#endif // _X86_

#ifdef _NO_INTRINSICS_
// The __vector4 structure is an intrinsic on Xbox but must be separately defined
// for x86:
typedef struct __vector4
{
    union
    {
        struct
        {
            FLOAT x;
            FLOAT y;
            FLOAT z;
            FLOAT w;
        };
        FLOAT v[4];
        UINT  u[4];
    };
} __vector4;
#endif // _NO_INTRINSICS_

#ifdef _X86_
typedef UINT __vector4i[4];
#else
typedef __declspec(align(16)) UINT __vector4i[4];
#endif

// Vector intrinsic: Four 32 bit floating point components aligned on a 16 byte 
// boundary and mapped to hardware vector registers
typedef __vector4  XMVECTOR;

// Vector integer type: Four 32 bit unsigned integer components aligned on a 16
// byte boundary (useful for initializing __vector4 data with integer values)
typedef __vector4i XMVECTORI;

// Vector operators
#ifdef __cplusplus

XMVECTOR    operator+ (XMVECTOR V);
XMVECTOR    operator- (XMVECTOR V);

XMVECTOR&   operator+= (XMVECTOR& V1, CONST XMVECTOR V2);
XMVECTOR&   operator-= (XMVECTOR& V1, CONST XMVECTOR V2);
XMVECTOR&   operator*= (XMVECTOR& V1, CONST XMVECTOR V2);
XMVECTOR&   operator/= (XMVECTOR& V1, CONST XMVECTOR V2);
XMVECTOR&   operator*= (XMVECTOR& V, FLOAT S);
XMVECTOR&   operator/= (XMVECTOR& V, FLOAT S);

XMVECTOR    operator+ (CONST XMVECTOR V1, CONST XMVECTOR V2);
XMVECTOR    operator- (CONST XMVECTOR V1, CONST XMVECTOR V2);
XMVECTOR    operator* (CONST XMVECTOR V1, CONST XMVECTOR V2);
XMVECTOR    operator/ (CONST XMVECTOR V1, CONST XMVECTOR V2);
XMVECTOR    operator* (CONST XMVECTOR V, FLOAT S);
XMVECTOR    operator* (FLOAT S, CONST XMVECTOR V);
XMVECTOR    operator/ (CONST XMVECTOR V, FLOAT S);

#endif // __cplusplus

// Matrix type: Sixteen 32 bit floating point components aligned on a
// 16 byte boundary and mapped to four hardware vector registers
#ifdef _X86_
typedef struct _XMMATRIX
#else
typedef _DECLSPEC_ALIGN_16_ struct _XMMATRIX
#endif
{
    union
    {
        XMVECTOR r[4];
        struct
        {
            FLOAT _11, _12, _13, _14;
            FLOAT _21, _22, _23, _24;
            FLOAT _31, _32, _33, _34;
            FLOAT _41, _42, _43, _44;
        };
        FLOAT m[4][4];
    };

#ifdef __cplusplus

                _XMMATRIX() {};
                _XMMATRIX(XMVECTOR R0, XMVECTOR R1, XMVECTOR R2, XMVECTOR R3);
                _XMMATRIX(FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
                         FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
                         FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
                         FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33);
                _XMMATRIX(CONST FLOAT *pArray);

    FLOAT       operator() (UINT Row, UINT Column) CONST;
    FLOAT&      operator() (UINT Row, UINT Column);

    _XMMATRIX&  operator= (CONST _XMMATRIX&);
    _XMMATRIX&  operator*= (CONST _XMMATRIX&);
    _XMMATRIX   operator* (CONST _XMMATRIX&) CONST;

#endif // __cplusplus

} XMMATRIX;

// 16 bit floating point number consisting of a sign bit, a 5 bit biased 
// exponent, and a 10 bit mantissa
//typedef WORD HALF;
typedef USHORT HALF;

// 2D Vector; 32 bit floating point components
typedef struct _XMFLOAT2
{
    FLOAT x;
    FLOAT y;

#ifdef __cplusplus

    _XMFLOAT2() {};
    _XMFLOAT2(FLOAT _x, FLOAT _y);
    _XMFLOAT2(CONST FLOAT *pArray);

    _XMFLOAT2& operator= (CONST _XMFLOAT2& Float2);

#endif // __cplusplus

} XMFLOAT2;

// 2D Vector; 32 bit floating point components aligned on a 16 byte boundary
typedef __declspec(align(16)) XMFLOAT2 XMFLOAT2A, XMFLOAT2A16;

// 2D Vector; 16 bit floating point components
typedef struct _XMHALF2
{
    HALF x;
    HALF y;

#ifdef __cplusplus

    _XMHALF2() {};
    _XMHALF2(HALF _x, HALF _y);
    _XMHALF2(CONST HALF *pArray);
    _XMHALF2(FLOAT _x, FLOAT _y);
    _XMHALF2(CONST FLOAT *pArray);

    _XMHALF2& operator= (CONST _XMHALF2& Half2);

#endif // __cplusplus

} XMHALF2;

// 2D Vector; 16 bit signed normalized integer components
typedef struct _XMSHORTN2
{
    SHORT x;
    SHORT y;

#ifdef __cplusplus

    _XMSHORTN2() {};
    _XMSHORTN2(SHORT _x, SHORT _y);
    _XMSHORTN2(CONST SHORT *pArray);
    _XMSHORTN2(FLOAT _x, FLOAT _y);
    _XMSHORTN2(CONST FLOAT *pArray);

    _XMSHORTN2& operator= (CONST _XMSHORTN2& ShortN2);

#endif // __cplusplus

} XMSHORTN2;

// 2D Vector; 16 bit signed integer components
typedef struct _XMSHORT2
{
    SHORT x;
    SHORT y;

#ifdef __cplusplus

    _XMSHORT2() {};
    _XMSHORT2(SHORT _x, SHORT _y);
    _XMSHORT2(CONST SHORT *pArray);
    _XMSHORT2(FLOAT _x, FLOAT _y);
    _XMSHORT2(CONST FLOAT *pArray);

    _XMSHORT2& operator= (CONST _XMSHORT2& Short2);

#endif // __cplusplus

} XMSHORT2;

// 2D Vector; 16 bit unsigned normalized integer components
typedef struct _XMUSHORTN2
{
    USHORT x;
    USHORT y;

#ifdef __cplusplus

    _XMUSHORTN2() {};
    _XMUSHORTN2(USHORT _x, USHORT _y);
    _XMUSHORTN2(CONST USHORT *pArray);
    _XMUSHORTN2(FLOAT _x, FLOAT _y);
    _XMUSHORTN2(CONST FLOAT *pArray);

    _XMUSHORTN2& operator= (CONST _XMUSHORTN2& UShortN2);

#endif // __cplusplus

} XMUSHORTN2;

// 2D Vector; 16 bit unsigned integer components
typedef struct _XMUSHORT2
{
    USHORT x;
    USHORT y;

#ifdef __cplusplus

    _XMUSHORT2() {};
    _XMUSHORT2(USHORT _x, USHORT _y);
    _XMUSHORT2(CONST USHORT *pArray);
    _XMUSHORT2(FLOAT _x, FLOAT _y);
    _XMUSHORT2(CONST FLOAT *pArray);

    _XMUSHORT2& operator= (CONST _XMUSHORT2& UShort2);

#endif // __cplusplus

} XMUSHORT2;

// 3D Vector; 32 bit floating point components
typedef struct _XMFLOAT3
{
    FLOAT x;
    FLOAT y;
    FLOAT z;

#ifdef __cplusplus

    _XMFLOAT3() {};
    _XMFLOAT3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMFLOAT3(CONST FLOAT *pArray);

    _XMFLOAT3& operator= (CONST _XMFLOAT3& Float3);

#endif // __cplusplus

} XMFLOAT3;

// 3D Vector; 32 bit floating point components aligned on a 16 byte boundary
typedef __declspec(align(16)) XMFLOAT3 XMFLOAT3A, XMFLOAT3A16;

// 3D Vector; 11-11-10 bit normalized components packed into a 32 bit integer
// The normalized 3D Vector is packed into 32 bits as follows: a 10 bit signed, 
// normalized integer for the z component and 11 bit signed, normalized 
// integers for the x and y components.  The z component is stored in the 
// most significant bits and the x component in the least significant bits
// (Z10Y11X11): [32] zzzzzzzz zzyyyyyy yyyyyxxx xxxxxxxx [0]
typedef struct _XMHENDN3
{
    union
    {
        struct
        {
            INT  x   : 11;    // -1023/1023 to 1023/1023
            INT  y   : 11;    // -1023/1023 to 1023/1023
            INT  z   : 10;    // -511/511 to 511/511
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMHENDN3() {};
    _XMHENDN3(UINT Packed);
    _XMHENDN3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMHENDN3(CONST FLOAT *pArray);

    operator UINT ();

    _XMHENDN3& operator= (CONST _XMHENDN3& HenDN3);
    _XMHENDN3& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMHENDN3;

// 3D Vector; 11-11-10 bit components packed into a 32 bit integer
// The 3D Vector is packed into 32 bits as follows: a 10 bit signed, 
// integer for the z component and 11 bit signed integers for the 
// x and y components.  The z component is stored in the 
// most significant bits and the x component in the least significant bits
// (Z10Y11X11): [32] zzzzzzzz zzyyyyyy yyyyyxxx xxxxxxxx [0]
typedef struct _XMHEND3
{
    union
    {
        struct
        {
            INT  x   : 11;    // -1023 to 1023
            INT  y   : 11;    // -1023 to 1023
            INT  z   : 10;    // -511 to 511
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMHEND3() {};
    _XMHEND3(UINT Packed);
    _XMHEND3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMHEND3(CONST FLOAT *pArray);

    operator UINT ();

    _XMHEND3& operator= (CONST _XMHEND3& HenD3);
    _XMHEND3& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMHEND3;

// 3D Vector; 11-11-10 bit normalized components packed into a 32 bit integer
// The normalized 3D Vector is packed into 32 bits as follows: a 10 bit unsigned, 
// normalized integer for the z component and 11 bit unsigned, normalized 
// integers for the x and y components.  The z component is stored in the 
// most significant bits and the x component in the least significant bits
// (Z10Y11X11): [32] zzzzzzzz zzyyyyyy yyyyyxxx xxxxxxxx [0]
typedef struct _XMUHENDN3
{
    union
    {
        struct
        {
            UINT  x  : 11;    // 0/2047 to 2047/2047
            UINT  y  : 11;    // 0/2047 to 2047/2047
            UINT  z  : 10;    // 0/1023 to 1023/1023
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMUHENDN3() {};
    _XMUHENDN3(UINT Packed);
    _XMUHENDN3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMUHENDN3(CONST FLOAT *pArray);

    operator UINT ();

    _XMUHENDN3& operator= (CONST _XMUHENDN3& UHenDN3);
    _XMUHENDN3& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMUHENDN3;

// 3D Vector; 11-11-10 bit components packed into a 32 bit integer
// The 3D Vector is packed into 32 bits as follows: a 10 bit unsigned
// integer for the z component and 11 bit unsigned integers 
// for the x and y components.  The z component is stored in the 
// most significant bits and the x component in the least significant bits
// (Z10Y11X11): [32] zzzzzzzz zzyyyyyy yyyyyxxx xxxxxxxx [0]
typedef struct _XMUHEND3
{
    union
    {
        struct
        {
            UINT  x  : 11;    // 0 to 2047
            UINT  y  : 11;    // 0 to 2047
            UINT  z  : 10;    // 0 to 1023
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMUHEND3() {};
    _XMUHEND3(UINT Packed);
    _XMUHEND3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMUHEND3(CONST FLOAT *pArray);

    operator UINT ();

    _XMUHEND3& operator= (CONST _XMUHEND3& UHenD3);
    _XMUHEND3& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMUHEND3;

// 3D Vector; 10-11-11 bit normalized components packed into a 32 bit integer
// The normalized 3D Vector is packed into 32 bits as follows: a 10 bit signed, 
// normalized integer for the x component and 11 bit signed, normalized 
// integers for the y and z components.  The z component is stored in the 
// most significant bits and the x component in the least significant bits
// (Z11Y11X10): [32] zzzzzzzz zzzyyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMDHENN3
{
    union
    {
        struct
        {
            INT  x   : 10;    // -511/511 to 511/511
            INT  y   : 11;    // -1023/1023 to 1023/1023
            INT  z   : 11;    // -1023/1023 to 1023/1023
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMDHENN3() {};
    _XMDHENN3(UINT Packed);
    _XMDHENN3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMDHENN3(CONST FLOAT *pArray);

    operator UINT ();

    _XMDHENN3& operator= (CONST _XMDHENN3& DHenN3);
    _XMDHENN3& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMDHENN3;

// 3D Vector; 10-11-11 bit components packed into a 32 bit integer
// The 3D Vector is packed into 32 bits as follows: a 10 bit signed, 
// integer for the x component and 11 bit signed integers for the 
// y and z components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (Z11Y11X10): [32] zzzzzzzz zzzyyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMDHEN3
{
    union
    {
        struct
        {
            INT  x   : 10;    // -511 to 511
            INT  y   : 11;    // -1023 to 1023
            INT  z   : 11;    // -1023 to 1023
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMDHEN3() {};
    _XMDHEN3(UINT Packed);
    _XMDHEN3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMDHEN3(CONST FLOAT *pArray);

    operator UINT ();

    _XMDHEN3& operator= (CONST _XMDHEN3& DHen3);
    _XMDHEN3& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMDHEN3;

// 3D Vector; 10-11-11 bit normalized components packed into a 32 bit integer
// The normalized 3D Vector is packed into 32 bits as follows: a 10 bit unsigned, 
// normalized integer for the x component and 11 bit unsigned, normalized 
// integers for the y and z components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (Z11Y11X10): [32] zzzzzzzz zzzyyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMUDHENN3
{
    union
    {
        struct
        {
            UINT  x  : 10;    // 0/1023 to 1023/1023
            UINT  y  : 11;    // 0/2047 to 2047/2047
            UINT  z  : 11;    // 0/2047 to 2047/2047
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMUDHENN3() {};
    _XMUDHENN3(UINT Packed);
    _XMUDHENN3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMUDHENN3(CONST FLOAT *pArray);

    operator UINT ();

    _XMUDHENN3& operator= (CONST _XMUDHENN3& UDHenN3);
    _XMUDHENN3& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMUDHENN3;

// 3D Vector; 10-11-11 bit components packed into a 32 bit integer
// The 3D Vector is packed into 32 bits as follows: a 10 bit unsigned, 
// integer for the x component and 11 bit unsigned integers 
// for the y and z components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (Z11Y11X10): [32] zzzzzzzz zzzyyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMUDHEN3
{
    union
    {
        struct
        {
            UINT  x  : 10;    // 0 to 1023
            UINT  y  : 11;    // 0 to 2047
            UINT  z  : 11;    // 0 to 2047
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMUDHEN3() {};
    _XMUDHEN3(UINT Packed);
    _XMUDHEN3(FLOAT _x, FLOAT _y, FLOAT _z);
    _XMUDHEN3(CONST FLOAT *pArray);

    operator UINT ();

    _XMUDHEN3& operator= (CONST _XMUDHEN3& UDHen3);
    _XMUDHEN3& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMUDHEN3;

// 4D Vector; 32 bit floating point components
typedef struct _XMFLOAT4
{
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT w;

#ifdef __cplusplus

    _XMFLOAT4() {};
    _XMFLOAT4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMFLOAT4(CONST FLOAT *pArray);

    _XMFLOAT4& operator= (CONST _XMFLOAT4& Float4);

#endif // __cplusplus

} XMFLOAT4;

// 4D Vector; 32 bit floating point components aligned on a 16 byte boundary
typedef __declspec(align(16)) XMFLOAT4 XMFLOAT4A, XMFLOAT4A16;

// 4D Vector; 16 bit floating point components
typedef struct _XMHALF4
{
    HALF x;
    HALF y;
    HALF z;
    HALF w;

#ifdef __cplusplus

    _XMHALF4() {};
    _XMHALF4(HALF _x, HALF _y, HALF _z, HALF _w);
    _XMHALF4(CONST HALF *pArray);
    _XMHALF4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMHALF4(CONST FLOAT *pArray);

    _XMHALF4& operator= (CONST _XMHALF4& Half4);

#endif // __cplusplus

} XMHALF4;

// 4D Vector; 16 bit signed normalized integer components
typedef struct _XMSHORTN4
{
    SHORT x;
    SHORT y;
    SHORT z;
    SHORT w;

#ifdef __cplusplus

    _XMSHORTN4() {};
    _XMSHORTN4(SHORT _x, SHORT _y, SHORT _z, SHORT _w);
    _XMSHORTN4(CONST SHORT *pArray);
    _XMSHORTN4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMSHORTN4(CONST FLOAT *pArray);

    _XMSHORTN4& operator= (CONST _XMSHORTN4& ShortN4);

#endif // __cplusplus

} XMSHORTN4;

// 4D Vector; 16 bit signed integer components
typedef struct _XMSHORT4
{
    SHORT x;
    SHORT y;
    SHORT z;
    SHORT w;

#ifdef __cplusplus

    _XMSHORT4() {};
    _XMSHORT4(SHORT _x, SHORT _y, SHORT _z, SHORT _w);
    _XMSHORT4(CONST SHORT *pArray);
    _XMSHORT4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMSHORT4(CONST FLOAT *pArray);

    _XMSHORT4& operator= (CONST _XMSHORT4& Short4);

#endif // __cplusplus

} XMSHORT4;

// 4D Vector; 16 bit unsigned normalized integer components
typedef struct _XMUSHORTN4
{
    USHORT x;
    USHORT y;
    USHORT z;
    USHORT w;

#ifdef __cplusplus

    _XMUSHORTN4() {};
    _XMUSHORTN4(USHORT _x, USHORT _y, USHORT _z, USHORT _w);
    _XMUSHORTN4(CONST USHORT *pArray);
    _XMUSHORTN4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMUSHORTN4(CONST FLOAT *pArray);

    _XMUSHORTN4& operator= (CONST _XMUSHORTN4& UShortN4);

#endif // __cplusplus

} XMUSHORTN4;

// 4D Vector; 16 bit unsigned integer components
typedef struct _XMUSHORT4
{
    USHORT x;
    USHORT y;
    USHORT z;
    USHORT w;

#ifdef __cplusplus

    _XMUSHORT4() {};
    _XMUSHORT4(USHORT _x, USHORT _y, USHORT _z, USHORT _w);
    _XMUSHORT4(CONST USHORT *pArray);
    _XMUSHORT4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMUSHORT4(CONST FLOAT *pArray);

    _XMUSHORT4& operator= (CONST _XMUSHORT4& UShort4);

#endif // __cplusplus

} XMUSHORT4;

// 4D Vector; 10-10-10-2 bit normalized components packed into a 32 bit integer
// The normalized 4D Vector is packed into 32 bits as follows: a 2 bit unsigned, 
// normalized integer for the w component and 10 bit signed, normalized 
// integers for the z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W2Z10Y10X10): [32] wwzzzzzz zzzzyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMXDECN4
{
    union
    {
        struct
        {
            INT  x   : 10;    // -511/511 to 511/511
            INT  y   : 10;    // -511/511 to 511/511
            INT  z   : 10;    // -511/511 to 511/511
            UINT w   : 2;     //      0/3 to     3/3
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMXDECN4() {};
    _XMXDECN4(UINT Packed);
    _XMXDECN4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMXDECN4(CONST FLOAT *pArray);

    operator UINT ();

    _XMXDECN4& operator= (CONST _XMXDECN4& XDecN4);
    _XMXDECN4& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMXDECN4;

// 4D Vector; 10-10-10-2 bit components packed into a 32 bit integer
// The normalized 4D Vector is packed into 32 bits as follows: a 2 bit unsigned
// integer for the w component and 10 bit signed integers for the 
// z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W2Z10Y10X10): [32] wwzzzzzz zzzzyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMXDEC4
{
    union
    {
        struct
        {
            INT  x   : 10;    // -511 to 511
            INT  y   : 10;    // -511 to 511
            INT  z   : 10;    // -511 to 511
            UINT w   : 2;     //    0 to   3
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMXDEC4() {};
    _XMXDEC4(UINT Packed);
    _XMXDEC4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMXDEC4(CONST FLOAT *pArray);

    operator UINT ();

    _XMXDEC4& operator= (CONST _XMXDEC4& XDec4);
    _XMXDEC4& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMXDEC4;

// 4D Vector; 10-10-10-2 bit normalized components packed into a 32 bit integer
// The normalized 4D Vector is packed into 32 bits as follows: a 2 bit signed, 
// normalized integer for the w component and 10 bit signed, normalized 
// integers for the z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W2Z10Y10X10): [32] wwzzzzzz zzzzyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMDECN4
{
    union
    {
        struct
        {
            INT  x   : 10;    // -511/511 to 511/511
            INT  y   : 10;    // -511/511 to 511/511
            INT  z   : 10;    // -511/511 to 511/511
            INT  w   : 2;     //     -1/1 to     1/1
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMDECN4() {};
    _XMDECN4(UINT Packed);
    _XMDECN4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMDECN4(CONST FLOAT *pArray);

    operator UINT ();

    _XMDECN4& operator= (CONST _XMDECN4& DecN4);
    _XMDECN4& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMDECN4;

// 4D Vector; 10-10-10-2 bit components packed into a 32 bit integer
// The 4D Vector is packed into 32 bits as follows: a 2 bit signed, 
// integer for the w component and 10 bit signed integers for the 
// z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W2Z10Y10X10): [32] wwzzzzzz zzzzyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMDEC4
{
    union
    {
        struct
        {
            INT  x   : 10;    // -511 to 511
            INT  y   : 10;    // -511 to 511
            INT  z   : 10;    // -511 to 511
            INT  w   : 2;     //   -1 to   1
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMDEC4() {};
    _XMDEC4(UINT Packed);
    _XMDEC4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMDEC4(CONST FLOAT *pArray);

    operator UINT ();

    _XMDEC4& operator= (CONST _XMDEC4& Dec4);
    _XMDEC4& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMDEC4;

// 4D Vector; 10-10-10-2 bit normalized components packed into a 32 bit integer
// The normalized 4D Vector is packed into 32 bits as follows: a 2 bit unsigned, 
// normalized integer for the w component and 10 bit unsigned, normalized 
// integers for the z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W2Z10Y10X10): [32] wwzzzzzz zzzzyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMUDECN4
{
    union
    {
        struct
        {
            UINT  x  : 10;    // 0/1023 to 1023/1023
            UINT  y  : 10;    // 0/1023 to 1023/1023
            UINT  z  : 10;    // 0/1023 to 1023/1023
            UINT  w  : 2;     //    0/3 to       3/3
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMUDECN4() {};
    _XMUDECN4(UINT Packed);
    _XMUDECN4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMUDECN4(CONST FLOAT *pArray);

    operator UINT ();

    _XMUDECN4& operator= (CONST _XMUDECN4& UDecN4);
    _XMUDECN4& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMUDECN4;

// 4D Vector; 10-10-10-2 bit components packed into a 32 bit integer
// The 4D Vector is packed into 32 bits as follows: a 2 bit unsigned, 
// integer for the w component and 10 bit unsigned integers 
// for the z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W2Z10Y10X10): [32] wwzzzzzz zzzzyyyy yyyyyyxx xxxxxxxx [0]
typedef struct _XMUDEC4
{
    union
    {
        struct
        {
            UINT  x  : 10;    // 0 to 1023
            UINT  y  : 10;    // 0 to 1023
            UINT  z  : 10;    // 0 to 1023
            UINT  w  : 2;     // 0 to    3
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMUDEC4() {};
    _XMUDEC4(UINT Packed);
    _XMUDEC4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMUDEC4(CONST FLOAT *pArray);

    operator UINT ();

    _XMUDEC4& operator= (CONST _XMUDEC4& UDec4);
    _XMUDEC4& operator= (CONST UINT Packed);

#endif // __cplusplus

} XMUDEC4;

// 4D Vector; 20-20-20-4 bit normalized components packed into a 64 bit integer
// The normalized 4D Vector is packed into 64 bits as follows: a 4 bit unsigned, 
// normalized integer for the w component and 20 bit signed, normalized 
// integers for the z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W4Z20Y20X20): [64] wwwwzzzz zzzzzzzz zzzzzzzz yyyyyyyy yyyyyyyy yyyyxxxx xxxxxxxx xxxxxxxx [0]
typedef struct _XMXICON4
{
    union
    {
        struct
        {
            INT64  x   : 20;    // -524287/524287 to 524287/524287
            INT64  y   : 20;    // -524287/524287 to 524287/524287
            INT64  z   : 20;    // -524287/524287 to 524287/524287
            UINT64 w   : 4;     //           0/15 to         15/15
        };
        UINT64 v;
    };

#ifdef __cplusplus

    _XMXICON4() {};
    _XMXICON4(UINT64 Packed);
    _XMXICON4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMXICON4(CONST FLOAT *pArray);

    operator UINT64 ();

    _XMXICON4& operator= (CONST _XMXICON4& XIcoN4);
    _XMXICON4& operator= (CONST UINT64 Packed);

#endif // __cplusplus

} XMXICON4;

// 4D Vector; 20-20-20-4 bit components packed into a 64 bit integer
// The 4D Vector is packed into 64 bits as follows: a 4 bit unsigned
// integer for the w component and 20 bit signed integers for the 
// z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W4Z20Y20X20): [64] wwwwzzzz zzzzzzzz zzzzzzzz yyyyyyyy yyyyyyyy yyyyxxxx xxxxxxxx xxxxxxxx [0]
typedef struct _XMXICO4
{
    union
    {
        struct
        {
            INT64  x   : 20;    // -524287 to 524287
            INT64  y   : 20;    // -524287 to 524287
            INT64  z   : 20;    // -524287 to 524287
            UINT64 w   : 4;     //       0 to     15
        };
        UINT64 v;
    };

#ifdef __cplusplus

    _XMXICO4() {};
    _XMXICO4(UINT64 Packed);
    _XMXICO4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMXICO4(CONST FLOAT *pArray);

    operator UINT64 ();

    _XMXICO4& operator= (CONST _XMXICO4& XIco4);
    _XMXICO4& operator= (CONST UINT64 Packed);

#endif // __cplusplus

} XMXICO4;

// 4D Vector; 20-20-20-4 bit normalized components packed into a 64 bit integer
// The normalized 4D Vector is packed into 64 bits as follows: a 4 bit signed, 
// normalized integer for the w component and 20 bit signed, normalized 
// integers for the z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W4Z20Y20X20): [64] wwwwzzzz zzzzzzzz zzzzzzzz yyyyyyyy yyyyyyyy yyyyxxxx xxxxxxxx xxxxxxxx [0]
typedef struct _XMICON4
{
    union
    {
        struct
        {
            INT64  x   : 20;    // -524287/524287 to 524287/524287
            INT64  y   : 20;    // -524287/524287 to 524287/524287
            INT64  z   : 20;    // -524287/524287 to 524287/524287
            INT64  w   : 4;     //           -7/7 to           7/7
        };
        UINT64 v;
    };

#ifdef __cplusplus

    _XMICON4() {};
    _XMICON4(UINT64 Packed);
    _XMICON4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMICON4(CONST FLOAT *pArray);

    operator UINT64 ();

    _XMICON4& operator= (CONST _XMICON4& IcoN4);
    _XMICON4& operator= (CONST UINT64 Packed);

#endif // __cplusplus

} XMICON4;

// 4D Vector; 20-20-20-4 bit components packed into a 64 bit integer
// The 4D Vector is packed into 64 bits as follows: a 4 bit signed, 
// integer for the w component and 20 bit signed integers for the 
// z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W4Z20Y20X20): [64] wwwwzzzz zzzzzzzz zzzzzzzz yyyyyyyy yyyyyyyy yyyyxxxx xxxxxxxx xxxxxxxx [0]
typedef struct _XMICO4
{
    union
    {
        struct
        {
            INT64  x   : 20;    // -524287 to 524287
            INT64  y   : 20;    // -524287 to 524287
            INT64  z   : 20;    // -524287 to 524287
            INT64  w   : 4;     //      -7 to      7
        };
        UINT64 v;
    };

#ifdef __cplusplus

    _XMICO4() {};
    _XMICO4(UINT64 Packed);
    _XMICO4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMICO4(CONST FLOAT *pArray);

    operator UINT64 ();

    _XMICO4& operator= (CONST _XMICO4& Ico4);
    _XMICO4& operator= (CONST UINT64 Packed);

#endif // __cplusplus

} XMICO4;

// 4D Vector; 20-20-20-4 bit normalized components packed into a 64 bit integer
// The normalized 4D Vector is packed into 64 bits as follows: a 4 bit unsigned, 
// normalized integer for the w component and 20 bit unsigned, normalized 
// integers for the z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W4Z20Y20X20): [64] wwwwzzzz zzzzzzzz zzzzzzzz yyyyyyyy yyyyyyyy yyyyxxxx xxxxxxxx xxxxxxxx [0]
typedef struct _XMUICON4
{
    union
    {
        struct
        {
            UINT64  x  : 20;    // 0/1048575 to 1048575/1048575
            UINT64  y  : 20;    // 0/1048575 to 1048575/1048575
            UINT64  z  : 20;    // 0/1048575 to 1048575/1048575
            UINT64  w  : 4;     //      0/15 to           15/15
        };
        UINT64 v;
    };

#ifdef __cplusplus

    _XMUICON4() {};
    _XMUICON4(UINT64 Packed);
    _XMUICON4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMUICON4(CONST FLOAT *pArray);

    operator UINT64 ();

    _XMUICON4& operator= (CONST _XMUICON4& UIcoN4);
    _XMUICON4& operator= (CONST UINT64 Packed);

#endif // __cplusplus

} XMUICON4;

// 4D Vector; 20-20-20-4 bit components packed into a 64 bit integer
// The 4D Vector is packed into 64 bits as follows: a 4 bit unsigned 
// integer for the w component and 20 bit unsigned integers for the 
// z, y, and x components.  The w component is stored in the 
// most significant bits and the x component in the least significant bits
// (W4Z20Y20X20): [64] wwwwzzzz zzzzzzzz zzzzzzzz yyyyyyyy yyyyyyyy yyyyxxxx xxxxxxxx xxxxxxxx [0]
typedef struct _XMUICO4
{
    union
    {
        struct
        {
            UINT64  x  : 20;    // 0 to 1048575
            UINT64  y  : 20;    // 0 to 1048575
            UINT64  z  : 20;    // 0 to 1048575
            UINT64  w  : 4;     // 0 to      15
        };
        UINT64 v;
    };

#ifdef __cplusplus

    _XMUICO4() {};
    _XMUICO4(UINT64 Packed);
    _XMUICO4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMUICO4(CONST FLOAT *pArray);

    operator UINT64 ();

    _XMUICO4& operator= (CONST _XMUICO4& UIco4);
    _XMUICO4& operator= (CONST UINT64 Packed);

#endif // __cplusplus

} XMUICO4;

// ARGB Color; 8-8-8-8 bit unsigned normalized integer components packed into
// a 32 bit integer.  The normalized color is packed into 32 bits using 8 bit
// unsigned, normalized integers for the alpha, red, green, and blue components.
// The alpha component is stored in the most significant bits and the blue
// component in the least significant bits (A8R8G8B8):
// [32] aaaaaaaa rrrrrrrr gggggggg bbbbbbbb [0]
typedef struct _XMCOLOR
{
    union
    {
        struct
        {
            UINT b    : 8;  // Blue:    0/255 to 255/255
            UINT g    : 8;  // Green:   0/255 to 255/255
            UINT r    : 8;  // Red:     0/255 to 255/255
            UINT a    : 8;  // Alpha:   0/255 to 255/255
        };
        UINT c;
    };

#ifdef __cplusplus

    _XMCOLOR() {};
    _XMCOLOR(UINT Color);
    _XMCOLOR(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMCOLOR(CONST FLOAT *pArray);

    operator UINT ();

    _XMCOLOR& operator= (CONST _XMCOLOR& Color);
    _XMCOLOR& operator= (CONST UINT Color);

#endif // __cplusplus

} XMCOLOR;

// 4D Vector; 8 bit signed normalized integer components
typedef struct _XMBYTEN4
{
    union
    {
        struct
        {
            CHAR x;
            CHAR y;
            CHAR z;
            CHAR w;
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMBYTEN4() {};
    _XMBYTEN4(CHAR _x, CHAR _y, CHAR _z, CHAR _w);
    _XMBYTEN4(UINT _v);
    _XMBYTEN4(CONST CHAR *pArray);
    _XMBYTEN4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMBYTEN4(CONST FLOAT *pArray);

    _XMBYTEN4& operator= (CONST _XMBYTEN4& ByteN4);

#endif // __cplusplus

} XMBYTEN4;

// 4D Vector; 8 bit signed integer components
typedef struct _XMBYTE4
{
    union
    {
        struct
        {
            CHAR x;
            CHAR y;
            CHAR z;
            CHAR w;
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMBYTE4() {};
    _XMBYTE4(CHAR _x, CHAR _y, CHAR _z, CHAR _w);
    _XMBYTE4(UINT _v);
    _XMBYTE4(CONST CHAR *pArray);
    _XMBYTE4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMBYTE4(CONST FLOAT *pArray);

    _XMBYTE4& operator= (CONST _XMBYTE4& Byte4);

#endif // __cplusplus

} XMBYTE4;

// 4D Vector; 8 bit unsigned normalized integer components
typedef struct _XMUBYTEN4
{
    union
    {
        struct
        {
            BYTE x;
            BYTE y;
            BYTE z;
            BYTE w;
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMUBYTEN4() {};
    _XMUBYTEN4(BYTE _x, BYTE _y, BYTE _z, BYTE _w);
    _XMUBYTEN4(UINT _v);
    _XMUBYTEN4(CONST BYTE *pArray);
    _XMUBYTEN4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMUBYTEN4(CONST FLOAT *pArray);

    _XMUBYTEN4& operator= (CONST _XMUBYTEN4& UByteN4);

#endif // __cplusplus

} XMUBYTEN4;

// 4D Vector; 8 bit unsigned integer components
typedef struct _XMUBYTE4
{
    union
    {
        struct
        {
            BYTE x;
            BYTE y;
            BYTE z;
            BYTE w;
        };
        UINT v;
    };

#ifdef __cplusplus

    _XMUBYTE4() {};
    _XMUBYTE4(BYTE _x, BYTE _y, BYTE _z, BYTE _w);
    _XMUBYTE4(UINT _v);
    _XMUBYTE4(CONST BYTE *pArray);
    _XMUBYTE4(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _w);
    _XMUBYTE4(CONST FLOAT *pArray);

    _XMUBYTE4& operator= (CONST _XMUBYTE4& UByte4);

#endif // __cplusplus

} XMUBYTE4;

// 3x3 Matrix: 32 bit floating point components
typedef struct _XMFLOAT3X3
{
    union
    {
        struct
        {
            FLOAT _11, _12, _13;
            FLOAT _21, _22, _23;
            FLOAT _31, _32, _33;
        };
        FLOAT m[3][3];
    };

#ifdef __cplusplus

                _XMFLOAT3X3() {};
                _XMFLOAT3X3(FLOAT m00, FLOAT m01, FLOAT m02,
                         FLOAT m10, FLOAT m11, FLOAT m12,
                         FLOAT m20, FLOAT m21, FLOAT m22);
                _XMFLOAT3X3(CONST FLOAT *pArray);

    FLOAT       operator() (UINT Row, UINT Column) CONST;
    FLOAT&      operator() (UINT Row, UINT Column);

    _XMFLOAT3X3& operator= (CONST _XMFLOAT3X3& Float3x3);

#endif // __cplusplus

} XMFLOAT3X3;

// 4x3 Matrix: 32 bit floating point components
typedef struct _XMFLOAT4X3
{
    union
    {
        struct
        {
            FLOAT _11, _12, _13;
            FLOAT _21, _22, _23;
            FLOAT _31, _32, _33;
            FLOAT _41, _42, _43;
        };
        FLOAT m[4][3];
    };

#ifdef __cplusplus

                _XMFLOAT4X3() {};
                _XMFLOAT4X3(FLOAT m00, FLOAT m01, FLOAT m02,
                         FLOAT m10, FLOAT m11, FLOAT m12,
                         FLOAT m20, FLOAT m21, FLOAT m22,
                         FLOAT m30, FLOAT m31, FLOAT m32);
                _XMFLOAT4X3(CONST FLOAT *pArray);

    FLOAT       operator() (UINT Row, UINT Column) CONST;
    FLOAT&      operator() (UINT Row, UINT Column);

    _XMFLOAT4X3& operator= (CONST _XMFLOAT4X3& Float4x3);

#endif // __cplusplus

} XMFLOAT4X3;

// 4x3 Matrix: 32 bit floating point components aligned on a 16 byte boundary
typedef __declspec(align(16)) XMFLOAT4X3 XMFLOAT4X3A, XMFLOAT4X3A16;

// 4x4 Matrix: 32 bit floating point components
typedef struct _XMFLOAT4X4
{
    union
    {
        struct
        {
            FLOAT _11, _12, _13, _14;
            FLOAT _21, _22, _23, _24;
            FLOAT _31, _32, _33, _34;
            FLOAT _41, _42, _43, _44;
        };
        FLOAT m[4][4];
    };

#ifdef __cplusplus

                _XMFLOAT4X4() {};
                _XMFLOAT4X4(FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
                         FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
                         FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
                         FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33);
                _XMFLOAT4X4(CONST FLOAT *pArray);

    FLOAT       operator() (UINT Row, UINT Column) CONST;
    FLOAT&      operator() (UINT Row, UINT Column);

    _XMFLOAT4X4& operator= (CONST _XMFLOAT4X4& Float4x4);

#endif // __cplusplus

} XMFLOAT4X4;

// 4x4 Matrix: 32 bit floating point components aligned on a 16 byte boundary
typedef __declspec(align(16)) XMFLOAT4X4 XMFLOAT4X4A, XMFLOAT4X4A16;

#ifndef _X86_
#pragma bitfield_order(pop)
#endif // _X86_

#pragma warning(pop)

/****************************************************************************
 *
 * Data conversion operations
 *
 ****************************************************************************/

FLOAT           XMConvertHalfToFloat(HALF Value);
FLOAT*          XMConvertHalfToFloatStream(FLOAT* pOutputStream, UINT OutputStride, CONST HALF* pInputStream, UINT InputStride, UINT HalfCount);
HALF            XMConvertFloatToHalf(FLOAT Value);
HALF*           XMConvertFloatToHalfStream(HALF* pOutputStream, UINT OutputStride, CONST FLOAT* pInputStream, UINT InputStride, UINT FloatCount);
#if defined(_NO_INTRINSICS_)
XMVECTOR        XMConvertVectorIntToFloat(XMVECTOR VInt, UINT DivExponent);
XMVECTOR        XMConvertVectorUIntToFloat(XMVECTOR VUInt, UINT DivExponent);
XMVECTOR        XMConvertVectorFloatToInt(XMVECTOR VFloat, UINT MulExponent);
XMVECTOR        XMConvertVectorFloatToUInt(XMVECTOR VFloat, UINT MulExponent);
#endif

/****************************************************************************
 *
 * Load operations
 *
 ****************************************************************************/

XMVECTOR        XMLoadScalar(CONST VOID* pSource);

XMVECTOR        XMLoadVector2(CONST VOID* pSource);
XMVECTOR        XMLoadVector2A(CONST VOID* pSource);
XMVECTOR        XMLoadFloat2(CONST XMFLOAT2* pSource);
XMVECTOR        XMLoadFloat2A(CONST XMFLOAT2A* pSource);
XMVECTOR        XMLoadHalf2(CONST XMHALF2* pSource);
XMVECTOR        XMLoadShortN2(CONST XMSHORTN2* pSource);
XMVECTOR        XMLoadShort2(CONST XMSHORT2* pSource);
XMVECTOR        XMLoadUShortN2(CONST XMUSHORTN2* pSource);
XMVECTOR        XMLoadUShort2(CONST XMUSHORT2* pSource);

XMVECTOR        XMLoadVector3(CONST VOID* pSource);
XMVECTOR        XMLoadVector3A(CONST VOID* pSource);
XMVECTOR        XMLoadFloat3(CONST XMFLOAT3* pSource);
XMVECTOR        XMLoadFloat3A(CONST XMFLOAT3A* pSource);
XMVECTOR        XMLoadHenDN3(CONST XMHENDN3* pSource);
XMVECTOR        XMLoadHenD3(CONST XMHEND3* pSource);
XMVECTOR        XMLoadUHenDN3(CONST XMUHENDN3* pSource);
XMVECTOR        XMLoadUHenD3(CONST XMUHEND3* pSource);
XMVECTOR        XMLoadDHenN3(CONST XMDHENN3* pSource);
XMVECTOR        XMLoadDHen3(CONST XMDHEN3* pSource);
XMVECTOR        XMLoadUDHenN3(CONST XMUDHENN3* pSource);
XMVECTOR        XMLoadUDHen3(CONST XMUDHEN3* pSource);

XMVECTOR        XMLoadVector4(CONST VOID* pSource);
XMVECTOR        XMLoadVector4A(CONST VOID* pSource);
XMVECTOR        XMLoadFloat4(CONST XMFLOAT4* pSource);
XMVECTOR        XMLoadFloat4A(CONST XMFLOAT4A* pSource);
XMVECTOR        XMLoadHalf4(CONST XMHALF4* pSource);
XMVECTOR        XMLoadShortN4(CONST XMSHORTN4* pSource);
XMVECTOR        XMLoadShort4(CONST XMSHORT4* pSource);
XMVECTOR        XMLoadUShortN4(CONST XMUSHORTN4* pSource);
XMVECTOR        XMLoadUShort4(CONST XMUSHORT4* pSource);
XMVECTOR        XMLoadXIcoN4(CONST XMXICON4* pSource);
XMVECTOR        XMLoadXIco4(CONST XMXICO4* pSource);
XMVECTOR        XMLoadIcoN4(CONST XMICON4* pSource);
XMVECTOR        XMLoadIco4(CONST XMICO4* pSource);
XMVECTOR        XMLoadUIcoN4(CONST XMUICON4* pSource);
XMVECTOR        XMLoadUIco4(CONST XMUICO4* pSource);
XMVECTOR        XMLoadXDecN4(CONST XMXDECN4* pSource);
XMVECTOR        XMLoadXDec4(CONST XMXDEC4* pSource);
XMVECTOR        XMLoadDecN4(CONST XMDECN4* pSource);
XMVECTOR        XMLoadDec4(CONST XMDEC4* pSource);
XMVECTOR        XMLoadUDecN4(CONST XMUDECN4* pSource);
XMVECTOR        XMLoadUDec4(CONST XMUDEC4* pSource);
XMVECTOR        XMLoadByteN4(CONST XMBYTEN4* pSource);
XMVECTOR        XMLoadByte4(CONST XMBYTE4* pSource);
XMVECTOR        XMLoadUByteN4(CONST XMUBYTEN4* pSource);
XMVECTOR        XMLoadUByte4(CONST XMUBYTE4* pSource);
XMVECTOR        XMLoadColor(CONST XMCOLOR* pSource);

XMMATRIX        XMLoadFloat3x3(CONST XMFLOAT3X3* pSource);
XMMATRIX        XMLoadFloat4x3(CONST XMFLOAT4X3* pSource);
XMMATRIX        XMLoadFloat4x3A(CONST XMFLOAT4X3A* pSource);
XMMATRIX        XMLoadFloat4x4(CONST XMFLOAT4X4* pSource);
XMMATRIX        XMLoadFloat4x4A(CONST XMFLOAT4X4A* pSource);

/****************************************************************************
 *
 * Store operations
 *
 ****************************************************************************/

VOID            XMStoreScalar(VOID* pDestination, XMVECTOR V);

VOID            XMStoreVector2(VOID* pDestination, XMVECTOR V);
VOID            XMStoreVector2A(VOID* pDestination, XMVECTOR V);
VOID            XMStoreFloat2(XMFLOAT2* pDestination, XMVECTOR V);
VOID            XMStoreFloat2A(XMFLOAT2A* pDestination, XMVECTOR V);
VOID            XMStoreHalf2(XMHALF2* pDestination, XMVECTOR V);
VOID            XMStoreShortN2(XMSHORTN2* pDestination, XMVECTOR V);
VOID            XMStoreShort2(XMSHORT2* pDestination, XMVECTOR V);
VOID            XMStoreUShortN2(XMUSHORTN2* pDestination, XMVECTOR V);
VOID            XMStoreUShort2(XMUSHORT2* pDestination, XMVECTOR V);

VOID            XMStoreVector3(VOID* pDestination, XMVECTOR V);
VOID            XMStoreVector3A(VOID* pDestination, XMVECTOR V);
VOID            XMStoreFloat3(XMFLOAT3* pDestination, XMVECTOR V);
VOID            XMStoreFloat3A(XMFLOAT3A* pDestination, XMVECTOR V);
VOID            XMStoreHenDN3(XMHENDN3* pDestination, XMVECTOR V);
VOID            XMStoreHenD3(XMHEND3* pDestination, XMVECTOR V);
VOID            XMStoreUHenDN3(XMUHENDN3* pDestination, XMVECTOR V);
VOID            XMStoreUHenD3(XMUHEND3* pDestination, XMVECTOR V);
VOID            XMStoreDHenN3(XMDHENN3* pDestination, XMVECTOR V);
VOID            XMStoreDHen3(XMDHEN3* pDestination, XMVECTOR V);
VOID            XMStoreUDHenN3(XMUDHENN3* pDestination, XMVECTOR V);
VOID            XMStoreUDHen3(XMUDHEN3* pDestination, XMVECTOR V);

VOID            XMStoreVector4(VOID* pDestination, XMVECTOR V);
VOID            XMStoreVector4A(VOID* pDestination, XMVECTOR V);
VOID            XMStoreVector4NC(VOID* pDestination, XMVECTOR V);
VOID            XMStoreFloat4(XMFLOAT4* pDestination, XMVECTOR V);
VOID            XMStoreFloat4A(XMFLOAT4A* pDestination, XMVECTOR V);
VOID            XMStoreFloat4NC(XMFLOAT4* pDestination, XMVECTOR V);
VOID            XMStoreHalf4(XMHALF4* pDestination, XMVECTOR V);
VOID            XMStoreShortN4(XMSHORTN4* pDestination, XMVECTOR V);
VOID            XMStoreShort4(XMSHORT4* pDestination, XMVECTOR V);
VOID            XMStoreUShortN4(XMUSHORTN4* pDestination, XMVECTOR V);
VOID            XMStoreUShort4(XMUSHORT4* pDestination, XMVECTOR V);
VOID            XMStoreXIcoN4(XMXICON4* pDestination, XMVECTOR V);
VOID            XMStoreXIco4(XMXICO4* pDestination, XMVECTOR V);
VOID            XMStoreIcoN4(XMICON4* pDestination, XMVECTOR V);
VOID            XMStoreIco4(XMICO4* pDestination, XMVECTOR V);
VOID            XMStoreUIcoN4(XMUICON4* pDestination, XMVECTOR V);
VOID            XMStoreUIco4(XMUICO4* pDestination, XMVECTOR V);
VOID            XMStoreXDecN4(XMXDECN4* pDestination, XMVECTOR V);
VOID            XMStoreXDec4(XMXDEC4* pDestination, XMVECTOR V);
VOID            XMStoreDecN4(XMDECN4* pDestination, XMVECTOR V);
VOID            XMStoreDec4(XMDEC4* pDestination, XMVECTOR V);
VOID            XMStoreUDecN4(XMUDECN4* pDestination, XMVECTOR V);
VOID            XMStoreUDec4(XMUDEC4* pDestination, XMVECTOR V);
VOID            XMStoreByteN4(XMBYTEN4* pDestination, XMVECTOR V);
VOID            XMStoreByte4(XMBYTE4* pDestination, XMVECTOR V);
VOID            XMStoreUByteN4(XMUBYTEN4* pDestination, XMVECTOR V);
VOID            XMStoreUByte4(XMUBYTE4* pDestination, XMVECTOR V);
VOID            XMStoreColor(XMCOLOR* pDestination, XMVECTOR V);

VOID            XMStoreFloat3x3(XMFLOAT3X3* pDestination, XMMATRIX M);
VOID            XMStoreFloat3x3NC(XMFLOAT3X3* pDestination, XMMATRIX M);
VOID            XMStoreFloat4x3(XMFLOAT4X3* pDestination, XMMATRIX M);
VOID            XMStoreFloat4x3A(XMFLOAT4X3A* pDestination, XMMATRIX M);
VOID            XMStoreFloat4x3NC(XMFLOAT4X3* pDestination, XMMATRIX M);
VOID            XMStoreFloat4x4(XMFLOAT4X4* pDestination, XMMATRIX M);
VOID            XMStoreFloat4x4A(XMFLOAT4X4A* pDestination, XMMATRIX M);
VOID            XMStoreFloat4x4NC(XMFLOAT4X4* pDestination, XMMATRIX M);

/****************************************************************************
 *
 * General vector operations
 *
 ****************************************************************************/

XMVECTOR        XMVectorZero();
XMVECTOR        XMVectorSet(FLOAT x, FLOAT y, FLOAT z, FLOAT w);
XMVECTOR        XMVectorSetInt(UINT x, UINT y, UINT z, UINT w);
XMVECTOR        XMVectorReplicate(FLOAT Value);
XMVECTOR        XMVectorTrueInt();
XMVECTOR        XMVectorFalseInt();
XMVECTOR        XMVectorSplatX(XMVECTOR V);
XMVECTOR        XMVectorSplatY(XMVECTOR V);
XMVECTOR        XMVectorSplatZ(XMVECTOR V);
XMVECTOR        XMVectorSplatW(XMVECTOR V);
XMVECTOR        XMVectorSplatOne();
XMVECTOR        XMVectorSplatInfinity();
XMVECTOR        XMVectorSplatQNaN();
XMVECTOR        XMVectorSplatEpsilon();
XMVECTOR        XMVectorSplatSignMask();
XMVECTOR        XMVectorPermuteControl(UINT ElementIndex0, UINT ElementIndex1, UINT ElementIndex2, UINT ElementIndex3);
XMVECTOR        XMVectorPermute(XMVECTOR V1, XMVECTOR V2, XMVECTOR Control);
XMVECTOR        XMVectorSelectControl(UINT VectorIndex0, UINT VectorIndex1, UINT VectorIndex2, UINT VectorIndex3);
XMVECTOR        XMVectorSelect(XMVECTOR V1, XMVECTOR V2, XMVECTOR Control);
XMVECTOR        XMVectorMergeXY(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorMergeZW(XMVECTOR V1, XMVECTOR V2);

XMVECTOR        XMVectorEqual(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorEqualR(UINT* pCR, XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorEqualInt(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorEqualIntR(UINT* pCR, XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorNearEqual(XMVECTOR V1, XMVECTOR V2, XMVECTOR Epsilon);
XMVECTOR        XMVectorNotEqual(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorNotEqualInt(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorGreater(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorGreaterR(UINT* pCR, XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorGreaterOrEqual(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorGreaterOrEqualR(UINT* pCR, XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorLess(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorLessOrEqual(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorInBounds(XMVECTOR V, XMVECTOR Bounds);
XMVECTOR        XMVectorInBoundsR(UINT* pCR, XMVECTOR V, XMVECTOR Bounds);

XMVECTOR        XMVectorIsNaN(XMVECTOR V);
XMVECTOR        XMVectorIsInfinite(XMVECTOR V);

XMVECTOR        XMVectorMin(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorMax(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorRound(XMVECTOR V);
XMVECTOR        XMVectorTruncate(XMVECTOR V);
XMVECTOR        XMVectorFloor(XMVECTOR V);
XMVECTOR        XMVectorCeiling(XMVECTOR V);
XMVECTOR        XMVectorClamp(XMVECTOR V, XMVECTOR Min, XMVECTOR Max);
XMVECTOR        XMVectorSaturate(XMVECTOR V);

XMVECTOR        XMVectorAndInt(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorAndCInt(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorOrInt(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorNorInt(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorXorInt(XMVECTOR V1, XMVECTOR V2);

XMVECTOR        XMVectorNegate(XMVECTOR V);
XMVECTOR        XMVectorAdd(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorAddAngles(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorSubtract(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorSubtractAngles(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorMultiply(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorMultiplyAdd(XMVECTOR V1, XMVECTOR V2, XMVECTOR V3);
XMVECTOR        XMVectorNegativeMultiplySubtract(XMVECTOR V1, XMVECTOR V2, XMVECTOR V3);
XMVECTOR        XMVectorScale(XMVECTOR V, FLOAT ScaleFactor);
XMVECTOR        XMVectorReciprocalEst(XMVECTOR V);
XMVECTOR        XMVectorReciprocal(XMVECTOR V);
XMVECTOR        XMVectorSqrtEst(XMVECTOR V);
XMVECTOR        XMVectorSqrt(XMVECTOR V);
XMVECTOR        XMVectorReciprocalSqrtEst(XMVECTOR V);
XMVECTOR        XMVectorReciprocalSqrt(XMVECTOR V);
XMVECTOR        XMVectorExpEst(XMVECTOR V);
XMVECTOR        XMVectorExp(XMVECTOR V);
XMVECTOR        XMVectorLogEst(XMVECTOR V);
XMVECTOR        XMVectorLog(XMVECTOR V);
XMVECTOR        XMVectorPowEst(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorPow(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorAbs(XMVECTOR V);
XMVECTOR        XMVectorMod(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVectorModAngles(XMVECTOR Angles);
XMVECTOR        XMVectorSin(XMVECTOR V);
XMVECTOR        XMVectorSinEst(XMVECTOR V);
XMVECTOR        XMVectorCos(XMVECTOR V);
XMVECTOR        XMVectorCosEst(XMVECTOR V);
VOID            XMVectorSinCos(XMVECTOR* pSin, XMVECTOR* pCos, XMVECTOR V);
VOID            XMVectorSinCosEst(XMVECTOR* pSin, XMVECTOR* pCos, XMVECTOR V);
XMVECTOR        XMVectorTan(XMVECTOR V);
XMVECTOR        XMVectorTanEst(XMVECTOR V);
XMVECTOR        XMVectorSinH(XMVECTOR V);
XMVECTOR        XMVectorSinHEst(XMVECTOR V);
XMVECTOR        XMVectorCosH(XMVECTOR V);
XMVECTOR        XMVectorCosHEst(XMVECTOR V);
XMVECTOR        XMVectorTanH(XMVECTOR V);
XMVECTOR        XMVectorTanHEst(XMVECTOR V);
XMVECTOR        XMVectorASin(XMVECTOR V);
XMVECTOR        XMVectorASinEst(XMVECTOR V);
XMVECTOR        XMVectorACos(XMVECTOR V);
XMVECTOR        XMVectorACosEst(XMVECTOR V);
XMVECTOR        XMVectorATan(XMVECTOR V);
XMVECTOR        XMVectorATanEst(XMVECTOR V);
XMVECTOR        XMVectorATan2(XMVECTOR Y, XMVECTOR X);
XMVECTOR        XMVectorATan2Est(XMVECTOR Y, XMVECTOR X);
XMVECTOR        XMVectorLerp(XMVECTOR V0, XMVECTOR V1, FLOAT t);
XMVECTOR        XMVectorLerpV(XMVECTOR V0, XMVECTOR V1, XMVECTOR T);
XMVECTOR        XMVectorHermite(XMVECTOR Position0, XMVECTOR Tangent0, XMVECTOR Position1, XMVECTOR Tangent1, FLOAT t);
XMVECTOR        XMVectorHermiteV(XMVECTOR Position0, XMVECTOR Tangent0, XMVECTOR Position1, XMVECTOR Tangent1, XMVECTOR T);
XMVECTOR        XMVectorCatmullRom(XMVECTOR Position0, XMVECTOR Position1, XMVECTOR Position2, XMVECTOR Position3, FLOAT t);
XMVECTOR        XMVectorCatmullRomV(XMVECTOR Position0, XMVECTOR Position1, XMVECTOR Position2, XMVECTOR Position3, XMVECTOR T);
XMVECTOR        XMVectorBaryCentric(XMVECTOR Position0, XMVECTOR Position1, XMVECTOR Position2, FLOAT f, FLOAT g);
XMVECTOR        XMVectorBaryCentricV(XMVECTOR Position0, XMVECTOR Position1, XMVECTOR Position2, XMVECTOR F, XMVECTOR G);

/****************************************************************************
 *
 * 2D vector operations
 *
 ****************************************************************************/

BOOL            XMVector2Equal(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector2EqualR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector2EqualInt(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector2EqualIntR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector2NearEqual(XMVECTOR V1, XMVECTOR V2, XMVECTOR Epsilon);
BOOL            XMVector2NotEqual(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector2NotEqualInt(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector2Greater(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector2GreaterR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector2GreaterOrEqual(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector2GreaterOrEqualR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector2Less(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector2LessOrEqual(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector2InBounds(XMVECTOR V, XMVECTOR Bounds);
UINT            XMVector2InBoundsR(XMVECTOR V, XMVECTOR Bounds);

BOOL            XMVector2IsNaN(XMVECTOR V);
BOOL            XMVector2IsInfinite(XMVECTOR V);

XMVECTOR        XMVector2Dot(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVector2Cross(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVector2LengthSq(XMVECTOR V);
XMVECTOR        XMVector2ReciprocalLengthEst(XMVECTOR V);
XMVECTOR        XMVector2ReciprocalLength(XMVECTOR V);
XMVECTOR        XMVector2LengthEst(XMVECTOR V);
XMVECTOR        XMVector2Length(XMVECTOR V);
XMVECTOR        XMVector2NormalizeEst(XMVECTOR V);
XMVECTOR        XMVector2Normalize(XMVECTOR V);
XMVECTOR        XMVector2ClampLength(XMVECTOR V, FLOAT LengthMin, FLOAT LengthMax);
XMVECTOR        XMVector2ClampLengthV(XMVECTOR V, XMVECTOR LengthMin, XMVECTOR LengthMax);
XMVECTOR        XMVector2Reflect(XMVECTOR Incident, XMVECTOR Normal);
XMVECTOR        XMVector2Refract(XMVECTOR Incident, XMVECTOR Normal, FLOAT RefractionIndex);
XMVECTOR        XMVector2RefractV(XMVECTOR Incident, XMVECTOR Normal, XMVECTOR RefractionIndex);
XMVECTOR        XMVector2Orthogonal(XMVECTOR V);
XMVECTOR        XMVector2AngleBetweenNormalsEst(XMVECTOR N1, XMVECTOR N2);
XMVECTOR        XMVector2AngleBetweenNormals(XMVECTOR N1, XMVECTOR N2);
XMVECTOR        XMVector2AngleBetweenVectors(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVector2LinePointDistance(XMVECTOR LinePoint1, XMVECTOR LinePoint2, XMVECTOR Point);
XMVECTOR        XMVector2IntersectLine(XMVECTOR Line1Point1, XMVECTOR Line1Point2, XMVECTOR Line2Point1, XMVECTOR Line2Point2);
XMVECTOR        XMVector2Transform(XMVECTOR V, XMMATRIX M);
XMFLOAT4*       XMVector2TransformStream(XMFLOAT4* pOutputStream, UINT OutputStride, CONST XMFLOAT2* pInputStream, UINT InputStride, UINT VectorCount, XMMATRIX M);
XMVECTOR        XMVector2TransformCoord(XMVECTOR V, XMMATRIX M);
XMFLOAT2*       XMVector2TransformCoordStream(XMFLOAT2* pOutputStream, UINT OutputStride, CONST XMFLOAT2* pInputStream, UINT InputStride, UINT VectorCount, XMMATRIX M);
XMVECTOR        XMVector2TransformNormal(XMVECTOR V, XMMATRIX M);
XMFLOAT2*       XMVector2TransformNormalStream(XMFLOAT2* pOutputStream, UINT OutputStride, CONST XMFLOAT2* pInputStream, UINT InputStride, UINT VectorCount, XMMATRIX M);

/****************************************************************************
 *
 * 3D vector operations
 *
 ****************************************************************************/

BOOL            XMVector3Equal(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector3EqualR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector3EqualInt(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector3EqualIntR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector3NearEqual(XMVECTOR V1, XMVECTOR V2, XMVECTOR Epsilon);
BOOL            XMVector3NotEqual(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector3NotEqualInt(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector3Greater(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector3GreaterR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector3GreaterOrEqual(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector3GreaterOrEqualR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector3Less(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector3LessOrEqual(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector3InBounds(XMVECTOR V, XMVECTOR Bounds);
UINT            XMVector3InBoundsR(XMVECTOR V, XMVECTOR Bounds);

BOOL            XMVector3IsNaN(XMVECTOR V);
BOOL            XMVector3IsInfinite(XMVECTOR V);

XMVECTOR        XMVector3Dot(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVector3Cross(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVector3LengthSq(XMVECTOR V);
XMVECTOR        XMVector3ReciprocalLengthEst(XMVECTOR V);
XMVECTOR        XMVector3ReciprocalLength(XMVECTOR V);
XMVECTOR        XMVector3LengthEst(XMVECTOR V);
XMVECTOR        XMVector3Length(XMVECTOR V);
XMVECTOR        XMVector3NormalizeEst(XMVECTOR V);
XMVECTOR        XMVector3Normalize(XMVECTOR V);
XMVECTOR        XMVector3ClampLength(XMVECTOR V, FLOAT LengthMin, FLOAT LengthMax);
XMVECTOR        XMVector3ClampLengthV(XMVECTOR V, XMVECTOR LengthMin, XMVECTOR LengthMax);
XMVECTOR        XMVector3Reflect(XMVECTOR Incident, XMVECTOR Normal);
XMVECTOR        XMVector3Refract(XMVECTOR Incident, XMVECTOR Normal, FLOAT RefractionIndex);
XMVECTOR        XMVector3RefractV(XMVECTOR Incident, XMVECTOR Normal, XMVECTOR RefractionIndex);
XMVECTOR        XMVector3Orthogonal(XMVECTOR V);
XMVECTOR        XMVector3AngleBetweenNormalsEst(XMVECTOR N1, XMVECTOR N2);
XMVECTOR        XMVector3AngleBetweenNormals(XMVECTOR N1, XMVECTOR N2);
XMVECTOR        XMVector3AngleBetweenVectors(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVector3LinePointDistance(XMVECTOR LinePoint1, XMVECTOR LinePoint2, XMVECTOR Point);
VOID            XMVector3ComponentsFromNormal(XMVECTOR* pParallel, XMVECTOR* pPerpendicular, XMVECTOR V, XMVECTOR Normal);
XMVECTOR        XMVector3Rotate(XMVECTOR V, XMVECTOR RotationQuaternion);
XMVECTOR        XMVector3InverseRotate(XMVECTOR V, XMVECTOR RotationQuaternion);
XMVECTOR        XMVector3Transform(XMVECTOR V, XMMATRIX M);
XMFLOAT4*       XMVector3TransformStream(XMFLOAT4* pOutputStream, UINT OutputStride, CONST XMFLOAT3* pInputStream, UINT InputStride, UINT VectorCount, XMMATRIX M);
XMVECTOR        XMVector3TransformCoord(XMVECTOR V, XMMATRIX M);
XMFLOAT3*       XMVector3TransformCoordStream(XMFLOAT3* pOutputStream, UINT OutputStride, CONST XMFLOAT3* pInputStream, UINT InputStride, UINT VectorCount, XMMATRIX M);
XMVECTOR        XMVector3TransformNormal(XMVECTOR V, XMMATRIX M);
XMFLOAT3*       XMVector3TransformNormalStream(XMFLOAT3* pOutputStream, UINT OutputStride, CONST XMFLOAT3* pInputStream, UINT InputStride, UINT VectorCount, XMMATRIX M);
XMVECTOR        XMVector3Project(XMVECTOR V, FLOAT ViewportX, FLOAT ViewportY, FLOAT ViewportWidth, FLOAT ViewportHeight, FLOAT ViewportMinZ, FLOAT ViewportMaxZ, 
                    XMMATRIX Projection, XMMATRIX View, XMMATRIX World);
XMFLOAT3*       XMVector3ProjectStream(XMFLOAT3* pOutputStream, UINT OutputStride, CONST XMFLOAT3* pInputStream, UINT InputStride, UINT VectorCount, 
                    FLOAT ViewportX, FLOAT ViewportY, FLOAT ViewportWidth, FLOAT ViewportHeight, FLOAT ViewportMinZ, FLOAT ViewportMaxZ, 
                    XMMATRIX Projection, XMMATRIX View, XMMATRIX World);
XMVECTOR        XMVector3Unproject(XMVECTOR V, FLOAT ViewportX, FLOAT ViewportY, FLOAT ViewportWidth, FLOAT ViewportHeight, FLOAT ViewportMinZ, FLOAT ViewportMaxZ, 
                    XMMATRIX Projection, XMMATRIX View, XMMATRIX World);
XMFLOAT3*       XMVector3UnprojectStream(XMFLOAT3* pOutputStream, UINT OutputStride, CONST XMFLOAT3* pInputStream, UINT InputStride, UINT VectorCount, 
                    FLOAT ViewportX, FLOAT ViewportY, FLOAT ViewportWidth, FLOAT ViewportHeight, FLOAT ViewportMinZ, FLOAT ViewportMaxZ, 
                    XMMATRIX Projection, XMMATRIX View, XMMATRIX World);

/****************************************************************************
 *
 * 4D vector operations
 *
 ****************************************************************************/

BOOL            XMVector4Equal(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector4EqualR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector4EqualInt(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector4EqualIntR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector4NearEqual(XMVECTOR V1, XMVECTOR V2, XMVECTOR Epsilon);
BOOL            XMVector4NotEqual(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector4NotEqualInt(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector4Greater(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector4GreaterR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector4GreaterOrEqual(XMVECTOR V1, XMVECTOR V2);
UINT            XMVector4GreaterOrEqualR(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector4Less(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector4LessOrEqual(XMVECTOR V1, XMVECTOR V2);
BOOL            XMVector4InBounds(XMVECTOR V, XMVECTOR Bounds);
UINT            XMVector4InBoundsR(XMVECTOR V, XMVECTOR Bounds);

BOOL            XMVector4IsNaN(XMVECTOR V);
BOOL            XMVector4IsInfinite(XMVECTOR V);

XMVECTOR        XMVector4Dot(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVector4Cross(XMVECTOR V1, XMVECTOR V2, XMVECTOR V3);
XMVECTOR        XMVector4LengthSq(XMVECTOR V);
XMVECTOR        XMVector4ReciprocalLengthEst(XMVECTOR V);
XMVECTOR        XMVector4ReciprocalLength(XMVECTOR V);
XMVECTOR        XMVector4LengthEst(XMVECTOR V);
XMVECTOR        XMVector4Length(XMVECTOR V);
XMVECTOR        XMVector4NormalizeEst(XMVECTOR V);
XMVECTOR        XMVector4Normalize(XMVECTOR V);
XMVECTOR        XMVector4ClampLength(XMVECTOR V, FLOAT LengthMin, FLOAT LengthMax);
XMVECTOR        XMVector4ClampLengthV(XMVECTOR V, XMVECTOR LengthMin, XMVECTOR LengthMax);
XMVECTOR        XMVector4Reflect(XMVECTOR Incident, XMVECTOR Normal);
XMVECTOR        XMVector4Refract(XMVECTOR Incident, XMVECTOR Normal, FLOAT RefractionIndex);
XMVECTOR        XMVector4RefractV(XMVECTOR Incident, XMVECTOR Normal, XMVECTOR RefractionIndex);
XMVECTOR        XMVector4Orthogonal(XMVECTOR V);
XMVECTOR        XMVector4AngleBetweenNormalsEst(XMVECTOR N1, XMVECTOR N2);
XMVECTOR        XMVector4AngleBetweenNormals(XMVECTOR N1, XMVECTOR N2);
XMVECTOR        XMVector4AngleBetweenVectors(XMVECTOR V1, XMVECTOR V2);
XMVECTOR        XMVector4Transform(XMVECTOR V, XMMATRIX M);
XMFLOAT4*       XMVector4TransformStream(XMFLOAT4* pOutputStream, UINT OutputStride, CONST XMFLOAT4* pInputStream, UINT InputStride, UINT VectorCount, XMMATRIX M);

/****************************************************************************
 *
 * Matrix operations
 *
 ****************************************************************************/

BOOL            XMMatrixIsNaN(XMMATRIX M);
BOOL            XMMatrixIsInfinite(XMMATRIX M);
BOOL            XMMatrixIsIdentity(XMMATRIX M);

XMMATRIX        XMMatrixMultiply(XMMATRIX M1, XMMATRIX M2);
XMMATRIX        XMMatrixMultiplyTranspose(XMMATRIX M1, XMMATRIX M2);
XMMATRIX        XMMatrixTranspose(XMMATRIX M);
XMMATRIX        XMMatrixInverse(XMVECTOR* pDeterminant, XMMATRIX M);
XMVECTOR        XMMatrixDeterminant(XMMATRIX M);

XMMATRIX        XMMatrixIdentity();
XMMATRIX        XMMatrixSet(FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
                         FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
                         FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
                         FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33);
XMMATRIX        XMMatrixTranslation(FLOAT OffsetX, FLOAT OffsetY, FLOAT OffsetZ);
XMMATRIX        XMMatrixTranslationFromVector(XMVECTOR Offset);
XMMATRIX        XMMatrixScaling(FLOAT ScaleX, FLOAT ScaleY, FLOAT ScaleZ);
XMMATRIX        XMMatrixScalingFromVector(XMVECTOR Scale);
XMMATRIX        XMMatrixRotationX(FLOAT Angle);
XMMATRIX        XMMatrixRotationY(FLOAT Angle);
XMMATRIX        XMMatrixRotationZ(FLOAT Angle);
XMMATRIX        XMMatrixRotationRollPitchYaw(FLOAT Pitch, FLOAT Yaw, FLOAT Roll);
XMMATRIX        XMMatrixRotationRollPitchYawFromVector(XMVECTOR Angles);
XMMATRIX        XMMatrixRotationNormal(XMVECTOR NormalAxis, FLOAT Angle);
XMMATRIX        XMMatrixRotationAxis(XMVECTOR Axis, FLOAT Angle);
XMMATRIX        XMMatrixRotationQuaternion(XMVECTOR Quaternion);
XMMATRIX        XMMatrixTransformation2D(XMVECTOR ScalingOrigin, FLOAT ScalingOrientation, XMVECTOR Scaling, 
                    XMVECTOR RotationOrigin, FLOAT Rotation, XMVECTOR Translation);
XMMATRIX        XMMatrixTransformation(XMVECTOR ScalingOrigin, XMVECTOR ScalingOrientationQuaternion, XMVECTOR Scaling, 
                    XMVECTOR RotationOrigin, XMVECTOR RotationQuaternion, XMVECTOR Translation);
XMMATRIX        XMMatrixAffineTransformation2D(XMVECTOR Scaling, XMVECTOR RotationOrigin, FLOAT Rotation, XMVECTOR Translation);
XMMATRIX        XMMatrixAffineTransformation(XMVECTOR Scaling, XMVECTOR RotationOrigin, XMVECTOR RotationQuaternion, XMVECTOR Translation);
XMMATRIX        XMMatrixReflect(XMVECTOR ReflectionPlane);
XMMATRIX        XMMatrixShadow(XMVECTOR ShadowPlane, XMVECTOR LightPosition);

XMMATRIX        XMMatrixLookAtLH(XMVECTOR EyePosition, XMVECTOR FocusPosition, XMVECTOR UpDirection);
XMMATRIX        XMMatrixLookAtRH(XMVECTOR EyePosition, XMVECTOR FocusPosition, XMVECTOR UpDirection);
XMMATRIX        XMMatrixLookToLH(XMVECTOR EyePosition, XMVECTOR EyeDirection, XMVECTOR UpDirection);
XMMATRIX        XMMatrixLookToRH(XMVECTOR EyePosition, XMVECTOR EyeDirection, XMVECTOR UpDirection);
XMMATRIX        XMMatrixPerspectiveLH(FLOAT ViewWidth, FLOAT ViewHeight, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveRH(FLOAT ViewWidth, FLOAT ViewHeight, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveFovLH(FLOAT FovAngleY, FLOAT AspectHByW, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveFovRH(FLOAT FovAngleY, FLOAT AspectHByW, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveOffCenterLH(FLOAT ViewLeft, FLOAT ViewRight, FLOAT ViewBottom, FLOAT ViewTop, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveOffCenterRH(FLOAT ViewLeft, FLOAT ViewRight, FLOAT ViewBottom, FLOAT ViewTop, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixOrthographicLH(FLOAT ViewWidth, FLOAT ViewHeight, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixOrthographicRH(FLOAT ViewWidth, FLOAT ViewHeight, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixOrthographicOffCenterLH(FLOAT ViewLeft, FLOAT ViewRight, FLOAT ViewBottom, FLOAT ViewTop, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixOrthographicOffCenterRH(FLOAT ViewLeft, FLOAT ViewRight, FLOAT ViewBottom, FLOAT ViewTop, FLOAT NearZ, FLOAT FarZ);

/****************************************************************************
 *
 * Quaternion operations
 *
 ****************************************************************************/

BOOL            XMQuaternionEqual(XMVECTOR Q1, XMVECTOR Q2);
BOOL            XMQuaternionNotEqual(XMVECTOR Q1, XMVECTOR Q2);

BOOL            XMQuaternionIsNaN(XMVECTOR Q);
BOOL            XMQuaternionIsInfinite(XMVECTOR Q);
BOOL            XMQuaternionIsIdentity(XMVECTOR Q);

XMVECTOR        XMQuaternionDot(XMVECTOR Q1, XMVECTOR Q2);
XMVECTOR        XMQuaternionMultiply(XMVECTOR Q1, XMVECTOR Q2);
XMVECTOR        XMQuaternionLengthSq(XMVECTOR Q);
XMVECTOR        XMQuaternionReciprocalLength(XMVECTOR Q);
XMVECTOR        XMQuaternionLength(XMVECTOR Q);
XMVECTOR        XMQuaternionNormalizeEst(XMVECTOR Q);
XMVECTOR        XMQuaternionNormalize(XMVECTOR Q);
XMVECTOR        XMQuaternionConjugate(XMVECTOR Q);
XMVECTOR        XMQuaternionInverse(XMVECTOR Q);
XMVECTOR        XMQuaternionLn(XMVECTOR Q);
XMVECTOR        XMQuaternionExp(XMVECTOR Q);
XMVECTOR        XMQuaternionSlerp(XMVECTOR Q0, XMVECTOR Q1, FLOAT t);
XMVECTOR        XMQuaternionSlerpV(XMVECTOR Q0, XMVECTOR Q1, XMVECTOR T);
XMVECTOR        XMQuaternionSquad(XMVECTOR Q0, XMVECTOR Q1, XMVECTOR Q2, XMVECTOR Q3, FLOAT t);
XMVECTOR        XMQuaternionSquadV(XMVECTOR Q0, XMVECTOR Q1, XMVECTOR Q2, XMVECTOR Q3, XMVECTOR T);
VOID            XMQuaternionSquadSetup(XMVECTOR* pA, XMVECTOR* pB, XMVECTOR* pC, XMVECTOR Q0, XMVECTOR Q1, XMVECTOR Q2, XMVECTOR Q3);
XMVECTOR        XMQuaternionBaryCentric(XMVECTOR Q0, XMVECTOR Q1, XMVECTOR Q2, FLOAT f, FLOAT g);
XMVECTOR        XMQuaternionBaryCentricV(XMVECTOR Q0, XMVECTOR Q1, XMVECTOR Q2, XMVECTOR F, XMVECTOR G);

XMVECTOR        XMQuaternionIdentity();
XMVECTOR        XMQuaternionRotationRollPitchYaw(FLOAT Pitch, FLOAT Yaw, FLOAT Roll);
XMVECTOR        XMQuaternionRotationRollPitchYawFromVector(XMVECTOR Angles);
XMVECTOR        XMQuaternionRotationNormal(XMVECTOR NormalAxis, FLOAT Angle);
XMVECTOR        XMQuaternionRotationAxis(XMVECTOR Axis, FLOAT Angle);
XMVECTOR        XMQuaternionRotationMatrix(XMMATRIX M);

VOID            XMQuaternionToAxisAngle(XMVECTOR* pAxis, FLOAT* pAngle, XMVECTOR Q);

/****************************************************************************
 *
 * Plane operations
 *
 ****************************************************************************/

BOOL            XMPlaneEqual(XMVECTOR P1, XMVECTOR P2);
BOOL            XMPlaneNearEqual(XMVECTOR P1, XMVECTOR P2, XMVECTOR Epsilon);
BOOL            XMPlaneNotEqual(XMVECTOR P1, XMVECTOR P2);

BOOL            XMPlaneIsNaN(XMVECTOR P);
BOOL            XMPlaneIsInfinite(XMVECTOR P);

XMVECTOR        XMPlaneDot(XMVECTOR P, XMVECTOR V);
XMVECTOR        XMPlaneDotCoord(XMVECTOR P, XMVECTOR V);
XMVECTOR        XMPlaneDotNormal(XMVECTOR P, XMVECTOR V);
XMVECTOR        XMPlaneNormalizeEst(XMVECTOR P);
XMVECTOR        XMPlaneNormalize(XMVECTOR P);
XMVECTOR        XMPlaneIntersectLine(XMVECTOR P, XMVECTOR LinePoint1, XMVECTOR LinePoint2);
VOID            XMPlaneIntersectPlane(XMVECTOR* pLinePoint1, XMVECTOR* pLinePoint2, XMVECTOR P1, XMVECTOR P2);
XMVECTOR        XMPlaneTransform(XMVECTOR P, XMMATRIX M);
XMFLOAT4*       XMPlaneTransformStream(XMFLOAT4* pOutputStream, UINT OutputStride, CONST XMFLOAT4* pInputStream, UINT InputStride, UINT PlaneCount, XMMATRIX M);

XMVECTOR        XMPlaneFromPointNormal(XMVECTOR Point, XMVECTOR Normal);
XMVECTOR        XMPlaneFromPoints(XMVECTOR Point1, XMVECTOR Point2, XMVECTOR Point3);

/****************************************************************************
 *
 * Color operations
 *
 ****************************************************************************/

BOOL            XMColorEqual(XMVECTOR C1, XMVECTOR C2);
BOOL            XMColorNotEqual(XMVECTOR C1, XMVECTOR C2);
BOOL            XMColorGreater(XMVECTOR C1, XMVECTOR C2);
BOOL            XMColorGreaterOrEqual(XMVECTOR C1, XMVECTOR C2);
BOOL            XMColorLess(XMVECTOR C1, XMVECTOR C2);
BOOL            XMColorLessOrEqual(XMVECTOR C1, XMVECTOR C2);

BOOL            XMColorIsNaN(XMVECTOR C);
BOOL            XMColorIsInfinite(XMVECTOR C);

XMVECTOR        XMColorNegative(XMVECTOR C);
XMVECTOR        XMColorModulate(XMVECTOR C1, XMVECTOR C2);
XMVECTOR        XMColorAdjustSaturation(XMVECTOR C, FLOAT Saturation);
XMVECTOR        XMColorAdjustContrast(XMVECTOR C, FLOAT Contrast);

/****************************************************************************
 *
 * Miscellaneous operations
 *
 ****************************************************************************/

VOID            XMAssert(CONST CHAR* pExpression, CONST CHAR* pFileName, UINT LineNumber);

XMVECTOR        XMFresnelTerm(XMVECTOR CosIncidentAngle, XMVECTOR RefractionIndex);

BOOL            XMScalarNearEqual(FLOAT S1, FLOAT S2, FLOAT Epsilon);
FLOAT           XMScalarModAngle(FLOAT Value);
FLOAT           XMScalarSin(FLOAT Value);
FLOAT           XMScalarCos(FLOAT Value);
VOID            XMScalarSinCos(FLOAT* pSin, FLOAT* pCos, FLOAT Value);
FLOAT           XMScalarASin(FLOAT Value);
FLOAT           XMScalarACos(FLOAT Value);
FLOAT           XMScalarSinEst(FLOAT Value);
FLOAT           XMScalarCosEst(FLOAT Value);
VOID            XMScalarSinCosEst(FLOAT* pSin, FLOAT* pCos, FLOAT Value);
FLOAT           XMScalarASinEst(FLOAT Value);
FLOAT           XMScalarACosEst(FLOAT Value);

/****************************************************************************
 *
 * Globals
 *
 ****************************************************************************/

// The purpose of the following global constants is to prevent redundant 
// reloading of the constants when they are referenced by more than one
// separate inline math routine called within the same function.  Declaring
// a constant locally within a routine is sufficient to prevent redundant
// reloads of that constant when that single routine is called multiple
// times in a function, but if the constant is used (and declared) in a 
// separate math routine it would be reloaded.

#define XMGLOBALCONST extern CONST __declspec(selectany)

XMGLOBALCONST XMVECTOR  g_XMSinCoefficients0    = {1.0f, -0.166666667f, 8.333333333e-3f, -1.984126984e-4f};
XMGLOBALCONST XMVECTOR  g_XMSinCoefficients1    = {2.755731922e-6f, -2.505210839e-8f, 1.605904384e-10f, -7.647163732e-13f};
XMGLOBALCONST XMVECTOR  g_XMSinCoefficients2    = {2.811457254e-15f, -8.220635247e-18f, 1.957294106e-20f, -3.868170171e-23f};
XMGLOBALCONST XMVECTOR  g_XMCosCoefficients0    = {1.0f, -0.5f, 4.166666667e-2f, -1.388888889e-3f};
XMGLOBALCONST XMVECTOR  g_XMCosCoefficients1    = {2.480158730e-5f, -2.755731922e-7f, 2.087675699e-9f, -1.147074560e-11f};
XMGLOBALCONST XMVECTOR  g_XMCosCoefficients2    = {4.779477332e-14f, -1.561920697e-16f, 4.110317623e-19f, -8.896791392e-22f};
XMGLOBALCONST XMVECTOR  g_XMTanCoefficients0    = {1.0f, 0.333333333f, 0.133333333f, 5.396825397e-2f};
XMGLOBALCONST XMVECTOR  g_XMTanCoefficients1    = {2.186948854e-2f, 8.863235530e-3f, 3.592128167e-3f, 1.455834485e-3f};
XMGLOBALCONST XMVECTOR  g_XMTanCoefficients2    = {5.900274264e-4f, 2.391290764e-4f, 9.691537707e-5f, 3.927832950e-5f};
XMGLOBALCONST XMVECTOR  g_XMASinCoefficients0   = {-0.05806367563904f, -0.41861972469416f, 0.22480114791621f, 2.17337241360606f};
XMGLOBALCONST XMVECTOR  g_XMASinCoefficients1   = {0.61657275907170f, 4.29696498283455f, -1.18942822255452f, -6.53784832094831f};
XMGLOBALCONST XMVECTOR  g_XMASinCoefficients2   = {-1.36926553863413f, -4.48179294237210f, 1.41810672941833f, 5.48179257935713f};
XMGLOBALCONST XMVECTOR  g_XMATanCoefficients0   = {1.0f, 0.333333334f, 0.2f, 0.142857143f};
XMGLOBALCONST XMVECTOR  g_XMATanCoefficients1   = {1.111111111e-1f, 9.090909091e-2f, 7.692307692e-2f, 6.666666667e-2f};
XMGLOBALCONST XMVECTOR  g_XMATanCoefficients2   = {5.882352941e-2f, 5.263157895e-2f, 4.761904762e-2f, 4.347826087e-2f};
XMGLOBALCONST XMVECTOR  g_XMSinEstCoefficients  = {1.0f, -1.66521856991541e-1f, 8.199913018755e-3f, -1.61475937228e-4f};
XMGLOBALCONST XMVECTOR  g_XMCosEstCoefficients  = {1.0f, -4.95348008918096e-1f, 3.878259962881e-2f, -9.24587976263e-4f};
XMGLOBALCONST XMVECTOR  g_XMTanEstCoefficients  = {2.484f, -1.954923183e-1f, 2.467401101f, XM_1DIVPI};
XMGLOBALCONST XMVECTOR  g_XMATanEstCoefficients = {7.689891418951e-1f, 1.104742493348f, 8.661844266006e-1f, XM_PIDIV2};
XMGLOBALCONST XMVECTOR  g_XMASinEstCoefficients = {-1.36178272886711f, 2.37949493464538f, -8.08228565650486e-1f, 2.78440142746736e-1f};
XMGLOBALCONST XMVECTOR  g_XMASinEstConstants    = {1.00000011921f, XM_PIDIV2, 0.0f, 0.0f};
XMGLOBALCONST XMVECTOR  g_XMPiConstants0        = {XM_PI, XM_2PI, XM_1DIVPI, XM_1DIV2PI};

#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

XMGLOBALCONST XMVECTORI g_XMNegativeZero        = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
XMGLOBALCONST XMVECTOR  g_XMOne                 = {1.0f, 1.0f, 1.0f, 1.0f};
XMGLOBALCONST XMVECTOR  g_XMNegativeOne         = {-1.0f, -1.0f, -1.0f, -1.0f};
XMGLOBALCONST XMVECTOR  g_XMOneHalf             = {0.5f, 0.5f, 0.5f, 0.5f};
XMGLOBALCONST XMVECTOR  g_XMNegativeTwoPi       = {-XM_2PI, -XM_2PI, -XM_2PI, -XM_2PI};
XMGLOBALCONST XMVECTOR  g_XMNegativePi          = {-XM_PI, -XM_PI, -XM_PI, -XM_PI};
XMGLOBALCONST XMVECTOR  g_XMHalfPi              = {XM_PIDIV2, XM_PIDIV2, XM_PIDIV2, XM_PIDIV2};
XMGLOBALCONST XMVECTOR  g_XMPi                  = {XM_PI, XM_PI, XM_PI, XM_PI};
XMGLOBALCONST XMVECTOR  g_XMReciprocalPi        = {XM_1DIVPI, XM_1DIVPI, XM_1DIVPI, XM_1DIVPI};
XMGLOBALCONST XMVECTOR  g_XMTwoPi               = {XM_2PI, XM_2PI, XM_2PI, XM_2PI};
XMGLOBALCONST XMVECTOR  g_XMReciprocalTwoPi     = {XM_1DIV2PI, XM_1DIV2PI, XM_1DIV2PI, XM_1DIV2PI};
XMGLOBALCONST XMVECTOR  g_XMEpsilon             = {1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f};
XMGLOBALCONST XMVECTORI g_XMInfinity            = {0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000};
XMGLOBALCONST XMVECTORI g_XMQNaN                = {0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000};
XMGLOBALCONST XMVECTORI g_XMAbsMask             = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
XMGLOBALCONST XMVECTORI g_XMFltMin              = {0x00800000, 0x00800000, 0x00800000, 0x00800000};
XMGLOBALCONST XMVECTORI g_XMFltMax              = {0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF};
XMGLOBALCONST XMVECTOR  g_XMIdentityR0          = {1.0f, 0.0f, 0.0f, 0.0f};
XMGLOBALCONST XMVECTOR  g_XMIdentityR1          = {0.0f, 1.0f, 0.0f, 0.0f};
XMGLOBALCONST XMVECTOR  g_XMIdentityR2          = {0.0f, 0.0f, 1.0f, 0.0f};
XMGLOBALCONST XMVECTOR  g_XMIdentityR3          = {0.0f, 0.0f, 0.0f, 1.0f};
XMGLOBALCONST XMVECTORI g_XMSelect1000          = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_0, XM_SELECT_0};
XMGLOBALCONST XMVECTORI g_XMSelect1100          = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};
XMGLOBALCONST XMVECTORI g_XMSelect1110          = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_1, XM_SELECT_0};
XMGLOBALCONST XMVECTORI g_XMSelect0101          = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1};
XMGLOBALCONST XMVECTORI g_XMSwizzleXYXY         = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0Y};
XMGLOBALCONST XMVECTORI g_XMSwizzleXYZX         = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X};
XMGLOBALCONST XMVECTORI g_XMSwizzleYXZW         = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_0W};
XMGLOBALCONST XMVECTORI g_XMSwizzleYZXW         = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
XMGLOBALCONST XMVECTORI g_XMSwizzleZXYW         = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
XMGLOBALCONST XMVECTORI g_XMPermute0X0Y1X1Y     = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y};
XMGLOBALCONST XMVECTORI g_XMPermute0Z0W1Z1W     = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_1W};

#endif

/****************************************************************************
 *
 * Implementation
 *
 ****************************************************************************/

#if !defined(__cplusplus) && !defined(_XBOX)
#define sqrtf(x)    ((FLOAT)sqrt((FLOAT)(x)))
#define floorf(x)   ((FLOAT)floor((FLOAT)(x)))
#define ceilf(x)    ((FLOAT)ceil((FLOAT)(x)))
#define powf(x, y)  ((FLOAT)pow((FLOAT)(x), (FLOAT)(y)))
#define logf(x)     ((FLOAT)log((FLOAT)(x)))
#endif /* !defined(__cplusplus) && !defined(_XBOX) */

#include <xmconvert.inl>
#include <xmvector.inl>
#include <xmmatrix.inl>
#include <xmmisc.inl>

#endif // __XBOXMATH_H__
