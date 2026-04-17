#ifndef EAXSOUND_AUDIOMEMBASE_H
#define EAXSOUND_AUDIOMEMBASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"

// total size: 0x4
struct AudioMemBase {
    virtual ~AudioMemBase() {}

    static void *operator new(unsigned int size) {
        return ::operator new(size);
    }

    static void *operator new(unsigned int size, void *ptr) {
        return ptr;
    }

    static void *operator new(unsigned int size, const char *debug_name) {
        return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
    }

    static void *operator new(unsigned int size, const char *debug_name, bool bfromtop) {
        return gAudioMemoryManager.AllocateMemory(size, debug_name, bfromtop);
    }

    void FreeMemory(void *mem) {
        gAudioMemoryManager.FreeMemory(mem);
    }

    void operator delete(void *pMem) {
        gAudioMemoryManager.FreeMemory(pMem);
    }
};

#endif
