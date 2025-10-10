#ifndef INTERFACES_SIMABLES_ISPIKEABLE_H
#define INTERFACES_SIMABLES_ISPIKEABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

class ISpikeable : public UTL::COM::IUnknown, public UTL::Collections::Listable<ISpikeable, 10> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISpikeable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ISpikeable() {}

  public:
    virtual eTireDamage GetTireDamage(unsigned int wheelId) const;
    virtual unsigned int GetNumBlowouts() const;
    virtual void Puncture(unsigned int wheelId);
};

#endif
