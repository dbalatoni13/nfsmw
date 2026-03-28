#ifndef EAXSOUND_EAXTRAFFICCAR_H
#define EAXSOUND_EAXTRAFFICCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

struct EAXTrafficCar : public CSTATE_Base {
    EAXTrafficCar();
    virtual ~EAXTrafficCar();

    virtual void Attach(void *pAttachment) override;
    virtual bool Detach() override;
    virtual StateInfo *GetStateInfo(void) const override;
    virtual const char *GetStateName(void) const override;

    static StateInfo *GetStaticStateInfo(void) {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(unsigned int allocator);

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

    static StateInfo s_StateInfo;

    bool IsLargeTrafficCar; // offset 0x44
};

#endif
