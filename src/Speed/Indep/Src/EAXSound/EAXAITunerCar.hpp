#ifndef EAXSOUND_EAXAITUNERCAR_H
#define EAXSOUND_EAXAITUNERCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct emEvent;

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

struct EAXAITunerCar : public EAXCar {
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

struct EAXCopCar : public EAXAITunerCar {
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

struct EAXTruck : public EAXAITunerCar {
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
