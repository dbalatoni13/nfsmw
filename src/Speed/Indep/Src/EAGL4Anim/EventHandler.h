#ifndef EAGL4ANIM_EVENTHANDLER_H
#define EAGL4ANIM_EVENTHANDLER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Event.h"

namespace EAGL4Anim {

class EventHandler {
  public:
    EventHandler() {}

    void SetSuccessor(EventHandler *h) {
        mpSuccessor = h;
    }

    EventHandler *GetSuccessor() {
        return mpSuccessor;
    }

    virtual void HandleEvent(float time, const Event &event, void *extraData) {}

  private:
    EventHandler *mpSuccessor; // offset 0x0, size 0x4
};

}; // namespace EAGL4Anim

#endif
