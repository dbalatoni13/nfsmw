#include "Point.hpp"

void cPoint::SplineSeek(tCubic1D *cubic, float dt, float maxDeriv, float maxSecondDeriv) {
    if (cubic->state != 1) {
        if (cubic->state < 2) {
            return;
        }
        if (cubic->state != 2) {
            return;
        }
        cubic->time = 0;
        if (cubic->flags == 0) {
            cubic->state = 1;
        }
        if (maxDeriv > 0.0f) {
            cubic->ClampDerivative(maxDeriv);
        }
        cubic->MakeCoeffs();
        if (maxSecondDeriv > 0.0f) {
            cubic->ClampSecondDerivative(maxSecondDeriv);
        }
    }
    float t = cubic->time + dt / cubic->duration;
    cubic->time = t;
    if (t > 1.0f) {
        cubic->time = 1.0f;
        cubic->Val = cubic->ValDesired;
        cubic->dVal = cubic->dValDesired;
        cubic->state = 0;
    }
    float ct = cubic->time;
    cubic->Val = cubic->GetVal(ct);
    cubic->dVal = cubic->GetdVal(ct);
}

void cPoint::SplineSeek(tCubic2D *cubic, float dt) {
    SplineSeek(&cubic->x, dt, 1.0f, 1.0f);
    SplineSeek(&cubic->y, dt, 1.0f, 1.0f);
}