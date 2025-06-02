#pragma once

#include "Speed/Indep/bWare/Inc/bVector.hpp"

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

struct Camera {
  // total size: 0x290
  CameraParams CurrentKey;  // offset 0x0, size 0xD4
  CameraParams PreviousKey; // offset 0xD4, size 0xD4
  CameraParams VelocityKey; // offset 0x1A8, size 0xD4
  bool bClearVelocity;      // offset 0x27C, size 0x1
  float ElapsedTime;        // offset 0x280, size 0x4
  int LastUpdateTime;       // offset 0x284, size 0x4
  int LastDisparateTime;    // offset 0x288, size 0x4
  int RenderDash;           // offset 0x28C, size 0x4

  bMatrix4 *GetCameraMatrix() {}

  int GetRenderDash() {}

  float GetFocalDistance() {}

  float GetDepthOfField() {}

  unsigned short GetFieldOfView() {}

  bMatrix4 *GetWorldToCameraMatrix() {}

  bVector3 *GetPosition() { return &this->CurrentKey.Position; }

  bVector3 *GetDirection() { return &this->CurrentKey.Direction; }

  bVector3 *GetTarget() {}

  unsigned short GetFov() {}

  bVector3 GetPositionSimSpace() {
    // Local variables
    bVector3 vec;
  }

  bVector3 *GetPreviousPosition() {}

  bVector3 *GetPreviousDirection() {}

  bVector3 *GetPreviousTarget() {}

  unsigned short GetPreviousFov() {}

  bVector3 *GetVelocityPosition() {}

  bVector3 *GetVelocityDirection() {}

  bVector3 *GetVelocityTarget() {}

  unsigned short GetVelocityFov() {}

  unsigned int GetLastDisparateTime() {}

  void ClearVelocity() {}

  void SetRenderDash(int r) {}

  void SetTargetDistance(float f) {}

  void SetFocalDistance(float f) {}

  void SetDepthOfField(float f) {}

  void SetFieldOfView(unsigned short fov) {}

  void SetNoiseFrequency1(float x, float y, float z, float w) {}

  void SetNoiseFrequency2(float x, float y, float z, float w) {}

  void SetNoiseAmplitude1(float x, float y, float z, float w) {}

  void SetNoiseAmplitude2(float x, float y, float z, float w) {}

  void SetNoiseFrequency1(bVector4 *p) {}

  void SetNoiseFrequency2(bVector4 *p) {}

  void SetNoiseAmplitude1(bVector4 *p) {}

  void SetNoiseAmplitude2(bVector4 *p) {}

  void SetNoiseFrequency1(float *p) {}

  void SetNoiseFrequency2(float *p) {}

  void SetNoiseAmplitude1(float *p) {}

  void SetNoiseAmplitude2(float *p) {}

  void SetNearZ(float near_z) {}

  void SetFarZ(float far_z) {}

  float GetNearZ() {}

  float GetFarZ() {}

  void SetLetterBox(float LB_h) {}

  float GetLetterBox() {}

  void SetSimTimeMultiplier(float multiplier) {}

  float GetSimTimeMultiplier() {}
};
