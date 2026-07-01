#ifndef POINT_HPP_
#define POINT_HPP_

#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x2C
// Decl: speed/indep/src/misc/Point.hpp:33
struct tCubic1D {
    float Val;         // offset 0x0, size 0x4, Decl: speed/indep/src/misc/Point.hpp:34
    float dVal;        // offset 0x4, size 0x4, Decl: speed/indep/src/misc/Point.hpp:35
    float ValDesired;  // offset 0x8, size 0x4, Decl: speed/indep/src/misc/Point.hpp:37
    float dValDesired; // offset 0xC, size 0x4, Decl: speed/indep/src/misc/Point.hpp:38
    float Coeff[4];    // offset 0x10, size 0x10, Decl: speed/indep/src/misc/Point.hpp:40
    float time;        // offset 0x20, size 0x4, Decl: speed/indep/src/misc/Point.hpp:41
    float duration;    // offset 0x24, size 0x4, Decl: speed/indep/src/misc/Point.hpp:42
    short int state;   // offset 0x28, size 0x2, Decl: speed/indep/src/misc/Point.hpp:44
    short int flags;   // offset 0x2A, size 0x2, Decl: speed/indep/src/misc/Point.hpp:45

    tCubic1D(short type, float dur)
        : Val(0.0f),         //
          dVal(0.0f),        //
          ValDesired(0.0f),  //
          dValDesired(0.0f), //
          time(0.0f),        //
          duration(dur),     //
          state(type),       //
          flags(1) {
        Coeff[0] = 0.0f;
        Coeff[1] = 0.0f;
        Coeff[2] = 0.0f;
        Coeff[3] = 0.0f;
    }

    void Update(float fSeconds, float fDClamp, float fDDClamp); // Decl: speed/indep/src/misc/Point.hpp:63
    void Snap() {                                               // Decl: speed/indep/src/misc/Point.hpp:64
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }
    void SetVal(const float v) { // Decl: speed/indep/src/misc/Point.hpp:66
        Val = v;
        if (v != ValDesired) {
            state = 2;
        }
    }
    void SetdVal(float v) { // Decl: speed/indep/src/misc/Point.hpp:67
        dVal = v;
        if (v != dValDesired) {
            state = 2;
        }
    }
    void SetValDesired(float v) { // Decl: speed/indep/src/misc/Point.hpp:68
        ValDesired = v;
        if (v != Val) {
            state = 2;
        }
    }
    void SetdValDesired(float v) { // Decl: speed/indep/src/misc/Point.hpp:69
        dValDesired = v;
    }

    void SetDuration(const float t) { // Decl: speed/indep/src/misc/Point.hpp:71
        duration = t;
    }
    void SetState(short s) { // Decl: speed/indep/src/misc/Point.hpp:72
        state = s;
    }
    void SetFlags(short f) { // Decl: speed/indep/src/misc/Point.hpp:73
        flags = f;
    }

    float GetVal(float t);   // Decl: speed/indep/src/misc/Point.hpp:75
    float GetdVal(float t);  // Decl: speed/indep/src/misc/Point.hpp:76
    float GetddVal(float t); // Decl: speed/indep/src/misc/Point.hpp:77
    float GetVal();          // Decl: speed/indep/src/misc/Point.hpp:78
    float GetdVal();         // Decl: speed/indep/src/misc/Point.hpp:79
    float GetddVal();        // Decl: speed/indep/src/misc/Point.hpp:80

    float GetValDesired();  // Decl: speed/indep/src/misc/Point.hpp:82
    float GetdValDesired(); // Decl: speed/indep/src/misc/Point.hpp:83

    float GetDerivative(float t);       // Decl: speed/indep/src/misc/Point.hpp:85
    float GetSecondDerivative(float t); // Decl: speed/indep/src/misc/Point.hpp:86

    void ClampDerivative(float fMag);       // Decl: speed/indep/src/misc/Point.hpp:88
    void ClampSecondDerivative(float fMag); // Decl: speed/indep/src/misc/Point.hpp:89

    void MakeCoeffs();             // Decl: speed/indep/src/misc/Point.hpp:91
    int HasArrived();              // Decl: speed/indep/src/misc/Point.hpp:92
    void PathdValDesired(float v); // Decl: speed/indep/src/misc/Point.hpp:93
};

// total size: 0x58
// Decl: speed/indep/src/misc/Point.hpp:98
struct tCubic2D {
    tCubic1D x; // offset 0x0, size 0x2C, Decl: speed/indep/src/misc/Point.hpp:109
    tCubic1D y; // offset 0x2C, size 0x2C, Decl: speed/indep/src/misc/Point.hpp:110

    tCubic2D(short type, float dur) : x(type, dur), y(type, dur) {}
    tCubic2D(short type, bVector2 *pDuration);

    void Update(float fSeconds, float fDClamp, float fDDClamp); // Decl: speed/indep/src/misc/Point.hpp:112
    int HasArrived();                                           // Decl: speed/indep/src/misc/Point.hpp:113
    void Snap() {                                               // Decl: speed/indep/src/misc/Point.hpp:114
        x.Snap();
        y.Snap();
    }
    void SetVal(const float vx, const float vy) { // Decl: speed/indep/src/misc/Point.hpp:121
        x.SetVal(vx);
        y.SetVal(vy);
    }
    void SetdVal(float vx, float vy) { // Decl: speed/indep/src/misc/Point.hpp:117
        x.SetdVal(vx);
        y.SetdVal(vy);
    }
    void SetValDesired(float vx, float vy) { // Decl: speed/indep/src/misc/Point.hpp:118
        x.SetValDesired(vx);
        y.SetValDesired(vy);
    }
    void SetdValDesired(float vx, float vy) { // Decl: speed/indep/src/misc/Point.hpp:119
        x.SetdValDesired(vx);
        y.SetdValDesired(vy);
    }

    void SetVal(const bVector2 *pV);   // Decl: speed/indep/src/misc/Point.hpp:121
    void SetdVal(bVector2 *pV);        // Decl: speed/indep/src/misc/Point.hpp:122
    void SetValDesired(bVector2 *pV);  // Decl: speed/indep/src/misc/Point.hpp:123
    void SetdValDesired(bVector2 *pV); // Decl: speed/indep/src/misc/Point.hpp:124

    void GetVal(bVector2 *pV);            // Decl: speed/indep/src/misc/Point.hpp:126
    void GetdVal(bVector2 *pV);           // Decl: speed/indep/src/misc/Point.hpp:127
    void GetddVal(bVector2 *pV);          // Decl: speed/indep/src/misc/Point.hpp:128
    void GetVal(bVector2 *pV, float t);   // Decl: speed/indep/src/misc/Point.hpp:129
    void GetdVal(bVector2 *pV, float t);  // Decl: speed/indep/src/misc/Point.hpp:130
    void GetddVal(bVector2 *pV, float t); // Decl: speed/indep/src/misc/Point.hpp:131

    void GetValDesired(bVector2 *pV);  // Decl: speed/indep/src/misc/Point.hpp:133
    void GetdValDesired(bVector2 *pV); // Decl: speed/indep/src/misc/Point.hpp:134

    void SetDuration(const bVector2 *pV);
    void SetDuration(const float t) { // Decl: speed/indep/src/misc/Point.hpp:138
        x.SetDuration(t);
        y.SetDuration(t);
    }
    void SetDuration(const float tx, const float ty);
    void SetState(short s) { // Decl: speed/indep/src/misc/Point.hpp:140
        x.SetState(s);
        y.SetState(s);
    }
    void SetFlags(short s) { // Decl: speed/indep/src/misc/Point.hpp:141
        x.SetFlags(s);
        y.SetFlags(s);
    }
    void PathdValDesired(float x2, float y2); // Decl: speed/indep/src/misc/Point.hpp:143
    void PathdValDesired(bVector2 *v);        // Decl: speed/indep/src/misc/Point.hpp:144

    void MakeCoeffs(); // Decl: speed/indep/src/misc/Point.hpp:146
};

// total size: 0x84
struct tCubic3D {
    tCubic1D x; // offset 0x0, size 0x2C, Decl: speed/indep/src/misc/Point.hpp:164
    tCubic1D y; // offset 0x2C, size 0x2C, Decl: speed/indep/src/misc/Point.hpp:165
    tCubic1D z; // offset 0x58, size 0x2C, Decl: speed/indep/src/misc/Point.hpp:166

    void Update(float dt, float maxDeriv, float maxSecondDeriv);
    int HasArrived() {} // Decl: speed/indep/src/misc/Point.hpp:169
    void Snap() {}      // Decl: speed/indep/src/misc/Point.hpp:170

    void SetVal(const float vx, const float vy, const float vz) {
        x.SetVal(vx);
        y.SetVal(vy);
        z.SetVal(vz);
    }
    void SetdVal(const float vx, const float vy, const float vz) { // Decl: speed/indep/src/misc/Point.hpp:173
        x.SetdVal(vx);
        y.SetdVal(vy);
        z.SetdVal(vz);
    }
    void SetValDesired(const float vx, const float vy, const float vz) { // Decl: speed/indep/src/misc/Point.hpp:174
        x.SetValDesired(vx);
        y.SetValDesired(vy);
        z.SetValDesired(vz);
    }

    void SetVal(const bVector3 *v);              // Decl: speed/indep/src/misc/Point.hpp:177
    void SetdVal(bVector3 *v);                   // Decl: speed/indep/src/misc/Point.hpp:178
    void SetValDesired(bVector3 *v);             // Decl: speed/indep/src/misc/Point.hpp:179
    void SetdValDesired(bVector3 *pV);           // Decl: speed/indep/src/misc/Point.hpp:180
    void GetVal(bVector3 *v);                    // Decl: speed/indep/src/misc/Point.hpp:182
    void GetdVal(struct bVector3 *pV);           // Decl: speed/indep/src/misc/Point.hpp:183
    void GetddVal(struct bVector3 *pV);          // Decl: speed/indep/src/misc/Point.hpp:184
    void GetVal(struct bVector3 *pV, float t);   // Decl: speed/indep/src/misc/Point.hpp:186
    void GetdVal(struct bVector3 *pV, float t);  // Decl: speed/indep/src/misc/Point.hpp:187
    void GetddVal(struct bVector3 *pV, float t); // Decl: speed/indep/src/misc/Point.hpp:188

    void GetValDesired(bVector3 *pV);  // Decl: speed/indep/src/misc/Point.hpp:190
    void GetdValDesired(bVector3 *pV); // Decl: speed/indep/src/misc/Point.hpp:191

    void SetDuration(const bVector3 *pV); // Decl: speed/indep/src/misc/Point.hpp:195

    void SetState(short s) {} // Decl: speed/indep/src/misc/Point.hpp:197
    void SetFlags(short s) {} // Decl: speed/indep/src/misc/Point.hpp:198

    void PathdValDesired(float x2, float y2, float z2) {} // Decl: speed/indep/src/misc/Point.hpp:200
    void PathdValDesired(bVector3 *v) {}                  // Decl: speed/indep/src/misc/Point.hpp:201

    void MakeCoeffs() {} // Decl: speed/indep/src/misc/Point.hpp:203
};

// total size: 0x178
// Decl: speed/indep/src/misc/Point.hpp:208
class cPoint {
  public:
    tCubic3D mPos;                        // offset 0x0, size 0x84
    tCubic3D mRot;                        // offset 0x84, size 0x84
    tCubic1D mScale;                      // offset 0x108, size 0x2C
    tCubic1D mYScale;                     // offset 0x134, size 0x2C
    float mZBias;                         // offset 0x160, size 0x4
    void (*mCallBackFunc)(int, int);      // offset 0x164, size 0x4
    void (*mUpdateFunc)(cPoint *, float); // offset 0x168, size 0x4
    int32 mCBParam1;                      // offset 0x16C, size 0x4
    int32 mCBParam2;                      // offset 0x170, size 0x4

    cPoint();
    virtual ~cPoint();

    virtual void Update(float time);
    int HasArrived() {} // Decl: speed/indep/src/misc/Point.hpp:215

    void SetCallBack(void (*func)(int32, int32), int32 p1, int32 p2) {} // Decl: speed/indep/src/misc/Point.hpp:217
    void ClearCallBack() {}                                             // Decl: speed/indep/src/misc/Point.hpp:218
    void CallBack() {}                                                  // Decl: speed/indep/src/misc/Point.hpp:219

    static void SplineSeek(tCubic1D *p, float time, float fDClamp, float fDDClamp);

    static void SplineSeek(tCubic2D *p, float time);

    static void SplineSeek(tCubic3D *p, float time);

    static void SpringDamper(tCubic1D *p, float time);

    static void SpringDamper(tCubic2D *p, float time);

    static void SpringDamper(tCubic3D *p, float time);

    void BuildMatrix(bMatrix4 *pMatrix);

    static void PointVelocity(cPoint *p, float time);

    static void PointVelocityWrap(cPoint *p, float time);

    static void PointSpline(cPoint *p, float time);

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

    void SetScaleDesired(float s);

    void SetdScale(float s);

    void SetdScaleDesired(float s);

    void SetYScale(float s);

    void SetYScaleDesired(float s);

    void SetdYScale(float s);

    void SetdYScaleDesired(float s);

    void SetDuration(float seconds);

    void Snap();

    void SetBiasZ(float ratio);
};

#endif
