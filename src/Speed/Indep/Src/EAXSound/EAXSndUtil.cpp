#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

cPathLine::cPathLine() {
    this->ClearStages();
    this->bComplete = true;
}

cPathLine::~cPathLine() {}

void cPathLine::Initialize(float _Start, float _Finish, int _Length) {
    this->ClearStages();
    this->AddStage(_Start, _Finish, _Length, LINEAR);
    this->CurValue = _Start;
}

void cPathLine::ClearStages() {
    this->CurValue = 0.0f;
    this->ElapsedTime = 0.0f;
    this->cur_stage = 0;
    this->num_stages = 0;
    for (int i = 0; i < 6; i++) {
        this->Length[i] = this->Finish[i] = this->Start[i] = 0.0f;
        this->IsLinked[i] = false;
        this->CurveTypes[i] = LINEAR;
    }
    this->bComplete = false;
}

int cPathLine::AddStage(float _Start, float _Finish, int _Length, eCURVETYPE _Curve) {
    if (this->num_stages == 5) {
        return -1;
    }

    this->Length[this->num_stages] = static_cast<float>(_Length) * 0.001f;
    if (this->Length[this->num_stages] <= 0.0f) {
        this->Length[this->num_stages] = 0.01f;
    }

    this->Finish[this->num_stages] = _Finish;
    this->Start[this->num_stages] = _Start;
    this->CurveTypes[this->num_stages] = _Curve;
    this->IsLinked[this->num_stages] = false;
    this->bComplete = false;

    if (this->num_stages == 0) {
        this->CurValue = this->Start[0];
    }

    this->num_stages = this->num_stages + 1;
    return this->num_stages;
}

int cPathLine::AddLinkedStage(float _Finish, int _Length, eCURVETYPE _Curve) {
    float _Start = 0.0f;
    if (this->num_stages != 0) {
        this->AddStage(_Start, _Finish, _Length, _Curve);
        this->IsLinked[this->num_stages - 1] = true;
        return this->num_stages;
    }
    return -1;
}

void cPathLine::Update(float delta_time) {
    if (this->bComplete) {
        return;
    }

    if (this->num_stages == 0) {
        return;
    }

    delta_time += this->ElapsedTime;
    this->ElapsedTime = delta_time;

    if (delta_time > this->Length[this->cur_stage]) {
        this->CurValue = this->Finish[this->cur_stage];
        if (this->cur_stage < this->num_stages - 1) {
            this->ElapsedTime = delta_time - this->Length[this->cur_stage];
            this->cur_stage++;
            if (this->IsLinked[this->cur_stage]) {
                this->Start[this->cur_stage] = this->Finish[this->cur_stage - 1];
            }
        } else {
            this->bComplete = true;
        }
        return;
    }

    switch (this->CurveTypes[this->cur_stage]) {
    case LINEAR:
    default:
        this->CurValue = (this->Finish[this->cur_stage] - this->Start[this->cur_stage]) * (delta_time / this->Length[this->cur_stage]) + this->Start[this->cur_stage];
        break;
    case PARABOLIC: {
        float t = this->Finish[this->cur_stage] - this->Start[this->cur_stage];
        float s;
        float x;
        this->CurValue = t * (delta_time / this->Length[this->cur_stage]) * (delta_time / this->Length[this->cur_stage]) + this->Start[this->cur_stage];
        break;
    }
    case INV_PARABOLIC: {
        float t = this->Finish[this->cur_stage] - this->Start[this->cur_stage];
        float s;
        float x;
        this->CurValue = t * (-(delta_time / this->Length[this->cur_stage] - 1.0f) * (delta_time / this->Length[this->cur_stage] - 1.0f) + 1.0f) +
                   this->Start[this->cur_stage];
        break;
    }
    case EQ_PWR_SQ: {
        float t = this->Finish[this->cur_stage] - this->Start[this->cur_stage];
        float s;
        float x;
        int Delta = bClamp(static_cast<int>((delta_time * 32767.0f) / this->Length[this->cur_stage]), 0, 0x7fff);
        float Result = static_cast<float>(NFSMixShape::GetCurveOutput(::SHAPE_UP_EQPWR_SQ, Delta, false));
        Result *= 3.051851e-05f;
        this->CurValue = t * Result + this->Start[this->cur_stage];
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
    this->Length = static_cast<float>(_Length) * 0.001f;
    if (this->Length <= 0.0f) {
        this->Length = 0.01f;
    }
    this->ElapsedTime = 0.0f;
    this->Finish = _Finish;
    this->CurveTypes = _Curve;
    this->bComplete = false;
    this->Start = _Start;
    this->CurValue = _Start;
}

void cInterpLine::Update(float delta_time, float _new_Finish) {
    this->Finish = _new_Finish;
    this->Update(delta_time);
    if (this->bComplete) {
        this->CurValue = _new_Finish;
    }
}

void cInterpLine::Update(float delta_time) {
    if (this->bComplete) {
        return;
    }

    delta_time += this->ElapsedTime;
    this->ElapsedTime = delta_time;

    if (delta_time > this->Length) {
        this->bComplete = true;
        this->CurValue = this->Finish;
        return;
    }

    switch (this->CurveTypes) {
    case LINEAR:
    default:
        this->CurValue = (this->Finish - this->Start) * (delta_time / this->Length) + this->Start;
        break;
    case PARABOLIC: {
        float t = this->Finish - this->Start;
        float s;
        float x;
        this->CurValue = t * (delta_time / this->Length) * (delta_time / this->Length) + this->Start;
        break;
    }
    case INV_PARABOLIC: {
        float t = this->Finish - this->Start;
        float s;
        float x;
        this->CurValue = t * (-(delta_time / this->Length - 1.0f) * (delta_time / this->Length - 1.0f) + 1.0f) + this->Start;
        break;
    }
    case EQ_PWR_SQ: {
        float t = this->Finish - this->Start;
        float s;
        float x;
        int Delta = bClamp(static_cast<int>((delta_time * 32767.0f) / this->Length), 0, 0x7fff);
        float Result = static_cast<float>(NFSMixShape::GetCurveOutput(::SHAPE_UP_EQPWR_SQ, Delta, false));
        Result *= 3.051851e-05f;
        this->CurValue = t * Result + this->Start;
        break;
    }
    }
}

Slope::Slope(float _Min, float _Max, float _Start, float _Finish) {
    this->Initialize(_Min, _Max, _Start, _Finish);
}

Slope::~Slope() {}

void Slope::Initialize(float _Min, float _Max, float _Start, float _Finish) {
    this->Min = _Min;
    this->Max = _Max;
    this->Start = _Start;
    this->Finish = _Finish;
    if (bAbs(_Finish - _Start) < 1e-06f) {
        this->Finish = _Finish + 1e-06f;
    }
    this->LastOutput = this->Min;
    this->LastInput = 0.0f;
}

float Slope::GetValue(float input) {
    this->LastInput = input;
    this->Regenerate();
    return this->LastOutput;
}

void Slope::Regenerate() {
    float Scale = (this->LastInput - this->Start) / (this->Finish - this->Start);
    this->bNeedsRegenerate = false;
    Scale = bClamp(Scale, 0.0f, 1.0f);
    this->LastOutput = Scale * (this->Max - this->Min) + this->Min;
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
