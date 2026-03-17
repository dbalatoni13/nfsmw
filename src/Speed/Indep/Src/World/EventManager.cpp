#include "Speed/Indep/Src/World/EventManager.hpp"

#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

struct vAABBTree;

void bEndianSwap32(void *value);
void SwapEndian(vAABBTree *tree);

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

SlotPool *EventSlotPool = 0;
SlotPool *EventHandlerSlotPool = 0;
bList EmptyEventTriggerPackList;
bList EventTriggerPackList;

void emEventManagerInit() {
    EventSlotPool = bNewSlotPool(0x24, 0x3C, "EventSlotPool", 0);
    EventHandlerSlotPool = bNewSlotPool(0x18, 0x14, "EventHandlerSlotPool", 0);
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
                    for (int n = 0; n < 8; n++) {
                        bEndianSwap32(&event_words[n]);
                    }
                }
            }
        }
    }

    if (event_trigger_pack) {
        SetEventTriggerPackEndianSwapped_EventManager(event_trigger_pack, 1);

        bList *event_trigger_pack_list = &EventTriggerPackList;
        if (GetEventTriggerPackNumEvents_EventManager(event_trigger_pack) == 0 || !GetEventTriggerPackTree_EventManager(event_trigger_pack) ||
            !GetEventTriggerPackData_EventManager(event_trigger_pack)) {
            event_trigger_pack_list = &EmptyEventTriggerPackList;
        }
        event_trigger_pack_list->AddTail(GetEventTriggerPackNode_EventManager(event_trigger_pack));
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
