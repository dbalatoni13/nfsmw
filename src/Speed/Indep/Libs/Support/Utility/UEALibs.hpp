#ifndef SUPPORT_UTILITY_UEALIBS_H
#define SUPPORT_UTILITY_UEALIBS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "./UTypes.h"

float rasin(float a);
float rsqrt(const float x);
float v3length(const UMath::Vector3 *v);
void rsincos(const float *ang, float *s, float *c);
void v3crossprod(const UMath::Vector3 *p1, const UMath::Vector3 *p2, UMath::Vector3 *result);
void v3unit(const UMath::Vector3 *v, UMath::Vector3 *result);
void v3sub(int num, const UMath::Vector3 *src, const UMath::Vector3 *vtosub, UMath::Vector3 *results);
void v3add(int num, const UMath::Vector3 *src, const UMath::Vector3 *vtosub, UMath::Vector3 *results);
void MATRIX4_multxrot(const UMath::Matrix4 *m4, float xbangle, UMath::Matrix4 *resultm);
void MATRIX4_multyrot(const UMath::Matrix4 *m4, float ybangle, UMath::Matrix4 *resultm);
void MATRIX4_multzrot(const UMath::Matrix4 *m4, float zbangle, UMath::Matrix4 *resultm);


#endif
