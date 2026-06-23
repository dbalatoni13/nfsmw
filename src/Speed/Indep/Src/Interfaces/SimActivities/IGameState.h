#ifndef INTERFACES_SIMACTIVITIES_IGAMESTATE_H
#define INTERFACES_SIMACTIVITIES_IGAMESTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

class IGameState : public UTL::COM::IUnknown, public UTL::Collections::Singleton<IGameState> {
  public:
    DECL_INTERFACE(IGameState);

    virtual bool InGameBreaker() const;
    virtual void RaceReset();
};

#endif
