#include "ICEMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

unsigned short ConvertLensLengthToFovAngle(float f_lens_mm) {
    return (bATan(f_lens_mm, 15.96f) & 0x7FFF) << 1;
}

float ConvertFovAngleToLensLength(unsigned short a_fov) {
    unsigned short half_angle = a_fov >> 1;
    float tangent = bSin(half_angle) / bCos(half_angle);

    if (tangent <= 0.0f) {
        tangent = 0.0001f;
    }

    return 15.96f / tangent;
}

float ConvertLensDeltaToFovDelta(float f_lens_mm, float f_lens_slope) {
    short a_fov = bATan(f_lens_mm, 15.96f) << 1;
    short a_fov_desired = bATan(f_lens_mm + f_lens_slope * 0.01f, 15.96f) << 1;
    return static_cast<float>(a_fov_desired - a_fov) * 100.0f;
}

float ConvertApertureNumberToFStop(float aperture) {
    static const float kFStops[] = {
        1.0f,
        1.1224620f,
        1.2599211f,
        1.4142135f,
        1.5874010f,
        1.7817974f,
        2.0f,
        2.2449241f,
        2.5198421f,
        2.8284271f,
        3.1748021f,
        3.5635948f,
        4.0f,
        4.4898481f,
        5.0396843f,
        5.6568542f,
        6.3496041f,
        7.1271896f,
        8.0f,
        8.9796963f,
        10.079369f,
        11.313708f,
        12.699208f,
        14.254379f,
        16.0f,
        17.959393f,
        20.158737f,
        22.627417f,
        25.398417f,
        28.508759f,
        32.0f,
        35.918785f,
        40.317474f,
        45.254833f,
        50.796833f,
        57.017517f,
        64.0f,
    };
    int index = static_cast<int>(aperture + 0.5f);

    if (index < 0) {
        index = 0;
    }
    if (index > 36) {
        index = 36;
    }

    return kFStops[index];
}

ICEAnchor::ICEAnchor()
    : mGeomPos(), //
      mGeomRot(), //
      mVelocity(), //
      mAccel(), //
      mVelMag(0.0f), //
      mTopSpeed(0.0f), //
      mRPM(0.0f), //
      mNumWheels(false), //
      mForwardSlip(0.0f), //
      mSlipAngle(0.0f), //
      mIsTouchingGround(true), //
      mIsNosEngaged(false), //
      mNosPercentageLeft(0.0f) {
    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&mGeomRot));
}

void ICEAnchor::Update(float dT, const ICE::Matrix4 &orientpos, const ICE::Vector3 &velocity, const ICE::Vector3 &) {
    float previous_mag = mVelMag;
    float dist = bDistBetween(reinterpret_cast<const bVector3 *>(&mGeomPos), reinterpret_cast<const bVector3 *>(&orientpos.v3));

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&orientpos), *reinterpret_cast<Mtx44 *>(&mGeomRot));
    mGeomRot.v3.x = 0.0f;
    mGeomRot.v3.y = 0.0f;
    mGeomRot.v3.z = 0.0f;
    mGeomPos.x = orientpos.v3.x;
    mGeomPos.y = orientpos.v3.y;
    mGeomPos.z = orientpos.v3.z;
    mVelocity = velocity;

    {
        float vel_squared = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;

        if (5.0e-11f < vel_squared) {
            float inv_mag = 1.0f / bSqrt(vel_squared);
            inv_mag += -(vel_squared * inv_mag * inv_mag - 1.0f) * inv_mag * 0.5f;
            mVelMag = (-(vel_squared * inv_mag * inv_mag - 1.0f) * inv_mag * 0.5f + inv_mag) * vel_squared;
        } else {
            mVelMag = 0.0f;
        }
    }

    if (dT <= 0.0f || 300.0f <= dist / dT) {
        mAccel.x = 0.0f;
        mAccel.y = 0.0f;
        mAccel.z = 0.0f;
        mAccel.pad = 0.0f;
    } else {
        ICE::Vector3 accel;

        accel.x = (mVelMag - previous_mag) / dT;
        accel.y = 0.0f;
        accel.z = 0.0f;
        accel.pad = 0.0f;
        bMulMatrix(reinterpret_cast<bVector3 *>(&mAccel), reinterpret_cast<const bMatrix4 *>(&mGeomRot), reinterpret_cast<const bVector3 *>(&accel));
    }
}

namespace ICE {

void Cubic1D::MakeCoeffs() {
    float delta = ValDesired - Val;

    Coeff[2] = dVal;
    Coeff[3] = Val;
    Coeff[0] = (dVal + dValDesired) - (delta + delta);
    Coeff[1] = (delta * 3.0f - dValDesired) - (dVal + dVal);
}

float Cubic1D::GetVal(float t) const {
    return ((Coeff[0] * t + Coeff[1]) * t + Coeff[2]) * t + Coeff[3];
}

float Cubic1D::GetdVal(float t) const {
    float value = Coeff[0] * (t * 3.0f) + Coeff[1];
    return (value + Coeff[1]) * t + Coeff[2];
}

float Cubic1D::GetddVal(float t) const {
    float value = Coeff[0] * (t * 6.0f) + Coeff[1];
    return value + Coeff[1];
}

float Cubic1D::GetValDesired() const {
    return ValDesired;
}

float Cubic1D::GetDerivative(float t) const {
    float scale = 1.0f / duration;
    return GetdVal(t * scale) * scale;
}

float Cubic1D::GetSecondDerivative(float t) const {
    float scale = 1.0f / duration;
    return GetddVal(t * scale) * (scale * scale);
}

void Cubic1D::ClampDerivative(float fMag) {
    float deriv = GetDerivative(duration);
    float deriv_abs = bAbs(deriv);

    if (deriv_abs > fMag) {
        dValDesired = (deriv_abs / deriv) * fMag * duration;
    }
}

void Cubic1D::ClampSecondDerivative(float fMag) {
    float acc0 = GetSecondDerivative(0.0f);
    float acc0_abs = bAbs(acc0);
    float acc1 = GetSecondDerivative(duration);
    float acc1_abs = bAbs(acc1);
    bool need_fix = false;

    if (acc0_abs > fMag) {
        acc0 = (acc0_abs / acc0) * fMag;
        need_fix = true;
    }
    if (acc1_abs > fMag) {
        acc1 = (acc1_abs / acc1) * fMag;
        need_fix = true;
    }
    if (need_fix) {
        float duration_squared = duration * duration;
        float start = acc0 * duration_squared;

        Coeff[0] = (acc1 * duration_squared - start) * (1.0f / 6.0f);
        Coeff[1] = start * 0.5f;
    }
}

void Cubic1D::Update(float fSeconds, float fDClamp, float fDDClamp) {
    if (state == 2) {
        time = 0.0f;

        if (flags == 0) {
            state = 1;
        }
        if (0.0f < fDClamp) {
            ClampDerivative(fDClamp);
        }

        MakeCoeffs();

        if (0.0f < fDDClamp) {
            ClampSecondDerivative(fDDClamp);
        }
    }

    if (state == 1 || state == 2) {
        float t = 1.0f;

        if (0.0f < duration) {
            t = time + fSeconds / duration;
        }

        time = t;

        if (1.0f < time) {
            time = 1.0f;
            Snap();
        }

        Val = GetVal(time);
        dVal = GetdVal(time);
    }
}

void Cubic3D::SetVal(const Vector3 *pV) {
    x.SetVal(pV->x);
    y.SetVal(pV->y);
    z.SetVal(pV->z);
}

void Cubic3D::SetdVal(const Vector3 *pV) {
    x.SetdVal(pV->x);
    y.SetdVal(pV->y);
    z.SetdVal(pV->z);
}

void Cubic3D::SetValDesired(const Vector3 *pV) {
    x.SetValDesired(pV->x);
    y.SetValDesired(pV->y);
    z.SetValDesired(pV->z);
}

void Cubic3D::SetdValDesired(const Vector3 *pV) {
    float vx = pV->x;
    float vy = pV->y;
    float vz = pV->z;

    x.dValDesired = vx;
    y.dValDesired = vy;
    z.dValDesired = vz;
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

} // namespace ICE

void ICEMover::DutchCubicInit(ICE::Cubic1D *pDutch) {
    pDutch->SetVal(0.0f);
    pDutch->SetdVal(0.0f);
}

void ICEMover::FovCubicInit(ICE::Cubic1D *pFov) {
    int fov_angle = GetCamera()->GetFov();
    int fov_velocity_angle = GetCamera()->GetVelocityFov();
    float fov = static_cast<float>(fov_velocity_angle) * (1.0f / 30.0f) + static_cast<float>(fov_angle);
    float fov_velocity = static_cast<float>(fov_velocity_angle) * pFov->duration;

    pFov->SetVal(fov);
    pFov->SetdVal(fov_velocity);
}

float ICEMover::GetDutch(float f_param) {
    if (pICEData == 0) {
        goto blend;
    }
    if (pICEData->bCubicEye == 0) {
        goto blend;
    }

    return pDutch->GetVal(f_param);

blend:
    return pDutch->GetVal() * (1.0f - f_param) + pDutch->GetValDesired() * f_param;
}

unsigned short ICEMover::GetFOV(float f_param) {
    if (pICEData == 0) {
        goto blend;
    }
    if (pICEData->bCubicEye == 0) {
        goto blend;
    }

    return static_cast<unsigned short>(pFov->GetVal(f_param));

blend:
    return static_cast<unsigned short>(pFov->GetVal() * (1.0f - f_param) + pFov->GetValDesired() * f_param);
}

ICEAnchor *GetICEAnchor() {
    CameraMover *camera_mover = 0;

    if (&eViews[1] != 0) {
        camera_mover = eViews[1].GetCameraMover();
    }
    if (camera_mover != 0 && camera_mover->GetType() == CM_ICE) {
        return reinterpret_cast<ICEMover *>(camera_mover)->GetICEAnchor();
    }

    return 0;
}
