#include "Point.hpp"

static const int bCubicDebug = 0; // Decl: 29

float dValFudge = 0.8f; // Decl: 32

// STRIPPED
void tCubic1D::PathdValDesired(float v) {}

void tCubic1D::MakeCoeffs() {
    this->Coeff[0] = (this->dVal + this->dValDesired) - 2.0f * (this->ValDesired - this->Val);
    this->Coeff[1] = ((this->ValDesired - this->Val) * 3.0f - this->dValDesired) - (this->dVal + this->dVal);
    this->Coeff[2] = this->dVal;
    this->Coeff[3] = this->Val;
}

float tCubic1D::GetVal(float t) {
    return ((this->Coeff[0] * t + this->Coeff[1]) * t + this->Coeff[2]) * t + this->Coeff[3];
}

float tCubic1D::GetdVal(float t) {
    return ((2.0f * this->Coeff[1]) + (3.0f * this->Coeff[0] * t)) * t + this->Coeff[2];
}

float tCubic1D::GetddVal(float t) {
    return (2.0f * this->Coeff[1]) + (6.0f * this->Coeff[0] * t);
}

// STRIPPED
float tCubic1D::GetValDesired() {}

// STRIPPED
float tCubic1D::GetdValDesired() {}

float tCubic1D::GetDerivative(float t) {
    float fDG = 1.0f / this->duration;
    float fG = this->GetdVal(t * fDG);
    float fDF = fG * fDG;
    return fDF;
}

float tCubic1D::GetSecondDerivative(float t) {
    float fDG = 1.0f / this->duration;
    float fG = this->GetddVal(t * fDG);
    float fDDF = fG * (fDG * fDG);
    return fDDF;
}

void tCubic1D::ClampDerivative(float maxDeriv) {
    float fDf = this->GetDerivative(this->duration);
    float fDfAbs = bAbs(fDf);
    if (fDfAbs > maxDeriv) {
        float fSign = fDfAbs / fDf;
        SetdValDesired(fSign * maxDeriv * this->duration);
    }
}

void tCubic1D::ClampSecondDerivative(float fMag) {
    float fAcc0 = this->GetSecondDerivative(0);
    float fAcc0Abs = bAbs(fAcc0);
    float fAcc1 = this->GetSecondDerivative(this->duration);
    float fAcc1Abs = bAbs(fAcc1);
    bool bNeedFix = false;
    if (fAcc0Abs > fMag) {
        float fSign = fAcc0Abs / fAcc0;
        fAcc0 = fSign * fMag;
        bNeedFix = true;
    }
    if (fAcc1Abs > fMag) {
        float fSign = fAcc1Abs / fAcc1;
        fAcc1 = fSign * fMag;
        bNeedFix = true;
    }
    if (bNeedFix) {
        float fDurationSquared = this->duration * this->duration;
        fAcc0 *= fDurationSquared;
        this->Coeff[1] = fAcc0 * 0.5f;
        this->Coeff[0] = (fAcc1 * fDurationSquared - fAcc0) / 6.0f;
    }
}

void tCubic1D::Update(float fSeconds, float fDClamp, float fDDClamp) {
    switch (this->state) {
        case 2: {
            this->time = 0;
            if (this->flags == 0) {
                this->state = 1;
            }
            if (fDClamp > 0.0f) {
                this->ClampDerivative(fDClamp);
            }
            this->MakeCoeffs();
            if (fDDClamp > 0.0f) {
                this->ClampSecondDerivative(fDDClamp);
            }
        }
        case 1: {
            if (this->duration > 1e-05f) {
                float interval = fSeconds / this->duration;
                this->time += interval;
            } else {
                this->time = 1.0f;
            }
            if (this->time > 1.0f) {
                this->time = 1.0f;
                this->Snap();
            }
            float t = this->time;
            this->Val = this->GetVal(t);
            this->dVal = this->GetdVal(t);
            break;
        }
        case 0:
            break;
    }
}

// STRIPPED
void tCubic2D::SetVal(const bVector2 *pV) {}

// STRIPPED
void tCubic2D::SetdVal(bVector2 *pV) {}

void tCubic2D::SetValDesired(bVector2 *v) {
    this->SetValDesired(v->x, v->y);
}

// STRIPPED
void tCubic2D::SetdValDesired(bVector2 *pV) {}

// STRIPPED
void tCubic2D::SetDuration(const bVector2 *pV) {}

void tCubic2D::GetVal(bVector2 *v) {
    v->x = this->x.Val;
    v->y = this->y.Val;
}

// STRIPPED
void tCubic2D::GetdVal(bVector2 *pV) {}

// STRIPPED
void tCubic2D::GetddVal(bVector2 *pV) {}

// STRIPPED
void tCubic2D::GetVal(bVector2 *pV, float t) {}

// STRIPPED
void tCubic2D::GetdVal(bVector2 *pV, float t) {}

// STRIPPED
void tCubic2D::GetddVal(bVector2 *pV, float t) {}

// STRIPPED
void tCubic2D::GetValDesired(bVector2 *pV) {}

// STRIPPED
void tCubic2D::GetdValDesired(bVector2 *pV) {}

// STRIPPED
void tCubic2D::Update(float fSeconds, float fDClamp, float fDDClamp) {}

void tCubic3D::SetVal(const bVector3 *v) {
    this->SetVal(v->x, v->y, v->z);
}

void tCubic3D::SetdVal(bVector3 *v) {
    this->SetdVal(v->x, v->y, v->z);
}

void tCubic3D::SetValDesired(bVector3 *v) {
    this->SetValDesired(v->x, v->y, v->z);
}

// STRIPPED
void tCubic3D::SetdValDesired(bVector3 *pV) {}

// STRIPPED
void tCubic3D::SetDuration(const bVector3 *pV) {}

void tCubic3D::GetVal(bVector3 *v) {
    v->x = this->x.Val;
    v->y = this->y.Val;
    v->z = this->z.Val;
}

// STRIPPED
void tCubic3D::GetdVal(bVector3 *pV) {}

// STRIPPED
void tCubic3D::GetddVal(bVector3 *pV) {}

// STRIPPED
void tCubic3D::GetVal(bVector3 *pV, float t) {}

// STRIPPED
void tCubic3D::GetdVal(bVector3 *pV, float t) {}

// STRIPPED
void tCubic3D::GetddVal(bVector3 *pV, float t) {}

void tCubic3D::GetValDesired(bVector3 *v) {
    v->x = this->x.ValDesired;
    v->y = this->y.ValDesired;
    v->z = this->z.ValDesired;
}

// STRIPPED
void tCubic3D::GetdValDesired(bVector3 *pV) {}

void tCubic3D::Update(float dt, float maxDeriv, float maxSecondDeriv) {
    this->x.Update(dt, maxDeriv, maxSecondDeriv);
    this->y.Update(dt, maxDeriv, maxSecondDeriv);
    this->z.Update(dt, maxDeriv, maxSecondDeriv);
}

// STRIPPED
void cPoint::Update(float time) {}

void cPoint::SplineSeek(tCubic1D *p, float time, float fDClamp, float fDDClamp) {
    switch (p->state) {
        case 2: {
            p->time = 0;
            if (p->flags == 0) {
                p->state = 1;
            }
            if (fDClamp > 0.0f) {
                p->ClampDerivative(fDClamp);
            }
            p->MakeCoeffs();
            if (fDDClamp > 0.0f) {
                p->ClampSecondDerivative(fDDClamp);
            }
        }
        case 1: {
            float interval = time / p->duration;
            p->time = p->time + interval;
            if (p->time > 1.0f) {
                p->time = 1.0f;
                p->Snap();
            }
            float t = p->time;
            p->Val = p->GetVal(t);
            p->dVal = p->GetdVal(t);
            break;
        }
        case 0:
            break;
    }
}

void cPoint::SplineSeek(tCubic2D *cubic, float dt) {
    SplineSeek(&cubic->x, dt, 0.0f, 0.0f);
    SplineSeek(&cubic->y, dt, 0.0f, 0.0f);
}

// STRIPPED
void cPoint::SplineSeek(tCubic3D *p, float time) {}

// STRIPPED
void cPoint::SpringDamper(tCubic1D *p, float time) {}

// STRIPPED
void cPoint::SpringDamper(tCubic2D *p, float time) {}

// STRIPPED
void cPoint::SpringDamper(tCubic3D *p, float time) {}

// STRIPPED
// this: r29
void cPoint::BuildMatrix(bMatrix4 *pMatrix) {}

// STRIPPED
void cPoint::PointSpline(cPoint *p, float time) {}

// STRIPPED
void cPoint::PointVelocity(cPoint *p, float time) {}

// STRIPPED
void cPoint::PointVelocityWrap(cPoint *p, float time) {}

// STRIPPED
void cPoint::PointSpringDamper(cPoint *p, float time) {}

// STRIPPED
void cPoint::SetPos(bVector3 *p) {}

// STRIPPED
void cPoint::SetPos(float x, float y, float z) {}

// STRIPPED
void cPoint::SetPosX(float x) {}

// STRIPPED
void cPoint::SetPosY(float y, bool ratio) {}

// STRIPPED
void cPoint::SetPosZ(float z) {}

// STRIPPED
void cPoint::SetPosDesired(bVector3 *p) {}

// STRIPPED
void cPoint::SetPosDesired(float x, float y, float z) {}

// STRIPPED
void cPoint::SetPosDesiredX(float x) {}

// STRIPPED
void cPoint::SetPosDesiredY(float y, bool ratio) {}

// STRIPPED
void cPoint::SetPosDesiredZ(float z) {}

// STRIPPED
void cPoint::SetdPos(bVector3 *p) {}

// STRIPPED
void cPoint::SetdPos(float x, float y, float z) {}

// STRIPPED
void cPoint::SetdPosX(float x) {}

// STRIPPED
void cPoint::SetdPosY(float y) {}

// STRIPPED
void cPoint::SetdPosZ(float z) {}

// STRIPPED
void cPoint::SetdPosDesired(bVector3 *p) {}

// STRIPPED
void cPoint::SetdPosDesired(float x, float y, float z) {}

// STRIPPED
void cPoint::SetdPosDesiredX(float x) {}

// STRIPPED
void cPoint::SetdPosDesiredY(float y) {}

// STRIPPED
void cPoint::SetdPosDesiredZ(float z) {}

// STRIPPED
void cPoint::SetRot(bVector3 *p) {}

// STRIPPED
void cPoint::SetRot(float x, float y, float z) {}

// STRIPPED
void cPoint::SetRotX(float x) {}

// STRIPPED
void cPoint::SetRotY(float y) {}

// STRIPPED
void cPoint::SetRotZ(float z) {}

// STRIPPED
void cPoint::SetRotDesired(bVector3 *p) {}

// STRIPPED
void cPoint::SetRotDesired(float x, float y, float z) {}

// STRIPPED
void cPoint::SetRotDesiredX(float x) {}

// STRIPPED
void cPoint::SetRotDesiredY(float y) {}

// STRIPPED
void cPoint::SetRotDesiredZ(float z) {}

// STRIPPED
void cPoint::SetdRot(bVector3 *p) {}

// STRIPPED
void cPoint::SetdRot(float x, float y, float z) {}

// STRIPPED
void cPoint::SetdRotX(float x) {}

// STRIPPED
void cPoint::SetdRotY(float y) {}

// STRIPPED
void cPoint::SetdRotZ(float z) {}

// STRIPPED
void cPoint::SetdRotDesired(bVector3 *p) {}

// STRIPPED
void cPoint::SetdRotDesired(float x, float y, float z) {}

// STRIPPED
void cPoint::SetdRotDesiredX(float x) {}

// STRIPPED
void cPoint::SetdRotDesiredY(float y) {}

// STRIPPED
void cPoint::SetdRotDesiredZ(float z) {}

// STRIPPED
void cPoint::SetScale(float s) {}

// STRIPPED
void cPoint::SetdScale(float s) {}

// STRIPPED
void cPoint::SetYScale(float s) {}

// STRIPPED
void cPoint::SetdYScale(float s) {}

// STRIPPED
void cPoint::SetScaleDesired(float s) {}

// STRIPPED
void cPoint::SetdScaleDesired(float s) {}

// STRIPPED
void cPoint::SetYScaleDesired(float s) {}

// STRIPPED
void cPoint::SetdYScaleDesired(float s) {}

// STRIPPED
void cPoint::SetDuration(float seconds) {}

// STRIPPED
void cPoint::Snap() {}

// STRIPPED
void cPoint::SetBiasZ(float ratio) {}

// STRIPPED
// TODO values
cPoint::cPoint()
    : mPos(0, 0.0f),   //
      mRot(0, 0.0f),   //
      mScale(0, 0.0f), //
      mYScale(0, 0.0f) {}

// STRIPPED
cPoint::~cPoint() {}
