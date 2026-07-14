#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

cPathLine::cPathLine() {
    ClearStages();
    bComplete = true;
}

cPathLine::~cPathLine() {}

void cPathLine::Initialize(float _Start, float _Finish, int _Length) {
    ClearStages();
    AddStage(_Start, _Finish, _Length, LINEAR);
    CurValue = _Start;
}

void cPathLine::ClearStages() {
    CurValue = 0.0f;
    ElapsedTime = 0.0f;
    cur_stage = 0;
    num_stages = 0;
    for (int i = 0; i < 6; i++) {
        Length[i] = Finish[i] = Start[i] = 0.0f;
        IsLinked[i] = false;
        CurveTypes[i] = LINEAR;
    }
    bComplete = false;
}

int cPathLine::AddStage(float _Start, float _Finish, int _Length, eCURVETYPE _Curve) {
    if (num_stages == 5) {
        return -1;
    }

    Length[num_stages] = static_cast<float>(_Length) * 0.001f;
    if (Length[num_stages] <= 0.0f) {
        Length[num_stages] = 0.01f;
    }

    Finish[num_stages] = _Finish;
    Start[num_stages] = _Start;
    CurveTypes[num_stages] = _Curve;
    IsLinked[num_stages] = false;
    bComplete = false;

    if (num_stages == 0) {
        CurValue = Start[0];
    }

    num_stages = num_stages + 1;
    return num_stages;
}

int cPathLine::AddLinkedStage(float _Finish, int _Length, eCURVETYPE _Curve) {
    float _Start = 0.0f;
    if (num_stages != 0) {
        AddStage(_Start, _Finish, _Length, _Curve);
        IsLinked[num_stages - 1] = true;
        return num_stages;
    }
    return -1;
}

void cPathLine::Update(float delta_time) {
    if (bComplete) {
        return;
    }

    if (num_stages == 0) {
        return;
    }

    delta_time += ElapsedTime;
    ElapsedTime = delta_time;

    if (delta_time > Length[cur_stage]) {
        CurValue = Finish[cur_stage];
        if (cur_stage < num_stages - 1) {
            ElapsedTime = delta_time - Length[cur_stage];
            cur_stage++;
            if (IsLinked[cur_stage]) {
                Start[cur_stage] = Finish[cur_stage - 1];
            }
        } else {
            bComplete = true;
        }
        return;
    }

    switch (CurveTypes[cur_stage]) {
    case LINEAR:
    default:
        CurValue = (Finish[cur_stage] - Start[cur_stage]) * (delta_time / Length[cur_stage]) + Start[cur_stage];
        break;
    case PARABOLIC: {
        float t = Finish[cur_stage] - Start[cur_stage];
        float s;
        float x;
        CurValue = t * (delta_time / Length[cur_stage]) * (delta_time / Length[cur_stage]) + Start[cur_stage];
        break;
    }
    case INV_PARABOLIC: {
        float t = Finish[cur_stage] - Start[cur_stage];
        float s;
        float x;
        CurValue = t * (-(delta_time / Length[cur_stage] - 1.0f) * (delta_time / Length[cur_stage] - 1.0f) + 1.0f) +
                   Start[cur_stage];
        break;
    }
    case EQ_PWR_SQ: {
        float t = Finish[cur_stage] - Start[cur_stage];
        float s;
        float x;
        int Delta = bClamp(static_cast<int>((delta_time * 32767.0f) / Length[cur_stage]), 0, 0x7fff);
        float Result = static_cast<float>(NFSMixShape::GetCurveOutput(::SHAPE_UP_EQPWR_SQ, Delta, false));
        Result *= 3.051851e-05f;
        CurValue = t * Result + Start[cur_stage];
        break;
    }
    }
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

void cInterpLine::Update(float delta_time) {
    if (bComplete) {
        return;
    }

    delta_time += ElapsedTime;
    ElapsedTime = delta_time;

    if (delta_time > Length) {
        bComplete = true;
        CurValue = Finish;
        return;
    }

    switch (CurveTypes) {
    case LINEAR:
    default:
        CurValue = (Finish - Start) * (delta_time / Length) + Start;
        break;
    case PARABOLIC: {
        float t = Finish - Start;
        float s;
        float x;
        CurValue = t * (delta_time / Length) * (delta_time / Length) + Start;
        break;
    }
    case INV_PARABOLIC: {
        float t = Finish - Start;
        float s;
        float x;
        CurValue = t * (-(delta_time / Length - 1.0f) * (delta_time / Length - 1.0f) + 1.0f) + Start;
        break;
    }
    case EQ_PWR_SQ: {
        float t = Finish - Start;
        float s;
        float x;
        int Delta = bClamp(static_cast<int>((delta_time * 32767.0f) / Length), 0, 0x7fff);
        float Result = static_cast<float>(NFSMixShape::GetCurveOutput(::SHAPE_UP_EQPWR_SQ, Delta, false));
        Result *= 3.051851e-05f;
        CurValue = t * Result + Start;
        break;
    }
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
    float Scale = (LastInput - Start) / (Finish - Start);
    bNeedsRegenerate = false;
    Scale = bClamp(Scale, 0.0f, 1.0f);
    LastOutput = Scale * (Max - Min) + Min;
}

EAX_CarState *GetClosestPlayerCar(const bVector3 *vPosition) {
    int CarID = 0;
    return GetClosestPlayerCar(vPosition, false, CarID);
}

bool IsCarInRadius(EAX_CarState *pCar, const bVector3 *vPos, float fRadius) {
    if (!pCar) {
        return false;
    }
    bVector3 *CarPos = pCar->GetPosition();
    return bDistBetween(vPos, CarPos) < fRadius;
}
