#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixerDefines.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"

static const int PRINT_SNS_HEAP = 0; // Decl: 13

// STRIPPED
void PrintSndHeapSize(char *StrTxt) {}

// STRIPPED
int CrossFadeVol(int Volume, int Fade) {}

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
    for (int i = 0; i < MAX_NUM_STAGES; i++) {
        this->Length[i] = this->Finish[i] = this->Start[i] = 0.0f;
        this->IsLinked[i] = false;
        this->CurveTypes[i] = LINEAR;
    }
    this->bComplete = false;
}

int cPathLine::AddStage(float _Start, float _Finish, int _Length, eCURVETYPE _Curve) {
    if (this->num_stages == MAX_NUM_STAGES - 1) {
        return -1;
    }

    this->Length[this->num_stages] = static_cast<float>(_Length) / 1000.0f;
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

    this->num_stages++;
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

// STRIPPED
void cPathLine::Update(float delta_time, float _new_Finish) {}

// STRIPPED
void cPathLine::Reset() {}

void cPathLine::Update(float delta_time) {
    if (this->bComplete) {
        return;
    }

    if (this->num_stages == 0) {
        return;
    }

    this->ElapsedTime += delta_time;
    delta_time = this->ElapsedTime;

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
            this->CurValue = this->Start[this->cur_stage] +
                             (this->Finish[this->cur_stage] - this->Start[this->cur_stage]) * (delta_time / this->Length[this->cur_stage]);
            break;
        case PARABOLIC: {
            float t = this->Finish[this->cur_stage] - this->Start[this->cur_stage];
            // TODO use variables
            float s;
            float x;
            this->CurValue =
                t * (delta_time / this->Length[this->cur_stage]) * (delta_time / this->Length[this->cur_stage]) + this->Start[this->cur_stage];
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
            float Result = static_cast<float>(NFSMixShape::GetCurveOutput(SHAPE_UP_EQPWR_SQ, Delta, false));
            Result /= 32767.0f;
            this->CurValue = t * Result + this->Start[this->cur_stage];
            break;
        }
    }
}

cInterpLine::cInterpLine() {
    this->CurValue = 0.0f;
    this->ElapsedTime = 0.0f;
    this->Start = 0.0f;
    this->Finish = 0.0f;
    this->Length = 0.0f;
    this->CurveTypes = LINEAR;
    this->bComplete = true;
}

cInterpLine::~cInterpLine() {}

// UNSOLVED
void cInterpLine::Initialize(float _Start, float _Finish, int _Length, eCURVETYPE _Curve) {
    this->Length = static_cast<float>(_Length) / 1000.0f;
    if (this->Length <= 0.0f) {
        this->Length = 0.01f;
    }

    this->Finish = _Finish;
    this->CurveTypes = _Curve;
    this->bComplete = false;
    this->Start = _Start;
    this->CurValue = _Start;
    this->ElapsedTime = 0.0f;
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

    this->ElapsedTime += delta_time;
    delta_time = this->ElapsedTime;

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
            float Result = static_cast<float>(NFSMixShape::GetCurveOutput(SHAPE_UP_EQPWR_SQ, Delta, false));
            Result /= 32767.0f;
            this->CurValue = t * Result + this->Start;
            break;
        }
    }
}

// STRIPPED
Oscillator::Oscillator() {}

// STRIPPED
Oscillator::Oscillator(float _seperation, float _RollOff) {}

// STRIPPED
Oscillator::~Oscillator() {}

// STRIPPED
void Oscillator::Reset() {}

// STRIPPED
void Oscillator::Update(float t) {}

// STRIPPED
PanEffect::PanEffect() {}

// STRIPPED
PanEffect::~PanEffect() {}

// STRIPPED
void PanEffect::Initialize(sCreatePanEffect _CreatePanStruct) {}

// STRIPPED
void PanEffect::ReInit(sCreatePanEffect _CreatePanStruct) {}

// STRIPPED
void PanEffect::Update(float t, float Velocity) {}

// STRIPPED
void PanEffect::FlipSamples() {}

// STRIPPED
Slope::Slope() {}

Slope::Slope(float _Min, float _Max, float _Start, float _Finish) {
    this->Initialize(_Min, _Max, _Start, _Finish);
}

Slope::~Slope() {}

// UNSOLVED
void Slope::Initialize(float _Min, float _Max, float _Start, float _Finish) {
    this->Min = _Min;
    this->Max = _Max;
    this->Start = _Start;
    this->Finish = _Finish;
    if (bAbs(_Finish - _Start) < UMath::Epsilon) {
        this->Finish = _Finish + UMath::Epsilon;
    }
    this->LastOutput = this->Min;
    this->LastInput = 0.0f;
}

// STRIPPED
float Slope::GetValue() {
    return 0.0f;
}

float Slope::GetValue(float input) {
    this->LastInput = input;
    this->Regenerate();
    return this->LastOutput;
}

// STRIPPED
float Slope::GetInputScale(float input) {
    return 0.0f;
}

void Slope::Update(float input) {}

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

EAX_CarState *GetClosestPlayerCar(const bVector3 *vPosition, bool CameraRelative, int &CarID) {
    float Dist[2] = {-1.0f, -1.0f};

    if (SndCamera::NumPlayers == 0) {
        return nullptr;
    }

    for (int n = 0; n < SndCamera::NumPlayers; n++) {
        if (!CameraRelative) {
            Dist[n] = bDistBetween(SndCamera::GetWorldCarPos3(n), vPosition);
        } else {
            Dist[n] = bDistBetween(SndCamera::GetCamPos3(n), vPosition);
        }
    }

    if (Dist[0] < Dist[1] || Dist[1] < 0.0f) {
        CarID = 0;
        return SndCamera::GetPlayerCar(0)->GetPhysCar();
    }

    if (Dist[1] < Dist[0]) {
        CarID = 1;
        return SndCamera::GetPlayerCar(1)->GetPhysCar();
    }

    CarID = 0;
    return SndCamera::GetPlayerCar(0)->GetPhysCar();
}

EAX_CarState *GetClosestCopCarToCamera() {
    EAX_CarState *closest = nullptr;
    float closest_range = 32767.0f;
    CSTATEMGR_Base *cops = EAXSound::GetStateMgr(eMM_COPCAR);

    for (int n = 0; n < cops->GetStateObjCount(); n++) {
        CSTATE_Base *cop = cops->GetStateObj(n);

        if (cop->IsAttached()) {
            EAX_CarState *copcar = cop->GetPhysCar();
            float range = bDistBetween(SndCamera::GetWorldCarPos3(0), copcar->GetPosition());

            if (range < closest_range) {
                closest = copcar;
            }
        }
    }

    return closest;
}

EAXCar *GetPlayerCarInRadius(bVector3 &objectpos, float distance) {
    EAXCar *pPlayerCar;
    for (int n = 0; n < SndCamera::NumPlayers; n++) {
        bVector3 m_pPlayerPosition(*SndCamera::GetWorldCarPos3(n));
        bVector3 vPlayerDirection;
        bSub(&vPlayerDirection, &m_pPlayerPosition, &objectpos);
        float playerdist = bLength(&vPlayerDirection);

        if (playerdist < distance) {
            pPlayerCar = SndCamera::GetPlayerCar(n);
            return pPlayerCar;
        }
    }

    return nullptr;
}

bool IsCarInRadius(EAX_CarState *pCar, const bVector3 *vPos, float fRadius) {
    if (pCar == nullptr) {
        return false;
    }
    bVector3 *CarPos = pCar->GetPosition();
    return bDistBetween(vPos, CarPos) < fRadius;
}
