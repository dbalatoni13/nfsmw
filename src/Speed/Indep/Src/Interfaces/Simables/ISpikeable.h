#ifndef ISPIKEABLE
#define ISPIKEABLE

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

class ISpikeable : public UTL::COM::IUnknown, public UTL::Collections::Listable<ISpikeable, 10> {
  public:
    DECL_INTERFACE(ISpikeable);

    virtual eTireDamage GetTireDamage(unsigned int wheelId) const = 0;
    virtual unsigned int GetNumBlowouts() const = 0;
    virtual void Puncture(unsigned int wheelId) = 0;
};

#endif
