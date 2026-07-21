#ifndef _SPEECH_MANAGER_H_
#define _SPEECH_MANAGER_H_

#include "SpeechModule.hpp"

namespace Speech {

class Manager {
  public:
    static Module *GetSpeechModule(int nindex);
    static void ClearPlayback();

  private:
};

}; // namespace Speech

// Decl: 225
#define SCHEDULE_SPEECH(_EVENT_, _DATA_, _CALLER_)                                                                                                   \
    Speech::Manager::ScheduleSpeech<Csis::_EVENT_##Struct>(_DATA_, Csis::_EVENT_##Id, Csis::g##_EVENT_##Handle, _CALLER_)

#endif
