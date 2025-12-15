#ifndef WORLD_ONLINEMANAGER_H
#define WORLD_ONLINEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#if defined(EA_PLATFORM_XENON) || defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WINDOWS)
#define ONLINE_SUPPORT (1)
#else
#define ONLINE_SUPPORT (0)
#endif

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
