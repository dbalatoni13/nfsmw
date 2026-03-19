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
bList EmptyEventTriggerPackList;
bList EventTriggerPackList;
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

int LoaderEventManager(bChunk *chunk) {
    if (chunk->GetID() != 0x80036000) {
        return false;
    }

    void *event_trigger_pack = 0;
    for (bChunk *child = chunk->GetFirstChunk(); child < chunk->GetLastChunk(); child = child->GetNext()) {
        if (child->GetID() == 0x36001) {
            event_trigger_pack = child->GetAlignedData(0x10);
            if (GetEventTriggerPackEndianSwapped_EventManager(event_trigger_pack) == 0) {
                int *event_trigger_pack_words = GetEventTriggerPackWords_EventManager(event_trigger_pack);
                bEndianSwap32(&event_trigger_pack_words[2]);
                bEndianSwap32(&event_trigger_pack_words[3]);
                bEndianSwap32(&event_trigger_pack_words[4]);
            }

            if (GetEventTriggerPackType_EventManager(event_trigger_pack) != 2) {
                return true;
            }

            VisibleSectionUserInfo *user_info =
                TheVisibleSectionManager.AllocateUserInfo(GetEventTriggerPackSectionNumber_EventManager(event_trigger_pack));
            user_info->pEventTriggerPack = reinterpret_cast<EventTriggerPack *>(event_trigger_pack);
        } else if (child->GetID() == 0x36002) {
            if (event_trigger_pack) {
                void *tree = child->GetAlignedData(0x10);
                SetEventTriggerPackTree_EventManager(event_trigger_pack, tree);
                *reinterpret_cast<int *>(tree) = reinterpret_cast<int>(reinterpret_cast<int *>(tree) + 4);
                if (GetEventTriggerPackEndianSwapped_EventManager(event_trigger_pack) == 0) {
                    SwapEndian(reinterpret_cast<vAABBTree *>(tree));
                }
            }
        } else if (child->GetID() == 0x36003 && event_trigger_pack) {
            int *event_data = reinterpret_cast<int *>(child->GetAlignedData(0x10));
            SetEventTriggerPackData_EventManager(event_trigger_pack, event_data);

            if (GetEventTriggerPackEndianSwapped_EventManager(event_trigger_pack) == 0) {
                int num_event_words =
                    (child->GetSize() - (reinterpret_cast<char *>(event_data) - child->GetData())) >> 5;
                for (int i = 0; i < num_event_words; i++) {
                    int *event_words = reinterpret_cast<int *>(reinterpret_cast<char *>(event_data) + i * 0x20);
                    bEndianSwap32(&event_words[0]);
                    bEndianSwap32(&event_words[1]);
                    bEndianSwap32(&event_words[2]);
                    bEndianSwap32(&event_words[3]);
                    bEndianSwap32(&event_words[4]);
                    bEndianSwap32(&event_words[5]);
                    bEndianSwap32(&event_words[6]);
                    bEndianSwap32(&event_words[7]);
                }
            }
        }
    }

    if (event_trigger_pack) {
        GetEventTriggerPackWords_EventManager(event_trigger_pack)[5] = 1;

        bList *event_trigger_pack_list = &EventTriggerPackList;
        if (GetEventTriggerPackNumEvents_EventManager(event_trigger_pack) == 0 || !GetEventTriggerPackTree_EventManager(event_trigger_pack) ||
            !GetEventTriggerPackData_EventManager(event_trigger_pack)) {
            event_trigger_pack_list = &EmptyEventTriggerPackList;
        }
        bNode *node = GetEventTriggerPackNode_EventManager(event_trigger_pack);
        bNode *new_prev = event_trigger_pack_list->HeadNode.Prev;
        new_prev->Next = node;
        event_trigger_pack_list->HeadNode.Prev = node;
        node->Next = &event_trigger_pack_list->HeadNode;
        node->Prev = new_prev;
    }

    return true;
}

int UnloaderEventManager(bChunk *chunk) {
    if (chunk->GetID() != 0x80036000) {
        return false;
    }

    for (bChunk *child = chunk->GetFirstChunk(); child < chunk->GetLastChunk(); child = child->GetNext()) {
        if (child->GetID() == 0x36001) {
            void *event_trigger_pack = child->GetAlignedData(0x10);
            if (GetEventTriggerPackType_EventManager(event_trigger_pack) == 2) {
                GetEventTriggerPackNode_EventManager(event_trigger_pack)->Remove();
            }

            int section_number = GetEventTriggerPackSectionNumber_EventManager(event_trigger_pack);
            VisibleSectionUserInfo *user_info = GetUserInfoTable_EventManager()[section_number];
            if (user_info) {
                user_info->pEventTriggerPack = 0;
            }
            TheVisibleSectionManager.UnallocateUserInfo(section_number);
            break;
        }
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
    bTList<emEvent> queued_events;
    bTList<emEvent> referenced_events;

    CurrentEventQueue = &queued_events;
    while (!MasterEventQueue.IsEmpty()) {
        emEvent *event = MasterEventQueue.GetHead();
        CurrentlyHandlingEvent = event;

        for (emEventHandler *handler = EventHandlerList.GetHead(); handler != EventHandlerList.EndOfList();
             handler = handler->GetNext()) {
            if ((event->ID & handler->StreamMask) != 0) {
                unsigned int start_ticks = bGetTicker();
                handler->HandlerFunction(event);
                handler->TotalTime += bGetTickerDifference(start_ticks, bGetTicker());
            }
        }

        MasterEventQueue.Remove(event);
        CurrentlyHandlingEvent = 0;
        if (event->ReferenceCount == 0) {
            bFree(EventSlotPool, event);
        } else {
            referenced_events.AddTail(event);
        }
    }

    while (!referenced_events.IsEmpty()) {
        MasterEventQueue.AddTail(referenced_events.RemoveHead());
    }
    while (!queued_events.IsEmpty()) {
        MasterEventQueue.AddTail(queued_events.RemoveHead());
    }

    CurrentEventQueue = &MasterEventQueue;
}
