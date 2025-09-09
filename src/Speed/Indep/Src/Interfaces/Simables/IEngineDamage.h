#ifndef INTERFACES_SIMABLES_IENGINEDAMAGE_H
#define INTERFACES_SIMABLES_IENGINEDAMAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// Credits: Brawltendo
class IEngineDamage : public UTL::COM::IUnknown {
  public:
    IEngineDamage(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IEngineDamage() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    virtual bool IsBlown();
    virtual void Blow();
    virtual void Sabotage(float time);
    virtual bool IsSabotaged();
    virtual void Repair();
};

#endif
