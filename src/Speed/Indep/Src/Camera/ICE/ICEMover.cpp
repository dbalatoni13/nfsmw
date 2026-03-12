#include "ICEMover.hpp"
#include "ICEManager.hpp"
#include "ICEAnimScene.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

namespace ICE {
void HideOverlay();
void ShowOverlay(unsigned char overlay);
ICEScene *FindAnimScene();
bool KeysShared(ICEData *p1, int n1, ICEData *p2, int n2);
void FireEventTag(int key);
}

void FlushAccumulationBuffer();
extern bool bMirrorICEData;
extern ICE::Vector3 vIceAccelLagScale;
extern ICE::Vector3 vIceAccelLagMin;
extern ICE::Vector3 vIceAccelLagMax;

bVector3 *bCross(bVector3 *dest, const bVector3 *v1, const bVector3 *v2);
void bQuaternionToMatrix(bMatrix4 *matrix, const bQuaternion *quaternion);

void bQuaternion::GetMatrix(bMatrix4 &mat) const {
    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    float sx = w * x2;
    float sy = w * y2;
    float sz = w * z2;
    mat.v0.x = 1.0f - yy - zz;
    mat.v0.y = xy + sz;
    mat.v0.z = xz - sy;
    mat.v0.w = 0.0f;
    mat.v1.x = xy - sz;
    mat.v1.y = 1.0f - xx - zz;
    mat.v1.z = yz + sx;
    mat.v1.w = 0.0f;
    mat.v2.x = xz + sy;
    mat.v2.y = yz - sx;
    mat.v2.z = 1.0f - xx - yy;
    mat.v2.w = 0.0f;
    mat.v3.x = 0.0f;
    mat.v3.y = 0.0f;
    mat.v3.z = 0.0f;
    mat.v3.w = 1.0f;
}

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
    unsigned short a = bATan(f_lens_mm, 15.96f) << 1;
    unsigned short b = bATan(f_lens_mm + f_lens_slope * 0.01f, 15.96f) << 1;
    return (SignExtendAng(b) - SignExtendAng(a)) * 100.0f;
}

float ConvertApertureNumberToFStop(float aperture) {
    const float kFStops[] = {
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
    int n = static_cast<int>(aperture + 0.5f);
    if (n < 0) {
        n = 0;
    }
    if (n > 36) {
        n = 36;
    }

    return kFStops[n];
}

static void CreateLookAtMatrix(ICE::Matrix4 *mat, ICE::Vector3 &eye, ICE::Vector3 &center, unsigned short dutch) {
    UMath::Vector3 c;
    UMath::Vector3 vUp;
    UMath::Vector3 b;
    UMath::Vector3 a;
    UMath::Matrix4 tl;

    c.x = center.x - eye.x;
    c.y = center.y - eye.y;
    c.z = center.z - eye.z;
    bNormalize(reinterpret_cast<bVector3 *>(&c), reinterpret_cast<const bVector3 *>(&c));

    vUp.x = 0.0f;
    vUp.y = 0.0f;
    vUp.z = 1.0f;

    bCross(reinterpret_cast<bVector3 *>(&b), reinterpret_cast<const bVector3 *>(&vUp), reinterpret_cast<const bVector3 *>(&c));
    bCross(reinterpret_cast<bVector3 *>(&a), reinterpret_cast<const bVector3 *>(&c), reinterpret_cast<const bVector3 *>(&b));
    bNormalize(reinterpret_cast<bVector3 *>(&b), reinterpret_cast<const bVector3 *>(&b));
    bNormalize(reinterpret_cast<bVector3 *>(&a), reinterpret_cast<const bVector3 *>(&a));

    bMatrix4 *m = reinterpret_cast<bMatrix4 *>(mat);
    m->v0.x = b.x;
    m->v0.y = a.x;
    m->v0.z = c.x;
    m->v0.w = 0.0f;
    m->v1.x = b.y;
    m->v1.y = a.y;
    m->v1.z = c.y;
    m->v1.w = 0.0f;
    m->v2.x = b.z;
    m->v2.y = a.z;
    m->v2.z = c.z;
    m->v2.w = 0.0f;
    m->v3.x = 0.0f;
    m->v3.y = 0.0f;
    m->v3.z = 0.0f;
    m->v3.w = 1.0f;

    eRotateZ(m, m, dutch);

    tl.v0.x = 1.0f;
    tl.v0.y = 0.0f;
    tl.v0.z = 0.0f;
    tl.v0.w = 0.0f;
    tl.v1.x = 0.0f;
    tl.v1.y = 1.0f;
    tl.v1.z = 0.0f;
    tl.v1.w = 0.0f;
    tl.v2.x = 0.0f;
    tl.v2.y = 0.0f;
    tl.v2.z = 1.0f;
    tl.v2.w = 0.0f;
    tl.v3.x = -eye.x;
    tl.v3.y = -eye.y;
    tl.v3.z = -eye.z;
    tl.v3.w = 1.0f;

    bMulMatrix(m, m, reinterpret_cast<const bMatrix4 *>(&tl));
}

ICEMover::ICEMover(int nView, ICEAnchor *pCar)
    : CameraMover(nView, CM_ICE), //
      pCar(pCar), //
      mHybridToWorld(), //
      vSmoothCarPos(), //
      vSmoothCarFwd() {
    GetCamera()->ClearVelocity();
    pEye = new (__FILE__, __LINE__) ICE::Cubic3D(0, 1.0f);
    pLook = new (__FILE__, __LINE__) ICE::Cubic3D(0, 1.0f);
    pDutch = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pFov = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pNearClip = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pNoiseAmplitude = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pNoiseFrequency = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pFocalDistance = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pAperture = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pLetterbox = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pSimSpeed = new (__FILE__, __LINE__) ICE::Cubic1D(0, 1.0f);
    pAccelOffset = new (__FILE__, __LINE__) ICE::Cubic3D(1, 1.0f);
    pICEData = 0;
    nSpaceEye = 0;
    nSpaceLook = 0;
    fParameter0 = 0.0f;
    bViolatesTopology = false;
    fParameter1 = 0.0f;
    ICE::HideOverlay();
    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&mHybridToWorld));
    bCopy(reinterpret_cast<bVector3 *>(&vSmoothCarPos), reinterpret_cast<const bVector3 *>(pCar->GetGeometryPosition()));
    bCopy(reinterpret_cast<bVector3 *>(&vSmoothCarFwd), reinterpret_cast<const bVector3 *>(pCar->GetForwardVector()));
    SetDesired(true, true);
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
    float dist = bDistBetween(reinterpret_cast<const bVector3 *>(&mGeomPos), reinterpret_cast<const bVector3 *>(&orientpos.v3));

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&orientpos), *reinterpret_cast<Mtx44 *>(&mGeomRot));
    float savedVelMag = mVelMag;
    mGeomRot.v3.z = 0.0f;
    mGeomRot.v3.y = 0.0f;
    mGeomRot.v3.x = 0.0f;
    bCopy(reinterpret_cast<bVector3 *>(&mGeomPos), reinterpret_cast<const bVector3 *>(&orientpos.v3));
    bCopy(reinterpret_cast<bVector3 *>(&mVelocity), reinterpret_cast<const bVector3 *>(&velocity));

    mVelMag = bLength(reinterpret_cast<const bVector3 *>(&velocity));

    if (dT > 0.0f && dist / dT < 300.0f) {
        ICE::Vector3 acc((mVelMag - savedVelMag) / dT, 0.0f, 0.0f);
        bMulMatrix(reinterpret_cast<bVector3 *>(&mAccel), reinterpret_cast<const bMatrix4 *>(&mGeomRot), reinterpret_cast<const bVector3 *>(&acc));
    } else {
        mAccel.x = 0.0f;
        mAccel.y = 0.0f;
        mAccel.z = 0.0f;
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
    return (Coeff[0] * 3.0f * t + (Coeff[1] + Coeff[1])) * t + Coeff[2];
}

float Cubic1D::GetddVal(float t) const {
    return Coeff[0] * (t * 6.0f) + (Coeff[1] + Coeff[1]);
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
    float fAcc0 = GetSecondDerivative(0.0f);
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
        float start = fAcc0 * fDurationSquared;

        Coeff[0] = (fAcc1 * fDurationSquared - start) * (1.0f / 6.0f);
        Coeff[1] = start * 0.5f;
    }
}

void Cubic1D::Update(float fSeconds, float fDClamp, float fDDClamp) {
    if (state == 1) {
        goto update;
    }
    if (state <= 1) {
        return;
    }
    if (state != 2) {
        return;
    }

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

update:
    {
        float t;

        if (0.0f < duration) {
            float interval = fSeconds / duration;
            time = time + interval;
        } else {
            time = 1.0f;
        }

        if (1.0f < time) {
            time = 1.0f;
            Snap();
        }

        t = time;
        Val = GetVal(t);
        dVal = GetdVal(t);
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

} // namespace ICE

void ICEMover::EyeCubicInit(ICE::Cubic3D *pEye, ICE::Matrix4 *pMatrix, ICE::Vector3 *pVelocity) {
    ICE::Vector3 vEye;
    ICE::Vector3 vEyeRel;
    UMath::Vector4 vEyeVel;

    bScaleAdd(reinterpret_cast<bVector3 *>(&vEye),
              GetCamera()->GetPosition(),
              GetCamera()->GetVelocityPosition(),
              1.0f / 30.0f);
    if (pMatrix != 0) {
        bMulMatrix(reinterpret_cast<bVector3 *>(&vEye), reinterpret_cast<const bMatrix4 *>(pMatrix), reinterpret_cast<const bVector3 *>(&vEye));
    }
    pEye->SetVal(&vEye);

    bVector3 *velPos = GetCamera()->GetVelocityPosition();
    vEyeRel.x = velPos->x;
    vEyeRel.y = velPos->y;
    vEyeRel.z = velPos->z;
    if (pVelocity != 0) {
        bSub(reinterpret_cast<bVector3 *>(&vEyeRel), reinterpret_cast<const bVector3 *>(&vEyeRel), reinterpret_cast<const bVector3 *>(pVelocity));
    }
    float dur = pEye->x.duration;
    bScale(reinterpret_cast<bVector3 *>(&vEyeVel), reinterpret_cast<const bVector3 *>(&vEyeRel), dur);
    vEyeVel.w = 0.0f;
    if (pMatrix != 0) {
        bMulMatrix(reinterpret_cast<bVector4 *>(&vEyeVel), reinterpret_cast<const bMatrix4 *>(pMatrix), reinterpret_cast<const bVector4 *>(&vEyeVel));
    }
    pEye->SetdVal(reinterpret_cast<ICE::Vector3 *>(&vEyeVel));
}

void ICEMover::LookCubicInit(ICE::Cubic3D *pLook, ICE::Matrix4 *pMatrix, ICE::Vector3 *pVelocity) {
    ICE::Vector3 vLook;
    ICE::Vector3 vLookRel;
    UMath::Vector4 vLookVel;

    bScaleAdd(reinterpret_cast<bVector3 *>(&vLook),
              GetCamera()->GetTarget(),
              GetCamera()->GetVelocityTarget(),
              1.0f / 30.0f);
    if (pMatrix != 0) {
        bMulMatrix(reinterpret_cast<bVector3 *>(&vLook), reinterpret_cast<const bMatrix4 *>(pMatrix), reinterpret_cast<const bVector3 *>(&vLook));
    }
    pLook->SetVal(&vLook);

    bVector3 *velTgt = GetCamera()->GetVelocityTarget();
    vLookRel.x = velTgt->x;
    vLookRel.y = velTgt->y;
    vLookRel.z = velTgt->z;
    if (pVelocity != 0) {
        bSub(reinterpret_cast<bVector3 *>(&vLookRel), reinterpret_cast<const bVector3 *>(&vLookRel), reinterpret_cast<const bVector3 *>(pVelocity));
    }
    float dur = pLook->x.duration;
    bScale(reinterpret_cast<bVector3 *>(&vLookVel), reinterpret_cast<const bVector3 *>(&vLookRel), dur);
    vLookVel.w = 0.0f;
    if (pMatrix != 0) {
        bMulMatrix(reinterpret_cast<bVector4 *>(&vLookVel), reinterpret_cast<const bMatrix4 *>(pMatrix), reinterpret_cast<const bVector4 *>(&vLookVel));
    }
    pLook->SetdVal(reinterpret_cast<ICE::Vector3 *>(&vLookVel));
}

void ICEMover::DutchCubicInit(ICE::Cubic1D *pDutch) {
    pDutch->SetVal(0.0f);
    pDutch->SetdVal(0.0f);
}

void ICEMover::FovCubicInit(ICE::Cubic1D *pFov) {
    float fFov = static_cast<float>(static_cast<int>(GetCamera()->GetFov())) + static_cast<float>(static_cast<int>(GetCamera()->GetVelocityFov())) * (1.0f / 30.0f);
    float fFovVel = static_cast<float>(static_cast<int>(GetCamera()->GetVelocityFov())) * pFov->duration;

    pFov->SetVal(fFov);
    pFov->SetdVal(fFovVel);
}

void ICEMover::SetDesired(bool b_snap, bool b_refresh) {
    ICEData *pOldCameraData = pICEData;
    ICETrack *p_track = 0;
    ICEData *pCameraData = TheICEManager.GetCameraData(&p_track, &fParameter0, &fParameter1);

    bool b_new_camera = (pICEData != pCameraData);

    if (!b_refresh && !b_new_camera) {
        return;
    }

    int hard_cut = 0;
    if (b_new_camera && pCameraData != 0 && pOldCameraData != 0) {
        hard_cut = (ICE::KeysShared(pOldCameraData, 1, pCameraData, 0) == 0);
    }

    int cond1 = 0;
    if (pCameraData != 0 && pICEData == 0 && !(pCameraData->bSmooth & 1)) {
        cond1 = 1;
    }
    int flush = (hard_cut | cond1) != 0;

    int cond2 = 0;
    if (pICEData != 0 && pCameraData == 0 && !(pICEData->bSmooth & 1)) {
        cond2 = 1;
    }
    flush = (flush | cond2) != 0;

    pICEData = pCameraData;

    if (flush) {
        FlushAccumulationBuffer();
        bCopy(reinterpret_cast<bVector3 *>(&vSmoothCarPos),
              reinterpret_cast<const bVector3 *>(pCar->GetGeometryPosition()));
        bCopy(reinterpret_cast<bVector3 *>(&vSmoothCarFwd),
              reinterpret_cast<const bVector3 *>(pCar->GetForwardVector()));
    }

    if (b_new_camera) {
        int key = TheICEManager.GetCameraIndex((fParameter0 + fParameter1) * 0.5f, p_track);
        ICE::FireEventTag(key);
    }

    if (pCameraData == 0) {
        return;
    }

    nSpaceEye = pCameraData->nSpaceEye;
    nSpaceLook = pCameraData->nSpaceLook;

    {
        ICE::Vector3 v_eye[2];
        ICE::Vector3 v_look[2];
        ICE::Vector3 v_eye_slope[2];
        ICE::Vector3 v_look_slope[2];
        float f_dutch_slope[2];
        float f_lens_slope[2];

        pCameraData->GetEye(0, &v_eye[0]);
        pCameraData->GetEye(1, &v_eye[1]);
        pCameraData->GetLook(0, &v_look[0]);
        pCameraData->GetLook(1, &v_look[1]);

        TheICEManager.GetSlope(&v_eye_slope[0], &v_look_slope[0], &f_lens_slope[0], &f_dutch_slope[0], pCameraData, 0, p_track);
        TheICEManager.GetSlope(&v_eye_slope[1], &v_look_slope[1], &f_lens_slope[1], &f_dutch_slope[1], pCameraData, 1, p_track);

        pEye->SetVal(&v_eye[0]);
        pEye->SetdVal(&v_eye_slope[0]);
        pEye->SetValDesired(&v_eye[1]);
        pEye->SetdValDesired(&v_eye_slope[1]);

        pLook->SetVal(&v_look[0]);
        pLook->SetdVal(&v_look_slope[0]);
        pLook->SetValDesired(&v_look[1]);
        pLook->SetdValDesired(&v_look_slope[1]);

        pDutch->SetVal(pCameraData->fDutch[0]);
        pDutch->SetdVal(f_dutch_slope[0]);
        pDutch->SetValDesired(pCameraData->fDutch[1]);
        pDutch->dValDesired = f_dutch_slope[1];

        pFov->SetVal(static_cast<float>(ConvertLensLengthToFovAngle(pCameraData->fLens[0])));
        pFov->SetdVal(ConvertLensDeltaToFovDelta(pCameraData->fLens[0], f_lens_slope[0]));
        pFov->SetValDesired(static_cast<float>(ConvertLensLengthToFovAngle(pCameraData->fLens[1])));
        pFov->dValDesired = ConvertLensDeltaToFovDelta(pCameraData->fLens[1], f_lens_slope[1]);

        if (flush) {
            PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&mHybridToWorld));
            bCopy(reinterpret_cast<bMatrix4 *>(&mHybridToWorld),
                  reinterpret_cast<const bMatrix4 *>(pCar->GetGeometryOrientation()),
                  reinterpret_cast<const bVector3 *>(pCar->GetGeometryPosition()));
        }

        if (b_new_camera) {
            bMirrorICEData = (pCameraData->bSmooth >> 1) & 1;

            if ((pCameraData->bSmooth & 1) != 0) {
                TheICEManager.SetSmoothExit(true);
                {
                    ICE::Matrix4 mCarToWorld;
                    ICE::Matrix4 mWorldToCar;

                    bCopy(reinterpret_cast<bMatrix4 *>(&mCarToWorld),
                          reinterpret_cast<const bMatrix4 *>(pCar->GetGeometryOrientation()),
                          reinterpret_cast<const bVector3 *>(pCar->GetGeometryPosition()));

                    eInvertTransformationMatrix(reinterpret_cast<bMatrix4 *>(&mWorldToCar),
                                                reinterpret_cast<const bMatrix4 *>(&mCarToWorld));

                    ICE::Matrix4 mWorldToHybrid;

                    eInvertTransformationMatrix(reinterpret_cast<bMatrix4 *>(&mWorldToHybrid),
                                                reinterpret_cast<const bMatrix4 *>(&mHybridToWorld));

                    ICE::Cubic3D eye(1, 1.0f);
                    ICE::Cubic3D look(1, 1.0f);
                    ICE::Cubic1D dutch(1, 1.0f);
                    ICE::Cubic1D fov(1, 1.0f);

                    ICE::Matrix4 mWorldToScene;
                    ICE::Matrix4 *pWorldToScene = 0;
                    ICE::Matrix4 *eye_space;
                    ICE::Matrix4 *look_space;

                    if (nSpaceEye == 3 || nSpaceLook == 3) {
                        ICEScene *scene = ICE::FindAnimScene();
                        if (scene != 0) {
                            bMatrix4 &sceneMat = scene->GetSceneTransformMatrix();
                            pWorldToScene = &mWorldToScene;
                            eInvertTransformationMatrix(reinterpret_cast<bMatrix4 *>(pWorldToScene), &sceneMat);
                        }
                    }

                    switch (nSpaceEye) {
                        case 0: eye_space = &mWorldToCar; break;
                        case 2: eye_space = &mWorldToHybrid; break;
                        case 3: eye_space = pWorldToScene; break;
                        default: eye_space = 0; break;
                    }

                    switch (nSpaceLook) {
                        case 0: look_space = &mWorldToCar; break;
                        case 2: look_space = &mWorldToHybrid; break;
                        case 3: look_space = pWorldToScene; break;
                        default: look_space = 0; break;
                    }

                    ICE::Vector3 *eye_vel = (nSpaceEye == 0) ? pCar->GetVelocity() : static_cast<ICE::Vector3 *>(0);
                    ICE::Vector3 *look_vel = (nSpaceLook == 0) ? pCar->GetVelocity() : static_cast<ICE::Vector3 *>(0);

                    EyeCubicInit(&eye, eye_space, eye_vel);
                    LookCubicInit(&look, look_space, look_vel);
                    DutchCubicInit(&dutch);
                    FovCubicInit(&fov);

                    float f_route_param = TheICEManager.GetParameter();
                    float f_to_start = bAbs(f_route_param - fParameter0);
                    float f_to_end = bAbs(f_route_param - fParameter1);

                    if (f_to_end <= f_to_start) {
                        ICE::Vector3 v_eye_val;
                        ICE::Vector3 v_deye;
                        ICE::Vector3 v_look_val;
                        ICE::Vector3 v_dlook;

                        eye.GetVal(&v_eye_val);
                        eye.GetdVal(&v_deye);
                        pEye->SetVal(&v_eye_val);
                        pEye->SetdVal(&v_deye);

                        look.GetVal(&v_look_val);
                        look.GetdVal(&v_dlook);
                        pLook->SetVal(&v_look_val);
                        pLook->SetdVal(&v_dlook);

                        pDutch->SetVal(dutch.GetVal());
                        pDutch->SetdVal(dutch.GetdVal());

                        pFov->SetVal(fov.GetVal());
                        pFov->SetdVal(fov.GetdVal());
                    } else {
                        ICE::Vector3 v_eye_val;
                        ICE::Vector3 v_deye;
                        ICE::Vector3 v_look_val;
                        ICE::Vector3 v_dlook;

                        eye.GetVal(&v_eye_val);
                        eye.GetdVal(&v_deye);
                        pEye->SetValDesired(&v_eye_val);
                        pEye->SetdValDesired(&v_deye);

                        look.GetVal(&v_look_val);
                        look.GetdVal(&v_dlook);
                        pLook->SetValDesired(&v_look_val);
                        pLook->SetdValDesired(&v_dlook);

                        pDutch->SetValDesired(dutch.GetVal());
                        pDutch->dValDesired = dutch.GetdVal();

                        pFov->SetValDesired(fov.GetVal());
                        pFov->dValDesired = fov.GetdVal();
                    }
                }
            }
        }
    }

    pEye->MakeCoeffs();
    pLook->MakeCoeffs();
    pDutch->MakeCoeffs();
    pFov->MakeCoeffs();

    pNearClip->SetVal(pCameraData->fNearClip[0]);
    pNearClip->SetValDesired(pCameraData->fNearClip[1]);
    pNearClip->MakeCoeffs();

    pNoiseAmplitude->SetVal(pCameraData->fNoiseAmplitude[0]);
    pNoiseAmplitude->SetValDesired(pCameraData->fNoiseAmplitude[1]);
    pNoiseAmplitude->MakeCoeffs();

    pNoiseFrequency->SetVal(pCameraData->fNoiseFrequency[0]);
    pNoiseFrequency->SetValDesired(pCameraData->fNoiseFrequency[1]);
    pNoiseFrequency->MakeCoeffs();

    pFocalDistance->SetVal(pCameraData->fFocalDistance[0]);
    pFocalDistance->SetValDesired(pCameraData->fFocalDistance[1]);
    pFocalDistance->MakeCoeffs();

    pAperture->SetVal(static_cast<float>(pCameraData->fAperture[0]));
    pAperture->SetValDesired(static_cast<float>(pCameraData->fAperture[1]));
    pAperture->MakeCoeffs();

    pLetterbox->SetVal(static_cast<float>(pCameraData->fLetterbox[0]));
    pLetterbox->SetValDesired(static_cast<float>(pCameraData->fLetterbox[1]));
    pLetterbox->MakeCoeffs();

    pSimSpeed->SetVal(static_cast<float>(pCameraData->fSimSpeed[0]));
    pSimSpeed->SetValDesired(static_cast<float>(pCameraData->fSimSpeed[1]));
    pSimSpeed->MakeCoeffs();

    int state = TheICEManager.GetState();
    if ((state == 0 || state == 5 || state == 7) &&
        (pOldCameraData == 0 || pOldCameraData->nOverlay != pCameraData->nOverlay)) {
        ICE::HideOverlay();
        ICE::ShowOverlay(pCameraData->nOverlay);
    }
}

float ICEMover::GetDutch(float f_param) {
    if (pICEData == 0) {
        goto blend;
    }
    if (pICEData->bCubicEye == 0) {
        goto blend;
    }

    return pDutch->GetVal(f_param);

blend: {
    float v0 = pDutch->GetVal();
    float v1 = pDutch->GetValDesired();
    return v0 * (1.0f - f_param) + v1 * f_param;
}
}

unsigned short ICEMover::GetFOV(float f_param) {
    float result;

    if (pICEData == 0) {
        goto blend;
    }
    if (pICEData->bCubicEye == 0) {
        goto blend;
    }

    result = pFov->GetVal(f_param);
    goto done;

blend:
    result = pFov->GetVal() * (1.0f - f_param) + pFov->GetValDesired() * f_param;

done:
    return static_cast<unsigned short>(result);
}

ICEMover::~ICEMover() {
    ICE::HideOverlay();
    delete pEye;
    delete pLook;
    delete pDutch;
    delete pFov;
    delete pNearClip;
    delete pNoiseAmplitude;
    delete pNoiseFrequency;
    delete pFocalDistance;
    delete pAperture;
    delete pLetterbox;
    delete pSimSpeed;
    delete pAccelOffset;
    GetCamera()->SetSimTimeMultiplier(1.0f);
    GetCamera()->SetLetterBox(0.0f);
    GetCamera()->SetDepthOfField(0.0f);
    GetCamera()->SetFocalDistance(0.0f);
    GetCamera()->ClearVelocity();
}

void ICEMover::GetEye(ICE::Vector3 *vEye, float f_param) {
    if (pICEData == 0) {
        goto blend;
    }
    if (pICEData->bCubicEye == 0) {
        goto blend;
    }

    pEye->GetVal(vEye, f_param);
    return;

blend:
    {
        ICE::Vector3 v0;
        ICE::Vector3 v1;
        pEye->GetVal(&v0);
        pEye->GetValDesired(&v1);
        float inv = 1.0f - f_param;
        bScale(reinterpret_cast<bVector3 *>(vEye), reinterpret_cast<const bVector3 *>(&v0), inv);
        bScaleAdd(reinterpret_cast<bVector3 *>(vEye), reinterpret_cast<const bVector3 *>(vEye),
                  reinterpret_cast<const bVector3 *>(&v1), f_param);
    }
}

void ICEMover::GetLook(ICE::Vector3 *vLook, float f_param) {
    if (pICEData == 0) {
        goto blend;
    }
    if (pICEData->bCubicLook == 0) {
        goto blend;
    }

    pLook->GetVal(vLook, f_param);
    return;

blend:
    {
        ICE::Vector3 v0;
        ICE::Vector3 v1;
        pLook->GetVal(&v0);
        pLook->GetValDesired(&v1);
        float inv = 1.0f - f_param;
        bScale(reinterpret_cast<bVector3 *>(vLook), reinterpret_cast<const bVector3 *>(&v0), inv);
        bScaleAdd(reinterpret_cast<bVector3 *>(vLook), reinterpret_cast<const bVector3 *>(vLook),
                  reinterpret_cast<const bVector3 *>(&v1), f_param);
    }
}

ICEAnchor *GetICEAnchor() {
    eView *view = eGetView(1, false);
    if (view != 0) {
        CameraMover *m = view->GetCameraMover();
        if (m != 0 && m->GetType() == CM_ICE) {
            return reinterpret_cast<ICEMover *>(m)->GetICEAnchor();
        }
    }
    return 0;
}

void ICEMover::Update(float dT) {
    ICETrack *p_track = TheICEManager.GetPlaybackTrack();
    bool bLerpLag;
    UMath::Matrix4 mSceneToWorld;

    float realTime = TheICEManager.IsUsingRealTime();
    if (realTime == 0.0f && TheGameFlowManager.IsPaused()) {
        return;
    }

    bLerpLag = false;
    if (p_track != nullptr) {
        bLerpLag = (p_track->GetContext() == 3);
        if (p_track->GetContext() != 2) {
            bMirrorICEData = false;
        }
    }

    bViolatesTopology = false;
    SetDesired(false, TheICEManager.RefreshCameraSplines());

    PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&mSceneToWorld));

    if (nSpaceEye == 3 || nSpaceLook == 3) {
        ICEScene *scene = ICE::FindAnimScene();
        if (scene == 0) {
            return;
        }
        bMatrix4 &sceneMat = scene->GetSceneTransformMatrix();
        PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&sceneMat), *reinterpret_cast<Mtx44 *>(&mSceneToWorld));
    }

    float f_route_param;
    if (p_track != nullptr) {
        f_route_param = p_track->GetParameter();
    } else {
        f_route_param = TheICEManager.GetParameter();
    }

    float f_param = 0.0f;
    if (0.0001f < bAbs(fParameter1 - fParameter0)) {
        f_param = (f_route_param - fParameter0) / (fParameter1 - fParameter0);
    }

    if (1.0f <= f_route_param) {
        TheICEManager.SetGenericCameraToPlay("", "");
    }

    float simspeed = pSimSpeed->GetVal(f_param);
    if (0.0f <= simspeed) {
        if (1.0f <= simspeed) {
            simspeed = 1.0f;
        }
        simspeed = simspeed * 30.0f;
        GetCamera()->SetSimTimeMultiplier(simspeed);
    }
    if (simspeed < 1.0f) {
        simspeed = 1.0f;
    }

    float f_near_clip = pNearClip->GetVal(f_param);
    if (0.0f <= f_near_clip) {
        GetCamera()->SetNearZ(f_near_clip);
    }

    unsigned short a_fov = GetFOV(f_param);
    if (a_fov != 0) {
        GetCamera()->SetFieldOfView(a_fov);
    }

    ICE::Vector3 vEye;
    ICE::Vector3 vLook;

    GetEye(&vEye, f_param);
    GetLook(&vLook, f_param);

    UMath::Matrix4 mCarToWorld;
    int n_state;

    if (pICEData == 0) {
        ICE::Vector3 *carPos = pCar->GetGeometryPosition();
        mCarToWorld.v0 = *reinterpret_cast<UMath::Vector4 *>(&pCar->GetGeometryOrientation()->v0);
        mCarToWorld.v1 = *reinterpret_cast<UMath::Vector4 *>(&pCar->GetGeometryOrientation()->v1);
        mCarToWorld.v2 = *reinterpret_cast<UMath::Vector4 *>(&pCar->GetGeometryOrientation()->v2);
        bCopy(reinterpret_cast<bVector4 *>(&mCarToWorld.v3), reinterpret_cast<const bVector3 *>(carPos), 1.0f);
    } else if (pICEData->bIgnoreOrientation == 0) {
        if (pICEData->bCarSpaceLag == 0 || !bLerpLag) {
            ICE::Vector3 *carPos = pCar->GetGeometryPosition();
            bCopy(reinterpret_cast<bMatrix4 *>(&mCarToWorld),
                  reinterpret_cast<const bMatrix4 *>(pCar->GetGeometryOrientation()),
                  reinterpret_cast<const bVector3 *>(carPos));
        } else {
            float lerp = f_route_param * 10.0f + 0.01f;
            ICE::Vector3 *carPos = pCar->GetGeometryPosition();

            vSmoothCarPos.x = (carPos->x - vSmoothCarPos.x) * lerp + vSmoothCarPos.x;
            vSmoothCarPos.y = (carPos->y - vSmoothCarPos.y) * lerp + vSmoothCarPos.y;
            vSmoothCarPos.z = (carPos->z - vSmoothCarPos.z) * lerp + vSmoothCarPos.z;

            ICE::Vector3 *carFwd = pCar->GetForwardVector();
            vSmoothCarFwd.x = (carFwd->x - vSmoothCarFwd.x) * lerp + vSmoothCarFwd.x;
            vSmoothCarFwd.y = (carFwd->y - vSmoothCarFwd.y) * lerp + vSmoothCarFwd.y;
            vSmoothCarFwd.z = (carFwd->z - vSmoothCarFwd.z) * lerp + vSmoothCarFwd.z;

            PSMTX44Identity(*reinterpret_cast<Mtx44 *>(&mCarToWorld));
            bNormalize(reinterpret_cast<bVector3 *>(&vSmoothCarFwd), reinterpret_cast<const bVector3 *>(&vSmoothCarFwd));
            bCopy(reinterpret_cast<bVector4 *>(&mCarToWorld.v0), reinterpret_cast<const bVector3 *>(&vSmoothCarFwd), 0.0f);
            bCross(reinterpret_cast<bVector3 *>(&mCarToWorld.v1), reinterpret_cast<const bVector3 *>(&mCarToWorld.v2), reinterpret_cast<const bVector3 *>(&mCarToWorld.v0));
            bCross(reinterpret_cast<bVector3 *>(&mCarToWorld.v2), reinterpret_cast<const bVector3 *>(&mCarToWorld.v0), reinterpret_cast<const bVector3 *>(&mCarToWorld.v1));
            bCopy(reinterpret_cast<bVector4 *>(&mCarToWorld.v3), reinterpret_cast<const bVector3 *>(&vSmoothCarPos), 1.0f);
        }
    } else {
        mCarToWorld.v0 = *reinterpret_cast<UMath::Vector4 *>(&mHybridToWorld.v0);
        mCarToWorld.v1 = *reinterpret_cast<UMath::Vector4 *>(&mHybridToWorld.v1);
        mCarToWorld.v2 = *reinterpret_cast<UMath::Vector4 *>(&mHybridToWorld.v2);
        ICE::Vector3 *carPos = pCar->GetGeometryPosition();
        bCopy(reinterpret_cast<bVector4 *>(&mCarToWorld.v3), reinterpret_cast<const bVector3 *>(carPos), 1.0f);
    }

    if (nSpaceEye == 2) {
        ICE::Vector3 *carPos = pCar->GetGeometryPosition();
        vEye.x += carPos->x;
        vEye.y += carPos->y;
        vEye.z += carPos->z;
    } else if (nSpaceEye == 0) {
        bMulMatrix(reinterpret_cast<bVector3 *>(&vEye), reinterpret_cast<const bMatrix4 *>(&mCarToWorld), reinterpret_cast<const bVector3 *>(&vEye));
    } else if (nSpaceEye == 3) {
        bMulMatrix(reinterpret_cast<bVector3 *>(&vEye), reinterpret_cast<const bMatrix4 *>(&mSceneToWorld), reinterpret_cast<const bVector3 *>(&vEye));
    }

    if (nSpaceLook == 2) {
        ICE::Vector3 *carPos = pCar->GetGeometryPosition();
        vLook.x += carPos->x;
        vLook.y += carPos->y;
        vLook.z += carPos->z;
    } else if (nSpaceLook == 0) {
        bMulMatrix(reinterpret_cast<bVector3 *>(&vLook), reinterpret_cast<const bMatrix4 *>(&mCarToWorld), reinterpret_cast<const bVector3 *>(&vLook));
    } else if (nSpaceLook == 3) {
        bMulMatrix(reinterpret_cast<bVector3 *>(&vLook), reinterpret_cast<const bMatrix4 *>(&mSceneToWorld), reinterpret_cast<const bVector3 *>(&vLook));
    }

    if (pICEData != 0 && pICEData->bCarSpaceLag != 0) {
        ICE::Vector3 accel_offset;
        pAccelOffset->SetValDesired(reinterpret_cast<const ICE::Vector3 *>(pCar->GetAcceleration()));
        pAccelOffset->Update(dT * simspeed, 0.0f, 0.0f);
        pAccelOffset->GetVal(&accel_offset);

        accel_offset.x = accel_offset.x * vIceAccelLagScale.x;
        if (accel_offset.x - vIceAccelLagMin.x < 0.0f) {
            accel_offset.x = vIceAccelLagMin.x;
        }
        if (vIceAccelLagMax.x - accel_offset.x < 0.0f) {
            accel_offset.x = vIceAccelLagMax.x;
        }

        accel_offset.y = accel_offset.y * vIceAccelLagScale.y;
        if (accel_offset.y - vIceAccelLagMin.y < 0.0f) {
            accel_offset.y = vIceAccelLagMin.y;
        }
        if (vIceAccelLagMax.y - accel_offset.y < 0.0f) {
            accel_offset.y = vIceAccelLagMax.y;
        }

        accel_offset.z = accel_offset.z * vIceAccelLagScale.z;
        if (accel_offset.z - vIceAccelLagMin.z < 0.0f) {
            accel_offset.z = vIceAccelLagMin.z;
        }
        if (vIceAccelLagMax.z - accel_offset.z < 0.0f) {
            accel_offset.z = vIceAccelLagMax.z;
        }

        vLook.x -= accel_offset.x;
        vLook.y -= accel_offset.y;
        vLook.z -= accel_offset.z;
        vEye.x -= accel_offset.x;
        vEye.y -= accel_offset.y;
        vEye.z -= accel_offset.z;
    }

    n_state = TheICEManager.GetState();
    unsigned short dutch = static_cast<unsigned short>(static_cast<int>(GetDutch(f_param) * 65536.0f) & 0xffff);

    UMath::Matrix4 mWorldToCamera;
    CreateLookAtMatrix(reinterpret_cast<ICE::Matrix4 *>(&mWorldToCamera),
                       *reinterpret_cast<ICE::Vector3 *>(&vEye),
                       *reinterpret_cast<ICE::Vector3 *>(&vLook),
                       dutch);

    float f_amplitude = pNoiseAmplitude->GetVal(f_param);
    float f_frequency = pNoiseFrequency->GetVal(f_param);
    if (f_amplitude < 0.0f) {
        f_amplitude = 0.0f;
    }
    if (f_frequency < 0.0f) {
        f_frequency = 0.0f;
    }

    if (pICEData == 0 || pICEData->nShakeType == 0) {
        GetCamera()->SetNoiseAmplitude1(0.0f, 0.0f, f_amplitude, f_amplitude);
        GetCamera()->SetNoiseFrequency1(0.0f, 0.0f, f_amplitude, f_amplitude);
        GetCamera()->SetNoiseAmplitude2(0.0f, 0.0f, 0.0f, 0.0f);
        GetCamera()->SetNoiseFrequency2(0.0f, 0.0f, 0.0f, 0.0f);

        float routeLen;
        if (p_track != nullptr) {
            routeLen = p_track->GetLength();
        } else {
            routeLen = TheICEManager.GetParameterLength();
        }

        float time = f_route_param * routeLen;
        GetCamera()->ApplyNoise(reinterpret_cast<bMatrix4 *>(&mWorldToCamera), time, 1.0f);
    } else {
        ICEShakeTrack *shake_track = TheICEManager.GetShakeTrack(pICEData->nShakeType);
        if (shake_track != 0) {
            float routeLen;
            if (p_track != nullptr) {
                routeLen = p_track->GetLength();
            } else {
                routeLen = TheICEManager.GetParameterLength();
            }

            int numKeys = static_cast<int>(shake_track->NumKeys);
            int frame = static_cast<int>(f_route_param * routeLen * f_frequency);

            if (numKeys < 1) {
                frame = 0;
            } else {
                while (frame < 0) {
                    frame += numKeys;
                }
                frame = frame % numKeys;
            }

            int maxKey = shake_track->NumKeys - 1;
            int clampedFrame = frame;
            if (clampedFrame < 0) clampedFrame = 0;
            if (clampedFrame > maxKey) clampedFrame = maxKey;

            ICEShakeData *shake_data;
            if (frame == clampedFrame) {
                shake_data = &shake_track->Keys[clampedFrame];
            } else {
                shake_data = 0;
            }

            if (shake_data != 0) {
                UMath::Vector3 r;
                r.x = shake_data->q[0] * f_amplitude;
                r.y = shake_data->q[1] * f_amplitude;
                r.z = shake_data->q[2] * f_amplitude;
                float w = bLength(reinterpret_cast<const bVector3 *>(&r));
                float halfAngle = bSqrt(1.0f - w);

                bQuaternion q(r.x, r.y, r.z, halfAngle);
                UMath::Matrix4 shake_matrix;
                q.GetMatrix(*reinterpret_cast<bMatrix4 *>(&shake_matrix));

                UMath::Vector3 t;
                t.x = shake_data->p[0] * f_amplitude;
                t.y = shake_data->p[1] * f_amplitude;
                t.z = shake_data->p[2] * f_amplitude;
                bCopy(reinterpret_cast<bMatrix4 *>(&shake_matrix),
                      reinterpret_cast<const bMatrix4 *>(&shake_matrix),
                      reinterpret_cast<const bVector3 *>(&t));

                bMulMatrix(reinterpret_cast<bMatrix4 *>(&mWorldToCamera),
                           reinterpret_cast<const bMatrix4 *>(&shake_matrix),
                           reinterpret_cast<const bMatrix4 *>(&mWorldToCamera));
            }
        }
    }

    if (p_track == nullptr || p_track->GetContext() != 2) {
        bool constrain_to_topology = false;
        if (pICEData != 0 && pICEData->bConstrainToWorld != 0) {
            constrain_to_topology = (n_state < 9);
        }
        if (constrain_to_topology) {
            MinGapTopology(reinterpret_cast<bMatrix4 *>(&mWorldToCamera),
                           reinterpret_cast<bVector3 *>(pCar->GetGeometryPosition()));
        }

        bool constrain_to_cars = false;
        if (pICEData != 0 && pICEData->bConstrainToCars != 0) {
            constrain_to_cars = (n_state < 9);
        }
        if (constrain_to_cars) {
            MinGapCars(reinterpret_cast<bMatrix4 *>(&mWorldToCamera),
                       reinterpret_cast<bVector3 *>(pCar->GetGeometryPosition()),
                       reinterpret_cast<bVector3 *>(pCar->GetVelocity()));
        }
    } else {
        int topViolation = MinGapTopology(reinterpret_cast<bMatrix4 *>(&mWorldToCamera),
                                          reinterpret_cast<bVector3 *>(pCar->GetGeometryPosition()));
        bViolatesTopology = topViolation != 0;
        int carViolation = MinGapCars(reinterpret_cast<bMatrix4 *>(&mWorldToCamera),
                                      reinterpret_cast<bVector3 *>(pCar->GetGeometryPosition()),
                                      reinterpret_cast<bVector3 *>(pCar->GetVelocity()));
        bViolatesTopology = (bViolatesTopology || carViolation != 0);
    }

    float letterbox = pLetterbox->GetVal(f_param);
    if (0.0f <= letterbox) {
        GetCamera()->SetLetterBox(letterbox * 0.01f);
    }

    float aperture = pAperture->GetVal(f_param);
    if (aperture < 0.0f || 37.0f <= aperture) {
        GetCamera()->SetFocalDistance(0.0f);
    } else {
        float focal = pFocalDistance->GetVal(f_param);
        if (focal < 1.0f) {
            focal = 1.0f;
        }
        focal = focal * 100.0f;

        unsigned short fovAngle = GetFOV(f_param);
        float lens = ConvertFovAngleToLensLength(fovAngle);
        float fstop = ConvertApertureNumberToFStop(aperture);

        float hyperFocal = (lens * lens) / (fstop * 0.03f) + lens;
        float dofNear = (hyperFocal - lens) * focal;
        float dofFar = 10000.0f;
        if (0.001f < hyperFocal - focal) {
            dofFar = (dofNear / (hyperFocal - focal)) * 2.0f;
        }
        if (dofFar < dofNear) {
            dofFar = dofNear + 1.0f;
        }
        GetCamera()->SetFocalDistance((dofFar + dofNear) * 0.5f);
    }
    GetCamera()->SetDepthOfField(0.0f);

    GetCamera()->SetTargetDistance(bDistBetween(reinterpret_cast<const bVector3 *>(&vEye), reinterpret_cast<const bVector3 *>(&vLook)));

    GetCamera()->SetCameraMatrix(*reinterpret_cast<const bMatrix4 *>(&mWorldToCamera), dT * simspeed);
}
