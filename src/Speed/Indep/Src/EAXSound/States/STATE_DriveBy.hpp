#ifndef EAXSOUND_STATES_STATE_DRIVEBY_H
#define EAXSOUND_STATES_STATE_DRIVEBY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct EAXCar;

enum eDRIVE_BY_TYPE {
    DRIVE_BY_UNKNOWN = 0,
    DRIVE_BY_TREE = 1,
    DRIVE_BY_LAMPPOST = 2,
    DRIVE_BY_SMOKABLE = 3,
    DRIVE_BY_TUNNEL_IN = 4,
    DRIVE_BY_TUNNEL_OUT = 5,
    DRIVE_BY_OVERPASS_IN = 6,
    DRIVE_BY_OVERPASS_OUT = 7,
    DRIVE_BY_AI_CAR = 8,
    DRIVE_BY_TRAFFIC = 9,
    DRIVE_BY_BRIDGE = 10,
    DRIVE_BY_PRE_COL = 11,
    DRIVE_BY_CAMERA_BY = 12,
    MAX_DRIVE_BY_TYPES = 13,
};

struct stDriveByInfo {
    eDRIVE_BY_TYPE eDriveByType; // offset 0x0, size 0x4
    EAXCar *pEAXCar;             // offset 0x4, size 0x4
    float ClosingVelocity;       // offset 0x8, size 0x4
    bVector3 vLocation;          // offset 0xC, size 0x10
    unsigned int UniqueID;       // offset 0x1C, size 0x4

    stDriveByInfo() {
        vLocation = bVector3(0.0f, 0.0f, 0.0f);
    }

    stDriveByInfo &operator=(const stDriveByInfo &from) {
        eDriveByType = from.eDriveByType;
        pEAXCar = from.pEAXCar;
        ClosingVelocity = from.ClosingVelocity;
        vLocation = from.vLocation;
        UniqueID = from.UniqueID;
        return *this;
    }
};

struct CSTATE_DriveBy : public CSTATE_Base {
  protected:
    static StateInfo s_StateInfo;

  public:
    // total size: 0x64
    stDriveByInfo m_DriveByInfo; // offset 0x44, size 0x20

    static StateInfo *GetStaticStateInfo() {
        return &s_StateInfo;
    }

    CSTATE_DriveBy();
    ~CSTATE_DriveBy() override;

    StateInfo *GetStateInfo() const override;
    const char *GetStateName() const override;

    static CSTATE_Base *CreateState(unsigned int allocator);

    void Attach(void *pAttachment) override;
    bool Detach() override;
};

#endif
