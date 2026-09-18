#ifndef EVENTS_ECELLCALL_H
#define EVENTS_ECELLCALL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class ECellCall : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fID; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    ECellCall(int pID);

    ~ECellCall() override;

    const char *GetEventName() const override;

  private:
    int fID; // offset: 0x8, size 0x4
};

void ECellCall_MakeEvent_Callback(const void *staticData);
int ECellCall_MakeEvent_LuaBinding(struct lua_State *L);
void ECellCall_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
