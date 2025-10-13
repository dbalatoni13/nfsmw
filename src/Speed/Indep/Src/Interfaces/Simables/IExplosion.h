#ifndef INTERFACES_SIMABLES_IEXPLOSION_H
#define INTERFACES_SIMABLES_IEXPLOSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class IExplosion : public UTL::COM::IUnknown, public UTL::Collections::Listable<IExplosion, 96> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IExplosion(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IExplosion() {}

    virtual const UMath::Vector3 &GetOrigin() const;
    virtual float GetExpansionSpeed() const;
    virtual float GetMaximumRadius() const;
    virtual float GetRadius() const;
    // TODO rest, why isn't GetCausality in the dwarf?
};

#endif
