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

static unsigned short aBaselineFovNoise;
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
        void *addr = const_cast<JollyRancherResponsePacket *>(const_cast<JollyRancherResponsePacket *>(&JollyRancherResponse));
        int protocol = DisableCommunication;
        bMatrix4 scaledmatrix;

        bMemCpy(&data[0], &addr, 4);
        bMemCpy(&data[4], &protocol, 4);
        PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(this), *reinterpret_cast<Mtx44 *>(&scaledmatrix));
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
        bMatrix4 scaledmatrix;

        if (cameralink == 0) {
            cameralink = 1;
        }

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
    bTransposeMatrix(&t, &CurrentKey.Matrix);
    eMulVector(&CurrentKey.Position, &t, reinterpret_cast<const bVector3 *>(&CurrentKey.Matrix.v3));
    CurrentKey.Position.x = -CurrentKey.Position.x;
    CurrentKey.Position.y = -CurrentKey.Position.y;
    CurrentKey.Position.z = -CurrentKey.Position.z;
    bNormalize(&CurrentKey.Direction, reinterpret_cast<const bVector3 *>(&t.v2));
    CurrentKey.Target.x = CurrentKey.Direction.x * CurrentKey.TargetDistance + CurrentKey.Position.x;
    CurrentKey.Target.y = CurrentKey.Direction.y * CurrentKey.TargetDistance + CurrentKey.Position.y;
    CurrentKey.Target.z = CurrentKey.Direction.z * CurrentKey.TargetDistance + CurrentKey.Position.z;

    if (bClearVelocity) {
        bClearVelocity = false;
        bMemCpy(&PreviousKey, &CurrentKey, sizeof(CameraParams));
        ElapsedTime = 1.0f;
    }

    if (ElapsedTime > 0.0f) {
        float scale = 1.0f / ElapsedTime;

        VelocityKey.Position.x = (CurrentKey.Position.x - PreviousKey.Position.x) * scale;
        VelocityKey.Position.y = (CurrentKey.Position.y - PreviousKey.Position.y) * scale;
        VelocityKey.Position.z = (CurrentKey.Position.z - PreviousKey.Position.z) * scale;

        VelocityKey.Direction.x = (CurrentKey.Direction.x - PreviousKey.Direction.x) * scale;
        VelocityKey.Direction.y = (CurrentKey.Direction.y - PreviousKey.Direction.y) * scale;
        VelocityKey.Direction.z = (CurrentKey.Direction.z - PreviousKey.Direction.z) * scale;

        VelocityKey.Target.x = (CurrentKey.Target.x - PreviousKey.Target.x) * scale;
        VelocityKey.Target.y = (CurrentKey.Target.y - PreviousKey.Target.y) * scale;
        VelocityKey.Target.z = (CurrentKey.Target.z - PreviousKey.Target.z) * scale;

        VelocityKey.FieldOfView =
            static_cast<unsigned short>(static_cast<int>(scale * static_cast<float>(static_cast<unsigned short>(CurrentKey.FieldOfView - PreviousKey.FieldOfView))));

        VelocityKey.TargetDistance = (CurrentKey.TargetDistance - PreviousKey.TargetDistance) * scale;
        VelocityKey.FocalDistance = (CurrentKey.FocalDistance - PreviousKey.FocalDistance) * scale;
        VelocityKey.DepthOfField = (CurrentKey.DepthOfField - PreviousKey.DepthOfField) * scale;
        VelocityKey.NearZ = (CurrentKey.NearZ - PreviousKey.NearZ) * scale;
        VelocityKey.FarZ = (CurrentKey.FarZ - PreviousKey.FarZ) * scale;
        VelocityKey.LB_height = (CurrentKey.LB_height - PreviousKey.LB_height) * scale;
        VelocityKey.SimTimeMultiplier = (CurrentKey.SimTimeMultiplier - PreviousKey.SimTimeMultiplier) * scale;

        VelocityKey.NoiseFrequency1.x = (CurrentKey.NoiseFrequency1.x - PreviousKey.NoiseFrequency1.x) * scale;
        VelocityKey.NoiseFrequency1.y = (CurrentKey.NoiseFrequency1.y - PreviousKey.NoiseFrequency1.y) * scale;
        VelocityKey.NoiseFrequency1.z = (CurrentKey.NoiseFrequency1.z - PreviousKey.NoiseFrequency1.z) * scale;
        VelocityKey.NoiseFrequency1.w = (CurrentKey.NoiseFrequency1.w - PreviousKey.NoiseFrequency1.w) * scale;

        VelocityKey.NoiseAmplitude1.x = (CurrentKey.NoiseAmplitude1.x - PreviousKey.NoiseAmplitude1.x) * scale;
        VelocityKey.NoiseAmplitude1.y = (CurrentKey.NoiseAmplitude1.y - PreviousKey.NoiseAmplitude1.y) * scale;
        VelocityKey.NoiseAmplitude1.z = (CurrentKey.NoiseAmplitude1.z - PreviousKey.NoiseAmplitude1.z) * scale;
        VelocityKey.NoiseAmplitude1.w = (CurrentKey.NoiseAmplitude1.w - PreviousKey.NoiseAmplitude1.w) * scale;

        VelocityKey.NoiseFrequency2.x = (CurrentKey.NoiseFrequency2.x - PreviousKey.NoiseFrequency2.x) * scale;
        VelocityKey.NoiseFrequency2.y = (CurrentKey.NoiseFrequency2.y - PreviousKey.NoiseFrequency2.y) * scale;
        VelocityKey.NoiseFrequency2.z = (CurrentKey.NoiseFrequency2.z - PreviousKey.NoiseFrequency2.z) * scale;
        VelocityKey.NoiseFrequency2.w = (CurrentKey.NoiseFrequency2.w - PreviousKey.NoiseFrequency2.w) * scale;

        VelocityKey.NoiseAmplitude2.x = (CurrentKey.NoiseAmplitude2.x - PreviousKey.NoiseAmplitude2.x) * scale;
        VelocityKey.NoiseAmplitude2.y = (CurrentKey.NoiseAmplitude2.y - PreviousKey.NoiseAmplitude2.y) * scale;
        VelocityKey.NoiseAmplitude2.z = (CurrentKey.NoiseAmplitude2.z - PreviousKey.NoiseAmplitude2.z) * scale;
        VelocityKey.NoiseAmplitude2.w = (CurrentKey.NoiseAmplitude2.w - PreviousKey.NoiseAmplitude2.w) * scale;
    }
}

void Camera::ApplyNoise(bMatrix4 *p_matrix, float time, float intensity) {
    bVector4 v(CurrentKey.NoiseFrequency1);
    bScale(&v, &v, time);
    bVector4 v1(Noise(v.x), Noise(v.y), Noise(v.z), Noise(v.w));
    bScale(&v1, &v1, &CurrentKey.NoiseAmplitude1);

    bVector4 v2(CurrentKey.NoiseFrequency2);
    bScale(&v2, &v2, time);
    bVector4 v_noise(Noise(v2.x), Noise(v2.y), Noise(v2.z), Noise(v2.w));
    bScale(&v_noise, &v_noise, &CurrentKey.NoiseAmplitude2);

    v_noise = v1 + v_noise;
    bScale(&v_noise, &v_noise, intensity);

    bMatrix4 m;
    bIdentity(&m);
    eRotateX(&m, &m, FovRelativeAngle(bDegToAng(v_noise.z)));
    eRotateY(&m, &m, FovRelativeAngle(bDegToAng(v_noise.w)));
    eMulMatrix(p_matrix, p_matrix, &m);
}
