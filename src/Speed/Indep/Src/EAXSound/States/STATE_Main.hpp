#ifndef STATE_MAIN_HPP
#define STATE_MAIN_HPP

#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

// total size: 0x48
// Decl: 11
class CSTATE_Main : public CSTATE_Base {
  public:
    DECLARE_STATETYPE();

    CSTATE_Main();
    ~CSTATE_Main() override;
};

#endif
