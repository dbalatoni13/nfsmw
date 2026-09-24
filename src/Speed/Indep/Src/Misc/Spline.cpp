#include "Spline.hpp"

// Everything is stripped here

static const int bSplineDebug = 0; // size: 0x4, Decl: 23

Spline::Spline(SplineType type, int expected_number_of_control_points, int table_size) {}

Spline::Spline(SplineType type, int number_of_control_points, bVector3 *control_point_table, int table_size) {}

Spline::~Spline() {}

void Spline::SetControlPoints(bVector3 *pPoints, int nPoints) {}

void Spline::DoSnapshot(ReplaySnapshot *snapshot) {}

void Spline::SetControlPoint(int i, bVector3 *p) {}

void Spline::AddControlPoint(bVector3 *p) {}

void Spline::RemoveFirstControlPoint() {}

bVector3 *Spline::GetControlPoint(float t) {}

int Spline::MinControlPoints() {}

bVector3 *Spline::Evaluate(bVector3 *pV, float fFloor, float b0, float b1, float b2, float b3) {}

bVector3 *Spline::GetPoint(bVector3 *point, float parameter) {}

bVector3 *Spline::GetDerivative(bVector3 *slope, float parameter) {}

bVector3 *Spline::GetSecondDerivative(bVector3 *ddv, float parameter) {}

// Decl: 315
float CalStepSize[7] = {16.0f, 64.0f, 256.0f, 1024.0f, 4096.0f, 16384.0f, 65536.0f};

// Decl: 319
float CalDiv[7] = {1.0f, 1.0f / 4, 1.0f / 16, 1.0f / 64, 1.0f / 256, 1.0f / 1024, 1.0f / 4096};

// Decl: 323
float CalMult[7] = {1.0f / 64, 1.0f / 256, 1.0f / 1024, 1.0f / 4096, 1.0f / 16384, 1.0f / 65536};

void Spline::Calibrate() {}

float Spline::LinearEstimateLength() {}

float Spline::GetLength() {}

float Spline::Dot(bVector3 *p, float fParam) {}

float Spline::NormalDot(bVector3 *p, float fParam) {}

float Spline::GetClosestParameter(bVector3 *p, float fDotDesired, float fTolerance) {}

float Spline::GetParameter(bVector3 *p, float fTolerance) {}

float Spline::GetParameter(float distance) {}

void Spline::ZeroAllZValues() {}
