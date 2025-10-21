#ifndef EAXSOUND_AUDIOMEMBASE_H
#define EAXSOUND_AUDIOMEMBASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"

struct AudioMemBase {
  public:
    virtual ~AudioMemBase() {}

    void operator delete(void *ptr) {
        gAudioMemoryManager.FreeMemory(ptr);
    }
};

#endif
