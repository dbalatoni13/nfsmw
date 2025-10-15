/*++

Copyright (c) Microsoft Corporation

Module Name:

    xmmisc.inl

Abstract:

    Quaternion, plane, and color functions.

--*/

#pragma once

#ifndef __XMMISC_INL__
#define __XMMISC_INL__

/****************************************************************************
 *
 * Quaternion
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparison operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE BOOL XMQuaternionEqual
(
    XMVECTOR Q1,
    XMVECTOR Q2
)
{
    return XMVector4Equal(Q1, Q2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMQuaternionNotEqual
(
    XMVECTOR Q1,
    XMVECTOR Q2
)
{
    return XMVector4NotEqual(Q1, Q2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMQuaternionIsNaN
(
    XMVECTOR Q
)
{
    return XMVector4IsNaN(Q);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMQuaternionIsInfinite
(
    XMVECTOR Q
)
{
    return XMVector4IsInfinite(Q);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMQuaternionIsIdentity
(
    XMVECTOR Q
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    return XMVector4Equal(Q, g_XMIdentityR3);

#else

    XMVECTOR One;
    XMVECTOR Zero;
    XMVECTOR IdentityR3;

    One = __vspltisw(1);
    Zero = __vspltisw(0);

    One = __vcfsx(One, 0);
    IdentityR3 = __vsldoi(Zero, One, 1 << 2);

    return XMVector4Equal(Q, IdentityR3);

#endif
}

//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionDot
(
    XMVECTOR Q1,
    XMVECTOR Q2
)
{
    return XMVector4Dot(Q1, Q2);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionMultiply
(
    XMVECTOR Q1,
    XMVECTOR Q2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR         NegativeQ1;
    XMVECTOR         Q2X;
    XMVECTOR         Q2Y;
    XMVECTOR         Q2Z;
    XMVECTOR         Q2W;
    XMVECTOR         Q1WZYX;
    XMVECTOR         Q1ZWXY;
    XMVECTOR         Q1YXWZ;
    XMVECTOR         Result;
    CONST XMVECTORI  ControlWZYX = {XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    CONST XMVECTORI  ControlZWXY = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_1Y};
    CONST XMVECTORI  ControlYXWZ = {XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z};

    NegativeQ1 = XMVectorNegate(Q1);

    Q2W = XMVectorSplatW(Q2);
    Q2X = XMVectorSplatX(Q2);
    Q2Y = XMVectorSplatY(Q2);
    Q2Z = XMVectorSplatZ(Q2);

    Q1WZYX = XMVectorPermute(Q1, NegativeQ1, *(XMVECTOR*)ControlWZYX);
    Q1ZWXY = XMVectorPermute(Q1, NegativeQ1, *(XMVECTOR*)ControlZWXY);
    Q1YXWZ = XMVectorPermute(Q1, NegativeQ1, *(XMVECTOR*)ControlYXWZ);

    Result = XMVectorMultiply(Q1, Q2W);
    Result = XMVectorMultiplyAdd(Q1WZYX, Q2X, Result);
    Result = XMVectorMultiplyAdd(Q1ZWXY, Q2Y, Result);
    Result = XMVectorMultiplyAdd(Q1YXWZ, Q2Z, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                NegativeQ1;
    XMVECTOR                Q2X;
    XMVECTOR                Q2Y;
    XMVECTOR                Q2Z;
    XMVECTOR                Q2W;
    XMVECTOR                Q1WZYX;
    XMVECTOR                Q1ZWXY;
    XMVECTOR                Q1YXWZ;
    XMVECTOR                SignMask;
    XMVECTOR                Result;
    static CONST XMVECTORI  ControlWZYX = {XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static CONST XMVECTORI  ControlZWXY = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_1Y};
    static CONST XMVECTORI  ControlYXWZ = {XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    NegativeQ1 = __vxor(Q1, SignMask);

    Q2W = __vspltw(Q2, 3);
    Q2X = __vspltw(Q2, 0);
    Q2Y = __vspltw(Q2, 1);
    Q2Z = __vspltw(Q2, 2);

    Result = __vmaddfp(Q1, Q2W, *(XMVECTOR*)g_XMNegativeZero);

    Q1WZYX = __vperm(Q1, NegativeQ1, *(XMVECTOR*)ControlWZYX);
    Q1ZWXY = __vperm(Q1, NegativeQ1, *(XMVECTOR*)ControlZWXY);
    Q1YXWZ = __vperm(Q1, NegativeQ1, *(XMVECTOR*)ControlYXWZ);

    Result = __vmaddfp(Q1WZYX, Q2X, Result);
    Result = __vmaddfp(Q1ZWXY, Q2Y, Result);
    Result = __vmaddfp(Q1YXWZ, Q2Z, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Q10, Q11, Q12, Q13;
    XMVECTOR Q2WZYX, Q2ZWXY, Q2YXWZ;
    XMVECTOR RX, RY, RZ, RW, RXZ, RYW, Result;
    XMVECTOR SignMask;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Q2ZWXY = __vpermwi(Q2, 0xB1);
    Q2YXWZ = __vpermwi(Q2, 0x4E);
    Q2WZYX = __vpermwi(Q2, 0xE4);

    Q13 = __vxor(Q1, SignMask);
    Q11 = __vxor(Q1, SignMask);
    Q12 = __vxor(Q1, SignMask);
    Q10 = __vxor(Q1, SignMask);

    Q13 = __vrlimi(Q13, Q1, 0x1, 0);
    Q11 = __vrlimi(Q11, Q1, 0xD, 0);
    Q12 = __vrlimi(Q12, Q1, 0x7, 0);
    Q10 = __vrlimi(Q10, Q1, 0xB, 0);

    RW = __vmsum4fp(Q13, Q2);
    RY = __vmsum4fp(Q11, Q2ZWXY);
    RZ = __vmsum4fp(Q12, Q2YXWZ);
    RX = __vmsum4fp(Q10, Q2WZYX);

    RYW = __vmrghw(RY, RW);
    RXZ = __vmrghw(RX, RZ);
    Result = __vmrghw(RXZ, RYW);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionLengthSq
(
    XMVECTOR Q
)
{
    return XMVector4LengthSq(Q);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionReciprocalLength
(
    XMVECTOR Q
)
{
    return XMVector4ReciprocalLength(Q);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionLength
(
    XMVECTOR Q
)
{
    return XMVector4Length(Q);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionNormalizeEst
(
    XMVECTOR Q
)
{
    return XMVector4NormalizeEst(Q);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionNormalize
(
    XMVECTOR Q
)
{
    return XMVector4Normalize(Q);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionConjugate
(
    XMVECTOR Q
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVectorNegate(Q);
    Result = XMVectorSelect(Q, Result, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR SignMask;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Result = __vxor(Q, SignMask);
    Result = __vsel(Q, Result, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR SignMask;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Result = __vxor(Q, SignMask);
    Result = __vrlimi(Result, Q, 0x1, 0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionInverse
(
    XMVECTOR Q
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR        Conjugate;
    XMVECTOR        L;
    XMVECTOR        Control;
    XMVECTOR        Result;
    CONST XMVECTOR  Zero = XMVectorZero();

    L = XMVector4LengthSq(Q);
    Conjugate = XMQuaternionConjugate(Q);

    Control = XMVectorLessOrEqual(L, g_XMEpsilon);

    L = XMVectorReciprocal(L);
    Result = XMVectorMultiply(Conjugate, L);

    Result = XMVectorSelect(Result, Zero, Control);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Conjugate;
    XMVECTOR L;
    XMVECTOR LR;
    XMVECTOR Reciprocal;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR Control;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    XMVECTOR Result;

    D0 = __vmaddfp(Q, Q, *(XMVECTOR*)g_XMNegativeZero);

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    Zero = __vspltisw(0);

    Conjugate = __vxor(Q, SignMask);

    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);

    Conjugate = __vsel(Q, Conjugate, *(XMVECTOR*)g_XMSelect1110);

    D1 = __vsldoi(D0, D0, 4);
    L = __vaddfp(D0, D1);

    Reciprocal = __vrefp(L);
    Scale = __vnmsubfp(L, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);
    Scale = __vnmsubfp(L, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    LR = __vsel(Reciprocal, Rcp, Refine);

    Control = __vcmpgefp(g_XMEpsilon, L);

    Result = __vmaddfp(Conjugate, LR, *(XMVECTOR*)g_XMNegativeZero);

    Result = __vsel(Result, Zero, Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Conjugate;
    XMVECTOR L;
    XMVECTOR R;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR SignMask;
    XMVECTOR Shift;
    XMVECTOR Epsilon;
    XMVECTOR Result;
    XMVECTOR Zero;

    XMDUMMY_INITIALIZE_VECTOR(S1);

    L = __vmsum4fp(Q, Q);

    Epsilon = __vspltisw(13);
    Shift = __vspltisw(-6); // Shift 26
    Epsilon = __vslw(Epsilon, Shift);

    Result = __vcmpgefp(Epsilon, L);

    Reciprocal = __vrefp(L);

    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(L, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);

    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(L, S0, S1);

    SignMask = __vspltisw(-1);

    L = __vcmpeqfp(S0, S0);

    SignMask = __vslw(SignMask, SignMask);

    S0 = __vmaddfp(S0, S1, S0);

    Conjugate = __vxor(Q, SignMask);
    Conjugate = __vrlimi(Conjugate, Q, 0x1, 0);

    L = __vsel(Reciprocal, S0, L);

    R = __vmulfp(Conjugate, L);

    Zero = __vspltisw(0);

    Result = __vsel(R, Zero, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionLn
(
    XMVECTOR Q
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Q0;
    XMVECTOR QW;
    XMVECTOR Theta;
    XMVECTOR SinTheta;
    XMVECTOR S;
    XMVECTOR ControlW;
    XMVECTOR Result;
    static CONST XMVECTOR OneMinusEpsilon = {1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f};

    QW = XMVectorSplatW(Q);
    Q0 = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, Q, *(XMVECTOR*)g_XMSelect1110);

    ControlW = XMVectorInBounds(QW, OneMinusEpsilon);

    Theta = XMVectorACos(QW);
    SinTheta = XMVectorSin(Theta);

    S = XMVectorReciprocal(SinTheta);
    S = XMVectorMultiply(Theta, S);

    Result = XMVectorMultiply(Q0, S);

    Result = XMVectorSelect(Q0, Result, ControlW);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Q0;
    XMVECTOR QW;
    XMVECTOR Theta;
    XMVECTOR SinTheta;
    XMVECTOR S;
    XMVECTOR Reciprocal;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR ControlW;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTOR OneMinusEpsilon = {1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f};

    QW = __vspltw(Q, 3);
    Q0 = __vsel(*(XMVECTOR*)g_XMSelect1110, Q, *(XMVECTOR*)g_XMSelect1110);

    Zero = __vspltisw(0);

    Theta = XMVectorACos(QW);

    ControlW = __vcmpbfp(QW, OneMinusEpsilon);

    SinTheta = XMVectorSin(Theta);

    ControlW = __vcmpequw(ControlW, Zero);

    Reciprocal = __vrefp(SinTheta);
    Scale = __vnmsubfp(SinTheta, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);
    Scale = __vnmsubfp(SinTheta, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    S = __vsel(Reciprocal, Rcp, Refine);

    S = __vmaddfp(Theta, S, *(XMVECTOR*)g_XMNegativeZero);

    Result = __vmaddfp(Q0, S, *(XMVECTOR*)g_XMNegativeZero);

    Result = __vsel(Q0, Result, ControlW);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Q0;
    XMVECTOR QW;
    XMVECTOR Theta;
    XMVECTOR SinTheta;
    XMVECTOR S;
    XMVECTOR R;
    XMVECTOR Result;
    XMVECTOR Zero, One, Epsilon, OneMinusEpsilon;

    QW = __vspltw(Q, 3);

    Theta = XMVectorACos(QW);
    SinTheta = XMVectorSin(Theta);
    S = XMVectorReciprocal(SinTheta);

    Q0 = __vspltisw(0);
    One = __vspltisw(1);
    Epsilon = __vcfsx(One, 16);
    One = __vcfsx(One, 0);

    Q0 = __vrlimi(Q0, Q, 0xE, 0);

    S = __vmulfp(Theta, S);

    OneMinusEpsilon = __vsubfp(One, Epsilon);
    Zero = __vspltisw(0);

    R = __vmulfp(Q0, S);

    Result = __vcmpbfp(QW, OneMinusEpsilon);
    Result = __vcmpequw(Result, Zero);

    Result = __vsel(Q0, R, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionExp
(
    XMVECTOR Q
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Theta;
    XMVECTOR SinTheta;
    XMVECTOR CosTheta;
    XMVECTOR S;
    XMVECTOR Control;
    XMVECTOR Zero;
    XMVECTOR Result;

    Theta = XMVector3Length(Q);
    XMVectorSinCos(&SinTheta, &CosTheta, Theta);

    S = XMVectorReciprocal(Theta);
    S = XMVectorMultiply(SinTheta, S);

    Result = XMVectorMultiply(Q, S);

    Zero = XMVectorZero();
    Control = XMVectorNearEqual(Theta, Zero, g_XMEpsilon);
    Result = XMVectorSelect(Result, Q, Control);

    Result = XMVectorSelect(CosTheta, Result, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Theta;
    XMVECTOR SinTheta;
    XMVECTOR CosTheta;
    XMVECTOR S;
    XMVECTOR Control;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Zero;
    XMVECTOR Result;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, Q, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, Q, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Zero = __vspltisw(0);

    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    S = __vmaddfp(Rsq, RT, Rsq);

    InfiniteLength = __vcmpequw(D0, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(D0, Zero);
    Theta = __vmaddfp(D0, S, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Theta = __vsel(D0, Theta, Select);

    XMVectorSinCos(&SinTheta, &CosTheta, Theta);

    S = __vmaddfp(SinTheta, S, *(XMVECTOR*)g_XMNegativeZero);

    Control = __vcmpbfp(Theta, g_XMEpsilon);

    Result = __vmaddfp(Q, S, *(XMVECTOR*)g_XMNegativeZero);

    Control = __vcmpequw(Control, Zero);

    Result = __vsel(Result, Q, Control);

    Result = __vsel(CosTheta, Result, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Theta;
    XMVECTOR SinTheta;
    XMVECTOR CosTheta;
    XMVECTOR S;
    XMVECTOR R;
    XMVECTOR Shift;
    XMVECTOR Zero, Epsilon;
    XMVECTOR Result;

    Theta = XMVector3Length(Q);
    XMVectorSinCos(&SinTheta, &CosTheta, Theta);

    S = XMVectorReciprocal(Theta);
    S = __vmulfp(SinTheta, S);

    Epsilon = __vspltisw(13);
    Shift = __vspltisw(-6);

    Zero = __vspltisw(0);

    Epsilon = __vslw(Epsilon, Shift);

    R = __vmulfp(Q, S);

    Result = __vcmpbfp(Theta, Epsilon);
    Result = __vcmpequw(Result, Zero);

    Result = __vsel(R, Q, Result);

    Result = __vrlimi(Result, CosTheta, 0x1, 0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMQuaternionSlerp
(
    XMVECTOR Q0,
    XMVECTOR Q1,
    FLOAT    t
)
{
    XMVECTOR T;

    T.v[0] = t;
    T.v[1] = t;
    T.v[2] = t;
    T.v[3] = t;

    return XMQuaternionSlerpV(Q0, Q1, T);
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMQuaternionSlerpV
(
    XMVECTOR Q0,
    XMVECTOR Q1,
    XMVECTOR T
)
{
#if defined(_NO_INTRINSICS_)

    // Result = Q0 * sin((1.0 - t) * Omega) / sin(Omega) + Q1 * sin(t * Omega) / sin(Omega)
    XMVECTOR Omega;
    XMVECTOR CosOmega;
    XMVECTOR SinOmega;
    XMVECTOR InvSinOmega;
    XMVECTOR V01;
    XMVECTOR C1000;
    XMVECTOR SignMask;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR Sign;
    XMVECTOR Control;
    XMVECTOR Result;
    XMVECTOR Zero;
    CONST XMVECTOR OneMinusEpsilon = {1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f};

    XMASSERT((T.v[1] == T.v[0]) && (T.v[2] == T.v[0]) && (T.v[3] == T.v[0]));

    CosOmega = XMQuaternionDot(Q0, Q1);

    Zero = XMVectorZero();
    Control = XMVectorLess(CosOmega, Zero);
    Sign = XMVectorSelect(g_XMOne, g_XMNegativeOne, Control);

    CosOmega = XMVectorMultiply(CosOmega, Sign);

    Control = XMVectorLess(CosOmega, OneMinusEpsilon);

    SinOmega = XMVectorNegativeMultiplySubtract(CosOmega, CosOmega, g_XMOne);
    SinOmega = XMVectorSqrt(SinOmega);

    Omega = XMVectorATan2(SinOmega, CosOmega);

    SignMask = XMVectorSplatSignMask();
    C1000 = XMVectorSetBinaryConstant(1, 0, 0, 0);
    V01 = XMVectorShiftLeft(T, Zero, 2);
    SignMask = XMVectorShiftLeft(SignMask, Zero, 3);
    V01 = XMVectorXorInt(V01, SignMask);
    V01 = XMVectorAdd(C1000, V01);

    InvSinOmega = XMVectorReciprocal(SinOmega);

    S0 = XMVectorMultiply(V01, Omega);
    S0 = XMVectorSin(S0);
    S0 = XMVectorMultiply(S0, InvSinOmega);

    S0 = XMVectorSelect(V01, S0, Control);

    S1 = XMVectorSplatY(S0);
    S0 = XMVectorSplatX(S0);

    S1 = XMVectorMultiply(S1, Sign);

    Result = XMVectorMultiply(Q0, S0);
    Result = XMVectorMultiplyAdd(Q1, S1, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Omega;
    XMVECTOR CosOmega;
    XMVECTOR SinOmega;
    XMVECTOR InvSinOmega;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR V01;
    XMVECTOR One;
    XMVECTOR C1000;
    XMVECTOR SignMask;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR Sign;
    XMVECTOR Control;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;
    XMVECTOR Zero;
    static CONST XMVECTOR OneMinusEpsilon = {1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f};

    XMASSERT((T.v[1] == T.v[0]) && (T.v[2] == T.v[0]) && (T.v[3] == T.v[0]));

    XMDUMMY_INITIALIZE_VECTOR(One);

    D0 = __vmaddfp(Q0, Q1, *(XMVECTOR*)g_XMNegativeZero);

    Zero = __vspltisw(0);
    C1000 = __vspltisw(1);
    SignMask = __vspltisw(-1);
    One = __vspltisw(1);
    One = __vcfsx(One, 0);
    C1000 = __vcfsx(C1000, 0);
    SignMask = __vslw(SignMask, SignMask);
    C1000 = __vsldoi(One, Zero, 3 << 2);
    SignMask = __vsldoi(SignMask, Zero, 3 << 2);
    V01 = __vsldoi(T, Zero, 2 << 2);
    V01 = __vxor(V01, SignMask);
    V01 = __vaddfp(C1000, V01);

    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    CosOmega = __vaddfp(D0, D1);

    Control = __vcmpgtfp(Zero, CosOmega);
    Sign = __vsel(g_XMOne, g_XMNegativeOne, Control);

    CosOmega = __vmaddfp(CosOmega, Sign, *(XMVECTOR*)g_XMNegativeZero);

    SinOmega = __vnmsubfp(CosOmega, CosOmega, g_XMOne);

    Control = __vcmpgtfp(OneMinusEpsilon, CosOmega);

    Rsq = __vrsqrtefp(SinOmega);
    RT = __vmaddfp(SinOmega, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    InvSinOmega = __vmaddfp(Rsq, RT, Rsq);

    InfiniteLength = __vcmpequw(SinOmega, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(SinOmega, Zero);
    S0 = __vmaddfp(SinOmega, InvSinOmega, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    SinOmega = __vsel(SinOmega, S0, Select);

    Omega = XMVectorATan2(SinOmega, CosOmega);

    S0 = __vmaddfp(V01, Omega, *(XMVECTOR*)g_XMNegativeZero);

    S0 = XMVectorSin(S0);

    S0 = __vmaddfp(S0, InvSinOmega, *(XMVECTOR*)g_XMNegativeZero);

    S0 = __vsel(V01, S0, Control);

    S1 = __vspltw(S0, 1);
    S0 = __vspltw(S0, 0);

    S1 = __vmaddfp(S1, Sign, *(XMVECTOR*)g_XMNegativeZero);

    Result = __vmaddfp(Q0, S0, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(Q1, S1, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Omega;
    XMVECTOR CosOmega;
    XMVECTOR SinOmega;
    XMVECTOR InvSinOmega;
    XMVECTOR V01;
    XMVECTOR C1000;
    XMVECTOR SignMask;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR Sign;
    XMVECTOR Control;
    XMVECTOR Result;
    XMVECTOR Zero, One, NegativeOne, Epsilon, OneMinusEpsilon;

    XMASSERT((T.v[1] == T.v[0]) && (T.v[2] == T.v[0]) && (T.v[3] == T.v[0]));

    XMDUMMY_INITIALIZE_VECTOR(One);

    One = __vupkd3d(One, VPACK_NORMSHORT2);
    Zero = __vspltisw(0);
    Epsilon = __vspltisw(1);
    NegativeOne = __vspltisw(-1);

    CosOmega = __vmsum4fp(Q0, Q1);

    SignMask = __vslw(NegativeOne, NegativeOne);
    NegativeOne = __vcfsx(NegativeOne, 0);
    C1000 = __vpermwi(One, 0xEA);
    One = __vspltw(One, 3);
    Epsilon = __vcfsx(Epsilon, 16);

    Sign = __vcmpgtfp(Zero, CosOmega);
    Sign = __vsel(Zero, SignMask, Sign);

    OneMinusEpsilon = __vsubfp(One, Epsilon);
    CosOmega = __vxor(CosOmega, Sign);

    Control = __vcmpgtfp(OneMinusEpsilon, CosOmega);

    One = __vnmsubfp(CosOmega, CosOmega, One);

    SignMask = __vsldoi(SignMask, Zero, 3 << 2);
    V01 = __vsldoi(T, Zero, 2 << 2);
    V01 = __vxor(V01, SignMask);

    SinOmega = XMVectorSqrt(One);

    V01 = __vaddfp(C1000, V01);

    Omega = XMVectorATan2(SinOmega, CosOmega);

    InvSinOmega = XMVectorReciprocal(SinOmega);

    S0 = __vmulfp(V01, Omega);
    S0 = XMVectorSin(S0);
    S0 = __vmulfp(S0, InvSinOmega);

    Control = __vsel(V01, S0, Control);

    S1 = __vspltw(Control, 1);
    S0 = __vspltw(Control, 0);

    S1 = __vxor(S1, Sign);

    Result = __vmulfp(Q0, S0);
    Result = __vmaddfp(Q1, S1, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionSquad
(
    XMVECTOR Q0,
    XMVECTOR Q1,
    XMVECTOR Q2,
    XMVECTOR Q3,
    FLOAT    t
)
{
    XMVECTOR T;

    T.v[0] =
    T.v[1] =
    T.v[2] =
    T.v[3] = t;

    return XMQuaternionSquadV(Q0, Q1, Q2, Q3, T);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionSquadV
(
    XMVECTOR Q0,
    XMVECTOR Q1,
    XMVECTOR Q2,
    XMVECTOR Q3,
    XMVECTOR T
)
{
    XMVECTOR Q03;
    XMVECTOR Q12;
    XMVECTOR TP;
    XMVECTOR Two;
    XMVECTOR Result;

    XMASSERT((T.v[1] == T.v[0]) && (T.v[2] == T.v[0]) && (T.v[3] == T.v[0]));

    TP = T;
    Two = XMVectorSplatConstant(2, 0);

    Q03 = XMQuaternionSlerpV(Q0, Q3, T);
    Q12 = XMQuaternionSlerpV(Q1, Q2, T);

    TP = XMVectorNegativeMultiplySubtract(TP, TP, TP);
    TP = XMVectorMultiply(TP, Two);

    Result = XMQuaternionSlerpV(Q03, Q12, TP);

    return Result;

}

//------------------------------------------------------------------------------

XMINLINE VOID XMQuaternionSquadSetup
(
    XMVECTOR* pA,
    XMVECTOR* pB,
    XMVECTOR* pC,
    XMVECTOR  Q0,
    XMVECTOR  Q1,
    XMVECTOR  Q2,
    XMVECTOR  Q3
)
{
    XMVECTOR SQ0, SQ2, SQ3;
    XMVECTOR InvQ1, InvQ2;
    XMVECTOR LnQ0, LnQ1, LnQ2, LnQ3;
    XMVECTOR ExpQ02, ExpQ13;
    XMVECTOR LS01, LS12, LS23;
    XMVECTOR LD01, LD12, LD23;
    XMVECTOR Control0, Control1, Control2;
    XMVECTOR NegativeOneQuarter;

    XMASSERT(pA);
    XMASSERT(pB);
    XMASSERT(pC);

    LS12 = XMQuaternionLengthSq(XMVectorAdd(Q1, Q2));
    LD12 = XMQuaternionLengthSq(XMVectorSubtract(Q1, Q2));
    SQ2 = XMVectorNegate(Q2);

    Control1 = XMVectorLess(LS12, LD12);
    SQ2 = XMVectorSelect(Q2, SQ2, Control1);

    LS01 = XMQuaternionLengthSq(XMVectorAdd(Q0, Q1));
    LD01 = XMQuaternionLengthSq(XMVectorSubtract(Q0, Q1));
    SQ0 = XMVectorNegate(Q0);

    LS23 = XMQuaternionLengthSq(XMVectorAdd(SQ2, Q3));
    LD23 = XMQuaternionLengthSq(XMVectorSubtract(SQ2, Q3));
    SQ3 = XMVectorNegate(Q3);

    Control0 = XMVectorLess(LS01, LD01);
    Control2 = XMVectorLess(LS23, LD23);

    SQ0 = XMVectorSelect(Q0, SQ0, Control0);
    SQ3 = XMVectorSelect(Q3, SQ3, Control2);

    InvQ1 = XMQuaternionInverse(Q1);
    InvQ2 = XMQuaternionInverse(SQ2);

    LnQ0 = XMQuaternionLn(XMQuaternionMultiply(InvQ1, SQ0));
    LnQ2 = XMQuaternionLn(XMQuaternionMultiply(InvQ1, SQ2));
    LnQ1 = XMQuaternionLn(XMQuaternionMultiply(InvQ2, Q1));
    LnQ3 = XMQuaternionLn(XMQuaternionMultiply(InvQ2, SQ3));

    NegativeOneQuarter = XMVectorSplatConstant(-1, 2);

    ExpQ02 = XMVectorMultiply(XMVectorAdd(LnQ0, LnQ2), NegativeOneQuarter);
    ExpQ13 = XMVectorMultiply(XMVectorAdd(LnQ1, LnQ3), NegativeOneQuarter);
    ExpQ02 = XMQuaternionExp(ExpQ02);
    ExpQ13 = XMQuaternionExp(ExpQ13);

    *pA = XMQuaternionMultiply(Q1, ExpQ02);
    *pB = XMQuaternionMultiply(SQ2, ExpQ13);
    *pC = SQ2;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionBaryCentric
(
    XMVECTOR Q0,
    XMVECTOR Q1,
    XMVECTOR Q2,
    FLOAT    f,
    FLOAT    g
)
{
    XMVECTOR Q01;
    XMVECTOR Q02;
    FLOAT    s;
    XMVECTOR Result;

    s = f + g;

    if (s < 0.00001f && s > -0.00001f)
    {
        Result = Q0;
    }
    else
    {
        Q01 = XMQuaternionSlerp(Q0, Q1, s);
        Q02 = XMQuaternionSlerp(Q0, Q2, s);

        Result = XMQuaternionSlerp(Q01, Q02, g / s);
    }

    return Result;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionBaryCentricV
(
    XMVECTOR Q0,
    XMVECTOR Q1,
    XMVECTOR Q2,
    XMVECTOR F,
    XMVECTOR G
)
{
    XMVECTOR Q01;
    XMVECTOR Q02;
    XMVECTOR S, GS;
    XMVECTOR Epsilon;
    XMVECTOR Result;

    XMASSERT((F.v[1] == F.v[0]) && (F.v[2] == F.v[0]) && (F.v[3] == F.v[0]));
    XMASSERT((G.v[1] == G.v[0]) && (G.v[2] == G.v[0]) && (G.v[3] == G.v[0]));

    Epsilon = XMVectorSplatConstant(1, 16);

    S = XMVectorAdd(F, G);

    if (XMVector4InBounds(S, Epsilon))
    {
        Result = Q0;
    }
    else
    {
        Q01 = XMQuaternionSlerpV(Q0, Q1, S);
        Q02 = XMQuaternionSlerpV(Q0, Q2, S);
        GS = XMVectorReciprocal(S);
        GS = XMVectorMultiply(G, GS);

        Result = XMQuaternionSlerpV(Q01, Q02, GS);
    }

    return Result;
}

//------------------------------------------------------------------------------
// Transformation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionIdentity()
{
#if defined(_NO_INTRINSICS_)

    return g_XMIdentityR3;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR One;
    XMVECTOR Zero;
    XMVECTOR Q;

    One = __vspltisw(1);
    Zero = __vspltisw(0);

    One = __vcfsx(One, 0);
    Q = __vsldoi(Zero, One, 1 << 2);

    return Q;

#else // _VMX128_INTRINSICS_

    XMVECTOR Q;

    XMDUMMY_INITIALIZE_VECTOR(Q);

    Q = __vupkd3d(Q, VPACK_NORMSHORT2);
    Q = __vpermwi(Q, 0xAB);

    return Q;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionRotationRollPitchYaw
(
    FLOAT Pitch,
    FLOAT Yaw,
    FLOAT Roll
)
{
    XMVECTOR Angles;
    XMVECTOR Q;

    Angles = XMVectorSet(Pitch, Yaw, Roll, 0.0f);
    Q = XMQuaternionRotationRollPitchYawFromVector(Angles);

    return Q;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionRotationRollPitchYawFromVector
(
    XMVECTOR Angles // <Pitch, Yaw, Roll, 0>
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR                Q, Q0, Q1;
    XMVECTOR                P0, P1, Y0, Y1, R0, R1;
    XMVECTOR                HalfAngles;
    XMVECTOR                SinAngles, CosAngles;
    static CONST XMVECTORI  ControlPitch = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1X, XM_PERMUTE_1X};
    static CONST XMVECTORI  ControlYaw = {XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1Y};
    static CONST XMVECTORI  ControlRoll = {XM_PERMUTE_1Z, XM_PERMUTE_1Z, XM_PERMUTE_0Z, XM_PERMUTE_1Z};
    static CONST XMVECTOR   Sign = {1.0f, -1.0f, -1.0f, 1.0f};

    HalfAngles = XMVectorMultiply(Angles, g_XMOneHalf);
    XMVectorSinCos(&SinAngles, &CosAngles, HalfAngles);

    P0 = XMVectorPermute(SinAngles, CosAngles, *(XMVECTOR*)ControlPitch);
    Y0 = XMVectorPermute(SinAngles, CosAngles, *(XMVECTOR*)ControlYaw);
    R0 = XMVectorPermute(SinAngles, CosAngles, *(XMVECTOR*)ControlRoll);
    P1 = XMVectorPermute(CosAngles, SinAngles, *(XMVECTOR*)ControlPitch);
    Y1 = XMVectorPermute(CosAngles, SinAngles, *(XMVECTOR*)ControlYaw);
    R1 = XMVectorPermute(CosAngles, SinAngles, *(XMVECTOR*)ControlRoll);

    Q1 = XMVectorMultiply(P1, Sign);
    Q0 = XMVectorMultiply(P0, Y0);
    Q1 = XMVectorMultiply(Q1, Y1);
    Q0 = XMVectorMultiply(Q0, R0);
    Q = XMVectorMultiplyAdd(Q1, R1, Q0);

    return Q;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                Q, Q0, Q1;
    XMVECTOR                P0, P1, Y0, Y1, R0, R1;
    XMVECTOR                HalfAngles;
    XMVECTOR                SinAngles, CosAngles;
    static CONST XMVECTORI  ControlPitch = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1X, XM_PERMUTE_1X};
    static CONST XMVECTORI  ControlYaw = {XM_PERMUTE_1Y, XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1Y};
    static CONST XMVECTORI  ControlRoll = {XM_PERMUTE_1Z, XM_PERMUTE_1Z, XM_PERMUTE_0Z, XM_PERMUTE_1Z};
    static CONST XMVECTOR   Sign = {1.0f, -1.0f, -1.0f, 1.0f};

    HalfAngles = __vmaddfp(Angles, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);

    XMVectorSinCos(&SinAngles, &CosAngles, HalfAngles);

    P0 = __vperm(SinAngles, CosAngles, *(XMVECTOR*)ControlPitch);
    Y0 = __vperm(SinAngles, CosAngles, *(XMVECTOR*)ControlYaw);
    R0 = __vperm(SinAngles, CosAngles, *(XMVECTOR*)ControlRoll);
    P1 = __vperm(CosAngles, SinAngles, *(XMVECTOR*)ControlPitch);
    Y1 = __vperm(CosAngles, SinAngles, *(XMVECTOR*)ControlYaw);
    R1 = __vperm(CosAngles, SinAngles, *(XMVECTOR*)ControlRoll);

    Q1 = __vmaddfp(P1, Sign, *(XMVECTOR*)g_XMNegativeZero);
    Q0 = __vmaddfp(P0, Y0, *(XMVECTOR*)g_XMNegativeZero);
    Q1 = __vmaddfp(Q1, Y1, *(XMVECTOR*)g_XMNegativeZero);
    Q0 = __vmaddfp(Q0, R0, *(XMVECTOR*)g_XMNegativeZero);
    Q = __vmaddfp(Q1, R1, Q0);

    return Q;

#else // _VMX128_INTRINSICS_

    XMVECTOR Q0, Q1;
    XMVECTOR P0, P1, Y0, Y1, R0, R1;
    XMVECTOR HalfAngles;
    XMVECTOR SinAngles, CosAngles;
    XMVECTOR OneHalf;
    XMVECTOR SignMask, Zero;

    OneHalf = __vspltisw(1);
    OneHalf = __vcfsx(OneHalf, 1);

    HalfAngles = __vmulfp(Angles, OneHalf);

    XMVectorSinCos(&SinAngles, &CosAngles, HalfAngles);

    SignMask = __vspltisw(-1);
    Zero = __vspltisw(0);
    SignMask = __vslw(SignMask, SignMask);
    SignMask = __vrlimi(SignMask, Zero, 0x9, 0);

    P0 = __vspltw(CosAngles, 0);
    Y0 = __vspltw(CosAngles, 1);
    R0 = __vspltw(CosAngles, 2);
    P1 = __vspltw(SinAngles, 0);
    Y1 = __vspltw(SinAngles, 1);
    R1 = __vspltw(SinAngles, 2);

    P0 = __vrlimi(P0, SinAngles, 0x8, 0);
    Y0 = __vrlimi(Y0, SinAngles, 0x4, 0);
    R0 = __vrlimi(R0, SinAngles, 0x2, 0);
    P1 = __vrlimi(P1, CosAngles, 0x8, 0);
    Y1 = __vrlimi(Y1, CosAngles, 0x4, 0);
    R1 = __vrlimi(R1, CosAngles, 0x2, 0);

    Q1 = __vxor(P1, SignMask);
    Q0 = __vmulfp(P0, Y0);
    Q1 = __vmulfp(Q1, Y1);
    Q0 = __vmulfp(Q0, R0);
    Q0 = __vmaddfp(Q1, R1, Q0);

    return Q0;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionRotationNormal
(
    XMVECTOR NormalAxis,
    FLOAT    Angle
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Q;
    XMVECTOR N;
    XMVECTOR Scale;

    N = XMVectorSelect(g_XMOne, NormalAxis, *(XMVECTOR*)g_XMSelect1110);

    XMScalarSinCos(&Scale.v[2], &Scale.v[3], 0.5f * Angle);

    Scale.v[0] = Scale.v[1] = Scale.v[2];

    Q = XMVectorMultiply(N, Scale);

    return Q;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Q;
    XMVECTOR N;
    XMVECTOR Scale;

    N = __vsel(g_XMOne, NormalAxis, *(XMVECTOR*)g_XMSelect1110);

    XMScalarSinCos(&Scale.v[2], &Scale.v[3], 0.5f * Angle);

    Scale.v[0] = Scale.v[1] = Scale.v[2];

    Q = __vmaddfp(N, Scale, *(XMVECTOR*)g_XMNegativeZero);

    return Q;

#else // _VMX128_INTRINSICS_

    XMVECTOR Q;
    XMVECTOR N;
    XMVECTOR Scale;

    XMDUMMY_INITIALIZE_VECTOR(N);

    N = __vupkd3d(N, VPACK_NORMSHORT2);

    XMScalarSinCos(&Scale.v[2], &Scale.v[3], 0.5f * Angle);

    N = __vrlimi(N, NormalAxis, 0xE, 0);

    Scale.v[0] = Scale.v[1] = Scale.v[2];

    Q = __vmulfp(N, Scale);

    return Q;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMQuaternionRotationAxis
(
    XMVECTOR Axis,
    FLOAT    Angle
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Normal;
    XMVECTOR Q;

    XMASSERT(!XMVector3Equal(Axis, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(Axis));

    Normal = XMVector3Normalize(Axis);
    Q = XMQuaternionRotationNormal(Normal, Angle);

    return Q;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Q;
    XMVECTOR NormalAxis;
    XMVECTOR N;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Scale;

    XMASSERT(!XMVector3Equal(Axis, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(Axis));

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, Axis, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);

    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);

    XMScalarSinCos(&Scale.v[2], &Scale.v[3], 0.5f * Angle);
    Scale.v[0] = Scale.v[1] = Scale.v[2];

    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);

    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D0 = __vmaddfp(Rsq, RT, Rsq);

    NormalAxis = __vmaddfp(Axis, D0, *(XMVECTOR*)g_XMNegativeZero);

    N = __vsel(g_XMOne, NormalAxis, *(XMVECTOR*)g_XMSelect1110);

    Q = __vmaddfp(N, Scale, *(XMVECTOR*)g_XMNegativeZero);

    return Q;

#else // _VMX128_INTRINSICS_

    XMVECTOR Normal;
    XMVECTOR D;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Zero;
    XMVECTOR H;
    XMVECTOR N, NA;
    XMVECTOR Scale, Scale2;
    XMVECTOR Q;

    XMASSERT(!XMVector3Equal(Axis, XMVectorZero()));
    XMASSERT(!XMVector3IsInfinite(Axis));

    XMDUMMY_INITIALIZE_VECTOR(NA);

    H = __vspltisw(1);
    D = __vmsum3fp(Axis, Axis);
    H = __vcfsx(H, 1);
    Zero = __vspltisw(0);
    RcpLength = __vrsqrtefp(D);

    XMScalarSinCos(&Scale2.v[2], &Scale2.v[3], 0.5f * Angle);

    OneHalfLengthSq = __vmulfp(D, H);
    Normal = __vcmpeqfp(D, Zero);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddcfp(RcpLength, H, RcpLength);

    NA = __vupkd3d(NA, VPACK_NORMSHORT2);
    Scale2.v[0] = Scale2.v[1] = Scale2.v[2];

    N = __vmulfp(Axis, H);
    Normal = __vsel(N, D, Normal);

    NA = __vrlimi(NA, Normal, 0xE, 0);

    Q = __vmulfp(NA, Scale2);

    return Q;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMQuaternionRotationMatrix
(
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMVECTOR Q0, Q1, Q2;
    XMVECTOR M00, M11, M22;
    XMVECTOR CQ0, CQ1, C;
    XMVECTOR CX, CY, CZ, CW;
    XMVECTOR SQ1, Scale;
    XMVECTOR Rsq, Sqrt, VEqualsInfinity, VEqualsZero, Select;
    XMVECTOR A, B, P;
    XMVECTOR PermuteSplat, PermuteSplatT;
    XMVECTOR SignB, SignBT;
    XMVECTOR PermuteControl, PermuteControlT;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTOR  OneQuarter = {0.25f, 0.25f, 0.25f, 0.25f};
    static CONST XMVECTOR  SignPNNP = {1.0f, -1.0f, -1.0f, 1.0f};
    static CONST XMVECTOR  SignNPNP = {-1.0f, 1.0f, -1.0f, 1.0f};
    static CONST XMVECTOR  SignNNPP = {-1.0f, -1.0f, 1.0f, 1.0f};
    static CONST XMVECTOR  SignPNPP = {1.0f, -1.0f, 1.0f, 1.0f};
    static CONST XMVECTOR  SignPPNP = {1.0f, 1.0f, -1.0f, 1.0f};
    static CONST XMVECTOR  SignNPPP = {-1.0f, 1.0f, 1.0f, 1.0f};
    static CONST XMVECTOR  SignNNNX = {-1.0f, -1.0f, -1.0f, 2.0e-126f};
    static CONST XMVECTORI Permute0X0X0Y0W = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute0Y0Z0Z1W = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_1W};
    static CONST XMVECTORI SplatX = {XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X};
    static CONST XMVECTORI SplatY = {XM_PERMUTE_0Y, XM_PERMUTE_0Y, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
    static CONST XMVECTORI SplatZ = {XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Z};
    static CONST XMVECTORI SplatW = {XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0W};
    static CONST XMVECTORI PermuteC = {XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Y};
    static CONST XMVECTORI PermuteA = {XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1Z, XM_PERMUTE_0W};
    static CONST XMVECTORI PermuteB = {XM_PERMUTE_1X, XM_PERMUTE_1W, XM_PERMUTE_0Z, XM_PERMUTE_0W};
    static CONST XMVECTORI Permute0 = {XM_PERMUTE_0X, XM_PERMUTE_1X, XM_PERMUTE_1Z, XM_PERMUTE_1Y};
    static CONST XMVECTORI Permute1 = {XM_PERMUTE_1X, XM_PERMUTE_0Y, XM_PERMUTE_1Y, XM_PERMUTE_1Z};
    static CONST XMVECTORI Permute2 = {XM_PERMUTE_1Z, XM_PERMUTE_1Y, XM_PERMUTE_0Z, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute3 = {XM_PERMUTE_1Y, XM_PERMUTE_1Z, XM_PERMUTE_1X, XM_PERMUTE_0W};

    M00 = XMVectorSplatX(M.r[0]);
    M11 = XMVectorSplatY(M.r[1]);
    M22 = XMVectorSplatZ(M.r[2]);

    Q0 = XMVectorMultiply(SignPNNP, M00);
    Q0 = XMVectorMultiplyAdd(SignNPNP, M11, Q0);
    Q0 = XMVectorMultiplyAdd(SignNNPP, M22, Q0);

    Q1 = XMVectorAdd(Q0, g_XMOne);

    Rsq = XMVectorReciprocalSqrt(Q1);
    Zero = XMVectorZero();
    VEqualsInfinity = XMVectorEqualInt(Q1, *(XMVECTOR*)g_XMInfinity);
    VEqualsZero = XMVectorEqual(Q1, Zero);
    Sqrt = XMVectorMultiply(Q1, Rsq);
    Select = XMVectorEqualInt(VEqualsInfinity, VEqualsZero);
    Q1 = XMVectorSelect(Q1, Sqrt, Select);

    Q1 = XMVectorMultiply(Q1, g_XMOneHalf);

    SQ1 = XMVectorMultiply(Rsq, g_XMOneHalf);

    CQ0 = XMVectorPermute(Q0, Q0, *(XMVECTOR*)Permute0X0X0Y0W);
    CQ1 = XMVectorPermute(Q0, SignNNNX, *(XMVECTOR*)Permute0Y0Z0Z1W);
    C = XMVectorGreaterOrEqual(CQ0, CQ1);

    CX = XMVectorSplatX(C);
    CY = XMVectorSplatY(C);
    CZ = XMVectorSplatZ(C);
    CW = XMVectorSplatW(C);

    PermuteSplat = XMVectorSelect(*(XMVECTOR*)SplatZ, *(XMVECTOR*)SplatY, CZ);
    SignB = XMVectorSelect(SignNPPP, SignPPNP, CZ);
    PermuteControl = XMVectorSelect(*(XMVECTOR*)Permute2, *(XMVECTOR*)Permute1, CZ);

    PermuteSplat = XMVectorSelect(PermuteSplat, *(XMVECTOR*)SplatZ, CX);
    SignB = XMVectorSelect(SignB, SignNPPP, CX);
    PermuteControl = XMVectorSelect(PermuteControl, *(XMVECTOR*)Permute2, CX);

    PermuteSplatT = XMVectorSelect(PermuteSplat, *(XMVECTOR*)SplatX, CY);
    SignBT = XMVectorSelect(SignB, SignPNPP, CY);
    PermuteControlT = XMVectorSelect(PermuteControl, *(XMVECTOR*)Permute0, CY);

    PermuteSplat = XMVectorSelect(PermuteSplat, PermuteSplatT, CX);
    SignB = XMVectorSelect(SignB, SignBT, CX);
    PermuteControl = XMVectorSelect(PermuteControl, PermuteControlT, CX);

    PermuteSplat = XMVectorSelect(PermuteSplat, *(XMVECTOR*)SplatW, CW);
    SignB = XMVectorSelect(SignB, SignNNNX, CW);
    PermuteControl = XMVectorSelect(PermuteControl, *(XMVECTOR*)Permute3, CW);

    Scale = XMVectorPermute(SQ1, SQ1, PermuteSplat);

    P = XMVectorPermute(M.r[1], M.r[2], *(XMVECTOR*)PermuteC);  // {M10, M12, M20, M21}
    A = XMVectorPermute(M.r[0], P, *(XMVECTOR*)PermuteA);       // {M01, M12, M20, M03}
    B = XMVectorPermute(M.r[0], P, *(XMVECTOR*)PermuteB);       // {M10, M21, M02, M03}

    Q2 = XMVectorMultiplyAdd(SignB, B, A);
    Q2 = XMVectorMultiply(Q2, Scale);

    Result = XMVectorPermute(Q1, Q2, PermuteControl);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Q0, Q1, Q2;
    XMVECTOR M00, M11, M22;
    XMVECTOR CQ0, CQ1, C;
    XMVECTOR SQ1, Scale;
    XMVECTOR A, B, P;
    XMVECTOR Sign;
    XMVECTOR PermuteControl;
    XMVECTOR Shift;
    XMVECTOR OneHalfV;
    XMVECTOR Reciprocal, Rsq, Root;
    XMVECTOR RsqNotNaN;
    XMVECTOR ScaleNotNaN;
    XMVECTOR Select;
    XMVECTOR OneHalf, One, NegativeOne, Epsilon;
    XMVECTOR SplatX, SplatY, SplatZ, SplatW;
    XMVECTOR Permute0X0Y0Z0W, Permute0, Permute1, Permute2, Permute3;
    XMVECTOR SignPNNP, SignNPNP, SignNNPP, SignPNPP, SignPPNP, SignNPPP;
    XMVECTOR S00, S01, S02, S03, S10, S11, S12, S13, S20, S21, S22, S23;
    XMVECTOR Result;

    One = __vspltisw(1);
    NegativeOne = __vspltisw(-1);
    Shift = __vspltisw(-9);
    Epsilon = __vslw(One, Shift);
    OneHalf = __vcfsx(One, 1);
    One = __vcfsx(One, 0);
    NegativeOne = __vcfsx(NegativeOne, 0);

    SignNNPP = __vsldoi(NegativeOne, One, 2 << 2);
    SignPNNP = __vpermwi(SignNNPP, 0xC3);

    M00 = __vspltw(M.r[0], 0);
    M11 = __vspltw(M.r[1], 1);
    M22 = __vspltw(M.r[2], 2);

    Q0 = __vmulfp(SignPNNP, M00);

    SignNPNP = __vpermwi(SignNNPP, 0x33);

    Q0 = __vmaddfp(SignNPNP, M11, Q0);

    Q0 = __vmaddfp(SignNNPP, M22, Q0);

    Q1 = __vaddfp(Q0, One);

    Scale = __vspltisw(1);
    Rsq = __vrsqrtefp(Q1);
    Scale = __vcfsx(Scale, 1);
    OneHalfV = __vmulfp(Q1, Scale);
    Reciprocal = __vmulfp(Rsq, Rsq);
    RsqNotNaN = __vcmpeqfp(Rsq, Rsq);
    Scale = __vnmsubfp(OneHalfV, Reciprocal, Scale);
    Rsq = __vmaddfp(Rsq, Scale, Rsq);
    ScaleNotNaN = __vcmpeqfp(Scale, Scale);
    Select = __vxor(ScaleNotNaN, RsqNotNaN);

    Permute0X0Y0Z0W = __lvsl(0, 0);
    Permute0 = __lvsl(0, 0xC); // Permute0W1X1Y1Z
    Permute1 = __lvsl(0, 0xC);
    Permute2 = __lvsl(0, 0xC);
    SplatX = __vspltw(Permute0X0Y0Z0W, 0);
    SplatY = __vspltw(Permute0X0Y0Z0W, 1);
    SplatZ = __vspltw(Permute0X0Y0Z0W, 2);
    SplatW = __vspltw(Permute0X0Y0Z0W, 3);

    Root = __vmulfp(Q1, Rsq);

    Permute3 = __vpermwi(Permute0, 0xB4);
    Permute0 = __vrlimi(Permute0, Permute0X0Y0Z0W, 0x8, 0); // 0X1X1Y1Z
    Permute1 = __vrlimi(Permute1, Permute0X0Y0Z0W, 0x8, 1); // 0Y1X1Y1Z
    Permute2 = __vrlimi(Permute2, Permute0X0Y0Z0W, 0x8, 2); // 0Z1X1Y1Z
    Permute0 = __vpermwi(Permute0, 0x1E);
    Permute1 = __vpermwi(Permute1, 0x4B);
    Permute2 = __vpermwi(Permute2, 0xE1);

    SignNPPP = __vsldoi(NegativeOne, One, 3 << 2);
    SignPNPP = __vpermwi(SignNNPP, 0xCF);
    SignPPNP = __vpermwi(SignNNPP, 0xF3);

    Select = __vsel(Root, Q1, Select);

    Q1 = __vmulfp(Select, OneHalf);

    SQ1 = __vmulfp(Rsq, OneHalf);

    CQ0 = __vpermwi(Q0, 0x7);

    CQ1 = __vpermwi(Q0, 0x6B);
    CQ1 = __vrlimi(CQ1, Epsilon, 0x1, 0);

    C = __vcmpgefp(CQ0, CQ1);

    S00 = __vspltw(C, 2);
    S10 = __vspltw(C, 2);
    S20 = __vspltw(C, 2);

    S00 = __vsel(SplatZ, SplatY, S00);
    S10 = __vsel(Permute2, Permute1, S10);
    S20 = __vsel(SignNPPP, SignPPNP, S20);

    S01 = __vspltw(C, 0);
    S11 = __vspltw(C, 0);
    S21 = __vspltw(C, 0);

    S01 = __vsel(S00, SplatZ, S01);
    S11 = __vsel(S10, Permute2, S11);
    S21 = __vsel(S20, SignNPPP, S21);

    S02 = __vspltw(C, 1);
    S12 = __vspltw(C, 1);
    S22 = __vspltw(C, 1);

    S02 = __vsel(S01, SplatX, S02);
    S12 = __vsel(S11, Permute0, S12);
    S22 = __vsel(S21, SignPNPP, S22);

    S03 = __vspltw(C, 0);
    S13 = __vspltw(C, 0);
    S23 = __vspltw(C, 0);
    Sign = __vspltw(C, 3);
    Scale = __vspltw(C, 3);
    PermuteControl = __vspltw(C, 3);

    S03 = __vsel(S01, S02, S03);
    S13 = __vsel(S11, S12, S13);
    S23 = __vsel(S21, S22, S23);
    Sign = __vsel(S23, NegativeOne, Sign);
    Scale = __vsel(S03, SplatW, Scale);
    PermuteControl = __vsel(S13, Permute3, PermuteControl);

    Scale = __vperm(SQ1, SQ1, Scale);

    P = __vpermwi(M.r[1], 0x20);
    P = __vrlimi(P, M.r[2], 0x3, 2);

    A = __vpermwi(M.r[0], 0x43);
    A = __vrlimi(A, P, 0x6, 0);

    B = __vpermwi(P, 0x30);
    B = __vrlimi(B, M.r[0], 0x3, 0);

    A = __vmaddfp(Sign, B, A);
    Q2 = __vmulfp(A, Scale);

    Result = __vperm(Q1, Q2, PermuteControl);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Conversion operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE VOID XMQuaternionToAxisAngle
(
    XMVECTOR* pAxis,
    FLOAT*    pAngle,
    XMVECTOR  Q
)
{
    XMASSERT(pAxis);
    XMASSERT(pAngle);

    *pAxis = Q;

    *pAngle = 2.0f * XMScalarACos(Q.v[3]);
}

/****************************************************************************
 *
 * Plane
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparison operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE BOOL XMPlaneEqual
(
    XMVECTOR P1,
    XMVECTOR P2
)
{
    return XMVector4Equal(P1, P2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMPlaneNearEqual
(
    XMVECTOR P1,
    XMVECTOR P2,
    XMVECTOR Epsilon
)
{
    XMVECTOR NP1, NP2;
    NP1 = XMPlaneNormalize(P1);
    NP2 = XMPlaneNormalize(P2);
    return XMVector4NearEqual(NP1, NP2, Epsilon);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMPlaneNotEqual
(
    XMVECTOR P1,
    XMVECTOR P2
)
{
    return XMVector4NotEqual(P1, P2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMPlaneIsNaN
(
    XMVECTOR P
)
{
    return XMVector4IsNaN(P);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMPlaneIsInfinite
(
    XMVECTOR P
)
{
    return XMVector4IsInfinite(P);
}

//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMPlaneDot
(
    XMVECTOR P,
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return XMVector4Dot(P, V);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(P, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return Result;

#else // _VMX128_INTRINSICS_

    return __vmsum4fp(P, V);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMPlaneDotCoord
(
    XMVECTOR P,
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V3;
    XMVECTOR Result;

    // Result = P[0] * V[0] + P[1] * V[1] + P[2] * V[2] + P[3]
    V3 = XMVectorSelect(g_XMOne, V, *(XMVECTOR*)g_XMSelect1110);
    Result = XMVector4Dot(P, V3);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V3;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    V3 = __vsel(g_XMOne, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(P, V3, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V3;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(V3);

    V3 = __vupkd3d(V3, VPACK_NORMSHORT2);
    V3 = __vspltw(V3, 3);
    V3 = __vrlimi(V3, V, 0xE, 0);
    Result = __vmsum4fp(P, V3);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMPlaneDotNormal
(
    XMVECTOR P,
    XMVECTOR V
)
{
    return XMVector3Dot(P, V);
}

//------------------------------------------------------------------------------
// XMPlaneNormalizeEst uses a reciprocal estimate and
// returns QNaN on zero and infinite vectors.

XMFINLINE XMVECTOR XMPlaneNormalizeEst
(
    XMVECTOR P
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;
    Result = XMVector3ReciprocalLength(P);
    Result = XMVectorMultiply(P, Result);
    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, P, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);
    Result = __vrsqrtefp(Result);
    Result = __vmaddfp(P, Result, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR H;
    XMVECTOR Result;

    D = __vmsum3fp(P, P);
    H = __vspltisw(1);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);
    Result = __vmulfp(P, Rsq);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMPlaneNormalize
(
    XMVECTOR P
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR LengthSq;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR Result;

    LengthSq = XMVector3LengthSq(P);
    Zero = XMVectorZero();
    Result = XMVectorReciprocalSqrt(LengthSq);
    InfiniteLength = XMVectorEqualInt(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = XMVectorEqual(LengthSq, Zero);
    Result = XMVectorMultiply(P, Result);
    Select = XMVectorEqualInt(InfiniteLength, ZeroLength);
    Result = XMVectorSelect(LengthSq, Result, Select);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Zero;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR Result;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, P, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Zero = __vspltisw(0);
    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    Result = __vmaddfp(Rsq, RT, Rsq);
    InfiniteLength = __vcmpequw(D0, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(D0, Zero);
    Result = __vmaddfp(P, Result, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Result = __vsel(D0, Result, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR LengthSq;
    XMVECTOR OneHalfV;
    XMVECTOR Reciprocal;
    XMVECTOR Scale;
    XMVECTOR Zero;
    XMVECTOR Select;
    XMVECTOR Result;

    LengthSq = __vmsum3fp(P, P);
    Scale = __vspltisw(1);
    Result = __vrsqrtefp(LengthSq);
    Scale = __vcfsx(Scale, 1);
    OneHalfV = __vmulfp(LengthSq, Scale);
    Reciprocal = __vmulfp(Result, Result);
    Scale = __vnmsubfp(OneHalfV, Reciprocal, Scale);
    Result = __vmaddfp(Result, Scale, Result);
    Zero = __vspltisw(0);
    Result = __vmulfp(P, Result);
    Select = __vcmpeqfp(LengthSq, Zero);
    Select = __vsel(Result, LengthSq, Select);

    return Select;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMPlaneIntersectLine
(
    XMVECTOR P,
    XMVECTOR LinePoint1,
    XMVECTOR LinePoint2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1;
    XMVECTOR V2;
    XMVECTOR D;
    XMVECTOR ReciprocalD;
    XMVECTOR VT;
    XMVECTOR Point;
    XMVECTOR Zero;
    XMVECTOR Control;
    XMVECTOR Result;

    V1 = XMVector3Dot(P, LinePoint1);
    V2 = XMVector3Dot(P, LinePoint2);
    D = XMVectorSubtract(V1, V2);

    ReciprocalD = XMVectorReciprocal(D);
    VT = XMPlaneDotCoord(P, LinePoint1);
    VT = XMVectorMultiply(VT, ReciprocalD);

    Point = XMVectorSubtract(LinePoint2, LinePoint1);
    Point = XMVectorMultiplyAdd(Point, VT, LinePoint1);

    Zero = XMVectorZero();
    Control = XMVectorNearEqual(D, Zero, g_XMEpsilon);

    Result = XMVectorSelect(Point, *(XMVECTOR*)g_XMQNaN, Control);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1;
    XMVECTOR V2;
    XMVECTOR D;
    XMVECTOR ReciprocalD;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR VT;
    XMVECTOR Point;
    XMVECTOR Control;
    XMVECTOR Zero;
    XMVECTOR PM;
    XMVECTOR D0A;
    XMVECTOR D0B;
    XMVECTOR D1A;
    XMVECTOR D1B;
    XMVECTOR PW;
    XMVECTOR Result;

    D0A = __vsel(*(XMVECTOR*)g_XMSelect1110, LinePoint1, *(XMVECTOR*)g_XMSelect1110);
    D0B = __vsel(*(XMVECTOR*)g_XMSelect1110, LinePoint2, *(XMVECTOR*)g_XMSelect1110);
    PM = __vsel(*(XMVECTOR*)g_XMSelect1110, P, *(XMVECTOR*)g_XMSelect1110);
    D0A = __vmaddfp(PM, D0A, *(XMVECTOR*)g_XMNegativeZero);
    D0B = __vmaddfp(PM, D0B, *(XMVECTOR*)g_XMNegativeZero);
    PW = __vspltw(P, 3);
    Point = __vsubfp(LinePoint2, LinePoint1);

    D1A = __vsldoi(D0A, D0A, 8);
    D1B = __vsldoi(D0B, D0B, 8);

    D0A = __vaddfp(D0A, D1A);
    D0B = __vaddfp(D0B, D1B);

    D1A = __vsldoi(D0A, D0A, 4);
    D1B = __vsldoi(D0B, D0B, 4);

    V1 = __vaddfp(D0A, D1A);
    V2 = __vaddfp(D0B, D1B);

    Zero = __vspltisw(0);

    D = __vsubfp(V1, V2);

    VT = __vaddfp(V1, PW);

    ReciprocalD = __vrefp(D);
    Scale = __vnmsubfp(D, ReciprocalD, g_XMOne);
    Rcp = __vmaddfp(ReciprocalD, Scale, ReciprocalD);
    Scale = __vnmsubfp(D, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    ReciprocalD = __vsel(ReciprocalD, Rcp, Refine);

    Control = __vcmpbfp(D, g_XMEpsilon);

    VT = __vmaddfp(VT, ReciprocalD, *(XMVECTOR*)g_XMNegativeZero);

    Control = __vcmpequw(Control, Zero);

    Point = __vmaddfp(Point, VT, LinePoint1);

    Result = __vsel(Point, *(XMVECTOR*)g_XMQNaN, Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V1;
    XMVECTOR V2;
    XMVECTOR D;
    XMVECTOR ReciprocalD;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR VT;
    XMVECTOR V3;
    XMVECTOR Point;
    XMVECTOR Zero, QNaN, Epsilon;
    XMVECTOR ShiftL, ShiftR;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(S1);

    V1 = __vmsum3fp(P, LinePoint1);
    V2 = __vmsum3fp(P, LinePoint2);
    D = __vsubfp(V1, V2);

    Reciprocal = __vrefp(D);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(D, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    V3 = __vspltw(S1, 3);
    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(D, S0, S1);
    ReciprocalD = __vcmpeqfp(S0, S0);
    V3 = __vrlimi(V3, LinePoint1, 0xE, 0);
    S0 = __vmaddfp(S0, S1, S0);
    VT = __vmsum4fp(P, V3);

    Epsilon = __vspltisw(13);
    ShiftL = __vspltisw(-6);
    Epsilon = __vslw(Epsilon, ShiftL);

    ReciprocalD = __vsel(Reciprocal, S0, ReciprocalD);

    VT = __vmulfp(VT, ReciprocalD);

    Point = __vsubfp(LinePoint2, LinePoint1);

    Zero = __vspltisw(0);
    Result = __vcmpbfp(D, Epsilon);

    QNaN = __vspltisw(-1);
    ShiftL = __vspltisw(-9);
    ShiftR = __vspltisw(1);

    Result = __vcmpequw(Result, Zero);

    QNaN = __vslw(QNaN, ShiftL);

    Point = __vmaddcfp(VT, Point, LinePoint1);

    QNaN = __vsrw(QNaN, ShiftR);

    Result = __vsel(Point, QNaN, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE VOID XMPlaneIntersectPlane
(
    XMVECTOR* pLinePoint1,
    XMVECTOR* pLinePoint2,
    XMVECTOR  P1,
    XMVECTOR  P2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1;
    XMVECTOR V2;
    XMVECTOR V3;
    XMVECTOR LengthSq;
    XMVECTOR RcpLengthSq;
    XMVECTOR Point;
    XMVECTOR P1W;
    XMVECTOR P2W;
    XMVECTOR Control;
    XMVECTOR LinePoint1;
    XMVECTOR LinePoint2;

    XMASSERT(pLinePoint1);
    XMASSERT(pLinePoint2);

    V1 = XMVector3Cross(P2, P1);

    LengthSq = XMVector3LengthSq(V1);

    V2 = XMVector3Cross(P2, V1);

    P1W = XMVectorSplatW(P1);
    Point = XMVectorMultiply(V2, P1W);

    V3 = XMVector3Cross(V1, P1);

    P2W = XMVectorSplatW(P2);
    Point = XMVectorMultiplyAdd(V3, P2W, Point);

    RcpLengthSq = XMVectorReciprocal(LengthSq);
    LinePoint1 = XMVectorMultiply(Point, RcpLengthSq);

    LinePoint2 = XMVectorAdd(LinePoint1, V1);

    Control = XMVectorLessOrEqual(LengthSq, g_XMEpsilon);
    *pLinePoint1 = XMVectorSelect(LinePoint1, *(XMVECTOR*)g_XMQNaN, Control);
    *pLinePoint2 = XMVectorSelect(LinePoint2, *(XMVECTOR*)g_XMQNaN, Control);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1;
    XMVECTOR V2;
    XMVECTOR V3;
    XMVECTOR LengthSq;
    XMVECTOR RcpLengthSq;
    XMVECTOR Reciprocal;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR Point;
    XMVECTOR P1W;
    XMVECTOR P2W;
    XMVECTOR Control;
    XMVECTOR LinePoint1;
    XMVECTOR LinePoint2;
    XMVECTOR CrossP1A;
    XMVECTOR CrossP1B;
    XMVECTOR CrossP2A;
    XMVECTOR CrossP2B;
    XMVECTOR CrossV1A;
    XMVECTOR CrossV1B;
    XMVECTOR CrossR1;
    XMVECTOR CrossR2;
    XMVECTOR D0;
    XMVECTOR D1;

    XMASSERT(pLinePoint1);
    XMASSERT(pLinePoint2);

    CrossP2A = __vperm(P2, P2, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossP1A = __vperm(P1, P1, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossP2B = __vperm(P2, P2, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossP1B = __vperm(P1, P1, *(XMVECTOR*)g_XMSwizzleYZXW);

    CrossR1 = __vmaddfp(CrossP2A, CrossP1A, *(XMVECTOR*)g_XMNegativeZero);

    P1W = __vspltw(P1, 3);
    P2W = __vspltw(P2, 3);

    V1 = __vnmsubfp(CrossP2B, CrossP1B, CrossR1);

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V1, *(XMVECTOR*)g_XMSelect1110);

    CrossV1A = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV1B = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleYZXW);

    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);

    CrossR1 = __vmaddfp(CrossV1A, CrossP2A, *(XMVECTOR*)g_XMNegativeZero);
    CrossR2 = __vmaddfp(CrossV1B, CrossP1A, *(XMVECTOR*)g_XMNegativeZero);

    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);

    V2 = __vnmsubfp(CrossV1B, CrossP2B, CrossR1);
    V3 = __vnmsubfp(CrossV1A, CrossP1B, CrossR2);

    D1 = __vsldoi(D0, D0, 4);
    LengthSq = __vaddfp(D0, D1);

    Point = __vmaddfp(V2, P1W, *(XMVECTOR*)g_XMNegativeZero);

    Reciprocal = __vrefp(LengthSq);

    Control = __vcmpgefp(g_XMEpsilon, LengthSq);

    Scale = __vnmsubfp(LengthSq, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);

    Point = __vmaddfp(V3, P2W, Point);

    Scale = __vnmsubfp(LengthSq, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    RcpLengthSq = __vsel(Reciprocal, Rcp, Refine);

    LinePoint1 = __vmaddfp(Point, RcpLengthSq, *(XMVECTOR*)g_XMNegativeZero);

    LinePoint2 = __vaddfp(LinePoint1, V1);

    *pLinePoint1 = __vsel(LinePoint1, *(XMVECTOR*)g_XMQNaN, Control);
    *pLinePoint2 = __vsel(LinePoint2, *(XMVECTOR*)g_XMQNaN, Control);

#else // _VMX128_INTRINSICS_

    XMVECTOR V1;
    XMVECTOR V2;
    XMVECTOR V3;
    XMVECTOR LengthSq;
    XMVECTOR RcpLengthSq;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR Point;
    XMVECTOR P1W;
    XMVECTOR P2W;
    XMVECTOR Epsilon, QNaN;
    XMVECTOR ShiftLE, ShiftLQ, ShiftR;
    XMVECTOR CrossV1A, CrossV1B, CrossV1C, CrossV1D;
    XMVECTOR CrossV2A, CrossV2B, CrossV2C, CrossV2D;
    XMVECTOR LinePoint1;
    XMVECTOR LinePoint2;

    XMASSERT(pLinePoint1);
    XMASSERT(pLinePoint2);

    XMDUMMY_INITIALIZE_VECTOR(S1);

    CrossV1C = __vpermwi(P2, 0x63);
    CrossV2C = __vpermwi(P1, 0x87);
    CrossV1D = __vpermwi(P2, 0x87);
    CrossV2D = __vpermwi(P1, 0x63);
    V1 = __vmulfp(CrossV1C, CrossV2C);
    V1 = __vnmsubfp(CrossV1D, CrossV2D, V1);

    CrossV2A = __vpermwi(V1, 0x87);
    CrossV1A = __vpermwi(V1, 0x63);
    LengthSq = __vmsum3fp(V1, V1);
    V2 = __vmulfp(CrossV1C, CrossV2A);
    V3 = __vmulfp(CrossV1A, CrossV2C);
    CrossV2B = __vpermwi(V1, 0x63);
    CrossV1B = __vpermwi(V1, 0x87);
    V2 = __vnmsubfp(CrossV1D, CrossV2B, V2);
    V3 = __vnmsubfp(CrossV1B, CrossV2D, V3);

    Reciprocal = __vrefp(LengthSq);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(LengthSq, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(S1, 3);
    P1W = __vspltw(P1, 3);
    S1 = __vnmsubfp(LengthSq, S0, S1);
    RcpLengthSq = __vcmpeqfp(S0, S0);
    Point = __vmulfp(V2, P1W);
    P2W = __vspltw(P2, 3);
    S0 = __vmaddfp(S0, S1, S0);
    Point = __vmaddfp(V3, P2W, Point);
    RcpLengthSq = __vsel(Reciprocal, S0, RcpLengthSq);

    LinePoint1 = __vmulfp(Point, RcpLengthSq);

    Epsilon = __vspltisw(13);
    ShiftLE = __vspltisw(-6);

    QNaN = __vspltisw(-1);
    ShiftLQ = __vspltisw(-9);
    ShiftR = __vspltisw(1);

    LinePoint2 = __vaddfp(LinePoint1, V1);

    Epsilon = __vslw(Epsilon, ShiftLE);

    QNaN = __vslw(QNaN, ShiftLQ);
    QNaN = __vsrw(QNaN, ShiftR);

    *pLinePoint1 = __vcmpgefp(Epsilon, LengthSq);
    *pLinePoint2 = __vcmpgefp(Epsilon, LengthSq);
    *pLinePoint1 = __vsel(LinePoint1, QNaN, *pLinePoint1);
    *pLinePoint2 = __vsel(LinePoint2, QNaN, *pLinePoint2);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMPlaneTransform
(
    XMVECTOR P,
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR W;
    XMVECTOR Result;

    W = XMVectorSplatW(P);
    Z = XMVectorSplatZ(P);
    Y = XMVectorSplatY(P);
    X = XMVectorSplatX(P);

    Result = XMVectorMultiply(W, M.r[3]);
    Result = XMVectorMultiplyAdd(Z, M.r[2], Result);
    Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
    Result = XMVectorMultiplyAdd(X, M.r[0], Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR W;
    XMVECTOR Result;

    W = __vspltw(P, 3);
    Z = __vspltw(P, 2);
    Y = __vspltw(P, 1);
    X = __vspltw(P, 0);

    Result = __vmaddfp(W, M.r[3], *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(Z, M.r[2], Result);
    Result = __vmaddfp(Y, M.r[1], Result);
    Result = __vmaddfp(X, M.r[0], Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMMATRIX PT, MT;
    XMVECTOR R0, R1, R2, R3, R0R2, R1R3;
    XMVECTOR Result;

    PT.r[0] = __vmrghw(M.r[0], M.r[2]);
    PT.r[1] = __vmrghw(M.r[1], M.r[3]);
    PT.r[2] = __vmrglw(M.r[0], M.r[2]);
    PT.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(PT.r[0], PT.r[1]);
    MT.r[1] = __vmrglw(PT.r[0], PT.r[1]);
    MT.r[2] = __vmrghw(PT.r[2], PT.r[3]);
    MT.r[3] = __vmrglw(PT.r[2], PT.r[3]);

    R0 = __vmsum4fp(P, MT.r[0]);
    R1 = __vmsum4fp(P, MT.r[1]);
    R2 = __vmsum4fp(P, MT.r[2]);
    R3 = __vmsum4fp(P, MT.r[3]);

    R0R2 = __vmrghw(R0, R2);
    R1R3 = __vmrghw(R1, R3);

    Result = __vmrghw(R0R2, R1R3);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT4* XMPlaneTransformStream
(
    XMFLOAT4*       pOutputStream,
    UINT            OutputStride,
    CONST XMFLOAT4* pInputStream,
    UINT            InputStride,
    UINT            PlaneCount,
    XMMATRIX        M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR P;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR W;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < PlaneCount; i++)
    {
        P = XMLoadFloat4((XMFLOAT4*)pInputVector);
        W = XMVectorSplatW(P);
        Z = XMVectorSplatZ(P);
        Y = XMVectorSplatY(P);
        X = XMVectorSplatX(P);
//        W = XMVectorReplicate(((XMFLOAT4*)pInputVector)->w);
//        Z = XMVectorReplicate(((XMFLOAT4*)pInputVector)->z);
//        Y = XMVectorReplicate(((XMFLOAT4*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT4*)pInputVector)->x);

        Result = XMVectorMultiply(W, M.r[3]);
        Result = XMVectorMultiplyAdd(Z, M.r[2], Result);
        Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat4((XMFLOAT4*)pOutputVector, Result);

        pInputVector += InputStride;
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR P;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR W;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < PlaneCount; i++)
    {
        P = XMLoadFloat4((XMFLOAT4*)pInputVector);
        W = XMVectorSplatW(P);
        Z = XMVectorSplatZ(P);
        Y = XMVectorSplatY(P);
        X = XMVectorSplatX(P);
//        W = XMVectorReplicate(((XMFLOAT4*)pInputVector)->w);
//        Z = XMVectorReplicate(((XMFLOAT4*)pInputVector)->z);
//        Y = XMVectorReplicate(((XMFLOAT4*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT4*)pInputVector)->x);

        Result = XMVectorMultiply(W, M.r[3]);
        Result = XMVectorMultiplyAdd(Z, M.r[2], Result);
        Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat4((XMFLOAT4*)pOutputVector, Result);

        pInputVector += InputStride;
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMVECTOR P;
    XMMATRIX PT, MT;
    XMVECTOR R0, R1, R2, R3, R0R2, R1R3;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    PT.r[0] = __vmrghw(M.r[0], M.r[2]);
    PT.r[1] = __vmrghw(M.r[1], M.r[3]);
    PT.r[2] = __vmrglw(M.r[0], M.r[2]);
    PT.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(PT.r[0], PT.r[1]);
    MT.r[1] = __vmrglw(PT.r[0], PT.r[1]);
    MT.r[2] = __vmrghw(PT.r[2], PT.r[3]);
    MT.r[3] = __vmrglw(PT.r[2], PT.r[3]);

    for (i = 0; i < PlaneCount; i++)
    {
        P = XMLoadFloat4((XMFLOAT4*)pInputVector);

        R0 = __vmsum4fp(P, MT.r[0]);
        R1 = __vmsum4fp(P, MT.r[1]);
        R2 = __vmsum4fp(P, MT.r[2]);
        R3 = __vmsum4fp(P, MT.r[3]);

        R0R2 = __vmrghw(R0, R2);
        R1R3 = __vmrghw(R1, R3);

        Result = __vmrghw(R0R2, R1R3);

        XMStoreFloat4((XMFLOAT4*)pOutputVector, Result);

        pInputVector += InputStride;
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Conversion operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMPlaneFromPointNormal
(
    XMVECTOR Point,
    XMVECTOR Normal
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR W;
    XMVECTOR Result;

    W = XMVector3Dot(Point, Normal);
    W = XMVectorNegate(W);
    Result = XMVectorSelect(W, Normal, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR W;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vsel(*(XMVECTOR*)g_XMSelect1110, Point, *(XMVECTOR*)g_XMSelect1110);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, Normal, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vnmsubfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    W = __vaddfp(D0, D1);

    Result = __vsel(W, Normal, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR SignMask;
    XMVECTOR Result;

    Result = __vmsum3fp(Point, Normal);
    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    Result = __vxor(Result, SignMask);
    Result = __vrlimi(Result, Normal, 0xE, 0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMPlaneFromPoints
(
    XMVECTOR Point1,
    XMVECTOR Point2,
    XMVECTOR Point3
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR N;
    XMVECTOR D;
    XMVECTOR V21;
    XMVECTOR V31;
    XMVECTOR Result;

    V21 = XMVectorSubtract(Point1, Point2);
    V31 = XMVectorSubtract(Point1, Point3);

    N = XMVector3Cross(V21, V31);
    N = XMVector3Normalize(N);

    D = XMPlaneDotNormal(N, Point1);
    D = XMVectorNegate(D);

    Result = XMVectorSelect(D, N, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR N;
    XMVECTOR D;
    XMVECTOR V21;
    XMVECTOR V31;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR CrossV1A;
    XMVECTOR CrossV1B;
    XMVECTOR CrossV2A;
    XMVECTOR CrossV2B;
    XMVECTOR CrossR;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    V21 = __vsubfp(Point1, Point2);
    V31 = __vsubfp(Point1, Point3);

    CrossV1A = __vperm(V21, V21, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossV2A = __vperm(V31, V31, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV1B = __vperm(V21, V21, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV2B = __vperm(V31, V31, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossR = __vmaddfp(CrossV1A, CrossV2A, *(XMVECTOR*)g_XMNegativeZero);
    N = __vnmsubfp(CrossV1B, CrossV2B, CrossR);

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, N, *(XMVECTOR*)g_XMSelect1110);
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

    N = __vmaddfp(N, D0, *(XMVECTOR*)g_XMNegativeZero);

    D0 = __vsel(*(XMVECTOR*)g_XMSelect1110, N, *(XMVECTOR*)g_XMSelect1110);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, Point1, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vnmsubfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D = __vaddfp(D0, D1);

    Result = __vsel(D, N, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR N;
    XMVECTOR V21;
    XMVECTOR V31;
    XMVECTOR CrossV1;
    XMVECTOR CrossV2;
    XMVECTOR D;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Zero;
    XMVECTOR H;
    XMVECTOR NR, NC;
    XMVECTOR SignMask;
    XMVECTOR Result;

    V21 = __vsubfp(Point1, Point2);
    V31 = __vsubfp(Point1, Point3);

    CrossV1 = __vpermwi(V21, 0x63);
    CrossV2 = __vpermwi(V31, 0x87);
    N = __vmulfp(CrossV1, CrossV2);
    CrossV1 = __vpermwi(V21, 0x87);
    CrossV2 = __vpermwi(V31, 0x63);
    N = __vnmsubfp(CrossV1, CrossV2, N);

    H = __vspltisw(1);
    D = __vmsum3fp(N, N);
    H = __vcfsx(H, 1);
    Zero = __vspltisw(0);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    NC = __vcmpeqfp(D, Zero);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddcfp(RcpLength, H, RcpLength);
    NR = __vmulfp(N, H);
    NC = __vsel(NR, D, NC);

    Result = __vmsum3fp(NC, Point1);

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    Result = __vxor(Result, SignMask);

    Result = __vrlimi(Result, NC, 0xE, 0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

/****************************************************************************
 *
 * Color
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparison operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE BOOL XMColorEqual
(
    XMVECTOR C1,
    XMVECTOR C2
)
{
    return XMVector4Equal(C1, C2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMColorNotEqual
(
    XMVECTOR C1,
    XMVECTOR C2
)
{
    return XMVector4NotEqual(C1, C2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMColorGreater
(
    XMVECTOR C1,
    XMVECTOR C2
)
{
    return XMVector4Greater(C1, C2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMColorGreaterOrEqual
(
    XMVECTOR C1,
    XMVECTOR C2
)
{
    return XMVector4GreaterOrEqual(C1, C2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMColorLess
(
    XMVECTOR C1,
    XMVECTOR C2
)
{
    return XMVector4Less(C1, C2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMColorLessOrEqual
(
    XMVECTOR C1,
    XMVECTOR C2
)
{
    return XMVector4LessOrEqual(C1, C2);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMColorIsNaN
(
    XMVECTOR C
)
{
    return XMVector4IsNaN(C);
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMColorIsInfinite
(
    XMVECTOR C
)
{
    return XMVector4IsInfinite(C);
}

//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMColorNegative
(
    XMVECTOR C
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

//    XMASSERT(XMVector4GreaterOrEqual(C, XMVectorReplicate(0.0f)));
//    XMASSERT(XMVector4LessOrEqual(C, XMVectorReplicate(1.0f)));

    Result.v[0] = 1.0f - C.v[0];
    Result.v[1] = 1.0f - C.v[1];
    Result.v[2] = 1.0f - C.v[2];
    Result.v[3] = C.v[3];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Negative;
    XMVECTOR Result;

//    XMASSERT(XMVector4GreaterOrEqual(C, XMVectorReplicate(0.0f)));
//    XMASSERT(XMVector4LessOrEqual(C, XMVectorReplicate(1.0f)));

    Negative = __vsubfp(g_XMOne, C);
    Result = __vsel(C, Negative, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Negative;
    XMVECTOR One;
    XMVECTOR Result;

//    XMASSERT(XMVector4GreaterOrEqual(C, XMVectorReplicate(0.0f)));
//    XMASSERT(XMVector4LessOrEqual(C, XMVectorReplicate(1.0f)));

    XMDUMMY_INITIALIZE_VECTOR(One);

    One = __vupkd3d(One, VPACK_NORMSHORT2);
    One = __vspltw(One, 3);
    Negative = __vsubfp(One, C);
    Result = __vrlimi(Negative, C, 0x1, 0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMColorModulate
(
    XMVECTOR C1,
    XMVECTOR C2
)
{
    return XMVectorMultiply(C1, C2);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMColorAdjustSaturation
(
    XMVECTOR C,
    FLOAT    Saturation
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR       S;
    XMVECTOR       L;
    XMVECTOR       Result;
    CONST XMVECTOR Luminance = XMVectorSet(0.2125f, 0.7154f, 0.0721f, 0.0f);

    // Luminance = 0.2125f * C[0] + 0.7154f * C[1] + 0.0721f * C[2];
    // Result = (C - Luminance) * Saturation + Luminance;

    L = XMVector3Dot(C, Luminance);
    S = XMVectorReplicate(Saturation);
    Result = XMVectorSubtract(C, L);
    Result = XMVectorMultiplyAdd(Result, S, L);
    Result.v[3] = C.v[3];

    return Result;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               S;
    XMVECTOR               L;
    XMVECTOR               Result;
    XMVECTOR               D0;
    XMVECTOR               D1;
    static CONST XMVECTOR  Luminance = {0.2125f, 0.7154f, 0.0721f, 0.0f};

#if defined(_VMX32_INTRINSICS_)
    D0 = __vmaddfp(C, Luminance, *(XMVECTOR*)g_XMNegativeZero);
#else
    D0 = __vmulfp(C, Luminance);
#endif
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    L = __vaddfp(D0, D1);
    S.v[0] =
    S.v[1] =
    S.v[2] =
    S.v[3] = Saturation;
    Result = __vsubfp(C, L);
    Result = __vmaddfp(Result, S, L);
#if defined(_VMX32_INTRINSICS_)
    Result = __vsel(C, Result, *(XMVECTOR*)g_XMSelect1110);
#else
    Result = __vrlimi(Result, C, 0x1, 0);
#endif

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR              S;
    XMVECTOR              L;
    XMVECTOR              Result;
    static CONST XMVECTOR Luminance = {0.2125f, 0.7154f, 0.0721f, 0.0f};

    XMASSERT(((UINT_PTR)&Saturation & 3) == 0);

    L = __vmsum3fp(C, Luminance);
    S = __lvlx(&Saturation, 0);
    S = __vspltw(S, 0);
    Result = __vsubfp(C, L);
    Result = __vmaddcfp(S, Result, L);
    Result = __vrlimi(Result, C, 0x1, 0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMColorAdjustContrast
(
    XMVECTOR C,
    FLOAT    Contrast
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Scale;
    XMVECTOR Result;

    // Result = (C.x - 0.5f) * Contrast + 0.5f;
    Scale = XMVectorReplicate(Contrast);
    Result = XMVectorSubtract(C, g_XMOneHalf);
    Result = XMVectorMultiplyAdd(Result, Scale, g_XMOneHalf);
    Result.v[3] = C.v[3];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Scale;
    XMVECTOR Result;

    Result = __vsubfp(C, g_XMOneHalf);
    Scale.v[0] =
    Scale.v[1] =
    Scale.v[2] =
    Scale.v[3] = Contrast;
    Result = __vmaddfp(Result, Scale, g_XMOneHalf);
    Result = __vsel(C, Result, *(XMVECTOR*)g_XMSelect1110);

    return Result;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR Scale;
    XMVECTOR OneHalf;
    XMVECTOR Result;

    OneHalf = __vspltisw(1);
    OneHalf = __vcfsx(OneHalf, 1);
    Result = __vsubfp(C, OneHalf);
    Scale.v[0] =
    Scale.v[1] =
    Scale.v[2] =
    Scale.v[3] = Contrast;
    Result = __vmaddcfp(Result, Scale, OneHalf);
    Result = __vrlimi(Result, C, 0x1, 0);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Scale;
    XMVECTOR OneHalf;
    XMVECTOR Result;

    XMASSERT(((UINT_PTR)&Contrast & 3) == 0);

    OneHalf = __vspltisw(1);
    OneHalf = __vcfsx(OneHalf, 1);
    Scale = __lvlx(&Contrast, 0);
    Scale = __vspltw(Scale, 0);
    Result = __vsubfp(C, OneHalf);
    Result = __vmaddcfp(Scale, Result, OneHalf);
    Result = __vrlimi(Result, C, 0x1, 0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

/****************************************************************************
 *
 * Miscellaneous
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

#define XMASSERT_LINE_STRING_SIZE 16

XMINLINE VOID XMAssert
(
    CONST CHAR* pExpression,
    CONST CHAR* pFileName,
    UINT        LineNumber
)
{
    CHAR        aLineString[XMASSERT_LINE_STRING_SIZE];
    CHAR*       pLineString;
    UINT        Line;

    aLineString[XMASSERT_LINE_STRING_SIZE - 2] = '0';
    aLineString[XMASSERT_LINE_STRING_SIZE - 1] = '\0';
    for (Line = LineNumber, pLineString = aLineString + XMASSERT_LINE_STRING_SIZE - 2;
         Line != 0 && pLineString >= aLineString;
         Line /= 10, pLineString--)
    {
        *pLineString = (CHAR)('0' + (Line % 10));
    }

#ifndef NO_OUTPUT_DEBUG_STRING
    OutputDebugStringA("Assertion failed: ");
    OutputDebugStringA(pExpression);
    OutputDebugStringA(", file ");
    OutputDebugStringA(pFileName);
    OutputDebugStringA(", line ");
    OutputDebugStringA(pLineString + 1);
    OutputDebugStringA("\r\n");
#else
    DbgPrint("Assertion failed: %s, file %s, line %d\r\n", pExpression, pFileName, LineNumber);
#endif

    __debugbreak();
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMFresnelTerm
(
    XMVECTOR CosIncidentAngle,
    XMVECTOR RefractionIndex
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR G;
    XMVECTOR D, S;
    XMVECTOR V0, V1, V2, V3;
    XMVECTOR Result;

    // Result = 0.5f * (g - c)^2 / (g + c)^2 * ((c * (g + c) - 1)^2 / (c * (g - c) + 1)^2 + 1) where
    // c = CosIncidentAngle
    // g = sqrt(c^2 + RefractionIndex^2 - 1)

    XMASSERT(!XMVector4IsInfinite(CosIncidentAngle));

    G = XMVectorMultiplyAdd(RefractionIndex, RefractionIndex, g_XMNegativeOne);
    G = XMVectorMultiplyAdd(CosIncidentAngle, CosIncidentAngle, G);
    G = XMVectorAbs(G);
    G = XMVectorSqrt(G);

    S = XMVectorAdd(G, CosIncidentAngle);
    D = XMVectorSubtract(G, CosIncidentAngle);

    V0 = XMVectorMultiply(D, D);
    V1 = XMVectorMultiply(S, S);
    V1 = XMVectorReciprocal(V1);
    V0 = XMVectorMultiply(g_XMOneHalf, V0);
    V0 = XMVectorMultiply(V0, V1);

    V2 = XMVectorMultiplyAdd(CosIncidentAngle, S, g_XMNegativeOne);
    V3 = XMVectorMultiplyAdd(CosIncidentAngle, D, g_XMOne);
    V2 = XMVectorMultiply(V2, V2);
    V3 = XMVectorMultiply(V3, V3);
    V3 = XMVectorReciprocal(V3);
    V2 = XMVectorMultiplyAdd(V2, V3, g_XMOne);

    Result = XMVectorMultiply(V0, V2);

    Result = XMVectorSaturate(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR G;
    XMVECTOR D, S;
    XMVECTOR V0, V1, V2, V3;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    XMVECTOR ZeroLength;
    XMVECTOR Result;

    XMASSERT(!XMVector4IsInfinite(CosIncidentAngle));

    G = __vmaddfp(RefractionIndex, RefractionIndex, g_XMNegativeOne);
    SignMask = __vspltisw(-1);
    Zero = __vspltisw(0);
    G = __vmaddfp(CosIncidentAngle, CosIncidentAngle, G);
    SignMask = __vslw(SignMask, SignMask);
    G = __vandc(G, SignMask);
    S = XMVectorReciprocalSqrt(G);
    ZeroLength = __vcmpeqfp(G, Zero);
    S = __vmaddfp(G, S, *(XMVECTOR*)g_XMNegativeZero);
    G = __vsel(S, G, ZeroLength);

    S = __vaddfp(G, CosIncidentAngle);
    D = __vsubfp(G, CosIncidentAngle);

    V1 = __vmaddfp(S, S, *(XMVECTOR*)g_XMNegativeZero);
    V3 = __vmaddfp(CosIncidentAngle, D, g_XMOne);
    V0 = __vmaddfp(D, D, *(XMVECTOR*)g_XMNegativeZero);
    V2 = __vmaddfp(CosIncidentAngle, S, g_XMNegativeOne);
    V1 = XMVectorReciprocal(V1);
    V3 = __vmaddfp(V3, V3, *(XMVECTOR*)g_XMNegativeZero);
    V0 = __vmaddfp(g_XMOneHalf, V0, *(XMVECTOR*)g_XMNegativeZero);
    V2 = __vmaddfp(V2, V2, *(XMVECTOR*)g_XMNegativeZero);
    V3 = XMVectorReciprocal(V3);
    V0 = __vmaddfp(V0, V1, *(XMVECTOR*)g_XMNegativeZero);
    V2 = __vmaddfp(V2, V3, g_XMOne);

    Result = __vmaddfp(V0, V2, *(XMVECTOR*)g_XMNegativeZero);

    Result = __vmaxfp(Zero, Result);
    Result = __vminfp(g_XMOne, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR G, G2;
    XMVECTOR D, S;
    XMVECTOR V0, V1, V2, V3;
    XMVECTOR V1S0, V3S0, V1S1, V3S1;
    XMVECTOR OneHalfG;
    XMVECTOR Reciprocal, V1Reciprocal, V3Reciprocal;
    XMVECTOR Scale;
    XMVECTOR SignMask;
    XMVECTOR Zero, One, OneHalf;
    XMVECTOR Result;

    XMASSERT(!XMVector4IsInfinite(CosIncidentAngle));

    XMDUMMY_INITIALIZE_VECTOR(One);

    SignMask = __vspltisw(-1);
    G = __vcfsx(SignMask, 0);

    G = __vmaddfp(RefractionIndex, RefractionIndex, G);
    Zero = __vspltisw(0);
    OneHalf = __vspltisw(1);
    V2 = __vcfsx(SignMask, 0);

    G = __vmaddfp(CosIncidentAngle, CosIncidentAngle, G);
    Scale = __vcfsx(OneHalf, 1);

    SignMask = __vslw(SignMask, SignMask);
    G = __vandc(G, SignMask);

    S = __vrsqrtefp(G);
    OneHalfG = __vmulfp(G, Scale);
    Reciprocal = __vmulfp(S, S);
    Scale = __vnmsubfp(OneHalfG, Reciprocal, Scale);
    S = __vmaddfp(S, Scale, S);

    G2 = __vcmpeqfp(G, Zero);
    S = __vmulfp(G, S);
    One = __vupkd3d(One, VPACK_NORMSHORT2);
    G2 = __vsel(S, G, G2);

    S = __vaddfp(G2, CosIncidentAngle);
    D = __vsubfp(G2, CosIncidentAngle);

    V3 = __vspltw(One, 3);

    V1 = __vmulfp(S, S);
    V3 = __vmaddfp(CosIncidentAngle, D, V3);
    V0 = __vmulfp(D, D);

    V2 = __vmaddfp(CosIncidentAngle, S, V2);

    V3 = __vmulfp(V3, V3);

    V1Reciprocal = __vrefp(V1);
    V3Reciprocal = __vrefp(V3);

    V1S0 = __vspltw(One, 3);
    V3S0 = __vspltw(One, 3);

    V1S0 = __vnmsubfp(V1, V1Reciprocal, V1S0);
    V3S0 = __vnmsubfp(V3, V3Reciprocal, V3S0);

    V1S0 = __vmaddcfp(V1Reciprocal, V1S0, V1Reciprocal);
    V3S0 = __vmaddcfp(V3Reciprocal, V3S0, V3Reciprocal);

    V1S1 = __vspltw(One, 3);
    V3S1 = __vspltw(One, 3);

    OneHalf = __vcfsx(OneHalf, 1);

    V1S1 = __vnmsubfp(V1, V1S0, V1S1);
    V3S1 = __vnmsubfp(V3, V3S0, V3S1);

    V1 = __vcmpeqfp(V1S0, V1S0);
    V3 = __vcmpeqfp(V3S0, V3S0);

    V1S0 = __vmaddfp(V1S0, V1S1, V1S0);
    V3S0 = __vmaddfp(V3S0, V3S1, V3S0);

    V0 = __vmulfp(OneHalf, V0);
    V2 = __vmulfp(V2, V2);

    One = __vspltw(One, 3);

    V1 = __vsel(V1Reciprocal, V1S0, V1);
    V3 = __vsel(V3Reciprocal, V3S0, V3);

    V0 = __vmulfp(V0, V1);
    V2 = __vmaddcfp(V3, V2, One);

    Result = __vmulfp(V0, V2);

    Result = __vmaxfp(Zero, Result);
    Result = __vminfp(One, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMScalarNearEqual
(
    FLOAT S1,
    FLOAT S2,
    FLOAT Epsilon
)
{
    FLOAT Delta = S1 - S2;
    UINT  AbsDelta = *(UINT*)&Delta & 0x7FFFFFFF;
    return (*(FLOAT*)&AbsDelta <= Epsilon);
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT XMScalarModAngle
(
    FLOAT Angle
)
{
    // Modulo the range of the given angle such that -XM_PI <= Angle < XM_PI
    // Result = fmod(Angle + Offset, 2Pi) - Offset
    //        = Angle - 2Pi * truncate((Angle + Offset) / 2Pi)
    // where Offset = Angle < 0.0 ? -Pi : Pi
    static CONST FLOAT Pi = XM_PI;
    FLOAT              Offset;

    *(UINT*)&Offset = (*(UINT*)&Pi | (*(UINT*)&Angle & 0x80000000));

    return Angle - XM_2PI * (FLOAT)(INT)((Angle + Offset) / XM_2PI);
}

//------------------------------------------------------------------------------

XMINLINE FLOAT XMScalarSin
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    FLOAT                  ValueMod;
    FLOAT                  ValueSq;
    XMVECTOR               V0123, V0246, V1357, V9111315, V17192123;
    XMVECTOR               V1, V7, V8;
    XMVECTOR               R0, R1, R2;

    ValueMod = XMScalarModAngle(Value);

    // sin(V) ~= V - V^3 / 3! + V^5 / 5! - V^7 / 7! + V^9 / 9! - V^11 / 11! + V^13 / 13! - V^15 / 15! +
    //           V^17 / 17! - V^19 / 19! + V^21 / 21! - V^23 / 23! (for -PI <= V < PI)

    ValueSq = ValueMod * ValueMod;

    V0123     = XMVectorSet(1.0f, ValueMod, ValueSq, ValueSq * ValueMod);
    V1        = XMVectorSplatY(V0123);
    V0246     = XMVectorMultiply(V0123, V0123);
    V1357     = XMVectorMultiply(V0246, V1);
    V7        = XMVectorSplatW(V1357);
    V8        = XMVectorMultiply(V7, V1);
    V9111315  = XMVectorMultiply(V1357, V8);
    V17192123 = XMVectorMultiply(V9111315, V8);

    R0        = XMVector4Dot(V1357, g_XMSinCoefficients0);
    R1        = XMVector4Dot(V9111315, g_XMSinCoefficients1);
    R2        = XMVector4Dot(V17192123, g_XMSinCoefficients2);

    return R0.v[0] + R1.v[0] + R2.v[0];

#elif defined(_VMX32_INTRINSICS_)

    FLOAT                  ValueMod;
    FLOAT                  ValueSq;
    XMVECTOR               V0123, V0246, V1357, V9111315, V17192123;
    XMVECTOR               V1, V7, V8;
    XMVECTOR               D0[3], D1[3];
    XMVECTOR               R0, R1, R2;
    FLOAT                  Offset;
    static CONST FLOAT     Pi = XM_PI;

    *(UINT*)&Offset = (*(UINT*)&Pi | (*(UINT*)&Value & 0x80000000));
    ValueMod = Value - XM_2PI * (FLOAT)(INT)((Value + Offset) / XM_2PI);
    ValueSq = ValueMod * ValueMod;

    V0123.v[0] = 1.0f;
    V0123.v[1] = ValueMod;
    V0123.v[2] = ValueSq;
    V0123.v[3] = ValueSq * ValueMod;

    V1         = __vspltw(V0123, 1);
    V0246      = __vmaddfp(V0123, V0123, *(XMVECTOR*)g_XMNegativeZero);

    V1357      = __vmaddfp(V0246, V1, *(XMVECTOR*)g_XMNegativeZero);
    V7         = __vspltw(V1357, 3);
    V8         = __vmaddfp(V7, V1, *(XMVECTOR*)g_XMNegativeZero);
    V9111315   = __vmaddfp(V1357, V8, *(XMVECTOR*)g_XMNegativeZero);
    V17192123  = __vmaddfp(V9111315, V8, *(XMVECTOR*)g_XMNegativeZero);

    D0[0] = __vmaddfp(V1357, g_XMSinCoefficients0, *(XMVECTOR*)g_XMNegativeZero);
    D0[1] = __vmaddfp(V9111315, g_XMSinCoefficients1, *(XMVECTOR*)g_XMNegativeZero);
    D0[2] = __vmaddfp(V17192123, g_XMSinCoefficients2, *(XMVECTOR*)g_XMNegativeZero);

    D1[0] = __vsldoi(D0[0], D0[0], 8);
    D1[1] = __vsldoi(D0[1], D0[1], 8);
    D1[2] = __vsldoi(D0[2], D0[2], 8);

    D0[0] = __vaddfp(D0[0], D1[0]);
    D0[1] = __vaddfp(D0[1], D1[1]);
    D0[2] = __vaddfp(D0[2], D1[2]);

    D1[0] = __vsldoi(D0[0], D0[0], 4);
    D1[1] = __vsldoi(D0[1], D0[1], 4);
    D1[2] = __vsldoi(D0[2], D0[2], 4);

    R0    = __vaddfp(D0[0], D1[0]);
    R1    = __vaddfp(D0[1], D1[1]);
    R2    = __vaddfp(D0[2], D1[2]);

    return R0.v[0] + R1.v[0] + R2.v[0];

#else // _VMX128_INTRINSICS_

    XMVECTOR V0123, V0246, V1357, V9111315, V17192123;
    XMVECTOR V1, V7, V8;
    XMVECTOR R0, R1, R2;

    V0123.v[0] = 1.0f;
    V0123.v[1] = XMScalarModAngle(Value);
    V0123.v[2] = V0123.v[1] * V0123.v[1];
    V0123.v[3] = V0123.v[2] * V0123.v[1];

    V1        = __vspltw(V0123, 1);
    V0246     = __vmulfp(V0123, V0123);
    V1357     = __vmulfp(V0246, V1);
    V7        = __vspltw(V1357, 3);
    V8        = __vmulfp(V7, V1);
    V9111315  = __vmulfp(V1357, V8);
    V17192123 = __vmulfp(V9111315, V8);

    R0        = __vmsum4fp(V1357, g_XMSinCoefficients0);
    R1        = __vmsum4fp(V9111315, g_XMSinCoefficients1);
    R2        = __vmsum4fp(V17192123, g_XMSinCoefficients2);

    return R0.v[0] + R1.v[0] + R2.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE FLOAT XMScalarCos
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    FLOAT                  ValueMod;
    FLOAT                  ValueSq;
    XMVECTOR               V0123, V0246, V8101214, V16182022;
    XMVECTOR               V2, V6, V8;
    XMVECTOR               R0, R1, R2;

    ValueMod = XMScalarModAngle(Value);

    // cos(V) ~= 1 - V^2 / 2! + V^4 / 4! - V^6 / 6! + V^8 / 8! - V^10 / 10! +
    //           V^12 / 12! - V^14 / 14! + V^16 / 16! - V^18 / 18! + V^20 / 20! - V^22 / 22! (for -PI <= V < PI)

    ValueSq = ValueMod * ValueMod;

    V0123 = XMVectorSet(1.0f, ValueMod, ValueSq, ValueSq * ValueMod);
    V0246 = XMVectorMultiply(V0123, V0123);

    V2 = XMVectorSplatZ(V0123);
    V6 = XMVectorSplatW(V0246);
    V8 = XMVectorMultiply(V6, V2);

    V8101214 = XMVectorMultiply(V0246, V8);
    V16182022 = XMVectorMultiply(V8101214, V8);

    R0 = XMVector4Dot(V0246, g_XMCosCoefficients0);
    R1 = XMVector4Dot(V8101214, g_XMCosCoefficients1);
    R2 = XMVector4Dot(V16182022, g_XMCosCoefficients2);

    return R0.v[0] + R1.v[0] + R2.v[0];

#elif defined(_VMX32_INTRINSICS_)

    FLOAT               ValueMod;
    FLOAT               ValueSq;
    XMVECTOR            V0123, V0246, V8101214, V16182022;
    XMVECTOR            V2, V6, V8;
    XMVECTOR            D0[3], D1[3];
    XMVECTOR            R0, R1, R2;
    FLOAT               Offset;
    static CONST FLOAT  Pi = XM_PI;

    *(UINT*)&Offset = (*(UINT*)&Pi | (*(UINT*)&Value & 0x80000000));
    ValueMod = Value - XM_2PI * (FLOAT)(INT)((Value + Offset) / XM_2PI);

    ValueSq = ValueMod * ValueMod;

    V0123.v[0] = 1.0f;
    V0123.v[1] = ValueMod;
    V0123.v[2] = ValueSq;
    V0123.v[3] = ValueSq * ValueMod;

    V0246 = __vmaddfp(V0123, V0123, *(XMVECTOR*)g_XMNegativeZero);

    V2 = __vspltw(V0123, 2);
    V6 = __vspltw(V0246, 3);
    V8 = __vmaddfp(V6, V2, *(XMVECTOR*)g_XMNegativeZero);

    V8101214 = __vmaddfp(V0246, V8, *(XMVECTOR*)g_XMNegativeZero);
    V16182022 = __vmaddfp(V8101214, V8, *(XMVECTOR*)g_XMNegativeZero);

    D0[0] = __vmaddfp(V0246, g_XMCosCoefficients0, *(XMVECTOR*)g_XMNegativeZero);
    D0[1] = __vmaddfp(V8101214, g_XMCosCoefficients1, *(XMVECTOR*)g_XMNegativeZero);
    D0[2] = __vmaddfp(V16182022, g_XMCosCoefficients2, *(XMVECTOR*)g_XMNegativeZero);

    D1[0] = __vsldoi(D0[0], D0[0], 8);
    D1[1] = __vsldoi(D0[1], D0[1], 8);
    D1[2] = __vsldoi(D0[2], D0[2], 8);

    D0[0] = __vaddfp(D0[0], D1[0]);
    D0[1] = __vaddfp(D0[1], D1[1]);
    D0[2] = __vaddfp(D0[2], D1[2]);

    D1[0] = __vsldoi(D0[0], D0[0], 4);
    D1[1] = __vsldoi(D0[1], D0[1], 4);
    D1[2] = __vsldoi(D0[2], D0[2], 4);

    R0    = __vaddfp(D0[0], D1[0]);
    R1    = __vaddfp(D0[1], D1[1]);
    R2    = __vaddfp(D0[2], D1[2]);

    return R0.v[0] + R1.v[0] + R2.v[0];

#else // _VMX128_INTRINSICS_

    XMVECTOR V0123, V0246, V8101214, V16182022;
    XMVECTOR V2, V6, V8;
    XMVECTOR R0, R1, R2;

    V0123.v[0] = 1.0f;
    V0123.v[1] = XMScalarModAngle(Value);
    V0123.v[2] = V0123.v[1] * V0123.v[1];
    V0123.v[3] = V0123.v[2] * V0123.v[1];

    V0246 = __vmulfp(V0123, V0123);

    V2 = __vspltw(V0123, 2);
    V6 = __vspltw(V0246, 3);
    V8 = __vmulfp(V6, V2);

    V8101214 = __vmulfp(V0246, V8);
    V16182022 = __vmulfp(V8101214, V8);

    R0 = __vmsum4fp(V0246, g_XMCosCoefficients0);
    R1 = __vmsum4fp(V8101214, g_XMCosCoefficients1);
    R2 = __vmsum4fp(V16182022, g_XMCosCoefficients2);

    return R0.v[0] + R1.v[0] + R2.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE VOID XMScalarSinCos
(
    FLOAT* pSin,
    FLOAT* pCos,
    FLOAT  Value
)
{
#if defined(_NO_INTRINSICS_)

    FLOAT                  ValueMod;
    FLOAT                  ValueSq;
    XMVECTOR               V0123, V0246, V1357, V8101214, V9111315, V16182022, V17192123;
    XMVECTOR               V1, V2, V6, V8;
    XMVECTOR               S0, S1, S2, C0, C1, C2;

    XMASSERT(pSin);
    XMASSERT(pCos);

    ValueMod = XMScalarModAngle(Value);

    // sin(V) ~= V - V^3 / 3! + V^5 / 5! - V^7 / 7! + V^9 / 9! - V^11 / 11! + V^13 / 13! - V^15 / 15! +
    //           V^17 / 17! - V^19 / 19! + V^21 / 21! - V^23 / 23! (for -PI <= V < PI)
    // cos(V) ~= 1 - V^2 / 2! + V^4 / 4! - V^6 / 6! + V^8 / 8! - V^10 / 10! +
    //           V^12 / 12! - V^14 / 14! + V^16 / 16! - V^18 / 18! + V^20 / 20! - V^22 / 22! (for -PI <= V < PI)

    ValueSq = ValueMod * ValueMod;

    V0123 = XMVectorSet(1.0f, ValueMod, ValueSq, ValueSq * ValueMod);

    V1 = XMVectorSplatY(V0123);
    V2 = XMVectorSplatZ(V0123);

    V0246 = XMVectorMultiply(V0123, V0123);
    V1357 = XMVectorMultiply(V0246, V1);

    V6 = XMVectorSplatW(V0246);
    V8 = XMVectorMultiply(V6, V2);

    V8101214 = XMVectorMultiply(V0246, V8);
    V9111315 = XMVectorMultiply(V1357, V8);
    V16182022 = XMVectorMultiply(V8101214, V8);
    V17192123 = XMVectorMultiply(V9111315, V8);

    C0 = XMVector4Dot(V0246, g_XMCosCoefficients0);
    S0 = XMVector4Dot(V1357, g_XMSinCoefficients0);
    C1 = XMVector4Dot(V8101214, g_XMCosCoefficients1);
    S1 = XMVector4Dot(V9111315, g_XMSinCoefficients1);
    C2 = XMVector4Dot(V16182022, g_XMCosCoefficients2);
    S2 = XMVector4Dot(V17192123, g_XMSinCoefficients2);

    *pCos = C0.v[0] + C1.v[0] + C2.v[0];
    *pSin = S0.v[0] + S1.v[0] + S2.v[0];

#elif defined(_VMX32_INTRINSICS_)

    FLOAT                  ValueMod;
    FLOAT                  ValueSq;
    XMVECTOR               V0123, V0246, V1357, V8101214, V9111315, V16182022, V17192123;
    XMVECTOR               V1, V2, V6, V8;
    XMVECTOR               S0, S1, S2, C0, C1, C2;
    XMVECTOR               D0[6];
    XMVECTOR               D1[6];
    FLOAT                  Offset;
    static CONST FLOAT     Pi = XM_PI;

    XMASSERT(pSin);
    XMASSERT(pCos);

    *(UINT*)&Offset = (*(UINT*)&Pi | (*(UINT*)&Value & 0x80000000));
    ValueMod = Value - XM_2PI * (FLOAT)(INT)((Value + Offset) / XM_2PI);

    ValueSq = ValueMod * ValueMod;

    V0123.v[0] = 1.0f;
    V0123.v[1] = ValueMod;
    V0123.v[2] = ValueSq;
    V0123.v[3] = ValueSq * ValueMod;

    V1 = __vspltw(V0123, 1);
    V2 = __vspltw(V0123, 2);

    V0246 = __vmaddfp(V0123, V0123, *(XMVECTOR*)g_XMNegativeZero);
    V1357 = __vmaddfp(V0246, V1, *(XMVECTOR*)g_XMNegativeZero);

    V6 = __vspltw(V0246, 3);
    V8 = __vmaddfp(V6, V2, *(XMVECTOR*)g_XMNegativeZero);

    V8101214 = __vmaddfp(V0246, V8, *(XMVECTOR*)g_XMNegativeZero);
    V9111315 = __vmaddfp(V1357, V8, *(XMVECTOR*)g_XMNegativeZero);

    D0[0] = __vmaddfp(V0246, g_XMCosCoefficients0, *(XMVECTOR*)g_XMNegativeZero);
    D0[1] = __vmaddfp(V1357, g_XMSinCoefficients0, *(XMVECTOR*)g_XMNegativeZero);

    V16182022 = __vmaddfp(V8101214, V8, *(XMVECTOR*)g_XMNegativeZero);
    V17192123 = __vmaddfp(V9111315, V8, *(XMVECTOR*)g_XMNegativeZero);

    D0[2] = __vmaddfp(V8101214, g_XMCosCoefficients1, *(XMVECTOR*)g_XMNegativeZero);
    D0[3] = __vmaddfp(V9111315, g_XMSinCoefficients1, *(XMVECTOR*)g_XMNegativeZero);
    D0[4] = __vmaddfp(V16182022, g_XMCosCoefficients2, *(XMVECTOR*)g_XMNegativeZero);
    D0[5] = __vmaddfp(V17192123, g_XMSinCoefficients2, *(XMVECTOR*)g_XMNegativeZero);

    D1[0] = __vsldoi(D0[0], D0[0], 8);
    D1[1] = __vsldoi(D0[1], D0[1], 8);
    D1[2] = __vsldoi(D0[2], D0[2], 8);
    D1[3] = __vsldoi(D0[3], D0[3], 8);
    D1[4] = __vsldoi(D0[4], D0[4], 8);
    D1[5] = __vsldoi(D0[5], D0[5], 8);

    D0[0] = __vaddfp(D0[0], D1[0]);
    D0[1] = __vaddfp(D0[1], D1[1]);
    D0[2] = __vaddfp(D0[2], D1[2]);
    D0[3] = __vaddfp(D0[3], D1[3]);
    D0[4] = __vaddfp(D0[4], D1[4]);
    D0[5] = __vaddfp(D0[5], D1[5]);

    D1[0] = __vsldoi(D0[0], D0[0], 4);
    D1[1] = __vsldoi(D0[1], D0[1], 4);
    D1[2] = __vsldoi(D0[2], D0[2], 4);
    D1[3] = __vsldoi(D0[3], D0[3], 4);
    D1[4] = __vsldoi(D0[4], D0[4], 4);
    D1[5] = __vsldoi(D0[5], D0[5], 4);

    C0 = __vaddfp(D0[0], D1[0]);
    S0 = __vaddfp(D0[1], D1[1]);
    C1 = __vaddfp(D0[2], D1[2]);
    S1 = __vaddfp(D0[3], D1[3]);
    C2 = __vaddfp(D0[4], D1[4]);
    S2 = __vaddfp(D0[5], D1[5]);

    *pCos = C0.v[0] + C1.v[0] + C2.v[0];
    *pSin = S0.v[0] + S1.v[0] + S2.v[0];

#else // _VMX128_INTRINSICS_

    XMVECTOR V0123, V0246, V1357, V8101214, V9111315, V16182022, V17192123;
    XMVECTOR V1, V2, V6, V8;
    XMVECTOR S0, S1, S2, C0, C1, C2;

    XMASSERT(pSin);
    XMASSERT(pCos);

    V0123.v[0] = 1.0f;
    V0123.v[1] = XMScalarModAngle(Value);
    V0123.v[2] = V0123.v[1] * V0123.v[1];
    V0123.v[3] = V0123.v[2] * V0123.v[1];

    V1 = __vspltw(V0123, 1);
    V2 = __vspltw(V0123, 2);

    V0246 = __vmulfp(V0123, V0123);
    V1357 = __vmulfp(V0246, V1);

    V6 = __vspltw(V0246, 3);
    V8 = __vmulfp(V6, V2);

    V8101214 = __vmulfp(V0246, V8);
    V9111315 = __vmulfp(V1357, V8);
    V16182022 = __vmulfp(V8101214, V8);
    V17192123 = __vmulfp(V9111315, V8);

    C0 = __vmsum4fp(V0246, g_XMCosCoefficients0);
    S0 = __vmsum4fp(V1357, g_XMSinCoefficients0);
    C1 = __vmsum4fp(V8101214, g_XMCosCoefficients1);
    S1 = __vmsum4fp(V9111315, g_XMSinCoefficients1);
    C2 = __vmsum4fp(V16182022, g_XMCosCoefficients2);
    S2 = __vmsum4fp(V17192123, g_XMSinCoefficients2);

    *pCos = C0.v[0] + C1.v[0] + C2.v[0];
    *pSin = S0.v[0] + S1.v[0] + S2.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE FLOAT XMScalarASin
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    FLOAT AbsValue, Value2, Value3, D;
    XMVECTOR AbsV, R0, R1, Result;
    XMVECTOR V3;

    *(UINT*)&AbsValue = *(UINT*)&Value & 0x7FFFFFFF;

    Value2 = Value * AbsValue;
    Value3 = Value * Value2;
    D = (Value - Value2) / sqrtf(1.00000011921f - AbsValue);

    AbsV = XMVectorReplicate(AbsValue);

    V3.v[0] = Value3;
    V3.v[1] = 1.0f;
    V3.v[2] = Value3;
    V3.v[3] = 1.0f;

    R1 = XMVectorSet(D, D, Value, Value);
    R1 = XMVectorMultiply(R1, V3);

    R0 = XMVectorMultiplyAdd(AbsV, g_XMASinCoefficients0, g_XMASinCoefficients1);
    R0 = XMVectorMultiplyAdd(AbsV, R0, g_XMASinCoefficients2);

    Result = XMVector4Dot(R0, R1);

    return Result.v[0];

#elif defined(_VMX32_INTRINSICS_)

    FLOAT AbsValue, Value2, Value3, D;
    XMVECTOR AbsV, R0, R1, Result;
    XMVECTOR V3;

    *(UINT*)&AbsValue = *(UINT*)&Value & 0x7FFFFFFF;

    AbsV.v[0] =
    AbsV.v[1] =
    AbsV.v[2] =
    AbsV.v[3] = AbsValue;

    Value2 = Value * AbsValue;
    Value3 = Value * Value2;
    D = (Value - Value2) / sqrtf(1.00000011921f - AbsValue);

    V3.v[0] = Value3;
    V3.v[1] = 1.0f;
    V3.v[2] = Value3;
    V3.v[3] = 1.0f;

    R1.v[0] = D;
    R1.v[1] = D;
    R1.v[2] = Value;
    R1.v[3] = Value;

    R0 = __vmaddfp(AbsV, g_XMASinCoefficients0, g_XMASinCoefficients1);

    R1 = XMVectorMultiply(R1, V3);

    R0 = __vmaddfp(AbsV, R0, g_XMASinCoefficients2);

    Result = XMVector4Dot(R0, R1);

    return Result.v[0];

#else // _VMX128_INTRINSICS_

    FLOAT Value2, Value3, D;
    XMVECTOR AbsV, R0, R1, Result;
    XMVECTOR V3;

    *(UINT*)&AbsV.v[0] = *(UINT*)&Value & 0x7FFFFFFF;

    Value2 = Value * AbsV.v[0];
    Value3 = Value * Value2;
    D = (Value - Value2) / sqrtf(1.00000011921f - AbsV.v[0]);

    AbsV = __vspltw(AbsV, 0);
    R0 = g_XMASinCoefficients0;

    V3.v[0] = Value3;
    V3.v[1] = 1.0f;
    V3.v[2] = Value3;
    V3.v[3] = 1.0f;

    R1.v[0] = D;
    R1.v[1] = D;
    R1.v[2] = Value;
    R1.v[3] = Value;

    R0 = __vmaddcfp(AbsV, R0, g_XMASinCoefficients1);
    R1 = __vmulfp(R1, V3);
    R0 = __vmaddcfp(AbsV, R0, g_XMASinCoefficients2);

    Result = __vmsum4fp(R0, R1);

    return Result.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE FLOAT XMScalarACos
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    return XM_PIDIV2 - XMScalarASin(Value);

#elif defined(_VMX32_INTRINSICS_)

    FLOAT AbsValue, Value2, Value3, D;
    XMVECTOR AbsV, R0, R1, Result;
    XMVECTOR V3;

    *(UINT*)&AbsValue = *(UINT*)&Value & 0x7FFFFFFF;

    AbsV.v[0] =
    AbsV.v[1] =
    AbsV.v[2] =
    AbsV.v[3] = AbsValue;

    Value2 = Value * AbsValue;
    Value3 = Value * Value2;
    D = (Value - Value2) / sqrtf(1.00000011921f - AbsValue);

    V3.v[0] = Value3;
    V3.v[1] = 1.0f;
    V3.v[2] = Value3;
    V3.v[3] = 1.0f;

    R1.v[0] = D;
    R1.v[1] = D;
    R1.v[2] = Value;
    R1.v[3] = Value;

    R0 = __vmaddfp(AbsV, g_XMASinCoefficients0, g_XMASinCoefficients1);

    R1 = XMVectorMultiply(R1, V3);

    R0 = __vmaddfp(AbsV, R0, g_XMASinCoefficients2);

    Result = XMVector4Dot(R0, R1);

    return XM_PIDIV2 - Result.v[0];

#else // _VMX128_INTRINSICS_

    FLOAT Value2, Value3, D;
    XMVECTOR AbsV, R0, R1, Result;
    XMVECTOR V3;

    *(UINT*)&AbsV.v[0] = *(UINT*)&Value & 0x7FFFFFFF;

    Value2 = Value * AbsV.v[0];
    Value3 = Value * Value2;
    D = (Value - Value2) / sqrtf(1.00000011921f - AbsV.v[0]);

    AbsV = __vspltw(AbsV, 0);
    R0 = g_XMASinCoefficients0;

    V3.v[0] = Value3;
    V3.v[1] = 1.0f;
    V3.v[2] = Value3;
    V3.v[3] = 1.0f;

    R1.v[0] = D;
    R1.v[1] = D;
    R1.v[2] = Value;
    R1.v[3] = Value;

    R0 = __vmaddcfp(AbsV, R0, g_XMASinCoefficients1);
    R1 = __vmulfp(R1, V3);
    R0 = __vmaddcfp(AbsV, R0, g_XMASinCoefficients2);

    Result = __vmsum4fp(R0, R1);

    return XM_PIDIV2 - Result.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT XMScalarSinEst
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    FLOAT                  ValueSq;
    XMVECTOR               V;
    XMVECTOR               Y;
    XMVECTOR               Result;

    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    // sin(V) ~= V - V^3 / 3! + V^5 / 5! - V^7 / 7! (for -PI <= V < PI)

    ValueSq = Value * Value;

    V = XMVectorSet(1.0f, Value, ValueSq, ValueSq * Value);
    Y = XMVectorSplatY(V);
    V = XMVectorMultiply(V, V);
    V = XMVectorMultiply(V, Y);

    Result = XMVector4Dot(V, g_XMSinEstCoefficients);

    return Result.v[0];

#elif defined(_VMX32_INTRINSICS_)

    FLOAT                  ValueSq;
    XMVECTOR               V;
    XMVECTOR               Y;
    XMVECTOR               D0, D1;
    XMVECTOR               Result;

    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    ValueSq = Value * Value;

    V.v[0] = 1.0f;
    V.v[1] = Value;
    V.v[2] = ValueSq;
    V.v[3] = ValueSq * Value;

    Y = __vspltw(V, 1);
    V = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    V = __vmaddfp(V, Y, *(XMVECTOR*)g_XMNegativeZero);

    D0 = __vmaddfp(V, g_XMSinEstCoefficients, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return Result.v[0];

#else // _VMX128_INTRINSICS_

    XMVECTOR V;
    XMVECTOR Y;
    XMVECTOR Result;

    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    V.v[0] = 1.0f;
    V.v[1] = Value;
    V.v[2] = V.v[1] * V.v[1];
    V.v[3] = V.v[2] * V.v[1];

    Y = __vspltw(V, 1);
    V = __vmulfp(V, V);
    V = __vmulfp(V, Y);

    Result = __vmsum4fp(V, g_XMSinEstCoefficients);

    return Result.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT XMScalarCosEst
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    FLOAT    ValueSq;
    XMVECTOR V;
    XMVECTOR Result;

    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    // cos(V) ~= 1 - V^2 / 2! + V^4 / 4! - V^6 / 6! (for -PI <= V < PI)

    ValueSq = Value * Value;

    V = XMVectorSet(1.0f, Value, ValueSq, ValueSq * Value);
    V = XMVectorMultiply(V, V);

    Result = XMVector4Dot(V, g_XMCosEstCoefficients);

    return Result.v[0];

#elif defined(_VMX32_INTRINSICS_)

    FLOAT    ValueSq;
    XMVECTOR V;
    XMVECTOR D0, D1;
    XMVECTOR Result;

    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    ValueSq = Value * Value;

    V.v[0] = 1.0f;
    V.v[1] = Value;
    V.v[2] = ValueSq;
    V.v[3] = ValueSq * Value;

    V = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);

    D0 = __vmaddfp(V, g_XMCosEstCoefficients, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return Result.v[0];

#else // _VMX128_INTRINSICS_

    XMVECTOR V;
    XMVECTOR Result;

    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    V.v[0] = 1.0f;
    V.v[1] = Value;
    V.v[2] = V.v[1] * V.v[1];
    V.v[3] = V.v[2] * V.v[1];

    V = __vmulfp(V, V);

    Result = __vmsum4fp(V, g_XMCosEstCoefficients);

    return Result.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMScalarSinCosEst
(
    FLOAT* pSin,
    FLOAT* pCos,
    FLOAT  Value
)
{
#if defined(_NO_INTRINSICS_)

    FLOAT    ValueSq;
    XMVECTOR V, Sin, Cos;
    XMVECTOR Y;

    XMASSERT(pSin);
    XMASSERT(pCos);
    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    // sin(V) ~= V - V^3 / 3! + V^5 / 5! - V^7 / 7! (for -PI <= V < PI)
    // cos(V) ~= 1 - V^2 / 2! + V^4 / 4! - V^6 / 6! (for -PI <= V < PI)

    ValueSq = Value * Value;

    V = XMVectorSet(1.0f, Value, ValueSq, Value * ValueSq);
    Y = XMVectorSplatY(V);

    Cos = XMVectorMultiply(V, V);
    Sin = XMVectorMultiply(Cos, Y);

    Cos = XMVector4Dot(Cos, g_XMCosEstCoefficients);
    Sin = XMVector4Dot(Sin, g_XMSinEstCoefficients);

    *pCos = Cos.v[0];
    *pSin = Sin.v[0];

#elif defined(_VMX32_INTRINSICS_)

    FLOAT    ValueSq;
    XMVECTOR V, Sin, Cos;
    XMVECTOR Y;
    XMVECTOR D0, D1, D2, D3;

    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    ValueSq = Value * Value;

    V.v[0] = 1.0f;
    V.v[1] = Value;
    V.v[2] = ValueSq;
    V.v[3] = ValueSq * Value;

    Y = __vspltw(V, 1);

    Cos = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    Sin = __vmaddfp(Cos, Y, *(XMVECTOR*)g_XMNegativeZero);

    D0 = __vmaddfp(Cos, g_XMCosEstCoefficients, *(XMVECTOR*)g_XMNegativeZero);
    D2 = __vmaddfp(Sin, g_XMSinEstCoefficients, *(XMVECTOR*)g_XMNegativeZero);

    D1 = __vsldoi(D0, D0, 8);
    D3 = __vsldoi(D2, D2, 8);

    D0 = __vaddfp(D0, D1);
    D2 = __vaddfp(D2, D3);

    D1 = __vsldoi(D0, D0, 4);
    D3 = __vsldoi(D2, D2, 4);

    Cos = __vaddfp(D0, D1);
    Sin = __vaddfp(D2, D3);

    *pCos = Cos.v[0];
    *pSin = Sin.v[0];

#else // _VMX128_INTRINSICS_

    XMVECTOR V, Sin, Cos;
    XMVECTOR Y;

    XMASSERT(pSin);
    XMASSERT(pCos);
    XMASSERT(Value >= -XM_PI);
    XMASSERT(Value < XM_PI);

    V.v[0] = 1.0f;
    V.v[1] = Value;
    V.v[2] = V.v[1] * V.v[1];
    V.v[3] = V.v[2] * V.v[1];

    Y = __vspltw(V, 1);

    Cos = __vmulfp(V, V);
    Sin = __vmulfp(Cos, Y);

    Cos = __vmsum4fp(Cos, g_XMCosEstCoefficients);
    Sin = __vmsum4fp(Sin, g_XMSinEstCoefficients);

    *pCos = Cos.v[0];
    *pSin = Sin.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT XMScalarASinEst
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR VR, CR, CS;
    XMVECTOR Result;
    FLOAT AbsV, V2, D;
    CONST FLOAT OnePlusEps = 1.00000011921f;

    *(UINT*)&AbsV = *(UINT*)&Value & 0x7FFFFFFF;
    V2 = Value * AbsV;
    D = OnePlusEps - AbsV;

    CS = XMVectorSet(Value, 1.0f, 1.0f, V2);
    VR = XMVectorSet(sqrtf(D), Value, V2, D * AbsV);
    CR = XMVectorMultiply(CS, g_XMASinEstCoefficients);

    Result = XMVector4Dot(VR, CR);

    return Result.v[0];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR VR, CR;
    XMVECTOR D0, D1;
    XMVECTOR Result;
    FLOAT AbsV, V2, D;
    CONST FLOAT OnePlusEps = 1.00000011921f;
    XMVECTOR CS;

    *(UINT*)&AbsV = *(UINT*)&Value & 0x7FFFFFFF;
    V2 = Value * AbsV;
    D = OnePlusEps - AbsV;

    CS.v[0] = Value;
    CS.v[1] = 1.0f;
    CS.v[2] = 1.0f;
    CS.v[3] = V2;

    VR.v[0] = sqrtf(D);
    VR.v[1] = Value;
    VR.v[2] = V2;
    VR.v[3] = D * AbsV;

    CR = __vmaddfp(CS, g_XMASinEstCoefficients, *(XMVECTOR*)g_XMNegativeZero);

    D0 = __vmaddfp(VR, CR, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return Result.v[0];

#else // _VMX128_INTRINSICS_

    XMVECTOR VR, CR, CS;
    XMVECTOR Result;
    FLOAT AbsV, V2, D;
    CONST FLOAT OnePlusEps = 1.00000011921f;

    *(UINT*)&AbsV = *(UINT*)&Value & 0x7FFFFFFF;
    V2 = Value * AbsV;
    D = OnePlusEps - AbsV;

    CS.v[0] = Value;
    CS.v[1] = 1.0f;
    CS.v[2] = 1.0f;
    CS.v[3] = V2;

    VR.v[0] = sqrtf(D);
    VR.v[1] = Value;
    VR.v[2] = V2;
    VR.v[3] = D * AbsV;
    CR = __vmulfp(CS, g_XMASinEstCoefficients);

    Result = __vmsum4fp(VR, CR);

    return Result.v[0];

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE FLOAT XMScalarACosEst
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR VR, CR, CS;
    XMVECTOR Result;
    FLOAT AbsV, V2, D;
    CONST FLOAT OnePlusEps = 1.00000011921f;

    // return XM_PIDIV2 - XMScalarASin(Value);

    *(UINT*)&AbsV = *(UINT*)&Value & 0x7FFFFFFF;
    V2 = Value * AbsV;
    D = OnePlusEps - AbsV;

    CS = XMVectorSet(Value, 1.0f, 1.0f, V2);
    VR = XMVectorSet(sqrtf(D), Value, V2, D * AbsV);
    CR = XMVectorMultiply(CS, g_XMASinEstCoefficients);

    Result = XMVector4Dot(VR, CR);

    return XM_PIDIV2 - Result.v[0];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR VR, CR;
    XMVECTOR D0, D1;
    XMVECTOR Result;
    FLOAT AbsV, V2, D;
    CONST FLOAT OnePlusEps = 1.00000011921f;
    XMVECTOR CS;

    *(UINT*)&AbsV = *(UINT*)&Value & 0x7FFFFFFF;
    V2 = Value * AbsV;
    D = OnePlusEps - AbsV;

    CS.v[0] = Value;
    CS.v[1] = 1.0f;
    CS.v[2] = 1.0f;
    CS.v[3] = V2;

    VR.v[0] = sqrtf(D);
    VR.v[1] = Value;
    VR.v[2] = V2;
    VR.v[3] = D * AbsV;

    CR = __vmaddfp(CS, g_XMASinEstCoefficients, *(XMVECTOR*)g_XMNegativeZero);

    D0 = __vmaddfp(VR, CR, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return XM_PIDIV2 - Result.v[0];

#else // _VMX128_INTRINSICS_

    XMVECTOR VR, CR, CS;
    XMVECTOR Result;
    FLOAT AbsV, V2, D;
    CONST FLOAT OnePlusEps = 1.00000011921f;

    *(UINT*)&AbsV = *(UINT*)&Value & 0x7FFFFFFF;
    V2 = Value * AbsV;
    D = OnePlusEps - AbsV;

    CS.v[0] = Value;
    CS.v[1] = 1.0f;
    CS.v[2] = 1.0f;
    CS.v[3] = V2;

    VR.v[0] = sqrtf(D);
    VR.v[1] = Value;
    VR.v[2] = V2;
    VR.v[3] = D * AbsV;
    CR = __vmulfp(CS, g_XMASinEstCoefficients);

    Result = __vmsum4fp(VR, CR);

    return XM_PIDIV2 - Result.v[0];

#endif // _VMX128_INTRINSICS_
}

#endif // __XMMISC_INL__
