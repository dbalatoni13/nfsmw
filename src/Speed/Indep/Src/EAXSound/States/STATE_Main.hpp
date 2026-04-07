#ifndef EAXSOUND_STATES_STATE_MAIN_H
#define EAXSOUND_STATES_STATE_MAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

 #include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

// total size: 0x44
struct CSTATE_Main : public CSTATE_Base {
  protected:
    static StateInfo s_StateInfo;

  public:
    static StateInfo *GetStaticStateInfo() {
        return &s_StateInfo;
    }

    CSTATE_Main();
    ~CSTATE_Main() override;

    StateInfo *GetStateInfo() const override;
    const char *GetStateName() const override;

    static CSTATE_Base *CreateState(unsigned int allocator);
};

#endif
