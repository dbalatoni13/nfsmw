#ifndef EAXSOUND_STATES_STATE_MUSIC_H
#define EAXSOUND_STATES_STATE_MUSIC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

 #include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

// total size: 0x44
struct CSTATE_Music : public CSTATE_Base {
  protected:
    static StateInfo s_StateInfo;

  public:
    static StateInfo *GetStaticStateInfo() {
        return &s_StateInfo;
    }

    CSTATE_Music();
    ~CSTATE_Music() override;

    StateInfo *GetStateInfo() const override;
    const char *GetStateName() const override;

    static CSTATE_Base *CreateState(unsigned int allocator);
};

#endif
