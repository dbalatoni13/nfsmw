#ifndef CAMERA_CAMERA_H
#define CAMERA_CAMERA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Src/bFunkPlat.cpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

// TODO GET RID OF THESE (from GameFlow.cpp)
#include "Speed/Indep/Src/Misc/GameFlow.cpp"


// TODO GET RID OF THESE
extern int32 RealTime; 
extern int32 LastUpdateTimeJR2;
extern int WeHaveCheckedIfJR2ServerExists; 
extern int bStreamingPositionFromICE;
extern int JR2ServerExists;

void UpdateCameraMovers(float dT);

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

struct JollyRancherResponsePacket {
    int UseMatrix;          // offset 0x0
    int Pad1;               // offset 0x4
    int Pad2;               // offset 0x8
    int Pad3;               // offset 0xC
    bMatrix4 CamMatrix;     // offset 0x10
};


extern int DisableCommunication;

struct CameraLink {
    int field_3469;  
};
extern CameraLink cameralink;

struct JR2Request {
        JollyRancherResponsePacket *response;
        int disableComm;
        bMatrix4 scaledMatrix;
        char cameraName[24];
    } request;

// total size: 0x290
class Camera {
    friend class CameraMover;
    friend void UpdateCameraMovers(float dT);

  public:
    static bool StopUpdating;
    static JollyRancherResponsePacket JollyRancherResponse;
    static int JR2ServerExists;

    bMatrix4 *GetCameraMatrix() {
        return &this->CurrentKey.Matrix;
    }

    int GetRenderDash() {
        return this->RenderDash;
    }

    Camera();
    void SetCameraMatrix(bMatrix4 *m,float fTime);

    void CommunicateWithJollyRancher(char *cameraname);

    unsigned short FovRelativeAngle(unsigned short a);

    void UpdateAll(float dT);



    // void UpdateCameraMovers(float dT); // not a member of Camera, but a global function

    // void UpdateCameraShakers(float dT); // not a member of Camera, but a global function

    // float GetFocalDistance() {}

    // float GetDepthOfField() {}

    // unsigned short GetFieldOfView() {}

    // bMatrix4 *GetWorldToCameraMatrix() {}

    bVector3 *GetPosition() {
        return &this->CurrentKey.Position;
    }


    bVector3 *GetDirection() {
        return &this->CurrentKey.Direction;
    }

    // bVector3 *GetTarget() {}

    // unsigned short GetFov() {}

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
        return &this->VelocityKey.Position;
    }

    // bVector3 *GetVelocityDirection() {}

    // bVector3 *GetVelocityTarget() {}

    // unsigned short GetVelocityFov() {}

    // unsigned int GetLastDisparateTime() {}

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

    // float GetNearZ() {}

    // float GetFarZ() {}

    void SetLetterBox(float LB_h) {}

    // float GetLetterBox() {}

    void SetSimTimeMultiplier(float multiplier) {}

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
extern TrackStreamer TheTrackStreamer;

#endif
