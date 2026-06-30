#ifndef MAIN_EVENT_H
#define MAIN_EVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

struct RegisterEvent {
    typedef void (* MakeEventCallback)(const void *);

    typedef void (* ResolveEventCallback)(void *, const struct UGroup *);

    typedef int (* LuaBinding)(struct lua_State *);

    static void (* LookupEvent(unsigned int tag))(const void *);

    static void (* ResolveEvent(unsigned int tag))(void *, const struct UGroup *);

    static int (* GetLuaBinding(unsigned int tag))(struct lua_State *);
};

class Event {
  public:
    struct StaticData {
        unsigned int fEventID;
    };

    void *operator new(size_t size);
    void operator delete(void *ptr, std::size_t size);

    virtual ~Event() {}

    virtual const char *GetEventName();

    Event(std::size_t size) : fEventSize(size) {}

    std::size_t fEventSize;
};

// total size: 0x1
class EventManager {
  public:
    static void BindMessagesToLua(struct HandlerVector &handlers);

    static void Init();

    static void Shutdown();

    static void Reset();

    static void RunEvents();

    static void FireEventList(const CARP::EventList *eventList, bool verbose);

    static void FireOneEvent(const CARP::EventList *eventList, unsigned int eventToFire, bool verbose);

    static bool ListHasEvent(const CARP::EventList *eventList, unsigned int eventID, const CARP::EventStaticData **foundEvent);

    static void AbortCurrentEventList();

    static bool EventsQueued();

    static const char *EmbedField(Event *event, const char *ptr);

    static Hermes::Message *EmbedField(Event *event, Hermes::Message *ptr);

    // static Event *GetCurrentEvent() {}

  private:
    static Event *fgCurrentEvent;  // size: 0x4, address: 0x8041E458
    static bool fgHaltCurrentList; // size: 0x1, address: 0x8041E45C
};

template<typename T>
unsigned int GetEmbeddedObjectSize(T *ptr); // ???

#endif
