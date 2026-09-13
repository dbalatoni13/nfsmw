#ifndef POINT_HPP_
#define POINT_HPP_

#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x2C
// Decl: 33
struct tCubic1D {
    float Val;         // offset 0x0, size 0x4, Decl: 34
    float dVal;        // offset 0x4, size 0x4, Decl: 35
    float ValDesired;  // offset 0x8, size 0x4, Decl: 37
    float dValDesired; // offset 0xC, size 0x4, Decl: 38
    float Coeff[4];    // offset 0x10, size 0x10, Decl: 40
    float time;        // offset 0x20, size 0x4, Decl: 41
    float duration;    // offset 0x24, size 0x4, Decl: 42
    short int state;   // offset 0x28, size 0x2, Decl: 44
    short int flags;   // offset 0x2A, size 0x2, Decl: 45

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

    void Update(float fSeconds, float fDClamp, float fDDClamp); // Decl: 63
    void Snap() {                                               // Decl: 64
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }
    void SetVal(const float v) { // Decl: 66
        Val = v;
        if (v != ValDesired) {
            state = 2;
        }
    }
    void SetdVal(float v) { // Decl: 67
        dVal = v;
        if (v != dValDesired) {
            state = 2;
        }
    }
    void SetValDesired(float v) { // Decl: 68
        ValDesired = v;
        if (v != Val) {
            state = 2;
        }
    }
    void SetdValDesired(float v) { // Decl: 69
        dValDesired = v;
    }

    void SetDuration(const float t) { // Decl: 71
        duration = t;
    }
    void SetState(short s) { // Decl: 72
        state = s;
    }
    void SetFlags(short f) { // Decl: 73
        flags = f;
    }

    float GetVal(float t);   // Decl: 75
    float GetdVal(float t);  // Decl: 76
    float GetddVal(float t); // Decl: 77
    float GetVal();          // Decl: 78
    float GetdVal();         // Decl: 79
    float GetddVal();        // Decl: 80

    float GetValDesired();  // Decl: 82
    float GetdValDesired(); // Decl: 83

    float GetDerivative(float t);       // Decl: 85
    float GetSecondDerivative(float t); // Decl: 86

    void ClampDerivative(float fMag);       // Decl: 88
    void ClampSecondDerivative(float fMag); // Decl: 89

    void MakeCoeffs(); // Decl: 91
    int HasArrived() { // Decl: 92
        return state == 0;
    };
    void PathdValDesired(float v); // Decl: 93
};

// total size: 0x58
// Decl: 98
struct tCubic2D {
    tCubic1D x; // offset 0x0, size 0x2C, Decl: 109
    tCubic1D y; // offset 0x2C, size 0x2C, Decl: 110

    tCubic2D(short type, float dur) : x(type, dur), y(type, dur) {}
    tCubic2D(short type, bVector2 *pDuration);

    void Update(float fSeconds, float fDClamp, float fDDClamp); // Decl: 112
    int HasArrived() {                                          // Decl: 113
        return x.HasArrived() && y.HasArrived();
    };
    void Snap() { // Decl: 114
        x.Snap();
        y.Snap();
    }
    void SetVal(const float vx, const float vy) { // Decl: 121
        x.SetVal(vx);
        y.SetVal(vy);
    }
    void SetdVal(float vx, float vy) { // Decl: 117
        x.SetdVal(vx);
        y.SetdVal(vy);
    }
    void SetValDesired(float vx, float vy) { // Decl: 118
        x.SetValDesired(vx);
        y.SetValDesired(vy);
    }
    void SetdValDesired(float vx, float vy) { // Decl: 119
        x.SetdValDesired(vx);
        y.SetdValDesired(vy);
    }

    void SetVal(const bVector2 *pV);   // Decl: 121
    void SetdVal(bVector2 *pV);        // Decl: 122
    void SetValDesired(bVector2 *pV);  // Decl: 123
    void SetdValDesired(bVector2 *pV); // Decl: 124

    void GetVal(bVector2 *pV);            // Decl: 126
    void GetdVal(bVector2 *pV);           // Decl: 127
    void GetddVal(bVector2 *pV);          // Decl: 128
    void GetVal(bVector2 *pV, float t);   // Decl: 129
    void GetdVal(bVector2 *pV, float t);  // Decl: 130
    void GetddVal(bVector2 *pV, float t); // Decl: 131

    void GetValDesired(bVector2 *pV);  // Decl: 133
    void GetdValDesired(bVector2 *pV); // Decl: 134

    void SetDuration(const bVector2 *pV);
    void SetDuration(const float t) { // Decl: 138
        x.SetDuration(t);
        y.SetDuration(t);
    }
    void SetDuration(const float tx, const float ty);
    void SetState(short s) { // Decl: 140
        x.SetState(s);
        y.SetState(s);
    }
    void SetFlags(short s) { // Decl: 141
        x.SetFlags(s);
        y.SetFlags(s);
    }
    void PathdValDesired(float x2, float y2); // Decl: 143
    void PathdValDesired(bVector2 *v);        // Decl: 144

    void MakeCoeffs(); // Decl: 146
};

// total size: 0x84
struct tCubic3D {
    tCubic1D x; // offset 0x0, size 0x2C, Decl: 164
    tCubic1D y; // offset 0x2C, size 0x2C, Decl: 165
    tCubic1D z; // offset 0x58, size 0x2C, Decl: 166

    void Update(float dt, float maxDeriv, float maxSecondDeriv);
    int HasArrived() {} // Decl: 169
    void Snap() {}      // Decl: 170

    void SetVal(const float vx, const float vy, const float vz) {
        x.SetVal(vx);
        y.SetVal(vy);
        z.SetVal(vz);
    }
    void SetdVal(const float vx, const float vy, const float vz) { // Decl: 173
        x.SetdVal(vx);
        y.SetdVal(vy);
        z.SetdVal(vz);
    }
    void SetValDesired(const float vx, const float vy, const float vz) { // Decl: 174
        x.SetValDesired(vx);
        y.SetValDesired(vy);
        z.SetValDesired(vz);
    }

    void SetVal(const bVector3 *v);              // Decl: 177
    void SetdVal(bVector3 *v);                   // Decl: 178
    void SetValDesired(bVector3 *v);             // Decl: 179
    void SetdValDesired(bVector3 *pV);           // Decl: 180
    void GetVal(bVector3 *v);                    // Decl: 182
    void GetdVal(struct bVector3 *pV);           // Decl: 183
    void GetddVal(struct bVector3 *pV);          // Decl: 184
    void GetVal(struct bVector3 *pV, float t);   // Decl: 186
    void GetdVal(struct bVector3 *pV, float t);  // Decl: 187
    void GetddVal(struct bVector3 *pV, float t); // Decl: 188

    void GetValDesired(bVector3 *pV);  // Decl: 190
    void GetdValDesired(bVector3 *pV); // Decl: 191

    void SetDuration(const bVector3 *pV); // Decl: 195

    void SetState(short s) {} // Decl: 197
    void SetFlags(short s) {} // Decl: 198

    void PathdValDesired(float x2, float y2, float z2) {} // Decl: 200
    void PathdValDesired(bVector3 *v) {}                  // Decl: 201

    void MakeCoeffs() {} // Decl: 203
};

// total size: 0x178
// Decl: 208
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
    int HasArrived() {} // Decl: 215

    void SetCallBack(void (*func)(int32, int32), int32 p1, int32 p2) {} // Decl: 217
    void ClearCallBack() {}                                             // Decl: 218
    void CallBack() {}                                                  // Decl: 219

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
