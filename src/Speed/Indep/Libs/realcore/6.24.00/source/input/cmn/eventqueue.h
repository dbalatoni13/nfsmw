#ifndef REALCORE_SOURCE_INPUT_CMN_EVENTQUEUE_H
#define REALCORE_SOURCE_INPUT_CMN_EVENTQUEUE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../../../include/common/realcore/input.h"

namespace RealInput {

struct EventContainer {
    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    static inline void *operator new[](unsigned int size) {
        return AllocateMemSize(nullptr, static_cast<int>(size), 0, 4, 0);
    }

    static inline void operator delete[](void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    EventContainer() {}
    ~EventContainer() {}

    Event mEvent;
    EventContainer *mpNext;
};

struct EventQueue {
    static inline void *operator new(unsigned int size) {
        return AllocateMemSize(nullptr, static_cast<int>(size), 0, 0, 0);
    }

    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, static_cast<int>(size));
    }

    EventQueue(unsigned int queueSize);
    virtual ~EventQueue();

    void Clear();
    RiResult AddEvent(Event *pEvent);
    Event *GetEvent();

    unsigned int mQueueSize;
    EventContainer *mpQueue;
    EventContainer *mpHead;
    EventContainer *mpTail;
};

} // namespace RealInput

#endif
