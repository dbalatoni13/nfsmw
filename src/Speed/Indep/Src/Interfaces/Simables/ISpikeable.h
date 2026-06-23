#ifndef ISPIKEABLE
#define ISPIKEABLE

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

class ISpikeable : public UTL::COM::IUnknown, public UTL::Collections::Listable<ISpikeable, 10> {
  public:
    DECL_INTERFACE(ISpikeable);

    virtual eTireDamage GetTireDamage(unsigned int wheelId) const;
    virtual unsigned int GetNumBlowouts() const;
    virtual void Puncture(unsigned int wheelId);
};

#endif
