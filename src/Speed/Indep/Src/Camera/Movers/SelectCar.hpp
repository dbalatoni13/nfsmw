#ifndef CAMERA_MOVERS_SELECTCAR_H
#define CAMERA_MOVERS_SELECTCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraMover.hpp"

struct SelectCarCameraData {
    float OrbitVAngle;  // offset 0x0, size 0x4
    float OrbitHAngle;  // offset 0x4, size 0x4
    float Radius;       // offset 0x8, size 0x4
    float RollAngle;    // offset 0xC, size 0x4
    float FOV;          // offset 0x10, size 0x4
    bVector3 LookAt;    // offset 0x14, size 0x10

    SelectCarCameraData() {}
};

// total size: 0x114
class SelectCarCameraMover : public CameraMover {
  public:
    SelectCarCameraMover(int view_id);
    ~SelectCarCameraMover() override;

    void Update(float dT) override;

    static void CreateCameraMatrix(bMatrix4 *camera_matrix, SelectCarCameraData *camera_data);
    void SetVRotateSpeed(float f);
    void SetHRotateSpeed(float f);
    void SetZoomSpeed(float f);
    void SetCurrentOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt);
    void SetDesiredOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt, float animSpeed, float damping, int periods);
    float FindBestAngleGoal(float start, float goal);

    float GetTotalAnimationTime() { return TotalAnimationTime; }
    float GetCurrentAnimationTime() { return CurrentAnimationTime; }
    float GetVAngle() { return CurrentCameraData.OrbitVAngle; }
    float GetHAngle() { return CurrentCameraData.OrbitHAngle; }
    float GetZoom() { return CurrentCameraData.Radius; }

  private:
    int ControlMode;                          // offset 0x80, size 0x4
    int DramaticMode;                         // offset 0x84, size 0x4
    int LookingAtParts;                       // offset 0x88, size 0x4
    SelectCarCameraData CurrentCameraData;    // offset 0x8C, size 0x24
    SelectCarCameraData StartAnimCameraData;  // offset 0xB0, size 0x24
    SelectCarCameraData GoalAnimCameraData;   // offset 0xD4, size 0x24
    float RadiusSpeed;                        // offset 0xF8, size 0x4
    float OrbitVSpeed;                        // offset 0xFC, size 0x4
    float OrbitHSpeed;                        // offset 0x100, size 0x4
    float Damping;                            // offset 0x104, size 0x4
    int Periods;                              // offset 0x108, size 0x4
    float CurrentAnimationTime;               // offset 0x10C, size 0x4
    float TotalAnimationTime;                 // offset 0x110, size 0x4
};

#endif
