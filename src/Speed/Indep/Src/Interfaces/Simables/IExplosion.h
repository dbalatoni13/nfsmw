#ifndef IEXPLOSION_H
#define IEXPLOSION_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

class IExplosion : public UTL::COM::IUnknown, public UTL::Collections::Listable<IExplosion, 96> {
  public:
    DECL_INTERFACE(IExplosion);

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
