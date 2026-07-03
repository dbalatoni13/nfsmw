#include "Point.hpp"

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
    SplineSeek(&cubic->x, dt, 1.0f, 1.0f);
    SplineSeek(&cubic->y, dt, 1.0f, 1.0f);
}

void tCubic1D::MakeCoeffs() {
    Coeff[0] = (dVal + dValDesired) - 2.0f * (ValDesired - Val);
    Coeff[1] = ((ValDesired - Val) * 3.0f - dValDesired) - (dVal + dVal);
    Coeff[2] = dVal;
    Coeff[3] = Val;
}

float tCubic1D::GetVal(float t) {
    return ((Coeff[0] * t + Coeff[1]) * t + Coeff[2]) * t + Coeff[3];
}

float tCubic1D::GetdVal(float t) {
    return ((2.0f * Coeff[1]) + (3.0f * Coeff[0] * t)) * t + Coeff[2];
}

float tCubic1D::GetddVal(float t) {
    return (2.0f * Coeff[1]) + (6.0f * Coeff[0] * t);
}

float tCubic1D::GetDerivative(float t) {
    float fDG = 1.0f / duration;
    float fG = GetdVal(t * fDG);
    float fDF = fG * fDG;
    return fDF;
}

float tCubic1D::GetSecondDerivative(float t) {
    float fDG = 1.0f / duration;
    float fG = GetddVal(t * fDG);
    float fDDF = fG * (fDG * fDG);
    return fDDF;
}

void tCubic1D::ClampDerivative(float maxDeriv) {
    float fDf = GetDerivative(duration);
    float fDfAbs = bAbs(fDf);
    if (fDfAbs > maxDeriv) {
        float fSign = fDfAbs / fDf;
        SetdValDesired(fSign * maxDeriv * duration);
    }
}

void tCubic1D::ClampSecondDerivative(float fMag) {
    float fAcc0 = GetSecondDerivative(0);
    float fAcc0Abs = bAbs(fAcc0);
    float fAcc1 = GetSecondDerivative(duration);
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
        float fDurationSquared = duration * duration;
        fAcc0 *= fDurationSquared;
        Coeff[1] = fAcc0 * 0.5f;
        Coeff[0] = (fAcc1 * fDurationSquared - fAcc0) / 6.0f;
    }
}

void tCubic1D::Update(float fSeconds, float fDClamp, float fDDClamp) {
    switch (state) {
        case 2: {
            time = 0;
            if (flags == 0) {
                state = 1;
            }
            if (fDClamp > 0.0f) {
                ClampDerivative(fDClamp);
            }
            MakeCoeffs();
            if (fDDClamp > 0.0f) {
                ClampSecondDerivative(fDDClamp);
            }
        }
        case 1: {
            if (duration > 1e-05f) {
                float interval = fSeconds / duration;
                time += interval;
            } else {
                time = 1.0f;
            }
            if (time > 1.0f) {
                time = 1.0f;
                Snap();
            }
            float t = time;
            Val = GetVal(t);
            dVal = GetdVal(t);
            break;
        }
        case 0:
            break;
    }
}

void tCubic2D::SetValDesired(bVector2 *v) {
    SetValDesired(v->x, v->y);
}

void tCubic2D::GetVal(bVector2 *v) {
    v->x = x.Val;
    v->y = y.Val;
}

void tCubic3D::SetVal(const bVector3 *v) {
    SetVal(v->x, v->y, v->z);
}

void tCubic3D::SetdVal(bVector3 *v) {
    SetdVal(v->x, v->y, v->z);
}

void tCubic3D::SetValDesired(bVector3 *v) {
    SetValDesired(v->x, v->y, v->z);
}

void tCubic3D::GetVal(bVector3 *v) {
    v->x = x.Val;
    v->y = y.Val;
    v->z = z.Val;
}

void tCubic3D::GetValDesired(bVector3 *v) {
    v->x = x.ValDesired;
    v->y = y.ValDesired;
    v->z = z.ValDesired;
}

void tCubic3D::Update(float dt, float maxDeriv, float maxSecondDeriv) {
    x.Update(dt, maxDeriv, maxSecondDeriv);
    y.Update(dt, maxDeriv, maxSecondDeriv);
    z.Update(dt, maxDeriv, maxSecondDeriv);
}
