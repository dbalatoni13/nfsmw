#ifndef ONLINE_BUDDYCORE_HPP
#define ONLINE_BUDDYCORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct BuddyCore {
    static BuddyCore *instance();
    void disconnect();
};

#endif
