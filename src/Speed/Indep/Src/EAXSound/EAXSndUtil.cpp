#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

cPathLine::cPathLine() {
    ClearStages();
    bComplete = true;
}

cPathLine::~cPathLine() {}

void cPathLine::ClearStages() {
    num_stages = 0;
    CurValue = 0.0f;
    ElapsedTime = 0.0f;
    cur_stage = 0;
    for (int i = 0; i < 6; i++) {
        Start[i] = 0.0f;
        Finish[i] = 0.0f;
        Length[i] = 0.0f;
        IsLinked[i] = false;
        CurveTypes[i] = LINEAR;
    }
    bComplete = false;
}

void cPathLine::Initialize(float _Start, float _Finish, int _Length) {
    ClearStages();
    AddStage(_Start, _Finish, _Length, LINEAR);
    CurValue = _Start;
}

int cPathLine::AddLinkedStage(float _Finish, int _Length, eCURVETYPE _Curve) {
    float _Start = 0.0f;
    int result = -1;
    if (num_stages != 0) {
        result = AddStage(_Start, _Finish, _Length, _Curve);
        IsLinked[num_stages - 1] = true;
        result = num_stages;
    }
    return result;
}

cInterpLine::cInterpLine()
: ElapsedTime(0.0f) //
, Length(0.0f) //
, Start(0.0f) //
, Finish(0.0f) //
, CurveTypes(LINEAR) //
, CurValue(0.0f) //
, bComplete(true)
{}

cInterpLine::~cInterpLine() {}

void cInterpLine::Initialize(float _Start, float _Finish, int _Length, eCURVETYPE _Curve) {
    Length = static_cast<float>(_Length) * 0.001f;
    if (Length <= 0.0f) {
        Length = 0.01f;
    }
    ElapsedTime = 0.0f;
    Finish = _Finish;
    CurveTypes = _Curve;
    bComplete = false;
    Start = _Start;
    CurValue = _Start;
}

void cInterpLine::Update(float delta_time, float _new_Finish) {
    Finish = _new_Finish;
    Update(delta_time);
    if (bComplete) {
        CurValue = _new_Finish;
    }
}

Slope::Slope(float _Min, float _Max, float _Start, float _Finish) {
    Initialize(_Min, _Max, _Start, _Finish);
}

Slope::~Slope() {}

void Slope::Initialize(float _Min, float _Max, float _Start, float _Finish) {
    Min = _Min;
    Max = _Max;
    Start = _Start;
    Finish = _Finish;
    if (bAbs(_Finish - _Start) < 1e-06f) {
        Finish = _Finish + 1e-06f;
    }
    LastOutput = Min;
    LastInput = 0.0f;
}

float Slope::GetValue(float input) {
    LastInput = input;
    Regenerate();
    return LastOutput;
}

void Slope::Regenerate() {
    float fVal = (LastInput - Start) / (Finish - Start);
    bNeedsRegenerate = false;
    fVal = bClamp(fVal, 0.0f, 1.0f);
    LastOutput = fVal * (Max - Min) + Min;
}

EAX_CarState *GetClosestPlayerCar(const bVector3 *vPosition) {
    int CarID = 0;
    return GetClosestPlayerCar(vPosition, false, CarID);
}

bool IsCarInRadius(EAX_CarState *pCar, const bVector3 *vPos, float fRadius) {
    if (pCar == nullptr) {
        return false;
    }
    float dist = bDistBetween(vPos, reinterpret_cast<const bVector3 *>(reinterpret_cast<const char *>(pCar) + 0x44));
    return dist < fRadius;
}
