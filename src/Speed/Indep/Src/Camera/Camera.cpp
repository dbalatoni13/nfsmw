#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include <cstddef>

Camera::Camera() {
    int RealTime;
    bMatrix4 defaultMatrix;

    RealTime = RealTimeFrames;
    LastUpdateTime = -0x80000000;
    LastDisparateTime = RealTime;
    RenderDash = 0;

    defaultMatrix.v2.z = -1.0f;
    defaultMatrix.v3.z = 1200.0f;
    defaultMatrix.v2.w = 100.0f;

    CurrentKey.TargetDistance = 10.0f;
    CurrentKey.NearZ = 0.5f;
    bClearVelocity = false;

    defaultMatrix.v1.x = 0.0f;
    defaultMatrix.v2.x = 0.0f;
    defaultMatrix.v3.x = 0.0f;
    defaultMatrix.v0.y = 0.0f;
    defaultMatrix.v1.y = -1.0f;
    defaultMatrix.v2.y = 0.0f;
    defaultMatrix.v3.y = 0.0f;

    defaultMatrix.v0.z = 0.0f;
    defaultMatrix.v1.z = 0.0f;
    defaultMatrix.v0.w = 0.0f;
    defaultMatrix.v1.w = 0.0f;

    CurrentKey.FocalDistance = 0.0f;
    CurrentKey.DepthOfField = 0.0f;
    ElapsedTime = 1.0f;

    defaultMatrix.v0.x = 1.0f;
    defaultMatrix.v3.w = 1.0f;

    CurrentKey.FarZ = 10000.0f;
    CurrentKey.FieldOfView = 0x36fb;

    CurrentKey.NoiseAmplitude2.w = 0.0f;
    CurrentKey.SimTimeMultiplier = 1.0f;
    CurrentKey.LB_height = 0.0f;
    CurrentKey.NoiseFrequency1.x = 1.0f;
    CurrentKey.NoiseAmplitude1.x = 0.0f;
    CurrentKey.NoiseAmplitude1.y = 0.0f;
    CurrentKey.NoiseAmplitude1.z = 0.0f;
    CurrentKey.NoiseAmplitude1.w = 0.0f;

    CurrentKey.NoiseFrequency1.y = 1.0f;
    CurrentKey.NoiseFrequency1.z = 1.0f;
    CurrentKey.NoiseFrequency1.w = 1.0f;
    CurrentKey.NoiseFrequency2.x = 1.0f;
    CurrentKey.NoiseFrequency2.y = 1.0f;
    CurrentKey.NoiseFrequency2.z = 1.0f;
    CurrentKey.NoiseFrequency2.w = 1.0f;

    CurrentKey.NoiseAmplitude2.x = 0.0f;
    CurrentKey.NoiseAmplitude2.y = 0.0f;
    CurrentKey.NoiseAmplitude2.z = 0.0f;

    SetCameraMatrix(defaultMatrix, 1.0f);
    SetCameraMatrix(defaultMatrix, 1.0f);
}

void Camera::SetCameraMatrix(const bMatrix4 &m, float fTime) {
    static int cameralink;
    struct bMatrix4 t;

    if (StopUpdating == 0) {
        bMatrix4 scaledmatrix;
        bMemCpy(&PreviousKey, this, 0xd4);
        ElapsedTime = fTime;

        if (Camera::JollyRancherResponse.UseMatrix == 0 || DisableCommunication != 0) {
            if (cameralink != 0) {
                cameralink = 0;
            }
            bMemCpy(this, &m, sizeof(bMatrix4));
        } else {
            if (cameralink == 0) {
                cameralink = 1;
            }
            bMemCpy(&t, const_cast<const bMatrix4 *>(&Camera::JollyRancherResponse.CamMatrix), sizeof(JollyRancherResponse.CamMatrix));
            t.v3.x *= 0.01f;
            t.v3.y *= 0.01f;
            t.v3.z *= 0.01f;
            t.v3.w = 1.0f;
            bMemCpy(this, &m, sizeof(bMatrix4));
        }

        bTransposeMatrix(&t, (bMatrix4 *)this);

        t.v0.w = 0.0f;
        t.v1.w = 0.0f;
        t.v2.w = 0.0f;
        eMulVector(&CurrentKey.Position, &t, (bVector3 *)&CurrentKey.Matrix.v3);

        CurrentKey.Position.x = -CurrentKey.Position.x;
        CurrentKey.Position.y = -CurrentKey.Position.y;
        CurrentKey.Position.z = -CurrentKey.Position.z;

        bNormalize(&CurrentKey.Direction, (bVector3 *)&t.v2);

        float targetDist = CurrentKey.TargetDistance;
        CurrentKey.Target.x = CurrentKey.Direction.x * targetDist + CurrentKey.Position.x;
        CurrentKey.Target.y = CurrentKey.Direction.y * targetDist + CurrentKey.Position.y;
        CurrentKey.Target.z = CurrentKey.Direction.z * targetDist + CurrentKey.Position.z;

        if (bClearVelocity) {
            bClearVelocity = false;
            bMemCpy(&PreviousKey, this, 0xd4);
            ElapsedTime = 1.0f;
        }

        if (ElapsedTime > 0.0f) {
            float invTime = 1.0f / ElapsedTime;

            VelocityKey.Position.x = (CurrentKey.Position.x - PreviousKey.Position.x) * invTime;
            VelocityKey.Position.y = (CurrentKey.Position.y - PreviousKey.Position.y) * invTime;
            VelocityKey.Position.z = (CurrentKey.Position.z - PreviousKey.Position.z) * invTime;

            VelocityKey.Direction.x = (CurrentKey.Direction.x - PreviousKey.Direction.x) * invTime;
            VelocityKey.Direction.y = (CurrentKey.Direction.y - PreviousKey.Direction.y) * invTime;
            VelocityKey.Direction.z = (CurrentKey.Direction.z - PreviousKey.Direction.z) * invTime;

            VelocityKey.Target.x = (CurrentKey.Target.x - PreviousKey.Target.x) * invTime;
            VelocityKey.Target.y = (CurrentKey.Target.y - PreviousKey.Target.y) * invTime;
            VelocityKey.Target.z = (CurrentKey.Target.z - PreviousKey.Target.z) * invTime;

            VelocityKey.TargetDistance = (CurrentKey.TargetDistance - PreviousKey.TargetDistance) * invTime;
            VelocityKey.FocalDistance = (CurrentKey.FocalDistance - PreviousKey.FocalDistance) * invTime;
            VelocityKey.DepthOfField = (CurrentKey.DepthOfField - PreviousKey.DepthOfField) * invTime;
            VelocityKey.NearZ = (CurrentKey.NearZ - PreviousKey.NearZ) * invTime;
            VelocityKey.FarZ = (CurrentKey.FarZ - PreviousKey.FarZ) * invTime;
            VelocityKey.LB_height = (CurrentKey.LB_height - PreviousKey.LB_height) * invTime;
            VelocityKey.SimTimeMultiplier = (CurrentKey.SimTimeMultiplier - PreviousKey.SimTimeMultiplier) * invTime;

            VelocityKey.FieldOfView = static_cast<unsigned short>(
                static_cast<int>(invTime * static_cast<float>(static_cast<short>(CurrentKey.FieldOfView - PreviousKey.FieldOfView))));

            VelocityKey.NoiseFrequency1.x = (CurrentKey.NoiseFrequency1.x - PreviousKey.NoiseFrequency1.x) * invTime;
            VelocityKey.NoiseFrequency1.y = (CurrentKey.NoiseFrequency1.y - PreviousKey.NoiseFrequency1.y) * invTime;
            VelocityKey.NoiseFrequency1.z = (CurrentKey.NoiseFrequency1.z - PreviousKey.NoiseFrequency1.z) * invTime;
            VelocityKey.NoiseFrequency1.w = (CurrentKey.NoiseFrequency1.w - PreviousKey.NoiseFrequency1.w) * invTime;

            VelocityKey.NoiseFrequency2.x = (CurrentKey.NoiseFrequency2.x - PreviousKey.NoiseFrequency2.x) * invTime;
            VelocityKey.NoiseFrequency2.y = (CurrentKey.NoiseFrequency2.y - PreviousKey.NoiseFrequency2.y) * invTime;
            VelocityKey.NoiseFrequency2.z = (CurrentKey.NoiseFrequency2.z - PreviousKey.NoiseFrequency2.z) * invTime;
            VelocityKey.NoiseFrequency2.w = (CurrentKey.NoiseFrequency2.w - PreviousKey.NoiseFrequency2.w) * invTime;

            VelocityKey.NoiseAmplitude1.x = (CurrentKey.NoiseAmplitude1.x - PreviousKey.NoiseAmplitude1.x) * invTime;
            VelocityKey.NoiseAmplitude1.y = (CurrentKey.NoiseAmplitude1.y - PreviousKey.NoiseAmplitude1.y) * invTime;
            VelocityKey.NoiseAmplitude1.z = (CurrentKey.NoiseAmplitude1.z - PreviousKey.NoiseAmplitude1.z) * invTime;
            VelocityKey.NoiseAmplitude1.w = (CurrentKey.NoiseAmplitude1.w - PreviousKey.NoiseAmplitude1.w) * invTime;

            VelocityKey.NoiseAmplitude2.x = (CurrentKey.NoiseAmplitude2.x - PreviousKey.NoiseAmplitude2.x) * invTime;
            VelocityKey.NoiseAmplitude2.y = (CurrentKey.NoiseAmplitude2.y - PreviousKey.NoiseAmplitude2.y) * invTime;
            VelocityKey.NoiseAmplitude2.z = (CurrentKey.NoiseAmplitude2.z - PreviousKey.NoiseAmplitude2.z) * invTime;
            VelocityKey.NoiseAmplitude2.w = (CurrentKey.NoiseAmplitude2.w - PreviousKey.NoiseAmplitude2.w) * invTime;
        }
    }
}

void Camera::CommunicateWithJollyRancher(char *cameraname) {
    if (DisableCommunication == 0) {
        char data[96];
        void *addr;
        int protocol;

        addr = &Camera::JollyRancherResponse;
        protocol = DisableCommunication;

        bMemCpy(data, &addr, 4);
        bMemCpy(data + 4, &protocol, 4);

        bMatrix4 scaledmatrix = *reinterpret_cast<const bMatrix4 *>(this);

        bScale(reinterpret_cast<bVector3 *>(&scaledmatrix.v3), reinterpret_cast<const bVector3 *>(&scaledmatrix.v3), 100.0f);

        scaledmatrix.v3.w = 1.0f;

        bMemCpy(data + 8, &scaledmatrix, 0x40);
        bStrCpy(data + 0x48, cameraname);

        bFunkCallASync("JR2Server", 1, data, sizeof(data));
    }
}

float NoiseBase(int x) {
    return 1.0f - (float)((((x << 13) ^ x) * (((x << 13) ^ x) * ((x << 13) ^ x) * 15731 + 789221) + 1376312589 & 0x7FFFFFFF) ^ 0x8000) *
                      (1.0f / 1073741824.0f); // Hugo Elias integer noise hash with modification ( ^ 0x8000 )
}

float NoiseInterpolated(float x) {
    int a;
    float s;
    float t;
    float f;

    a = (int)bFloor(x);
    s = NoiseBase(a);
    t = NoiseBase(a + 1);
    f = (x - (float)a);

    return f * t + (1.0f - (x - f) * s);
}

float Noise(float x) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;

    for (int i = 0; i < 6; ++i) {
        total += NoiseInterpolated(x * frequency) * amplitude;

        frequency *= 2.0f;
        amplitude *= 0.5f;
    }

    return total;
}

unsigned short Camera::FovRelativeAngle(unsigned short a) {
    float f = (float)(bSin(a) * bSin((unsigned short)(CurrentKey.FieldOfView >> 1)) / bSin((unsigned short)(0x2aaa >> 1)));
    return f;
}

void Camera::UpdateAll(float dT) {
    UpdateCameraMovers(dT);
    UpdateCameraShakers(dT);
}

void Camera::ApplyNoise(bMatrix4 *p_matrix, float time, float intensity) {
    int a;

    unsigned short angle;
    float x1;
    float y1;
    float z1;
    float w1;

    float x2;
    float y2;
    float z2;
    float w2;

    long long local_48;

    x1 = Noise(CurrentKey.NoiseFrequency1.x * time);
    y1 = Noise(CurrentKey.NoiseFrequency1.y * time);
    z1 = Noise(CurrentKey.NoiseFrequency1.z * time);
    w1 = Noise(CurrentKey.NoiseFrequency1.w * time);

    x2 = Noise(CurrentKey.NoiseFrequency2.x * time);
    y2 = Noise(CurrentKey.NoiseFrequency2.y * time);
    z2 = Noise(CurrentKey.NoiseFrequency2.z * time);
    w2 = Noise(CurrentKey.NoiseFrequency2.w * time);

    bMatrix4 bmatrix4;

    a = (int)((z1 * CurrentKey.NoiseFrequency1.w + z2 * CurrentKey.NoiseFrequency2.w) * intensity * 65536.0);

    bmatrix4.v3.x = (x1 * CurrentKey.NoiseAmplitude1.x + x2 * CurrentKey.NoiseAmplitude2.x) * intensity;
    bmatrix4.v3.y = (y1 * CurrentKey.NoiseAmplitude1.y + y2 * CurrentKey.NoiseAmplitude2.y) * intensity;

    angle = Camera::FovRelativeAngle((unsigned short)(a / 0x168));
    eRotateX(&bmatrix4, &bmatrix4, angle);

    a = (int)((w1 * CurrentKey.NoiseAmplitude1.w + w2 * CurrentKey.NoiseAmplitude2.w) * intensity * 65536.0);
    angle = Camera::FovRelativeAngle((unsigned short)(a / 0x168));
    eRotateY(&bmatrix4, &bmatrix4, angle);

    eMulMatrix(p_matrix, p_matrix, &bmatrix4);
    return;
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

                // espCentrePlaneView(...);
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
