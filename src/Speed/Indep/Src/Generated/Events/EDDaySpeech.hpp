#ifndef EVENTS_EDDAYSPEECH_H
#define EVENTS_EDDAYSPEECH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x8
class EDDaySpeech : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EDDaySpeech();

    ~EDDaySpeech() override;

    const char *GetEventName() const override;
};

void EDDaySpeech_MakeEvent_Callback(const void *staticData);
int EDDaySpeech_MakeEvent_LuaBinding(struct lua_State *L);
void EDDaySpeech_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
