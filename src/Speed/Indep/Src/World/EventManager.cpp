#include "Speed/Indep/Src/World/EventManager.hpp"

#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

struct vAABB {
    float PositionX;
    float PositionY;
    float PositionZ;
    short ParentIndex;
    short NumChildren;
    float ExtentX;
    float ExtentY;
    float ExtentZ;
    short ChildrenIndicies[10];
};

struct vAABBTree {
    vAABB *NodeArray;
    short NumLeafNodes;
    short NumParentNodes;
    short TotalNodes;
    short Depth;
    int pad1;

    vAABB *QueryLeaf(float x, float y, float z);
};

struct EventTrigger {
    unsigned int NameHash;
    unsigned int EventID;
    unsigned int Parameter;
    int TrackDirectionMask;
    float PositionX;
    float PositionY;
    float PositionZ;
    float Radius;

    unsigned int GetEventID() {
        return EventID;
    }

    float GetRadius() {
        return Radius;
    }
};

struct EventTriggerPack : public bTNode<EventTriggerPack> {
    int Version;
    int ScenerySectionNumber;
    int NumEventTriggers;
    int EndianSwapped;
    vAABBTree *EventTree;
    EventTrigger *EventTriggerArray;
};

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
    TRIGGER_EVENT_STOP_SIREN = 262145
};

struct emEvent : public bTNode<emEvent> {
    int ReferenceCount;
    unsigned int ID;
    void *pEventTrigger;
    Car *CarPtr;
    int Parameter0;
    int Parameter1;
    int Parameter2;

};

typedef void (*EVENT_HANDLER_FUNC)(emEvent *);

struct emEventHandler : public bTNode<emEventHandler> {
    EVENT_HANDLER_FUNC HandlerFunction;
    unsigned int StreamMask;
    int ReferenceCount;
    float TotalTime;
};

void bEndianSwap32(void *value);
void SwapEndian(vAABBTree *tree);
emEvent *emAddEvent(EVENT_ID event_id);

static inline bNode *GetEventTriggerPackNode_EventManager(void *event_trigger_pack) {
    return reinterpret_cast<bNode *>(event_trigger_pack);
}

static inline int *GetEventTriggerPackWords_EventManager(void *event_trigger_pack) {
    return reinterpret_cast<int *>(event_trigger_pack);
}

static inline int GetEventTriggerPackType_EventManager(void *event_trigger_pack) {
    return GetEventTriggerPackWords_EventManager(event_trigger_pack)[2];
}

static inline int GetEventTriggerPackSectionNumber_EventManager(void *event_trigger_pack) {
    return GetEventTriggerPackWords_EventManager(event_trigger_pack)[3];
}

static inline int GetEventTriggerPackNumEvents_EventManager(void *event_trigger_pack) {
    return GetEventTriggerPackWords_EventManager(event_trigger_pack)[4];
}

static inline int GetEventTriggerPackEndianSwapped_EventManager(void *event_trigger_pack) {
    return GetEventTriggerPackWords_EventManager(event_trigger_pack)[5];
}

static inline void SetEventTriggerPackEndianSwapped_EventManager(void *event_trigger_pack, int swapped) {
    GetEventTriggerPackWords_EventManager(event_trigger_pack)[5] = swapped;
}

static inline void *GetEventTriggerPackTree_EventManager(void *event_trigger_pack) {
    return reinterpret_cast<void **>(event_trigger_pack)[6];
}

static inline void SetEventTriggerPackTree_EventManager(void *event_trigger_pack, void *tree) {
    reinterpret_cast<void **>(event_trigger_pack)[6] = tree;
}

static inline void *GetEventTriggerPackData_EventManager(void *event_trigger_pack) {
    return reinterpret_cast<void **>(event_trigger_pack)[7];
}

static inline void SetEventTriggerPackData_EventManager(void *event_trigger_pack, void *data) {
    reinterpret_cast<void **>(event_trigger_pack)[7] = data;
}

static inline VisibleSectionUserInfo **GetUserInfoTable_EventManager() {
    return reinterpret_cast<VisibleSectionUserInfo **>(reinterpret_cast<char *>(&TheVisibleSectionManager) + 0x60);
}

emEvent *TriggerEventArray[41];
SlotPool *EventSlotPool = 0;
SlotPool *EventHandlerSlotPool = 0;
int EventManagerStats[5];
bTList<EventTriggerPack> EmptyEventTriggerPackList;
bTList<EventTriggerPack> EventTriggerPackList;
bTList<emEventHandler> EventHandlerList;
bTList<emEvent> MasterEventQueue;
bTList<emEvent> *CurrentEventQueue = &MasterEventQueue;
emEvent *CurrentlyHandlingEvent = 0;

void emEventManagerInit() {
    EventSlotPool = bNewSlotPool(0x24, 0x3C, "EventSlotPool", 0);
    EventHandlerSlotPool = bNewSlotPool(0x18, 0x14, "EventHandlerSlotPool", 0);
}

int emAddHandler(EVENT_HANDLER_FUNC function, unsigned int stream_mask) {
    if (function && stream_mask) {
        for (emEventHandler *handler = EventHandlerList.GetHead(); handler != EventHandlerList.EndOfList();
             handler = handler->GetNext()) {
            if (handler->HandlerFunction == function) {
                handler->ReferenceCount += 1;
                return 1;
            }
        }

        emEventHandler *handler = reinterpret_cast<emEventHandler *>(bOMalloc(EventHandlerSlotPool));
        if (handler) {
            handler->HandlerFunction = function;
            handler->StreamMask = stream_mask;
            handler->ReferenceCount = 1;
            EventHandlerList.AddTail(handler);
            EventManagerStats[1] += 1;
            if (EventManagerStats[4] < EventManagerStats[1]) {
                EventManagerStats[4] = EventManagerStats[1];
            }
            return 1;
        }
    }

    return 0;
}

void emRemoveHandler(EVENT_HANDLER_FUNC function) {
    for (emEventHandler *handler = EventHandlerList.GetHead(); handler != EventHandlerList.EndOfList();
         handler = handler->GetNext()) {
        if (handler->HandlerFunction == function) {
            int ref_count = handler->ReferenceCount;
            handler->ReferenceCount = ref_count - 1;
            if (ref_count - 1 == 0) {
                handler->Remove();
                bFree(EventHandlerSlotPool, handler);
                EventManagerStats[1] -= 1;
            }
            return;
        }
    }
}

emEvent *emAddEvent(EVENT_ID event_id) {
    emEvent *event = reinterpret_cast<emEvent *>(bOMalloc(EventSlotPool));
    if (event) {
        bMemSet(event, 0, sizeof(emEvent));
        event->ReferenceCount = 0;
        event->ID = event_id;
        CurrentEventQueue->AddTail(event);
        EventManagerStats[0] += 1;
    }
    return event;
}

int LoaderEventManager(bChunk *bchunk) {
    if (bchunk->GetID() != 0x80036000) {
        return false;
    }

    EventTriggerPack *trigger_pack = 0;
    bChunk *chunk = &bchunk[1];
    bChunk *last_chunk = reinterpret_cast<bChunk *>(reinterpret_cast<char *>(bchunk) + bchunk->Size) + 1;
    for (; chunk != last_chunk; chunk = chunk->GetNext()) {
        switch (chunk->GetID()) {
        case 0x36001: {
            trigger_pack = reinterpret_cast<EventTriggerPack *>(chunk->GetAlignedData(0x10));
            if (trigger_pack->EndianSwapped == 0) {
                bPlatEndianSwap(&trigger_pack->ScenerySectionNumber);
                bPlatEndianSwap(&trigger_pack->Version);
                bPlatEndianSwap(&trigger_pack->NumEventTriggers);
            }

            if (trigger_pack->Version != 2) {
                return true;
            }

            VisibleSectionUserInfo *user_info =
                TheVisibleSectionManager.AllocateUserInfo(trigger_pack->ScenerySectionNumber);
            user_info->pEventTriggerPack = trigger_pack;
            break;
        }

        case 0x36002:
            if (trigger_pack) {
                trigger_pack->EventTree = reinterpret_cast<vAABBTree *>(chunk->GetAlignedData(0x10));
                trigger_pack->EventTree->NodeArray =
                    reinterpret_cast<vAABB *>(reinterpret_cast<int *>(trigger_pack->EventTree) + 4);
                if (trigger_pack->EndianSwapped == 0) {
                    SwapEndian(trigger_pack->EventTree);
                }
            }
            break;

        case 0x36003:
            if (trigger_pack) {
                trigger_pack->EventTriggerArray = reinterpret_cast<EventTrigger *>(chunk->GetAlignedData(0x10));
                if (trigger_pack->EndianSwapped == 0) {
                    int num_triggers = static_cast<unsigned int>(chunk->GetAlignedSize(0x10)) >> 5;
                    for (int n = 0; n < num_triggers; n++) {
                        EventTrigger *event_trigger = &trigger_pack->EventTriggerArray[n];
                        bPlatEndianSwap(&event_trigger->NameHash);
                        bPlatEndianSwap(&event_trigger->EventID);
                        bPlatEndianSwap(&event_trigger->Parameter);
                        bPlatEndianSwap(&event_trigger->TrackDirectionMask);
                        bPlatEndianSwap(&event_trigger->PositionX);
                        bPlatEndianSwap(&event_trigger->PositionY);
                        bPlatEndianSwap(&event_trigger->PositionZ);
                        bPlatEndianSwap(&event_trigger->Radius);
                    }
                }
            }
            break;
        }
    }

    trigger_pack->EndianSwapped = 1;
    if (trigger_pack) {
        if (trigger_pack->NumEventTriggers != 0 && trigger_pack->EventTree && trigger_pack->EventTriggerArray) {
            EventTriggerPackList.AddTail(trigger_pack);
        } else {
            EmptyEventTriggerPackList.AddTail(trigger_pack);
        }
    }

    return true;
}

int UnloaderEventManager(bChunk *bchunk) {
    if (bchunk->GetID() != 0x80036000) {
        return false;
    }

    bChunk *chunk = bchunk->GetFirstChunk();
    bChunk *last_chunk = bchunk->GetLastChunk();
    if (chunk != last_chunk) {
        do {
            if (chunk->GetID() == 0x36001) {
                EventTriggerPack *trigger_pack = reinterpret_cast<EventTriggerPack *>(bchunk->GetAlignedData(0x10));
                if (trigger_pack->Version == 2) {
                    trigger_pack->Remove();
                }

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(trigger_pack->ScenerySectionNumber);
                user_info->pEventTriggerPack = 0;
                TheVisibleSectionManager.UnallocateUserInfo(trigger_pack->ScenerySectionNumber);
                break;
            }

            chunk = reinterpret_cast<bChunk *>(reinterpret_cast<char *>(chunk) + chunk->Size) + 1;
        } while (chunk != last_chunk);
    }

    return true;
}

emEvent **emTriggerEventsInSection(bVector3 *position, int section_number) {
    emEvent **current_event = TriggerEventArray;
    float x = position->x;
    float y = position->y;
    float z = position->z;
    VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(section_number);

    if (user_info && user_info->pEventTriggerPack) {
        EventTriggerPack *trigger_pack = user_info->pEventTriggerPack;
        vAABBTree *tree = trigger_pack->EventTree;
        if (tree) {
            vAABB *aabb = tree->QueryLeaf(x, y, z);
            if (aabb) {
                EventTrigger *root_event = trigger_pack->EventTriggerArray;
                int num_hits = -aabb->NumChildren;

                for (int i = 0; i < num_hits && current_event < &TriggerEventArray[40]; i++) {
                    EventTrigger *event = &root_event[aabb->ChildrenIndicies[i]];
                    float event_x = event->PositionX;
                    float event_y = event->PositionY;
                    float event_z = event->PositionZ;
                    float dx = bAbs(x - event_x);
                    float dy = bAbs(y - event_y);
                    float dz = bAbs(z - event_z);
                    float r2 = event->GetRadius();
                    float dist2 = dz * dz + dx * dx + dy * dy;

                    r2 *= r2;
                    if (dist2 < r2) {
                        emEvent *new_event = emAddEvent(static_cast<EVENT_ID>(event->GetEventID()));
                        new_event->pEventTrigger = event;
                        *current_event = new_event;
                        current_event++;
                    }
                }
            }
        }
    }

    if (current_event == TriggerEventArray) {
        return 0;
    }

    *current_event = 0;
    return TriggerEventArray;
}

void emProcessAllEvents() {
    bTList<emEvent> temp_event_queue;
    bTList<emEvent> locked_event_queue;
    emEvent *event;

    CurrentEventQueue = &temp_event_queue;
    emEvent *next_event;

    for (event = MasterEventQueue.GetHead(); event != MasterEventQueue.EndOfList(); event = next_event) {
        next_event = event->GetNext();
        int event_id = event->ID;
        int event_handled = 0;

        CurrentlyHandlingEvent = event;

        for (emEventHandler *handler = EventHandlerList.GetHead(); handler != EventHandlerList.EndOfList();
             handler = handler->GetNext()) {
            int handler_stream_mask = handler->StreamMask;

            if ((event_id & handler_stream_mask) != 0) {
                unsigned int start_ticks = bGetTicker();

                handler->HandlerFunction(event);
                handler->TotalTime += bGetTickerDifference(start_ticks, bGetTicker());
                event_handled = 1;
            }
        }

        CurrentlyHandlingEvent = 0;
        event->Remove();
        if (event->ReferenceCount == 0) {
            if (event) {
                bFree(EventSlotPool, event);
            }
        } else {
            locked_event_queue.AddTail(event);
        }
    }

    while (!locked_event_queue.IsEmpty()) {
        MasterEventQueue.AddTail(locked_event_queue.RemoveHead());
    }
    while (!temp_event_queue.IsEmpty()) {
        MasterEventQueue.AddTail(temp_event_queue.RemoveHead());
    }

    CurrentEventQueue = &MasterEventQueue;

    while (!locked_event_queue.IsEmpty()) {
        emEvent *locked_event = locked_event_queue.RemoveHead();
        if (locked_event) {
            bFree(EventSlotPool, locked_event);
        }
    }
    while (!temp_event_queue.IsEmpty()) {
        emEvent *temp_event = temp_event_queue.RemoveHead();
        if (temp_event) {
            bFree(EventSlotPool, temp_event);
        }
    }
}
