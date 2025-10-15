/*++

Copyright (c) Microsoft Corporation

Module Name:

    xmconvert.inl

Abstract:

    Conversion, loading, and storing functions.

--*/

#pragma once

#ifndef __XMCONVERT_INL__
#define __XMCONVERT_INL__

#if defined(_VMX32_INTRINSICS_)
XMINLINE XMVECTOR XMConvertVectorHalfToFloat(XMVECTOR V);
XMINLINE XMVECTOR XMConvertVectorFloatToHalf(XMVECTOR V);
#endif // _VMX32_INTRINSICS_

#define XM_PACK_FACTOR                  (FLOAT)(1 << 22)
#define XM_UNPACK_FACTOR_UNSIGNED       (FLOAT)(1 << 23)
#define XM_UNPACK_FACTOR_SIGNED         XM_PACK_FACTOR

#define XM_UNPACK_UNSIGNEDN_SCALE_OFFSET(BitsX, BitsY, BitsZ, BitsW) \
                                        {-XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << (BitsX)) - 1), \
                                         -XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << (BitsY)) - 1), \
                                         -XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << (BitsZ)) - 1), \
                                         -XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << (BitsW)) - 1)}

#define XM_UNPACK_SIGNEDN_SCALE(BitsX, BitsY, BitsZ, BitsW) \
                                        {-XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << ((BitsX) - 1)) - 1), \
                                         -XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << ((BitsY) - 1)) - 1), \
                                         -XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << ((BitsZ) - 1)) - 1), \
                                         -XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << ((BitsW) - 1)) - 1)}

//#define XM_UNPACK_SIGNEDN_OFFSET(BitsX, BitsY, BitsZ, BitsW) \
//                                        {-XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << ((BitsX) - 1)) - 1) * 3.0f, \
//                                         -XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << ((BitsY) - 1)) - 1) * 3.0f, \
//                                         -XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << ((BitsZ) - 1)) - 1) * 3.0f, \
//                                         -XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << ((BitsW) - 1)) - 1) * 3.0f}

#define XM_PACK_UNSIGNEDN_SCALE(BitsX, BitsY, BitsZ, BitsW) \
                                        {-(FLOAT)((1 << (BitsX)) - 1) / XM_PACK_FACTOR, \
                                         -(FLOAT)((1 << (BitsY)) - 1) / XM_PACK_FACTOR, \
                                         -(FLOAT)((1 << (BitsZ)) - 1) / XM_PACK_FACTOR, \
                                         -(FLOAT)((1 << (BitsW)) - 1) / XM_PACK_FACTOR}

#define XM_PACK_SIGNEDN_SCALE(BitsX, BitsY, BitsZ, BitsW) \
                                        {-(FLOAT)((1 << ((BitsX) - 1)) - 1) / XM_PACK_FACTOR, \
                                         -(FLOAT)((1 << ((BitsY) - 1)) - 1) / XM_PACK_FACTOR, \
                                         -(FLOAT)((1 << ((BitsZ) - 1)) - 1) / XM_PACK_FACTOR, \
                                         -(FLOAT)((1 << ((BitsW) - 1)) - 1) / XM_PACK_FACTOR}

#define XM_PACK_OFFSET                  XMVectorSplatConstant(3, 0)
//#define XM_UNPACK_OFFSET                XM_PACK_OFFSET

/****************************************************************************
 *
 * Data conversion
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE FLOAT XMConvertHalfToFloat
(
    HALF Value
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    UINT Mantissa;
    UINT Exponent;
    UINT Result;

    Mantissa = (UINT)(Value & 0x03FF);

    if ((Value & 0x7C00) != 0)  // The value is normalized
    {
        Exponent = (UINT)((Value >> 10) & 0x1F);
    }
    else if (Mantissa != 0)     // The value is denormalized
    {
        // Normalize the value in the resulting float
        Exponent = 1;

        do
        {
            Exponent--;
            Mantissa <<= 1;
        } while ((Mantissa & 0x0400) == 0);

        Mantissa &= 0x03FF;
    }
    else                        // The value is zero
    {
        Exponent = (UINT)-112;
    }

    Result = ((Value & 0x8000) << 16) | // Sign
             ((Exponent + 112) << 23) | // Exponent
             (Mantissa << 13);          // Mantissa

    return *(FLOAT*)&Result;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR ValueV;
    XMVECTOR Permute;
    FLOAT Result;

    XMASSERT(((UINT_PTR)&Value & 1) == 0);

    Permute = __lvsl(&Value, 0);
    ValueV = __lvx(&Value, 0);
    Permute = __vsldoi(Permute, Permute, 1 << 2);
    ValueV = __vperm(ValueV, ValueV, Permute);
    ValueV = __vupkd3d(ValueV, VPACK_FLOAT16_2);
    ValueV = __vspltw(ValueV, 0);

    __stvewx(ValueV, &Result, 0);

    return Result;

#else

    XMVECTOR ValueV;
    FLOAT Result;

    XMASSERT(((UINT_PTR)&Value & 1) == 0);

    ValueV = __lvlx(&Value, 0);
    ValueV = __vsldoi(ValueV, ValueV, 1 << 2);
    ValueV = __vupkd3d(ValueV, VPACK_FLOAT16_2);
    ValueV = __vspltw(ValueV, 0);

    __stvewx(ValueV, &Result, 0);

    return Result;

#endif
}

//------------------------------------------------------------------------------

XMINLINE FLOAT* XMConvertHalfToFloatStream
(
    FLOAT*      pOutputStream, 
    UINT        OutputStride, 
    CONST HALF* pInputStream, 
    UINT        InputStride, 
    UINT        HalfCount
)
{
#if defined(_NO_INTRINSICS_)

    UINT  i;
    BYTE* pHalf = (BYTE*)pInputStream;
    BYTE* pFloat = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < HalfCount; i++)
    {
        *(FLOAT*)pFloat = XMConvertHalfToFloat(*(HALF*)pHalf);
        pHalf += InputStride;
        pFloat += OutputStride; 
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    UINT  i;
    BYTE* pHalf = (BYTE*)pInputStream;
    BYTE* pFloat = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < HalfCount; i++)
    {
        *(FLOAT*)pFloat = XMConvertHalfToFloat(*(HALF*)pHalf);
        pHalf += InputStride;
        pFloat += OutputStride; 
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    UINT  i;
    BYTE* pHalf = (BYTE*)pInputStream;
    BYTE* pFloat = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < HalfCount; i++)
    {
        *(FLOAT*)pFloat = XMConvertHalfToFloat(*(HALF*)pHalf);
        pHalf += InputStride;
        pFloat += OutputStride; 
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE HALF XMConvertFloatToHalf
(
    FLOAT Value
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    UINT IValue;
    UINT Sign;
    HALF Result;

    IValue = *(UINT*)&Value & 0x7FFFFFFF;
    Sign = (*(UINT*)&Value & 0x80000000) >> 16;

    if (IValue > 0x47FFEFFF)
    {
        // The number is too large to be represented as a half.  Saturate to infinity.
        Result = (HALF)(Sign | 0x7FFF);
    }
    else
    {
        if (IValue < 0x38800000)
        {
            // The number is too small to be represented as a normalized half.
            // Convert it to a denormalized value.
            UINT Shift = 113 - (IValue >> 23);
            IValue = (0x800000 | (IValue & 0x7FFFFF)) >> Shift;
        }
        else
        {
            // Rebias the exponent to represent the value as a normalized half.
            IValue += 0xC8000000;
        }

        Result = (HALF)(Sign | ((IValue + 0x0FFF + ((IValue >> 13) & 1)) >> 13)); 
    }

    return Result;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR ValueV;
    XMVECTOR Permute;
    HALF Result;

    XMASSERT(((UINT_PTR)&Value & 3) == 0);

    ValueV = __lvx(&Value, 0);
    Permute = __lvsl(&Value, 0);
    ValueV = __vperm(ValueV, ValueV, Permute);
    ValueV = __vpkd3d(ValueV, ValueV, VPACK_FLOAT16_4, VPACK_64LO, 2);
    ValueV = __vsplth(ValueV, 0);

    __stvehx(ValueV, &Result, 0);

    return Result;

#else

    XMVECTOR ValueV;
    HALF Result;

    XMASSERT(((UINT_PTR)&Value & 3) == 0);

    ValueV = __lvlx(&Value, 0);
    ValueV = __vpkd3d(ValueV, ValueV, VPACK_FLOAT16_4, VPACK_64LO, 2);
    ValueV = __vsplth(ValueV, 0);

    __stvehx(ValueV, &Result, 0);

    return Result;

#endif
}

//------------------------------------------------------------------------------

XMINLINE HALF* XMConvertFloatToHalfStream
(
    HALF*        pOutputStream, 
    UINT         OutputStride, 
    CONST FLOAT* pInputStream, 
    UINT         InputStride, 
    UINT         FloatCount
)
{
#if defined(_NO_INTRINSICS_)

    UINT  i;
    BYTE* pFloat = (BYTE*)pInputStream;
    BYTE* pHalf = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < FloatCount; i++)
    {
        *(HALF*)pHalf = XMConvertFloatToHalf(*(FLOAT*)pFloat);
        pFloat += InputStride; 
        pHalf += OutputStride;
    }

    return pOutputStream;

#elif defined(_VMX32_INTRINSICS_)

    UINT  i;
    BYTE* pFloat = (BYTE*)pInputStream;
    BYTE* pHalf = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < FloatCount; i++)
    {
        *(HALF*)pHalf = XMConvertFloatToHalf(*(FLOAT*)pFloat);
        pFloat += InputStride; 
        pHalf += OutputStride;
    }

    return pOutputStream;

#else // _VMX128_INTRINSICS_

    UINT  i;
    BYTE* pFloat = (BYTE*)pInputStream;
    BYTE* pHalf = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < FloatCount; i++)
    {
        *(HALF*)pHalf = XMConvertFloatToHalf(*(FLOAT*)pFloat);
        pFloat += InputStride; 
        pHalf += OutputStride;
    }

    return pOutputStream;

#endif // _VMX128_INTRINSICS_
}

#if defined(_NO_INTRINSICS_)

#pragma warning(push)
#pragma warning(disable:4701) // Prevent warnings about 'Result' potentially being used without having been initialized

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMConvertVectorIntToFloat
(
    XMVECTOR VInt,
    UINT     DivExponent
)
{
    UINT ElementIndex;
    FLOAT Scale;
    XMVECTOR Result;

    Scale = 1.0f / (FLOAT)(1 << DivExponent);

    for (ElementIndex = 0; ElementIndex < 4; ElementIndex++)
    {
        Result.v[ElementIndex] = (FLOAT)*(INT*)&VInt.u[ElementIndex] * Scale;
    }

    return Result;
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMConvertVectorUIntToFloat
(
    XMVECTOR VUInt,
    UINT     DivExponent
)
{
    UINT ElementIndex;
    FLOAT Scale;
    XMVECTOR Result;

    Scale = 1.0f / (FLOAT)(1 << DivExponent);

    for (ElementIndex = 0; ElementIndex < 4; ElementIndex++)
    {
        Result.v[ElementIndex] = (FLOAT)VUInt.u[ElementIndex] * Scale;
    }

    return Result;
}

#pragma warning(pop)

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMConvertVectorFloatToInt
(
    XMVECTOR VFloat,
    UINT     MulExponent
)
{
    UINT ElementIndex;
    XMVECTOR Scale;
    XMVECTOR Result;

    Scale = XMVectorReplicate((FLOAT)(1 << MulExponent));
    Result = XMVectorMultiply(VFloat, Scale);
    Result = XMVectorTruncate(Result);

    for (ElementIndex = 0; ElementIndex < 4; ElementIndex++)
    {
        if (Result.v[ElementIndex] <= -2147483648.0f)
        {
            *(INT*)&Result.u[ElementIndex] = -2147483647 - 1;
        }
        else if (Result.v[ElementIndex] >= 2147483647.0f)
        {
            *(INT*)&Result.u[ElementIndex] = 0x7FFFFFFF;
        }
        else
        {
            *(INT*)&Result.u[ElementIndex] = (INT)Result.v[ElementIndex];
        }
    }

    return Result;
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMConvertVectorFloatToUInt
(
    XMVECTOR VFloat,
    UINT     MulExponent
)
{
    UINT ElementIndex;
    XMVECTOR Scale;
    XMVECTOR Result;
    XMVECTOR MinUInt;

    MinUInt = XMVectorZero();
    Scale = XMVectorReplicate((FLOAT)(1 << MulExponent));
    Result = XMVectorMultiply(VFloat, Scale);
    Result = XMVectorTruncate(Result);

    for (ElementIndex = 0; ElementIndex < 4; ElementIndex++)
    {
        if (Result.v[ElementIndex] <= 0.0f)
        {
            Result.u[ElementIndex] = 0;
        }
        else if (Result.v[ElementIndex] >= 4294967295.0f)
        {
            Result.u[ElementIndex] = 0xFFFFFFFF;
        }
        else
        {
            Result.u[ElementIndex] = (UINT)Result.v[ElementIndex];
        }
    }

    return Result;
}

#endif

/****************************************************************************
 *
 * Vector and matrix load operations
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadScalar
(
    CONST VOID* pSource
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);

    V.u[0] = *pElement;

    return V;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V;
    XMVECTOR V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);                            // Load the 16 bytes starting at address pSource & ~0xF
    Permute = __lvsl(pSource, 0);                      // Compute the permute control vector for shift left
    V1 = __lvx(pSource, 3);                            // Load the 16 bytes starting at address ((BYTE*)pSource + 3) & ~0xF
    V = __vperm(V0, V1, Permute);                      // Align the vector

    return V;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V;
    XMVECTOR Permute;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V = __vperm(V, V, Permute);

    return V;

#else // _VMX128_INTRINSICS_

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    return __lvlx(pSource, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadVector2
(
    CONST VOID* pSource
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);

    V.u[0] = pElement[0];
    V.u[1] = pElement[1];

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V;
    XMVECTOR V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);                            // Load the 16 bytes starting at address pSource & ~0xF
    Permute = __lvsl(pSource, 0);                      // Compute the permute control vector for shift left
    V1 = __lvx(pSource, 7);                            // Load the 16 bytes starting at address ((BYTE*)pSource + 7) & ~0xF
    V = __vperm(V0, V1, Permute);                      // Align the vector

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR VL;
    XMVECTOR VR;
    XMVECTOR V;

    XMASSERT(pSource);

    VR = __lvrx(pSource, 8);
    VL = __lvlx(pSource, 0);
    VR = __vsldoi(VR, VR, 2 << 2);
    V = __vor(VL, VR);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadFloat2
(
    CONST XMFLOAT2* pSource
)
{
    return XMLoadVector2(pSource);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadVector2A
(
    CONST VOID* pSource
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    V.u[0] = pElement[0];
    V.u[1] = pElement[1];

    return V;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    return __lvx(pSource, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadFloat2A
(
    CONST XMFLOAT2A* pSource
)
{
    return XMLoadVector2A(pSource);
}
    
//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadHalf2
(
    CONST XMHALF2* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = XMConvertHalfToFloat(pSource->x);
    V.v[1] = XMConvertHalfToFloat(pSource->y);

    return V;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V, V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    // Load the vector, placing each half value in the low order 16 bits of each component
    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    V = __vperm(V0, V1, Permute);               // Shift the 2 half values into the most significant 32 bits
    V = __vupkhsh(V);                           // Unpack the 2 half values

    return XMConvertVectorHalfToFloat(V);

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V, V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    Permute = __lvsl(pSource, 0);
    V0 = __lvx(pSource, 0);
    Permute = __vsldoi(Permute, Permute, 1 << 2);
    V1 = __lvx(pSource, 3);
    V = __vperm(V0, V1, Permute);
    V = __vupkd3d(V, VPACK_FLOAT16_2);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR VL;
    XMVECTOR VR;
    XMVECTOR V;

    XMASSERT(pSource);

    VL = __lvlx(pSource, 0);
    VR = __lvrx(pSource, 4);
    VL = __vsldoi(VL, VL, 1 << 2);
    V = __vor(VL, VR);
    V = __vupkd3d(V, VPACK_FLOAT16_2);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadShortN2
(
    CONST XMSHORTN2* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);

    V.v[0] = (FLOAT)pSource->x / 32767.0f;
    V.v[1] = (FLOAT)pSource->y / 32767.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
    static CONST XMVECTOR   UnpackScale = {3.051850948e-5f, 3.051850948e-5f, 3.051850948e-5f, 3.051850948e-5f};

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    V = __vperm(V0, V1, Permute);                                     // Shift the 2 short values into the most significant 32 bits
    V = __vupkhsh(V);                                                 // Unpack the 2 short values
    V = __vcfsx(V, 0);                                                // Convert to floating point
    V = __vmaddfp(V, UnpackScale, *(XMVECTOR*)g_XMNegativeZero);      // Scale to the correct range

    return V;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V0, V1;
    XMVECTOR                V;
    XMVECTOR                Permute;
    XMVECTOR                Result;
    XMVECTOR                UnpackOffset;
    static CONST XMVECTOR   UnpackScale = XM_UNPACK_SIGNEDN_SCALE(16, 16, 16, 16);

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);

    UnpackOffset = __vspltisw(3);
    UnpackOffset = __vcfsx(UnpackOffset, 0);

    Permute = __lvsl(pSource, 0);
    V0 = __lvx(pSource, 0);
    Permute = __vsldoi(Permute, Permute, 1 << 2);
    V1 = __lvx(pSource, 3);
    Result = __vmulfp(UnpackScale, UnpackOffset);
    V = __vperm(V0, V1, Permute);
    V = __vupkd3d(V, VPACK_NORMSHORT2);
    Result = __vnmsubfp(V, UnpackScale, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR                VL;
    XMVECTOR                VR;
    XMVECTOR                V;
    XMVECTOR                Result;
    XMVECTOR                UnpackOffset;
    static CONST XMVECTOR   UnpackScale = XM_UNPACK_SIGNEDN_SCALE(16, 16, 16, 16);

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);

    UnpackOffset = __vspltisw(3);
    UnpackOffset = __vcfsx(UnpackOffset, 0);
    VL = __lvlx(pSource, 0);
    VR = __lvrx(pSource, 4);
    Result = __vmulfp(UnpackScale, UnpackOffset);
    VL = __vsldoi(VL, VL, 1 << 2);
    V = __vor(VL, VR);
    V = __vupkd3d(V, VPACK_NORMSHORT2);
    Result = __vnmsubfp(V, UnpackScale, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadShort2
(
    CONST XMSHORT2* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);

    V.v[0] = (FLOAT)pSource->x;
    V.v[1] = (FLOAT)pSource->y;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    V = __vperm(V0, V1, Permute);                                     // Shift the 2 short values into the most significant 32 bits
    V = __vupkhsh(V);                                                 // Unpack the 2 short values
    V = __vcfsx(V, 0);                                                // Convert to floating point

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                VL;
    XMVECTOR                VR;
    XMVECTOR                V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);

    VR = __lvrx(pSource, 4);
    VL = __lvlx(pSource, 0);
    VR = __vsldoi(VR, VR, 3 << 2);
    V = __vor(VL, VR);
    V = __vupkhsh(V);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUShortN2
(
    CONST XMUSHORTN2* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)pSource->x / 65535.0f;
    V.v[1] = (FLOAT)pSource->y / 65535.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Zero;
    XMVECTOR                Permute;
    static CONST XMVECTOR   UnpackScale = {1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1)};

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    Zero = __vspltisw(0);
    V = __vperm(V0, V1, Permute);                                     // Shift the 2 ushort values into the most significant 32 bits
    V = __vmrghh(Zero, V);                                            // Unpack the 2 ushort values
    V = __vcfsx(V, 0);                                                // Convert to floating point
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, UnpackScale, *(XMVECTOR*)g_XMNegativeZero);      // Scale to the correct range
#else
    V = __vmulfp(V, UnpackScale);      // Scale to the correct range
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                VL;
    XMVECTOR                VR;
    XMVECTOR                V;
    XMVECTOR                Zero;
    static CONST XMVECTOR   UnpackScale = {1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1)};

    XMASSERT(pSource);

    VR = __lvrx(pSource, 4);
    VL = __lvlx(pSource, 0);
    VR = __vsldoi(VR, VR, 12);
    Zero = __vspltisw(0);
    V = __vor(VL, VR);
    V = __vmrghh(Zero, V);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, UnpackScale);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUShort2
(
    CONST XMUSHORT2* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)pSource->x;
    V.v[1] = (FLOAT)pSource->y;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Zero;
    XMVECTOR                Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    Zero = __vspltisw(0);
    V = __vperm(V0, V1, Permute);                                     // Shift the 2 ushort values into the most significant 32 bits
    V = __vmrghh(Zero, V);                                            // Unpack the 2 ushort values
    V = __vcfsx(V, 0);                                                // Convert to floating point

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                VL;
    XMVECTOR                VR;
    XMVECTOR                V;
    XMVECTOR                Zero;

    XMASSERT(pSource);

    VR = __lvrx(pSource, 4);
    VL = __lvlx(pSource, 0);
    VR = __vsldoi(VR, VR, 12);
    Zero = __vspltisw(0);
    V = __vor(VL, VR);
    V = __vmrghh(Zero, V);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadVector3
(
    CONST VOID* pSource
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);

    V.u[0] = pElement[0];
    V.u[1] = pElement[1];
    V.u[2] = pElement[2];

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V;
    XMVECTOR V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);                            // Load the 16 bytes starting at address pSource & ~0xF
    Permute = __lvsl(pSource, 0);                      // Compute the permute control vector for shift left
    V1 = __lvx(pSource, 11);                           // Load the 16 bytes starting at address ((BYTE*)pSource + 11) & ~0xF
    V = __vperm(V0, V1, Permute);                      // Align the vector

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR VL;
    XMVECTOR VR;
    XMVECTOR V;

    XMASSERT(pSource);

    VR = __lvrx(pSource, 12);
    VL = __lvlx(pSource, 0);
    VR = __vsldoi(VR, VR, 1 << 2);
    V = __vor(VL, VR);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadFloat3
(
    CONST XMFLOAT3* pSource
)
{
    return XMLoadVector3(pSource);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadVector3A
(
    CONST VOID* pSource
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    V.u[0] = pElement[0];
    V.u[1] = pElement[1];
    V.u[2] = pElement[2];

    return V;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    return __lvx(pSource, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadFloat3A
(
    CONST XMFLOAT3A* pSource
)
{
    return XMLoadVector3A(pSource);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUHenDN3
(
    CONST XMUHENDN3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;

    XMASSERT(pSource);

    Element = pSource->v & 0x7FF;
    V.v[0] = (FLOAT)Element / 2047.0f;
    Element = (pSource->v >> 11) & 0x7FF;
    V.v[1] = (FLOAT)Element / 2047.0f;
    Element = (pSource->v >> 22) & 0x3FF;
    V.v[2] = (FLOAT)Element / 1023.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    static CONST XMVECTOR  Scale = {1.0f / (FLOAT)((1 << 11) - 1), 1.0f / (FLOAT)((1 << 11) - 1), 1.0f / (FLOAT)((1 << 10) - 1), 0.0f};

    XMASSERT(pSource);

    I[0] = pSource->v & 0x7FF;
    I[1] = (pSource->v >> 11) & 0x7FF;
    I[2] = (pSource->v >> 22) & 0x3FF;

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               Scale;
    XMVECTOR               Mask;
    static CONST XMVECTORI C0 = {0x3A001002, 0x3A802008, 0x7FF, 0x3FF}; // {1.0f / (FLOAT)((1 << 11) - 1), 1.0f / (FLOAT)((1 << 10) - 1), *(FLOAT*)&0x7FF, *(FLOAT*)&0x3FF};
    static CONST XMVECTORI Shift = {0, 11, 22, 0};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    V = __vspltw(V, 0);
    Mask = __vpermwi(*(XMVECTOR*)C0, 0xAF);
    V = __vsrw(V, *(XMVECTOR*)Shift);
    V = __vand(V, Mask);
    Scale = __vpermwi(*(XMVECTOR*)C0, 0x5);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, Scale);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUHenD3
(
    CONST XMUHEND3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;

    XMASSERT(pSource);

    Element = pSource->v & 0x7FF;
    V.v[0] = (FLOAT)Element;
    Element = (pSource->v >> 11) & 0x7FF;
    V.v[1] = (FLOAT)Element;
    Element = (pSource->v >> 22) & 0x3FF;
    V.v[2] = (FLOAT)Element;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;

    XMASSERT(pSource);

    I[0] = pSource->v & 0x7FF;
    I[1] = (pSource->v >> 11) & 0x7FF;
    I[2] = (pSource->v >> 22) & 0x3FF;

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               Shift;
    XMVECTOR               Mask;
    static CONST XMVECTORI ShiftMask = {0x7FF0000, 0x7FF000B, 0x3FF0016, 0x0};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    Shift = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    Mask = __vsrw(*(XMVECTOR*)ShiftMask, Shift);
    V = __vsrw(V, *(XMVECTOR*)ShiftMask);
    V = __vand(V, Mask);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadHenDN3
(
    CONST XMHENDN3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtendXY[] = {0x00000000, 0xFFFFF800};
    static CONST UINT SignExtendZ[] = {0x00000000, 0xFFFFFC00};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 11) & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 22) & 0x3FF) != 0x200);

    Element = pSource->v & 0x7FF;
    V.v[0] = (FLOAT)(SHORT)(Element | SignExtendXY[Element >> 10]) / 1023.0f;
    Element = (pSource->v >> 11) & 0x7FF;
    V.v[1] = (FLOAT)(SHORT)(Element | SignExtendXY[Element >> 10]) / 1023.0f;
    Element = (pSource->v >> 22) & 0x3FF;
    V.v[2] = (FLOAT)(SHORT)(Element | SignExtendZ[Element >> 9]) / 511.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtendXY[] = {0x00000000, 0xFFFFF800};
    static CONST UINT      SignExtendZ[] = {0x00000000, 0xFFFFFC00};
    static CONST XMVECTOR  Scale = {1.0f / (FLOAT)((1 << (11 - 1)) - 1), 1.0f / (FLOAT)((1 << (11 - 1)) - 1), 1.0f / (FLOAT)((1 << (10 - 1)) - 1), 1.0f};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 11) & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 22) & 0x3FF) != 0x200);

    Element = pSource->v & 0x7FF;
    I[0] = Element | SignExtendXY[Element >> 10];
    Element = (pSource->v >> 11) & 0x7FF;
    I[1] = Element | SignExtendXY[Element >> 10];
    Element = (pSource->v >> 22) & 0x3FF;
    I[2] = Element | SignExtendZ[Element >> 9];

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               ShiftR;
    static CONST XMVECTORI Scale = {0x3A802008, 0x3A802008, 0x3B004020, 0}; // {1.0f / (FLOAT)((1 << (11 - 1)) - 1), ..., 1.0f / (FLOAT)((1 << (10 - 1)) - 1)};
    static CONST XMVECTORI Shift = {((32 - 11) << 16) | (32 - 11), ((32 - 11) << 16) | (32 - 22), ((32 - 10) << 16) | (32 - 32), 0};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    ShiftR = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    ShiftR = __vsrw(*(XMVECTOR*)Shift, ShiftR);
    V = __vslw(V, *(XMVECTOR*)Shift);
    V = __vsraw(V, ShiftR);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, *(XMVECTOR*)Scale);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadHenD3
(
    CONST XMHEND3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtendXY[] = {0x00000000, 0xFFFFF800};
    static CONST UINT SignExtendZ[] = {0x00000000, 0xFFFFFC00};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 11) & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 22) & 0x3FF) != 0x200);

    Element = pSource->v & 0x7FF;
    V.v[0] = (FLOAT)(SHORT)(Element | SignExtendXY[Element >> 10]);
    Element = (pSource->v >> 11) & 0x7FF;
    V.v[1] = (FLOAT)(SHORT)(Element | SignExtendXY[Element >> 10]);
    Element = (pSource->v >> 22) & 0x3FF;
    V.v[2] = (FLOAT)(SHORT)(Element | SignExtendZ[Element >> 9]);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtendXY[] = {0x00000000, 0xFFFFF800};
    static CONST UINT      SignExtendZ[] = {0x00000000, 0xFFFFFC00};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 11) & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 22) & 0x3FF) != 0x200);

    Element = pSource->v & 0x7FF;
    I[0] = Element | SignExtendXY[Element >> 10];
    Element = (pSource->v >> 11) & 0x7FF;
    I[1] = Element | SignExtendXY[Element >> 10];
    Element = (pSource->v >> 22) & 0x3FF;
    I[2] = Element | SignExtendZ[Element >> 9];

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               ShiftR;
    static CONST XMVECTORI Shift = {((32 - 11) << 16) | (32 - 11), ((32 - 11) << 16) | (32 - 22), ((32 - 10) << 16) | (32 - 32), 0};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    ShiftR = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    ShiftR = __vsrw(*(XMVECTOR*)Shift, ShiftR);
    V = __vslw(V, *(XMVECTOR*)Shift);
    V = __vsraw(V, ShiftR);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUDHenN3
(
    CONST XMUDHENN3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;

    XMASSERT(pSource);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)Element / 1023.0f;
    Element = (pSource->v >> 10) & 0x7FF;
    V.v[1] = (FLOAT)Element / 2047.0f;
    Element = (pSource->v >> 21) & 0x7FF;
    V.v[2] = (FLOAT)Element / 2047.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    static CONST XMVECTOR  Scale = {1.0f / (FLOAT)((1 << 10) - 1), 1.0f / (FLOAT)((1 << 11) - 1), 1.0f / (FLOAT)((1 << 11) - 1), 0.0f};

    XMASSERT(pSource);

    I[0] = pSource->v & 0x3FF;
    I[1] = (pSource->v >> 10) & 0x7FF;
    I[2] = (pSource->v >> 21) & 0x7FF;

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               Scale;
    XMVECTOR               Mask;
    static CONST XMVECTORI C0 = {0x3A802008, 0x3A001002, 0x3FF, 0x7FF}; // {1.0f / (FLOAT)((1 << 10) - 1), 1.0f / (FLOAT)((1 << 11) - 1), *(FLOAT*)&0x3FF, *(FLOAT*)&0x7FF};
    static CONST XMVECTORI Shift = {0, 10, 21, 0};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    V = __vspltw(V, 0);
    Mask = __vpermwi(*(XMVECTOR*)C0, 0xBC);
    V = __vsrw(V, *(XMVECTOR*)Shift);
    V = __vand(V, Mask);
    Scale = __vpermwi(*(XMVECTOR*)C0, 0x14);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, Scale);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUDHen3
(
    CONST XMUDHEN3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;

    XMASSERT(pSource);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)Element;
    Element = (pSource->v >> 10) & 0x7FF;
    V.v[1] = (FLOAT)Element;
    Element = (pSource->v >> 21) & 0x7FF;
    V.v[2] = (FLOAT)Element;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;

    XMASSERT(pSource);

    I[0] = pSource->v & 0x3FF;
    I[1] = (pSource->v >> 10) & 0x7FF;
    I[2] = (pSource->v >> 21) & 0x7FF;

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               Shift;
    XMVECTOR               Mask;
    static CONST XMVECTORI ShiftMask = {0x3FF0000, 0x7FF000A, 0x7FF0015, 0x0};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    Shift = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    Mask = __vsrw(*(XMVECTOR*)ShiftMask, Shift);
    V = __vsrw(V, *(XMVECTOR*)ShiftMask);
    V = __vand(V, Mask);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadDHenN3
(
    CONST XMDHENN3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtendX[] = {0x00000000, 0xFFFFFC00};
    static CONST UINT SignExtendYZ[] = {0x00000000, 0xFFFFF800};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 21) & 0x7FF) != 0x400);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)(SHORT)(Element | SignExtendX[Element >> 9]) / 511.0f;
    Element = (pSource->v >> 10) & 0x7FF;
    V.v[1] = (FLOAT)(SHORT)(Element | SignExtendYZ[Element >> 10]) / 1023.0f;
    Element = (pSource->v >> 21) & 0x7FF;
    V.v[2] = (FLOAT)(SHORT)(Element | SignExtendYZ[Element >> 10]) / 1023.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtendX[] = {0x00000000, 0xFFFFFC00};
    static CONST UINT      SignExtendYZ[] = {0x00000000, 0xFFFFF800};
    static CONST XMVECTOR  Scale = {1.0f / (FLOAT)((1 << (10 - 1)) - 1), 1.0f / (FLOAT)((1 << (11 - 1)) - 1), 1.0f / (FLOAT)((1 << (11 - 1)) - 1), 1.0f};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 21) & 0x7FF) != 0x400);

    Element = pSource->v & 0x3FF;
    I[0] = Element | SignExtendX[Element >> 9];
    Element = (pSource->v >> 10) & 0x7FF;
    I[1] = Element | SignExtendYZ[Element >> 10];
    Element = (pSource->v >> 21) & 0x7FF;
    I[2] = Element | SignExtendYZ[Element >> 10];

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               ShiftR;
    static CONST XMVECTORI Scale = {0x3B004020, 0x3A802008, 0x3A802008, 0}; // {1.0f / (FLOAT)((1 << (10 - 1)) - 1), ..., 1.0f / (FLOAT)((1 << (11 - 1)) - 1)};
    static CONST XMVECTORI Shift = {((32 - 10) << 16) | (32 - 10), ((32 - 11) << 16) | (32 - 21), ((32 - 11) << 16) | (32 - 32), 0};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    ShiftR = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    ShiftR = __vsrw(*(XMVECTOR*)Shift, ShiftR);
    V = __vslw(V, *(XMVECTOR*)Shift);
    V = __vsraw(V, ShiftR);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, *(XMVECTOR*)Scale);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadDHen3
(
    CONST XMDHEN3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtendX[] = {0x00000000, 0xFFFFFC00};
    static CONST UINT SignExtendYZ[] = {0x00000000, 0xFFFFF800};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 21) & 0x7FF) != 0x400);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)(SHORT)(Element | SignExtendX[Element >> 9]);
    Element = (pSource->v >> 10) & 0x7FF;
    V.v[1] = (FLOAT)(SHORT)(Element | SignExtendYZ[Element >> 10]);
    Element = (pSource->v >> 21) & 0x7FF;
    V.v[2] = (FLOAT)(SHORT)(Element | SignExtendYZ[Element >> 10]);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtendX[] = {0x00000000, 0xFFFFFC00};
    static CONST UINT      SignExtendYZ[] = {0x00000000, 0xFFFFF800};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x7FF) != 0x400);
    XMASSERT(((pSource->v >> 21) & 0x7FF) != 0x400);

    Element = pSource->v & 0x3FF;
    I[0] = Element | SignExtendX[Element >> 9];
    Element = (pSource->v >> 10) & 0x7FF;
    I[1] = Element | SignExtendYZ[Element >> 10];
    Element = (pSource->v >> 21) & 0x7FF;
    I[2] = Element | SignExtendYZ[Element >> 10];

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               ShiftR;
    static CONST XMVECTORI Shift = {((32 - 10) << 16) | (32 - 10), ((32 - 11) << 16) | (32 - 21), ((32 - 11) << 16) | (32 - 32), 0};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    ShiftR = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    ShiftR = __vsrw(*(XMVECTOR*)Shift, ShiftR);
    V = __vslw(V, *(XMVECTOR*)Shift);
    V = __vsraw(V, ShiftR);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadVector4
(
    CONST VOID* pSource
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);

    V.u[0] = pElement[0];
    V.u[1] = pElement[1];
    V.u[2] = pElement[2];
    V.u[3] = pElement[3];

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V;
    XMVECTOR V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);                            // Load the 16 bytes starting at address pSource & ~0xF
    Permute = __lvsl(pSource, 0);                      // Compute the permute control vector for shift left
    V1 = __lvx(pSource, 15);                           // Load the 16 bytes starting at address ((BYTE*)pSource + 15) & ~0xF
    V = __vperm(V0, V1, Permute);                      // Align the vector

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR VL;
    XMVECTOR VR;
    XMVECTOR V;

    XMASSERT(pSource);

    VL = __lvlx(pSource, 0);
    VR = __lvrx(pSource, 16);
    V = __vor(VL, VR);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadFloat4
(
    CONST XMFLOAT4* pSource
)
{
    return XMLoadVector4(pSource);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadVector4A
(
    CONST VOID* pSource
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    V.u[0] = pElement[0];
    V.u[1] = pElement[1];
    V.u[2] = pElement[2];
    V.u[3] = pElement[3];

    return V;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    return __lvx(pSource, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadFloat4A
(
    CONST XMFLOAT4A* pSource
)
{
    return XMLoadVector4A(pSource);
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadHalf4
(
    CONST XMHALF4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = XMConvertHalfToFloat(pSource->x);
    V.v[1] = XMConvertHalfToFloat(pSource->y);
    V.v[2] = XMConvertHalfToFloat(pSource->z);
    V.v[3] = XMConvertHalfToFloat(pSource->w);

    return V;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V, V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    // Load the vector, placing each half value in the low order 16 bits of each component
    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);
    V = __vperm(V0, V1, Permute);               // Shift the 4 short values into the most significant 64 bits
    V = __vupkhsh(V);                           // Unpack the 4 short values

    return XMConvertVectorHalfToFloat(V);

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V, V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);
    Permute = __vsldoi(Permute, Permute, 2 << 2);
    V = __vperm(V0, V1, Permute);
    V = __vupkd3d(V, VPACK_FLOAT16_4);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR VL;
    XMVECTOR VR;
    XMVECTOR V;

    XMASSERT(pSource);

    VL = __lvlx(pSource, 0);
    VR = __lvrx(pSource, 8);
    VL = __vsldoi(VL, VL, 2 << 2);
    V = __vor(VL, VR);
    V = __vupkd3d(V, VPACK_FLOAT16_4);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadShortN4
(
    CONST XMSHORTN4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);
    XMASSERT(pSource->z != -32768);
    XMASSERT(pSource->w != -32768);

    V.v[0] = (FLOAT)pSource->x / 32767.0f;
    V.v[1] = (FLOAT)pSource->y / 32767.0f;
    V.v[2] = (FLOAT)pSource->z / 32767.0f;
    V.v[3] = (FLOAT)pSource->w / 32767.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
    static CONST XMVECTOR   UnpackScale = {3.051850948e-5f, 3.051850948e-5f, 3.051850948e-5f, 3.051850948e-5f};

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);
    XMASSERT(pSource->z != -32768);
    XMASSERT(pSource->w != -32768);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);
    V = __vperm(V0, V1, Permute);                         // Shift the 4 short values into the most significant 64 bits
    V = __vupkhsh(V);                                     // Unpack the 4 short values
    V = __vcfsx(V, 0);                                    // Convert to floating point
    V = __vmaddfp(V, UnpackScale, *(XMVECTOR*)g_XMNegativeZero);      // Scale to the correct range

    return V;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
    XMVECTOR                Result;
    XMVECTOR                UnpackOffset;
    static CONST XMVECTOR   UnpackScale = XM_UNPACK_SIGNEDN_SCALE(16, 16, 16, 16);

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);
    XMASSERT(pSource->z != -32768);
    XMASSERT(pSource->w != -32768);

    UnpackOffset = __vspltisw(3);
    UnpackOffset = __vcfsx(UnpackOffset, 0);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);
    Permute = __vsldoi(Permute, Permute, 2 << 2);
    Result = __vmulfp(UnpackScale, UnpackOffset);
    V = __vperm(V0, V1, Permute);
    V = __vupkd3d(V, VPACK_NORMSHORT4);
    Result = __vnmsubfp(V, UnpackScale, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR                VL;
    XMVECTOR                VR;
    XMVECTOR                V;
    XMVECTOR                Result;
    XMVECTOR                UnpackOffset;
    static CONST XMVECTOR   UnpackScale = XM_UNPACK_SIGNEDN_SCALE(16, 16, 16, 16);

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);
    XMASSERT(pSource->z != -32768);
    XMASSERT(pSource->w != -32768);

    UnpackOffset = __vspltisw(3);
    UnpackOffset = __vcfsx(UnpackOffset, 0);
    VL = __lvlx(pSource, 0);
    VR = __lvrx(pSource, 8);
    Result = __vmulfp(UnpackScale, UnpackOffset);
    VL = __vsldoi(VL, VL, 2 << 2);
    V = __vor(VL, VR);
    V = __vupkd3d(V, VPACK_NORMSHORT4);
    Result = __vnmsubfp(V, UnpackScale, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadShort4
(
    CONST XMSHORT4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);
    XMASSERT(pSource->z != -32768);
    XMASSERT(pSource->w != -32768);

    V.v[0] = (FLOAT)pSource->x;
    V.v[1] = (FLOAT)pSource->y;
    V.v[2] = (FLOAT)pSource->z;
    V.v[3] = (FLOAT)pSource->w;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);
    XMASSERT(pSource->z != -32768);
    XMASSERT(pSource->w != -32768);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);
    V = __vperm(V0, V1, Permute);                                     // Shift the 2 short values into the most significant 32 bits
    V = __vupkhsh(V);                                                 // Unpack the 2 short values
    V = __vcfsx(V, 0);                                                // Convert to floating point

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                VL;
    XMVECTOR                VR;
    XMVECTOR                V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -32768);
    XMASSERT(pSource->y != -32768);
    XMASSERT(pSource->z != -32768);
    XMASSERT(pSource->w != -32768);

    VR = __lvrx(pSource, 8);
    VL = __lvlx(pSource, 0);
    VR = __vsldoi(VR, VR, 2 << 2);
    V = __vor(VL, VR);
    V = __vupkhsh(V);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUShortN4
(
    CONST XMUSHORTN4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)pSource->x / 65535.0f;
    V.v[1] = (FLOAT)pSource->y / 65535.0f;
    V.v[2] = (FLOAT)pSource->z / 65535.0f;
    V.v[3] = (FLOAT)pSource->w / 65535.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Zero;
    XMVECTOR                Permute;
    static CONST XMVECTOR   UnpackScale = {1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1)};

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);
    Zero = __vspltisw(0);
    V = __vperm(V0, V1, Permute);                                     // Shift the 2 ushort values into the most significant 32 bits
    V = __vmrghh(Zero, V);                                            // Unpack the 2 ushort values
    V = __vcfsx(V, 0);                                                // Convert to floating point
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, UnpackScale, *(XMVECTOR*)g_XMNegativeZero);      // Scale to the correct range
#else
    V = __vmulfp(V, UnpackScale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                VL;
    XMVECTOR                VR;
    XMVECTOR                V;
    XMVECTOR                Zero;
    static CONST XMVECTOR   UnpackScale = {1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1), 1.0f / (FLOAT)((1 << 16) - 1)};

    XMASSERT(pSource);

    VR = __lvrx(pSource, 8);
    VL = __lvlx(pSource, 0);
    VR = __vsldoi(VR, VR, 2 << 2);
    Zero = __vspltisw(0);
    V = __vor(VL, VR);
    V = __vmrghh(Zero, V);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, UnpackScale);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUShort4
(
    CONST XMUSHORT4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)pSource->x;
    V.v[1] = (FLOAT)pSource->y;
    V.v[2] = (FLOAT)pSource->z;
    V.v[3] = (FLOAT)pSource->w;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Zero;
    XMVECTOR                Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);
    Zero = __vspltisw(0);
    V = __vperm(V0, V1, Permute);                                     // Shift the 2 ushort values into the most significant 32 bits
    V = __vmrghh(Zero, V);                                            // Unpack the 2 ushort values
    V = __vcfsx(V, 0);                                                // Convert to floating point

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                VL;
    XMVECTOR                VR;
    XMVECTOR                V;
    XMVECTOR                Zero;

    XMASSERT(pSource);

    VR = __lvrx(pSource, 8);
    VL = __lvlx(pSource, 0);
    VR = __vsldoi(VR, VR, 2 << 2);
    Zero = __vspltisw(0);
    V = __vor(VL, VR);
    V = __vmrghh(Zero, V);
    V = __vcfux(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadXIcoN4
(
    CONST XMXICON4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtend[] = {0x00000000, 0xFFF00000};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    Element = (UINT)pSource->v & 0xFFFFF;
    V.v[0] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]) / 524287.0f;
    Element = (UINT)(pSource->v >> 20) & 0xFFFFF;
    V.v[1] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]) / 524287.0f;
    Element = (UINT)(pSource->v >> 40) & 0xFFFFF;
    V.v[2] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]) / 524287.0f;
    V.v[3] = (FLOAT)(pSource->v >> 60) / 15.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtend[] = {0x00000000, 0xFFF00000};
    static CONST XMVECTOR  Scale = {1.0f / (FLOAT)((1 << (20 - 1)) - 1), 1.0f / (FLOAT)((1 << (20 - 1)) - 1), 1.0f / (FLOAT)((1 << (20 - 1)) - 1), 1.0f / (FLOAT)((1 << 4) - 1)};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    Element = (UINT)pSource->v & 0xFFFFF;
    I[0] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 20) & 0xFFFFF;
    I[1] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 40) & 0xFFFFF;
    I[2] = Element | SignExtend[Element >> 19];
    I[3] = (UINT)(pSource->v >> 60);

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                V;
    XMVECTOR                Result;
    XMVECTOR                UnpackScale;
    static CONST XMVECTOR   UnpackScaleOffset = {-XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << (20 - 1)) - 1), // Signed scale
                                                 -XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << 4) - 1), // Unsigned scale
                                                 -XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << (20 - 1)) - 1) * 3.0f, // Signed offset
                                                 -XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << 4) - 1)}; // Unsigned offset

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 7) == 0);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    V = __lvlx(pSource, 0);
    Result = __vpermwi(UnpackScaleOffset, 0xAB); // UnpackOffset
    UnpackScale = __vpermwi(UnpackScaleOffset, 0x1);
    V = __vsldoi(V, V, 2 << 2);
    V = __vupkd3d(V, VPACK_NORMPACKED64);
    Result = __vnmsubfp(V, UnpackScale, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadXIco4
(
    CONST XMXICO4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtend[] = {0x00000000, 0xFFF00000};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    Element = (UINT)pSource->v & 0xFFFFF;
    V.v[0] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]);
    Element = (UINT)(pSource->v >> 20) & 0xFFFFF;
    V.v[1] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]);
    Element = (UINT)(pSource->v >> 40) & 0xFFFFF;
    V.v[2] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]);
    V.v[3] = (FLOAT)(pSource->v >> 60);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtend[] = {0x00000000, 0xFFF00000};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    Element = (UINT)pSource->v & 0xFFFFF;
    I[0] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 20) & 0xFFFFF;
    I[1] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 40) & 0xFFFFF;
    I[2] = Element | SignExtend[Element >> 19];
    I[3] = (UINT)(pSource->v >> 60);

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                V;
    XMVECTOR                Result;
    XMVECTOR                UnpackScale;
    static CONST XMVECTOR   UnpackScaleOffset = {-XM_UNPACK_FACTOR_SIGNED, // Signed scale
                                                 -XM_UNPACK_FACTOR_UNSIGNED, // Unsigned scale
                                                 -XM_UNPACK_FACTOR_SIGNED * 3.0f, // Signed offset
                                                 -XM_UNPACK_FACTOR_UNSIGNED}; // Unsigned offset

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 7) == 0);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    V = __lvlx(pSource, 0);
    Result = __vpermwi(UnpackScaleOffset, 0xAB); // UnpackOffset
    UnpackScale = __vpermwi(UnpackScaleOffset, 0x1);
    V = __vsldoi(V, V, 2 << 2);
    V = __vupkd3d(V, VPACK_NORMPACKED64);
    Result = __vnmsubfp(V, UnpackScale, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUIcoN4
(
    CONST XMUICON4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)(pSource->v & 0xFFFFF) / 1048575.0f;
    V.v[1] = (FLOAT)((pSource->v >> 20) & 0xFFFFF) / 1048575.0f;
    V.v[2] = (FLOAT)((pSource->v >> 40) & 0xFFFFF) / 1048575.0f;
    V.v[3] = (FLOAT)(pSource->v >> 60) / 15.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    static CONST XMVECTOR  Scale = {1.0f / (FLOAT)((1 << 20) - 1), 1.0f / (FLOAT)((1 << 20) - 1), 1.0f / (FLOAT)((1 << 20) - 1), 1.0f / (FLOAT)((1 << 4) - 1)};

    XMASSERT(pSource);

    I[0] = (UINT)pSource->v & 0xFFFFF;
    I[1] = (UINT)(pSource->v >> 20) & 0xFFFFF;
    I[2] = (UINT)(pSource->v >> 40) & 0xFFFFF;
    I[3] = (UINT)(pSource->v >> 60);

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    #define XM_SignedScale          (-XM_UNPACK_FACTOR_SIGNED)
    #define XM_UnsignedScaleOffset  (-XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << 4) - 1))
    #define XM_SignedOffset         (-XM_UNPACK_FACTOR_SIGNED * 3.0f)
    #define XM_URange               ((FLOAT)(1 << 20))
    #define XM_UMax                 ((FLOAT)((1 << 20) - 1))

    XMVECTOR V;
    XMVECTOR Scale1, Scale2, Mod;
    XMVECTOR Convert;
    XMVECTOR Result;

    static CONST XMVECTOR C0 = {XM_SignedScale / XM_URange,
                                XM_SignedScale / XM_UMax,
                                XM_SignedOffset / XM_URange + 1.0f,
                                XM_UnsignedScaleOffset};
    static CONST XMVECTOR C1 = {XM_URange / XM_UMax,
                                0.0f,
                                (XM_SignedOffset + XM_URange) / XM_UMax,
                                XM_UnsignedScaleOffset};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 7) == 0);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    V = __lvlx(pSource, 0);
    Scale1 = __vpermwi(C0, 0x3);
    V = __vsldoi(V, V, 2 << 2);
    Convert = __vpermwi(C0, 0xAB);
    Scale2 = __vpermwi(C0, 0x57);
    Result = __vpermwi(C1, 0xAB);
    V = __vupkd3d(V, VPACK_NORMPACKED64);
    Convert = __vnmsubfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vpermwi(C1, 0x1);
    Convert = __vrfiz(Convert);
    Result = __vnmsubfp(Convert, Mod, Result);

    return Result;

    #undef XM_SignedScale
    #undef XM_UnsignedScaleOffset
    #undef XM_SignedOffset
    #undef XM_URange
    #undef XM_UMax

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUIco4
(
    CONST XMUICO4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)(pSource->v & 0xFFFFF);
    V.v[1] = (FLOAT)((pSource->v >> 20) & 0xFFFFF);
    V.v[2] = (FLOAT)((pSource->v >> 40) & 0xFFFFF);
    V.v[3] = (FLOAT)(pSource->v >> 60);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;

    XMASSERT(pSource);

    I[0] = (UINT)pSource->v & 0xFFFFF;
    I[1] = (UINT)(pSource->v >> 20) & 0xFFFFF;
    I[2] = (UINT)(pSource->v >> 40) & 0xFFFFF;
    I[3] = (UINT)(pSource->v >> 60);

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    #define XM_SignedScale          (-XM_UNPACK_FACTOR_SIGNED)
    #define XM_UnsignedScaleOffset  (-XM_UNPACK_FACTOR_UNSIGNED)
    #define XM_SignedOffset         (-XM_UNPACK_FACTOR_SIGNED * 3.0f)
    #define XM_URange               ((FLOAT)(1 << 20))
    #define XM_UMax                 ((FLOAT)((1 << 20) - 1))

    XMVECTOR V;
    XMVECTOR Scale1, Scale2, Mod;
    XMVECTOR Convert;
    XMVECTOR Result;

    static CONST XMVECTOR C0 = {XM_SignedScale / XM_URange,
                                XM_SignedScale,
                                XM_SignedOffset / XM_URange + 1.0f,
                                XM_UnsignedScaleOffset};
    static CONST XMVECTOR C1 = {XM_URange,
                                0.0f,
                                XM_SignedOffset + XM_URange,
                                XM_UnsignedScaleOffset};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 7) == 0);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    V = __lvlx(pSource, 0);
    Scale1 = __vpermwi(C0, 0x3);
    V = __vsldoi(V, V, 2 << 2);
    Convert = __vpermwi(C0, 0xAB);
    Scale2 = __vpermwi(C0, 0x57);
    Result = __vpermwi(C1, 0xAB);
    V = __vupkd3d(V, VPACK_NORMPACKED64);
    Convert = __vnmsubfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vpermwi(C1, 0x1);
    Convert = __vrfiz(Convert);
    Result = __vnmsubfp(Convert, Mod, Result);

    return Result;

    #undef XM_SignedScale
    #undef XM_UnsignedScaleOffset
    #undef XM_SignedOffset
    #undef XM_URange
    #undef XM_UMax

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadIcoN4
(
    CONST XMICON4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtend[] = {0x00000000, 0xFFF00000};
    static CONST UINT SignExtendW[] = {0x00000000, 0xFFFFFFF0};

    XMASSERT(pSource);

    Element = (UINT)pSource->v & 0xFFFFF;
    V.v[0] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]) / 524287.0f;
    Element = (UINT)(pSource->v >> 20) & 0xFFFFF;
    V.v[1] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]) / 524287.0f;
    Element = (UINT)(pSource->v >> 40) & 0xFFFFF;
    V.v[2] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]) / 524287.0f;
    Element = (UINT)(pSource->v >> 60);
    V.v[3] = (FLOAT)(INT)(Element | SignExtendW[Element >> 3]) / 7.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtend[] = {0x00000000, 0xFFF00000};
    static CONST UINT      SignExtendW[] = {0x00000000, 0xFFFFFFF0};
    static CONST XMVECTOR  Scale = {1.0f / (FLOAT)((1 << (20 - 1)) - 1), 1.0f / (FLOAT)((1 << (20 - 1)) - 1), 1.0f / (FLOAT)((1 << (20 - 1)) - 1), 1.0f / (FLOAT)((1 << (4 - 1)) - 1)};

    XMASSERT(pSource);

    Element = (UINT)pSource->v & 0xFFFFF;
    I[0] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 20) & 0xFFFFF;
    I[1] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 40) & 0xFFFFF;
    I[2] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 60);
    I[3] = Element | SignExtendW[Element >> 3];

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    #define XM_SignedScale          (-XM_UNPACK_FACTOR_SIGNED)
    #define XM_UnsignedScaleOffset  (-XM_UNPACK_FACTOR_UNSIGNED)
    #define XM_SignedOffset         (-XM_UNPACK_FACTOR_SIGNED * 3.0f)
    #define XM_URange               ((FLOAT)(1 << 4))
    #define XM_URangeDiv2           ((FLOAT)(1 << 3))

    XMVECTOR V;
    XMVECTOR Scale1, Scale2, Mod;
    XMVECTOR Convert;
    XMVECTOR Result;

    static CONST XMVECTOR C0 = {XM_SignedScale / (FLOAT)((1 << (20 - 1)) - 1),
                                XM_SignedOffset / (FLOAT)((1 << (20 - 1)) - 1),
                                XM_URange / (FLOAT)((1 << (4 - 1)) - 1),
                                XM_UnsignedScaleOffset / (FLOAT)((1 << (4 - 1)) - 1)};
    static CONST XMVECTOR C1 = {XM_SignedScale,
                                XM_UnsignedScaleOffset / XM_URange,
                                XM_SignedOffset,
                                (XM_UnsignedScaleOffset + XM_URangeDiv2) / XM_URange};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 7) == 0);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    V = __lvlx(pSource, 0);
    Scale1 = __vpermwi(C1, 0x1);
    V = __vsldoi(V, V, 2 << 2);
    Convert = __vpermwi(C1, 0xAB);
    Scale2 = __vpermwi(C0, 0x3);
    Result = __vpermwi(C0, 0x57);
    V = __vupkd3d(V, VPACK_NORMPACKED64);
    Mod = __vspltisw(0);
    Convert = __vnmsubfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vrlimi(Mod, C0, 0x1, 3);
    Convert = __vrfiz(Convert);
    Result = __vnmsubfp(Convert, Mod, Result);

    return Result;

    #undef XM_SignedScale
    #undef XM_UnsignedScaleOffset
    #undef XM_SignedOffset
    #undef XM_URange
    #undef XM_URangeDiv2

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadIco4
(
    CONST XMICO4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtend[] = {0x00000000, 0xFFF00000};
    static CONST UINT SignExtendW[] = {0x00000000, 0xFFFFFFF0};

    XMASSERT(pSource);

    Element = (UINT)pSource->v & 0xFFFFF;
    V.v[0] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]);
    Element = (UINT)(pSource->v >> 20) & 0xFFFFF;
    V.v[1] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]);
    Element = (UINT)(pSource->v >> 40) & 0xFFFFF;
    V.v[2] = (FLOAT)(INT)(Element | SignExtend[Element >> 19]);
    Element = (UINT)(pSource->v >> 60);
    V.v[3] = (FLOAT)(INT)(Element | SignExtendW[Element >> 3]);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtend[] = {0x00000000, 0xFFF00000};
    static CONST UINT      SignExtendW[] = {0x00000000, 0xFFFFFFF0};

    XMASSERT(pSource);

    Element = (UINT)pSource->v & 0xFFFFF;
    I[0] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 20) & 0xFFFFF;
    I[1] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 40) & 0xFFFFF;
    I[2] = Element | SignExtend[Element >> 19];
    Element = (UINT)(pSource->v >> 60);
    I[3] = Element | SignExtendW[Element >> 3];

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    #define XM_SignedScale          (-XM_UNPACK_FACTOR_SIGNED)
    #define XM_UnsignedScaleOffset  (-XM_UNPACK_FACTOR_UNSIGNED)
    #define XM_SignedOffset         (-XM_UNPACK_FACTOR_SIGNED * 3.0f)
    #define XM_URange               ((FLOAT)(1 << 4))
    #define XM_URangeDiv2           ((FLOAT)(1 << 3))

    XMVECTOR V;
    XMVECTOR Scale1, Scale2, Mod;
    XMVECTOR Convert;
    XMVECTOR Result;

    static CONST XMVECTOR C0 = {XM_SignedScale,
                                XM_SignedOffset,
                                XM_UnsignedScaleOffset / XM_URange,
                                XM_UnsignedScaleOffset};
    static CONST XMVECTOR C1 = {0.0f,
                                XM_URange,
                                XM_SignedOffset,
                                (XM_UnsignedScaleOffset + XM_URangeDiv2) / XM_URange};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 7) == 0);
    XMASSERT((pSource->v & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 20) & 0xFFFFFull) != 0x80000ull);
    XMASSERT(((pSource->v >> 40) & 0xFFFFFull) != 0x80000ull);

    V = __lvlx(pSource, 0);
    Scale1 = __vpermwi(C0, 0x2);
    V = __vsldoi(V, V, 2 << 2);
    Convert = __vpermwi(C1, 0xAB);
    Scale2 = __vpermwi(C0, 0x3);
    Result = __vpermwi(C0, 0x57);
    V = __vupkd3d(V, VPACK_NORMPACKED64);
    Convert = __vnmsubfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vpermwi(C1, 0x1);
    Convert = __vrfiz(Convert);
    Result = __vnmsubfp(Convert, Mod, Result);

    return Result;

    #undef XM_SignedScale
    #undef XM_UnsignedScaleOffset
    #undef XM_SignedOffset
    #undef XM_URange
    #undef XM_URangeDiv2

#endif // _VMX128_INTRINSICS_
}


//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadXDecN4
(
    CONST XMXDECN4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtend[] = {0x00000000, 0xFFFFFC00};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]) / 511.0f;
    Element = (pSource->v >> 10) & 0x3FF;
    V.v[1] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]) / 511.0f;
    Element = (pSource->v >> 20) & 0x3FF;
    V.v[2] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]) / 511.0f;
    V.v[3] = (FLOAT)(pSource->v >> 30) / 3.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtend[] = {0x00000000, 0xFFFFFC00};
    static CONST XMVECTOR  Scale = {1.956947162e-3f, 1.956947162e-3f, 1.956947162e-3f, 3.333333333e-1f};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);

    Element = pSource->v & 0x3FF;
    I[0] = Element | SignExtend[Element >> 9];
    Element = (pSource->v >> 10) & 0x3FF;
    I[1] = Element | SignExtend[Element >> 9];
    Element = (pSource->v >> 20) & 0x3FF;
    I[2] = Element | SignExtend[Element >> 9];
    I[3] = pSource->v >> 30;

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                V;
    XMVECTOR                Result;
    XMVECTOR                UnpackScale;
    static CONST XMVECTOR   UnpackScaleOffset = {-XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << (10 - 1)) - 1), // Signed scale
                                                 -XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << 2) - 1), // Unsigned scale
                                                 -XM_UNPACK_FACTOR_SIGNED / (FLOAT)((1 << (10 - 1)) - 1) * 3.0f, // Signed offset
                                                 -XM_UNPACK_FACTOR_UNSIGNED / (FLOAT)((1 << 2) - 1)}; // Unsigned offset

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);

    V = __lvlx(pSource, 0);
    Result = __vpermwi(UnpackScaleOffset, 0xAB); // UnpackOffset
    UnpackScale = __vpermwi(UnpackScaleOffset, 0x1);
    V = __vsldoi(V, V, 1 << 2);
    V = __vupkd3d(V, VPACK_NORMPACKED32);
    Result = __vnmsubfp(V, UnpackScale, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadXDec4
(
    CONST XMXDEC4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtend[] = {0x00000000, 0xFFFFFC00};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]);
    Element = (pSource->v >> 10) & 0x3FF;
    V.v[1] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]);
    Element = (pSource->v >> 20) & 0x3FF;
    V.v[2] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]);
    V.v[3] = (FLOAT)(pSource->v >> 30);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtend[] = {0x00000000, 0xFFFFFC00};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);

    Element = pSource->v & 0x3FF;
    I[0] = Element | SignExtend[Element >> 9];
    Element = (pSource->v >> 10) & 0x3FF;
    I[1] = Element | SignExtend[Element >> 9];
    Element = (pSource->v >> 20) & 0x3FF;
    I[2] = Element | SignExtend[Element >> 9];
    I[3] = pSource->v >> 30;

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                V;
    XMVECTOR                Result;
    XMVECTOR                UnpackScale;
    static CONST XMVECTOR   UnpackScaleOffset = {-XM_UNPACK_FACTOR_SIGNED, // Signed scale
                                                 -XM_UNPACK_FACTOR_UNSIGNED, // Unsigned scale
                                                 -XM_UNPACK_FACTOR_SIGNED * 3.0f, // Signed offset
                                                 -XM_UNPACK_FACTOR_UNSIGNED}; // Unsigned offset

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);

    V = __lvlx(pSource, 0);
    Result = __vpermwi(UnpackScaleOffset, 0xAB); // UnpackOffset
    UnpackScale = __vpermwi(UnpackScaleOffset, 0x1);
    V = __vsldoi(V, V, 1 << 2);
    V = __vupkd3d(V, VPACK_NORMPACKED32);
    Result = __vnmsubfp(V, UnpackScale, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUDecN4
(
    CONST XMUDECN4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;

    XMASSERT(pSource);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)Element / 1023.0f;
    Element = (pSource->v >> 10) & 0x3FF;
    V.v[1] = (FLOAT)Element / 1023.0f;
    Element = (pSource->v >> 20) & 0x3FF;
    V.v[2] = (FLOAT)Element / 1023.0f;
    V.v[3] = (FLOAT)(pSource->v >> 30) / 3.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    static CONST XMVECTOR  Scale = {1.0f / (FLOAT)((1 << 10) - 1), 1.0f / (FLOAT)((1 << 10) - 1), 1.0f / (FLOAT)((1 << 10) - 1), 1.0f / (FLOAT)((1 << 2) - 1)};

    XMASSERT(pSource);

    I[0] = pSource->v & 0x3FF;
    I[1] = (pSource->v >> 10) & 0x3FF;
    I[2] = (pSource->v >> 20) & 0x3FF;
    I[3] = pSource->v >> 30;

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               Scale;
    XMVECTOR               Mask;
    static CONST XMVECTORI C0 = {0x3A802008, 0x3EAAAAAB, 0x3FF, 0}; // {1.0f / (FLOAT)((1 << 10) - 1), 1.0f / (FLOAT)((1 << 2) - 1), *(FLOAT*)&0x3FF, 0.0f};
    static CONST XMVECTORI Shift = {0, 10, 20, 30};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    V = __vspltw(V, 0);
    Mask = __vspltw(*(XMVECTOR*)C0, 2);
    V = __vsrw(V, *(XMVECTOR*)Shift);
    V = __vand(V, Mask);
    Scale = __vpermwi(*(XMVECTOR*)C0, 0x1);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, Scale);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUDec4
(
    CONST XMUDEC4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;

    XMASSERT(pSource);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)Element;
    Element = (pSource->v >> 10) & 0x3FF;
    V.v[1] = (FLOAT)Element;
    Element = (pSource->v >> 20) & 0x3FF;
    V.v[2] = (FLOAT)Element;
    V.v[3] = (FLOAT)(pSource->v >> 30);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;

    XMASSERT(pSource);

    I[0] = pSource->v & 0x3FF;
    I[1] = (pSource->v >> 10) & 0x3FF;
    I[2] = (pSource->v >> 20) & 0x3FF;
    I[3] = pSource->v >> 30;

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               Shift;
    XMVECTOR               Mask;
    static CONST XMVECTORI ShiftMask = {0x3FF0000, 0x3FF000A, 0x3FF0014, 0x3FF001E};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    Shift = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    Mask = __vsrw(*(XMVECTOR*)ShiftMask, Shift);
    V = __vsrw(V, *(XMVECTOR*)ShiftMask);
    V = __vand(V, Mask);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadDecN4
(
    CONST XMDECN4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtend[] = {0x00000000, 0xFFFFFC00};
    static CONST UINT SignExtendW[] = {0x00000000, 0xFFFFFFFC};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 30) & 0x3) != 0x2);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]) / 511.0f;
    Element = (pSource->v >> 10) & 0x3FF;
    V.v[1] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]) / 511.0f;
    Element = (pSource->v >> 20) & 0x3FF;
    V.v[2] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]) / 511.0f;
    Element = pSource->v >> 30;
    V.v[3] = (FLOAT)(SHORT)(Element | SignExtendW[Element >> 1]);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtend[] = {0x00000000, 0xFFFFFC00};
    static CONST UINT      SignExtendW[] = {0x00000000, 0xFFFFFFFC};
    static CONST XMVECTOR  Scale = {1.956947162e-3f, 1.956947162e-3f, 1.956947162e-3f, 1.0f};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 30) & 0x3) != 0x2);

    Element = pSource->v & 0x3FF;
    I[0] = Element | SignExtend[Element >> 9];
    Element = (pSource->v >> 10) & 0x3FF;
    I[1] = Element | SignExtend[Element >> 9];
    Element = (pSource->v >> 20) & 0x3FF;
    I[2] = Element | SignExtend[Element >> 9];
    Element = pSource->v >> 30;
    I[3] = Element | SignExtendW[Element >> 1];

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);
#if defined(_VMX32_INTRINSICS_)
    V = __vmaddfp(V, Scale, *(XMVECTOR*)g_XMNegativeZero);
#else
    V = __vmulfp(V, Scale);
#endif

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               ShiftR;
    static CONST XMVECTORI Scale = {0x3B004020, 0x3B004020, 0x3B004020, 0x3F800000}; // {1.0f / (FLOAT)((1 << (10 - 1)) - 1), ..., 1.0f / (FLOAT)((1 << (2 - 1)) - 1)};
    static CONST XMVECTORI Shift = {0x00160016, 0x0016000C, 0x00160002, 0x001E0000};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    ShiftR = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    ShiftR = __vsrw(*(XMVECTOR*)Shift, ShiftR);
    V = __vslw(V, *(XMVECTOR*)Shift);
    V = __vsraw(V, ShiftR);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, *(XMVECTOR*)Scale);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadDec4
(
    CONST XMDEC4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR          V;
    UINT              Element;
    static CONST UINT SignExtend[] = {0x00000000, 0xFFFFFC00};
    static CONST UINT SignExtendW[] = {0x00000000, 0xFFFFFFFC};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 30) & 0x3) != 0x2);

    Element = pSource->v & 0x3FF;
    V.v[0] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]);
    Element = (pSource->v >> 10) & 0x3FF;
    V.v[1] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]);
    Element = (pSource->v >> 20) & 0x3FF;
    V.v[2] = (FLOAT)(SHORT)(Element | SignExtend[Element >> 9]);
    Element = pSource->v >> 30;
    V.v[3] = (FLOAT)(SHORT)(Element | SignExtendW[Element >> 1]);

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR               V;
    XMVECTORI              I;
    UINT                   Element;
    static CONST UINT      SignExtend[] = {0x00000000, 0xFFFFFC00};
    static CONST UINT      SignExtendW[] = {0x00000000, 0xFFFFFFFC};

    XMASSERT(pSource);
    XMASSERT((pSource->v & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 10) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 20) & 0x3FF) != 0x200);
    XMASSERT(((pSource->v >> 30) & 0x3) != 0x2);

    Element = pSource->v & 0x3FF;
    I[0] = Element | SignExtend[Element >> 9];
    Element = (pSource->v >> 10) & 0x3FF;
    I[1] = Element | SignExtend[Element >> 9];
    Element = (pSource->v >> 20) & 0x3FF;
    I[2] = Element | SignExtend[Element >> 9];
    Element = pSource->v >> 30;
    I[3] = Element | SignExtendW[Element >> 1];

    V = __lvx(I, 0);
    V = __vcfsx(V, 0);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR               V;
    XMVECTOR               ShiftR;
    static CONST XMVECTORI Shift = {0x00160016, 0x0016000C, 0x00160002, 0x001E0000};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    ShiftR = __vspltisw(-16); // Shift 16
    V = __vspltw(V, 0);
    ShiftR = __vsrw(*(XMVECTOR*)Shift, ShiftR);
    V = __vslw(V, *(XMVECTOR*)Shift);
    V = __vsraw(V, ShiftR);
    V = __vcfsx(V, 0);

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUByteN4
(
    CONST XMUBYTEN4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)pSource->x / 255.0f;
    V.v[1] = (FLOAT)pSource->y / 255.0f;
    V.v[2] = (FLOAT)pSource->z / 255.0f;
    V.v[3] = (FLOAT)pSource->w / 255.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
    XMVECTOR                Zero;
    static CONST XMVECTORI  UnpackMove = {0x10111201, 0x14151602, 0x18191A03, 0x1C1D1E00};
    static CONST XMVECTOR   UnpackScale = {3.921568627e-3f, 3.921568627e-3f, 3.921568627e-3f, 3.921568627e-3f};
    static CONST XMVECTORI  SwizzleWXYZ = {XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Z};

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    Zero = __vspltisw(0);
    V = __vperm(V0, V1, Permute);
    V = __vperm(V, Zero, *(XMVECTOR*)UnpackMove);
    V = __vcfux(V, 0);
    V = __vmaddfp(V, UnpackScale, *(XMVECTOR*)g_XMNegativeZero);
    V = __vperm(V, V, *(XMVECTOR*)SwizzleWXYZ);

    return V;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
    XMVECTOR                Result;
    static CONST XMVECTOR   UnpackScaleOffset = XM_UNPACK_UNSIGNEDN_SCALE_OFFSET(8, 8, 8, 8);

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    Result = UnpackScaleOffset;
    Permute = __vsldoi(Permute, Permute, 1 << 2);
    V = __vperm(V0, V1, Permute);
    V = __vupkd3d(V, VPACK_D3DCOLOR);
    Result = __vnmsubfp(V, UnpackScaleOffset, Result);
    Result = __vpermwi(Result, 0xC6); // 3, 0, 1, 2

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR                V;
    XMVECTOR                Result;
    static CONST XMVECTOR   UnpackScaleOffset = XM_UNPACK_UNSIGNEDN_SCALE_OFFSET(8, 8, 8, 8);

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    Result = UnpackScaleOffset;
    V = __vsldoi(V, V, 1 << 2);
    V = __vupkd3d(V, VPACK_D3DCOLOR);
    Result = __vnmsubfp(V, UnpackScaleOffset, Result);
    Result = __vpermwi(Result, 0xC6); // 3, 0, 1, 2

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadUByte4
(
    CONST XMUBYTE4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)pSource->x;
    V.v[1] = (FLOAT)pSource->y;
    V.v[2] = (FLOAT)pSource->z;
    V.v[3] = (FLOAT)pSource->w;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Zero;
    XMVECTOR                Permute;
//    static CONST XMVECTORI  SwizzleWZYX = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0X};

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);
    Zero = __vspltisw(0);
    V = __vperm(V0, V1, Permute);
    V = __vmrghb(Zero, V);
    V = __vmrghh(Zero, V);
    V = __vcfux(V, 0);
//    V = __vperm(V, V, *(XMVECTOR*)SwizzleWZYX);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                V;
    XMVECTOR                Zero;

    XMASSERT(pSource);

    Zero = __vspltisw(0);

    V = __lvlx(pSource, 0);
    V = __vmrghb(Zero, V);
    V = __vmrghh(Zero, V);
    V = __vcfux(V, 0);
//    V = __vpermwi(V, 0xE4); // 3, 2, 1, 0

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadByteN4
(
    CONST XMBYTEN4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -128);
    XMASSERT(pSource->y != -128);
    XMASSERT(pSource->z != -128);
    XMASSERT(pSource->w != -128);

    V.v[0] = (FLOAT)pSource->x / 127.0f;
    V.v[1] = (FLOAT)pSource->y / 127.0f;
    V.v[2] = (FLOAT)pSource->z / 127.0f;
    V.v[3] = (FLOAT)pSource->w / 127.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
    static CONST XMVECTOR   Scale = {1.0f / (FLOAT)((1 << (8 - 1)) - 1), 
                                     1.0f / (FLOAT)((1 << (8 - 1)) - 1), 
                                     1.0f / (FLOAT)((1 << (8 - 1)) - 1), 
                                     1.0f / (FLOAT)((1 << (8 - 1)) - 1)};
//    static CONST XMVECTORI  SwizzleWZYX = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0X};

    XMASSERT(pSource);
    XMASSERT(pSource->x != -128);
    XMASSERT(pSource->y != -128);
    XMASSERT(pSource->z != -128);
    XMASSERT(pSource->w != -128);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    V = __vperm(V0, V1, Permute);
    V = __vupkhsb(V);
    V = __vupkhsh(V);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, Scale);
//    V = __vperm(V, V, *(XMVECTOR*)SwizzleWZYX);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR                V;
    static CONST XMVECTOR   Scale = {1.0f / (FLOAT)((1 << (8 - 1)) - 1), 
                                     1.0f / (FLOAT)((1 << (8 - 1)) - 1), 
                                     1.0f / (FLOAT)((1 << (8 - 1)) - 1), 
                                     1.0f / (FLOAT)((1 << (8 - 1)) - 1)};

    XMASSERT(pSource);
    XMASSERT(pSource->x != -128);
    XMASSERT(pSource->y != -128);
    XMASSERT(pSource->z != -128);
    XMASSERT(pSource->w != -128);

    V = __lvlx(pSource, 0);
    V = __vupkhsb(V);
    V = __vupkhsh(V);
    V = __vcfsx(V, 0);
    V = __vmulfp(V, Scale);
//    V = __vpermwi(V, 0xE4); // 3, 2, 1, 0

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadByte4
(
    CONST XMBYTE4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -128);
    XMASSERT(pSource->y != -128);
    XMASSERT(pSource->z != -128);
    XMASSERT(pSource->w != -128);

    V.v[0] = (FLOAT)pSource->x;
    V.v[1] = (FLOAT)pSource->y;
    V.v[2] = (FLOAT)pSource->z;
    V.v[3] = (FLOAT)pSource->w;

    return V;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
//    static CONST XMVECTORI  SwizzleWZYX = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0X};

    XMASSERT(pSource);
    XMASSERT(pSource->x != -128);
    XMASSERT(pSource->y != -128);
    XMASSERT(pSource->z != -128);
    XMASSERT(pSource->w != -128);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    V = __vperm(V0, V1, Permute);
    V = __vupkhsb(V);
    V = __vupkhsh(V);
    V = __vcfsx(V, 0);
//    V = __vperm(V, V, *(XMVECTOR*)SwizzleWZYX);

    return V;

#else // _VMX128_INTRINSICS_

    XMVECTOR V;

    XMASSERT(pSource);
    XMASSERT(pSource->x != -128);
    XMASSERT(pSource->y != -128);
    XMASSERT(pSource->z != -128);
    XMASSERT(pSource->w != -128);

    V = __lvlx(pSource, 0);
    V = __vupkhsb(V);
    V = __vupkhsh(V);
    V = __vcfsx(V, 0);
//    V = __vpermwi(V, 0xE4); // 3, 2, 1, 0

    return V;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMVECTOR XMLoadColor
(
    CONST XMCOLOR* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR V;

    XMASSERT(pSource);

    V.v[0] = (FLOAT)((pSource->c >> 16) & 0xFF) / 255.0f;
    V.v[1] = (FLOAT)((pSource->c >> 8) & 0xFF) / 255.0f;
    V.v[2] = (FLOAT)(pSource->c & 0xFF) / 255.0f;
    V.v[3] = (FLOAT)((pSource->c >> 24) & 0xFF) / 255.0f;

    return V;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
    XMVECTOR                Zero;
    static CONST XMVECTORI  UnpackMove = {0x10111201, 0x14151602, 0x18191A03, 0x1C1D1E00};
    static CONST XMVECTOR   UnpackScale = {3.921568627e-3f, 3.921568627e-3f, 3.921568627e-3f, 3.921568627e-3f};

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    Zero = __vspltisw(0);
    V = __vperm(V0, V1, Permute);                           // Shift the color into the most significant 32 bits
    V = __vperm(V, Zero, *(XMVECTOR*)UnpackMove);           // Move the 4 component values into the least significant bytes of each 32 bit element, 
                                                            //      rotate the components left by 1 (to move alpha in the least significant component), 
                                                            //      and fill the rest of the vector with 0s
    V = __vcfux(V, 0);                                      // Convert from integer to float
    V = __vmaddfp(V, UnpackScale, *(XMVECTOR*)g_XMNegativeZero);        // Scale into the correct range

    return V;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR                V;
    XMVECTOR                V0, V1;
    XMVECTOR                Permute;
    XMVECTOR                Result;
    static CONST XMVECTOR   UnpackScaleOffset = XM_UNPACK_UNSIGNEDN_SCALE_OFFSET(8, 8, 8, 8);

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 3);
    Result = UnpackScaleOffset;
    Permute = __vsldoi(Permute, Permute, 1 << 2);
    V = __vperm(V0, V1, Permute);
    V = __vupkd3d(V, VPACK_D3DCOLOR);
    Result = __vnmsubfp(V, UnpackScaleOffset, Result);

    return Result;

#else // _VMX128_INTRINSICS_

    XMVECTOR                V;
    XMVECTOR                Result;
    static CONST XMVECTOR   UnpackScaleOffset = XM_UNPACK_UNSIGNEDN_SCALE_OFFSET(8, 8, 8, 8);

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    V = __lvlx(pSource, 0);
    Result = UnpackScaleOffset;
    V = __vsldoi(V, V, 1 << 2);
    V = __vupkd3d(V, VPACK_D3DCOLOR);
    Result = __vnmsubfp(V, UnpackScaleOffset, Result);

    return Result;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMLoadFloat3x3
(
    CONST XMFLOAT3X3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;

    XMASSERT(pSource);

    M.r[0].v[0] = pSource->m[0][0];
    M.r[0].v[1] = pSource->m[0][1];
    M.r[0].v[2] = pSource->m[0][2];
    M.r[0].v[3] = 0.0f;

    M.r[1].v[0] = pSource->m[1][0];
    M.r[1].v[1] = pSource->m[1][1];
    M.r[1].v[2] = pSource->m[1][2];
    M.r[1].v[3] = 0.0f;

    M.r[2].v[0] = pSource->m[2][0];
    M.r[2].v[1] = pSource->m[2][1];
    M.r[2].v[2] = pSource->m[2][2];
    M.r[2].v[3] = 0.0f;

    M.r[3].v[0] = 0.0f;
    M.r[3].v[1] = 0.0f;
    M.r[3].v[2] = 0.0f;
    M.r[3].v[3] = 1.0f;

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V0, V1, V2, V3;
    XMVECTOR P0, P1, P2;
    XMVECTOR R0, R1, R2;
    XMMATRIX M;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    V1 = __lvx(pSource, 12);
    V2 = __lvx(pSource, 24);
    V3 = __lvx(pSource, 35);

    P0 = __lvsl(pSource, 0);
    P1 = __lvsl(pSource, 12);
    P2 = __lvsl(pSource, 24);

    R0 = __vperm(V0, V1, P0);
    R1 = __vperm(V1, V2, P1);
    R2 = __vperm(V2, V3, P2);
    M.r[3] = g_XMIdentityR3;

    M.r[0] = __vsel(*(XMVECTOR*)g_XMSelect1110, R0, *(XMVECTOR*)g_XMSelect1110);
    M.r[1] = __vsel(*(XMVECTOR*)g_XMSelect1110, R1, *(XMVECTOR*)g_XMSelect1110);
    M.r[2] = __vsel(*(XMVECTOR*)g_XMSelect1110, R2, *(XMVECTOR*)g_XMSelect1110);

    return M;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V0, V1, V2, V3;
    XMVECTOR P0, P1, P2;
    XMVECTOR ZO;
    XMMATRIX M;

    XMASSERT(pSource);

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    V0 = __lvx(pSource, 0);
    V1 = __lvx(pSource, 12);
    V2 = __lvx(pSource, 24);
    V3 = __lvx(pSource, 35);

    P0 = __lvsl(pSource, 0);
    P1 = __lvsl(pSource, 12);
    P2 = __lvsl(pSource, 24);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vperm(V0, V1, P0);
    M.r[1] = __vperm(V1, V2, P1);
    M.r[2] = __vperm(V2, V3, P2);
    M.r[3] = __vpermwi(ZO, 0xAB);

    M.r[0] = __vrlimi(M.r[0], ZO, 0x1, 3);
    M.r[1] = __vrlimi(M.r[1], ZO, 0x1, 3);
    M.r[2] = __vrlimi(M.r[2], ZO, 0x1, 3);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR V0L, V0R;
    XMVECTOR V1L, V1R, V1;
    XMVECTOR V2;
    XMVECTOR ZO;
    XMMATRIX M;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 3) == 0);

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    V0L = __lvlx(pSource, 0);
    V0R = __lvrx(pSource, 16);
    V1L = __lvlx(pSource, 16);
    V1R = __lvrx(pSource, 32);
    V2  = __lvlx(pSource, 32);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vor(V0L, V0R);
    V1 = __vor(V1L, V1R);
    V2 = __vrlimi(V2, ZO, 0x4, 1);

    M.r[1] = __vsldoi(M.r[0], V1, 3 << 2);

    M.r[3] = __vpermwi(ZO, 0xAB);
    M.r[0] = __vrlimi(M.r[0], ZO, 0x1, 3);
    M.r[2] = __vsldoi(V1, V2, 2 << 2);
    M.r[1] = __vrlimi(M.r[1], ZO, 0x1, 3);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMLoadFloat4x3
(
    CONST XMFLOAT4X3* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;

    XMASSERT(pSource);

    M.r[0].v[0] = pSource->m[0][0];
    M.r[0].v[1] = pSource->m[0][1];
    M.r[0].v[2] = pSource->m[0][2];
    M.r[0].v[3] = 0.0f;

    M.r[1].v[0] = pSource->m[1][0];
    M.r[1].v[1] = pSource->m[1][1];
    M.r[1].v[2] = pSource->m[1][2];
    M.r[1].v[3] = 0.0f;

    M.r[2].v[0] = pSource->m[2][0];
    M.r[2].v[1] = pSource->m[2][1];
    M.r[2].v[2] = pSource->m[2][2];
    M.r[2].v[3] = 0.0f;

    M.r[3].v[0] = pSource->m[3][0];
    M.r[3].v[1] = pSource->m[3][1];
    M.r[3].v[2] = pSource->m[3][2];
    M.r[3].v[3] = 1.0f;

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V0, V1, V2, V3, V4;
    XMVECTOR P0, P1, P2, P3;
    XMVECTOR R0, R1, R2, R3;
    XMMATRIX M;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    V1 = __lvx(pSource, 12);
    V2 = __lvx(pSource, 24);
    V3 = __lvx(pSource, 36);
    V4 = __lvx(pSource, 47);

    P0 = __lvsl(pSource, 0);
    P1 = __lvsl(pSource, 12);
    P2 = __lvsl(pSource, 24);
    P3 = __lvsl(pSource, 36);

    R0 = __vperm(V0, V1, P0);
    R1 = __vperm(V1, V2, P1);
    R2 = __vperm(V2, V3, P2);
    R3 = __vperm(V3, V4, P3);

    M.r[0] = __vsel(*(XMVECTOR*)g_XMSelect1110, R0, *(XMVECTOR*)g_XMSelect1110);
    M.r[1] = __vsel(*(XMVECTOR*)g_XMSelect1110, R1, *(XMVECTOR*)g_XMSelect1110);
    M.r[2] = __vsel(*(XMVECTOR*)g_XMSelect1110, R2, *(XMVECTOR*)g_XMSelect1110);
    M.r[3] = __vsel(g_XMIdentityR3, R3, *(XMVECTOR*)g_XMSelect1110);

    return M;

#elif defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMVECTOR V0, V1, V2, V3, V4;
    XMVECTOR P0, P1, P2, P3;
    XMVECTOR ZO;
    XMMATRIX M;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    V1 = __lvx(pSource, 12);
    V2 = __lvx(pSource, 24);
    V3 = __lvx(pSource, 36);
    V4 = __lvx(pSource, 47);

    P0 = __lvsl(pSource, 0);
    P1 = __lvsl(pSource, 12);
    P2 = __lvsl(pSource, 24);
    P3 = __lvsl(pSource, 36);

    XMDUMMY_INITIALIZE_VECTOR(ZO);

    M.r[0] = __vperm(V0, V1, P0);
    M.r[1] = __vperm(V1, V2, P1);
    M.r[2] = __vperm(V2, V3, P2);
    M.r[3] = __vperm(V3, V4, P3);

    ZO = __vupkd3d(ZO, VPACK_NORMSHORT2);

    M.r[0] = __vrlimi(M.r[0], M.r[3], 0x1, 3);
    M.r[1] = __vrlimi(M.r[1], M.r[3], 0x1, 3);
    M.r[2] = __vrlimi(M.r[2], M.r[3], 0x1, 3);
    M.r[3] = __vrlimi(M.r[3], ZO, 0x1, 0);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR V0L, V0R;
    XMVECTOR V1L, V1R, V1;
    XMVECTOR V2L, V2R, V2;
    XMMATRIX M;

    XMASSERT(pSource);

    XMDUMMY_INITIALIZE_VECTOR(M.r[3]);

    V0L = __lvlx(pSource, 0);
    V0R = __lvrx(pSource, 16);
    V1L = __lvlx(pSource, 16);
    V1R = __lvrx(pSource, 32);
    V2L = __lvlx(pSource, 32);
    V2R = __lvrx(pSource, 48);

    M.r[3] = __vupkd3d(M.r[3], VPACK_NORMSHORT2);

    M.r[0] = __vor(V0L, V0R);
    V1 = __vor(V1L, V1R);
    V2 = __vor(V2L, V2R);

    M.r[1] = __vsldoi(M.r[0], V1, 3 << 2);
    M.r[2] = __vsldoi(V1, V2, 2 << 2);

    M.r[0] = __vrlimi(M.r[0], M.r[3], 0x1, 3);
    M.r[1] = __vrlimi(M.r[1], M.r[3], 0x1, 3);
    M.r[2] = __vrlimi(M.r[2], M.r[3], 0x1, 3);
    M.r[3] = __vrlimi(M.r[3], V2, 0xE, 1);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMLoadFloat4x3A
(
    CONST XMFLOAT4X3A* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    M.r[0].v[0] = pSource->m[0][0];
    M.r[0].v[1] = pSource->m[0][1];
    M.r[0].v[2] = pSource->m[0][2];
    M.r[0].v[3] = 0.0f;

    M.r[1].v[0] = pSource->m[1][0];
    M.r[1].v[1] = pSource->m[1][1];
    M.r[1].v[2] = pSource->m[1][2];
    M.r[1].v[3] = 0.0f;

    M.r[2].v[0] = pSource->m[2][0];
    M.r[2].v[1] = pSource->m[2][1];
    M.r[2].v[2] = pSource->m[2][2];
    M.r[2].v[3] = 0.0f;

    M.r[3].v[0] = pSource->m[3][0];
    M.r[3].v[1] = pSource->m[3][1];
    M.r[3].v[2] = pSource->m[3][2];
    M.r[3].v[3] = 1.0f;

    return M;

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR V0, V1, V2;
    XMVECTOR R1, R2;
    XMMATRIX M;
    static CONST XMVECTORI P1 = {XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_0W};
    static CONST XMVECTORI P2 = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_0W};
    static CONST XMVECTORI P3 = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X};

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    V0 = __lvx(pSource, 0);
    V1 = __lvx(pSource, 16);
    V2 = __lvx(pSource, 32);

    R1 = __vperm(V0, V1, *(XMVECTOR*)P1);
    R2 = __vperm(V1, V2, *(XMVECTOR*)P2);
    M.r[0] = __vsel(*(XMVECTOR*)g_XMSelect1110, V0, *(XMVECTOR*)g_XMSelect1110);
    M.r[3] = __vperm(V2, g_XMOne, *(XMVECTOR*)P3);
    M.r[1] = __vsel(*(XMVECTOR*)g_XMSelect1110, R1, *(XMVECTOR*)g_XMSelect1110);
    M.r[2] = __vsel(*(XMVECTOR*)g_XMSelect1110, R2, *(XMVECTOR*)g_XMSelect1110);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR V1, V2;
    XMMATRIX M;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    XMDUMMY_INITIALIZE_VECTOR(M.r[3]);

    M.r[0] = __lvx(pSource, 0);
    V1 = __lvx(pSource, 16);
    V2 = __lvx(pSource, 32);

    M.r[3] = __vupkd3d(M.r[3], VPACK_NORMSHORT2);

    M.r[1] = __vsldoi(M.r[0], V1, 3 << 2);
    M.r[2] = __vsldoi(V1, V2, 2 << 2);

    M.r[0] = __vrlimi(M.r[0], M.r[3], 0x1, 3);
    M.r[1] = __vrlimi(M.r[1], M.r[3], 0x1, 3);
    M.r[2] = __vrlimi(M.r[2], M.r[3], 0x1, 3);
    M.r[3] = __vrlimi(M.r[3], V2, 0xE, 1);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMLoadFloat4x4
(
    CONST XMFLOAT4X4* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;

    XMASSERT(pSource);

    M.r[0].v[0] = pSource->m[0][0];
    M.r[0].v[1] = pSource->m[0][1];
    M.r[0].v[2] = pSource->m[0][2];
    M.r[0].v[3] = pSource->m[0][3];

    M.r[1].v[0] = pSource->m[1][0];
    M.r[1].v[1] = pSource->m[1][1];
    M.r[1].v[2] = pSource->m[1][2];
    M.r[1].v[3] = pSource->m[1][3];

    M.r[2].v[0] = pSource->m[2][0];
    M.r[2].v[1] = pSource->m[2][1];
    M.r[2].v[2] = pSource->m[2][2];
    M.r[2].v[3] = pSource->m[2][3];

    M.r[3].v[0] = pSource->m[3][0];
    M.r[3].v[1] = pSource->m[3][1];
    M.r[3].v[2] = pSource->m[3][2];
    M.r[3].v[3] = pSource->m[3][3];

    return M;

#elif defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMMATRIX M;
    XMVECTOR V0, V1, V2, V3, V4;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 16);
    V2 = __lvx(pSource, 32);
    V3 = __lvx(pSource, 48);
    V4 = __lvx(pSource, 63);
    M.r[0] = __vperm(V0, V1, Permute);
    M.r[1] = __vperm(V1, V2, Permute);
    M.r[2] = __vperm(V2, V3, Permute);
    M.r[3] = __vperm(V3, V4, Permute);

    return M;

#else // _VMX128_INTRINSICS_

    XMVECTOR V0L, V0R;
    XMVECTOR V1L, V1R;
    XMVECTOR V2L, V2R;
    XMVECTOR V3L, V3R;
    XMMATRIX M;

    XMASSERT(pSource);

    V0L = __lvlx(pSource, 0);
    V0R = __lvrx(pSource, 16);
    V1L = __lvlx(pSource, 16);
    V1R = __lvrx(pSource, 32);
    V2L = __lvlx(pSource, 32);
    V2R = __lvrx(pSource, 48);
    V3L = __lvlx(pSource, 48);
    V3R = __lvrx(pSource, 64);

    M.r[0] = __vor(V0L, V0R);
    M.r[1] = __vor(V1L, V1R);
    M.r[2] = __vor(V2L, V2R);
    M.r[3] = __vor(V3L, V3R);

    return M;

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE XMMATRIX XMLoadFloat4x4A
(
    CONST XMFLOAT4X4A* pSource
)
{
#if defined(_NO_INTRINSICS_)

    XMMATRIX M;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    M.r[0].v[0] = pSource->m[0][0];
    M.r[0].v[1] = pSource->m[0][1];
    M.r[0].v[2] = pSource->m[0][2];
    M.r[0].v[3] = pSource->m[0][3];

    M.r[1].v[0] = pSource->m[1][0];
    M.r[1].v[1] = pSource->m[1][1];
    M.r[1].v[2] = pSource->m[1][2];
    M.r[1].v[3] = pSource->m[1][3];

    M.r[2].v[0] = pSource->m[2][0];
    M.r[2].v[1] = pSource->m[2][1];
    M.r[2].v[2] = pSource->m[2][2];
    M.r[2].v[3] = pSource->m[2][3];

    M.r[3].v[0] = pSource->m[3][0];
    M.r[3].v[1] = pSource->m[3][1];
    M.r[3].v[2] = pSource->m[3][2];
    M.r[3].v[3] = pSource->m[3][3];

    return M;

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMMATRIX M;

    XMASSERT(pSource);
    XMASSERT(((UINT_PTR)pSource & 0xF) == 0);

    M.r[0] = __lvx(pSource, 0);
    M.r[1] = __lvx(pSource, 16);
    M.r[2] = __lvx(pSource, 32);
    M.r[3] = __lvx(pSource, 48);

    return M;

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

/****************************************************************************
 *
 * Vector and matrix store operations
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreScalar
(
    VOID*    pDestination,
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pDestination;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    *pElement = V.u[0];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR X;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    X = __vspltw(V, 0);
    __stvewx(X, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreVector2
(
    VOID*    pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pDestination;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    pElement[0] = V.u[0];
    pElement[1] = V.u[1];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR X, Y;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    X = __vspltw(V, 0);
    Y = __vspltw(V, 1);
    __stvewx(X, pDestination, 0);
    __stvewx(Y, pDestination, 4);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat2
(
    XMFLOAT2* pDestination, 
    XMVECTOR  V
)
{
    XMStoreVector2(pDestination, V);
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreVector2A
(
    VOID*    pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pDestination;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    pElement[0] = V.u[0];
    pElement[1] = V.u[1];

#elif defined(_VMX32_INTRINSICS_)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    __stvewx(V, pDestination, 0);
    __stvewx(V, pDestination, 4);

#else // _VMX128_INTRINSICS_

    XMVECTOR AlignedV;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    AlignedV = __vsldoi(V, V, 2 << 2);
    __stvrx(AlignedV, pDestination, 8);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat2A
(
    XMFLOAT2A*   pDestination, 
    XMVECTOR     V
)
{
    XMStoreVector2A(pDestination, V);
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreHalf2
(
    XMHALF2* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMASSERT(pDestination);

    pDestination->x = XMConvertFloatToHalf(V.v[0]);
    pDestination->y = XMConvertFloatToHalf(V.v[1]);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR H;
    static CONST XMVECTORI Permute = {0x02030607, 0x02030607, 0x02030607, 0x02030607};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    H = XMConvertVectorFloatToHalf(V);
    H = __vperm(H, H, *(XMVECTOR*)Permute);

    __stvewx(H, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR H;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    XMDUMMY_INITIALIZE_VECTOR(H);
    H = __vpkd3d(H, V, VPACK_FLOAT16_2, VPACK_32, 3);
    H = __vspltw(H, 0);

    __stvewx(H, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreShortN2
(
    XMSHORTN2* pDestination, 
    XMVECTOR   V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {32767.0f, 32767.0f, 32767.0f, 32767.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, g_XMNegativeOne, g_XMOne);
    N = XMVectorMultiply(N, Scale);
    N = XMVectorRound(N);

    pDestination->x = (SHORT)N.v[0];
    pDestination->y = (SHORT)N.v[1];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                N;
    static CONST XMVECTOR   PackScale = {7.8122615814208984375e-3f, 7.8122615814208984375e-3f, 7.8122615814208984375e-3f, 7.8122615814208984375e-3f}; // 32767.0f / (FLOAT)(1 << 22)
    static CONST XMVECTOR   Three = {3.0f, 3.0f, 3.0f, 3.0f};
    static CONST XMVECTORI  ShiftAmount = {10, 10, 10, 10};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    N = __vmaddfp(V, PackScale, Three);                     // Scale into the normalized range, adjust the exponent, shift the mantissa into the correct location, and round the result into an integer form
    N = __vslw(N, *(XMVECTOR*)ShiftAmount);
    N = __vsraw(N, *(XMVECTOR*)ShiftAmount);                // Sign extend the lower 22 bits
    N = __vpkswss(N, N);                                    // Saturate into signed short range and pack the short values
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR N;
    static CONST XMVECTOR PackScale = XM_PACK_SIGNEDN_SCALE(16, 16, 16, 16);

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    N = XM_PACK_OFFSET;
    N = __vnmsubfp(V, PackScale, N);
    N = __vpkd3d(N, N, VPACK_NORMSHORT2, VPACK_32, 3);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreShort2
(
    XMSHORT2* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-32767.0f, -32767.0f, -32767.0f, -32767.0f};
    static CONST XMVECTOR  Max = {32767.0f, 32767.0f, 32767.0f, 32767.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);
    N = XMVectorRound(N);

    pDestination->x = (SHORT)N.v[0];
    pDestination->y = (SHORT)N.v[1];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR N;

    N = __vctsxs(V, 0);
    N = __vpkswss(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUShortN2
(
    XMUSHORTN2* pDestination, 
    XMVECTOR    V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {65535.0f, 65535.0f, 65535.0f, 65535.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), g_XMOne);
    N = XMVectorMultiplyAdd(N, Scale, g_XMOneHalf);
    N = XMVectorTruncate(N);

    pDestination->x = (SHORT)N.v[0];
    pDestination->y = (SHORT)N.v[1];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR N;
    XMVECTOR Scale, Offset;
    static CONST XMVECTOR C0 = {65535.0f, 0.5f, 0.0f, 0.0f};

    Scale = __vspltw(C0, 0);
    Offset = __vspltw(C0, 1);
    N = __vmaddfp(V, Scale, Offset);
    N = __vctuxs(N, 0);
    N = __vpkuwus(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUShort2
(
    XMUSHORT2* pDestination, 
    XMVECTOR   V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Max = {65535.0f, 65535.0f, 65535.0f, 65535.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), Max);
    N = XMVectorRound(N);

    pDestination->x = (SHORT)N.v[0];
    pDestination->y = (SHORT)N.v[1];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR N;

    N = __vctuxs(V, 0);
    N = __vpkuwus(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreVector3
(
    VOID*    pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pDestination;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    pElement[0] = V.u[0];
    pElement[1] = V.u[1];
    pElement[2] = V.u[2];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR S;
    XMVECTOR Permute;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Permute = __lvsr(pDestination, 0);                  // Compute the permute control vector for shift right
    S = __vperm(V, V, Permute);                         // Right rotate the vector

    __stvewx(S, pDestination, 0);
    __stvewx(S, pDestination, 4);
    __stvewx(S, pDestination, 8);

#else // _VMX128_INTRINSICS_

    XMVECTOR X, Y, Z;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    X = __vspltw(V, 0);
    Y = __vspltw(V, 1);
    Z = __vspltw(V, 2);

    __stvewx(X, pDestination, 0);
    __stvewx(Y, pDestination, 4);
    __stvewx(Z, pDestination, 8);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat3
(
    XMFLOAT3* pDestination, 
    XMVECTOR  V
)
{
    XMStoreVector3(pDestination, V);
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreVector3A
(
    VOID*    pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pDestination;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    pElement[0] = V.u[0];
    pElement[1] = V.u[1];
    pElement[2] = V.u[2];

#elif defined(_VMX32_INTRINSICS_)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    __stvewx(V, pDestination, 0);
    __stvewx(V, pDestination, 4);
    __stvewx(V, pDestination, 8);

#else // _VMX128_INTRINSICS_

    XMVECTOR AlignedV;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    AlignedV = __vsldoi(V, V, 3 << 2);
    __stvrx(AlignedV, pDestination, 12);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat3A
(
    XMFLOAT3A*   pDestination, 
    XMVECTOR     V
)
{
    XMStoreVector3A(pDestination, V);
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUHenDN3
(
    XMUHENDN3* pDestination, 
    XMVECTOR   V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {2047.0f, 2047.0f, 1023.0f, 0.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), g_XMOne);
    N = XMVectorMultiply(N, Scale);

    pDestination->v = (((UINT)N.v[2] & 0x3FF) << 22) |
                      (((UINT)N.v[1] & 0x7FF) << 11) |
                      (((UINT)N.v[0] & 0x7FF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale, ScaleXY, OneHalf;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x447FC000, 0x0007FF0B, 0x3FFFFF16, 0x44FFE000}; // (FLOAT)((1 << 10) - 1), ..., (FLOAT)((1 << 11) - 1)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    OneHalf = __vspltisw(1);
    ScaleXY = __vspltw(*(XMVECTOR*)C0, 3);
    Scale = __vsldoi(ScaleXY, *(XMVECTOR*)C0, 2 << 2);
    OneHalf = __vcfsx(OneHalf, 1);

    Convert = __vmaddfp(V, Scale, OneHalf);
    Select = __vspltisw(8);
    Select = __vsrw(*(XMVECTOR*)C0, Select);
    Convert = __vctuxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale, OneHalf;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x44FFE000, 0x0007FF0B, 0x3FFFFF16, 0x447FC000}; // (FLOAT)((1 << 11) - 1), ..., (FLOAT)((1 << 10) - 1)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    OneHalf = __vspltisw(1);
    OneHalf = __vcfsx(OneHalf, 1);
    Scale = __vpermwi(*(XMVECTOR*)C0, 0xC);

    Convert = __vmaddfp(V, Scale, OneHalf);
    Select = __vspltisw(8);
    Select = __vsrw(*(XMVECTOR*)C0, Select);
    Convert = __vctuxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUHenD3
(
    XMUHEND3* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Max = {2047.0f, 2047.0f, 1023.0f, 0.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), Max);

    pDestination->v = (((UINT)N.v[2] & 0x3FF) << 22) |
                      (((UINT)N.v[1] & 0x7FF) << 11) |
                      (((UINT)N.v[0] & 0x7FF));

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0xFFC00000, 0x0000000B, 0x00000016, 0x000007FF};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Convert = __vctuxs(V, 0);
    XY = __vspltw(*(XMVECTOR*)C0, 3);
    S = __vspltw(*(XMVECTOR*)C0, 0);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(XY, Z, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreHenDN3
(
    XMHENDN3* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {1023.0f, 1023.0f, 511.0f, 1.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, g_XMNegativeOne, g_XMOne);
    N = XMVectorMultiply(N, Scale);

    pDestination->v = (((INT)N.v[2] & 0x3FF) << 22) |
                      (((INT)N.v[1] & 0x7FF) << 11) |
                      (((INT)N.v[0] & 0x7FF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale, ScaleXY;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x43FF8000, 0x0007FF0B, 0x3FFFFF16, 0x447FC000}; // (FLOAT)((1 << (10 - 1)) - 1), ..., (FLOAT)((1 << (11 - 1)) - 1)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    ScaleXY = __vspltw(*(XMVECTOR*)C0, 3);
    Scale = __vsldoi(ScaleXY, *(XMVECTOR*)C0, 2 << 2);

    Convert = __vmulfp(V, Scale);
    Select = __vspltisw(8);
    Convert = __vrfin(Convert);
    Select = __vsrw(*(XMVECTOR*)C0, Select);
    Convert = __vctsxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x447FC000, 0x0007FF0B, 0x3FFFFF16, 0x43FF8000}; // (FLOAT)((1 << (11 - 1)) - 1), ..., (FLOAT)((1 << (10 - 1)) - 1)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Scale = __vpermwi(*(XMVECTOR*)C0, 0xC);

    Convert = __vmulfp(V, Scale);
    Select = __vspltisw(8);
    Convert = __vrfin(Convert);
    Select = __vsrw(*(XMVECTOR*)C0, Select);
    Convert = __vctsxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreHenD3
(
    XMHEND3* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-1023.0f, -1023.0f, -511.0f, -1.0f};
    static CONST XMVECTOR  Max = {1023.0f, 1023.0f, 511.0f, 1.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);

    pDestination->v = (((INT)N.v[2] & 0x3FF) << 22) |
                      (((INT)N.v[1] & 0x7FF) << 11) |
                      (((INT)N.v[0] & 0x7FF));

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0xFFC00000, 0x0000000B, 0x00000016, 0x000007FF};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Convert = __vctsxs(V, 0);
    XY = __vspltw(*(XMVECTOR*)C0, 3);
    S = __vspltw(*(XMVECTOR*)C0, 0);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(XY, Z, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUDHenN3
(
    XMUDHENN3* pDestination, 
    XMVECTOR   V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {1023.0f, 2047.0f, 2047.0f, 0.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), g_XMOne);
    N = XMVectorMultiply(N, Scale);

    pDestination->v = (((UINT)N.v[2] & 0x7FF) << 21) |
                      (((UINT)N.v[1] & 0x7FF) << 10) |
                      (((UINT)N.v[0] & 0x3FF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale, ScaleYZ, OneHalf;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x44FFE000, 0x0003FF0A, 0x1FFFFF15, 0x447FC000}; // (FLOAT)((1 << 11) - 1), ..., (FLOAT)((1 << 10) - 1)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    OneHalf = __vspltisw(1);
    OneHalf = __vcfsx(OneHalf, 1);
    ScaleYZ = __vspltw(*(XMVECTOR*)C0, 0);
    Scale = __vsldoi(*(XMVECTOR*)C0, ScaleYZ, 3 << 2);

    Convert = __vmaddfp(V, Scale, OneHalf);
    Select = __vspltisw(8);
    Select = __vsrw(*(XMVECTOR*)C0, Select);
    Convert = __vctuxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale, OneHalf;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x44FFE000, 0x0003FF0A, 0x1FFFFF15, 0x447FC000}; // (FLOAT)((1 << 11) - 1), ..., (FLOAT)((1 << 10) - 1)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    OneHalf = __vspltisw(1);
    OneHalf = __vcfsx(OneHalf, 1);
    Scale = __vpermwi(*(XMVECTOR*)C0, 0xC0);

    Convert = __vmaddfp(V, Scale, OneHalf);
    Select = __vspltisw(8);
    Select = __vsrw(*(XMVECTOR*)C0, Select);
    Convert = __vctuxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUDHen3
(
    XMUDHEN3* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Max = {1023.0f, 2047.0f, 2047.0f, 0.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), Max);

    pDestination->v = (((UINT)N.v[2] & 0x7FF) << 21) |
                      (((UINT)N.v[1] & 0x7FF) << 10) |
                      (((UINT)N.v[0] & 0x3FF));

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0xFFE00000, 0x0000000A, 0x00000015, 0x000003FF};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Convert = __vctuxs(V, 0);
    XY = __vspltw(*(XMVECTOR*)C0, 3);
    S = __vspltw(*(XMVECTOR*)C0, 0);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(XY, Z, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreDHenN3
(
    XMDHENN3* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {511.0f, 1023.0f, 1023.0f, 1.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, g_XMNegativeOne, g_XMOne);
    N = XMVectorMultiply(N, Scale);

    pDestination->v = (((INT)N.v[2] & 0x7FF) << 21) |
                      (((INT)N.v[1] & 0x7FF) << 10) |
                      (((INT)N.v[0] & 0x3FF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale, ScaleYZ;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x447FC000, 0x0003FF0A, 0x1FFFFF15, 0x43FF8000}; // (FLOAT)((1 << (11 - 1)) - 1), ..., (FLOAT)((1 << (10 - 1)) - 1)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    ScaleYZ = __vspltw(*(XMVECTOR*)C0, 0);
    Scale = __vsldoi(*(XMVECTOR*)C0, ScaleYZ, 3 << 2);

    Convert = __vmulfp(V, Scale);
    Select = __vspltisw(8);
    Convert = __vrfin(Convert);
    Select = __vsrw(*(XMVECTOR*)C0, Select);
    Convert = __vctsxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x447FC000, 0x0003FF0A, 0x1FFFFF15, 0x43FF8000}; // (FLOAT)((1 << (11 - 1)) - 1), ..., (FLOAT)((1 << (10 - 1)) - 1)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Scale = __vpermwi(*(XMVECTOR*)C0, 0xC0);

    Convert = __vmulfp(V, Scale);
    Select = __vspltisw(8);
    Convert = __vrfin(Convert);
    Select = __vsrw(*(XMVECTOR*)C0, Select);
    Convert = __vctsxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreDHen3
(
    XMDHEN3* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-511.0f, -1023.0f, -1023.0f, -1.0f};
    static CONST XMVECTOR  Max = {511.0f, 1023.0f, 1023.0f, 1.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);

    pDestination->v = (((INT)N.v[2] & 0x7FF) << 21) |
                      (((INT)N.v[1] & 0x7FF) << 10) |
                      (((INT)N.v[0] & 0x3FF));

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0xFFE00000, 0x0000000A, 0x00000015, 0x000003FF};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Convert = __vctsxs(V, 0);
    XY = __vspltw(*(XMVECTOR*)C0, 3);
    S = __vspltw(*(XMVECTOR*)C0, 0);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(XY, Z, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreVector4
(
    VOID*    pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMStoreVector4NC(pDestination, V);

#else // _VMX128_INTRINSICS_

    XMASSERT(pDestination);

    __stvlx(V, pDestination, 0);
    __stvrx(V, pDestination, 16);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4
(
    XMFLOAT4* pDestination, 
    XMVECTOR  V
)
{
    XMStoreVector4(pDestination, V);
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreVector4A
(
    VOID*    pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pDestination;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    pElement[0] = V.u[0];
    pElement[1] = V.u[1];
    pElement[2] = V.u[2];
    pElement[3] = V.u[3];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    __stvx(V, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4A
(
    XMFLOAT4A*   pDestination, 
    XMVECTOR     V
)
{
    XMStoreVector4A(pDestination, V);
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreVector4NC
(
    VOID*    pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pDestination;

    XMASSERT(pDestination);

    pElement[0] = V.u[0];
    pElement[1] = V.u[1];
    pElement[2] = V.u[2];
    pElement[3] = V.u[3];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR S;
    XMVECTOR Permute;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Permute = __lvsr(pDestination, 0);                 // Compute the permute control vector for shift right
    S = __vperm(V, V, Permute);                        // Right rotate the vector

    __stvewx(S, pDestination, 0);
    __stvewx(S, pDestination, 4);
    __stvewx(S, pDestination, 8);
    __stvewx(S, pDestination, 12);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4NC
(
    XMFLOAT4* pDestination, 
    XMVECTOR  V
)
{
    XMStoreVector4NC(pDestination, V);
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreHalf4
(
    XMHALF4* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMASSERT(pDestination);

    pDestination->x = XMConvertFloatToHalf(V.v[0]);
    pDestination->y = XMConvertFloatToHalf(V.v[1]);
    pDestination->z = XMConvertFloatToHalf(V.v[2]);
    pDestination->w = XMConvertFloatToHalf(V.v[3]);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR H;
    XMVECTOR S;
    XMVECTOR Permute;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Permute = __lvsr(pDestination, 0);                      // Compute the permute control vector for shift right

    H = XMConvertVectorFloatToHalf(V);
    H = __vpkuwum(H, H);                                    // Pack the 4 half values
    S = __vperm(H, H, Permute);                             // Right rotate the vector

    __stvewx(S, pDestination, 0);
    __stvewx(S, pDestination, 4);

#else // _VMX128_INTRINSICS_

    XMVECTOR H, XY, ZW;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    XMDUMMY_INITIALIZE_VECTOR(H);
    H = __vpkd3d(H, V, VPACK_FLOAT16_4, VPACK_64LO, 2);
    XY = __vspltw(H, 0);
    ZW = __vspltw(H, 1);

    __stvewx(XY, pDestination, 0);
    __stvewx(ZW, pDestination, 4);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreShortN4
(
    XMSHORTN4* pDestination, 
    XMVECTOR   V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {32767.0f, 32767.0f, 32767.0f, 32767.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, g_XMNegativeOne, g_XMOne);
    N = XMVectorMultiply(N, Scale);
    N = XMVectorRound(N);

    pDestination->x = (SHORT)N.v[0];
    pDestination->y = (SHORT)N.v[1];
    pDestination->z = (SHORT)N.v[2];
    pDestination->w = (SHORT)N.v[3];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                N;
    XMVECTOR                S;
    XMVECTOR                Permute;
    static CONST XMVECTOR   PackScale = {7.8122615814208984375e-3f, 7.8122615814208984375e-3f, 7.8122615814208984375e-3f, 7.8122615814208984375e-3f}; // 32767.0f / (FLOAT)(1 << 22)
    static CONST XMVECTOR   Three = {3.0f, 3.0f, 3.0f, 3.0f};
    static CONST XMVECTORI  ShiftAmount = {10, 10, 10, 10};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    N = __vmaddfp(V, PackScale, Three);                     // Scale into the normalized range, adjust the exponent, shift the mantissa into the correct location, and round the result into an integer form
    Permute = __lvsr(pDestination, 0);                      // Compute the permute control vector for shift right
    N = __vslw(N, *(XMVECTOR*)ShiftAmount);
    N = __vsraw(N, *(XMVECTOR*)ShiftAmount);                // Sign extend the lower 22 bits
    N = __vpkswss(N, N);                                    // Saturate into signed short range and pack the short values
    S = __vperm(N, N, Permute);                             // Right rotate the vector

    __stvewx(S, pDestination, 0);
    __stvewx(S, pDestination, 4);

#else // _VMX128_INTRINSICS_

    XMVECTOR N, XY, ZW;
    static CONST XMVECTOR PackScale = XM_PACK_SIGNEDN_SCALE(16, 16, 16, 16);

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    N = XM_PACK_OFFSET;
    N = __vnmsubfp(V, PackScale, N);
    N = __vpkd3d(N, N, VPACK_NORMSHORT4, VPACK_64LO, 2);
    XY = __vspltw(N, 0);
    ZW = __vspltw(N, 1);

    __stvewx(XY, pDestination, 0);
    __stvewx(ZW, pDestination, 4);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreShort4
(
    XMSHORT4* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-32767.0f, -32767.0f, -32767.0f, -32767.0f};
    static CONST XMVECTOR  Max = {32767.0f, 32767.0f, 32767.0f, 32767.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);
    N = XMVectorRound(N);

    pDestination->x = (SHORT)N.v[0];
    pDestination->y = (SHORT)N.v[1];
    pDestination->z = (SHORT)N.v[2];
    pDestination->w = (SHORT)N.v[3];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR N, XY, ZW;

    N = __vctsxs(V, 0);
    N = __vpkswss(N, N);
    XY = __vspltw(N, 0);
    ZW = __vspltw(N, 1);

    __stvewx(XY, pDestination, 0);
    __stvewx(ZW, pDestination, 4);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUShortN4
(
    XMUSHORTN4* pDestination, 
    XMVECTOR    V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {65535.0f, 65535.0f, 65535.0f, 65535.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), g_XMOne);
    N = XMVectorMultiplyAdd(N, Scale, g_XMOneHalf);
    N = XMVectorTruncate(N);

    pDestination->x = (SHORT)N.v[0];
    pDestination->y = (SHORT)N.v[1];
    pDestination->z = (SHORT)N.v[2];
    pDestination->w = (SHORT)N.v[3];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR N, XY, ZW;
    XMVECTOR Scale, Offset;
    static CONST XMVECTOR C0 = {65535.0f, 0.5f, 0.0f, 0.0f};

    Scale = __vspltw(C0, 0);
    Offset = __vspltw(C0, 1);
    N = __vmaddfp(V, Scale, Offset);
    N = __vctuxs(N, 0);
    N = __vpkuwus(N, N);
    XY = __vspltw(N, 0);
    ZW = __vspltw(N, 1);

    __stvewx(XY, pDestination, 0);
    __stvewx(ZW, pDestination, 4);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUShort4
(
    XMUSHORT4* pDestination, 
    XMVECTOR   V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Max = {65535.0f, 65535.0f, 65535.0f, 65535.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), Max);
    N = XMVectorRound(N);

    pDestination->x = (SHORT)N.v[0];
    pDestination->y = (SHORT)N.v[1];
    pDestination->z = (SHORT)N.v[2];
    pDestination->w = (SHORT)N.v[3];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR N, XY, ZW;

    N = __vctuxs(V, 0);
    N = __vpkuwus(N, N);
    XY = __vspltw(N, 0);
    ZW = __vspltw(N, 1);

    __stvewx(XY, pDestination, 0);
    __stvewx(ZW, pDestination, 4);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreXIcoN4
(
    XMXICON4*  pDestination, 
    XMVECTOR   V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-1.0f, -1.0f, -1.0f, 0.0f};
    static CONST XMVECTOR  Scale = {524287.0f, 524287.0f, 524287.0f, 15.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, g_XMOne);
    N = XMVectorMultiply(N, Scale);
    N = XMVectorRound(N);

    pDestination->v = ((UINT64)N.v[3] << 60) |
                       (((INT64)N.v[2] & 0xFFFFF) << 40) |
                       (((INT64)N.v[1] & 0xFFFFF) << 20) |
                       (((INT64)N.v[0] & 0xFFFFF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               N;
    XMVECTORI              I;
    static CONST XMVECTOR  PackScale = {524287.0f / (FLOAT)(1 << 22), 524287.0f / (FLOAT)(1 << 22), 524287.0f / (FLOAT)(1 << 22), 15.0f / (FLOAT)(1 << 22)};
    static CONST XMVECTORI PackMin = {(UINT)-524287, (UINT)-524287, (UINT)-524287, 0};
    static CONST XMVECTORI PackMax = {524287, 524287, 524287, 15};
    static CONST XMVECTOR  Three = {3.0f, 3.0f, 3.0f, 3.0f};
    static CONST XMVECTORI ShiftAmount = {10, 10, 10, 10};

    XMASSERT(pDestination);

    N = __vmaddfp(V, PackScale, Three);      // Scale into the normalized range, adjust the exponent, shift the mantissa into the correct location, and round the result into an integer form

    N = __vslw(N, *(XMVECTOR*)ShiftAmount);

    N = __vsraw(N, *(XMVECTOR*)ShiftAmount); // Sign extend the lower 22 bits of the signed components

    N = __vmaxsw(*(XMVECTOR*)PackMin, N);
    N = __vminsw(*(XMVECTOR*)PackMax, N);    // Saturate into the correct range

    __stvx(N, &I, 0);

    pDestination->v = ((UINT64)I[3] << 60) |
                      ((UINT64)(I[2] & 0xFFFFF) << 40) |
                      ((UINT64)(I[1] & 0xFFFFF) << 20) |
                      ((UINT64)(I[0] & 0xFFFFF));

#else // _VMX128_INTRINSICS_

    XMVECTOR N;
    static CONST XMVECTOR PackScale = {-(FLOAT)((1 << ((20) - 1)) - 1) / XM_PACK_FACTOR,
                                       -(FLOAT)((1 << ((20) - 1)) - 1) / XM_PACK_FACTOR,
                                       -(FLOAT)((1 << ((20) - 1)) - 1) / XM_PACK_FACTOR,
                                       -(FLOAT)((1 << (4)) - 1) / XM_PACK_FACTOR};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    N = XM_PACK_OFFSET;
    N = __vnmsubfp(V, PackScale, N);
    N = __vpkd3d(N, N, VPACK_NORMPACKED64, VPACK_64LO, 2);
    N = __vpermwi(N, 0x11);

    __stvewx(N, pDestination, 0);
    __stvewx(N, pDestination, 4);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreXIco4
(
    XMXICO4*  pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-524287.0f, -524287.0f, -524287.0f, 0.0f};
    static CONST XMVECTOR  Max = {524287.0f, 524287.0f, 524287.0f, 15.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);

    pDestination->v = ((UINT64)N.v[3] << 60) |
                       (((INT64)N.v[2] & 0xFFFFF) << 40) |
                       (((INT64)N.v[1] & 0xFFFFF) << 20) |
                       (((INT64)N.v[0] & 0xFFFFF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               N;
    XMVECTORI              I;
    static CONST XMVECTOR  PackScale = {1.0f / (FLOAT)(1 << 22), 1.0f / (FLOAT)(1 << 22), 1.0f / (FLOAT)(1 << 22), 1.0f / (FLOAT)(1 << 22)};
    static CONST XMVECTORI PackMin = {(UINT)-524287, (UINT)-524287, (UINT)-524287, 0};
    static CONST XMVECTORI PackMax = {524287, 524287, 524287, 15};
    static CONST XMVECTOR  Three = {3.0f, 3.0f, 3.0f, 3.0f};
    static CONST XMVECTORI ShiftAmount = {10, 10, 10, 10};

    XMASSERT(pDestination);

    N = __vmaddfp(V, PackScale, Three);      // Scale into the normalized range, adjust the exponent, shift the mantissa into the correct location, and round the result into an integer form

    N = __vslw(N, *(XMVECTOR*)ShiftAmount);

    N = __vsraw(N, *(XMVECTOR*)ShiftAmount); // Sign extend the lower 22 bits of the signed components

    N = __vmaxsw(*(XMVECTOR*)PackMin, N);
    N = __vminsw(*(XMVECTOR*)PackMax, N);    // Saturate into the correct range

    __stvx(N, &I, 0);

    pDestination->v = ((UINT64)I[3] << 60) |
                      ((UINT64)(I[2] & 0xFFFFF) << 40) |
                      ((UINT64)(I[1] & 0xFFFFF) << 20) |
                      ((UINT64)(I[0] & 0xFFFFF));

#else // _VMX128_INTRINSICS_

    XMVECTOR N;
    static CONST XMVECTOR PackScale = {-1.0f / XM_PACK_FACTOR,
                                       -1.0f / XM_PACK_FACTOR,
                                       -1.0f / XM_PACK_FACTOR,
                                       -1.0f / XM_PACK_FACTOR};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    N = XM_PACK_OFFSET;
    N = __vnmsubfp(V, PackScale, N);
    N = __vpkd3d(N, N, VPACK_NORMPACKED64, VPACK_64LO, 2);
    N = __vpermwi(N, 0x11);

    __stvewx(N, pDestination, 0);
    __stvewx(N, pDestination, 4);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUIcoN4
(
    XMUICON4*  pDestination, 
    XMVECTOR   V
)
{
    #define XM_URange       ((FLOAT)(1 << 20))
    #define XM_URangeDiv2   ((FLOAT)(1 << 19))
    #define XM_UMaxXYZ      ((FLOAT)((1 << 20) - 1))
    #define XM_UMaxW        ((FLOAT)((1 << 4) - 1))
    #define XM_ScaleXYZ     (-(FLOAT)((1 << 20) - 1) / XM_PACK_FACTOR)
    #define XM_ScaleW       (-(FLOAT)((1 << 4) - 1) / XM_PACK_FACTOR)
    #define XM_Scale        (-1.0f / XM_PACK_FACTOR)
    #define XM_Offset       (3.0f)

#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {1048575.0f, 1048575.0f, 1048575.0f, 15.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), g_XMOne);
    N = XMVectorMultiplyAdd(N, Scale, g_XMOneHalf);

    pDestination->v = ((UINT64)N.v[3] << 60) |
                       (((UINT64)N.v[2] & 0xFFFFF) << 40) |
                       (((UINT64)N.v[1] & 0xFFFFF) << 20) |
                       (((UINT64)N.v[0] & 0xFFFFF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR  Scale1, Scale2, Convert, Mod;
    XMVECTOR  Result;
    XMVECTORI I;

    static CONST XMVECTORI PackMax = {1048575, 1048575, 1048575, 15};
    static CONST XMVECTORI ShiftAmount = {10, 10, 10, 10};

    static CONST XMVECTOR C0 = {XM_UMaxXYZ / XM_URange,
                                XM_UMaxW / XM_URange,
                                XM_ScaleXYZ,
                                XM_ScaleW};
    static CONST XMVECTOR C1 = {XM_URangeDiv2 / XM_URange,
                                XM_Offset,
                                XM_URange * XM_Scale,
                                0.0f};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    Scale1 = __vpermwi(C0, 0x1);
    Convert = __vspltw(C1, 0);
    Scale2 = __vpermwi(C0, 0xAB);
    Result = __vspltw(C1, 1);
    Mod = __vpermwi(C1, 0xAB);

    Convert = __vmaddfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Convert = __vrfiz(Convert);
    Result = __vmaddfp(Convert, Mod, Result);
    Result = __vslw(Result, *(XMVECTOR*)ShiftAmount);
    Result = __vsraw(Result, *(XMVECTOR*)ShiftAmount);
    Result = __vmaxsw(XMVectorZero(), Result);
    Result = __vminsw(*(XMVECTOR*)PackMax, Result);

    __stvx(Result, &I, 0);

    pDestination->v = ((UINT64)I[3] << 60) |
                      ((UINT64)(I[2] & 0xFFFFF) << 40) |
                      ((UINT64)(I[1] & 0xFFFFF) << 20) |
                      ((UINT64)(I[0] & 0xFFFFF));

#else // _VMX128_INTRINSICS_

    XMVECTOR Scale1, Scale2, Convert, Mod;
    XMVECTOR Result;

    static CONST XMVECTOR C0 = {XM_UMaxXYZ / XM_URange,
                                XM_UMaxW / XM_URange,
                                XM_ScaleXYZ,
                                XM_ScaleW};
    static CONST XMVECTOR C1 = {XM_URangeDiv2 / XM_URange,
                                XM_Offset,
                                XM_URange * XM_Scale,
                                0.0f};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    Scale1 = __vpermwi(C0, 0x1);
    Convert = __vspltw(C1, 0);
    Scale2 = __vpermwi(C0, 0xAB);
    Result = __vspltw(C1, 1);
    Mod = __vpermwi(C1, 0xAB);

    Convert = __vmaddfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Convert = __vrfiz(Convert);
    Result = __vmaddfp(Convert, Mod, Result);
    Result = __vpkd3d(Result, Result, VPACK_NORMPACKED64, VPACK_64LO, 2);
    Result = __vpermwi(Result, 0x11);

    __stvewx(Result, pDestination, 0);
    __stvewx(Result, pDestination, 4);

#endif // _VMX128_INTRINSICS_

    #undef XM_URange
    #undef XM_URangeDiv2
    #undef XM_UMaxXYZ
    #undef XM_UMaxW
    #undef XM_ScaleXYZ
    #undef XM_ScaleW
    #undef XM_Scale
    #undef XM_Offset
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUIco4
(
    XMUICO4*  pDestination, 
    XMVECTOR  V
)
{
    #define XM_Scale        (-1.0f / XM_PACK_FACTOR)
    #define XM_URange       ((FLOAT)(1 << 20))
    #define XM_URangeDiv2   ((FLOAT)(1 << 19))

#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Max = {1048575.0f, 1048575.0f, 1048575.0f, 15.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), Max);
    N = XMVectorRound(N);

    pDestination->v = ((UINT64)N.v[3] << 60) |
                       (((UINT64)N.v[2] & 0xFFFFF) << 40) |
                       (((UINT64)N.v[1] & 0xFFFFF) << 20) |
                       (((UINT64)N.v[0] & 0xFFFFF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Scale1, Scale2, Convert, Mod, Zero;
    XMVECTORI I;
    XMVECTOR Result;

    static CONST XMVECTORI PackMax = {1048575, 1048575, 1048575, 15};
    static CONST XMVECTORI ShiftAmount = {10, 10, 10, 10};

    static CONST XMVECTOR C0 = {1.0f / XM_URange,
                                XM_URangeDiv2 / XM_URange,
                                XM_Scale,
                                XM_URange * XM_Scale};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    Result = __vspltisw(3);
    Scale1 = __vspltw(C0, 0);
    Convert = __vspltw(C0, 1);
    Scale2 = __vspltw(C0, 2);
    Result = __vcfsx(Result, 0); // Offset
    Mod = __vspltw(C0, 3);
    Zero = __vspltisw(0);

    Convert = __vmaddfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vsldoi(Mod, Zero, 1 << 2);
    Convert = __vrfiz(Convert);
    Result = __vmaddfp(Convert, Mod, Result);
    Result = __vslw(Result, *(XMVECTOR*)ShiftAmount);
    Result = __vsraw(Result, *(XMVECTOR*)ShiftAmount);
    Result = __vmaxsw(XMVectorZero(), Result);
    Result = __vminsw(*(XMVECTOR*)PackMax, Result);

    __stvx(Result, &I, 0);

    pDestination->v = ((UINT64)I[3] << 60) |
                      ((UINT64)(I[2] & 0xFFFFF) << 40) |
                      ((UINT64)(I[1] & 0xFFFFF) << 20) |
                      ((UINT64)(I[0] & 0xFFFFF));

#else // _VMX128_INTRINSICS_

    XMVECTOR Scale1, Scale2, Convert, Mod, Zero;
    XMVECTOR Result;

    static CONST XMVECTOR C0 = {1.0f / XM_URange,
                                XM_URangeDiv2 / XM_URange,
                                XM_Scale,
                                XM_URange * XM_Scale};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    Result = __vspltisw(3);
    Result = __vcfsx(Result, 0); // Offset
    Scale1 = __vspltw(C0, 0);
    Convert = __vspltw(C0, 1);
    Scale2 = __vspltw(C0, 2);
    Mod = __vspltw(C0, 3);
    Zero = __vspltisw(0);

    Convert = __vmaddfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vsldoi(Mod, Zero, 1 << 2);
    Convert = __vrfiz(Convert);
    Result = __vmaddfp(Convert, Mod, Result);
    Result = __vpkd3d(Result, Result, VPACK_NORMPACKED64, VPACK_64LO, 2);
    Result = __vpermwi(Result, 0x11);

    __stvewx(Result, pDestination, 0);
    __stvewx(Result, pDestination, 4);

#endif // _VMX128_INTRINSICS_

    #undef XM_Scale
    #undef XM_URange
    #undef XM_URangeDiv2
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreIcoN4
(
    XMICON4*  pDestination, 
    XMVECTOR  V
)
{
    #define XM_Scale    (-1.0f / XM_PACK_FACTOR)
    #define XM_URange   ((FLOAT)(1 << 4))
    #define XM_Offset   (3.0f)
    #define XM_UMaxXYZ  ((FLOAT)((1 << (20 - 1)) - 1))
    #define XM_UMaxW    ((FLOAT)((1 << (4 - 1)) - 1))

#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {524287.0f, 524287.0f, 524287.0f, 7.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, g_XMNegativeOne, g_XMOne);
    N = XMVectorMultiplyAdd(N, Scale, *(XMVECTOR*)g_XMNegativeZero);
    N = XMVectorRound(N);

    pDestination->v = ((UINT64)N.v[3] << 60) |
                       (((UINT64)N.v[2] & 0xFFFFF) << 40) |
                       (((UINT64)N.v[1] & 0xFFFFF) << 20) |
                       (((UINT64)N.v[0] & 0xFFFFF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Scale1, Scale2, Convert, Mod;
    XMVECTORI I;
    XMVECTOR Result;

    static CONST XMVECTORI PackMin = {(UINT)-524287, (UINT)-524287, (UINT)-524287, (UINT)-7};
    static CONST XMVECTORI PackMax = {524287, 524287, 524287, 7};
    static CONST XMVECTORI ShiftAmount = {10, 10, 10, 10};

    static CONST XMVECTOR C0 = {XM_UMaxXYZ / XM_URange,
                                XM_UMaxW / XM_URange,
                                XM_Scale * XM_UMaxXYZ,
                                XM_Scale * XM_UMaxW};

    static CONST XMVECTOR C1 = {3.0f,
                                -XM_URange * XM_Scale + XM_Offset,
                                XM_URange * XM_Scale,
                                1.0f};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    Scale1 = __vpermwi(C0, 0x1);
    Convert = __vspltw(C1, 3);
    Scale2 = __vpermwi(C0, 0xAB);
    Result = __vpermwi(C1, 0x1);

    Convert = __vmaddfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vspltisw(0);
    Mod = XMVectorInsert(Mod, C1, 3, 0, 0, 0, 1);
    Convert = __vrfiz(Convert);
    Result = __vmaddfp(Convert, Mod, Result);
    Result = __vslw(Result, *(XMVECTOR*)ShiftAmount);
    Result = __vsraw(Result, *(XMVECTOR*)ShiftAmount);
    Result = __vmaxsw(*(XMVECTOR*)PackMin, Result);
    Result = __vminsw(*(XMVECTOR*)PackMax, Result);

    __stvx(Result, &I, 0);

    pDestination->v = ((UINT64)I[3] << 60) |
                      ((UINT64)(I[2] & 0xFFFFF) << 40) |
                      ((UINT64)(I[1] & 0xFFFFF) << 20) |
                      ((UINT64)(I[0] & 0xFFFFF));

#else // _VMX128_INTRINSICS_

    XMVECTOR Scale1, Scale2, Convert, Mod;
    XMVECTOR Result;

    static CONST XMVECTOR C0 = {XM_UMaxXYZ / XM_URange,
                                XM_UMaxW / XM_URange,
                                XM_Scale * XM_UMaxXYZ,
                                XM_Scale * XM_UMaxW};

    static CONST XMVECTOR C1 = {3.0f,
                                -XM_URange * XM_Scale + XM_Offset,
                                XM_URange * XM_Scale,
                                1.0f};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    Scale1 = __vpermwi(C0, 0x1);
    Convert = __vspltw(C1, 3);
    Scale2 = __vpermwi(C0, 0xAB);
    Result = __vpermwi(C1, 0x1);

    Convert = __vmaddfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vspltisw(0);
    Mod = __vrlimi(Mod, C1, 0x1, 3);
    Convert = __vrfiz(Convert);
    Result = __vmaddfp(Convert, Mod, Result);

    Result = __vpkd3d(Result, Result, VPACK_NORMPACKED64, VPACK_64LO, 2);
    Result = __vpermwi(Result, 0x11);

    __stvewx(Result, pDestination, 0);
    __stvewx(Result, pDestination, 4);

#endif // _VMX128_INTRINSICS_

    #undef XM_Scale
    #undef XM_URange
    #undef XM_Offset
    #undef XM_UMaxXYZ
    #undef XM_UMaxW
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreIco4
(
    XMICO4*  pDestination, 
    XMVECTOR V
)
{
    #define XM_Scale    (-1.0f / XM_PACK_FACTOR)
    #define XM_URange   ((FLOAT)(1 << 4))
    #define XM_Offset   (3.0f)

#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-524287.0f, -524287.0f, -524287.0f, -7.0f};
    static CONST XMVECTOR  Max = {524287.0f, 524287.0f, 524287.0f, 7.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);
    N = XMVectorRound(N);

    pDestination->v = ((INT64)N.v[3] << 60) |
                       (((INT64)N.v[2] & 0xFFFFF) << 40) |
                       (((INT64)N.v[1] & 0xFFFFF) << 20) |
                       (((INT64)N.v[0] & 0xFFFFF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR Scale1, Scale2, Convert, Mod;
    XMVECTORI I;
    XMVECTOR Result;

    static CONST XMVECTORI PackMin = {(UINT)-524287, (UINT)-524287, (UINT)-524287, (UINT)-7};
    static CONST XMVECTORI PackMax = {524287, 524287, 524287, 7};
    static CONST XMVECTORI ShiftAmount = {10, 10, 10, 10};

    static CONST XMVECTOR C0 = {1.0f / XM_URange,
                                XM_Scale,
                                -XM_URange * XM_Scale + XM_Offset,
                                XM_URange * XM_Scale};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    Convert = __vspltisw(1);
    Result = __vspltisw(3);
    Convert = __vcfsx(Convert, 0);
    Result = __vcfsx(Result, 0);
    Scale1 = __vspltw(C0, 0);
    Scale2 = __vspltw(C0, 1);
    Result = XMVectorInsert(Result, C0, 3, 0, 0, 0, 1);

    Convert = __vmaddfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vspltisw(0);
    Mod = XMVectorInsert(Mod, C0, 0, 0, 0, 0, 1);
    Convert = __vrfiz(Convert);
    Result = __vmaddfp(Convert, Mod, Result);

    Result = __vslw(Result, *(XMVECTOR*)ShiftAmount);
    Result = __vsraw(Result, *(XMVECTOR*)ShiftAmount);
    Result = __vmaxsw(*(XMVECTOR*)PackMin, Result);
    Result = __vminsw(*(XMVECTOR*)PackMax, Result);

    __stvx(Result, &I, 0);

    pDestination->v = ((UINT64)I[3] << 60) |
                      ((UINT64)(I[2] & 0xFFFFF) << 40) |
                      ((UINT64)(I[1] & 0xFFFFF) << 20) |
                      ((UINT64)(I[0] & 0xFFFFF));

#else // _VMX128_INTRINSICS_

    XMVECTOR Scale1, Scale2, Convert, Mod;
    XMVECTOR Result;

    static CONST XMVECTOR C0 = {1.0f / XM_URange,
                                XM_Scale,
                                -XM_URange * XM_Scale + XM_Offset,
                                XM_URange * XM_Scale};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 7) == 0);

    XMDUMMY_INITIALIZE_VECTOR(Convert);

    Convert = __vupkd3d(Convert, VPACK_NORMSHORT2);
    Result = __vspltisw(3);
    Convert = __vspltw(Convert, 3);
    Result = __vcfsx(Result, 0);
    Scale1 = __vspltw(C0, 0);
    Scale2 = __vspltw(C0, 1);
    Result = __vrlimi(Result, C0, 0x1, 3);

    Convert = __vmaddfp(V, Scale1, Convert);
    Result = __vnmsubfp(V, Scale2, Result);
    Mod = __vspltisw(0);
    Mod = __vrlimi(Mod, C0, 0x1, 0);
    Convert = __vrfiz(Convert);
    Result = __vmaddfp(Convert, Mod, Result);

    Result = __vpkd3d(Result, Result, VPACK_NORMPACKED64, VPACK_64LO, 2);
    Result = __vpermwi(Result, 0x11);

    __stvewx(Result, pDestination, 0);
    __stvewx(Result, pDestination, 4);

#endif // _VMX128_INTRINSICS_

    #undef XM_Scale
    #undef XM_URange
    #undef XM_Offset
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreXDecN4
(
    XMXDECN4* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-1.0f, -1.0f, -1.0f, 0.0f};
    static CONST XMVECTOR  Scale = {511.0f, 511.0f, 511.0f, 3.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, g_XMOne);
    N = XMVectorMultiply(N, Scale);
    N = XMVectorRound(N);

    pDestination->v = ((UINT)N.v[3] << 30) |
                       (((INT)N.v[2] & 0x3FF) << 20) |
                       (((INT)N.v[1] & 0x3FF) << 10) |
                       (((INT)N.v[0] & 0x3FF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               N;
    XMVECTORI              I;
    static CONST XMVECTOR  PackScale = {1.218318939208984375e-4f, 1.218318939208984375e-4f, 1.218318939208984375e-4f, 7.152557373046875e-7f}; // 511.0f / (FLOAT)(1 << 22), 3.0f / (FLOAT)(1 << 22)
    static CONST XMVECTORI PackMin = {(UINT)-511, (UINT)-511, (UINT)-511, 0};
    static CONST XMVECTORI PackMax = {511, 511, 511, 3};
    static CONST XMVECTOR  Three = {3.0f, 3.0f, 3.0f, 3.0f};
    static CONST XMVECTORI ShiftAmount = {10, 10, 10, 10};

    XMASSERT(pDestination);

    N = __vmaddfp(V, PackScale, Three);      // Scale into the normalized range, adjust the exponent, shift the mantissa into the correct location, and round the result into an integer form

    N = __vslw(N, *(XMVECTOR*)ShiftAmount);

    N = __vsraw(N, *(XMVECTOR*)ShiftAmount); // Sign extend the lower 22 bits of the signed components

    N = __vmaxsw(*(XMVECTOR*)PackMin, N);
    N = __vminsw(*(XMVECTOR*)PackMax, N);    // Saturate into the correct range

    __stvx(N, &I, 0);

    pDestination->v = (I[3] << 30) |
                      ((I[2] & 0x3FF) << 20) |
                      ((I[1] & 0x3FF) << 10) |
                      ((I[0] & 0x3FF));

#else // _VMX128_INTRINSICS_

    XMVECTOR N;
    static CONST XMVECTOR PackScale = {-(FLOAT)((1 << ((10) - 1)) - 1) / XM_PACK_FACTOR,
                                       -(FLOAT)((1 << ((10) - 1)) - 1) / XM_PACK_FACTOR,
                                       -(FLOAT)((1 << ((10) - 1)) - 1) / XM_PACK_FACTOR,
                                       -(FLOAT)((1 << (2)) - 1) / XM_PACK_FACTOR};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    N = XM_PACK_OFFSET;
    N = __vnmsubfp(V, PackScale, N);
    N = __vpkd3d(N, N, VPACK_NORMPACKED32, VPACK_32, 3);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreXDec4
(
    XMXDEC4* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-511.0f, -511.0f, -511.0f, 0.0f};
    static CONST XMVECTOR  Max = {511.0f, 511.0f, 511.0f, 3.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);

    pDestination->v = ((UINT)N.v[3] << 30) |
                       (((INT)N.v[2] & 0x3FF) << 20) |
                       (((INT)N.v[1] & 0x3FF) << 10) |
                       (((INT)N.v[0] & 0x3FF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               N;
    XMVECTORI              I;
    static CONST XMVECTOR  PackScale = {1.0f / (FLOAT)(1 << 22), 1.0f / (FLOAT)(1 << 22), 1.0f / (FLOAT)(1 << 22), 1.0f / (FLOAT)(1 << 22)};
    static CONST XMVECTORI PackMin = {(UINT)-511, (UINT)-511, (UINT)-511, 0};
    static CONST XMVECTORI PackMax = {511, 511, 511, 3};
    static CONST XMVECTOR  Three = {3.0f, 3.0f, 3.0f, 3.0f};
    static CONST XMVECTORI ShiftAmount = {10, 10, 10, 10};

    XMASSERT(pDestination);

    N = __vmaddfp(V, PackScale, Three);      // Scale into the normalized range, adjust the exponent, shift the mantissa into the correct location, and round the result into an integer form

    N = __vslw(N, *(XMVECTOR*)ShiftAmount);

    N = __vsraw(N, *(XMVECTOR*)ShiftAmount); // Sign extend the lower 22 bits of the signed components

    N = __vmaxsw(*(XMVECTOR*)PackMin, N);
    N = __vminsw(*(XMVECTOR*)PackMax, N);    // Saturate into the correct range

    __stvx(N, &I, 0);

    pDestination->v = (I[3] << 30) |
                      ((I[2] & 0x3FF) << 20) |
                      ((I[1] & 0x3FF) << 10) |
                      ((I[0] & 0x3FF));

#else // _VMX128_INTRINSICS_

    XMVECTOR N;
    static CONST XMVECTOR PackScale = {-1.0f / XM_PACK_FACTOR,
                                       -1.0f / XM_PACK_FACTOR,
                                       -1.0f / XM_PACK_FACTOR,
                                       -1.0f / XM_PACK_FACTOR};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    N = XM_PACK_OFFSET;
    N = __vnmsubfp(V, PackScale, N);
    N = __vpkd3d(N, N, VPACK_NORMPACKED32, VPACK_32, 3);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUDecN4
(
    XMUDECN4* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {1023.0f, 1023.0f, 1023.0f, 3.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), g_XMOne);
    N = XMVectorMultiply(N, Scale);

    pDestination->v = ((UINT)N.v[3] << 30) |
                       (((UINT)N.v[2] & 0x3FF) << 20) |
                       (((UINT)N.v[1] & 0x3FF) << 10) |
                       (((UINT)N.v[0] & 0x3FF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               Convert;
    XMVECTOR               Scale, ScaleXYZ, OneHalf;
    XMVECTOR               X, Y, Z, W, XY, ZW;
    XMVECTOR               S;

    static CONST XMVECTORI C0 = {0x40400000, 0x447FC000, 0x3FF, 0xFFFFF}; // (FLOAT)((1 << 2) - 1), (FLOAT)((1 << 10) - 1), ...
    static CONST XMVECTORI C1 = {0x3FF00000, 0x0000000A, 0x00000014, 0x0000001E};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    OneHalf = __vspltisw(1);
    OneHalf = __vcfsx(OneHalf, 1);
    ScaleXYZ = __vspltw(*(XMVECTOR*)C0, 1);
    Scale = __vsldoi(ScaleXYZ, *(XMVECTOR*)C0, 1 << 2);

    Convert = __vmaddfp(V, Scale, OneHalf);
    Convert = __vctuxs(Convert, 0);
    XY = __vspltw(*(XMVECTOR*)C0, 2);
    ZW = __vspltw(*(XMVECTOR*)C1, 0);
    S = __vspltw(*(XMVECTOR*)C0, 3);
    Convert = __vslw(Convert, *(XMVECTOR*)C1);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    W = __vspltw(Convert, 3);
    XY = __vsel(Y, X, XY);
    ZW = __vsel(W, Z, ZW);
    S = __vsel(ZW, XY, S);

    __stvewx(S, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               Scale, OneHalf;
    XMVECTOR               X, Y, Z, W, XY, ZW;
    XMVECTOR               S;

    static CONST XMVECTORI C0 = {0x447FC000, 0x40400000, 0x3FF, 0xFFFFF}; // (FLOAT)((1 << 10) - 1), (FLOAT)((1 << 2) - 1), ...
    static CONST XMVECTORI C1 = {0x3FF00000, 0x0000000A, 0x00000014, 0x0000001E};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    OneHalf = __vspltisw(1);
    OneHalf = __vcfsx(OneHalf, 1);
    Scale = __vpermwi(*(XMVECTOR*)C0, 0x1);

    Convert = __vmaddfp(V, Scale, OneHalf);
    Convert = __vctuxs(Convert, 0);
    XY = __vspltw(*(XMVECTOR*)C0, 2);
    ZW = __vspltw(*(XMVECTOR*)C1, 0);
    S = __vspltw(*(XMVECTOR*)C0, 3);
    Convert = __vslw(Convert, *(XMVECTOR*)C1);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    W = __vspltw(Convert, 3);
    XY = __vsel(Y, X, XY);
    ZW = __vsel(W, Z, ZW);
    S = __vsel(ZW, XY, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUDec4
(
    XMUDEC4* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Max = {1023.0f, 1023.0f, 1023.0f, 3.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), Max);

    pDestination->v = ((UINT)N.v[3] << 30) |
                       (((UINT)N.v[2] & 0x3FF) << 20) |
                       (((UINT)N.v[1] & 0x3FF) << 10) |
                       (((UINT)N.v[0] & 0x3FF));

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               X, Y, Z, W, XY, ZW;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x3FF00000, 0x0000000A, 0x003FF014, 0xFFFFF01E};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Select = __vspltisw(12);
    Select = __vsrw(*(XMVECTOR*)C0, Select);

    Convert = __vctuxs(V, 0);
    ZW = __vspltw(*(XMVECTOR*)C0, 0);
    XY = __vspltw(Select, 2);
    S = __vspltw(Select, 3);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    Z = __vspltw(Convert, 2);
    W = __vspltw(Convert, 3);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    ZW = __vsel(W, Z, ZW);
    XY = __vsel(Y, X, XY);
    S = __vsel(ZW, XY, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreDecN4
(
    XMDECN4* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {511.0f, 511.0f, 511.0f, 1.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, g_XMNegativeOne, g_XMOne);
    N = XMVectorMultiply(N, Scale);

    pDestination->v = ((INT)N.v[3] << 30) |
                       (((INT)N.v[2] & 0x3FF) << 20) |
                       (((INT)N.v[1] & 0x3FF) << 10) |
                       (((INT)N.v[0] & 0x3FF));

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR               Convert;
    XMVECTOR               Scale, ScaleXYZ;
    XMVECTOR               X, Y, Z, W, XY, ZW;
    XMVECTOR               S;

    static CONST XMVECTORI C0 = {0x3F800000, 0x43FF8000, 0x3FF, 0xFFFFF}; // (FLOAT)((1 << (2 - 1)) - 1), (FLOAT)((1 << (10 - 1)) - 1), ...
    static CONST XMVECTORI C1 = {0x3FF00000, 0x0000000A, 0x00000014, 0x0000001E};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    ScaleXYZ = __vspltw(*(XMVECTOR*)C0, 1);
    Scale = __vsldoi(ScaleXYZ, *(XMVECTOR*)C0, 1 << 2);

    Convert = __vmulfp(V, Scale);
    Convert = __vrfin(Convert);
    Convert = __vctsxs(Convert, 0);
    XY = __vspltw(*(XMVECTOR*)C0, 2);
    ZW = __vspltw(*(XMVECTOR*)C1, 0);
    S = __vspltw(*(XMVECTOR*)C0, 3);
    Convert = __vslw(Convert, *(XMVECTOR*)C1);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    W = __vspltw(Convert, 3);
    XY = __vsel(Y, X, XY);
    ZW = __vsel(W, Z, ZW);
    S = __vsel(ZW, XY, S);

    __stvewx(S, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               Scale;
    XMVECTOR               X, Y, Z, W, XY, ZW;
    XMVECTOR               S;

    static CONST XMVECTORI C0 = {0x43FF8000, 0x3F800000, 0x3FF, 0xFFFFF}; // (FLOAT)((1 << (10 - 1)) - 1), (FLOAT)((1 << (2 - 1)) - 1), ...
    static CONST XMVECTORI C1 = {0x3FF00000, 0x0000000A, 0x00000014, 0x0000001E};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Scale = __vpermwi(*(XMVECTOR*)C0, 0x1);

    Convert = __vmulfp(V, Scale);
    Convert = __vrfin(Convert);
    Convert = __vctsxs(Convert, 0);
    XY = __vspltw(*(XMVECTOR*)C0, 2);
    ZW = __vspltw(*(XMVECTOR*)C1, 0);
    S = __vspltw(*(XMVECTOR*)C0, 3);
    Convert = __vslw(Convert, *(XMVECTOR*)C1);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    W = __vspltw(Convert, 3);
    XY = __vsel(Y, X, XY);
    ZW = __vsel(W, Z, ZW);
    S = __vsel(ZW, XY, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreDec4
(
    XMDEC4*  pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-511.0f, -511.0f, -511.0f, -1.0f};
    static CONST XMVECTOR  Max = {511.0f, 511.0f, 511.0f, 1.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);

    pDestination->v = ((INT)N.v[3] << 30) |
                       (((INT)N.v[2] & 0x3FF) << 20) |
                       (((INT)N.v[1] & 0x3FF) << 10) |
                       (((INT)N.v[0] & 0x3FF));

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               X, Y, Z, W, XY, ZW;
    XMVECTOR               S;
    static CONST XMVECTORI C0 = {0x3FF00000, 0x0000000A, 0x003FF014, 0xFFFFF01E};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Select = __vspltisw(12);
    Select = __vsrw(*(XMVECTOR*)C0, Select);

    Convert = __vctsxs(V, 0);
    ZW = __vspltw(*(XMVECTOR*)C0, 0);
    XY = __vspltw(Select, 2);
    S = __vspltw(Select, 3);
    Convert = __vslw(Convert, *(XMVECTOR*)C0);
    Z = __vspltw(Convert, 2);
    W = __vspltw(Convert, 3);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    ZW = __vsel(W, Z, ZW);
    XY = __vsel(Y, X, XY);
    S = __vsel(ZW, XY, S);

    __stvewx(S, pDestination, 0);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUByteN4
(
    XMUBYTEN4* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {255.0f, 255.0f, 255.0f, 255.0f};

    XMASSERT(pDestination);

    N = XMVectorSaturate(V);
    N = XMVectorMultiply(N, Scale);
    N = XMVectorRound(N);

    pDestination->x = (BYTE)N.v[0];
    pDestination->y = (BYTE)N.v[1];
    pDestination->z = (BYTE)N.v[2];
    pDestination->w = (BYTE)N.v[3];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                S, N;
    XMVECTOR                Zero;
    XMVECTOR                LessThanZero;
    static CONST XMVECTOR   PackScale = {6.07967376708984375e-5f, 6.07967376708984375e-5f, 6.07967376708984375e-5f, 6.07967376708984375e-5f}; // 255.0f / (FLOAT)(1 << 22)
    static CONST XMVECTOR   Three = {3.0f, 3.0f, 3.0f, 3.0f};
    static CONST XMVECTORI  Permute = {0x07010305, 0x07010305, 0x07010305, 0x07010305};
    static CONST XMVECTORI  SwizzleYZWX = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0X};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Zero = __vspltisw(0);
    S = __vperm(V, V, *(XMVECTOR*)SwizzleYZWX);
    LessThanZero = __vcmpgtfp(Zero, V);
    N = __vmaddfp(S, PackScale, Three);                     // Scale into the normalized range, adjust the exponent, shift the mantissa into the correct location, and round the result into an integer form
    LessThanZero = __vpkuhus(LessThanZero, LessThanZero);   // Pack the comparison results into the most significant 64 bits
    N = __vpkuhus(N, N);                                    // Saturate into unsigned byte range and pack the component color values into the most sigificant 64 bits
    N = __vsel(N, Zero, LessThanZero);                      // Since an unsigned half-word pack had to be used, clamp negative components to zero
    N = __vperm(N, N, *(XMVECTOR*)Permute);                 // Rotate alpha into the most significant component and replicate the results into all vector elements

    __stvewx(N, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR S, N;
    static CONST XMVECTOR PackScale = XM_PACK_UNSIGNEDN_SCALE(8, 8, 8, 8);

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    N = XM_PACK_OFFSET;
    S = __vpermwi(V, 0x6C); // 1, 2, 3, 0
    N = __vnmsubfp(S, PackScale, N);
    N = __vpkd3d(N, N, VPACK_D3DCOLOR, VPACK_32, 3);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreUByte4
(
    XMUBYTE4* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Max = {255.0f, 255.0f, 255.0f, 255.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, XMVectorZero(), Max);
    N = XMVectorRound(N);

    pDestination->x = (BYTE)N.v[0];
    pDestination->y = (BYTE)N.v[1];
    pDestination->z = (BYTE)N.v[2];
    pDestination->w = (BYTE)N.v[3];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR N;
//    static CONST XMVECTORI SwizzleWZYX = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0X};

//    N = __vperm(V, V, *(XMVECTOR*)SwizzleWZYX);
    N = __vctuxs(V, 0);
    N = __vpkuwus(N, N);
    N = __vpkuhus(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR N;

//    N = __vpermwi(V, 0xE4); // 3, 2, 1, 0
    N = __vctuxs(V, 0);
    N = __vpkuwus(N, N);
    N = __vpkuhus(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreByteN4
(
    XMBYTEN4* pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {127.0f, 127.0f, 127.0f, 127.0f};

    XMASSERT(pDestination);

    N = XMVectorMultiply(V, Scale);
    N = XMVectorRound(N);

    pDestination->x = (CHAR)N.v[0];
    pDestination->y = (CHAR)N.v[1];
    pDestination->z = (CHAR)N.v[2];
    pDestination->w = (CHAR)N.v[3];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR N;
    static CONST XMVECTOR Scale = {127.0f, 127.0f, 127.0f, 127.0f};
//    static CONST XMVECTORI SwizzleWZYX = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0X};

//    N = __vperm(V, V, *(XMVECTOR*)SwizzleWZYX);
    N = __vmulfp(V, Scale);
    N = __vrfin(N);
    N = __vctsxs(N, 0);
    N = __vpkswss(N, N);
    N = __vpkshss(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR N;
    static CONST XMVECTOR Scale = {127.0f, 127.0f, 127.0f, 127.0f};

//    N = __vpermwi(V, 0xE4); // 3, 2, 1, 0
    N = __vmulfp(V, Scale);
    N = __vrfin(N);
    N = __vctsxs(N, 0);
    N = __vpkswss(N, N);
    N = __vpkshss(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreByte4
(
    XMBYTE4*  pDestination, 
    XMVECTOR  V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Min = {-127.0f, -127.0f, -127.0f, -127.0f};
    static CONST XMVECTOR  Max = {127.0f, 127.0f, 127.0f, 127.0f};

    XMASSERT(pDestination);

    N = XMVectorClamp(V, Min, Max);
    N = XMVectorRound(N);

    pDestination->x = (CHAR)N.v[0];
    pDestination->y = (CHAR)N.v[1];
    pDestination->z = (CHAR)N.v[2];
    pDestination->w = (CHAR)N.v[3];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR N;
//    static CONST XMVECTORI SwizzleWZYX = {XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0X};

//    N = __vperm(V, V, *(XMVECTOR*)SwizzleWZYX);
    N = __vctsxs(V, 0);
    N = __vpkswss(N, N);
    N = __vpkshss(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR N;

//    N = __vpermwi(V, 0xE4); // 3, 2, 1, 0
    N = __vctsxs(V, 0);
    N = __vpkswss(N, N);
    N = __vpkshss(N, N);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreColor
(
    XMCOLOR* pDestination, 
    XMVECTOR V
)
{
#if defined(_NO_INTRINSICS_)

    XMVECTOR               N;
    static CONST XMVECTOR  Scale = {255.0f, 255.0f, 255.0f, 255.0f};

    XMASSERT(pDestination);

    N = XMVectorSaturate(V);
    N = XMVectorMultiply(N, Scale);
    N = XMVectorRound(N);

    pDestination->c = ((UINT)N.v[3] << 24) |
                      ((UINT)N.v[0] << 16) |
                      ((UINT)N.v[1] <<  8) |
                      ((UINT)N.v[2]);

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR                N;
    XMVECTOR                Zero;
    XMVECTOR                LessThanZero;
    static CONST XMVECTOR   PackScale = {6.07967376708984375e-5f, 6.07967376708984375e-5f, 6.07967376708984375e-5f, 6.07967376708984375e-5f}; // 255.0f / (FLOAT)(1 << 22)
    static CONST XMVECTOR   Three = {3.0f, 3.0f, 3.0f, 3.0f};
    static CONST XMVECTORI  Permute = {0x07010305, 0x07010305, 0x07010305, 0x07010305};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Zero = __vspltisw(0);
    LessThanZero = __vcmpgtfp(Zero, V);
    N = __vmaddfp(V, PackScale, Three);                     // Scale into the normalized range, adjust the exponent, shift the mantissa into the correct location, and round the result into an integer form
    LessThanZero = __vpkuhus(LessThanZero, LessThanZero);   // Pack the comparison results into the most significant 64 bits
    N = __vpkuhus(N, N);                                    // Saturate into unsigned byte range and pack the component color values into the most sigificant 64 bits
    N = __vsel(N, Zero, LessThanZero);                      // Since an unsigned half-word pack had to be used, clamp negative components to zero
    N = __vperm(N, N, *(XMVECTOR*)Permute);                 // Rotate alpha into the most significant component and replicate the results into all vector elements

    __stvewx(N, pDestination, 0);

#else // _VMX128_INTRINSICS_

    XMVECTOR N;
    static CONST XMVECTOR PackScale = XM_PACK_UNSIGNEDN_SCALE(8, 8, 8, 8);

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    N = XM_PACK_OFFSET;
    N = __vnmsubfp(V, PackScale, N);
    N = __vpkd3d(N, N, VPACK_D3DCOLOR, VPACK_32, 3);
    N = __vspltw(N, 0);

    __stvewx(N, pDestination, 0);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat3x3
(
    XMFLOAT3X3* pDestination, 
    XMMATRIX    M
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMStoreFloat3x3NC(pDestination, M);

#else // _VMX128_INTRINSICS_

    XMVECTOR S0, S1, S2;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    S0 = M.r[0];
    S1 = __vsldoi(M.r[1], M.r[1], 1 << 2);
    S2 = __vspltw(M.r[2], 2);
    S0 = __vrlimi(S0, M.r[1], 0x1, 1);
    S1 = __vrlimi(S1, M.r[2], 0x3, 2);

    __stvlx(S0, pDestination, 0);
    __stvrx(S0, pDestination, 16);
    __stvlx(S1, pDestination, 16);
    __stvrx(S1, pDestination, 32);
    __stvewx(S2, pDestination, 32);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat3x3NC
(
    XMFLOAT3X3* pDestination, 
    XMMATRIX    M
)
{
#if defined(_NO_INTRINSICS_)

    XMASSERT(pDestination);

    pDestination->m[0][0] = M.r[0].v[0];
    pDestination->m[0][1] = M.r[0].v[1];
    pDestination->m[0][2] = M.r[0].v[2];

    pDestination->m[1][0] = M.r[1].v[0];
    pDestination->m[1][1] = M.r[1].v[1];
    pDestination->m[1][2] = M.r[1].v[2];

    pDestination->m[2][0] = M.r[2].v[0];
    pDestination->m[2][1] = M.r[2].v[1];
    pDestination->m[2][2] = M.r[2].v[2];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Permute;
    XMVECTOR S0, S1, S2;
    static CONST XMVECTORI Permute0X0Y0Z1X = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute0Y0Z1X1Y = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Y};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Permute = __lvsr(pDestination, 0);

    S0 = __vperm(M.r[0], M.r[1], *(XMVECTOR*)Permute0X0Y0Z1X);
    S1 = __vperm(M.r[1], M.r[2], *(XMVECTOR*)Permute0Y0Z1X1Y);
    S2 = __vspltw(M.r[2], 2);

    S0 = __vperm(S0, S0, Permute);
    S1 = __vperm(S1, S1, Permute);

    __stvewx(S0, pDestination, 0);
    __stvewx(S0, pDestination, 4);
    __stvewx(S0, pDestination, 8);
    __stvewx(S0, pDestination, 12);

    __stvewx(S1, pDestination, 16);
    __stvewx(S1, pDestination, 20);
    __stvewx(S1, pDestination, 24);
    __stvewx(S1, pDestination, 28);

    __stvewx(S2, pDestination, 32);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4x3
(
    XMFLOAT4X3* pDestination, 
    XMMATRIX    M
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMStoreFloat4x3NC(pDestination, M);

#else // _VMX128_INTRINSICS_

    XMVECTOR S0, S1, S2;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    S0 = M.r[0];
    S1 = __vsldoi(M.r[1], M.r[1], 1 << 2);
    S2 = __vsldoi(M.r[2], M.r[2], 2 << 2);
    S0 = __vrlimi(S0, M.r[1], 0x1, 1);
    S1 = __vrlimi(S1, M.r[2], 0x3, 2);
    S2 = __vrlimi(S2, M.r[3], 0x7, 3);

    __stvlx(S0, pDestination, 0);
    __stvrx(S0, pDestination, 16);
    __stvlx(S1, pDestination, 16);
    __stvrx(S1, pDestination, 32);
    __stvlx(S2, pDestination, 32);
    __stvrx(S2, pDestination, 48);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4x3A
(
    XMFLOAT4X3A*   pDestination, 
    XMMATRIX       M
)
{
#if defined(_NO_INTRINSICS_)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    pDestination->m[0][0] = M.r[0].v[0];
    pDestination->m[0][1] = M.r[0].v[1];
    pDestination->m[0][2] = M.r[0].v[2];

    pDestination->m[1][0] = M.r[1].v[0];
    pDestination->m[1][1] = M.r[1].v[1];
    pDestination->m[1][2] = M.r[1].v[2];

    pDestination->m[2][0] = M.r[2].v[0];
    pDestination->m[2][1] = M.r[2].v[1];
    pDestination->m[2][2] = M.r[2].v[2];

    pDestination->m[3][0] = M.r[3].v[0];
    pDestination->m[3][1] = M.r[3].v[1];
    pDestination->m[3][2] = M.r[3].v[2];

#elif defined(_VMX32_INTRINSICS_)

    XMVECTOR S0, S1, S2;
    static CONST XMVECTORI Permute0X0Y0Z1X = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute0Y0Z1X1Y = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Y};
    static CONST XMVECTORI Permute0Z1X1Y1Z = {XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_1Z};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    S0 = __vperm(M.r[0], M.r[1], *(XMVECTOR*)Permute0X0Y0Z1X);
    S1 = __vperm(M.r[1], M.r[2], *(XMVECTOR*)Permute0Y0Z1X1Y);
    S2 = __vperm(M.r[2], M.r[3], *(XMVECTOR*)Permute0Z1X1Y1Z);

    __stvx(S0, pDestination, 0);
    __stvx(S1, pDestination, 16);
    __stvx(S2, pDestination, 32);

#else // _VMX128_INTRINSICS_

    XMVECTOR S0, S1, S2;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    S0 = M.r[0];
    S1 = __vsldoi(M.r[1], M.r[1], 1 << 2);
    S2 = __vsldoi(M.r[2], M.r[2], 2 << 2);
    S0 = __vrlimi(S0, M.r[1], 0x1, 1);
    S1 = __vrlimi(S1, M.r[2], 0x3, 2);
    S2 = __vrlimi(S2, M.r[3], 0x7, 3);

    __stvx(S0, pDestination, 0);
    __stvx(S1, pDestination, 16);
    __stvx(S2, pDestination, 32);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4x3NC
(
    XMFLOAT4X3* pDestination, 
    XMMATRIX    M
)
{
#if defined(_NO_INTRINSICS_)

    XMASSERT(pDestination);

    pDestination->m[0][0] = M.r[0].v[0];
    pDestination->m[0][1] = M.r[0].v[1];
    pDestination->m[0][2] = M.r[0].v[2];

    pDestination->m[1][0] = M.r[1].v[0];
    pDestination->m[1][1] = M.r[1].v[1];
    pDestination->m[1][2] = M.r[1].v[2];

    pDestination->m[2][0] = M.r[2].v[0];
    pDestination->m[2][1] = M.r[2].v[1];
    pDestination->m[2][2] = M.r[2].v[2];

    pDestination->m[3][0] = M.r[3].v[0];
    pDestination->m[3][1] = M.r[3].v[1];
    pDestination->m[3][2] = M.r[3].v[2];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR Permute;
    XMVECTOR S0, S1, S2;
    static CONST XMVECTORI Permute0X0Y0Z1X = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_1X};
    static CONST XMVECTORI Permute0Y0Z1X1Y = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Y};
    static CONST XMVECTORI Permute0Z1X1Y1Z = {XM_PERMUTE_0Z, XM_PERMUTE_1X, XM_PERMUTE_1Y, XM_PERMUTE_1Z};

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Permute = __lvsr(pDestination, 0);

    S0 = __vperm(M.r[0], M.r[1], *(XMVECTOR*)Permute0X0Y0Z1X);
    S1 = __vperm(M.r[1], M.r[2], *(XMVECTOR*)Permute0Y0Z1X1Y);
    S2 = __vperm(M.r[2], M.r[3], *(XMVECTOR*)Permute0Z1X1Y1Z);

    S0 = __vperm(S0, S0, Permute);
    S1 = __vperm(S1, S1, Permute);
    S2 = __vperm(S2, S2, Permute);

    __stvewx(S0, pDestination, 0);
    __stvewx(S0, pDestination, 4);
    __stvewx(S0, pDestination, 8);
    __stvewx(S0, pDestination, 12);

    __stvewx(S1, pDestination, 16);
    __stvewx(S1, pDestination, 20);
    __stvewx(S1, pDestination, 24);
    __stvewx(S1, pDestination, 28);

    __stvewx(S2, pDestination, 32);
    __stvewx(S2, pDestination, 36);
    __stvewx(S2, pDestination, 40);
    __stvewx(S2, pDestination, 44);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4x4
(
    XMFLOAT4X4* pDestination, 
    XMMATRIX    M
)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_) || defined(XM_NO_MISALIGNED_VECTOR_ACCESS)

    XMStoreFloat4x4NC(pDestination, M);

#else // _VMX128_INTRINSICS_

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    __stvlx(M.r[0], pDestination, 0);
    __stvrx(M.r[0], pDestination, 16);
    __stvlx(M.r[1], pDestination, 16);
    __stvrx(M.r[1], pDestination, 32);
    __stvlx(M.r[2], pDestination, 32);
    __stvrx(M.r[2], pDestination, 48);
    __stvlx(M.r[3], pDestination, 48);
    __stvrx(M.r[3], pDestination, 64);

#endif // _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4x4A
(
    XMFLOAT4X4A*   pDestination, 
    XMMATRIX       M
)
{
#if defined(_NO_INTRINSICS_)

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    pDestination->m[0][0] = M.r[0].v[0];
    pDestination->m[0][1] = M.r[0].v[1];
    pDestination->m[0][2] = M.r[0].v[2];
    pDestination->m[0][3] = M.r[0].v[3];

    pDestination->m[1][0] = M.r[1].v[0];
    pDestination->m[1][1] = M.r[1].v[1];
    pDestination->m[1][2] = M.r[1].v[2];
    pDestination->m[1][3] = M.r[1].v[3];

    pDestination->m[2][0] = M.r[2].v[0];
    pDestination->m[2][1] = M.r[2].v[1];
    pDestination->m[2][2] = M.r[2].v[2];
    pDestination->m[2][3] = M.r[2].v[3];

    pDestination->m[3][0] = M.r[3].v[0];
    pDestination->m[3][1] = M.r[3].v[1];
    pDestination->m[3][2] = M.r[3].v[2];
    pDestination->m[3][3] = M.r[3].v[3];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 0xF) == 0);

    __stvx(M.r[0], pDestination, 0);
    __stvx(M.r[1], pDestination, 16);
    __stvx(M.r[2], pDestination, 32);
    __stvx(M.r[3], pDestination, 48);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

//------------------------------------------------------------------------------

XMFINLINE VOID XMStoreFloat4x4NC
(
    XMFLOAT4X4* pDestination, 
    XMMATRIX    M
)
{
#if defined(_NO_INTRINSICS_)

    XMASSERT(pDestination);

    pDestination->m[0][0] = M.r[0].v[0];
    pDestination->m[0][1] = M.r[0].v[1];
    pDestination->m[0][2] = M.r[0].v[2];
    pDestination->m[0][3] = M.r[0].v[3];

    pDestination->m[1][0] = M.r[1].v[0];
    pDestination->m[1][1] = M.r[1].v[1];
    pDestination->m[1][2] = M.r[1].v[2];
    pDestination->m[1][3] = M.r[1].v[3];

    pDestination->m[2][0] = M.r[2].v[0];
    pDestination->m[2][1] = M.r[2].v[1];
    pDestination->m[2][2] = M.r[2].v[2];
    pDestination->m[2][3] = M.r[2].v[3];

    pDestination->m[3][0] = M.r[3].v[0];
    pDestination->m[3][1] = M.r[3].v[1];
    pDestination->m[3][2] = M.r[3].v[2];
    pDestination->m[3][3] = M.r[3].v[3];

#else // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_

    XMVECTOR S0, S1, S2, S3;
    XMVECTOR Permute;

    XMASSERT(pDestination);
    XMASSERT(((UINT_PTR)pDestination & 3) == 0);

    Permute = __lvsr(pDestination, 0);                  // Compute the permute control vector for shift right

    S0 = __vperm(M.r[0], M.r[0], Permute);
    S1 = __vperm(M.r[1], M.r[1], Permute);
    S2 = __vperm(M.r[2], M.r[2], Permute);
    S3 = __vperm(M.r[3], M.r[3], Permute);

    __stvewx(S0, pDestination, 0);
    __stvewx(S0, pDestination, 4);
    __stvewx(S0, pDestination, 8);
    __stvewx(S0, pDestination, 12);

    __stvewx(S1, pDestination, 16);
    __stvewx(S1, pDestination, 20);
    __stvewx(S1, pDestination, 24);
    __stvewx(S1, pDestination, 28);

    __stvewx(S2, pDestination, 32);
    __stvewx(S2, pDestination, 36);
    __stvewx(S2, pDestination, 40);
    __stvewx(S2, pDestination, 44);

    __stvewx(S3, pDestination, 48);
    __stvewx(S3, pDestination, 52);
    __stvewx(S3, pDestination, 56);
    __stvewx(S3, pDestination, 60);

#endif // _VMX32_INTRINSICS_ || _VMX128_INTRINSICS_
}

/****************************************************************************
 *
 * Compatibility defines
 *
 ****************************************************************************/

XMFINLINE XMVECTOR        XMLoadVector2A16(CONST VOID* pSource) { return XMLoadVector2A(pSource); }
XMFINLINE XMVECTOR        XMLoadVector3A16(CONST VOID* pSource) { return XMLoadVector3A(pSource); }
XMFINLINE XMVECTOR        XMLoadVector4A16(CONST VOID* pSource) { return XMLoadVector4A(pSource); }
XMFINLINE XMVECTOR        XMLoadFloat2A16(CONST XMFLOAT2A16* pSource) { return XMLoadFloat2A(pSource); }
XMFINLINE XMVECTOR        XMLoadFloat3A16(CONST XMFLOAT3A16* pSource) { return XMLoadFloat3A(pSource); }
XMFINLINE XMVECTOR        XMLoadFloat4A16(CONST XMFLOAT4A16* pSource) { return XMLoadFloat4A(pSource); }
XMFINLINE XMMATRIX        XMLoadFloat4x3A16(CONST XMFLOAT4X3A16* pSource) { return XMLoadFloat4x3A(pSource); }
XMFINLINE XMMATRIX        XMLoadFloat4x4A16(CONST XMFLOAT4X4A16* pSource) { return XMLoadFloat4x4A(pSource); }

XMFINLINE VOID            XMStoreVector2A16(VOID* pDestination, XMVECTOR V) { XMStoreVector2A(pDestination, V); }
XMFINLINE VOID            XMStoreVector3A16(VOID* pDestination, XMVECTOR V) { XMStoreVector3A(pDestination, V); }
XMFINLINE VOID            XMStoreVector4A16(VOID* pDestination, XMVECTOR V) { XMStoreVector4A(pDestination, V); }
XMFINLINE VOID            XMStoreFloat2A16(XMFLOAT2A16* pDestination, XMVECTOR V) { XMStoreFloat2A(pDestination, V); }
XMFINLINE VOID            XMStoreFloat3A16(XMFLOAT3A16* pDestination, XMVECTOR V) { XMStoreFloat3A(pDestination, V); }
XMFINLINE VOID            XMStoreFloat4A16(XMFLOAT4A16* pDestination, XMVECTOR V) { XMStoreFloat4A(pDestination, V); }
XMFINLINE VOID            XMStoreFloat4x3A16(XMFLOAT4X3A16* pDestination, XMMATRIX M) { XMStoreFloat4x3A(pDestination, M); }
XMFINLINE VOID            XMStoreFloat4x4A16(XMFLOAT4X4A16* pDestination, XMMATRIX M) { XMStoreFloat4x4A(pDestination, M); }

#if defined(_VMX32_INTRINSICS_)

/****************************************************************************
 *
 * VMX32 float to half vector conversions
 *
 ****************************************************************************/

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMConvertVectorHalfToFloat
(
    XMVECTOR V
)
{
    XMVECTOR                Sign;
    XMVECTOR                Exponent, ExponentN;
    XMVECTOR                Mantissa, MantissaD, MantissaShift, MantissaBit;
    XMVECTOR                ShiftAmount;
    XMVECTOR                SelectControl;
    XMVECTOR                Zero;
    XMVECTOR                Result;
    static CONST XMVECTORI  SignMask = {0x00008000, 0x00008000, 0x00008000, 0x00008000};
    static CONST XMVECTORI  ExponentMask = {0x00007C00, 0x00007C00, 0x00007C00, 0x00007C00};
    static CONST XMVECTORI  MantissaMask = {0x000003FF, 0x000003FF, 0x000003FF, 0x000003FF};
    static CONST XMVECTORI  MantissaMSBMask = {0x00000200, 0x00000200, 0x00000200, 0x00000200};
    static CONST XMVECTORI  One = {1, 1, 1, 1};
    static CONST XMVECTORI  Nine = {9, 9, 9, 9};
    static CONST XMVECTORI  Ten = {10, 10, 10, 10};
    static CONST XMVECTORI  Thirteen = {13, 13, 13, 13};
    static CONST XMVECTORI  Sixteen = {16, 16, 16, 16};
    static CONST XMVECTORI  TwentyThree = {23, 23, 23, 23};
    static CONST XMVECTORI  OneHundredTwelve = {112, 112, 112, 112};

    Zero = __vspltisw(0);

    Mantissa = __vand(V, *(XMVECTOR*)MantissaMask);

    // Convert denormalized half values
    Exponent = *(XMVECTOR*)OneHundredTwelve;

    // Mantissa bit: 0000000001
    MantissaShift = __vslw(Mantissa, *(XMVECTOR*)Nine);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(*(XMVECTOR*)Nine, Exponent, SelectControl);

    ShiftAmount = __vsubuwm(*(XMVECTOR*)Nine, *(XMVECTOR*)One);

    // Mantissa bit: 0000000010
    MantissaShift = __vslw(Mantissa, ShiftAmount);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(ShiftAmount, Exponent, SelectControl);

    ShiftAmount = __vsubuwm(ShiftAmount, *(XMVECTOR*)One);

    // Mantissa bit: 0000000100
    MantissaShift = __vslw(Mantissa, ShiftAmount);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(ShiftAmount, Exponent, SelectControl);

    ShiftAmount = __vsubuwm(ShiftAmount, *(XMVECTOR*)One);

    // Mantissa bit: 0000001000
    MantissaShift = __vslw(Mantissa, ShiftAmount);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(ShiftAmount, Exponent, SelectControl);

    ShiftAmount = __vsubuwm(ShiftAmount, *(XMVECTOR*)One);

    // Mantissa bit: 0000010000
    MantissaShift = __vslw(Mantissa, ShiftAmount);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(ShiftAmount, Exponent, SelectControl);

    ShiftAmount = __vsubuwm(ShiftAmount, *(XMVECTOR*)One);

    // Mantissa bit: 0000100000
    MantissaShift = __vslw(Mantissa, ShiftAmount);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(ShiftAmount, Exponent, SelectControl);

    ShiftAmount = __vsubuwm(ShiftAmount, *(XMVECTOR*)One);

    // Mantissa bit: 0001000000
    MantissaShift = __vslw(Mantissa, ShiftAmount);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(ShiftAmount, Exponent, SelectControl);

    ShiftAmount = __vsubuwm(ShiftAmount, *(XMVECTOR*)One);

    // Mantissa bit: 0010000000
    MantissaShift = __vslw(Mantissa, ShiftAmount);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(ShiftAmount, Exponent, SelectControl);

    ShiftAmount = __vsubuwm(ShiftAmount, *(XMVECTOR*)One);

    // Mantissa bit: 0100000000
    MantissaShift = __vslw(Mantissa, ShiftAmount);
    MantissaBit = __vand(MantissaShift, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(ShiftAmount, Exponent, SelectControl);

    // Mantissa bit: 1000000000
    MantissaBit = __vand(Mantissa, *(XMVECTOR*)MantissaMSBMask);
    SelectControl = __vcmpequw(MantissaBit, Zero);
    Exponent = __vsel(Zero, Exponent, SelectControl);

    ShiftAmount = __vadduwm(Exponent, *(XMVECTOR*)One);
    MantissaD = __vslw(Mantissa, ShiftAmount);
    MantissaD = __vand(MantissaD, *(XMVECTOR*)MantissaMask);
    Exponent = __vsubuwm(Zero, Exponent);

    // Convert normalized half values
    ExponentN = __vand(V, *(XMVECTOR*)ExponentMask);
    ExponentN = __vsrw(ExponentN, *(XMVECTOR*)Ten);

    // Select the correct exponent and mantissa
    SelectControl = __vcmpequw(ExponentN, Zero);
    Exponent = __vsel(ExponentN, Exponent, SelectControl);
    Mantissa = __vsel(Mantissa, MantissaD, SelectControl);

    // Construct the single precision values
    Sign = __vand(V, *(XMVECTOR*)SignMask);
    Sign = __vslw(Sign, *(XMVECTOR*)Sixteen);
    Exponent = __vadduwm(Exponent, *(XMVECTOR*)OneHundredTwelve);
    Exponent = __vslw(Exponent, *(XMVECTOR*)TwentyThree);
    Mantissa = __vslw(Mantissa, *(XMVECTOR*)Thirteen);

    Result = __vor(Sign, Exponent);
    Result = __vor(Result, Mantissa);

    return Result;
}

//------------------------------------------------------------------------------

XMINLINE XMVECTOR XMConvertVectorFloatToHalf
(
    XMVECTOR V
)
{
    XMVECTOR                I, D, N, VF;
    XMVECTOR                Sign;
    XMVECTOR                Finite;
    XMVECTOR                Shift;
    XMVECTOR                SelectControl;
    XMVECTOR                Result;
    static CONST XMVECTORI  SignMask = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
    static CONST XMVECTORI  MantissaMask = {0x007FFFFF, 0x007FFFFF, 0x007FFFFF, 0x007FFFFF};
    static CONST XMVECTORI  Leading1 = {0x00800000, 0x00800000, 0x00800000, 0x00800000};
    static CONST XMVECTORI  ExponentBias = {0xC8000000, 0xC8000000, 0xC8000000, 0xC8000000};
    static CONST XMVECTORI  NormalizedMin = {0x38800000, 0x38800000, 0x38800000, 0x38800000};
    static CONST XMVECTORI  NormalizedMax = {0x47FFEFFF, 0x47FFEFFF, 0x47FFEFFF, 0x47FFEFFF};
    static CONST XMVECTORI  FiniteOffset = {0x00000FFF, 0x00000FFF, 0x00000FFF, 0x00000FFF};
    static CONST XMVECTORI  InfiniteHalf = {0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF};
    static CONST XMVECTORI  One = {1, 1, 1, 1};
    static CONST XMVECTORI  Thirteen = {13, 13, 13, 13};
    static CONST XMVECTORI  Sixteen = {16, 16, 16, 16};
    static CONST XMVECTORI  TwentyThree = {23, 23, 23, 23};
    static CONST XMVECTORI  OneHundredThirteen = {113, 113, 113, 113};

    I = __vand(V, *(XMVECTOR*)g_XMAbsMask);
    Sign = __vand(V, *(XMVECTOR*)SignMask);
    Sign = __vsrw(Sign, *(XMVECTOR*)Sixteen);

    Shift = __vsrw(I, *(XMVECTOR*)TwentyThree);
    Shift = __vsubuwm(*(XMVECTOR*)OneHundredThirteen, Shift);

    D = __vand(I, *(XMVECTOR*)MantissaMask);
    D = __vor(*(XMVECTOR*)Leading1, D);
    D = __vsrw(D, Shift);

    N = __vadduwm(I, *(XMVECTOR*)ExponentBias);

    SelectControl = __vcmpgtuw(*(XMVECTOR*)NormalizedMin, I);
    VF = __vsel(N, D, SelectControl);
    Finite = __vsrw(VF, *(XMVECTOR*)Thirteen);
    VF = __vadduwm(VF, *(XMVECTOR*)FiniteOffset);
    Finite = __vand(Finite, *(XMVECTOR*)One);
    Finite = __vadduwm(VF, Finite);
    Finite = __vsrw(Finite, *(XMVECTOR*)Thirteen);

    SelectControl = __vcmpgtuw(I, *(XMVECTOR*)NormalizedMax);
    I = __vsel(Finite, *(XMVECTOR*)InfiniteHalf, SelectControl);

    Result = __vor(Sign, I);

    return Result;
}

#endif // _VMX32_INTRINSICS_

#endif // __XMCONVERT_INL__
