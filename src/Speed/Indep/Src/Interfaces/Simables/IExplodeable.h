#ifndef INTERFACES_SIMABLES_IEXPLODEABLE_H
#define INTERFACES_SIMABLES_IEXPLODEABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class IExplosion;

class IExplodeable : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IExplodeable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IExplodeable() {}

    virtual bool OnExplosion(const UMath::Vector3 &normal, const UMath::Vector3 &position, float dT, IExplosion *explosion) = 0;
};

#endif
