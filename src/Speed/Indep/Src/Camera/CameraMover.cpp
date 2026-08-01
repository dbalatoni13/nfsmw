#include "Speed\Indep\Src\Camera\CameraMover.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

bool DoesCameraTypeDisablePreculler(CameraMoverTypes type) {
    if (type == CM_DEBUG_WORLD) {
        return true;
    }
    return type == CM_TRACK_CAR;
}

CameraMover::CameraMover(int view_id, CameraMoverTypes type) : mWPos(0.025f) {
    mCollider = WCollider::Create(0, WCollider::kColliderShape_Sphere, 0x1c, 0);
    Type = type;
    ViewID = view_id;
    Enabled = 0;
    fAccumulatedClearance = 0;
    fAccumulatedAdjust = 0;
    fSavedAdjust = 0;
    vSavedForward.x = 0;
    vSavedForward.z = 0;
    vSavedForward.y = 0;
    if (view_id == -1) {
        this->RenderDash = 0;
        this->pView = nullptr;
        this->pCamera = nullptr;
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

void CameraMover::Render(eView *view) {}

void CameraMover::Enable() {
    if (Enabled == 0) {
        Enabled = 1;
        if (pCamera->StopUpdating == 0) {
            pCamera->SetRenderDash(RenderDash);
        }
        pView->AttachCameraMover(this);
        pCamera->SetNearZ(0.5);
    }
}

void CameraMover::Disable() {
    if (Enabled != 0) {
        Enabled = 0;
        RenderDash = pCamera->RenderDash;
        pView->UnattachCameraMover(this);
    }
    return;
}
void CameraMover::ChopperNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer) {

    if (f_scale > 0.0) {
        const UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List &vehicles =
            UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_ALL);
        for (IVehicle *const *iter = vehicles.begin(); iter != vehicles.end(); iter++) {
            IVehicle *vehicle = *iter;
            if (vehicle->IsActive()) {
                continue;
            }
            if (vehicle->GetVehicleClass() != VehicleClass::CHOPPER) {
                continue;
            }
            const UMath::Vector3 &pos = vehicle->GetPosition();

            bVector3 bpos;

            eSwizzleWorldVector(pos, bpos);

            bVector3 dir;

            bSub(&dir, &bpos, &pCamera->CurrentKey.Position);

            float distance = bLength(&dir);

            float intensity = f_scale * (1.0f - distance * 0.025f);

            bVector4 v_frequency = CameraNoiseChopperFrequency;
            bVector4 v_magnitude;
            bScale(&v_magnitude, &CameraNoiseChopperAmplitude, intensity);

            if (distance < 40.0f) {

                pCamera->SetNoiseFrequency1(&v_frequency);
                pCamera->SetNoiseAmplitude1(&v_magnitude);
            }
            float time = useWorldTimer ? WorldTimer.GetSeconds() : RealTimer.GetSeconds();
            pCamera->ApplyNoise(world_to_camera, time * 0.00025, 1.0);
        }
    }
}

void CameraMover::HandheldNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer) {
    bVector4 v_frequency;
    bVector4 v_magnitude;

    if (f_scale <= 0.0f)
        return;

    bVector4 *pfreq = &v_frequency;

    pfreq->x = CameraNoiseHandheldFrequency.x;
    pfreq->y = CameraNoiseHandheldFrequency.y;

    float ampW = CameraNoiseHandheldAmplitude.w;
    float ampY = CameraNoiseHandheldAmplitude.y;

    if (useWorldTimer) {
    }

    float ampZ = CameraNoiseHandheldAmplitude.z;

    ampW *= f_scale;

    pfreq->z = CameraNoiseHandheldFrequency.z;

    ampY *= f_scale;

    float ampX = CameraNoiseHandheldAmplitude.x;
    float freqW = CameraNoiseHandheldFrequency.w;

    ampX *= f_scale;

    v_magnitude.x = ampX;
    v_magnitude.y = ampY;
    v_magnitude.z = ampZ * f_scale;
    v_magnitude.w = ampW;

    float fx = pfreq->x;
    float fy = pfreq->y;
    float fz = pfreq->z;

    float ax = v_magnitude.x;
    float ay = v_magnitude.y;
    float az = v_magnitude.z;
    float aw = v_magnitude.w;

    pCamera->CurrentKey.NoiseFrequency1.w = freqW;
    pCamera->CurrentKey.NoiseFrequency1.x = fx;
    pCamera->CurrentKey.NoiseFrequency1.y = fy;
    pCamera->CurrentKey.NoiseFrequency1.z = fz;

    pCamera->CurrentKey.NoiseAmplitude1.x = ax;
    pCamera->CurrentKey.NoiseAmplitude1.w = aw;
    pCamera->CurrentKey.NoiseAmplitude1.y = ay;
    pCamera->CurrentKey.NoiseAmplitude1.z = az;

    int packedTime = useWorldTimer ? WorldTimer.GetPackedTime() : RealTimer.GetPackedTime();

    pCamera->ApplyNoise(world_to_camera, (float)packedTime * 0.00025f, 1.0f);
}

void CameraMover::ComputeBankedUpVector(bVector3 *up, bVector3 *eye, bVector3 *look, bAngle bank) {

    bVector3 diff;

    diff.x = look->x - eye->x;
    diff.z = look->z - eye->z;
    diff.y = look->y - eye->y;

    bVector3 axis;
    bNormalize(&axis, &diff);

    bMatrix4 rotationMatrix;
    eCreateAxisRotationMatrix(&rotationMatrix, *&axis, bank);

    bVector3 defaultVec = bVector3(0.0, 0.0, 1.0);
    eMulVector(up, &rotationMatrix, &defaultVec);
    return;
}

CameraMover::~CameraMover() {
    WCollider::Destroy(mCollider);

    if (DoesCameraTypeDisablePreculler(Type)) {
        DisablePrecullerCounter--;
    }
    Disable();
}

void CameraMover::Update(float dT) {
    return;
}

float CameraMover::MinDistToWall() {
    return 0.7;
}

void CameraMoverRestartRace() {

    WeHaveCheckedIfJR2ServerExists = 0;
    CameraAI::Reset();

    for (int view_id = 1; view_id < 4; ++view_id) {
        eView *view = eGetView(view_id, false);
        CameraMover *cm = view->GetCameraMover();
        if (cm != nullptr) {
            cm->ResetState();
        }
    }
}
