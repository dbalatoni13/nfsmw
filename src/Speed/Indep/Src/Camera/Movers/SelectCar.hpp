#ifndef CAMERA_MOVERS_SELECT_CAR_H
#define CAMERA_MOVERS_SELECT_CAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraMover.hpp"

class SelectCarCameraData {
  public:
    float OrbitVAngle;
    float OrbitHAngle;
    float Radius;
    float RollAngle;
    float FOV;
    bVector3 LookAt;
};

class SelectCarCameraMover : public CameraMover {
  public:
    SelectCarCameraMover(int view_id);
    ~SelectCarCameraMover() override;
    void Update(float dT) override;
    void SetVRotateSpeed(float f);
    void SetHRotateSpeed(float f);
    void SetZoomSpeed(float f);
    void SetCurrentOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt);
    void SetDesiredOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt, float animSpeed, float damping, int periods);
    float GetTotalAnimationTime() {};
    float GetCurrentAnimationTime() {};
    float GetVAngle() {};
    float GetHAngle() {}
    float GetZoom() {};

  private:
    static void CreateCameraMatrix(bMatrix4 *camera_matrix, SelectCarCameraData *camera_data);
    float FindBestAngleGoal(float start, float goal);

    int ControlMode;                         // offset 0x80, size 0x4
    int DramaticMode;                        // offset 0x84, size 0x4
    int LookingAtParts;                      // offset 0x88, size 0x4
    SelectCarCameraData CurrentCameraData;   // offset 0x8C, size 0x24
    SelectCarCameraData StartAnimCameraData; // offset 0xB0, size 0x24
    SelectCarCameraData GoalAnimCameraData;  // offset 0xD4, size 0x24
    float RadiusSpeed;                       // offset 0xF8, size 0x4
    float OrbitVSpeed;                       // offset 0xFC, size 0x4
    float OrbitHSpeed;                       // offset 0x100, size 0x4
    float Damping;                           // offset 0x104, size 0x4
    int Periods;                             // offset 0x108, size 0x4
    float CurrentAnimationTime;              // offset 0x10C, size 0x4
    float TotalAnimationTime;                // offset 0x110, size 0x4
};

#endif
