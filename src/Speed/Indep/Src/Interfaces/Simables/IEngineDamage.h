#ifndef IENGINE_DAMAGE_H
#define IENGINE_DAMAGE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class IEngineDamage : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IEngineDamage);

    virtual bool IsBlown() const = 0;
    virtual bool Blow() = 0;
    virtual void Sabotage(float time) = 0;
    virtual bool IsSabotaged() const = 0;
    virtual void Repair() = 0;
};

#endif
