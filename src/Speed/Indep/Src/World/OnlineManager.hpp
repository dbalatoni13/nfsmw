#ifndef WORLD_ONLINEMANAGER_H
#define WORLD_ONLINEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class OnlineManager {
  public:
    bool IsOnlineRace() {
#if !ONLINE_SUPPORT
        return false;
#else

#endif
    }
};

extern OnlineManager TheOnlineManager;

#endif
