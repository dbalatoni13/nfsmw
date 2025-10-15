/*++

Copyright (c) Microsoft Corporation

Module Name:

    xmmatrix.inl

Abstract:

    Matrix functions.

--*/

#pragma once

#ifndef __XMMATRIX_INL__
#define __XMMATRIX_INL__

/****************************************************************************
 *
 * Matrix
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparison operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE BOOL XMMatrixIsNaN
(
    XMMATRIX M
)
{
    BOOL Result;

    Result = XMVector4IsNaN(M.r[0]);
    Result = XMVector4IsNaN(M.r[1]) || Result;
    Result = XMVector4IsNaN(M.r[2]) || Result;
    Result = XMVector4IsNaN(M.r[3]) || Result;

    return Result;
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMMatrixIsInfinite
(
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    BOOL Result;

    Result = XMVector4IsInfinite(M.r[0]);
    Result = XMVector4IsInfinite(M.r[1]) || Result;
    Result = XMVector4IsInfinite(M.r[2]) || Result;
    Result = XMVector4IsInfinite(M.r[3]) || Result;

    return Result;

#else

    XMVECTOR OneInt, TrueInt;
    XMVECTOR InfinityShifted;
    XMVECTOR Shift;
    XMVECTOR R0, R1, R2, R3;
    UINT CR0, CR1, CR2, CR3;

    XMDUMMY_INITIALIZE_UINT(CR0);
    XMDUMMY_INITIALIZE_UINT(CR1);
    XMDUMMY_INITIALIZE_UINT(CR2);
    XMDUMMY_INITIALIZE_UINT(CR3);

    TrueInt = __vspltisw(-1);
    Shift = __vspltisw(-8); // Shift 24
    OneInt = __vspltisw(1);

    InfinityShifted = __vslw(TrueInt, Shift);
    R0 = __vslw(M.r[0], OneInt);
    R1 = __vslw(M.r[1], OneInt);
    R2 = __vslw(M.r[2], OneInt);
    R3 = __vslw(M.r[3], OneInt);

    __vcmpequwR(R0, InfinityShifted, &CR0);
    __vcmpequwR(R1, InfinityShifted, &CR1);
    __vcmpequwR(R2, InfinityShifted, &CR2);
    __vcmpequwR(R3, InfinityShifted, &CR3);

    return XMComparisonAnyTrue(CR0) ||
           XMComparisonAnyTrue(CR1) ||
           XMComparisonAnyTrue(CR2) ||
           XMComparisonAnyTrue(CR3);

#endif
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMMatrixIsIdentity
(
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR R2, R1, R0;
    BOOL     Result;

    R2 = XMVectorRotateLeft(g_XMIdentityR3, 1);
    Result = XMVector4Equal(M.r[3], g_XMIdentityR3);
    R1 = XMVectorRotateLeft(R2, 1);
    Result = XMVector4Equal(M.r[2], R2) && Result;
    R0 = XMVectorRotateLeft(R1, 1);
    Result = XMVector4Equal(M.r[1], R1) && Result;
    Result = XMVector4Equal(M.r[0], R0) && Result;

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR R2, R1, R0;
    UINT     CR;
    BOOL     Result;

    XMDUMMY_INITIALIZE_UINT(CR);

    R2 = __vsldoi(g_XMIdentityR3, g_XMIdentityR3, 4);

    __vcmpeqfpR(M.r[3], g_XMIdentityR3, &CR);
    Result = XMComparisonAllTrue(CR);

    R1 = __vsldoi(R2, R2, 4);

    __vcmpeqfpR(M.r[2], R2, &CR);
    Result = XMComparisonAllTrue(CR) && Result;

    R0 = __vsldoi(R1, R1, 4);

    __vcmpeqfpR(M.r[1], R1, &CR);
    Result = XMComparisonAllTrue(CR) && Result;

    __vcmpeqfpR(M.r[0], R0, &CR);
    Result = XMComparisonAllTrue(CR) && Result;

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR ZO;
    XMVECTOR R0, R1, R2, R3;
    UINT CR0, CR1, CR2, CR3;

    XMDUMMY_INITIALIZE_VECTOR(ZO);
    XMDUMMY_INITIALIZE_UINT(CR0);
    XMDUMMY_INITIALIZE_UINT(CR1);
    XMDUMMY_INITIALIZE_UINT(CR2);
    XMDUMMY_INITIALIZE_UINT(CR3);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    R0 = __vpermwi(ZO, 0xEA);
    R1 = __vpermwi(ZO, 0xBA);
    R2 = __vpermwi(ZO, 0xAE);
    R3 = __vpermwi(ZO, 0xAB);

    __vcmpeqfpR(M.r[0], R0, &CR0);
    __vcmpeqfpR(M.r[1], R1, &CR1);
    __vcmpeqfpR(M.r[2], R2, &CR2);
    __vcmpeqfpR(M.r[3], R3, &CR3);

    return XMComparisonAllTrue(CR0) &&
           XMComparisonAllTrue(CR1) &&
           XMComparisonAllTrue(CR2) &&
           XMComparisonAllTrue(CR3);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixMultiply
(
    XMMATRIX M1, 
    XMMATRIX M2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X, Y, Z, W;
    XMMATRIX Result;

    X = XMVectorSplatX(M1.r[0]);
    Y = XMVectorSplatY(M1.r[0]);
    Z = XMVectorSplatZ(M1.r[0]);
    W = XMVectorSplatW(M1.r[0]);

    Result.r[0] = XMVectorMultiply(X, M2.r[0]);
    Result.r[0] = XMVectorMultiplyAdd(Y, M2.r[1], Result.r[0]);
    Result.r[0] = XMVectorMultiplyAdd(Z, M2.r[2], Result.r[0]);
    Result.r[0] = XMVectorMultiplyAdd(W, M2.r[3], Result.r[0]);

    X = XMVectorSplatX(M1.r[1]);
    Y = XMVectorSplatY(M1.r[1]);
    Z = XMVectorSplatZ(M1.r[1]);
    W = XMVectorSplatW(M1.r[1]);

    Result.r[1] = XMVectorMultiply(X, M2.r[0]);
    Result.r[1] = XMVectorMultiplyAdd(Y, M2.r[1], Result.r[1]);
    Result.r[1] = XMVectorMultiplyAdd(Z, M2.r[2], Result.r[1]);
    Result.r[1] = XMVectorMultiplyAdd(W, M2.r[3], Result.r[1]);

    X = XMVectorSplatX(M1.r[2]);
    Y = XMVectorSplatY(M1.r[2]);
    Z = XMVectorSplatZ(M1.r[2]);
    W = XMVectorSplatW(M1.r[2]);

    Result.r[2] = XMVectorMultiply(X, M2.r[0]);
    Result.r[2] = XMVectorMultiplyAdd(Y, M2.r[1], Result.r[2]);
    Result.r[2] = XMVectorMultiplyAdd(Z, M2.r[2], Result.r[2]);
    Result.r[2] = XMVectorMultiplyAdd(W, M2.r[3], Result.r[2]);

    X = XMVectorSplatX(M1.r[3]);
    Y = XMVectorSplatY(M1.r[3]);
    Z = XMVectorSplatZ(M1.r[3]);
    W = XMVectorSplatW(M1.r[3]);

    Result.r[3] = XMVectorMultiply(X, M2.r[0]);
    Result.r[3] = XMVectorMultiplyAdd(Y, M2.r[1], Result.r[3]);
    Result.r[3] = XMVectorMultiplyAdd(Z, M2.r[2], Result.r[3]);
    Result.r[3] = XMVectorMultiplyAdd(W, M2.r[3], Result.r[3]);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR M1S0, M1S1, M1S2, M1S3;
    XMVECTOR R0, R1, R2, R3;
    XMMATRIX Result;

    M1S0 = __vspltw(M1.r[0], 0);
    M1S1 = __vspltw(M1.r[1], 0);
    M1S2 = __vspltw(M1.r[2], 0);
    M1S3 = __vspltw(M1.r[3], 0);

    R0 = __vmaddfp(M1S0, M2.r[0], *(XMVECTOR*)g_XMNegativeZero);
    R1 = __vmaddfp(M1S1, M2.r[0], *(XMVECTOR*)g_XMNegativeZero);
    R2 = __vmaddfp(M1S2, M2.r[0], *(XMVECTOR*)g_XMNegativeZero);
    R3 = __vmaddfp(M1S3, M2.r[0], *(XMVECTOR*)g_XMNegativeZero);

    M1S0 = __vspltw(M1.r[0], 1);
    M1S1 = __vspltw(M1.r[1], 1);
    M1S2 = __vspltw(M1.r[2], 1);
    M1S3 = __vspltw(M1.r[3], 1);

    R0 = __vmaddfp(M1S0, M2.r[1], R0);
    R1 = __vmaddfp(M1S1, M2.r[1], R1);
    R2 = __vmaddfp(M1S2, M2.r[1], R2);
    R3 = __vmaddfp(M1S3, M2.r[1], R3);

    M1S0 = __vspltw(M1.r[0], 2);
    M1S1 = __vspltw(M1.r[1], 2);
    M1S2 = __vspltw(M1.r[2], 2);
    M1S3 = __vspltw(M1.r[3], 2);

    R0 = __vmaddfp(M1S0, M2.r[2], R0);
    R1 = __vmaddfp(M1S1, M2.r[2], R1);
    R2 = __vmaddfp(M1S2, M2.r[2], R2);
    R3 = __vmaddfp(M1S3, M2.r[2], R3);

    M1S0 = __vspltw(M1.r[0], 3);
    M1S1 = __vspltw(M1.r[1], 3);
    M1S2 = __vspltw(M1.r[2], 3);
    M1S3 = __vspltw(M1.r[3], 3);

    Result.r[0] = __vmaddfp(M1S0, M2.r[3], R0);
    Result.r[1] = __vmaddfp(M1S1, M2.r[3], R1);
    Result.r[2] = __vmaddfp(M1S2, M2.r[3], R2);
    Result.r[3] = __vmaddfp(M1S3, M2.r[3], R3);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR M00, M01, M02, M03,
             M10, M11, M12, M13,
             M20, M21, M22, M23,
             M30, M31, M32, M33;
    XMVECTOR M00M02, M01M03, M10M12, M11M13,
             M20M22, M21M23, M30M32, M31M33;
    XMMATRIX M2T, P;
    XMMATRIX Result;

    P.r[0] = __vmrghw(M2.r[0], M2.r[2]);
    P.r[1] = __vmrghw(M2.r[1], M2.r[3]);
    P.r[2] = __vmrglw(M2.r[0], M2.r[2]);
    P.r[3] = __vmrglw(M2.r[1], M2.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00 = __vmsum4fp(M1.r[0], M2T.r[0]);
    M02 = __vmsum4fp(M1.r[0], M2T.r[2]);
    M01 = __vmsum4fp(M1.r[0], M2T.r[1]);
    M03 = __vmsum4fp(M1.r[0], M2T.r[3]);

    M10 = __vmsum4fp(M1.r[1], M2T.r[0]);
    M12 = __vmsum4fp(M1.r[1], M2T.r[2]);
    M11 = __vmsum4fp(M1.r[1], M2T.r[1]);
    M13 = __vmsum4fp(M1.r[1], M2T.r[3]);

    M20 = __vmsum4fp(M1.r[2], M2T.r[0]);
    M22 = __vmsum4fp(M1.r[2], M2T.r[2]);
    M21 = __vmsum4fp(M1.r[2], M2T.r[1]);
    M23 = __vmsum4fp(M1.r[2], M2T.r[3]);

    M30 = __vmsum4fp(M1.r[3], M2T.r[0]);
    M32 = __vmsum4fp(M1.r[3], M2T.r[2]);
    M31 = __vmsum4fp(M1.r[3], M2T.r[1]);
    M33 = __vmsum4fp(M1.r[3], M2T.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Result.r[0] = __vmrghw(M00M02, M01M03);
    Result.r[1] = __vmrghw(M10M12, M11M13);
    Result.r[2] = __vmrghw(M20M22, M21M23);
    Result.r[3] = __vmrghw(M30M32, M31M33);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixMultiplyTranspose
(
    XMMATRIX M1, 
    XMMATRIX M2
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX Product;
    XMMATRIX Result;

    Product = XMMatrixMultiply(M1, M2);
    Result = XMMatrixTranspose(Product);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                R0, R1, R2, R3;
    XMVECTOR                M1S0, M1S1, M1S2, M1S3;
    XMVECTOR                P0, P1, P2, P3;
    XMMATRIX                MT;
    XMVECTOR                MR0, MR1, MR2, MR3;
    static CONST XMVECTORI  Control0 = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1W, XM_PERMUTE_0W};
    static CONST XMVECTORI  Control1 = {XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_1Z};
    static CONST XMVECTORI  Control2 = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1X};
    static CONST XMVECTORI  Control3 = {XM_PERMUTE_1W, XM_PERMUTE_1Z, XM_PERMUTE_0Z, XM_PERMUTE_0W};

    M1S0 = __vspltw(M1.r[0], 0);
    M1S1 = __vspltw(M1.r[1], 0);
    M1S2 = __vspltw(M1.r[2], 0);
    M1S3 = __vspltw(M1.r[3], 0);

    MR0 = __vmaddfp(M1S0, M2.r[0], *(XMVECTOR*)g_XMNegativeZero);
    MR1 = __vmaddfp(M1S1, M2.r[0], *(XMVECTOR*)g_XMNegativeZero);
    MR2 = __vmaddfp(M1S2, M2.r[0], *(XMVECTOR*)g_XMNegativeZero);
    MR3 = __vmaddfp(M1S3, M2.r[0], *(XMVECTOR*)g_XMNegativeZero);

    M1S0 = __vspltw(M1.r[0], 1);
    M1S1 = __vspltw(M1.r[1], 1);
    M1S2 = __vspltw(M1.r[2], 1);
    M1S3 = __vspltw(M1.r[3], 1);

    MR0 = __vmaddfp(M1S0, M2.r[1], MR0);
    MR1 = __vmaddfp(M1S1, M2.r[1], MR1);
    MR2 = __vmaddfp(M1S2, M2.r[1], MR2);
    MR3 = __vmaddfp(M1S3, M2.r[1], MR3);

    M1S0 = __vspltw(M1.r[0], 2);
    M1S1 = __vspltw(M1.r[1], 2);
    M1S2 = __vspltw(M1.r[2], 2);
    M1S3 = __vspltw(M1.r[3], 2);

    MR0 = __vmaddfp(M1S0, M2.r[2], MR0);
    MR1 = __vmaddfp(M1S1, M2.r[2], MR1);
    MR2 = __vmaddfp(M1S2, M2.r[2], MR2);
    MR3 = __vmaddfp(M1S3, M2.r[2], MR3);

    M1S0 = __vspltw(M1.r[0], 3);
    M1S1 = __vspltw(M1.r[1], 3);
    M1S2 = __vspltw(M1.r[2], 3);
    M1S3 = __vspltw(M1.r[3], 3);

    R0 = __vmaddfp(M1S0, M2.r[3], MR0);
    R1 = __vmaddfp(M1S1, M2.r[3], MR1);
    R2 = __vmaddfp(M1S2, M2.r[3], MR2);
    R3 = __vmaddfp(M1S3, M2.r[3], MR3);

    P2 = __vmrghw(R0, R1);
    P3 = __vmrghw(R2, R3);
    P0 = __vmrglw(R0, R1);
    P1 = __vmrglw(R2, R3);

    MT.r[0] = __vperm(P2, P3, *(XMVECTOR*)g_XMPermute0X0Y1X1Y);
    MT.r[1] = __vperm(P2, P3, *(XMVECTOR*)g_XMPermute0Z0W1Z1W);
    MT.r[2] = __vperm(P0, P1, *(XMVECTOR*)g_XMPermute0X0Y1X1Y);
    MT.r[3] = __vperm(P0, P1, *(XMVECTOR*)g_XMPermute0Z0W1Z1W);

    return MT;

#else // _VMX128_INTRINSICS_

    XMVECTOR M00, M01, M02, M03,
             M10, M11, M12, M13,
             M20, M21, M22, M23,
             M30, M31, M32, M33;
    XMVECTOR M00M20, M10M30, M01M21, M11M31,
             M02M22, M12M32, M03M23, M13M33;
    XMMATRIX M2T, P;
    XMMATRIX Result;

    P.r[0] = __vmrghw(M2.r[0], M2.r[2]);
    P.r[1] = __vmrghw(M2.r[1], M2.r[3]);
    P.r[2] = __vmrglw(M2.r[0], M2.r[2]);
    P.r[3] = __vmrglw(M2.r[1], M2.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00 = __vmsum4fp(M1.r[0], M2T.r[0]);
    M02 = __vmsum4fp(M1.r[0], M2T.r[2]);
    M01 = __vmsum4fp(M1.r[0], M2T.r[1]);
    M03 = __vmsum4fp(M1.r[0], M2T.r[3]);

    M10 = __vmsum4fp(M1.r[1], M2T.r[0]);
    M12 = __vmsum4fp(M1.r[1], M2T.r[2]);
    M11 = __vmsum4fp(M1.r[1], M2T.r[1]);
    M13 = __vmsum4fp(M1.r[1], M2T.r[3]);

    M20 = __vmsum4fp(M1.r[2], M2T.r[0]);
    M22 = __vmsum4fp(M1.r[2], M2T.r[2]);
    M21 = __vmsum4fp(M1.r[2], M2T.r[1]);
    M23 = __vmsum4fp(M1.r[2], M2T.r[3]);

    M30 = __vmsum4fp(M1.r[3], M2T.r[0]);
    M32 = __vmsum4fp(M1.r[3], M2T.r[2]);
    M31 = __vmsum4fp(M1.r[3], M2T.r[1]);
    M33 = __vmsum4fp(M1.r[3], M2T.r[3]);

    M00M20 = __vmrghw(M00, M20);
    M10M30 = __vmrghw(M10, M30);
    M01M21 = __vmrghw(M01, M21);
    M11M31 = __vmrghw(M11, M31);
    M02M22 = __vmrghw(M02, M22);
    M12M32 = __vmrghw(M12, M32);
    M03M23 = __vmrghw(M03, M23);
    M13M33 = __vmrghw(M13, M33);

    Result.r[0] = __vmrghw(M00M20, M10M30);
    Result.r[1] = __vmrghw(M01M21, M11M31);
    Result.r[2] = __vmrghw(M02M22, M12M32);
    Result.r[3] = __vmrghw(M03M23, M13M33);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixTranspose
(
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX P;
    XMMATRIX MT;

    // Original matrix:
    //
    //     m00m01m02m03
    //     m10m11m12m13
    //     m20m21m22m23
    //     m30m31m32m33

    P.r[0] = XMVectorMergeXY(M.r[0], M.r[2]); // m00m20m01m21
    P.r[1] = XMVectorMergeXY(M.r[1], M.r[3]); // m10m30m11m31
    P.r[2] = XMVectorMergeZW(M.r[0], M.r[2]); // m02m22m03m23
    P.r[3] = XMVectorMergeZW(M.r[1], M.r[3]); // m12m32m13m33

    MT.r[0] = XMVectorMergeXY(P.r[0], P.r[1]); // m00m10m20m30
    MT.r[1] = XMVectorMergeZW(P.r[0], P.r[1]); // m01m11m21m31
    MT.r[2] = XMVectorMergeXY(P.r[2], P.r[3]); // m02m12m22m32
    MT.r[3] = XMVectorMergeZW(P.r[2], P.r[3]); // m03m13m23m33

    return MT;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMMATRIX P;
    XMMATRIX MT;

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    return MT;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixInverse
(
    XMVECTOR* pDeterminant, 
    XMMATRIX  M
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX               R;
    XMMATRIX               MT;
    XMVECTOR               D0, D1, D2;
    XMVECTOR               C0, C1, C2, C3, C4, C5, C6, C7;
    XMVECTOR               V0[4], V1[4];
    XMVECTOR               Determinant;
    XMVECTOR               Reciprocal;
    XMMATRIX               Result;
    static CONST XMVECTORI SwizzleXXYY = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
    static CONST XMVECTORI SwizzleZWZW = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleYZXY = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y};
    static CONST XMVECTORI SwizzleZWYZ = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI SwizzleWXWX = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static CONST XMVECTORI SwizzleZXYX = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0X};
    static CONST XMVECTORI SwizzleYWXZ = {XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Z};
    static CONST XMVECTORI SwizzleWZWY = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y};
    static CONST XMVECTORI Permute0X0Z1X1Z = {XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Z};
    static CONST XMVECTORI Permute0Y0W1Y1W = {XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_1W};
    static CONST XMVECTORI Permute1Y0Y0W0X = {XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0W0X0Y1X = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute0Z1Y1X0Z = {XM_PERMUTE_0Z, XM_PERMUTE_1Y, XM_PERMUTE_1X, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0W1Y0Y0Z = {XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0Z0Y1X0X = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute1Y0X0W1X = {XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute1W0Y0W0X = {XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0W0X0Y1Z = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1Z};
    static CONST XMVECTORI Permute0Z1W1Z0Z = {XM_PERMUTE_0Z, XM_PERMUTE_1W, XM_PERMUTE_1Z, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0W1W0Y0Z = {XM_PERMUTE_0W, XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0Z0Y1Z0X = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1Z, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute1W0X0W1Z = {XM_PERMUTE_1W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z};

    XMASSERT(pDeterminant);

    MT = XMMatrixTranspose(M);

    V0[0] = XMVectorPermute(MT.r[2], MT.r[2], *(XMVECTOR*)SwizzleXXYY);
    V1[0] = XMVectorPermute(MT.r[3], MT.r[3], *(XMVECTOR*)SwizzleZWZW);
    V0[1] = XMVectorPermute(MT.r[0], MT.r[0], *(XMVECTOR*)SwizzleXXYY);
    V1[1] = XMVectorPermute(MT.r[1], MT.r[1], *(XMVECTOR*)SwizzleZWZW);
    V0[2] = XMVectorPermute(MT.r[2], MT.r[0], *(XMVECTOR*)Permute0X0Z1X1Z);
    V1[2] = XMVectorPermute(MT.r[3], MT.r[1], *(XMVECTOR*)Permute0Y0W1Y1W);

    D0 = XMVectorMultiply(V0[0], V1[0]);
    D1 = XMVectorMultiply(V0[1], V1[1]);
    D2 = XMVectorMultiply(V0[2], V1[2]);

    V0[0] = XMVectorPermute(MT.r[2], MT.r[2], *(XMVECTOR*)SwizzleZWZW);
    V1[0] = XMVectorPermute(MT.r[3], MT.r[3], *(XMVECTOR*)SwizzleXXYY);
    V0[1] = XMVectorPermute(MT.r[0], MT.r[0], *(XMVECTOR*)SwizzleZWZW);
    V1[1] = XMVectorPermute(MT.r[1], MT.r[1], *(XMVECTOR*)SwizzleXXYY);
    V0[2] = XMVectorPermute(MT.r[2], MT.r[0], *(XMVECTOR*)Permute0Y0W1Y1W);
    V1[2] = XMVectorPermute(MT.r[3], MT.r[1], *(XMVECTOR*)Permute0X0Z1X1Z);

    D0 = XMVectorNegativeMultiplySubtract(V0[0], V1[0], D0);
    D1 = XMVectorNegativeMultiplySubtract(V0[1], V1[1], D1);
    D2 = XMVectorNegativeMultiplySubtract(V0[2], V1[2], D2);

    V0[0] = XMVectorPermute(MT.r[1], MT.r[1], *(XMVECTOR*)SwizzleYZXY);
    V1[0] = XMVectorPermute(D0, D2, *(XMVECTOR*)Permute1Y0Y0W0X);
    V0[1] = XMVectorPermute(MT.r[0], MT.r[0], *(XMVECTOR*)SwizzleZXYX);
    V1[1] = XMVectorPermute(D0, D2, *(XMVECTOR*)Permute0W1Y0Y0Z);
    V0[2] = XMVectorPermute(MT.r[3], MT.r[3], *(XMVECTOR*)SwizzleYZXY);
    V1[2] = XMVectorPermute(D1, D2, *(XMVECTOR*)Permute1W0Y0W0X);
    V0[3] = XMVectorPermute(MT.r[2], MT.r[2], *(XMVECTOR*)SwizzleZXYX);
    V1[3] = XMVectorPermute(D1, D2, *(XMVECTOR*)Permute0W1W0Y0Z);

    C0 = XMVectorMultiply(V0[0], V1[0]);
    C2 = XMVectorMultiply(V0[1], V1[1]);
    C4 = XMVectorMultiply(V0[2], V1[2]);
    C6 = XMVectorMultiply(V0[3], V1[3]);

    V0[0] = XMVectorPermute(MT.r[1], MT.r[1], *(XMVECTOR*)SwizzleZWYZ);
    V1[0] = XMVectorPermute(D0, D2, *(XMVECTOR*)Permute0W0X0Y1X);
    V0[1] = XMVectorPermute(MT.r[0], MT.r[0], *(XMVECTOR*)SwizzleWZWY);
    V1[1] = XMVectorPermute(D0, D2, *(XMVECTOR*)Permute0Z0Y1X0X);
    V0[2] = XMVectorPermute(MT.r[3], MT.r[3], *(XMVECTOR*)SwizzleZWYZ);
    V1[2] = XMVectorPermute(D1, D2, *(XMVECTOR*)Permute0W0X0Y1Z);
    V0[3] = XMVectorPermute(MT.r[2], MT.r[2], *(XMVECTOR*)SwizzleWZWY);
    V1[3] = XMVectorPermute(D1, D2, *(XMVECTOR*)Permute0Z0Y1Z0X);

    C0 = XMVectorNegativeMultiplySubtract(V0[0], V1[0], C0);
    C2 = XMVectorNegativeMultiplySubtract(V0[1], V1[1], C2);
    C4 = XMVectorNegativeMultiplySubtract(V0[2], V1[2], C4);
    C6 = XMVectorNegativeMultiplySubtract(V0[3], V1[3], C6);

    V0[0] = XMVectorPermute(MT.r[1], MT.r[1], *(XMVECTOR*)SwizzleWXWX);
    V1[0] = XMVectorPermute(D0, D2, *(XMVECTOR*)Permute0Z1Y1X0Z);
    V0[1] = XMVectorPermute(MT.r[0], MT.r[0], *(XMVECTOR*)SwizzleYWXZ);
    V1[1] = XMVectorPermute(D0, D2, *(XMVECTOR*)Permute1Y0X0W1X);
    V0[2] = XMVectorPermute(MT.r[3], MT.r[3], *(XMVECTOR*)SwizzleWXWX);
    V1[2] = XMVectorPermute(D1, D2, *(XMVECTOR*)Permute0Z1W1Z0Z);
    V0[3] = XMVectorPermute(MT.r[2], MT.r[2], *(XMVECTOR*)SwizzleYWXZ);
    V1[3] = XMVectorPermute(D1, D2, *(XMVECTOR*)Permute1W0X0W1Z);

    C1 = XMVectorNegativeMultiplySubtract(V0[0], V1[0], C0);
    C0 = XMVectorMultiplyAdd(V0[0], V1[0], C0);
    C3 = XMVectorMultiplyAdd(V0[1], V1[1], C2);
    C2 = XMVectorNegativeMultiplySubtract(V0[1], V1[1], C2);
    C5 = XMVectorNegativeMultiplySubtract(V0[2], V1[2], C4);
    C4 = XMVectorMultiplyAdd(V0[2], V1[2], C4);
    C7 = XMVectorMultiplyAdd(V0[3], V1[3], C6);
    C6 = XMVectorNegativeMultiplySubtract(V0[3], V1[3], C6);

    R.r[0] = XMVectorSelect(C0, C1, *(XMVECTOR*)g_XMSelect0101);
    R.r[1] = XMVectorSelect(C2, C3, *(XMVECTOR*)g_XMSelect0101);
    R.r[2] = XMVectorSelect(C4, C5, *(XMVECTOR*)g_XMSelect0101);
    R.r[3] = XMVectorSelect(C6, C7, *(XMVECTOR*)g_XMSelect0101);

    Determinant = XMVector4Dot(R.r[0], MT.r[0]);

    *pDeterminant = Determinant;

    Reciprocal = XMVectorReciprocal(Determinant);

    Result.r[0] = XMVectorMultiply(R.r[0], Reciprocal);
    Result.r[1] = XMVectorMultiply(R.r[1], Reciprocal);
    Result.r[2] = XMVectorMultiply(R.r[2], Reciprocal);
    Result.r[3] = XMVectorMultiply(R.r[3], Reciprocal);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               R0, R1, R2, R3;
    XMVECTOR               MT0, MT1, MT2, MT3;
    XMVECTOR               P0, P1, P2, P3;
    XMVECTOR               D0, D1, D2;
    XMVECTOR               C0, C1, C2, C3, C4, C5, C6, C7;
    XMVECTOR               V0[4], V1[4];
    XMVECTOR               Determinant;
    XMVECTOR               Reciprocal, Rcp, Scale, Refine;
    XMMATRIX               Result;
    static CONST XMVECTORI SwizzleXXYY = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
    static CONST XMVECTORI SwizzleZWZW = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleYZXY = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y};
    static CONST XMVECTORI SwizzleZWYZ = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI SwizzleWXWX = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static CONST XMVECTORI SwizzleZXYX = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0X};
    static CONST XMVECTORI SwizzleYWXZ = {XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Z};
    static CONST XMVECTORI SwizzleWZWY = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y};
    static CONST XMVECTORI Permute0X0Z1X1Z = {XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Z};
    static CONST XMVECTORI Permute0Y0W1Y1W = {XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_1W};
    static CONST XMVECTORI Permute1Y0Y0W0X = {XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0W0X0Y1X = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute0Z1Y1X0Z = {XM_PERMUTE_0Z, XM_PERMUTE_1Y, XM_PERMUTE_1X, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0W1Y0Y0Z = {XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0Z0Y1X0X = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute1Y0X0W1X = {XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute1W0Y0W0X = {XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0W0X0Y1Z = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1Z};
    static CONST XMVECTORI Permute0Z1W1Z0Z = {XM_PERMUTE_0Z, XM_PERMUTE_1W, XM_PERMUTE_1Z, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0W1W0Y0Z = {XM_PERMUTE_0W, XM_PERMUTE_1W, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0Z0Y1Z0X = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1Z, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute1W0X0W1Z = {XM_PERMUTE_1W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z};

    XMASSERT(pDeterminant);

    P2 = __vmrghw(M.r[0], M.r[1]);
    P3 = __vmrghw(M.r[2], M.r[3]);
    P0 = __vmrglw(M.r[0], M.r[1]);
    P1 = __vmrglw(M.r[2], M.r[3]);

    MT0 = __vperm(P2, P3, *(XMVECTOR*)g_XMPermute0X0Y1X1Y);
    MT1 = __vperm(P2, P3, *(XMVECTOR*)g_XMPermute0Z0W1Z1W);
    MT2 = __vperm(P0, P1, *(XMVECTOR*)g_XMPermute0X0Y1X1Y);
    MT3 = __vperm(P0, P1, *(XMVECTOR*)g_XMPermute0Z0W1Z1W);

    V0[0] = __vperm(MT2, MT2, *(XMVECTOR*)SwizzleXXYY);
    V1[0] = __vperm(MT3, MT3, *(XMVECTOR*)SwizzleZWZW);
    V0[1] = __vperm(MT0, MT0, *(XMVECTOR*)SwizzleXXYY);
    V1[1] = __vperm(MT1, MT1, *(XMVECTOR*)SwizzleZWZW);
    V0[2] = __vperm(MT2, MT0, *(XMVECTOR*)Permute0X0Z1X1Z);
    V1[2] = __vperm(MT3, MT1, *(XMVECTOR*)Permute0Y0W1Y1W);

    D0 = __vmaddfp(V0[0], V1[0], *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vmaddfp(V0[1], V1[1], *(XMVECTOR*)g_XMNegativeZero);
    D2 = __vmaddfp(V0[2], V1[2], *(XMVECTOR*)g_XMNegativeZero);

    V0[0] = __vperm(MT2, MT2, *(XMVECTOR*)SwizzleZWZW);
    V1[0] = __vperm(MT3, MT3, *(XMVECTOR*)SwizzleXXYY);
    V0[1] = __vperm(MT0, MT0, *(XMVECTOR*)SwizzleZWZW);
    V1[1] = __vperm(MT1, MT1, *(XMVECTOR*)SwizzleXXYY);
    V0[2] = __vperm(MT2, MT0, *(XMVECTOR*)Permute0Y0W1Y1W);
    V1[2] = __vperm(MT3, MT1, *(XMVECTOR*)Permute0X0Z1X1Z);

    D0 = __vnmsubfp(V0[0], V1[0], D0);
    D1 = __vnmsubfp(V0[1], V1[1], D1);
    D2 = __vnmsubfp(V0[2], V1[2], D2);

    V0[0] = __vperm(MT1, MT1, *(XMVECTOR*)SwizzleYZXY);
    V1[0] = __vperm(D0, D2, *(XMVECTOR*)Permute1Y0Y0W0X);
    V0[1] = __vperm(MT0, MT0, *(XMVECTOR*)SwizzleZXYX);
    V1[1] = __vperm(D0, D2, *(XMVECTOR*)Permute0W1Y0Y0Z);
    V0[2] = __vperm(MT3, MT3, *(XMVECTOR*)SwizzleYZXY);
    V1[2] = __vperm(D1, D2, *(XMVECTOR*)Permute1W0Y0W0X);
    V0[3] = __vperm(MT2, MT2, *(XMVECTOR*)SwizzleZXYX);
    V1[3] = __vperm(D1, D2, *(XMVECTOR*)Permute0W1W0Y0Z);

    C0 = __vmaddfp(V0[0], V1[0], *(XMVECTOR*)g_XMNegativeZero);
    C2 = __vmaddfp(V0[1], V1[1], *(XMVECTOR*)g_XMNegativeZero);
    C4 = __vmaddfp(V0[2], V1[2], *(XMVECTOR*)g_XMNegativeZero);
    C6 = __vmaddfp(V0[3], V1[3], *(XMVECTOR*)g_XMNegativeZero);

    V0[0] = __vperm(MT1, MT1, *(XMVECTOR*)SwizzleZWYZ);
    V1[0] = __vperm(D0, D2, *(XMVECTOR*)Permute0W0X0Y1X);
    V0[1] = __vperm(MT0, MT0, *(XMVECTOR*)SwizzleWZWY);
    V1[1] = __vperm(D0, D2, *(XMVECTOR*)Permute0Z0Y1X0X);
    V0[2] = __vperm(MT3, MT3, *(XMVECTOR*)SwizzleZWYZ);
    V1[2] = __vperm(D1, D2, *(XMVECTOR*)Permute0W0X0Y1Z);
    V0[3] = __vperm(MT2, MT2, *(XMVECTOR*)SwizzleWZWY);
    V1[3] = __vperm(D1, D2, *(XMVECTOR*)Permute0Z0Y1Z0X);

    C0 = __vnmsubfp(V0[0], V1[0], C0);
    C2 = __vnmsubfp(V0[1], V1[1], C2);
    C4 = __vnmsubfp(V0[2], V1[2], C4);
    C6 = __vnmsubfp(V0[3], V1[3], C6);

    V0[0] = __vperm(MT1, MT1, *(XMVECTOR*)SwizzleWXWX);
    V1[0] = __vperm(D0, D2, *(XMVECTOR*)Permute0Z1Y1X0Z);
    V0[1] = __vperm(MT0, MT0, *(XMVECTOR*)SwizzleYWXZ);
    V1[1] = __vperm(D0, D2, *(XMVECTOR*)Permute1Y0X0W1X);
    V0[2] = __vperm(MT3, MT3, *(XMVECTOR*)SwizzleWXWX);
    V1[2] = __vperm(D1, D2, *(XMVECTOR*)Permute0Z1W1Z0Z);
    V0[3] = __vperm(MT2, MT2, *(XMVECTOR*)SwizzleYWXZ);
    V1[3] = __vperm(D1, D2, *(XMVECTOR*)Permute1W0X0W1Z);

    C1 = __vnmsubfp(V0[0], V1[0], C0);
    C0 = __vmaddfp(V0[0], V1[0], C0);
    C3 = __vmaddfp(V0[1], V1[1], C2);
    C2 = __vnmsubfp(V0[1], V1[1], C2);
    C5 = __vnmsubfp(V0[2], V1[2], C4);
    C4 = __vmaddfp(V0[2], V1[2], C4);
    C7 = __vmaddfp(V0[3], V1[3], C6);
    C6 = __vnmsubfp(V0[3], V1[3], C6);

    R0 = __vsel(C0, C1, *(XMVECTOR*)g_XMSelect0101);
    R1 = __vsel(C2, C3, *(XMVECTOR*)g_XMSelect0101);
    R2 = __vsel(C4, C5, *(XMVECTOR*)g_XMSelect0101);
    R3 = __vsel(C6, C7, *(XMVECTOR*)g_XMSelect0101);

    D0 = __vmaddfp(R0, MT0, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Determinant = __vaddfp(D0, D1);

    Reciprocal = __vrefp(Determinant);

    *pDeterminant = Determinant;

    Scale = __vnmsubfp(Determinant, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);
    Scale = __vnmsubfp(Determinant, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Reciprocal = __vsel(Reciprocal, Rcp, Refine);

    Result.r[0] = __vmaddfp(R0, Reciprocal, *(XMVECTOR*)g_XMNegativeZero);
    Result.r[1] = __vmaddfp(R1, Reciprocal, *(XMVECTOR*)g_XMNegativeZero);
    Result.r[2] = __vmaddfp(R2, Reciprocal, *(XMVECTOR*)g_XMNegativeZero);
    Result.r[3] = __vmaddfp(R3, Reciprocal, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMMATRIX R;
    XMMATRIX P, MT;
    XMVECTOR PR1, PR2;
    XMVECTOR D0, D1, D2;
    XMVECTOR S0, S1, Refine;
    XMVECTOR V0[4], V1[4];
    XMVECTOR Determinant;
    XMVECTOR Reciprocal;
    XMVECTOR Zero, SignMask, Sign0, Sign1;
    XMMATRIX Result;

    XMASSERT(pDeterminant);

    XMDUMMY_INITIALIZE_VECTOR(S1);

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    PR2 = P.r[2];
    PR1 = P.r[1];

    V0[0] = __vpermwi(MT.r[2], 0x5);
    V1[0] = __vpermwi(MT.r[3], 0xBB);
    V0[1] = __vpermwi(MT.r[0], 0x5);
    V1[1] = __vpermwi(MT.r[1], 0xBB);
    PR2 = __vrlimi(PR2, P.r[0], 0x3, 2);
    PR1 = __vrlimi(PR1, P.r[3], 0xC, 2);

    D0 = __vmulfp(V0[0], V1[0]);
    D1 = __vmulfp(V0[1], V1[1]);
    D2 = __vmulfp(PR2, PR1);

    V0[0] = __vpermwi(MT.r[2], 0xBB);
    V1[0] = __vpermwi(MT.r[3], 0x5);
    V0[1] = __vpermwi(MT.r[0], 0xBB);
    V1[1] = __vpermwi(MT.r[1], 0x5);
    P.r[3] = __vrlimi(P.r[3], P.r[1], 0x3, 2);
    P.r[0] = __vrlimi(P.r[0], P.r[2], 0xC, 2);

    D0 = __vnmsubfp(V0[0], V1[0], D0);
    D1 = __vnmsubfp(V0[1], V1[1], D1);
    D2 = __vnmsubfp(P.r[3], P.r[0], D2);

    SignMask = __vspltisw(-1);
    Zero = __vspltisw(0);
    SignMask = __vslw(SignMask, SignMask);
    Sign0 = __vmrghw(Zero, SignMask);
    Sign1 = __vmrghw(SignMask, Zero);

    V1[0] = __vpermwi(D0, 0x9C);
    V1[1] = __vpermwi(D0, 0xC6);
    V1[2] = __vpermwi(D1, 0x9C);
    V1[3] = __vpermwi(D1, 0xC6);

    V0[0] = __vpermwi(MT.r[1], 0x61);
    V1[0] = __vrlimi(V1[0], D2, 0x8, 1);
    V0[1] = __vpermwi(MT.r[0], 0x84);
    V1[1] = __vrlimi(V1[1], D2, 0x4, 0);
    V0[2] = __vpermwi(MT.r[3], 0x61);
    V1[2] = __vrlimi(V1[2], D2, 0x8, 3);
    V0[3] = __vpermwi(MT.r[2], 0x84);
    V1[3] = __vrlimi(V1[3], D2, 0x4, 2);

    R.r[0] = __vmulfp(V0[0], V1[0]);
    R.r[1] = __vmulfp(V0[1], V1[1]);
    R.r[2] = __vmulfp(V0[2], V1[2]);
    R.r[3] = __vmulfp(V0[3], V1[3]);

    V1[0] = __vpermwi(D0, 0xC6);
    V1[1] = __vpermwi(D0, 0x9C);
    V1[2] = __vpermwi(D1, 0xC6);
    V1[3] = __vpermwi(D1, 0x9C);

    V0[0] = __vpermwi(MT.r[1], 0xB6);
    V1[0] = __vrlimi(V1[0], D2, 0x1, 1);
    V0[1] = __vpermwi(MT.r[0], 0xED);
    V1[1] = __vrlimi(V1[1], D2, 0x2, 2);
    V0[2] = __vpermwi(MT.r[3], 0xB6);
    V1[2] = __vrlimi(V1[2], D2, 0x1, 3);
    V0[3] = __vpermwi(MT.r[2], 0xED);
    V1[3] = __vrlimi(V1[3], D2, 0x2, 0);

    R.r[0] = __vnmsubfp(V0[0], V1[0], R.r[0]);
    R.r[1] = __vnmsubfp(V0[1], V1[1], R.r[1]);
    R.r[2] = __vnmsubfp(V0[2], V1[2], R.r[2]);
    R.r[3] = __vnmsubfp(V0[3], V1[3], R.r[3]);

    V1[0] = __vsldoi(D0, D2, 2 << 2);
    V1[1] = __vpermwi(D0, 0x4E);
    V1[2] = __vmrglw(D1, D2);
    V1[3] = __vpermwi(D1, 0x4E);

    V0[0] = __vpermwi(MT.r[1], 0xCC);
    V1[0] = __vpermwi(V1[0], 0x38);
    V0[1] = __vpermwi(MT.r[0], 0x72);
    V1[1] = __vrlimi(V1[1], D2, 0x9, 1);
    V0[2] = __vpermwi(MT.r[3], 0xCC);
    V1[2] = __vpermwi(V1[2], 0x34);
    V0[3] = __vpermwi(MT.r[2], 0x72);
    V1[3] = __vrlimi(V1[3], D2, 0x9, 3);

    V0[0] = __vxor(V0[0], Sign0);
    V0[1] = __vxor(V0[1], Sign1);
    V0[2] = __vxor(V0[2], Sign0);
    V0[3] = __vxor(V0[3], Sign1);

    R.r[0] = __vmaddfp(V0[0], V1[0], R.r[0]);
    R.r[1] = __vmaddfp(V0[1], V1[1], R.r[1]);
    R.r[2] = __vmaddfp(V0[2], V1[2], R.r[2]);
    R.r[3] = __vmaddfp(V0[3], V1[3], R.r[3]);

    Determinant = __vmsum4fp(R.r[0], MT.r[0]);

    *pDeterminant = Determinant;

    Reciprocal = __vrefp(Determinant);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(Determinant, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(Determinant, S0, S1);
    Refine = __vcmpeqfp(S0, S0);
    S0 = __vmaddfp(S0, S1, S0);
    Refine = __vsel(Reciprocal, S0, Refine);

    Result.r[0] = __vmulfp(R.r[0], Refine);
    Result.r[1] = __vmulfp(R.r[1], Refine);
    Result.r[2] = __vmulfp(R.r[2], Refine);
    Result.r[3] = __vmulfp(R.r[3], Refine);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMMatrixDeterminant
(
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR                V0, V1, V2, V3, V4, V5;
    XMVECTOR                P0, P1, P2, R, S;
    XMVECTOR                Result;
    static CONST XMVECTORI  SwizzleYXXX = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X};
    static CONST XMVECTORI  SwizzleZZYY = {XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
    static CONST XMVECTORI  SwizzleWWWZ = {XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0Z};
    static CONST XMVECTOR   Sign = {1.0f, -1.0f, 1.0f, -1.0f};

    V0 = XMVectorPermute(M.r[2], M.r[2], *(XMVECTOR*)SwizzleYXXX);
    V1 = XMVectorPermute(M.r[3], M.r[3], *(XMVECTOR*)SwizzleZZYY);
    V2 = XMVectorPermute(M.r[2], M.r[2], *(XMVECTOR*)SwizzleYXXX);
    V3 = XMVectorPermute(M.r[3], M.r[3], *(XMVECTOR*)SwizzleWWWZ);
    V4 = XMVectorPermute(M.r[2], M.r[2], *(XMVECTOR*)SwizzleZZYY);
    V5 = XMVectorPermute(M.r[3], M.r[3], *(XMVECTOR*)SwizzleWWWZ);

    P0 = XMVectorMultiply(V0, V1);
    P1 = XMVectorMultiply(V2, V3);
    P2 = XMVectorMultiply(V4, V5);

    V0 = XMVectorPermute(M.r[2], M.r[2], *(XMVECTOR*)SwizzleZZYY);
    V1 = XMVectorPermute(M.r[3], M.r[3], *(XMVECTOR*)SwizzleYXXX);
    V2 = XMVectorPermute(M.r[2], M.r[2], *(XMVECTOR*)SwizzleWWWZ);
    V3 = XMVectorPermute(M.r[3], M.r[3], *(XMVECTOR*)SwizzleYXXX);
    V4 = XMVectorPermute(M.r[2], M.r[2], *(XMVECTOR*)SwizzleWWWZ);
    V5 = XMVectorPermute(M.r[3], M.r[3], *(XMVECTOR*)SwizzleZZYY);

    P0 = XMVectorNegativeMultiplySubtract(V0, V1, P0);
    P1 = XMVectorNegativeMultiplySubtract(V2, V3, P1);
    P2 = XMVectorNegativeMultiplySubtract(V4, V5, P2);

    V0 = XMVectorPermute(M.r[1], M.r[1], *(XMVECTOR*)SwizzleWWWZ);
    V1 = XMVectorPermute(M.r[1], M.r[1], *(XMVECTOR*)SwizzleZZYY);
    V2 = XMVectorPermute(M.r[1], M.r[1], *(XMVECTOR*)SwizzleYXXX);

    S = XMVectorMultiply(M.r[0], Sign);
    R = XMVectorMultiply(V0, P0);
    R = XMVectorNegativeMultiplySubtract(V1, P1, R);
    R = XMVectorMultiplyAdd(V2, P2, R);

    Result = XMVector4Dot(S, R);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                V0, V1, V2, V3, V4, V5;
    XMVECTOR                P0, P1, P2, R, S;
    XMVECTOR                D0, D1;
    XMVECTOR                Result;
    static CONST XMVECTORI  SwizzleYXXX = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X};
    static CONST XMVECTORI  SwizzleZZYY = {XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
    static CONST XMVECTORI  SwizzleWWWZ = {XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0Z};
    static CONST XMVECTOR   Sign = {1.0f, -1.0f, 1.0f, -1.0f};

    V0 = __vperm(M.r[2], M.r[2], *(XMVECTOR*)SwizzleYXXX);
    V1 = __vperm(M.r[3], M.r[3], *(XMVECTOR*)SwizzleZZYY);
    V2 = __vperm(M.r[2], M.r[2], *(XMVECTOR*)SwizzleYXXX);
    V3 = __vperm(M.r[3], M.r[3], *(XMVECTOR*)SwizzleWWWZ);
    V4 = __vperm(M.r[2], M.r[2], *(XMVECTOR*)SwizzleZZYY);
    V5 = __vperm(M.r[3], M.r[3], *(XMVECTOR*)SwizzleWWWZ);

    P0 = __vmaddfp(V0, V1, *(XMVECTOR*)g_XMNegativeZero);
    P1 = __vmaddfp(V2, V3, *(XMVECTOR*)g_XMNegativeZero);
    P2 = __vmaddfp(V4, V5, *(XMVECTOR*)g_XMNegativeZero);

    V0 = __vperm(M.r[2], M.r[2], *(XMVECTOR*)SwizzleZZYY);
    V1 = __vperm(M.r[3], M.r[3], *(XMVECTOR*)SwizzleYXXX);
    V2 = __vperm(M.r[2], M.r[2], *(XMVECTOR*)SwizzleWWWZ);
    V3 = __vperm(M.r[3], M.r[3], *(XMVECTOR*)SwizzleYXXX);
    V4 = __vperm(M.r[2], M.r[2], *(XMVECTOR*)SwizzleWWWZ);
    V5 = __vperm(M.r[3], M.r[3], *(XMVECTOR*)SwizzleZZYY);

    P0 = __vnmsubfp(V0, V1, P0);
    P1 = __vnmsubfp(V2, V3, P1);
    P2 = __vnmsubfp(V4, V5, P2);

    V0 = __vperm(M.r[1], M.r[1], *(XMVECTOR*)SwizzleWWWZ);
    V1 = __vperm(M.r[1], M.r[1], *(XMVECTOR*)SwizzleZZYY);
    V2 = __vperm(M.r[1], M.r[1], *(XMVECTOR*)SwizzleYXXX);

    S = __vmaddfp(M.r[0], Sign, *(XMVECTOR*)g_XMNegativeZero);
    R = __vmaddfp(V0, P0, *(XMVECTOR*)g_XMNegativeZero);
    R = __vnmsubfp(V1, P1, R);
    R = __vmaddfp(V2, P2, R);

    D0 = __vmaddfp(S, R, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V0, V1, V2, V3, V4, V5;
    XMVECTOR P0, P1, P2, R, S;
    XMVECTOR Result;
    XMVECTOR Zero, SignMask;

    V0 = __vpermwi(M.r[2], 0x40);
    V1 = __vpermwi(M.r[3], 0xA5);
    V2 = __vpermwi(M.r[2], 0x40);
    V3 = __vpermwi(M.r[3], 0xFE);
    V4 = __vpermwi(M.r[2], 0xA5);
    V5 = __vpermwi(M.r[3], 0xFE);

    P0 = __vmulfp(V0, V1);
    P1 = __vmulfp(V2, V3);
    P2 = __vmulfp(V4, V5);

    V0 = __vpermwi(M.r[2], 0xA5);
    V1 = __vpermwi(M.r[3], 0x40);
    V2 = __vpermwi(M.r[2], 0xFE);
    V3 = __vpermwi(M.r[3], 0x40);
    V4 = __vpermwi(M.r[2], 0xFE);
    V5 = __vpermwi(M.r[3], 0xA5);

    P0 = __vnmsubfp(V0, V1, P0);
    P1 = __vnmsubfp(V2, V3, P1);
    P2 = __vnmsubfp(V4, V5, P2);

    V0 = __vpermwi(M.r[1], 0xFE);
    V1 = __vpermwi(M.r[1], 0xA5);
    V2 = __vpermwi(M.r[1], 0x40);

    R = __vmulfp(V0, P0);
    SignMask = __vspltisw(-1);
    Zero = __vspltisw(0);
    R = __vnmsubfp(V1, P1, R);
    SignMask = __vslw(SignMask, SignMask);
    SignMask = __vmrghw(Zero, SignMask);
    R = __vmaddfp(V2, P2, R);
    S = __vxor(M.r[0], SignMask);

    Result = __vmsum4fp(S, R);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Transformation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixIdentity()
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX M;

    M.r[0] = g_XMIdentityR0;
    M.r[1] = g_XMIdentityR1;
    M.r[2] = g_XMIdentityR2;
    M.r[3] = g_XMIdentityR3;

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR ZO;
    XMMATRIX M;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vpermwi(ZO, 0xEA);
    M.r[1] = __vpermwi(ZO, 0xBA);
    M.r[2] = __vpermwi(ZO, 0xAE);
    M.r[3] = __vpermwi(ZO, 0xAB);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixSet
(
    FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
    FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
    FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
    FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33
)
{
    XMMATRIX M;

    M.r[0] = XMVectorSet(m00, m01, m02, m03);
    M.r[1] = XMVectorSet(m10, m11, m12, m13);
    M.r[2] = XMVectorSet(m20, m21, m22, m23);
    M.r[3] = XMVectorSet(m30, m31, m32, m33);

    return M;
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixTranslation
(
    FLOAT OffsetX, 
    FLOAT OffsetY, 
    FLOAT OffsetZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX M;

    M.r[0] = g_XMIdentityR0;
    M.r[1] = g_XMIdentityR1;
    M.r[2] = g_XMIdentityR2;

    M.r[3] = XMVectorSet(OffsetX, OffsetY, OffsetZ, 1.0f);

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;
    XMVECTOR ZO;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[3] = XMVectorSet(OffsetX, OffsetY, OffsetZ, 1.0f);
    M.r[2] = __vpermwi(ZO, 0xAE);
    M.r[1] = __vpermwi(ZO, 0xBA);
    M.r[0] = __vpermwi(ZO, 0xEA);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixTranslationFromVector
(
    XMVECTOR Offset
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX M;

    M.r[3] = XMVectorSelect(g_XMIdentityR3, Offset, *(XMVECTOR*)g_XMSelect1110);

    M.r[0] = g_XMIdentityR0;
    M.r[1] = g_XMIdentityR1;
    M.r[2] = g_XMIdentityR2;

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;

    XMDUMMY_INITIALIZE_VECTOR(M.r[3]);

    M.r[3]  = __vupkd3d(M.r[3], VPACK_NORMSHORT2);

    M.r[0] = __vpermwi(M.r[3], 0xEA);
    M.r[1] = __vpermwi(M.r[3], 0xBA);
    M.r[2] = __vpermwi(M.r[3], 0xAE);
    M.r[3] = __vrlimi(M.r[3], Offset, 0xE, 0);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixScaling
(
    FLOAT ScaleX, 
    FLOAT ScaleY, 
    FLOAT ScaleZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX M;

    M.r[0] = XMVectorSet(ScaleX, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, ScaleY, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, ScaleZ, 0.0f);

    M.r[3] = g_XMIdentityR3;

    return M;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR SY, SZ;
    XMVECTOR ZO;
    XMMATRIX M;

    XMASSERT(((UINT_PTR)&ScaleX & 3) == 0);
    XMASSERT(((UINT_PTR)&ScaleY & 3) == 0);
    XMASSERT(((UINT_PTR)&ScaleZ & 3) == 0);

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);
    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = XMVectorSet(ScaleX, 0.0f, 0.0f, 0.0f);
    SY = XMVectorSet(ScaleY, 0.0f, 0.0f, 0.0f);
    SZ = XMVectorSet(ScaleZ, 0.0f, 0.0f, 0.0f);

    M.r[0] = __vrlimi(M.r[0], M.r[1], 0x7, 0);
    M.r[1] = __vrlimi(M.r[1], SY, 0x4, 3);
    M.r[2] = __vrlimi(M.r[2], SZ, 0x2, 2);
    M.r[3] = __vpermwi(ZO, 0xAB);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR SY, SZ;
    XMVECTOR ZO;
    XMMATRIX M;

    XMASSERT(((UINT_PTR)&ScaleX & 3) == 0);
    XMASSERT(((UINT_PTR)&ScaleY & 3) == 0);
    XMASSERT(((UINT_PTR)&ScaleZ & 3) == 0);

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);
    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __lvlx(&ScaleX, 0);
    SY = __lvlx(&ScaleY, 0);
    SZ = __lvlx(&ScaleZ, 0);

    M.r[0] = __vrlimi(M.r[0], M.r[1], 0x7, 0);
    M.r[1] = __vrlimi(M.r[1], SY, 0x4, 3);
    M.r[2] = __vrlimi(M.r[2], SZ, 0x2, 2);
    M.r[3] = __vpermwi(ZO, 0xAB);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixScalingFromVector
(
    XMVECTOR Scale
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX               M;
    static CONST XMVECTORI Select1000 = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_0, XM_SELECT_0};
    static CONST XMVECTORI Select0100 = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};
    static CONST XMVECTORI Select0010 = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};

    M.r[0] = XMVectorSelect(*(XMVECTOR*)Select1000, Scale, *(XMVECTOR*)Select1000);
    M.r[1] = XMVectorSelect(*(XMVECTOR*)Select0100, Scale, *(XMVECTOR*)Select0100);
    M.r[2] = XMVectorSelect(*(XMVECTOR*)Select0010, Scale, *(XMVECTOR*)Select0010);

    M.r[3] = g_XMIdentityR3;

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR ZO;
    XMMATRIX M;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);
    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vrlimi(M.r[0], Scale, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], Scale, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], Scale, 0x2, 0);
    M.r[3] = __vpermwi(ZO, 0xAB);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixRotationX
(
    FLOAT Angle
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    SinAngle;
    FLOAT    CosAngle;
    XMVECTOR V;
    XMMATRIX M;
    static CONST XMVECTORI SwizzleWZXW = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleWYZW = {XM_PERMUTE_0W, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0W};

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    V = XMVectorSet(SinAngle, -SinAngle, CosAngle, 0.0f);

    M.r[0] = g_XMIdentityR0;
    M.r[3] = g_XMIdentityR3;

    M.r[1] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleWZXW);
    M.r[2] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleWYZW);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    SinAngle;
    FLOAT    CosAngle;
    XMVECTOR V;
    XMVECTOR ZO;
    XMMATRIX M;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    V = XMVectorSet(SinAngle, -SinAngle, CosAngle, 0.0f);

    M.r[0] = __vpermwi(ZO, 0xEA);
    M.r[3] = __vpermwi(ZO, 0xAB);
    M.r[1] = __vpermwi(V, 0xE3);
    M.r[2] = __vpermwi(V, 0xDB);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixRotationY
(
    FLOAT Angle
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    SinAngle;
    FLOAT    CosAngle;
    XMVECTOR V;
    XMMATRIX M;
    static CONST XMVECTORI SwizzleZWYW = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleXWZW = {XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W};

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    V = XMVectorSet(SinAngle, -SinAngle, CosAngle, 0.0f);

    M.r[1] = g_XMIdentityR1;
    M.r[3] = g_XMIdentityR3;

    M.r[0] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleZWYW);
    M.r[2] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleXWZW);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    SinAngle;
    FLOAT    CosAngle;
    XMVECTOR V;
    XMVECTOR ZO;
    XMMATRIX M;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    V = XMVectorSet(SinAngle, -SinAngle, CosAngle, 0.0f);

    M.r[1] = __vpermwi(ZO, 0xBA);
    M.r[3] = __vpermwi(ZO, 0xAB);
    M.r[0] = __vpermwi(V, 0xB7);
    M.r[2] = __vpermwi(V, 0x3B);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixRotationZ
(
    FLOAT Angle
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    SinAngle;
    FLOAT    CosAngle;
    XMVECTOR V;
    XMMATRIX M;
    static CONST XMVECTORI SwizzleZXWW = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleYZWW = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0W};

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    V = XMVectorSet(SinAngle, -SinAngle, CosAngle, 0.0f);

    M.r[2] = g_XMIdentityR2;
    M.r[3] = g_XMIdentityR3;

    M.r[0] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleZXWW);
    M.r[1] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleYZWW);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    SinAngle;
    FLOAT    CosAngle;
    XMVECTOR V;
    XMVECTOR ZO;
    XMMATRIX M;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    V = XMVectorSet(SinAngle, -SinAngle, CosAngle, 0.0f);

    M.r[2] = __vpermwi(ZO, 0xAE);
    M.r[3] = __vpermwi(ZO, 0xAB);
    M.r[0] = __vpermwi(V, 0x8F);
    M.r[1] = __vpermwi(V, 0x6F);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixRotationRollPitchYaw
(
    FLOAT Pitch, 
    FLOAT Yaw, 
    FLOAT Roll
)
{
    XMVECTOR Angles;
    XMMATRIX M;

    Angles = XMVectorSet(Pitch, Yaw, Roll, 0.0f);
    M = XMMatrixRotationRollPitchYawFromVector(Angles);

    return M;
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixRotationRollPitchYawFromVector
(
    XMVECTOR Angles // <Pitch, Yaw, Roll, undefined>
)
{
    XMVECTOR Q;
    XMMATRIX M;
    
    Q = XMQuaternionRotationRollPitchYawFromVector(Angles);
    M = XMMatrixRotationQuaternion(Q);

    return M;
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixRotationNormal
(
    XMVECTOR NormalAxis, 
    FLOAT    Angle
)
{
#if defined(_NO_INTRINSICS_)

    FLOAT                  SinAngle;
    FLOAT                  CosAngle;
    XMVECTOR               A;
    XMVECTOR               N0, N1;
    XMVECTOR               V0, V1, V2;
    XMVECTOR               R0, R1, R2;
    XMVECTOR               C0, C1, C2;
    XMMATRIX               M;
    static CONST XMVECTORI SwizzleYZXW = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleZXYW = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute0Z1Y1Z0X = {XM_PERMUTE_0Z, XM_PERMUTE_1Y, XM_PERMUTE_1Z, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0Y1X0Y1X = {XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute0X1X1Y0W = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1Z0Y1W0W = {XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1W, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1X1Y0Z0W = {XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W};

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    A = XMVectorSet(SinAngle, CosAngle, 1.0f - CosAngle, 0.0f);

    C2 = XMVectorSplatZ(A);
    C1 = XMVectorSplatY(A);
    C0 = XMVectorSplatX(A);

    N0 = XMVectorPermute(NormalAxis, NormalAxis, *(XMVECTOR*)SwizzleYZXW);
    N1 = XMVectorPermute(NormalAxis, NormalAxis, *(XMVECTOR*)SwizzleZXYW);

    V0 = XMVectorMultiply(C2, N0);
    V0 = XMVectorMultiply(V0, N1);

    R0 = XMVectorMultiply(C2, NormalAxis);
    R0 = XMVectorMultiplyAdd(R0, NormalAxis, C1);

    R1 = XMVectorMultiplyAdd(C0, NormalAxis, V0);
    R2 = XMVectorNegativeMultiplySubtract(C0, NormalAxis, V0);

    V0 = XMVectorSelect(A, R0, *(XMVECTOR*)g_XMSelect1110);
    V1 = XMVectorPermute(R1, R2, *(XMVECTOR*)Permute0Z1Y1Z0X);
    V2 = XMVectorPermute(R1, R2, *(XMVECTOR*)Permute0Y1X0Y1X);

    M.r[0] = XMVectorPermute(V0, V1, *(XMVECTOR*)Permute0X1X1Y0W);
    M.r[1] = XMVectorPermute(V0, V1, *(XMVECTOR*)Permute1Z0Y1W0W);
    M.r[2] = XMVectorPermute(V0, V2, *(XMVECTOR*)Permute1X1Y0Z0W);
    M.r[3] = g_XMIdentityR3;

    return M;

#elif defined(_VMX32_INTRINSICS_)

    FLOAT                  SinAngle;
    FLOAT                  CosAngle;
    XMVECTOR               A;
    XMVECTOR               N0, N1;
    XMVECTOR               V0, V1, V2;
    XMVECTOR               R0, R1, R2;
    XMVECTOR               C0, C1, C2;
    XMMATRIX               M;
    static CONST XMVECTORI SwizzleYZXW = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleZXYW = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute0Z1Y1Z0X = {XM_PERMUTE_0Z, XM_PERMUTE_1Y, XM_PERMUTE_1Z, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0Y1X0Y1X = {XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute0X1X1Y0W = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1Z0Y1W0W = {XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1W, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1X1Y0Z0W = {XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W};

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    N0 = __vperm(NormalAxis, NormalAxis, *(XMVECTOR*)SwizzleYZXW);
    N1 = __vperm(NormalAxis, NormalAxis, *(XMVECTOR*)SwizzleZXYW);

    A.v[0] = SinAngle;
    A.v[1] = CosAngle;
    A.v[2] = 1.0f - CosAngle;
    A.v[3] = 0.0f;

    C2 = __vspltw(A, 2);
    C1 = __vspltw(A, 1);
    C0 = __vspltw(A, 0);

    V0 = __vmaddfp(C2, N0, *(XMVECTOR*)g_XMNegativeZero);

    R0 = __vmaddfp(C2, NormalAxis, *(XMVECTOR*)g_XMNegativeZero);

    V0 = __vmaddfp(V0, N1, *(XMVECTOR*)g_XMNegativeZero);

    R0 = __vmaddfp(R0, NormalAxis, C1);

    R1 = __vmaddfp(C0, NormalAxis, V0);
    R2 = __vnmsubfp(C0, NormalAxis, V0);

    V0 = __vsel(A, R0, *(XMVECTOR*)g_XMSelect1110);
    V1 = __vperm(R1, R2, *(XMVECTOR*)Permute0Z1Y1Z0X);
    V2 = __vperm(R1, R2, *(XMVECTOR*)Permute0Y1X0Y1X);

    M.r[0] = __vperm(V0, V1, *(XMVECTOR*)Permute0X1X1Y0W);
    M.r[1] = __vperm(V0, V1, *(XMVECTOR*)Permute1Z0Y1W0W);
    M.r[2] = __vperm(V0, V2, *(XMVECTOR*)Permute1X1Y0Z0W);
    M.r[3] = g_XMIdentityR3;

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    SinAngle;
    FLOAT    CosAngle;
    XMVECTOR N0, N1;
    XMVECTOR A;
    XMVECTOR V0;
    XMVECTOR R0, R1, R2;
    XMVECTOR C0, C1, C2;
    XMVECTOR ZO;
    XMMATRIX M;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    N0 = __vpermwi(NormalAxis, 0x63);
    N1 = __vpermwi(NormalAxis, 0x87);

    A.v[0] = SinAngle;
    A.v[1] = CosAngle;
    A.v[2] = 1.0f - CosAngle;
    A.v[3] = 0.0f;

    C2 = __vspltw(A, 2);
    C1 = __vspltw(A, 1);
    C0 = __vspltw(A, 0);
    R1 = __vspltw(A, 0);

    V0 = __vmulfp(C2, N0);

    R0 = __vmulfp(C2, NormalAxis);

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);

    R2 = __vmulfp(V0, N1);

    R0 = __vmaddcfp(NormalAxis, R0, C1);

    R1 = __vmaddcfp(NormalAxis, R1, R2);
    R2 = __vnmsubfp(NormalAxis, C0, R2);

    M.r[0] = __vrlimi(M.r[0], R0, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], R2, 0x8, 2);
    M.r[2] = __vrlimi(M.r[2], R1, 0x8, 1);

    M.r[0] = __vrlimi(M.r[0], R1, 0x4, 1);
    M.r[1] = __vrlimi(M.r[1], R0, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], R2, 0x4, 3);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vrlimi(M.r[0], R2, 0x2, 3); // <R0.x, R1.z, R2.y, 0>
    M.r[1] = __vrlimi(M.r[1], R1, 0x2, 2); // <R2.z, R0.y, R1.x, 0>
    M.r[2] = __vrlimi(M.r[2], R0, 0x2, 0); // <R1.y, R2.x, R0.z, 0>

    M.r[3] = __vpermwi(ZO, 0xAB);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixRotationAxis
(
    XMVECTOR Axis, 
    FLOAT    Angle
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Normal;
    XMMATRIX M;

    XMASSERT(!XMVector3Equal(Axis, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(Axis));

    Normal = XMVector3Normalize(Axis);
    M = XMMatrixRotationNormal(Normal, Angle);

    return M;

#elif defined(_VMX32_INTRINSICS_)

    FLOAT                  SinAngle;
    FLOAT                  CosAngle;
    XMVECTOR               NormalAxis;
    XMVECTOR               Rsq, Rcp, RT;
    XMVECTOR               A;
    XMVECTOR               N0, N1;
    XMVECTOR               V0, V1, V2;
    XMVECTOR               R0, R1, R2;
    XMVECTOR               C0, C1, C2;
    XMVECTOR               D0, D1;
    XMMATRIX               M;
    static CONST XMVECTORI SwizzleYZXW = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleZXYW = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute0Z1Y1Z0X = {XM_PERMUTE_0Z, XM_PERMUTE_1Y, XM_PERMUTE_1Z, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0Y1X0Y1X = {XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute0X1X1Y0W = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1Z0Y1W0W = {XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1W, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1X1Y0Z0W = {XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W};

    XMASSERT(!XMVector3Equal(Axis, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(Axis));

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, Axis, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);

    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D0 = __vmaddfp(Rsq, RT, Rsq);

    NormalAxis = __vmaddfp(Axis, D0, *(XMVECTOR*)g_XMNegativeZero);

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    N0 = __vperm(NormalAxis, NormalAxis, *(XMVECTOR*)SwizzleYZXW);
    N1 = __vperm(NormalAxis, NormalAxis, *(XMVECTOR*)SwizzleZXYW);

    A.v[0] = SinAngle;
    A.v[1] = CosAngle;
    A.v[2] = 1.0f - CosAngle;
    A.v[3] = 0.0f;

    C2 = __vspltw(A, 2);
    C1 = __vspltw(A, 1);
    C0 = __vspltw(A, 0);

    V0 = __vmaddfp(C2, N0, *(XMVECTOR*)g_XMNegativeZero);

    R0 = __vmaddfp(C2, NormalAxis, *(XMVECTOR*)g_XMNegativeZero);

    V0 = __vmaddfp(V0, N1, *(XMVECTOR*)g_XMNegativeZero);

    R0 = __vmaddfp(R0, NormalAxis, C1);

    R1 = __vmaddfp(C0, NormalAxis, V0);
    R2 = __vnmsubfp(C0, NormalAxis, V0);

    V0 = __vsel(A, R0, *(XMVECTOR*)g_XMSelect1110);
    V1 = __vperm(R1, R2, *(XMVECTOR*)Permute0Z1Y1Z0X);
    V2 = __vperm(R1, R2, *(XMVECTOR*)Permute0Y1X0Y1X);

    M.r[0] = __vperm(V0, V1, *(XMVECTOR*)Permute0X1X1Y0W);
    M.r[1] = __vperm(V0, V1, *(XMVECTOR*)Permute1Z0Y1W0W);
    M.r[2] = __vperm(V0, V2, *(XMVECTOR*)Permute1X1Y0Z0W);
    M.r[3] = g_XMIdentityR3;

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    SinAngle;
    FLOAT    CosAngle;
    XMVECTOR NormalAxis;
    XMVECTOR N0, N1;
    XMVECTOR A;
    XMVECTOR V0;
    XMVECTOR R0, R1, R2;
    XMVECTOR C0, C1, C2;
    XMVECTOR ZO;
    XMVECTOR D;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Zero;
    XMVECTOR H;
    XMVECTOR N;
    XMMATRIX M;

    XMASSERT(!XMVector3Equal(Axis, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(Axis));

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    H = __vspltisw(1);
    D = __vmsum3fp(Axis, Axis);
    H = __vcfsx(H, 1);
    Zero = __vspltisw(0);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    NormalAxis = __vcmpeqfp(D, Zero);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddcfp(RcpLength, H, RcpLength);
    N = __vmulfp(Axis, H);
    NormalAxis = __vsel(N, D, NormalAxis);

    XMScalarSinCos(&SinAngle, &CosAngle, Angle);

    N0 = __vpermwi(NormalAxis, 0x63);
    N1 = __vpermwi(NormalAxis, 0x87);

    A.v[0] = SinAngle;
    A.v[1] = CosAngle;
    A.v[2] = 1.0f - CosAngle;
    A.v[3] = 0.0f;

    C2 = __vspltw(A, 2);
    C1 = __vspltw(A, 1);
    C0 = __vspltw(A, 0);
    R1 = __vspltw(A, 0);

    V0 = __vmulfp(C2, N0);

    R0 = __vmulfp(C2, NormalAxis);

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);

    R2 = __vmulfp(V0, N1);

    R0 = __vmaddcfp(NormalAxis, R0, C1);

    R1 = __vmaddcfp(NormalAxis, R1, R2);
    R2 = __vnmsubfp(NormalAxis, C0, R2);

    M.r[0] = __vrlimi(M.r[0], R0, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], R2, 0x8, 2);
    M.r[2] = __vrlimi(M.r[2], R1, 0x8, 1);

    M.r[0] = __vrlimi(M.r[0], R1, 0x4, 1);
    M.r[1] = __vrlimi(M.r[1], R0, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], R2, 0x4, 3);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vrlimi(M.r[0], R2, 0x2, 3); // <R0.x, R1.z, R2.y, 0>
    M.r[1] = __vrlimi(M.r[1], R1, 0x2, 2); // <R2.z, R0.y, R1.x, 0>
    M.r[2] = __vrlimi(M.r[2], R0, 0x2, 0); // <R1.y, R2.x, R0.z, 0>

    M.r[3] = __vpermwi(ZO, 0xAB);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixRotationQuaternion
(
    XMVECTOR Quaternion
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX               M;
    XMVECTOR               Q0, Q1;
    XMVECTOR               V0, V1, V2;
    XMVECTOR               R0, R1, R2;
    static CONST XMVECTOR  Constant1110 = {1.0f, 1.0f, 1.0f, 0.0f};
    static CONST XMVECTORI SwizzleXXYW = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleZYZW = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleYZXW = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute0Y0X0X1W = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_1W};
    static CONST XMVECTORI Permute0Z0Z0Y1W = {XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1W};
    static CONST XMVECTORI Permute0Y1X1Y0Z = {XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0X1Z0X1Z = {XM_PERMUTE_0X, XM_PERMUTE_1Z, XM_PERMUTE_0X, XM_PERMUTE_1Z};
    static CONST XMVECTORI Permute0X1X1Y0W = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1Z0Y1W0W = {XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1W, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1X1Y0Z0W = {XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W};

    Q0 = XMVectorAdd(Quaternion, Quaternion);
    Q1 = XMVectorMultiply(Quaternion, Q0);

    V0 = XMVectorPermute(Q1, Constant1110, *(XMVECTOR*)Permute0Y0X0X1W);
    V1 = XMVectorPermute(Q1, Constant1110, *(XMVECTOR*)Permute0Z0Z0Y1W);
    R0 = XMVectorSubtract(Constant1110, V0);
    R0 = XMVectorSubtract(R0, V1);

    V0 = XMVectorPermute(Quaternion, Quaternion, *(XMVECTOR*)SwizzleXXYW);
    V1 = XMVectorPermute(Q0, Q0, *(XMVECTOR*)SwizzleZYZW);
    V0 = XMVectorMultiply(V0, V1);

    V1 = XMVectorSplatW(Quaternion);
    V2 = XMVectorPermute(Q0, Q0, *(XMVECTOR*)SwizzleYZXW);
    V1 = XMVectorMultiply(V1, V2);

    R1 = XMVectorAdd(V0, V1);
    R2 = XMVectorSubtract(V0, V1);

    V0 = XMVectorPermute(R1, R2, *(XMVECTOR*)Permute0Y1X1Y0Z);
    V1 = XMVectorPermute(R1, R2, *(XMVECTOR*)Permute0X1Z0X1Z);

    M.r[0] = XMVectorPermute(R0, V0, *(XMVECTOR*)Permute0X1X1Y0W);
    M.r[1] = XMVectorPermute(R0, V0, *(XMVECTOR*)Permute1Z0Y1W0W);
    M.r[2] = XMVectorPermute(R0, V1, *(XMVECTOR*)Permute1X1Y0Z0W);
    M.r[3] = g_XMIdentityR3;

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMMATRIX               M;
    XMVECTOR               Q0, Q1;
    XMVECTOR               V0, V1, V2, V3, V4, V5;
    XMVECTOR               R0, R1, R2;
    static CONST XMVECTOR  Constant1110 = {1.0f, 1.0f, 1.0f, 0.0f};
    static CONST XMVECTORI SwizzleXXYW = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleZYZW = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0W};
    static CONST XMVECTORI SwizzleYZXW = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute0Y0X0X1W = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_1W};
    static CONST XMVECTORI Permute0Z0Z0Y1W = {XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_1W};
    static CONST XMVECTORI Permute0Y1X1Y0Z = {XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI Permute0X1Z0X1Z = {XM_PERMUTE_0X, XM_PERMUTE_1Z, XM_PERMUTE_0X, XM_PERMUTE_1Z};
    static CONST XMVECTORI Permute0X1X1Y0W = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1Z0Y1W0W = {XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1W, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute1X1Y0Z0W = {XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_0W};

    Q0 = __vaddfp(Quaternion, Quaternion);

    V2 = __vperm(Quaternion, Quaternion, *(XMVECTOR*)SwizzleXXYW);
    V4 = __vspltw(Quaternion, 3);

    Q1 = __vmaddfp(Quaternion, Q0, *(XMVECTOR*)g_XMNegativeZero);

    V3 = __vperm(Q0, Q0, *(XMVECTOR*)SwizzleZYZW);
    V5 = __vperm(Q0, Q0, *(XMVECTOR*)SwizzleYZXW);

    V0 = __vperm(Q1, Constant1110, *(XMVECTOR*)Permute0Y0X0X1W);
    V1 = __vperm(Q1, Constant1110, *(XMVECTOR*)Permute0Z0Z0Y1W);

    V2 = __vmaddfp(V2, V3, *(XMVECTOR*)g_XMNegativeZero);
    V4 = __vmaddfp(V4, V5, *(XMVECTOR*)g_XMNegativeZero);

    R0 = __vsubfp(Constant1110, V0);

    R1 = __vaddfp(V2, V4);
    R2 = __vsubfp(V2, V4);

    R0 = __vsubfp(R0, V1);

    V0 = __vperm(R1, R2, *(XMVECTOR*)Permute0Y1X1Y0Z);
    V1 = __vperm(R1, R2, *(XMVECTOR*)Permute0X1Z0X1Z);

    M.r[0] = __vperm(R0, V0, *(XMVECTOR*)Permute0X1X1Y0W);
    M.r[1] = __vperm(R0, V0, *(XMVECTOR*)Permute1Z0Y1W0W);
    M.r[2] = __vperm(R0, V1, *(XMVECTOR*)Permute1X1Y0Z0W);
    M.r[3] = g_XMIdentityR3;

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;
    XMVECTOR Q0, Q1;
    XMVECTOR V0, V1, V2;
    XMVECTOR R1, R2;
    XMVECTOR ZO;
    XMVECTOR Constant1110;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    Q0 = __vaddfp(Quaternion, Quaternion);
    Q1 = __vmulfp(Quaternion, Q0);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);
    Constant1110 = __vpermwi(ZO, 0xFE);

    V0 = __vpermwi(Q1, 0x40);
    V1 = __vpermwi(Q1, 0xA4);

    M.r[0] = __vsubfp(Constant1110, V0);
    M.r[0] = __vsubfp(M.r[0], V1);

    V0 = __vpermwi(Quaternion, 0x7);
    V1 = __vpermwi(Q0, 0x9B);
    V0 = __vmulfp(V0, V1);

    V1 = __vspltw(Quaternion, 3);
    V2 = __vpermwi(Q0, 0x63);
    V1 = __vmulfp(V1, V2);

    R1 = __vaddfp(V0, V1);
    R2 = __vsubfp(V0, V1);

    M.r[0] = __vrlimi(M.r[0], ZO, 1, 3);
    M.r[1] = __vpermwi(M.r[0], 0x7);

    V0 = __vpermwi(R1, 0x42);
    M.r[2] = __vsldoi(R2, R1, 2 << 2);
    V0 = __vrlimi(V0, R2, 0x6, 3);
    M.r[2] = __vpermwi(M.r[2], 0x88);

    M.r[3] = __vpermwi(ZO, 0xAB);
    M.r[2] = __vrlimi(M.r[2], M.r[0], 0x3, 0);
    M.r[1] = __vmrglw(V0, M.r[1]);
    M.r[0] = __vrlimi(M.r[0], V0, 0x6, 3);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixTransformation2D
(
    XMVECTOR ScalingOrigin, 
    FLOAT    ScalingOrientation, 
    XMVECTOR Scaling, 
    XMVECTOR RotationOrigin, 
    FLOAT    Rotation, 
    XMVECTOR Translation
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;
    XMVECTOR VScaling;
    XMVECTOR NegScalingOrigin;
    XMVECTOR VScalingOrigin;
    XMMATRIX MScalingOriginI;
    XMMATRIX MScalingOrientation;
    XMMATRIX MScalingOrientationT;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    // M = Inverse(MScalingOrigin) * Transpose(MScalingOrientation) * MScaling * MScalingOrientation *
    //         MScalingOrigin * Inverse(MRotationOrigin) * MRotation * MRotationOrigin * MTranslation;

    VScalingOrigin       = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, ScalingOrigin, *(XMVECTOR*)g_XMSelect1100);
    NegScalingOrigin     = XMVectorNegate(VScalingOrigin);

    MScalingOriginI      = XMMatrixTranslationFromVector(NegScalingOrigin);
    MScalingOrientation  = XMMatrixRotationZ(ScalingOrientation);
    MScalingOrientationT = XMMatrixTranspose(MScalingOrientation);
    VScaling             = XMVectorSelect(g_XMOne, Scaling, *(XMVECTOR*)g_XMSelect1100);
    MScaling             = XMMatrixScalingFromVector(VScaling);
    VRotationOrigin      = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, RotationOrigin, *(XMVECTOR*)g_XMSelect1100);
    MRotation            = XMMatrixRotationZ(Rotation);
    VTranslation         = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, Translation, *(XMVECTOR*)g_XMSelect1100);

    M      = XMMatrixMultiply(MScalingOriginI, MScalingOrientationT);
    M      = XMMatrixMultiply(M, MScaling);
    M      = XMMatrixMultiply(M, MScalingOrientation);
    M.r[3] = XMVectorAdd(M.r[3], VScalingOrigin);
    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], VRotationOrigin);
    M.r[3] = XMVectorAdd(M.r[3], VTranslation);

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMMATRIX                M;
    XMMATRIX                P0, P1;
    XMVECTOR                V0, V1;
    XMVECTOR                NegScalingOrigin;
    XMVECTOR                VScalingOrigin;
    XMMATRIX                MScalingOriginI;
    XMMATRIX                MScalingOrientation;
    XMMATRIX                MScalingOrientationT;
    XMMATRIX                MScaling;
    XMVECTOR                VRotationOrigin;
    XMMATRIX                MRotation;
    XMVECTOR                VTranslation;
    FLOAT                   SinAngle, CosAngle;
    XMVECTOR                V;
    static CONST XMVECTORI  SwizzleZXWW = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0W};
    static CONST XMVECTORI  SwizzleZYWW = {XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0W};
    static CONST XMVECTORI  SwizzleXZWW = {XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0W};
    static CONST XMVECTORI  SwizzleYZWW = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0W};
    static CONST XMVECTORI  Select1000 = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_0, XM_SELECT_0};
    static CONST XMVECTORI  Select0100 = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};

    VScalingOrigin       = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, ScalingOrigin, *(XMVECTOR*)g_XMSelect1100);
    NegScalingOrigin     = XMVectorNegate(VScalingOrigin);

    MScalingOriginI      = XMMatrixTranslationFromVector(NegScalingOrigin);

    XMScalarSinCos(&SinAngle, &CosAngle, ScalingOrientation);

    V = XMVectorSet(SinAngle, -SinAngle, CosAngle, 0.0f);

    MScalingOrientation.r[2] = g_XMIdentityR2;
    MScalingOrientation.r[3] = g_XMIdentityR3;
    MScalingOrientation.r[0] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleZXWW);
    MScalingOrientation.r[1] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleYZWW);

    MScalingOrientationT.r[2] = g_XMIdentityR2;
    MScalingOrientationT.r[3] = g_XMIdentityR3;
    MScalingOrientationT.r[0] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleZYWW);
    MScalingOrientationT.r[1] = XMVectorPermute(V, V, *(XMVECTOR*)SwizzleXZWW);

    MScaling.r[0]        = XMVectorSelect(*(XMVECTOR*)Select1000, Scaling, *(XMVECTOR*)Select1000);
    MScaling.r[1]        = XMVectorSelect(*(XMVECTOR*)Select0100, Scaling, *(XMVECTOR*)Select0100);
    MScaling.r[2] = g_XMIdentityR2;
    MScaling.r[3] = g_XMIdentityR3;

    VRotationOrigin      = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, RotationOrigin, *(XMVECTOR*)g_XMSelect1100);
    MRotation            = XMMatrixRotationZ(Rotation);
    VTranslation         = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, Translation, *(XMVECTOR*)g_XMSelect1100);

    P0      = XMMatrixMultiply(MScalingOriginI, MScalingOrientationT);
    P1      = XMMatrixMultiply(MScaling, MScalingOrientation);
    V0      = XMVectorSubtract(VScalingOrigin, VRotationOrigin);
    V1      = XMVectorAdd(VRotationOrigin, VTranslation);
    P0      = XMMatrixMultiply(P0, P1);
    P0.r[3] = XMVectorAdd(P0.r[3], V0);
    M       = XMMatrixMultiply(P0, MRotation);
    M.r[3]  = XMVectorAdd(M.r[3], V1);

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;
    XMVECTOR VScaling;
    XMVECTOR NegScalingOrigin;
    XMVECTOR VScalingOrigin;
    XMMATRIX MScalingOriginI;
    XMMATRIX MScalingOrientation;
    XMMATRIX MScalingOrientationT;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    VScalingOrigin       = XMVectorZero();
    VScaling             = XMVectorSplatOne();
    VRotationOrigin      = XMVectorZero();
    VTranslation         = XMVectorZero();

    VScalingOrigin       = XMVectorInsert(VScalingOrigin, ScalingOrigin, 0, 1, 1, 0, 0);
    NegScalingOrigin     = XMVectorNegate(VScalingOrigin);

    MScalingOriginI      = XMMatrixTranslationFromVector(NegScalingOrigin);
    MScalingOrientation  = XMMatrixRotationZ(ScalingOrientation);
    MScalingOrientationT = XMMatrixTranspose(MScalingOrientation);
    VScaling             = XMVectorInsert(VScaling, Scaling, 0, 1, 1, 0, 0);
    MScaling             = XMMatrixScalingFromVector(VScaling);
    VRotationOrigin      = XMVectorInsert(VRotationOrigin, RotationOrigin, 0, 1, 1, 0, 0);
    MRotation            = XMMatrixRotationZ(Rotation);
    VTranslation         = XMVectorInsert(VTranslation, Translation, 0, 1, 1, 0, 0);

    M      = XMMatrixMultiply(MScalingOriginI, MScalingOrientationT);
    M      = XMMatrixMultiply(M, MScaling);
    M      = XMMatrixMultiply(M, MScalingOrientation);
    M.r[3] = XMVectorAdd(M.r[3], VScalingOrigin);
    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], VRotationOrigin);
    M.r[3] = XMVectorAdd(M.r[3], VTranslation);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixTransformation
(
    XMVECTOR ScalingOrigin, 
    XMVECTOR ScalingOrientationQuaternion, 
    XMVECTOR Scaling, 
    XMVECTOR RotationOrigin, 
    XMVECTOR RotationQuaternion, 
    XMVECTOR Translation
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;
    XMVECTOR NegScalingOrigin;
    XMVECTOR VScalingOrigin;
    XMMATRIX MScalingOriginI;
    XMMATRIX MScalingOrientation;
    XMMATRIX MScalingOrientationT;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    // M = Inverse(MScalingOrigin) * Transpose(MScalingOrientation) * MScaling * MScalingOrientation *
    //         MScalingOrigin * Inverse(MRotationOrigin) * MRotation * MRotationOrigin * MTranslation;

    VScalingOrigin       = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, ScalingOrigin, *(XMVECTOR*)g_XMSelect1110);
    NegScalingOrigin     = XMVectorNegate(ScalingOrigin);

    MScalingOriginI      = XMMatrixTranslationFromVector(NegScalingOrigin);
    MScalingOrientation  = XMMatrixRotationQuaternion(ScalingOrientationQuaternion);
    MScalingOrientationT = XMMatrixTranspose(MScalingOrientation);
    MScaling             = XMMatrixScalingFromVector(Scaling);
    VRotationOrigin      = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, RotationOrigin, *(XMVECTOR*)g_XMSelect1110);
    MRotation            = XMMatrixRotationQuaternion(RotationQuaternion);
    VTranslation         = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, Translation, *(XMVECTOR*)g_XMSelect1110);

    M      = XMMatrixMultiply(MScalingOriginI, MScalingOrientationT);
    M      = XMMatrixMultiply(M, MScaling);
    M      = XMMatrixMultiply(M, MScalingOrientation);
    M.r[3] = XMVectorAdd(M.r[3], VScalingOrigin);
    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], VRotationOrigin);
    M.r[3] = XMVectorAdd(M.r[3], VTranslation);

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMMATRIX M;
    XMMATRIX P0, P1;
    XMVECTOR V0, V1;
    XMVECTOR NegScalingOrigin;
    XMVECTOR VScalingOrigin;
    XMMATRIX MScalingOriginI;
    XMMATRIX MScalingOrientation;
    XMMATRIX MScalingOrientationT;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    VScalingOrigin       = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, ScalingOrigin, *(XMVECTOR*)g_XMSelect1110);
    NegScalingOrigin     = XMVectorNegate(ScalingOrigin);

    MScalingOriginI      = XMMatrixTranslationFromVector(NegScalingOrigin);
    MScalingOrientation  = XMMatrixRotationQuaternion(ScalingOrientationQuaternion);
    MScalingOrientationT = XMMatrixTranspose(MScalingOrientation);
    MScaling             = XMMatrixScalingFromVector(Scaling);
    VRotationOrigin      = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, RotationOrigin, *(XMVECTOR*)g_XMSelect1110);
    MRotation            = XMMatrixRotationQuaternion(RotationQuaternion);
    VTranslation         = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, Translation, *(XMVECTOR*)g_XMSelect1110);

    P0      = XMMatrixMultiply(MScalingOriginI, MScalingOrientationT);
    P1      = XMMatrixMultiply(MScaling, MScalingOrientation);
    V0      = XMVectorSubtract(VScalingOrigin, VRotationOrigin);
    V1      = XMVectorAdd(VRotationOrigin, VTranslation);
    P0      = XMMatrixMultiply(P0, P1);
    P0.r[3] = XMVectorAdd(P0.r[3], V0);
    M       = XMMatrixMultiply(P0, MRotation);
    M.r[3]  = XMVectorAdd(M.r[3], V1);

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;
    XMVECTOR NegScalingOrigin;
    XMVECTOR VScalingOrigin;
    XMMATRIX MScalingOriginI;
    XMMATRIX MScalingOrientation;
    XMMATRIX MScalingOrientationT;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    VScalingOrigin       = XMVectorZero();
    VRotationOrigin      = XMVectorZero();
    VTranslation         = XMVectorZero();

    VScalingOrigin       = XMVectorInsert(VScalingOrigin, ScalingOrigin, 0, 1, 1, 1, 0);
    NegScalingOrigin     = XMVectorNegate(ScalingOrigin);

    MScalingOriginI      = XMMatrixTranslationFromVector(NegScalingOrigin);
    MScalingOrientation  = XMMatrixRotationQuaternion(ScalingOrientationQuaternion);
    MScalingOrientationT = XMMatrixTranspose(MScalingOrientation);
    MScaling             = XMMatrixScalingFromVector(Scaling);
    VRotationOrigin      = XMVectorInsert(VRotationOrigin, RotationOrigin, 0, 1, 1, 1, 0);
    MRotation            = XMMatrixRotationQuaternion(RotationQuaternion);
    VTranslation         = XMVectorInsert(VTranslation, Translation, 0, 1, 1, 1, 0);

    M      = XMMatrixMultiply(MScalingOriginI, MScalingOrientationT);
    M      = XMMatrixMultiply(M, MScaling);
    M      = XMMatrixMultiply(M, MScalingOrientation);
    M.r[3] = XMVectorAdd(M.r[3], VScalingOrigin);
    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], VRotationOrigin);
    M.r[3] = XMVectorAdd(M.r[3], VTranslation);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixAffineTransformation2D
(
    XMVECTOR Scaling, 
    XMVECTOR RotationOrigin, 
    FLOAT    Rotation, 
    XMVECTOR Translation
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;
    XMVECTOR VScaling;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    // M = MScaling * Inverse(MRotationOrigin) * MRotation * MRotationOrigin * MTranslation;

    VScaling             = XMVectorSelect(g_XMOne, Scaling, *(XMVECTOR*)g_XMSelect1100);
    MScaling             = XMMatrixScalingFromVector(VScaling);
    VRotationOrigin      = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, RotationOrigin, *(XMVECTOR*)g_XMSelect1100);
    MRotation            = XMMatrixRotationZ(Rotation);
    VTranslation         = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, Translation, *(XMVECTOR*)g_XMSelect1100);

    M      = MScaling;
    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], VRotationOrigin);
    M.r[3] = XMVectorAdd(M.r[3], VTranslation);

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMMATRIX                M;
    XMVECTOR                V;
    XMVECTOR                VRotationOrigin;
    XMMATRIX                MRotation;
    XMVECTOR                VTranslation;
    static CONST XMVECTORI  Select1000 = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_0, XM_SELECT_0};
    static CONST XMVECTORI  Select0100 = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};

    M.r[0]               = XMVectorSelect(*(XMVECTOR*)Select1000, Scaling, *(XMVECTOR*)Select1000);
    M.r[1]               = XMVectorSelect(*(XMVECTOR*)Select0100, Scaling, *(XMVECTOR*)Select0100);
    M.r[2]        = g_XMIdentityR2;
    M.r[3]        = g_XMIdentityR3;

    VRotationOrigin      = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, RotationOrigin, *(XMVECTOR*)g_XMSelect1100);
    MRotation            = XMMatrixRotationZ(Rotation);
    VTranslation         = XMVectorSelect(*(XMVECTOR*)g_XMSelect1100, Translation, *(XMVECTOR*)g_XMSelect1100);

    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    V      = XMVectorAdd(VRotationOrigin, VTranslation);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], V);

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;
    XMVECTOR VScaling;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    VScaling             = XMVectorSplatOne();
    VRotationOrigin      = XMVectorZero();
    VTranslation         = XMVectorZero();

    VScaling             = XMVectorInsert(VScaling, Scaling, 0, 1, 1, 0, 0);
    MScaling             = XMMatrixScalingFromVector(VScaling);
    VRotationOrigin      = XMVectorInsert(VRotationOrigin, RotationOrigin, 0, 1, 1, 0, 0);
    MRotation            = XMMatrixRotationZ(Rotation);
    VTranslation         = XMVectorInsert(VTranslation, Translation, 0, 1, 1, 0, 0);

    M      = MScaling;
    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], VRotationOrigin);
    M.r[3] = XMVectorAdd(M.r[3], VTranslation);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixAffineTransformation
(
    XMVECTOR Scaling, 
    XMVECTOR RotationOrigin, 
    XMVECTOR RotationQuaternion, 
    XMVECTOR Translation
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    // M = MScaling * Inverse(MRotationOrigin) * MRotation * MRotationOrigin * MTranslation;

    MScaling            = XMMatrixScalingFromVector(Scaling);
    VRotationOrigin     = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, RotationOrigin, *(XMVECTOR*)g_XMSelect1110);
    MRotation           = XMMatrixRotationQuaternion(RotationQuaternion);
    VTranslation        = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, Translation, *(XMVECTOR*)g_XMSelect1110);

    M      = MScaling;
    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], VRotationOrigin);
    M.r[3] = XMVectorAdd(M.r[3], VTranslation);

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMMATRIX M;
    XMVECTOR V;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    M                   = XMMatrixScalingFromVector(Scaling);
    VRotationOrigin     = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, RotationOrigin, *(XMVECTOR*)g_XMSelect1110);
    MRotation           = XMMatrixRotationQuaternion(RotationQuaternion);
    VTranslation        = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, Translation, *(XMVECTOR*)g_XMSelect1110);

    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    V      = XMVectorAdd(VRotationOrigin, VTranslation);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], V);

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;
    XMMATRIX MScaling;
    XMVECTOR VRotationOrigin;
    XMMATRIX MRotation;
    XMVECTOR VTranslation;

    VRotationOrigin     = XMVectorZero();
    VTranslation        = XMVectorZero();

    MScaling            = XMMatrixScalingFromVector(Scaling);
    VRotationOrigin     = XMVectorInsert(VRotationOrigin, RotationOrigin, 0, 1, 1, 1, 0);
    MRotation           = XMMatrixRotationQuaternion(RotationQuaternion);
    VTranslation        = XMVectorInsert(VTranslation, Translation, 0, 1, 1, 1, 0);

    M      = MScaling;
    M.r[3] = XMVectorSubtract(M.r[3], VRotationOrigin);
    M      = XMMatrixMultiply(M, MRotation);
    M.r[3] = XMVectorAdd(M.r[3], VRotationOrigin);
    M.r[3] = XMVectorAdd(M.r[3], VTranslation);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixReflect
(
    XMVECTOR ReflectionPlane
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               P;
    XMVECTOR               S;
    XMVECTOR               A, B, C, D;
    XMMATRIX               M;
    static CONST XMVECTOR  NegativeTwo = {-2.0f, -2.0f, -2.0f, 0.0f};

    XMASSERT(!XMVector3Equal(ReflectionPlane, XMVectorZero()));
    XMASSERT(!XMPlaneIsInfinite(ReflectionPlane));

    P = XMPlaneNormalize(ReflectionPlane);
    S = XMVectorMultiply(P, NegativeTwo);

    A = XMVectorSplatX(P);
    B = XMVectorSplatY(P);
    C = XMVectorSplatZ(P);
    D = XMVectorSplatW(P);

    M.r[0] = XMVectorMultiplyAdd(A, S, g_XMIdentityR0);
    M.r[1] = XMVectorMultiplyAdd(B, S, g_XMIdentityR1);
    M.r[2] = XMVectorMultiplyAdd(C, S, g_XMIdentityR2);
    M.r[3] = XMVectorMultiplyAdd(D, S, g_XMIdentityR3);

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               P;
    XMVECTOR               S;
    XMVECTOR               A, B, C, D;
    XMVECTOR               D0, D1;
    XMVECTOR               Rsq, Rcp, RT;
    XMMATRIX               M;
    static CONST XMVECTOR  NegativeTwo = {-2.0f, -2.0f, -2.0f, 0.0f};

    XMASSERT(!XMVector3Equal(ReflectionPlane, XMVectorZero()));
    XMASSERT(!XMPlaneIsInfinite(ReflectionPlane));

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, ReflectionPlane, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);

    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D1 = __vmaddfp(Rsq, RT, Rsq);

    P = __vmaddfp(ReflectionPlane, D1, *(XMVECTOR*)g_XMNegativeZero);

    S = __vmaddfp(P, NegativeTwo, *(XMVECTOR*)g_XMNegativeZero);

    A = __vspltw(P, 0);
    B = __vspltw(P, 1);
    C = __vspltw(P, 2);
    D = __vspltw(P, 3);

    M.r[0] = __vmaddfp(A, S, g_XMIdentityR0);
    M.r[1] = __vmaddfp(B, S, g_XMIdentityR1);
    M.r[2] = __vmaddfp(C, S, g_XMIdentityR2);
    M.r[3] = __vmaddfp(D, S, g_XMIdentityR3);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR P;
    XMVECTOR S;
    XMVECTOR A, B, C, D;
    XMVECTOR NegativeTwo;
    XMVECTOR NegDoubleRP;
    XMVECTOR LengthSq;
    XMVECTOR OneHalfV;
    XMVECTOR Reciprocal;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR ZO, IdentityR0, IdentityR1, IdentityR2, IdentityR3;
    XMMATRIX M;

    XMASSERT(!XMVector3Equal(ReflectionPlane, XMVectorZero()));
    XMASSERT(!XMPlaneIsInfinite(ReflectionPlane));

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    LengthSq = __vmsum3fp(ReflectionPlane, ReflectionPlane);
    Scale = __vspltisw(1);
    Result = __vrsqrtefp(LengthSq);
    Scale = __vcfsx(Scale, 1);
    OneHalfV = __vmulfp(LengthSq, Scale);
    Reciprocal = __vmulfp(Result, Result);
    NegativeTwo = __vspltisw(-2);
    NegDoubleRP = __vspltisw(0);
    Scale = __vnmsubfp(OneHalfV, Reciprocal, Scale);
    NegativeTwo = __vcfsx(NegativeTwo, 0);
    NegDoubleRP = __vrlimi(NegDoubleRP, ReflectionPlane, 0xE, 0);
    Result = __vmaddfp(Result, Scale, Result);
    NegDoubleRP = __vmulfp(NegDoubleRP, NegativeTwo);
    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);
    P = __vmulfp(ReflectionPlane, Result);
    S = __vmulfp(NegDoubleRP, Result);

    IdentityR0 = __vpermwi(ZO, 0xEA);
    IdentityR1 = __vpermwi(ZO, 0xBA);
    IdentityR2 = __vpermwi(ZO, 0xAE);
    IdentityR3 = __vpermwi(ZO, 0xAB);

    A = __vspltw(P, 0);
    B = __vspltw(P, 1);
    C = __vspltw(P, 2);
    D = __vspltw(P, 3);

    M.r[0] = __vmaddfp(A, S, IdentityR0);
    M.r[1] = __vmaddfp(B, S, IdentityR1);
    M.r[2] = __vmaddfp(C, S, IdentityR2);
    M.r[3] = __vmaddfp(D, S, IdentityR3);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixShadow
(
    XMVECTOR ShadowPlane, 
    XMVECTOR LightPosition
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               P;
    XMVECTOR               Dot;
    XMVECTOR               A, B, C, D;
    XMMATRIX               M;
    static CONST XMVECTORI Select0001 = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_0, XM_SELECT_1};

    XMASSERT(!XMVector3Equal(ShadowPlane, XMVectorZero()));
    XMASSERT(!XMPlaneIsInfinite(ShadowPlane));

    P = XMPlaneNormalize(ShadowPlane);
    Dot = XMPlaneDot(P, LightPosition);
    P = XMVectorNegate(P);

    D = XMVectorSplatW(P);
    C = XMVectorSplatZ(P);
    B = XMVectorSplatY(P);
    A = XMVectorSplatX(P);

    Dot = XMVectorSelect(*(XMVECTOR*)Select0001, Dot, *(XMVECTOR*)Select0001);

    M.r[3] = XMVectorMultiplyAdd(D, LightPosition, Dot);

    Dot = XMVectorRotateLeft(Dot, 1);

    M.r[2] = XMVectorMultiplyAdd(C, LightPosition, Dot);

    Dot = XMVectorRotateLeft(Dot, 1);

    M.r[1] = XMVectorMultiplyAdd(B, LightPosition, Dot);

    Dot = XMVectorRotateLeft(Dot, 1);

    M.r[0] = XMVectorMultiplyAdd(A, LightPosition, Dot);

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               P;
    XMVECTOR               Dot;
    XMVECTOR               A, B, C, D;
    XMVECTOR               D0, D1;
    XMVECTOR               Rsq, Rcp, RT;
    XMVECTOR               SignMask;
    XMMATRIX               M;
    static CONST XMVECTORI Select0001 = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_0, XM_SELECT_1};

    XMASSERT(!XMVector3Equal(ShadowPlane, XMVectorZero()));
    XMASSERT(!XMPlaneIsInfinite(ShadowPlane));

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, ShadowPlane, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);

    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D1 = __vmaddfp(Rsq, RT, Rsq);

    P = __vmaddfp(ShadowPlane, D1, *(XMVECTOR*)g_XMNegativeZero);

    D0 = __vmaddfp(P, LightPosition, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    SignMask = __vspltisw(-1);
    D1 = __vsldoi(D0, D0, 4);
    SignMask = __vslw(SignMask, SignMask);
    Dot = __vaddfp(D0, D1);

    P = __vxor(P, SignMask);

    Dot = __vsel(*(XMVECTOR*)Select0001, Dot, *(XMVECTOR*)Select0001);

    D = __vspltw(P, 3);
    C = __vspltw(P, 2);
    B = __vspltw(P, 1);
    A = __vspltw(P, 0);

    M.r[3] = __vmaddfp(D, LightPosition, Dot);

    Dot = __vsldoi(Dot, Dot, 4);

    M.r[2] = __vmaddfp(C, LightPosition, Dot);

    Dot = __vsldoi(Dot, Dot, 4);

    M.r[1] = __vmaddfp(B, LightPosition, Dot);

    Dot = __vsldoi(Dot, Dot, 4);

    M.r[0] = __vmaddfp(A, LightPosition, Dot);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR P;
    XMVECTOR Dot;
    XMVECTOR A, B, C, D;
    XMVECTOR LengthSq;
    XMVECTOR OneHalfV;
    XMVECTOR Reciprocal;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR SignMask;
    XMVECTOR D0, D1, D2, D3;
    XMMATRIX M;

    XMASSERT(!XMVector3Equal(ShadowPlane, XMVectorZero()));
    XMASSERT(!XMPlaneIsInfinite(ShadowPlane));

    LengthSq = __vmsum3fp(ShadowPlane, ShadowPlane);
    Scale = __vspltisw(1);
    Result = __vrsqrtefp(LengthSq);
    Scale = __vcfsx(Scale, 1);
    OneHalfV = __vmulfp(LengthSq, Scale);
    Reciprocal = __vmulfp(Result, Result);
    Scale = __vnmsubfp(OneHalfV, Reciprocal, Scale);
    Result = __vmaddfp(Result, Scale, Result);
    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    P = __vmulfp(ShadowPlane, Result);

    Dot = __vmsum4fp(P, LightPosition);

    P = __vxor(P, SignMask);

    D0 = __vspltisw(0);
    D1 = __vspltisw(0);
    D2 = __vspltisw(0);
    D3 = __vspltisw(0);

    A = __vspltw(P, 0);
    B = __vspltw(P, 1);
    C = __vspltw(P, 2);
    D = __vspltw(P, 3);

    D0 = __vrlimi(D0, Dot, 0x8, 3);
    D1 = __vrlimi(D1, Dot, 0x4, 2);
    D2 = __vrlimi(D2, Dot, 0x2, 1);
    D3 = __vrlimi(D3, Dot, 0x1, 0);

    M.r[0] = XMVectorMultiplyAdd(A, LightPosition, D0);
    M.r[1] = XMVectorMultiplyAdd(B, LightPosition, D1);
    M.r[2] = XMVectorMultiplyAdd(C, LightPosition, D2);
    M.r[3] = XMVectorMultiplyAdd(D, LightPosition, D3);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// View and projection initialization operations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixLookAtLH
(
    XMVECTOR EyePosition, 
    XMVECTOR FocusPosition, 
    XMVECTOR UpDirection
)
{
    XMVECTOR EyeDirection;
    XMMATRIX M;

    EyeDirection = XMVectorSubtract(FocusPosition, EyePosition);
    M = XMMatrixLookToLH(EyePosition, EyeDirection, UpDirection);
    
    return M;
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixLookAtRH
(
    XMVECTOR EyePosition, 
    XMVECTOR FocusPosition, 
    XMVECTOR UpDirection
)
{
    XMVECTOR NegEyeDirection;
    XMMATRIX M;

    NegEyeDirection = XMVectorSubtract(EyePosition, FocusPosition);
    M = XMMatrixLookToLH(EyePosition, NegEyeDirection, UpDirection);
    
    return M;
}

//------------------------------------------------------------------------------

XMINLINE XMMATRIX XMMatrixLookToLH
(
    XMVECTOR EyePosition, 
    XMVECTOR EyeDirection, 
    XMVECTOR UpDirection
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR NegEyePosition;
    XMVECTOR D0, D1, D2;
    XMVECTOR R0, R1, R2;
    XMMATRIX M;

    XMASSERT(!XMVector3Equal(EyeDirection, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(EyeDirection));
    XMASSERT(!XMVector3Equal(UpDirection, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(UpDirection));

    R2 = XMVector3Normalize(EyeDirection);

    R0 = XMVector3Cross(UpDirection, R2);
    R0 = XMVector3Normalize(R0);

    R1 = XMVector3Cross(R2, R0);

    NegEyePosition = XMVectorNegate(EyePosition);

    D0 = XMVector3Dot(R0, NegEyePosition);
    D1 = XMVector3Dot(R1, NegEyePosition);
    D2 = XMVector3Dot(R2, NegEyePosition);

    M.r[0] = XMVectorSelect(D0, R0, *(XMVECTOR*)g_XMSelect1110);
    M.r[1] = XMVectorSelect(D1, R1, *(XMVECTOR*)g_XMSelect1110);
    M.r[2] = XMVectorSelect(D2, R2, *(XMVECTOR*)g_XMSelect1110);
    M.r[3] = g_XMIdentityR3;

    M = XMMatrixTranspose(M);

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               EyeDir;
    XMVECTOR               R0, R1, R2;
    XMVECTOR               V0, V1;
    XMVECTOR               X, Y, Z;
    XMVECTOR               D0, D1;
    XMVECTOR               Rsq, Rcp, RT;
    XMVECTOR               CrossV1A, CrossV1B, CrossV2A, CrossV2B, CrossR;
    XMMATRIX               M;
    static CONST XMVECTORI Permute0X1X0Y1Y = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_0Y, XM_PERMUTE_1Y};
    static CONST XMVECTORI Permute0Z1Z0Z1Z = {XM_PERMUTE_0Z, XM_PERMUTE_1Z, XM_PERMUTE_0Z, XM_PERMUTE_1Z};
    static CONST XMVECTORI Permute0X0Y1X1W = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1W};
    static CONST XMVECTORI Permute0Z0W1Y1W = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1Y, XM_PERMUTE_1W};
    static CONST XMVECTORI Permute0X0Y1Z1W = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1Z, XM_PERMUTE_1W};

    XMASSERT(!XMVector3Equal(EyeDirection, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(EyeDirection));
    XMASSERT(!XMVector3Equal(UpDirection, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(UpDirection));

    EyeDir = __vsel(*(XMVECTOR*)g_XMSelect1110, EyeDirection, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(EyeDir, EyeDir, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);

    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D0 = __vmaddfp(Rsq, RT, Rsq);

    R2 = __vmaddfp(EyeDir, D0, *(XMVECTOR*)g_XMNegativeZero);

    CrossV1A = __vperm(UpDirection, UpDirection, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossV1B = __vperm(UpDirection, UpDirection, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV2A = __vperm(R2, R2, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV2B = __vperm(R2, R2, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossR = __vmaddfp(CrossV1A, CrossV2A, *(XMVECTOR*)g_XMNegativeZero);
    R0 = __vnmsubfp(CrossV1B, CrossV2B, CrossR);

    R0 = __vsel(*(XMVECTOR*)g_XMSelect1110, R0, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(R0, R0, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);

    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D0 = __vmaddfp(Rsq, RT, Rsq);

    R0 = __vmaddfp(R0, D0, *(XMVECTOR*)g_XMNegativeZero);

    CrossV1A = __vperm(R2, R2, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossV1B = __vperm(R2, R2, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV2A = __vperm(R0, R0, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV2B = __vperm(R0, R0, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossR = __vmaddfp(CrossV1A, CrossV2A, *(XMVECTOR*)g_XMNegativeZero);
    R1 = __vnmsubfp(CrossV1B, CrossV2B, CrossR);

    V0 = __vperm(R0, R1, *(XMVECTOR*)Permute0X1X0Y1Y);
    V1 = __vperm(R0, R1, *(XMVECTOR*)Permute0Z1Z0Z1Z);

    X = __vspltw(EyePosition, 0);
    Y = __vspltw(EyePosition, 1);
    Z = __vspltw(EyePosition, 2);

    M.r[0] = __vperm(V0, R2, *(XMVECTOR*)Permute0X0Y1X1W);
    M.r[1] = __vperm(V0, R2, *(XMVECTOR*)Permute0Z0W1Y1W);
    M.r[2] = __vperm(V1, R2, *(XMVECTOR*)Permute0X0Y1Z1W);

    M.r[3] = __vnmsubfp(M.r[0], X, g_XMIdentityR3);
    M.r[3] = __vnmsubfp(M.r[1], Y, M.r[3]);
    M.r[3] = __vnmsubfp(M.r[2], Z, M.r[3]);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR NegEyePosition;
    XMVECTOR R0, R1, R2;
    XMVECTOR ZO;
    XMMATRIX P;
    XMVECTOR D;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR H, HS;
    XMVECTOR CrossV1, CrossV2;
    XMVECTOR CrossR2A, CrossR2B;
    XMVECTOR SignMask;
    XMMATRIX M;

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    XMASSERT(!XMVector3Equal(EyeDirection, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(EyeDirection));
    XMASSERT(!XMVector3Equal(UpDirection, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(UpDirection));

    H = __vspltisw(1);
    D = __vmsum3fp(EyeDirection, EyeDirection);
    H = __vcfsx(H, 1);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    Scale = __vmulfp(RcpLength, RcpLength);
    HS = H;
    HS = __vnmsubfp(OneHalfLengthSq, Scale, HS);
    HS = __vmaddcfp(RcpLength, HS, RcpLength);

    R2 = __vmulfp(EyeDirection, HS);

    CrossV1 = __vpermwi(UpDirection, 0x63);
    CrossR2A = __vpermwi(R2, 0x87);
    R0 = __vmulfp(CrossV1, CrossR2A);
    CrossV1 = __vpermwi(UpDirection, 0x87);
    CrossR2B = __vpermwi(R2, 0x63);
    R0 = __vnmsubfp(CrossV1, CrossR2B, R0);

    D = __vmsum3fp(R0, R0);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddcfp(RcpLength, H, RcpLength);
    SignMask = __vspltisw(-1);
    R0 = __vmulfp(R0, H);

    SignMask = __vslw(SignMask, SignMask);
    NegEyePosition = __vxor(EyePosition, SignMask);

    CrossV2 = __vpermwi(R0, 0x87);
    R1 = __vmulfp(CrossR2B, CrossV2);
    CrossV2 = __vpermwi(R0, 0x63);
    R1 = __vnmsubfp(CrossR2A, CrossV2, R1);

    M.r[0] = __vmsum3fp(R0, NegEyePosition);
    M.r[1] = __vmsum3fp(R1, NegEyePosition);
    M.r[2] = __vmsum3fp(R2, NegEyePosition);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vrlimi(M.r[0], R0, 0xE, 0);
    M.r[1] = __vrlimi(M.r[1], R1, 0xE, 0);
    M.r[2] = __vrlimi(M.r[2], R2, 0xE, 0);
    M.r[3] = __vpermwi(ZO, 0xAB);

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    M.r[0] = __vmrghw(P.r[0], P.r[1]);
    M.r[1] = __vmrglw(P.r[0], P.r[1]);
    M.r[2] = __vmrghw(P.r[2], P.r[3]);
    M.r[3] = __vmrglw(P.r[2], P.r[3]);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixLookToRH
(
    XMVECTOR EyePosition, 
    XMVECTOR EyeDirection, 
    XMVECTOR UpDirection
)
{
    XMVECTOR NegEyeDirection;
    XMMATRIX M;

    NegEyeDirection = XMVectorNegate(EyeDirection);
    M = XMMatrixLookToLH(EyePosition, NegEyeDirection, UpDirection);

    return M;
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixPerspectiveLH
(
    FLOAT ViewWidth, 
    FLOAT ViewHeight, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX M;
    FLOAT    TwoNearZ;

    XMASSERT(!XMScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    TwoNearZ = NearZ + NearZ;

    M.r[0] = XMVectorSet(TwoNearZ / ViewWidth, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, TwoNearZ / ViewHeight, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, FarZ / (FarZ - NearZ), 1.0f);
    M.r[3] = XMVectorSet(0.0f, 0.0f, -M.r[2].v[2] * NearZ, 0.0f);

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;
    XMVECTOR V;
    XMVECTOR ZO;
    FLOAT    TwoNearZ;

    XMASSERT(!XMScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[3] = __vspltisw(0);
    M.r[2] = __vpermwi(ZO, 0xAB);

    TwoNearZ = NearZ + NearZ;

    V.v[0] = TwoNearZ / ViewWidth;
    V.v[1] = TwoNearZ / ViewHeight;
    V.v[2] = FarZ / (FarZ - NearZ);
    V.v[3] = -V.v[2] * NearZ;

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], V, 0x2, 0);
    M.r[3] = __vrlimi(M.r[3], V, 0x2, 1);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixPerspectiveRH
(
    FLOAT ViewWidth, 
    FLOAT ViewHeight, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX M;
    FLOAT    TwoNearZ;

    XMASSERT(!XMScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    TwoNearZ = NearZ + NearZ;

    M.r[0] = XMVectorSet(TwoNearZ / ViewWidth, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, TwoNearZ / ViewHeight, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, FarZ / (NearZ - FarZ), -1.0f);
    M.r[3] = XMVectorSet(0.0f, 0.0f, M.r[2].v[2] * NearZ, 0.0f);

    return M;

#else // _VMX128_INTRINSICS_

    XMMATRIX M;
    XMVECTOR V;
    FLOAT    TwoNearZ;

    XMASSERT(!XMScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    M.r[2] = __vspltisw(-1);
    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[3] = __vspltisw(0);
    M.r[2] = __vcfsx(M.r[2], 0);

    TwoNearZ = NearZ + NearZ;

    V.v[0] = TwoNearZ / ViewWidth;
    V.v[1] = TwoNearZ / ViewHeight;
    V.v[2] = FarZ / (NearZ - FarZ);
    V.v[3] = V.v[2] * NearZ;

    M.r[2] = __vsldoi(M.r[0], M.r[2], 1 << 2);

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[3] = __vrlimi(M.r[3], V, 0x2, 1);
    M.r[2] = __vrlimi(M.r[2], V, 0x2, 0);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixPerspectiveFovLH
(
    FLOAT FovAngleY, 
    FLOAT AspectHByW, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    SinFov;
    FLOAT    CosFov;
    FLOAT    Height;
    FLOAT    Width;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
    XMASSERT(!XMScalarNearEqual(AspectHByW, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

    Height = CosFov / SinFov;
    Width = Height / AspectHByW;

    M.r[0] = XMVectorSet(Width, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, Height, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, FarZ / (FarZ - NearZ), 1.0f);
    M.r[3] = XMVectorSet(0.0f, 0.0f, -M.r[2].v[2] * NearZ, 0.0f);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    SinFov;
    FLOAT    CosFov;
    FLOAT    Height;
    FLOAT    Width;
    XMVECTOR ZO, V;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
    XMASSERT(!XMScalarNearEqual(AspectHByW, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[3] = __vspltisw(0);
    M.r[2] = __vpermwi(ZO, 0xAB);

    XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

    Height = CosFov / SinFov;
    Width = Height / AspectHByW;

    V.v[0] = Width;
    V.v[1] = Height;
    V.v[2] = FarZ / (FarZ - NearZ);
    V.v[3] = -V.v[2] * NearZ;

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], V, 0x2, 0);
    M.r[3] = __vrlimi(M.r[3], V, 0x2, 1);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixPerspectiveFovRH
(
    FLOAT FovAngleY, 
    FLOAT AspectHByW, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    SinFov;
    FLOAT    CosFov;
    FLOAT    Height;
    FLOAT    Width;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
    XMASSERT(!XMScalarNearEqual(AspectHByW, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

    Height = CosFov / SinFov;
    Width = Height / AspectHByW;

    M.r[0] = XMVectorSet(Width, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, Height, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, FarZ / (NearZ - FarZ), -1.0f);
    M.r[3] = XMVectorSet(0.0f, 0.0f, M.r[2].v[2] * NearZ, 0.0f);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    SinFov;
    FLOAT    CosFov;
    FLOAT    Height;
    FLOAT    Width;
    XMVECTOR V;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
    XMASSERT(!XMScalarNearEqual(AspectHByW, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    M.r[2] = __vspltisw(-1);
    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[3] = __vspltisw(0);
    M.r[2] = __vcfsx(M.r[2], 0);

    XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

    Height = CosFov / SinFov;
    Width = Height / AspectHByW;

    V.v[0] = Width;
    V.v[1] = Height;
    V.v[2] = FarZ / (NearZ - FarZ);
    V.v[3] = V.v[2] * NearZ;

    M.r[2] = __vsldoi(M.r[0], M.r[2], 1 << 2);

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[3] = __vrlimi(M.r[3], V, 0x2, 1);
    M.r[2] = __vrlimi(M.r[2], V, 0x2, 0);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixPerspectiveOffCenterLH
(
    FLOAT ViewLeft, 
    FLOAT ViewRight, 
    FLOAT ViewBottom, 
    FLOAT ViewTop, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    TwoNearZ;
    FLOAT    ReciprocalWidth;
    FLOAT    ReciprocalHeight;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    TwoNearZ = NearZ + NearZ;
    ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

    M.r[0] = XMVectorSet(TwoNearZ * ReciprocalWidth, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, TwoNearZ * ReciprocalHeight, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(-(ViewLeft + ViewRight) * ReciprocalWidth, 
                         -(ViewTop + ViewBottom) * ReciprocalHeight,
                         FarZ / (FarZ - NearZ),
                         1.0f);
    M.r[3] = XMVectorSet(0.0f, 0.0f, -M.r[2].v[2] * NearZ, 0.0f);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    TwoNearZ;
    FLOAT    ReciprocalWidth;
    FLOAT    ReciprocalHeight;
    FLOAT    R22;
    XMVECTOR V;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[3] = __vspltisw(0);

    TwoNearZ = NearZ + NearZ;
    ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

    R22 = FarZ / (FarZ - NearZ);

    V.v[0] = TwoNearZ * ReciprocalWidth;
    V.v[1] = TwoNearZ * ReciprocalHeight;
    V.v[2] = 0.0;
    V.v[3] = -R22 * NearZ;

    M.r[2].v[0] = -(ViewLeft + ViewRight) * ReciprocalWidth;
    M.r[2].v[1] = -(ViewTop + ViewBottom) * ReciprocalHeight;
    M.r[2].v[2] = R22;
    M.r[2].v[3] = 1.0f;

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[3] = __vrlimi(M.r[3], V, 0x2, 1);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixPerspectiveOffCenterRH
(
    FLOAT ViewLeft, 
    FLOAT ViewRight, 
    FLOAT ViewBottom, 
    FLOAT ViewTop, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    TwoNearZ;
    FLOAT    ReciprocalWidth;
    FLOAT    ReciprocalHeight;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    TwoNearZ = NearZ + NearZ;
    ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

    M.r[0] = XMVectorSet(TwoNearZ * ReciprocalWidth, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, TwoNearZ * ReciprocalHeight, 0.0f, 0.0f);
    M.r[2] = XMVectorSet((ViewLeft + ViewRight) * ReciprocalWidth, 
                         (ViewTop + ViewBottom) * ReciprocalHeight,
                         FarZ / (NearZ - FarZ),
                         -1.0f);
    M.r[3] = XMVectorSet(0.0f, 0.0f, M.r[2].v[2] * NearZ, 0.0f);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    TwoNearZ;
    FLOAT    ReciprocalWidth;
    FLOAT    ReciprocalHeight;
    FLOAT    R22;
    XMVECTOR V;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[3] = __vspltisw(0);

    TwoNearZ = NearZ + NearZ;
    ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

    R22 = FarZ / (NearZ - FarZ);

    V.v[0] = TwoNearZ * ReciprocalWidth;
    V.v[1] = TwoNearZ * ReciprocalHeight;
    V.v[2] = 0.0;
    V.v[3] = R22 * NearZ;

    M.r[2].v[0] = (ViewLeft + ViewRight) * ReciprocalWidth;
    M.r[2].v[1] = (ViewTop + ViewBottom) * ReciprocalHeight;
    M.r[2].v[2] = R22;
    M.r[2].v[3] = -1.0f;

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[3] = __vrlimi(M.r[3], V, 0x2, 1);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixOrthographicLH
(
    FLOAT ViewWidth, 
    FLOAT ViewHeight, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    M.r[0] = XMVectorSet(2.0f / ViewWidth, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, 2.0f / ViewHeight, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f / (FarZ - NearZ), 0.0f);
    M.r[3] = XMVectorSet(0.0f, 0.0f, -M.r[2].v[2] * NearZ, 1.0f);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR ZO, V;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);
    M.r[3] = __vpermwi(ZO, 0xAB);

    V.v[0] = 2.0f / ViewWidth;
    V.v[1] = 2.0f / ViewHeight;
    V.v[2] = 1.0f / (FarZ - NearZ);
    V.v[3] = -V.v[2] * NearZ;

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], V, 0x2, 0);
    M.r[3] = __vrlimi(M.r[3], V, 0x2, 1);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixOrthographicRH
(
    FLOAT ViewWidth, 
    FLOAT ViewHeight, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    M.r[0] = XMVectorSet(2.0f / ViewWidth, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, 2.0f / ViewHeight, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f / (NearZ - FarZ), 0.0f);
    M.r[3] = XMVectorSet(0.0f, 0.0f, M.r[2].v[2] * NearZ, 1.0f);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR ZO, V;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);
    M.r[3] = __vpermwi(ZO, 0xAB);

    V.v[0] = 2.0f / ViewWidth;
    V.v[1] = 2.0f / ViewHeight;
    V.v[2] = 1.0f / (NearZ - FarZ);
    V.v[3] = V.v[2] * NearZ;

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], V, 0x2, 0);
    M.r[3] = __vrlimi(M.r[3], V, 0x2, 1);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixOrthographicOffCenterLH
(
    FLOAT ViewLeft, 
    FLOAT ViewRight, 
    FLOAT ViewBottom, 
    FLOAT ViewTop, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    ReciprocalWidth;
    FLOAT    ReciprocalHeight;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

    M.r[0] = XMVectorSet(ReciprocalWidth + ReciprocalWidth, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, ReciprocalHeight + ReciprocalHeight, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f / (FarZ - NearZ), 0.0f);
    M.r[3] = XMVectorSet(-(ViewLeft + ViewRight) * ReciprocalWidth, 
                         -(ViewTop + ViewBottom) * ReciprocalHeight,
                         -M.r[2].v[2] * NearZ,
                         1.0f);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    ReciprocalWidth;
    FLOAT    ReciprocalHeight;
    FLOAT    R22;
    XMVECTOR V;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);

    ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

    R22 = 1.0f / (FarZ - NearZ);

    V.v[0] = ReciprocalWidth + ReciprocalWidth;
    V.v[1] = ReciprocalHeight + ReciprocalHeight;
    V.v[2] = R22;
    V.v[3] = 0.0f;

    M.r[3].v[0] = -(ViewLeft + ViewRight) * ReciprocalWidth;
    M.r[3].v[1] = -(ViewTop + ViewBottom) * ReciprocalHeight;
    M.r[3].v[2] = -R22 * NearZ;
    M.r[3].v[3] = 1.0f;

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], V, 0x2, 0);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMMatrixOrthographicOffCenterRH
(
    FLOAT ViewLeft, 
    FLOAT ViewRight, 
    FLOAT ViewBottom, 
    FLOAT ViewTop, 
    FLOAT NearZ, 
    FLOAT FarZ
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    FLOAT    ReciprocalWidth;
    FLOAT    ReciprocalHeight;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

    M.r[0] = XMVectorSet(ReciprocalWidth + ReciprocalWidth, 0.0f, 0.0f, 0.0f);
    M.r[1] = XMVectorSet(0.0f, ReciprocalHeight + ReciprocalHeight, 0.0f, 0.0f);
    M.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f / (NearZ - FarZ), 0.0f);
    M.r[3] = XMVectorSet(-(ViewLeft + ViewRight) * ReciprocalWidth, 
                         -(ViewTop + ViewBottom) * ReciprocalHeight,
                         M.r[2].v[2] * NearZ,
                         1.0f);

    return M;

#else // _VMX128_INTRINSICS_

    FLOAT    ReciprocalWidth;
    FLOAT    ReciprocalHeight;
    FLOAT    R22;
    XMVECTOR V;
    XMMATRIX M;

    XMASSERT(!XMScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
    XMASSERT(!XMScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
    XMASSERT(!XMScalarNearEqual(FarZ, NearZ, 0.00001f));

    M.r[0] = __vspltisw(0);
    M.r[1] = __vspltisw(0);
    M.r[2] = __vspltisw(0);

    ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
    ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

    R22 = 1.0f / (NearZ - FarZ);

    V.v[0] = ReciprocalWidth + ReciprocalWidth;
    V.v[1] = ReciprocalHeight + ReciprocalHeight;
    V.v[2] = R22;
    V.v[3] = 0.0f;

    M.r[3].v[0] = -(ViewLeft + ViewRight) * ReciprocalWidth;
    M.r[3].v[1] = -(ViewTop + ViewBottom) * ReciprocalHeight;
    M.r[3].v[2] = R22 * NearZ;
    M.r[3].v[3] = 1.0f;

    M.r[0] = __vrlimi(M.r[0], V, 0x8, 0);
    M.r[1] = __vrlimi(M.r[1], V, 0x4, 0);
    M.r[2] = __vrlimi(M.r[2], V, 0x2, 0);

    return M;

#endif // _VMX128_INTRINSICS_
}

#ifdef __cplusplus

/****************************************************************************
 *
 * XMMATRIX operators and methods
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMMATRIX::_XMMATRIX
(
    XMVECTOR R0,
    XMVECTOR R1,
    XMVECTOR R2,
    XMVECTOR R3
)
{
    r[0] = R0;
    r[1] = R1;
    r[2] = R2;
    r[3] = R3;
}

//------------------------------------------------------------------------------

XMFINLINE _XMMATRIX::_XMMATRIX
(
    FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
    FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
    FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
    FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33
)
{
    r[0] = XMVectorSet(m00, m01, m02, m03);
    r[1] = XMVectorSet(m10, m11, m12, m13);
    r[2] = XMVectorSet(m20, m21, m22, m23);
    r[3] = XMVectorSet(m30, m31, m32, m33);
}

//------------------------------------------------------------------------------

XMFINLINE _XMMATRIX::_XMMATRIX
(
    CONST FLOAT* pArray
)
{
    r[0] = XMLoadFloat4((XMFLOAT4*)pArray);
    r[1] = XMLoadFloat4((XMFLOAT4*)(pArray + 4));
    r[2] = XMLoadFloat4((XMFLOAT4*)(pArray + 8));
    r[3] = XMLoadFloat4((XMFLOAT4*)(pArray + 12));
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT _XMMATRIX::operator()
(
    UINT Row,
    UINT Column
) CONST
{
    return r[Row].v[Column];
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT& _XMMATRIX::operator()
(
    UINT Row,
    UINT Column
)
{
    return r[Row].v[Column];
}

//------------------------------------------------------------------------------

XMFINLINE _XMMATRIX& _XMMATRIX::operator=
(
    CONST _XMMATRIX& M
)
{
    r[0] = M.r[0];
    r[1] = M.r[1];
    r[2] = M.r[2];
    r[3] = M.r[3];
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMMATRIX& _XMMATRIX::operator*=
(
    CONST _XMMATRIX& M
)
{
    *this = XMMatrixMultiply(*this, M);
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMMATRIX _XMMATRIX::operator* 
(
    CONST _XMMATRIX& M
) CONST
{
    return XMMatrixMultiply(*this, M);
}

/****************************************************************************
 *
 * XMFLOAT3X3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT3X3::_XMFLOAT3X3
(
    FLOAT m00, FLOAT m01, FLOAT m02,
    FLOAT m10, FLOAT m11, FLOAT m12,
    FLOAT m20, FLOAT m21, FLOAT m22
)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;

    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;

    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT3X3::_XMFLOAT3X3
(
    CONST FLOAT* pArray
)
{
    UINT Row;
    UINT Column;

    for (Row = 0; Row < 3; Row++)
    {
        for (Column = 0; Column < 3; Column++)
        {
            m[Row][Column] = pArray[Row * 3 + Column];
        }
    }
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT _XMFLOAT3X3::operator()
(
    UINT Row,
    UINT Column
) CONST
{
    return m[Row][Column];
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT& _XMFLOAT3X3::operator()
(
    UINT Row,
    UINT Column
)
{
    return m[Row][Column];
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT3X3& _XMFLOAT3X3::operator=
(
    CONST _XMFLOAT3X3& Float3x3
)
{
    _11 = Float3x3._11;
    _12 = Float3x3._12;
    _13 = Float3x3._13;
    _21 = Float3x3._21;
    _22 = Float3x3._22;
    _23 = Float3x3._23;
    _31 = Float3x3._31;
    _32 = Float3x3._32;
    _33 = Float3x3._33;

    return *this;
}

/****************************************************************************
 *
 * XMFLOAT4X3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4X3::_XMFLOAT4X3
(
    FLOAT m00, FLOAT m01, FLOAT m02,
    FLOAT m10, FLOAT m11, FLOAT m12,
    FLOAT m20, FLOAT m21, FLOAT m22,
    FLOAT m30, FLOAT m31, FLOAT m32
)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;

    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;

    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;

    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4X3::_XMFLOAT4X3
(
    CONST FLOAT* pArray
)
{
    UINT Row;
    UINT Column;

    for (Row = 0; Row < 4; Row++)
    {
        for (Column = 0; Column < 3; Column++)
        {
            m[Row][Column] = pArray[Row * 3 + Column];
        }
    }
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT _XMFLOAT4X3::operator()
(
    UINT Row,
    UINT Column
) CONST
{
    return m[Row][Column];
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT& _XMFLOAT4X3::operator()
(
    UINT Row,
    UINT Column
)
{
    return m[Row][Column];
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4X3& _XMFLOAT4X3::operator=
(
    CONST _XMFLOAT4X3& Float4x3
)
{
    XMVECTOR V1 = XMLoadFloat4((XMFLOAT4*)&Float4x3._11);
    XMVECTOR V2 = XMLoadFloat4((XMFLOAT4*)&Float4x3._22);
    XMVECTOR V3 = XMLoadFloat4((XMFLOAT4*)&Float4x3._33);

    XMStoreFloat4((XMFLOAT4*)&_11, V1);
    XMStoreFloat4((XMFLOAT4*)&_22, V2);
    XMStoreFloat4((XMFLOAT4*)&_33, V3);

    return *this;
}

/****************************************************************************
 *
 * XMFLOAT4X4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4X4::_XMFLOAT4X4
(
    FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
    FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
    FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
    FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33
)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;

    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;

    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;

    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4X4::_XMFLOAT4X4
(
    CONST FLOAT* pArray
)
{
    UINT Row;
    UINT Column;

    for (Row = 0; Row < 4; Row++)
    {
        for (Column = 0; Column < 4; Column++)
        {
            m[Row][Column] = pArray[Row * 4 + Column];
        }
    }
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT _XMFLOAT4X4::operator()
(
    UINT Row,
    UINT Column
) CONST
{
    return m[Row][Column];
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT& _XMFLOAT4X4::operator()
(
    UINT Row,
    UINT Column
)
{
    return m[Row][Column];
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4X4& _XMFLOAT4X4::operator=
(
    CONST _XMFLOAT4X4& Float4x4
)
{
    XMVECTOR V1 = XMLoadFloat4((XMFLOAT4*)&Float4x4._11);
    XMVECTOR V2 = XMLoadFloat4((XMFLOAT4*)&Float4x4._21);
    XMVECTOR V3 = XMLoadFloat4((XMFLOAT4*)&Float4x4._31);
    XMVECTOR V4 = XMLoadFloat4((XMFLOAT4*)&Float4x4._41);

    XMStoreFloat4((XMFLOAT4*)&_11, V1);
    XMStoreFloat4((XMFLOAT4*)&_21, V2);
    XMStoreFloat4((XMFLOAT4*)&_31, V3);
    XMStoreFloat4((XMFLOAT4*)&_41, V4);

    return *this;
}

#endif // __cplusplus

#endif // __XMMATRIX_INL__
