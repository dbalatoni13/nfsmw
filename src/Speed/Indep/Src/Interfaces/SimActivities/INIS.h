#ifndef INTERFACES_SIMACTIVITIES_INIS_H
#define INTERFACES_SIMACTIVITIES_INIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

class INIS : public UTL::COM::IUnknown, public UTL::Collections::Singleton<INIS> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    INIS(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~INIS() {}
};

#endif
