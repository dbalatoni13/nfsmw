#ifndef WORLD_CAR_H
#define WORLD_CAR_H

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

enum SignalLightStateEnum {
    SIGNAL_LIGHT_ALL_OFF = 0,
    SIGNAL_LIGHT_LEFT_ON = 1,
    SIGNAL_LIGHT_RIGHT_ON = 2,
    SIGNAL_LIGHT_ALL_ON = 3,
};

enum CopLightState {
    COP_LIGHTS_OFF = 0,
    COP_LIGHTS_SLOW = 1,
    COP_LIGHTS_FAST = 2,
};

enum SirenMode {
    SIREN_MODE_NONE = 0,
    SIREN_MODE_01 = 1,
    SIREN_MODE_02 = 2,
    SIREN_MODE_03 = 3,
    SIREN_MODE_BROKEN = 4,
};

enum ControlMode {
    NO_CONTROL = 0,
    REAL_CONTROL = 1,
    DRAG_CONTROL = 2,
    AI_CONTROL = 3,
    TRAFFIC_CONTROL = 4,
    COP_CONTROL = 5,
};

enum DampeningMode {
    NORMAL_DAMPENING = 0,
    WILDCOLLISION_DAMPENING = 1,
    COLLISION_DAMPENING = 2,
};

enum CarEffect {
    CARFX_NONE = 0,
    CARFX_DRIVE_OVER = 1,
    CARFX_DRIVE_OVER2 = 2,
    CARFX_SKID_SMOKE = 3,
    CARFX_TIRE_SPEW = 4,
    CARFX_BOTTOM_OUT = 5,
    CARFX_DAM_RADIATOR = 6,
    CARFX_DAM_ENGINE = 7,
    CARFX_BLOWN_TIRE = 8,
    CARFX_DRIVE_ON_FLAT_TIRE = 9,
    CARFX_NITRO = 10,
    CARFX_EXHAUST_SMOKE = 11,
    CARFX_EXHAUST_BLOWOFF = 12,
    CARFX_NOS_BLOWOFF = 13,
    CARFX_BREAK_SIDE_MIRROR_LEFT = 14,
    CARFX_BREAK_SIDE_MIRROR_RIGHT = 15,
    CARFX_BREAK_LICENSE_PLATE_FRONT = 16,
    CARFX_BREAK_LICENSE_PLATE_RIGHT = 17,
    CARFX_BREAK_HEADLIGHT_LEFT = 18,
    CARFX_BREAK_HEADLIGHT_RIGHT = 19,
    CARFX_BREAK_BRAKELIGHT_LEFT = 20,
    CARFX_BREAK_BRAKELIGHT_RIGHT = 21,
    CARFX_BREAK_BRAKELIGHT_CENTRE = 22,
    CARFX_BREAK_WINDSHIELD = 23,
    CARFX_BREAK_WINDOW_REAR = 24,
    CARFX_BREAK_WINDOW_LEFT_FRONT = 25,
    CARFX_BREAK_WINDOW_LEFT_REAR = 26,
    CARFX_BREAK_WINDOW_RIGHT_FRONT = 27,
    CARFX_BREAK_WINDOW_RIGHT_REAR = 28,
    NUM_CARFX = 29,
};

enum RPMInterval {
    IDLE_INTERVAL = 0,
    RPM2_INTERVAL = 1,
    RPM3_INTERVAL = 2,
    RPM4_INTERVAL = 3,
    RPM5_INTERVAL = 4,
    RPM6_INTERVAL = 5,
};

enum HydraulicState {
    HYDRAULICS_OFF = 0,
    HYDRAULICS_EXTENDED = 1,
    HYDRAULICS_EXTENDING = 2,
    HYDRAULICS_RELAXING = 3,
};

// total size: 0x8
class Car : public bTNode<Car> {
  public:
    bVector3 *GetGeometryPosition() {
        return nullptr; // TODO
    }
};

class CarRenderInfo;

class FrontEndRenderingCar : public bTNode<FrontEndRenderingCar> {
  public:
    void SetPosition(bVector3 *position) {
        Position = *position;
    }

    void SetBodyMatrix(bMatrix4 *body_matrix) {
        BodyMatrix = *body_matrix;
    }

    void SetTireMatrices(bMatrix4 *tire_matrices) {
        for (int n = 0; n < 4; n++) {
            TireMatrices[n] = tire_matrices[n];
        }
    }

    void SetBrakeMatrices(bMatrix4 *brake_matrices) {
        for (int n = 0; n < 4; n++) {
            BrakeMatrices[n] = brake_matrices[n];
        }
    }

    void SetTireMatrix(int n, bMatrix4 *m) {
        TireMatrices[n] = *m;
    }

    void SetBrakeMatrix(int n, bMatrix4 *m) {
        BrakeMatrices[n] = *m;
    }

    void SetOverrideModel(eModel *override_model) {}

    RideInfo *GetRideInfo() {
        return &mRideInfo;
    }

    CarRenderInfo *GetRenderInfo() {
        return this->RenderInfo;
    }

    CarType GetCarType() {
        return mRideInfo.Type;
    }

    FrontEndRenderingCar(RideInfo *ride_info, int view_id);

    bool LookupWheelPosition(unsigned int index, bVector4 *position);

    bool LookupWheelRadius(unsigned int index, float &radius);

    void ReInit(RideInfo *ride_info);

    ~FrontEndRenderingCar();

    void OverRideAlpha(uint8 nAlphaBits, bool bSemiTrans, bool bWriteZ);

    void RestoreAlpha();

  private:
    RideInfo mRideInfo; // offset 0x8, size 0x310

  public:
    CarRenderInfo *RenderInfo; // offset 0x318, size 0x4
    int ViewID;                // offset 0x31C, size 0x4
    bVector3 Position;         // offset 0x320, size 0x10
    bMatrix4 BodyMatrix;       // offset 0x330, size 0x40
    bMatrix4 TireMatrices[4];  // offset 0x370, size 0x100
    bMatrix4 BrakeMatrices[4]; // offset 0x470, size 0x100
    eModel *OverrideModel;     // offset 0x570, size 0x4
    int Visible;               // offset 0x574, size 0x4
    int nPasses;               // offset 0x578, size 0x4
    int Reflection;            // offset 0x57C, size 0x4
    int LightsOn;              // offset 0x580, size 0x4
    int CopLightsOn;           // offset 0x584, size 0x4
};

#endif
