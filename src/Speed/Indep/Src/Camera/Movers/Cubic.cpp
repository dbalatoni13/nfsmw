#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

extern tTable<CubicPovData> aCubicPovTables[];
extern float CameraImpcatCurveH[];
extern float CameraImpcatCurveV[];
extern float CameraGearChangingCurve[];
extern float PovHandheldNoiseScale[];
extern float PovHandheldChopperScale[];
extern float PovVelocityNoiseScale[];
extern float PovTerrainNoiseScale[];
extern float _SmokeShowEyeOffset;
extern float _HydraulicsEyeOffset;
extern unsigned short _SmokeShowLookAngle;
extern unsigned short _HydraulicsLookAngle;
extern unsigned short _NOSFovWidening;
extern RaceParameters theRaceParameters;
extern Timer RealTimer;

bool OutsidePovType(int pov_type);
void FlushAccumulationBuffer();
void ApplyCameraShake(int nViewID, bMatrix4 *pMatrix);
int AmIinATunnel(eView *view, int CheckOverPass);

bVector3* bClamp(bVector3* dest, const bVector3* pMin, const bVector3* pMax) {
    float x = bClamp(dest->x, pMin->x, pMax->x);
    float y = bClamp(dest->y, pMin->y, pMax->y);
    float z = bClamp(dest->z, pMin->z, pMax->z);
    dest->x = x;
    dest->y = y;
    dest->z = z;
    return dest;
}

void tTable<CubicPovData>::Blend(CubicPovData *dest, CubicPovData *a, CubicPovData *b, float blend_a) {
    float blend_b = 1.0f - blend_a;
    float f1 = blend_b * b->fEyeDuration;
    float f2 = blend_b * b->fLookDuration;
    float f3 = blend_b * b->fFovDuration;
    float f4 = blend_b * b->fUpDuration;

    dest->fEyeDuration = blend_a * a->fEyeDuration + f1;
    dest->fLookDuration = blend_a * a->fLookDuration + f2;
    dest->fFovDuration = blend_a * a->fFovDuration + f3;
    dest->fUpDuration = blend_a * a->fUpDuration + f4;

    bVector3 v0, v1, v2, v3, v4, v5, v6, v7, v8, v9;

    bScale(&v0, a->GetUpAccel(), blend_a);
    bScale(&v1, a->GetUpAccelMin(), blend_a);
    bScale(&v2, a->GetUpAccelMax(), blend_a);
    bScale(&v3, a->GetEyeAccel(), blend_a);
    bScale(&v4, a->GetEyeAccelMin(), blend_a);
    bScale(&v5, a->GetEyeAccelMax(), blend_a);
    bScale(&v6, a->GetLookAccel(), blend_a);
    bScale(&v7, a->GetLookAccelMin(), blend_a);
    bScale(&v8, a->GetLookAccelMax(), blend_a);
    bScale(&v9, a->GetForwardDuration(), blend_a);

    bScaleAdd(dest->GetUpAccel(), &v0, b->GetUpAccel(), blend_b);
    bScaleAdd(dest->GetUpAccelMin(), &v1, b->GetUpAccelMin(), blend_b);
    bScaleAdd(dest->GetUpAccelMax(), &v2, b->GetUpAccelMax(), blend_b);
    bScaleAdd(dest->GetEyeAccel(), &v3, b->GetEyeAccel(), blend_b);
    bScaleAdd(dest->GetEyeAccelMin(), &v4, b->GetEyeAccelMin(), blend_b);
    bScaleAdd(dest->GetEyeAccelMax(), &v5, b->GetEyeAccelMax(), blend_b);
    bScaleAdd(dest->GetLookAccel(), &v6, b->GetLookAccel(), blend_b);
    bScaleAdd(dest->GetLookAccelMin(), &v7, b->GetLookAccelMin(), blend_b);
    bScaleAdd(dest->GetLookAccelMax(), &v8, b->GetLookAccelMax(), blend_b);
    bScaleAdd(dest->GetForwardDuration(), &v9, b->GetForwardDuration(), blend_b);
}

CubicCameraMover::CubicCameraMover(int nView, CameraAnchor *p_car, int pov_type, bool smooth, bool disable_lag, bool look_back, bool perfect_focus)
    : CameraMover(nView, CM_DRIVE_CUBIC) {
    bSnapNext = 0;
    bAccelLag = !disable_lag;
    bLookBack = look_back;
    bPerfectFocus = perfect_focus;
    pCar = p_car;
    nPovType = pov_type;
    nPovTypeUsed = pov_type;
    tLastGrounded = WorldTimer - Timer(8000);
    tLastUnderVehicle = WorldTimer - Timer(0x1900);
    tLastGearChange = WorldTimer - Timer(6000);
    fIgnoreSetSnapNextTimer = 0.0f;
    bFirstTime = 1;

    POV *pov = pCar->GetPov(nPovType);

    CubicPovData pov_data;
    aCubicPovTables[nPovType].GetValue(&pov_data, 0.0f);

    pFov = new (__FILE__, __LINE__) tCubic1D(1, pov_data.fFovDuration);
    pEye = new (__FILE__, __LINE__) tCubic3D(1, pov_data.fEyeDuration);
    pLook = new (__FILE__, __LINE__) tCubic3D(1, pov_data.fLookDuration);
    pForward = new (__FILE__, __LINE__) tCubic3D(1, pov_data.GetForwardDuration());
    pUp = new (__FILE__, __LINE__) tCubic3D(1, pov_data.GetForwardDuration());

    pAvgAccel = new tAverage<bVector3>(5);

    bMatrix4 mCarToWorld;
    SetDesired(&mCarToWorld, pov, &pov_data, true);

    bMatrix4 mWorldToCar;
    eInvertTransformationMatrix(&mWorldToCar, &mCarToWorld);

    if (bLookBack) {
        mWorldToCar.v0.x = -mWorldToCar.v0.x;
        mWorldToCar.v0.y = -mWorldToCar.v0.y;
        mWorldToCar.v1.x = -mWorldToCar.v1.x;
        mWorldToCar.v1.y = -mWorldToCar.v1.y;
        mWorldToCar.v2.x = -mWorldToCar.v2.x;
        mWorldToCar.v2.y = -mWorldToCar.v2.y;
        mWorldToCar.v3.x = -mWorldToCar.v3.x;
        mWorldToCar.v3.y = -mWorldToCar.v3.y;
    }

    SetEyeLook(pEye, pLook, pFov, &mWorldToCar, pCar->GetVelocity());

    bVector3 eye_current;
    bVector3 eye_desired;
    bVector3 look_current;
    bVector3 look_desired;

    pEye->GetVal(&eye_current);
    pLook->GetVal(&look_current);
    pEye->GetValDesired(&eye_desired);
    pLook->GetValDesired(&look_desired);

    bVector3 eye_movement(eye_desired - eye_current);
    bVector3 direction_current(look_current - eye_current);
    bVector3 direction_desired(look_desired - eye_desired);

    bNormalize(&direction_current, &direction_current);
    bNormalize(&direction_desired, &direction_desired);

    vSavedEye.z = 0.0f;
    vSavedEye.y = 0.0f;
    vSavedEye.x = 0.0f;
    vCameraImpcat.y = 0.0f;
    vCameraImpcat.x = 0.0f;
    vCameraImpcatTimer.y = 0.0f;
    vCameraImpcatTimer.x = 0.0f;

    if (smooth && bLength(&eye_movement) <= 50.0f && bDot(&direction_current, &direction_desired) >= -0.9f)
        goto set_timer;

    pUp->Snap();
    pFov->Snap();
    pEye->Snap();
    pLook->Snap();
    goto done;

set_timer:
    fIgnoreSetSnapNextTimer = 1.0f;

done:;
}

bool CubicCameraMover::IsUnderVehicle() {
    const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_ALL);

    for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
        IVehicle *ivehicle = *iter;

        if (!ivehicle->IsActive()) {
            continue;
        }
        if (ivehicle->IsDestroyed()) {
            continue;
        }

        ISimable *isimable = ivehicle->GetSimable();
        if (isimable == nullptr) {
            continue;
        }

        if (isimable->GetWorldID() == pCar->GetWorldID()) {
            continue;
        }

        IRigidBody *irb = isimable->GetRigidBody();
        if (irb == nullptr) {
            continue;
        }

        if (!irb->IsSimple()) {
            continue;
        }

        UMath::Vector4 vehiclePos;
        const UMath::Vector3 &pos = irb->GetPosition();
        vehiclePos.x = pos.x;
        vehiclePos.y = pos.y;
        vehiclePos.z = pos.z;
        vehiclePos.w = 0.0f;

        UMath::Vector4 testPos;
        testPos.x = vehiclePos.x;
        testPos.y = vehiclePos.y;
        testPos.z = vehiclePos.z;
        testPos.w = 0.0f;

        UMath::Vector3 dim;
        irb->GetDimension(dim);

        UMath::Matrix4 mat;
        irb->GetMatrix4(mat);

        UMath::Matrix4 world2local;
        UMath::Transpose(mat, world2local);

        UMath::Vector3 forward;
        irb->GetForwardVector(forward);

        bVector3 carFwd;
        eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(forward), carFwd);

        bVector3 *camFwd = GetCamera()->GetDirection();
        float dot = bDot(camFwd, &carFwd);

        if (bAbs(dot) >= 0.5f) {
            continue;
        }

        {
            bVector3 predictedPos;
            bVector3 *geomPos = pCar->GetGeometryPosition();
            bVector3 *vel = const_cast<bVector3 *>(pCar->GetVelocity());
            bVector3 scaled = *vel * 0.5f;
            bVector3 sum = *geomPos + scaled;
            eUnSwizzleWorldVector(sum, reinterpret_cast<bVector3 &>(testPos));
            testPos.w = 0.0f;

            UMath::Vector4 test2vehicle;
            UMath::Sub(testPos, vehiclePos, test2vehicle);
            float dist = UMath::Length(test2vehicle);

            if (dist >= irb->GetRadius()) {
                continue;
            }

            UMath::Vector4 test2vehicleLocal;
            UMath::Rotate(test2vehicle, world2local, test2vehicleLocal);

            float absX = test2vehicleLocal.x;
            if (absX < 0.0f) {
                absX = -absX;
            }
            if (absX < dim.x) {
                float absZ = test2vehicleLocal.z;
                if (absZ < 0.0f) {
                    absZ = -absZ;
                }
                if (absZ < dim.z) {
                    return true;
                }
            }
        }
    }
    return false;
}

void CubicCameraMover::SetDesired(bMatrix4 *pCarToWorld, POV *pov, CubicPovData *pov_data, bool bSnapForward) {
    bool bOutside = OutsidePovType(pov->Type);

    if (pCar->IsTouchingGround()) {
        tLastGrounded = WorldTimer;
    }

    if (IsUnderVehicle()) {
        tLastUnderVehicle = WorldTimer;
    }

    SetForward(pov, bSnapForward);
    MakeSpace(pCarToWorld);

    bMatrix4 mWorldToCar;
    eInvertTransformationMatrix(&mWorldToCar, pCarToWorld);

    bVector4 vAccel(0.0f, 0.0f, 0.0f, 0.0f);
    if (bAccelLag) {
        bVector3 *avgAccelVal = pAvgAccel->GetValue();
        vAccel.x = avgAccelVal->x;
        vAccel.y = avgAccelVal->y;
        vAccel.z = avgAccelVal->z;
        vAccel.w = 0.0f;
        bMulMatrix(reinterpret_cast<bVector4 *>(&vAccel), &mWorldToCar, &vAccel);
    }

    bVector3 vEyeOffset;
    unsigned short aAngle;

    bool b_snap_always = false;
    if (theRaceParameters.IsBurnout()) {
        b_snap_always = true;
    }

    if (b_snap_always) {
        vEyeOffset = bVector3(_SmokeShowEyeOffset, 0.0f, 0.0f);
        aAngle = _SmokeShowLookAngle;
    } else {
        if (!HighliteMode()) {
            vEyeOffset.x = pov->Lag;
            vEyeOffset.y = pov->LatOffset;
            vEyeOffset.z = pov->Height;
            CameraSpeedHug(&vEyeOffset);
            aAngle = pov->Angle;
        } else {
            vEyeOffset = bVector3(_HydraulicsEyeOffset, 0.0f, 0.0f);
            aAngle = _HydraulicsLookAngle;
        }
    }

    float fTan = bSin(aAngle) / bCos(aAngle);

    bVector3 vLookOffset;
    if (!bOutside) {
        vLookOffset.x = vEyeOffset.x + 1.0f;
        vLookOffset.z = vEyeOffset.z - fTan;
    } else {
        vLookOffset.z = fTan * vEyeOffset.x + vEyeOffset.z;
        vLookOffset.x = 0.0f;
    }

    bVector3 vEye;
    vEye.x = pov_data->vEyeAccel.x * vAccel.x;
    vEye.y = pov_data->vEyeAccel.y * vAccel.y;
    vEye.z = pov_data->vEyeAccel.z * vAccel.z;
    vLookOffset.y = 0.0f;
    bClamp(&vEye, pov_data->GetEyeAccelMin(), pov_data->GetEyeAccelMax());

    if (HighliteMode()) {
        vEyeOffset *= 0.25f;
    }

    vEye += vEyeOffset;
    pEye->SetValDesired(&vEye);

    bVector3 vLook;
    vLook.x = pov_data->vLookAccel.x * vAccel.x;
    vLook.y = pov_data->vLookAccel.y * vAccel.y;
    vLook.z = pov_data->vLookAccel.z * vAccel.z;
    bClamp(&vLook, pov_data->GetLookAccelMin(), pov_data->GetLookAccelMax());
    vLook += vLookOffset;
    pLook->SetValDesired(&vLook);

    bVector3 v_up(0.0f, 0.0f, 1.0f);
    if (pov->AllowTilting) {
        bVector3 *avgVal = pAvgAccel->GetValue();
        bVector3 scaled = bScale(*pov_data->GetUpAccel(), *avgVal);
        v_up = v_up + scaled;
        bClamp(&v_up, pov_data->GetUpAccelMin(), pov_data->GetUpAccelMax());
    }

    if (0.1f < pCar->GetVelMag()) {
        bVector3 *dutch = DutchAroundCar(pCar->GetGeometryPosition(), const_cast<bVector3 *>(pCar->GetVelocity()));
        v_up += *dutch;
    }
    bNormalize(&v_up, &v_up);
    pUp->SetValDesired(&v_up);

    unsigned short aFov = pov->Fov;
    if (bOutside && pCar->IsNosEngaged() && 0.0f < pCar->GetVelMag()) {
        aFov = aFov + _NOSFovWidening;
    }
    pFov->SetValDesired(static_cast<float>(aFov));

    if (bSnapNext || !bOutside) {
        pUp->Snap();
        pFov->Snap();
        pEye->Snap();
        pLook->Snap();
    }

    if (bSnapNext) {
        FlushAccumulationBuffer();
    }
}

void CubicCameraMover::Update(float dT) {
    if (TheGameFlowManager.IsPaused() && !bFirstTime) {
        return;
    }

    bFirstTime = 0;

    if (0.0f < fIgnoreSetSnapNextTimer) {
        fIgnoreSetSnapNextTimer -= dT;
    }

    nPovTypeUsed = nPovType;
    POV *pov = pCar->GetPov(nPovType);

    float collision_damper = pCar->GetCollisionDamping();
    float drift_damper = pCar->GetDrift();
    float stiffness = bClamp(pCar->GetVelMag() * 0.005f, 0.0f, 1.0f);

    CubicPovData pov_data;
    aCubicPovTables[nPovTypeUsed].GetValue(&pov_data, stiffness);

    if (theRaceParameters.IsDriftRace() || theRaceParameters.IsBurnout()) {
        pov_data.vEyeAccel = bVector3(0.0f, 0.0f, 0.0f);
    }

    bVector3 vAccel(0.0f, 0.0f, 0.0f);
    if (fIgnoreSetSnapNextTimer <= 0.0f) {
        CameraAccelCurve(&vAccel);
    }

    tAverage<bVector3> *avg = pAvgAccel;
    if (avg->GetNumSamples() < avg->nSlots) {
        avg->nSamples = avg->nSamples + 1;
    }

    int slot = static_cast<int>(avg->nCurrentSlot);
    bVector3 *slotData = &avg->pData[slot];
    avg->Total -= *slotData;
    avg->Total += vAccel;
    *slotData = vAccel;

    bVector3 avgVal = avg->Total * (1.0f / static_cast<float>(static_cast<int>(avg->nSamples)));
    avg->Average = avgVal;

    avg->nCurrentSlot = avg->nCurrentSlot + 1;
    if (avg->nSlots <= avg->nCurrentSlot) {
        avg->nCurrentSlot = 0;
    }

    float fAccelH = bMax(bAbs(vAccel.x), bAbs(vAccel.y));
    float fAccelV;

    if (24.0f <= fAccelH && 0.0f < pCar->GetCollisionDamping() &&
        vCameraImpcat.x < pCar->GetCollisionDamping()) {
        vCameraImpcat.x = pCar->GetCollisionDamping();
        vCameraImpcatTimer.x = 1.0f;
    }

    fAccelV = vAccel.z;
    if (fAccelV >= 24.0f) {
        float impact = bClamp((fAccelV - 24.0f) * (1.0f / 6.0f), 0.0f, 1.0f);
        if (impact > vCameraImpcat.y) {
            vCameraImpcat.y = impact;
            vCameraImpcatTimer.y = 1.0f;
        }
    }

    float eye_duration = pov_data.fEyeDuration;
    pUp->SetDuration(pov_data.fUpDuration);
    pFov->SetDuration(pov_data.fFovDuration);
    pLook->SetDuration(pov_data.fLookDuration);
    bVector3 *foward_duration = pov_data.GetForwardDuration();
    pForward->SetDuration(foward_duration->x + collision_damper + drift_damper,
                          foward_duration->y + collision_damper + drift_damper,
                          foward_duration->z + collision_damper + drift_damper);

    if (HighliteMode()) {
        eye_duration = pov_data.fEyeDuration * (10.0f / 13.0f);
    }

    pEye->SetDuration(eye_duration, eye_duration, eye_duration);

    pUp->Update(dT, 0.0f, 0.0f);
    pFov->Update(dT, 0.0f, 0.0f);
    pEye->Update(dT, 0.0f, 0.0f);
    pLook->Update(dT, 0.0f, 0.0f);
    pForward->Update(dT, 0.0f, 0.0f);

    bool bOutside = OutsidePovType(pov->Type);

    float target_dist = bDistBetween(GetCamera()->GetPosition(), pCar->GetGeometryPosition());
    GetCamera()->SetTargetDistance(target_dist);

    float dof;
    if (!bPerfectFocus) {
        GetCamera()->SetFocalDistance(40.0f);
        dof = 100.0f;
    } else {
        GetCamera()->SetFocalDistance(0.0f);
    }
    GetCamera()->SetDepthOfField(dof);

    bMatrix4 mCarToWorld;
    SetDesired(&mCarToWorld, pov, &pov_data, bSnapNext);
    bSnapNext = 0;

    float fSign;
    if (bLookBack) {
        fSign = -1.0f;
    } else {
        fSign = 1.0f;
    }
    bVector3 vUp;
    vUp.x = (*pUp).x.Val;
    vUp.y = (*pUp).y.Val;
    vUp.z = 1.0f;
    if (!bOutside) {
        vUp.x = pCar->GetUpVector()->x;
        vUp.y = pCar->GetUpVector()->y;
        vUp.z = pCar->GetUpVector()->z;
    }

    bVector3 vEye(fSign * (*pEye).x.Val, fSign * (*pEye).y.Val, (*pEye).z.Val);
    bVector3 vLook(fSign * (*pLook).x.Val, fSign * (*pLook).y.Val, (*pLook).z.Val);

    {
        float impact;
        if (vCameraImpcat.x > 0.0f) {
            if (0.0f < vCameraImpcatTimer.x) {
                tTable<float> envelope(CameraImpcatCurveH, 5, 0.0f, 1.0f);
                float s;
                envelope.GetValue(&s, vCameraImpcatTimer.x);
                s = s * vCameraImpcat.x * 0.001f;
                vEye.x = vEye.x * (1.0f - s);
                vLook.x = vLook.x * (s + 1.0f);
                vCameraImpcatTimer.x -= dT;
                if (vCameraImpcatTimer.x <= 0.0f) {
                    goto clearH;
                }
            } else {
            clearH:
                vCameraImpcat.x = 0.0f;
            }
        }
    }

    {
        if (vCameraImpcat.y > 0.0f) {
            if (0.0f < vCameraImpcatTimer.y) {
                tTable<float> envelope(CameraImpcatCurveV, 5, 0.0f, 1.0f);
                float s;
                envelope.GetValue(&s, vCameraImpcatTimer.y);
                vEye.z = vEye.z * (1.0f - s * vCameraImpcat.y * 0.001f);
                vCameraImpcatTimer.y -= dT;
                if (vCameraImpcatTimer.y <= 0.0f) {
                    goto clearV;
                }
            } else {
            clearV:
                vCameraImpcat.y = 0.0f;
            }
        }
    }

    if (!bOutside) {
        float fSeconds = (Timer(WorldTimer - tLastGearChange)).GetSeconds();
        if (fSeconds >= 1.0f) {
            if (0.4f < pCar->GetVelMag() && pCar->IsGearChanging()) {
                tLastGearChange = WorldTimer;
            }
        } else {
            tTable<float> gear_changing_table(CameraGearChangingCurve, 9, 0.0f, 1.0f);
            float eyeMove;
            gear_changing_table.GetValue(&eyeMove, fSeconds);
            float speed_attenuation = bClamp(pCar->GetVelMag() * 0.01f, 0.0f, 1.0f);
            float lag_scale = bAbs(vEye.x);
            vEye.x = vEye.x + eyeMove * lag_scale * 0.5f * (1.0f - speed_attenuation);
        }
    }

    bool isUnder = false;
    if (!bOutside) {
        float fSeconds = (Timer(WorldTimer - tLastUnderVehicle)).GetSeconds() * 0.001f;
        float fCurve = 2.0f - bClamp(fSeconds, 0.0f, 2.0f);
        if (fCurve > 1.0f) {
            fCurve = 3.0f - fCurve;
        }
        vEye.z = fCurve * 0.05f + vEye.z;
        if (0.0f < fCurve) {
            isUnder = true;
        }
    }

    if (GRaceStatus::Exists() && pCar->IsDragRace()) {
        float seconds = GRaceStatus::Get().GetRaceTimeElapsed();
        if (seconds < 0.0f) {
            float t = -seconds;
            bVector3 velCopy = *pCar->GetVelocity();
            vEye = (*pCar->GetUpVector()) * t + velCopy * (t * 0.5f) + vEye;
        }
    }

    bVector3 vDiff = vEye - vLook;
    vDiff *= fSign;
    vLook = vLook + vDiff;
    vEye = vLook;

    unsigned short a_fov = static_cast<unsigned short>(static_cast<int>(pFov->Val));
    float f_tan_fov = bTan(a_fov >> 1);
    unsigned short a_new_fov = bATan(1.0f, f_tan_fov) << 1;

    GetCamera()->SetFieldOfView(a_new_fov);

    bMatrix4 mWorldToCamera;
    eCreateLookAtMatrix(&mWorldToCamera, vEye, vLook, vUp);

    ApplyCameraShake(0, &mWorldToCamera);
    HandheldNoise(&mWorldToCamera, PovHandheldNoiseScale[nPovTypeUsed], true);

    if (!AmIinATunnel(eGetView(0, false), 1)) {
        float speed_attenuation = bClamp(pCar->GetVelMag() * 0.01f, 0.0f, 1.0f);
        float f_chopper_scale = PovHandheldChopperScale[nPovTypeUsed] * (1.0f - speed_attenuation * 0.5f);
        ChopperNoise(&mWorldToCamera, f_chopper_scale, true);
    }

    TerrainVelocityNoise(&mWorldToCamera, pCar, PovVelocityNoiseScale[nPovTypeUsed],
                         PovTerrainNoiseScale[nPovTypeUsed]);

    if (!bOutside) {
        MinGapTopology(&mWorldToCamera, pCar->GetGeometryPosition());
        if (!isUnder) {
            MinGapCars(&mWorldToCamera, pCar->GetGeometryPosition(), const_cast<bVector3 *>(pCar->GetVelocity()));
        }
    }

    GetCamera()->SetCameraMatrix(mWorldToCamera, dT);
}

template class tAverage<bVector3>;