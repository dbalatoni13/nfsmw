#ifndef INTERFACES_SIMABLES_IRIGIDBODY_H
#define INTERFACES_SIMABLES_IRIGIDBODY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

class IRigidBody : public UTL::COM::IUnknown, public UTL::Collections::Listable<IRigidBody, 160> {
  public:
    IRigidBody(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IRigidBody() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }
};

#endif
