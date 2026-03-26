#ifndef INTERFACES_SIMABLES_IEFFECTS_H
#define INTERFACES_SIMABLES_IEFFECTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IEffects : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IEffects(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IEffects() {}

    virtual void HitGround();
    virtual void HitWorld();
    virtual void HitObject();
    virtual void ScrapeObject();
    virtual void ScrapeGround();
    virtual void ScrapeWorld();
    virtual void Purge();
};

#endif
