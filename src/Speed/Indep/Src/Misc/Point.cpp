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
