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

    virtual void HitGround() = 0;
    virtual void HitWorld() = 0;
    virtual void HitObject() = 0;
    virtual void ScrapeObject() = 0;
    virtual void ScrapeGround() = 0;
    virtual void ScrapeWorld() = 0;
    virtual void Purge() = 0;
};

#endif
