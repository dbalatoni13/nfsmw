#include "Speed\Indep\Src\Camera\CameraMover.hpp"
#include "ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

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
    vSavedForward.x = 0;
    vSavedForward.y = 0;
    vSavedForward.z = 0;
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

CameraMover::~CameraMover() {
    WCollider::Destroy(mCollider);

    if (DoesCameraTypeDisablePreculler(Type)) {
        EnablePreculler();
    }
    Disable();
}

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

                float time = 0.00025f * (useWorldTimer ? WorldTimer.GetSeconds() : RealTimer.GetSeconds()); // GetSeconds() generate fmuls...
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

        float time = 0.00025f * (useWorldTimer ? WorldTimer.GetSeconds()
                                               : RealTimer.GetSeconds()); //  / TIMER_SHIFT_VALUE_FLOAT in GetSeconds() generate fmuls...

        pCamera->ApplyNoise(world_to_camera, time, 1.0f);
    }
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

float CameraMover::MinDistToWall() {
    return fMinDistToWall;
}

void UpdateCameraMovers(float dT) {

    for (int view_id = 0; view_id < 22; ++view_id) {
        eView *view = eGetView(view_id, false);

        if (view != nullptr) {
            CameraMover *m = view->GetCameraMover();

            if (m != nullptr) {
                Camera *camera = view->GetCamera();
                if (camera != nullptr) {
                    bVector3 *cam_pos = camera->GetPosition();
                    m->Update(dT);
                }
            }
        }
    }

    if (!WeHaveCheckedIfJR2ServerExists) {
        JR2ServerExists = bFunkDoesServerExist("JR2Server");
        WeHaveCheckedIfJR2ServerExists = 1;
    }

    if (JR2ServerExists) {
        eView *view = eGetView(1, false);
        int elapsed = bAbs(RealTime - LastUpdateTimeJR2);
        if (elapsed > 16) {
            LastUpdateTimeJR2 = RealTime;
            view->pCamera->CommunicateWithJollyRancher("SpeedCam");
        }
    }

    if (RemoteCaffeinating != 0 && DisableCommunication == 0) {
        eView *view = eGetView(1, false);
        if (view->pCamera != nullptr && bAbs(RealTime - LastUpdateTimeCaffeine) > 16) {

            LastUpdateTimeCaffeine = RealTime;

            bVector3 eye;
            bVector3 look;
            LongVector fix_eye;
            LongVector fix_look;

            bVector3 prev_position(0.0f, 0.0f, 0.0f);

            float scale = 50.0f;

            bScale(&look, view->pCamera->GetPosition(), scale);
            bScale(&look, view->pCamera->GetDirection(), scale);
            bVector3 diff = eye - prev_position;

            // espSetCameraPositionFix(&fix_eye, &fix_look); // need

            float dist = bDistBetween(&diff, &prev_position);
            if (dist < 10.0f) {
            }

            // espCentrePlaneView();
            // todo espresso
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
        eView *view = eGetView(view_id, false);

        if (!view->Active) {
            continue;
        }

        CameraMover *cm = view->GetCameraMover();
        if (cm == nullptr) {
            continue;
        }

        if (!streamerCleared) {
            TheTrackStreamer.ClearStreamingPositions();
            streamerCleared = true;
        }

        Camera *camera = view->GetCamera();

        bVector3 position = *camera->GetPosition();
        bVector3 velocity = *camera->GetVelocityPosition();
        bVector3 direction = *camera->GetDirection();

        IPlayer *player = IPlayer::First(PLAYER_LOCAL);
        if (player != nullptr) {
            ISimable *simable = player->GetSimable();
            if (simable != nullptr) {
                IRigidBody *body = simable->GetRigidBody();
                if (body) {
                    bConvertFromBond(position, body->GetPosition());
                }
            }
        }

        if (bStreamingPositionFromICE) {
            INIS *inis = UTL::Collections::Singleton<INIS>::Get();
            if (inis != nullptr) {
                const UMath::Vector3 *editorPos = inis->GetStartCameraLocation();

                position.x = editorPos->z;
                position.z = editorPos->y;
                position.y = -editorPos->x;
            }

            velocity = bVector3(0.0f, 0.0f, 0.0f);
            direction = bVector3(0.0f, 0.0f, 0.0f);
        }

        const bool rearView = (view_id == 2);

        const bool freezePrediction = (view->CameraMoverList.GetHead()->Next->Prev == reinterpret_cast<bNode *>(1));

        TheTrackStreamer.PredictStreamingPosition(rearView, &position, &velocity, &direction, freezePrediction);
    }
}
