#ifndef EAXSOUND_STATES_MANAGERS_STATEMGR_ENVIRO_H
#define EAXSOUND_STATES_MANAGERS_STATEMGR_ENVIRO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_WorldObject.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

struct CSTATE_WorldObject : public CSTATE_Base {
    static StateInfo *GetStaticStateInfo() {
        return &s_StateInfo;
    }

    static StateInfo s_StateInfo;
    StateInfo *GetStateInfo() const override;
    const char *GetStateName() const override;
    static CSTATE_Base *CreateState(unsigned int allocator);
    void *operator new(size_t, void *p) { return p; }
    CSTATE_WorldObject();
    ~CSTATE_WorldObject() override;
    void Attach(void *pAttachment) override;
    bool Detach() override;
    ISndAttachable *mSndAttachment; // offset 0x44, size 0x4
};

// total size: 0x24
struct CSTATEMGR_Enviro : public CSTATEMGR_Base {
    UTL::Std::list<WorldObject *, _type_list> m_WorldObjects; // offset 0x1C, size 0x8

    CSTATEMGR_Enviro();
    ~CSTATEMGR_Enviro() override;
    void EnterWorld(eSndGameMode esgm) override;
    void UpdateParams(float t) override;
    void AddWorldObject(float x, float y, float z, WORLDOBJECT_TYPES type);
    void RegisterWorldObjects();
};

#endif
