#include "Spline.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

void tCubic1D::MakeCoeffs() {
    float diff = ValDesired - Val;
    float d = dVal;
    Coeff[3] = Val;
    Coeff[2] = d;
    Coeff[0] = (d + dValDesired) - (diff + diff);
    Coeff[1] = (diff * 3.0f - dValDesired) - (d + d);
}

float tCubic1D::GetVal(float t) {
    return ((Coeff[0] * t + Coeff[1]) * t + Coeff[2]) * t + Coeff[3];
}

float tCubic1D::GetdVal(float t) {
    return (Coeff[0] * (t * 3.0f) + Coeff[1] + Coeff[1]) * t + Coeff[2];
}

float tCubic1D::GetddVal(float t) {
    return Coeff[0] * (t * 6.0f) + Coeff[1] + Coeff[1];
}

float tCubic1D::GetDerivative(float t) {
    float inv = 1.0f / duration;
    float d = GetdVal(t * inv);
    return d * inv;
}

float tCubic1D::GetSecondDerivative(float t) {
    float inv = 1.0f / duration;
    float dd = GetddVal(t * inv);
    return dd * inv * inv;
}

void tCubic1D::ClampDerivative(float maxDeriv) {
    float d = GetDerivative(duration);
    if (maxDeriv < bAbs(d)) {
        dValDesired = (bAbs(d) / d) * maxDeriv * duration;
    }
}

void tCubic1D::ClampSecondDerivative(float maxSecondDeriv) {
    float dd0 = GetSecondDerivative(0);
    float abs_dd0 = bAbs(dd0);
    float dd1 = GetSecondDerivative(duration);
    float abs_dd1 = bAbs(dd1);
    if (maxSecondDeriv < abs_dd0) {
        dd0 = (abs_dd0 / dd0) * maxSecondDeriv;
    }
    if (maxSecondDeriv < abs_dd1) {
        dd1 = (abs_dd1 / dd1) * maxSecondDeriv;
    }
    if (maxSecondDeriv < abs_dd1 || maxSecondDeriv < abs_dd0) {
        float dur_sq = duration * duration;
        float c1 = dd0 * dur_sq;
        Coeff[1] = c1 * 0.5f;
        Coeff[0] = (dd1 * dur_sq - c1) * 0.16666667f;
    }
}

void tCubic1D::Update(float dt, float maxDeriv, float maxSecondDeriv) {
    if (state != 1) {
        if (state < 2) {
            return;
        }
        if (state != 2) {
            return;
        }
        time = 0;
        if (flags == 0) {
            state = 1;
        }
        if (maxDeriv > 0.0f) {
            ClampDerivative(maxDeriv);
        }
        MakeCoeffs();
        if (maxSecondDeriv > 0.0f) {
            ClampSecondDerivative(maxSecondDeriv);
        }
    }
    float t;
    if (duration > 1e-05f) {
        t = time + dt / duration;
    } else {
        t = 1.0f;
    }
    time = t;
    if (time > 1.0f) {
        time = 1.0f;
        Val = ValDesired;
        dVal = dValDesired;
        state = 0;
    }
    t = time;
    Val = GetVal(t);
    dVal = GetdVal(t);
}

void tCubic2D::SetValDesired(bVector2 *v) {
    float vx = v->x;
    float vy = v->y;
    x.ValDesired = vx;
    if (vx != x.Val) {
        x.state = 2;
    }
    y.ValDesired = vy;
    if (vy != y.Val) {
        y.state = 2;
    }
}

void tCubic2D::GetVal(bVector2 *v) {
    v->x = x.Val;
    v->y = y.Val;
}

void tCubic3D::SetVal(const bVector3 *v) {
    float vx = v->x;
    float vy = v->y;
    float vz = v->z;
    x.Val = vx;
    if (vx != x.ValDesired) {
        x.state = 2;
    }
    y.Val = vy;
    if (vy != y.ValDesired) {
        y.state = 2;
    }
    z.Val = vz;
    if (vz != z.ValDesired) {
        z.state = 2;
    }
}

void tCubic3D::SetdVal(bVector3 *v) {
    float vx = v->x;
    float vy = v->y;
    float vz = v->z;
    x.dVal = vx;
    if (vx != x.dValDesired) {
        x.state = 2;
    }
    y.dVal = vy;
    if (vy != y.dValDesired) {
        y.state = 2;
    }
    z.dVal = vz;
    if (vz != z.dValDesired) {
        z.state = 2;
    }
}

void tCubic3D::SetValDesired(bVector3 *v) {
    float vx = v->x;
    float vy = v->y;
    float vz = v->z;
    x.ValDesired = vx;
    if (vx != x.Val) {
        x.state = 2;
    }
    y.ValDesired = vy;
    if (vy != y.Val) {
        y.state = 2;
    }
    z.ValDesired = vz;
    if (vz != z.Val) {
        z.state = 2;
    }
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