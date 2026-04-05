#ifndef GENERATED_MESSAGES_MNOTIFYSPEECHSTATUS_H
#define GENERATED_MESSAGES_MNOTIFYSPEECHSTATUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

namespace Speech {
struct ScheduledSpeechEvent;
}

// total size: 0x14
class MNotifySpeechStatus : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifySpeechStatus);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifySpeechStatus");

        return k;
    }

    MNotifySpeechStatus(Speech::ScheduledSpeechEvent *_Event) : Hermes::Message(_GetKind(), _GetSize(), 0), fEvent(_Event) {}

    ~MNotifySpeechStatus() {}

    Speech::ScheduledSpeechEvent *GetEvent() const {
        return fEvent;
    }

    void SetEvent(Speech::ScheduledSpeechEvent *_Event) {
        fEvent = _Event;
    }

  private:
    Speech::ScheduledSpeechEvent *fEvent; // offset 0x10, size 0x4
};

#endif
