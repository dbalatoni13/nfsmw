#include "Speed/Indep/Src/World/EventManager.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

emEvent *emAddEvent(EVENT_ID event_id);

int EventManagerStats[5];
emEvent *TriggerEventArray[41];
SlotPool *EventSlotPool = nullptr;
SlotPool *EventHandlerSlotPool = nullptr;
bTList<EventTriggerPack> EmptyEventTriggerPackList;
bTList<EventTriggerPack> EventTriggerPackList;
bTList<emEventHandler> EventHandlerList;
bTList<emEvent> MasterEventQueue;
bTList<emEvent> *CurrentEventQueue = &MasterEventQueue;
emEvent *CurrentlyHandlingEvent = nullptr;

void emEventManagerInit() {
    EventSlotPool = bNewSlotPool(0x24, 60, "EventSlotPool", 0);
    EventHandlerSlotPool = bNewSlotPool(0x18, 20, "EventHandlerSlotPool", 0);
}

int LoaderEventManager(bChunk *bchunk) {
    if (bchunk->GetID() != BCHUNK_EVENT_TRIGGER) {
        return false;
    }

    EventTriggerPack *trigger_pack = nullptr;
    bChunk *chunk = bchunk->GetFirstChunk();
    bChunk *last_chunk = bchunk->GetLastChunk();
    for (; chunk != last_chunk; chunk = chunk->GetNext()) {
        switch (chunk->GetID()) {
            case BCHUNK_EVENT_TRIGGER_PACK_HEADER: {
                trigger_pack = reinterpret_cast<EventTriggerPack *>(chunk->GetAlignedData(16));
                if (!trigger_pack->EndianSwapped) {
                    bPlatEndianSwap(&trigger_pack->ScenerySectionNumber);
                    bPlatEndianSwap(&trigger_pack->Version);
                    bPlatEndianSwap(&trigger_pack->NumEventTriggers);
                }

                if (trigger_pack->Version != 2) {
                    return true;
                }

                VisibleSectionUserInfo *user_info = TheVisibleSectionManager.AllocateUserInfo(trigger_pack->ScenerySectionNumber);
                user_info->pEventTriggerPack = trigger_pack;
                break;
            }

            case BCHUNK_EVENT_TRIGGER_NODES:
                if (trigger_pack) {
                    trigger_pack->EventTree = reinterpret_cast<vAABBTree *>(chunk->GetAlignedData(16));
                    trigger_pack->EventTree->NodeArray = reinterpret_cast<vAABB *>(reinterpret_cast<int *>(trigger_pack->EventTree) + 4);
                    if (!trigger_pack->EndianSwapped) {
                        trigger_pack->EventTree->SwapEndian();
                    }
                }
                break;

            case BCHUNK_EVENT_TRIGGER_ENTRIES:
                if (trigger_pack) {
                    trigger_pack->EventTriggerArray = reinterpret_cast<EventTrigger *>(chunk->GetAlignedData(16));
                    if (!trigger_pack->EndianSwapped) {
                        int num_triggers = static_cast<unsigned int>(chunk->GetAlignedSize(16)) / sizeof(EventTrigger);
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

    trigger_pack->EndianSwapped = true;
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
    if (bchunk->GetID() != BCHUNK_EVENT_TRIGGER) {
        return false;
    }

    bChunk *chunk = bchunk->GetFirstChunk();
    bChunk *last_chunk = bchunk->GetLastChunk();
    // The loop doesn't make sense (we don't do chunk = chunk->GetNext())
    while (chunk != last_chunk) {
        if (chunk->GetID() == BCHUNK_EVENT_TRIGGER_PACK_HEADER) {
            EventTriggerPack *trigger_pack = reinterpret_cast<EventTriggerPack *>(chunk->GetAlignedData(16));
            if (trigger_pack->Version == 2) {
                trigger_pack->Remove();
            }

            VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(trigger_pack->ScenerySectionNumber);
            user_info->pEventTriggerPack = 0;
            TheVisibleSectionManager.UnallocateUserInfo(trigger_pack->ScenerySectionNumber);
            break;
        }
    }

    return true;
}

int emAddHandler(EVENT_HANDLER_FUNC function, unsigned int stream_mask) {
    if ((function && stream_mask) == 0) {
        return 0;
    }
    emEventHandler *handler;
    for (handler = EventHandlerList.GetHead(); handler != EventHandlerList.EndOfList(); handler = handler->GetNext()) {
        if (handler->HandlerFunction == function) {
            handler->ReferenceCount += 1;
            return 1;
        }
    }

    handler = new emEventHandler();
    if (!handler) {
        return 0;
    }

    handler->HandlerFunction = function;
    handler->StreamMask = stream_mask;
    handler->ReferenceCount = 1;
    EventHandlerList.AddTail(handler);
    EventManagerStats[1]++;
    if (EventManagerStats[1] > EventManagerStats[4]) {
        EventManagerStats[4] = EventManagerStats[1];
    }
    return 1;
}

void emRemoveHandler(EVENT_HANDLER_FUNC function) {
    for (emEventHandler *handler = EventHandlerList.GetHead(); handler != EventHandlerList.EndOfList(); handler = handler->GetNext()) {
        if (handler->HandlerFunction == function) {
            if (--handler->ReferenceCount == 0) {
                if (handler->Remove()) {
                    delete handler;
                }
                EventManagerStats[1]--;
            }
            return;
        }
    }
}

emEvent *emAddEvent(EVENT_ID event_id) {
    emEvent *event = new emEvent;
    if (!event) {
        return 0;
    }

    bMemSet(event, 0, sizeof(emEvent));
    event->ReferenceCount = 0;
    event->ID = event_id;
    CurrentEventQueue->AddTail(event);
    EventManagerStats[0]++;
    return event;
}

void emProcessAllEvents() {
    bTList<emEvent> temp_event_queue;
    bTList<emEvent> locked_event_queue;

    CurrentEventQueue = &temp_event_queue;
    emEvent *event = MasterEventQueue.GetHead();

    while (event != MasterEventQueue.EndOfList()) {
        emEvent *next_event = event->GetNext();
        int event_id = event->ID;
        int event_handled = 0;

        CurrentlyHandlingEvent = event;

        for (emEventHandler *handler = EventHandlerList.GetHead(); handler != EventHandlerList.EndOfList(); handler = handler->GetNext()) {
            int handler_stream_mask = handler->StreamMask;

            if ((event_id & handler_stream_mask) != 0) {
                unsigned int start_time = bGetTicker();

                handler->HandlerFunction(event);
                handler->TotalTime += bGetTickerDifference(start_time, bGetTicker());
                event_handled = 1;
            }
        }

        CurrentlyHandlingEvent = nullptr;
        event->Remove();
        if (event->ReferenceCount == 0) {
            delete event;
        } else {
            locked_event_queue.AddTail(event);
        }

        event = next_event;
    }

    while (!locked_event_queue.IsEmpty()) {
        MasterEventQueue.AddTail(locked_event_queue.RemoveHead());
    }
    while (!temp_event_queue.IsEmpty()) {
        MasterEventQueue.AddTail(temp_event_queue.RemoveHead());
    }

    CurrentEventQueue = &MasterEventQueue;
}

emEvent **emTriggerEventsInSection(bVector3 *position, int section_number) {
    emEvent **current_event = TriggerEventArray;
    emEvent **sentinel_event = &TriggerEventArray[40];
    float x = position->x;
    float y = position->y;
    float z = position->z;
    VisibleSectionUserInfo *user_info = TheVisibleSectionManager.GetUserInfo(section_number);

    if (user_info && user_info->pEventTriggerPack) {
        EventTriggerPack *trigger_pack = user_info->pEventTriggerPack;
        vAABBTree *tree = trigger_pack->EventTree;
        vAABB *aabb = tree->QueryLeaf(x, y, z);
        if (aabb) {
            EventTrigger *root_event = trigger_pack->EventTriggerArray;
            int num_hits = -aabb->NumChildren;

            for (int i = 0; i < num_hits && current_event < sentinel_event; i++) {
                EventTrigger *event = &root_event[aabb->ChildrenIndicies[i]];
                float event_x = event->PositionX;
                float event_y = event->PositionY;
                float event_z = event->PositionZ;
                float dx = bAbs(x - event_x);
                float dy = bAbs(y - event_y);
                float dz = bAbs(z - event_z);
                float r2 = event->GetRadius();
                float dist2 = dx * dx + dy * dy + dz * dz;

                r2 *= event->GetRadius();
                if (dist2 < r2) {
                    emEvent *new_event = emAddEvent(static_cast<EVENT_ID>(event->GetEventID()));
                    new_event->pEventTrigger = event;
                    *current_event = new_event;
                    current_event++;
                }
            }
        }
    }

    if (current_event == TriggerEventArray) {
        return nullptr;
    }

    *current_event = nullptr;
    return TriggerEventArray;
}
