#ifndef INTERFACES_SIMENTITIES_IONLINEPLAYER_H
#define INTERFACES_SIMENTITIES_IONLINEPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// total size: 0x8
struct IOnlinePlayer : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IOnlinePlayer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IOnlinePlayer() {}

    virtual void SetOnlineRacer(class OnlineRacer *racer) = 0;
    virtual class OnlineRacer *GetOnlineRacer() = 0;
    virtual void Reposition() = 0;
};

#endif
