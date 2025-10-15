//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx9math.inl
//  Content:    D3DX math inline functions
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __D3DX9MATH_INL__
#define __D3DX9MATH_INL__

//===========================================================================
//
// XM_NO_MISALIGNED_VECTOR_ACCESS versions of functions copied from the Xbox
// math library.
//
//===========================================================================

XMFINLINE XMVECTOR D3DX_LoadFloat2(CONST XMFLOAT2* pSource)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);

    V.u[0] = pElement[0];
    V.u[1] = pElement[1];

    return V;

#else

    XMVECTOR V;
    XMVECTOR V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);      
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 7);      
    V = __vperm(V0, V1, Permute);

    return V;

#endif
}

XMFINLINE XMVECTOR D3DX_LoadFloat3(CONST XMFLOAT3* pSource)
{
#if defined(_NO_INTRINSICS_)

    UINT* pElement = (UINT*)pSource;
    XMVECTOR V;

    XMASSERT(pSource);

    V.u[0] = pElement[0];
    V.u[1] = pElement[1];
    V.u[2] = pElement[2];

    return V;

#else

    XMVECTOR V;
    XMVECTOR V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);      
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 11);     
    V = __vperm(V0, V1, Permute);

    return V;

#endif
}

XMFINLINE XMVECTOR D3DX_LoadFloat4(CONST XMFLOAT4* pSource)
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

#else

    XMVECTOR V;
    XMVECTOR V0, V1;
    XMVECTOR Permute;

    XMASSERT(pSource);

    V0 = __lvx(pSource, 0);      
    Permute = __lvsl(pSource, 0);
    V1 = __lvx(pSource, 15);     
    V = __vperm(V0, V1, Permute);

    return V;

#endif
}

XMFINLINE XMMATRIX D3DX_LoadFloat4x4(CONST XMFLOAT4X4* pSource)
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

#else

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

#endif
}

XMFINLINE FLOAT D3DX_ConvertHalfToFloat(HALF Value)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    UINT Mantissa;
    UINT Exponent;
    UINT Result;

    Mantissa = (UINT)(Value & 0x03FF);

    if ((Value & 0x7C00) != 0)
    {
        Exponent = (UINT)((Value >> 10) & 0x1F);
    }
    else if (Mantissa != 0)
    {
        Exponent = 1;

        do
        {
            Exponent--;
            Mantissa <<= 1;
        } while ((Mantissa & 0x0400) == 0);

        Mantissa &= 0x03FF;
    }
    else
    {
        Exponent = (UINT)-112;
    }

    Result = ((Value & 0x8000) << 16) |
             ((Exponent + 112) << 23) |
             (Mantissa << 13);

    return *(FLOAT*)&Result;

#else

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

#endif
}

XMFINLINE FLOAT* D3DX_ConvertHalfToFloatStream(FLOAT* pOutputStream, UINT OutputStride, CONST HALF* pInputStream, UINT InputStride, UINT HalfCount)
{
    UINT  i;
    BYTE* pHalf = (BYTE*)pInputStream;
    BYTE* pFloat = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < HalfCount; i++)
    {
        *(FLOAT*)pFloat = D3DX_ConvertHalfToFloat(*(HALF*)pHalf);
        pHalf += InputStride;
        pFloat += OutputStride; 
    }

    return pOutputStream;
}

XMFINLINE HALF D3DX_ConvertFloatToHalf(FLOAT Value)
{
#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)

    UINT IValue;
    UINT Sign;
    HALF Result;

    IValue = *(UINT*)&Value & 0x7FFFFFFF;
    Sign = (*(UINT*)&Value & 0x80000000) >> 16;

    if (IValue > 0x47FFEFFF)
    {
        Result = (HALF)(Sign | 0x7FFF);
    }
    else
    {
        if (IValue < 0x38800000)
        {
            UINT Shift = 113 - (IValue >> 23);
            IValue = (0x800000 | (IValue & 0x7FFFFF)) >> Shift;
        }
        else
        {
            IValue += 0xC8000000;
        }

        Result = (HALF)(Sign | ((IValue + 0x0FFF + ((IValue >> 13) & 1)) >> 13)); 
    }

    return Result;

#else

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

#endif
}

XMFINLINE HALF* D3DX_ConvertFloatToHalfStream(HALF* pOutputStream, UINT OutputStride, CONST FLOAT* pInputStream, UINT InputStride, UINT FloatCount)
{
    UINT  i;
    BYTE* pFloat = (BYTE*)pInputStream;
    BYTE* pHalf = (BYTE*)pOutputStream;

    XMASSERT(pOutputStream);
    XMASSERT(pInputStream);

    for (i = 0; i < FloatCount; i++)
    {
        *(HALF*)pHalf = D3DX_ConvertFloatToHalf(*(FLOAT*)pFloat);
        pFloat += InputStride; 
        pHalf += OutputStride;
    }

    return pOutputStream;
}

XMFINLINE XMVECTOR D3DX_VectorReplicate(FLOAT Value)
{
    XMVECTOR V;

    V.v[0] = 
    V.v[1] = 
    V.v[2] = 
    V.v[3] = Value;

    return V;
}

XMFINLINE XMVECTOR D3DX_VectorLerp(XMVECTOR V0, XMVECTOR V1, FLOAT t)
{
    XMVECTOR Scale;
    XMVECTOR Length;
    XMVECTOR Result;

    Scale = D3DX_VectorReplicate(t);
    Length = XMVectorSubtract(V1, V0);
    Result = XMVectorMultiplyAdd(Length, Scale, V0);

    return Result;
}

//===========================================================================
//
// Inline Class Methods
//
//===========================================================================

#ifdef __cplusplus

//--------------------------
// Float16
//--------------------------

D3DXINLINE
D3DXFLOAT16::D3DXFLOAT16( FLOAT f )
{
#ifdef _NO_XBOXMATH
    D3DXFloat32To16Array(this, &f, 1);
#else
    value = D3DX_ConvertFloatToHalf(f);
#endif
}

D3DXINLINE
D3DXFLOAT16::D3DXFLOAT16( CONST D3DXFLOAT16& f )
{
    value = f.value;
}

// casting
D3DXINLINE
D3DXFLOAT16::operator FLOAT ()
{
#ifdef _NO_XBOXMATH
    FLOAT f;
    D3DXFloat16To32Array(&f, this, 1);
    return f;
#else
    return D3DX_ConvertHalfToFloat(value);
#endif
}

// binary operators
D3DXINLINE BOOL
D3DXFLOAT16::operator == ( CONST D3DXFLOAT16& f ) const
{
    return value == f.value;
}

D3DXINLINE BOOL
D3DXFLOAT16::operator != ( CONST D3DXFLOAT16& f ) const
{
    return value != f.value;
}


//--------------------------
// 2D Vector
//--------------------------

D3DXINLINE
D3DXVECTOR2::D3DXVECTOR2( CONST FLOAT *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    x = pf[0];
    y = pf[1];
}

D3DXINLINE
D3DXVECTOR2::D3DXVECTOR2( CONST D3DXFLOAT16 *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat16To32Array(&x, pf, 2);    
#else
    D3DX_ConvertHalfToFloatStream(&x, sizeof(FLOAT), (CONST HALF*)pf, sizeof(D3DXFLOAT16), 2);
#endif
}

D3DXINLINE
D3DXVECTOR2::D3DXVECTOR2( FLOAT fx, FLOAT fy )
{
    x = fx;
    y = fy;
}

// casting
D3DXINLINE
D3DXVECTOR2::operator FLOAT* ()
{
    return (FLOAT *) &x;
}

D3DXINLINE
D3DXVECTOR2::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &x;
}

// assignment operators
D3DXINLINE D3DXVECTOR2&
D3DXVECTOR2::operator = ( CONST D3DXVECTOR2& v )
{
    x = v.x;
    y = v.y;
    return *this;
}

D3DXINLINE D3DXVECTOR2&
D3DXVECTOR2::operator += ( CONST D3DXVECTOR2& v )
{
    x += v.x;
    y += v.y;
    return *this;
}

D3DXINLINE D3DXVECTOR2&
D3DXVECTOR2::operator -= ( CONST D3DXVECTOR2& v )
{
    x -= v.x;
    y -= v.y;
    return *this;
}

D3DXINLINE D3DXVECTOR2&
D3DXVECTOR2::operator *= ( FLOAT f )
{
    x *= f;
    y *= f;
    return *this;
}

D3DXINLINE D3DXVECTOR2&
D3DXVECTOR2::operator /= ( FLOAT f )
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
#else // !_NO_XBOXMATH
    XMVECTOR V, S;
    S = D3DX_VectorReplicate(f);
    V = D3DX_LoadFloat2((CONST XMFLOAT2*)this);
    S = XMVectorReciprocal(S);
    V = XMVectorMultiply(V, S);
    XMStoreFloat2((XMFLOAT2*)this, V);
#endif // !_NO_XBOXMATH
    return *this;
}

// unary operators
D3DXINLINE D3DXVECTOR2
D3DXVECTOR2::operator + () const
{
    return *this;
}

D3DXINLINE D3DXVECTOR2
D3DXVECTOR2::operator - () const
{
    return D3DXVECTOR2(-x, -y);
}

// binary operators
D3DXINLINE D3DXVECTOR2
D3DXVECTOR2::operator + ( CONST D3DXVECTOR2& v ) const
{
    return D3DXVECTOR2(x + v.x, y + v.y);
}

D3DXINLINE D3DXVECTOR2
D3DXVECTOR2::operator - ( CONST D3DXVECTOR2& v ) const
{
    return D3DXVECTOR2(x - v.x, y - v.y);
}

D3DXINLINE D3DXVECTOR2
D3DXVECTOR2::operator * ( FLOAT f ) const
{
    return D3DXVECTOR2(x * f, y * f);
}

D3DXINLINE D3DXVECTOR2
D3DXVECTOR2::operator / ( FLOAT f ) const
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    return D3DXVECTOR2(x * fInv, y * fInv);
#else // !_NO_XBOXMATH
    XMVECTOR    V, S;
    D3DXVECTOR2 Result;

    S = D3DX_VectorReplicate(f);
    V = D3DX_LoadFloat2((CONST XMFLOAT2*)this);
    S = XMVectorReciprocal(S);
    V = XMVectorMultiply(V, S);
    XMStoreFloat2((XMFLOAT2*)&Result, V);
    return Result;
#endif // !_NO_XBOXMATH
}


D3DXINLINE D3DXVECTOR2
operator * ( FLOAT f, CONST D3DXVECTOR2& v )
{
    return D3DXVECTOR2(f * v.x, f * v.y);
}

D3DXINLINE BOOL
D3DXVECTOR2::operator == ( CONST D3DXVECTOR2& v ) const
{
    return x == v.x && y == v.y;
}

D3DXINLINE BOOL
D3DXVECTOR2::operator != ( CONST D3DXVECTOR2& v ) const
{
    return x != v.x || y != v.y;
}



//--------------------------
// 2D Vector (16 bit)
//--------------------------

D3DXINLINE
D3DXVECTOR2_16F::D3DXVECTOR2_16F( CONST FLOAT *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat32To16Array(&x, pf, 2);
#else
    D3DX_ConvertFloatToHalfStream((HALF*)&x, sizeof(D3DXFLOAT16), pf, sizeof(FLOAT), 2);
#endif
}

D3DXINLINE
D3DXVECTOR2_16F::D3DXVECTOR2_16F( CONST D3DXFLOAT16 *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    *((DWORD *) &x) = *((DWORD *) &pf[0]);
}

D3DXINLINE
D3DXVECTOR2_16F::D3DXVECTOR2_16F( CONST D3DXFLOAT16 &fx, CONST D3DXFLOAT16 &fy )
{
    x = fx;
    y = fy;
}

// casting
D3DXINLINE
D3DXVECTOR2_16F::operator D3DXFLOAT16* ()
{
    return (D3DXFLOAT16*) &x;
}

D3DXINLINE
D3DXVECTOR2_16F::operator CONST D3DXFLOAT16* () const
{
    return (CONST D3DXFLOAT16*) &x;
}

// assignment operators
D3DXINLINE D3DXVECTOR2_16F&
D3DXVECTOR2_16F::operator = ( CONST D3DXVECTOR2_16F& v )
{
    x = v.x;
    y = v.y;
    return *this;
}

// binary operators
D3DXINLINE BOOL 
D3DXVECTOR2_16F::operator == ( CONST D3DXVECTOR2_16F &v ) const
{
    return *((DWORD *) &x) == *((DWORD *) &v.x);
}

D3DXINLINE BOOL 
D3DXVECTOR2_16F::operator != ( CONST D3DXVECTOR2_16F &v ) const
{
    return *((DWORD *) &x) != *((DWORD *) &v.x);
}


//--------------------------
// 3D Vector
//--------------------------
D3DXINLINE
D3DXVECTOR3::D3DXVECTOR3( CONST FLOAT *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    x = pf[0];
    y = pf[1];
    z = pf[2];
}

D3DXINLINE
D3DXVECTOR3::D3DXVECTOR3( CONST D3DVECTOR& v )
{
    x = v.x;
    y = v.y;
    z = v.z;
}

D3DXINLINE
D3DXVECTOR3::D3DXVECTOR3( CONST D3DXFLOAT16 *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat16To32Array(&x, pf, 3);
#else
    D3DX_ConvertHalfToFloatStream(&x, sizeof(FLOAT), (CONST HALF*)pf, sizeof(D3DXFLOAT16), 3);
#endif
}

D3DXINLINE
D3DXVECTOR3::D3DXVECTOR3( FLOAT fx, FLOAT fy, FLOAT fz )
{
    x = fx;
    y = fy;
    z = fz;
}


// casting
D3DXINLINE
D3DXVECTOR3::operator FLOAT* ()
{
    return (FLOAT *) &x;
}

D3DXINLINE
D3DXVECTOR3::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &x;
}


// assignment operators
D3DXINLINE D3DXVECTOR3&
D3DXVECTOR3::operator = ( CONST D3DXVECTOR3& v )
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

D3DXINLINE D3DXVECTOR3&
D3DXVECTOR3::operator += ( CONST D3DXVECTOR3& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

D3DXINLINE D3DXVECTOR3&
D3DXVECTOR3::operator -= ( CONST D3DXVECTOR3& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

D3DXINLINE D3DXVECTOR3&
D3DXVECTOR3::operator *= ( FLOAT f )
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}

D3DXINLINE D3DXVECTOR3&
D3DXVECTOR3::operator /= ( FLOAT f )
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
#else // !_NO_XBOXMATH
    XMVECTOR V, S;
    S = D3DX_VectorReplicate(f);
    V = D3DX_LoadFloat3((CONST XMFLOAT3*)this);
    S = XMVectorReciprocal(S);
    V = XMVectorMultiply(V, S);
    XMStoreFloat3((XMFLOAT3*)this, V);
#endif // !_NO_XBOXMATH
    return *this;
}


// unary operators
D3DXINLINE D3DXVECTOR3
D3DXVECTOR3::operator + () const
{
    return *this;
}

D3DXINLINE D3DXVECTOR3
D3DXVECTOR3::operator - () const
{
    return D3DXVECTOR3(-x, -y, -z);
}


// binary operators
D3DXINLINE D3DXVECTOR3
D3DXVECTOR3::operator + ( CONST D3DXVECTOR3& v ) const
{
    return D3DXVECTOR3(x + v.x, y + v.y, z + v.z);
}

D3DXINLINE D3DXVECTOR3
D3DXVECTOR3::operator - ( CONST D3DXVECTOR3& v ) const
{
    return D3DXVECTOR3(x - v.x, y - v.y, z - v.z);
}

D3DXINLINE D3DXVECTOR3
D3DXVECTOR3::operator * ( FLOAT f ) const
{
    return D3DXVECTOR3(x * f, y * f, z * f);
}

D3DXINLINE D3DXVECTOR3
D3DXVECTOR3::operator / ( FLOAT f ) const
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    return D3DXVECTOR3(x * fInv, y * fInv, z * fInv);
#else // !_NO_XBOXMATH
    XMVECTOR    V, S;
    D3DXVECTOR3 Result;

    S = D3DX_VectorReplicate(f);
    V = D3DX_LoadFloat3((CONST XMFLOAT3*)this);
    S = XMVectorReciprocal(S);
    V = XMVectorMultiply(V, S);
    XMStoreFloat3((XMFLOAT3*)&Result, V);
    return Result;
#endif // !_NO_XBOXMATH
}


D3DXINLINE D3DXVECTOR3
operator * ( FLOAT f, CONST struct D3DXVECTOR3& v )
{
    return D3DXVECTOR3(f * v.x, f * v.y, f * v.z);
}


D3DXINLINE BOOL
D3DXVECTOR3::operator == ( CONST D3DXVECTOR3& v ) const
{
    return x == v.x && y == v.y && z == v.z;
}

D3DXINLINE BOOL
D3DXVECTOR3::operator != ( CONST D3DXVECTOR3& v ) const
{
    return x != v.x || y != v.y || z != v.z;
}



//--------------------------
// 3D Vector (16 bit)
//--------------------------

D3DXINLINE
D3DXVECTOR3_16F::D3DXVECTOR3_16F( CONST FLOAT *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat32To16Array(&x, pf, 3);
#else
    D3DX_ConvertFloatToHalfStream((HALF*)&x, sizeof(D3DXFLOAT16), pf, sizeof(FLOAT), 3);
#endif
}

D3DXINLINE
D3DXVECTOR3_16F::D3DXVECTOR3_16F( CONST D3DVECTOR& v )
{
#ifdef _NO_XBOXMATH
    D3DXFloat32To16Array(&x, &v.x, 1);
    D3DXFloat32To16Array(&y, &v.y, 1);
    D3DXFloat32To16Array(&z, &v.z, 1);
#else
    x = D3DX_ConvertFloatToHalf(v.x);
    y = D3DX_ConvertFloatToHalf(v.y);
    z = D3DX_ConvertFloatToHalf(v.z);
#endif
}

D3DXINLINE
D3DXVECTOR3_16F::D3DXVECTOR3_16F( CONST D3DXFLOAT16 *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    *((DWORD *) &x) = *((DWORD *) &pf[0]);
    *((WORD  *) &z) = *((WORD  *) &pf[2]);
}

D3DXINLINE
D3DXVECTOR3_16F::D3DXVECTOR3_16F( CONST D3DXFLOAT16 &fx, CONST D3DXFLOAT16 &fy, CONST D3DXFLOAT16 &fz )
{
    x = fx;
    y = fy;
    z = fz;
}

// casting
D3DXINLINE
D3DXVECTOR3_16F::operator D3DXFLOAT16* ()
{
    return (D3DXFLOAT16*) &x;
}

D3DXINLINE
D3DXVECTOR3_16F::operator CONST D3DXFLOAT16* () const
{
    return (CONST D3DXFLOAT16*) &x;
}

// assignment operators
D3DXINLINE D3DXVECTOR3_16F&
D3DXVECTOR3_16F::operator = ( CONST D3DXVECTOR3_16F& v )
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

// binary operators
D3DXINLINE BOOL 
D3DXVECTOR3_16F::operator == ( CONST D3DXVECTOR3_16F &v ) const
{
    return *((DWORD *) &x) == *((DWORD *) &v.x) &&
           *((WORD  *) &z) == *((WORD  *) &v.z);
}

D3DXINLINE BOOL 
D3DXVECTOR3_16F::operator != ( CONST D3DXVECTOR3_16F &v ) const
{
    return *((DWORD *) &x) != *((DWORD *) &v.x) ||
           *((WORD  *) &z) != *((WORD  *) &v.z);
}



//--------------------------
// 4D Vector
//--------------------------
D3DXINLINE
D3DXVECTOR4::D3DXVECTOR4( CONST FLOAT *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    x = pf[0];
    y = pf[1];
    z = pf[2];
    w = pf[3];
}

D3DXINLINE
D3DXVECTOR4::D3DXVECTOR4( CONST D3DXFLOAT16 *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat16To32Array(&x, pf, 4);
#else
    D3DX_ConvertHalfToFloatStream(&x, sizeof(FLOAT), (CONST HALF*)pf, sizeof(D3DXFLOAT16), 4);
#endif
}

D3DXINLINE
D3DXVECTOR4::D3DXVECTOR4( FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw )
{
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}

// casting
D3DXINLINE
D3DXVECTOR4::operator FLOAT* ()
{
    return (FLOAT *) &x;
}

D3DXINLINE
D3DXVECTOR4::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &x;
}


// assignment operators
D3DXINLINE D3DXVECTOR4&
D3DXVECTOR4::operator = ( CONST D3DXVECTOR4& v )
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}

D3DXINLINE D3DXVECTOR4&
D3DXVECTOR4::operator += ( CONST D3DXVECTOR4& v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

D3DXINLINE D3DXVECTOR4&
D3DXVECTOR4::operator -= ( CONST D3DXVECTOR4& v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

D3DXINLINE D3DXVECTOR4&
D3DXVECTOR4::operator *= ( FLOAT f )
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}

D3DXINLINE D3DXVECTOR4&
D3DXVECTOR4::operator /= ( FLOAT f )
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    w *= fInv;
#else // !_NO_XBOXMATH
    XMVECTOR V, S;
    S = D3DX_VectorReplicate(f);
    V = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    S = XMVectorReciprocal(S);
    V = XMVectorMultiply(V, S);
    XMStoreFloat4NC((XMFLOAT4*)this, V);
#endif // !_NO_XBOXMATH
    return *this;
}


// unary operators
D3DXINLINE D3DXVECTOR4
D3DXVECTOR4::operator + () const
{
    return *this;
}

D3DXINLINE D3DXVECTOR4
D3DXVECTOR4::operator - () const
{
    return D3DXVECTOR4(-x, -y, -z, -w);
}


// binary operators
D3DXINLINE D3DXVECTOR4
D3DXVECTOR4::operator + ( CONST D3DXVECTOR4& v ) const
{
    return D3DXVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w);
}

D3DXINLINE D3DXVECTOR4
D3DXVECTOR4::operator - ( CONST D3DXVECTOR4& v ) const
{
    return D3DXVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w);
}

D3DXINLINE D3DXVECTOR4
D3DXVECTOR4::operator * ( FLOAT f ) const
{
    return D3DXVECTOR4(x * f, y * f, z * f, w * f);
}

D3DXINLINE D3DXVECTOR4
D3DXVECTOR4::operator / ( FLOAT f ) const
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    return D3DXVECTOR4(x * fInv, y * fInv, z * fInv, w * fInv);
#else // !_NO_XBOXMATH
    XMVECTOR    V, S;
    D3DXVECTOR4 Result;

    S = D3DX_VectorReplicate(f);
    V = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    S = XMVectorReciprocal(S);
    V = XMVectorMultiply(V, S);
    XMStoreFloat4NC((XMFLOAT4*)&Result, V);
    return Result;
#endif // !_NO_XBOXMATH
}


D3DXINLINE D3DXVECTOR4
operator * ( FLOAT f, CONST D3DXVECTOR4& v )
{
    return D3DXVECTOR4(f * v.x, f * v.y, f * v.z, f * v.w);
}


D3DXINLINE BOOL
D3DXVECTOR4::operator == ( CONST D3DXVECTOR4& v ) const
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

D3DXINLINE BOOL
D3DXVECTOR4::operator != ( CONST D3DXVECTOR4& v ) const
{
    return x != v.x || y != v.y || z != v.z || w != v.w;
}



//--------------------------
// 4D Vector (16 bit)
//--------------------------

D3DXINLINE
D3DXVECTOR4_16F::D3DXVECTOR4_16F( CONST FLOAT *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat32To16Array(&x, pf, 4);
#else
    D3DX_ConvertFloatToHalfStream((HALF*)&x, sizeof(D3DXFLOAT16), pf, sizeof(FLOAT), 4);
#endif
}

D3DXINLINE
D3DXVECTOR4_16F::D3DXVECTOR4_16F( CONST D3DXFLOAT16 *pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    *((DWORD *) &x) = *((DWORD *) &pf[0]);
    *((DWORD *) &z) = *((DWORD *) &pf[2]);
}

D3DXINLINE
D3DXVECTOR4_16F::D3DXVECTOR4_16F( CONST D3DXFLOAT16 &fx, CONST D3DXFLOAT16 &fy, CONST D3DXFLOAT16 &fz, CONST D3DXFLOAT16 &fw )
{
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}

// casting
D3DXINLINE
D3DXVECTOR4_16F::operator D3DXFLOAT16* ()
{
    return (D3DXFLOAT16*) &x;
}

D3DXINLINE
D3DXVECTOR4_16F::operator CONST D3DXFLOAT16* () const
{
    return (CONST D3DXFLOAT16*) &x;
}

// assignment operators
D3DXINLINE D3DXVECTOR4_16F&
D3DXVECTOR4_16F::operator = ( CONST D3DXVECTOR4_16F& v )
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}

// binary operators
D3DXINLINE BOOL 
D3DXVECTOR4_16F::operator == ( CONST D3DXVECTOR4_16F &v ) const
{
    return *((DWORD *) &x) == *((DWORD *) &v.x) &&
           *((DWORD *) &z) == *((DWORD *) &v.z);
}

D3DXINLINE BOOL 
D3DXVECTOR4_16F::operator != ( CONST D3DXVECTOR4_16F &v ) const
{
    return *((DWORD *) &x) != *((DWORD *) &v.x) ||
           *((DWORD *) &z) != *((DWORD *) &v.z);
}


//--------------------------
// Matrix
//--------------------------
D3DXINLINE
D3DXMATRIX::D3DXMATRIX( CONST FLOAT* pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    memcpy(&_11, pf, sizeof(D3DXMATRIX));
}

D3DXINLINE
D3DXMATRIX::D3DXMATRIX( CONST D3DMATRIX& mat )
{
    memcpy(&_11, &mat, sizeof(D3DXMATRIX));
}

D3DXINLINE
D3DXMATRIX::D3DXMATRIX( CONST D3DXFLOAT16* pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat16To32Array(&_11, pf, 16);
#else
    D3DX_ConvertHalfToFloatStream(&_11, sizeof(FLOAT), (CONST HALF*)pf, sizeof(D3DXFLOAT16), 16);
#endif
}

D3DXINLINE
D3DXMATRIX::D3DXMATRIX( FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14,
                        FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24,
                        FLOAT f31, FLOAT f32, FLOAT f33, FLOAT f34,
                        FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44 )
{
    _11 = f11; _12 = f12; _13 = f13; _14 = f14;
    _21 = f21; _22 = f22; _23 = f23; _24 = f24;
    _31 = f31; _32 = f32; _33 = f33; _34 = f34;
    _41 = f41; _42 = f42; _43 = f43; _44 = f44;
}



// access grants
D3DXINLINE FLOAT&
D3DXMATRIX::operator () ( UINT iRow, UINT iCol )
{
    return m[iRow][iCol];
}

D3DXINLINE FLOAT
D3DXMATRIX::operator () ( UINT iRow, UINT iCol ) const
{
    return m[iRow][iCol];
}


// casting operators
D3DXINLINE
D3DXMATRIX::operator FLOAT* ()
{
    return (FLOAT *) &_11;
}

D3DXINLINE
D3DXMATRIX::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &_11;
}


// assignment operators
D3DXINLINE D3DXMATRIX&
D3DXMATRIX::operator = ( CONST D3DXMATRIX& mat )
{
    _11 = mat._11; _12 = mat._12; _13 = mat._13; _14 = mat._14;
    _21 = mat._21; _22 = mat._22; _23 = mat._23; _24 = mat._24;
    _31 = mat._31; _32 = mat._32; _33 = mat._33; _34 = mat._34;
    _41 = mat._41; _42 = mat._42; _43 = mat._43; _44 = mat._44;
    return *this;
}

D3DXINLINE D3DXMATRIX&
D3DXMATRIX::operator *= ( CONST D3DXMATRIX& mat )
{
    D3DXMatrixMultiply(this, this, &mat);
    return *this;
}

D3DXINLINE D3DXMATRIX&
D3DXMATRIX::operator += ( CONST D3DXMATRIX& mat )
{
    _11 += mat._11; _12 += mat._12; _13 += mat._13; _14 += mat._14;
    _21 += mat._21; _22 += mat._22; _23 += mat._23; _24 += mat._24;
    _31 += mat._31; _32 += mat._32; _33 += mat._33; _34 += mat._34;
    _41 += mat._41; _42 += mat._42; _43 += mat._43; _44 += mat._44;
    return *this;
}

D3DXINLINE D3DXMATRIX&
D3DXMATRIX::operator -= ( CONST D3DXMATRIX& mat )
{
    _11 -= mat._11; _12 -= mat._12; _13 -= mat._13; _14 -= mat._14;
    _21 -= mat._21; _22 -= mat._22; _23 -= mat._23; _24 -= mat._24;
    _31 -= mat._31; _32 -= mat._32; _33 -= mat._33; _34 -= mat._34;
    _41 -= mat._41; _42 -= mat._42; _43 -= mat._43; _44 -= mat._44;
    return *this;
}

D3DXINLINE D3DXMATRIX&
D3DXMATRIX::operator *= ( FLOAT f )
{
    _11 *= f; _12 *= f; _13 *= f; _14 *= f;
    _21 *= f; _22 *= f; _23 *= f; _24 *= f;
    _31 *= f; _32 *= f; _33 *= f; _34 *= f;
    _41 *= f; _42 *= f; _43 *= f; _44 *= f;
    return *this;
}

D3DXINLINE D3DXMATRIX&
D3DXMATRIX::operator /= ( FLOAT f )
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    _11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
    _21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
    _31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
    _41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;
#else // !_NO_XBOXMATH
    XMMATRIX M;
    XMVECTOR S;

    S = D3DX_VectorReplicate(f);
    M = D3DX_LoadFloat4x4((CONST XMFLOAT4X4*)this);
    S = XMVectorReciprocal(S);
    M.r[0] = XMVectorMultiply(M.r[0], S);
    M.r[1] = XMVectorMultiply(M.r[1], S);
    M.r[2] = XMVectorMultiply(M.r[2], S);
    M.r[3] = XMVectorMultiply(M.r[3], S);
    XMStoreFloat4x4NC((XMFLOAT4X4*)this, M);
#endif // !_NO_XBOXMATH
    return *this;
}


// unary operators
D3DXINLINE D3DXMATRIX
D3DXMATRIX::operator + () const
{
    return *this;
}

D3DXINLINE D3DXMATRIX
D3DXMATRIX::operator - () const
{
    return D3DXMATRIX(-_11, -_12, -_13, -_14,
                      -_21, -_22, -_23, -_24,
                      -_31, -_32, -_33, -_34,
                      -_41, -_42, -_43, -_44);
}


// binary operators
D3DXINLINE D3DXMATRIX
D3DXMATRIX::operator * ( CONST D3DXMATRIX& mat ) const
{
    D3DXMATRIX matT;
    D3DXMatrixMultiply(&matT, this, &mat);
    return matT;
}

D3DXINLINE D3DXMATRIX
D3DXMATRIX::operator + ( CONST D3DXMATRIX& mat ) const
{
    return D3DXMATRIX(_11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
                      _21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
                      _31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
                      _41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44);
}

D3DXINLINE D3DXMATRIX
D3DXMATRIX::operator - ( CONST D3DXMATRIX& mat ) const
{
    return D3DXMATRIX(_11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
                      _21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
                      _31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
                      _41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44);
}

D3DXINLINE D3DXMATRIX
D3DXMATRIX::operator * ( FLOAT f ) const
{
    return D3DXMATRIX(_11 * f, _12 * f, _13 * f, _14 * f,
                      _21 * f, _22 * f, _23 * f, _24 * f,
                      _31 * f, _32 * f, _33 * f, _34 * f,
                      _41 * f, _42 * f, _43 * f, _44 * f);
}

D3DXINLINE D3DXMATRIX
D3DXMATRIX::operator / ( FLOAT f ) const
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    return D3DXMATRIX(_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv,
                      _21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv,
                      _31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv,
                      _41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv);
#else // !_NO_XBOXMATH
    XMMATRIX    M;
    XMVECTOR    S;
    D3DXMATRIX  Result;

    S = D3DX_VectorReplicate(f);
    M = D3DX_LoadFloat4x4((CONST XMFLOAT4X4*)this);
    S = XMVectorReciprocal(S);
    M.r[0] = XMVectorMultiply(M.r[0], S);
    M.r[1] = XMVectorMultiply(M.r[1], S);
    M.r[2] = XMVectorMultiply(M.r[2], S);
    M.r[3] = XMVectorMultiply(M.r[3], S);
    XMStoreFloat4x4NC((XMFLOAT4X4*)&Result, M);
    return Result;
#endif // !_NO_XBOXMATH
}


D3DXINLINE D3DXMATRIX
operator * ( FLOAT f, CONST D3DXMATRIX& mat )
{
    return D3DXMATRIX(f * mat._11, f * mat._12, f * mat._13, f * mat._14,
                      f * mat._21, f * mat._22, f * mat._23, f * mat._24,
                      f * mat._31, f * mat._32, f * mat._33, f * mat._34,
                      f * mat._41, f * mat._42, f * mat._43, f * mat._44);
}


D3DXINLINE BOOL
D3DXMATRIX::operator == ( CONST D3DXMATRIX& mat ) const
{
    return 0 == memcmp(this, &mat, sizeof(D3DXMATRIX));
}

D3DXINLINE BOOL
D3DXMATRIX::operator != ( CONST D3DXMATRIX& mat ) const
{
    return 0 != memcmp(this, &mat, sizeof(D3DXMATRIX));
}



//--------------------------
// Aligned Matrices
//--------------------------

D3DXINLINE
_D3DXMATRIXA16::_D3DXMATRIXA16( CONST FLOAT* f ) : 
    D3DXMATRIX( f ) 
{
}

D3DXINLINE
_D3DXMATRIXA16::_D3DXMATRIXA16( CONST D3DMATRIX& m ) : 
    D3DXMATRIX( m ) 
{
}

D3DXINLINE
_D3DXMATRIXA16::_D3DXMATRIXA16( CONST D3DXFLOAT16* f ) : 
    D3DXMATRIX( f ) 
{
}

D3DXINLINE
_D3DXMATRIXA16::_D3DXMATRIXA16( FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _14,
                                FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _24,
                                FLOAT _31, FLOAT _32, FLOAT _33, FLOAT _34,
                                FLOAT _41, FLOAT _42, FLOAT _43, FLOAT _44 ) :
    D3DXMATRIX(_11, _12, _13, _14,
               _21, _22, _23, _24,
               _31, _32, _33, _34,
               _41, _42, _43, _44) 
{
}

D3DXINLINE void* 
_D3DXMATRIXA16::operator new( size_t s )
{
    LPBYTE p = ::new BYTE[s + 16];
    if (p)
    {
        BYTE offset = (BYTE)(16 - ((UINT_PTR)p & 15));
        p += offset;
        p[-1] = offset;
    }
    return p;
}

D3DXINLINE void* 
_D3DXMATRIXA16::operator new[]( size_t s )
{
    LPBYTE p = ::new BYTE[s + 16];
    if (p)
    {
        BYTE offset = (BYTE)(16 - ((UINT_PTR)p & 15));
        p += offset;
        p[-1] = offset;
    }
    return p;
}

D3DXINLINE void 
_D3DXMATRIXA16::operator delete(void* p)
{
    if(p)
    {
        BYTE* pb = static_cast<BYTE*>(p);
        pb -= pb[-1];
        ::delete [] pb;
    }
}

D3DXINLINE void 
_D3DXMATRIXA16::operator delete[](void* p)
{
    if(p)
    {
        BYTE* pb = static_cast<BYTE*>(p);
        pb -= pb[-1];
        ::delete [] pb;
    }
}

D3DXINLINE _D3DXMATRIXA16& 
_D3DXMATRIXA16::operator=(CONST D3DXMATRIX& rhs)
{
    memcpy(&_11, &rhs, sizeof(D3DXMATRIX));
    return *this;
}


//--------------------------
// Quaternion
//--------------------------

D3DXINLINE
D3DXQUATERNION::D3DXQUATERNION( CONST FLOAT* pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    x = pf[0];
    y = pf[1];
    z = pf[2];
    w = pf[3];
}

D3DXINLINE
D3DXQUATERNION::D3DXQUATERNION( CONST D3DXFLOAT16* pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat16To32Array(&x, pf, 4);
#else
    D3DX_ConvertHalfToFloatStream(&x, sizeof(FLOAT), (CONST HALF*)pf, sizeof(D3DXFLOAT16), 4);
#endif
}

D3DXINLINE
D3DXQUATERNION::D3DXQUATERNION( FLOAT fx, FLOAT fy, FLOAT fz, FLOAT fw )
{
    x = fx;
    y = fy;
    z = fz;
    w = fw;
}


// casting
D3DXINLINE
D3DXQUATERNION::operator FLOAT* ()
{
    return (FLOAT *) &x;
}

D3DXINLINE
D3DXQUATERNION::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &x;
}


// assignment operators
D3DXINLINE D3DXQUATERNION&
D3DXQUATERNION::operator = ( CONST D3DXQUATERNION& q )
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
    return *this;
}

D3DXINLINE D3DXQUATERNION&
D3DXQUATERNION::operator += ( CONST D3DXQUATERNION& q )
{
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
}

D3DXINLINE D3DXQUATERNION&
D3DXQUATERNION::operator -= ( CONST D3DXQUATERNION& q )
{
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
}

D3DXINLINE D3DXQUATERNION&
D3DXQUATERNION::operator *= ( CONST D3DXQUATERNION& q )
{
#ifdef _NO_XBOXMATH
    D3DXQuaternionMultiply(this, this, &q);
#else
    XMVECTOR Q0, Q1;
    Q0 = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    Q1 = D3DX_LoadFloat4((CONST XMFLOAT4*)&q);
    Q0 = XMQuaternionMultiply(Q0, Q1);
    XMStoreFloat4NC((XMFLOAT4*)this, Q0);
#endif
    return *this;
}

D3DXINLINE D3DXQUATERNION&
D3DXQUATERNION::operator *= ( FLOAT f )
{
    x *= f;
    y *= f;
    z *= f;
    w *= f;
    return *this;
}

D3DXINLINE D3DXQUATERNION&
D3DXQUATERNION::operator /= ( FLOAT f )
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    x *= fInv;
    y *= fInv;
    z *= fInv;
    w *= fInv;
#else // !_NO_XBOXMATH
    XMVECTOR Q, S;
    S = D3DX_VectorReplicate(f);
    Q = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    S = XMVectorReciprocal(S);
    Q = XMVectorMultiply(Q, S);
    XMStoreFloat4NC((XMFLOAT4*)this, Q);
#endif // !_NO_XBOXMATH
    return *this;
}


// unary operators
D3DXINLINE D3DXQUATERNION
D3DXQUATERNION::operator + () const
{
    return *this;
}

D3DXINLINE D3DXQUATERNION
D3DXQUATERNION::operator - () const
{
    return D3DXQUATERNION(-x, -y, -z, -w);
}


// binary operators
D3DXINLINE D3DXQUATERNION
D3DXQUATERNION::operator + ( CONST D3DXQUATERNION& q ) const
{
    return D3DXQUATERNION(x + q.x, y + q.y, z + q.z, w + q.w);
}

D3DXINLINE D3DXQUATERNION
D3DXQUATERNION::operator - ( CONST D3DXQUATERNION& q ) const
{
    return D3DXQUATERNION(x - q.x, y - q.y, z - q.z, w - q.w);
}

D3DXINLINE D3DXQUATERNION
D3DXQUATERNION::operator * ( CONST D3DXQUATERNION& q ) const
{
    D3DXQUATERNION qT;
#ifdef _NO_XBOXMATH
    D3DXQuaternionMultiply(&qT, this, &q);
#else
    XMVECTOR Q0, Q1;
    Q0 = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    Q1 = D3DX_LoadFloat4((CONST XMFLOAT4*)&q);
    Q0 = XMQuaternionMultiply(Q0, Q1);
    XMStoreFloat4NC((XMFLOAT4*)&qT, Q0);
#endif
    return qT;
}

D3DXINLINE D3DXQUATERNION
D3DXQUATERNION::operator * ( FLOAT f ) const
{
    return D3DXQUATERNION(x * f, y * f, z * f, w * f);
}

D3DXINLINE D3DXQUATERNION
D3DXQUATERNION::operator / ( FLOAT f ) const
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    return D3DXQUATERNION(x * fInv, y * fInv, z * fInv, w * fInv);
#else // !_NO_XBOXMATH
    XMVECTOR       Q, S;
    D3DXQUATERNION Result;

    S = D3DX_VectorReplicate(f);
    Q = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    S = XMVectorReciprocal(S);
    Q = XMVectorMultiply(Q, S);
    XMStoreFloat4NC((XMFLOAT4*)&Result, Q);
    return Result;
#endif // !_NO_XBOXMATH
}


D3DXINLINE D3DXQUATERNION
operator * (FLOAT f, CONST D3DXQUATERNION& q )
{
    return D3DXQUATERNION(f * q.x, f * q.y, f * q.z, f * q.w);
}


D3DXINLINE BOOL
D3DXQUATERNION::operator == ( CONST D3DXQUATERNION& q ) const
{
    return x == q.x && y == q.y && z == q.z && w == q.w;
}

D3DXINLINE BOOL
D3DXQUATERNION::operator != ( CONST D3DXQUATERNION& q ) const
{
    return x != q.x || y != q.y || z != q.z || w != q.w;
}



//--------------------------
// Plane
//--------------------------

D3DXINLINE
D3DXPLANE::D3DXPLANE( CONST FLOAT* pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    a = pf[0];
    b = pf[1];
    c = pf[2];
    d = pf[3];
}

D3DXINLINE
D3DXPLANE::D3DXPLANE( CONST D3DXFLOAT16* pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat16To32Array(&a, pf, 4);
#else
    D3DX_ConvertHalfToFloatStream(&a, sizeof(FLOAT), (CONST HALF*)pf, sizeof(D3DXFLOAT16), 4);
#endif
}

D3DXINLINE
D3DXPLANE::D3DXPLANE( FLOAT fa, FLOAT fb, FLOAT fc, FLOAT fd )
{
    a = fa;
    b = fb;
    c = fc;
    d = fd;
}


// casting
D3DXINLINE
D3DXPLANE::operator FLOAT* ()
{
    return (FLOAT *) &a;
}

D3DXINLINE
D3DXPLANE::operator CONST FLOAT* () const
{
    return (CONST FLOAT *) &a;
}


// assignment operators
D3DXINLINE D3DXPLANE&
D3DXPLANE::operator = ( CONST D3DXPLANE& p )
{
    a = p.a;
    b = p.b;
    c = p.c;
    d = p.d;
    return *this;
}

D3DXINLINE D3DXPLANE&
D3DXPLANE::operator *= ( FLOAT f )
{
    a *= f;
    b *= f;
    c *= f;
    d *= f;
    return *this;
}

D3DXINLINE D3DXPLANE&
D3DXPLANE::operator /= ( FLOAT f )
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    a *= fInv;
    b *= fInv;
    c *= fInv;
    d *= fInv;
#else // !_NO_XBOXMATH
    XMVECTOR P, S;
    S = D3DX_VectorReplicate(f);
    P = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    S = XMVectorReciprocal(S);
    P = XMVectorMultiply(P, S);
    XMStoreFloat4NC((XMFLOAT4*)this, P);
#endif // !_NO_XBOXMATH
    return *this;
}


// unary operators
D3DXINLINE D3DXPLANE
D3DXPLANE::operator + () const
{
    return *this;
}

D3DXINLINE D3DXPLANE
D3DXPLANE::operator - () const
{
    return D3DXPLANE(-a, -b, -c, -d);
}


// binary operators
D3DXINLINE D3DXPLANE
D3DXPLANE::operator * ( FLOAT f ) const
{
    return D3DXPLANE(a * f, b * f, c * f, d * f);
}

D3DXINLINE D3DXPLANE
D3DXPLANE::operator / ( FLOAT f ) const
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    return D3DXPLANE(a * fInv, b * fInv, c * fInv, d * fInv);
#else // !_NO_XBOXMATH
    XMVECTOR  P, S;
    D3DXPLANE Result;

    S = D3DX_VectorReplicate(f);
    P = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    S = XMVectorReciprocal(S);
    P = XMVectorMultiply(P, S);
    XMStoreFloat4NC((XMFLOAT4*)&Result, P);
    return Result;
#endif // !_NO_XBOXMATH
}

D3DXINLINE D3DXPLANE
operator * (FLOAT f, CONST D3DXPLANE& p )
{
    return D3DXPLANE(f * p.a, f * p.b, f * p.c, f * p.d);
}

D3DXINLINE BOOL
D3DXPLANE::operator == ( CONST D3DXPLANE& p ) const
{
    return a == p.a && b == p.b && c == p.c && d == p.d;
}

D3DXINLINE BOOL
D3DXPLANE::operator != ( CONST D3DXPLANE& p ) const
{
    return a != p.a || b != p.b || c != p.c || d != p.d;
}




//--------------------------
// Color
//--------------------------

D3DXINLINE
D3DXCOLOR::D3DXCOLOR( DWORD dw )
{
    CONST FLOAT f = 1.0f / 255.0f;
    r = f * (FLOAT) (unsigned char) (dw >> 16);
    g = f * (FLOAT) (unsigned char) (dw >>  8);
    b = f * (FLOAT) (unsigned char) (dw >>  0);
    a = f * (FLOAT) (unsigned char) (dw >> 24);
}

D3DXINLINE
D3DXCOLOR::D3DXCOLOR( CONST FLOAT* pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

    r = pf[0];
    g = pf[1];
    b = pf[2];
    a = pf[3];
}

D3DXINLINE
D3DXCOLOR::D3DXCOLOR( CONST D3DXFLOAT16* pf )
{
#ifdef D3DX_DEBUG
    if(!pf)
        return;
#endif

#ifdef _NO_XBOXMATH
    D3DXFloat16To32Array(&r, pf, 4);
#else
    D3DX_ConvertHalfToFloatStream(&r, sizeof(FLOAT), (CONST HALF*)pf, sizeof(D3DXFLOAT16), 4);
#endif
}

D3DXINLINE
D3DXCOLOR::D3DXCOLOR( CONST D3DCOLORVALUE& c )
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
}

D3DXINLINE
D3DXCOLOR::D3DXCOLOR( FLOAT fr, FLOAT fg, FLOAT fb, FLOAT fa )
{
    r = fr;
    g = fg;
    b = fb;
    a = fa;
}


// casting
D3DXINLINE
D3DXCOLOR::operator DWORD () const
{
#ifdef _NO_XBOXMATH
    DWORD dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (DWORD) (r * 255.0f + 0.5f);
    DWORD dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (DWORD) (g * 255.0f + 0.5f);
    DWORD dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (DWORD) (b * 255.0f + 0.5f);
    DWORD dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (DWORD) (a * 255.0f + 0.5f);

    return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
#else
    XMVECTOR ColorSrc;
    XMCOLOR  ColorDst;

    ColorSrc = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    XMStoreColor(&ColorDst, ColorSrc);

    return ColorDst.c;
#endif
}


D3DXINLINE
D3DXCOLOR::operator FLOAT * ()
{
    return (FLOAT *) &r;
}

D3DXINLINE
D3DXCOLOR::operator CONST FLOAT * () const
{
    return (CONST FLOAT *) &r;
}


D3DXINLINE
D3DXCOLOR::operator D3DCOLORVALUE * ()
{
    return (D3DCOLORVALUE *) &r;
}

D3DXINLINE
D3DXCOLOR::operator CONST D3DCOLORVALUE * () const
{
    return (CONST D3DCOLORVALUE *) &r;
}


D3DXINLINE
D3DXCOLOR::operator D3DCOLORVALUE& ()
{
    return *((D3DCOLORVALUE *) &r);
}

D3DXINLINE
D3DXCOLOR::operator CONST D3DCOLORVALUE& () const
{
    return *((CONST D3DCOLORVALUE *) &r);
}


// assignment operators
D3DXINLINE D3DXCOLOR&
D3DXCOLOR::operator = ( CONST D3DXCOLOR& c )
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
    return *this;
}

D3DXINLINE D3DXCOLOR&
D3DXCOLOR::operator += ( CONST D3DXCOLOR& c )
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return *this;
}

D3DXINLINE D3DXCOLOR&
D3DXCOLOR::operator -= ( CONST D3DXCOLOR& c )
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return *this;
}

D3DXINLINE D3DXCOLOR&
D3DXCOLOR::operator *= ( FLOAT f )
{
    r *= f;
    g *= f;
    b *= f;
    a *= f;
    return *this;
}

D3DXINLINE D3DXCOLOR&
D3DXCOLOR::operator /= ( FLOAT f )
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    r *= fInv;
    g *= fInv;
    b *= fInv;
    a *= fInv;
#else // !_NO_XBOXMATH
    XMVECTOR C, S;
    S = D3DX_VectorReplicate(f);
    C = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    S = XMVectorReciprocal(S);
    C = XMVectorMultiply(C, S);
    XMStoreFloat4NC((XMFLOAT4*)this, C);
#endif // !_NO_XBOXMATH
    return *this;
}


// unary operators
D3DXINLINE D3DXCOLOR
D3DXCOLOR::operator + () const
{
    return *this;
}

D3DXINLINE D3DXCOLOR
D3DXCOLOR::operator - () const
{
    return D3DXCOLOR(-r, -g, -b, -a);
}


// binary operators
D3DXINLINE D3DXCOLOR
D3DXCOLOR::operator + ( CONST D3DXCOLOR& c ) const
{
    return D3DXCOLOR(r + c.r, g + c.g, b + c.b, a + c.a);
}

D3DXINLINE D3DXCOLOR
D3DXCOLOR::operator - ( CONST D3DXCOLOR& c ) const
{
    return D3DXCOLOR(r - c.r, g - c.g, b - c.b, a - c.a);
}

D3DXINLINE D3DXCOLOR
D3DXCOLOR::operator * ( FLOAT f ) const
{
    return D3DXCOLOR(r * f, g * f, b * f, a * f);
}

D3DXINLINE D3DXCOLOR
D3DXCOLOR::operator / ( FLOAT f ) const
{
#ifdef _NO_XBOXMATH
    FLOAT fInv = 1.0f / f;
    return D3DXCOLOR(r * fInv, g * fInv, b * fInv, a * fInv);
#else // !_NO_XBOXMATH
    XMVECTOR  C, S;
    D3DXCOLOR Result;

    S = D3DX_VectorReplicate(f);
    C = D3DX_LoadFloat4((CONST XMFLOAT4*)this);
    S = XMVectorReciprocal(S);
    C = XMVectorMultiply(C, S);
    XMStoreFloat4NC((XMFLOAT4*)&Result, C);
    return Result;
#endif // !_NO_XBOXMATH
}


D3DXINLINE D3DXCOLOR
operator * (FLOAT f, CONST D3DXCOLOR& c )
{
    return D3DXCOLOR(f * c.r, f * c.g, f * c.b, f * c.a);
}


D3DXINLINE BOOL
D3DXCOLOR::operator == ( CONST D3DXCOLOR& c ) const
{
    return r == c.r && g == c.g && b == c.b && a == c.a;
}

D3DXINLINE BOOL
D3DXCOLOR::operator != ( CONST D3DXCOLOR& c ) const
{
    return r != c.r || g != c.g || b != c.b || a != c.a;
}


#endif //__cplusplus



//===========================================================================
//
// Inline functions
//
//===========================================================================


//--------------------------
// 2D Vector
//--------------------------

D3DXINLINE FLOAT D3DXVec2Length
    ( CONST D3DXVECTOR2 *pV )
{
#ifdef D3DX_DEBUG
    if(!pV)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

#ifdef __cplusplus
    return sqrtf(pV->x * pV->x + pV->y * pV->y);
#else
    return (FLOAT) sqrt(pV->x * pV->x + pV->y * pV->y);
#endif

#else // !_NO_XBOXMATH

    XMVECTOR V, Result;

    V = D3DX_LoadFloat2((CONST XMFLOAT2*)pV);
    Result = XMVector2Length(V);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXVec2LengthSq
    ( CONST D3DXVECTOR2 *pV )
{
#ifdef D3DX_DEBUG
    if(!pV)
        return 0.0f;
#endif

    return pV->x * pV->x + pV->y * pV->y;
}

D3DXINLINE FLOAT D3DXVec2Dot
    ( CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pV1 || !pV2)
        return 0.0f;
#endif

    return pV1->x * pV2->x + pV1->y * pV2->y;
}

D3DXINLINE FLOAT D3DXVec2CCW
    ( CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pV1 || !pV2)
        return 0.0f;
#endif

    return pV1->x * pV2->y - pV1->y * pV2->x;
}

D3DXINLINE D3DXVECTOR2* D3DXVec2Add
    ( D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + pV2->x;
    pOut->y = pV1->y + pV2->y;
    return pOut;
}

D3DXINLINE D3DXVECTOR2* D3DXVec2Subtract
    ( D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    return pOut;
}

D3DXINLINE D3DXVECTOR2* D3DXVec2Minimize
    ( D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x < pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y < pV2->y ? pV1->y : pV2->y;
    return pOut;
}

D3DXINLINE D3DXVECTOR2* D3DXVec2Maximize
    ( D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x > pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y > pV2->y ? pV1->y : pV2->y;
    return pOut;
}

D3DXINLINE D3DXVECTOR2* D3DXVec2Scale
    ( D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV, FLOAT s )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV)
        return NULL;
#endif

    pOut->x = pV->x * s;
    pOut->y = pV->y * s;
    return pOut;
}

D3DXINLINE D3DXVECTOR2* D3DXVec2Lerp
    ( D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2,
      FLOAT s )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    return pOut;
}


//--------------------------
// 3D Vector
//--------------------------

D3DXINLINE FLOAT D3DXVec3Length
    ( CONST D3DXVECTOR3 *pV )
{
#ifdef D3DX_DEBUG
    if(!pV)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

#ifdef __cplusplus
    return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
#else
    return (FLOAT) sqrt(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
#endif

#else // !_NO_XBOXMATH

    XMVECTOR V, Result;

    V = D3DX_LoadFloat3((CONST XMFLOAT3*)pV);
    Result = XMVector3Length(V);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXVec3LengthSq
    ( CONST D3DXVECTOR3 *pV )
{
#ifdef D3DX_DEBUG
    if(!pV)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z;

#else // !_NO_XBOXMATH

    XMVECTOR V, Result;

    V = D3DX_LoadFloat3((CONST XMFLOAT3*)pV);
    Result = XMVector3LengthSq(V);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXVec3Dot
    ( CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pV1 || !pV2)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV1);
    V2 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV2);
    Result = XMVector3Dot(V1, V2);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE D3DXVECTOR3* D3DXVec3Cross
    ( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    D3DXVECTOR3 v;

    v.x = pV1->y * pV2->z - pV1->z * pV2->y;
    v.y = pV1->z * pV2->x - pV1->x * pV2->z;
    v.z = pV1->x * pV2->y - pV1->y * pV2->x;

    *pOut = v;

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV1);
    V2 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV2);
    Result = XMVector3Cross(V1, V2);
    XMStoreFloat3((XMFLOAT3*)pOut, Result);

#endif // !_NO_XBOXMATH

    return pOut;
}

D3DXINLINE D3DXVECTOR3* D3DXVec3Add
    ( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + pV2->x;
    pOut->y = pV1->y + pV2->y;
    pOut->z = pV1->z + pV2->z;
    return pOut;
}

D3DXINLINE D3DXVECTOR3* D3DXVec3Subtract
    ( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    pOut->z = pV1->z - pV2->z;
    return pOut;
}

D3DXINLINE D3DXVECTOR3* D3DXVec3Minimize
    ( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->x = pV1->x < pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y < pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z < pV2->z ? pV1->z : pV2->z;

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV1);
    V2 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV2);
    Result = XMVectorMinimize(V1, V2);
    XMStoreFloat3((XMFLOAT3*)pOut, Result);

#endif // !_NO_XBOXMATH

    return pOut;
}

D3DXINLINE D3DXVECTOR3* D3DXVec3Maximize
    ( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->x = pV1->x > pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y > pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z > pV2->z ? pV1->z : pV2->z;

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV1);
    V2 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV2);
    Result = XMVectorMaximize(V1, V2);
    XMStoreFloat3((XMFLOAT3*)pOut, Result);

#endif // !_NO_XBOXMATH

    return pOut;
}

D3DXINLINE D3DXVECTOR3* D3DXVec3Scale
    ( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, FLOAT s)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV)
        return NULL;
#endif

    pOut->x = pV->x * s;
    pOut->y = pV->y * s;
    pOut->z = pV->z * s;
    return pOut;
}

D3DXINLINE D3DXVECTOR3* D3DXVec3Lerp
    ( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2,
      FLOAT s )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    pOut->z = pV1->z + s * (pV2->z - pV1->z);

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV1);
    V2 = D3DX_LoadFloat3((CONST XMFLOAT3*)pV2);
    Result = D3DX_VectorLerp(V1, V2, s);
    XMStoreFloat3((XMFLOAT3*)pOut, Result);

#endif // !_NO_XBOXMATH

    return pOut;
}


//--------------------------
// 4D Vector
//--------------------------

D3DXINLINE FLOAT D3DXVec4Length
    ( CONST D3DXVECTOR4 *pV )
{
#ifdef D3DX_DEBUG
    if(!pV)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

#ifdef __cplusplus
    return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w);
#else
    return (FLOAT) sqrt(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w);
#endif

#else // !_NO_XBOXMATH

    XMVECTOR V, Result;

    V = D3DX_LoadFloat4((CONST XMFLOAT4*)pV);
    Result = XMVector4Length(V);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXVec4LengthSq
    ( CONST D3DXVECTOR4 *pV )
{
#ifdef D3DX_DEBUG
    if(!pV)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z + pV->w * pV->w;

#else // !_NO_XBOXMATH

    XMVECTOR V, Result;

    V = D3DX_LoadFloat4((CONST XMFLOAT4*)pV);
    Result = XMVector4LengthSq(V);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXVec4Dot
    ( CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2 )
{
#ifdef D3DX_DEBUG
    if(!pV1 || !pV2)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z + pV1->w * pV2->w;

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat4((CONST XMFLOAT4*)pV1);
    V2 = D3DX_LoadFloat4((CONST XMFLOAT4*)pV2);
    Result = XMVector4Dot(V1, V2);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE D3DXVECTOR4* D3DXVec4Add
    ( D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x + pV2->x;
    pOut->y = pV1->y + pV2->y;
    pOut->z = pV1->z + pV2->z;
    pOut->w = pV1->w + pV2->w;
    return pOut;
}

D3DXINLINE D3DXVECTOR4* D3DXVec4Subtract
    ( D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

    pOut->x = pV1->x - pV2->x;
    pOut->y = pV1->y - pV2->y;
    pOut->z = pV1->z - pV2->z;
    pOut->w = pV1->w - pV2->w;
    return pOut;
}

D3DXINLINE D3DXVECTOR4* D3DXVec4Minimize
    ( D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->x = pV1->x < pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y < pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z < pV2->z ? pV1->z : pV2->z;
    pOut->w = pV1->w < pV2->w ? pV1->w : pV2->w;

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat4((CONST XMFLOAT4*)pV1);
    V2 = D3DX_LoadFloat4((CONST XMFLOAT4*)pV2);
    Result = XMVectorMinimize(V1, V2);
    XMStoreFloat4NC((XMFLOAT4*)pOut, Result);

#endif // !_NO_XBOXMATH

    return pOut;
}

D3DXINLINE D3DXVECTOR4* D3DXVec4Maximize
    ( D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->x = pV1->x > pV2->x ? pV1->x : pV2->x;
    pOut->y = pV1->y > pV2->y ? pV1->y : pV2->y;
    pOut->z = pV1->z > pV2->z ? pV1->z : pV2->z;
    pOut->w = pV1->w > pV2->w ? pV1->w : pV2->w;

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat4((CONST XMFLOAT4*)pV1);
    V2 = D3DX_LoadFloat4((CONST XMFLOAT4*)pV2);
    Result = XMVectorMaximize(V1, V2);
    XMStoreFloat4NC((XMFLOAT4*)pOut, Result);

#endif // !_NO_XBOXMATH

    return pOut;
}

D3DXINLINE D3DXVECTOR4* D3DXVec4Scale
    ( D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV, FLOAT s)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV)
        return NULL;
#endif

    pOut->x = pV->x * s;
    pOut->y = pV->y * s;
    pOut->z = pV->z * s;
    pOut->w = pV->w * s;
    return pOut;
}

D3DXINLINE D3DXVECTOR4* D3DXVec4Lerp
    ( D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2,
      FLOAT s )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pV1 || !pV2)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    pOut->z = pV1->z + s * (pV2->z - pV1->z);
    pOut->w = pV1->w + s * (pV2->w - pV1->w);

#else // !_NO_XBOXMATH

    XMVECTOR V1, V2, Result;

    V1 = D3DX_LoadFloat4((CONST XMFLOAT4*)pV1);
    V2 = D3DX_LoadFloat4((CONST XMFLOAT4*)pV2);
    Result = D3DX_VectorLerp(V1, V2, s);
    XMStoreFloat4NC((XMFLOAT4*)pOut, Result);

#endif // !_NO_XBOXMATH

    return pOut;
}


//--------------------------
// 4D Matrix
//--------------------------

D3DXINLINE D3DXMATRIX* D3DXMatrixIdentity
    ( D3DXMATRIX *pOut )
{
#ifdef D3DX_DEBUG
    if(!pOut)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->m[0][1] = pOut->m[0][2] = pOut->m[0][3] =
    pOut->m[1][0] = pOut->m[1][2] = pOut->m[1][3] =
    pOut->m[2][0] = pOut->m[2][1] = pOut->m[2][3] =
    pOut->m[3][0] = pOut->m[3][1] = pOut->m[3][2] = 0.0f;

    pOut->m[0][0] = pOut->m[1][1] = pOut->m[2][2] = pOut->m[3][3] = 1.0f;

#else // !_NO_XBOXMATH

    XMMATRIX M;
    M = XMMatrixIdentity();
    XMStoreFloat4x4NC((XMFLOAT4X4*)pOut, M);

#endif // !_NO_XBOXMATH

    return pOut;
}


D3DXINLINE BOOL D3DXMatrixIsIdentity
    ( CONST D3DXMATRIX *pM )
{
#ifdef D3DX_DEBUG
    if(!pM)
        return FALSE;
#endif

#ifdef _NO_XBOXMATH

    return pM->m[0][0] == 1.0f && pM->m[0][1] == 0.0f && pM->m[0][2] == 0.0f && pM->m[0][3] == 0.0f &&
           pM->m[1][0] == 0.0f && pM->m[1][1] == 1.0f && pM->m[1][2] == 0.0f && pM->m[1][3] == 0.0f &&
           pM->m[2][0] == 0.0f && pM->m[2][1] == 0.0f && pM->m[2][2] == 1.0f && pM->m[2][3] == 0.0f &&
           pM->m[3][0] == 0.0f && pM->m[3][1] == 0.0f && pM->m[3][2] == 0.0f && pM->m[3][3] == 1.0f;

#else // !_NO_XBOXMATH

    XMMATRIX M;
    M = D3DX_LoadFloat4x4((CONST XMFLOAT4X4*)pM);
    return XMMatrixIsIdentity(M);

#endif // !_NO_XBOXMATH
}


//--------------------------
// Quaternion
//--------------------------

D3DXINLINE FLOAT D3DXQuaternionLength
    ( CONST D3DXQUATERNION *pQ )
{
#ifdef D3DX_DEBUG
    if(!pQ)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

#ifdef __cplusplus
    return sqrtf(pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z + pQ->w * pQ->w);
#else
    return (FLOAT) sqrt(pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z + pQ->w * pQ->w);
#endif

#else // !_NO_XBOXMATH

    XMVECTOR Q, Result;

    Q = D3DX_LoadFloat4((CONST XMFLOAT4*)pQ);
    Result = XMQuaternionLength(Q);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXQuaternionLengthSq
    ( CONST D3DXQUATERNION *pQ )
{
#ifdef D3DX_DEBUG
    if(!pQ)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pQ->x * pQ->x + pQ->y * pQ->y + pQ->z * pQ->z + pQ->w * pQ->w;

#else // !_NO_XBOXMATH

    XMVECTOR Q, Result;

    Q = D3DX_LoadFloat4((CONST XMFLOAT4*)pQ);
    Result = XMQuaternionLengthSq(Q);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXQuaternionDot
    ( CONST D3DXQUATERNION *pQ1, CONST D3DXQUATERNION *pQ2 )
{
#ifdef D3DX_DEBUG
    if(!pQ1 || !pQ2)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pQ1->x * pQ2->x + pQ1->y * pQ2->y + pQ1->z * pQ2->z + pQ1->w * pQ2->w;

#else // !_NO_XBOXMATH

    XMVECTOR Q1, Q2, Result;

    Q1 = D3DX_LoadFloat4((CONST XMFLOAT4*)pQ1);
    Q2 = D3DX_LoadFloat4((CONST XMFLOAT4*)pQ2);
    Result = XMQuaternionDot(Q1, Q2);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}


D3DXINLINE D3DXQUATERNION* D3DXQuaternionIdentity
    ( D3DXQUATERNION *pOut )
{
#ifdef D3DX_DEBUG
    if(!pOut)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->x = pOut->y = pOut->z = 0.0f;
    pOut->w = 1.0f;

#else // !_NO_XBOXMATH

    XMVECTOR Q;
    Q = XMQuaternionIdentity();
    XMStoreFloat4NC((XMFLOAT4*)pOut, Q);

#endif // !_NO_XBOXMATH

    return pOut;
}

D3DXINLINE BOOL D3DXQuaternionIsIdentity
    ( CONST D3DXQUATERNION *pQ )
{
#ifdef D3DX_DEBUG
    if(!pQ)
        return FALSE;
#endif

#ifdef _NO_XBOXMATH

    return pQ->x == 0.0f && pQ->y == 0.0f && pQ->z == 0.0f && pQ->w == 1.0f;

#else // !_NO_XBOXMATH

    XMVECTOR Q;
    Q = D3DX_LoadFloat4((CONST XMFLOAT4*)pQ);
    return XMQuaternionIsIdentity(Q);

#endif // !_NO_XBOXMATH
}


D3DXINLINE D3DXQUATERNION* D3DXQuaternionConjugate
    ( D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ )
{
#ifdef D3DX_DEBUG
    if(!pOut || !pQ)
        return NULL;
#endif

    pOut->x = -pQ->x;
    pOut->y = -pQ->y;
    pOut->z = -pQ->z;
    pOut->w =  pQ->w;
    return pOut;
}


//--------------------------
// Plane
//--------------------------

D3DXINLINE FLOAT D3DXPlaneDot
    ( CONST D3DXPLANE *pP, CONST D3DXVECTOR4 *pV)
{
#ifdef D3DX_DEBUG
    if(!pP || !pV)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d * pV->w;

#else // !_NO_XBOXMATH

    XMVECTOR P, V, Result;

    P = D3DX_LoadFloat4((CONST XMFLOAT4*)pP);
    V = D3DX_LoadFloat4((CONST XMFLOAT4*)pV);
    Result = XMPlaneDot(P, V);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXPlaneDotCoord
    ( CONST D3DXPLANE *pP, CONST D3DXVECTOR3 *pV)
{
#ifdef D3DX_DEBUG
    if(!pP || !pV)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;

#else // !_NO_XBOXMATH

    XMVECTOR P, V, Result;

    P = D3DX_LoadFloat4((CONST XMFLOAT4*)pP);
    V = D3DX_LoadFloat3((CONST XMFLOAT3*)pV);
    Result = XMPlaneDotCoord(P, V);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE FLOAT D3DXPlaneDotNormal
    ( CONST D3DXPLANE *pP, CONST D3DXVECTOR3 *pV)
{
#ifdef D3DX_DEBUG
    if(!pP || !pV)
        return 0.0f;
#endif

#ifdef _NO_XBOXMATH

    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z;

#else // !_NO_XBOXMATH

    XMVECTOR P, V, Result;

    P = D3DX_LoadFloat4((CONST XMFLOAT4*)pP);
    V = D3DX_LoadFloat3((CONST XMFLOAT3*)pV);
    Result = XMPlaneDotNormal(P, V);

    return Result.v[0];

#endif // !_NO_XBOXMATH
}

D3DXINLINE D3DXPLANE* D3DXPlaneScale
    (D3DXPLANE *pOut, CONST D3DXPLANE *pP, FLOAT s)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pP)
        return NULL;
#endif

    pOut->a = pP->a * s;
    pOut->b = pP->b * s;
    pOut->c = pP->c * s;
    pOut->d = pP->d * s;
    return pOut;
}


//--------------------------
// Color
//--------------------------

D3DXINLINE D3DXCOLOR* D3DXColorNegative
    (D3DXCOLOR *pOut, CONST D3DXCOLOR *pC)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pC)
        return NULL;
#endif

    pOut->r = 1.0f - pC->r;
    pOut->g = 1.0f - pC->g;
    pOut->b = 1.0f - pC->b;
    pOut->a = pC->a;
    return pOut;
}

D3DXINLINE D3DXCOLOR* D3DXColorAdd
    (D3DXCOLOR *pOut, CONST D3DXCOLOR *pC1, CONST D3DXCOLOR *pC2)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pC1 || !pC2)
        return NULL;
#endif

    pOut->r = pC1->r + pC2->r;
    pOut->g = pC1->g + pC2->g;
    pOut->b = pC1->b + pC2->b;
    pOut->a = pC1->a + pC2->a;
    return pOut;
}

D3DXINLINE D3DXCOLOR* D3DXColorSubtract
    (D3DXCOLOR *pOut, CONST D3DXCOLOR *pC1, CONST D3DXCOLOR *pC2)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pC1 || !pC2)
        return NULL;
#endif

    pOut->r = pC1->r - pC2->r;
    pOut->g = pC1->g - pC2->g;
    pOut->b = pC1->b - pC2->b;
    pOut->a = pC1->a - pC2->a;
    return pOut;
}

D3DXINLINE D3DXCOLOR* D3DXColorScale
    (D3DXCOLOR *pOut, CONST D3DXCOLOR *pC, FLOAT s)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pC)
        return NULL;
#endif

    pOut->r = pC->r * s;
    pOut->g = pC->g * s;
    pOut->b = pC->b * s;
    pOut->a = pC->a * s;
    return pOut;
}

D3DXINLINE D3DXCOLOR* D3DXColorModulate
    (D3DXCOLOR *pOut, CONST D3DXCOLOR *pC1, CONST D3DXCOLOR *pC2)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pC1 || !pC2)
        return NULL;
#endif

    pOut->r = pC1->r * pC2->r;
    pOut->g = pC1->g * pC2->g;
    pOut->b = pC1->b * pC2->b;
    pOut->a = pC1->a * pC2->a;
    return pOut;
}

D3DXINLINE D3DXCOLOR* D3DXColorLerp
    (D3DXCOLOR *pOut, CONST D3DXCOLOR *pC1, CONST D3DXCOLOR *pC2, FLOAT s)
{
#ifdef D3DX_DEBUG
    if(!pOut || !pC1 || !pC2)
        return NULL;
#endif

#ifdef _NO_XBOXMATH

    pOut->r = pC1->r + s * (pC2->r - pC1->r);
    pOut->g = pC1->g + s * (pC2->g - pC1->g);
    pOut->b = pC1->b + s * (pC2->b - pC1->b);
    pOut->a = pC1->a + s * (pC2->a - pC1->a);

#else // !_NO_XBOXMATH

    XMVECTOR C1, C2, Result;

    C1 = D3DX_LoadFloat4((CONST XMFLOAT4*)pC1);
    C2 = D3DX_LoadFloat4((CONST XMFLOAT4*)pC2);
    Result = D3DX_VectorLerp(C1, C2, s);
    XMStoreFloat4NC((XMFLOAT4*)pOut, Result);

#endif // !_NO_XBOXMATH

    return pOut;
}


#endif // __D3DX9MATH_INL__
