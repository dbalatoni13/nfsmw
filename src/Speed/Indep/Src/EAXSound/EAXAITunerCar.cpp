#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include <new>
#include <types.h>

struct emEvent;
struct bVector3;

enum eSFXMessageType {
    SFX_NONE = 0,
    SFX_CHANGEGEAR = 1,
    SFX_SHIFT_UP = 2,
    SFX_SHIFT_DOWN = 3,
    SFX_NITROUS = 4,
    SFX_BOTTOMOUT = 5,
    SFX_TRUNKBOUNCE = 6,
    SFX_NIS_PATTERN = 7,
    SFX_NIS_REV = 8,
};

void emAddHandler(void (*function)(emEvent *), unsigned int stream_mask);
void emRemoveHandler(void (*function)(emEvent *));
bool g_EAXIsPaused(void);

struct SFXCTL_Physics;

struct EAXCar : public CSTATE_Base {
    // padding from 0x44 to 0x5c
    char _pad_eaxcar0[0x5c - 0x44];
    /* 0x5c */ SFXCTL_Physics *m_pPhysicsCTL;
    // padding from 0x60 to 0xb4
    char _pad_eaxcar1[0xb4 - 0x60];
    /* 0xb4 */ eAemsUpgradeLevel m_TurboUGL;
    // padding from 0xb8 to 0x118
    char _pad_eaxcar2[0x118 - 0xb8];

    virtual void PreLoadAssets() override;
    virtual void Attach(void *pAttachment) override;
    virtual void UpdateParams(float t) override;
    virtual void ProcessUpdate() override;
    virtual bool Detach() override;
    virtual StateInfo *GetStateInfo(void) const override;
    virtual char *GetStateName(void) const override;
    virtual void ProcessSoundSphere(unsigned int, int, bVector3 *, float);
    virtual void UpdateCarPhysics();
    virtual void StartHonkHorn();
    virtual void StopHonkHorn();
    virtual bool IsHonking();
    virtual int GetEngineUpgradeLevel();
    virtual int SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2);
    virtual void UpdatePov();
    virtual int UpdateRotation();

    static StateInfo s_StateInfo;
};

struct EAXAITunerCar : public EAXCar {
    // offset 0x118 from base
    int mPhysicsChangedGear; // offset 0x118, size 0x4

    EAXAITunerCar();
    virtual ~EAXAITunerCar();
    virtual void UpdateParams(float t) override;
    virtual StateInfo *GetStateInfo(void) const override;
    virtual char *GetStateName(void) const override;
    virtual void UpdateCarPhysics() override;
    virtual int SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) override;

    static StateInfo *GetStaticStateInfo(void) {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(unsigned int allocator);
    static void ProcessEvent(emEvent *event);

    void *operator new(size_t s, unsigned int allocator) {
        if (allocator != 0) {
            return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, true);
        } else {
            return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, false);
        }
    }

    void *operator new(size_t s) {
        return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, 0);
    }

    void *operator new(size_t, void *p) { return p; }

    void UpdatAIDriveBy(float t);

    static StateInfo s_StateInfo;
};

CSTATE_Base::StateInfo EAXAITunerCar::s_StateInfo = {
    0x00030000,
    "EAXAITunerCar",
    &EAXCar::s_StateInfo,
    reinterpret_cast< CSTATE_Base *(*)(void) >(&EAXAITunerCar::CreateState),
};

CSTATE_Base::StateInfo *EAXAITunerCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

char *EAXAITunerCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXAITunerCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXAITunerCar), GetStaticStateInfo()->stateName, false)) EAXAITunerCar;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXAITunerCar), GetStaticStateInfo()->stateName, true)) EAXAITunerCar;
    }
}

EAXAITunerCar::EAXAITunerCar() {
    emAddHandler(ProcessEvent, 0x00040000);
    mPhysicsChangedGear = 0;
}

int EAXAITunerCar::SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) {
    if (SFXMessageType == SFX_CHANGEGEAR) {
        mPhysicsChangedGear = 1;
        return 0;
    }
    return EAXCar::SFXMessage(SFXMessageType, param1, param2);
}

void EAXAITunerCar::UpdateCarPhysics() {
    g_EAXIsPaused();
}

void EAXAITunerCar::UpdateParams(float t) {
    if (m_pCar != nullptr) {
        EAXCar::UpdateParams(t);
        mPhysicsChangedGear = 0;
    }
}

void EAXAITunerCar::ProcessEvent(emEvent *event) {
}

struct EAXCopCar : public EAXAITunerCar {
    virtual void Attach(void *pAttachment) override;
    virtual void UpdateParams(float t) override;
    virtual CSTATE_Base::StateInfo *GetStateInfo(void) const override;
    virtual char *GetStateName(void) const override;

    static CSTATE_Base *CreateState(unsigned int allocator);

    static CSTATE_Base::StateInfo s_StateInfo;
};

CSTATE_Base::StateInfo EAXCopCar::s_StateInfo = {
    0x00030000,
    "EAXCopCar",
    &EAXCar::s_StateInfo,
    reinterpret_cast< CSTATE_Base *(*)(void) >(&EAXCopCar::CreateState),
};

CSTATE_Base::StateInfo *EAXCopCar::GetStateInfo(void) const {
    return &s_StateInfo;
}

char *EAXCopCar::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXCopCar::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXCopCar), s_StateInfo.stateName, false)) EAXCopCar;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXCopCar), s_StateInfo.stateName, true)) EAXCopCar;
    }
}

struct EAXTruck : public EAXAITunerCar {
    virtual void UpdateParams(float t) override;
    virtual CSTATE_Base::StateInfo *GetStateInfo(void) const override;
    virtual char *GetStateName(void) const override;

    static CSTATE_Base *CreateState(unsigned int allocator);

    static CSTATE_Base::StateInfo s_StateInfo;
};

CSTATE_Base::StateInfo EAXTruck::s_StateInfo = {
    0x00030000,
    "EAXTruck",
    &EAXCar::s_StateInfo,
    reinterpret_cast< CSTATE_Base *(*)(void) >(&EAXTruck::CreateState),
};

CSTATE_Base::StateInfo *EAXTruck::GetStateInfo(void) const {
    return &s_StateInfo;
}

char *EAXTruck::GetStateName(void) const {
    return s_StateInfo.stateName;
}

CSTATE_Base *EAXTruck::CreateState(unsigned int allocator) {
    if (allocator == 0) {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTruck), s_StateInfo.stateName, false)) EAXTruck;
    } else {
        return new (gAudioMemoryManager.AllocateMemory(
            sizeof(EAXTruck), s_StateInfo.stateName, true)) EAXTruck;
    }
}

void EAXTruck::UpdateParams(float t) {
    UpdatAIDriveBy(t);
    EAXAITunerCar::UpdateParams(t);
}

void EAXCopCar::UpdateParams(float t) {
    UpdatAIDriveBy(t);
    EAXAITunerCar::UpdateParams(t);
}
