#ifndef FRONTEND_HUD_FEREPUTATION_H
#define FRONTEND_HUD_FEREPUTATION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IReputation : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IReputation(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IReputation() {}

  public:
    virtual void SetReputationCareer(int rep);
    virtual void SetReputationPursuit(int rep);
};

#endif
