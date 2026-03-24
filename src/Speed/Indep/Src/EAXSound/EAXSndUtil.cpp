#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

class NFSMixShape {
  public:
    enum eMIXTABLEID { SHAPE_UP_EQPWR_SQ = 0 };
    static int GetCurveOutput(eMIXTABLEID id, int nQ15Ratio, bool invert);
};

cPathLine::cPathLine() {
    ClearStages();
    bComplete = true;
}

cPathLine::~cPathLine() {}

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

void cPathLine::Initialize(float _Start, float _Finish, int _Length) {
    ClearStages();
    AddStage(_Start, _Finish, _Length, LINEAR);
    CurValue = _Start;
}

int cPathLine::AddStage(float _Start, float _Finish, int _Length, eCURVETYPE _Curve) {
    if (num_stages == 5) {
        return -1;
    }

    Length[num_stages] = static_cast<float>(_Length) * 0.001f;
    if (Length[num_stages] <= 0.0f) {
        Length[num_stages] = 0.01f;
    }

    IsLinked[num_stages] = 0;
    Finish[num_stages] = _Finish;
    Start[num_stages] = _Start;
    CurveTypes[num_stages] = _Curve;
    *(int *)&bComplete = 0;

    if (num_stages == 0) {
        CurValue = Start[0];
    }

    num_stages = num_stages + 1;
    return num_stages;
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

void cPathLine::Update(float delta_time) {
    if (*(int *)&bComplete != 0) {
        return;
    }

    if (num_stages == 0) {
        return;
    }

    int stage = cur_stage;
    float elapsed = ElapsedTime + delta_time;
    ElapsedTime = elapsed;
    float length = Length[stage];

    if (length < elapsed) {
        CurValue = Finish[stage];
        if (stage < num_stages - 1) {
            length = Length[stage];
            cur_stage = stage + 1;
            ElapsedTime = elapsed - length;
            if (IsLinked[stage + 1] != 0) {
                Start[stage + 1] = Finish[stage];
            }
        } else {
            *(int *)&bComplete = 1;
        }
        return;
    }

    eCURVETYPE curve = CurveTypes[stage];
    if (curve == PARABOLIC) {
        float t = elapsed / length;
        float s = Finish[stage] - Start[stage];
        float x = Start[stage];
        CurValue = s * t * t + x;
        return;
    }

    if (curve <= 1) {
        float t = elapsed / length;
        float s = Finish[stage] - Start[stage];
        float x = Start[stage];
        CurValue = s * t + x;
        return;
    }

    if (curve == INV_PARABOLIC) {
        float t = elapsed / length - 1.0f;
        float s = Finish[stage] - Start[stage];
        float x = Start[stage];
        CurValue = s * (-t * t + 1.0f) + x;
        return;
    }

    if (curve == EQ_PWR_SQ) {
        float t = Start[stage];
        float s = Finish[stage];
        float x = Start[cur_stage];
        int Delta = static_cast<int>((elapsed * 32767.0f) / length);
        float Result = static_cast<float>(NFSMixShape::GetCurveOutput(
            static_cast<NFSMixShape::eMIXTABLEID>(3), bClamp(Delta, 0, 0x7fff), false));
        CurValue = (s - t) * Result * 3.051851e-05f + x;
            return;
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
    if (*(int *)&bComplete != 0) {
        return;
    }

    float length = Length;
    float elapsed = ElapsedTime + delta_time;
    ElapsedTime = elapsed;

    if (length < elapsed) {
        *(int *)&bComplete = 1;
        CurValue = Finish;
        return;
    }

    eCURVETYPE curve = CurveTypes;
    if (curve == PARABOLIC) {
        CurValue = Start + (Finish - Start) * (elapsed / length) * (elapsed / length);
        return;
    }

    if (curve > 1) {
        if (curve == INV_PARABOLIC) {
            float ratio = elapsed / length - 1.0f;
            CurValue = Start + (Finish - Start) * (-ratio * ratio + 1.0f);
            return;
        }
        if (curve == EQ_PWR_SQ) {
            float start = Start;
            float finish = Finish;
            int nQ15Ratio = static_cast<int>((elapsed * 32767.0f) / length);
            if (nQ15Ratio < 0) {
                nQ15Ratio = 0;
            }
            if (nQ15Ratio > 0x7fff) {
                nQ15Ratio = 0x7fff;
            }
            int curveOutput = NFSMixShape::GetCurveOutput(NFSMixShape::SHAPE_UP_EQPWR_SQ, nQ15Ratio, false);
            CurValue = Start + (finish - start) * static_cast<float>(curveOutput) * 3.051851e-05f;
            return;
        }
    }

    CurValue = Start + (Finish - Start) * (elapsed / length);
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
    LastInput = 0.0f;
    LastOutput = Min;
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
    const bVector3 *carPos = static_cast<const bVector3 *>(static_cast<const void *>(&pCar->mMatrix.v3));
    float dist = bDistBetween(vPos, carPos);
    return dist < fRadius;
}
