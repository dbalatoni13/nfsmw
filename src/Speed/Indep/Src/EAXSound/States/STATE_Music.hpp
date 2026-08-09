#ifndef STATE_MUSIC_HPP
#define STATE_MUSIC_HPP

#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

// total size: 0x64
// Decl: 29
class CSTATE_Music : public CSTATE_Base {
  public:
    DECLARE_STATETYPE();

    CSTATE_Music();
    ~CSTATE_Music() override;
};

#endif
