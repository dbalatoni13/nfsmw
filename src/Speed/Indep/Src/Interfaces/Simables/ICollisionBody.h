#ifndef INTERFACES_SIMABLES_ICOLLISIONBODY_H
#define INTERFACES_SIMABLES_ICOLLISIONBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

class ICollisionBody : public UTL::COM::IUnknown, public UTL::Collections::Listable<ICollisionBody, 160> {
  public:
    ICollisionBody(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~ICollisionBody() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  public:
    // TODO
};

#endif
