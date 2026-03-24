#ifndef EAXSOUND_AUDIOMEMBASE_H
#define EAXSOUND_AUDIOMEMBASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"

struct AudioMemBase {
  public:
    virtual ~AudioMemBase() {}

    static void *operator new(unsigned int size) {
        return ::operator new(size);
    }

    static void *operator new(unsigned int size, void *ptr) {
        return ptr;
    }

    static void *operator new(unsigned int size, const char *debug_name, bool bfromtop) {
        return gAudioMemoryManager.AllocateMemory(size, debug_name, bfromtop);
    }

    void operator delete(void *ptr) {
        gAudioMemoryManager.FreeMemory(ptr);
    }
};

#endif
