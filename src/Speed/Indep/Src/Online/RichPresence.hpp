#ifndef ONLINE_RICHPRESENCE_HPP
#define ONLINE_RICHPRESENCE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

enum HLBStatE {
    HLB_STAT_UNDEFINED = -1,
    HLB_STAT_DISC = 0,
    HLB_STAT_CHAT = 1,
    HLB_STAT_AWAY = 2,
    HLB_STAT_XA = 3,
    HLB_STAT_DND = 4,
    HLB_STAT_PASSIVE = 5
};

struct RichPresenceMsg {
    char IPaddress[32];
    uint32 eventHash;
    uint32 isRacing;
    char session[32];
    uint32 isSessionPassworded;
    char password[32];
    uint32 isSessionPrivate;

    RichPresenceMsg();
    void Default();
};

struct RichPresence {
  private:
    RichPresenceMsg mCurrentRichPresence;
    HLBStatE mBuddyState;

  public:
    RichPresence();
    static RichPresence *Instance();
    void Init();
    void SetEventHash(uint32 eventHash);
    void SetSession(const char *session);
    void SetBuddyState(HLBStatE buddyState);
    void SetIsRacing(bool isRacing);
    void SetSessionPasswordProtected(bool isPasswordProtected);
    void SetPassword(const char *password);
    void SetSessionPrivate(bool isPrivate);
    void UpdatePresence();
};

#endif
