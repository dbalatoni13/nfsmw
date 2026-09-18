#include "Speed/Indep/Libs/Support/Utility/USpline.h"

static const UMath::Matrix4 fBezierBasisMat = {
    {-1.0f, 3.0f, -3.0f, 1.0f},
    {3.0f, -6.0f, 3.0f, 0.0f},
    {-3.0f, 3.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f},
};

static const UMath::Matrix4 fCatRomBasisMat = {
    {-1.0f, 3.0f, -3.0f, 1.0f},
    {2.0f, -5.0f, 4.0f, -1.0f},
    {-1.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 2.0f, 0.0f, 0.0f},
};

static const UMath::Matrix4 fBezierTanBasisMat = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {-3.0f, 9.0f, -9.0f, 3.0f},
    {6.0f, -12.0f, 6.0f, 0.0f},
    {-3.0f, 3.0f, 0.0f, 0.0f},
};

static const UMath::Matrix4 fCatRomTanBasisMat = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {-3.0f, 6.0f, 3.0f, 0.0f},
    {6.0f, -10.0f, 0.0f, 4.0f},
    {-3.0f, 4.0f, 1.0f, 0.0f},
};

static const UMath::Matrix4 fBezier2ndBasisMat = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {-6.0f, 18.0f, -18.0f, 6.0f},
    {6.0f, -12.0f, 6.0f, 0.0f},
};

static const UMath::Matrix4 fCatRom2ndBasisMat = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {-6.0f, 12.0f, 6.0f, 0.0f},
    {6.0f, -10.0f, 0.0f, 4.0f},
};

USpline::USpline() {
    fSplineType = kSpline_Invalid;
    fSplineMat = UMath::Matrix4::kIdentity;
    VU0_v4Init(fLookAt[0]);
    VU0_v4Init(fLookAt[1]);
}

USpline::~USpline() {}

const UMath::Matrix4 &USpline::GetBasisMatrix(SplineType splineType) {
    switch (splineType) {
    case kSpline_Bezier:
        break;
    case kSpline_CatMulRom:
        return fCatRomBasisMat;
    }
    return fBezierBasisMat;
}

const UMath::Matrix4 &USpline::Get2ndBasisMatrix(SplineType splineType) {
    switch (splineType) {
    case kSpline_Bezier:
        break;
    case kSpline_CatMulRom:
        return fCatRom2ndBasisMat;
    }
    return fBezier2ndBasisMat;
}

const UMath::Matrix4 &USpline::GetTangentBasisMatrix(SplineType splineType) {
    switch (splineType) {
    case kSpline_Bezier:
        break;
    case kSpline_CatMulRom:
        return fCatRomTanBasisMat;
    }
    return fBezierTanBasisMat;
}

void USpline::BuildSplineEx(const UMath::Vector3 &start, const UMath::Vector3 &startControl, const UMath::Vector3 &end, const UMath::Vector3 &endControl) {
    if (fSplinePtList.size() <= 1) {
        fSplineType = kSpline_Bezier;

        UMath::Vector4To3(fSplineMat.v0) = start;
        UMath::Vector4To3(fSplineMat.v1) = startControl;
        UMath::Vector4To3(fSplineMat.v2) = endControl;
        UMath::Vector4To3(fSplineMat.v3) = end;
        fSplineMat.v0.w = 0.0f;
        fSplineMat.v1.w = 0.0f;
        fSplineMat.v2.w = 0.0f;
        fSplineMat.v3.w = 0.0f;
    }
}

void USpline::EvaluateSpline(float t, UMath::Vector4 &result) {
    UMath::Vector4 tvec;

    tvec.w = 1.0f;
    tvec.x = t * t * t;
    tvec.z = t;
    tvec.y = t * t;

    UMath::Matrix4 matrix;

    UMath::Mult(GetBasisMatrix(), fSplineMat, matrix);
    UMath::RotateTranslate(tvec, matrix, result);
    result.w = 1.0f;

    if (fSplineType == kSpline_CatMulRom) {
        VU0_v4scalexyz(result, 0.5f, result);
    }
}

void USpline::EvaluateTangent(float t, UMath::Vector4 &tangent) {
    EvaluateDerivative(t, tangent);
    VU0_v4unitxyz(tangent, tangent);
}

void USpline::EvaluateDerivative(float t, UMath::Vector4 &derivative) {
    UMath::Vector4 tvec;

    tvec.w = 1.0f;
    tvec.x = t * t * t;
    tvec.z = t;
    tvec.y = t * t;

    UMath::Matrix4 matrix;

    UMath::Mult(GetTangentBasisMatrix(), fSplineMat, matrix);
    UMath::RotateTranslate(tvec, matrix, derivative);
    derivative.w = 1.0f;
}

void USpline::Evaluate2ndDerivative(float t, UMath::Vector4 &derivative) {
    UMath::Vector4 tvec;

    tvec.w = 1.0f;
    tvec.x = t * t * t;
    tvec.z = t;
    tvec.y = t * t;

    UMath::Matrix4 matrix;

    UMath::Mult(Get2ndBasisMatrix(), fSplineMat, matrix);
    UMath::RotateTranslate(tvec, matrix, derivative);
    derivative.w = 1.0f;
}

float USpline::EvaluateCurvatureXZ(float t) {
    UMath::Vector4 derivative;
    UMath::Vector4 secondDerivative;

    EvaluateDerivative(t, derivative);
    UMath::Vector4 *pder = &derivative;
    Evaluate2ndDerivative(t, secondDerivative);

    const float tangentLength = VU0_sqrt(derivative.x * derivative.x + pder->z * pder->z);
    const float crossLength = UMath::CrossXZ(derivative, secondDerivative);
    const float tangentLengthCubed = tangentLength * tangentLength * tangentLength;

    if (tangentLengthCubed == UMath::Clamp(tangentLengthCubed, -0.0001f, 0.0001f)) {
        return 1000.0f;
    }

    return crossLength / tangentLengthCubed;
}
