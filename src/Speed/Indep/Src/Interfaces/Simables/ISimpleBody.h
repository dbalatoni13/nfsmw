#ifndef INTERFACES_SIMABLES_ISIMPLEBODY_H
#define INTERFACES_SIMABLES_ISIMPLEBODY_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

class ISimpleBody : public UTL::COM::IUnknown, public UTL::Collections::Listable<ISimpleBody, 96> {
  public:
    DECL_INTERFACE(ISimpleBody);

    virtual void ModifyFlags(unsigned int uRemove, unsigned int uAdd);
    virtual bool CanCollideWithSRB() const;
    virtual bool CanCollideWithRB() const;
    virtual bool CanHitTrigger() const;
    virtual const SimCollisionMap *GetCollisionMap() const;
    virtual SimCollisionMap *GetCollisionMap();
};

#endif
