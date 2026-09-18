#ifndef ONLINE_NETWORKMUTEX_HPP
#define ONLINE_NETWORKMUTEX_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class NetworkMutex {
  public:
    NetworkMutex() {}
    static void operator delete(void *mem, size_t size);
    void Create() {}
    void Lock() {}
    void Unlock() {}
    void Destroy() {}
};

#endif
