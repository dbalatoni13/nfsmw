#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/attribuserinclude.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstddef>

Camera::Camera() {

    LastDisparateTime = RealTimeFrames;
    LastUpdateTime = -2147483648;
    RenderDash = 0;
    bClearVelocity = false;
    ElapsedTime = 1.0f;

    bMatrix4 m;

    m.v0 = bVector4(1.0f, 0.0f, 0.0f, 0.0f);
    m.v1 = bVector4(0.0f, -1.0f, 0.0f, 0.0f);
    m.v2 = bVector4(0.0f, 0.0f, -1.0f, 100.0f);
    m.v3 = bVector4(0.0f, 0.0f, 1200.0f, 1.0f);

    CurrentKey.Position = bVector3(0.0f, 0.0f, 0.0f);
    CurrentKey.Direction = bVector3(0.0f, 0.0f, 0.0f);
    CurrentKey.Target = bVector3(0.0f, 0.0f, 0.0f);

    CurrentKey.TargetDistance = 10.0f;
    CurrentKey.FocalDistance = 0.0f;
    CurrentKey.DepthOfField = 0.0f;
    CurrentKey.NearZ = 0.5f;
    CurrentKey.FarZ = 10000.0f;
    CurrentKey.FieldOfView = 0x36FB;
    CurrentKey.LB_height = 0.0f;
    CurrentKey.SimTimeMultiplier = 1.0f;

    CurrentKey.NoiseFrequency1 = bVector4(1.0f, 1.0f, 1.0f, 1.0f);
    CurrentKey.NoiseAmplitude1 = bVector4(0.0f, 0.0f, 0.0f, 0.0f);
    CurrentKey.NoiseFrequency2 = bVector4(1.0f, 1.0f, 1.0f, 1.0f);
    CurrentKey.NoiseAmplitude2 = bVector4(0.0f, 0.0f, 0.0f, 0.0f);

    SetCameraMatrix(m, 1.0f);
    SetCameraMatrix(m, 1.0f);
}

void Camera::SetCameraMatrix(const bMatrix4 &m, float fTime) {
    static int cameralink;
    bMatrix4 t;

    if (!StopUpdating) {
        bMatrix4 scaledmatrix;
        bMemCpy(&PreviousKey, this, 0xd4);
        ElapsedTime = fTime;
        if (Camera::JollyRancherResponse.UseMatrix == 0 || DisableCommunication != 0) {
            if (cameralink != 0) {
                cameralink = 0;
            }
            *reinterpret_cast<bMatrix4 *>(this) = m;

        } else {

            bMemCpy(reinterpret_cast<bMatrix4 *>(&scaledmatrix), const_cast<const bMatrix4 *>(&Camera::JollyRancherResponse.CamMatrix), 0x40);
            bScale(reinterpret_cast<bVector3 *>(&scaledmatrix.v3), reinterpret_cast<const bVector3 *>(&scaledmatrix.v3), 0.01f);

            scaledmatrix.v3.w = 1.0f;

            *reinterpret_cast<bMatrix4 *>(this) = scaledmatrix;

            if (cameralink == 0) {
                cameralink = 1;
            }
        }

        bTransposeMatrix(&t, reinterpret_cast<bMatrix4 *>(this));

        t.v0.w = 0.0f;
        t.v1.w = 0.0f;
        t.v2.w = 0.0f;

        eMulVector(&CurrentKey.Position, &t, reinterpret_cast<bVector3 *>(&CurrentKey.Matrix.v3));

        bNeg(&CurrentKey.Position, &CurrentKey.Position);

        bNormalize(&CurrentKey.Direction, reinterpret_cast<bVector3 *>(&t.v2));

        float targetDist = CurrentKey.TargetDistance;

        CurrentKey.Target = CurrentKey.Direction * targetDist + CurrentKey.Position;

        if (bClearVelocity) {
            bClearVelocity = false;
            bMemCpy(&PreviousKey, this, sizeof(PreviousKey));
            this->ElapsedTime = 1.0f;
        }

        if (this->ElapsedTime > 0.0f) {
            float invTime = 1.0f / this->ElapsedTime;

            VelocityKey.Position = CurrentKey.Position - PreviousKey.Position;
            VelocityKey.Position *= invTime;

            VelocityKey.Direction = CurrentKey.Direction - PreviousKey.Direction;
            VelocityKey.Direction *= invTime;

            VelocityKey.Target = CurrentKey.Target - PreviousKey.Target;
            VelocityKey.Target *= invTime;

            VelocityKey.TargetDistance = (CurrentKey.TargetDistance - PreviousKey.TargetDistance) * invTime;
            VelocityKey.FocalDistance = (CurrentKey.FocalDistance - PreviousKey.FocalDistance) * invTime;
            VelocityKey.DepthOfField = (CurrentKey.DepthOfField - PreviousKey.DepthOfField) * invTime;
            VelocityKey.NearZ = (CurrentKey.NearZ - PreviousKey.NearZ) * invTime;
            VelocityKey.FarZ = (CurrentKey.FarZ - PreviousKey.FarZ) * invTime;
            VelocityKey.LB_height = (CurrentKey.LB_height - PreviousKey.LB_height) * invTime;
            VelocityKey.SimTimeMultiplier = (CurrentKey.SimTimeMultiplier - PreviousKey.SimTimeMultiplier) * invTime;

            VelocityKey.FieldOfView = static_cast<unsigned short>(invTime * static_cast<float>(CurrentKey.FieldOfView - PreviousKey.FieldOfView));

            VelocityKey.NoiseFrequency1 = CurrentKey.NoiseFrequency1 - PreviousKey.NoiseFrequency1;
            VelocityKey.NoiseFrequency1 *= invTime;

            VelocityKey.NoiseFrequency2 = CurrentKey.NoiseFrequency2 - PreviousKey.NoiseFrequency2;
            VelocityKey.NoiseFrequency2 *= invTime;

            VelocityKey.NoiseAmplitude1 = CurrentKey.NoiseAmplitude1 - PreviousKey.NoiseAmplitude1;
            VelocityKey.NoiseAmplitude1 *= invTime;

            VelocityKey.NoiseAmplitude2 = CurrentKey.NoiseAmplitude2 - PreviousKey.NoiseAmplitude2;
            VelocityKey.NoiseAmplitude2 *= invTime;
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
    float f = bSin(a) * bSin(static_cast<unsigned short>(CurrentKey.FieldOfView >> 1)) / bSin(static_cast<unsigned short>(aBaselineFovNoise >> 1));
    return bASin(f);
}

void Camera::ApplyNoise(bMatrix4 *p_matrix, float time, float intensity) {

    bVector4 v(CurrentKey.NoiseFrequency1);
    bVector4 v1;
    bVector4 v2;
    bVector4 v_noise;

    bScale(&v1, &v, time);

    v1.x = Noise(v1.x);
    v1.y = Noise(v1.y);
    v1.z = Noise(v1.z);
    v1.w = Noise(v1.w);

    bScale(&v1, &v1, (bVector4 *)&CurrentKey.NoiseAmplitude1);

    v = CurrentKey.NoiseFrequency2;
    bScale(&v2, &v, time);

    v2.x = Noise(v2.x);
    v2.y = Noise(v2.y);
    v2.z = Noise(v2.z);
    v2.w = Noise(v2.w);

    bScale(&v2, &v2, (bVector4 *)&CurrentKey.NoiseAmplitude2);

    v_noise = v1 + v2;

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
