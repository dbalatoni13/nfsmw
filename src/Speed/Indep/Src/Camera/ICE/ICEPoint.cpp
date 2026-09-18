#include "Speed/Indep/Src/Camera/ICE/ICEPoint.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace ICE {

void Cubic1D::MakeCoeffs() {
    Coeff[0] = (dVal + dValDesired) - 2.0f * (ValDesired - Val);
    Coeff[1] = ((ValDesired - Val) * 3.0f - dValDesired) - (dVal + dVal);
    Coeff[2] = dVal;
    Coeff[3] = Val;
}

float Cubic1D::GetVal(float t) const {
    return ((Coeff[0] * t + Coeff[1]) * t + Coeff[2]) * t + Coeff[3];
}

float Cubic1D::GetdVal(float t) const {
    return ((2.0f * Coeff[1]) + (3.0f * Coeff[0] * t)) * t + Coeff[2];
}

float Cubic1D::GetddVal(float t) const {
    return (2.0f * Coeff[1]) + (6.0f * Coeff[0] * t);
}

float Cubic1D::GetValDesired() const {
    return ValDesired;
}

float Cubic1D::GetDerivative(float t) const {
    float fDG = 1.0f / duration;
    float fG = GetdVal(t * fDG);
    float fDF = fG * fDG;
    return fDF;
}

float Cubic1D::GetSecondDerivative(float t) const {
    float fDG = 1.0f / duration;
    float fG = GetddVal(t * fDG);
    float fDDF = fG * (fDG * fDG);
    return fDDF;
}

void Cubic1D::ClampDerivative(float fMag) {
    float fDf = GetDerivative(duration);
    float fDfAbs = bAbs(fDf);
    if (fDfAbs > fMag) {
        float fSign = fDfAbs / fDf;
        SetdValDesired(fSign * fMag * duration);
    }
}

void Cubic1D::ClampSecondDerivative(float fMag) {
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

void Cubic1D::Update(float fSeconds, float fDClamp, float fDDClamp) {
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
            if (duration > 1e-06f) {
                float interval = fSeconds / duration;
                time += interval;
            } else {
                time = 1.0f;
            }
            if (time > 1.0f) {
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

void Cubic3D::SetVal(const Vector3 *pV) {
    SetVal(pV->x, pV->y, pV->z);
}

void Cubic3D::SetdVal(const Vector3 *pV) {
    SetdVal(pV->x, pV->y, pV->z);
}

void Cubic3D::SetValDesired(const Vector3 *pV) {
    SetValDesired(pV->x, pV->y, pV->z);
}

void Cubic3D::SetdValDesired(const Vector3 *pV) {
    SetdValDesired(pV->x, pV->y, pV->z);
}

void Cubic3D::GetVal(Vector3 *pV) const {
    pV->x = x.Val;
    pV->y = y.Val;
    pV->z = z.Val;
}

void Cubic3D::GetdVal(Vector3 *pV) const {
    pV->x = x.dVal;
    pV->y = y.dVal;
    pV->z = z.dVal;
}

void Cubic3D::GetVal(Vector3 *pV, float t) const {
    pV->x = x.GetVal(t);
    pV->y = y.GetVal(t);
    pV->z = z.GetVal(t);
}

void Cubic3D::GetValDesired(Vector3 *pV) const {
    pV->x = x.ValDesired;
    pV->y = y.ValDesired;
    pV->z = z.ValDesired;
}

void Cubic3D::Update(float fSeconds, float fDClamp, float fDDClamp) {
    x.Update(fSeconds, fDClamp, fDDClamp);
    y.Update(fSeconds, fDClamp, fDDClamp);
    z.Update(fSeconds, fDClamp, fDDClamp);
}

}; // namespace ICE
