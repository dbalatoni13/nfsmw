#ifndef IENGINE_DAMAGE_H
#define IENGINE_DAMAGE_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class IEngineDamage : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IEngineDamage);

    virtual bool IsBlown() const;
    virtual bool Blow();
    virtual void Sabotage(float time);
    virtual bool IsSabotaged() const;
    virtual void Repair();
};

#endif
