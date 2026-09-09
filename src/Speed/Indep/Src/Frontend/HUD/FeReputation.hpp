#ifndef FRONTEND_HUD_FEREPUTATION_H
#define FRONTEND_HUD_FEREPUTATION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IReputation : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IReputation);

    virtual void SetReputationCareer(int rep) = 0;
    virtual void SetReputationPursuit(int rep) = 0;
};

#endif
