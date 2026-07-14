#ifndef ISIMPLEBODY_H
#define ISIMPLEBODY_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

class ISimpleBody : public UTL::COM::IUnknown, public UTL::Collections::Listable<ISimpleBody, 96> {
  public:
    DECL_INTERFACE(ISimpleBody);

    virtual void ModifyFlags(unsigned int uRemove, unsigned int uAdd) = 0;
    virtual bool CanCollideWithSRB() const = 0;
    virtual bool CanCollideWithRB() const = 0;
    virtual bool CanHitTrigger() const = 0;
    virtual const SimCollisionMap *GetCollisionMap() const = 0;
    virtual SimCollisionMap *GetCollisionMap() = 0;
};

#endif
