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
    int32 GetNumControlPoints() {}                         // Decl: 46
    void ClearControlPoints() {}                           // Decl: 47
    bVector3 *GetControlPoints() {}                        // Decl: 48

    bVector3 *GetPoint(bVector3 *point, float parameter);          // Decl: 51
    bVector3 *GetDerivative(bVector3 *slope, float parameter);     // Decl: 52
    bVector3 *GetSecondDerivative(bVector3 *ddv, float parameter); // Decl: 53

    bVector3 *GetSlope(bVector3 *slope, float parameter) {} // Decl: 56

    float GetLength();        // Decl: 60
    float GetFloatLength() {} // Decl: 61

    float LinearEstimateLength(); // Decl: 62

    float GetClosestParameter(bVector3 *p, float fDotDesired, float fTolerance); // Decl: 66

    float GetParameter(bVector3 *p, float fTolerance); // Decl: 70

    float GetParameter(float distance); // Decl: 73

    bVector3 *GetPointByDistance(bVector3 *point, float distance) {} // Decl: 75

    float MaxParameter() {} // Decl: 77

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

#endif
