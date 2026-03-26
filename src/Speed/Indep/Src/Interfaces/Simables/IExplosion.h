#ifndef INTERFACES_SIMABLES_IEXPLOSION_H
#define INTERFACES_SIMABLES_IEXPLOSION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

struct HCAUSE__;
typedef HCAUSE__ *HCAUSE;

struct HMODEL__;
typedef HMODEL__ *HMODEL;

class IExplosion : public UTL::COM::IUnknown, public UTL::Collections::Listable<IExplosion, 96> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IExplosion(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IExplosion() {}

    virtual const UMath::Vector3 &GetOrigin() const = 0;
    virtual float GetExpansionSpeed() const = 0;
    virtual float GetMaximumRadius() const = 0;
    virtual float GetRadius() const = 0;
    virtual HCAUSE GetCausality() const = 0;
    virtual float GetCausalityTime() const = 0;
    virtual bool HasDamage() const = 0;
    virtual unsigned int GetTargets() const = 0;
    virtual HMODEL GetSource() const = 0;
};

#endif
