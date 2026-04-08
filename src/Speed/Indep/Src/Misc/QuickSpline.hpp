#ifndef MISC_QUICKSPLINE_H
#define MISC_QUICKSPLINE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Replay.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

enum QuickSplineEndPointType {
    QUICKSPLINE_LOOP = 0,
    QUICKSPLINE_LINE = 1,
    QUICKSPLINE_EXTRAPOLATED = 2,
};

enum QuickSplineBasisType {
    QUICKSPLINE_OVERHAUSER = 0,
};

// total size: 0x2C
class QuickSpline {
    uint32 nHash;                         // offset 0x0, size 0x4
    QuickSplineEndPointType EndPointType; // offset 0x4, size 0x4
    QuickSplineBasisType BasisType;       // offset 0x8, size 0x4
    float MaxParam;                       // offset 0xC, size 0x4
    float MinParam;                       // offset 0x10, size 0x4
    float Length;                         // offset 0x14, size 0x4
    int8 ControlPointsDirty;              // offset 0x18, size 0x1
    int8 BufferWasAllocated;              // offset 0x19, size 0x1
    int8 MinControlPoints;                // offset 0x1A, size 0x1
    uint16 MaxControlPoints;              // offset 0x1C, size 0x2
    uint16 NumControlPoints;              // offset 0x1E, size 0x2
    bVector4 *pControlPointBuffer;        // offset 0x20, size 0x4
    bVector4 *pControlPoints;             // offset 0x24, size 0x4
    bMatrix4 *pBasisMatricies;            // offset 0x28, size 0x4

  public:
    QuickSpline(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type, int max_control_points);
    QuickSpline(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type, bVector4 *control_point_buffer, int num_buffer_entries);
    ~QuickSpline();
    void DoSnapshot(ReplaySnapshot *snapshot, int num_fields);
    void SetNumControlPoints(int num_control_points);
    bVector4 *GetPoint(bVector4 *point, float param, int deriv);
    int FindClosestControlPoint(const bVector3 &point, int start_point_number);
    int FindClosestControlPoint(const bVector3 &point);
    float FindClosestParam(const bVector3 &point, float initial_param, float error_tolerance, int use_euans_hack);
    float FindClosestLateralOffset(const bVector3 &point, float initial_param, float *closest_param, float error_tolerance, int use_euans_hack);
    float GetParam(float f_distance);
    void CalibrateLength(int num_steps);
    void MemoryImageLoad(bVector4 *control_point_buffer);
    void MemoryImageUnload();
    bool HasKink();
    void PlotToCaffeine();
    void EmptyCaffeineLayer();
    void MakeControlPointsEquiDistant();
    void ZeroAllZValues();

    enum QuickSplineEndPointType GetEndPointType() {}

    enum QuickSplineBasisType GetBasisType() {}

    struct bVector4 *GetControlPoints() {}

    struct bVector4 *GetControlPointBuffer() {}

    int GetControlPointBufferSize() {}

    int GetMaxControlPoints() {}

    int GetNumControlPoints() {}

    float GetMaxParam() {}

    float GetMinParam() {}

    void SetControlPoint(int point_number, float value) {}

    void SetControlPoint(int point_number, const bVector2 &point) {}

    void SetControlPoint(int point_number, const bVector3 &point) {}

    void SetControlPoint(int point_number, const bVector4 &point) {}

    void GetControlPoint(bVector4 *point, int point_number) {}

    bVector4 *GetControlPoint(int point_number) {}

    float GetPoint(float param, int deriv) {}

    bVector2 *GetPoint(bVector2 *point, float param, int deriv) {}

    bVector3 *GetPoint(bVector3 *point, float param, int deriv) {}

    float GetDerivative(float param) {}

    bVector2 *GetDerivative(bVector2 *deriv, float param) {}

    bVector3 *GetDerivative(bVector3 *deriv, float param) {}

    bVector4 *GetDerivative(bVector4 *deriv, float param) {}

    float GetSecondDerivative(float param) {}

    bVector2 *GetSecondDerivative(bVector2 *deriv, float param) {}

    bVector3 *GetSecondDerivative(bVector3 *deriv, float param) {}

    bVector4 *GetSecondDerivative(bVector4 *deriv, float param) {}

    float GetDistance(float param) {}

    float GetLength() {}

    void SetLength(float length) {}

    char GetMinControlPoints() {}

    uint32 GetHash() {
        return nHash;
    }

    void SetHash(unsigned int hash) {}

    friend int LoaderQuickSpline(bChunk *pChunk);

  private:
    void Init(QuickSplineEndPointType endpoint_type, QuickSplineBasisType basis_type);
    void FixupEndpoints();
    void GenerateExtrapolatedControlPoints(bVector4 *extrapolated_control_points, int control_point_num);
    void EvaluateSpline(bVector4 *point, float t, int deriv, const bVector4 *control_points);
    float ClampParam(float param);
};

#endif
