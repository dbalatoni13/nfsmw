#ifndef INTERFACES_SIMENTITIES_IPLAYER_H
#define INTERFACES_SIMENTITIES_IPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

// TODO move?
enum ePlayerList {
    PLAYER_MAX = 3,
    PLAYER_REMOTE = 2,
    PLAYER_LOCAL = 1,
    PLAYER_ALL = 0,
};

class IPlayer : public UTL::COM::IUnknown, public UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX> {
  public:
    IPlayer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IPlayer() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }
};

#endif
