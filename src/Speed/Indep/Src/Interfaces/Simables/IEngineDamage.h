#ifndef INTERFACES_SIMABLES_IENGINEDAMAGE_H
#define INTERFACES_SIMABLES_IENGINEDAMAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class IEngineDamage : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IEngineDamage(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IEngineDamage() {}

  public:
    virtual bool IsBlown();
    virtual bool Blow();
    virtual void Sabotage(float time);
    virtual bool IsSabotaged();
    virtual void Repair();
};

#endif
