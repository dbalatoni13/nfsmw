#ifndef MISC_POINT_H
#define MISC_POINT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x2C
struct tCubic1D {
    float Val;         // offset 0x0, size 0x4
    float dVal;        // offset 0x4, size 0x4
    float ValDesired;  // offset 0x8, size 0x4
    float dValDesired; // offset 0xC, size 0x4
    float Coeff[4];    // offset 0x10, size 0x10
    float time;        // offset 0x20, size 0x4
    float duration;    // offset 0x24, size 0x4
    short state;       // offset 0x28, size 0x2
    short flags;       // offset 0x2A, size 0x2

    tCubic1D(short type, float dur)
        : Val(0.0f) //
          ,
          dVal(0.0f) //
          ,
          ValDesired(0.0f) //
          ,
          dValDesired(0.0f) //
          ,
          time(0.0f) //
          ,
          duration(dur) //
          ,
          state(type) //
          ,
          flags(1) {
        Coeff[0] = 0.0f;
        Coeff[1] = 0.0f;
        Coeff[2] = 0.0f;
        Coeff[3] = 0.0f;
    }

    void Snap() {
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }
    void SetVal(const float v) {
        Val = v;
        state = 2;
    }
    void SetdVal(float v) {
        dVal = v;
        state = 2;
    }
    void SetValDesired(float v) {
        ValDesired = v;
        state = 2;
    }
    void SetdValDesired(float v) {
        dValDesired = v;
    }
    void SetDuration(const float t) {
        duration = t;
    }
    void SetState(short s) {
        state = s;
    }
    void SetFlags(short f) {
        flags = f;
    }
    float GetVal();
    float GetdVal();
    float GetddVal();
    int HasArrived();
    void PathdValDesired(float v);
    void MakeCoeffs();
    float GetVal(float t);
    float GetdVal(float t);
    float GetddVal(float t);
    float GetValDesired();
    float GetdValDesired();
    float GetDerivative(float t);
    float GetSecondDerivative(float t);
    void ClampDerivative(float fMag);
    void ClampSecondDerivative(float fMag);
    void Update(float fSeconds, float fDClamp, float fDDClamp);
};

// total size: 0x58
struct tCubic2D {
    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C

    tCubic2D(short type, float dur) : x(type, dur), y(type, dur) {}
    tCubic2D(short type, bVector2 *pDuration);

    int HasArrived();
    void Snap() {
        x.Snap();
        y.Snap();
    }
    void SetVal(const float vx, const float vy) {
        x.SetVal(vx);
        y.SetVal(vy);
    }
    void SetdVal(float vx, float vy) {
        x.SetdVal(vx);
        y.SetdVal(vy);
    }
    void SetValDesired(float vx, float vy) {
        x.SetValDesired(vx);
        y.SetValDesired(vy);
    }
    void SetdValDesired(float vx, float vy) {
        x.SetdValDesired(vx);
        y.SetdValDesired(vy);
    }
    void SetDuration(const float t) {
        x.SetDuration(t);
        y.SetDuration(t);
    }
    void SetDuration(const float tx, const float ty);
    void SetState(short s) {
        x.SetState(s);
        y.SetState(s);
    }
    void SetFlags(short s) {
        x.SetFlags(s);
        y.SetFlags(s);
    }
    void PathdValDesired(float x2, float y2);
    void PathdValDesired(bVector2 *v);
    void MakeCoeffs();

    void SetVal(const bVector2 *pV);
    void SetdVal(bVector2 *pV);
    void SetValDesired(bVector2 *pV);
    void SetdValDesired(bVector2 *pV);
    void SetDuration(const bVector2 *pV);
    void GetVal(bVector2 *pV);
    void GetdVal(bVector2 *pV);
    void GetddVal(bVector2 *pV);
    void GetVal(bVector2 *pV, float t);
    void GetdVal(bVector2 *pV, float t);
    void GetddVal(bVector2 *pV, float t);
    void GetValDesired(bVector2 *pV);
    void GetdValDesired(bVector2 *pV);
    void Update(float fSeconds, float fDClamp, float fDDClamp);
};

// total size: 0x84
struct tCubic3D {
    void SetVal(const bVector3 *v);
    void SetdVal(bVector3 *v);
    void SetValDesired(bVector3 *v);
    void GetVal(bVector3 *v);
    void GetValDesired(bVector3 *v);
    void Update(float dt, float maxDeriv, float maxSecondDeriv);

    void SetVal(const float vx, const float vy, const float vz) {
        x.SetVal(vx);
        y.SetVal(vy);
        z.SetVal(vz);
    }

    void SetdVal(const float vx, const float vy, const float vz) {
        x.SetdVal(vx);
        y.SetdVal(vy);
        z.SetdVal(vz);
    }

    void SetValDesired(const float vx, const float vy, const float vz) {
        x.SetValDesired(vx);
        y.SetValDesired(vy);
        z.SetValDesired(vz);
    }

    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C
    tCubic1D z; // offset 0x58, size 0x2C
};

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
    int32 mCBParam1;                      // offset 0x16C, size 0x4
    int32 mCBParam2;                      // offset 0x170, size 0x4
};

#endif
