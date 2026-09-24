//
//
//
//
#ifndef QUICK_SPLINE_HPP
#define QUICK_SPLINE_HPP

#include "Replay.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// Decl: 9
enum QuickSplineBasisType {
    QUICKSPLINE_OVERHAUSER = 0,
};

// Decl: 14
enum QuickSplineEndPointType {
    QUICKSPLINE_LOOP = 0,
    QUICKSPLINE_LINE = 1,
    QUICKSPLINE_EXTRAPOLATED = 2,
};

#define QUICKSPLINE_NUM_BUFFER_ENTRIES(max_control_points) ((max_control_points) + 3) // Decl: 28

// total size: 0x2C
// Decl: 32
class QuickSpline {
  public:
    // Decl: 37
    QuickSpline(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type, int max_control_points);
    // Decl: 40
    QuickSpline(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type, bVector4 *control_point_buffer, int num_buffer_entries);

    ~QuickSpline(); // Decl: 42

    void MemoryImageLoad(bVector4 *control_point_buffer); // Decl: 46
    void MemoryImageUnload();                             // Decl: 47

    QuickSplineEndPointType GetEndPointType() {}               // Decl: 49
    QuickSplineBasisType GetBasisType() {}                     // Decl: 50
    bVector4 *GetControlPoints() {}                            // Decl: 51
    bVector4 *GetControlPointBuffer() {}                       // Decl: 52
    int GetControlPointBufferSize() {}                         // Decl: 53
    void DoSnapshot(ReplaySnapshot *snapshot, int num_fields); // Decl: 54
    int GetMaxControlPoints() {}                               // Decl: 55
    void SetNumControlPoints(int num_control_points);          // Decl: 56
    int GetNumControlPoints() {}                               // Decl: 57

    float GetMaxParam() {} // Decl: 63
    float GetMinParam() {} // Decl: 64

    void SetControlPoint(int point_number, float value) {}           // Decl: 68
    void SetControlPoint(int point_number, const bVector2 &point) {} // Decl: 69
    void SetControlPoint(int point_number, const bVector3 &point) {} // Decl: 70
    void SetControlPoint(int point_number, const bVector4 &point) {} // Decl: 71

    void GetControlPoint(bVector4 *point, int point_number) {} // Decl: 72
    bVector4 *GetControlPoint(int point_number) {}             // Decl: 73

    float GetPoint(float param, int deriv) {}                      // Decl: 77
    bVector2 *GetPoint(bVector2 *point, float param, int deriv) {} // Decl: 78
    bVector3 *GetPoint(bVector3 *point, float param, int deriv) {} // Decl: 79
    bVector4 *GetPoint(bVector4 *point, float param, int deriv);   // Decl: 80

    float GetDerivative(float param) {}                      // Decl: 83
    bVector2 *GetDerivative(bVector2 *deriv, float param) {} // Decl: 84
    bVector3 *GetDerivative(bVector3 *deriv, float param) {} // Decl: 85
    bVector4 *GetDerivative(bVector4 *deriv, float param) {} // Decl: 86

    float GetSecondDerivative(float param) {}                      // Decl: 89
    bVector2 *GetSecondDerivative(bVector2 *deriv, float param) {} // Decl: 90
    bVector3 *GetSecondDerivative(bVector3 *deriv, float param) {} // Decl: 91
    bVector4 *GetSecondDerivative(bVector4 *deriv, float param) {} // Decl: 92

    float FindClosestParam(const bVector3 &point, float initial_param, float error_tolerance, int use_euans_hack); // Decl: 95

    // Decl: 98
    float FindClosestLateralOffset(const bVector3 &point, float initial_param, float *closest_param, float error_tolerance, int use_euans_hack);

    int FindClosestControlPoint(const bVector3 &point, int start_point_number); // Decl: 102
    int FindClosestControlPoint(const bVector3 &point);                         // Decl: 103

    void CalibrateLength(int num_steps); // Decl: 107
    float GetDistance(float param) {}    // Decl: 108
    float GetParam(float f_distance);    // Decl: 109
    float GetLength() {}                 // Decl: 110
    void SetLength(float length) {}      // Decl: 111

    char GetMinControlPoints() {} // Decl: 113

    // Decl: 115
    uint32 GetHash() {
        return this->nHash;
    }
    void SetHash(unsigned int hash) {} // Decl: 116

    bool HasKink();                      // Decl: 119
    void PlotToCaffeine();               // Decl: 120
    void EmptyCaffeineLayer();           // Decl: 121
    void MakeControlPointsEquiDistant(); // Decl: 123

    void ZeroAllZValues(); // Decl: 125

    friend int LoaderQuickSpline(bChunk *pChunk);

  private:
    void Init(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type);                    // Decl: 128
    void FixupEndpoints();                                                                                // Decl: 129
    void GenerateExtrapolatedControlPoints(bVector4 *extrapolated_control_points, int control_point_num); // Decl: 130
    void EvaluateSpline(bVector4 *point, float t, int deriv, const bVector4 *control_points);             // Decl: 131
    float ClampParam(float param);                                                                        // Decl: 132

    uint32 nHash;                         // offset 0x0, size 0x4, Decl: 134
    QuickSplineEndPointType EndPointType; // offset 0x4, size 0x4, Decl: 136
    QuickSplineBasisType BasisType;       // offset 0x8, size 0x4, Decl: 137
    float MaxParam;                       // offset 0xC, size 0x4, Decl: 139
    float MinParam;                       // offset 0x10, size 0x4, Decl: 140
    float Length;                         // offset 0x14, size 0x4, Decl: 141
    int8 ControlPointsDirty;              // offset 0x18, size 0x1, Decl: 143
    int8 BufferWasAllocated;              // offset 0x19, size 0x1, Decl: 144
    int8 MinControlPoints;                // offset 0x1A, size 0x1, Decl: 145
    uint16 MaxControlPoints;              // offset 0x1C, size 0x2, Decl: 146
    uint16 NumControlPoints;              // offset 0x1E, size 0x2, Decl: 147
    bVector4 *pControlPointBuffer;        // offset 0x20, size 0x4, Decl: 148
    bVector4 *pControlPoints;             // offset 0x24, size 0x4, Decl: 149
    bMatrix4 *pBasisMatricies;            // offset 0x28, size 0x4, Decl: 151
};

#endif
