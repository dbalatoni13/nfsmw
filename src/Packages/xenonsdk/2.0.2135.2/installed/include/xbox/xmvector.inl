/*++

Copyright (c) Microsoft Corporation

Module Name:

    xmvector.inl

Abstract:

    Vector functions.

--*/

#pragma once

#ifndef __XMVECTOR_INL__
#define __XMVECTOR_INL__

#ifdef _NO_INTRINSICS_
#define XMISNAN(x)  ((*(UINT*)&(x) & 0x7F800000) == 0x7F800000 && (*(UINT*)&(x) & 0x7FFFFF) != 0)
#define XMISINF(x)  ((*(UINT*)&(x) & 0x7FFFFFFF) == 0x7F800000)
#endif // _NO_INTRINSICS_

/****************************************************************************
 *
 * General Vector
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Assignment operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorZero()
{
#if defined(_NO_INTRINSICS_)

    return XMVectorReplicate(0.0f);

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vspltisw(0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSet
(
    FLOAT x, 
    FLOAT y, 
    FLOAT z, 
    FLOAT w
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V;

    V.v[0] = x;
    V.v[1] = y;
    V.v[2] = z;
    V.v[3] = w;

    return V;

#else

    XMVECTOR XYZW, ZW, Y, W;

    XMASSERT(((UINT_PTR)&x & 3) == 0);
    XMASSERT(((UINT_PTR)&y & 3) == 0);
    XMASSERT(((UINT_PTR)&z & 3) == 0);
    XMASSERT(((UINT_PTR)&w & 3) == 0);

    XYZW = __lvlx(&x, 0);
    Y    = __lvlx(&y, 0);
    ZW   = __lvlx(&z, 0);
    W    = __lvlx(&w, 0);

    XYZW = __vrlimi(XYZW, Y, 0x4, 3);
    ZW   = __vrlimi(ZW, W, 0x4, 3);
    XYZW = __vrlimi(XYZW, ZW, 0x3, 2);

    return XYZW;

#endif
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSetInt
(
    UINT x, 
    UINT y, 
    UINT z, 
    UINT w
)
{
    XMVECTOR V;

    V.u[0] = x;
    V.u[1] = y;
    V.u[2] = z;
    V.u[3] = w;

    return V;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorReplicate
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    V.v[0] = 
    V.v[1] = 
    V.v[2] = 
    V.v[3] = Value;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V;

    V.v[0] = 
    V.v[1] = 
    V.v[2] = 
    V.v[3] = Value;

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR V;

    XMASSERT(((UINT_PTR)&Value & 3) == 0);

    V = __lvlx(&Value, 0);
    V = __vspltw(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorTrueInt()
{
#if defined(_NO_INTRINSICS_)

    return XMVectorSetInt(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vspltisw(-1);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorFalseInt()
{
    return XMVectorZero();
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatX
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = 
    Result.v[1] = 
    Result.v[2] = 
    Result.v[3] = V.v[0];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vspltw(V, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatY
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = 
    Result.v[1] = 
    Result.v[2] = 
    Result.v[3] = V.v[1];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vspltw(V, 1);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatZ
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = 
    Result.v[1] = 
    Result.v[2] = 
    Result.v[3] = V.v[2];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vspltw(V, 2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatW
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = 
    Result.v[1] = 
    Result.v[2] = 
    Result.v[3] = V.v[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vspltw(V, 3);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatOne()
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    return XMVectorReplicate(1.0f);

#else //  _VMX128_INTRINSICS_

    // Leverage the unpack instruction to generate a 1.0 with half the
    // latency required by XMVectorSplatConstant.

    XMVECTOR One;

    XMDUMMY_INITIALIZE_VECTOR(One);

    One = __vupkd3d(One, VPACK_NORMSHORT2);
    One = __vspltw(One, 3);

    return One;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatInfinity()
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = 
    Result.u[1] = 
    Result.u[2] = 
    Result.u[3] = 0x7F800000;

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Infinity;
    XMVECTOR ShiftL;
    XMVECTOR ShiftR;

    Infinity = __vspltisw(-1);
    ShiftL = __vspltisw(-8); // Shift 24
    ShiftR = __vspltisw(1);
    Infinity = __vslw(Infinity, ShiftL);
    Infinity = __vsrw(Infinity, ShiftR);

    return Infinity;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatQNaN()
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = 
    Result.u[1] = 
    Result.u[2] = 
    Result.u[3] = 0x7FC00000;

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR QNaN;
    XMVECTOR ShiftL;
    XMVECTOR ShiftR;

    QNaN = __vspltisw(-1);
    ShiftL = __vspltisw(-9); // Shift 23
    ShiftR = __vspltisw(1);
    QNaN = __vslw(QNaN, ShiftL);
    QNaN = __vsrw(QNaN, ShiftR);

    return QNaN;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatEpsilon()
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = 
    Result.u[1] = 
    Result.u[2] = 
    Result.u[3] = 0x34000000;

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Epsilon;
    XMVECTOR ShiftL;

    Epsilon = __vspltisw(13);
    ShiftL = __vspltisw(-6); // Shift 26
    Epsilon = __vslw(Epsilon, ShiftL);

    return Epsilon;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSplatSignMask()
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = 
    Result.u[1] = 
    Result.u[2] = 
    Result.u[3] = 0x80000000;

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR SignMask;

    SignMask = __vspltisw(-1);             // 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF (low order 5 bits of each element = 31)
    SignMask = __vslw(SignMask, SignMask); // 0x80000000 0x80000000 0x80000000 0x80000000 

    return SignMask;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Define a control vector to be used in XMVectorPermute
// operations.  Visualize the two vectors V1 and V2 given
// in a permute as arranged back to back in a linear fashion,
// such that they form an array of 8 floating point values.
// The four integers specified in XMVectorPermuteControl
// will serve as indices into the array to select components
// from the two vectors.  ElementIndex0 is used to select
// an element from the vectors to be placed in the first
// component of the resulting vector, ElementIndex1 is used
// to select an element for the second component, etc.

XMFINLINE XMVECTOR XMVectorPermuteControl
(
    UINT     ElementIndex0, 
    UINT     ElementIndex1, 
    UINT     ElementIndex2, 
    UINT     ElementIndex3
)
{
    XMVECTOR    ControlVector;
    CONST UINT  ControlElement[] =
                {
                    XM_PERMUTE_0X,
                    XM_PERMUTE_0Y,
                    XM_PERMUTE_0Z,
                    XM_PERMUTE_0W,
                    XM_PERMUTE_1X,
                    XM_PERMUTE_1Y,
                    XM_PERMUTE_1Z,
                    XM_PERMUTE_1W
                };

    XMASSERT(ElementIndex0 < 8);
    XMASSERT(ElementIndex1 < 8);
    XMASSERT(ElementIndex2 < 8);
    XMASSERT(ElementIndex3 < 8);

    ControlVector.u[0] = ControlElement[ElementIndex0];
    ControlVector.u[1] = ControlElement[ElementIndex1];
    ControlVector.u[2] = ControlElement[ElementIndex2];
    ControlVector.u[3] = ControlElement[ElementIndex3];

    return ControlVector;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorPermute
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR Control
)
{
#if defined(_NO_INTRINSICS_)

    BYTE*    aByte[4];
    UINT     VectorIndex;
    UINT     ComponentIndex;
    UINT     i;
    XMVECTOR Result;

    // Indices must range from 0 to 31
    XMASSERT((Control.u[0] & 0xE0E0E0E0) == 0);
    XMASSERT((Control.u[1] & 0xE0E0E0E0) == 0);
    XMASSERT((Control.u[2] & 0xE0E0E0E0) == 0);
    XMASSERT((Control.u[3] & 0xE0E0E0E0) == 0);

    aByte[0] = (BYTE*)&V1;
    aByte[1] = (BYTE*)&V2;
    aByte[2] = (BYTE*)&Control;
    aByte[3] = (BYTE*)&Result;

    for (i = 0; i < 16; i++)
    {
        VectorIndex = aByte[2][i] / 16;
        ComponentIndex = aByte[2][i] % 16;
#if defined(_X86_)
        ComponentIndex ^= 3; // Swap byte ordering
#endif
        aByte[3][i] = aByte[VectorIndex][ComponentIndex];
    }

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    // XMVectorPermute should be used conservatively, since Control is only
    // able to be assigned to a possible 8 out of the 128 available registers.

    return __vperm(V1, V2, Control);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Define a control vector to be used in XMVectorSelect 
// operations.  The four integers specified in XMVectorSelectControl
// serve as indices to select between components in two vectors.
// The first index controls selection for the first component of 
// the vectors involved in a select operation, the second index 
// controls selection for the second component etc.  A value of
// zero for an index causes the corresponding component from the first 
// vector to be selected whereas a one causes the component from the
// second vector to be selected instead.

XMFINLINE XMVECTOR XMVectorSelectControl
(
    UINT VectorIndex0, 
    UINT VectorIndex1, 
    UINT VectorIndex2, 
    UINT VectorIndex3
)
{
    XMVECTOR    ControlVector;
    CONST UINT  ControlElement[] =
                {
                    XM_SELECT_0,
                    XM_SELECT_1
                };

    XMASSERT(VectorIndex0 < 2);
    XMASSERT(VectorIndex1 < 2);
    XMASSERT(VectorIndex2 < 2);
    XMASSERT(VectorIndex3 < 2);

    ControlVector.u[0] = ControlElement[VectorIndex0];
    ControlVector.u[1] = ControlElement[VectorIndex1];
    ControlVector.u[2] = ControlElement[VectorIndex2];
    ControlVector.u[3] = ControlElement[VectorIndex3];

    return ControlVector;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSelect
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR Control
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = (V1.u[0] & ~Control.u[0]) | (V2.u[0] & Control.u[0]);
    Result.u[1] = (V1.u[1] & ~Control.u[1]) | (V2.u[1] & Control.u[1]);
    Result.u[2] = (V1.u[2] & ~Control.u[2]) | (V2.u[2] & Control.u[2]);
    Result.u[3] = (V1.u[3] & ~Control.u[3]) | (V2.u[3] & Control.u[3]);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    // The result should be assigned to Control to avoid an implicit 
    // copy of Control if it is later referenced as an input (i.e.
    // Control = XMVectorSelect(V1, V2, Control);).

    return __vsel(V1, V2, Control);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorMergeXY
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = V1.u[0];
    Result.u[1] = V2.u[0];
    Result.u[2] = V1.u[1];
    Result.u[3] = V2.u[1];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vmrghw(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorMergeZW
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = V1.u[2];
    Result.u[1] = V2.u[2];
    Result.u[2] = V1.u[3];
    Result.u[3] = V2.u[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vmrglw(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Comparison operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V1.v[0] == V2.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V1.v[1] == V2.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V1.v[2] == V2.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V1.v[3] == V2.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpeqfp(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorEqualR
(
    UINT*    pCR,
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control = XMVectorEqual(V1, V2);

    *pCR = 0;

    if (XMVector4EqualInt(Control, XMVectorTrueInt()))
    {
        // All elements are equal
        *pCR |= XM_CRMASK_CR6TRUE;
    }
    else if (XMVector4EqualInt(Control, XMVectorFalseInt()))
    {
        // All elements are not equal
        *pCR |= XM_CRMASK_CR6FALSE;
    }

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpeqfpR(V1, V2, pCR);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Treat the components of the vectors as unsigned integers and
// compare individual bits between the two.  This is useful for
// comparing control vectors and result vectors returned from
// other comparison operations.

XMFINLINE XMVECTOR XMVectorEqualInt
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V1.u[0] == V2.u[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V1.u[1] == V2.u[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V1.u[2] == V2.u[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V1.u[3] == V2.u[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpequw(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorEqualIntR
(
    UINT*    pCR,
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control = XMVectorEqualInt(V1, V2);

    *pCR = 0;

    if (XMVector4EqualInt(Control, XMVectorTrueInt()))
    {
        // All elements are equal
        *pCR |= XM_CRMASK_CR6TRUE;
    }
    else if (XMVector4EqualInt(Control, XMVectorFalseInt()))
    {
        // All elements are not equal
        *pCR |= XM_CRMASK_CR6FALSE;
    }

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpequwR(V1, V2, pCR);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorNearEqual
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR Epsilon
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Delta;
    XMVECTOR Control;

    Delta = XMVectorSubtract(V1, V2);
    Delta = XMVectorAbs(Delta);

    Control.u[0] = (Delta.v[0] <= Epsilon.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (Delta.v[1] <= Epsilon.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (Delta.v[2] <= Epsilon.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (Delta.v[3] <= Epsilon.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Delta;
    XMVECTOR Zero;
    XMVECTOR Control;

    Delta = __vsubfp(V1, V2);
    Zero = __vspltisw(0);
    Control = __vcmpbfp(Delta, Epsilon);
    Control = __vcmpequw(Control, Zero);

    return Control;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorNotEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V1.v[0] != V2.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V1.v[1] != V2.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V1.v[2] != V2.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V1.v[3] != V2.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Control;

    Control = __vcmpeqfp(V1, V2);
    Control = __vnor(Control, Control);

    return Control;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorNotEqualInt
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V1.u[0] != V2.u[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V1.u[1] != V2.u[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V1.u[2] != V2.u[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V1.u[3] != V2.u[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Control;
    Control = __vcmpequw(V1, V2);
    Control = __vnor(Control, Control);
    return Control;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorGreater
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V1.v[0] > V2.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V1.v[1] > V2.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V1.v[2] > V2.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V1.v[3] > V2.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpgtfp(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorGreaterR
(
    UINT*    pCR,
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control = XMVectorGreater(V1, V2);

    *pCR = 0;

    if (XMVector4EqualInt(Control, XMVectorTrueInt()))
    {
        // All elements are greater
        *pCR |= XM_CRMASK_CR6TRUE;
    }
    else if (XMVector4EqualInt(Control, XMVectorFalseInt()))
    {
        // All elements are not greater
        *pCR |= XM_CRMASK_CR6FALSE;
    }

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpgtfpR(V1, V2, pCR);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorGreaterOrEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V1.v[0] >= V2.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V1.v[1] >= V2.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V1.v[2] >= V2.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V1.v[3] >= V2.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpgefp(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorGreaterOrEqualR
(
    UINT*    pCR,
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control = XMVectorGreaterOrEqual(V1, V2);

    *pCR = 0;

    if (XMVector4EqualInt(Control, XMVectorTrueInt()))
    {
        // All elements are greater or equal
        *pCR |= XM_CRMASK_CR6TRUE;
    }
    else if (XMVector4EqualInt(Control, XMVectorFalseInt()))
    {
        // All elements are not greater or equal
        *pCR |= XM_CRMASK_CR6FALSE;
    }

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpgefpR(V1, V2, pCR);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorLess
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V1.v[0] < V2.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V1.v[1] < V2.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V1.v[2] < V2.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V1.v[3] < V2.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpgtfp(V2, V1);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorLessOrEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V1.v[0] <= V2.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V1.v[1] <= V2.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V1.v[2] <= V2.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V1.v[3] <= V2.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vcmpgefp(V2, V1);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorInBounds
(
    XMVECTOR V, 
    XMVECTOR Bounds
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = (V.v[0] <= Bounds.v[0] && V.v[0] >= -Bounds.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = (V.v[1] <= Bounds.v[1] && V.v[1] >= -Bounds.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = (V.v[2] <= Bounds.v[2] && V.v[2] >= -Bounds.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = (V.v[3] <= Bounds.v[3] && V.v[3] >= -Bounds.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Control;
    XMVECTOR Zero;

    Zero = __vspltisw(0);
    Control = __vcmpbfp(V, Bounds);
    Control = __vcmpequw(Control, Zero);

    return Control;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorInBoundsR
(
    UINT*    pCR,
    XMVECTOR V, 
    XMVECTOR Bounds
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control = XMVectorInBounds(V, Bounds);

    *pCR = 0;

    if (XMVector4EqualInt(Control, XMVectorTrueInt()))
    {
        // All elements are in bounds
        *pCR |= XM_CRMASK_CR6BOUNDS;
    }

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Control;
    XMVECTOR Zero;

    Zero = __vspltisw(0);
    Control = __vcmpbfpR(V, Bounds, pCR);
    Control = __vcmpequw(Control, Zero);

    return Control;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorIsNaN
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = XMISNAN(V.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = XMISNAN(V.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = XMISNAN(V.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = XMISNAN(V.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Control;

    Control = __vcmpeqfp(V, V);
    Control = __vnor(Control, Control);

    return Control;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorIsInfinite
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Control;

    Control.u[0] = XMISINF(V.v[0]) ? 0xFFFFFFFF : 0;
    Control.u[1] = XMISINF(V.v[1]) ? 0xFFFFFFFF : 0;
    Control.u[2] = XMISINF(V.v[2]) ? 0xFFFFFFFF : 0;
    Control.u[3] = XMISINF(V.v[3]) ? 0xFFFFFFFF : 0;

    return Control;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR OneInt, TrueInt;
    XMVECTOR VShifted, InfinityShifted;
    XMVECTOR VEqualsInfinity;
    XMVECTOR Shift;

    OneInt = __vspltisw(1);
    TrueInt = __vspltisw(-1);
    Shift = __vspltisw(-8); // Shift 24
    VShifted = __vslw(V, OneInt);
    InfinityShifted = __vslw(TrueInt, Shift);
    VEqualsInfinity = __vcmpequw(VShifted, InfinityShifted);

    return VEqualsInfinity;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Rounding and clamping operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorMin
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = (V1.v[0] < V2.v[0]) ? V1.v[0] : V2.v[0];
    Result.v[1] = (V1.v[1] < V2.v[1]) ? V1.v[1] : V2.v[1];
    Result.v[2] = (V1.v[2] < V2.v[2]) ? V1.v[2] : V2.v[2];
    Result.v[3] = (V1.v[3] < V2.v[3]) ? V1.v[3] : V2.v[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vminfp(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorMax
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = (V1.v[0] > V2.v[0]) ? V1.v[0] : V2.v[0];
    Result.v[1] = (V1.v[1] > V2.v[1]) ? V1.v[1] : V2.v[1];
    Result.v[2] = (V1.v[2] > V2.v[2]) ? V1.v[2] : V2.v[2];
    Result.v[3] = (V1.v[3] > V2.v[3]) ? V1.v[3] : V2.v[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vmaxfp(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorRound
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR       Result;
    XMVECTOR       Bias;
    CONST XMVECTOR Zero = XMVectorZero();
    CONST XMVECTOR BiasPos = XMVectorReplicate(0.5f);
    CONST XMVECTOR BiasNeg = XMVectorReplicate(-0.5f);

    Bias = XMVectorLess(V, Zero);
    Bias = XMVectorSelect(BiasPos, BiasNeg, Bias);
    Result = XMVectorAdd(V, Bias);
    Result = XMVectorTruncate(Result);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vrfin(V);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorTruncate
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = (FLOAT)(INT)V.v[0];
    Result.v[1] = (FLOAT)(INT)V.v[1];
    Result.v[2] = (FLOAT)(INT)V.v[2];
    Result.v[3] = (FLOAT)(INT)V.v[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vrfiz(V);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorFloor
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = floorf(V.v[0]);
    Result.v[1] = floorf(V.v[1]);
    Result.v[2] = floorf(V.v[2]);
    Result.v[3] = floorf(V.v[3]);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vrfim(V);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorCeiling
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = ceilf(V.v[0]);
    Result.v[1] = ceilf(V.v[1]);
    Result.v[2] = ceilf(V.v[2]);
    Result.v[3] = ceilf(V.v[3]);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vrfip(V);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorClamp
(
    XMVECTOR V, 
    XMVECTOR Min, 
    XMVECTOR Max
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    XMASSERT(XMVector4LessOrEqual(Min, Max));

    Result = XMVectorMax(Min, V);
    Result = XMVectorMin(Max, Result);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Result;

    XMASSERT(XMVector4LessOrEqual(Min, Max));

    Result = __vmaxfp(Min, V);
    Result = __vminfp(Max, Result);

    return Result;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSaturate
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    CONST XMVECTOR Zero = XMVectorZero();

    return XMVectorClamp(V, Zero, g_XMOne);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Zero;
    XMVECTOR One;
    XMVECTOR Result;

    One = __vspltisw(1);
    One = __vcfsx(One, 0);
    Zero = __vspltisw(0);

    Result = __vmaxfp(Zero, V);
    Result = __vminfp(One, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Zero;
    XMVECTOR One;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(One);

    One = __vupkd3d(One, VPACK_NORMSHORT2);
    Zero = __vspltisw(0);
    One = __vspltw(One, 3);

    Result = __vmaxfp(Zero, V);
    Result = __vminfp(One, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Bitwise logical operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorAndInt
(
    XMVECTOR V1,
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = V1.u[0] & V2.u[0];
    Result.u[1] = V1.u[1] & V2.u[1];
    Result.u[2] = V1.u[2] & V2.u[2];
    Result.u[3] = V1.u[3] & V2.u[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vand(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorAndCInt
(
    XMVECTOR V1,
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = V1.u[0] & ~V2.u[0];
    Result.u[1] = V1.u[1] & ~V2.u[1];
    Result.u[2] = V1.u[2] & ~V2.u[2];
    Result.u[3] = V1.u[3] & ~V2.u[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vandc(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorOrInt
(
    XMVECTOR V1,
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = V1.u[0] | V2.u[0];
    Result.u[1] = V1.u[1] | V2.u[1];
    Result.u[2] = V1.u[2] | V2.u[2];
    Result.u[3] = V1.u[3] | V2.u[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vor(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorNorInt
(
    XMVECTOR V1,
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = ~(V1.u[0] | V2.u[0]);
    Result.u[1] = ~(V1.u[1] | V2.u[1]);
    Result.u[2] = ~(V1.u[2] | V2.u[2]);
    Result.u[3] = ~(V1.u[3] | V2.u[3]);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vnor(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorXorInt
(
    XMVECTOR V1,
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.u[0] = V1.u[0] ^ V2.u[0];
    Result.u[1] = V1.u[1] ^ V2.u[1];
    Result.u[2] = V1.u[2] ^ V2.u[2];
    Result.u[3] = V1.u[3] ^ V2.u[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vxor(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorNegate
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = -V.v[0];
    Result.v[1] = -V.v[1];
    Result.v[2] = -V.v[2];
    Result.v[3] = -V.v[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR SignMask;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    Result = __vxor(V, SignMask);

    return Result;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorAdd
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = V1.v[0] + V2.v[0];
    Result.v[1] = V1.v[1] + V2.v[1];
    Result.v[2] = V1.v[2] + V2.v[2];
    Result.v[3] = V1.v[3] + V2.v[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vaddfp(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorAddAngles
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR       Mask;
    XMVECTOR       Offset;
    XMVECTOR       Result;
    CONST XMVECTOR Zero = XMVectorZero();

    // Add the given angles together.  If the range of V1 is such
    // that -Pi <= V1 < Pi and the range of V2 is such that
    // -2Pi <= V2 <= 2Pi, then the range of the resulting angle
    // will be -Pi <= Result < Pi.
    Result = XMVectorAdd(V1, V2);

    Mask = XMVectorLess(Result, g_XMNegativePi);
    Offset = XMVectorSelect(Zero, g_XMTwoPi, Mask);

    Mask = XMVectorGreaterOrEqual(Result, g_XMPi);
    Offset = XMVectorSelect(Offset, g_XMNegativeTwoPi, Mask);

    Result = XMVectorAdd(Result, Offset);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Mask1;
    XMVECTOR Mask2;
    XMVECTOR Offset;
    XMVECTOR Zero;
    XMVECTOR Result;

    Result = __vaddfp(V1, V2);
    Zero = __vspltisw(0);
    Mask1 = __vcmpgtfp(g_XMNegativePi, Result);
    Mask2 = __vcmpgefp(Result, g_XMPi);
    Offset = __vsel(Zero, g_XMTwoPi, Mask1);
    Offset = __vsel(Offset, g_XMNegativeTwoPi, Mask2);
    Result = __vaddfp(Result, Offset);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Offset1;
    XMVECTOR Offset2;
    XMVECTOR NegativePiConstants0, SignMask;
    XMVECTOR Pi, NegativePi, TwoPi, NegativeTwoPi;
    XMVECTOR Zero;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    Result = __vaddfp(V1, V2);
    NegativePiConstants0 = __vor(g_XMPiConstants0, SignMask);
    Pi = __vspltw(g_XMPiConstants0, 0);
    Zero = __vspltisw(0);
    TwoPi = __vspltw(g_XMPiConstants0, 1);
    NegativePi = __vspltw(NegativePiConstants0, 0);
    NegativeTwoPi = __vspltw(NegativePiConstants0, 1);
    Offset1 = __vcmpgtfp(NegativePi, Result);
    Offset2 = __vcmpgefp(Result, Pi);
    Offset1 = __vsel(Zero, TwoPi, Offset1);
    Offset2 = __vsel(Offset1, NegativeTwoPi, Offset2);
    Result = __vaddfp(Result, Offset2);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSubtract
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = V1.v[0] - V2.v[0];
    Result.v[1] = V1.v[1] - V2.v[1];
    Result.v[2] = V1.v[2] - V2.v[2];
    Result.v[3] = V1.v[3] - V2.v[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vsubfp(V1, V2);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSubtractAngles
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR       Mask;
    XMVECTOR       Offset;
    XMVECTOR       Result;
    CONST XMVECTOR Zero = XMVectorZero();

    // Subtract the given angles.  If the range of V1 is such
    // that -Pi <= V1 < Pi and the range of V2 is such that
    // -2Pi <= V2 <= 2Pi, then the range of the resulting angle
    // will be -Pi <= Result < Pi.
    Result = XMVectorSubtract(V1, V2);

    Mask = XMVectorLess(Result, g_XMNegativePi);
    Offset = XMVectorSelect(Zero, g_XMTwoPi, Mask);

    Mask = XMVectorGreaterOrEqual(Result, g_XMPi);
    Offset = XMVectorSelect(Offset, g_XMNegativeTwoPi, Mask);

    Result = XMVectorAdd(Result, Offset);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Mask1;
    XMVECTOR Mask2;
    XMVECTOR Offset;
    XMVECTOR Zero;
    XMVECTOR Result;

    Result = __vsubfp(V1, V2);
    Zero = __vspltisw(0);
    Mask1 = __vcmpgtfp(g_XMNegativePi, Result);
    Mask2 = __vcmpgefp(Result, g_XMPi);
    Offset = __vsel(Zero, g_XMTwoPi, Mask1);
    Offset = __vsel(Offset, g_XMNegativeTwoPi, Mask2);
    Result = __vaddfp(Result, Offset);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Offset1;
    XMVECTOR Offset2;
    XMVECTOR NegativePiConstants0, SignMask;
    XMVECTOR Pi, NegativePi, TwoPi, NegativeTwoPi;
    XMVECTOR Zero;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    Result = __vsubfp(V1, V2);
    NegativePiConstants0 = __vor(g_XMPiConstants0, SignMask);
    Pi = __vspltw(g_XMPiConstants0, 0);
    Zero = __vspltisw(0);
    TwoPi = __vspltw(g_XMPiConstants0, 1);
    NegativePi = __vspltw(NegativePiConstants0, 0);
    NegativeTwoPi = __vspltw(NegativePiConstants0, 1);
    Offset1 = __vcmpgtfp(NegativePi, Result);
    Offset2 = __vcmpgefp(Result, Pi);
    Offset1 = __vsel(Zero, TwoPi, Offset1);
    Offset2 = __vsel(Offset1, NegativeTwoPi, Offset2);
    Result = __vaddfp(Result, Offset2);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorMultiply
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = V1.v[0] * V2.v[0];
    Result.v[1] = V1.v[1] * V2.v[1];
    Result.v[2] = V1.v[2] * V2.v[2];
    Result.v[3] = V1.v[3] * V2.v[3];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Result;
    Result = __vmaddfp(V1, V2, *(XMVECTOR*)g_XMNegativeZero);
    return Result;

#else // _VMX128_INTRINSICS_

    return __vmulfp(V1, V2);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorMultiplyAdd
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR V3
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = V1.v[0] * V2.v[0] + V3.v[0];
    Result.v[1] = V1.v[1] * V2.v[1] + V3.v[1];
    Result.v[2] = V1.v[2] * V2.v[2] + V3.v[2];
    Result.v[3] = V1.v[3] * V2.v[3] + V3.v[3];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    // For best performance, the result should be assigned back to either V2 or V3.

    return __vmaddfp(V1, V2, V3);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorNegativeMultiplySubtract
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR V3
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = -(V1.v[0] * V2.v[0] - V3.v[0]);
    Result.v[1] = -(V1.v[1] * V2.v[1] - V3.v[1]);
    Result.v[2] = -(V1.v[2] * V2.v[2] - V3.v[2]);
    Result.v[3] = -(V1.v[3] * V2.v[3] - V3.v[3]);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    // For best performance, the result should be assigned back to V3.

    return __vnmsubfp(V1, V2, V3);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorScale
(
    XMVECTOR V, 
    FLOAT    ScaleFactor
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = V.v[0] * ScaleFactor;
    Result.v[1] = V.v[1] * ScaleFactor;
    Result.v[2] = V.v[2] * ScaleFactor;
    Result.v[3] = V.v[3] * ScaleFactor;

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Scale;
    XMVECTOR Result;

    Scale.v[0] = 
    Scale.v[1] = 
    Scale.v[2] = 
    Scale.v[3] = ScaleFactor;

    Result = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR Scale;
    XMVECTOR Result;

    Scale.v[0] = 
    Scale.v[1] = 
    Scale.v[2] = 
    Scale.v[3] = ScaleFactor;

    Result = __vmulfp(V, Scale);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Scale;
    XMVECTOR Result;
    XMASSERT(((UINT_PTR)&ScaleFactor & 3) == 0);

    Scale = __lvlx(&ScaleFactor, 0);
    Scale = __vspltw(Scale, 0);

    Result = __vmulfp(V, Scale);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorReciprocalEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;
    UINT     i;

    // Avoid C4701
    Result.v[0] = 0.0f;

    for (i = 0; i < 4; i++)
    {
        if (XMISINF(V.v[i]))
        {
            Result.v[i] = (V.v[i] < 0.0f) ? -0.0f : 0.0f;
        }
        else if (V.v[i] == -0.0f)
        {
            Result.u[i] = 0xFF800000;
        }
        else if (V.v[i] == 0.0f)
        {
            Result.u[i] = 0x7F800000;
        }
        else
        {
            Result.v[i] = 1.0f / V.v[i];
        }
    }

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vrefp(V);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorReciprocal
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return XMVectorReciprocalEst(V);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Reciprocal;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR Result;

    Reciprocal = __vrefp(V);

    // First refinement iteration (Newton-Raphson) for 1.0 / x
    //     y0 = reciprocal_estimate(x)
    //     y1 = y0 + y0 * (1.0 - x * y0)
    Scale = __vnmsubfp(V, Reciprocal, g_XMOne);
    Result = __vmaddfp(Reciprocal, Scale, Reciprocal);

    // Second refinement iteration
    //     y2 = y1 + y1 * (1.0 - x * y1)
    Scale = __vnmsubfp(V, Result, g_XMOne);

    Refine = __vcmpeqfp(Result, Result);

    Result = __vmaddfp(Result, Scale, Result);

    Result = __vsel(Reciprocal, Result, Refine);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR Refine;

    XMDUMMY_INITIALIZE_VECTOR(S1);

    Reciprocal = __vrefp(V);

    // First refinement iteration (Newton-Raphson) for 1.0 / x
    //     y0 = reciprocal_estimate(x)
    //     y1 = y0 + y0 * (1.0 - x * y0)
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(V, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);

    // Second refinement iteration
    //     y2 = y1 + y1 * (1.0 - x * y1)
    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(V, S0, S1);

    Refine = __vcmpeqfp(S0, S0);

    S0 = __vmaddfp(S0, S1, S0);

    Refine = __vsel(Reciprocal, S0, Refine);

    return Refine;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSqrtEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

#if 0
    // Faster, but returns QNaN if the input is zero or infinity
    Result = XMVectorReciprocalSqrtEst(V);
    Result = XMVectorMultiply(V, Result);
#else
    XMVECTOR Zero;
    XMVECTOR VEqualsInfinity, VEqualsZero;
    XMVECTOR Select;

    // if (x == +Infinity)  sqrt(x) = +Infinity
    // if (x == +0.0f)      sqrt(x) = +0.0f
    // if (x == -0.0f)      sqrt(x) = -0.0f
    // if (x < -0.0f)       sqrt(x) = QNaN

    Result = XMVectorReciprocalSqrtEst(V);
    Zero = XMVectorZero();
    VEqualsInfinity = XMVectorEqualInt(V, *(XMVECTOR*)g_XMInfinity);
    VEqualsZero = XMVectorEqual(V, Zero);
    Result = XMVectorMultiply(V, Result);
    Select = XMVectorEqualInt(VEqualsInfinity, VEqualsZero);
    Result = XMVectorSelect(V, Result, Select);
#endif

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Result;

#if 0
    Result = __vrsqrtefp(V);
    Result = __vmaddfp(V, Result, *(XMVECTOR*)g_XMNegativeZero);
#else
    XMVECTOR Zero;
    XMVECTOR VEqualsInfinity, VEqualsZero;
    XMVECTOR Select;

    Result = __vrsqrtefp(V);
    Zero = __vspltisw(0);
    VEqualsInfinity = __vcmpequw(V, *(XMVECTOR*)g_XMInfinity);
    VEqualsZero = __vcmpeqfp(V, Zero); // True for V.v[n] == +0.0f or V.v[n] == -0.0f
    Result = __vmaddfp(V, Result, *(XMVECTOR*)g_XMNegativeZero);
//    Select = __vor(VEqualsInfinity, VEqualsZero);
//    Result = __vsel(Result, V, Select);
    Select = __vcmpequw(VEqualsInfinity, VEqualsZero);
    Result = __vsel(V, Result, Select);
#endif

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Result;

#if 0
    Result = __vrsqrtefp(V);
    Result = __vmulfp(V, Result);
#else
    XMVECTOR OneInt, TrueInt, Zero;
    XMVECTOR VShifted, InfinityShifted;
    XMVECTOR VEqualsInfinity, VEqualsZero;
    XMVECTOR Shift;
    XMVECTOR Rsq;

    Rsq = __vrsqrtefp(V);
    OneInt = __vspltisw(1);
    TrueInt = __vspltisw(-1);
    Shift = __vspltisw(-8); // Shift 24
    Zero = __vspltisw(0);
    VShifted = __vslw(V, OneInt);
    InfinityShifted = __vslw(TrueInt, Shift);
    VEqualsZero = __vcmpeqfp(V, Zero);
    VEqualsInfinity = __vcmpequw(VShifted, InfinityShifted);
    Rsq = __vmulfp(V, Rsq);
//    Result = __vor(VEqualsInfinity, VEqualsZero);
//    Result = __vsel(Rsq, V, Result);
    Result = __vcmpequw(VEqualsInfinity, VEqualsZero);
    Result = __vsel(V, Rsq, Result);
#endif

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSqrt
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Zero;
    XMVECTOR VEqualsInfinity, VEqualsZero;
    XMVECTOR Select;
    XMVECTOR Result;

    // if (x == +Infinity)  sqrt(x) = +Infinity
    // if (x == +0.0f)      sqrt(x) = +0.0f
    // if (x == -0.0f)      sqrt(x) = -0.0f
    // if (x < -0.0f)       sqrt(x) = QNaN

    Result = XMVectorReciprocalSqrt(V);
    Zero = XMVectorZero();
    VEqualsInfinity = XMVectorEqualInt(V, *(XMVECTOR*)g_XMInfinity);
    VEqualsZero = XMVectorEqual(V, Zero);
    Result = XMVectorMultiply(V, Result);
    Select = XMVectorEqualInt(VEqualsInfinity, VEqualsZero);
    Result = XMVectorSelect(V, Result, Select);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR ReciprocalSqrt;
    XMVECTOR OneHalfV;
    XMVECTOR Reciprocal;
    XMVECTOR Scale;
    XMVECTOR RsqNotNaN;
    XMVECTOR ScaleNotNaN;
    XMVECTOR Select;
    XMVECTOR Result;

    // Refinement (Newton-Raphson) for 1.0 / sqrt(x)
    //     y0 = reciprocal_sqrt_estimate(x)
    //     y1 = y0 + 0.5 * y0 * (1.0 - x * y0 * y0) 
    //        = y0 + y0 * (0.5 - 0.5 * x * y0 * y0)

    ReciprocalSqrt = __vrsqrtefp(V);
    OneHalfV = __vmaddfp(V, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Reciprocal = __vmaddfp(ReciprocalSqrt, ReciprocalSqrt, *(XMVECTOR*)g_XMNegativeZero);
    RsqNotNaN = __vcmpeqfp(ReciprocalSqrt, ReciprocalSqrt); // TRUE if V >= -0.0f
    Scale = __vnmsubfp(OneHalfV, Reciprocal, g_XMOneHalf);
    Result = __vmaddfp(ReciprocalSqrt, Scale, ReciprocalSqrt);
    ScaleNotNaN = __vcmpeqfp(Scale, Scale); // TRUE if V > 0.0f and V < +Infinity
    Select = __vxor(ScaleNotNaN, RsqNotNaN); // Select result if Scale is not NaN or V < -0.0f
    Result = __vmaddfp(V, Result, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vsel(Result, V, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR OneHalfV;
    XMVECTOR Reciprocal;
    XMVECTOR Scale;
    XMVECTOR RsqNotNaN;
    XMVECTOR ScaleNotNaN;
    XMVECTOR Select;
    XMVECTOR Result;

    // Refinement (Newton-Raphson) for 1.0 / sqrt(x)
    //     y0 = reciprocal_sqrt_estimate(x)
    //     y1 = y0 + 0.5 * y0 * (1.0 - x * y0 * y0) 
    //        = y0 + y0 * (0.5 - 0.5 * x * y0 * y0)

    Scale = __vspltisw(1);
    Result = __vrsqrtefp(V);
    Scale = __vcfsx(Scale, 1);
    OneHalfV = __vmulfp(V, Scale);
    Reciprocal = __vmulfp(Result, Result);
    RsqNotNaN = __vcmpeqfp(Result, Result); // TRUE if V >= -0.0f
    Scale = __vnmsubfp(OneHalfV, Reciprocal, Scale);
    Result = __vmaddfp(Result, Scale, Result);
    ScaleNotNaN = __vcmpeqfp(Scale, Scale); // TRUE if V > 0.0f and V < +Infinity
    Select = __vxor(ScaleNotNaN, RsqNotNaN); // Select result if Scale is not NaN or V < -0.0f
    Result = __vmulfp(V, Result);
    Select = __vsel(Result, V, Select);

    return Select;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorReciprocalSqrtEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;
    UINT     i;

    // Avoid C4701
    Result.v[0] = 0.0f;

    for (i = 0; i < 4; i++)
    {
        if (V.v[i] == 0.0f)
        {
            Result.u[i] = 0x7F800000;
        }
        else if (V.v[i] == -0.0f)
        {
            Result.u[i] = 0xFF800000;
        }
        else if (V.v[i] < 0.0f)
        {
            Result.u[i] = 0x7FFFFFFF;
        }
        else if (XMISINF(V.v[i]))
        {
            Result.v[i] = 0.0f;
        }
        else
        {
            Result.v[i] = 1.0f / sqrtf(V.v[i]);
        }
    }

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vrsqrtefp(V);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorReciprocalSqrt
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return XMVectorReciprocalSqrtEst(V);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR ReciprocalSqrt;
    XMVECTOR OneHalfV;
    XMVECTOR Reciprocal;
    XMVECTOR Scale;
    XMVECTOR Result;

    // Refinement (Newton-Raphson) for 1.0 / sqrt(x)
    //     y0 = reciprocal_sqrt_estimate(x)
    //     y1 = y0 + 0.5 * y0 * (1.0 - x * y0 * y0) 
    //        = y0 + y0 * (0.5 - 0.5 * x * y0 * y0)
    ReciprocalSqrt = __vrsqrtefp(V);
    OneHalfV = __vmaddfp(V, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Reciprocal = __vmaddfp(ReciprocalSqrt, ReciprocalSqrt, *(XMVECTOR*)g_XMNegativeZero);
    Scale = __vnmsubfp(OneHalfV, Reciprocal, g_XMOneHalf);
    Result = __vmaddfp(ReciprocalSqrt, Scale, ReciprocalSqrt);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR ReciprocalSqrt;
    XMVECTOR OneHalfV;
    XMVECTOR Reciprocal;
    XMVECTOR Scale;

    // Refinement (Newton-Raphson) for 1.0 / sqrt(x)
    //     y0 = reciprocal_sqrt_estimate(x)
    //     y1 = y0 + 0.5 * y0 * (1.0 - x * y0 * y0) 
    //        = y0 + y0 * (0.5 - 0.5 * x * y0 * y0)
    Scale = __vspltisw(1);
    ReciprocalSqrt = __vrsqrtefp(V);
    Scale = __vcfsx(Scale, 1);
    OneHalfV = __vmulfp(V, Scale);
    Reciprocal = __vmulfp(ReciprocalSqrt, ReciprocalSqrt);
    Scale = __vnmsubfp(OneHalfV, Reciprocal, Scale);
    ReciprocalSqrt = __vmaddfp(ReciprocalSqrt, Scale, ReciprocalSqrt);

    return ReciprocalSqrt;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorExpEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = powf(2.0f, V.v[0]);
    Result.v[1] = powf(2.0f, V.v[1]);
    Result.v[2] = powf(2.0f, V.v[2]);
    Result.v[3] = powf(2.0f, V.v[3]);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vexptefp(V);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorExp
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               E, S;
    XMVECTOR               R, R2, R3, R4;
    XMVECTOR               V0, V1;
    XMVECTOR               C0X, C0Y, C0Z, C0W;
    XMVECTOR               C1X, C1Y, C1Z, C1W;
    XMVECTOR               Result;
    static CONST XMVECTOR  C0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  C1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};

    R = XMVectorFloor(V);
    E = XMVectorExpEst(R);
    R = XMVectorSubtract(V, R);
    R2 = XMVectorMultiply(R, R);
    R3 = XMVectorMultiply(R, R2);
    R4 = XMVectorMultiply(R2, R2);

    C0X = XMVectorSplatX(C0);
    C0Y = XMVectorSplatY(C0);
    C0Z = XMVectorSplatZ(C0);
    C0W = XMVectorSplatW(C0);

    C1X = XMVectorSplatX(C1);
    C1Y = XMVectorSplatY(C1);
    C1Z = XMVectorSplatZ(C1);
    C1W = XMVectorSplatW(C1);

    V0 = XMVectorMultiplyAdd(R, C0Y, C0X);
    V0 = XMVectorMultiplyAdd(R2, C0Z, V0);
    V0 = XMVectorMultiplyAdd(R3, C0W, V0);

    V1 = XMVectorMultiplyAdd(R, C1Y, C1X);
    V1 = XMVectorMultiplyAdd(R2, C1Z, V1);
    V1 = XMVectorMultiplyAdd(R3, C1W, V1);

    S = XMVectorMultiplyAdd(R4, V1, V0);

    S = XMVectorReciprocal(S);
    Result = XMVectorMultiply(E, S);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               E, S;
    XMVECTOR               R, R2, R3, R4;
    XMVECTOR               V0, V1;
    XMVECTOR               C0X, C0Y, C0Z, C0W;
    XMVECTOR               C1X, C1Y, C1Z, C1W;
    XMVECTOR               Reciprocal, Rcp, Scale, Refine;
    XMVECTOR               Result;
    static CONST XMVECTOR  C0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  C1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};

    R = __vrfim(V);
    E = __vexptefp(R);

    C0X = __vspltw(C0, 0);
    C0Y = __vspltw(C0, 1);
    C1X = __vspltw(C1, 0);
    C1Y = __vspltw(C1, 1);

    R = __vsubfp(V, R);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2 = __vmaddfp(R, R, *(XMVECTOR*)g_XMNegativeZero);

    V0 = __vmaddfp(R, C0Y, C0X);
    V1 = __vmaddfp(R, C1Y, C1X);

    R3 = __vmaddfp(R, R2, *(XMVECTOR*)g_XMNegativeZero);

    V0 = __vmaddfp(R2, C0Z, V0);
    V1 = __vmaddfp(R2, C1Z, V1);

    V0 = __vmaddfp(R3, C0W, V0);

    R4 = __vmaddfp(R2, R2, *(XMVECTOR*)g_XMNegativeZero);

    V1 = __vmaddfp(R3, C1W, V1);

    S = __vmaddfp(R4, V1, V0);

    Reciprocal = __vrefp(S);
    Scale = __vnmsubfp(S, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);
    Scale = __vnmsubfp(S, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    S = __vsel(Reciprocal, Rcp, Refine);

    Result = __vmaddfp(E, S, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR               E;
    XMVECTOR               R, R2, R3, R4;
    XMVECTOR               V0, V1;
    XMVECTOR               C0Y, C0Z, C0W;
    XMVECTOR               C1Y, C1Z, C1W;
    XMVECTOR               Reciprocal, Scale1, Scale2, Refine;
    XMVECTOR               Result;
    static CONST XMVECTOR  C0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  C1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};

    R = __vrfim(V);
    E = __vexptefp(R);

    V0 = __vspltw(C0, 0);
    C0Y = __vspltw(C0, 1);
    V1 = __vspltw(C1, 0);
    C1Y = __vspltw(C1, 1);

    R = __vsubfp(V, R);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2 = __vmulfp(R, R);

    V0 = __vmaddfp(R, C0Y, V0);
    V1 = __vmaddfp(R, C1Y, V1);

    R3 = __vmulfp(R, R2);

    V0 = __vmaddfp(R2, C0Z, V0);
    V1 = __vmaddfp(R2, C1Z, V1);

    V0 = __vmaddfp(R3, C0W, V0);

    R4 = __vmulfp(R2, R2);

    V1 = __vmaddfp(R3, C1W, V1);

    V0 = __vmaddfp(R4, V1, V0);

    Reciprocal = __vrefp(V0);
    Scale1 = __vspltw(C0, 0);
    Scale1 = __vnmsubfp(V0, Reciprocal, Scale1);
    Scale1 = __vmaddcfp(Reciprocal, Scale1, Reciprocal);
    Scale2 = __vspltw(C0, 0);
    Scale2 = __vnmsubfp(V0, Scale1, Scale2);
    Refine = __vcmpeqfp(Scale1, Scale1);
    Scale1 = __vmaddfp(Scale1, Scale2, Scale1);
    Refine = __vsel(Reciprocal, Scale1, Refine);

    Result = __vmulfp(E, Refine);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorLogEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR       Result;
    CONST XMVECTOR Scale = XMVectorReplicate(1.0f / logf(2.0f));

    Result.v[0] = logf(V.v[0]);
    Result.v[1] = logf(V.v[1]);
    Result.v[2] = logf(V.v[2]);
    Result.v[3] = logf(V.v[3]);

    Result = XMVectorMultiply(Result, Scale);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    return __vlogefp(V);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorLog
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return XMVectorLogEst(V);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               L, S;
    XMVECTOR               R, R2, R3, R4;
    XMVECTOR               V0, V1;
    XMVECTOR               C0X, C0Y, C0Z, C0W;
    XMVECTOR               C1X, C1Y, C1Z, C1W;
    XMVECTOR               Result;
    static CONST XMVECTOR  C0 = {1.44268966f, -7.21165776e-1f, 4.78684813e-1f, -3.47305417e-1f};
    static CONST XMVECTOR  C1 = {2.41873696e-1f, -1.37531206e-1f, 5.20646796e-2f, -9.31049418e-3f};
    static CONST XMVECTORI Mask = {0xFF800000, 0xFF800000, 0xFF800000, 0xFF800000};

    R = __vsel(V, g_XMOne, *(XMVECTOR*)Mask);

    C0X = __vspltw(C0, 0);
    C0Y = __vspltw(C0, 1);
    C1X = __vspltw(C1, 0);
    C1Y = __vspltw(C1, 1);

    R = __vsubfp(R, g_XMOne);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2 = __vmaddfp(R, R, *(XMVECTOR*)g_XMNegativeZero);

    V0 = __vmaddfp(R, C0Y, C0X);
    V1 = __vmaddfp(R, C1Y, C1X);

    R3 = __vmaddfp(R, R2, *(XMVECTOR*)g_XMNegativeZero);

    V0 = __vmaddfp(R2, C0Z, V0);
    V1 = __vmaddfp(R2, C1Z, V1);

    L = __vlogefp(V);

    V0 = __vmaddfp(R3, C0W, V0);

    R4 = __vmaddfp(R2, R2, *(XMVECTOR*)g_XMNegativeZero);

    V1 = __vmaddfp(R3, C1W, V1);

    L = __vrfim(L);

    S = __vmaddfp(R4, V1, V0);

    Result = __vmaddfp(R, S, L);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR               L;
    XMVECTOR               R, R2, R3, R4;
    XMVECTOR               V0, V1;
    XMVECTOR               C0Y, C0Z, C0W;
    XMVECTOR               C1Y, C1Z, C1W;
    XMVECTOR               One, Shift;
    static CONST XMVECTOR  C0 = {1.44268966f, -7.21165776e-1f, 4.78684813e-1f, -3.47305417e-1f};
    static CONST XMVECTOR  C1 = {2.41873696e-1f, -1.37531206e-1f, 5.20646796e-2f, -9.31049418e-3f};

    XMDUMMY_INITIALIZE_VECTOR(One);

    One = __vupkd3d(One, VPACK_NORMSHORT2);
    One = __vspltw(One, 3);

    R = __vspltisw(-1);
    Shift = __vspltisw(-9); // Shift 23
    R = __vslw(R, Shift);

    R = __vsel(V, One, R);

    V0 = __vspltw(C0, 0);
    C0Y = __vspltw(C0, 1);
    V1 = __vspltw(C1, 0);
    C1Y = __vspltw(C1, 1);

    R = __vsubfp(R, One);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2 = __vmulfp(R, R);

    V0 = __vmaddfp(R, C0Y, V0);
    V1 = __vmaddfp(R, C1Y, V1);

    R3 = __vmulfp(R, R2);

    V0 = __vmaddfp(R2, C0Z, V0);
    V1 = __vmaddfp(R2, C1Z, V1);

    L = __vlogefp(V);

    V0 = __vmaddfp(R3, C0W, V0);

    R4 = __vmulfp(R2, R2);

    V1 = __vmaddfp(R3, C1W, V1);

    L = __vrfim(L);

    V0 = __vmaddfp(R4, V1, V0);

    L = __vmaddfp(R, V0, L);

    return L;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorPowEst
(
    XMVECTOR V1,
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = powf(V1.v[0], V2.v[0]);
    Result.v[1] = powf(V1.v[1], V2.v[1]);
    Result.v[2] = powf(V1.v[2], V2.v[2]);
    Result.v[3] = powf(V1.v[3], V2.v[3]);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR AbsV1;
    XMVECTOR V1IsNegative, V1IsZero, V1Sign, V2IsNegative, V2IsZero, V1OrV2IsZero, V2Int, V2IsInt, ResultIsAlt, ResultAlt;
    XMVECTOR Shift, SignMask, Sign;
    XMVECTOR Zero, SignedZero, OneInt;
    XMVECTOR Result;

    Shift = __vspltisw(-1); // Shift 31
    SignMask = __vslw(Shift, Shift);

    Zero = XMVectorZero();
    V1IsNegative = XMVectorLess(V1, Zero);
    V2IsNegative = XMVectorLess(V2, Zero);
    V1Sign = XMVectorAndInt(V1, SignMask);

    OneInt = XMVectorSplatConstantInt(1);
    V2Int = XMConvertVectorFloatToInt(V2, 0);
    Sign = XMVectorAndInt(V2Int, OneInt);
    Sign = __vslw(Sign, Shift);
    Sign = XMVectorAndInt(V1Sign, Sign);

    V2IsInt = XMVectorTruncate(V2);
    V2IsInt = XMVectorEqual(V2, V2IsInt);

    V1IsZero = XMVectorEqual(V1, Zero);
    V2IsZero = XMVectorEqual(V2, Zero);
    V1OrV2IsZero = XMVectorOrInt(V1IsZero, V2IsZero);
    SignedZero = XMVectorOrInt(Zero, Sign);

    ResultAlt = XMVectorAndCInt(V1IsZero, V2IsNegative);
    ResultAlt = XMVectorSelect(*(XMVECTOR*)g_XMQNaN, SignedZero, ResultAlt);
    ResultAlt = XMVectorSelect(ResultAlt, g_XMOne, V2IsZero);

    ResultIsAlt = XMVectorAndCInt(V1IsNegative, V2IsInt);
    ResultIsAlt = XMVectorOrInt(ResultIsAlt, V1OrV2IsZero);

    AbsV1 = XMVectorAbs(V1);

    Result = XMVectorLogEst(AbsV1);
    Result = XMVectorMultiply(Result, V2);
    Result = XMVectorExpEst(Result);

    Result = XMVectorOrInt(Result, Sign);
    Result = XMVectorSelect(Result, ResultAlt, ResultIsAlt);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR AbsV1, PowEst;
    XMVECTOR V1IsNegative, V1IsZero, V1Sign, V2IsNegative, V1OrV2IsZero, V2Int, V2IsInt, ResultIsAlt, Alt, ResultAlt;
    XMVECTOR Shift, SignMask, Sign;
    XMVECTOR Zero, SignedZero, One, OneInt, QNaN;

    XMDUMMY_INITIALIZE_VECTOR(One);

    Shift = __vspltisw(-1); // Shift 31
    SignMask = __vslw(Shift, Shift);

    AbsV1 = __vandc(V1, SignMask);

    PowEst = __vlogefp(AbsV1);

    V2IsInt = __vrfiz(V2);

    OneInt = __vspltisw(1);

    Zero = __vspltisw(0);

    QNaN = __vspltisw(-9); // Shift 23

    V2Int = __vctsxs(V2, 0);

    PowEst = __vmulfp(PowEst, V2);

    Sign = __vand(V2Int, OneInt);
    V1Sign = __vand(V1, SignMask);
    QNaN = __vslw(Shift, QNaN);

    Sign = __vslw(Sign, Shift);
    V2IsNegative = __vcmpgtfp(Zero, V2);
    V1IsZero = __vcmpeqfp(V1, Zero);
    ResultAlt = __vcmpeqfp(V2, Zero); // V2IsZero
    QNaN = __vsrw(QNaN, OneInt);
    Sign = __vand(V1Sign, Sign);
    V1IsNegative = __vcmpgtfp(Zero, V1);

    PowEst = __vexptefp(PowEst);

    SignedZero = __vor(Zero, Sign);
    Alt = __vandc(V1IsZero, V2IsNegative);
    One = __vupkd3d(One, VPACK_NORMSHORT2);
    V2IsInt = __vcmpeqfp(V2, V2IsInt);
    V1OrV2IsZero = __vor(V1IsZero, ResultAlt);

    Alt = __vsel(QNaN, SignedZero, Alt);
    One = __vspltw(One, 3);
    ResultIsAlt = __vandc(V1IsNegative, V2IsInt);

    ResultAlt = __vsel(Alt, One, ResultAlt);
    ResultIsAlt = __vor(ResultIsAlt, V1OrV2IsZero);

    PowEst = __vor(PowEst, Sign);
    ResultIsAlt = __vsel(PowEst, ResultAlt, ResultIsAlt);

    return ResultIsAlt;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorPow
(
    XMVECTOR V1,
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    return XMVectorPowEst(V1, V2);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR AbsV1;
    XMVECTOR V1IsNegative, V1IsZero, V1Sign, V2IsNegative, V2IsZero, V1OrV2IsZero, V2Int, V2IsInt, ResultIsAlt, ResultAlt;
    XMVECTOR Shift, SignMask, Sign;
    XMVECTOR Zero, SignedZero, OneInt;
    XMVECTOR Result;

    Shift = __vspltisw(-1); // Shift 31
    SignMask = __vslw(Shift, Shift);

    Zero = XMVectorZero();
    V1IsNegative = XMVectorLess(V1, Zero);
    V2IsNegative = XMVectorLess(V2, Zero);
    V1Sign = XMVectorAndInt(V1, SignMask);

    OneInt = XMVectorSplatConstantInt(1);
    V2Int = XMConvertVectorFloatToInt(V2, 0);
    Sign = XMVectorAndInt(V2Int, OneInt);
    Sign = __vslw(Sign, Shift);
    Sign = XMVectorAndInt(V1Sign, Sign);

    V2IsInt = XMVectorTruncate(V2);
    V2IsInt = XMVectorEqual(V2, V2IsInt);

    V1IsZero = XMVectorEqual(V1, Zero);
    V2IsZero = XMVectorEqual(V2, Zero);
    V1OrV2IsZero = XMVectorOrInt(V1IsZero, V2IsZero);
    SignedZero = XMVectorOrInt(Zero, Sign);

    ResultAlt = XMVectorAndCInt(V1IsZero, V2IsNegative);
    ResultAlt = XMVectorSelect(*(XMVECTOR*)g_XMQNaN, SignedZero, ResultAlt);
    ResultAlt = XMVectorSelect(ResultAlt, g_XMOne, V2IsZero);

    ResultIsAlt = XMVectorAndCInt(V1IsNegative, V2IsInt);
    ResultIsAlt = XMVectorOrInt(ResultIsAlt, V1OrV2IsZero);

    AbsV1 = XMVectorAbs(V1);
    Result = XMVectorLog(AbsV1);
    Result = XMVectorMultiply(V2, Result);
    Result = XMVectorExp(Result);

    Result = XMVectorOrInt(Result, Sign);
    Result = XMVectorSelect(Result, ResultAlt, ResultIsAlt);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR               L, E;
    XMVECTOR               R, R2, R3, R4;
    XMVECTOR               L0, L1;
    XMVECTOR               E0, E1;
    XMVECTOR               C0Y, C0Z, C0W;
    XMVECTOR               C1Y, C1Z, C1W;
    XMVECTOR               One, Shift;
    XMVECTOR               Reciprocal, Scale1, Scale2, Refine;
    XMVECTOR               AbsV1, V1IsNegative, V1IsZero, V1Sign, V2IsNegative, V1OrV2IsZero, V2Int, V2IsInt;
    XMVECTOR               Alt, ResultIsAlt, ResultAlt;
    XMVECTOR               SignMask, Sign;
    XMVECTOR               Zero, SignedZero, OneInt, QNaN;
    XMVECTOR               Result;
    static CONST XMVECTOR  LC0 = {1.44268966f, -7.21165776e-1f, 4.78684813e-1f, -3.47305417e-1f};
    static CONST XMVECTOR  LC1 = {2.41873696e-1f, -1.37531206e-1f, 5.20646796e-2f, -9.31049418e-3f};
    static CONST XMVECTOR  EC0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  EC1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};

    XMDUMMY_INITIALIZE_VECTOR(One);

    Shift = __vspltisw(-1); // Shift 31
    SignMask = __vslw(Shift, Shift);

    AbsV1 = __vandc(V1, SignMask);

    One = __vupkd3d(One, VPACK_NORMSHORT2);
    One = __vspltw(One, 3);

    R = __vspltisw(-1);
    Shift = __vspltisw(-9); // Shift 23
    R = __vslw(R, Shift);

    R = __vsel(AbsV1, One, R);

    L0 = __vspltw(LC0, 0);
    C0Y = __vspltw(LC0, 1);
    L1 = __vspltw(LC1, 0);
    C1Y = __vspltw(LC1, 1);

    R = __vsubfp(R, One);

    C0Z = __vspltw(LC0, 2);
    C0W = __vspltw(LC0, 3);
    C1Z = __vspltw(LC1, 2);
    C1W = __vspltw(LC1, 3);

    R2 = __vmulfp(R, R);

    L0 = __vmaddfp(R, C0Y, L0);
    L1 = __vmaddfp(R, C1Y, L1);

    R3 = __vmulfp(R, R2);

    L0 = __vmaddfp(R2, C0Z, L0);
    L1 = __vmaddfp(R2, C1Z, L1);

    L = __vlogefp(AbsV1);

    L0 = __vmaddfp(R3, C0W, L0);

    R4 = __vmulfp(R2, R2);

    L1 = __vmaddfp(R3, C1W, L1);

    L = __vrfim(L);

    R = __vmulfp(R, V2);
    L = __vmulfp(L, V2);

    L0 = __vmaddfp(R4, L1, L0);

    L = __vmaddfp(R, L0, L);

    R = __vrfim(L);
    E = __vexptefp(R);

    E0 = __vspltw(EC0, 0);
    C0Y = __vspltw(EC0, 1);
    E1 = __vspltw(EC1, 0);
    C1Y = __vspltw(EC1, 1);

    R = __vsubfp(L, R);

    C0Z = __vspltw(EC0, 2);
    C0W = __vspltw(EC0, 3);
    C1Z = __vspltw(EC1, 2);
    C1W = __vspltw(EC1, 3);

    R2 = __vmulfp(R, R);

    E0 = __vmaddfp(R, C0Y, E0);
    E1 = __vmaddfp(R, C1Y, E1);

    R3 = __vmulfp(R, R2);

    E0 = __vmaddfp(R2, C0Z, E0);
    E1 = __vmaddfp(R2, C1Z, E1);

    E0 = __vmaddfp(R3, C0W, E0);

    R4 = __vmulfp(R2, R2);

    E1 = __vmaddfp(R3, C1W, E1);

    E0 = __vmaddfp(R4, E1, E0);

    Reciprocal = __vrefp(E0);
    Scale1 = __vspltw(EC0, 0);
    Scale1 = __vnmsubfp(E0, Reciprocal, Scale1);
    Scale1 = __vmaddcfp(Reciprocal, Scale1, Reciprocal);
    Scale2 = __vspltw(EC0, 0);
    Scale2 = __vnmsubfp(E0, Scale1, Scale2);
    Refine = __vcmpeqfp(Scale1, Scale1);
    Scale1 = __vmaddfp(Scale1, Scale2, Scale1);

    OneInt = __vspltisw(1);
    V2Int = __vctsxs(V2, 0);
    Zero = __vspltisw(0);
    QNaN = __vspltisw(-9); // Shift 23
    Shift = __vspltisw(-1); // Regenerate shift 31
    Sign = __vand(V2Int, OneInt);
    V2IsInt = __vrfiz(V2);
    V1IsNegative = __vcmpgtfp(Zero, V1);
    V2IsNegative = __vcmpgtfp(Zero, V2);
    QNaN = __vslw(Shift, QNaN);
    V1Sign = __vand(V1, SignMask);
    Sign = __vslw(Sign, Shift);
    ResultAlt = __vcmpeqfp(V2, Zero);
    QNaN = __vsrw(QNaN, OneInt);

    Refine = __vsel(Reciprocal, Scale1, Refine);

    Sign = __vand(V1Sign, Sign);
    V1IsZero = __vcmpeqfp(V1, Zero);

    Refine = __vmulfp(E, Refine);

    SignedZero = __vor(Zero, Sign);
    Alt = __vandc(V1IsZero, V2IsNegative);
    V2IsInt = __vcmpeqfp(V2, V2IsInt);
    V1OrV2IsZero = __vor(V1IsZero, ResultAlt);

    Alt = __vsel(QNaN, SignedZero, Alt);
    ResultIsAlt = __vandc(V1IsNegative, V2IsInt);

    ResultAlt = __vsel(Alt, One, ResultAlt);
    ResultIsAlt = __vor(ResultIsAlt, V1OrV2IsZero);

    Refine = __vor(Refine, Sign);
    Result = __vsel(Refine, ResultAlt, ResultIsAlt);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorAbs
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR NegativeV;
    XMVECTOR Result;

    NegativeV = XMVectorNegate(V);
    Result = XMVectorMax(V, NegativeV);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Result;
    Result = __vand(V, *(XMVECTOR*)g_XMAbsMask);
    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR SignMask;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    Result = __vandc(V, SignMask);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorMod
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Reciprocal;
    XMVECTOR Quotient;
    XMVECTOR Result;

    // V1 % V2 = V1 - V2 * truncate(V1 / V2)
    Reciprocal = XMVectorReciprocal(V2);
    Quotient = XMVectorMultiply(V1, Reciprocal);
    Quotient = XMVectorTruncate(Quotient);
    Result = XMVectorNegativeMultiplySubtract(V2, Quotient, V1);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Reciprocal;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR Quotient;
    XMVECTOR Result;

    Reciprocal = __vrefp(V2);
    Scale = __vnmsubfp(V2, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);
    Scale = __vnmsubfp(V2, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Reciprocal = __vsel(Reciprocal, Rcp, Refine);
    Quotient = __vmaddfp(V1, Reciprocal, *(XMVECTOR*)g_XMNegativeZero);
    Quotient = __vrfiz(Quotient);
    Result = __vnmsubfp(V2, Quotient, V1);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Reciprocal;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR Quotient;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(Rcp);

    Reciprocal = __vrefp(V2);
    Rcp = __vupkd3d(Rcp, VPACK_NORMSHORT2);
    Scale = __vspltw(Rcp, 3);
    Rcp = __vspltw(Rcp, 3);
    Rcp = __vnmsubfp(V2, Reciprocal, Rcp);
    Rcp = __vmaddcfp(Reciprocal, Rcp, Reciprocal);
    Scale = __vnmsubfp(V2, Rcp, Scale);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Refine = __vsel(Reciprocal, Rcp, Refine);
    Quotient = __vmulfp(V1, Refine);
    Result = V1;
    Quotient = __vrfiz(Quotient);
    Result = __vnmsubfp(V2, Quotient, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorModAngles
(
    XMVECTOR Angles
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR Result;

    // Modulo the range of the given angles such that -XM_PI <= Angles < XM_PI
    V = XMVectorMultiply(Angles, g_XMReciprocalTwoPi);
    V = XMVectorRound(V);
    Result = XMVectorNegativeMultiplySubtract(g_XMTwoPi, V, Angles);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR Result;

    V = __vmaddfp(Angles, g_XMReciprocalTwoPi, *(XMVECTOR*)g_XMNegativeZero);
    V = __vrfin(V);
    Result = __vnmsubfp(g_XMTwoPi, V, Angles);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V;
    XMVECTOR ReciprocalTwoPi, TwoPi;
    XMVECTOR Result;

    ReciprocalTwoPi = __vspltw(g_XMPiConstants0, 3);
    V = __vmulfp(Angles, ReciprocalTwoPi);
    TwoPi = __vspltw(g_XMPiConstants0, 1);
    Result = Angles;
    V = __vrfin(V);
    Result = __vnmsubfp(TwoPi, V, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorSin
(
    XMVECTOR V
)
{

#if defined(_NO_INTRINSICS_)

    XMVECTOR V1, V2, V3, V5, V7, V9, V11, V13, V15, V17, V19, V21, V23;
    XMVECTOR S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11;
    XMVECTOR Result;

    V1 = XMVectorModAngles(V);

    // sin(V) ~= V - V^3 / 3! + V^5 / 5! - V^7 / 7! + V^9 / 9! - V^11 / 11! + V^13 / 13! - 
    //           V^15 / 15! + V^17 / 17! - V^19 / 19! + V^21 / 21! - V^23 / 23! (for -PI <= V < PI)
    V2  = XMVectorMultiply(V1, V1);
    V3  = XMVectorMultiply(V2, V1);
    V5  = XMVectorMultiply(V3, V2);
    V7  = XMVectorMultiply(V5, V2);
    V9  = XMVectorMultiply(V7, V2);
    V11 = XMVectorMultiply(V9, V2);
    V13 = XMVectorMultiply(V11, V2);
    V15 = XMVectorMultiply(V13, V2);
    V17 = XMVectorMultiply(V15, V2);
    V19 = XMVectorMultiply(V17, V2);
    V21 = XMVectorMultiply(V19, V2);
    V23 = XMVectorMultiply(V21, V2);

    S1  = XMVectorSplatY(g_XMSinCoefficients0);
    S2  = XMVectorSplatZ(g_XMSinCoefficients0);
    S3  = XMVectorSplatW(g_XMSinCoefficients0);
    S4  = XMVectorSplatX(g_XMSinCoefficients1);
    S5  = XMVectorSplatY(g_XMSinCoefficients1);
    S6  = XMVectorSplatZ(g_XMSinCoefficients1);
    S7  = XMVectorSplatW(g_XMSinCoefficients1);
    S8  = XMVectorSplatX(g_XMSinCoefficients2);
    S9  = XMVectorSplatY(g_XMSinCoefficients2);
    S10 = XMVectorSplatZ(g_XMSinCoefficients2);
    S11 = XMVectorSplatW(g_XMSinCoefficients2);

    Result = XMVectorMultiplyAdd(S1, V3, V1);
    Result = XMVectorMultiplyAdd(S2, V5, Result);
    Result = XMVectorMultiplyAdd(S3, V7, Result);
    Result = XMVectorMultiplyAdd(S4, V9, Result);
    Result = XMVectorMultiplyAdd(S5, V11, Result);
    Result = XMVectorMultiplyAdd(S6, V13, Result);
    Result = XMVectorMultiplyAdd(S7, V15, Result);
    Result = XMVectorMultiplyAdd(S8, V17, Result);
    Result = XMVectorMultiplyAdd(S9, V19, Result);
    Result = XMVectorMultiplyAdd(S10, V21, Result);
    Result = XMVectorMultiplyAdd(S11, V23, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1, V2, V3, V5, V7, V9, V11, V13, V15, V17, V19, V21, V23;
    XMVECTOR S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11;
    XMVECTOR Result;

    V1 = __vmaddfp(V, g_XMReciprocalTwoPi, *(XMVECTOR*)g_XMNegativeZero);
    V1 = __vrfin(V1);
    V1 = __vnmsubfp(g_XMTwoPi, V1, V);

    V2 = __vmaddfp(V1, V1, *(XMVECTOR*)g_XMNegativeZero);
    S1 = __vspltw(g_XMSinCoefficients0, 1);
    S2 = __vspltw(g_XMSinCoefficients0, 2);
    S3 = __vspltw(g_XMSinCoefficients0, 3);
    S4 = __vspltw(g_XMSinCoefficients1, 0);
    S5 = __vspltw(g_XMSinCoefficients1, 1);
    S6 = __vspltw(g_XMSinCoefficients1, 2);
    S7 = __vspltw(g_XMSinCoefficients1, 3);
    S8 = __vspltw(g_XMSinCoefficients2, 0);
    S9 = __vspltw(g_XMSinCoefficients2, 1);
    S10 = __vspltw(g_XMSinCoefficients2, 2);
    S11 = __vspltw(g_XMSinCoefficients2, 3);
    V3 = __vmaddfp(V2, V1, *(XMVECTOR*)g_XMNegativeZero);
    V5 = __vmaddfp(V3, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S1, V3, V1);
    V7 = __vmaddfp(V5, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S2, V5, Result);
    V9 = __vmaddfp(V7, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S3, V7, Result);
    V11 = __vmaddfp(V9, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S4, V9, Result);
    V13 = __vmaddfp(V11, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S5, V11, Result);
    V15 = __vmaddfp(V13, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S6, V13, Result);
    V17 = __vmaddfp(V15, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S7, V15, Result);
    V19 = __vmaddfp(V17, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S8, V17, Result);
    V21 = __vmaddfp(V19, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S9, V19, Result);
    V23 = __vmaddfp(V21, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S10, V21, Result);
    Result = __vmaddfp(S11, V23, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V1, V2, V3, V5, V7, V9, V11, V13, V15, V17, V19, V21, V23;
    XMVECTOR S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11;
    XMVECTOR ReciprocalTwoPi, TwoPi;
    XMVECTOR Result;

    ReciprocalTwoPi = __vspltw(g_XMPiConstants0, 3);
    V1 = __vmulfp(V, ReciprocalTwoPi);
    TwoPi = __vspltw(g_XMPiConstants0, 1);
    Result = V;
    V1 = __vrfin(V1);
    Result = __vnmsubfp(TwoPi, V1, Result);

    V2 = __vmulfp(Result, Result);
    S1 = __vspltw(g_XMSinCoefficients0, 1);
    S2 = __vspltw(g_XMSinCoefficients0, 2);
    S3 = __vspltw(g_XMSinCoefficients0, 3);
    S4 = __vspltw(g_XMSinCoefficients1, 0);
    S5 = __vspltw(g_XMSinCoefficients1, 1);
    S6 = __vspltw(g_XMSinCoefficients1, 2);
    S7 = __vspltw(g_XMSinCoefficients1, 3);
    S8 = __vspltw(g_XMSinCoefficients2, 0);
    S9 = __vspltw(g_XMSinCoefficients2, 1);
    S10 = __vspltw(g_XMSinCoefficients2, 2);
    S11 = __vspltw(g_XMSinCoefficients2, 3);
    V3 = __vmulfp(V2, Result);
    V5 = __vmulfp(V3, V2);
    Result = __vmaddfp(S1, V3, Result);
    V7 = __vmulfp(V5, V2);
    Result = __vmaddfp(S2, V5, Result);
    V9 = __vmulfp(V7, V2);
    Result = __vmaddfp(S3, V7, Result);
    V11 = __vmulfp(V9, V2);
    Result = __vmaddfp(S4, V9, Result);
    V13 = __vmulfp(V11, V2);
    Result = __vmaddfp(S5, V11, Result);
    V15 = __vmulfp(V13, V2);
    Result = __vmaddfp(S6, V13, Result);
    V17 = __vmulfp(V15, V2);
    Result = __vmaddfp(S7, V15, Result);
    V19 = __vmulfp(V17, V2);
    Result = __vmaddfp(S8, V17, Result);
    V21 = __vmulfp(V19, V2);
    Result = __vmaddfp(S9, V19, Result);
    V23 = __vmulfp(V21, V2);
    Result = __vmaddfp(S10, V21, Result);
    Result = __vmaddfp(S11, V23, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorCos
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1, V2, V4, V6, V8, V10, V12, V14, V16, V18, V20, V22;
    XMVECTOR C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR Result;

    V1 = XMVectorModAngles(V);

    // cos(V) ~= 1 - V^2 / 2! + V^4 / 4! - V^6 / 6! + V^8 / 8! - V^10 / 10! + V^12 / 12! - 
    //           V^14 / 14! + V^16 / 16! - V^18 / 18! + V^20 / 20! - V^22 / 22! (for -PI <= V < PI)
    V2 = XMVectorMultiply(V1, V1);
    V4 = XMVectorMultiply(V2, V2);
    V6 = XMVectorMultiply(V4, V2);
    V8 = XMVectorMultiply(V4, V4);
    V10 = XMVectorMultiply(V6, V4);
    V12 = XMVectorMultiply(V6, V6);
    V14 = XMVectorMultiply(V8, V6);
    V16 = XMVectorMultiply(V8, V8);
    V18 = XMVectorMultiply(V10, V8);
    V20 = XMVectorMultiply(V10, V10);
    V22 = XMVectorMultiply(V12, V10);

    C1 = XMVectorSplatY(g_XMCosCoefficients0);
    C2 = XMVectorSplatZ(g_XMCosCoefficients0);
    C3 = XMVectorSplatW(g_XMCosCoefficients0);
    C4 = XMVectorSplatX(g_XMCosCoefficients1);
    C5 = XMVectorSplatY(g_XMCosCoefficients1);
    C6 = XMVectorSplatZ(g_XMCosCoefficients1);
    C7 = XMVectorSplatW(g_XMCosCoefficients1);
    C8 = XMVectorSplatX(g_XMCosCoefficients2);
    C9 = XMVectorSplatY(g_XMCosCoefficients2);
    C10 = XMVectorSplatZ(g_XMCosCoefficients2);
    C11 = XMVectorSplatW(g_XMCosCoefficients2);

    Result = XMVectorMultiplyAdd(C1, V2, g_XMOne);
    Result = XMVectorMultiplyAdd(C2, V4, Result);
    Result = XMVectorMultiplyAdd(C3, V6, Result);
    Result = XMVectorMultiplyAdd(C4, V8, Result);
    Result = XMVectorMultiplyAdd(C5, V10, Result);
    Result = XMVectorMultiplyAdd(C6, V12, Result);
    Result = XMVectorMultiplyAdd(C7, V14, Result);
    Result = XMVectorMultiplyAdd(C8, V16, Result);
    Result = XMVectorMultiplyAdd(C9, V18, Result);
    Result = XMVectorMultiplyAdd(C10, V20, Result);
    Result = XMVectorMultiplyAdd(C11, V22, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1, V2, V4, V6, V8, V10, V12, V14, V16, V18, V20, V22;
    XMVECTOR C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR Result;

    V1 = __vmaddfp(V, g_XMReciprocalTwoPi, *(XMVECTOR*)g_XMNegativeZero);
    V1 = __vrfin(V1);
    V1 = __vnmsubfp(g_XMTwoPi, V1, V);

    V2 = __vmaddfp(V1, V1, *(XMVECTOR*)g_XMNegativeZero);
    C1 = __vspltw(g_XMCosCoefficients0, 1);
    C2 = __vspltw(g_XMCosCoefficients0, 2);
    C3 = __vspltw(g_XMCosCoefficients0, 3);
    C4 = __vspltw(g_XMCosCoefficients1, 0);
    C5 = __vspltw(g_XMCosCoefficients1, 1);
    C6 = __vspltw(g_XMCosCoefficients1, 2);
    C7 = __vspltw(g_XMCosCoefficients1, 3);
    C8 = __vspltw(g_XMCosCoefficients2, 0);
    C9 = __vspltw(g_XMCosCoefficients2, 1);
    C10 = __vspltw(g_XMCosCoefficients2, 2);
    C11 = __vspltw(g_XMCosCoefficients2, 3);
    V4 = __vmaddfp(V2, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C1, V2, g_XMOne);
    V6 = __vmaddfp(V4, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C2, V4, Result);
    V8 = __vmaddfp(V4, V4, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C3, V6, Result);
    V10 = __vmaddfp(V6, V4, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C4, V8, Result);
    V12 = __vmaddfp(V6, V6, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C5, V10, Result);
    V14 = __vmaddfp(V8, V6, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C6, V12, Result);
    V16 = __vmaddfp(V8, V8, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C7, V14, Result);
    V18 = __vmaddfp(V10, V8, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C8, V16, Result);
    V20 = __vmaddfp(V10, V10, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C9, V18, Result);
    V22 = __vmaddfp(V12, V10, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C10, V20, Result);
    Result = __vmaddfp(C11, V22, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR VN, V1, V2, V4, V6, V8, V10, V12, V14, V16, V18, V20, V22;
    XMVECTOR C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR ReciprocalTwoPi, TwoPi;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(Result);

    ReciprocalTwoPi = __vspltw(g_XMPiConstants0, 3);
    VN = __vmulfp(V, ReciprocalTwoPi);
    TwoPi = __vspltw(g_XMPiConstants0, 1);
    V1 = V;
    VN = __vrfin(VN);
    V1 = __vnmsubfp(TwoPi, VN, V1);

    V2 = __vmulfp(V1, V1);
    Result = __vupkd3d(Result, VPACK_NORMSHORT2);
    C1 = __vspltw(g_XMCosCoefficients0, 1);
    C2 = __vspltw(g_XMCosCoefficients0, 2);
    C3 = __vspltw(g_XMCosCoefficients0, 3);
    C4 = __vspltw(g_XMCosCoefficients1, 0);
    C5 = __vspltw(g_XMCosCoefficients1, 1);
    Result = __vspltw(Result, 3);
    V4 = __vmulfp(V2, V2);
    Result = __vmaddfp(C1, V2, Result);
    C6 = __vspltw(g_XMCosCoefficients1, 2);
    C7 = __vspltw(g_XMCosCoefficients1, 3);
    C8 = __vspltw(g_XMCosCoefficients2, 0);
    C9 = __vspltw(g_XMCosCoefficients2, 1);
    C10 = __vspltw(g_XMCosCoefficients2, 2);
    C11 = __vspltw(g_XMCosCoefficients2, 3);
    V6 = __vmulfp(V4, V2);
    Result = __vmaddfp(C2, V4, Result);
    V8 = __vmulfp(V4, V4);
    Result = __vmaddfp(C3, V6, Result);
    V10 = __vmulfp(V6, V4);
    Result = __vmaddfp(C4, V8, Result);
    V12 = __vmulfp(V6, V6);
    Result = __vmaddfp(C5, V10, Result);
    V14 = __vmulfp(V8, V6);
    Result = __vmaddfp(C6, V12, Result);
    V16 = __vmulfp(V8, V8);
    Result = __vmaddfp(C7, V14, Result);
    V18 = __vmulfp(V10, V8);
    Result = __vmaddfp(C8, V16, Result);
    V20 = __vmulfp(V10, V10);
    Result = __vmaddfp(C9, V18, Result);
    V22 = __vmulfp(V12, V10);
    Result = __vmaddfp(C10, V20, Result);
    Result = __vmaddfp(C11, V22, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE VOID XMVectorSinCos
(
    XMVECTOR* pSin, 
    XMVECTOR* pCos, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13;
    XMVECTOR V14, V15, V16, V17, V18, V19, V20, V21, V22, V23;
    XMVECTOR S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11;
    XMVECTOR C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR Sin, Cos;

    XMASSERT(pSin);
    XMASSERT(pCos);

    V1 = XMVectorModAngles(V);

    // sin(V) ~= V - V^3 / 3! + V^5 / 5! - V^7 / 7! + V^9 / 9! - V^11 / 11! + V^13 / 13! - 
    //           V^15 / 15! + V^17 / 17! - V^19 / 19! + V^21 / 21! - V^23 / 23! (for -PI <= V < PI)
    // cos(V) ~= 1 - V^2 / 2! + V^4 / 4! - V^6 / 6! + V^8 / 8! - V^10 / 10! + V^12 / 12! - 
    //           V^14 / 14! + V^16 / 16! - V^18 / 18! + V^20 / 20! - V^22 / 22! (for -PI <= V < PI)

    V2 = XMVectorMultiply(V1, V1);
    V3 = XMVectorMultiply(V2, V1);
    V4 = XMVectorMultiply(V2, V2);
    V5 = XMVectorMultiply(V3, V2);
    V6 = XMVectorMultiply(V3, V3);
    V7 = XMVectorMultiply(V4, V3);
    V8 = XMVectorMultiply(V4, V4);
    V9 = XMVectorMultiply(V5, V4);
    V10 = XMVectorMultiply(V5, V5);
    V11 = XMVectorMultiply(V6, V5);
    V12 = XMVectorMultiply(V6, V6);
    V13 = XMVectorMultiply(V7, V6);
    V14 = XMVectorMultiply(V7, V7);
    V15 = XMVectorMultiply(V8, V7);
    V16 = XMVectorMultiply(V8, V8);
    V17 = XMVectorMultiply(V9, V8);
    V18 = XMVectorMultiply(V9, V9);
    V19 = XMVectorMultiply(V10, V9);
    V20 = XMVectorMultiply(V10, V10);
    V21 = XMVectorMultiply(V11, V10);
    V22 = XMVectorMultiply(V11, V11);
    V23 = XMVectorMultiply(V12, V11);

    S1  = XMVectorSplatY(g_XMSinCoefficients0);
    S2  = XMVectorSplatZ(g_XMSinCoefficients0);
    S3  = XMVectorSplatW(g_XMSinCoefficients0);
    S4  = XMVectorSplatX(g_XMSinCoefficients1);
    S5  = XMVectorSplatY(g_XMSinCoefficients1);
    S6  = XMVectorSplatZ(g_XMSinCoefficients1);
    S7  = XMVectorSplatW(g_XMSinCoefficients1);
    S8  = XMVectorSplatX(g_XMSinCoefficients2);
    S9  = XMVectorSplatY(g_XMSinCoefficients2);
    S10  = XMVectorSplatZ(g_XMSinCoefficients2);
    S11  = XMVectorSplatW(g_XMSinCoefficients2);

    C1 = XMVectorSplatY(g_XMCosCoefficients0);
    C2 = XMVectorSplatZ(g_XMCosCoefficients0);
    C3 = XMVectorSplatW(g_XMCosCoefficients0);
    C4 = XMVectorSplatX(g_XMCosCoefficients1);
    C5 = XMVectorSplatY(g_XMCosCoefficients1);
    C6 = XMVectorSplatZ(g_XMCosCoefficients1);
    C7 = XMVectorSplatW(g_XMCosCoefficients1);
    C8 = XMVectorSplatX(g_XMCosCoefficients2);
    C9 = XMVectorSplatY(g_XMCosCoefficients2);
    C10 = XMVectorSplatZ(g_XMCosCoefficients2);
    C11 = XMVectorSplatW(g_XMCosCoefficients2);

    Sin = XMVectorMultiplyAdd(S1, V3, V1);
    Sin = XMVectorMultiplyAdd(S2, V5, Sin);
    Sin = XMVectorMultiplyAdd(S3, V7, Sin);
    Sin = XMVectorMultiplyAdd(S4, V9, Sin);
    Sin = XMVectorMultiplyAdd(S5, V11, Sin);
    Sin = XMVectorMultiplyAdd(S6, V13, Sin);
    Sin = XMVectorMultiplyAdd(S7, V15, Sin);
    Sin = XMVectorMultiplyAdd(S8, V17, Sin);
    Sin = XMVectorMultiplyAdd(S9, V19, Sin);
    Sin = XMVectorMultiplyAdd(S10, V21, Sin);
    Sin = XMVectorMultiplyAdd(S11, V23, Sin);

    Cos = XMVectorMultiplyAdd(C1, V2, g_XMOne);
    Cos = XMVectorMultiplyAdd(C2, V4, Cos);
    Cos = XMVectorMultiplyAdd(C3, V6, Cos);
    Cos = XMVectorMultiplyAdd(C4, V8, Cos);
    Cos = XMVectorMultiplyAdd(C5, V10, Cos);
    Cos = XMVectorMultiplyAdd(C6, V12, Cos);
    Cos = XMVectorMultiplyAdd(C7, V14, Cos);
    Cos = XMVectorMultiplyAdd(C8, V16, Cos);
    Cos = XMVectorMultiplyAdd(C9, V18, Cos);
    Cos = XMVectorMultiplyAdd(C10, V20, Cos);
    Cos = XMVectorMultiplyAdd(C11, V22, Cos);

    *pSin = Sin;
    *pCos = Cos;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13;
    XMVECTOR V14, V15, V16, V17, V18, V19, V20, V21, V22, V23;
    XMVECTOR S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11;
    XMVECTOR C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR Sin, Cos;

    XMASSERT(pSin);
    XMASSERT(pCos);

    V1 = __vmaddfp(V, g_XMReciprocalTwoPi, *(XMVECTOR*)g_XMNegativeZero);
    V1 = __vrfin(V1);
    V1 = __vnmsubfp(g_XMTwoPi, V1, V);

    V2 = __vmaddfp(V1, V1, *(XMVECTOR*)g_XMNegativeZero);
    C1 = __vspltw(g_XMCosCoefficients0, 1);
    S1 = __vspltw(g_XMSinCoefficients0, 1);
    C2 = __vspltw(g_XMCosCoefficients0, 2);
    S2 = __vspltw(g_XMSinCoefficients0, 2);
    C3 = __vspltw(g_XMCosCoefficients0, 3);
    S3 = __vspltw(g_XMSinCoefficients0, 3);
    C4 = __vspltw(g_XMCosCoefficients1, 0);
    S4 = __vspltw(g_XMSinCoefficients1, 0);
    C5 = __vspltw(g_XMCosCoefficients1, 1);
    S5 = __vspltw(g_XMSinCoefficients1, 1);
    C6 = __vspltw(g_XMCosCoefficients1, 2);
    S6 = __vspltw(g_XMSinCoefficients1, 2);
    C7 = __vspltw(g_XMCosCoefficients1, 3);
    S7 = __vspltw(g_XMSinCoefficients1, 3);
    C8 = __vspltw(g_XMCosCoefficients2, 0);
    S8 = __vspltw(g_XMSinCoefficients2, 0);
    C9 = __vspltw(g_XMCosCoefficients2, 1);
    S9 = __vspltw(g_XMSinCoefficients2, 1);
    C10 = __vspltw(g_XMCosCoefficients2, 2);
    S10 = __vspltw(g_XMSinCoefficients2, 2);
    C11 = __vspltw(g_XMCosCoefficients2, 3);
    S11 = __vspltw(g_XMSinCoefficients2, 3);

    V3 = __vmaddfp(V2, V1, *(XMVECTOR*)g_XMNegativeZero);
    V4 = __vmaddfp(V2, V2, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C1, V2, g_XMOne);
    V5 = __vmaddfp(V3, V2, *(XMVECTOR*)g_XMNegativeZero);
    Sin = __vmaddfp(S1, V3, V1);
    V6 = __vmaddfp(V3, V3, *(XMVECTOR*)g_XMNegativeZero);
    V7 = __vmaddfp(V4, V3, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C2, V4, Cos);
    Sin = __vmaddfp(S2, V5, Sin);
    V8 = __vmaddfp(V4, V4, *(XMVECTOR*)g_XMNegativeZero);
    V9 = __vmaddfp(V5, V4, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C3, V6, Cos);
    Sin = __vmaddfp(S3, V7, Sin);
    V10 = __vmaddfp(V5, V5, *(XMVECTOR*)g_XMNegativeZero);
    V11 = __vmaddfp(V6, V5, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C4, V8, Cos);
    Sin = __vmaddfp(S4, V9, Sin);
    V12 = __vmaddfp(V6, V6, *(XMVECTOR*)g_XMNegativeZero);
    V13 = __vmaddfp(V7, V6, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C5, V10, Cos);
    Sin = __vmaddfp(S5, V11, Sin);
    V14 = __vmaddfp(V7, V7, *(XMVECTOR*)g_XMNegativeZero);
    V15 = __vmaddfp(V8, V7, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C6, V12, Cos);
    Sin = __vmaddfp(S6, V13, Sin);
    V16 = __vmaddfp(V8, V8, *(XMVECTOR*)g_XMNegativeZero);
    V17 = __vmaddfp(V9, V8, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C7, V14, Cos);
    Sin = __vmaddfp(S7, V15, Sin);
    V18 = __vmaddfp(V9, V9, *(XMVECTOR*)g_XMNegativeZero);
    V19 = __vmaddfp(V10, V9, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C8, V16, Cos);
    Sin = __vmaddfp(S8, V17, Sin);
    V20 = __vmaddfp(V10, V10, *(XMVECTOR*)g_XMNegativeZero);
    V21 = __vmaddfp(V11, V10, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C9, V18, Cos);
    Sin = __vmaddfp(S9, V19, Sin);
    V22 = __vmaddfp(V11, V11, *(XMVECTOR*)g_XMNegativeZero);
    V23 = __vmaddfp(V12, V11, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C10, V20, Cos);
    Sin = __vmaddfp(S10, V21, Sin);
    Cos = __vmaddfp(C11, V22, Cos);
    Sin = __vmaddfp(S11, V23, Sin);

    *pCos = Cos;
    *pSin = Sin;

#else // _VMX128_INTRINSICS_

    XMVECTOR VN, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13;
    XMVECTOR V14, V15, V16, V17, V18, V19, V20, V21, V22, V23;
    XMVECTOR S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11;
    XMVECTOR C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR ReciprocalTwoPi, TwoPi;
    XMVECTOR Sin, Cos;

    XMASSERT(pSin);
    XMASSERT(pCos);

    XMDUMMY_INITIALIZE_VECTOR(Cos);

    ReciprocalTwoPi = __vspltw(g_XMPiConstants0, 3);
    VN = __vmulfp(V, ReciprocalTwoPi);
    TwoPi = __vspltw(g_XMPiConstants0, 1);
    V1 = V;
    VN = __vrfin(VN);
    V1 = __vnmsubfp(TwoPi, VN, V1);

    Cos = __vupkd3d(Cos, VPACK_NORMSHORT2);

    V2 = __vmulfp(V1, V1);
    Cos = __vspltw(Cos, 3);
    C1 = __vspltw(g_XMCosCoefficients0, 1);
    S1 = __vspltw(g_XMSinCoefficients0, 1);
    C2 = __vspltw(g_XMCosCoefficients0, 2);
    S2 = __vspltw(g_XMSinCoefficients0, 2);
    C3 = __vspltw(g_XMCosCoefficients0, 3);
    S3 = __vspltw(g_XMSinCoefficients0, 3);
    C4 = __vspltw(g_XMCosCoefficients1, 0);
    S4 = __vspltw(g_XMSinCoefficients1, 0);
    V3 = __vmulfp(V2, V1);
    V4 = __vmulfp(V2, V2);
    Cos = __vmaddfp(C1, V2, Cos);
    C5 = __vspltw(g_XMCosCoefficients1, 1);
    S5 = __vspltw(g_XMSinCoefficients1, 1);
    C6 = __vspltw(g_XMCosCoefficients1, 2);
    S6 = __vspltw(g_XMSinCoefficients1, 2);
    C7 = __vspltw(g_XMCosCoefficients1, 3);
    S7 = __vspltw(g_XMSinCoefficients1, 3);
    C8 = __vspltw(g_XMCosCoefficients2, 0);
    S8 = __vspltw(g_XMSinCoefficients2, 0);
    V5 = __vmulfp(V3, V2);
    Sin = __vmaddfp(S1, V3, V1);
    V6 = __vmulfp(V3, V3);
    V7 = __vmulfp(V4, V3);
    Cos = __vmaddfp(C2, V4, Cos);
    C9 = __vspltw(g_XMCosCoefficients2, 1);
    S9 = __vspltw(g_XMSinCoefficients2, 1);
    C10 = __vspltw(g_XMCosCoefficients2, 2);
    S10 = __vspltw(g_XMSinCoefficients2, 2);
    C11 = __vspltw(g_XMCosCoefficients2, 3);
    S11 = __vspltw(g_XMSinCoefficients2, 3);
    Sin = __vmaddfp(S2, V5, Sin);
    V8 = __vmulfp(V4, V4);
    V9 = __vmulfp(V5, V4);
    Cos = __vmaddfp(C3, V6, Cos);
    Sin = __vmaddfp(S3, V7, Sin);
    V10 = __vmulfp(V5, V5);
    V11 = __vmulfp(V6, V5);
    Cos = __vmaddfp(C4, V8, Cos);
    Sin = __vmaddfp(S4, V9, Sin);
    V12 = __vmulfp(V6, V6);
    V13 = __vmulfp(V7, V6);
    Cos = __vmaddfp(C5, V10, Cos);
    Sin = __vmaddfp(S5, V11, Sin);
    V14 = __vmulfp(V7, V7);
    V15 = __vmulfp(V8, V7);
    Cos = __vmaddfp(C6, V12, Cos);
    Sin = __vmaddfp(S6, V13, Sin);
    V16 = __vmulfp(V8, V8);
    V17 = __vmulfp(V9, V8);
    Cos = __vmaddfp(C7, V14, Cos);
    Sin = __vmaddfp(S7, V15, Sin);
    V18 = __vmulfp(V9, V9);
    V19 = __vmulfp(V10, V9);
    Cos = __vmaddfp(C8, V16, Cos);
    Sin = __vmaddfp(S8, V17, Sin);
    V20 = __vmulfp(V10, V10);
    V21 = __vmulfp(V11, V10);
    Cos = __vmaddfp(C9, V18, Cos);
    Sin = __vmaddfp(S9, V19, Sin);
    V22 = __vmulfp(V11, V11);
    V23 = __vmulfp(V12, V11);
    Cos = __vmaddfp(C10, V20, Cos);
    Sin = __vmaddfp(S10, V21, Sin);
    Cos = __vmaddfp(C11, V22, Cos);
    Sin = __vmaddfp(S11, V23, Sin);

    *pCos = Cos;
    *pSin = Sin;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorTan
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    // Cody and Waite algorithm to compute tangent.

    XMVECTOR VA, VB, VC, VC2;
    XMVECTOR T0, T1, T2, T3, T4, T5, T6, T7;
    XMVECTOR C0, C1, TwoDivPi, Epsilon;
    XMVECTOR N, D;
    XMVECTOR R0, R1;
    XMVECTOR VIsZero, VCNearZero, VBIsEven;
    XMVECTOR Zero;
    XMVECTOR Result;
    UINT     i;
    static CONST XMVECTOR TanCoefficients0 = {1.0f, -4.667168334e-1f, 2.566383229e-2f, -3.118153191e-4f};
    static CONST XMVECTOR TanCoefficients1 = {4.981943399e-7f, -1.333835001e-1f, 3.424887824e-3f, -1.786170734e-5f};
    static CONST XMVECTOR TanConstants = {1.570796371f, 6.077100628e-11f, 0.000244140625f, 2.0f / XM_PI};
    static CONST XMVECTORI Mask = {0x1, 0x1, 0x1, 0x1};

    TwoDivPi = XMVectorSplatW(TanConstants);

    Zero = XMVectorZero();

    C0 = XMVectorSplatX(TanConstants);
    C1 = XMVectorSplatY(TanConstants);
    Epsilon = XMVectorSplatZ(TanConstants);

    VA = XMVectorMultiply(V, TwoDivPi);

    VA = XMVectorRound(VA);

    VC = XMVectorNegativeMultiplySubtract(VA, C0, V);

    VB = XMVectorAbs(VA);

    VC = XMVectorNegativeMultiplySubtract(VA, C1, VC);

    for (i = 0; i < 4; i++)
    {
        VB.u[i] = (UINT)VB.v[i];
    }

    VC2 = XMVectorMultiply(VC, VC);

    T7 = XMVectorSplatW(TanCoefficients1);
    T6 = XMVectorSplatZ(TanCoefficients1);
    T4 = XMVectorSplatX(TanCoefficients1);
    T3 = XMVectorSplatW(TanCoefficients0);
    T5 = XMVectorSplatY(TanCoefficients1);
    T2 = XMVectorSplatZ(TanCoefficients0);
    T1 = XMVectorSplatY(TanCoefficients0);
    T0 = XMVectorSplatX(TanCoefficients0);

    VBIsEven = XMVectorAndInt(VB, *(XMVECTOR*)Mask);
    VBIsEven = XMVectorEqualInt(VBIsEven, Zero);

    N = XMVectorMultiplyAdd(VC2, T7, T6);
    D = XMVectorMultiplyAdd(VC2, T4, T3);
    N = XMVectorMultiplyAdd(VC2, N, T5);
    D = XMVectorMultiplyAdd(VC2, D, T2);
    N = XMVectorMultiply(VC2, N);
    D = XMVectorMultiplyAdd(VC2, D, T1);
    N = XMVectorMultiplyAdd(VC, N, VC);
    VCNearZero = XMVectorInBounds(VC, Epsilon);
    D = XMVectorMultiplyAdd(VC2, D, T0);

    N = XMVectorSelect(N, VC, VCNearZero);
    D = XMVectorSelect(D, g_XMOne, VCNearZero);

    R0 = XMVectorNegate(N);
    R1 = XMVectorReciprocal(D);
    R0 = XMVectorReciprocal(R0);
    R1 = XMVectorMultiply(N, R1);
    R0 = XMVectorMultiply(D, R0);

    VIsZero = XMVectorEqual(V, Zero);

    Result = XMVectorSelect(R0, R1, VBIsEven);

    Result = XMVectorSelect(Result, Zero, VIsZero);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR VA, VB, VC, VC2;
    XMVECTOR T0, T1, T2, T3, T4, T5, T6, T7;
    XMVECTOR C0, C1, TwoDivPi, Epsilon;
    XMVECTOR N, D;
    XMVECTOR R0, R1;
    XMVECTOR VIsZero, VCNearZero, VBIsEven;
    XMVECTOR Reciprocal0, Reciprocal1, Rcp0, Rcp1, Scale0, Scale1, Refine0, Refine1;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTOR TanCoefficients0 = {1.0f, -4.667168334e-1f, 2.566383229e-2f, -3.118153191e-4f};
    static CONST XMVECTOR TanCoefficients1 = {4.981943399e-7f, -1.333835001e-1f, 3.424887824e-3f, -1.786170734e-5f};
    static CONST XMVECTOR TanConstants = {1.570796371f, 6.077100628e-11f, 0.000244140625f, 2.0f / XM_PI};
    static CONST XMVECTORI Mask = {0x1, 0x1, 0x1, 0x1};

    TwoDivPi = __vspltw(TanConstants, 3);

    Zero = __vspltisw(0);

    C0 = __vspltw(TanConstants, 0);
    C1 = __vspltw(TanConstants, 1);
    Epsilon = __vspltw(TanConstants, 2);

    VA = __vmaddfp(V, TwoDivPi, *(XMVECTOR*)g_XMNegativeZero);

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    VA = __vrfin(VA);

    VC = __vnmsubfp(VA, C0, V);

    VB = __vandc(VA, SignMask);

    VC = __vnmsubfp(VA, C1, VC);

    VB = __vctsxs(VB, 0);

    VC2 = __vmaddfp(VC, VC, *(XMVECTOR*)g_XMNegativeZero);

    T7 = __vspltw(TanCoefficients1, 3);
    T6 = __vspltw(TanCoefficients1, 2);
    T4 = __vspltw(TanCoefficients1, 0);
    T3 = __vspltw(TanCoefficients0, 3);
    T5 = __vspltw(TanCoefficients1, 1);
    T2 = __vspltw(TanCoefficients0, 2);
    T1 = __vspltw(TanCoefficients0, 1);
    T0 = __vspltw(TanCoefficients0, 0);

    VBIsEven = __vand(VB, *(XMVECTOR*)Mask);
    VBIsEven = __vcmpequw(VBIsEven, Zero);

    N = __vmaddfp(VC2, T7, T6);
    D = __vmaddfp(VC2, T4, T3);
    N = __vmaddfp(VC2, N, T5);
    D = __vmaddfp(VC2, D, T2);
    N = __vmaddfp(VC2, N, *(XMVECTOR*)g_XMNegativeZero);
    D = __vmaddfp(VC2, D, T1);
    N = __vmaddfp(VC, N, VC);
    VCNearZero = __vcmpbfp(VC, Epsilon);
    D = __vmaddfp(VC2, D, T0);

    N = __vsel(N, VC, VCNearZero);
    D = __vsel(D, g_XMOne, VCNearZero);

    R0 = __vxor(N, SignMask);

    Reciprocal0 = __vrefp(D);
    Reciprocal1 = __vrefp(R0);
    Scale0 = __vnmsubfp(D, Reciprocal0, g_XMOne);
    Scale1 = __vnmsubfp(R0, Reciprocal1, g_XMOne);
    Rcp0 = __vmaddfp(Reciprocal0, Scale0, Reciprocal0);
    Rcp1 = __vmaddfp(Reciprocal1, Scale1, Reciprocal1);
    Scale0 = __vnmsubfp(D, Rcp0, g_XMOne);
    Scale1 = __vnmsubfp(R0, Rcp1, g_XMOne);
    Refine0 = __vcmpeqfp(Rcp0, Rcp0);
    Refine1 = __vcmpeqfp(Rcp1, Rcp1);
    Rcp0 = __vmaddfp(Rcp0, Scale0, Rcp0);
    Rcp1 = __vmaddfp(Rcp1, Scale1, Rcp1);
    R1 = __vsel(Reciprocal0, Rcp0, Refine0);
    R0 = __vsel(Reciprocal1, Rcp1, Refine1);

    R1 = __vmaddfp(N, R1, *(XMVECTOR*)g_XMNegativeZero);
    R0 = __vmaddfp(D, R0, *(XMVECTOR*)g_XMNegativeZero);

    VIsZero = __vcmpeqfp(V, Zero);

    Result = __vsel(R0, R1, VBIsEven);

    Result = __vsel(Result, Zero, VIsZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR VA, VB, VC, VC2;
    XMVECTOR T0, T1, T2, T3, T5, T6;
    XMVECTOR C0, C1, TwoDivPi, Epsilon;
    XMVECTOR N, D, NS, DS;
    XMVECTOR R0, R1;
    XMVECTOR Select;
    XMVECTOR Reciprocal0, Reciprocal1, Rcp0, Rcp1, Scale0, Scale1;
    XMVECTOR Mask;
    XMVECTOR SignMask;
    XMVECTOR One;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTOR TanCoefficients0 = {1.0f, -4.667168334e-1f, 2.566383229e-2f, -3.118153191e-4f};
    static CONST XMVECTOR TanCoefficients1 = {4.981943399e-7f, -1.333835001e-1f, 3.424887824e-3f, -1.786170734e-5f};
    static CONST XMVECTOR TanConstants = {1.570796371f, 6.077100628e-11f, 0.000244140625f, 2.0f / XM_PI};

    TwoDivPi = __vspltw(TanConstants, 3);

    SignMask = __vspltisw(-1);

    C0 = __vspltw(TanConstants, 0);
    C1 = __vspltw(TanConstants, 1);

    VA = __vmulfp(V, TwoDivPi);

    SignMask = __vslw(SignMask, SignMask);

    VC = V;

    VA = __vrfin(VA);

    VC = __vnmsubfp(VA, C0, VC);

    VB = __vandc(VA, SignMask);

    VC = __vnmsubfp(VA, C1, VC);

    VB = __vctsxs(VB, 0);

    Mask = __vspltisw(1);
    Zero = __vspltisw(0);

    VC2 = __vmulfp(VC, VC);

    N = __vspltw(TanCoefficients1, 3);
    T6 = __vspltw(TanCoefficients1, 2);
    D = __vspltw(TanCoefficients1, 0);
    T3 = __vspltw(TanCoefficients0, 3);
    T5 = __vspltw(TanCoefficients1, 1);
    T2 = __vspltw(TanCoefficients0, 2);
    T1 = __vspltw(TanCoefficients0, 1);
    T0 = __vspltw(TanCoefficients0, 0);

    Select = __vand(VB, Mask);
    Select = __vcmpequw(Select, Zero); // VBIsEven

    N = __vmaddcfp(VC2, N, T6);
    D = __vmaddcfp(VC2, D, T3);
    N = __vmaddcfp(VC2, N, T5);
    D = __vmaddcfp(VC2, D, T2);
    N = __vmulfp(VC2, N);
    D = __vmaddcfp(VC2, D, T1);
    Epsilon = __vspltw(TanConstants, 2);
    N = __vmaddcfp(VC, N, VC);
    One = __vcfsx(Mask, 0);
    D = __vmaddcfp(VC2, D, T0);
    NS = __vcmpbfp(VC, Epsilon); // VCNearZero
    DS = __vcmpbfp(VC, Epsilon); // VCNearZero

    NS = __vsel(N, VC, NS);
    DS = __vsel(D, One, DS);

    R0 = __vxor(NS, SignMask);

    Reciprocal0 = __vrefp(DS);
    Rcp0 = One;
    Reciprocal1 = __vrefp(R0);
    Rcp1 = One;
    Rcp0 = __vnmsubfp(DS, Reciprocal0, Rcp0);
    Rcp1 = __vnmsubfp(R0, Reciprocal1, Rcp1);
    Rcp0 = __vmaddcfp(Reciprocal0, Rcp0, Reciprocal0);
    Scale0 = One;
    Rcp1 = __vmaddcfp(Reciprocal1, Rcp1, Reciprocal1);
    Scale1 = One;
    Scale0 = __vnmsubfp(DS, Rcp0, Scale0);
    Scale1 = __vnmsubfp(R0, Rcp1, Scale1);
    R1 = __vcmpeqfp(Rcp0, Rcp0);
    R0 = __vcmpeqfp(Rcp1, Rcp1);
    Rcp0 = __vmaddfp(Rcp0, Scale0, Rcp0);
    Rcp1 = __vmaddfp(Rcp1, Scale1, Rcp1);
    R1 = __vsel(Reciprocal0, Rcp0, R1);
    R0 = __vsel(Reciprocal1, Rcp1, R0);

    R1 = __vmulfp(NS, R1);
    R0 = __vmulfp(DS, R0);

    Result = __vcmpeqfp(V, Zero); // VIsZero

    Select = __vsel(R0, R1, Select);

    Result = __vsel(Select, Zero, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorSinH
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1, V2;
    XMVECTOR E1, E2;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f}; // 1.0f / ln(2.0f)

    V1 = XMVectorMultiplyAdd(V, Scale, g_XMNegativeOne);
    V2 = XMVectorNegativeMultiplySubtract(V, Scale, g_XMNegativeOne);

    E1 = XMVectorExp(V1);
    E2 = XMVectorExp(V2);

    Result = XMVectorSubtract(E1, E2);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               E1, E2;
    XMVECTOR               E[2], S[2];
    XMVECTOR               R[2], R2[2], R3[2], R4[2];
    XMVECTOR               V0[2], V1[2];
    XMVECTOR               C0X, C0Y, C0Z, C0W;
    XMVECTOR               C1X, C1Y, C1Z, C1W;
    XMVECTOR               Reciprocal[2], Rcp[2], Scale[2], Refine[2];
    XMVECTOR               Result;
    static CONST XMVECTOR  S0 = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f};
    static CONST XMVECTOR  C0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  C1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};

    E1 = __vmaddfp(V, S0, g_XMNegativeOne);
    E2 = __vnmsubfp(V, S0, g_XMNegativeOne);

    R[0] = __vrfim(E1);
    R[1] = __vrfim(E2);
    E[0] = __vexptefp(R[0]);
    E[1] = __vexptefp(R[1]);

    C0X = __vspltw(C0, 0);
    C0Y = __vspltw(C0, 1);
    C1X = __vspltw(C1, 0);
    C1Y = __vspltw(C1, 1);

    R[0] = __vsubfp(E1, R[0]);
    R[1] = __vsubfp(E2, R[1]);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2[0] = __vmaddfp(R[0], R[0], *(XMVECTOR*)g_XMNegativeZero);
    R2[1] = __vmaddfp(R[1], R[1], *(XMVECTOR*)g_XMNegativeZero);

    V0[0] = __vmaddfp(R[0], C0Y, C0X);
    V0[1] = __vmaddfp(R[1], C0Y, C0X);
    V1[0] = __vmaddfp(R[0], C1Y, C1X);
    V1[1] = __vmaddfp(R[1], C1Y, C1X);

    R3[0] = __vmaddfp(R[0], R2[0], *(XMVECTOR*)g_XMNegativeZero);
    R3[1] = __vmaddfp(R[1], R2[1], *(XMVECTOR*)g_XMNegativeZero);

    V0[0] = __vmaddfp(R2[0], C0Z, V0[0]);
    V0[1] = __vmaddfp(R2[1], C0Z, V0[1]);
    V1[0] = __vmaddfp(R2[0], C1Z, V1[0]);
    V1[1] = __vmaddfp(R2[1], C1Z, V1[1]);

    V0[0] = __vmaddfp(R3[0], C0W, V0[0]);
    V0[1] = __vmaddfp(R3[1], C0W, V0[1]);

    R4[0] = __vmaddfp(R2[0], R2[0], *(XMVECTOR*)g_XMNegativeZero);
    R4[1] = __vmaddfp(R2[1], R2[1], *(XMVECTOR*)g_XMNegativeZero);

    V1[0] = __vmaddfp(R3[0], C1W, V1[0]);
    V1[1] = __vmaddfp(R3[1], C1W, V1[1]);

    S[0] = __vmaddfp(R4[0], V1[0], V0[0]);
    S[1] = __vmaddfp(R4[1], V1[1], V0[1]);

    Reciprocal[0] = __vrefp(S[0]);
    Reciprocal[1] = __vrefp(S[1]);
    Scale[0] = __vnmsubfp(S[0], Reciprocal[0], g_XMOne);
    Scale[1] = __vnmsubfp(S[1], Reciprocal[1], g_XMOne);
    Rcp[0] = __vmaddfp(Reciprocal[0], Scale[0], Reciprocal[0]);
    Rcp[1] = __vmaddfp(Reciprocal[1], Scale[1], Reciprocal[1]);
    Scale[0] = __vnmsubfp(S[0], Rcp[0], g_XMOne);
    Scale[1] = __vnmsubfp(S[1], Rcp[1], g_XMOne);
    Refine[0] = __vcmpeqfp(Rcp[0], Rcp[0]);
    Refine[1] = __vcmpeqfp(Rcp[1], Rcp[1]);
    Rcp[0] = __vmaddfp(Rcp[0], Scale[0], Rcp[0]);
    Rcp[1] = __vmaddfp(Rcp[1], Scale[1], Rcp[1]);
    S[0] = __vsel(Reciprocal[0], Rcp[0], Refine[0]);
    S[1] = __vsel(Reciprocal[1], Rcp[1], Refine[1]);

    E2 = __vnmsubfp(E[1], S[1], *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(E[0], S[0], E2);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR               E1, E2;
    XMVECTOR               E[2];
    XMVECTOR               R[2], R2[2], R3[2], R4[2];
    XMVECTOR               V0[2], V1[2];
    XMVECTOR               C0Y, C0Z, C0W;
    XMVECTOR               C1Y, C1Z, C1W;
    XMVECTOR               Reciprocal[2], Rcp[2], Scale[2], Refine[2];
    XMVECTOR               Result;
    static CONST XMVECTOR  S0 = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f};
    static CONST XMVECTOR  C0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  C1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};

    XMDUMMY_INITIALIZE_VECTOR(Scale[1]);

    E2 = __vspltisw(-1);
    E1 = __vcfsx(E2, 0);
    E2 = __vcfsx(E2, 0);

    E1 = __vmaddfp(V, S0, E1);
    E2 = __vnmsubfp(V, S0, E2);

    R[0] = __vrfim(E1);
    R[1] = __vrfim(E2);
    E[0] = __vexptefp(R[0]);
    E[1] = __vexptefp(R[1]);

    V0[0] = __vspltw(C0, 0); // C0X
    V0[1] = __vspltw(C0, 0); // C0X
    C0Y = __vspltw(C0, 1);
    V1[0] = __vspltw(C1, 0); // C1X
    V1[1] = __vspltw(C1, 0); // C1X
    C1Y = __vspltw(C1, 1);

    R[0] = __vsubfp(E1, R[0]);
    R[1] = __vsubfp(E2, R[1]);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2[0] = __vmulfp(R[0], R[0]);
    R2[1] = __vmulfp(R[1], R[1]);

    V0[0] = __vmaddfp(R[0], C0Y, V0[0]);
    V0[1] = __vmaddfp(R[1], C0Y, V0[1]);
    V1[0] = __vmaddfp(R[0], C1Y, V1[0]);
    V1[1] = __vmaddfp(R[1], C1Y, V1[1]);

    R3[0] = __vmulfp(R[0], R2[0]);
    R3[1] = __vmulfp(R[1], R2[1]);

    V0[0] = __vmaddfp(R2[0], C0Z, V0[0]);
    V0[1] = __vmaddfp(R2[1], C0Z, V0[1]);
    V1[0] = __vmaddfp(R2[0], C1Z, V1[0]);
    V1[1] = __vmaddfp(R2[1], C1Z, V1[1]);

    V0[0] = __vmaddfp(R3[0], C0W, V0[0]);
    V0[1] = __vmaddfp(R3[1], C0W, V0[1]);

    R4[0] = __vmulfp(R2[0], R2[0]);
    R4[1] = __vmulfp(R2[1], R2[1]);

    V1[0] = __vmaddfp(R3[0], C1W, V1[0]);
    V1[1] = __vmaddfp(R3[1], C1W, V1[1]);

    V0[0] = __vmaddfp(R4[0], V1[0], V0[0]);
    V0[1] = __vmaddfp(R4[1], V1[1], V0[1]);

    Reciprocal[0] = __vrefp(V0[0]);
    Reciprocal[1] = __vrefp(V0[1]);
    Scale[1] = __vupkd3d(Scale[1], VPACK_NORMSHORT2);
    Rcp[0] = __vspltw(Scale[1], 3);
    Rcp[1] = __vspltw(Scale[1], 3);
    Rcp[0] = __vnmsubfp(V0[0], Reciprocal[0], Rcp[0]);
    Rcp[1] = __vnmsubfp(V0[1], Reciprocal[1], Rcp[1]);
    Rcp[0] = __vmaddcfp(Reciprocal[0], Rcp[0], Reciprocal[0]);
    Rcp[1] = __vmaddcfp(Reciprocal[1], Rcp[1], Reciprocal[1]);
    Scale[0] = __vspltw(Scale[1], 3);
    Scale[1] = __vspltw(Scale[1], 3);
    Scale[0] = __vnmsubfp(V0[0], Rcp[0], Scale[0]);
    Scale[1] = __vnmsubfp(V0[1], Rcp[1], Scale[1]);
    Refine[0] = __vcmpeqfp(Rcp[0], Rcp[0]);
    Refine[1] = __vcmpeqfp(Rcp[1], Rcp[1]);
    Rcp[0] = __vmaddfp(Rcp[0], Scale[0], Rcp[0]);
    Rcp[1] = __vmaddfp(Rcp[1], Scale[1], Rcp[1]);
    Result = __vspltisw(0);
    Refine[0] = __vsel(Reciprocal[0], Rcp[0], Refine[0]);
    Refine[1] = __vsel(Reciprocal[1], Rcp[1], Refine[1]);

    Result = __vnmsubfp(E[1], Refine[1], Result);
    Result = __vmaddfp(E[0], Refine[0], Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorCosH
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1, V2;
    XMVECTOR E1, E2;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f}; // 1.0f / ln(2.0f)

    V1 = XMVectorMultiplyAdd(V, Scale, g_XMNegativeOne);
    V2 = XMVectorNegativeMultiplySubtract(V, Scale, g_XMNegativeOne);

    E1 = XMVectorExp(V1);
    E2 = XMVectorExp(V2);

    Result = XMVectorAdd(E1, E2);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               E1, E2;
    XMVECTOR               E[2], S[2];
    XMVECTOR               R[2], R2[2], R3[2], R4[2];
    XMVECTOR               V0[2], V1[2];
    XMVECTOR               C0X, C0Y, C0Z, C0W;
    XMVECTOR               C1X, C1Y, C1Z, C1W;
    XMVECTOR               Reciprocal[2], Rcp[2], Scale[2], Refine[2];
    XMVECTOR               Result;
    static CONST XMVECTOR  S0 = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f};
    static CONST XMVECTOR  C0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  C1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};

    E1 = __vmaddfp(V, S0, g_XMNegativeOne);
    E2 = __vnmsubfp(V, S0, g_XMNegativeOne);

    R[0] = __vrfim(E1);
    R[1] = __vrfim(E2);
    E[0] = __vexptefp(R[0]);
    E[1] = __vexptefp(R[1]);

    C0X = __vspltw(C0, 0);
    C0Y = __vspltw(C0, 1);
    C1X = __vspltw(C1, 0);
    C1Y = __vspltw(C1, 1);

    R[0] = __vsubfp(E1, R[0]);
    R[1] = __vsubfp(E2, R[1]);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2[0] = __vmaddfp(R[0], R[0], *(XMVECTOR*)g_XMNegativeZero);
    R2[1] = __vmaddfp(R[1], R[1], *(XMVECTOR*)g_XMNegativeZero);

    V0[0] = __vmaddfp(R[0], C0Y, C0X);
    V0[1] = __vmaddfp(R[1], C0Y, C0X);
    V1[0] = __vmaddfp(R[0], C1Y, C1X);
    V1[1] = __vmaddfp(R[1], C1Y, C1X);

    R3[0] = __vmaddfp(R[0], R2[0], *(XMVECTOR*)g_XMNegativeZero);
    R3[1] = __vmaddfp(R[1], R2[1], *(XMVECTOR*)g_XMNegativeZero);

    V0[0] = __vmaddfp(R2[0], C0Z, V0[0]);
    V0[1] = __vmaddfp(R2[1], C0Z, V0[1]);
    V1[0] = __vmaddfp(R2[0], C1Z, V1[0]);
    V1[1] = __vmaddfp(R2[1], C1Z, V1[1]);

    V0[0] = __vmaddfp(R3[0], C0W, V0[0]);
    V0[1] = __vmaddfp(R3[1], C0W, V0[1]);

    R4[0] = __vmaddfp(R2[0], R2[0], *(XMVECTOR*)g_XMNegativeZero);
    R4[1] = __vmaddfp(R2[1], R2[1], *(XMVECTOR*)g_XMNegativeZero);

    V1[0] = __vmaddfp(R3[0], C1W, V1[0]);
    V1[1] = __vmaddfp(R3[1], C1W, V1[1]);

    S[0] = __vmaddfp(R4[0], V1[0], V0[0]);
    S[1] = __vmaddfp(R4[1], V1[1], V0[1]);

    Reciprocal[0] = __vrefp(S[0]);
    Reciprocal[1] = __vrefp(S[1]);
    Scale[0] = __vnmsubfp(S[0], Reciprocal[0], g_XMOne);
    Scale[1] = __vnmsubfp(S[1], Reciprocal[1], g_XMOne);
    Rcp[0] = __vmaddfp(Reciprocal[0], Scale[0], Reciprocal[0]);
    Rcp[1] = __vmaddfp(Reciprocal[1], Scale[1], Reciprocal[1]);
    Scale[0] = __vnmsubfp(S[0], Rcp[0], g_XMOne);
    Scale[1] = __vnmsubfp(S[1], Rcp[1], g_XMOne);
    Refine[0] = __vcmpeqfp(Rcp[0], Rcp[0]);
    Refine[1] = __vcmpeqfp(Rcp[1], Rcp[1]);
    Rcp[0] = __vmaddfp(Rcp[0], Scale[0], Rcp[0]);
    Rcp[1] = __vmaddfp(Rcp[1], Scale[1], Rcp[1]);
    S[0] = __vsel(Reciprocal[0], Rcp[0], Refine[0]);
    S[1] = __vsel(Reciprocal[1], Rcp[1], Refine[1]);

    E1 = __vmaddfp(E[0], S[0], *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(E[1], S[1], E1);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR               E1, E2;
    XMVECTOR               E[2];
    XMVECTOR               R[2], R2[2], R3[2], R4[2];
    XMVECTOR               V0[2], V1[2];
    XMVECTOR               C0Y, C0Z, C0W;
    XMVECTOR               C1Y, C1Z, C1W;
    XMVECTOR               Reciprocal[2], Rcp[2], Scale[2], Refine[2];
    static CONST XMVECTOR  S0 = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f};
    static CONST XMVECTOR  C0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  C1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};

    XMDUMMY_INITIALIZE_VECTOR(Scale[1]);

    E2 = __vspltisw(-1);
    E1 = __vcfsx(E2, 0);
    E2 = __vcfsx(E2, 0);

    E1 = __vmaddfp(V, S0, E1);
    E2 = __vnmsubfp(V, S0, E2);

    R[0] = __vrfim(E1);
    R[1] = __vrfim(E2);
    E[0] = __vexptefp(R[0]);
    E[1] = __vexptefp(R[1]);

    V0[0] = __vspltw(C0, 0);
    V0[1] = __vspltw(C0, 0);
    C0Y = __vspltw(C0, 1);
    V1[0] = __vspltw(C1, 0);
    V1[1] = __vspltw(C1, 0);
    C1Y = __vspltw(C1, 1);

    R[0] = __vsubfp(E1, R[0]);
    R[1] = __vsubfp(E2, R[1]);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2[0] = __vmulfp(R[0], R[0]);
    R2[1] = __vmulfp(R[1], R[1]);

    V0[0] = __vmaddfp(R[0], C0Y, V0[0]);
    V0[1] = __vmaddfp(R[1], C0Y, V0[1]);
    V1[0] = __vmaddfp(R[0], C1Y, V1[0]);
    V1[1] = __vmaddfp(R[1], C1Y, V1[1]);

    R3[0] = __vmulfp(R[0], R2[0]);
    R3[1] = __vmulfp(R[1], R2[1]);

    V0[0] = __vmaddfp(R2[0], C0Z, V0[0]);
    V0[1] = __vmaddfp(R2[1], C0Z, V0[1]);
    V1[0] = __vmaddfp(R2[0], C1Z, V1[0]);
    V1[1] = __vmaddfp(R2[1], C1Z, V1[1]);

    V0[0] = __vmaddfp(R3[0], C0W, V0[0]);
    V0[1] = __vmaddfp(R3[1], C0W, V0[1]);

    R4[0] = __vmulfp(R2[0], R2[0]);
    R4[1] = __vmulfp(R2[1], R2[1]);

    V1[0] = __vmaddfp(R3[0], C1W, V1[0]);
    V1[1] = __vmaddfp(R3[1], C1W, V1[1]);

    V0[0] = __vmaddfp(R4[0], V1[0], V0[0]);
    V0[1] = __vmaddfp(R4[1], V1[1], V0[1]);

    Reciprocal[0] = __vrefp(V0[0]);
    Reciprocal[1] = __vrefp(V0[1]);
    Scale[1] = __vupkd3d(Scale[1], VPACK_NORMSHORT2);
    Rcp[0] = __vspltw(Scale[1], 3);
    Rcp[1] = __vspltw(Scale[1], 3);
    Rcp[0] = __vnmsubfp(V0[0], Reciprocal[0], Rcp[0]);
    Rcp[1] = __vnmsubfp(V0[1], Reciprocal[1], Rcp[1]);
    Rcp[0] = __vmaddcfp(Reciprocal[0], Rcp[0], Reciprocal[0]);
    Rcp[1] = __vmaddcfp(Reciprocal[1], Rcp[1], Reciprocal[1]);
    Scale[0] = __vspltw(Scale[1], 3);
    Scale[1] = __vspltw(Scale[1], 3);
    Scale[0] = __vnmsubfp(V0[0], Rcp[0], Scale[0]);
    Scale[1] = __vnmsubfp(V0[1], Rcp[1], Scale[1]);
    Refine[0] = __vcmpeqfp(Rcp[0], Rcp[0]);
    Refine[1] = __vcmpeqfp(Rcp[1], Rcp[1]);
    Rcp[0] = __vmaddfp(Rcp[0], Scale[0], Rcp[0]);
    Rcp[1] = __vmaddfp(Rcp[1], Scale[1], Rcp[1]);
    Refine[0] = __vsel(Reciprocal[0], Rcp[0], Refine[0]);
    Refine[1] = __vsel(Reciprocal[1], Rcp[1], Refine[1]);

    E1 = __vmulfp(E[0], Refine[0]);
    E1 = __vmaddfp(E[1], Refine[1], E1);

    return E1;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorTanH
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR E;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f}; // 2.0f / ln(2.0f)

    E = XMVectorMultiply(V, Scale);
    E = XMVectorExp(E);
    E = XMVectorMultiplyAdd(E, g_XMOneHalf, g_XMOneHalf);
    E = XMVectorReciprocal(E);

    Result = XMVectorSubtract(g_XMOne, E);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR E;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f}; // 2.0f / ln(2.0f)

    E = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
    E = XMVectorExp(E);
    E = __vmaddfp(E, g_XMOneHalf, g_XMOneHalf);
    E = XMVectorReciprocal(E);

    Result = __vsubfp(g_XMOne, E);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR               VE, VR;
    XMVECTOR               E;
    XMVECTOR               R, R2, R3, R4;
    XMVECTOR               V0, V1;
    XMVECTOR               C0Y, C0Z, C0W;
    XMVECTOR               C1Y, C1Z, C1W;
    XMVECTOR               Reciprocal, Scale1, Scale2, Refine;
    XMVECTOR               S0, S1;
    XMVECTOR               One, OneInt;
    XMVECTOR               Result;
    static CONST XMVECTOR  C0 = {1.0f, -6.93147182e-1f, 2.40226462e-1f, -5.55036440e-2f};
    static CONST XMVECTOR  C1 = {9.61597636e-3f, -1.32823968e-3f, 1.47491097e-4f, -1.08635004e-5f};
    static CONST XMVECTOR  Scale = {2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f}; // 2.0f / ln(2.0f)

    XMDUMMY_INITIALIZE_VECTOR(One);

    VE = __vmulfp(V, Scale);

    R = __vrfim(VE);
    E = __vexptefp(R);

    V0 = __vspltw(C0, 0);
    C0Y = __vspltw(C0, 1);
    V1 = __vspltw(C1, 0);
    C1Y = __vspltw(C1, 1);

    R = __vsubfp(VE, R);

    C0Z = __vspltw(C0, 2);
    C0W = __vspltw(C0, 3);
    C1Z = __vspltw(C1, 2);
    C1W = __vspltw(C1, 3);

    R2 = __vmulfp(R, R);

    V0 = __vmaddfp(R, C0Y, V0);
    V1 = __vmaddfp(R, C1Y, V1);

    R3 = __vmulfp(R, R2);

    V0 = __vmaddfp(R2, C0Z, V0);
    V1 = __vmaddfp(R2, C1Z, V1);

    V0 = __vmaddfp(R3, C0W, V0);

    R4 = __vmulfp(R2, R2);

    V1 = __vmaddfp(R3, C1W, V1);

    V0 = __vmaddfp(R4, V1, V0);

    Reciprocal = __vrefp(V0);
    One = __vupkd3d(One, VPACK_NORMSHORT2);
    Scale1 = __vspltw(One, 3);

    Scale1 = __vnmsubfp(V0, Reciprocal, Scale1);
    Scale2 = __vspltw(One, 3);
    Scale1 = __vmaddcfp(Reciprocal, Scale1, Reciprocal);
    Scale2 = __vnmsubfp(V0, Scale1, Scale2);
    Refine = __vcmpeqfp(Scale1, Scale1);
    Scale1 = __vmaddfp(Scale1, Scale2, Scale1);
    OneInt = __vspltisw(1);
    Refine = __vsel(Reciprocal, Scale1, Refine);

    VR = __vcfsx(OneInt, 1);

    VE = __vmulfp(E, Refine);

    VR = __vmaddfp(VE, VR, VR);

    Reciprocal = __vrefp(VR);

    S0 = __vspltw(One, 3);
    S0 = __vnmsubfp(VR, Reciprocal, S0);
    S1 = __vspltw(One, 3);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);

    S1 = __vnmsubfp(VR, S0, S1);

    VR = __vcmpeqfp(S0, S0);

    One = __vspltw(One, 3);

    S0 = __vmaddfp(S0, S1, S0);

    VR = __vsel(Reciprocal, S0, VR);

    Result = __vsubfp(One, VR);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorASin
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V2, V3, AbsV;
    XMVECTOR C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR R0, R1, R2, R3, R4;
    XMVECTOR OneMinusAbsV;
    XMVECTOR Rsq;
    XMVECTOR Result;
    static CONST XMVECTOR OnePlusEpsilon = {1.00000011921f, 1.00000011921f, 1.00000011921f, 1.00000011921f};

    // asin(V) = V * (C0 + C1 * V + C2 * V^2 + C3 * V^3 + C4 * V^4 + C5 * V^5) + (1 - V) * rsq(1 - V) * 
    //           V * (C6 + C7 * V + C8 * V^2 + C9 * V^3 + C10 * V^4 + C11 * V^5)

    AbsV = XMVectorAbs(V);

    V2 = XMVectorMultiply(V, V);
    V3 = XMVectorMultiply(V2, AbsV);

    R4 = XMVectorNegativeMultiplySubtract(AbsV, V, V);

    OneMinusAbsV = XMVectorSubtract(OnePlusEpsilon, AbsV);
    Rsq = XMVectorReciprocalSqrt(OneMinusAbsV);

    C0 = XMVectorSplatX(g_XMASinCoefficients0);
    C1 = XMVectorSplatY(g_XMASinCoefficients0);
    C2 = XMVectorSplatZ(g_XMASinCoefficients0);
    C3 = XMVectorSplatW(g_XMASinCoefficients0);

    C4 = XMVectorSplatX(g_XMASinCoefficients1);
    C5 = XMVectorSplatY(g_XMASinCoefficients1);
    C6 = XMVectorSplatZ(g_XMASinCoefficients1);
    C7 = XMVectorSplatW(g_XMASinCoefficients1);

    C8 = XMVectorSplatX(g_XMASinCoefficients2);
    C9 = XMVectorSplatY(g_XMASinCoefficients2);
    C10 = XMVectorSplatZ(g_XMASinCoefficients2);
    C11 = XMVectorSplatW(g_XMASinCoefficients2);

    R0 = XMVectorMultiplyAdd(C3, AbsV, C7);
    R1 = XMVectorMultiplyAdd(C1, AbsV, C5);
    R2 = XMVectorMultiplyAdd(C2, AbsV, C6);
    R3 = XMVectorMultiplyAdd(C0, AbsV, C4);

    R0 = XMVectorMultiplyAdd(R0, AbsV, C11);
    R1 = XMVectorMultiplyAdd(R1, AbsV, C9);
    R2 = XMVectorMultiplyAdd(R2, AbsV, C10);
    R3 = XMVectorMultiplyAdd(R3, AbsV, C8);

    R0 = XMVectorMultiplyAdd(R2, V3, R0);
    R1 = XMVectorMultiplyAdd(R3, V3, R1);

    R0 = XMVectorMultiply(V, R0);
    R1 = XMVectorMultiply(R4, R1);

    Result = XMVectorMultiplyAdd(R1, Rsq, R0);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V2, V3, AbsV;
    XMVECTOR C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR R0, R1, R2, R3, R4;
    XMVECTOR OneMinusAbsV, HalfOneMinusAbsV;
    XMVECTOR Rsq, Scale;
    XMVECTOR SignMask;
    XMVECTOR Result;
    static CONST XMVECTOR OnePlusEpsilon = {1.00000011921f, 1.00000011921f, 1.00000011921f, 1.00000011921f};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    V2 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    AbsV = __vandc(V, SignMask);

    C3 = __vspltw(g_XMASinCoefficients0, 3);
    C7 = __vspltw(g_XMASinCoefficients1, 3);
    C1 = __vspltw(g_XMASinCoefficients0, 1);
    C5 = __vspltw(g_XMASinCoefficients1, 1);
    C2 = __vspltw(g_XMASinCoefficients0, 2);
    C6 = __vspltw(g_XMASinCoefficients1, 2);
    C0 = __vspltw(g_XMASinCoefficients0, 0);
    C4 = __vspltw(g_XMASinCoefficients1, 0);

    R0 = __vmaddfp(C3, AbsV, C7);
    R1 = __vmaddfp(C1, AbsV, C5);
    R2 = __vmaddfp(C2, AbsV, C6);
    R3 = __vmaddfp(C0, AbsV, C4);

    V3 = __vmaddfp(V2, AbsV, *(XMVECTOR*)g_XMNegativeZero);
    OneMinusAbsV = __vsubfp(OnePlusEpsilon, AbsV);

    C11 = __vspltw(g_XMASinCoefficients2, 3);
    C9 = __vspltw(g_XMASinCoefficients2, 1);
    C10 = __vspltw(g_XMASinCoefficients2, 2);
    C8 = __vspltw(g_XMASinCoefficients2, 0);

    Rsq = __vrsqrtefp(OneMinusAbsV);
    HalfOneMinusAbsV = __vmaddfp(OneMinusAbsV, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);

    R0 = __vmaddfp(R0, AbsV, C11);
    R1 = __vmaddfp(R1, AbsV, C9);
    R2 = __vmaddfp(R2, AbsV, C10);
    R3 = __vmaddfp(R3, AbsV, C8);

    Scale = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);

    R4 = __vnmsubfp(AbsV, V, V);

    R0 = __vmaddfp(R2, V3, R0);
    R1 = __vmaddfp(R3, V3, R1);

    Scale = __vnmsubfp(HalfOneMinusAbsV, Scale, g_XMOneHalf);

    R0 = __vmaddfp(V, R0, *(XMVECTOR*)g_XMNegativeZero);
    R1 = __vmaddfp(R4, R1, *(XMVECTOR*)g_XMNegativeZero);

    Rsq = __vmaddfp(Rsq, Scale, Rsq);

    Result = __vmaddfp(R1, Rsq, R0);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V2, V3, AbsV;
    XMVECTOR C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR R0, R1, R2;
    XMVECTOR OneMinusAbsV, HalfOneMinusAbsV;
    XMVECTOR Rsq, Scale, H;
    XMVECTOR SignMask;
    static CONST XMVECTOR OnePlusEpsilon = {1.00000011921f, 1.00000011921f, 1.00000011921f, 1.00000011921f};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    V2 = __vmulfp(V, V);
    AbsV = __vandc(V, SignMask);

    C3 = __vspltw(g_XMASinCoefficients0, 3);
    C7 = __vspltw(g_XMASinCoefficients1, 3);
    C1 = __vspltw(g_XMASinCoefficients0, 1);
    C5 = __vspltw(g_XMASinCoefficients1, 1);
    C2 = __vspltw(g_XMASinCoefficients0, 2);
    C6 = __vspltw(g_XMASinCoefficients1, 2);
    C0 = __vspltw(g_XMASinCoefficients0, 0);
    C4 = __vspltw(g_XMASinCoefficients1, 0);

    OneMinusAbsV = __vsubfp(OnePlusEpsilon, AbsV);

    H = __vspltisw(1);

    C7 = __vmaddfp(C3, AbsV, C7);
    C5 = __vmaddfp(C1, AbsV, C5);
    C6 = __vmaddfp(C2, AbsV, C6);
    C4 = __vmaddfp(C0, AbsV, C4);

    V3 = __vmulfp(V2, AbsV);

    H = __vcfsx(H, 1);

    C11 = __vspltw(g_XMASinCoefficients2, 3);
    C9 = __vspltw(g_XMASinCoefficients2, 1);
    C10 = __vspltw(g_XMASinCoefficients2, 2);
    C8 = __vspltw(g_XMASinCoefficients2, 0);

    Rsq = __vrsqrtefp(OneMinusAbsV);
    HalfOneMinusAbsV = __vmulfp(OneMinusAbsV, H);

    R2 = V;

    C11 = __vmaddfp(C7, AbsV, C11);
    C9 = __vmaddfp(C5, AbsV, C9);
    C10 = __vmaddfp(C6, AbsV, C10);
    C8 = __vmaddfp(C4, AbsV, C8);

    Scale = __vmulfp(Rsq, Rsq);

    R2 = __vnmsubfp(AbsV, R2, R2);

    C11 = __vmaddfp(C10, V3, C11);
    C9 = __vmaddfp(C8, V3, C9);

    H = __vnmsubfp(HalfOneMinusAbsV, Scale, H);

    R0 = __vmulfp(V, C11);
    R1 = __vmulfp(R2, C9);

    Rsq = __vmaddfp(Rsq, H, Rsq);

    R0 = __vmaddfp(R1, Rsq, R0);

    return R0;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorACos
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V2, V3, AbsV;
    XMVECTOR C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR R0, R1, R2, R3, R4;
    XMVECTOR OneMinusAbsV;
    XMVECTOR Rsq;
    XMVECTOR Result;
    static CONST XMVECTOR OnePlusEpsilon = {1.00000011921f, 1.00000011921f, 1.00000011921f, 1.00000011921f};

    // acos(V) = PI / 2 - asin(V)

    AbsV = XMVectorAbs(V);

    V2 = XMVectorMultiply(V, V);
    V3 = XMVectorMultiply(V2, AbsV);

    R4 = XMVectorNegativeMultiplySubtract(AbsV, V, V);

    OneMinusAbsV = XMVectorSubtract(OnePlusEpsilon, AbsV);
    Rsq = XMVectorReciprocalSqrt(OneMinusAbsV);

    C0 = XMVectorSplatX(g_XMASinCoefficients0);
    C1 = XMVectorSplatY(g_XMASinCoefficients0);
    C2 = XMVectorSplatZ(g_XMASinCoefficients0);
    C3 = XMVectorSplatW(g_XMASinCoefficients0);

    C4 = XMVectorSplatX(g_XMASinCoefficients1);
    C5 = XMVectorSplatY(g_XMASinCoefficients1);
    C6 = XMVectorSplatZ(g_XMASinCoefficients1);
    C7 = XMVectorSplatW(g_XMASinCoefficients1);

    C8 = XMVectorSplatX(g_XMASinCoefficients2);
    C9 = XMVectorSplatY(g_XMASinCoefficients2);
    C10 = XMVectorSplatZ(g_XMASinCoefficients2);
    C11 = XMVectorSplatW(g_XMASinCoefficients2);

    R0 = XMVectorMultiplyAdd(C3, AbsV, C7);
    R1 = XMVectorMultiplyAdd(C1, AbsV, C5);
    R2 = XMVectorMultiplyAdd(C2, AbsV, C6);
    R3 = XMVectorMultiplyAdd(C0, AbsV, C4);

    R0 = XMVectorMultiplyAdd(R0, AbsV, C11);
    R1 = XMVectorMultiplyAdd(R1, AbsV, C9);
    R2 = XMVectorMultiplyAdd(R2, AbsV, C10);
    R3 = XMVectorMultiplyAdd(R3, AbsV, C8);

    R0 = XMVectorMultiplyAdd(R2, V3, R0);
    R1 = XMVectorMultiplyAdd(R3, V3, R1);

    R0 = XMVectorMultiply(V, R0);
    R1 = XMVectorMultiply(R4, R1);

    Result = XMVectorMultiplyAdd(R1, Rsq, R0);

    Result = XMVectorSubtract(g_XMHalfPi, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V2, V3, AbsV;
    XMVECTOR C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR R0, R1, R2, R3, R4;
    XMVECTOR OneMinusAbsV, HalfOneMinusAbsV;
    XMVECTOR Rsq, Scale;
    XMVECTOR SignMask;
    XMVECTOR Result;
    static CONST XMVECTOR OnePlusEpsilon = {1.00000011921f, 1.00000011921f, 1.00000011921f, 1.00000011921f};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    V2 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    AbsV = __vandc(V, SignMask);

    C3 = __vspltw(g_XMASinCoefficients0, 3);
    C7 = __vspltw(g_XMASinCoefficients1, 3);
    C1 = __vspltw(g_XMASinCoefficients0, 1);
    C5 = __vspltw(g_XMASinCoefficients1, 1);
    C2 = __vspltw(g_XMASinCoefficients0, 2);
    C6 = __vspltw(g_XMASinCoefficients1, 2);
    C0 = __vspltw(g_XMASinCoefficients0, 0);
    C4 = __vspltw(g_XMASinCoefficients1, 0);

    R0 = __vmaddfp(C3, AbsV, C7);
    R1 = __vmaddfp(C1, AbsV, C5);
    R2 = __vmaddfp(C2, AbsV, C6);
    R3 = __vmaddfp(C0, AbsV, C4);

    V3 = __vmaddfp(V2, AbsV, *(XMVECTOR*)g_XMNegativeZero);
    OneMinusAbsV = __vsubfp(OnePlusEpsilon, AbsV);

    C11 = __vspltw(g_XMASinCoefficients2, 3);
    C9 = __vspltw(g_XMASinCoefficients2, 1);
    C10 = __vspltw(g_XMASinCoefficients2, 2);
    C8 = __vspltw(g_XMASinCoefficients2, 0);

    Rsq = __vrsqrtefp(OneMinusAbsV);
    HalfOneMinusAbsV = __vmaddfp(OneMinusAbsV, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);

    R0 = __vmaddfp(R0, AbsV, C11);
    R1 = __vmaddfp(R1, AbsV, C9);
    R2 = __vmaddfp(R2, AbsV, C10);
    R3 = __vmaddfp(R3, AbsV, C8);

    Scale = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);

    R4 = __vnmsubfp(AbsV, V, V);

    R0 = __vmaddfp(R2, V3, R0);
    R1 = __vmaddfp(R3, V3, R1);

    Scale = __vnmsubfp(HalfOneMinusAbsV, Scale, g_XMOneHalf);

    R0 = __vmaddfp(V, R0, *(XMVECTOR*)g_XMNegativeZero);
    R1 = __vmaddfp(R4, R1, *(XMVECTOR*)g_XMNegativeZero);

    Rsq = __vmaddfp(Rsq, Scale, Rsq);

    Result = __vmaddfp(R1, Rsq, R0);

    Result = __vsubfp(g_XMHalfPi, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V2, V3, AbsV;
    XMVECTOR C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11;
    XMVECTOR R0, R1, R2;
    XMVECTOR OneMinusAbsV, HalfOneMinusAbsV;
    XMVECTOR Rsq, Scale, H;
    XMVECTOR HalfPi;
    XMVECTOR SignMask;
    static CONST XMVECTOR OnePlusEpsilon = {1.00000011921f, 1.00000011921f, 1.00000011921f, 1.00000011921f};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    V2 = __vmulfp(V, V);
    AbsV = __vandc(V, SignMask);

    C3 = __vspltw(g_XMASinCoefficients0, 3);
    C7 = __vspltw(g_XMASinCoefficients1, 3);
    C1 = __vspltw(g_XMASinCoefficients0, 1);
    C5 = __vspltw(g_XMASinCoefficients1, 1);
    C2 = __vspltw(g_XMASinCoefficients0, 2);
    C6 = __vspltw(g_XMASinCoefficients1, 2);
    C0 = __vspltw(g_XMASinCoefficients0, 0);
    C4 = __vspltw(g_XMASinCoefficients1, 0);

    OneMinusAbsV = __vsubfp(OnePlusEpsilon, AbsV);

    H = __vspltisw(1);

    C7 = __vmaddfp(C3, AbsV, C7);
    C5 = __vmaddfp(C1, AbsV, C5);
    C6 = __vmaddfp(C2, AbsV, C6);
    C4 = __vmaddfp(C0, AbsV, C4);

    V3 = __vmulfp(V2, AbsV);

    H = __vcfsx(H, 1);

    C11 = __vspltw(g_XMASinCoefficients2, 3);
    C9 = __vspltw(g_XMASinCoefficients2, 1);
    C10 = __vspltw(g_XMASinCoefficients2, 2);
    C8 = __vspltw(g_XMASinCoefficients2, 0);

    Rsq = __vrsqrtefp(OneMinusAbsV);
    HalfOneMinusAbsV = __vmulfp(OneMinusAbsV, H);

    R2 = V;

    C11 = __vmaddfp(C7, AbsV, C11);
    C9 = __vmaddfp(C5, AbsV, C9);
    C10 = __vmaddfp(C6, AbsV, C10);
    C8 = __vmaddfp(C4, AbsV, C8);

    HalfPi = __vspltw(g_XMPiConstants0, 0);

    Scale = __vmulfp(Rsq, Rsq);

    R2 = __vnmsubfp(AbsV, R2, R2);

    C11 = __vmaddfp(C10, V3, C11);
    C9 = __vmaddfp(C8, V3, C9);

    HalfPi = __vmulfp(HalfPi, H);

    H = __vnmsubfp(HalfOneMinusAbsV, Scale, H);

    R0 = __vmulfp(V, C11);
    R1 = __vmulfp(R2, C9);

    Rsq = __vmaddfp(Rsq, H, Rsq);

    R0 = __vmaddfp(R1, Rsq, R0);

    R0 = __vsubfp(HalfPi, R0);

    return R0;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorATan
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    // Cody and Waite algorithm to compute inverse tangent.

    XMVECTOR P[4];
    XMVECTOR Q[4];
    XMVECTOR N, D;
    XMVECTOR VF, G, ReciprocalF, AbsF, FA, FB;
    XMVECTOR Sqrt3, Sqrt3MinusOne, TwoMinusSqrt3;
    XMVECTOR HalfPi, OneThirdPi, OneSixthPi, Epsilon, MinV, MaxV;
    XMVECTOR Zero;
    XMVECTOR NegativeHalfPi;
    XMVECTOR Angle1, Angle2;
    XMVECTOR F_GT_One, F_GT_TwoMinusSqrt3, AbsF_LT_Epsilon, V_LT_Zero, V_GT_MaxV, V_LT_MinV;
    XMVECTOR NegativeResult, Result;
    static CONST XMVECTOR ATanConstants0 = {-1.3688768894e+1f, -2.0505855195e+1f, -8.4946240351f, -8.3758299368e-1f};
    static CONST XMVECTOR ATanConstants1 = {4.1066306682e+1f, 8.6157349597e+1f, 5.9578436142e+1f, 1.5024001160e+1f};
    static CONST XMVECTOR ATanConstants2 = {1.732050808f, 7.320508076e-1f, 2.679491924e-1f, 0.000244140625f}; // <sqrt(3), sqrt(3) - 1, 2 - sqrt(3), Epsilon>
    static CONST XMVECTOR ATanConstants3 = {XM_PIDIV2, XM_PI / 3.0f, XM_PI / 6.0f, 8.507059173e+37f}; // <Pi / 2, Pi / 3, Pi / 6, MaxV>

    Zero = XMVectorZero();

    P[0] = XMVectorSplatX(ATanConstants0);
    P[1] = XMVectorSplatY(ATanConstants0);
    P[2] = XMVectorSplatZ(ATanConstants0);
    P[3] = XMVectorSplatW(ATanConstants0);

    Q[0] = XMVectorSplatX(ATanConstants1);
    Q[1] = XMVectorSplatY(ATanConstants1);
    Q[2] = XMVectorSplatZ(ATanConstants1);
    Q[3] = XMVectorSplatW(ATanConstants1);

    Sqrt3 = XMVectorSplatX(ATanConstants2);
    Sqrt3MinusOne = XMVectorSplatY(ATanConstants2);
    TwoMinusSqrt3 = XMVectorSplatZ(ATanConstants2);
    Epsilon = XMVectorSplatW(ATanConstants2);

    HalfPi = XMVectorSplatX(ATanConstants3);
    OneThirdPi = XMVectorSplatY(ATanConstants3);
    OneSixthPi = XMVectorSplatZ(ATanConstants3);
    MaxV = XMVectorSplatW(ATanConstants3);

    VF = XMVectorAbs(V);
    ReciprocalF = XMVectorReciprocal(VF);

    F_GT_One = XMVectorGreater(VF, g_XMOne);

    VF = XMVectorSelect(VF, ReciprocalF, F_GT_One);
    Angle1 = XMVectorSelect(Zero, HalfPi, F_GT_One);
    Angle2 = XMVectorSelect(OneSixthPi, OneThirdPi, F_GT_One);

    F_GT_TwoMinusSqrt3 = XMVectorGreater(VF, TwoMinusSqrt3);

    FA = XMVectorMultiplyAdd(Sqrt3MinusOne, VF, VF);
    FA = XMVectorAdd(FA, g_XMNegativeOne);
    FB = XMVectorAdd(VF, Sqrt3);
    FB = XMVectorReciprocal(FB);
    FA = XMVectorMultiply(FA, FB);

    VF = XMVectorSelect(VF, FA, F_GT_TwoMinusSqrt3);
    Angle1 = XMVectorSelect(Angle1, Angle2, F_GT_TwoMinusSqrt3);

    AbsF = XMVectorAbs(VF);
    AbsF_LT_Epsilon = XMVectorLess(AbsF, Epsilon);

    G = XMVectorMultiply(VF, VF);

    D = XMVectorAdd(G, Q[3]);
    D = XMVectorMultiplyAdd(D, G, Q[2]);
    D = XMVectorMultiplyAdd(D, G, Q[1]);
    D = XMVectorMultiplyAdd(D, G, Q[0]);
    D = XMVectorReciprocal(D);

    N = XMVectorMultiplyAdd(P[3], G, P[2]);
    N = XMVectorMultiplyAdd(N, G, P[1]);
    N = XMVectorMultiplyAdd(N, G, P[0]);
    N = XMVectorMultiply(N, G);
    Result = XMVectorMultiply(N, D);

    Result = XMVectorMultiplyAdd(Result, VF, VF);

    Result = XMVectorSelect(Result, VF, AbsF_LT_Epsilon);

    NegativeResult = XMVectorNegate(Result);
    Result = XMVectorSelect(Result, NegativeResult, F_GT_One);

    Result = XMVectorAdd(Result, Angle1);

    V_LT_Zero = XMVectorLess(V, Zero);
    NegativeResult = XMVectorNegate(Result);
    Result = XMVectorSelect(Result, NegativeResult, V_LT_Zero);

    MinV = XMVectorNegate(MaxV);
    NegativeHalfPi = XMVectorNegate(HalfPi);
    V_GT_MaxV = XMVectorGreater(V, MaxV);
    V_LT_MinV = XMVectorLess(V, MinV);
    Result = XMVectorSelect(Result, g_XMHalfPi, V_GT_MaxV);
    Result = XMVectorSelect(Result, NegativeHalfPi, V_LT_MinV);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR P[4];
    XMVECTOR Q[4];
    XMVECTOR N, D;
    XMVECTOR VF, G, ReciprocalF, AbsF, FA, FB;
    XMVECTOR Sqrt3, Sqrt3MinusOne, TwoMinusSqrt3;
    XMVECTOR HalfPi, OneThirdPi, OneSixthPi, Epsilon, MinV, MaxV;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    XMVECTOR Scale, RcpE, Rcp, Refine;
    XMVECTOR NegativeHalfPi;
    XMVECTOR Angle1, Angle2;
    XMVECTOR F_GT_One, F_GT_TwoMinusSqrt3, AbsF_LT_Epsilon, V_LT_Zero, V_GT_MaxV, V_LT_MinV;
    XMVECTOR NegativeResult, Result;
    static CONST XMVECTOR ATanConstants0 = {-1.3688768894e+1f, -2.0505855195e+1f, -8.4946240351f, -8.3758299368e-1f};
    static CONST XMVECTOR ATanConstants1 = {4.1066306682e+1f, 8.6157349597e+1f, 5.9578436142e+1f, 1.5024001160e+1f};
    static CONST XMVECTOR ATanConstants2 = {1.732050808f, 7.320508076e-1f, 2.679491924e-1f, 0.000244140625f}; // <sqrt(3), sqrt(3) - 1, 2 - sqrt(3), Epsilon>
    static CONST XMVECTOR ATanConstants3 = {XM_PIDIV2, XM_PI / 3.0f, XM_PI / 6.0f, 8.507059173e+37f}; // <Pi / 2, Pi / 3, Pi / 6, MaxV>

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    VF = __vandc(V, SignMask);
    Zero          = __vspltisw(0);
    HalfPi        = __vspltw(ATanConstants3, 0);
    OneThirdPi    = __vspltw(ATanConstants3, 1);
    OneSixthPi    = __vspltw(ATanConstants3, 2);
    MaxV          = __vspltw(ATanConstants3, 3);

    ReciprocalF = __vrefp(VF);
    F_GT_One = __vcmpgtfp(VF, g_XMOne);
    Sqrt3         = __vspltw(ATanConstants2, 0);
    Sqrt3MinusOne = __vspltw(ATanConstants2, 1);
    TwoMinusSqrt3 = __vspltw(ATanConstants2, 2);
    Epsilon       = __vspltw(ATanConstants2, 3);
    V_LT_Zero = __vcmpgtfp(Zero, V);
    NegativeHalfPi = __vxor(HalfPi, SignMask);

    Scale = __vnmsubfp(VF, ReciprocalF, g_XMOne);
    Angle1 = __vsel(Zero, HalfPi, F_GT_One);
    Angle2 = __vsel(OneSixthPi, OneThirdPi, F_GT_One);
    MinV = __vxor(MaxV, SignMask);

    Rcp = __vmaddfp(ReciprocalF, Scale, ReciprocalF);
    Q[0] = __vspltw(ATanConstants1, 0);
    Q[1] = __vspltw(ATanConstants1, 1);
    Q[2] = __vspltw(ATanConstants1, 2);
    Q[3] = __vspltw(ATanConstants1, 3);

    Scale = __vnmsubfp(VF, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    P[0] = __vspltw(ATanConstants0, 0);
    P[1] = __vspltw(ATanConstants0, 1);
    P[2] = __vspltw(ATanConstants0, 2);
    P[3] = __vspltw(ATanConstants0, 3);
    V_GT_MaxV = __vcmpgtfp(V, MaxV);
    V_LT_MinV = __vcmpgtfp(MinV, V);

    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    ReciprocalF = __vsel(ReciprocalF, Rcp, Refine);

    VF = __vsel(VF, ReciprocalF, F_GT_One);

    FB = __vaddfp(VF, Sqrt3);

    FA = __vmaddfp(Sqrt3MinusOne, VF, VF);

    F_GT_TwoMinusSqrt3 = __vcmpgtfp(VF, TwoMinusSqrt3);

    RcpE = __vrefp(FB);

    FA = __vaddfp(FA, g_XMNegativeOne);

    Angle1 = __vsel(Angle1, Angle2, F_GT_TwoMinusSqrt3);

    Scale = __vnmsubfp(FB, RcpE, g_XMOne);
    Rcp = __vmaddfp(RcpE, Scale, RcpE);
    Scale = __vnmsubfp(FB, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    FB = __vsel(RcpE, Rcp, Refine);

    FA = __vmaddfp(FA, FB, *(XMVECTOR*)g_XMNegativeZero);

    VF = __vsel(VF, FA, F_GT_TwoMinusSqrt3);

    AbsF = __vandc(VF, SignMask);
    G = __vmaddfp(VF, VF, *(XMVECTOR*)g_XMNegativeZero);
    AbsF_LT_Epsilon = __vcmpgtfp(Epsilon, AbsF);

    D = __vaddfp(G, Q[3]);
    N = __vmaddfp(P[3], G, P[2]);
    D = __vmaddfp(D, G, Q[2]);
    N = __vmaddfp(N, G, P[1]);
    D = __vmaddfp(D, G, Q[1]);
    N = __vmaddfp(N, G, P[0]);
    D = __vmaddfp(D, G, Q[0]);
    N = __vmaddfp(N, G, *(XMVECTOR*)g_XMNegativeZero);

    RcpE = __vrefp(D);
    Scale = __vnmsubfp(D, RcpE, g_XMOne);
    Rcp = __vmaddfp(RcpE, Scale, RcpE);
    Scale = __vnmsubfp(D, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    D = __vsel(RcpE, Rcp, Refine);

    Result = __vmaddfp(N, D, *(XMVECTOR*)g_XMNegativeZero);

    Result = __vmaddfp(Result, VF, VF);

    Result = __vsel(Result, VF, AbsF_LT_Epsilon);

    NegativeResult = __vxor(Result, SignMask);
    Result = __vsel(Result, NegativeResult, F_GT_One);

    Result = __vaddfp(Result, Angle1);

    NegativeResult = __vxor(Result, SignMask);
    Result = __vsel(Result, NegativeResult, V_LT_Zero);

    Result = __vsel(Result, g_XMHalfPi, V_GT_MaxV);
    Result = __vsel(Result, NegativeHalfPi, V_LT_MinV);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR P[4];
    XMVECTOR Q[4];
    XMVECTOR N, D;
    XMVECTOR VF, G, ReciprocalF, AbsF, FA, FB;
    XMVECTOR Sqrt3, Sqrt3MinusOne, TwoMinusSqrt3;
    XMVECTOR HalfPi, OneThirdPi, OneSixthPi, Epsilon, MinV, MaxV;
    XMVECTOR SignMask;
    XMVECTOR OneW, One, Zero, NegativeOne;
    XMVECTOR Scale, RcpE, Rcp, Refine;
    XMVECTOR NegativeHalfPi;
    XMVECTOR Angle1, Angle2;
    XMVECTOR GT1, GT2, F_GT_One, AbsF_LT_Epsilon, V_LT_Zero, V_GT_MaxV, V_LT_MinV;
    XMVECTOR NegativeResult, Result;
    static CONST XMVECTOR ATanConstants0 = {-1.3688768894e+1f, -2.0505855195e+1f, -8.4946240351f, -8.3758299368e-1f};
    static CONST XMVECTOR ATanConstants1 = {4.1066306682e+1f, 8.6157349597e+1f, 5.9578436142e+1f, 1.5024001160e+1f};
    static CONST XMVECTOR ATanConstants2 = {1.732050808f, 7.320508076e-1f, 2.679491924e-1f, 0.000244140625f}; // <sqrt(3), sqrt(3) - 1, 2 - sqrt(3), Epsilon>
    static CONST XMVECTOR ATanConstants3 = {XM_PIDIV2, XM_PI / 3.0f, XM_PI / 6.0f, 8.507059173e+37f}; // <Pi / 2, Pi / 3, Pi / 6, MaxV>

    XMDUMMY_INITIALIZE_VECTOR(OneW);

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Zero = __vspltisw(0);

    OneW = __vupkd3d(OneW, VPACK_NORMSHORT2);

    VF = __vandc(V, SignMask);
    HalfPi = __vspltw(ATanConstants3, 0);
    OneThirdPi = __vspltw(ATanConstants3, 1);
    OneSixthPi = __vspltw(ATanConstants3, 2);
    MaxV = __vspltw(ATanConstants3, 3);

    One = __vspltw(OneW, 3);

    ReciprocalF = __vrefp(VF);
    Angle1 = __vcmpgtfp(VF, One);
    Angle2 = __vcmpgtfp(VF, One);
    F_GT_One = __vcmpgtfp(VF, One);
    Sqrt3 = __vspltw(ATanConstants2, 0);
    Sqrt3MinusOne = __vspltw(ATanConstants2, 1);
    TwoMinusSqrt3 = __vspltw(ATanConstants2, 2);
    Epsilon = __vspltw(ATanConstants2, 3);
    V_LT_Zero = __vcmpgtfp(Zero, V);
    NegativeHalfPi = __vxor(HalfPi, SignMask);
    Scale = __vspltw(OneW, 3);

    Scale = __vnmsubfp(VF, ReciprocalF, Scale);
    Rcp = ReciprocalF;
    Angle1 = __vsel(Zero, HalfPi, Angle1);
    Angle2 = __vsel(OneSixthPi, OneThirdPi, Angle2);
    MinV = __vxor(MaxV, SignMask);

    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Scale = __vspltw(OneW, 3);
    Q[0] = __vspltw(ATanConstants1, 0);
    Q[1] = __vspltw(ATanConstants1, 1);
    Q[2] = __vspltw(ATanConstants1, 2);
    Q[3] = __vspltw(ATanConstants1, 3);

    Scale = __vnmsubfp(VF, Rcp, Scale);
    Refine = __vcmpeqfp(Rcp, Rcp);
    P[0] = __vspltw(ATanConstants0, 0);
    P[1] = __vspltw(ATanConstants0, 1);
    P[2] = __vspltw(ATanConstants0, 2);
    N = __vspltw(ATanConstants0, 3);
    V_GT_MaxV = __vcmpgtfp(V, MaxV);
    V_LT_MinV = __vcmpgtfp(MinV, V);

    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Refine = __vsel(ReciprocalF, Rcp, Refine);

    VF = __vsel(VF, Refine, F_GT_One);

    FB = __vaddfp(VF, Sqrt3);

    GT1 = __vcmpgtfp(VF, TwoMinusSqrt3); // F_GT_TwoMinusSqrt3
    GT2 = __vcmpgtfp(VF, TwoMinusSqrt3);

    Sqrt3MinusOne = __vmaddcfp(VF, Sqrt3MinusOne, VF);

    RcpE = __vrefp(FB);

    NegativeOne = __vspltisw(-1);
    NegativeOne = __vcfsx(NegativeOne, 0);

    FA = __vaddfp(Sqrt3MinusOne, NegativeOne);

    GT1 = __vsel(Angle1, Angle2, GT1);

    Scale = __vspltw(OneW, 3);

    Rcp = __vnmsubfp(FB, RcpE, Scale);
    Rcp = __vmaddcfp(RcpE, Rcp, RcpE);
    Scale = __vspltw(OneW, 3);
    Scale = __vnmsubfp(FB, Rcp, Scale);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Refine = __vsel(RcpE, Rcp, Refine);

    FA = __vmulfp(FA, Refine);

    GT2 = __vsel(VF, FA, GT2);

    AbsF = __vandc(GT2, SignMask);
    G = __vmulfp(GT2, GT2);
    AbsF_LT_Epsilon = __vcmpgtfp(Epsilon, AbsF);

    D = __vaddfp(G, Q[3]);
    N = __vmaddcfp(G, N, P[2]);
    D = __vmaddcfp(G, D, Q[2]);
    N = __vmaddcfp(G, N, P[1]);
    D = __vmaddcfp(G, D, Q[1]);
    N = __vmaddcfp(G, N, P[0]);
    D = __vmaddcfp(G, D, Q[0]);
    N = __vmulfp(N, G);

    RcpE = __vrefp(D);
    Scale = __vspltw(OneW, 3);
    Rcp = __vnmsubfp(D, RcpE, Scale);
    Rcp = __vmaddcfp(RcpE, Rcp, RcpE);
    Scale = __vspltw(OneW, 3);
    Scale = __vnmsubfp(D, Rcp, Scale);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Refine = __vsel(RcpE, Rcp, Refine);

    Result = __vmulfp(N, Refine);

    Result = __vmaddcfp(GT2, Result, GT2);

    AbsF_LT_Epsilon = __vsel(Result, GT2, AbsF_LT_Epsilon);

    NegativeResult = __vxor(AbsF_LT_Epsilon, SignMask);
    F_GT_One = __vsel(AbsF_LT_Epsilon, NegativeResult, F_GT_One);

    Result = __vaddfp(F_GT_One, GT1);

    NegativeResult = __vxor(Result, SignMask);
    V_LT_Zero = __vsel(Result, NegativeResult, V_LT_Zero);

    V_GT_MaxV = __vsel(V_LT_Zero, HalfPi, V_GT_MaxV);
    V_LT_MinV = __vsel(V_GT_MaxV, NegativeHalfPi, V_LT_MinV);

    return V_LT_MinV;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVectorATan2
(
    XMVECTOR Y, 
    XMVECTOR X
)
{
#if defined(_NO_INTRINSICS_)

    // Return the inverse tangent of Y / X in the range of -Pi to Pi with the following exceptions:

    //     Y == 0 and X is Negative         -> Pi with the sign of Y
    //     Y == 0 and X is Positive         -> 0 with the sign of Y
    //     Y != 0 and X == 0                -> Pi / 2 with the sign of Y
    //     X == -Infinity and Finite Y > 0  -> Pi with the sign of Y
    //     X == +Infinity and Finite Y > 0  -> 0 with the sign of Y
    //     Y == Infinity and X is Finite    -> Pi / 2 with the sign of Y
    //     Y == Infinity and X == -Infinity -> 3Pi / 4 with the sign of Y
    //     Y == Infinity and X == +Infinity -> Pi / 4 with the sign of Y
    //     TODO: Return Y / X if the result underflows

    XMVECTOR Reciprocal;
    XMVECTOR V;
    XMVECTOR YSign;
    XMVECTOR Pi, PiOverTwo, PiOverFour, ThreePiOverFour;
    XMVECTOR YEqualsZero, XEqualsZero, XIsPositive, YEqualsInfinity, XEqualsInfinity, FiniteYGreaterZero;
    XMVECTOR ATanResultValid;
    XMVECTOR R0, R1, R2, R3, R4, R5, R6, R7;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTOR ATan2Constants = {XM_PI, XM_PIDIV2, XM_PIDIV4, XM_PI * 3.0f / 4.0f};

    Zero = XMVectorZero();
    ATanResultValid = XMVectorTrueInt();

    Pi = XMVectorSplatX(ATan2Constants);
    PiOverTwo = XMVectorSplatY(ATan2Constants);
    PiOverFour = XMVectorSplatZ(ATan2Constants);
    ThreePiOverFour = XMVectorSplatW(ATan2Constants);

    YEqualsZero = XMVectorEqual(Y, Zero);
    XEqualsZero = XMVectorEqual(X, Zero);
    XIsPositive = XMVectorAndInt(X, *(XMVECTOR*)g_XMNegativeZero);
    XIsPositive = XMVectorEqualInt(XIsPositive, Zero);
    YEqualsInfinity = XMVectorIsInfinite(Y);
    XEqualsInfinity = XMVectorIsInfinite(X);
    FiniteYGreaterZero = XMVectorGreater(Y, Zero);
    FiniteYGreaterZero = XMVectorSelect(FiniteYGreaterZero, Zero, YEqualsInfinity);

    YSign = XMVectorAndInt(Y, *(XMVECTOR*)g_XMNegativeZero);
    Pi = XMVectorOrInt(Pi, YSign);
    PiOverTwo = XMVectorOrInt(PiOverTwo, YSign);
    PiOverFour = XMVectorOrInt(PiOverFour, YSign);
    ThreePiOverFour = XMVectorOrInt(ThreePiOverFour, YSign);

    R1 = XMVectorSelect(Pi, YSign, XIsPositive);
    R2 = XMVectorSelect(ATanResultValid, PiOverTwo, XEqualsZero);
    R3 = XMVectorSelect(R2, R1, YEqualsZero);
    R4 = XMVectorSelect(ThreePiOverFour, PiOverFour, XIsPositive);
    R5 = XMVectorSelect(PiOverTwo, R4, XEqualsInfinity);
    R6 = XMVectorSelect(R3, R5, YEqualsInfinity);
    R7 = XMVectorSelect(R6, R1, FiniteYGreaterZero);
    Result = XMVectorSelect(R6, R7, XEqualsInfinity);
    ATanResultValid = XMVectorEqualInt(Result, ATanResultValid);

    Reciprocal = XMVectorReciprocal(X);
    V = XMVectorMultiply(Y, Reciprocal);
    R0 = XMVectorATan(V);

    Result = XMVectorSelect(Result, R0, ATanResultValid);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Reciprocal;
    XMVECTOR V;
    XMVECTOR YSign;
    XMVECTOR Pi, PiOverTwo, PiOverFour, ThreePiOverFour;
    XMVECTOR YEqualsZero, XEqualsZero, XIsPositive, YEqualsInfinity, XEqualsInfinity, FiniteYGreaterZero;
    XMVECTOR ATanResultValid;
    XMVECTOR R0, R1, R2, R3, R4, R5, R6, R7;
    XMVECTOR Rcp, Scale, Refine;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTOR ATan2Constants = {XM_PI, XM_PIDIV2, XM_PIDIV4, XM_PI * 3.0f / 4.0f};

    SignMask = __vspltisw(-1);

    Reciprocal = __vrefp(X);

    YSign = __vand(Y, *(XMVECTOR*)g_XMNegativeZero);
    Zero = __vspltisw(0);
    SignMask = __vslw(SignMask, SignMask);
    XIsPositive = __vand(X, *(XMVECTOR*)g_XMNegativeZero);
    FiniteYGreaterZero = __vcmpgtfp(Y, Zero);
    YEqualsInfinity = __vandc(Y, SignMask);
    XEqualsInfinity = __vandc(X, SignMask);

    Scale = __vnmsubfp(X, Reciprocal, g_XMOne);

    Pi = __vspltw(ATan2Constants, 0);
    PiOverTwo = __vspltw(ATan2Constants, 1);
    ATanResultValid = __vspltisw(-1);
    PiOverFour = __vspltw(ATan2Constants, 2);
    ThreePiOverFour = __vspltw(ATan2Constants, 3);

    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);

    XIsPositive = __vcmpequw(XIsPositive, Zero);
    XEqualsZero = __vcmpeqfp(X, Zero);
    YEqualsZero = __vcmpeqfp(Y, Zero);
    YEqualsInfinity = __vcmpequw(YEqualsInfinity, *(XMVECTOR*)g_XMInfinity);
    XEqualsInfinity = __vcmpequw(XEqualsInfinity, *(XMVECTOR*)g_XMInfinity);

    Scale = __vnmsubfp(X, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);

    Pi = __vor(Pi, YSign);
    PiOverTwo = __vor(PiOverTwo, YSign);
    PiOverFour = __vor(PiOverFour, YSign);
    ThreePiOverFour = __vor(ThreePiOverFour, YSign);

    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Reciprocal = __vsel(Reciprocal, Rcp, Refine);

    FiniteYGreaterZero = __vsel(FiniteYGreaterZero, Zero, YEqualsInfinity);

    R1 = __vsel(Pi, YSign, XIsPositive);
    R2 = __vsel(ATanResultValid, PiOverTwo, XEqualsZero);
    R4 = __vsel(ThreePiOverFour, PiOverFour, XIsPositive);
    R3 = __vsel(R2, R1, YEqualsZero);
    R5 = __vsel(PiOverTwo, R4, XEqualsInfinity);
    R6 = __vsel(R3, R5, YEqualsInfinity);
    R7 = __vsel(R6, R1, FiniteYGreaterZero);
    Result = __vsel(R6, R7, XEqualsInfinity);
    ATanResultValid = __vcmpequw(Result, ATanResultValid);

    V = __vmaddfp(Y, Reciprocal, *(XMVECTOR*)g_XMNegativeZero);
    R0 = XMVectorATan(V);

    Result = __vsel(Result, R0, ATanResultValid);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Reciprocal;
    XMVECTOR V;
    XMVECTOR YSign;
    XMVECTOR Pi, PiOverTwo, PiOverFour, ThreePiOverFour;
    XMVECTOR XShifted, YShifted, InfinityShifted;
    XMVECTOR FiniteYGreaterZero;
    XMVECTOR ATanResultValid;
    XMVECTOR R0, R1, R2, R3, R4, R5, R6, R7, R8;
    XMVECTOR Rcp, Scale, Refine;
    XMVECTOR SignMask;
    XMVECTOR Zero, One, OneInt, TrueInt, Shift;
    XMVECTOR Result;
    static CONST XMVECTOR ATan2Constants = {XM_PI, XM_PIDIV2, XM_PIDIV4, XM_PI * 3.0f / 4.0f};

    XMDUMMY_INITIALIZE_VECTOR(One);

    TrueInt = __vspltisw(-1);
    One = __vupkd3d(One, VPACK_NORMSHORT2);
    OneInt = __vspltisw(1);
    Shift = __vspltisw(-8); // Shift 24
    SignMask = __vslw(TrueInt, TrueInt);

    Reciprocal = __vrefp(X);
    Scale = __vspltw(One, 3);

    YSign = __vand(Y, SignMask);
    Zero = __vspltisw(0);
    R1 = __vand(X, SignMask);
    R4 = __vand(X, SignMask);
    FiniteYGreaterZero = __vcmpgtfp(Y, Zero);
    InfinityShifted = __vslw(TrueInt, Shift);
    YShifted = __vslw(Y, OneInt);
    XShifted = __vslw(X, OneInt);

    Rcp = __vnmsubfp(X, Reciprocal, Scale);

    Pi = __vspltw(ATan2Constants, 0);
    PiOverTwo = __vspltw(ATan2Constants, 1);
    ATanResultValid = __vspltisw(-1);
    PiOverFour = __vspltw(ATan2Constants, 2);
    ThreePiOverFour = __vspltw(ATan2Constants, 3);

    Rcp = __vmaddcfp(Reciprocal, Rcp, Reciprocal);

    Scale = __vspltw(One, 3);
    R1 = __vcmpequw(R1, Zero); // XIsPositive
    R2 = __vcmpeqfp(X, Zero); // XEqualsZero
    R3 = __vcmpeqfp(Y, Zero); // YEqualsZero
    R6 = __vcmpequw(YShifted, InfinityShifted); // YEqualsInfinity
    R7 = __vcmpequw(YShifted, InfinityShifted);
    R5 = __vcmpequw(XShifted, InfinityShifted); // XEqualsInfinity
    R8 = __vcmpequw(XShifted, InfinityShifted);

    Scale = __vnmsubfp(X, Rcp, Scale);
    Refine = __vcmpeqfp(Rcp, Rcp);

    Pi = __vor(Pi, YSign);
    PiOverTwo = __vor(PiOverTwo, YSign);
    PiOverFour = __vor(PiOverFour, YSign);
    ThreePiOverFour = __vor(ThreePiOverFour, YSign);

    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Refine = __vsel(Reciprocal, Rcp, Refine);

    R7 = __vsel(FiniteYGreaterZero, Zero, R7);

    R1 = __vsel(Pi, YSign, R1);
    R2 = __vsel(ATanResultValid, PiOverTwo, R2);
    R4 = __vsel(ThreePiOverFour, PiOverFour, R4);
    R3 = __vsel(R2, R1, R3);
    R5 = __vsel(PiOverTwo, R4, R5);
    R6 = __vsel(R3, R5, R6);
    R7 = __vsel(R6, R1, R7);
    R8 = __vsel(R6, R7, R8);
    ATanResultValid = __vcmpequw(R8, ATanResultValid);

    V = __vmulfp(Y, Refine);
    R0 = XMVectorATan(V);

    Result = __vsel(R8, R0, ATanResultValid);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSinEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V2, V3, V5, V7;
    XMVECTOR S1, S2, S3;
    XMVECTOR Result;

    // sin(V) ~= V - V^3 / 3! + V^5 / 5! - V^7 / 7! (for -PI <= V < PI)
    V2 = XMVectorMultiply(V, V);
    V3 = XMVectorMultiply(V2, V);
    V5 = XMVectorMultiply(V3, V2);
    V7 = XMVectorMultiply(V5, V2);

    S1 = XMVectorSplatY(g_XMSinEstCoefficients);
    S2 = XMVectorSplatZ(g_XMSinEstCoefficients);
    S3 = XMVectorSplatW(g_XMSinEstCoefficients);

    Result = XMVectorMultiplyAdd(S1, V3, V);
    Result = XMVectorMultiplyAdd(S2, V5, Result);
    Result = XMVectorMultiplyAdd(S3, V7, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V2, V3, V5, V7;
    XMVECTOR S1, S2, S3;
    XMVECTOR Result;

    V2 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    S1 = __vspltw(g_XMSinEstCoefficients, 1);
    S2 = __vspltw(g_XMSinEstCoefficients, 2);
    S3 = __vspltw(g_XMSinEstCoefficients, 3);
    V3 = __vmaddfp(V2, V, *(XMVECTOR*)g_XMNegativeZero);
    V5 = __vmaddfp(V3, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S1, V3, V);
    V7 = __vmaddfp(V5, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(S2, V5, Result);
    Result = __vmaddfp(S3, V7, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V2, V5, V7;
    XMVECTOR S1, S2, S3;
    XMVECTOR Result;

    V2 = __vmulfp(V, V);
    S1 = __vspltw(g_XMSinEstCoefficients, 1);
    S2 = __vspltw(g_XMSinEstCoefficients, 2);
    S3 = __vspltw(g_XMSinEstCoefficients, 3);
    Result = __vmulfp(V2, V);
    V5 = __vmulfp(Result, V2);
    Result = __vmaddcfp(S1, Result, V);
    V7 = __vmulfp(V5, V2);
    Result = __vmaddfp(S2, V5, Result);
    Result = __vmaddfp(S3, V7, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorCosEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V2, V4, V6;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR Result;

    V2 = XMVectorMultiply(V, V);
    V4 = XMVectorMultiply(V2, V2);
    V6 = XMVectorMultiply(V4, V2);

    C0 = XMVectorSplatX(g_XMCosEstCoefficients);
    C1 = XMVectorSplatY(g_XMCosEstCoefficients);
    C2 = XMVectorSplatZ(g_XMCosEstCoefficients);
    C3 = XMVectorSplatW(g_XMCosEstCoefficients);

    Result = XMVectorMultiplyAdd(C1, V2, C0);
    Result = XMVectorMultiplyAdd(C2, V4, Result);
    Result = XMVectorMultiplyAdd(C3, V6, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V2, V4, V6;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR Result;

    V2 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    C0 = __vspltw(g_XMCosEstCoefficients, 0);
    C1 = __vspltw(g_XMCosEstCoefficients, 1);
    C2 = __vspltw(g_XMCosEstCoefficients, 2);
    C3 = __vspltw(g_XMCosEstCoefficients, 3);
    V4 = __vmaddfp(V2, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C1, V2, C0);
    V6 = __vmaddfp(V4, V2, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(C2, V4, Result);
    Result = __vmaddfp(C3, V6, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V2, V4, V6;
    XMVECTOR C1, C2, C3;
    XMVECTOR Result;

    V2 = __vmulfp(V, V);
    Result = __vspltw(g_XMCosEstCoefficients, 0);
    C1 = __vspltw(g_XMCosEstCoefficients, 1);
    C2 = __vspltw(g_XMCosEstCoefficients, 2);
    C3 = __vspltw(g_XMCosEstCoefficients, 3);
    V4 = __vmulfp(V2, V2);
    Result = __vmaddfp(C1, V2, Result);
    V6 = __vmulfp(V4, V2);
    Result = __vmaddfp(C2, V4, Result);
    Result = __vmaddfp(C3, V6, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMVectorSinCosEst
(
    XMVECTOR* pSin, 
    XMVECTOR* pCos, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V2, V3, V4, V5, V6, V7;
    XMVECTOR S1, S2, S3;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR Sin, Cos;

    XMASSERT(pSin);
    XMASSERT(pCos);

    // sin(V) ~= V - V^3 / 3! + V^5 / 5! - V^7 / 7! (for -PI <= V < PI)
    // cos(V) ~= 1 - V^2 / 2! + V^4 / 4! - V^6 / 6! (for -PI <= V < PI)
    V2 = XMVectorMultiply(V, V);
    V3 = XMVectorMultiply(V2, V);
    V4 = XMVectorMultiply(V2, V2);
    V5 = XMVectorMultiply(V3, V2);
    V6 = XMVectorMultiply(V3, V3);
    V7 = XMVectorMultiply(V4, V3);

    S1 = XMVectorSplatY(g_XMSinEstCoefficients);
    S2 = XMVectorSplatZ(g_XMSinEstCoefficients);
    S3 = XMVectorSplatW(g_XMSinEstCoefficients);

    C0 = XMVectorSplatX(g_XMCosEstCoefficients);
    C1 = XMVectorSplatY(g_XMCosEstCoefficients);
    C2 = XMVectorSplatZ(g_XMCosEstCoefficients);
    C3 = XMVectorSplatW(g_XMCosEstCoefficients);

    Sin = XMVectorMultiplyAdd(S1, V3, V);
    Sin = XMVectorMultiplyAdd(S2, V5, Sin);
    Sin = XMVectorMultiplyAdd(S3, V7, Sin);

    Cos = XMVectorMultiplyAdd(C1, V2, C0);
    Cos = XMVectorMultiplyAdd(C2, V4, Cos);
    Cos = XMVectorMultiplyAdd(C3, V6, Cos);

    *pSin = Sin;
    *pCos = Cos;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V2, V3, V4, V5, V6, V7;
    XMVECTOR S1, S2, S3;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR Sin, Cos;

    XMASSERT(pSin);
    XMASSERT(pCos);

    V2 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    C0 = __vspltw(g_XMCosEstCoefficients, 0);
    C1 = __vspltw(g_XMCosEstCoefficients, 1);
    S1 = __vspltw(g_XMSinEstCoefficients, 1);
    C2 = __vspltw(g_XMCosEstCoefficients, 2);
    S2 = __vspltw(g_XMSinEstCoefficients, 2);
    C3 = __vspltw(g_XMCosEstCoefficients, 3);
    S3 = __vspltw(g_XMSinEstCoefficients, 3);
    V3 = __vmaddfp(V2, V, *(XMVECTOR*)g_XMNegativeZero);
    V4 = __vmaddfp(V2, V2, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C1, V2, C0);
    V5 = __vmaddfp(V3, V2, *(XMVECTOR*)g_XMNegativeZero);
    Sin = __vmaddfp(S1, V3, V);
    V6 = __vmaddfp(V3, V3, *(XMVECTOR*)g_XMNegativeZero);
    V7 = __vmaddfp(V4, V3, *(XMVECTOR*)g_XMNegativeZero);
    Cos = __vmaddfp(C2, V4, Cos);
    Sin = __vmaddfp(S2, V5, Sin);
    Cos = __vmaddfp(C3, V6, Cos);
    Sin = __vmaddfp(S3, V7, Sin);

    *pCos = Cos;
    *pSin = Sin;

#else // _VMX128_INTRINSICS_

    XMVECTOR V2, V3, V4, V5, V6, V7;
    XMVECTOR S2, S3;
    XMVECTOR C0, C2, C3;
    XMVECTOR Sin, Cos;

    XMASSERT(pSin);
    XMASSERT(pCos);

    V2 = __vmulfp(V, V);
    C0 = __vspltw(g_XMCosEstCoefficients, 0);
    Cos = __vspltw(g_XMCosEstCoefficients, 1);
    Sin = __vspltw(g_XMSinEstCoefficients, 1);
    C2 = __vspltw(g_XMCosEstCoefficients, 2);
    S2 = __vspltw(g_XMSinEstCoefficients, 2);
    C3 = __vspltw(g_XMCosEstCoefficients, 3);
    S3 = __vspltw(g_XMSinEstCoefficients, 3);
    V3 = __vmulfp(V2, V);
    V4 = __vmulfp(V2, V2);
    Cos = __vmaddcfp(V2, Cos, C0);
    V5 = __vmulfp(V3, V2);
    Sin = __vmaddcfp(V3, Sin, V);
    V6 = __vmulfp(V3, V3);
    V7 = __vmulfp(V4, V3);
    Cos = __vmaddfp(C2, V4, Cos);
    Sin = __vmaddfp(S2, V5, Sin);
    Cos = __vmaddfp(C3, V6, Cos);
    Sin = __vmaddfp(S3, V7, Sin);

    *pCos = Cos;
    *pSin = Sin;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorTanEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1, V2, V1T0, V1T1, V2T2;
    XMVECTOR T0, T1, T2;
    XMVECTOR N, D;
    XMVECTOR OneOverPi;
    XMVECTOR Result;

    OneOverPi = XMVectorSplatW(g_XMTanEstCoefficients);

    V1 = XMVectorMultiply(V, OneOverPi);
    V1 = XMVectorRound(V1);

    V1 = XMVectorNegativeMultiplySubtract(g_XMPi, V1, V);

    T0 = XMVectorSplatX(g_XMTanEstCoefficients);
    T1 = XMVectorSplatY(g_XMTanEstCoefficients);
    T2 = XMVectorSplatZ(g_XMTanEstCoefficients);

    V2T2 = XMVectorNegativeMultiplySubtract(V1, V1, T2);
    V2 = XMVectorMultiply(V1, V1);
    V1T0 = XMVectorMultiply(V1, T0);
    V1T1 = XMVectorMultiply(V1, T1);

    D = XMVectorReciprocalEst(V2T2);
    N = XMVectorMultiplyAdd(V2, V1T1, V1T0);

    Result = XMVectorMultiply(N, D);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1, V2, V1T0, V1T1, V2T2;
    XMVECTOR T0, T1, T2;
    XMVECTOR N, D;
    XMVECTOR OneOverPi;
    XMVECTOR Result;

    OneOverPi = __vspltw(g_XMTanEstCoefficients, 3);

    V1 = __vmaddfp(V, OneOverPi, *(XMVECTOR*)g_XMNegativeZero);
    V1 = __vrfin(V1);

    V1 = __vnmsubfp(g_XMPi, V1, V);

    T0 = __vspltw(g_XMTanEstCoefficients, 0);
    T1 = __vspltw(g_XMTanEstCoefficients, 1);
    T2 = __vspltw(g_XMTanEstCoefficients, 2);

    V2T2 = __vnmsubfp(V1, V1, T2);
    V2 = __vmaddfp(V1, V1, *(XMVECTOR*)g_XMNegativeZero);
    V1T0 = __vmaddfp(V1, T0, *(XMVECTOR*)g_XMNegativeZero);
    V1T1 = __vmaddfp(V1, T1, *(XMVECTOR*)g_XMNegativeZero);

    D = __vrefp(V2T2);
    N = __vmaddfp(V2, V1T1, V1T0);

    Result = __vmaddfp(N, D, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V1, V2, V1D, V1T0, V1T1, V2T2;
    XMVECTOR T0, T1, T2;
    XMVECTOR N, D;
    XMVECTOR OneOverPi, Pi;
    XMVECTOR Result;

    OneOverPi = __vspltw(g_XMTanEstCoefficients, 3);

    V1 = __vmulfp(V, OneOverPi);
    Pi = __vspltw(g_XMPiConstants0, 0);
    V1 = __vrfin(V1);
    V1D = V;

    V1D = __vnmsubfp(Pi, V1, V1D);

    T0 = __vspltw(g_XMTanEstCoefficients, 0);
    T1 = __vspltw(g_XMTanEstCoefficients, 1);
    T2 = __vspltw(g_XMTanEstCoefficients, 2);

    V2T2 = __vnmsubfp(V1D, V1D, T2);
    V2 = __vmulfp(V1D, V1D);
    V1T0 = __vmulfp(V1D, T0);
    V1T1 = __vmulfp(V1D, T1);

    D = __vrefp(V2T2);
    N = __vmaddfp(V2, V1T1, V1T0);

    Result = __vmulfp(N, D);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorSinHEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1, V2;
    XMVECTOR E1, E2;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f}; // 1.0f / ln(2.0f)

    V1 = XMVectorMultiplyAdd(V, Scale, g_XMNegativeOne);
    V2 = XMVectorNegativeMultiplySubtract(V, Scale, g_XMNegativeOne);

    E1 = XMVectorExpEst(V1);
    E2 = XMVectorExpEst(V2);

    Result = XMVectorSubtract(E1, E2);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1, V2;
    XMVECTOR E1, E2;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f};

    V1 = __vmaddfp(V, Scale, g_XMNegativeOne);
    V2 = __vnmsubfp(V, Scale, g_XMNegativeOne);

    E1 = __vexptefp(V1);
    E2 = __vexptefp(V2);

    Result = __vsubfp(E1, E2);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V1, V2;
    XMVECTOR E1, E2;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f};

    V2 = __vspltisw(-1);
    V1 = __vcfsx(V2, 0);
    V2 = __vcfsx(V2, 0);

    V1 = __vmaddfp(V, Scale, V1);
    V2 = __vnmsubfp(V, Scale, V2);

    E1 = __vexptefp(V1);
    E2 = __vexptefp(V2);

    Result = __vsubfp(E1, E2);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorCosHEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V1, V2;
    XMVECTOR E1, E2;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f}; // 1.0f / ln(2.0f)

    V1 = XMVectorMultiplyAdd(V, Scale, g_XMNegativeOne);
    V2 = XMVectorNegativeMultiplySubtract(V, Scale, g_XMNegativeOne);

    E1 = XMVectorExpEst(V1);
    E2 = XMVectorExpEst(V2);

    Result = XMVectorAdd(E1, E2);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1, V2;
    XMVECTOR E1, E2;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f};

    V1 = __vmaddfp(V, Scale, g_XMNegativeOne);
    V2 = __vnmsubfp(V, Scale, g_XMNegativeOne);

    E1 = __vexptefp(V1);
    E2 = __vexptefp(V2);

    Result = __vaddfp(E1, E2);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V1, V2;
    XMVECTOR E1, E2;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f};

    V2 = __vspltisw(-1);
    V1 = __vcfsx(V2, 0);
    V2 = __vcfsx(V2, 0);

    V1 = __vmaddfp(V, Scale, V1);
    V2 = __vnmsubfp(V, Scale, V2);

    E1 = __vexptefp(V1);
    E2 = __vexptefp(V2);

    Result = __vaddfp(E1, E2);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorTanHEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR E;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f}; // 2.0f / ln(2.0f)

    E = XMVectorMultiply(V, Scale);
    E = XMVectorExpEst(E);
    E = XMVectorMultiplyAdd(E, g_XMOneHalf, g_XMOneHalf);
    E = XMVectorReciprocalEst(E);

    Result = XMVectorSubtract(g_XMOne, E);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR E;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f};

    E = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
    E = __vexptefp(E);
    E = __vmaddfp(E, g_XMOneHalf, g_XMOneHalf);
    E = __vrefp(E);

    Result = __vsubfp(g_XMOne, E);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR E;
    XMVECTOR OneHalf, One;
    XMVECTOR Result;
    static CONST XMVECTOR Scale = {2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f};

    E = __vmulfp(V, Scale);
    One = __vspltisw(1);
    E = __vexptefp(E);
    OneHalf = __vcfsx(One, 1);
    E = __vmaddfp(E, OneHalf, OneHalf);
    One = __vcfsx(One, 0);
    E = __vrefp(E);

    Result = __vsubfp(One, E);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorASinEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR AbsV, V2, VD, VC0, V2C3;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR D, Rsq, SqrtD;
    XMVECTOR OnePlusEps;
    XMVECTOR Result;

    AbsV = XMVectorAbs(V);

    OnePlusEps = XMVectorSplatX(g_XMASinEstConstants);

    C0 = XMVectorSplatX(g_XMASinEstCoefficients);
    C1 = XMVectorSplatY(g_XMASinEstCoefficients);
    C2 = XMVectorSplatZ(g_XMASinEstCoefficients);
    C3 = XMVectorSplatW(g_XMASinEstCoefficients);

    D = XMVectorSubtract(OnePlusEps, AbsV);

    Rsq = XMVectorReciprocalSqrtEst(D);
    SqrtD = XMVectorMultiply(D, Rsq);

    V2 = XMVectorMultiply(V, AbsV);
    V2C3 = XMVectorMultiply(V2, C3);
    VD = XMVectorMultiply(D, AbsV);
    VC0 = XMVectorMultiply(V, C0);

    Result = XMVectorMultiply(V, C1);
    Result = XMVectorMultiplyAdd(V2, C2, Result);
    Result = XMVectorMultiplyAdd(V2C3, VD, Result);
    Result = XMVectorMultiplyAdd(VC0, SqrtD, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR AbsV, V2, VD, VC0, V2C3;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR D, Rsq, SqrtD;
    XMVECTOR OnePlusEps;
    XMVECTOR Result;

    AbsV = XMVectorAbs(V);
    OnePlusEps = XMVectorSplatX(g_XMASinEstConstants);
    C1 = XMVectorSplatY(g_XMASinEstCoefficients);
    C3 = XMVectorSplatW(g_XMASinEstCoefficients);

    D = XMVectorSubtract(OnePlusEps, AbsV);

    V2 = XMVectorMultiply(V, AbsV);
    Result = XMVectorMultiply(V, C1);
    C2 = XMVectorSplatZ(g_XMASinEstCoefficients);

    C0 = XMVectorSplatX(g_XMASinEstCoefficients);

    VD = XMVectorMultiply(D, AbsV);
    V2C3 = XMVectorMultiply(V2, C3);

    Rsq = XMVectorReciprocalSqrtEst(D);
    Result = XMVectorMultiplyAdd(V2, C2, Result);
    VC0 = XMVectorMultiply(V, C0);
    SqrtD = XMVectorMultiply(D, Rsq);
    Result = XMVectorMultiplyAdd(V2C3, VD, Result);
    Result = XMVectorMultiplyAdd(VC0, SqrtD, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR AbsV, V2, VD, VC0, V2C3;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR D, Rsq, SqrtD;
    XMVECTOR SignMask;
    XMVECTOR OnePlusEps;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    AbsV = __vandc(V, SignMask);

    OnePlusEps = __vspltw(g_XMASinEstConstants, 0);
    C1 = __vspltw(g_XMASinEstCoefficients, 1);
    C3 = __vspltw(g_XMASinEstCoefficients, 3);

    D = __vsubfp(OnePlusEps, AbsV);

    V2 = __vmulfp(V, AbsV);
    Result = __vmulfp(V, C1);
    C2 = __vspltw(g_XMASinEstCoefficients, 2);

    C0 = __vspltw(g_XMASinEstCoefficients, 0);

    VD = __vmulfp(D, AbsV);
    V2C3 = __vmulfp(V2, C3);

    Rsq = __vrsqrtefp(D);
    Result = __vmaddfp(V2, C2, Result);
    VC0 = __vmulfp(V, C0);
    SqrtD = __vmulfp(D, Rsq);
    Result = __vmaddfp(V2C3, VD, Result);
    Result = __vmaddfp(VC0, SqrtD, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorACosEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR AbsV, V2, VD, VC0, V2C3;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR D, Rsq, SqrtD;
    XMVECTOR OnePlusEps, HalfPi;
    XMVECTOR Result;

    // acos(V) = PI / 2 - asin(V)

    AbsV = XMVectorAbs(V);

    OnePlusEps = XMVectorSplatX(g_XMASinEstConstants);
    HalfPi = XMVectorSplatY(g_XMASinEstConstants);

    C0 = XMVectorSplatX(g_XMASinEstCoefficients);
    C1 = XMVectorSplatY(g_XMASinEstCoefficients);
    C2 = XMVectorSplatZ(g_XMASinEstCoefficients);
    C3 = XMVectorSplatW(g_XMASinEstCoefficients);

    D = XMVectorSubtract(OnePlusEps, AbsV);

    Rsq = XMVectorReciprocalSqrtEst(D);
    SqrtD = XMVectorMultiply(D, Rsq);

    V2 = XMVectorMultiply(V, AbsV);
    V2C3 = XMVectorMultiply(V2, C3);
    VD = XMVectorMultiply(D, AbsV);
    VC0 = XMVectorMultiply(V, C0);

    Result = XMVectorMultiply(V, C1);
    Result = XMVectorMultiplyAdd(V2, C2, Result);
    Result = XMVectorMultiplyAdd(V2C3, VD, Result);
    Result = XMVectorMultiplyAdd(VC0, SqrtD, Result);
    Result = XMVectorSubtract(HalfPi, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR AbsV, V2, VD, VC0, V2C3;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR D, Rsq, SqrtD;
    XMVECTOR OnePlusEps, HalfPi;
    XMVECTOR Result;

    AbsV = XMVectorAbs(V);
    OnePlusEps = XMVectorSplatX(g_XMASinEstConstants);
    C1 = XMVectorSplatY(g_XMASinEstCoefficients);
    C3 = XMVectorSplatW(g_XMASinEstCoefficients);

    D = XMVectorSubtract(OnePlusEps, AbsV);

    V2 = XMVectorMultiply(V, AbsV);
    Result = XMVectorMultiply(V, C1);
    C2 = XMVectorSplatZ(g_XMASinEstCoefficients);

    C0 = XMVectorSplatX(g_XMASinEstCoefficients);

    VD = XMVectorMultiply(D, AbsV);
    V2C3 = XMVectorMultiply(V2, C3);

    Rsq = XMVectorReciprocalSqrtEst(D);
    Result = XMVectorMultiplyAdd(V2, C2, Result);
    VC0 = XMVectorMultiply(V, C0);
    SqrtD = XMVectorMultiply(D, Rsq);
    Result = XMVectorMultiplyAdd(V2C3, VD, Result);
    HalfPi = XMVectorSplatY(g_XMASinEstConstants);
    Result = XMVectorMultiplyAdd(VC0, SqrtD, Result);
    Result = XMVectorSubtract(HalfPi, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR AbsV, V2, VD, VC0, V2C3;
    XMVECTOR C0, C1, C2, C3;
    XMVECTOR D, Rsq, SqrtD;
    XMVECTOR OnePlusEps, HalfPi, SignMask;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    AbsV = __vandc(V, SignMask);

    OnePlusEps = __vspltw(g_XMASinEstConstants, 0);
    C1 = __vspltw(g_XMASinEstCoefficients, 1);
    C3 = __vspltw(g_XMASinEstCoefficients, 3);

    D = __vsubfp(OnePlusEps, AbsV);

    V2 = __vmulfp(V, AbsV);
    Result = __vmulfp(V, C1);
    C2 = __vspltw(g_XMASinEstCoefficients, 2);

    C0 = __vspltw(g_XMASinEstCoefficients, 0);

    VD = __vmulfp(D, AbsV);
    V2C3 = __vmulfp(V2, C3);

    Rsq = __vrsqrtefp(D);
    Result = __vmaddfp(V2, C2, Result);
    VC0 = __vmulfp(V, C0);
    SqrtD = __vmulfp(D, Rsq);
    Result = __vmaddfp(V2C3, VD, Result);
    HalfPi = __vspltw(g_XMASinEstConstants, 1);
    Result = __vmaddfp(VC0, SqrtD, Result);
    Result = __vsubfp(HalfPi, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorATanEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR AbsV, V2S2, N, D;
    XMVECTOR S0, S1, S2;
    XMVECTOR HalfPi;
    XMVECTOR Result;

    S0 = XMVectorSplatX(g_XMATanEstCoefficients);
    S1 = XMVectorSplatY(g_XMATanEstCoefficients);
    S2 = XMVectorSplatZ(g_XMATanEstCoefficients);
    HalfPi = XMVectorSplatW(g_XMATanEstCoefficients);

    AbsV = XMVectorAbs(V);

    V2S2 = XMVectorMultiplyAdd(V, V, S2);
    N = XMVectorMultiplyAdd(AbsV, HalfPi, S0);
    D = XMVectorMultiplyAdd(AbsV, S1, V2S2);
    N = XMVectorMultiply(N, V);
    D = XMVectorReciprocalEst(D);

    Result = XMVectorMultiply(N, D);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR AbsV, V2S2, N, D;
    XMVECTOR S0, S1, S2;
    XMVECTOR SignMask;
    XMVECTOR HalfPi;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    S2 = __vspltw(g_XMATanEstCoefficients, 2);
    AbsV = __vandc(V, SignMask);
    HalfPi = __vspltw(g_XMATanEstCoefficients, 3);
    S0 = __vspltw(g_XMATanEstCoefficients, 0);
    S1 = __vspltw(g_XMATanEstCoefficients, 1);

    V2S2 = __vmaddfp(V, V, S2);
    N = __vmaddfp(AbsV, HalfPi, S0);
    D = __vmaddfp(AbsV, S1, V2S2);
    N = __vmaddfp(N, V, *(XMVECTOR*)g_XMNegativeZero);
    D = __vrefp(D);

    Result = __vmaddfp(N, D, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR AbsV, V2S2, N, D;
    XMVECTOR SignMask;
    XMVECTOR HalfPi;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    V2S2 = __vspltw(g_XMATanEstCoefficients, 2);
    AbsV = __vandc(V, SignMask);
    HalfPi = __vspltw(g_XMATanEstCoefficients, 3);
    N = __vspltw(g_XMATanEstCoefficients, 0);
    D = __vspltw(g_XMATanEstCoefficients, 1);

    V2S2 = __vmaddfp(V, V, V2S2);
    N = __vmaddfp(AbsV, HalfPi, N);
    D = __vmaddcfp(AbsV, D, V2S2);
    N = __vmulfp(N, V);
    D = __vrefp(D);

    Result = __vmulfp(N, D);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorATan2Est
(
    XMVECTOR Y, 
    XMVECTOR X
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Reciprocal;
    XMVECTOR V;
    XMVECTOR YSign;
    XMVECTOR Pi, PiOverTwo, PiOverFour, ThreePiOverFour;
    XMVECTOR YEqualsZero, XEqualsZero, XIsPositive, YEqualsInfinity, XEqualsInfinity, FiniteYGreaterZero;
    XMVECTOR ATanResultValid;
    XMVECTOR R0, R1, R2, R3, R4, R5, R6, R7;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTOR ATan2Constants = {XM_PI, XM_PIDIV2, XM_PIDIV4, XM_PI * 3.0f / 4.0f};

    Zero = XMVectorZero();
    ATanResultValid = XMVectorTrueInt();

    Pi = XMVectorSplatX(ATan2Constants);
    PiOverTwo = XMVectorSplatY(ATan2Constants);
    PiOverFour = XMVectorSplatZ(ATan2Constants);
    ThreePiOverFour = XMVectorSplatW(ATan2Constants);

    YEqualsZero = XMVectorEqual(Y, Zero);
    XEqualsZero = XMVectorEqual(X, Zero);
    XIsPositive = XMVectorAndInt(X, *(XMVECTOR*)g_XMNegativeZero);
    XIsPositive = XMVectorEqualInt(XIsPositive, Zero);
    YEqualsInfinity = XMVectorIsInfinite(Y);
    XEqualsInfinity = XMVectorIsInfinite(X);
    FiniteYGreaterZero = XMVectorGreater(Y, Zero);
    FiniteYGreaterZero = XMVectorSelect(FiniteYGreaterZero, Zero, YEqualsInfinity);

    YSign = XMVectorAndInt(Y, *(XMVECTOR*)g_XMNegativeZero);
    Pi = XMVectorOrInt(Pi, YSign);
    PiOverTwo = XMVectorOrInt(PiOverTwo, YSign);
    PiOverFour = XMVectorOrInt(PiOverFour, YSign);
    ThreePiOverFour = XMVectorOrInt(ThreePiOverFour, YSign);

    R1 = XMVectorSelect(Pi, YSign, XIsPositive);
    R2 = XMVectorSelect(ATanResultValid, PiOverTwo, XEqualsZero);
    R3 = XMVectorSelect(R2, R1, YEqualsZero);
    R4 = XMVectorSelect(ThreePiOverFour, PiOverFour, XIsPositive);
    R5 = XMVectorSelect(PiOverTwo, R4, XEqualsInfinity);
    R6 = XMVectorSelect(R3, R5, YEqualsInfinity);
    R7 = XMVectorSelect(R6, R1, FiniteYGreaterZero);
    Result = XMVectorSelect(R6, R7, XEqualsInfinity);
    ATanResultValid = XMVectorEqualInt(Result, ATanResultValid);

    Reciprocal = XMVectorReciprocalEst(X);
    V = XMVectorMultiply(Y, Reciprocal);
    R0 = XMVectorATanEst(V);

    Result = XMVectorSelect(Result, R0, ATanResultValid);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Reciprocal;
    XMVECTOR V;
    XMVECTOR YSign;
    XMVECTOR Pi, PiOverTwo, PiOverFour, ThreePiOverFour;
    XMVECTOR YEqualsZero, XEqualsZero, XIsPositive, YEqualsInfinity, XEqualsInfinity, FiniteYGreaterZero;
    XMVECTOR ATanResultValid;
    XMVECTOR R0, R1, R2, R3, R4, R5, R6, R7;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTOR ATan2Constants = {XM_PI, XM_PIDIV2, XM_PIDIV4, XM_PI * 3.0f / 4.0f};

    SignMask = __vspltisw(-1);

    Reciprocal = __vrefp(X);

    YSign = __vand(Y, *(XMVECTOR*)g_XMNegativeZero);
    Zero = __vspltisw(0);
    SignMask = __vslw(SignMask, SignMask);
    XIsPositive = __vand(X, *(XMVECTOR*)g_XMNegativeZero);
    FiniteYGreaterZero = __vcmpgtfp(Y, Zero);
    YEqualsInfinity = __vandc(Y, SignMask);
    XEqualsInfinity = __vandc(X, SignMask);

    Pi = __vspltw(ATan2Constants, 0);
    PiOverTwo = __vspltw(ATan2Constants, 1);
    ATanResultValid = __vspltisw(-1);
    PiOverFour = __vspltw(ATan2Constants, 2);
    ThreePiOverFour = __vspltw(ATan2Constants, 3);

    XIsPositive = __vcmpequw(XIsPositive, Zero);
    XEqualsZero = __vcmpeqfp(X, Zero);
    YEqualsZero = __vcmpeqfp(Y, Zero);
    YEqualsInfinity = __vcmpequw(YEqualsInfinity, *(XMVECTOR*)g_XMInfinity);
    XEqualsInfinity = __vcmpequw(XEqualsInfinity, *(XMVECTOR*)g_XMInfinity);

    V = __vmaddfp(Y, Reciprocal, *(XMVECTOR*)g_XMNegativeZero);

    Pi = __vor(Pi, YSign);
    PiOverTwo = __vor(PiOverTwo, YSign);
    PiOverFour = __vor(PiOverFour, YSign);
    ThreePiOverFour = __vor(ThreePiOverFour, YSign);

    FiniteYGreaterZero = __vsel(FiniteYGreaterZero, Zero, YEqualsInfinity);

    R1 = __vsel(Pi, YSign, XIsPositive);
    R2 = __vsel(ATanResultValid, PiOverTwo, XEqualsZero);
    R4 = __vsel(ThreePiOverFour, PiOverFour, XIsPositive);
    R3 = __vsel(R2, R1, YEqualsZero);
    R5 = __vsel(PiOverTwo, R4, XEqualsInfinity);
    R6 = __vsel(R3, R5, YEqualsInfinity);
    R7 = __vsel(R6, R1, FiniteYGreaterZero);
    Result = __vsel(R6, R7, XEqualsInfinity);
    ATanResultValid = __vcmpequw(Result, ATanResultValid);

    R0 = XMVectorATanEst(V);

    Result = __vsel(Result, R0, ATanResultValid);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Reciprocal;
    XMVECTOR V;
    XMVECTOR YSign;
    XMVECTOR OneInt;
    XMVECTOR Shift, InfinityShifted;
    XMVECTOR Pi, PiOverTwo, PiOverFour, ThreePiOverFour;
    XMVECTOR FiniteYGreaterZero;
    XMVECTOR AbsV, V2S2, N, D;
    XMVECTOR HalfPi;
    XMVECTOR FinalResult;
    XMVECTOR R0, R1, R2, R3, R4, R5, R6, R7, R8;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    static CONST XMVECTOR ATan2Constants = {XM_PI, XM_PIDIV2, XM_PIDIV4, XM_PI * 3.0f / 4.0f};

    SignMask = __vspltisw(-1);

    Reciprocal = __vrefp(X);

    Pi = __vspltw(ATan2Constants, 0);
    PiOverTwo = __vspltw(ATan2Constants, 1);
    PiOverFour = __vspltw(ATan2Constants, 2);
    ThreePiOverFour = __vspltw(ATan2Constants, 3);

    SignMask = __vslw(SignMask, SignMask);

    OneInt = __vspltisw(1);
    FinalResult = __vspltisw(-1);
    Shift = __vspltisw(-8); // Shift 24
    Zero = __vspltisw(0);

    R1 = __vand(X, SignMask);
    R6 = __vslw(Y, OneInt);
    R5 = __vslw(X, OneInt);
    InfinityShifted = __vslw(FinalResult, Shift);
    FiniteYGreaterZero = __vcmpgtfp(Y, Zero);

    YSign = __vand(Y, SignMask);

    V = __vmulfp(Y, Reciprocal);

    R4 = __vcmpequw(R1, Zero);
    R1 = __vcmpequw(R1, Zero);
    R2 = __vcmpeqfp(X, Zero);
    R3 = __vcmpeqfp(Y, Zero);

    V2S2 = __vspltw(g_XMATanEstCoefficients, 2);
    AbsV = __vandc(V, SignMask);
    HalfPi = __vspltw(g_XMATanEstCoefficients, 3);
    N = __vspltw(g_XMATanEstCoefficients, 0);
    D = __vspltw(g_XMATanEstCoefficients, 1);

    R7 = __vcmpequw(R6, InfinityShifted);
    R6 = __vcmpequw(R6, InfinityShifted);
    R8 = __vcmpequw(R5, InfinityShifted);
    R5 = __vcmpequw(R5, InfinityShifted);

    V2S2 = __vmaddfp(V, V, V2S2);
    N = __vmaddfp(AbsV, HalfPi, N);

    Pi = __vor(Pi, YSign);
    PiOverTwo = __vor(PiOverTwo, YSign);
    PiOverFour = __vor(PiOverFour, YSign);
    ThreePiOverFour = __vor(ThreePiOverFour, YSign);

    R7 = __vsel(FiniteYGreaterZero, Zero, R7);

    D = __vmaddcfp(AbsV, D, V2S2);

    R1 = __vsel(Pi, YSign, R1);
    R2 = __vsel(FinalResult, PiOverTwo, R2);
    R4 = __vsel(ThreePiOverFour, PiOverFour, R4);

    N = __vmulfp(N, V);
    D = __vrefp(D);

    R3 = __vsel(R2, R1, R3);
    R5 = __vsel(PiOverTwo, R4, R5);
    R6 = __vsel(R3, R5, R6);
    R7 = __vsel(R6, R1, R7);

    R0 = __vmulfp(N, D);

    R8 = __vsel(R6, R7, R8);
    FinalResult = __vcmpequw(R8, FinalResult);

    FinalResult = __vsel(R8, R0, FinalResult);

    return FinalResult;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorLerp
(
    XMVECTOR V0, 
    XMVECTOR V1, 
    FLOAT    t
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Scale;
    XMVECTOR Length;
    XMVECTOR Result;

    // V0 + t * (V1 - V0)
    Scale = XMVectorReplicate(t);
    Length = XMVectorSubtract(V1, V0);
    Result = XMVectorMultiplyAdd(Length, Scale, V0);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Scale;
    XMVECTOR Length;
    XMVECTOR Result;

    Length = __vsubfp(V1, V0);

    Scale.v[0] = 
    Scale.v[1] = 
    Scale.v[2] = 
    Scale.v[3] = t;

    Result = __vmaddfp(Length, Scale, V0);

    return Result;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR Scale;
    XMVECTOR Length;
    XMVECTOR Result;

    Length = __vsubfp(V1, V0);

    Scale.v[0] = 
    Scale.v[1] = 
    Scale.v[2] = 
    Scale.v[3] = t;

    Result = __vmaddcfp(Length, Scale, V0);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Lerp;
    XMVECTOR Length;

    XMASSERT((((UINT_PTR)&t) & 3) == 0);

    Length = __vsubfp(V1, V0);
    Lerp = __lvlx(&t, 0);
    Lerp = __vspltw(Lerp, 0);
    Lerp = __vmaddcfp(Length, Lerp, V0);

    return Lerp;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorLerpV
(
    XMVECTOR V0, 
    XMVECTOR V1, 
    XMVECTOR T
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Length;
    XMVECTOR Result;

    // V0 + T * (V1 - V0)
    Length = XMVectorSubtract(V1, V0);
    Result = XMVectorMultiplyAdd(Length, T, V0);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Length;
    XMVECTOR Result;

    Length = __vsubfp(V1, V0);
    Result = __vmaddfp(Length, T, V0);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Result;

    Result = __vsubfp(V1, V0);
    Result = __vmaddcfp(T, Result, V0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorHermite
(
    XMVECTOR Position0, 
    XMVECTOR Tangent0, 
    XMVECTOR Position1, 
    XMVECTOR Tangent1, 
    FLOAT    t
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR P0;
    XMVECTOR T0;
    XMVECTOR P1;
    XMVECTOR T1;
    XMVECTOR Result;
    FLOAT    t2;
    FLOAT    t3;

    // Result = (2 * t^3 - 3 * t^2 + 1) * Position0 +
    //          (t^3 - 2 * t^2 + t) * Tangent0 +
    //          (-2 * t^3 + 3 * t^2) * Position1 +
    //          (t^3 - t^2) * Tangent1
    t2 = t * t;
    t3 = t * t2;

    P0 = XMVectorReplicate(2.0f * t3 - 3.0f * t2 + 1.0f);
    T0 = XMVectorReplicate(t3 - 2.0f * t2 + t);
    P1 = XMVectorReplicate(-2.0f * t3 + 3.0f * t2);
    T1 = XMVectorReplicate(t3 - t2);

    Result = XMVectorMultiply(P0, Position0);
    Result = XMVectorMultiplyAdd(T0, Tangent0, Result);
    Result = XMVectorMultiplyAdd(P1, Position1, Result);
    Result = XMVectorMultiplyAdd(T1, Tangent1, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR P0;
    XMVECTOR T0;
    XMVECTOR P1;
    XMVECTOR T1;
    XMVECTOR Result;
    FLOAT    t2;
    FLOAT    t3;
    FLOAT    t2x3;
    FLOAT    t3x2;

    t2 = t * t;
    t3 = t * t2;
    t2x3 = t2 * 3.0f;
    t3x2 = t3 + t3;

    P0.v[0] = 
    P0.v[1] = 
    P0.v[2] = 
    P0.v[3] = t3x2 - t2x3 + 1.0f;

    T0.v[0] = 
    T0.v[1] = 
    T0.v[2] = 
    T0.v[3] = t3 - (t2 + t2) + t;

    P1.v[0] = 
    P1.v[1] = 
    P1.v[2] = 
    P1.v[3] = -t3x2 + t2x3;

    T1.v[0] = 
    T1.v[1] = 
    T1.v[2] = 
    T1.v[3] = t3 - t2;

#if defined(_VMX32_INTRINSICS_)
    Result = __vmaddfp(P0, Position0, *(XMVECTOR*)g_XMNegativeZero);
#else
    Result = __vmulfp(P0, Position0);
#endif
    Result = __vmaddfp(T0, Tangent0, Result);
    Result = __vmaddfp(P1, Position1, Result);
    Result = __vmaddfp(T1, Tangent1, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR P0;
    XMVECTOR T0;
    XMVECTOR P1;
    XMVECTOR T1;
    XMVECTOR T;
    XMVECTOR TS, TC, S0, S1, PT;
    XMVECTOR Result;
    static CONST XMVECTOR C0 = {-2.0f, 3.0f, 2.0f, -3.0f};

    XMASSERT((((UINT_PTR)&t) & 3) == 0);

    XMDUMMY_INITIALIZE_VECTOR(S1);

    PT = __vspltisw(0);

    T = __lvlx(&t, 0);
    PT = __vrlimi(PT, T, 0x4, 3);
    T = __vspltw(T, 0);

    TS = __vmulfp(T, T);

    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S1 = __vspltw(S1, 3);
    PT = __vrlimi(PT, S1, 0x8, 0);
    S0 = __vsldoi(C0, S1, 1 << 2);

    PT = __vnmsubfp(TS, S0, PT);
    TC = __vmulfp(T, TS);
    S1 = __vrlimi(S1, C0, 0xA, 2);

    PT = __vmaddfp(TC, S1, PT);

    P0 = __vspltw(PT, 0);
    T0 = __vspltw(PT, 1);
    P1 = __vspltw(PT, 2);
    T1 = __vspltw(PT, 3);

    Result = __vmulfp(P0, Position0);
    Result = __vmaddfp(T0, Tangent0, Result);
    Result = __vmaddfp(P1, Position1, Result);
    Result = __vmaddfp(T1, Tangent1, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorHermiteV
(
    XMVECTOR Position0, 
    XMVECTOR Tangent0, 
    XMVECTOR Position1, 
    XMVECTOR Tangent1, 
    XMVECTOR T
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR P0;
    XMVECTOR T0;
    XMVECTOR P1;
    XMVECTOR T1;
    XMVECTOR Result;
    XMVECTOR T2;
    XMVECTOR T3;

    // Result = (2 * t^3 - 3 * t^2 + 1) * Position0 +
    //          (t^3 - 2 * t^2 + t) * Tangent0 +
    //          (-2 * t^3 + 3 * t^2) * Position1 +
    //          (t^3 - t^2) * Tangent1
    T2 = XMVectorMultiply(T, T);
    T3 = XMVectorMultiply(T , T2);

    P0 = XMVectorReplicate(2.0f * T3.v[0] - 3.0f * T2.v[0] + 1.0f);
    T0 = XMVectorReplicate(T3.v[1] - 2.0f * T2.v[1] + T.v[1]);
    P1 = XMVectorReplicate(-2.0f * T3.v[2] + 3.0f * T2.v[2]);
    T1 = XMVectorReplicate(T3.v[3] - T2.v[3]);

    Result = XMVectorMultiply(P0, Position0);
    Result = XMVectorMultiplyAdd(T0, Tangent0, Result);
    Result = XMVectorMultiplyAdd(P1, Position1, Result);
    Result = XMVectorMultiplyAdd(T1, Tangent1, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR P0;
    XMVECTOR T0;
    XMVECTOR P1;
    XMVECTOR T1;
    XMVECTOR Result;
    XMVECTOR T2;
    XMVECTOR T3;
    XMVECTOR T2x3;
    XMVECTOR T3x2;

    T2 = XMVectorMultiply(T, T);
    T3 = XMVectorMultiply(T , T2);
    T2x3 = XMVectorMultiply(T2, XMVectorSplatConstant(3, 0));
    T3x2 = XMVectorAdd(T3, T3);

    P0.v[0] = 
    P0.v[1] = 
    P0.v[2] = 
    P0.v[3] = T3x2.v[0] - T2x3.v[0] + 1.0f;

    T0.v[0] = 
    T0.v[1] = 
    T0.v[2] = 
    T0.v[3] = T3.v[1] - (T2.v[1] + T2.v[1]) + T.v[1];

    P1.v[0] = 
    P1.v[1] = 
    P1.v[2] = 
    P1.v[3] = -T3x2.v[3] + T2x3.v[3];

    T1.v[0] = 
    T1.v[1] = 
    T1.v[2] = 
    T1.v[3] = T3.v[3] - T2.v[3];

    Result = __vmaddfp(P0, Position0, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(T0, Tangent0, Result);
    Result = __vmaddfp(P1, Position1, Result);
    Result = __vmaddfp(T1, Tangent1, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR P0;
    XMVECTOR T0;
    XMVECTOR P1;
    XMVECTOR T1;
    XMVECTOR TS, TC, S0, S1, PT;
    XMVECTOR Result;
    static CONST XMVECTOR C0 = {-2.0f, 3.0f, 2.0f, -3.0f};

    XMDUMMY_INITIALIZE_VECTOR(S1);

    PT = __vspltisw(0);

    PT = __vrlimi(PT, T, 0x4, 0);

    TS = __vmulfp(T, T);

    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S1 = __vspltw(S1, 3);
    PT = __vrlimi(PT, S1, 0x8, 0);
    S0 = __vsldoi(C0, S1, 1 << 2);

    PT = __vnmsubfp(TS, S0, PT);
    TC = __vmulfp(T, TS);
    S1 = __vrlimi(S1, C0, 0xA, 2);

    PT = __vmaddfp(TC, S1, PT);

    P0 = __vspltw(PT, 0);
    T0 = __vspltw(PT, 1);
    P1 = __vspltw(PT, 2);
    T1 = __vspltw(PT, 3);

    Result = __vmulfp(P0, Position0);
    Result = __vmaddfp(T0, Tangent0, Result);
    Result = __vmaddfp(P1, Position1, Result);
    Result = __vmaddfp(T1, Tangent1, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorCatmullRom
(
    XMVECTOR Position0, 
    XMVECTOR Position1, 
    XMVECTOR Position2, 
    XMVECTOR Position3, 
    FLOAT    t
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR P0;
    XMVECTOR P1;
    XMVECTOR P2;
    XMVECTOR P3;
    XMVECTOR Result;
    FLOAT    t2;
    FLOAT    t3;

    // Result = ((-t^3 + 2 * t^2 - t) * Position0 +
    //           (3 * t^3 - 5 * t^2 + 2) * Position1 +
    //           (-3 * t^3 + 4 * t^2 + t) * Position2 +
    //           (t^3 - t^2) * Position3) * 0.5
    t2 = t * t;
    t3 = t * t2;

    P0 = XMVectorReplicate((-t3 + 2.0f * t2 - t) * 0.5f);
    P1 = XMVectorReplicate((3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f);
    P2 = XMVectorReplicate((-3.0f * t3 + 4.0f * t2 + t) * 0.5f);
    P3 = XMVectorReplicate((t3 - t2) * 0.5f);

    Result = XMVectorMultiply(P0, Position0);
    Result = XMVectorMultiplyAdd(P1, Position1, Result);
    Result = XMVectorMultiplyAdd(P2, Position2, Result);
    Result = XMVectorMultiplyAdd(P3, Position3, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR P0;
    XMVECTOR P1;
    XMVECTOR P2;
    XMVECTOR P3;
    XMVECTOR Result;
    FLOAT    t2;
    FLOAT    t3;

    t2 = t * t;
    t3 = t * t2;

    P0.v[0] = 
    P0.v[1] = 
    P0.v[2] = 
    P0.v[3] = (-t3 + 2.0f * t2 - t) * 0.5f;

    P1.v[0] = 
    P1.v[1] = 
    P1.v[2] = 
    P1.v[3] = (3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f;

    P2.v[0] = 
    P2.v[1] = 
    P2.v[2] = 
    P2.v[3] = (-3.0f * t3 + 4.0f * t2 + t) * 0.5f;

    P3.v[0] = 
    P3.v[1] = 
    P3.v[2] = 
    P3.v[3] = (t3 - t2) * 0.5f;

#if defined(_VMX32_INTRINSICS_)
    Result = __vmaddfp(P0, Position0, *(XMVECTOR*)g_XMNegativeZero);
#else
    Result = __vmulfp(P0, Position0);
#endif
    Result = __vmaddfp(P1, Position1, Result);
    Result = __vmaddfp(P2, Position2, Result);
    Result = __vmaddfp(P3, Position3, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR P0, P1, P2, P3;
    XMVECTOR S0, S1, S2, S3;
    XMVECTOR PT, T, TS, TC, Zero, One;
    XMVECTOR Result;
    static CONST XMVECTOR C0 = {-2.5f, 1.5f, -2.0f, -0.5f};

    XMASSERT((((UINT_PTR)&t) & 3) == 0);

    XMDUMMY_INITIALIZE_VECTOR(One);

    T = __lvlx(&t, 0);
    T = __vspltw(T, 0);
    PT = __vspltw(T, 0);

    TS = __vmulfp(T, T);

    One = __vupkd3d(One, VPACK_NORMSHORT2);
    One = __vspltw(One, 3);
    S2 = __vspltisw(-1);
    Zero = __vspltisw(0);

    PT = __vrlimi(PT, One, 0x1, 0);

    S0 = __vspltw(C0, 3);
    S1 = __vpermwi(C0, 0x40);
    S2 = __vslw(S2, S2);
    S3 = __vspltisw(0);

    TC = __vmulfp(T, TS);
    PT = __vrlimi(PT, TS, 0x4, 0);

    S0 = __vsldoi(S0, Zero, 1 << 2);
    S1 = __vrlimi(S1, Zero, 0x2, 0);
    S3 = __vrlimi(S3, C0, 0x4, 2);
    S2 = __vxor(C0, S2);

    S0 = __vrlimi(S0, One, 0x4, 0); // <-0.5, 1.0, -0.5, 0.0>
    S1 = __vrlimi(S1, One, 0x1, 0); // <1.5, -2.5, 0.0, 1.0>
    S3 = __vrlimi(S3, S2, 0x8, 3); // <0.5, -0.5, 0.0, 0.0>
    S2 = __vsldoi(S2, Zero, 1 << 2); // <-1.5, 2.0, 0.5, 0.0>

    PT = __vrlimi(PT, TC, 0x8, 0);

    P0 = __vmsum4fp(PT, S0);
    P1 = __vmsum4fp(PT, S1);
    P2 = __vmsum4fp(PT, S2);
    P3 = __vmsum4fp(PT, S3);

    Result = __vmulfp(P0, Position0);
    Result = __vmaddfp(P1, Position1, Result);
    Result = __vmaddfp(P2, Position2, Result);
    Result = __vmaddfp(P3, Position3, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorCatmullRomV
(
    XMVECTOR Position0, 
    XMVECTOR Position1, 
    XMVECTOR Position2, 
    XMVECTOR Position3, 
    XMVECTOR T
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR P0;
    XMVECTOR P1;
    XMVECTOR P2;
    XMVECTOR P3;
    XMVECTOR Result;
    XMVECTOR T2;
    XMVECTOR T3;

    // Result = ((-t^3 + 2 * t^2 - t) * Position0 +
    //           (3 * t^3 - 5 * t^2 + 2) * Position1 +
    //           (-3 * t^3 + 4 * t^2 + t) * Position2 +
    //           (t^3 - t^2) * Position3) * 0.5
    T2 = XMVectorMultiply(T, T);
    T3 = XMVectorMultiply(T , T2);

    P0 = XMVectorReplicate((-T3.v[0] + 2.0f * T2.v[0] - T.v[0]) * 0.5f);
    P1 = XMVectorReplicate((3.0f * T3.v[1] - 5.0f * T2.v[1] + 2.0f) * 0.5f);
    P2 = XMVectorReplicate((-3.0f * T3.v[2] + 4.0f * T2.v[2] + T.v[2]) * 0.5f);
    P3 = XMVectorReplicate((T3.v[3] - T2.v[3]) * 0.5f);

    Result = XMVectorMultiply(P0, Position0);
    Result = XMVectorMultiplyAdd(P1, Position1, Result);
    Result = XMVectorMultiplyAdd(P2, Position2, Result);
    Result = XMVectorMultiplyAdd(P3, Position3, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR P0;
    XMVECTOR P1;
    XMVECTOR P2;
    XMVECTOR P3;
    XMVECTOR Result;
    XMVECTOR T2;
    XMVECTOR T3;

    T2 = XMVectorMultiply(T, T);
    T3 = XMVectorMultiply(T , T2);

    P0.v[0] = 
    P0.v[1] = 
    P0.v[2] = 
    P0.v[3] = (-T3.v[0] + 2.0f * T2.v[0] - T.v[0]) * 0.5f;

    P1.v[0] = 
    P1.v[1] = 
    P1.v[2] = 
    P1.v[3] = (3.0f * T3.v[1] - 5.0f * T2.v[1] + 2.0f) * 0.5f;

    P2.v[0] = 
    P2.v[1] = 
    P2.v[2] = 
    P2.v[3] = (-3.0f * T3.v[2] + 4.0f * T2.v[2] + T.v[2]) * 0.5f;

    P3.v[0] = 
    P3.v[1] = 
    P3.v[2] = 
    P3.v[3] = (T3.v[3] - T2.v[3]) * 0.5f;

    Result = __vmaddfp(P0, Position0, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(P1, Position1, Result);
    Result = __vmaddfp(P2, Position2, Result);
    Result = __vmaddfp(P3, Position3, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR P0, P1, P2, P3;
    XMVECTOR S0, S1, S2, S3;
    XMVECTOR PT, TS, TC, Zero, One;
    XMVECTOR Result;
    static CONST XMVECTOR C0 = {-2.5f, 1.5f, -2.0f, -0.5f};

    XMDUMMY_INITIALIZE_VECTOR(One);

    PT = T;

    TS = __vmulfp(T, T);

    One = __vupkd3d(One, VPACK_NORMSHORT2);
    One = __vspltw(One, 3);
    S2 = __vspltisw(-1);
    Zero = __vspltisw(0);

    PT = __vrlimi(PT, One, 0x1, 0);

    S0 = __vspltw(C0, 3);
    S1 = __vpermwi(C0, 0x40);
    S2 = __vslw(S2, S2);
    S3 = __vspltisw(0);

    TC = __vmulfp(T, TS);
    PT = __vrlimi(PT, TS, 0x4, 0);

    S0 = __vsldoi(S0, Zero, 1 << 2);
    S1 = __vrlimi(S1, Zero, 0x2, 0);
    S3 = __vrlimi(S3, C0, 0x4, 2);
    S2 = __vxor(C0, S2);

    S0 = __vrlimi(S0, One, 0x4, 0); // <-0.5, 1.0, -0.5, 0.0>
    S1 = __vrlimi(S1, One, 0x1, 0); // <1.5, -2.5, 0.0, 1.0>
    S3 = __vrlimi(S3, S2, 0x8, 3); // <0.5, -0.5, 0.0, 0.0>
    S2 = __vsldoi(S2, Zero, 1 << 2); // <-1.5, 2.0, 0.5, 0.0>

    PT = __vrlimi(PT, TC, 0x8, 0);

    P0 = __vmsum4fp(PT, S0);
    P1 = __vmsum4fp(PT, S1);
    P2 = __vmsum4fp(PT, S2);
    P3 = __vmsum4fp(PT, S3);

    Result = __vmulfp(P0, Position0);
    Result = __vmaddfp(P1, Position1, Result);
    Result = __vmaddfp(P2, Position2, Result);
    Result = __vmaddfp(P3, Position3, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorBaryCentric
(
    XMVECTOR Position0, 
    XMVECTOR Position1, 
    XMVECTOR Position2, 
    FLOAT    f, 
    FLOAT    g
)
{
#if defined(_NO_INTRINSICS_)

    // Result = Position0 + f * (Position1 - Position0) + g * (Position2 - Position0)
    XMVECTOR P10;
    XMVECTOR P20;
    XMVECTOR ScaleF;
    XMVECTOR ScaleG;
    XMVECTOR Result;

    P10 = XMVectorSubtract(Position1, Position0);
    ScaleF = XMVectorReplicate(f);

    P20 = XMVectorSubtract(Position2, Position0);
    ScaleG = XMVectorReplicate(g);

    Result = XMVectorMultiplyAdd(P10, ScaleF, Position0);
    Result = XMVectorMultiplyAdd(P20, ScaleG, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR P10;
    XMVECTOR P20;
    XMVECTOR ScaleF;
    XMVECTOR ScaleG;
    XMVECTOR Result;

    P10 = __vsubfp(Position1, Position0);
    P20 = __vsubfp(Position2, Position0);

    ScaleF.v[0] = 
    ScaleF.v[1] = 
    ScaleF.v[2] = 
    ScaleF.v[3] = f;

    ScaleG.v[0] = 
    ScaleG.v[1] = 
    ScaleG.v[2] = 
    ScaleG.v[3] = g;

    Result = __vmaddfp(P10, ScaleF, Position0);
    Result = __vmaddfp(P20, ScaleG, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR P10;
    XMVECTOR P20;
    XMVECTOR ScaleF;
    XMVECTOR ScaleG;
    XMVECTOR Result;

    XMASSERT((((UINT_PTR)&f) & 3) == 0);
    XMASSERT((((UINT_PTR)&g) & 3) == 0);

    P10 = __vsubfp(Position1, Position0);
    P20 = __vsubfp(Position2, Position0);

    ScaleG = __lvlx(&g, 0);
    ScaleF = __lvlx(&f, 0);
    ScaleG = __vspltw(ScaleG, 0);
    Result = __vspltw(ScaleF, 0);

    Result = __vmaddcfp(P10, Result, Position0);
    Result = __vmaddfp(P20, ScaleG, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVectorBaryCentricV
(
    XMVECTOR Position0, 
    XMVECTOR Position1, 
    XMVECTOR Position2, 
    XMVECTOR F, 
    XMVECTOR G
)
{
#if defined(_NO_INTRINSICS_)

    // Result = Position0 + f * (Position1 - Position0) + g * (Position2 - Position0)
    XMVECTOR P10;
    XMVECTOR P20;
    XMVECTOR Result;

    P10 = XMVectorSubtract(Position1, Position0);
    P20 = XMVectorSubtract(Position2, Position0);

    Result = XMVectorMultiplyAdd(P10, F, Position0);
    Result = XMVectorMultiplyAdd(P20, G, Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR P10;
    XMVECTOR P20;
    XMVECTOR Result;

    P10 = __vsubfp(Position1, Position0);
    P20 = __vsubfp(Position2, Position0);

    Result = __vmaddfp(P10, F, Position0);
    Result = __vmaddfp(P20, G, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR P20;
    XMVECTOR Result;

    Result = __vsubfp(Position1, Position0);
    P20 = __vsubfp(Position2, Position0);

    Result = __vmaddcfp(F, Result, Position0);
    Result = __vmaddfp(G, P20, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

/****************************************************************************
 *
 * 2D Vector
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparison operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2Equal
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector2EqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector2EqualR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] == V2.v[0] && 
        V1.v[1] == V2.v[1])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] != V2.v[0] && 
        V1.v[1] != V2.v[1])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleXYXY);
    C2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleXYXY);

    __vcmpeqfpR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V1, 0x11);
    C2 = __vpermwi(V2, 0x11);

    __vcmpeqfpR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2EqualInt
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector2EqualIntR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector2EqualIntR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.u[0] == V2.u[0] && 
        V1.u[1] == V2.u[1])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.u[0] != V2.u[0] && 
        V1.u[1] != V2.u[1])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleXYXY);
    C2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleXYXY);

    __vcmpequwR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V1, 0x11);
    C2 = __vpermwi(V2, 0x11);

    __vcmpequwR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2NearEqual
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR Epsilon
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Delta;

    Delta = XMVectorSubtract(V1, V2);
    Delta = XMVectorAbs(Delta);

    return (Delta.v[0] <= Epsilon.v[0] &&
            Delta.v[1] <= Epsilon.v[1]);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    Delta;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    Delta = __vsubfp(V1, V2);
    Delta = __vsel(Epsilon, Delta, *(XMVECTOR*)g_XMSelect1100);
    __vcmpbfpR(Delta, Epsilon, &CR);

    return XMComparisonAllInBounds(CR);

#else // _VMX128_INTRINSICS_

    XMVECTOR    Delta;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    Delta = __vsubfp(V1, V2);
    Delta = __vrlimi(Delta, Epsilon, 0x3, 0);
    __vcmpbfpR(Delta, Epsilon, &CR);

    return XMComparisonAllInBounds(CR);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2NotEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAnyFalse(XMVector2EqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2NotEqualInt
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAnyFalse(XMVector2EqualIntR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2Greater
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector2GreaterR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector2GreaterR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] > V2.v[0] && 
        V1.v[1] > V2.v[1])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] <= V2.v[0] && 
        V1.v[1] <= V2.v[1])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleXYXY);
    C2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleXYXY);

    __vcmpgtfpR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V1, 0x11);
    C2 = __vpermwi(V2, 0x11);

    __vcmpgtfpR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2GreaterOrEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector2GreaterOrEqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector2GreaterOrEqualR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] >= V2.v[0] && 
        V1.v[1] >= V2.v[1])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] < V2.v[0] && 
        V1.v[1] < V2.v[1])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleXYXY);
    C2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleXYXY);

    __vcmpgefpR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V1, 0x11);
    C2 = __vpermwi(V2, 0x11);

    __vcmpgefpR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2Less
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector2GreaterR(V2, V1));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2LessOrEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector2GreaterOrEqualR(V2, V1));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2InBounds
(
    XMVECTOR V, 
    XMVECTOR Bounds
)
{
    return XMComparisonAllInBounds(XMVector2InBoundsR(V, Bounds));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector2InBoundsR
(
    XMVECTOR V, 
    XMVECTOR Bounds
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if ((V.v[0] <= Bounds.v[0] && V.v[0] >= -Bounds.v[0]) && 
        (V.v[1] <= Bounds.v[1] && V.v[1] >= -Bounds.v[1]))
    {
        CR |= XM_CRMASK_CR6BOUNDS;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V, V, *(XMVECTOR*)g_XMSwizzleXYXY);
    C2 = __vperm(Bounds, Bounds, *(XMVECTOR*)g_XMSwizzleXYXY);

    __vcmpbfpR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V, 0x11);
    C2 = __vpermwi(Bounds, 0x11);

    __vcmpbfpR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2IsNaN
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return (XMISNAN(V.v[0]) ||
            XMISNAN(V.v[1]));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C = __vsel(*(XMVECTOR*)g_XMSelect1100, V, *(XMVECTOR*)g_XMSelect1100);

    __vcmpeqfpR(C, C, &CR);

    return XMComparisonAnyFalse(CR);

#else // _VMX128_INTRINSICS_

    XMVECTOR    C;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C = __vpermwi(V, 0x11);

    __vcmpeqfpR(C, C, &CR);

    return XMComparisonAnyFalse(CR);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector2IsInfinite
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return (XMISINF(V.v[0]) ||
            XMISINF(V.v[1]));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                C;
    UINT                    CR;
    static CONST XMVECTORI  Mask = {0x7FFFFFFF, 0x7FFFFFFF, 0x00000000, 0x00000000};

    XMDUMMY_INITIALIZE_UINT(CR);

    C = __vand(V, *(XMVECTOR*)Mask);
    __vcmpequwR(C, *(XMVECTOR*)g_XMInfinity, &CR);

    return XMComparisonAnyTrue(CR);

#else // _VMX128_INTRINSICS_

    XMVECTOR OneInt, TrueInt;
    XMVECTOR XYXY, VShifted, InfinityShifted;
    XMVECTOR Shift;
    UINT     CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    XYXY = __vpermwi(V, 0x11);
    OneInt = __vspltisw(1);
    TrueInt = __vspltisw(-1);
    Shift = __vspltisw(-8); // Shift 24
    VShifted = __vslw(XYXY, OneInt);
    InfinityShifted = __vslw(TrueInt, Shift);

    __vcmpequwR(VShifted, InfinityShifted, &CR);

    return XMComparisonAnyTrue(CR);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2Dot
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] =
    Result.v[1] =
    Result.v[2] =
    Result.v[3] = V1.v[0] * V2.v[0] + V1.v[1] * V2.v[1];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    // VMX2 hardware will have dedicated dot product logic, but with VMX1 it must be accomplished
    // using a multiply, an addition, and a rotation.
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(V1, V2, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Result = __vspltw(D0, 0);

    return Result;

#else // _VMX128_INTRINSICS_

    // Overflow will result in a QNaN instead of +/- infinity.

    XMVECTOR D1, D2, Zero;
    XMVECTOR Result;

    Zero = __vspltisw(0);
    D1 = __vsldoi(Zero, V1, 2 << 2);
    D2 = __vsldoi(Zero, V2, 2 << 2);

    Result = __vmsum4fp(D1, D2);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2Cross
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] =
    Result.v[1] =
    Result.v[2] =
    Result.v[3] = V1.v[0] * V2.v[1] - V1.v[1] * V2.v[0];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR CrossP;
    XMVECTOR CrossR;
    XMVECTOR Result;

    CrossP = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleYXZW);
    CrossR = __vmaddfp(V1, CrossP, *(XMVECTOR*)g_XMNegativeZero);
    CrossP = __vperm(CrossR, CrossR, *(XMVECTOR*)g_XMSwizzleYXZW);
    CrossR = __vsubfp(CrossR, CrossP);
    Result = __vspltw(CrossR, 0);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR CrossP;
    XMVECTOR CrossR;
    XMVECTOR Result;

    CrossP = __vpermwi(V2, 0x4B);
    CrossR = __vmulfp(V1, CrossP);
    CrossP = __vpermwi(CrossR, 0x4B);
    CrossR = __vsubfp(CrossR, CrossP);
    Result = __vspltw(CrossR, 0);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2LengthSq
(
    XMVECTOR V
)
{
    return XMVector2Dot(V, V);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2ReciprocalLengthEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector2LengthSq(V);
    Result = XMVectorReciprocalSqrtEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    D1 = __vspltw(D0, 0);
    Result = __vrsqrtefp(D1);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Zero;
    XMVECTOR Result;

    Zero = __vspltisw(0);
    Result = __vsldoi(Zero, V, 2 << 2);
    Result = __vmsum4fp(Result, Result);
    Result = __vrsqrtefp(Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2ReciprocalLength
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector2LengthSq(V);
    Result = XMVectorReciprocalSqrt(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    D1 = __vspltw(D0, 0);
    Rsq = __vrsqrtefp(D1);
    RT = __vmaddfp(D1, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    Result = __vmaddfp(Rsq, RT, Rsq);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Zero;
    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR H;

    Zero = __vspltisw(0);
    D = __vsldoi(Zero, V, 2 << 2);
    D = __vmsum4fp(D, D);
    H = __vspltisw(1);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);

    return Rsq;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2LengthEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector2LengthSq(V);
    Result = XMVectorSqrtEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR LengthSq;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

#if 0

    // Faster, but returns a QNaN on zero and infinite vectors
    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    LengthSq = __vspltw(D0, 0);
    Result = __vrsqrtefp(LengthSq);
    Result = __vmaddfp(LengthSq, Result, *(XMVECTOR*)g_XMNegativeZero);

#else

    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    Zero = __vspltisw(0);
    D0 = __vaddfp(D0, D1);
    LengthSq = __vspltw(D0, 0);
    Result = __vrsqrtefp(LengthSq);
    InfiniteLength = __vcmpequw(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(LengthSq, Zero);
    Result = __vmaddfp(LengthSq, Result, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Result = __vsel(LengthSq, Result, Select);

#endif

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Zero;
    XMVECTOR Result;

#if 0

    // Faster, but returns a QNaN on zero and infinite vectors.

    Zero = __vspltisw(0);
    D = __vsldoi(Zero, V, 2 << 2);
    D = __vmsum4fp(D, D);
    Rsq = __vrsqrtefp(D);
    Result = __vmulfp(D, Rsq);

#else

    // Returns a QNaN on infinite vectors.

    XMVECTOR Length;

    Zero = __vspltisw(0);
    D = __vsldoi(Zero, V, 2 << 2);
    D = __vmsum4fp(D, D);
    Rsq = __vrsqrtefp(D);
    Result = __vcmpeqfp(D, Zero);
    Length = __vmulfp(D, Rsq);
    Result = __vsel(Length, D, Result);

#endif

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2Length
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector2LengthSq(V);
    Result = XMVectorSqrt(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR LengthSq;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    Zero = __vspltisw(0);
    D0 = __vaddfp(D0, D1);
    LengthSq = __vspltw(D0, 0);
    Rsq = __vrsqrtefp(LengthSq);
    RT = __vmaddfp(LengthSq, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    Result = __vmaddfp(Rsq, RT, Rsq);
    InfiniteLength = __vcmpequw(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(LengthSq, Zero);
    Result = __vmaddfp(LengthSq, Result, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Result = __vsel(LengthSq, Result, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    // Returns a QNaN on infinite vectors.

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR Length;
    XMVECTOR H;

    Zero = __vspltisw(0);
    D = __vsldoi(Zero, V, 2 << 2);
    H = __vspltisw(1);
    D = __vmsum4fp(D, D);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);
    Result = __vcmpeqfp(D, Zero);
    Length = __vmulfp(D, Rsq);
    Result = __vsel(Length, D, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// XMVector2NormalizeEst uses a reciprocal estimate and
// returns QNaN on zero and infinite vectors.

XMFINLINE XMVECTOR XMVector2NormalizeEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;
    Result = XMVector2ReciprocalLength(V);
    Result = XMVectorMultiply(V, Result);
    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR LengthSq;
    XMVECTOR Result;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    LengthSq = __vspltw(D0, 0);
    Result = __vrsqrtefp(LengthSq);
    Result = __vmaddfp(V, Result, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Zero;
    XMVECTOR Result;

#if 1

    // Faster, but returns a QNaN on zero and infinite vectors.

    Zero = __vspltisw(0);
    D = __vsldoi(Zero, V, 2 << 2);
    D = __vmsum4fp(D, D);
    Rsq = __vrsqrtefp(D);
    Result = __vmulfp(V, Rsq);

#else

    // Returns a QNaN on infinite vectors.

    XMVECTOR Zero;
    XMVECTOR N;

    Zero = __vspltisw(0);
    D = __vsldoi(Zero, V, 2 << 2);
    D = __vmsum4fp(D, D);
    Zero = __vspltisw(0);
    Rsq = __vrsqrtefp(D);
    Result = __vcmpeqfp(D, Zero);
    N = __vmulfp(V, Rsq);
    Result = __vsel(N, D, Result);

#endif

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2Normalize
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR LengthSq;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR Result;

    LengthSq = XMVector2LengthSq(V);
    Zero = XMVectorZero();
    Result = XMVectorReciprocalSqrt(LengthSq);
    InfiniteLength = XMVectorEqualInt(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = XMVectorEqual(LengthSq, Zero);
    Result = XMVectorMultiply(V, Result);
    Select = XMVectorEqualInt(InfiniteLength, ZeroLength);
    Result = XMVectorSelect(LengthSq, Result, Select);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR LengthSq;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Zero = __vspltisw(0);
    LengthSq = __vspltw(D0, 0);
    RcpLength = __vrsqrtefp(LengthSq);
    OneHalfLengthSq = __vmaddfp(LengthSq, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    InfiniteLength = __vcmpequw(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(LengthSq, Zero);
    Scale = __vmaddfp(RcpLength, RcpLength, *(XMVECTOR*)g_XMNegativeZero);
    Scale = __vnmsubfp(OneHalfLengthSq, Scale, g_XMOneHalf);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Result = __vmaddfp(RcpLength, Scale, RcpLength);
    Result = __vmaddfp(V, Result, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vsel(LengthSq, Result, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    // Returns a QNaN on infinite vectors.

    XMVECTOR D;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR H;
    XMVECTOR N;

    Zero = __vspltisw(0);
    D = __vsldoi(Zero, V, 2 << 2);
    H = __vspltisw(1);
    D = __vmsum4fp(D, D);
    H = __vcfsx(H, 1);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    Result = __vcmpeqfp(D, Zero);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddcfp(RcpLength, H, RcpLength);
    N = __vmulfp(V, H);
    Result = __vsel(N, D, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2ClampLength
(
    XMVECTOR V, 
    FLOAT    LengthMin, 
    FLOAT    LengthMax
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    ClampMax = XMVectorReplicate(LengthMax);
    ClampMin = XMVectorReplicate(LengthMin);

    return XMVector2ClampLengthV(V, ClampMin, ClampMax);

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    ClampMax.v[0] =
    ClampMax.v[1] = 
    ClampMax.v[2] =
    ClampMax.v[3] = LengthMax;

    ClampMin.v[0] =
    ClampMin.v[1] = 
    ClampMin.v[2] =
    ClampMin.v[3] = LengthMin;

    return XMVector2ClampLengthV(V, ClampMin, ClampMax);

#else // _VMX128_INTRINSICS_

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    XMASSERT(((UINT_PTR)&LengthMin & 3) == 0);
    XMASSERT(((UINT_PTR)&LengthMax & 3) == 0);

    ClampMax = __lvlx(&LengthMax, 0);
    ClampMin = __lvlx(&LengthMin, 0);

    ClampMax = __vspltw(ClampMax, 0);
    ClampMin = __vspltw(ClampMin, 0);

    return XMVector2ClampLengthV(V, ClampMin, ClampMax);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2ClampLengthV
(
    XMVECTOR V, 
    XMVECTOR LengthMin, 
    XMVECTOR LengthMax
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR RcpLength;
    XMVECTOR Length;
    XMVECTOR Normal;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR ControlMax;
    XMVECTOR ControlMin;
    XMVECTOR Control;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x));
    XMASSERT(XMVector2GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector2GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector2GreaterOrEqual(LengthMax, LengthMin));

    LengthSq = XMVector2LengthSq(V);

    Zero = XMVectorZero();

    RcpLength = XMVectorReciprocalSqrt(LengthSq);

    InfiniteLength = XMVectorEqualInt(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = XMVectorEqual(LengthSq, Zero);

    Length = XMVectorMultiply(LengthSq, RcpLength);

    Normal = XMVectorMultiply(V, RcpLength);

    Select = XMVectorEqualInt(InfiniteLength, ZeroLength);
    Length = XMVectorSelect(LengthSq, Length, Select);
    Normal = XMVectorSelect(LengthSq, Normal, Select);

    ControlMax = XMVectorGreater(Length, LengthMax);
    ControlMin = XMVectorLess(Length, LengthMin);

    ClampLength = XMVectorSelect(Length, LengthMax, ControlMax);
    ClampLength = XMVectorSelect(ClampLength, LengthMin, ControlMin);

    Result = XMVectorMultiply(Normal, ClampLength);

    // Preserve the original vector (with no precision loss) if the length falls within the given range
    Control = XMVectorEqualInt(ControlMax, ControlMin);
    Result = XMVectorSelect(Result, V, Control);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR RcpLength;
    XMVECTOR Length;
    XMVECTOR Rcp;
    XMVECTOR Rsq;
    XMVECTOR RT;
    XMVECTOR Normal;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR ControlMax;
    XMVECTOR ControlMin;
    XMVECTOR Control;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x));
    XMASSERT(XMVector2GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector2GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector2GreaterOrEqual(LengthMax, LengthMin));

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);

    D1 = __vsldoi(D0, D0, 4);

    Zero = __vspltisw(0);

    D0 = __vaddfp(D0, D1);

    LengthSq = __vspltw(D0, 0);

    Rsq = __vrsqrtefp(LengthSq);
    RT = __vmaddfp(LengthSq, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    RcpLength = __vmaddfp(Rsq, RT, Rsq);

    InfiniteLength = __vcmpequw(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(LengthSq, Zero);

    Length = __vmaddfp(LengthSq, RcpLength, *(XMVECTOR*)g_XMNegativeZero);

    Normal = __vmaddfp(V, RcpLength, *(XMVECTOR*)g_XMNegativeZero);

    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Length = __vsel(LengthSq, Length, Select);
    Normal = __vsel(LengthSq, Normal, Select);

    ControlMax = __vcmpgtfp(Length, LengthMax);
    ControlMin = __vcmpgtfp(LengthMin, Length);

    ClampLength = __vsel(Length, LengthMax, ControlMax);
    ClampLength = __vsel(ClampLength, LengthMin, ControlMin);

    Result = __vmaddfp(Normal, ClampLength, *(XMVECTOR*)g_XMNegativeZero);

    Control = __vcmpequw(ControlMax, ControlMin);

    Result = __vsel(Result, V, Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR Length, LengthS;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Normal, NormalS;
    XMVECTOR Zero;
    XMVECTOR Control;
    XMVECTOR H;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x));
    XMASSERT(XMVector2GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector2GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector2GreaterOrEqual(LengthMax, LengthMin));

    Zero = __vspltisw(0);

    D = __vsldoi(Zero, V, 2 << 2);
    H = __vspltisw(1);
    LengthSq = __vmsum4fp(D, D);

    H = __vcfsx(H, 1);

    Rsq = __vrsqrtefp(LengthSq);
    RT = __vmulfp(LengthSq, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    H = __vmaddcfp(Rsq, H, Rsq);

    Length = __vmulfp(LengthSq, H);

    Normal = __vmulfp(V, H);

    LengthS = __vcmpeqfp(LengthSq, Zero);
    NormalS = __vcmpeqfp(LengthSq, Zero);

    LengthS = __vsel(Length, LengthSq, LengthS);
    NormalS = __vsel(Normal, LengthSq, NormalS);

    Control = __vcmpgtfp(LengthS, LengthMax);
    ClampLength = __vcmpgtfp(LengthMin, LengthS);

    Result = __vcmpequw(Control, ClampLength);

    Control = __vsel(LengthS, LengthMax, Control);
    ClampLength = __vsel(Control, LengthMin, ClampLength);

    RT = __vmulfp(NormalS, ClampLength);

    Result = __vsel(RT, V, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2Reflect
(
    XMVECTOR Incident, 
    XMVECTOR Normal
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    // Result = Incident - (2 * dot(Incident, Normal)) * Normal
    Result = XMVector2Dot(Incident, Normal);
    Result = XMVectorAdd(Result, Result);
    Result = XMVectorNegativeMultiplySubtract(Result, Normal, Incident);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(Incident, Normal, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Result = __vspltw(D0, 0);
    Result = __vaddfp(Result, Result);
    Result = __vnmsubfp(Result, Normal, Incident);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D, DI, DN;
    XMVECTOR Zero;
    XMVECTOR Result;

    Zero = __vspltisw(0);
    DI = __vsldoi(Zero, Incident, 2 << 2);
    DN = __vsldoi(Zero, Normal, 2 << 2);
    D = __vmsum4fp(DI, DN);
    D = __vaddfp(D, D);
    Result = Incident;
    Result = __vnmsubfp(D, Normal, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2Refract
(
    XMVECTOR Incident, 
    XMVECTOR Normal, 
    FLOAT    RefractionIndex
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Index;

    Index = XMVectorReplicate(RefractionIndex);

    return XMVector2RefractV(Incident, Normal, Index);

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR Index;

    Index.v[0] =
    Index.v[1] = 
    Index.v[2] =
    Index.v[3] = RefractionIndex;

    return XMVector2RefractV(Incident, Normal, Index);

#else // _VMX128_INTRINSICS_

    XMVECTOR Index;

    XMASSERT(((UINT_PTR)&RefractionIndex & 3) == 0);

    Index = __lvlx(&RefractionIndex, 0);
    Index = __vspltw(Index, 0);

    return XMVector2RefractV(Incident, Normal, Index);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2RefractV
(
    XMVECTOR Incident, 
    XMVECTOR Normal, 
    XMVECTOR RefractionIndex
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR        IDotN;
    XMVECTOR        R;
    CONST XMVECTOR  Zero = XMVectorZero();

    // Result = RefractionIndex * Incident - Normal * (RefractionIndex * dot(Incident, Normal) + 
    // sqrt(1 - RefractionIndex * RefractionIndex * (1 - dot(Incident, Normal) * dot(Incident, Normal))))

    IDotN = XMVector2Dot(Incident, Normal);

    // R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
    R = XMVectorNegativeMultiplySubtract(IDotN, IDotN, g_XMOne);
    R = XMVectorMultiply(R, RefractionIndex);
    R = XMVectorNegativeMultiplySubtract(R, RefractionIndex, g_XMOne);

    if (XMVector4LessOrEqual(R, Zero))
    {
        // Total internal reflection
        return Zero;
    }
    else
    {
        XMVECTOR Result;

        // R = RefractionIndex * IDotN + sqrt(R)
        R = XMVectorSqrt(R);
        R = XMVectorMultiplyAdd(RefractionIndex, IDotN, R);

        // Result = RefractionIndex * Incident - Normal * R
        Result = XMVectorMultiply(RefractionIndex, Incident);
        Result = XMVectorNegativeMultiplySubtract(Normal, R, Result);

        return Result;
    }

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR IDotN;
    XMVECTOR R;
    XMVECTOR I;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR Control;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Zero;
    XMVECTOR Result;

    D0 = __vmaddfp(Incident, Normal, *(XMVECTOR*)g_XMNegativeZero);

    Zero = __vspltisw(0);

    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);

    IDotN = __vspltw(D0, 0);

    R = __vnmsubfp(IDotN, IDotN, g_XMOne);
    R = __vmaddfp(R, RefractionIndex, *(XMVECTOR*)g_XMNegativeZero);
    R = __vnmsubfp(R, RefractionIndex, g_XMOne);

    Rsq = __vrsqrtefp(R);
    RT = __vmaddfp(R, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D0 = __vmaddfp(Rsq, RT, Rsq);

    InfiniteLength = __vcmpequw(R, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(R, Zero);

    Control = __vcmpgefp(Zero, R);

    D1 = __vmaddfp(R, D0, *(XMVECTOR*)g_XMNegativeZero);

    Select = __vcmpequw(InfiniteLength, ZeroLength);

    I = __vmaddfp(RefractionIndex, Incident, *(XMVECTOR*)g_XMNegativeZero);

    R = __vsel(R, D1, Select);

    R = __vmaddfp(RefractionIndex, IDotN, R);

    Result = __vnmsubfp(Normal, R, I);

    Result = __vsel(Result, Zero, Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR DI, DN;
    XMVECTOR IDotN;
    XMVECTOR R;
    XMVECTOR I;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR ZL;
    XMVECTOR D1;
    XMVECTOR H;
    XMVECTOR Zero;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(R);

    Zero = __vspltisw(0);
    DI = __vsldoi(Zero, Incident, 2 << 2);
    DN = __vsldoi(Zero, Normal, 2 << 2);
    IDotN = __vmsum4fp(DI, DN);

    H = __vspltisw(1);
    R = __vupkd3d(R, VPACK_NORMSHORT2);
    H = __vcfsx(H, 1);
    RT = __vspltw(R, 3);
    R = __vspltw(R, 3);

    RT = __vnmsubfp(IDotN, IDotN, RT);
    RT = __vmulfp(RT, RefractionIndex);
    R = __vnmsubfp(RT, RefractionIndex, R);

    Rsq = __vrsqrtefp(R);
    RT = __vmulfp(R, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Zero = __vspltisw(0);
    H = __vmaddcfp(Rsq, H, Rsq);

    ZL = __vcmpeqfp(R, Zero);

    Result = __vcmpgefp(Zero, R);

    D1 = __vmulfp(R, H);

    I = __vmulfp(RefractionIndex, Incident);

    ZL = __vsel(D1, R, ZL);

    ZL = __vmaddfp(RefractionIndex, IDotN, ZL);

    I = __vnmsubfp(Normal, ZL, I);

    Result = __vsel(I, Zero, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2Orthogonal
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = -V.v[1];
    Result.v[1] = V.v[0];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                NegativeV;
    XMVECTOR                SignMask;
    XMVECTOR                Result;
    static CONST XMVECTORI  Control = {XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_1Y, XM_PERMUTE_0X};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    NegativeV = __vxor(V, SignMask);

    Result = __vperm(V, NegativeV, *(XMVECTOR*)Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR NegativeV;
    XMVECTOR SignMask;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Result = __vspltw(V, 0);
    NegativeV = __vxor(V, SignMask);

    Result = __vrlimi(Result, NegativeV, 0x8, 1);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2AngleBetweenNormalsEst
(
    XMVECTOR N1, 
    XMVECTOR N2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector2Dot(N1, N2);
    Result = XMVectorACosEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR CosAngle;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(N1, N2, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    CosAngle = __vspltw(D0, 0);

    Result = XMVectorACosEst(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D1, D2, Zero;
    XMVECTOR Result;

    Zero = __vspltisw(0);
    D1 = __vsldoi(Zero, N1, 2 << 2);
    D2 = __vsldoi(Zero, N2, 2 << 2);
    Result = __vmsum4fp(D1, D2);

    Result = XMVectorACosEst(Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2AngleBetweenNormals
(
    XMVECTOR N1, 
    XMVECTOR N2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector2Dot(N1, N2);
    Result = XMVectorACos(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR CosAngle;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(N1, N2, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    CosAngle = __vspltw(D0, 0);

    Result = XMVectorACos(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D1, D2, Zero;
    XMVECTOR Result;

    Zero = __vspltisw(0);
    D1 = __vsldoi(Zero, N1, 2 << 2);
    D2 = __vsldoi(Zero, N2, 2 << 2);
    Result = __vmsum4fp(D1, D2);

    Result = XMVectorACos(Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2AngleBetweenVectors
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR L1;
    XMVECTOR L2;
    XMVECTOR Dot;
    XMVECTOR CosAngle;
    XMVECTOR Result;

    L1 = XMVector2ReciprocalLength(V1);
    L2 = XMVector2ReciprocalLength(V2);

    Dot = XMVector2Dot(V1, V2);

    L1 = XMVectorMultiply(L1, L2);

    CosAngle = XMVectorMultiply(Dot, L1);

    Result = XMVectorACos(CosAngle);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR L1;
    XMVECTOR L2;
    XMVECTOR Dot;
    XMVECTOR CosAngle;
    XMVECTOR L1A, L1B, L2A, L2B, DotA, DotB;
    XMVECTOR Rsq1, Rsq2, Rcp1, Rcp2, RT1, RT2;
    XMVECTOR Result;

    L1A = __vmaddfp(V1, V1, *(XMVECTOR*)g_XMNegativeZero);
    L2A = __vmaddfp(V2, V2, *(XMVECTOR*)g_XMNegativeZero);
    DotA = __vmaddfp(V1, V2, *(XMVECTOR*)g_XMNegativeZero);

    L1B = __vsldoi(L1A, L1A, 4);
    L2B = __vsldoi(L2A, L2A, 4);
    DotB = __vsldoi(DotA, DotA, 4);

    L1A = __vaddfp(L1A, L1B);
    L2A = __vaddfp(L2A, L2B);
    DotA = __vaddfp(DotA, DotB);

    L1B = __vspltw(L1A, 0);
    L2B = __vspltw(L2A, 0);
    Dot = __vspltw(DotA, 0);

    Rsq1 = __vrsqrtefp(L1B);
    Rsq2 = __vrsqrtefp(L2B);
    RT1 = __vmaddfp(L1B, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    RT2 = __vmaddfp(L2B, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp1 = __vmaddfp(Rsq1, Rsq1, *(XMVECTOR*)g_XMNegativeZero);
    Rcp2 = __vmaddfp(Rsq2, Rsq2, *(XMVECTOR*)g_XMNegativeZero);
    RT1 = __vnmsubfp(RT1, Rcp1, g_XMOneHalf);
    RT2 = __vnmsubfp(RT2, Rcp2, g_XMOneHalf);
    L1 = __vmaddfp(Rsq1, RT1, Rsq1);
    L2 = __vmaddfp(Rsq2, RT2, Rsq2);

    L1 = __vmaddfp(L1, L2, *(XMVECTOR*)g_XMNegativeZero);

    CosAngle = __vmaddfp(Dot, L1, *(XMVECTOR*)g_XMNegativeZero);

    Result = XMVectorACos(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Zero;
    XMVECTOR VS1, VS2;
    XMVECTOR D1, D2;
    XMVECTOR Rsq1, Rsq2;
    XMVECTOR Rcp1, Rcp2;
    XMVECTOR RT1, RT2;
    XMVECTOR H1, H2;
    XMVECTOR L;
    XMVECTOR CosAngle;
    XMVECTOR Result;

    Zero = __vspltisw(0);
    VS1 = __vsldoi(Zero, V1, 2 << 2);
    VS2 = __vsldoi(Zero, V2, 2 << 2);
    D1 = __vmsum4fp(VS1, VS1);
    D2 = __vmsum4fp(VS2, VS2);
    H2 = __vspltisw(1);
    H1 = __vcfsx(H2, 1);
    H2 = __vcfsx(H2, 1);
    Rsq1 = __vrsqrtefp(D1);
    Rsq2 = __vrsqrtefp(D2);
    RT1 = __vmulfp(D1, H1);
    RT2 = __vmulfp(D2, H2);
    Rcp1 = __vmulfp(Rsq1, Rsq1);
    Rcp2 = __vmulfp(Rsq2, Rsq2);
    H1 = __vnmsubfp(RT1, Rcp1, H1);
    H2 = __vnmsubfp(RT2, Rcp2, H2);
    Result = __vmsum4fp(VS1, VS2);
    Rsq1 = __vmaddfp(Rsq1, H1, Rsq1);
    Rsq2 = __vmaddfp(Rsq2, H2, Rsq2);

    L = __vmulfp(Rsq1, Rsq2);

    CosAngle = __vmulfp(Result, L);

    Result = XMVectorACos(CosAngle);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2LinePointDistance
(
    XMVECTOR LinePoint1, 
    XMVECTOR LinePoint2, 
    XMVECTOR Point
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR PointVector;
    XMVECTOR LineVector;
    XMVECTOR ReciprocalLengthSq;
    XMVECTOR PointProjectionScale;
    XMVECTOR DistanceVector;
    XMVECTOR Result;

    // Given a vector PointVector from LinePoint1 to Point and a vector
    // LineVector from LinePoint1 to LinePoint2, the scaled distance 
    // PointProjectionScale from LinePoint1 to the perpendicular projection
    // of PointVector onto the line is defined as:
    //
    //     PointProjectionScale = dot(PointVector, LineVector) / LengthSq(LineVector)

    PointVector = XMVectorSubtract(Point, LinePoint1);
    LineVector = XMVectorSubtract(LinePoint2, LinePoint1);

    ReciprocalLengthSq = XMVector2LengthSq(LineVector);
    ReciprocalLengthSq = XMVectorReciprocal(ReciprocalLengthSq);

    PointProjectionScale = XMVector2Dot(PointVector, LineVector);
    PointProjectionScale = XMVectorMultiply(PointProjectionScale, ReciprocalLengthSq);

    DistanceVector = XMVectorMultiply(LineVector, PointProjectionScale);
    DistanceVector = XMVectorSubtract(PointVector, DistanceVector);

    Result = XMVector2Length(DistanceVector);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR PointVector;
    XMVECTOR LineVector;
    XMVECTOR ReciprocalLength;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Scale;
    XMVECTOR D0, D1;
    XMVECTOR CrossP;
    XMVECTOR CrossR;
    XMVECTOR SignMask;
    XMVECTOR Result;

    LineVector = __vsubfp(LinePoint2, LinePoint1);
    PointVector = __vsubfp(Point, LinePoint1);

    CrossP = __vperm(PointVector, PointVector, *(XMVECTOR*)g_XMSwizzleYXZW);
    D0 = __vmaddfp(LineVector, LineVector, *(XMVECTOR*)g_XMNegativeZero);

    CrossR = __vmaddfp(LineVector, CrossP, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 4);

    CrossP = __vperm(CrossR, CrossR, *(XMVECTOR*)g_XMSwizzleYXZW);
    D0 = __vaddfp(D0, D1);

    CrossR = __vsubfp(CrossR, CrossP);
    D1 = __vspltw(D0, 0);

    Scale = __vspltw(CrossR, 0);

    Rsq = __vrsqrtefp(D1);
    SignMask = __vspltisw(-1);
    RT = __vmaddfp(D1, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    SignMask = __vslw(SignMask, SignMask);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);

    Scale = __vandc(Scale, SignMask);

    ReciprocalLength = __vmaddfp(Rsq, RT, Rsq);

    Result = __vmaddfp(Scale, ReciprocalLength, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR PointVector;
    XMVECTOR LineVector;
    XMVECTOR PointProjectionScale;
    XMVECTOR DistanceVector;
    XMVECTOR ReciprocalLengthSq;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Length;
    XMVECTOR H;
    XMVECTOR LVS, PVS, Zero;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(S1);

    LineVector = __vsubfp(LinePoint2, LinePoint1);

    Zero = __vspltisw(0);
    LVS = __vsldoi(Zero, LineVector, 2 << 2);

    ReciprocalLengthSq = __vmsum4fp(LVS, LVS);

    Reciprocal = __vrefp(ReciprocalLengthSq);

    PointVector = __vsubfp(Point, LinePoint1);

    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(ReciprocalLengthSq, Reciprocal, S0);

    PVS = __vsldoi(Zero, PointVector, 2 << 2);

    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);

    S1 = __vspltw(S1, 3);

    S1 = __vnmsubfp(ReciprocalLengthSq, S0, S1);

    ReciprocalLengthSq = __vcmpeqfp(S0, S0);

    PointProjectionScale = __vmsum4fp(PVS, LVS);

    S0 = __vmaddfp(S0, S1, S0);

    ReciprocalLengthSq = __vsel(Reciprocal, S0, ReciprocalLengthSq);

    PointProjectionScale = __vmulfp(PointProjectionScale, ReciprocalLengthSq);

    DistanceVector = __vmulfp(LineVector, PointProjectionScale);
    DistanceVector = __vsubfp(PointVector, DistanceVector);

    D = __vsldoi(Zero, DistanceVector, 2 << 2);
    H = __vspltisw(1);
    D = __vmsum4fp(D, D);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);
    Result = __vcmpeqfp(D, Zero);
    Length = __vmulfp(D, Rsq);
    Result = __vsel(Length, D, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2IntersectLine
(
    XMVECTOR Line1Point1, 
    XMVECTOR Line1Point2, 
    XMVECTOR Line2Point1, 
    XMVECTOR Line2Point2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR        V1;
    XMVECTOR        V2;
    XMVECTOR        V3;
    XMVECTOR        C1;
    XMVECTOR        C2;
    XMVECTOR        Result;
    CONST XMVECTOR  Zero = XMVectorZero();

    V1 = XMVectorSubtract(Line1Point2, Line1Point1);
    V2 = XMVectorSubtract(Line2Point2, Line2Point1);
    V3 = XMVectorSubtract(Line1Point1, Line2Point1);

    C1 = XMVector2Cross(V1, V2);
    C2 = XMVector2Cross(V2, V3);

    if (XMVector2NearEqual(C1, Zero, g_XMEpsilon))
    {
        if (XMVector2NearEqual(C2, Zero, g_XMEpsilon))
        {
            // Coincident
            Result = *(XMVECTOR*)g_XMInfinity;
        }
        else
        {
            // Parallel
            Result = *(XMVECTOR*)g_XMQNaN;
        }
    }
    else
    {
        // Intersection point = Line1Point1 + V1 * (C2 / C1)
        XMVECTOR Scale;
        Scale = XMVectorReciprocal(C1);
        Scale = XMVectorMultiply(C2, Scale);
        Result = XMVectorMultiplyAdd(V1, Scale, Line1Point1);
    }

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V1;
    XMVECTOR V2;
    XMVECTOR V3;
    XMVECTOR C1;
    XMVECTOR C2;
    XMVECTOR Scale;
    XMVECTOR Rcp;
    XMVECTOR Reciprocal;
    XMVECTOR Refine;
    XMVECTOR ControlC1;
    XMVECTOR ControlC2;
    XMVECTOR Zero;
    XMVECTOR Intersection;
    XMVECTOR Result;

    V3 = __vsubfp(Line1Point1, Line2Point1);
    V2 = __vsubfp(Line2Point2, Line2Point1);
    V1 = __vsubfp(Line1Point2, Line1Point1);

    Zero = __vspltisw(0);

    C2 = XMVector2Cross(V2, V3);
    C1 = XMVector2Cross(V1, V2);

    Reciprocal = __vrefp(C1);
    Scale = __vnmsubfp(C1, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);
    Scale = __vnmsubfp(C1, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    Scale = __vsel(Reciprocal, Rcp, Refine);

    ControlC2 = __vcmpbfp(C2, g_XMEpsilon);
    ControlC1 = __vcmpbfp(C1, g_XMEpsilon);

    Scale = __vmaddfp(C2, Scale, *(XMVECTOR*)g_XMNegativeZero);

    ControlC2 = __vcmpequw(ControlC2, Zero);
    ControlC1 = __vcmpequw(ControlC1, Zero);

    Intersection = __vmaddfp(V1, Scale, Line1Point1);

    Result = __vsel(*(XMVECTOR*)g_XMQNaN, *(XMVECTOR*)g_XMInfinity, ControlC2);
    Result = __vsel(Intersection, Result, ControlC1);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR V1;
    XMVECTOR V2;
    XMVECTOR V3;
    XMVECTOR C1;
    XMVECTOR C2;
    XMVECTOR Scale;
    XMVECTOR Rcp;
    XMVECTOR Reciprocal;
    XMVECTOR Refine;
    XMVECTOR ControlC1;
    XMVECTOR ControlC2;
    XMVECTOR Zero;
    XMVECTOR Intersection;
    XMVECTOR Result;

    XMVECTOR CrossP1, CrossP2;
    XMVECTOR CrossR1, CrossR2;
    XMVECTOR TrueInt, ShiftL, ShiftR, ShiftLI, ShiftLQ;
    XMVECTOR One, Epsilon, Infinity, QNaN;

    XMDUMMY_INITIALIZE_VECTOR(One);

    V3 = __vsubfp(Line1Point1, Line2Point1);
    V2 = __vsubfp(Line2Point2, Line2Point1);
    V1 = __vsubfp(Line1Point2, Line1Point1);

    CrossP2 = __vpermwi(V3, 0x4B);
    CrossP1 = __vpermwi(V2, 0x4B);
    CrossR2 = __vmulfp(V2, CrossP2);
    CrossR1 = __vmulfp(V1, CrossP1);
    CrossP2 = __vpermwi(CrossR2, 0x4B);
    CrossP1 = __vpermwi(CrossR1, 0x4B);
    CrossR2 = __vsubfp(CrossR2, CrossP2);
    CrossR1 = __vsubfp(CrossR1, CrossP1);
    C2 = __vspltw(CrossR2, 0);
    C1 = __vspltw(CrossR1, 0);

    Reciprocal = __vrefp(C1);
    One = __vupkd3d(One, VPACK_NORMSHORT2);
    Rcp = __vspltw(One, 3);
    Rcp = __vnmsubfp(C1, Reciprocal, Rcp);
    Rcp = __vmaddcfp(Reciprocal, Rcp, Reciprocal);
    Scale = __vspltw(One, 3);
    Scale = __vnmsubfp(C1, Rcp, Scale);
    Refine = __vcmpeqfp(Rcp, Rcp);

    Epsilon = __vspltisw(13);

    Rcp = __vmaddfp(Rcp, Scale, Rcp);

    ShiftL = __vspltisw(-6); // Shift 26
    Epsilon = __vslw(Epsilon, ShiftL);
    Zero = __vspltisw(0);

    Refine = __vsel(Reciprocal, Rcp, Refine);

    ControlC2 = __vcmpbfp(C2, Epsilon);
    ControlC1 = __vcmpbfp(C1, Epsilon);

    Intersection = __vmulfp(C2, Refine);

    TrueInt = __vspltisw(-1);
    ShiftLQ = __vspltisw(-9); // Shift 23
    ShiftLI = __vspltisw(-8); // Shift 24
    ShiftR = __vspltisw(1);

    ControlC2 = __vcmpequw(ControlC2, Zero);

    QNaN = __vslw(TrueInt, ShiftLQ);
    Infinity = __vslw(TrueInt, ShiftLI);

    Result = __vcmpequw(ControlC1, Zero);

    Intersection = __vmaddcfp(V1, Intersection, Line1Point1);

    QNaN = __vsrw(QNaN, ShiftR);
    Infinity = __vsrw(Infinity, ShiftR);

    ControlC2 = __vsel(QNaN, Infinity, ControlC2);
    Result = __vsel(Intersection, ControlC2, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2Transform
(
    XMVECTOR V, 
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;

    Y = XMVectorSplatY(V);
    X = XMVectorSplatX(V);

    Result = XMVectorMultiplyAdd(Y, M.r[1], M.r[3]);
    Result = XMVectorMultiplyAdd(X, M.r[0], Result);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR X;
    XMVECTOR Result;

    Result = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddcfp(M.r[1], Result, M.r[3]);
    Result = __vmaddfp(X, M.r[0], Result);

    return Result;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT4* XMVector2TransformStream
(
    XMFLOAT4*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT2* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    XMMATRIX        M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Y = XMVectorReplicate(((XMFLOAT2*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT2*)pInputVector)->x);

        Result = XMVectorMultiplyAdd(Y, M.r[1], M.r[3]);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat4((XMFLOAT4*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Y = XMVectorReplicate(((XMFLOAT2*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT2*)pInputVector)->x);

        Result = XMVectorMultiplyAdd(Y, M.r[1], M.r[3]);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat4((XMFLOAT4*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Result = __vspltw(V, 1);
        X = __vspltw(V, 0);

        Result = __vmaddcfp(M.r[1], Result, M.r[3]);
        Result = __vmaddfp(X, M.r[0], Result);

        XMStoreFloat4((XMFLOAT4*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2TransformCoord
(
    XMVECTOR V, 
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR InverseW;
    XMVECTOR Result;

    Y = XMVectorSplatY(V);
    X = XMVectorSplatX(V);

    Result = XMVectorMultiplyAdd(Y, M.r[1], M.r[3]);
    Result = XMVectorMultiplyAdd(X, M.r[0], Result);

    InverseW = XMVectorSplatW(Result);
    InverseW = XMVectorReciprocal(InverseW);

    Result = XMVectorMultiply(Result, InverseW);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR InverseW;
    XMVECTOR Reciprocal;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR Result;

    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddfp(Y, M.r[1], M.r[3]);
    Result = __vmaddfp(X, M.r[0], Result);

    InverseW = __vspltw(Result, 3);

    Reciprocal = __vrefp(InverseW);
    Scale = __vnmsubfp(InverseW, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);
    Scale = __vnmsubfp(InverseW, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    InverseW = __vsel(Reciprocal, Rcp, Refine);

    Result = __vmaddfp(Result, InverseW, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR X;
    XMVECTOR InverseW;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(S1);

    Result = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddcfp(M.r[1], Result, M.r[3]);
    Result = __vmaddfp(X, M.r[0], Result);

    InverseW = __vspltw(Result, 3);

    Reciprocal = __vrefp(InverseW);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(InverseW, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(InverseW, S0, S1);
    InverseW = __vcmpeqfp(S0, S0);
    S0 = __vmaddfp(S0, S1, S0);
    InverseW = __vsel(Reciprocal, S0, InverseW);

    Result = __vmulfp(Result, InverseW);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT2* XMVector2TransformCoordStream
(
    XMFLOAT2*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT2* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    XMMATRIX        M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR InverseW;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Y = XMVectorReplicate(((XMFLOAT2*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT2*)pInputVector)->x);

        Result = XMVectorMultiplyAdd(Y, M.r[1], M.r[3]);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        InverseW = XMVectorSplatW(Result);
        InverseW = XMVectorReciprocal(InverseW);

        Result = XMVectorMultiply(Result, InverseW);

        XMStoreFloat2((XMFLOAT2*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR InverseW;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Y = XMVectorReplicate(((XMFLOAT2*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT2*)pInputVector)->x);

        Result = XMVectorMultiplyAdd(Y, M.r[1], M.r[3]);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        InverseW = XMVectorSplatW(Result);
        InverseW = XMVectorReciprocal(InverseW);

        Result = XMVectorMultiply(Result, InverseW);

        XMStoreFloat2((XMFLOAT2*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR InverseW;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Result = __vspltw(V, 1);
        X = __vspltw(V, 0);

        Result = __vmaddcfp(M.r[1], Result, M.r[3]);
        Result = __vmaddfp(X, M.r[0], Result);

        InverseW = __vspltw(Result, 3);
        InverseW = XMVectorReciprocal(InverseW);

        Result = __vmulfp(Result, InverseW);

        XMStoreFloat2((XMFLOAT2*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector2TransformNormal
(
    XMVECTOR V, 
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;

    Y = XMVectorSplatY(V);
    X = XMVectorSplatX(V);

    Result = XMVectorMultiply(Y, M.r[1]);
    Result = XMVectorMultiplyAdd(X, M.r[0], Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;

    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddfp(Y, M.r[1], *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(X, M.r[0], Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;

    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmulfp(Y, M.r[1]);
    Result = __vmaddfp(X, M.r[0], Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT2* XMVector2TransformNormalStream
(
    XMFLOAT2*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT2* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    XMMATRIX        M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Y = XMVectorReplicate(((XMFLOAT2*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT2*)pInputVector)->x);

        Result = XMVectorMultiply(Y, M.r[1]);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat2((XMFLOAT2*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Y = XMVectorReplicate(((XMFLOAT2*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT2*)pInputVector)->x);

        Result = XMVectorMultiply(Y, M.r[1]);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat2((XMFLOAT2*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat2((XMFLOAT2*)pInputVector);
        Y = __vspltw(V, 1);
        X = __vspltw(V, 0);

        Result = __vmulfp(Y, M.r[1]);
        Result = __vmaddfp(X, M.r[0], Result);

        XMStoreFloat2((XMFLOAT2*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

/****************************************************************************
 *
 * 3D Vector
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparison operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3Equal
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector3EqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector3EqualR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] == V2.v[0] && 
        V1.v[1] == V2.v[1] &&
        V1.v[2] == V2.v[2])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] != V2.v[0] && 
        V1.v[1] != V2.v[1] &&
        V1.v[2] != V2.v[2])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleXYZX);
    C2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleXYZX);

    __vcmpeqfpR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V1, 0x18);
    C2 = __vpermwi(V2, 0x18);

    __vcmpeqfpR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3EqualInt
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector3EqualIntR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector3EqualIntR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.u[0] == V2.u[0] && 
        V1.u[1] == V2.u[1] &&
        V1.u[2] == V2.u[2])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.u[0] != V2.u[0] && 
        V1.u[1] != V2.u[1] &&
        V1.u[2] != V2.u[2])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleXYZX);
    C2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleXYZX);

    __vcmpequwR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V1, 0x18);
    C2 = __vpermwi(V2, 0x18);

    __vcmpequwR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3NearEqual
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR Epsilon
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Delta;

    Delta = XMVectorSubtract(V1, V2);
    Delta = XMVectorAbs(Delta);

    return (Delta.v[0] <= Epsilon.v[0] &&
            Delta.v[1] <= Epsilon.v[1] &&
            Delta.v[2] <= Epsilon.v[2]);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    Delta;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    Delta = __vsubfp(V1, V2);
    Delta = __vsel(Epsilon, Delta, *(XMVECTOR*)g_XMSelect1110);
    __vcmpbfpR(Delta, Epsilon, &CR);

    return XMComparisonAllInBounds(CR);

#else // _VMX128_INTRINSICS_

    XMVECTOR    Delta;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    Delta = __vsubfp(V1, V2);
    Delta = __vrlimi(Delta, Epsilon, 0x1, 0);
    __vcmpbfpR(Delta, Epsilon, &CR);

    return XMComparisonAllInBounds(CR);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3NotEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAnyFalse(XMVector3EqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3NotEqualInt
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAnyFalse(XMVector3EqualIntR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3Greater
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector3GreaterR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector3GreaterR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] > V2.v[0] && 
        V1.v[1] > V2.v[1] &&
        V1.v[2] > V2.v[2])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] <= V2.v[0] && 
        V1.v[1] <= V2.v[1] &&
        V1.v[2] <= V2.v[2])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleXYZX);
    C2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleXYZX);

    __vcmpgtfpR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V1, 0x18);
    C2 = __vpermwi(V2, 0x18);

    __vcmpgtfpR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3GreaterOrEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector3GreaterOrEqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector3GreaterOrEqualR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] >= V2.v[0] && 
        V1.v[1] >= V2.v[1] &&
        V1.v[2] >= V2.v[2])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] < V2.v[0] && 
        V1.v[1] < V2.v[1] &&
        V1.v[2] < V2.v[2])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleXYZX);
    C2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleXYZX);

    __vcmpgefpR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V1, 0x18);
    C2 = __vpermwi(V2, 0x18);

    __vcmpgefpR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3Less
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector3GreaterR(V2, V1));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3LessOrEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector3GreaterOrEqualR(V2, V1));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3InBounds
(
    XMVECTOR V, 
    XMVECTOR Bounds
)
{
    return XMComparisonAllInBounds(XMVector3InBoundsR(V, Bounds));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector3InBoundsR
(
    XMVECTOR V, 
    XMVECTOR Bounds
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if ((V.v[0] <= Bounds.v[0] && V.v[0] >= -Bounds.v[0]) && 
        (V.v[1] <= Bounds.v[1] && V.v[1] >= -Bounds.v[1]) &&
        (V.v[2] <= Bounds.v[2] && V.v[2] >= -Bounds.v[2]))
    {
        CR |= XM_CRMASK_CR6BOUNDS;
    }

    return CR;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vperm(V, V, *(XMVECTOR*)g_XMSwizzleXYZX);
    C2 = __vperm(Bounds, Bounds, *(XMVECTOR*)g_XMSwizzleXYZX);

    __vcmpbfpR(C1, C2, &CR);

    return CR;

#else // _VMX128_INTRINSICS_

    XMVECTOR    C1, C2;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C1 = __vpermwi(V, 0x18);
    C2 = __vpermwi(Bounds, 0x18);

    __vcmpbfpR(C1, C2, &CR);

    return CR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3IsNaN
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return (XMISNAN(V.v[0]) ||
            XMISNAN(V.v[1]) ||
            XMISNAN(V.v[2]));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);

    __vcmpeqfpR(C, C, &CR);

    return XMComparisonAnyFalse(CR);

#else // _VMX128_INTRINSICS_

    XMVECTOR    C;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C = __vpermwi(V, 0x18);
    __vcmpeqfpR(C, C, &CR);

    return XMComparisonAnyFalse(CR);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector3IsInfinite
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return (XMISINF(V.v[0]) ||
            XMISINF(V.v[1]) ||
            XMISINF(V.v[2]));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                C;
    UINT                    CR;
    static CONST XMVECTORI  Mask = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x00000000};

    XMDUMMY_INITIALIZE_UINT(CR);

    C = __vand(V, *(XMVECTOR*)Mask);
    __vcmpequwR(C, *(XMVECTOR*)g_XMInfinity, &CR);

    return XMComparisonAnyTrue(CR);

#else // _VMX128_INTRINSICS_

    XMVECTOR OneInt, TrueInt;
    XMVECTOR XYZX, VShifted, InfinityShifted;
    XMVECTOR Shift;
    UINT     CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    XYZX = __vpermwi(V, 0x18);
    OneInt = __vspltisw(1);
    TrueInt = __vspltisw(-1);
    Shift = __vspltisw(-8); // Shift 24
    VShifted = __vslw(XYZX, OneInt);
    InfinityShifted = __vslw(TrueInt, Shift);

    __vcmpequwR(VShifted, InfinityShifted, &CR);

    return XMComparisonAnyTrue(CR);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3Dot
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] =
    Result.v[1] =
    Result.v[2] =
    Result.v[3] = V1.v[0] * V2.v[0] + V1.v[1] * V2.v[1] + V1.v[2] * V2.v[2];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    // VMX2 hardware will have dedicated dot product logic, but with VMX1 it must be accomplished
    // using a multiply, 2 additions, and 2 rotations.
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vsel(*(XMVECTOR*)g_XMSelect1110, V1, *(XMVECTOR*)g_XMSelect1110);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V2, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return Result;

#else // _VMX128_INTRINSICS_

    // Overflow will result in a QNaN instead of +/- infinity.

    return __vmsum3fp(V1, V2);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3Cross
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = V1.v[1] * V2.v[2] - V1.v[2] * V2.v[1];
    Result.v[1] = V1.v[2] * V2.v[0] - V1.v[0] * V2.v[2];
    Result.v[2] = V1.v[0] * V2.v[1] - V1.v[1] * V2.v[0];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

#if 1

    XMVECTOR CrossV1A;
    XMVECTOR CrossV1B;
    XMVECTOR CrossV2A;
    XMVECTOR CrossV2B;
    XMVECTOR CrossR;
    XMVECTOR Result;

    CrossV1A = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossV2A = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV1B = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV2B = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossR = __vmaddfp(CrossV1A, CrossV2A, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vnmsubfp(CrossV1B, CrossV2B, CrossR);

    return Result;

#else

    // The following alternative may instead be used if so desired.
    // It saves a permute at the expense of increased latency.

    XMVECTOR CrossV1;
    XMVECTOR CrossV2;
    XMVECTOR CrossR;
    XMVECTOR Result;

    CrossV2 = __vperm(V2, V2, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossV1 = __vperm(V1, V1, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossR = __vmaddfp(V1, CrossV2, *(XMVECTOR*)g_XMNegativeZero);
    CrossR = __vnmsubfp(CrossV1, V2, CrossR);
    Result = __vperm(CrossR, CrossR, *(XMVECTOR*)g_XMSwizzleYZXW);

    return Result;

#endif

#else // _VMX128_INTRINSICS_

    XMVECTOR CrossV1;
    XMVECTOR CrossV2;
    XMVECTOR CrossR;

    CrossV1 = __vpermwi(V1, 0x63);
    CrossV2 = __vpermwi(V2, 0x87);
    CrossR = __vmulfp(CrossV1, CrossV2);
    CrossV1 = __vpermwi(V1, 0x87);
    CrossV2 = __vpermwi(V2, 0x63);
    CrossR = __vnmsubfp(CrossV1, CrossV2, CrossR);

    return CrossR;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3LengthSq
(
    XMVECTOR V
)
{
    return XMVector3Dot(V, V);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3ReciprocalLengthEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector3LengthSq(V);
    Result = XMVectorReciprocalSqrtEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Result = __vrsqrtefp(D0);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Result;

    Result = __vmsum3fp(V, V);
    Result = __vrsqrtefp(Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3ReciprocalLength
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector3LengthSq(V);
    Result = XMVectorReciprocalSqrt(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    Result = __vmaddfp(Rsq, RT, Rsq);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR H;

    D = __vmsum3fp(V, V);
    H = __vspltisw(1);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);

    return Rsq;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3LengthEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector3LengthSq(V);
    Result = XMVectorSqrtEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

#if 0

    // Faster, but returns a QNaN on zero and infinite vectors
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    D1 = __vrsqrtefp(D0);
    Result = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);

#else

    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Zero = __vspltisw(0);
    D0 = __vaddfp(D0, D1);
    D1 = __vrsqrtefp(D0);
    InfiniteLength = __vcmpequw(D0, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(D0, Zero);
    Result = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Result = __vsel(D0, Result, Select);

#endif

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Result;

#if 0

    // Faster, but returns a QNaN on zero and infinite vectors.

    D = __vmsum3fp(V, V);
    Rsq = __vrsqrtefp(D);
    Result = __vmulfp(D, Rsq);

#else

    // Returns a QNaN on infinite vectors.

    XMVECTOR Zero;
    XMVECTOR Length;

    D = __vmsum3fp(V, V);
    Zero = __vspltisw(0);
    Rsq = __vrsqrtefp(D);
    Result = __vcmpeqfp(D, Zero);
    Length = __vmulfp(D, Rsq);
    Result = __vsel(Length, D, Result);

#endif

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3Length
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector3LengthSq(V);
    Result = XMVectorSqrt(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Zero = __vspltisw(0);
    D0 = __vaddfp(D0, D1);
    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D1 = __vmaddfp(Rsq, RT, Rsq);
    InfiniteLength = __vcmpequw(D0, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(D0, Zero);
    Result = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Result = __vsel(D0, Result, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    // Returns a QNaN on infinite vectors.

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR Length;
    XMVECTOR H;

    H = __vspltisw(1);
    D = __vmsum3fp(V, V);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    Zero = __vspltisw(0);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);
    Result = __vcmpeqfp(D, Zero);
    Length = __vmulfp(D, Rsq);
    Result = __vsel(Length, D, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// XMVector3NormalizeEst uses a reciprocal estimate and
// returns QNaN on zero and infinite vectors.

XMFINLINE XMVECTOR XMVector3NormalizeEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;
    Result = XMVector3ReciprocalLength(V);
    Result = XMVectorMultiply(V, Result);
    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    D0 = __vrsqrtefp(D0);
    Result = __vmaddfp(V, D0, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Result;

#if 1

    // Faster, but returns a QNaN on zero and infinite vectors.

    D = __vmsum3fp(V, V);
    Rsq = __vrsqrtefp(D);
    Result = __vmulfp(V, Rsq);

#else

    // Returns a QNaN on infinite vectors.

    XMVECTOR Zero;
    XMVECTOR N;

    D = __vmsum3fp(V, V);
    Zero = __vspltisw(0);
    Rsq = __vrsqrtefp(D);
    Result = __vcmpeqfp(D, Zero);
    N = __vmulfp(V, Rsq);
    Result = __vsel(N, D, Result);

#endif

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3Normalize
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR LengthSq;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR Result;

    LengthSq = XMVector3LengthSq(V);
    Zero = XMVectorZero();
    Result = XMVectorReciprocalSqrt(LengthSq);
    InfiniteLength = XMVectorEqualInt(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = XMVectorEqual(LengthSq, Zero);
    Result = XMVectorMultiply(V, Result);
    Select = XMVectorEqualInt(InfiniteLength, ZeroLength);
    Result = XMVectorSelect(LengthSq, Result, Select);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Zero = __vspltisw(0);
    RcpLength = __vrsqrtefp(D0);
    OneHalfLengthSq = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    InfiniteLength = __vcmpequw(D0, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(D0, Zero);
    Scale = __vmaddfp(RcpLength, RcpLength, *(XMVECTOR*)g_XMNegativeZero);
    Scale = __vnmsubfp(OneHalfLengthSq, Scale, g_XMOneHalf);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    D1 = __vmaddfp(RcpLength, Scale, RcpLength);
    Result = __vmaddfp(V, D1, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vsel(D0, Result, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    // Returns a QNaN on infinite vectors.

    XMVECTOR D;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR H;
    XMVECTOR N;

    H = __vspltisw(1);
    D = __vmsum3fp(V, V);
    H = __vcfsx(H, 1);
    Zero = __vspltisw(0);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    Result = __vcmpeqfp(D, Zero);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddcfp(RcpLength, H, RcpLength);
    N = __vmulfp(V, H);
    Result = __vsel(N, D, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3ClampLength
(
    XMVECTOR V, 
    FLOAT    LengthMin, 
    FLOAT    LengthMax
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    ClampMax = XMVectorReplicate(LengthMax);
    ClampMin = XMVectorReplicate(LengthMin);

    return XMVector3ClampLengthV(V, ClampMin, ClampMax);

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    ClampMax.v[0] =
    ClampMax.v[1] = 
    ClampMax.v[2] =
    ClampMax.v[3] = LengthMax;

    ClampMin.v[0] =
    ClampMin.v[1] = 
    ClampMin.v[2] =
    ClampMin.v[3] = LengthMin;

    return XMVector3ClampLengthV(V, ClampMin, ClampMax);

#else // _VMX128_INTRINSICS_

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    XMASSERT(((UINT_PTR)&LengthMin & 3) == 0);
    XMASSERT(((UINT_PTR)&LengthMax & 3) == 0);

    ClampMax = __lvlx(&LengthMax, 0);
    ClampMin = __lvlx(&LengthMin, 0);

    ClampMax = __vspltw(ClampMax, 0);
    ClampMin = __vspltw(ClampMin, 0);

    return XMVector3ClampLengthV(V, ClampMin, ClampMax);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3ClampLengthV
(
    XMVECTOR V, 
    XMVECTOR LengthMin, 
    XMVECTOR LengthMax
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR RcpLength;
    XMVECTOR Length;
    XMVECTOR Normal;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR ControlMax;
    XMVECTOR ControlMin;
    XMVECTOR Control;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x) && (LengthMin.z == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x) && (LengthMax.z == LengthMax.x));
    XMASSERT(XMVector3GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector3GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector3GreaterOrEqual(LengthMax, LengthMin));

    LengthSq = XMVector3LengthSq(V);

    Zero = XMVectorZero();

    RcpLength = XMVectorReciprocalSqrt(LengthSq);

    InfiniteLength = XMVectorEqualInt(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = XMVectorEqual(LengthSq, Zero);

    Normal = XMVectorMultiply(V, RcpLength);

    Length = XMVectorMultiply(LengthSq, RcpLength);

    Select = XMVectorEqualInt(InfiniteLength, ZeroLength);
    Length = XMVectorSelect(LengthSq, Length, Select);
    Normal = XMVectorSelect(LengthSq, Normal, Select);

    ControlMax = XMVectorGreater(Length, LengthMax);
    ControlMin = XMVectorLess(Length, LengthMin);

    ClampLength = XMVectorSelect(Length, LengthMax, ControlMax);
    ClampLength = XMVectorSelect(ClampLength, LengthMin, ControlMin);

    Result = XMVectorMultiply(Normal, ClampLength);

    // Preserve the original vector (with no precision loss) if the length falls within the given range
    Control = XMVectorEqualInt(ControlMax, ControlMin);
    Result = XMVectorSelect(Result, V, Control);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR RcpLength;
    XMVECTOR Length;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Normal;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR ControlMax;
    XMVECTOR ControlMin;
    XMVECTOR Control;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x) && (LengthMin.z == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x) && (LengthMax.z == LengthMax.x));
    XMASSERT(XMVector3GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector3GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector3GreaterOrEqual(LengthMax, LengthMin));

    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);

    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);

    D1 = __vsldoi(D0, D0, 4);

    LengthSq = __vaddfp(D0, D1);

    Zero = __vspltisw(0);

    Rsq = __vrsqrtefp(LengthSq);
    RT = __vmaddfp(LengthSq, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    RcpLength = __vmaddfp(Rsq, RT, Rsq);

    InfiniteLength = __vcmpequw(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(LengthSq, Zero);

    Length = __vmaddfp(LengthSq, RcpLength, *(XMVECTOR*)g_XMNegativeZero);

    Normal = __vmaddfp(V, RcpLength, *(XMVECTOR*)g_XMNegativeZero);

    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Length = __vsel(LengthSq, Length, Select);
    Normal = __vsel(LengthSq, Normal, Select);

    ControlMax = __vcmpgtfp(Length, LengthMax);
    ControlMin = __vcmpgtfp(LengthMin, Length);

    ClampLength = __vsel(Length, LengthMax, ControlMax);
    ClampLength = __vsel(ClampLength, LengthMin, ControlMin);

    Result = __vmaddfp(Normal, ClampLength, *(XMVECTOR*)g_XMNegativeZero);

    Control = __vcmpequw(ControlMax, ControlMin);

    Result = __vsel(Result, V, Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR Length, LengthS;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Normal, NormalS;
    XMVECTOR Zero;
    XMVECTOR Control;
    XMVECTOR H;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x) && (LengthMin.z == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x) && (LengthMax.z == LengthMax.x));
    XMASSERT(XMVector3GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector3GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector3GreaterOrEqual(LengthMax, LengthMin));

    H = __vspltisw(1);

    LengthSq = __vmsum3fp(V, V);

    H = __vcfsx(H, 1);

    Rsq = __vrsqrtefp(LengthSq);
    RT = __vmulfp(LengthSq, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    H = __vmaddcfp(Rsq, H, Rsq);

    Zero = __vspltisw(0);

    Length = __vmulfp(LengthSq, H);

    Normal = __vmulfp(V, H);

    LengthS = __vcmpeqfp(LengthSq, Zero);
    NormalS = __vcmpeqfp(LengthSq, Zero);

    LengthS = __vsel(Length, LengthSq, LengthS);
    NormalS = __vsel(Normal, LengthSq, NormalS);

    Control = __vcmpgtfp(LengthS, LengthMax);
    ClampLength = __vcmpgtfp(LengthMin, LengthS);

    Result = __vcmpequw(Control, ClampLength);

    Control = __vsel(LengthS, LengthMax, Control);
    ClampLength = __vsel(Control, LengthMin, ClampLength);

    RT = __vmulfp(NormalS, ClampLength);

    Result = __vsel(RT, V, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3Reflect
(
    XMVECTOR Incident, 
    XMVECTOR Normal
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    // Result = Incident - (2 * dot(Incident, Normal)) * Normal
    Result = XMVector3Dot(Incident, Normal);
    Result = XMVectorAdd(Result, Result);
    Result = XMVectorNegativeMultiplySubtract(Result, Normal, Incident);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vsel(*(XMVECTOR*)g_XMSelect1110, Incident, *(XMVECTOR*)g_XMSelect1110);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, Normal, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);
    Result = __vaddfp(Result, Result);
    Result = __vnmsubfp(Result, Normal, Incident);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Result;

    D = __vmsum3fp(Incident, Normal);
    D = __vaddfp(D, D);
    Result = Incident;
    Result = __vnmsubfp(D, Normal, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3Refract
(
    XMVECTOR Incident, 
    XMVECTOR Normal, 
    FLOAT    RefractionIndex
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Index;

    Index = XMVectorReplicate(RefractionIndex);

    return XMVector3RefractV(Incident, Normal, Index);

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR Index;

    Index.v[0] =
    Index.v[1] = 
    Index.v[2] =
    Index.v[3] = RefractionIndex;

    return XMVector3RefractV(Incident, Normal, Index);

#else // _VMX128_INTRINSICS_

    XMVECTOR Index;

    XMASSERT(((UINT_PTR)&RefractionIndex & 3) == 0);

    Index = __lvlx(&RefractionIndex, 0);
    Index = __vspltw(Index, 0);

    return XMVector3RefractV(Incident, Normal, Index);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3RefractV
(
    XMVECTOR Incident, 
    XMVECTOR Normal, 
    XMVECTOR RefractionIndex
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR        IDotN;
    XMVECTOR        R;
    CONST XMVECTOR  Zero = XMVectorZero();

    // Result = RefractionIndex * Incident - Normal * (RefractionIndex * dot(Incident, Normal) + 
    // sqrt(1 - RefractionIndex * RefractionIndex * (1 - dot(Incident, Normal) * dot(Incident, Normal))))

    IDotN = XMVector3Dot(Incident, Normal);

    // R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
    R = XMVectorNegativeMultiplySubtract(IDotN, IDotN, g_XMOne);
    R = XMVectorMultiply(R, RefractionIndex);
    R = XMVectorNegativeMultiplySubtract(R, RefractionIndex, g_XMOne);

    if (XMVector4LessOrEqual(R, Zero))
    {
        // Total internal reflection
        return Zero;
    }
    else
    {
        XMVECTOR Result;

        // R = RefractionIndex * IDotN + sqrt(R)
        R = XMVectorSqrt(R);
        R = XMVectorMultiplyAdd(RefractionIndex, IDotN, R);

        // Result = RefractionIndex * Incident - Normal * R
        Result = XMVectorMultiply(RefractionIndex, Incident);
        Result = XMVectorNegativeMultiplySubtract(Normal, R, Result);

        return Result;
    }

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR IDotN;
    XMVECTOR R;
    XMVECTOR I;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR Control;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Zero;
    XMVECTOR Result;

    D0 = __vsel(*(XMVECTOR*)g_XMSelect1110, Incident, *(XMVECTOR*)g_XMSelect1110);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, Normal, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);

    Zero = __vspltisw(0);

    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);

    D1 = __vsldoi(D0, D0, 4);
    IDotN = __vaddfp(D0, D1);

    R = __vnmsubfp(IDotN, IDotN, g_XMOne);
    R = __vmaddfp(R, RefractionIndex, *(XMVECTOR*)g_XMNegativeZero);
    R = __vnmsubfp(R, RefractionIndex, g_XMOne);

    Rsq = __vrsqrtefp(R);
    RT = __vmaddfp(R, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D0 = __vmaddfp(Rsq, RT, Rsq);

    InfiniteLength = __vcmpequw(R, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(R, Zero);

    Control = __vcmpgefp(Zero, R);

    D1 = __vmaddfp(R, D0, *(XMVECTOR*)g_XMNegativeZero);

    Select = __vcmpequw(InfiniteLength, ZeroLength);

    I = __vmaddfp(RefractionIndex, Incident, *(XMVECTOR*)g_XMNegativeZero);

    R = __vsel(R, D1, Select);

    R = __vmaddfp(RefractionIndex, IDotN, R);

    Result = __vnmsubfp(Normal, R, I);

    Result = __vsel(Result, Zero, Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR IDotN;
    XMVECTOR R;
    XMVECTOR I;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR ZL;
    XMVECTOR D1;
    XMVECTOR H;
    XMVECTOR Zero;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(R);

    IDotN = __vmsum3fp(Incident, Normal);

    H = __vspltisw(1);
    R = __vupkd3d(R, VPACK_NORMSHORT2);
    H = __vcfsx(H, 1);
    RT = __vspltw(R, 3);
    R = __vspltw(R, 3);

    RT = __vnmsubfp(IDotN, IDotN, RT);
    RT = __vmulfp(RT, RefractionIndex);
    R = __vnmsubfp(RT, RefractionIndex, R);

    Rsq = __vrsqrtefp(R);
    RT = __vmulfp(R, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Zero = __vspltisw(0);
    H = __vmaddcfp(Rsq, H, Rsq);

    ZL = __vcmpeqfp(R, Zero);

    Result = __vcmpgefp(Zero, R);

    D1 = __vmulfp(R, H);

    I = __vmulfp(RefractionIndex, Incident);

    ZL = __vsel(D1, R, ZL);

    ZL = __vmaddfp(RefractionIndex, IDotN, ZL);

    I = __vnmsubfp(Normal, ZL, I);

    Result = __vsel(I, Zero, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3Orthogonal
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR NegativeV;
    XMVECTOR Z, YZYY;
    XMVECTOR ZIsNegative, YZYYIsNegative;
    XMVECTOR S, D;
    XMVECTOR R0, R1;
    XMVECTOR Select;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTORI Permute1X0X0X0X = {XM_PERMUTE_1X, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0Y0Z0Y0Y= {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0Y};

    Zero = XMVectorZero();
    Z = XMVectorSplatZ(V);
    YZYY = XMVectorPermute(V, V, *(XMVECTOR*)Permute0Y0Z0Y0Y);

    NegativeV = XMVectorSubtract(Zero, V);

    ZIsNegative = XMVectorLess(Z, Zero);
    YZYYIsNegative = XMVectorLess(YZYY, Zero);

    S = XMVectorAdd(YZYY, Z);
    D = XMVectorSubtract(YZYY, Z);

    Select = XMVectorEqualInt(ZIsNegative, YZYYIsNegative);

    R0 = XMVectorPermute(NegativeV, S, *(XMVECTOR*)Permute1X0X0X0X);
    R1 = XMVectorPermute(V, D, *(XMVECTOR*)Permute1X0X0X0X);

    Result = XMVectorSelect(R1, R0, Select);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR NegativeV;
    XMVECTOR Z, YZYY;
    XMVECTOR ZIsNegative, YZYYIsNegative;
    XMVECTOR S, D;
    XMVECTOR R0, R1;
    XMVECTOR Select;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    XMVECTOR Result;
    static CONST XMVECTORI Permute1X0X0X0X = {XM_PERMUTE_1X, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X};
    static CONST XMVECTORI Permute0Y0Z0Y0Y= {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0Y};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Zero = __vspltisw(0);
    Z = __vspltw(V, 2);
    YZYY = __vperm(V, V, *(XMVECTOR*)Permute0Y0Z0Y0Y);

    NegativeV = __vxor(V, SignMask);

    ZIsNegative = __vcmpgtfp(Zero, Z);
    YZYYIsNegative = __vcmpgtfp(Zero, YZYY);

    S = __vaddfp(YZYY, Z);
    D = __vsubfp(YZYY, Z);

    Select = __vcmpequw(ZIsNegative, YZYYIsNegative);

    R0 = __vperm(NegativeV, S, *(XMVECTOR*)Permute1X0X0X0X);
    R1 = __vperm(V, D, *(XMVECTOR*)Permute1X0X0X0X);

    Result = __vsel(R1, R0, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR NegativeV;
    XMVECTOR Z, YZYY;
    XMVECTOR ZIsNegative, YZYYIsNegative;
    XMVECTOR S, D;
    XMVECTOR R0, R1;
    XMVECTOR SignMask;
    XMVECTOR Zero;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Zero = __vspltisw(0);
    Z = __vspltw(V, 2);
    YZYY = __vpermwi(V, 0x65);

    NegativeV = __vxor(V, SignMask);

    ZIsNegative = __vcmpgtfp(Zero, Z);
    YZYYIsNegative = __vcmpgtfp(Zero, YZYY);

    S = __vaddfp(YZYY, Z);
    D = __vsubfp(YZYY, Z);
    R0 = __vspltw(NegativeV, 0);
    R1 = __vspltw(V, 0);

    Result = __vcmpequw(ZIsNegative, YZYYIsNegative);

    R0 = __vrlimi(R0, S, 0x8, 0);
    R1 = __vrlimi(R1, D, 0x8, 0);

    Result = __vsel(R1, R0, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3AngleBetweenNormalsEst
(
    XMVECTOR N1, 
    XMVECTOR N2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector3Dot(N1, N2);
    Result = XMVectorACosEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR CosAngle;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vsel(*(XMVECTOR*)g_XMSelect1110, N1, *(XMVECTOR*)g_XMSelect1110);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, N2, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    CosAngle = __vaddfp(D0, D1);

    Result = XMVectorACosEst(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR CosAngle;
    XMVECTOR Result;

    CosAngle = __vmsum3fp(N1, N2);
    Result = XMVectorACosEst(CosAngle);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3AngleBetweenNormals
(
    XMVECTOR N1, 
    XMVECTOR N2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector3Dot(N1, N2);
    Result = XMVectorACos(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR CosAngle;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vsel(*(XMVECTOR*)g_XMSelect1110, N1, *(XMVECTOR*)g_XMSelect1110);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, N2, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    CosAngle = __vaddfp(D0, D1);

    Result = XMVectorACos(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Result;

    Result = __vmsum3fp(N1, N2);
    Result = XMVectorACos(Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3AngleBetweenVectors
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR L1;
    XMVECTOR L2;
    XMVECTOR Dot;
    XMVECTOR CosAngle;
    XMVECTOR Result;

    L1 = XMVector3ReciprocalLength(V1);
    L2 = XMVector3ReciprocalLength(V2);

    Dot = XMVector3Dot(V1, V2);

    L1 = XMVectorMultiply(L1, L2);

    CosAngle = XMVectorMultiply(Dot, L1);

    Result = XMVectorACos(CosAngle);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR L1;
    XMVECTOR L2;
    XMVECTOR Dot;
    XMVECTOR CosAngle;
    XMVECTOR L1A, L1B, L2A, L2B, DotA, DotB;
    XMVECTOR Rsq1, Rsq2, Rcp1, Rcp2, RT1, RT2;
    XMVECTOR Result;

    L1B = __vsel(*(XMVECTOR*)g_XMSelect1110, V1, *(XMVECTOR*)g_XMSelect1110);
    L2B = __vsel(*(XMVECTOR*)g_XMSelect1110, V2, *(XMVECTOR*)g_XMSelect1110);

    L1A = __vmaddfp(L1B, L1B, *(XMVECTOR*)g_XMNegativeZero);
    L2A = __vmaddfp(L2B, L2B, *(XMVECTOR*)g_XMNegativeZero);
    DotA = __vmaddfp(L1B, L2B, *(XMVECTOR*)g_XMNegativeZero);

    L1B = __vsldoi(L1A, L1A, 8);
    L2B = __vsldoi(L2A, L2A, 8);
    DotB = __vsldoi(DotA, DotA, 8);

    L1A = __vaddfp(L1A, L1B);
    L2A = __vaddfp(L2A, L2B);
    DotA = __vaddfp(DotA, DotB);

    L1B = __vsldoi(L1A, L1A, 4);
    L2B = __vsldoi(L2A, L2A, 4);
    DotB = __vsldoi(DotA, DotA, 4);

    L1A = __vaddfp(L1A, L1B);
    L2A = __vaddfp(L2A, L2B);
    Dot = __vaddfp(DotA, DotB);

    Rsq1 = __vrsqrtefp(L1A);
    Rsq2 = __vrsqrtefp(L2A);
    RT1 = __vmaddfp(L1A, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    RT2 = __vmaddfp(L2A, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp1 = __vmaddfp(Rsq1, Rsq1, *(XMVECTOR*)g_XMNegativeZero);
    Rcp2 = __vmaddfp(Rsq2, Rsq2, *(XMVECTOR*)g_XMNegativeZero);
    RT1 = __vnmsubfp(RT1, Rcp1, g_XMOneHalf);
    RT2 = __vnmsubfp(RT2, Rcp2, g_XMOneHalf);
    L1 = __vmaddfp(Rsq1, RT1, Rsq1);
    L2 = __vmaddfp(Rsq2, RT2, Rsq2);

    L1 = __vmaddfp(L1, L2, *(XMVECTOR*)g_XMNegativeZero);

    CosAngle = __vmaddfp(Dot, L1, *(XMVECTOR*)g_XMNegativeZero);

    Result = XMVectorACos(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D1, D2;
    XMVECTOR Rsq1, Rsq2;
    XMVECTOR Rcp1, Rcp2;
    XMVECTOR RT1, RT2;
    XMVECTOR H1, H2;
    XMVECTOR L;
    XMVECTOR Dot;
    XMVECTOR CosAngle;
    XMVECTOR Result;

    D1 = __vmsum3fp(V1, V1);
    D2 = __vmsum3fp(V2, V2);
    H2 = __vspltisw(1);
    H1 = __vcfsx(H2, 1);
    H2 = __vcfsx(H2, 1);
    Rsq1 = __vrsqrtefp(D1);
    Rsq2 = __vrsqrtefp(D2);
    RT1 = __vmulfp(D1, H1);
    RT2 = __vmulfp(D2, H2);
    Rcp1 = __vmulfp(Rsq1, Rsq1);
    Rcp2 = __vmulfp(Rsq2, Rsq2);
    H1 = __vnmsubfp(RT1, Rcp1, H1);
    H2 = __vnmsubfp(RT2, Rcp2, H2);
    Rsq1 = __vmaddfp(Rsq1, H1, Rsq1);
    Rsq2 = __vmaddfp(Rsq2, H2, Rsq2);
    Dot = __vmsum3fp(V1, V2);
    L = __vmulfp(Rsq1, Rsq2);
    CosAngle = __vmulfp(Dot, L);

    Result = XMVectorACos(CosAngle);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3LinePointDistance
(
    XMVECTOR LinePoint1, 
    XMVECTOR LinePoint2, 
    XMVECTOR Point
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR PointVector;
    XMVECTOR LineVector;
    XMVECTOR ReciprocalLengthSq;
    XMVECTOR PointProjectionScale;
    XMVECTOR DistanceVector;
    XMVECTOR Result;

    // Given a vector PointVector from LinePoint1 to Point and a vector
    // LineVector from LinePoint1 to LinePoint2, the scaled distance 
    // PointProjectionScale from LinePoint1 to the perpendicular projection
    // of PointVector onto the line is defined as:
    //
    //     PointProjectionScale = dot(PointVector, LineVector) / LengthSq(LineVector)

    PointVector = XMVectorSubtract(Point, LinePoint1);
    LineVector = XMVectorSubtract(LinePoint2, LinePoint1);

    ReciprocalLengthSq = XMVector3LengthSq(LineVector);
    ReciprocalLengthSq = XMVectorReciprocal(ReciprocalLengthSq);

    PointProjectionScale = XMVector3Dot(PointVector, LineVector);
    PointProjectionScale = XMVectorMultiply(PointProjectionScale, ReciprocalLengthSq);

    DistanceVector = XMVectorMultiply(LineVector, PointProjectionScale);
    DistanceVector = XMVectorSubtract(PointVector, DistanceVector);

    Result = XMVector3Length(DistanceVector);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR PointVector;
    XMVECTOR LineVector;
    XMVECTOR ReciprocalLength;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Scale;
    XMVECTOR D0, D1, D2, D3;
    XMVECTOR CrossV1A;
    XMVECTOR CrossV1B;
    XMVECTOR CrossV2A;
    XMVECTOR CrossV2B;
    XMVECTOR CrossR;
    XMVECTOR Zero, D2EqualsInfinity, D2EqualsZero, Select;
    XMVECTOR Result;

    LineVector = __vsubfp(LinePoint2, LinePoint1);
    PointVector = __vsubfp(Point, LinePoint1);

    CrossV1A = __vperm(LineVector, LineVector, *(XMVECTOR*)g_XMSwizzleYZXW);
    CrossV2A = __vperm(PointVector, PointVector, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV1B = __vperm(LineVector, LineVector, *(XMVECTOR*)g_XMSwizzleZXYW);
    CrossV2B = __vperm(PointVector, PointVector, *(XMVECTOR*)g_XMSwizzleYZXW);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, LineVector, *(XMVECTOR*)g_XMSelect1110);

    CrossR = __vmaddfp(CrossV1A, CrossV2A, *(XMVECTOR*)g_XMNegativeZero);
    D0 = __vmaddfp(D1, D1, *(XMVECTOR*)g_XMNegativeZero);

    Scale = __vnmsubfp(CrossV1B, CrossV2B, CrossR);

    D1 = __vsldoi(D0, D0, 8);
    Scale = __vsel(*(XMVECTOR*)g_XMSelect1110, Scale, *(XMVECTOR*)g_XMSelect1110);

    D0 = __vaddfp(D0, D1);
    D2 = __vmaddfp(Scale, Scale, *(XMVECTOR*)g_XMNegativeZero);

    D1 = __vsldoi(D0, D0, 4);
    D3 = __vsldoi(D2, D2, 8);

    D0 = __vaddfp(D0, D1);
    D2 = __vaddfp(D2, D3);

    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    ReciprocalLength = __vmaddfp(Rsq, RT, Rsq);

    D3 = __vsldoi(D2, D2, 4);
    D2 = __vaddfp(D2, D3);

    Rsq = __vrsqrtefp(D2);
    RT = __vmaddfp(D2, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    Zero = __vspltisw(0);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D2EqualsInfinity = __vcmpequw(D2, *(XMVECTOR*)g_XMInfinity);
    D2EqualsZero = __vcmpeqfp(D2, Zero);
    D3 = __vmaddfp(Rsq, RT, Rsq);
    Select = __vcmpequw(D2EqualsInfinity, D2EqualsZero);

    Scale = __vmaddfp(D2, D3, *(XMVECTOR*)g_XMNegativeZero);
    Scale = __vsel(D2, Scale, Select);

    Result = __vmaddfp(Scale, ReciprocalLength, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR PointVector;
    XMVECTOR LineVector;
    XMVECTOR ReciprocalLengthSq;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR PointProjectionScale;
    XMVECTOR DistanceVector;
    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Zero;
    XMVECTOR Length;
    XMVECTOR H;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(S1);

    LineVector = __vsubfp(LinePoint2, LinePoint1);

    ReciprocalLengthSq = __vmsum3fp(LineVector, LineVector);

    Reciprocal = __vrefp(ReciprocalLengthSq);

    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(ReciprocalLengthSq, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);

    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(ReciprocalLengthSq, S0, S1);

    PointVector = __vsubfp(Point, LinePoint1);

    ReciprocalLengthSq = __vcmpeqfp(S0, S0);

    S0 = __vmaddfp(S0, S1, S0);

    PointProjectionScale = __vmsum3fp(PointVector, LineVector);

    ReciprocalLengthSq = __vsel(Reciprocal, S0, ReciprocalLengthSq);

    PointProjectionScale = __vmulfp(PointProjectionScale, ReciprocalLengthSq);

    DistanceVector = __vmulfp(LineVector, PointProjectionScale);
    DistanceVector = __vsubfp(PointVector, DistanceVector);

    H = __vspltisw(1);
    D = __vmsum3fp(DistanceVector, DistanceVector);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    Zero = __vspltisw(0);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);
    Result = __vcmpeqfp(D, Zero);
    Length = __vmulfp(D, Rsq);
    Result = __vsel(Length, D, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMVector3ComponentsFromNormal
(
    XMVECTOR* pParallel, 
    XMVECTOR* pPerpendicular, 
    XMVECTOR  V, 
    XMVECTOR  Normal
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Parallel;
    XMVECTOR Scale;

    XMASSERT(pParallel);
    XMASSERT(pPerpendicular);

    Scale = XMVector3Dot(V, Normal);

    Parallel = XMVectorMultiply(Normal, Scale);

    *pParallel = Parallel;
    *pPerpendicular = XMVectorSubtract(V, Parallel);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Parallel;
    XMVECTOR Scale;
    XMVECTOR D0;
    XMVECTOR D1;

    XMASSERT(pParallel);
    XMASSERT(pPerpendicular);

    D0 = __vsel(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    D1 = __vsel(*(XMVECTOR*)g_XMSelect1110, Normal, *(XMVECTOR*)g_XMSelect1110);
    D0 = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Scale = __vaddfp(D0, D1);

    Parallel = __vmaddfp(Normal, Scale, *(XMVECTOR*)g_XMNegativeZero);

    *pParallel = Parallel;
    *pPerpendicular = __vsubfp(V, Parallel);

#else // _VMX128_INTRINSICS_

    XMVECTOR Parallel;
    XMVECTOR Scale;

    XMASSERT(pParallel);
    XMASSERT(pPerpendicular);

    Scale = __vmsum3fp(V, Normal);

    Parallel = __vmulfp(Normal, Scale);

    *pParallel = Parallel;
    *pPerpendicular = __vsubfp(V, Parallel);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Transform a vector using a rotation expressed as a unit quaternion

XMFINLINE XMVECTOR XMVector3Rotate
(
    XMVECTOR V, 
    XMVECTOR RotationQuaternion
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR A;
    XMVECTOR Q;
    XMVECTOR Result;

    A = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    Q = XMQuaternionConjugate(RotationQuaternion);
    Result = XMQuaternionMultiply(Q, A);
    Result = XMQuaternionMultiply(Result, RotationQuaternion);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                VX, VY, VZ;
    XMVECTOR                Q, NegativeQ;
    XMVECTOR                QV, NegativeQV;
    XMVECTOR                RQX, RQY, RQZ, RQW;
    XMVECTOR                QWZYX;
    XMVECTOR                QZWXY;
    XMVECTOR                QYXWZ;
    XMVECTOR                SignMask;
    XMVECTOR                Result;
    static CONST XMVECTORI  ControlWZYX = {XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static CONST XMVECTORI  ControlZWXY = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_1Y};
    static CONST XMVECTORI  ControlYXWZ = {XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Q = __vxor(RotationQuaternion, SignMask);

    VX = __vspltw(V, 0);
    VY = __vspltw(V, 1);
    VZ = __vspltw(V, 2);

    Q = __vsel(RotationQuaternion, Q, *(XMVECTOR*)g_XMSelect1110);
    NegativeQ = __vsel(Q, RotationQuaternion, *(XMVECTOR*)g_XMSelect1110);

    QWZYX = __vperm(Q, NegativeQ, *(XMVECTOR*)ControlWZYX);
    QZWXY = __vperm(Q, NegativeQ, *(XMVECTOR*)ControlZWXY);
    QYXWZ = __vperm(Q, NegativeQ, *(XMVECTOR*)ControlYXWZ);

    QV = __vmaddfp(QWZYX, VX, XMVectorZero());
    QV = __vmaddfp(QZWXY, VY, QV);
    QV = __vmaddfp(QYXWZ, VZ, QV);

    RQW = __vspltw(RotationQuaternion, 3);
    RQX = __vspltw(RotationQuaternion, 0);
    RQY = __vspltw(RotationQuaternion, 1);
    RQZ = __vspltw(RotationQuaternion, 2);

    NegativeQV = __vxor(QV, SignMask);

    Result = __vmaddfp(QV, RQW, *(XMVECTOR*)g_XMNegativeZero);

    QWZYX = __vperm(QV, NegativeQV, *(XMVECTOR*)ControlWZYX);
    QZWXY = __vperm(QV, NegativeQV, *(XMVECTOR*)ControlZWXY);
    QYXWZ = __vperm(QV, NegativeQV, *(XMVECTOR*)ControlYXWZ);

    Result = __vmaddfp(QWZYX, RQX, Result);
    Result = __vmaddfp(QZWXY, RQY, Result);
    Result = __vmaddfp(QYXWZ, RQZ, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Q, A, QA;
    XMVECTOR Q10, Q11, Q12, Q13;
    XMVECTOR Q2WZYX, Q2ZWXY, Q2YXWZ;
    XMVECTOR RX, RY, RZ, RW, RXZ, RYW, Result;
    XMVECTOR SignMask;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);
    A = __vspltisw(0);

    Q = __vxor(RotationQuaternion, SignMask);
    A = __vrlimi(A, V, 0xE, 0);
    Q = __vrlimi(Q, RotationQuaternion, 0x1, 0);

    Q2ZWXY = __vpermwi(A, 0xB1);
    Q2YXWZ = __vpermwi(A, 0x4E);
    Q2WZYX = __vpermwi(A, 0xE4);

    Q13 = __vxor(Q, SignMask);
    Q11 = __vxor(Q, SignMask);
    Q12 = __vxor(Q, SignMask);
    Q10 = __vxor(Q, SignMask);

    Q13 = __vrlimi(Q13, Q, 0x1, 0);
    Q11 = __vrlimi(Q11, Q, 0xD, 0);
    Q12 = __vrlimi(Q12, Q, 0x7, 0);
    Q10 = __vrlimi(Q10, Q, 0xB, 0);

    RW = __vmsum4fp(Q13, A);
    RY = __vmsum4fp(Q11, Q2ZWXY);
    RZ = __vmsum4fp(Q12, Q2YXWZ);
    RX = __vmsum4fp(Q10, Q2WZYX);

    RYW = __vmrghw(RY, RW);
    RXZ = __vmrghw(RX, RZ);
    QA = __vmrghw(RXZ, RYW);

    Q2ZWXY = __vpermwi(RotationQuaternion, 0xB1);
    Q2YXWZ = __vpermwi(RotationQuaternion, 0x4E);
    Q2WZYX = __vpermwi(RotationQuaternion, 0xE4);

    Q13 = __vxor(QA, SignMask);
    Q11 = __vxor(QA, SignMask);
    Q12 = __vxor(QA, SignMask);
    Q10 = __vxor(QA, SignMask);

    Q13 = __vrlimi(Q13, QA, 0x1, 0);
    Q11 = __vrlimi(Q11, QA, 0xD, 0);
    Q12 = __vrlimi(Q12, QA, 0x7, 0);
    Q10 = __vrlimi(Q10, QA, 0xB, 0);

    RW = __vmsum4fp(Q13, RotationQuaternion);
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
// Transform a vector using the inverse of a rotation expressed as a unit quaternion

XMFINLINE XMVECTOR XMVector3InverseRotate
(
    XMVECTOR V, 
    XMVECTOR RotationQuaternion
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR A;
    XMVECTOR Q;
    XMVECTOR Result;

    A = XMVectorSelect(*(XMVECTOR*)g_XMSelect1110, V, *(XMVECTOR*)g_XMSelect1110);
    Result = XMQuaternionMultiply(RotationQuaternion, A);
    Q = XMQuaternionConjugate(RotationQuaternion);
    Result = XMQuaternionMultiply(Result, Q);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                QV, NegativeQV;
    XMVECTOR                NegativeRQ;
    XMVECTOR                X, Y, Z, W;
    XMVECTOR                RQWZYX;
    XMVECTOR                RQZWXY;
    XMVECTOR                RQYXWZ;
    XMVECTOR                SignMask;
    XMVECTOR                Result;
    static CONST XMVECTORI  ControlWZYX = {XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_1X};
    static CONST XMVECTORI  ControlZWXY = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_1Y};
    static CONST XMVECTORI  ControlYXWZ = {XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_1Z};

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    NegativeRQ = __vxor(RotationQuaternion, SignMask);

    X = __vspltw(V, 0);
    Y = __vspltw(V, 1);
    Z = __vspltw(V, 2);

    RQWZYX = __vperm(RotationQuaternion, NegativeRQ, *(XMVECTOR*)ControlWZYX);
    RQZWXY = __vperm(RotationQuaternion, NegativeRQ, *(XMVECTOR*)ControlZWXY);
    RQYXWZ = __vperm(RotationQuaternion, NegativeRQ, *(XMVECTOR*)ControlYXWZ);

    W = __vspltw(RotationQuaternion, 3);

    QV = __vmaddfp(RQWZYX, X, XMVectorZero());
    QV = __vmaddfp(RQZWXY, Y, QV);
    QV = __vmaddfp(RQYXWZ, Z, QV);

    NegativeQV = __vxor(QV, SignMask);

    Result = __vmaddfp(QV, W, *(XMVECTOR*)g_XMNegativeZero);

    X = __vspltw(NegativeRQ, 0);
    Y = __vspltw(NegativeRQ, 1);
    Z = __vspltw(NegativeRQ, 2);

    RQWZYX = __vperm(QV, NegativeQV, *(XMVECTOR*)ControlWZYX);
    RQZWXY = __vperm(QV, NegativeQV, *(XMVECTOR*)ControlZWXY);
    RQYXWZ = __vperm(QV, NegativeQV, *(XMVECTOR*)ControlYXWZ);

    Result = __vmaddfp(RQWZYX, X, Result);
    Result = __vmaddfp(RQZWXY, Y, Result);
    Result = __vmaddfp(RQYXWZ, Z, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Q10, Q11, Q12, Q13;
    XMVECTOR Q2WZYX, Q2ZWXY, Q2YXWZ;
    XMVECTOR RX, RY, RZ, RW, RXZ, RYW, Result;
    XMVECTOR SignMask;
    XMVECTOR A, Q, QA;

    A = __vspltisw(0);
    A = __vrlimi(A, V, 0xE, 0);

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    Q2ZWXY = __vpermwi(A, 0xB1);
    Q2YXWZ = __vpermwi(A, 0x4E);
    Q2WZYX = __vpermwi(A, 0xE4);

    Q13 = __vxor(RotationQuaternion, SignMask);
    Q11 = __vxor(RotationQuaternion, SignMask);
    Q12 = __vxor(RotationQuaternion, SignMask);
    Q10 = __vxor(RotationQuaternion, SignMask);

    Q13 = __vrlimi(Q13, RotationQuaternion, 0x1, 0);
    Q11 = __vrlimi(Q11, RotationQuaternion, 0xD, 0);
    Q12 = __vrlimi(Q12, RotationQuaternion, 0x7, 0);
    Q10 = __vrlimi(Q10, RotationQuaternion, 0xB, 0);

    RW = __vmsum4fp(Q13, A);
    RY = __vmsum4fp(Q11, Q2ZWXY);
    RZ = __vmsum4fp(Q12, Q2YXWZ);
    RX = __vmsum4fp(Q10, Q2WZYX);

    Q = __vxor(RotationQuaternion, SignMask);
    RYW = __vmrghw(RY, RW);
    RXZ = __vmrghw(RX, RZ);
    Q = __vrlimi(Q, RotationQuaternion, 0x1, 0);
    QA = __vmrghw(RXZ, RYW);

    Q2ZWXY = __vpermwi(Q, 0xB1);
    Q2YXWZ = __vpermwi(Q, 0x4E);
    Q2WZYX = __vpermwi(Q, 0xE4);

    Q13 = __vxor(QA, SignMask);
    Q11 = __vxor(QA, SignMask);
    Q12 = __vxor(QA, SignMask);
    Q10 = __vxor(QA, SignMask);

    Q13 = __vrlimi(Q13, QA, 0x1, 0);
    Q11 = __vrlimi(Q11, QA, 0xD, 0);
    Q12 = __vrlimi(Q12, QA, 0x7, 0);
    Q10 = __vrlimi(Q10, QA, 0xB, 0);

    RW = __vmsum4fp(Q13, Q);
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

XMFINLINE XMVECTOR XMVector3Transform
(
    XMVECTOR V, 
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR Result;

    Z = XMVectorSplatZ(V);
    Y = XMVectorSplatY(V);
    X = XMVectorSplatX(V);

    Result = XMVectorMultiplyAdd(Z, M.r[2], M.r[3]);
    Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
    Result = XMVectorMultiplyAdd(X, M.r[0], Result);

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Result;

    Result = __vspltw(V, 2);
    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddcfp(M.r[2], Result, M.r[3]);
    Result = __vmaddfp(Y, M.r[1], Result);
    Result = __vmaddfp(X, M.r[0], Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT4* XMVector3TransformStream
(
    XMFLOAT4*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT3* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    XMMATRIX        M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);
        Z = XMVectorSplatZ(V);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Z = XMVectorReplicate(((XMFLOAT3*)pInputVector)->z);
//        Y = XMVectorReplicate(((XMFLOAT3*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT3*)pInputVector)->x);

        Result = XMVectorMultiplyAdd(Z, M.r[2], M.r[3]);
        Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat4((XMFLOAT4*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);
        Z = XMVectorSplatZ(V);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Z = XMVectorReplicate(((XMFLOAT3*)pInputVector)->z);
//        Y = XMVectorReplicate(((XMFLOAT3*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT3*)pInputVector)->x);

        Result = XMVectorMultiplyAdd(Z, M.r[2], M.r[3]);
        Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat4((XMFLOAT4*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMMATRIX P, MT;
    XMVECTOR R0, R1, R2, R3, R0R2, R1R3;
    XMVECTOR I, V;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    XMDUMMY_INITIALIZE_VECTOR(V);

    V = __vupkd3d(V, VPACK_NORMSHORT2);

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    for (i = 0; i < VectorCount; i++)
    {
        I = XMLoadFloat3((XMFLOAT3*)pInputVector);
        V = __vrlimi(V, I, 0xE, 0);

        R0 = __vmsum4fp(V, MT.r[0]);
        R1 = __vmsum4fp(V, MT.r[1]);
        R2 = __vmsum4fp(V, MT.r[2]);
        R3 = __vmsum4fp(V, MT.r[3]);

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

XMFINLINE XMVECTOR XMVector3TransformCoord
(
    XMVECTOR V, 
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR InverseW;
    XMVECTOR Result;

    Z = XMVectorSplatZ(V);
    Y = XMVectorSplatY(V);
    X = XMVectorSplatX(V);

    Result = XMVectorMultiplyAdd(Z, M.r[2], M.r[3]);
    Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
    Result = XMVectorMultiplyAdd(X, M.r[0], Result);

    InverseW = XMVectorSplatW(Result);
    InverseW = XMVectorReciprocal(InverseW);

    Result = XMVectorMultiply(Result, InverseW);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR InverseW;
    XMVECTOR Reciprocal;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Refine;
    XMVECTOR Result;

    Z = __vspltw(V, 2);
    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddfp(Z, M.r[2], M.r[3]);
    Result = __vmaddfp(Y, M.r[1], Result);
    Result = __vmaddfp(X, M.r[0], Result);

    InverseW = __vspltw(Result, 3);

    Reciprocal = __vrefp(InverseW);
    Scale = __vnmsubfp(InverseW, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, Scale, Reciprocal);
    Scale = __vnmsubfp(InverseW, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, Scale, Rcp);
    InverseW = __vsel(Reciprocal, Rcp, Refine);

    Result = __vmaddfp(Result, InverseW, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR InverseW;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(S1);

    Result = __vspltw(V, 2);
    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddcfp(M.r[2], Result, M.r[3]);
    Result = __vmaddfp(Y, M.r[1], Result);
    Result = __vmaddfp(X, M.r[0], Result);

    InverseW = __vspltw(Result, 3);

    Reciprocal = __vrefp(InverseW);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(InverseW, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(InverseW, S0, S1);
    InverseW = __vcmpeqfp(S0, S0);
    S0 = __vmaddfp(S0, S1, S0);
    InverseW = __vsel(Reciprocal, S0, InverseW);

    Result = __vmulfp(Result, InverseW);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT3* XMVector3TransformCoordStream
(
    XMFLOAT3*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT3* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    XMMATRIX        M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR InverseW;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);
        Z = XMVectorSplatZ(V);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Z = XMVectorReplicate(((XMFLOAT3*)pInputVector)->z);
//        Y = XMVectorReplicate(((XMFLOAT3*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT3*)pInputVector)->x);

        Result = XMVectorMultiplyAdd(Z, M.r[2], M.r[3]);
        Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        InverseW = XMVectorSplatW(Result);
        InverseW = XMVectorReciprocal(InverseW);

        Result = XMVectorMultiply(Result, InverseW);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR InverseW;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);
        Z = XMVectorSplatZ(V);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Z = XMVectorReplicate(((XMFLOAT3*)pInputVector)->z);
//        Y = XMVectorReplicate(((XMFLOAT3*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT3*)pInputVector)->x);

        Result = XMVectorMultiplyAdd(Z, M.r[2], M.r[3]);
        Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        InverseW = XMVectorSplatW(Result);
        InverseW = XMVectorReciprocal(InverseW);

        Result = XMVectorMultiply(Result, InverseW);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMMATRIX P, MT;
    XMVECTOR R0, R1, R2, R3, R0R2;
    XMVECTOR I, V;
    XMVECTOR InverseW;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    XMDUMMY_INITIALIZE_VECTOR(V);

    V = __vupkd3d(V, VPACK_NORMSHORT2);

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    for (i = 0; i < VectorCount; i++)
    {
        I = XMLoadFloat3((XMFLOAT3*)pInputVector);
        V = __vrlimi(V, I, 0xE, 0);

        R0 = __vmsum4fp(V, MT.r[0]);
        R1 = __vmsum4fp(V, MT.r[1]);
        R2 = __vmsum4fp(V, MT.r[2]);
        R3 = __vmsum4fp(V, MT.r[3]);

        R0R2 = __vmrghw(R0, R2);

        Result = __vmrghw(R0R2, R1);

        InverseW = XMVectorReciprocal(R3);

        Result = __vmulfp(Result, InverseW);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3TransformNormal
(
    XMVECTOR V, 
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR Result;

    Z = XMVectorSplatZ(V);
    Y = XMVectorSplatY(V);
    X = XMVectorSplatX(V);

    Result = XMVectorMultiply(Z, M.r[2]);
    Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
    Result = XMVectorMultiplyAdd(X, M.r[0], Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR Result;

    Z = __vspltw(V, 2);
    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddfp(Z, M.r[2], *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(Y, M.r[1], Result);
    Result = __vmaddfp(X, M.r[0], Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR Result;

    Z = __vspltw(V, 2);
    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmulfp(Z, M.r[2]);
    Result = __vmaddfp(Y, M.r[1], Result);
    Result = __vmaddfp(X, M.r[0], Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT3* XMVector3TransformNormalStream
(
    XMFLOAT3*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT3* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    XMMATRIX        M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);
        Z = XMVectorSplatZ(V);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Z = XMVectorReplicate(((XMFLOAT3*)pInputVector)->z);
//        Y = XMVectorReplicate(((XMFLOAT3*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT3*)pInputVector)->x);

        Result = XMVectorMultiply(Z, M.r[2]);
        Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);
        Z = XMVectorSplatZ(V);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
//        Z = XMVectorReplicate(((XMFLOAT3*)pInputVector)->z);
//        Y = XMVectorReplicate(((XMFLOAT3*)pInputVector)->y);
//        X = XMVectorReplicate(((XMFLOAT3*)pInputVector)->x);

        Result = XMVectorMultiply(Z, M.r[2]);
        Result = XMVectorMultiplyAdd(Y, M.r[1], Result);
        Result = XMVectorMultiplyAdd(X, M.r[0], Result);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMMATRIX P, MT;
    XMVECTOR R0, R1, R2, R0R2;
    XMVECTOR V;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);

        R0 = __vmsum3fp(V, MT.r[0]);
        R1 = __vmsum3fp(V, MT.r[1]);
        R2 = __vmsum3fp(V, MT.r[2]);

        R0R2 = __vmrghw(R0, R2);

        Result = __vmrghw(R0R2, R1);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMVector3Project
(
    XMVECTOR V, 
    FLOAT    ViewportX, 
    FLOAT    ViewportY, 
    FLOAT    ViewportWidth, 
    FLOAT    ViewportHeight, 
    FLOAT    ViewportMinZ, 
    FLOAT    ViewportMaxZ, 
    XMMATRIX Projection, 
    XMMATRIX View, 
    XMMATRIX World
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX Transform;
    XMVECTOR Scale;
    XMVECTOR Offset;
    XMVECTOR Result;
    FLOAT    HalfViewportWidth = ViewportWidth * 0.5f;
    FLOAT    HalfViewportHeight = ViewportHeight * 0.5f;

    Scale = XMVectorSet(HalfViewportWidth, 
                        -HalfViewportHeight,
                        ViewportMaxZ - ViewportMinZ,
                        0.0f);

    Offset = XMVectorSet(ViewportX + HalfViewportWidth,
                        ViewportY + HalfViewportHeight,
                        ViewportMinZ,
                        0.0f);

    Transform = XMMatrixMultiply(World, View);
    Transform = XMMatrixMultiply(Transform, Projection);

    Result = XMVector3TransformCoord(V, Transform);

    Result = XMVectorMultiplyAdd(Result, Scale, Offset);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Transform0, Transform1, Transform2, Transform3;
    XMVECTOR WorldView0, WorldView1, WorldView2, WorldView3;
    XMVECTOR M1S0, M1S1, M1S2, M1S3;
    XMVECTOR Reciprocal;
    XMVECTOR Refine;
    XMVECTOR S;
    XMVECTOR Rcp;
    XMVECTOR Scale;
    XMVECTOR Offset;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR InverseW;
    XMVECTOR Result;
    FLOAT    HalfViewportWidth;
    FLOAT    HalfViewportHeight;

//    WorldView = XMMatrixMultiply(World, View);

    M1S0 = __vspltw(World.r[0], 0);
    M1S1 = __vspltw(World.r[1], 0);
    M1S2 = __vspltw(World.r[2], 0);
    M1S3 = __vspltw(World.r[3], 0);

    WorldView0 = __vmaddfp(M1S0, View.r[0], *(XMVECTOR*)g_XMNegativeZero);
    WorldView1 = __vmaddfp(M1S1, View.r[0], *(XMVECTOR*)g_XMNegativeZero);
    WorldView2 = __vmaddfp(M1S2, View.r[0], *(XMVECTOR*)g_XMNegativeZero);
    WorldView3 = __vmaddfp(M1S3, View.r[0], *(XMVECTOR*)g_XMNegativeZero);

    M1S0 = __vspltw(World.r[0], 1);
    M1S1 = __vspltw(World.r[1], 1);
    M1S2 = __vspltw(World.r[2], 1);
    M1S3 = __vspltw(World.r[3], 1);

    WorldView0 = __vmaddfp(M1S0, View.r[1], WorldView0);
    WorldView1 = __vmaddfp(M1S1, View.r[1], WorldView1);
    WorldView2 = __vmaddfp(M1S2, View.r[1], WorldView2);
    WorldView3 = __vmaddfp(M1S3, View.r[1], WorldView3);

    M1S0 = __vspltw(World.r[0], 2);
    M1S1 = __vspltw(World.r[1], 2);
    M1S2 = __vspltw(World.r[2], 2);
    M1S3 = __vspltw(World.r[3], 2);

    WorldView0 = __vmaddfp(M1S0, View.r[2], WorldView0);
    WorldView1 = __vmaddfp(M1S1, View.r[2], WorldView1);
    WorldView2 = __vmaddfp(M1S2, View.r[2], WorldView2);
    WorldView3 = __vmaddfp(M1S3, View.r[2], WorldView3);

    M1S0 = __vspltw(World.r[0], 3);
    M1S1 = __vspltw(World.r[1], 3);
    M1S2 = __vspltw(World.r[2], 3);
    M1S3 = __vspltw(World.r[3], 3);

    WorldView0 = __vmaddfp(M1S0, View.r[3], WorldView0);
    WorldView1 = __vmaddfp(M1S1, View.r[3], WorldView1);
    WorldView2 = __vmaddfp(M1S2, View.r[3], WorldView2);
    WorldView3 = __vmaddfp(M1S3, View.r[3], WorldView3);

    HalfViewportWidth = ViewportWidth * 0.5f;
    HalfViewportHeight = ViewportHeight * 0.5f;

    Scale.v[0] = HalfViewportWidth;
    Scale.v[1] = -HalfViewportHeight;
    Scale.v[2] = ViewportMaxZ - ViewportMinZ;
    Scale.v[3] = 0.0f;

    Offset.v[0] = ViewportX + HalfViewportWidth;
    Offset.v[1] = ViewportY + HalfViewportHeight;
    Offset.v[2] = ViewportMinZ;
    Offset.v[3] = 0.0f;

//    Transform = XMMatrixMultiply(WorldView, Projection);

    M1S0 = __vspltw(WorldView0, 0);
    M1S1 = __vspltw(WorldView1, 0);
    M1S2 = __vspltw(WorldView2, 0);
    M1S3 = __vspltw(WorldView3, 0);

    Transform0 = __vmaddfp(M1S0, Projection.r[0], *(XMVECTOR*)g_XMNegativeZero);
    Transform1 = __vmaddfp(M1S1, Projection.r[0], *(XMVECTOR*)g_XMNegativeZero);
    Transform2 = __vmaddfp(M1S2, Projection.r[0], *(XMVECTOR*)g_XMNegativeZero);
    Transform3 = __vmaddfp(M1S3, Projection.r[0], *(XMVECTOR*)g_XMNegativeZero);

    M1S0 = __vspltw(WorldView0, 1);
    M1S1 = __vspltw(WorldView1, 1);
    M1S2 = __vspltw(WorldView2, 1);
    M1S3 = __vspltw(WorldView3, 1);
    Z = __vspltw(V, 2);

    Transform0 = __vmaddfp(M1S0, Projection.r[1], Transform0);
    Transform1 = __vmaddfp(M1S1, Projection.r[1], Transform1);
    Transform2 = __vmaddfp(M1S2, Projection.r[1], Transform2);
    Transform3 = __vmaddfp(M1S3, Projection.r[1], Transform3);

    M1S0 = __vspltw(WorldView0, 2);
    M1S1 = __vspltw(WorldView1, 2);
    M1S2 = __vspltw(WorldView2, 2);
    M1S3 = __vspltw(WorldView3, 2);
    Y = __vspltw(V, 1);

    Transform0 = __vmaddfp(M1S0, Projection.r[2], Transform0);
    Transform1 = __vmaddfp(M1S1, Projection.r[2], Transform1);
    Transform2 = __vmaddfp(M1S2, Projection.r[2], Transform2);
    Transform3 = __vmaddfp(M1S3, Projection.r[2], Transform3);

    M1S0 = __vspltw(WorldView0, 3);
    M1S1 = __vspltw(WorldView1, 3);
    M1S2 = __vspltw(WorldView2, 3);
    M1S3 = __vspltw(WorldView3, 3);
    X = __vspltw(V, 0);

    Transform0 = __vmaddfp(M1S0, Projection.r[3], Transform0);
    Transform1 = __vmaddfp(M1S1, Projection.r[3], Transform1);
    Transform2 = __vmaddfp(M1S2, Projection.r[3], Transform2);
    Transform3 = __vmaddfp(M1S3, Projection.r[3], Transform3);

//    Result = XMVector3TransformCoord(V, Transform);

    Result = __vmaddfp(Z, Transform2, Transform3);
    Result = __vmaddfp(Y, Transform1, Result);
    Result = __vmaddfp(X, Transform0, Result);

    InverseW = __vspltw(Result, 3);

    Reciprocal = __vrefp(InverseW);
    S = __vnmsubfp(InverseW, Reciprocal, g_XMOne);
    Rcp = __vmaddfp(Reciprocal, S, Reciprocal);
    S = __vnmsubfp(InverseW, Rcp, g_XMOne);
    Refine = __vcmpeqfp(Rcp, Rcp);
    Rcp = __vmaddfp(Rcp, S, Rcp);
    InverseW = __vsel(Reciprocal, Rcp, Refine);

    Result = __vmaddfp(Result, InverseW, *(XMVECTOR*)g_XMNegativeZero);

    Result = __vmaddfp(Result, Scale, Offset);

    return Result;

#else // _VMX128_INTRINSICS_

    XMMATRIX Transform;
    XMMATRIX M2T, P;
    XMVECTOR M00, M01, M02, M03,
             M10, M11, M12, M13,
             M20, M21, M22, M23,
             M30, M31, M32, M33;
    XMVECTOR M00M02, M01M03, M10M12, M11M13,
             M20M22, M21M23, M30M32, M31M33;
    XMVECTOR M00M20, M10M30, M01M21, M11M31,
             M02M22, M12M32, M03M23, M13M33;
    XMVECTOR R0, R1, R2, R3, R0R2;
    XMVECTOR Scale;
    XMVECTOR Offset;
    XMVECTOR InverseW;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR I;
    XMVECTOR Result;
    FLOAT    HalfViewportWidth = ViewportWidth * 0.5f;
    FLOAT    HalfViewportHeight = ViewportHeight * 0.5f;

    XMDUMMY_INITIALIZE_VECTOR(I);

    Scale.v[0] = HalfViewportWidth;
    Scale.v[1] = -HalfViewportHeight;
    Scale.v[2] = ViewportMaxZ - ViewportMinZ;
    Scale.v[3] = 0.0f;

    Offset.v[0] = ViewportX + HalfViewportWidth;
    Offset.v[1] = ViewportY + HalfViewportHeight;
    Offset.v[2] = ViewportMinZ;
    Offset.v[3] = 0.0f;

    P.r[0] = __vmrghw(View.r[0], View.r[2]);
    P.r[1] = __vmrghw(View.r[1], View.r[3]);
    P.r[2] = __vmrglw(View.r[0], View.r[2]);
    P.r[3] = __vmrglw(View.r[1], View.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00 = __vmsum4fp(World.r[0], M2T.r[0]);
    M02 = __vmsum4fp(World.r[0], M2T.r[2]);
    M01 = __vmsum4fp(World.r[0], M2T.r[1]);
    M03 = __vmsum4fp(World.r[0], M2T.r[3]);

    M10 = __vmsum4fp(World.r[1], M2T.r[0]);
    M12 = __vmsum4fp(World.r[1], M2T.r[2]);
    M11 = __vmsum4fp(World.r[1], M2T.r[1]);
    M13 = __vmsum4fp(World.r[1], M2T.r[3]);

    M20 = __vmsum4fp(World.r[2], M2T.r[0]);
    M22 = __vmsum4fp(World.r[2], M2T.r[2]);
    M21 = __vmsum4fp(World.r[2], M2T.r[1]);
    M23 = __vmsum4fp(World.r[2], M2T.r[3]);

    M30 = __vmsum4fp(World.r[3], M2T.r[0]);
    M32 = __vmsum4fp(World.r[3], M2T.r[2]);
    M31 = __vmsum4fp(World.r[3], M2T.r[1]);
    M33 = __vmsum4fp(World.r[3], M2T.r[3]);

    P.r[0] = __vmrghw(Projection.r[0], Projection.r[2]);
    P.r[1] = __vmrghw(Projection.r[1], Projection.r[3]);
    P.r[2] = __vmrglw(Projection.r[0], Projection.r[2]);
    P.r[3] = __vmrglw(Projection.r[1], Projection.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Transform.r[0] = __vmrghw(M00M02, M01M03);
    Transform.r[1] = __vmrghw(M10M12, M11M13);
    Transform.r[2] = __vmrghw(M20M22, M21M23);
    Transform.r[3] = __vmrghw(M30M32, M31M33);

    M00 = __vmsum4fp(Transform.r[0], M2T.r[0]);
    M02 = __vmsum4fp(Transform.r[0], M2T.r[2]);
    M01 = __vmsum4fp(Transform.r[0], M2T.r[1]);
    M03 = __vmsum4fp(Transform.r[0], M2T.r[3]);

    M10 = __vmsum4fp(Transform.r[1], M2T.r[0]);
    M12 = __vmsum4fp(Transform.r[1], M2T.r[2]);
    M11 = __vmsum4fp(Transform.r[1], M2T.r[1]);
    M13 = __vmsum4fp(Transform.r[1], M2T.r[3]);

    M20 = __vmsum4fp(Transform.r[2], M2T.r[0]);
    M22 = __vmsum4fp(Transform.r[2], M2T.r[2]);
    M21 = __vmsum4fp(Transform.r[2], M2T.r[1]);
    M23 = __vmsum4fp(Transform.r[2], M2T.r[3]);

    M30 = __vmsum4fp(Transform.r[3], M2T.r[0]);
    M32 = __vmsum4fp(Transform.r[3], M2T.r[2]);
    M31 = __vmsum4fp(Transform.r[3], M2T.r[1]);
    M33 = __vmsum4fp(Transform.r[3], M2T.r[3]);

    I = __vupkd3d(I, VPACK_NORMSHORT2);

    M00M20 = __vmrghw(M00, M20);
    M10M30 = __vmrghw(M10, M30);
    M01M21 = __vmrghw(M01, M21);
    M11M31 = __vmrghw(M11, M31);
    M02M22 = __vmrghw(M02, M22);
    M12M32 = __vmrghw(M12, M32);
    M03M23 = __vmrghw(M03, M23);
    M13M33 = __vmrghw(M13, M33);

    I = __vrlimi(I, V, 0xE, 0);

    Transform.r[0] = __vmrghw(M00M20, M10M30);
    Transform.r[1] = __vmrghw(M01M21, M11M31);
    Transform.r[2] = __vmrghw(M02M22, M12M32);
    Transform.r[3] = __vmrghw(M03M23, M13M33);

    R0 = __vmsum4fp(I, Transform.r[0]);
    R1 = __vmsum4fp(I, Transform.r[1]);
    R2 = __vmsum4fp(I, Transform.r[2]);
    R3 = __vmsum4fp(I, Transform.r[3]);

    R0R2 = __vmrghw(R0, R2);

    Result = __vmrghw(R0R2, R1);

    Reciprocal = __vrefp(R3);
    S0 = __vspltw(I, 3);
    S0 = __vnmsubfp(R3, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(I, 3);
    S1 = __vnmsubfp(R3, S0, S1);
    InverseW = __vcmpeqfp(S0, S0);
    S0 = __vmaddfp(S0, S1, S0);
    InverseW = __vsel(Reciprocal, S0, InverseW);

    Result = __vmulfp(Result, InverseW);

    Result = __vmaddcfp(Scale, Result, Offset);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT3* XMVector3ProjectStream
(
    XMFLOAT3*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT3* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    FLOAT           ViewportX, 
    FLOAT           ViewportY, 
    FLOAT           ViewportWidth, 
    FLOAT           ViewportHeight, 
    FLOAT           ViewportMinZ, 
    FLOAT           ViewportMaxZ, 
    XMMATRIX        Projection, 
    XMMATRIX        View, 
    XMMATRIX        World
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX Transform;
    XMVECTOR V;
    XMVECTOR Scale;
    XMVECTOR Offset;
    XMVECTOR Result;
    UINT     i;
    FLOAT    HalfViewportWidth = ViewportWidth * 0.5f;
    FLOAT    HalfViewportHeight = ViewportHeight * 0.5f;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    Scale = XMVectorSet(HalfViewportWidth, 
                        -HalfViewportHeight,
                        ViewportMaxZ - ViewportMinZ,
                        1.0f);

    Offset = XMVectorSet(ViewportX + HalfViewportWidth,
                        ViewportY + HalfViewportHeight,
                        ViewportMinZ,
                        0.0f);

    Transform = XMMatrixMultiply(World, View);
    Transform = XMMatrixMultiply(Transform, Projection);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);

        Result = XMVector3TransformCoord(V, Transform);

        Result = XMVectorMultiplyAdd(Result, Scale, Offset);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMMATRIX Transform;
    XMVECTOR V;
    XMVECTOR Scale;
    XMVECTOR Offset;
    XMVECTOR Result;
    UINT     i;
    FLOAT    HalfViewportWidth = ViewportWidth * 0.5f;
    FLOAT    HalfViewportHeight = ViewportHeight * 0.5f;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    Scale = XMVectorSet(HalfViewportWidth, 
                        -HalfViewportHeight,
                        ViewportMaxZ - ViewportMinZ,
                        1.0f);

    Offset = XMVectorSet(ViewportX + HalfViewportWidth,
                        ViewportY + HalfViewportHeight,
                        ViewportMinZ,
                        0.0f);

    Transform = XMMatrixMultiply(World, View);
    Transform = XMMatrixMultiply(Transform, Projection);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);

        Result = XMVector3TransformCoord(V, Transform);

        Result = XMVectorMultiplyAdd(Result, Scale, Offset);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMMATRIX Transform;
    XMMATRIX M2T, P;
    XMVECTOR M00, M01, M02, M03,
             M10, M11, M12, M13,
             M20, M21, M22, M23,
             M30, M31, M32, M33;
    XMVECTOR M00M02, M01M03, M10M12, M11M13,
             M20M22, M21M23, M30M32, M31M33;
    XMVECTOR M00M20, M10M30, M01M21, M11M31,
             M02M22, M12M32, M03M23, M13M33;
    XMVECTOR R0, R1, R2, R3, R0R2;
    XMVECTOR Scale;
    XMVECTOR Offset;
    XMVECTOR InverseW;
    XMVECTOR Reciprocal;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR V, I;
    XMVECTOR Result;
    FLOAT    HalfViewportWidth = ViewportWidth * 0.5f;
    FLOAT    HalfViewportHeight = ViewportHeight * 0.5f;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMDUMMY_INITIALIZE_VECTOR(I);

    Scale.v[0] = HalfViewportWidth;
    Scale.v[1] = -HalfViewportHeight;
    Scale.v[2] = ViewportMaxZ - ViewportMinZ;
    Scale.v[3] = 0.0f;

    Offset.v[0] = ViewportX + HalfViewportWidth;
    Offset.v[1] = ViewportY + HalfViewportHeight;
    Offset.v[2] = ViewportMinZ;
    Offset.v[3] = 0.0f;

    P.r[0] = __vmrghw(View.r[0], View.r[2]);
    P.r[1] = __vmrghw(View.r[1], View.r[3]);
    P.r[2] = __vmrglw(View.r[0], View.r[2]);
    P.r[3] = __vmrglw(View.r[1], View.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00 = __vmsum4fp(World.r[0], M2T.r[0]);
    M02 = __vmsum4fp(World.r[0], M2T.r[2]);
    M01 = __vmsum4fp(World.r[0], M2T.r[1]);
    M03 = __vmsum4fp(World.r[0], M2T.r[3]);

    M10 = __vmsum4fp(World.r[1], M2T.r[0]);
    M12 = __vmsum4fp(World.r[1], M2T.r[2]);
    M11 = __vmsum4fp(World.r[1], M2T.r[1]);
    M13 = __vmsum4fp(World.r[1], M2T.r[3]);

    M20 = __vmsum4fp(World.r[2], M2T.r[0]);
    M22 = __vmsum4fp(World.r[2], M2T.r[2]);
    M21 = __vmsum4fp(World.r[2], M2T.r[1]);
    M23 = __vmsum4fp(World.r[2], M2T.r[3]);

    M30 = __vmsum4fp(World.r[3], M2T.r[0]);
    M32 = __vmsum4fp(World.r[3], M2T.r[2]);
    M31 = __vmsum4fp(World.r[3], M2T.r[1]);
    M33 = __vmsum4fp(World.r[3], M2T.r[3]);

    P.r[0] = __vmrghw(Projection.r[0], Projection.r[2]);
    P.r[1] = __vmrghw(Projection.r[1], Projection.r[3]);
    P.r[2] = __vmrglw(Projection.r[0], Projection.r[2]);
    P.r[3] = __vmrglw(Projection.r[1], Projection.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Transform.r[0] = __vmrghw(M00M02, M01M03);
    Transform.r[1] = __vmrghw(M10M12, M11M13);
    Transform.r[2] = __vmrghw(M20M22, M21M23);
    Transform.r[3] = __vmrghw(M30M32, M31M33);

    M00 = __vmsum4fp(Transform.r[0], M2T.r[0]);
    M02 = __vmsum4fp(Transform.r[0], M2T.r[2]);
    M01 = __vmsum4fp(Transform.r[0], M2T.r[1]);
    M03 = __vmsum4fp(Transform.r[0], M2T.r[3]);

    M10 = __vmsum4fp(Transform.r[1], M2T.r[0]);
    M12 = __vmsum4fp(Transform.r[1], M2T.r[2]);
    M11 = __vmsum4fp(Transform.r[1], M2T.r[1]);
    M13 = __vmsum4fp(Transform.r[1], M2T.r[3]);

    M20 = __vmsum4fp(Transform.r[2], M2T.r[0]);
    M22 = __vmsum4fp(Transform.r[2], M2T.r[2]);
    M21 = __vmsum4fp(Transform.r[2], M2T.r[1]);
    M23 = __vmsum4fp(Transform.r[2], M2T.r[3]);

    M30 = __vmsum4fp(Transform.r[3], M2T.r[0]);
    M32 = __vmsum4fp(Transform.r[3], M2T.r[2]);
    M31 = __vmsum4fp(Transform.r[3], M2T.r[1]);
    M33 = __vmsum4fp(Transform.r[3], M2T.r[3]);

    I = __vupkd3d(I, VPACK_NORMSHORT2);

    M00M20 = __vmrghw(M00, M20);
    M10M30 = __vmrghw(M10, M30);
    M01M21 = __vmrghw(M01, M21);
    M11M31 = __vmrghw(M11, M31);
    M02M22 = __vmrghw(M02, M22);
    M12M32 = __vmrghw(M12, M32);
    M03M23 = __vmrghw(M03, M23);
    M13M33 = __vmrghw(M13, M33);

    Transform.r[0] = __vmrghw(M00M20, M10M30);
    Transform.r[1] = __vmrghw(M01M21, M11M31);
    Transform.r[2] = __vmrghw(M02M22, M12M32);
    Transform.r[3] = __vmrghw(M03M23, M13M33);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);
        I = __vrlimi(I, V, 0xE, 0);

        R0 = __vmsum4fp(I, Transform.r[0]);
        R1 = __vmsum4fp(I, Transform.r[1]);
        R2 = __vmsum4fp(I, Transform.r[2]);
        R3 = __vmsum4fp(I, Transform.r[3]);

        R0R2 = __vmrghw(R0, R2);

        Result = __vmrghw(R0R2, R1);

        Reciprocal = __vrefp(R3);
        S0 = __vspltw(I, 3);
        S0 = __vnmsubfp(R3, Reciprocal, S0);
        S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
        S1 = __vspltw(I, 3);
        S1 = __vnmsubfp(R3, S0, S1);
        InverseW = __vcmpeqfp(S0, S0);
        S0 = __vmaddfp(S0, S1, S0);
        InverseW = __vsel(Reciprocal, S0, InverseW);

        Result = __vmulfp(Result, InverseW);

        Result = __vmaddcfp(Scale, Result, Offset);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector3Unproject
(
    XMVECTOR V, 
    FLOAT    ViewportX, 
    FLOAT    ViewportY, 
    FLOAT    ViewportWidth, 
    FLOAT    ViewportHeight, 
    FLOAT    ViewportMinZ, 
    FLOAT    ViewportMaxZ, 
    XMMATRIX Projection, 
    XMMATRIX View, 
    XMMATRIX World
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX        Transform;
    XMVECTOR        Scale;
    XMVECTOR        Offset;
    XMVECTOR        Determinant;
    XMVECTOR        Result;
    CONST XMVECTOR  D = XMVectorSet(-1.0f, 1.0f, 0.0f, 0.0f);

    Scale = XMVectorSet(ViewportWidth * 0.5f,
                        -ViewportHeight * 0.5f,
                        ViewportMaxZ - ViewportMinZ,
                        1.0f);
    Scale = XMVectorReciprocal(Scale);

    Offset = XMVectorSet(-ViewportX,
                        -ViewportY,
                        -ViewportMinZ,
                        0.0f);
    Offset = XMVectorMultiplyAdd(Scale, Offset, D);

    Transform = XMMatrixMultiply(World, View);
    Transform = XMMatrixMultiply(Transform, Projection);
    Transform = XMMatrixInverse(&Determinant, Transform);

    Result = XMVectorMultiplyAdd(V, Scale, Offset);

    Result = XMVector3TransformCoord(Result, Transform);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMMATRIX              Transform;
    XMVECTOR              Scale;
    XMVECTOR              Offset;
    XMVECTOR              Result;
    XMVECTOR              Determinant;
    static CONST XMVECTOR D = {-1.0f, 1.0f, 0.0f, 0.0f};

    Scale = XMVectorSet(ViewportWidth * 0.5f,
                        -ViewportHeight * 0.5f,
                        ViewportMaxZ - ViewportMinZ,
                        1.0f);
    Scale = XMVectorReciprocal(Scale);

    Offset = XMVectorSet(-ViewportX,
                        -ViewportY,
                        -ViewportMinZ,
                        0.0f);
    Offset = XMVectorMultiplyAdd(Scale, Offset, D);

    Transform = XMMatrixMultiply(World, View);
    Transform = XMMatrixMultiply(Transform, Projection);
    Transform = XMMatrixInverse(&Determinant, Transform);

    Result = XMVectorMultiplyAdd(V, Scale, Offset);

    Result = XMVector3TransformCoord(Result, Transform);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR M00, M01, M02, M03,
             M10, M11, M12, M13,
             M20, M21, M22, M23,
             M30, M31, M32, M33;
    XMVECTOR M00M02, M01M03, M10M12, M11M13,
             M20M22, M21M23, M30M32, M31M33;
    XMMATRIX M2T, P, MT;
    XMMATRIX Transform;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR Determinant;
    XMVECTOR Reciprocal;
    XMVECTOR PR1, PR2;
    XMVECTOR D0, D1, D2;
    XMVECTOR Refine;
    XMVECTOR V0[4], V1[4];
    XMVECTOR Zero, SignMask, Sign0, Sign1;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR D;
    XMMATRIX R;
    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR RZ;
    XMVECTOR InverseW;
    XMVECTOR NegativeOne;

    XMDUMMY_INITIALIZE_VECTOR(S1);

    Scale.v[0] = ViewportWidth * 0.5f;
    Scale.v[1] = -ViewportHeight * 0.5f;
    Scale.v[2] = ViewportMaxZ - ViewportMinZ;
    Scale.v[3] = 1.0f;

    Result.v[0] = -ViewportX;
    Result.v[1] = -ViewportY;
    Result.v[2] = -ViewportMinZ;
    Result.v[3] = 0.0f;

    P.r[0] = __vmrghw(View.r[0], View.r[2]);
    P.r[1] = __vmrghw(View.r[1], View.r[3]);
    P.r[2] = __vmrglw(View.r[0], View.r[2]);
    P.r[3] = __vmrglw(View.r[1], View.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00 = __vmsum4fp(World.r[0], M2T.r[0]);
    M02 = __vmsum4fp(World.r[0], M2T.r[2]);
    M01 = __vmsum4fp(World.r[0], M2T.r[1]);
    M03 = __vmsum4fp(World.r[0], M2T.r[3]);

    M10 = __vmsum4fp(World.r[1], M2T.r[0]);
    M12 = __vmsum4fp(World.r[1], M2T.r[2]);
    M11 = __vmsum4fp(World.r[1], M2T.r[1]);
    M13 = __vmsum4fp(World.r[1], M2T.r[3]);

    M20 = __vmsum4fp(World.r[2], M2T.r[0]);
    M22 = __vmsum4fp(World.r[2], M2T.r[2]);
    M21 = __vmsum4fp(World.r[2], M2T.r[1]);
    M23 = __vmsum4fp(World.r[2], M2T.r[3]);

    M30 = __vmsum4fp(World.r[3], M2T.r[0]);
    M32 = __vmsum4fp(World.r[3], M2T.r[2]);
    M31 = __vmsum4fp(World.r[3], M2T.r[1]);
    M33 = __vmsum4fp(World.r[3], M2T.r[3]);

    P.r[0] = __vmrghw(Projection.r[0], Projection.r[2]);
    P.r[1] = __vmrghw(Projection.r[1], Projection.r[3]);
    P.r[2] = __vmrglw(Projection.r[0], Projection.r[2]);
    P.r[3] = __vmrglw(Projection.r[1], Projection.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Transform.r[0] = __vmrghw(M00M02, M01M03);
    Transform.r[1] = __vmrghw(M10M12, M11M13);
    Transform.r[2] = __vmrghw(M20M22, M21M23);
    Transform.r[3] = __vmrghw(M30M32, M31M33);

    M00 = __vmsum4fp(Transform.r[0], M2T.r[0]);
    M02 = __vmsum4fp(Transform.r[0], M2T.r[2]);
    M01 = __vmsum4fp(Transform.r[0], M2T.r[1]);
    M03 = __vmsum4fp(Transform.r[0], M2T.r[3]);

    M10 = __vmsum4fp(Transform.r[1], M2T.r[0]);
    M12 = __vmsum4fp(Transform.r[1], M2T.r[2]);
    M11 = __vmsum4fp(Transform.r[1], M2T.r[1]);
    M13 = __vmsum4fp(Transform.r[1], M2T.r[3]);

    M20 = __vmsum4fp(Transform.r[2], M2T.r[0]);
    M22 = __vmsum4fp(Transform.r[2], M2T.r[2]);
    M21 = __vmsum4fp(Transform.r[2], M2T.r[1]);
    M23 = __vmsum4fp(Transform.r[2], M2T.r[3]);

    M30 = __vmsum4fp(Transform.r[3], M2T.r[0]);
    M32 = __vmsum4fp(Transform.r[3], M2T.r[2]);
    M31 = __vmsum4fp(Transform.r[3], M2T.r[1]);
    M33 = __vmsum4fp(Transform.r[3], M2T.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Transform.r[0] = __vmrghw(M00M02, M01M03);
    Transform.r[1] = __vmrghw(M10M12, M11M13);
    Transform.r[2] = __vmrghw(M20M22, M21M23);
    Transform.r[3] = __vmrghw(M30M32, M31M33);

    P.r[0] = __vmrghw(Transform.r[0], Transform.r[2]);
    P.r[1] = __vmrghw(Transform.r[1], Transform.r[3]);
    P.r[2] = __vmrglw(Transform.r[0], Transform.r[2]);
    P.r[3] = __vmrglw(Transform.r[1], Transform.r[3]);

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

    Transform.r[0] = __vmulfp(R.r[0], Refine);
    Transform.r[1] = __vmulfp(R.r[1], Refine);
    Transform.r[2] = __vmulfp(R.r[2], Refine);
    Transform.r[3] = __vmulfp(R.r[3], Refine);

    Reciprocal = __vrefp(Scale);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    D = __vpermwi(S1, 0xEA);
    S0 = __vnmsubfp(Scale, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(S1, 3);
    NegativeOne = __vspltisw(-1);
    S1 = __vnmsubfp(Scale, S0, S1);
    Scale = __vcmpeqfp(S0, S0);
    NegativeOne = __vcfsx(NegativeOne, 0);
    S0 = __vmaddfp(S0, S1, S0);
    D = __vsldoi(NegativeOne, D, 3 << 2);
    Scale = __vsel(Reciprocal, S0, Scale);

    Result = __vmaddcfp(Scale, Result, D);

    Result = __vmaddfp(V, Scale, Result);

    RZ = __vspltw(Result, 2);
    Y = __vspltw(Result, 1);
    X = __vspltw(Result, 0);

    RZ = __vmaddcfp(Transform.r[2], RZ, Transform.r[3]);
    RZ = __vmaddfp(Y, Transform.r[1], RZ);
    RZ = __vmaddfp(X, Transform.r[0], RZ);

    InverseW = __vspltw(RZ, 3);

    Reciprocal = __vrefp(InverseW);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    S0 = __vnmsubfp(InverseW, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(S1, 3);
    S1 = __vnmsubfp(InverseW, S0, S1);
    InverseW = __vcmpeqfp(S0, S0);
    S0 = __vmaddfp(S0, S1, S0);
    InverseW = __vsel(Reciprocal, S0, InverseW);

    Result = __vmulfp(RZ, InverseW);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT3* XMVector3UnprojectStream
(
    XMFLOAT3*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT3* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    FLOAT           ViewportX, 
    FLOAT           ViewportY, 
    FLOAT           ViewportWidth, 
    FLOAT           ViewportHeight, 
    FLOAT           ViewportMinZ, 
    FLOAT           ViewportMaxZ, 
    XMMATRIX        Projection, 
    XMMATRIX        View, 
    XMMATRIX        World)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX        Transform;
    XMVECTOR        Scale;
    XMVECTOR        Offset;
    XMVECTOR        V;
    XMVECTOR        Determinant;
    XMVECTOR        Result;
    UINT            i;
    BYTE*           pInputVector = (BYTE*)pInputStream;
    BYTE*           pOutputVector = (BYTE*)pOutputStream;
    CONST XMVECTOR  D = XMVectorSet(-1.0f, 1.0f, 0.0f, 0.0f);

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    Scale = XMVectorSet(ViewportWidth * 0.5f,
                        -ViewportHeight * 0.5f,
                        ViewportMaxZ - ViewportMinZ,
                        1.0f);
    Scale = XMVectorReciprocal(Scale);

    Offset = XMVectorSet(-ViewportX,
                        -ViewportY,
                        -ViewportMinZ,
                        0.0f);
    Offset = XMVectorMultiplyAdd(Scale, Offset, D);

    Transform = XMMatrixMultiply(World, View);
    Transform = XMMatrixMultiply(Transform, Projection);
    Transform = XMMatrixInverse(&Determinant, Transform);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);

        Result = XMVectorMultiplyAdd(V, Scale, Offset);

        Result = XMVector3TransformCoord(Result, Transform);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    XMMATRIX              Transform;
    XMVECTOR              Scale;
    XMVECTOR              Offset;
    XMVECTOR              V;
    XMVECTOR              Determinant;
    XMVECTOR              Result;
    UINT                  i;
    BYTE*                 pInputVector = (BYTE*)pInputStream;
    BYTE*                 pOutputVector = (BYTE*)pOutputStream;
    static CONST XMVECTOR D = {-1.0f, 1.0f, 0.0f, 0.0f};

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    Scale = XMVectorSet(ViewportWidth * 0.5f,
                        -ViewportHeight * 0.5f,
                        ViewportMaxZ - ViewportMinZ,
                        1.0f);
    Scale = XMVectorReciprocal(Scale);

    Offset = XMVectorSet(-ViewportX,
                        -ViewportY,
                        -ViewportMinZ,
                        0.0f);
    Offset = XMVectorMultiplyAdd(Scale, Offset, D);

    Transform = XMMatrixMultiply(World, View);
    Transform = XMMatrixMultiply(Transform, Projection);
    Transform = XMMatrixInverse(&Determinant, Transform);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);

        Result = XMVectorMultiplyAdd(V, Scale, Offset);

        Result = XMVector3TransformCoord(Result, Transform);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    XMVECTOR M00, M01, M02, M03,
             M10, M11, M12, M13,
             M20, M21, M22, M23,
             M30, M31, M32, M33;
    XMVECTOR M00M02, M01M03, M10M12, M11M13,
             M20M22, M21M23, M30M32, M31M33;
    XMMATRIX M2T, P, MT;
    XMMATRIX Transform;
    XMVECTOR Scale;
    XMVECTOR Offset;
    XMVECTOR Result;
    XMVECTOR Determinant;
    XMVECTOR Reciprocal;
    XMVECTOR PR1, PR2;
    XMVECTOR D0, D1, D2;
    XMVECTOR Refine;
    XMVECTOR V0[4], V1[4];
    XMVECTOR Zero, SignMask, Sign0, Sign1;
    XMVECTOR S0;
    XMVECTOR S1;
    XMVECTOR D;
    XMMATRIX R;
    XMVECTOR NegativeOne;
    XMVECTOR V;
    XMVECTOR I;
    XMVECTOR InverseW;
    XMVECTOR R0, R1, R2, R3, R0R2;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMDUMMY_INITIALIZE_VECTOR(S1);
    XMDUMMY_INITIALIZE_VECTOR(I);

    Scale.v[0] = ViewportWidth * 0.5f;
    Scale.v[1] = -ViewportHeight * 0.5f;
    Scale.v[2] = ViewportMaxZ - ViewportMinZ;
    Scale.v[3] = 1.0f;

    Offset.v[0] = -ViewportX;
    Offset.v[1] = -ViewportY;
    Offset.v[2] = -ViewportMinZ;
    Offset.v[3] = 0.0f;

    P.r[0] = __vmrghw(View.r[0], View.r[2]);
    P.r[1] = __vmrghw(View.r[1], View.r[3]);
    P.r[2] = __vmrglw(View.r[0], View.r[2]);
    P.r[3] = __vmrglw(View.r[1], View.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00 = __vmsum4fp(World.r[0], M2T.r[0]);
    M02 = __vmsum4fp(World.r[0], M2T.r[2]);
    M01 = __vmsum4fp(World.r[0], M2T.r[1]);
    M03 = __vmsum4fp(World.r[0], M2T.r[3]);

    M10 = __vmsum4fp(World.r[1], M2T.r[0]);
    M12 = __vmsum4fp(World.r[1], M2T.r[2]);
    M11 = __vmsum4fp(World.r[1], M2T.r[1]);
    M13 = __vmsum4fp(World.r[1], M2T.r[3]);

    M20 = __vmsum4fp(World.r[2], M2T.r[0]);
    M22 = __vmsum4fp(World.r[2], M2T.r[2]);
    M21 = __vmsum4fp(World.r[2], M2T.r[1]);
    M23 = __vmsum4fp(World.r[2], M2T.r[3]);

    M30 = __vmsum4fp(World.r[3], M2T.r[0]);
    M32 = __vmsum4fp(World.r[3], M2T.r[2]);
    M31 = __vmsum4fp(World.r[3], M2T.r[1]);
    M33 = __vmsum4fp(World.r[3], M2T.r[3]);

    P.r[0] = __vmrghw(Projection.r[0], Projection.r[2]);
    P.r[1] = __vmrghw(Projection.r[1], Projection.r[3]);
    P.r[2] = __vmrglw(Projection.r[0], Projection.r[2]);
    P.r[3] = __vmrglw(Projection.r[1], Projection.r[3]);

    M2T.r[0] = __vmrghw(P.r[0], P.r[1]);
    M2T.r[2] = __vmrghw(P.r[2], P.r[3]);
    M2T.r[1] = __vmrglw(P.r[0], P.r[1]);
    M2T.r[3] = __vmrglw(P.r[2], P.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Transform.r[0] = __vmrghw(M00M02, M01M03);
    Transform.r[1] = __vmrghw(M10M12, M11M13);
    Transform.r[2] = __vmrghw(M20M22, M21M23);
    Transform.r[3] = __vmrghw(M30M32, M31M33);

    M00 = __vmsum4fp(Transform.r[0], M2T.r[0]);
    M02 = __vmsum4fp(Transform.r[0], M2T.r[2]);
    M01 = __vmsum4fp(Transform.r[0], M2T.r[1]);
    M03 = __vmsum4fp(Transform.r[0], M2T.r[3]);

    M10 = __vmsum4fp(Transform.r[1], M2T.r[0]);
    M12 = __vmsum4fp(Transform.r[1], M2T.r[2]);
    M11 = __vmsum4fp(Transform.r[1], M2T.r[1]);
    M13 = __vmsum4fp(Transform.r[1], M2T.r[3]);

    M20 = __vmsum4fp(Transform.r[2], M2T.r[0]);
    M22 = __vmsum4fp(Transform.r[2], M2T.r[2]);
    M21 = __vmsum4fp(Transform.r[2], M2T.r[1]);
    M23 = __vmsum4fp(Transform.r[2], M2T.r[3]);

    M30 = __vmsum4fp(Transform.r[3], M2T.r[0]);
    M32 = __vmsum4fp(Transform.r[3], M2T.r[2]);
    M31 = __vmsum4fp(Transform.r[3], M2T.r[1]);
    M33 = __vmsum4fp(Transform.r[3], M2T.r[3]);

    M00M02 = __vmrghw(M00, M02);
    M01M03 = __vmrghw(M01, M03);
    M10M12 = __vmrghw(M10, M12);
    M11M13 = __vmrghw(M11, M13);
    M20M22 = __vmrghw(M20, M22);
    M21M23 = __vmrghw(M21, M23);
    M30M32 = __vmrghw(M30, M32);
    M31M33 = __vmrghw(M31, M33);

    Transform.r[0] = __vmrghw(M00M02, M01M03);
    Transform.r[1] = __vmrghw(M10M12, M11M13);
    Transform.r[2] = __vmrghw(M20M22, M21M23);
    Transform.r[3] = __vmrghw(M30M32, M31M33);

    P.r[0] = __vmrghw(Transform.r[0], Transform.r[2]);
    P.r[1] = __vmrghw(Transform.r[1], Transform.r[3]);
    P.r[2] = __vmrglw(Transform.r[0], Transform.r[2]);
    P.r[3] = __vmrglw(Transform.r[1], Transform.r[3]);

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

    Transform.r[0] = __vmulfp(R.r[0], Refine);
    Transform.r[1] = __vmulfp(R.r[1], Refine);
    Transform.r[2] = __vmulfp(R.r[2], Refine);
    Transform.r[3] = __vmulfp(R.r[3], Refine);

    Reciprocal = __vrefp(Scale);
    S1 = __vupkd3d(S1, VPACK_NORMSHORT2);
    S0 = __vspltw(S1, 3);
    D = __vpermwi(S1, 0xEA);
    S0 = __vnmsubfp(Scale, Reciprocal, S0);
    S0 = __vmaddcfp(Reciprocal, S0, Reciprocal);
    S1 = __vspltw(S1, 3);
    NegativeOne = __vspltisw(-1);
    S1 = __vnmsubfp(Scale, S0, S1);
    Scale = __vcmpeqfp(S0, S0);
    NegativeOne = __vcfsx(NegativeOne, 0);
    S0 = __vmaddfp(S0, S1, S0);
    D = __vsldoi(NegativeOne, D, 3 << 2);
    Scale = __vsel(Reciprocal, S0, Scale);

    Offset = __vmaddcfp(Scale, Offset, D);

    I = __vupkd3d(I, VPACK_NORMSHORT2);

    P.r[0] = __vmrghw(Transform.r[0], Transform.r[2]);
    P.r[1] = __vmrghw(Transform.r[1], Transform.r[3]);
    P.r[2] = __vmrglw(Transform.r[0], Transform.r[2]);
    P.r[3] = __vmrglw(Transform.r[1], Transform.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat3((XMFLOAT3*)pInputVector);

        V = __vmaddcfp(Scale, V, Offset);

        V = __vrlimi(V, I, 0x1, 0);

        R0 = __vmsum4fp(V, MT.r[0]);
        R1 = __vmsum4fp(V, MT.r[1]);
        R2 = __vmsum4fp(V, MT.r[2]);
        R3 = __vmsum4fp(V, MT.r[3]);

        R0R2 = __vmrghw(R0, R2);

        Result = __vmrghw(R0R2, R1);

        InverseW = XMVectorReciprocal(R3);

        Result = __vmulfp(Result, InverseW);

        XMStoreFloat3((XMFLOAT3*)pOutputVector, Result);

        pInputVector += InputStride; 
        pOutputVector += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

/****************************************************************************
 *
 * 4D Vector
 *
 ****************************************************************************/

//------------------------------------------------------------------------------
// Comparison operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4Equal
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector4EqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector4EqualR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] == V2.v[0] && 
        V1.v[1] == V2.v[1] &&
        V1.v[2] == V2.v[2] &&
        V1.v[3] == V2.v[3])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] != V2.v[0] && 
        V1.v[1] != V2.v[1] &&
        V1.v[2] != V2.v[2] &&
        V1.v[3] != V2.v[3])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    UINT CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    __vcmpeqfpR(V1, V2, &CR);

    return CR;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4EqualInt
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector4EqualIntR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector4EqualIntR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.u[0] == V2.u[0] && 
        V1.u[1] == V2.u[1] &&
        V1.u[2] == V2.u[2] &&
        V1.u[3] == V2.u[3])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.u[0] != V2.u[0] && 
        V1.u[1] != V2.u[1] &&
        V1.u[2] != V2.u[2] &&
        V1.u[3] != V2.u[3])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    UINT CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    __vcmpequwR(V1, V2, &CR);

    return CR;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4NearEqual
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR Epsilon
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Delta;

    Delta = XMVectorSubtract(V1, V2);
    Delta = XMVectorAbs(Delta);

    return (Delta.v[0] <= Epsilon.v[0] &&
            Delta.v[1] <= Epsilon.v[1] &&
            Delta.v[2] <= Epsilon.v[2] &&
            Delta.v[3] <= Epsilon.v[3]);

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR    Delta;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    Delta = __vsubfp(V1, V2);
    __vcmpbfpR(Delta, Epsilon, &CR);

    return XMComparisonAllInBounds(CR);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4NotEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAnyFalse(XMVector4EqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4NotEqualInt
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAnyFalse(XMVector4EqualIntR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4Greater
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector4GreaterR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector4GreaterR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] > V2.v[0] && 
        V1.v[1] > V2.v[1] &&
        V1.v[2] > V2.v[2] &&
        V1.v[3] > V2.v[3])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] <= V2.v[0] && 
        V1.v[1] <= V2.v[1] &&
        V1.v[2] <= V2.v[2] &&
        V1.v[3] <= V2.v[3])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    UINT CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    __vcmpgtfpR(V1, V2, &CR);

    return CR;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4GreaterOrEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector4GreaterOrEqualR(V1, V2));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector4GreaterOrEqualR
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if (V1.v[0] >= V2.v[0] && 
        V1.v[1] >= V2.v[1] &&
        V1.v[2] >= V2.v[2] &&
        V1.v[3] >= V2.v[3])
    {
        CR |= XM_CRMASK_CR6TRUE;
    }
    else if (V1.v[0] < V2.v[0] && 
        V1.v[1] < V2.v[1] &&
        V1.v[2] < V2.v[2] &&
        V1.v[3] < V2.v[3])
    {
        CR |= XM_CRMASK_CR6FALSE;
    }

    return CR;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    UINT CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    __vcmpgefpR(V1, V2, &CR);

    return CR;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4Less
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector4GreaterR(V2, V1));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4LessOrEqual
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
    return XMComparisonAllTrue(XMVector4GreaterOrEqualR(V2, V1));
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4InBounds
(
    XMVECTOR V, 
    XMVECTOR Bounds
)
{
    return XMComparisonAllInBounds(XMVector4InBoundsR(V, Bounds));
}

//------------------------------------------------------------------------------

XMFINLINE UINT XMVector4InBoundsR
(
    XMVECTOR V, 
    XMVECTOR Bounds
)
{
#if defined(_NO_INTRINSICS_)

    UINT CR = 0;

    if ((V.v[0] <= Bounds.v[0] && V.v[0] >= -Bounds.v[0]) && 
        (V.v[1] <= Bounds.v[1] && V.v[1] >= -Bounds.v[1]) &&
        (V.v[2] <= Bounds.v[2] && V.v[2] >= -Bounds.v[2]) &&
        (V.v[3] <= Bounds.v[3] && V.v[3] >= -Bounds.v[3]))
    {
        CR |= XM_CRMASK_CR6BOUNDS;
    }

    return CR;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    UINT CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    __vcmpbfpR(V, Bounds, &CR);

    return CR;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4IsNaN
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return (XMISNAN(V.v[0]) ||
            XMISNAN(V.v[1]) ||
            XMISNAN(V.v[2]) ||
            XMISNAN(V.v[3]));

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    UINT CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    __vcmpeqfpR(V, V, &CR);

    return XMComparisonAnyFalse(CR);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE BOOL XMVector4IsInfinite
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    return (XMISINF(V.v[0]) ||
            XMISINF(V.v[1]) ||
            XMISINF(V.v[2]) ||
            XMISINF(V.v[3]));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR    C;
    UINT        CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    C = __vand(V, *(XMVECTOR*)g_XMAbsMask);
    __vcmpequwR(C, *(XMVECTOR*)g_XMInfinity, &CR);

    return XMComparisonAnyTrue(CR);

#else // _VMX128_INTRINSICS_

    XMVECTOR OneInt, TrueInt;
    XMVECTOR VShifted, InfinityShifted;
    XMVECTOR Shift;
    UINT     CR;

    XMDUMMY_INITIALIZE_UINT(CR);

    OneInt = __vspltisw(1);
    TrueInt = __vspltisw(-1);
    Shift = __vspltisw(-8); // Shift 24
    VShifted = __vslw(V, OneInt);
    InfinityShifted = __vslw(TrueInt, Shift);
    __vcmpequwR(VShifted, InfinityShifted, &CR);

    return XMComparisonAnyTrue(CR);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4Dot
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] =
    Result.v[1] =
    Result.v[2] =
    Result.v[3] = V1.v[0] * V2.v[0] + V1.v[1] * V2.v[1] + V1.v[2] * V2.v[2] + V1.v[3] * V2.v[3];

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    // VMX2 hardware will have dedicated dot product logic, but with VMX1 it must be accomplished
    // using a multiply, 2 additions, and 2 rotations.
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(V1, V2, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);

    return Result;

#else // _VMX128_INTRINSICS_

    // Overflow will result in a QNaN instead of +/- infinity.

    return __vmsum4fp(V1, V2);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4Cross
(
    XMVECTOR V1, 
    XMVECTOR V2, 
    XMVECTOR V3
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               V1A, V2A, V2B, V3A, V3B;
    XMVECTOR               R1, R2, R3;
    XMVECTOR               Result;
    static CONST XMVECTORI ControlYXXX = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X};
    static CONST XMVECTORI ControlZWYZ = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI ControlWZWY = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y};
    static CONST XMVECTORI ControlZZYY = {XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
    static CONST XMVECTORI ControlYWXZ = {XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Z};
    static CONST XMVECTORI ControlWXWX = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static CONST XMVECTORI ControlWWWZ = {XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0Z};
    static CONST XMVECTORI ControlYZXY = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y};
    static CONST XMVECTORI ControlZXYX = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0X};

    V2A = XMVectorPermute(V2, V2, *(XMVECTOR*)ControlZWYZ);
    V3A = XMVectorPermute(V3, V3, *(XMVECTOR*)ControlWZWY);
    V2B = XMVectorPermute(V2, V2, *(XMVECTOR*)ControlWZWY);
    V3B = XMVectorPermute(V3, V3, *(XMVECTOR*)ControlZWYZ);
    V1A = XMVectorPermute(V1, V1, *(XMVECTOR*)ControlYXXX);

    R1 = XMVectorMultiply(V2A, V3A);
    R1 = XMVectorNegativeMultiplySubtract(V2B, V3B, R1);
    R1 = XMVectorMultiply(V1A, R1);
    
    V2A = XMVectorPermute(V2, V2, *(XMVECTOR*)ControlYWXZ);
    V3A = XMVectorPermute(V3, V3, *(XMVECTOR*)ControlWXWX);
    V2B = XMVectorPermute(V2, V2, *(XMVECTOR*)ControlWXWX);
    V3B = XMVectorPermute(V3, V3, *(XMVECTOR*)ControlYWXZ);
    V1A = XMVectorPermute(V1, V1, *(XMVECTOR*)ControlZZYY);

    R2 = XMVectorMultiply(V2A, V3A);
    R2 = XMVectorNegativeMultiplySubtract(V2B, V3B, R2);
    R2 = XMVectorNegativeMultiplySubtract(V1A, R2, R1);

    V2A = XMVectorPermute(V2, V2, *(XMVECTOR*)ControlYZXY);
    V3A = XMVectorPermute(V3, V3, *(XMVECTOR*)ControlZXYX);
    V2B = XMVectorPermute(V2, V2, *(XMVECTOR*)ControlZXYX);
    V3B = XMVectorPermute(V3, V3, *(XMVECTOR*)ControlYZXY);
    V1A = XMVectorPermute(V1, V1, *(XMVECTOR*)ControlWWWZ);

    R3 = XMVectorMultiply(V2A, V3A);
    R3 = XMVectorNegativeMultiplySubtract(V2B, V3B, R3);
    Result = XMVectorMultiplyAdd(V1A, R3, R2);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               V1A, V1B, V1C, V2A, V2B, V2C, V3A, V3B, V3C;
    XMVECTOR               R1, R2, R3;
    XMVECTOR               Result;
    static CONST XMVECTORI ControlYXXX = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X};
    static CONST XMVECTORI ControlZWYZ = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y, XM_PERMUTE_0Z};
    static CONST XMVECTORI ControlWZWY = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0Y};
    static CONST XMVECTORI ControlZZYY = {XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0Y};
    static CONST XMVECTORI ControlYWXZ = {XM_PERMUTE_0Y, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Z};
    static CONST XMVECTORI ControlWXWX = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0X};
    static CONST XMVECTORI ControlWWWZ = {XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0W, XM_PERMUTE_0Z};
    static CONST XMVECTORI ControlYZXY = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y};
    static CONST XMVECTORI ControlZXYX = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0X};

    V2A = __vperm(V2, V2, *(XMVECTOR*)ControlZWYZ);
    V3A = __vperm(V3, V3, *(XMVECTOR*)ControlWZWY);
    V2B = __vperm(V2, V2, *(XMVECTOR*)ControlYWXZ);
    V3B = __vperm(V3, V3, *(XMVECTOR*)ControlWXWX);
    V2C = __vperm(V2, V2, *(XMVECTOR*)ControlYZXY);
    V3C = __vperm(V3, V3, *(XMVECTOR*)ControlZXYX);

    R1 = __vmaddfp(V2A, V3A, *(XMVECTOR*)g_XMNegativeZero);
    R2 = __vmaddfp(V2B, V3B, *(XMVECTOR*)g_XMNegativeZero);
    R3 = __vmaddfp(V2C, V3C, *(XMVECTOR*)g_XMNegativeZero);

    V2A = __vperm(V2, V2, *(XMVECTOR*)ControlWZWY);
    V3A = __vperm(V3, V3, *(XMVECTOR*)ControlZWYZ);
    V2B = __vperm(V2, V2, *(XMVECTOR*)ControlWXWX);
    V3B = __vperm(V3, V3, *(XMVECTOR*)ControlYWXZ);
    V2C = __vperm(V2, V2, *(XMVECTOR*)ControlZXYX);
    V3C = __vperm(V3, V3, *(XMVECTOR*)ControlYZXY);

    R1 = __vnmsubfp(V2A, V3A, R1);
    R2 = __vnmsubfp(V2B, V3B, R2);
    R3 = __vnmsubfp(V2C, V3C, R3);

    V1A = __vperm(V1, V1, *(XMVECTOR*)ControlYXXX);
    V1B = __vperm(V1, V1, *(XMVECTOR*)ControlZZYY);
    V1C = __vperm(V1, V1, *(XMVECTOR*)ControlWWWZ);

    R1 = __vmaddfp(V1A, R1, *(XMVECTOR*)g_XMNegativeZero);
    R2 = __vnmsubfp(V1B, R2, R1);
    Result = __vmaddfp(V1C, R3, R2);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V1A, V1B, V1C, V2A, V2B, V2C, V3A, V3B, V3C;
    XMVECTOR               R1, R2, R3;

    V2A = __vpermwi(V2, 0xB6);
    V3A = __vpermwi(V3, 0xED);
    V2B = __vpermwi(V2, 0x72);
    V3B = __vpermwi(V3, 0xCC);
    V2C = __vpermwi(V2, 0x61);
    V3C = __vpermwi(V3, 0x84);

    R1 = __vmulfp(V2A, V3A);
    R2 = __vmulfp(V2B, V3B);
    R3 = __vmulfp(V2C, V3C);

    V2A = __vpermwi(V2, 0xED);
    V3A = __vpermwi(V3, 0xB6);
    V2B = __vpermwi(V2, 0xCC);
    V3B = __vpermwi(V3, 0x72);
    V2C = __vpermwi(V2, 0x84);
    V3C = __vpermwi(V3, 0x61);

    R1 = __vnmsubfp(V2A, V3A, R1);
    R2 = __vnmsubfp(V2B, V3B, R2);
    R3 = __vnmsubfp(V2C, V3C, R3);

    V1A = __vpermwi(V1, 0x40);
    V1B = __vpermwi(V1, 0xA5);
    V1C = __vpermwi(V1, 0xFE);

    R1 = __vmulfp(V1A, R1);
    R1 = __vnmsubfp(V1B, R2, R1);
    R1 = __vmaddfp(V1C, R3, R1);

    return R1;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4LengthSq
(
    XMVECTOR V
)
{
    return XMVector4Dot(V, V);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4ReciprocalLengthEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector4LengthSq(V);
    Result = XMVectorReciprocalSqrtEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Result = __vrsqrtefp(D0);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Result;

    Result = __vmsum4fp(V, V);
    Result = __vrsqrtefp(Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4ReciprocalLength
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector4LengthSq(V);
    Result = XMVectorReciprocalSqrt(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    Result = __vmaddfp(Rsq, RT, Rsq);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR H;

    D = __vmsum4fp(V, V);
    H = __vspltisw(1);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);

    return Rsq;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4LengthEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector4LengthSq(V);
    Result = XMVectorSqrtEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

#if 0

    // Faster, but returns a QNaN on zero and infinite vectors
    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    D1 = __vrsqrtefp(D0);
    Result = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);

#else

    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Zero = __vspltisw(0);
    D0 = __vaddfp(D0, D1);
    D1 = __vrsqrtefp(D0);
    InfiniteLength = __vcmpequw(D0, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(D0, Zero);
    Result = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Result = __vsel(D0, Result, Select);

#endif

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Result;

#if 0

    // Faster, but returns a QNaN on zero and infinite vectors.

    D = __vmsum4fp(V, V);
    Rsq = __vrsqrtefp(D);
    Result = __vmulfp(D, Rsq);

#else

    // Returns a QNaN on infinite vectors.

    XMVECTOR Zero;
    XMVECTOR Length;

    D = __vmsum4fp(V, V);
    Zero = __vspltisw(0);
    Rsq = __vrsqrtefp(D);
    Result = __vcmpeqfp(D, Zero);
    Length = __vmulfp(D, Rsq);
    Result = __vsel(Length, D, Result);

#endif

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4Length
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector4LengthSq(V);
    Result = XMVectorSqrt(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Zero = __vspltisw(0);
    D0 = __vaddfp(D0, D1);
    Rsq = __vrsqrtefp(D0);
    RT = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D1 = __vmaddfp(Rsq, RT, Rsq);
    InfiniteLength = __vcmpequw(D0, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(D0, Zero);
    Result = __vmaddfp(D0, D1, *(XMVECTOR*)g_XMNegativeZero);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Result = __vsel(D0, Result, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    // Returns a QNaN on infinite vectors.

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR Length;
    XMVECTOR H;

    H = __vspltisw(1);
    D = __vmsum4fp(V, V);
    H = __vcfsx(H, 1);
    Rsq = __vrsqrtefp(D);
    RT = __vmulfp(D, H);
    Rcp = __vmulfp(Rsq, Rsq);
    Zero = __vspltisw(0);
    H = __vnmsubfp(RT, Rcp, H);
    Rsq = __vmaddfp(Rsq, H, Rsq);
    Result = __vcmpeqfp(D, Zero);
    Length = __vmulfp(D, Rsq);
    Result = __vsel(Length, D, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------
// XMVector4NormalizeEst uses a reciprocal estimate and
// returns QNaN on zero and infinite vectors.

XMFINLINE XMVECTOR XMVector4NormalizeEst
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;
    Result = XMVector4ReciprocalLength(V);
    Result = XMVectorMultiply(V, Result);
    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    D0 = __vaddfp(D0, D1);
    D1 = __vrsqrtefp(D0);
    Result = __vmaddfp(V, D1, *(XMVECTOR*)g_XMNegativeZero);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Rsq;
    XMVECTOR Result;

#if 1

    // Faster, but returns a QNaN on zero and infinite vectors.

    D = __vmsum4fp(V, V);
    Rsq = __vrsqrtefp(D);
    Result = __vmulfp(V, Rsq);

#else

    // Returns a QNaN on infinite vectors.

    XMVECTOR Zero;
    XMVECTOR N;

    D = __vmsum4fp(V, V);
    Zero = __vspltisw(0);
    Rsq = __vrsqrtefp(D);
    Result = __vcmpeqfp(D, Zero);
    N = __vmulfp(V, Rsq);
    Result = __vsel(N, D, Result);

#endif

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4Normalize
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR LengthSq;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR Result;

    LengthSq = XMVector4LengthSq(V);
    Zero = XMVectorZero();
    Result = XMVectorReciprocalSqrt(LengthSq);
    InfiniteLength = XMVectorEqualInt(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = XMVectorEqual(LengthSq, Zero);
    Result = XMVectorMultiply(V, Result);
    Select = XMVectorEqualInt(InfiniteLength, ZeroLength);
    Result = XMVectorSelect(LengthSq, Result, Select);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Zero = __vspltisw(0);
    D0 = __vaddfp(D0, D1);
    RcpLength = __vrsqrtefp(D0);
    OneHalfLengthSq = __vmaddfp(D0, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    InfiniteLength = __vcmpequw(D0, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(D0, Zero);
    Scale = __vmaddfp(RcpLength, RcpLength, *(XMVECTOR*)g_XMNegativeZero);
    Scale = __vnmsubfp(OneHalfLengthSq, Scale, g_XMOneHalf);
    Select = __vcmpequw(InfiniteLength, ZeroLength);
    D1 = __vmaddfp(RcpLength, Scale, RcpLength);
    Result = __vmaddfp(V, D1, *(XMVECTOR*)g_XMNegativeZero);
    Result = __vsel(D0, Result, Select);

    return Result;

#else // _VMX128_INTRINSICS_

    // Returns a QNaN on infinite vectors.

    XMVECTOR D;
    XMVECTOR RcpLength;
    XMVECTOR OneHalfLengthSq;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR Zero;
    XMVECTOR H;
    XMVECTOR N;

    H = __vspltisw(1);
    D = __vmsum4fp(V, V);
    H = __vcfsx(H, 1);
    Zero = __vspltisw(0);
    RcpLength = __vrsqrtefp(D);
    OneHalfLengthSq = __vmulfp(D, H);
    Result = __vcmpeqfp(D, Zero);
    Scale = __vmulfp(RcpLength, RcpLength);
    H = __vnmsubfp(OneHalfLengthSq, Scale, H);
    H = __vmaddcfp(RcpLength, H, RcpLength);
    N = __vmulfp(V, H);
    Result = __vsel(N, D, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4ClampLength
(
    XMVECTOR V, 
    FLOAT    LengthMin, 
    FLOAT    LengthMax
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    ClampMax = XMVectorReplicate(LengthMax);
    ClampMin = XMVectorReplicate(LengthMin);

    return XMVector4ClampLengthV(V, ClampMin, ClampMax);

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    ClampMax.v[0] =
    ClampMax.v[1] = 
    ClampMax.v[2] =
    ClampMax.v[3] = LengthMax;

    ClampMin.v[0] =
    ClampMin.v[1] = 
    ClampMin.v[2] =
    ClampMin.v[3] = LengthMin;

    return XMVector4ClampLengthV(V, ClampMin, ClampMax);

#else // _VMX128_INTRINSICS_

    XMVECTOR ClampMax;
    XMVECTOR ClampMin;

    XMASSERT(((UINT_PTR)&LengthMin & 3) == 0);
    XMASSERT(((UINT_PTR)&LengthMax & 3) == 0);

    ClampMax = __lvlx(&LengthMax, 0);
    ClampMin = __lvlx(&LengthMin, 0);

    ClampMax = __vspltw(ClampMax, 0);
    ClampMin = __vspltw(ClampMin, 0);

    return XMVector4ClampLengthV(V, ClampMin, ClampMax);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4ClampLengthV
(
    XMVECTOR V, 
    XMVECTOR LengthMin, 
    XMVECTOR LengthMax
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR RcpLength;
    XMVECTOR Length;
    XMVECTOR Normal;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR ControlMax;
    XMVECTOR ControlMin;
    XMVECTOR Control;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x) && (LengthMin.z == LengthMin.x) && (LengthMin.w == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x) && (LengthMax.z == LengthMax.x) && (LengthMax.w == LengthMax.x));
    XMASSERT(XMVector4GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector4GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector4GreaterOrEqual(LengthMax, LengthMin));

    LengthSq = XMVector4LengthSq(V);

    Zero = XMVectorZero();

    RcpLength = XMVectorReciprocalSqrt(LengthSq);

    InfiniteLength = XMVectorEqualInt(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = XMVectorEqual(LengthSq, Zero);

    Normal = XMVectorMultiply(V, RcpLength);

    Length = XMVectorMultiply(LengthSq, RcpLength);

    Select = XMVectorEqualInt(InfiniteLength, ZeroLength);
    Length = XMVectorSelect(LengthSq, Length, Select);
    Normal = XMVectorSelect(LengthSq, Normal, Select);

    ControlMax = XMVectorGreater(Length, LengthMax);
    ControlMin = XMVectorLess(Length, LengthMin);

    ClampLength = XMVectorSelect(Length, LengthMax, ControlMax);
    ClampLength = XMVectorSelect(ClampLength, LengthMin, ControlMin);

    Result = XMVectorMultiply(Normal, ClampLength);

    // Preserve the original vector (with no precision loss) if the length falls within the given range
    Control = XMVectorEqualInt(ControlMax, ControlMin);
    Result = XMVectorSelect(Result, V, Control);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR RcpLength;
    XMVECTOR Length;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Normal;
    XMVECTOR Zero;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR ControlMax;
    XMVECTOR ControlMin;
    XMVECTOR Control;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x) && (LengthMin.z == LengthMin.x) && (LengthMin.w == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x) && (LengthMax.z == LengthMax.x) && (LengthMax.w == LengthMax.x));
    XMASSERT(XMVector4GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector4GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector4GreaterOrEqual(LengthMax, LengthMin));

    D0 = __vmaddfp(V, V, *(XMVECTOR*)g_XMNegativeZero);

    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);

    D1 = __vsldoi(D0, D0, 4);
    LengthSq = __vaddfp(D0, D1);

    Zero = __vspltisw(0);

    Rsq = __vrsqrtefp(LengthSq);
    RT = __vmaddfp(LengthSq, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    RcpLength = __vmaddfp(Rsq, RT, Rsq);

    InfiniteLength = __vcmpequw(LengthSq, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(LengthSq, Zero);

    Length = __vmaddfp(LengthSq, RcpLength, *(XMVECTOR*)g_XMNegativeZero);

    Normal = __vmaddfp(V, RcpLength, *(XMVECTOR*)g_XMNegativeZero);

    Select = __vcmpequw(InfiniteLength, ZeroLength);
    Length = __vsel(LengthSq, Length, Select);
    Normal = __vsel(LengthSq, Normal, Select);

    ControlMax = __vcmpgtfp(Length, LengthMax);
    ControlMin = __vcmpgtfp(LengthMin, Length);

    ClampLength = __vsel(Length, LengthMax, ControlMax);
    ClampLength = __vsel(ClampLength, LengthMin, ControlMin);

    Result = __vmaddfp(Normal, ClampLength, *(XMVECTOR*)g_XMNegativeZero);

    Control = __vcmpequw(ControlMax, ControlMin);

    Result = __vsel(Result, V, Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR ClampLength;
    XMVECTOR LengthSq;
    XMVECTOR Length, LengthS;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR Normal, NormalS;
    XMVECTOR Zero;
    XMVECTOR Control;
    XMVECTOR H;
    XMVECTOR Result;

    XMASSERT((LengthMin.y == LengthMin.x) && (LengthMin.z == LengthMin.x) && (LengthMin.w == LengthMin.x));
    XMASSERT((LengthMax.y == LengthMax.x) && (LengthMax.z == LengthMax.x) && (LengthMax.w == LengthMax.x));
    XMASSERT(XMVector4GreaterOrEqual(LengthMin, XMVectorZero()));
    XMASSERT(XMVector4GreaterOrEqual(LengthMax, XMVectorZero()));
    XMASSERT(XMVector4GreaterOrEqual(LengthMax, LengthMin));

    H = __vspltisw(1);

    LengthSq = __vmsum4fp(V, V);

    H = __vcfsx(H, 1);

    Rsq = __vrsqrtefp(LengthSq);
    RT = __vmulfp(LengthSq, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    H = __vmaddcfp(Rsq, H, Rsq);

    Zero = __vspltisw(0);

    Length = __vmulfp(LengthSq, H);

    Normal = __vmulfp(V, H);

    LengthS = __vcmpeqfp(LengthSq, Zero);
    NormalS = __vcmpeqfp(LengthSq, Zero);

    LengthS = __vsel(Length, LengthSq, LengthS);
    NormalS = __vsel(Normal, LengthSq, NormalS);

    Control = __vcmpgtfp(LengthS, LengthMax);
    ClampLength = __vcmpgtfp(LengthMin, LengthS);

    Result = __vcmpequw(Control, ClampLength);

    Control = __vsel(LengthS, LengthMax, Control);
    ClampLength = __vsel(Control, LengthMin, ClampLength);

    RT = __vmulfp(NormalS, ClampLength);

    Result = __vsel(RT, V, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4Reflect
(
    XMVECTOR Incident, 
    XMVECTOR Normal
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    // Result = Incident - (2 * dot(Incident, Normal)) * Normal
    Result = XMVector4Dot(Incident, Normal);
    Result = XMVectorAdd(Result, Result);
    Result = XMVectorNegativeMultiplySubtract(Result, Normal, Incident);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(Incident, Normal, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    Result = __vaddfp(D0, D1);
    Result = __vaddfp(Result, Result);
    Result = __vnmsubfp(Result, Normal, Incident);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D;
    XMVECTOR Result;

    D = __vmsum4fp(Incident, Normal);
    D = __vaddfp(D, D);
    Result = Incident;
    Result = __vnmsubfp(D, Normal, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4Refract
(
    XMVECTOR Incident, 
    XMVECTOR Normal, 
    FLOAT    RefractionIndex
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Index;

    Index = XMVectorReplicate(RefractionIndex);

    return XMVector4RefractV(Incident, Normal, Index);

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR Index;

    Index.v[0] =
    Index.v[1] = 
    Index.v[2] =
    Index.v[3] = RefractionIndex;

    return XMVector4RefractV(Incident, Normal, Index);

#else // _VMX128_INTRINSICS_

    XMVECTOR Index;

    XMASSERT(((UINT_PTR)&RefractionIndex & 3) == 0);

    Index = __lvlx(&RefractionIndex, 0);
    Index = __vspltw(Index, 0);

    return XMVector4RefractV(Incident, Normal, Index);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4RefractV
(
    XMVECTOR Incident, 
    XMVECTOR Normal, 
    XMVECTOR RefractionIndex
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR        IDotN;
    XMVECTOR        R;
    CONST XMVECTOR  Zero = XMVectorZero();

    // Result = RefractionIndex * Incident - Normal * (RefractionIndex * dot(Incident, Normal) + 
    // sqrt(1 - RefractionIndex * RefractionIndex * (1 - dot(Incident, Normal) * dot(Incident, Normal))))

    IDotN = XMVector4Dot(Incident, Normal);

    // R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
    R = XMVectorNegativeMultiplySubtract(IDotN, IDotN, g_XMOne);
    R = XMVectorMultiply(R, RefractionIndex);
    R = XMVectorNegativeMultiplySubtract(R, RefractionIndex, g_XMOne);

    if (XMVector4LessOrEqual(R, Zero))
    {
        // Total internal reflection
        return Zero;
    }
    else
    {
        XMVECTOR Result;

        // R = RefractionIndex * IDotN + sqrt(R)
        R = XMVectorSqrt(R);
        R = XMVectorMultiplyAdd(RefractionIndex, IDotN, R);

        // Result = RefractionIndex * Incident - Normal * R
        Result = XMVectorMultiply(RefractionIndex, Incident);
        Result = XMVectorNegativeMultiplySubtract(Normal, R, Result);

        return Result;
    }

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR IDotN;
    XMVECTOR R;
    XMVECTOR I;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR InfiniteLength;
    XMVECTOR ZeroLength;
    XMVECTOR Select;
    XMVECTOR Control;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Zero;
    XMVECTOR Result;

    D0 = __vmaddfp(Incident, Normal, *(XMVECTOR*)g_XMNegativeZero);

    Zero = __vspltisw(0);

    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);

    D1 = __vsldoi(D0, D0, 4);
    IDotN = __vaddfp(D0, D1);

    R = __vnmsubfp(IDotN, IDotN, g_XMOne);
    R = __vmaddfp(R, RefractionIndex, *(XMVECTOR*)g_XMNegativeZero);
    R = __vnmsubfp(R, RefractionIndex, g_XMOne);

    Rsq = __vrsqrtefp(R);
    RT = __vmaddfp(R, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp = __vmaddfp(Rsq, Rsq, *(XMVECTOR*)g_XMNegativeZero);
    RT = __vnmsubfp(RT, Rcp, g_XMOneHalf);
    D0 = __vmaddfp(Rsq, RT, Rsq);

    InfiniteLength = __vcmpequw(R, *(XMVECTOR*)g_XMInfinity);
    ZeroLength = __vcmpeqfp(R, Zero);

    Control = __vcmpgefp(Zero, R);

    D1 = __vmaddfp(R, D0, *(XMVECTOR*)g_XMNegativeZero);

    Select = __vcmpequw(InfiniteLength, ZeroLength);

    I = __vmaddfp(RefractionIndex, Incident, *(XMVECTOR*)g_XMNegativeZero);

    R = __vsel(R, D1, Select);

    R = __vmaddfp(RefractionIndex, IDotN, R);

    Result = __vnmsubfp(Normal, R, I);

    Result = __vsel(Result, Zero, Control);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR IDotN;
    XMVECTOR R;
    XMVECTOR I;
    XMVECTOR Rsq;
    XMVECTOR Rcp;
    XMVECTOR RT;
    XMVECTOR ZL;
    XMVECTOR D1;
    XMVECTOR H;
    XMVECTOR Zero;
    XMVECTOR Result;

    XMDUMMY_INITIALIZE_VECTOR(R);

    IDotN = __vmsum4fp(Incident, Normal);

    H = __vspltisw(1);
    R = __vupkd3d(R, VPACK_NORMSHORT2);
    H = __vcfsx(H, 1);
    RT = __vspltw(R, 3);
    R = __vspltw(R, 3);

    RT = __vnmsubfp(IDotN, IDotN, RT);
    RT = __vmulfp(RT, RefractionIndex);
    R = __vnmsubfp(RT, RefractionIndex, R);

    Rsq = __vrsqrtefp(R);
    RT = __vmulfp(R, H);
    Rcp = __vmulfp(Rsq, Rsq);
    H = __vnmsubfp(RT, Rcp, H);
    Zero = __vspltisw(0);
    H = __vmaddcfp(Rsq, H, Rsq);

    ZL = __vcmpeqfp(R, Zero);

    Result = __vcmpgefp(Zero, R);

    D1 = __vmulfp(R, H);

    I = __vmulfp(RefractionIndex, Incident);

    ZL = __vsel(D1, R, ZL);

    ZL = __vmaddfp(RefractionIndex, IDotN, ZL);

    I = __vnmsubfp(Normal, ZL, I);

    Result = __vsel(I, Zero, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4Orthogonal
(
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result.v[0] = V.v[2];
    Result.v[1] = V.v[3];
    Result.v[2] = -V.v[0];
    Result.v[3] = -V.v[1];

    return Result;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR NegativeV;
    XMVECTOR SignMask;
    XMVECTOR Result;

    SignMask = __vspltisw(-1);
    SignMask = __vslw(SignMask, SignMask);

    NegativeV = __vxor(V, SignMask);

    Result = __vsldoi(V, NegativeV, 2 << 2);

    return Result;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4AngleBetweenNormalsEst
(
    XMVECTOR N1, 
    XMVECTOR N2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector4Dot(N1, N2);
    Result = XMVectorACosEst(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR CosAngle;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(N1, N2, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    CosAngle = __vaddfp(D0, D1);

    Result = XMVectorACosEst(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR CosAngle;
    XMVECTOR Result;

    CosAngle = __vmsum4fp(N1, N2);
    Result = XMVectorACosEst(CosAngle);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4AngleBetweenNormals
(
    XMVECTOR N1, 
    XMVECTOR N2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR Result;

    Result = XMVector4Dot(N1, N2);
    Result = XMVectorACos(Result);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR CosAngle;
    XMVECTOR D0;
    XMVECTOR D1;
    XMVECTOR Result;

    D0 = __vmaddfp(N1, N2, *(XMVECTOR*)g_XMNegativeZero);
    D1 = __vsldoi(D0, D0, 8);
    D0 = __vaddfp(D0, D1);
    D1 = __vsldoi(D0, D0, 4);
    CosAngle = __vaddfp(D0, D1);

    Result = XMVectorACos(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR Result;

    Result = __vmsum4fp(N1, N2);
    Result = XMVectorACos(Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4AngleBetweenVectors
(
    XMVECTOR V1, 
    XMVECTOR V2
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR L1;
    XMVECTOR L2;
    XMVECTOR Dot;
    XMVECTOR CosAngle;
    XMVECTOR Result;

    L1 = XMVector4ReciprocalLength(V1);
    L2 = XMVector4ReciprocalLength(V2);

    Dot = XMVector4Dot(V1, V2);

    L1 = XMVectorMultiply(L1, L2);

    CosAngle = XMVectorMultiply(Dot, L1);

    Result = XMVectorACos(CosAngle);

    return Result;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR L1;
    XMVECTOR L2;
    XMVECTOR Dot;
    XMVECTOR CosAngle;
    XMVECTOR L1A, L1B, L2A, L2B, DotA, DotB;
    XMVECTOR Rsq1, Rsq2, Rcp1, Rcp2, RT1, RT2;
    XMVECTOR Result;

//    XMASSERT(!XMVector4NearEqual(XMVector2LengthSq(V1), XMVectorZero(), g_XMEpsilon));
//    XMASSERT(!XMVector4NearEqual(XMVector2LengthSq(V2), XMVectorZero(), g_XMEpsilon));

    L1A = __vmaddfp(V1, V1, *(XMVECTOR*)g_XMNegativeZero);
    L2A = __vmaddfp(V2, V2, *(XMVECTOR*)g_XMNegativeZero);
    DotA = __vmaddfp(V1, V2, *(XMVECTOR*)g_XMNegativeZero);

    L1B = __vsldoi(L1A, L1A, 8);
    L2B = __vsldoi(L2A, L2A, 8);
    DotB = __vsldoi(DotA, DotA, 8);

    L1A = __vaddfp(L1A, L1B);
    L2A = __vaddfp(L2A, L2B);
    DotA = __vaddfp(DotA, DotB);

    L1B = __vsldoi(L1A, L1A, 4);
    L2B = __vsldoi(L2A, L2A, 4);
    DotB = __vsldoi(DotA, DotA, 4);

    L1A = __vaddfp(L1A, L1B);
    L2A = __vaddfp(L2A, L2B);
    Dot = __vaddfp(DotA, DotB);

    Rsq1 = __vrsqrtefp(L1A);
    Rsq2 = __vrsqrtefp(L2A);
    RT1 = __vmaddfp(L1A, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    RT2 = __vmaddfp(L2A, g_XMOneHalf, *(XMVECTOR*)g_XMNegativeZero);
    Rcp1 = __vmaddfp(Rsq1, Rsq1, *(XMVECTOR*)g_XMNegativeZero);
    Rcp2 = __vmaddfp(Rsq2, Rsq2, *(XMVECTOR*)g_XMNegativeZero);
    RT1 = __vnmsubfp(RT1, Rcp1, g_XMOneHalf);
    RT2 = __vnmsubfp(RT2, Rcp2, g_XMOneHalf);
    L1 = __vmaddfp(Rsq1, RT1, Rsq1);
    L2 = __vmaddfp(Rsq2, RT2, Rsq2);

    L1 = __vmaddfp(L1, L2, *(XMVECTOR*)g_XMNegativeZero);

    CosAngle = __vmaddfp(Dot, L1, *(XMVECTOR*)g_XMNegativeZero);

    Result = XMVectorACos(CosAngle);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR D1, D2;
    XMVECTOR Rsq1, Rsq2;
    XMVECTOR Rcp1, Rcp2;
    XMVECTOR RT1, RT2;
    XMVECTOR H1, H2;
    XMVECTOR L;
    XMVECTOR Dot;
    XMVECTOR CosAngle;
    XMVECTOR Result;

    D1 = __vmsum4fp(V1, V1);
    D2 = __vmsum4fp(V2, V2);
    H2 = __vspltisw(1);
    H1 = __vcfsx(H2, 1);
    H2 = __vcfsx(H2, 1);
    Rsq1 = __vrsqrtefp(D1);
    Rsq2 = __vrsqrtefp(D2);
    RT1 = __vmulfp(D1, H1);
    RT2 = __vmulfp(D2, H2);
    Rcp1 = __vmulfp(Rsq1, Rsq1);
    Rcp2 = __vmulfp(Rsq2, Rsq2);
    H1 = __vnmsubfp(RT1, Rcp1, H1);
    H2 = __vnmsubfp(RT2, Rcp2, H2);
    Rsq1 = __vmaddfp(Rsq1, H1, Rsq1);
    Rsq2 = __vmaddfp(Rsq2, H2, Rsq2);
    Dot = __vmsum4fp(V1, V2);
    L = __vmulfp(Rsq1, Rsq2);
    CosAngle = __vmulfp(Dot, L);

    Result = XMVectorACos(CosAngle);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMVector4Transform
(
    XMVECTOR V, 
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR X;
    XMVECTOR Y;
    XMVECTOR Z;
    XMVECTOR W;
    XMVECTOR Result;

    W = XMVectorSplatW(V);
    Z = XMVectorSplatZ(V);
    Y = XMVectorSplatY(V);
    X = XMVectorSplatX(V);

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

    W = __vspltw(V, 3);
    Z = __vspltw(V, 2);
    Y = __vspltw(V, 1);
    X = __vspltw(V, 0);

    Result = __vmaddfp(W, M.r[3], *(XMVECTOR*)g_XMNegativeZero);
    Result = __vmaddfp(Z, M.r[2], Result);
    Result = __vmaddfp(Y, M.r[1], Result);
    Result = __vmaddfp(X, M.r[0], Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMMATRIX P, MT;
    XMVECTOR R0, R1, R2, R3, R0R2, R1R3;
    XMVECTOR Result;

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    R0 = __vmsum4fp(V, MT.r[0]);
    R1 = __vmsum4fp(V, MT.r[1]);
    R2 = __vmsum4fp(V, MT.r[2]);
    R3 = __vmsum4fp(V, MT.r[3]);

    R0R2 = __vmrghw(R0, R2);
    R1R3 = __vmrghw(R1, R3);

    Result = __vmrghw(R0R2, R1R3);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMINLINE XMFLOAT4* XMVector4TransformStream
(
    XMFLOAT4*       pOutputStream, 
    UINT            OutputStride, 
    CONST XMFLOAT4* pInputStream, 
    UINT            InputStride, 
    UINT            VectorCount, 
    XMMATRIX M
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;
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

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat4((XMFLOAT4*)pInputVector);
        W = XMVectorSplatW(V);
        Z = XMVectorSplatZ(V);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
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

    XMVECTOR V;
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

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat4((XMFLOAT4*)pInputVector);
        W = XMVectorSplatW(V);
        Z = XMVectorSplatZ(V);
        Y = XMVectorSplatY(V);
        X = XMVectorSplatX(V);
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

    XMMATRIX P, MT;
    XMVECTOR R0, R1, R2, R3, R0R2, R1R3;
    XMVECTOR V;
    XMVECTOR Result;
    UINT     i;
    BYTE*    pInputVector = (BYTE*)pInputStream;
    BYTE*    pOutputVector = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    P.r[0] = __vmrghw(M.r[0], M.r[2]);
    P.r[1] = __vmrghw(M.r[1], M.r[3]);
    P.r[2] = __vmrglw(M.r[0], M.r[2]);
    P.r[3] = __vmrglw(M.r[1], M.r[3]);

    MT.r[0] = __vmrghw(P.r[0], P.r[1]);
    MT.r[1] = __vmrglw(P.r[0], P.r[1]);
    MT.r[2] = __vmrghw(P.r[2], P.r[3]);
    MT.r[3] = __vmrglw(P.r[2], P.r[3]);

    for (i = 0; i < VectorCount; i++)
    {
        V = XMLoadFloat4((XMFLOAT4*)pInputVector);

        R0 = __vmsum4fp(V, MT.r[0]);
        R1 = __vmsum4fp(V, MT.r[1]);
        R2 = __vmsum4fp(V, MT.r[2]);
        R3 = __vmsum4fp(V, MT.r[3]);

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

/****************************************************************************
 *
 * Compatibility defines
 *
 ****************************************************************************/

XMFINLINE XMVECTOR XMVectorMinimize(XMVECTOR V1, XMVECTOR V2) { return XMVectorMin(V1, V2); }
XMFINLINE XMVECTOR XMVectorMaximize(XMVECTOR V1, XMVECTOR V2) { return XMVectorMax(V1, V2); }

#ifdef __cplusplus

/****************************************************************************
 *
 * XMVECTOR operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator+ (XMVECTOR V)
{
    return V;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator- (XMVECTOR V)
{
    return XMVectorNegate(V);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR& operator+=
(
    XMVECTOR&       V1,
    CONST XMVECTOR  V2
)
{
    V1 = XMVectorAdd(V1, V2);
    return V1;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR& operator-=
(
    XMVECTOR&       V1,
    CONST XMVECTOR  V2
)
{
    V1 = XMVectorSubtract(V1, V2);
    return V1;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR& operator*=
(
    XMVECTOR&       V1,
    CONST XMVECTOR  V2
)
{
    V1 = XMVectorMultiply(V1, V2);
    return V1;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR& operator/=
(
    XMVECTOR&       V1,
    CONST XMVECTOR  V2
)
{
    XMVECTOR InvV = XMVectorReciprocal(V2);
    V1 = XMVectorMultiply(V1, InvV);
    return V1;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR& operator*=
(
    XMVECTOR&   V,
    CONST FLOAT S
)
{
    V = XMVectorScale(V, S);
    return V;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR& operator/=
(
    XMVECTOR&   V,
    CONST FLOAT S
)
{
    V = XMVectorScale(V, 1.0f / S);
    return V;
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator+
(
    CONST XMVECTOR V1,
    CONST XMVECTOR V2
)
{
    return XMVectorAdd(V1, V2);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator-
(
    CONST XMVECTOR V1,
    CONST XMVECTOR V2
)
{
    return XMVectorSubtract(V1, V2);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator*
(
    CONST XMVECTOR V1,
    CONST XMVECTOR V2
)
{
    return XMVectorMultiply(V1, V2);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator/
(
    CONST XMVECTOR V1,
    CONST XMVECTOR V2
)
{
    XMVECTOR InvV = XMVectorReciprocal(V2);
    return XMVectorMultiply(V1, InvV);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator*
(
    CONST XMVECTOR V,
    FLOAT          S
)
{
    return XMVectorScale(V, S);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator/
(
    CONST XMVECTOR V,
    FLOAT          S
)
{
    return XMVectorScale(V, 1.0f / S);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR operator*
(
    FLOAT           S,
    CONST XMVECTOR  V
)
{
    return XMVectorScale(V, S);
}

/****************************************************************************
 *
 * XMFLOAT2 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT2::_XMFLOAT2
(
    FLOAT _x,
    FLOAT _y
)
{
    x = _x;
    y = _y;
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT2::_XMFLOAT2
(
    CONST FLOAT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT2& _XMFLOAT2::operator=
(
    CONST _XMFLOAT2& Float2
)
{
    x = Float2.x;
    y = Float2.y;
    return *this;
}

/****************************************************************************
 *
 * XMHALF2 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMHALF2::_XMHALF2
(
    HALF _x,
    HALF _y
)
{
    x = _x;
    y = _y;
}

//------------------------------------------------------------------------------

XMFINLINE _XMHALF2::_XMHALF2
(
    CONST HALF* pArray
)
{
    x = pArray[0];
    y = pArray[1];
}

//------------------------------------------------------------------------------

XMFINLINE _XMHALF2::_XMHALF2
(
    FLOAT _x,
    FLOAT _y
)
{
    x = XMConvertFloatToHalf(_x);
    y = XMConvertFloatToHalf(_y);
}

//------------------------------------------------------------------------------

XMFINLINE _XMHALF2::_XMHALF2
(
    CONST FLOAT* pArray
)
{
    x = XMConvertFloatToHalf(pArray[0]);
    y = XMConvertFloatToHalf(pArray[1]);
}

//------------------------------------------------------------------------------

XMFINLINE _XMHALF2& _XMHALF2::operator=
(
    CONST _XMHALF2& Half2
)
{
    x = Half2.x;
    y = Half2.y;
    return *this;
}

/****************************************************************************
 *
 * XMSHORTN2 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN2::_XMSHORTN2
(
    SHORT _x,
    SHORT _y
)
{
    x = _x;
    y = _y;
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN2::_XMSHORTN2
(
    CONST SHORT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN2::_XMSHORTN2
(
    FLOAT _x,
    FLOAT _y
)
{
    XMStoreShortN2(this, XMVectorSet(_x, _y, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN2::_XMSHORTN2
(
    CONST FLOAT* pArray
)
{
    XMStoreShortN2(this, XMLoadFloat2((XMFLOAT2*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN2& _XMSHORTN2::operator=
(
    CONST _XMSHORTN2& ShortN2
)
{
    x = ShortN2.x;
    y = ShortN2.y;
    return *this;
}

/****************************************************************************
 *
 * XMSHORT2 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT2::_XMSHORT2
(
    SHORT _x,
    SHORT _y
)
{
    x = _x;
    y = _y;
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT2::_XMSHORT2
(
    CONST SHORT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT2::_XMSHORT2
(
    FLOAT _x,
    FLOAT _y
)
{
    XMStoreShort2(this, XMVectorSet(_x, _y, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT2::_XMSHORT2
(
    CONST FLOAT* pArray
)
{
    XMStoreShort2(this, XMLoadFloat2((XMFLOAT2*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT2& _XMSHORT2::operator=
(
    CONST _XMSHORT2& Short2
)
{
    x = Short2.x;
    y = Short2.y;
    return *this;
}

/****************************************************************************
 *
 * XMUSHORTN2 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN2::_XMUSHORTN2
(
    USHORT _x,
    USHORT _y
)
{
    x = _x;
    y = _y;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN2::_XMUSHORTN2
(
    CONST USHORT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN2::_XMUSHORTN2
(
    FLOAT _x,
    FLOAT _y
)
{
    XMStoreUShortN2(this, XMVectorSet(_x, _y, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN2::_XMUSHORTN2
(
    CONST FLOAT* pArray
)
{
    XMStoreUShortN2(this, XMLoadFloat2((XMFLOAT2*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN2& _XMUSHORTN2::operator=
(
    CONST _XMUSHORTN2& UShortN2
)
{
    x = UShortN2.x;
    y = UShortN2.y;
    return *this;
}

/****************************************************************************
 *
 * XMUSHORT2 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT2::_XMUSHORT2
(
    USHORT _x,
    USHORT _y
)
{
    x = _x;
    y = _y;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT2::_XMUSHORT2
(
    CONST USHORT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT2::_XMUSHORT2
(
    FLOAT _x,
    FLOAT _y
)
{
    XMStoreUShort2(this, XMVectorSet(_x, _y, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT2::_XMUSHORT2
(
    CONST FLOAT* pArray
)
{
    XMStoreUShort2(this, XMLoadFloat2((XMFLOAT2*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT2& _XMUSHORT2::operator=
(
    CONST _XMUSHORT2& UShort2
)
{
    x = UShort2.x;
    y = UShort2.y;
    return *this;
}

/****************************************************************************
 *
 * XMFLOAT3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT3::_XMFLOAT3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    x = _x;
    y = _y;
    z = _z;
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT3::_XMFLOAT3
(
    CONST FLOAT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT3& _XMFLOAT3::operator=
(
    CONST _XMFLOAT3& Float3
)
{
    x = Float3.x;
    y = Float3.y;
    z = Float3.z;
    return *this;
}

/****************************************************************************
 *
 * XMHENDN3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMHENDN3::_XMHENDN3
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMHENDN3::_XMHENDN3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    XMStoreHenDN3(this, XMVectorSet(_x, _y, _z, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMHENDN3::_XMHENDN3
(
    CONST FLOAT* pArray
)
{
    XMStoreHenDN3(this, XMLoadFloat3((XMFLOAT3*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMHENDN3::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMHENDN3& _XMHENDN3::operator=
(
    CONST _XMHENDN3& HenDN3
)
{
    v = HenDN3.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMHENDN3& _XMHENDN3::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMHEND3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMHEND3::_XMHEND3
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMHEND3::_XMHEND3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    XMStoreHenD3(this, XMVectorSet(_x, _y, _z, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMHEND3::_XMHEND3
(
    CONST FLOAT* pArray
)
{
    XMStoreHenD3(this, XMLoadFloat3((XMFLOAT3*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMHEND3::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMHEND3& _XMHEND3::operator=
(
    CONST _XMHEND3& HenD3
)
{
    v = HenD3.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMHEND3& _XMHEND3::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMUHENDN3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUHENDN3::_XMUHENDN3
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHENDN3::_XMUHENDN3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    XMStoreUHenDN3(this, XMVectorSet(_x, _y, _z, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHENDN3::_XMUHENDN3
(
    CONST FLOAT* pArray
)
{
    XMStoreUHenDN3(this, XMLoadFloat3((XMFLOAT3*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHENDN3::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHENDN3& _XMUHENDN3::operator=
(
    CONST _XMUHENDN3& UHenDN3
)
{
    v = UHenDN3.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHENDN3& _XMUHENDN3::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMUHEND3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUHEND3::_XMUHEND3
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHEND3::_XMUHEND3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    XMStoreUHenD3(this, XMVectorSet(_x, _y, _z, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHEND3::_XMUHEND3
(
    CONST FLOAT* pArray
)
{
    XMStoreUHenD3(this, XMLoadFloat3((XMFLOAT3*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHEND3::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHEND3& _XMUHEND3::operator=
(
    CONST _XMUHEND3& UHenD3
)
{
    v = UHenD3.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUHEND3& _XMUHEND3::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMDHENN3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMDHENN3::_XMDHENN3
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHENN3::_XMDHENN3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    XMStoreDHenN3(this, XMVectorSet(_x, _y, _z, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHENN3::_XMDHENN3
(
    CONST FLOAT* pArray
)
{
    XMStoreDHenN3(this, XMLoadFloat3((XMFLOAT3*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHENN3::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHENN3& _XMDHENN3::operator=
(
    CONST _XMDHENN3& DHenN3
)
{
    v = DHenN3.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHENN3& _XMDHENN3::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMDHEN3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMDHEN3::_XMDHEN3
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHEN3::_XMDHEN3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    XMStoreDHen3(this, XMVectorSet(_x, _y, _z, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHEN3::_XMDHEN3
(
    CONST FLOAT* pArray
)
{
    XMStoreDHen3(this, XMLoadFloat3((XMFLOAT3*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHEN3::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHEN3& _XMDHEN3::operator=
(
    CONST _XMDHEN3& DHen3
)
{
    v = DHen3.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDHEN3& _XMDHEN3::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMUDHENN3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUDHENN3::_XMUDHENN3
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHENN3::_XMUDHENN3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    XMStoreUDHenN3(this, XMVectorSet(_x, _y, _z, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHENN3::_XMUDHENN3
(
    CONST FLOAT* pArray
)
{
    XMStoreUDHenN3(this, XMLoadFloat3((XMFLOAT3*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHENN3::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHENN3& _XMUDHENN3::operator=
(
    CONST _XMUDHENN3& UDHenN3
)
{
    v = UDHenN3.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHENN3& _XMUDHENN3::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMUDHEN3 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUDHEN3::_XMUDHEN3
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHEN3::_XMUDHEN3
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z
)
{
    XMStoreUDHen3(this, XMVectorSet(_x, _y, _z, 0.0f));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHEN3::_XMUDHEN3
(
    CONST FLOAT* pArray
)
{
    XMStoreUDHen3(this, XMLoadFloat3((XMFLOAT3*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHEN3::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHEN3& _XMUDHEN3::operator=
(
    CONST _XMUDHEN3& UDHen3
)
{
    v = UDHen3.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDHEN3& _XMUDHEN3::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMFLOAT4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4::_XMFLOAT4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4::_XMFLOAT4
(
    CONST FLOAT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMFLOAT4& _XMFLOAT4::operator=
(
    CONST _XMFLOAT4& Float4
)
{
    x = Float4.x;
    y = Float4.y;
    z = Float4.z;
    w = Float4.w;
    return *this;
}

/****************************************************************************
 *
 * XMHALF4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMHALF4::_XMHALF4
(
    HALF _x,
    HALF _y,
    HALF _z,
    HALF _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMHALF4::_XMHALF4
(
    CONST HALF* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMHALF4::_XMHALF4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    x = XMConvertFloatToHalf(_x);
    y = XMConvertFloatToHalf(_y);
    z = XMConvertFloatToHalf(_z);
    w = XMConvertFloatToHalf(_w);
}

//------------------------------------------------------------------------------

XMFINLINE _XMHALF4::_XMHALF4
(
    CONST FLOAT* pArray
)
{
    XMConvertFloatToHalfStream(&x, sizeof(HALF), pArray, sizeof(FLOAT), 4);
}

//------------------------------------------------------------------------------

XMFINLINE _XMHALF4& _XMHALF4::operator=
(
    CONST _XMHALF4& Half4
)
{
    x = Half4.x;
    y = Half4.y;
    z = Half4.z;
    w = Half4.w;
    return *this;
}

/****************************************************************************
 *
 * XMSHORTN4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN4::_XMSHORTN4
(
    SHORT _x,
    SHORT _y,
    SHORT _z,
    SHORT _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN4::_XMSHORTN4
(
    CONST SHORT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN4::_XMSHORTN4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreShortN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN4::_XMSHORTN4
(
    CONST FLOAT* pArray
)
{
    XMStoreShortN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORTN4& _XMSHORTN4::operator=
(
    CONST _XMSHORTN4& ShortN4
)
{
    x = ShortN4.x;
    y = ShortN4.y;
    z = ShortN4.z;
    w = ShortN4.w;
    return *this;
}

/****************************************************************************
 *
 * XMSHORT4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT4::_XMSHORT4
(
    SHORT _x,
    SHORT _y,
    SHORT _z,
    SHORT _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT4::_XMSHORT4
(
    CONST SHORT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT4::_XMSHORT4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreShort4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT4::_XMSHORT4
(
    CONST FLOAT* pArray
)
{
    XMStoreShort4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMSHORT4& _XMSHORT4::operator=
(
    CONST _XMSHORT4& Short4
)
{
    x = Short4.x;
    y = Short4.y;
    z = Short4.z;
    w = Short4.w;
    return *this;
}

/****************************************************************************
 *
 * XMUSHORTN4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN4::_XMUSHORTN4
(
    USHORT _x,
    USHORT _y,
    USHORT _z,
    USHORT _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN4::_XMUSHORTN4
(
    CONST USHORT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN4::_XMUSHORTN4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreUShortN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN4::_XMUSHORTN4
(
    CONST FLOAT* pArray
)
{
    XMStoreUShortN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORTN4& _XMUSHORTN4::operator=
(
    CONST _XMUSHORTN4& UShortN4
)
{
    x = UShortN4.x;
    y = UShortN4.y;
    z = UShortN4.z;
    w = UShortN4.w;
    return *this;
}

/****************************************************************************
 *
 * XMUSHORT4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT4::_XMUSHORT4
(
    USHORT _x,
    USHORT _y,
    USHORT _z,
    USHORT _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT4::_XMUSHORT4
(
    CONST USHORT* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT4::_XMUSHORT4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreUShort4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT4::_XMUSHORT4
(
    CONST FLOAT* pArray
)
{
    XMStoreUShort4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUSHORT4& _XMUSHORT4::operator=
(
    CONST _XMUSHORT4& UShort4
)
{
    x = UShort4.x;
    y = UShort4.y;
    z = UShort4.z;
    w = UShort4.w;
    return *this;
}

/****************************************************************************
 *
 * XMXDECN4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMXDECN4::_XMXDECN4
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDECN4::_XMXDECN4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreXDecN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDECN4::_XMXDECN4
(
    CONST FLOAT* pArray
)
{
    XMStoreXDecN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDECN4::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDECN4& _XMXDECN4::operator=
(
    CONST _XMXDECN4& XDecN4
)
{
    v = XDecN4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDECN4& _XMXDECN4::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMXDEC4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMXDEC4::_XMXDEC4
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDEC4::_XMXDEC4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreXDec4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDEC4::_XMXDEC4
(
    CONST FLOAT* pArray
)
{
    XMStoreXDec4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDEC4::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDEC4& _XMXDEC4::operator=
(
    CONST _XMXDEC4& XDec4
)
{
    v = XDec4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXDEC4& _XMXDEC4::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMDECN4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMDECN4::_XMDECN4
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDECN4::_XMDECN4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreDecN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMDECN4::_XMDECN4
(
    CONST FLOAT* pArray
)
{
    XMStoreDecN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMDECN4::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDECN4& _XMDECN4::operator=
(
    CONST _XMDECN4& DecN4
)
{
    v = DecN4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDECN4& _XMDECN4::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMDEC4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMDEC4::_XMDEC4
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDEC4::_XMDEC4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreDec4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMDEC4::_XMDEC4
(
    CONST FLOAT* pArray
)
{
    XMStoreDec4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMDEC4::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDEC4& _XMDEC4::operator=
(
    CONST _XMDEC4& Dec4
)
{
    v = Dec4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMDEC4& _XMDEC4::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMUDECN4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUDECN4::_XMUDECN4
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDECN4::_XMUDECN4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreUDecN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDECN4::_XMUDECN4
(
    CONST FLOAT* pArray
)
{
    XMStoreUDecN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDECN4::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDECN4& _XMUDECN4::operator=
(
    CONST _XMUDECN4& UDecN4
)
{
    v = UDecN4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDECN4& _XMUDECN4::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMUDEC4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUDEC4::_XMUDEC4
(
    UINT Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDEC4::_XMUDEC4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreUDec4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDEC4::_XMUDEC4
(
    CONST FLOAT* pArray
)
{
    XMStoreUDec4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDEC4::operator UINT ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDEC4& _XMUDEC4::operator=
(
    CONST _XMUDEC4& UDec4
)
{
    v = UDec4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUDEC4& _XMUDEC4::operator=
(
    CONST UINT Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMXICON4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMXICON4::_XMXICON4
(
    UINT64 Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICON4::_XMXICON4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreXIcoN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICON4::_XMXICON4
(
    CONST FLOAT* pArray
)
{
    XMStoreXIcoN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICON4::operator UINT64 ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICON4& _XMXICON4::operator=
(
    CONST _XMXICON4& XIcoN4
)
{
    v = XIcoN4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICON4& _XMXICON4::operator=
(
    CONST UINT64 Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMXICO4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMXICO4::_XMXICO4
(
    UINT64 Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICO4::_XMXICO4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreXIco4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICO4::_XMXICO4
(
    CONST FLOAT* pArray
)
{
    XMStoreXIco4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICO4::operator UINT64 ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICO4& _XMXICO4::operator=
(
    CONST _XMXICO4& XIco4
)
{
    v = XIco4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMXICO4& _XMXICO4::operator=
(
    CONST UINT64 Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMICON4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMICON4::_XMICON4
(
    UINT64 Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMICON4::_XMICON4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreIcoN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMICON4::_XMICON4
(
    CONST FLOAT* pArray
)
{
    XMStoreIcoN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMICON4::operator UINT64 ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMICON4& _XMICON4::operator=
(
    CONST _XMICON4& IcoN4
)
{
    v = IcoN4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMICON4& _XMICON4::operator=
(
    CONST UINT64 Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMICO4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMICO4::_XMICO4
(
    UINT64 Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMICO4::_XMICO4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreIco4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMICO4::_XMICO4
(
    CONST FLOAT* pArray
)
{
    XMStoreIco4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMICO4::operator UINT64 ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMICO4& _XMICO4::operator=
(
    CONST _XMICO4& Ico4
)
{
    v = Ico4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMICO4& _XMICO4::operator=
(
    CONST UINT64 Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMUICON4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUICON4::_XMUICON4
(
    UINT64 Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICON4::_XMUICON4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreUIcoN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICON4::_XMUICON4
(
    CONST FLOAT* pArray
)
{
    XMStoreUIcoN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICON4::operator UINT64 ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICON4& _XMUICON4::operator=
(
    CONST _XMUICON4& UIcoN4
)
{
    v = UIcoN4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICON4& _XMUICON4::operator=
(
    CONST UINT64 Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMUICO4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUICO4::_XMUICO4
(
    UINT64 Packed
)
{
    v = Packed;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICO4::_XMUICO4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreUIco4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICO4::_XMUICO4
(
    CONST FLOAT* pArray
)
{
    XMStoreUIco4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICO4::operator UINT64 ()
{
    return v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICO4& _XMUICO4::operator=
(
    CONST _XMUICO4& UIco4
)
{
    v = UIco4.v;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUICO4& _XMUICO4::operator=
(
    CONST UINT64 Packed
)
{
    v = Packed;
    return *this;
}

/****************************************************************************
 *
 * XMCOLOR4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMCOLOR::_XMCOLOR
(
    UINT Color
)
{
    c = Color;
}

//------------------------------------------------------------------------------

XMFINLINE _XMCOLOR::_XMCOLOR
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreColor(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMCOLOR::_XMCOLOR
(
    CONST FLOAT* pArray
)
{
    XMStoreColor(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMCOLOR::operator UINT ()
{
    return c;
}

//------------------------------------------------------------------------------

XMFINLINE _XMCOLOR& _XMCOLOR::operator=
(
    CONST _XMCOLOR& Color
)
{
    c = Color.c;
    return *this;
}

//------------------------------------------------------------------------------

XMFINLINE _XMCOLOR& _XMCOLOR::operator=
(
    CONST UINT Color
)
{
    c = Color;
    return *this;
}

/****************************************************************************
 *
 * XMBYTEN4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMBYTEN4::_XMBYTEN4
(
    CHAR _x,
    CHAR _y,
    CHAR _z,
    CHAR _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTEN4::_XMBYTEN4
(
    UINT _v
)
{
    v = _v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTEN4::_XMBYTEN4
(
    CONST CHAR* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTEN4::_XMBYTEN4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreByteN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTEN4::_XMBYTEN4
(
    CONST FLOAT* pArray
)
{
    XMStoreByteN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTEN4& _XMBYTEN4::operator=
(
    CONST _XMBYTEN4& ByteN4
)
{
    x = ByteN4.x;
    y = ByteN4.y;
    z = ByteN4.z;
    w = ByteN4.w;
    return *this;
}

/****************************************************************************
 *
 * XMBYTE4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMBYTE4::_XMBYTE4
(
    CHAR _x,
    CHAR _y,
    CHAR _z,
    CHAR _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTE4::_XMBYTE4
(
    UINT _v
)
{
    v = _v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTE4::_XMBYTE4
(
    CONST CHAR* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTE4::_XMBYTE4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreByte4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTE4::_XMBYTE4
(
    CONST FLOAT* pArray
)
{
    XMStoreByte4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMBYTE4& _XMBYTE4::operator=
(
    CONST _XMBYTE4& Byte4
)
{
    x = Byte4.x;
    y = Byte4.y;
    z = Byte4.z;
    w = Byte4.w;
    return *this;
}

/****************************************************************************
 *
 * XMUBYTEN4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTEN4::_XMUBYTEN4
(
    BYTE _x,
    BYTE _y,
    BYTE _z,
    BYTE _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTEN4::_XMUBYTEN4
(
    UINT _v
)
{
    v = _v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTEN4::_XMUBYTEN4
(
    CONST BYTE* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTEN4::_XMUBYTEN4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreUByteN4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTEN4::_XMUBYTEN4
(
    CONST FLOAT* pArray
)
{
    XMStoreUByteN4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTEN4& _XMUBYTEN4::operator=
(
    CONST _XMUBYTEN4& UByteN4
)
{
    x = UByteN4.x;
    y = UByteN4.y;
    z = UByteN4.z;
    w = UByteN4.w;
    return *this;
}

/****************************************************************************
 *
 * XMUBYTE4 operators
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTE4::_XMUBYTE4
(
    BYTE _x,
    BYTE _y,
    BYTE _z,
    BYTE _w
)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTE4::_XMUBYTE4
(
    UINT _v
)
{
    v = _v;
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTE4::_XMUBYTE4
(
    CONST BYTE* pArray
)
{
    x = pArray[0];
    y = pArray[1];
    z = pArray[2];
    w = pArray[3];
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTE4::_XMUBYTE4
(
    FLOAT _x,
    FLOAT _y,
    FLOAT _z,
    FLOAT _w
)
{
    XMStoreUByte4(this, XMVectorSet(_x, _y, _z, _w));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTE4::_XMUBYTE4
(
    CONST FLOAT* pArray
)
{
    XMStoreUByte4(this, XMLoadFloat4((XMFLOAT4*)pArray));
}

//------------------------------------------------------------------------------

XMFINLINE _XMUBYTE4& _XMUBYTE4::operator=
(
    CONST _XMUBYTE4& UByte4
)
{
    x = UByte4.x;
    y = UByte4.y;
    z = UByte4.z;
    w = UByte4.w;
    return *this;
}

#endif // __cplusplus

#ifdef _NO_INTRINSICS_
#undef XMISNAN
#undef XMISINF
#endif // _NO_INTRINSICS_

#endif // __XMVECTOR_INL__
