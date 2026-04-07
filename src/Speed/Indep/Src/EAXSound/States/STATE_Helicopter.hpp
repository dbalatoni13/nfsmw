#ifndef EAXSOUND_STATES_STATE_HELICOPTER_H
#define EAXSOUND_STATES_STATE_HELICOPTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

 #include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"
 #include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

// total size: 0x48
struct CSTATE_Helicopter : public CSTATE_Base {
  protected:
    static StateInfo s_StateInfo;

  public:
    static StateInfo *GetStaticStateInfo() {
        return &s_StateInfo;
    }

    CSTATE_Helicopter();
    ~CSTATE_Helicopter() override;

    StateInfo *GetStateInfo() const override;
    const char *GetStateName() const override;

    static CSTATE_Base *CreateState(unsigned int allocator);

    void UpdateParams(float t) override;
    void Attach(void *pAttachment) override;
    bool Detach() override;

    EAX_HeliState *m_pHeliState; // offset 0x44, size 0x4
};

#endif
