#include "Speed\Indep\Src\Camera\CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Misc/Spline.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/WWorldMath.h"
#include "Speed/Indep/Src/World/Rain.hpp"

int CameraDebugWatchCar = 0;
int Tweak_EnableICEAuthoring = 0;
int Tweak_ForceICEReplay = 0;

const bVector4 CameraNoiseHandheldFrequency(0.213f, 0.175f, 0.153f, 0.192f); // .bss: 0x8045AB38
const bVector4 CameraNoiseHandheldAmplitude(0.01f, 0.01f, 0.03f, 0.03f);    // .bss: 0x8045AB48
const bVector4 CameraNoiseChopperFrequency(3.141f, 2.971f, 0.84234f, 0.92345f); // size: 0x10, address: 0x8045AB58
const bVector4 CameraNoiseChopperAmplitude(0.01f, 0.05f, 1.1, 2.7f);            // size: 0x10, address: 0x8045AB68
const bVector4 CameraNoiseSpeedFrequency(1.8f, 2.0f, 2.125f, 2.0f);             // size: 0x10, address: 0x8045AB78
const bVector4 CameraNoiseSpeedAmplitude(0.03f, 0.025f, 0.68f, 0.28f);          // size: 0x10, address: 0x8045AB88
const bVector4 CameraNoiseTerrainFrequency(3.0f, 5.0f, 7.0f, 5.5f);             // size: 0x10, address: 0x8045AB98
const bVector4 CameraNoiseTerrainAmplitude(0.007f, 0.01f, 0.3f, 0.4f);          // size: 0x10, address: 0x8045ABA8

bVector4 CameraNoiseSpeedData[5] = {
    bVector4(0.0f, 0.0f, 0.6f, 1.0f),     //
    bVector4(0.005f, 0.5f, 1.1f, 1.1f),   //
    bVector4(0.03f, 0.8f, 1.2f, 1.2f),    //
    bVector4(0.07f, 1.0f, 1.3f, 1.3f),    //
    bVector4(0.02f, 1.0f, 1.0f, 1.4f),    //
}; // size: 0x50

// booo
static const float fMinDistToWall = 0.7f; // size: 0x4

void CameraMoverRestartRace() {

    WeHaveCheckedIfJR2ServerExists = 0;
    CameraAI::Reset();

    for (int view_id = 1; view_id < 4; ++view_id) {
        eView *view = eGetView(view_id, false);

        if (view != nullptr) {
            CameraMover *cm = view->GetCameraMover();

            if (cm != nullptr) {
                cm->ResetState();
            }
        }
    }
}

bool DoesCameraTypeDisablePreculler(CameraMoverTypes type) {
    if (type == CM_DEBUG_WORLD) {
        return true;
    }
    return type == CM_TRACK_CAR;
}

CameraMover::CameraMover(int view_id, CameraMoverTypes type)
    : mCollider(WCollider::Create(0, WCollider::kColliderShape_Sphere, 0x1c, 0)), mWPos(0.025f) {
    Type = type;
    ViewID = view_id;
    Enabled = 0;
    fAccumulatedClearance = 0;
    fAccumulatedAdjust = 0;
    fSavedAdjust = 0;
    vSavedForward.z = 0; vSavedForward.y = 0; vSavedForward.x = 0;
    if (view_id == -1) {
        this->pView = nullptr;
        this->pCamera = nullptr;
        this->RenderDash = 0;
    } else {
        this->pView = eGetView(view_id, false);
        this->pCamera = pView->GetCamera();
        this->pCamera->SetFarZ(12000.0f);
        this->RenderDash = this->pCamera->GetRenderDash();
        Enable();
    }
    if (DoesCameraTypeDisablePreculler(Type)) {
        DisablePreculler();
    }
}

CameraMover::~CameraMover() {
    WCollider::Destroy(mCollider);

    if (DoesCameraTypeDisablePreculler(Type)) {
        EnablePreculler();
    }
    Disable();
}

void AverageBase::Recalculate() {}

void CameraMover::Update(float dT) {
    return;
}

void CameraMover::Render(eView *view) {}

void CameraMover::Enable() {
    if (Enabled)
        return;

    Enabled = 1;

    pCamera->SetRenderDash(RenderDash);

    pView->AttachCameraMover(this);
    pCamera->SetNearZ(0.5f);
}

void CameraMover::Disable() {
    if (Enabled != 0) {
        Enabled = 0;
        RenderDash = pCamera->GetRenderDash();
        pView->UnattachCameraMover(this);
    }
    return;
}

void CameraMover::ChopperNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer) {

    if (f_scale > 0.0f) {
        const UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List &vehicles =
            UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AICOPS);
        for (IVehicle *const *iter = vehicles.begin(); iter != vehicles.end(); iter++) {
            IVehicle *vehicle = *iter;
            if (!vehicle->IsActive()) {
                continue;
            }
            if (vehicle->GetVehicleClass() != VehicleClass::CHOPPER) {
                continue;
            }
            const UMath::Vector3 &pos = vehicle->GetPosition();

            bVector3 bpos;

            eSwizzleWorldVector(pos, bpos);

            bVector3 dir;

            bSub(&dir, &bpos, pCamera->GetPosition());
            dir.z = 0.0f;
            float distance = bLength(&dir);

            if (distance < 40.0f) {
                float intensity = f_scale * (1.0f - distance * 0.025f);

                bVector4 v_frequency;
                bVector4 v_magnitude;
                bScale(&v_frequency, &CameraNoiseChopperFrequency, 1.0f);
                bScale(&v_magnitude, &CameraNoiseChopperAmplitude, intensity);

                pCamera->SetNoiseFrequency1(&v_frequency);
                pCamera->SetNoiseAmplitude1(&v_magnitude);

                float time = useWorldTimer ? WorldTimer.GetSeconds() : RealTimer.GetSeconds();
                pCamera->ApplyNoise(world_to_camera, time, 1.0f);
            }
        }
    }
}

void CameraMover::HandheldNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer) {

    if (f_scale > 0.0f) {

        bVector4 v_frequency;
        bVector4 v_magnitude;
        bScale(&v_frequency, &CameraNoiseHandheldFrequency, 1.0f);
        bScale(&v_magnitude, &CameraNoiseHandheldAmplitude, f_scale);

        pCamera->SetNoiseFrequency1(&v_frequency);
        pCamera->SetNoiseAmplitude1(&v_magnitude);

        float time = useWorldTimer ? WorldTimer.GetSeconds() : RealTimer.GetSeconds();

        pCamera->ApplyNoise(world_to_camera, time, 1.0f);
    }
}

void CameraMover::TerrainVelocityNoise(bMatrix4 *world_to_camera, CameraAnchor *p_car, float f_speed_scale, float f_terrain_scale) {

    if (!p_car) {

        return;
    }

    const float speed_tresh = 5.0f;

    bVector4 v_speed_terrain_freq;
    tTable<bVector4> speed_table(CameraNoiseSpeedData, 5, 0.0f, 80.0f);

    speed_table.GetValue(&v_speed_terrain_freq, p_car->GetVelocityMagnitude());

    float f_road_noise_amplitude = p_car->GetSurface().CAMERA_NOISE(0);
    float f_road_noise_grid_spacing_inverse = p_car->GetSurface().CAMERA_NOISE(1);

    float f_speed_magnitude = f_speed_scale * v_speed_terrain_freq.x;
    float f_speed_frequency = v_speed_terrain_freq.z;

    const float accel_min = 0.0f;

    if (p_car->IsDragRace()) {

        f_speed_magnitude *= 1.5f;
    }

    if (p_car->GetVelocityMagnitude() > speed_tresh) {

        const float accel_max = 20.0f;

        float accel = bClamp(bDot(p_car->GetAcceleration(), p_car->GetForwardVector()), accel_min, accel_max) * 0.05f;
        f_speed_magnitude += accel * 0.15f;
        f_speed_frequency += (0.5f - f_speed_frequency) * accel;
    }
    if (p_car->IsOverRev()) {

        f_speed_magnitude += 0.15f;
        f_speed_frequency *= 0.7f;
    }
    if (p_car->IsNosEngaged() && p_car->GetVelocityMagnitude() > speed_tresh) {

        f_speed_magnitude = 0.3f;
        f_speed_frequency = 2.0f;
    }
    if (p_car->IsBrakeEngaged() && p_car->GetVelocityMagnitude() > speed_tresh) {

        f_speed_magnitude = 0.3f;
        f_speed_frequency = 5.0f;
    }

    float f_terrain_magnitude = f_terrain_scale * v_speed_terrain_freq.y * f_road_noise_amplitude;
    float f_terrain_frequency = v_speed_terrain_freq.w * f_road_noise_grid_spacing_inverse;

    if (!OutsidePOV()) {

        f_speed_magnitude *= 0.25f;
        f_terrain_magnitude *= 0.25f;
    }

    bVector4 v_speed_frequency;
    bVector4 v_speed_magnitude;

    bScale(&v_speed_frequency, &CameraNoiseSpeedFrequency, f_speed_frequency);
    bScale(&v_speed_magnitude, &CameraNoiseSpeedAmplitude, f_speed_magnitude);

    pCamera->SetNoiseFrequency1(&v_speed_frequency);
    pCamera->SetNoiseAmplitude1(&v_speed_magnitude);

    bVector4 v_terrain_frequency;
    bVector4 v_terrain_magnitude;

    bScale(&v_terrain_frequency, &CameraNoiseTerrainFrequency, f_terrain_frequency);
    bScale(&v_terrain_magnitude, &CameraNoiseTerrainAmplitude, f_terrain_magnitude);

    pCamera->SetNoiseFrequency2(&v_terrain_frequency);
    pCamera->SetNoiseAmplitude2(&v_terrain_magnitude);

    pCamera->ApplyNoise(world_to_camera, WorldTimer.GetSeconds(), 1.0f);
}

void CameraMover::ComputeBankedUpVector(bVector3 *up, bVector3 *eye, bVector3 *look, bAngle bank) {
    bMatrix4 axis_rotation;
    bVector3 axis;

    bSub(&axis, look, eye);

    bNormalize(&axis, &axis);

    eCreateAxisRotationMatrix(&axis_rotation, axis, bank);

    bVector3 new_up(0.0f, 0.0f, 1.0f);

    eMulVector(up, &axis_rotation, &new_up);
}

bool CameraMover::IsSomethingInBetween(const UMath::Vector4 &pos1, const UMath::Vector4 &pos2) {
    UMath::Vector4 seg[2];

    seg[0] = pos1;
    seg[1] = pos2;
    seg[0].y += 0.5f;
    seg[1].y += 0.5f;

    WCollisionMgr::WorldCollisionInfo cInfo;

    // Temporal, no local con nombre: el original guarda su direccion en un pseudo
    // (addi rX, r1, off + store por 0x4(rX)) en vez de plegarla a r1.
    WCollisionMgr(0, 3).CheckHitWorld(seg, cInfo, 3);
    if (cInfo.HitSomething()) {
        return true;
    }
    return false;
}

bool CameraMover::IsSomethingInBetween(const bVector3 *pos1, const bVector3 *pos2) {
    UMath::Vector4 v1;
    UMath::Vector4 v2;

    eUnSwizzleWorldVector(*pos1, (bVector3 &)v1);
    eUnSwizzleWorldVector(*pos2, (bVector3 &)v2);

    return IsSomethingInBetween(v1, v2);
}

float CameraMover::MinDistToWall() {
    return fMinDistToWall;
}

bool CameraMover::EnforceMinGapToWalls(WCollider *collider, bVector3 *pos, bVector3 *target, bVector4 *push) {
    bool hit = false;
    bVector3 adjust(0.0f, 0.0f, 0.0f);

    {
        float fov_scale = bClamp(bAngToDeg(pCamera->GetFieldOfView()) / 70.0f, 1.0f, 1.5f);

        bVector3 delta = *pos - *target;
        float dist = bLength(&delta);

        if (dist < 0.001f) {
            return false;
        }

        bVector3 dir = delta * (1.0f / dist);
        bVector3 up(0.0f, 0.0f, 1.0f);
        bVector3 side;

        bCross(&side, &up, &dir);

        const int kProbeSize = 2;
        bVector3 edge[kProbeSize];

        float half_width = fov_scale * 2.2f * dist * 0.2f;

        bScaleAdd(&edge[0], &delta, &side, half_width);
        bScaleAdd(&edge[1], &delta, &side, -half_width);

        float worst = 0.0f;
        float push_out;

        for (int i = 0; i < kProbeSize; i++) {
            {
                bVector3 normal = bNormalize(edge[i]);
                bVector3 probe;

                bScaleAdd(&probe, target, &normal, -0.9f);

                UMath::Vector4 seg[2];

                eUnSwizzleWorldVector(*pos, (bVector3 &)seg[0]);
                eUnSwizzleWorldVector(probe, (bVector3 &)seg[1]);

                WCollisionMgr::WorldCollisionInfo cInfo;

                hit = WCollisionMgr(0, 3).CheckHitWorld(seg, cInfo, 2);
                if (!hit) {
                    continue;
                }
                {
                    bVector3 hit_pos;
                    bVector3 hit_normal;

                    eSwizzleWorldVector((const bVector3 &)cInfo.fCollidePt, hit_pos);
                    eSwizzleWorldVector((const bVector3 &)cInfo.fNormal, hit_normal);

                    float grazing = (1.0f - bAbs(bDot(&dir, &hit_normal))) * 0.6f;
                    float gap = bDistBetween(pos, &probe);
                    float wall = bDistBetween(pos, &hit_pos);
                    push_out = (gap - wall) * (grazing + 1.0f);

                    if (push_out > worst) {
                        worst = push_out;
                    }
                }
            }
        }

        float average = (fAccumulatedClearance + worst) * 0.5f;
        fAccumulatedClearance += worst - average;

        if (average > 0.0f) {
            bScale(&adjust, &dir, average);
        }
    }
    bCopy(push, &adjust);

    return hit;
}

void CameraMover::IsoProjectionMatrix(bMatrix4 *dest, bVector3 *eye, bVector3 *look, bVector2 *offset) {
    bVector3 up(-dest->v0.y, -dest->v1.y, -dest->v2.y);
    bMatrix4 lookat;
    bMatrix4 inverse;
    bVector3 local;
    bVector3 world;

    eCreateLookAtMatrix(&lookat, *eye, *look, up);
    eInvertTransformationMatrix(&inverse, &lookat);

    eMulVector(&local, &lookat, look);
    local.x -= local.z * offset->x;
    local.y -= local.z * offset->y;

    eMulVector(&world, &inverse, &local);
    eCreateLookAtMatrix(dest, *eye, world, up);
}

float CameraMover::AdjustHeightAroundCar(const bVector3 *pos, bVector3 *eye, bVector3 *look) {
    for (Avoidables::const_iterator i = TheAvoidables->begin(); i != TheAvoidables->end(); ++i) {
        UMath::Matrix4 mat;
        bMatrix4 car;
        UMath::Vector3 dim;
        bVector3 bdim;
        bVector3 local;
        bVector3 unused; // sin usar: el original le reserva el hueco de 0x10 en la pila
        bMatrix4 inverse;
        bVector3 world;
        bVector2 diff;
        float zero = 0.0f; // muerta, pero mete el 0.0f en el pool antes que el 0.85f

        IBody *body = *i;

        body->GetTransform(mat);
        eSwizzleWorldMatrix((const bMatrix4 &)mat, car);

        bSub(&diff, (const bVector2 *)&car.v3, (const bVector2 *)pos);
        float dz = bAbs(car.v3.z - pos->z);
        float dist_sq = bDot(&diff, &diff);

        body->GetDimension(dim);

        bFill(&bdim, dim.z + 0.85f, dim.x + 0.85f, dim.y + 0.85f);

        float radius = bLength((const bVector2 *)&bdim);
        float radius_sq = radius * radius;

        if (dist_sq < radius_sq && dz < bdim.z * 2.0f) {
            eInvertTransformationMatrix(&inverse, &car);
            eMulVector(&local, &inverse, pos);

            float x2 = local.x * local.x;
            float y2 = local.y * local.y;
            float a2 = bdim.x * bdim.x;
            float b2 = bdim.y * bdim.y;
            float x4 = x2 * x2;
            float y4 = y2 * y2;
            float a4 = a2 * a2;
            float b4 = b2 * b2;
            float ellipse = x4 / a4 + y4 / b4;

            if (ellipse < 1.0f) {
                float height = bSqrt(bSqrt(1.0f - ellipse)) * bdim.z;

                if (height > local.z) {
                    local.z = height;

                    eMulVector(&world, &car, &local);

                    float adjust = world.z - pos->z;
                    if (adjust > radius) {
                        adjust = 0.0f;
                    }
                    return adjust;
                }
            }
        }
    }
    return 0.0f;
}

bVector3 *CameraMover::DutchAroundCar(bVector3 *eye, bVector3 *look) {
    static bVector3 ret(0.0f, 0.0f, 0.0f);

    ret.x = 0.0f;
    ret.z = 0.0f;
    ret.y = 0.0f;

    for (Avoidables::const_iterator i = TheAvoidables->begin(); i != TheAvoidables->end(); ++i) {
        UMath::Matrix4 mat;
        bMatrix4 car;
        bVector3 delta;
        bVector3 look_dir;
        UMath::Vector3 vel;
        bVector3 bvel;
        bVector3 vel_dir;

        IBody *body = *i;

        body->GetTransform(mat);
        eSwizzleWorldMatrix((const bMatrix4 &)mat, car);

        bSub(&delta, (const bVector3 *)&car.v3, eye);
        float dist_sq = bDot(&delta, &delta);

        if (dist_sq > 0.8f * 0.8f && dist_sq < 49.0f) {
            bNormalize(&delta, &delta);
            bNormalize(&look_dir, look);

            if (bDot(&delta, &look_dir) > 0.5f) {
                float strength = bClamp(4.84f / dist_sq, 0.0f, 1.0f);

                body->GetLinearVelocity(vel);
                eSwizzleWorldVector((const bVector3 &)vel, bvel);

                float speed = bClamp((bDistBetween(look, &bvel) - 15.0f) / 85.0f, 0.0f, 1.0f);

                bNormalize(&vel_dir, &bvel);
                bNormalize(&look_dir, look);

                float dot = bDot(&look_dir, &vel_dir);
                float dutch = 0.0f;
                if (dot < 0.0f) {
                    dutch = -dot;
                }
                bScaleAdd(&ret, &ret, &delta, strength * (speed + dutch));
            }
        }
    }
    return &ret;
}

bool CameraMover::MinGapCars(bMatrix4 *dest, bVector3 *pos, bVector3 *dim) {
    bMatrix4 inverse;
    bVector3 local;
    bVector2 scale;

    bool heighted = false;

    eInvertTransformationMatrix(&inverse, dest);
    bVector3 *cam = (bVector3 *)&inverse.v3;

    float saved_z = cam->z;

    int count = 0;
    float height;

    while (1) {
        height = AdjustHeightAroundCar(cam, pos, dim);
        if (count > 15) {
            break;
        }
        if (height <= 0.0f) {
            break;
        }
        cam->z += height;
        heighted = true;
        count++;
        // el original vuelve a llamar aqui y descarta el resultado: la condicion
        // del bucle lo recalcula acto seguido.
        height = AdjustHeightAroundCar(cam, pos, dim);
    }

    float dot = bDot(&vSavedForward, (const bVector3 *)&dest->v0);
    bCopy(&vSavedForward, (const bVector3 *)&dest->v0);

    height = cam->z - saved_z;

    if (bLength((const bVector2 *)dim) < 1.0f && dot > 0.9f) {
        if (height < fSavedAdjust) {
            height = fSavedAdjust;
        }
    }
    fSavedAdjust = height;

    float average = (fAccumulatedAdjust + height) * 0.5f;
    fAccumulatedAdjust += height - average;

    cam->z = saved_z + average;

    eMulVector(&local, dest, pos);
    if (local.z <= 0.0f) {
        return false;
    }
    bFill(&scale, local.x / local.z, local.y / local.z);
    IsoProjectionMatrix(dest, cam, pos, &scale);

    return heighted;
}

bool CameraMover::MinGapTopology(bMatrix4 *pMatrix, bVector3 *pCarPos) {

    bMatrix4 mCameraToWorld;

    eInvertTransformationMatrix(&mCameraToWorld, pMatrix);

    bVector3 *pCameraPos = (bVector3 *)&mCameraToWorld.v3;

    bVector3 vCenter;

    bScale(&vCenter, pCameraPos, 0.5f);
    bScaleAdd(&vCenter, &vCenter, pCarPos, 0.5f);

    bVector4 vAdjust(0.0f, 0.0f, 0.0f, 1.0f);

    bool bViolates;

    {
        float fRadius = (bDistBetween(pCarPos, pCameraPos) + 1.0f) * 0.5f;

        if (fRadius > 24.9f) {
            fRadius = 24.9f;
        }

        UMath::Vector3 usCenter;

        eUnSwizzleWorldVector(vCenter, (bVector3 &)usCenter);

        mCollider->Refresh(usCenter, fRadius, true);
    }

    UMath::Vector3 usCamPos;
    UMath::Vector3 usCarPos;
    UMath::Vector4 seg[2];

    eUnSwizzleWorldVector(*pCameraPos, (bVector3 &)usCamPos);
    eUnSwizzleWorldVector(*pCarPos, (bVector3 &)usCarPos);

    seg[0] = UMath::Vector4Make(usCamPos, 1.0f);
    seg[1] = UMath::Vector4Make(usCarPos, 1.0f);

    bViolates = EnforceMinGapToWalls(mCollider, pCarPos, pCameraPos, &vAdjust);

    *pCameraPos += (const bVector3 &)vAdjust;

    {
        bool inTunnel = AmIinATunnel(eGetView(ViewID, true), 1);

        eUnSwizzleWorldVector(*pCameraPos, (bVector3 &)usCamPos);

        if (!inTunnel && usCamPos.y < pCarPos->z + 0.5f) {
            usCamPos.y = pCarPos->z + 0.5f;
        }

        WWorldPos temp(0.025f);

        temp.FindClosestFace(mCollider, usCamPos, true);

        if (temp.OnValidFace()) {
            UMath::Vector3 norm;

            temp.UNormal(&norm);

            float height = WWorldMath::GetPlaneY(norm, UMath::Vector4To3(temp.FacePoint(0)), usCamPos);

            if (pCameraPos->z < height + 0.25f) {
                pCameraPos->z = height + 0.25f;
            }

            if (inTunnel && pCameraPos->z > height + 4.5f) {
                pCameraPos->z = height + 4.5f;
            }
        }
    }

    bVector3 vCarCameraSpace;

    eMulVector(&vCarCameraSpace, pMatrix, pCarPos);

    if (vCarCameraSpace.z <= 0.0f) {
        return false;
    }

    bVector2 vProjection(vCarCameraSpace.x / vCarCameraSpace.z, vCarCameraSpace.y / vCarCameraSpace.z);

    IsoProjectionMatrix(pMatrix, pCameraPos, pCarPos, &vProjection);

    return bViolates;
}

bool CameraMover::OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) {
    if (userdata != NULL) {
        bVector3 pos;

        eSwizzleWorldVector(cPoint, pos);
    }
    return true;
}

WUID CameraMover::GetAnchorID() {
    CameraAnchor *anchor = GetAnchor();
    return anchor == NULL ? 0 : anchor->GetWorldID();
}

void CameraMover::FovCubicInit(tCubic1D *cubic) {
    float val = (float)pCamera->GetFieldOfView() + (float)pCamera->GetVelocityFov() * (1.0f / 30.0f);
    float dval = (float)pCamera->GetVelocityFov() * cubic->duration;
    cubic->SetVal(val);
    cubic->SetdVal(dval);
}

void CameraMover::EyeCubicInit(tCubic3D *cubic, bMatrix4 *world_to_camera, bVector3 *offset) {
    bVector3 pos;
    bScaleAdd(&pos, pCamera->GetPosition(), pCamera->GetVelocityPosition(), 1.0f / 30.0f);
    if (world_to_camera != NULL) {
        bMulMatrix(&pos, world_to_camera, &pos);
    }
    cubic->SetVal(&pos);

    bVector3 vel(*pCamera->GetVelocityPosition());
    if (offset != NULL) {
        bSub(&vel, &vel, offset);
    }

    bVector4 dvel(vel.x * cubic->x.duration, vel.y * cubic->x.duration, vel.z * cubic->x.duration, 0.0f);
    if (world_to_camera != NULL) {
        bMulMatrix(&dvel, world_to_camera, &dvel);
    }
    cubic->SetdVal((bVector3 *) &dvel);
}

void CameraMover::LookCubicInit(tCubic3D *cubic, bMatrix4 *world_to_camera, bVector3 *offset) {
    bVector3 pos;
    bScaleAdd(&pos, pCamera->GetTarget(), pCamera->GetVelocityTarget(), 1.0f / 30.0f);
    if (world_to_camera != NULL) {
        bMulMatrix(&pos, world_to_camera, &pos);
    }
    cubic->SetVal(&pos);

    bVector3 vel(*pCamera->GetVelocityTarget());
    if (offset != NULL) {
        bSub(&vel, &vel, offset);
    }

    bVector4 dvel(vel.x * cubic->x.duration, vel.y * cubic->x.duration, vel.z * cubic->x.duration, 0.0f);
    if (world_to_camera != NULL) {
        bMulMatrix(&dvel, world_to_camera, &dvel);
    }
    cubic->SetdVal((bVector3 *) &dvel);
}

void CameraMover::SetEyeLook(tCubic3D *eye, tCubic3D *look, tCubic1D *fov, bMatrix4 *world_to_camera, bVector3 *offset) {
    FovCubicInit(fov);
    EyeCubicInit(eye, world_to_camera, offset);
    LookCubicInit(look, world_to_camera, offset);
}

void RenderCameraMovers(eView *view) {
    CameraMover *cm = view->GetCameraMover();

    if (cm != NULL) {
        view->GetCameraMover()->Render(view);
    }
}

void UpdateCameraMovers(float dT) {
    eView *view;

    for (int view_id = 0; view_id < 22; ++view_id) {
        eView *view = eGetView(view_id, false);
        if (view != nullptr) {
            CameraMover *m = view->GetCameraMover();
            if (m != nullptr) {
                m->Update(dT);
                {
                    Camera *camera = view->GetCamera();
                    {
                        bVector3 *cam_pos = camera->GetPosition();
                    }
                }
            }
        }
    }

    if (!WeHaveCheckedIfJR2ServerExists) {
        JR2ServerExists = bFunkDoesServerExist("JR2Server");
        WeHaveCheckedIfJR2ServerExists = 1;
    }

    if (JR2ServerExists) {
        view = eGetView(1, false);
        Camera *camera = view->GetCamera();
        if (camera != nullptr) {
            if (bAbs(RealTime - LastUpdateTimeJR2) > 16) {
                LastUpdateTimeJR2 = RealTime;
                camera->CommunicateWithJollyRancher("SpeedCam");
            }
        }
    }

    extern const int RemoteCaffeinating;
    if (RemoteCaffeinating != 0 && DisableCommunication == 0) {
        view = eGetView(1, false);
        Camera *camera = view->GetCamera();
        if (camera != nullptr) {
            if (bAbs(RealTime - LastUpdateTimeCaffeine) > 16) {
                LastUpdateTimeCaffeine = RealTime;

                bVector3 eye = *camera->GetPosition() - *camera->GetDirection() * 50.0f;
                bVector3 look = *camera->GetPosition();

                LongVector fix_eye;
                LongVector fix_look;
                fix_eye.x = static_cast<int>(eye.x * 65536.0f);
                fix_eye.y = static_cast<int>(eye.y * 65536.0f);
                fix_eye.z = static_cast<int>(eye.z * 65536.0f);
                fix_look.x = static_cast<int>(look.x * 65536.0f);
                fix_look.y = static_cast<int>(look.y * 65536.0f);
                fix_look.z = static_cast<int>(look.z * 65536.0f);
                espSetCameraPositionFix(&fix_eye, &fix_look);

                {
                    static bVector3 prev_position(9999.0f, 9999.0f, 9999.0f);

                    if (bDistBetween(&prev_position, camera->GetPosition()) > 50.0f) {
                        prev_position = *camera->GetPosition();
                        bLength(reinterpret_cast<const bVector2 *>(camera->GetPosition()));
                    }
                }
            }
        }
    }

    if (GManager::Exists() && GManager::Get().GetIsWarping()) {
        return;
    }
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetIsScriptWaitingForLoading()) {
        return;
    }

    bool streamerCleared = false;
    for (int view_id = 1; view_id < 3; ++view_id) {
        view = eGetView(view_id, false);
        if (view->IsActive() && view->GetCameraMover() != nullptr) {
            if (!streamerCleared) {
                streamerCleared = true;
                TheTrackStreamer.ClearStreamingPositions();
            }

            bVector3 position;
            bVector3 velocity;
            bVector3 direction;

            bCopy(&position, view->GetCamera()->GetPosition());
            bCopy(&velocity, view->GetCamera()->GetVelocityPosition());
            bCopy(&direction, view->GetCamera()->GetDirection());

            if (bStreamingPositionFromICE) {
                INIS *inis = UTL::Collections::Singleton<INIS>::Get();
                if (inis != nullptr) {
                    const UMath::Vector3 *editorPos = inis->GetStartCameraLocation();
                    bConvertFromBond(position, *editorPos);
                }
                velocity.x = 0.0f;
                velocity.y = 0.0f;
                direction = velocity;
            }

            bool freezePrediction = false;
            CameraMoverTypes type = view->GetCameraMover()->GetType();
            if (type == CM_DRIVE_CUBIC) {
                freezePrediction = true;
            }

            int streaming_pos = view_id == 2;

            TheTrackStreamer.PredictStreamingPosition(streaming_pos, &position, &velocity, &direction, freezePrediction);
        }
    }

}

CameraAnchor::~CameraAnchor() {}

void CameraAnchor::Update(float dT, const bMatrix4 &mat, const bVector3 &vel, const bVector3 &dim) {
    float distance = bDistBetween(&mGeomPos, (const bVector3 *)&mat.v3);

    bCopy(&mGeomRot, &mat);
    mGeomRot.v3.z = 0.0f;
    mGeomRot.v3.y = 0.0f;
    mGeomRot.v3.x = 0.0f;

    mGeomPos.x = mat.v3.x;
    mGeomPos.y = mat.v3.y;
    mGeomPos.z = mat.v3.z;

    bCopy(&mVelocity, &vel);

    float prev_mag = mVelMag;
    mVelMag = bLength(&mVelocity);

    if (dT > 0.0f && distance / dT < 300.0f) {
        bVector3 accel((mVelMag - prev_mag) / dT, 0.0f, 0.0f);

        bMulMatrix(&mAccel, &mGeomRot, &accel);
    } else {
        mAccel.z = 0.0f;
        mAccel.y = 0.0f;
        mAccel.x = 0.0f;
    }
}

void CameraAnchor::SetModel(int model) {
    if (mModel != model) {
        char *name = GetCarTypeInfoFromHash(model)->GetName();

        if (name == NULL) {
            name = "";
        }
        mModel = model;
        mModelAttributes.ChangeWithDefault(Attrib::StringToLowerCaseKey(name));
    }
}

POV *CameraAnchor::GetPov(int pov_type) {
    mPOV.Type = pov_type;

    switch (pov_type) {
    case 0:
        mCameraInfoAttributes.ChangeWithDefault(mModelAttributes.CameraInfo_Bumper());
        break;
    case 1:
        mCameraInfoAttributes.ChangeWithDefault(mModelAttributes.CameraInfo_Hood());
        break;
    case 2:
        mCameraInfoAttributes.ChangeWithDefault(mModelAttributes.CameraInfo_Close());
        break;
    case 3:
        mCameraInfoAttributes.ChangeWithDefault(mModelAttributes.CameraInfo_Far());
        break;
    case 4:
        mCameraInfoAttributes.ChangeWithDefault(mModelAttributes.CameraInfo_SuperFar());
        break;
    case 5:
        mCameraInfoAttributes.ChangeWithDefault(mModelAttributes.CameraInfo_Drift());
        break;
    case 6:
        mCameraInfoAttributes.ChangeWithDefault(mModelAttributes.CameraInfo_Pursuit());
        break;
    default:
        mPOV.Type = 3;
        mCameraInfoAttributes.Change(0xeec2271a);
        break;
    }

    float zoom = mZoom > 0.0f ? mZoom : 1.0f;
    int index = eGetCurrentViewMode() == EVIEWMODE_TWOH;

    mPOV.Angle = bDegToAng(mCameraInfoAttributes.ANGLE(index));
    mPOV.Lag = mCameraInfoAttributes.LAG(index) / zoom;
    mPOV.Height = mCameraInfoAttributes.HEIGHT(index);
    mPOV.LatOffset = mCameraInfoAttributes.LATOFFSET(index);
    mPOV.Fov = bDegToAng(mCameraInfoAttributes.FOV(index) * zoom);
    mPOV.AllowTilting = mCameraInfoAttributes.TILTING(index);
    mPOV.Stiffness = mCameraInfoAttributes.STIFFNESS(index);

    return &mPOV;
}

CameraAnchor::CameraAnchor(int model)
    : mVelocity(0.0f, 0.0f, 0.0f), mVelMag(0.0f), mTopSpeed(0.0f), mGeomPos(0.0f, 0.0f, 0.0f), mDimension(0.0f, 0.0f, 0.0f),
      mAccel(0.0f, 0.0f, 0.0f), mModel(0), mWorldID(0), mSurface(SimSurface::kNull), mCollisionDamping(0.0f), mDrift(0.0f), mGroundCollision(0.0f),
      mObjectCollision(0.0f), mIsNosEngaged(false), mIsBrakeEngaged(false), mIsDragRace(false), mIsOverRev(false), mIsTouchingGround(false),
      mIsVehicleDestroyed(false), mIsGearChanging(false), mIsCloseToRoadBlock(false), mZoom(1.0f), mModelAttributes(0xeec2271a, 0, NULL),
      mCameraInfoAttributes(0xeec2271a, 0, NULL) {
    mPOV.Type = 3;
    mPOV.Angle = bDegToAng(mCameraInfoAttributes.ANGLE(0));
    mPOV.Lag = mCameraInfoAttributes.LAG(0);
    mPOV.Height = mCameraInfoAttributes.HEIGHT(0);
    mPOV.LatOffset = mCameraInfoAttributes.LATOFFSET(0);
    mPOV.Fov = bDegToAng(mCameraInfoAttributes.FOV(0));
    mPOV.AllowTilting = mCameraInfoAttributes.TILTING(0);
    mPOV.Stiffness = mCameraInfoAttributes.STIFFNESS(0);

    SetModel(model);
    bIdentity(&mGeomRot);
}
