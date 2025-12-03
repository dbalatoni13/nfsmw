#ifndef INTERFACES_SIMABLES_ISIMPLEBODY_H
#define INTERFACES_SIMABLES_ISIMPLEBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

class ISimpleBody : public UTL::COM::IUnknown, public UTL::Collections::Listable<ISimpleBody, 96> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISimpleBody(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ISimpleBody() {}

  public:
    virtual void ModifyFlags(unsigned int uRemove, unsigned int uAdd);
    virtual bool CanCollideWithSRB() const;
    virtual bool CanCollideWithRB() const;
    virtual bool CanHitTrigger() const;
    virtual const SimCollisionMap *GetCollisionMap() const;
    virtual SimCollisionMap *GetCollisionMap();
};

#endif
