#ifndef EAXSOUND_STATES_STATE_COLLISION_H
#define EAXSOUND_STATES_STATE_COLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

namespace Sound {
class CollisionEvent;
}

struct CSTATE_Collision : public CSTATE_Base {
    static StateInfo s_StateInfo;
    static StateInfo *GetStaticStateInfo() { return &s_StateInfo; }

    StateInfo *GetStateInfo() const override;
    const char *GetStateName() const override;
    static CSTATE_Base *CreateState(unsigned int allocator);

    CSTATE_Collision();
    ~CSTATE_Collision() override;

    void Attach(void *pAttachment) override;
    bool Detach() override;

    Sound::CollisionEvent *m_pCollisionEvent; // offset 0x44, size 0x4
};

#endif
