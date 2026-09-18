#ifndef EVENTS_EWAKEOBJECT_H
#define EVENTS_EWAKEOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EWakeObject : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EWakeObject(unsigned int phModel);

    ~EWakeObject() override;

    const char *GetEventName() const override;

  private:
    unsigned int fhModel; // offset: 0x8, size 0x4
};

void EWakeObject_MakeEvent_Callback(const void *staticData);
int EWakeObject_MakeEvent_LuaBinding(struct lua_State *L);
void EWakeObject_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
