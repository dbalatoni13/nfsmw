#include "Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void UpdateCameraMovers(float dT);
void UpdateCameraShakers(float dT);

extern int DisableCommunication;
extern int JR2ServerExists;

int Camera::StopUpdating;
volatile JollyRancherResponsePacket Camera::JollyRancherResponse;

static const unsigned short aBaselineFovNoise = 0x2AAA;
static int cameralink;

Camera::Camera() {
    bMatrix4 m;

    m.v0 = bVector4(1.0f, 0.0f, 0.0f, 0.0f);
    m.v1 = bVector4(0.0f, -1.0f, 0.0f, 0.0f);
    m.v2 = bVector4(0.0f, 0.0f, -1.0f, 100.0f);
    m.v3 = bVector4(0.0f, 0.0f, 1200.0f, 1.0f);

    LastUpdateTime = 0x80000000;
    LastDisparateTime = RealTimeFrames;
    RenderDash = 0;
    CurrentKey.TargetDistance = 10.0f;
    CurrentKey.NearZ = 0.5f;
    bClearVelocity = false;
    CurrentKey.FocalDistance = 0.0f;
    CurrentKey.DepthOfField = 0.0f;
    ElapsedTime = 1.0f;
    CurrentKey.FarZ = 10000.0f;
    CurrentKey.FieldOfView = 0x36FB;
    CurrentKey.NoiseAmplitude2.w = 0.0f;
    CurrentKey.LB_height = 0.0f;
    CurrentKey.NoiseAmplitude1.x = 0.0f;
    CurrentKey.NoiseAmplitude1.y = 0.0f;
    CurrentKey.NoiseAmplitude1.z = 0.0f;
    CurrentKey.NoiseAmplitude1.w = 0.0f;
    CurrentKey.SimTimeMultiplier = 1.0f;
    CurrentKey.NoiseFrequency1.x = 1.0f;
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
    SetCameraMatrix(m, 1.0f);
    SetCameraMatrix(m, 1.0f);
}

void Camera::UpdateAll(float dT) {
    UpdateCameraMovers(dT);
    UpdateCameraShakers(dT);
}

float NoiseBase(int x) {
    int n = x << 13 ^ x;
    return 1.0f - static_cast<float>((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) * 0.0000000009313226f;
}

float NoiseInterpolated(float x) {
    int a = static_cast<int>(bFloor(x));
    float t = NoiseBase(a);
    float s = NoiseBase(a + 1);
    float f = x - static_cast<float>(a);

    return f * s + (1.0f - f) * t;
}

float Noise(float x) {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;

    for (int i = 0; i <= 5; i++) {
        total += amplitude * NoiseInterpolated(x * frequency);
        frequency += frequency;
        amplitude *= 0.5f;
    }

    return total;
}

unsigned short Camera::FovRelativeAngle(unsigned short angle) {
    float sine = bSin(angle);
    float fov_sine = bSin(static_cast<unsigned short>(CurrentKey.FieldOfView >> 1));
    float baseline_sine = bSin(static_cast<unsigned short>(aBaselineFovNoise >> 1));

    return bASin((sine * fov_sine) / baseline_sine);
}

void Camera::CommunicateWithJollyRancher(char *cameraname) {
    if (DisableCommunication == 0) {
        char data[96];
        void *addr = const_cast<JollyRancherResponsePacket *>(&JollyRancherResponse);
        int protocol = DisableCommunication;

        bMemCpy(&data[0], &addr, 4);
        bMemCpy(&data[4], &protocol, 4);
        bMatrix4 scaledmatrix(*reinterpret_cast<const bMatrix4 *>(this));
        scaledmatrix.v3.x *= 100.0f;
        scaledmatrix.v3.y *= 100.0f;
        scaledmatrix.v3.z *= 100.0f;
        scaledmatrix.v3.w = 1.0f;
        bMemCpy(&data[8], &scaledmatrix, 0x40);
        bStrCpy(&data[72], cameraname);
        bFunkCallASync("JR2Server", 1, data, 0x60);
    }
}

void Camera::SetCameraMatrix(const bMatrix4 &m, float fTime) {
    if (StopUpdating != 0) {
        return;
    }

    bMemCpy(&PreviousKey, &CurrentKey, sizeof(CameraParams));
    ElapsedTime = fTime;

    if (JollyRancherResponse.UseMatrix != 0 && DisableCommunication == 0) {
        if (cameralink == 0) {
            cameralink = 1;
        }

        bMatrix4 scaledmatrix;
        bMemCpy(&scaledmatrix, const_cast<const bMatrix4 *>(&JollyRancherResponse.CamMatrix), sizeof(bMatrix4));
        scaledmatrix.v3.x *= 0.01f;
        scaledmatrix.v3.y *= 0.01f;
        scaledmatrix.v3.z *= 0.01f;
        scaledmatrix.v3.w = 1.0f;
        PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&scaledmatrix), *reinterpret_cast<Mtx44 *>(&CurrentKey.Matrix));
    } else {
        if (cameralink != 0) {
            cameralink = 0;
        }

        PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&m), *reinterpret_cast<Mtx44 *>(&CurrentKey.Matrix));
    }

    bMatrix4 t;
    eTransposeMatrix(&t, &CurrentKey.Matrix);
    eMulVector(&CurrentKey.Position, &t, reinterpret_cast<const bVector3 *>(&CurrentKey.Matrix.v3));
    CurrentKey.Position.x = -CurrentKey.Position.x;
    CurrentKey.Position.y = -CurrentKey.Position.y;
    CurrentKey.Position.z = -CurrentKey.Position.z;
    bNormalize(&CurrentKey.Direction, reinterpret_cast<const bVector3 *>(&t.v2));
    bScale(&CurrentKey.Target, &CurrentKey.Direction, CurrentKey.TargetDistance);
    CurrentKey.Target += CurrentKey.Position;

    if (bClearVelocity) {
        bClearVelocity = false;
        bMemCpy(&PreviousKey, &CurrentKey, sizeof(CameraParams));
        ElapsedTime = 1.0f;
    }

    if (ElapsedTime > 0.0f) {
        float fTimeRecip = 1.0f / ElapsedTime;

        VelocityKey.Position = CurrentKey.Position - PreviousKey.Position;
        VelocityKey.Position *= fTimeRecip;

        VelocityKey.Direction = CurrentKey.Direction - PreviousKey.Direction;
        VelocityKey.Direction *= fTimeRecip;

        VelocityKey.Target = CurrentKey.Target - PreviousKey.Target;
        VelocityKey.Target *= fTimeRecip;

        VelocityKey.FieldOfView = CurrentKey.FieldOfView - PreviousKey.FieldOfView;
        VelocityKey.FieldOfView = static_cast<unsigned short>(static_cast<int>(fTimeRecip * static_cast<float>(VelocityKey.FieldOfView)));

        VelocityKey.TargetDistance = (CurrentKey.TargetDistance - PreviousKey.TargetDistance) * fTimeRecip;
        VelocityKey.FocalDistance = (CurrentKey.FocalDistance - PreviousKey.FocalDistance) * fTimeRecip;
        VelocityKey.DepthOfField = (CurrentKey.DepthOfField - PreviousKey.DepthOfField) * fTimeRecip;
        VelocityKey.NearZ = (CurrentKey.NearZ - PreviousKey.NearZ) * fTimeRecip;
        VelocityKey.FarZ = (CurrentKey.FarZ - PreviousKey.FarZ) * fTimeRecip;
        VelocityKey.LB_height = (CurrentKey.LB_height - PreviousKey.LB_height) * fTimeRecip;
        VelocityKey.SimTimeMultiplier = (CurrentKey.SimTimeMultiplier - PreviousKey.SimTimeMultiplier) * fTimeRecip;

        VelocityKey.NoiseFrequency1 = CurrentKey.NoiseFrequency1 - PreviousKey.NoiseFrequency1;
        VelocityKey.NoiseFrequency1 *= fTimeRecip;

        VelocityKey.NoiseFrequency2 = CurrentKey.NoiseFrequency2 - PreviousKey.NoiseFrequency2;
        VelocityKey.NoiseFrequency2 *= fTimeRecip;

        VelocityKey.NoiseAmplitude1 = CurrentKey.NoiseAmplitude1 - PreviousKey.NoiseAmplitude1;
        VelocityKey.NoiseAmplitude1 *= fTimeRecip;

        VelocityKey.NoiseAmplitude2 = CurrentKey.NoiseAmplitude2 - PreviousKey.NoiseAmplitude2;
        VelocityKey.NoiseAmplitude2 *= fTimeRecip;
    }
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

    bVector4 v_noise;
    bAdd(&v_noise, &v1, &v2);
    bScale(&v_noise, &v_noise, intensity);

    bMatrix4 m;
    bIdentity(&m);
    m.v3.x = v_noise.x;
    m.v3.y = v_noise.y;
    eRotateX(&m, &m, FovRelativeAngle(bDegToAng(v_noise.z)));
    eRotateY(&m, &m, FovRelativeAngle(bDegToAng(v_noise.w)));
    eMulMatrix(p_matrix, p_matrix, &m);
}
