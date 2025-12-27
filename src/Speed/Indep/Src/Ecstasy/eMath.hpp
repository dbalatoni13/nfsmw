#ifndef ECSTASY_EMATH_H
#define ECSTASY_EMATH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

void eMathInit(void);

void eCopyMatrix(bMatrix4 *dest, bMatrix4 *src);
void eMulMatrix(bMatrix4 *ab, bMatrix4 *a, bMatrix4 *b);
void eMulVector(bVector4 *vm, const bMatrix4 *m, const bVector4 *v);
void eMulVector(bVector3 *vm, const bMatrix4 *m, const bVector3 *v);
void eProject(float x, float y, float z, float (*mtx)[4], float *pm, float *vp, float *sx, float *sy, float *sz);
void eRotTransPers(bVector3 *dest, const bVector3 *src, bMatrix4 *wv, bMatrix4 *vs, float xOrig, float yOrig, float width, float height, float zNear,
                   float zFar);
void eCreateAxisRotationMatrix(bMatrix4 *dest, bVector3 &axis, unsigned short angle);
void eCreateLookAtMatrix(bMatrix4 *mat, bVector3 &eye, bVector3 &center, bVector3 &up);
float eSin(float a);
void eCreateRotationZ(bMatrix4 *dest, unsigned short angle);
void eRotateX(bMatrix4 *dest, bMatrix4 *a, unsigned short angle);
void eRotateY(bMatrix4 *dest, bMatrix4 *a, unsigned short angle);
void eRotateZ(bMatrix4 *dest, bMatrix4 *a, unsigned short angle);
void eTranslate(bMatrix4 *dest, bMatrix4 *a, bVector3 *tran);
void eCreateTranslationMatrix(bMatrix4 *dest, bVector3 &tran);
bMatrix4 *eInvertMatrix(bMatrix4 *dest, bMatrix4 *m);
bMatrix4 *eInvertTransformationMatrix(bMatrix4 *dest, const bMatrix4 *tm);
bMatrix4 *eInvertRotationMatrix(bMatrix4 *dest, bMatrix4 *src);
float ePowf(float x, float y);

extern bMatrix4 eMathIdentityMatrix; // size: 0x40
extern bMatrix4 eMathZeroMatrix;     // size: 0x40

inline struct bMatrix4 *eGetIdentityMatrix() {
    return &eMathIdentityMatrix;
}

inline struct bMatrix4 *eGetZeroMatrix() {
    return &eMathZeroMatrix;
}

#endif
