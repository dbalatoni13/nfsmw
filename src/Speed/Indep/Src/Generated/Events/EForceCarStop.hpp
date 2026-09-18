#ifndef EVENTS_EFORCECARSTOP_H
#define EVENTS_EFORCECARSTOP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x10
class EForceCarStop : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        int fStopInstantly; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EForceCarStop(int pStopInstantly, unsigned int phSimable);

    ~EForceCarStop() override;

    const char *GetEventName() const override;

  private:
    int fStopInstantly; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0x10, size 0x4
};

void EForceCarStop_MakeEvent_Callback(const void *staticData);
int EForceCarStop_MakeEvent_LuaBinding(struct lua_State *L);
void EForceCarStop_ResolveEvent_Callback(void *event, const struct UGroup *group);

#endif
