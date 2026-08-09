#ifndef EAXTRAFFICCAR_HPP
#define EAXTRAFFICCAR_HPP

#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

// total size: 0x48
// Decl: 9
class EAXTrafficCar : public CSTATE_Base {
  public:
    DECLARE_STATETYPE();
    EAXTrafficCar();
    ~EAXTrafficCar() override;

    // Overrides: CSTATE_Base
    void Attach(void *pAttachment) override;

    // Overrides: CSTATE_Base
    bool Detach() override;

    bool IsLargeTrafficCar; // offset 0x44, size 0x1, Decl: 30
};

#endif
