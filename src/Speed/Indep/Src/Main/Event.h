#ifndef MAIN_EVENT_H
#define MAIN_EVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>
#include <types.h>

extern char *gCreationPoint;
extern char *gDeletionPoint;

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

#endif
