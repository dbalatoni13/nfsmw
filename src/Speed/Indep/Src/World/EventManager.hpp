#ifndef WORLD_EVENTMANAGER_H
#define WORLD_EVENTMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Car.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/Src/Misc/VolumeTree.hpp"

enum EVENT_ID {
    TRIGGER_EVENT_CAR_ON_FERN = 65539,
    TRIGGER_EVENT_VIEW_DRIVING_LINE = 65541,
    TRIGGER_EVENT_ACTIVATE_TRAIN = 65542,
    TRIGGER_EVENT_SOUND = 65543,
    TRIGGER_EVENT_GUIDE_ARROW = 65544,
    TRIGGER_EVENT_ACTIVATE_PLANE = 65545,
    TRIGGER_EVENT_INITIATE_PURSUIT = 131072,
    TRIGGER_EVENT_CALL_FOR_BACKUP = 131073,
    TRIGGER_EVENT_CALL_FOR_ROADBLOCK = 131074,
    TRIGGER_EVENT_STRATEGY_INITIATE = 131075,
    TRIGGER_EVENT_COLLISION = 131076,
    TRIGGER_EVENT_ANNOUNCE_ARREST = 131077,
    TRIGGER_EVENT_STRATEGY_OUTCOME = 131078,
    TRIGGER_EVENT_ROADBLOCK_UPDATE = 131079,
    TRIGGER_EVENT_CANCEL_PURSUIT = 131080,
    TRIGGER_EVENT_START_SIREN = 262144,
    TRIGGER_EVENT_STOP_SIREN = 262145,
};

// total size: 0x20
class EventTrigger {
  public:
    unsigned int GetNameHash() {
        return NameHash;
    }

    unsigned int GetEventID() {
        return EventID;
    }

    unsigned int GetParameter() {
        return Parameter;
    }

    float GetRadius() {
        return Radius;
    }

    bVector3 *GetPosition() {
        return reinterpret_cast<bVector3 *>(&PositionX);
    }

    // void SetName(const char *name) {}

    void SetNameHash(unsigned int name_hash) {
        NameHash = name_hash;
    }

    void SetEventID(unsigned int event_id) {
        EventID = event_id;
    }

    void SetParameter(unsigned int parameter) {
        parameter = parameter;
    }

    void SetTrackDirectionMask(int mask) {
        TrackDirectionMask = mask;
    }

    void SetRadius(float radius) {
        Radius = radius;
    }

    // void SetPosition(bVector3 *position) {}

    uint32 NameHash;        // offset 0x0, size 0x4
    uint32 EventID;         // offset 0x4, size 0x4
    uint32 Parameter;       // offset 0x8, size 0x4
    int TrackDirectionMask; // offset 0xC, size 0x4
    float PositionX;        // offset 0x10, size 0x4
    float PositionY;        // offset 0x14, size 0x4
    float PositionZ;        // offset 0x18, size 0x4
    float Radius;           // offset 0x1C, size 0x4
};

// total size: 0x20
class EventTriggerPack : public bTNode<EventTriggerPack> {
  public:
    int32 Version;                   // offset 0x8, size 0x4
    int32 ScenerySectionNumber;      // offset 0xC, size 0x4
    int32 NumEventTriggers;          // offset 0x10, size 0x4
    int32 EndianSwapped;             // offset 0x14, size 0x4
    vAABBTree *EventTree;            // offset 0x18, size 0x4
    EventTrigger *EventTriggerArray; // offset 0x1C, size 0x4
};

extern SlotPool *EventSlotPool;
extern SlotPool *EventHandlerSlotPool;

class emEvent : public bTNode<emEvent> {
  public:
    void *operator new(size_t size) {
        return bOMalloc(EventSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(EventSlotPool, ptr);
    }

    emEvent() {}

    int32 ReferenceCount;        // offset 0x8, size 0x4
    uint32 ID;                   // offset 0xC, size 0x4
    EventTrigger *pEventTrigger; // offset 0x10, size 0x4
    Car *CarPtr;                 // offset 0x14, size 0x4
    int32 Parameter0;            // offset 0x18, size 0x4
    int32 Parameter1;            // offset 0x1C, size 0x4
    int32 Parameter2;            // offset 0x20, size 0x4
};

typedef void (*EVENT_HANDLER_FUNC)(emEvent *);

class emEventHandler : public bTNode<emEventHandler> {
  public:
    void *operator new(size_t size) {
        return bOMalloc(EventHandlerSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(EventHandlerSlotPool, ptr);
    }

    EVENT_HANDLER_FUNC HandlerFunction;
    uint32 StreamMask;
    int32 ReferenceCount;
    float TotalTime;
};

void emEventManagerInit();
void emProcessAllEvents();

#endif
