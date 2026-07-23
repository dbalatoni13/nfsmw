//
//
//
//
//
//
//
//
//
//
//
#ifndef AUDIOMEMBASE_HPP
#define AUDIOMEMBASE_HPP

#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"

struct AudioMemBase {
  public:
    virtual ~AudioMemBase() {}

    void *operator new(size_t size, const char *debug_name) {
        return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
    }

    void *operator new(size_t size, const char *debug_name, bool bfromtop) {
        return gAudioMemoryManager.AllocateMemory(size, debug_name, bfromtop);
    }

    void operator delete(void *pMem) {
        gAudioMemoryManager.FreeMemory(pMem);
    }
};

#endif
