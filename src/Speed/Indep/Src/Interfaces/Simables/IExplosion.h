#ifndef INTERFACES_SIMABLES_IEXPLOSION_H
#define INTERFACES_SIMABLES_IEXPLOSION_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class IExplosion : public UTL::COM::IUnknown, public UTL::Collections::Listable<IExplosion, 96> {
  public:
    DECL_INTERFACE(IExplosion);

    virtual const UMath::Vector3 &GetOrigin() const;
    virtual float GetExpansionSpeed() const;
    virtual float GetMaximumRadius() const;
    virtual float GetRadius() const;
    // TODO rest, why isn't GetCausality in the dwarf?
};

#endif
