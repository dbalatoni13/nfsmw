#ifndef INTTERFACES_SIMACTIVITIES_ITRAFFIC_MGR_H
#define INTTERFACES_SIMACTIVITIES_ITRAFFIC_MGR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

class ITrafficMgr : public UTL::COM::IUnknown, public UTL::Collections::Singleton<ITrafficMgr> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITrafficMgr(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ITrafficMgr() {}
};

#endif
