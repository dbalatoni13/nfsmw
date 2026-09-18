#include "./UBezierLite.hpp"
#include "./UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/USpline.h"
#include "UMath.h"

void UBezierLite::Evaluate(const UMath::Matrix4 &fSplineMat, const float t, UMath::Vector4 &position) {
    UMath::Vector4 tvec;

    tvec.w = 1.0f;
    tvec.x = t * t * t;
    tvec.z = t;
    tvec.y = t * t;
    
    UMath::Matrix4 matrix;

    UMath::Mult(USpline::GetBasisMatrix(USpline::kSpline_Bezier), fSplineMat, matrix);
    UMath::RotateTranslate(tvec, matrix, position);
    position.w = 1.0f;
}

// Los cuatro enganches del stub de depuracion de SN. El objetivo los tiene en
// .data 0x8041D1C0..0x8041D1D0 con valor 1 (los macros SN_LEAVE_*_FOR_OS() de
// libsn.h ponen 0: el original NO usaba esos macros). metrotrk.s y sndvd.c los
// referencian, asi que sin ellos zFoundation no enlaza.
extern "C" {
long SN_DSI = 1;
long SN_ISI = 1;
long SN_ALIGNMENT = 1;
long SN_FPE = 1;
}

// STRIPPED
float UBezierLite::EvaluateForY(const UMath::Matrix4 &fSplineMat, const float t) {}

// STRIPPED
void UBezierLite::EvaluateTangent(const UMath::Matrix4 &fSplineMat, const float t, UMath::Vector4 &tangent) {}

// STRIPPED
void SetQuatCompressionPrecision(unsigned int numbits) {}

// STRIPPED
void CompressQuaternion(const UMath::Vector4 &q, unsigned int *compressedQuat) {}
