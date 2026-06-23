#ifndef INTTERFACES_SIMACTIVITIES_ITRAFFIC_MGR_H
#define INTTERFACES_SIMACTIVITIES_ITRAFFIC_MGR_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

// total size: 0xC
class ITrafficMgr : public UTL::COM::IUnknown, public UTL::Collections::Singleton<ITrafficMgr> {
  public:
    DECL_INTERFACE(ITrafficMgr);

    virtual void FlushAllTraffic(bool release);
};

#endif
