#ifndef EAXSOUND_EAXAITUNERCAR_H
#define EAXSOUND_EAXAITUNERCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct emEvent;

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

namespace AIDriveBy {
enum eAIDRIVE_BY_TYPE {
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

struct stAIDriveByInfo {
    eAIDRIVE_BY_TYPE eDriveByType;
    EAXCar *pEAXCar;
    float ClosingVelocity;
    bVector3 vLocation;
    unsigned int UniqueID;

    stAIDriveByInfo() {
        vLocation = bVector3(0.0f, 0.0f, 0.0f);
    }
};
} // namespace AIDriveBy

class EAXAITunerCar : public EAXCar {
  public:
    int m_PitchOffset;        // offset 0x114, size 0x4
    bool mPhysicsChangedGear; // offset 0x118, size 0x1

    EAXAITunerCar();
    virtual ~EAXAITunerCar();
    virtual void UpdateParams(float t) override;
    virtual StateInfo *GetStateInfo(void) const override;
    virtual const char *GetStateName(void) const override;
    virtual void UpdateCarPhysics() override;
    virtual int SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) override;

    static StateInfo *GetStaticStateInfo(void) {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(unsigned int allocator);
    static void ProcessEvent(emEvent *event);

    void *operator new(size_t s) {
        return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, 0);
    }

    void *operator new(size_t, void *p) { return p; }

    void UpdatAIDriveBy(float t);

    static StateInfo s_StateInfo;
};

class EAXCopCar : public EAXAITunerCar {
  public:
    virtual void Attach(void *pAttachment) override;
    virtual void UpdateParams(float t) override;
    virtual CSTATE_Base::StateInfo *GetStateInfo(void) const override;
    virtual const char *GetStateName(void) const override;

    static CSTATE_Base::StateInfo *GetStaticStateInfo(void) {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(unsigned int allocator);

    static CSTATE_Base::StateInfo s_StateInfo;
};

class EAXTruck : public EAXAITunerCar {
  public:
    virtual void UpdateParams(float t) override;
    virtual CSTATE_Base::StateInfo *GetStateInfo(void) const override;
    virtual const char *GetStateName(void) const override;

    static CSTATE_Base::StateInfo *GetStaticStateInfo(void) {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(unsigned int allocator);

    static CSTATE_Base::StateInfo s_StateInfo;
};

#endif
