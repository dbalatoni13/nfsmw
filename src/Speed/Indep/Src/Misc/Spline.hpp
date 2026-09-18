//
//
//
//
//
//
//
//
//
#ifndef SPLINE_HPP
#define SPLINE_HPP

#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// Decl: 20
enum SplineType {
    OVERHAUSER_LINE = 0,
    OVERHAUSER_LOOP = 1,
    OVERHAUSER_EXTRAPOLATED = 2,
};

// total size: 0x30
// Decl: 31
class Spline {
  public:
    // Decl: 34
    Spline(SplineType type, int32 expected_number_of_control_points, int32 table_size);
    // Decl: 35
    Spline(SplineType type, int32 number_of_control_points, bVector3 *control_point_table, int32 table_size);

    ~Spline(); // Decl: 39

    void SetControlPoints(bVector3 *pPoints, int nPoints); // Decl: 41
    void SetControlPoint(int i, bVector3 *p);              // Decl: 42
    void AddControlPoint(bVector3 *p);                     // Decl: 43
    void AddControlPoint(float x, float y, float z) {}     // Decl: 44
    void RemoveFirstControlPoint();                        // Decl: 45
    int32 GetNumControlPoints();                         // Decl: 46
    void ClearControlPoints() {}                           // Decl: 47
    bVector3 *GetControlPoints();                        // Decl: 48

    bVector3 *GetPoint(bVector3 *point, float parameter);          // Decl: 51
    bVector3 *GetDerivative(bVector3 *slope, float parameter);     // Decl: 52
    bVector3 *GetSecondDerivative(bVector3 *ddv, float parameter); // Decl: 53

    bVector3 *GetSlope(bVector3 *slope, float parameter); // Decl: 56

    float GetLength();        // Decl: 60
    float GetFloatLength(); // Decl: 61

    float LinearEstimateLength(); // Decl: 62

    float GetClosestParameter(bVector3 *p, float fDotDesired, float fTolerance); // Decl: 66

    float GetParameter(bVector3 *p, float fTolerance); // Decl: 70

    float GetParameter(float distance); // Decl: 73

    bVector3 *GetPointByDistance(bVector3 *point, float distance); // Decl: 75

    float MaxParameter(); // Decl: 77

    void Calibrate(); // Decl: 92

    void ZeroAllZValues(); // Decl: 94

    void DoSnapshot(ReplaySnapshot *snapshot); // Decl: 96

  private:
    int32 ControlPointBufferSize; // offset 0x0, size 0x4, Decl: 99
    int32 NumControlPoints;       // offset 0x4, size 0x4, Decl: 100
    SplineType Type;              // offset 0x8, size 0x4, Decl: 101

    bVector3 *pControlPoints;     // offset 0xC, size 0x4, Decl: 104
    bVector3 *pControlPointArray; // offset 0x10, size 0x4, Decl: 105

    int MinControlPoints();                                                                 // Decl: 107
    bVector3 *GetControlPoint(float t);                                                     // Decl: 108
    bVector3 *Evaluate(bVector3 *pV, float fFloor, float b0, float b1, float b2, float b3); // Decl: 109
    float Dot(bVector3 *p, float fParam);                                                   // Decl: 110
    float NormalDot(bVector3 *p, float fParam);                                             // Decl: 111

    int32 Dirty;     // offset 0x14, size 0x4, Decl: 113
    int32 TableSize; // offset 0x18, size 0x4, Decl: 114
    int32 CalLevel;  // offset 0x1C, size 0x4, Decl: 115

    float Length;       // offset 0x20, size 0x4, Decl: 117
    float CalTableStep; // offset 0x24, size 0x4, Decl: 118
    int32 CalTableSize; // offset 0x28, size 0x4, Decl: 119
    float *pCalTable;   // offset 0x2C, size 0x4, Decl: 120
};

#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x2C
struct tCubic1D {
    tCubic1D() {}

    // El volcado DWARF del original expande `inline tCubic1D::tCubic1D(short type,
    // float dur)` en los constructores de TrackCarCameraMover, TrackCopCameraMover,
    // CubicCameraMover e IconScroller. El orden de los stores es el que ya estaba
    // replicado a mano en Cubic.cpp/TrackCop.cpp.
    tCubic1D(short type, float dur)
        : Val(0.0f), dVal(0.0f), ValDesired(0.0f), dValDesired(0.0f), time(0.0f), duration(dur), state(0), flags(type) {
        Coeff[0] = 0.0f;
        Coeff[1] = 0.0f;
        Coeff[2] = 0.0f;
        Coeff[3] = 0.0f;
    }

    void MakeCoeffs();
    float GetVal(float t);
    float GetdVal(float t);
    float GetddVal(float t);
    float GetDerivative(float t);
    float GetSecondDerivative(float t);
    void ClampDerivative(float maxDeriv);
    void ClampSecondDerivative(float maxSecondDeriv);
    void Update(float dt, float maxDeriv, float maxSecondDeriv);

    void Snap() {
        time = 1.0f;
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }

    void SetVal(const float v) {
        Val = v;
        if (v != ValDesired) {
            state = 2;
        }
    }

    void SetdVal(float v) {
        dVal = v;
        if (v != dValDesired) {
            state = 2;
        }
    }

    void SetValDesired(float v) {
        ValDesired = v;
        if (v != Val) {
            state = 2;
        }
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

    int HasArrived() {

        return state == 0;
    }

    float Val;         // offset 0x0, size 0x4
    float dVal;        // offset 0x4, size 0x4
    float ValDesired;  // offset 0x8, size 0x4
    float dValDesired; // offset 0xC, size 0x4
    float Coeff[4];    // offset 0x10, size 0x10
    float time;        // offset 0x20, size 0x4
    float duration;    // offset 0x24, size 0x4
    short state;       // offset 0x28, size 0x2
    short flags;       // offset 0x2A, size 0x2
};

// total size: 0x58
struct tCubic2D {
    tCubic2D() {}
    tCubic2D(short type, float dur) : x(type, dur), y(type, dur) {}
    tCubic2D(short type, bVector2 *pDuration) : x(type, pDuration->x), y(type, pDuration->y) {}

    void SetValDesired(bVector2 *v);
    void GetVal(bVector2 *v);

    int HasArrived() {
        return x.HasArrived() && y.HasArrived();
    }

    // El DWARF lista ademas PathdValDesired(float, float) y
    // PathdValDesired(bVector2 *), inline; tCubic1D no tiene uno al que delegar y
    // su cuerpo no se ve, asi que no se escriben.
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

    void SetDuration(const float tx, const float ty) {
        x.SetDuration(tx);
        y.SetDuration(ty);
    }

    void SetState(short s) {
        x.SetState(s);
        y.SetState(s);
    }

    void SetFlags(short s) {
        x.SetFlags(s);
        y.SetFlags(s);
    }

    void MakeCoeffs() {
        x.MakeCoeffs();
        y.MakeCoeffs();
    }

    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C
};

// total size: 0x84
struct tCubic3D {
    tCubic3D() {}
    tCubic3D(short type, float dur) : x(type, dur), y(type, dur), z(type, dur) {}
    tCubic3D(short type, bVector3 *pDuration) : x(type, pDuration->x), y(type, pDuration->y), z(type, pDuration->z) {}

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

    void SetDuration(const float t) {
        x.SetDuration(t);
        y.SetDuration(t);
        z.SetDuration(t);
    }

    void SetDuration(const float tx, const float ty, const float tz) {
        x.SetDuration(tx);
        y.SetDuration(ty);
        z.SetDuration(tz);
    }


    void Snap() {
        x.Snap();
        y.Snap();
        z.Snap();
    }

    tCubic1D x; // offset 0x0, size 0x2C
    tCubic1D y; // offset 0x2C, size 0x2C
    tCubic1D z; // offset 0x58, size 0x2C
};

#endif
