#ifndef MISC_POINT_H
#define MISC_POINT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Spline.hpp"

// total size: 0x178
class cPoint {
  public:
    virtual void Update(float time);

    static void SplineSeek(tCubic1D *p, float time, float fDClamp, float fDDClamp);

    static void SplineSeek(tCubic2D *p, float time);

    static void SplineSeek(tCubic3D *p, float time);

    static void SpringDamper(tCubic1D *p, float time);

    static void SpringDamper(tCubic2D *p, float time);

    static void SpringDamper(tCubic3D *p, float time);

    void BuildMatrix(bMatrix4 *pMatrix);

    static void PointSpline(cPoint *p, float time);

    static void PointVelocity(cPoint *p, float time);

    static void PointVelocityWrap(cPoint *p, float time);

    static void PointSpringDamper(cPoint *p, float time);

    void SetPos(bVector3 *p);

    void SetPos(float x, float y, float z);

    void SetPosX(float x);

    void SetPosY(float y, bool ratio);

    void SetPosZ(float z);

    void SetPosDesired(bVector3 *p);

    void SetPosDesired(float x, float y, float z);

    void SetPosDesiredX(float x);

    void SetPosDesiredY(float y, bool ratio);

    void SetPosDesiredZ(float z);

    void SetdPos(bVector3 *p);

    void SetdPos(float x, float y, float z);

    void SetdPosX(float x);

    void SetdPosY(float y);

    void SetdPosZ(float z);

    void SetdPosDesired(bVector3 *p);

    void SetdPosDesired(float x, float y, float z);

    void SetdPosDesiredX(float x);

    void SetdPosDesiredY(float y);

    void SetdPosDesiredZ(float z);

    void SetRot(bVector3 *p);

    void SetRot(float x, float y, float z);

    void SetRotX(float x);

    void SetRotY(float y);

    void SetRotZ(float z);

    void SetRotDesired(bVector3 *p);

    void SetRotDesired(float x, float y, float z);

    void SetRotDesiredX(float x);

    void SetRotDesiredY(float y);

    void SetRotDesiredZ(float z);

    void SetdRot(bVector3 *p);

    void SetdRot(float x, float y, float z);

    void SetdRotX(float x);

    void SetdRotY(float y);

    void SetdRotZ(float z);

    void SetdRotDesired(bVector3 *p);

    void SetdRotDesired(float x, float y, float z);

    void SetdRotDesiredX(float x);

    void SetdRotDesiredY(float y);

    void SetdRotDesiredZ(float z);

    void SetScale(float s);

    void SetdScale(float s);

    void SetYScale(float s);

    void SetdYScale(float s);

    void SetScaleDesired(float s);

    void SetdScaleDesired(float s);

    void SetYScaleDesired(float s);

    void SetdYScaleDesired(float s);

    void SetDuration(float seconds);

    void Snap();

    void SetBiasZ(float ratio);

    cPoint();

    virtual ~cPoint();

    // int HasArrived() {}

    // void SetCallBack(void (*func)(int, int), int p1, int p2) {}

    // void ClearCallBack() {}

    // void CallBack() {}

    tCubic3D mPos;                        // offset 0x0, size 0x84
    tCubic3D mRot;                        // offset 0x84, size 0x84
    tCubic1D mScale;                      // offset 0x108, size 0x2C
    tCubic1D mYScale;                     // offset 0x134, size 0x2C
    float mZBias;                         // offset 0x160, size 0x4
    void (*mCallBackFunc)(int, int);      // offset 0x164, size 0x4
    void (*mUpdateFunc)(cPoint *, float); // offset 0x168, size 0x4
    int mCBParam1;                        // offset 0x16C, size 0x4
    int mCBParam2;                        // offset 0x170, size 0x4
};

#endif
