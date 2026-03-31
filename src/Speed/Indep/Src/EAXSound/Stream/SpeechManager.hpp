#ifndef EAXSOUND_STREAM_SPEECHMANAGER_H
#define EAXSOUND_STREAM_SPEECHMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "SpeechModule.hpp"

namespace Speech {

class Manager {
  public:
    static Module *GetSpeechModule(int nindex);
    static void ClearPlayback();
    static short m_frameindex;

  private:
};

}; // namespace Speech

#endif
