#include "TrackStreamer.hpp"

#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern BOOL bMemoryTracing;

struct bMemoryTraceAllocatePacket {
    uintptr_t PoolID;
    uintptr_t MemoryAddress;
    int Size;
    int AllocationNumber;
    char Name[48];
};

TSMemoryNode *TSMemoryPool::GetNewNode(int address, int size, bool allocated, const char *debug_name) {
    TSMemoryNode *node = 0;

    if (!UnusedNodeList.IsEmpty()) {
        node = UnusedNodeList.RemoveHead();
    } else {
        for (int i = 0; i < 192; i++) {
            TSMemoryNode *candidate = &MemoryNodes[i];
            if (!candidate->Allocated && candidate->Size == 0 && candidate->DebugName[0] == 0) {
                node = candidate;
                break;
            }
        }
    }

    if (!node) {
        return 0;
    }

    node->Address = address;
    node->Size = size;
    node->Allocated = allocated;
    bMemSet(node->DebugName, 0, sizeof(node->DebugName));
    if (debug_name) {
        bStrNCpy(node->DebugName, debug_name, sizeof(node->DebugName) - 1);
    }
    NodeList.AddTail(node);
    return node;
}

void TSMemoryPool::RemoveNode(TSMemoryNode *node) {
    if (!node) {
        return;
    }

    NodeList.Remove(node);
    node->Address = 0;
    node->Size = 0;
    node->Allocated = false;
    bMemSet(node->DebugName, 0, sizeof(node->DebugName));
    UnusedNodeList.AddTail(node);
}

void *TSMemoryPool::Malloc(int size, const char *debug_name, bool best_fit, bool allocate_from_top, int address) {
    TSMemoryNode *found_node = 0;
    TSMemoryNode *new_node;
    int new_bottom_size;
    int new_top_size;

    size = (size + 0x7f) & ~0x7f;

    if (address != 0) {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (!node->Allocated && node->Size >= size && address >= node->Address && address + size <= node->Address + node->Size) {
                found_node = node;
                break;
            }
        }
    } else if (best_fit) {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (!node->Allocated && node->Size >= size &&
                (!found_node || node->Size - size < found_node->Size - size)) {
                found_node = node;
            }
        }
    } else if (allocate_from_top) {
        for (TSMemoryNode *node = NodeList.GetTail(); node != NodeList.EndOfList(); node = node->GetPrev()) {
            if (!node->Allocated && node->Size >= size) {
                found_node = node;
                break;
            }
        }
    } else {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (!node->Allocated && node->Size >= size) {
                found_node = node;
                break;
            }
        }
    }

    if (!found_node) {
        return 0;
    }

    if (address == 0) {
        if (allocate_from_top) {
            address = found_node->Address;
        } else {
            address = found_node->Address + found_node->Size - size;
        }
    }

    AmountFree -= size;
    if (found_node->Size == LargestFree) {
        NeedToRecalcLargestFree = true;
    }

    new_node = GetNewNode(address, size, true, debug_name);
    if (!new_node) {
        return 0;
    }
    new_node->AddAfter(found_node);

    new_bottom_size = address - found_node->Address;
    new_top_size = found_node->Address + found_node->Size - (address + size);
    found_node->Size = new_bottom_size;
    if (new_bottom_size == 0) {
        RemoveNode(found_node);
    }

    if (new_top_size != 0) {
        TSMemoryNode *top_node = GetNewNode(address + size, new_top_size, false, 0);
        if (top_node) {
            top_node->AddAfter(new_node);
        }
    }

    if (TracingEnabled && bMemoryTracing) {
        bMemoryTraceAllocatePacket packet;
        bMemSet(&packet, 0, sizeof(packet));
        packet.PoolID = reinterpret_cast<uintptr_t>(this);
        packet.MemoryAddress = static_cast<uintptr_t>(address);
        packet.Size = size;
        packet.AllocationNumber = AllocationNumber;
        if (debug_name) {
            bStrNCpy(packet.Name, debug_name, sizeof(packet.Name) - 1);
        }
        bFunkCallASync("CODEINE", 0x1c, &packet, bStrLen(packet.Name) + 0x15);
    }

    Updated = true;
    AllocationNumber += 1;
    return reinterpret_cast<void *>(address);
}
