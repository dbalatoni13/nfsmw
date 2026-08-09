#ifndef HELICOPTERSTATE_HPP
#define HELICOPTERSTATE_HPP

#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

// total size: 0x64
// Decl: 29
class CSTATE_Helicopter : public CSTATE_Base {
  public:
    DECLARE_STATETYPE();

    CSTATE_Helicopter();
    ~CSTATE_Helicopter() override;

    // Overrides: CSTATE_Base
    void UpdateParams(float t) override;
    void Attach(void *pAttachment) override;
    bool Detach() override;

    EAX_HeliState *m_pHeliState; // offset 0x44, size 0x4, Decl: 20
};

#endif
