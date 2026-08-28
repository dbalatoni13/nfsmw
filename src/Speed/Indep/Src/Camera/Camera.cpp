#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEReplay.hpp"

Camera::Camera() : LastDisparateTime(RealTimeFrames), LastUpdateTime(-0x80000000), ElapsedTime(1.0f), RenderDash(0), bClearVelocity(false) {
    bMatrix4 m;

    m.v0.x = 1.0f;
    m.v1.x = 0.0f;
    m.v2.x = 0.0f;
    m.v3.x = 0.0f;

    m.v0.y = 0.0f;
    m.v1.y = -1.0f;
    m.v2.y = 0.0f;
    m.v3.y = 0.0f;

    m.v0.z = 0.0f;
    m.v1.z = 0.0f;
    m.v2.z = -1.0f;
    m.v3.z = 1200.0f;

    m.v0.w = 0.0f;
    m.v1.w = 0.0f;
    m.v2.w = 100.0f;
    m.v3.w = 1.0f;

    SetFocalDistance(0.0f);
    SetTargetDistance(10.0f);
    SetDepthOfField(0.0f);

    SetNearZ(0.5f);
    SetFarZ(10000.0f);

    SetFieldOfView(0x36FB);
    CurrentKey.LB_height = 0.0f;

    SetSimTimeMultiplier(1.0f);

    SetNoiseFrequency1(&bVector4(1.0f, 1.0f, 1.0f, 1.0f));
    SetNoiseFrequency2(&bVector4(1.0f, 1.0f, 1.0f, 1.0f));

    SetNoiseAmplitude1(&bVector4(0.0f, 0.0f, 0.0f, 0.0f));

    SetNoiseAmplitude2(&bVector4(0.0f, 0.0f, 0.0f, 0.0f));

    SetCameraMatrix(m, 1.0f);
    SetCameraMatrix(m, 1.0f);
}

void Camera::SetCameraMatrix(const bMatrix4 &m, float fTime) {
    static int cameralink;
    if (StopUpdating)
        return;

    bMemCpy(&PreviousKey, &CurrentKey, sizeof(CameraParams));
    ElapsedTime = fTime;

    if (Camera::JollyRancherResponse.UseMatrix != 0 && DisableCommunication == 0) {

        if (cameralink == 0) {
            cameralink = 1;
        }
        bMatrix4 scaledmatrix;

        bMemCpy(reinterpret_cast<bMatrix4 *>(&scaledmatrix), const_cast<const bMatrix4 *>(&Camera::JollyRancherResponse.CamMatrix), sizeof(bMatrix4));
        bScale(reinterpret_cast<bVector3 *>(&scaledmatrix.v3), reinterpret_cast<const bVector3 *>(&scaledmatrix.v3), 0.01f);

        scaledmatrix.v3.w = 1.0f;

        CurrentKey.Matrix = scaledmatrix;

    } else {

        if (cameralink != 0) {
            cameralink = 0;
        }

        CurrentKey.Matrix = m;
    }
    bMatrix4 t;

    eTransposeMatrix(&t, &CurrentKey.Matrix);

    t.v0.w = 0.0f;
    t.v1.w = 0.0f;
    t.v2.w = 0.0f;

    eMulVector(&CurrentKey.Position, &t, reinterpret_cast<bVector3 *>(&CurrentKey.Matrix.v3));

    bNeg(&CurrentKey.Position, &CurrentKey.Position);

    bNormalize(&CurrentKey.Direction, reinterpret_cast<bVector3 *>(&t.v2));
    bScale(&CurrentKey.Target, &CurrentKey.Direction, CurrentKey.TargetDistance);
    CurrentKey.Target += CurrentKey.Position;

    if (bClearVelocity) {
        bClearVelocity = false;
        bMemCpy(&PreviousKey, this, sizeof(PreviousKey));
        this->ElapsedTime = 1.0f;
    }

    if (this->ElapsedTime > 0.0f) {
        float fTimeRecip = 1.0f / this->ElapsedTime;

        VelocityKey.Position = CurrentKey.Position - PreviousKey.Position;
        VelocityKey.Position *= fTimeRecip;

        VelocityKey.Direction = CurrentKey.Direction - PreviousKey.Direction;
        VelocityKey.Direction *= fTimeRecip;

        VelocityKey.Target = CurrentKey.Target - PreviousKey.Target;
        VelocityKey.Target *= fTimeRecip;

        VelocityKey.TargetDistance = (CurrentKey.TargetDistance - PreviousKey.TargetDistance) * fTimeRecip;
        VelocityKey.FocalDistance = (CurrentKey.FocalDistance - PreviousKey.FocalDistance) * fTimeRecip;
        VelocityKey.DepthOfField = (CurrentKey.DepthOfField - PreviousKey.DepthOfField) * fTimeRecip;
        VelocityKey.NearZ = (CurrentKey.NearZ - PreviousKey.NearZ) * fTimeRecip;
        VelocityKey.FarZ = (CurrentKey.FarZ - PreviousKey.FarZ) * fTimeRecip;
        VelocityKey.LB_height = (CurrentKey.LB_height - PreviousKey.LB_height) * fTimeRecip;
        VelocityKey.SimTimeMultiplier = (CurrentKey.SimTimeMultiplier - PreviousKey.SimTimeMultiplier) * fTimeRecip;

        VelocityKey.FieldOfView = CurrentKey.FieldOfView - PreviousKey.FieldOfView;
        VelocityKey.FieldOfView = fTimeRecip * static_cast<float>(VelocityKey.FieldOfView);

        VelocityKey.NoiseFrequency1 =
            static_cast<const CameraParams &>(CurrentKey).NoiseFrequency1 - static_cast<const CameraParams &>(PreviousKey).NoiseFrequency1;
        VelocityKey.NoiseFrequency1 *= fTimeRecip;

        VelocityKey.NoiseFrequency2 =
            static_cast<const CameraParams &>(CurrentKey).NoiseFrequency2 - static_cast<const CameraParams &>(PreviousKey).NoiseFrequency2;
        VelocityKey.NoiseFrequency2 *= fTimeRecip;

        VelocityKey.NoiseAmplitude1 =
            static_cast<const CameraParams &>(CurrentKey).NoiseAmplitude1 - static_cast<const CameraParams &>(PreviousKey).NoiseAmplitude1;
        VelocityKey.NoiseAmplitude1 *= fTimeRecip;

        VelocityKey.NoiseAmplitude2 =
            static_cast<const CameraParams &>(CurrentKey).NoiseAmplitude2 - static_cast<const CameraParams &>(PreviousKey).NoiseAmplitude2;
        VelocityKey.NoiseAmplitude2 *= fTimeRecip;
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
    return 1.0f - (float)((((x << 13) ^ x) * (((x << 13) ^ x) * ((x << 13) ^ x) * 15731 + 789221) + 1376312589 & 0x7FFFFFFF)) *
                      (1.0f / 1073741824.0f); // Hugo Elias integer noise hash
}

float NoiseInterpolated(float x) {
    int a;
    float s;
    float t;
    float f;

    a = bFloor(x);
    s = NoiseBase(a);
    t = NoiseBase(a + 1);
    f = (x - a);

    return f * t + (1.0f - f) * s;
}

float Noise(float x) {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;

    for (int i = 0; i < 6; ++i) {
        total += amplitude * NoiseInterpolated(x * frequency);

        frequency *= 2.0f;
        amplitude *= 0.5f;
    }

    return total;
}

static unsigned short aBaselineFovNoise = 0x2aaa;

unsigned short Camera::FovRelativeAngle(unsigned short a) {
    float f = bSin(a) * bSin(static_cast<unsigned short>(CurrentKey.FieldOfView >> 1)) / bSin(static_cast<unsigned short>(aBaselineFovNoise >> 1));
    return bASin(f);
}

void Camera::ApplyNoise(bMatrix4 *p_matrix, float time, float intensity) {
    bVector4 v(CurrentKey.NoiseFrequency1);
    bScale(&v, &v, time);

    bVector4 v1(Noise(v.x), Noise(v.y), Noise(v.z), Noise(v.w));

    bScale(&v1, &v1, &CurrentKey.NoiseAmplitude1);

    v = CurrentKey.NoiseFrequency2;
    bScale(&v, &v, time);

    bVector4 v2(Noise(v.x), Noise(v.y), Noise(v.z), Noise(v.w));

    bScale(&v2, &v2, &CurrentKey.NoiseAmplitude2);

    bVector4 v_noise = static_cast<const bVector4 &>(v1) + v2; // dwarf require operator+() const
    bScale(&v_noise, &v_noise, intensity);

    bMatrix4 m;
    bIdentity(&m);

    m.v3.x = v_noise.x;
    m.v3.y = v_noise.y;

    eRotateX(&m, &m, Camera::FovRelativeAngle(bDegToAng(v_noise.z)));

    eRotateY(&m, &m, Camera::FovRelativeAngle(bDegToAng(v_noise.w)));

    eMulMatrix(p_matrix, p_matrix, &m);
    return;
}

void Camera::UpdateAll(float dT) {
    UpdateCameraMovers(dT);
    UpdateCameraShakers(dT);
}
