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
        if (view->pCamera != nullptr) {
            int elapsed = bAbs(RealTime - LastUpdateTimeCaffeine);
            if (elapsed > 16) {
                LastUpdateTimeCaffeine = RealTime;

                bVector3 eye;
                bVector3 look;
                Vector3 fix_eye;  // LongVector
                Vector3 fix_look; // LongVector

                bVector3 prev_position(0.0f, 0.0f, 0.0f);

                float scale = 50.0f; // DAT_803d1e90
                eye = view->pCamera->CurrentKey.Position * scale;
                look = view->pCamera->CurrentKey.Direction * scale;

                bVector3 diff = eye - prev_position;

                // espSetCameraPositionFix(&fix_eye, &fix_look); // need

                float dist = bDistBetween(&diff, &prev_position);
                if (dist < 10.0f) {
                }

                // espCentrePlaneView();
                // todo
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
    for (int viewIndex = 1; viewIndex < 3; ++viewIndex) {
        eView *view = eGetView(viewIndex, false);

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

        bVector3 position = camera->CurrentKey.Position;
        bVector3 velocity = camera->VelocityKey.Position;
        bVector3 direction = camera->CurrentKey.Direction;

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
                const Vector3 *editorPos = inis->GetStartCameraLocation();

                position.x = editorPos->z;
                position.z = editorPos->y;
                position.y = -editorPos->x;
            }

            velocity = bVector3(0.0f, 0.0f, 0.0f);
            direction = bVector3(0.0f, 0.0f, 0.0f);
        }

        const bool rearView = (viewIndex == 2);

        const bool freezePrediction = (view->CameraMoverList.GetHead()->Next->Prev == reinterpret_cast<bNode *>(1));

        TheTrackStreamer.PredictStreamingPosition(rearView, &position, &velocity, &direction, freezePrediction);
    }
}
