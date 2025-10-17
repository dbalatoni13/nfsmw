#include "./UBezierLite.hpp"
#include "./UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/USpline.h"
#include "UMath.h"

void UBezierLite::Evaluate(const UMath::Matrix4 &fSplineMat, const float t, UMath::Vector4 &position) {
    UMath::Vector4 tvec;

    tvec.x = t * t * t;
    tvec.y = t * t;
    tvec.z = t;
    tvec.w = 1.0f;
    
    UMath::Matrix4 matrix;

    UMath::Mult(USpline::GetBasisMatrix(USpline::OVERHAUSER_LINE), fSplineMat, matrix);
    UMath::RotateTranslate(tvec, matrix, position);
    position.w = 1.0f;
}

// STRIPPED
float UBezierLite::EvaluateForY(const UMath::Matrix4 &fSplineMat, const float t) {}

// STRIPPED
void UBezierLite::EvaluateTangent(const UMath::Matrix4 &fSplineMat, const float t, UMath::Vector4 &tangent) {}

// STRIPPED
void SetQuatCompressionPrecision(unsigned int numbits) {}

// STRIPPED
void CompressQuaternion(const UMath::Vector4 &q, unsigned int *compressedQuat) {}
