#ifndef SUPPORT_UTILITY_UVECTOR_MATH_H
#define SUPPORT_UTILITY_UVECTOR_MATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// Avoid circular dependency
namespace UMath {

struct Vector3;
struct Vector4;
struct Matrix4;

} // namespace UMath

void VU0_v3scale(const UMath::Vector3 &a, const float scaleby, UMath::Vector3 &result);
void VU0_v3scale(const UMath::Vector3 &a, const UMath::Vector3 &b, UMath::Vector3 &result);
float VU0_v3distancesquare(const UMath::Vector3 &p1, const UMath::Vector3 &p2);
float VU0_v3distancesquarexz(const UMath::Vector3 &p1, const UMath::Vector3 &p2);

void VU0_v4subxyz(const UMath::Vector4 &a, const UMath::Vector4 &b, UMath::Vector4 &result);
void VU0_v4scale(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &result);
float VU0_v4distancesquarexyz(const UMath::Vector4 &p1, const UMath::Vector4 &p2);

void VU0_MATRIX4_vect3mult(const UMath::Vector3 &v, const UMath::Matrix4 &m, UMath::Vector3 &result);
void VU0_MATRIX4_vect4mult(const UMath::Vector4 &v, const UMath::Matrix4 &m, UMath::Vector4 &result);

#endif
