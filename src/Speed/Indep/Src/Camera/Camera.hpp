#ifndef CAMERA_CAMERA_H
#define CAMERA_CAMERA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern int RealTimeFrames;

struct JollyRancherResponsePacket {
    volatile int UseMatrix;   // offset 0x0, size 0x4
    volatile int Pad1;        // offset 0x4, size 0x4
    volatile int Pad2;        // offset 0x8, size 0x4
    volatile int Pad3;        // offset 0xC, size 0x4
    volatile bMatrix4 CamMatrix; // offset 0x10, size 0x40
};

struct CameraParams {
    // total size: 0xD4
    bMatrix4 Matrix;            // offset 0x0, size 0x40
    bVector3 Position;          // offset 0x40, size 0x10
    bVector3 Direction;         // offset 0x50, size 0x10
    bVector3 Target;            // offset 0x60, size 0x10
    bVector4 NoiseFrequency1;   // offset 0x70, size 0x10
    bVector4 NoiseAmplitude1;   // offset 0x80, size 0x10
    bVector4 NoiseFrequency2;   // offset 0x90, size 0x10
    bVector4 NoiseAmplitude2;   // offset 0xA0, size 0x10
    float TargetDistance;       // offset 0xB0, size 0x4
    float FocalDistance;        // offset 0xB4, size 0x4
    float DepthOfField;         // offset 0xB8, size 0x4
    float NearZ;                // offset 0xBC, size 0x4
    float FarZ;                 // offset 0xC0, size 0x4
    unsigned short FieldOfView; // offset 0xC4, size 0x2
    float LB_height;            // offset 0xC8, size 0x4
    float SimTimeMultiplier;    // offset 0xCC, size 0x4
    unsigned short DummyAngle;  // offset 0xD0, size 0x2
};

// total size: 0x290
class Camera {
  public:
    static int StopUpdating;
    static volatile JollyRancherResponsePacket JollyRancherResponse;

    Camera();

    static void UpdateAll(float dT);

    void CommunicateWithJollyRancher(char *cameraname);
    void SetCameraMatrix(const bMatrix4 &m, float fTime);
    void ApplyNoise(bMatrix4 *p_matrix, float time, float intensity);

    bMatrix4 *GetCameraMatrix() {
        return &this->CurrentKey.Matrix;
    }

    int GetRenderDash() {
        return this->RenderDash;
    }

    // float GetFocalDistance() {}

    // float GetDepthOfField() {}

    // unsigned short GetFieldOfView() {}
    unsigned short GetFieldOfView() {
        return CurrentKey.FieldOfView;
    }

    // bMatrix4 *GetWorldToCameraMatrix() {}

    bVector3 *GetPosition() {
        return &this->CurrentKey.Position;
    }

    bVector3 *GetDirection() {
        return &this->CurrentKey.Direction;
    }

    bVector3 *GetTarget() {
        return &CurrentKey.Target;
    }

    unsigned short GetFov() {
        return CurrentKey.FieldOfView;
    }

    unsigned short FovRelativeAngle(unsigned short angle);

    bVector3 GetPositionSimSpace() {
        bVector3 vec(CurrentKey.Position);
        eUnSwizzleWorldVector(vec, vec);

        return vec;
    }

    // bVector3 *GetPreviousPosition() {}

    // bVector3 *GetPreviousDirection() {}

    // bVector3 *GetPreviousTarget() {}

    // unsigned short GetPreviousFov() {}

    bVector3 *GetVelocityPosition() {
        return &VelocityKey.Position;
    }

    bVector3 *GetVelocityDirection() {
        return &VelocityKey.Direction;
    }

    bVector3 *GetVelocityTarget() {
        return &VelocityKey.Target;
    }

    // unsigned short GetVelocityFov() {}
    unsigned short GetVelocityFov() {
        return VelocityKey.FieldOfView;
    }

    // unsigned int GetLastDisparateTime() {}

    void ClearVelocity() {
        bClearVelocity = true;
        LastDisparateTime = RealTimeFrames;
    }

    void SetRenderDash(int r) {
        if (!StopUpdating) {
            RenderDash = r;
        }
    }

    void SetTargetDistance(float f) {
        if (!StopUpdating) {
            CurrentKey.TargetDistance = f;
        }
    }

    void SetFocalDistance(float f) {
        if (!StopUpdating) {
            CurrentKey.FocalDistance = f;
        }
    }

    void SetDepthOfField(float f) {
        if (!StopUpdating) {
            CurrentKey.DepthOfField = f;
        }
    }

    void SetFieldOfView(unsigned short fov) {
        if (!StopUpdating) {
            CurrentKey.FieldOfView = fov;
        }
    }

    void SetNoiseFrequency1(float x, float y, float z, float w) {
        CurrentKey.NoiseFrequency1.x = x;
        CurrentKey.NoiseFrequency1.y = y;
        CurrentKey.NoiseFrequency1.z = z;
        CurrentKey.NoiseFrequency1.w = w;
    }

    void SetNoiseFrequency2(float x, float y, float z, float w) {
        CurrentKey.NoiseFrequency2.x = x;
        CurrentKey.NoiseFrequency2.y = y;
        CurrentKey.NoiseFrequency2.z = z;
        CurrentKey.NoiseFrequency2.w = w;
    }

    void SetNoiseAmplitude1(float x, float y, float z, float w) {
        CurrentKey.NoiseAmplitude1.x = x;
        CurrentKey.NoiseAmplitude1.y = y;
        CurrentKey.NoiseAmplitude1.z = z;
        CurrentKey.NoiseAmplitude1.w = w;
    }

    void SetNoiseAmplitude2(float x, float y, float z, float w) {
        CurrentKey.NoiseAmplitude2.x = x;
        CurrentKey.NoiseAmplitude2.y = y;
        CurrentKey.NoiseAmplitude2.z = z;
        CurrentKey.NoiseAmplitude2.w = w;
    }

    void SetNoiseFrequency1(bVector4 *p) {
        CurrentKey.NoiseFrequency1 = *p;
    }

    void SetNoiseFrequency2(bVector4 *p) {
        CurrentKey.NoiseFrequency2 = *p;
    }

    void SetNoiseAmplitude1(bVector4 *p) {
        CurrentKey.NoiseAmplitude1 = *p;
    }

    void SetNoiseAmplitude2(bVector4 *p) {
        CurrentKey.NoiseAmplitude2 = *p;
    }

    void SetNoiseFrequency1(float *p) {
        SetNoiseFrequency1(p[0], p[1], p[2], p[3]);
    }

    void SetNoiseFrequency2(float *p) {
        SetNoiseFrequency2(p[0], p[1], p[2], p[3]);
    }

    void SetNoiseAmplitude1(float *p) {
        SetNoiseAmplitude1(p[0], p[1], p[2], p[3]);
    }

    void SetNoiseAmplitude2(float *p) {
        SetNoiseAmplitude2(p[0], p[1], p[2], p[3]);
    }

    void SetNearZ(float near_z) {
        CurrentKey.NearZ = near_z;
    }

    void SetFarZ(float far_z) {
        CurrentKey.FarZ = far_z;
    }

    float GetNearZ() {
        return CurrentKey.NearZ;
    }

    // float GetFarZ() {}

    void SetLetterBox(float LB_h) {
        CurrentKey.LB_height = LB_h;
    }

    // float GetLetterBox() {}

    void SetSimTimeMultiplier(float multiplier) {
        CurrentKey.SimTimeMultiplier = multiplier;
    }

    // float GetSimTimeMultiplier() {}

  private:
    CameraParams CurrentKey;  // offset 0x0, size 0xD4
    CameraParams PreviousKey; // offset 0xD4, size 0xD4
    CameraParams VelocityKey; // offset 0x1A8, size 0xD4
    bool bClearVelocity;      // offset 0x27C, size 0x1
    float ElapsedTime;        // offset 0x280, size 0x4
    int LastUpdateTime;       // offset 0x284, size 0x4
    int LastDisparateTime;    // offset 0x288, size 0x4
    int RenderDash;           // offset 0x28C, size 0x4
};

// TODO move?
extern bool gCinematicMomementCamera;

float NoiseBase(int x);
float NoiseInterpolated(float x);
float Noise(float x);

#endif
