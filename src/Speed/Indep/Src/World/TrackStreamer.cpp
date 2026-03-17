#include "TrackStreamer.hpp"

#include "TrackPath.hpp"
#include "VisibleSection.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/PPCArch.h"
#include "dolphin/os/OSCache.h"

extern BOOL bMemoryTracing;
extern int ScenerySectionLODOffset;
extern int SeeulatorToolActive;
extern int ScenerySectionToBlink;
extern int RealLoopCounter;
extern bool PostLoadFixupDisabled;
extern int AllowDuplicateSolids;
extern int ForceHoleFillerMethod;
extern int WaitForFrameBufferSwapDisabled;
extern unsigned int eFrameCounter;
int Get2PlayerSectionNumber(int section_number);
void GetScenerySectionName(char *name, int section_number);
const char *GetScenerySectionName(int section_number);
void PostLoadFixup();
void SetDuplicateTextureWarning(BOOL enabled);
bool LoadTempPermChunks(bChunk **ppchunks, int *psizeof_chunks, int allocation_params, const char *debug_name);
int DoLinesIntersect(const bVector2 &line1_start, const bVector2 &line1_end, const bVector2 &line2_start, const bVector2 &line2_end);
void eWaitUntilRenderingDone();
void MoveChunks(bChunk *dest_chunks, bChunk *source_chunks, int sizeof_chunks, const char *debug_name);

static unsigned int prev_need_loading_bar_26275 = 0;
static const float kMaxDistance_TrackStreamer = 3.4028235e+38f;
static const float kVelocityEpsilon_TrackStreamer = 0.0f;
static const float kFuturePositionScale_TrackStreamer = 0.5f;
static const float kPredictionScaleA_TrackStreamer = 1.0f;
static const float kPredictionScaleB_TrackStreamer = 1.0f;
static const float kLoadingBarDistanceThreshold_TrackStreamer = 15.0f;
static const float kLoadingBarSpeedThreshold_TrackStreamer = 100.0f;
static const float kSwitchZoneFarLoadThreshold_TrackStreamer = 0.1f;
static const float kPredictedZoneScale_TrackStreamer = 1.5f;
static const float kPredictedZoneMaxDistance_TrackStreamer = 100.0f;
static const float kPredictedZoneStopProjectSpeed_TrackStreamer = 178.81091f;
static const float kPredictedZoneEqualEpsilon_TrackStreamer = 0.001f;
static unsigned int last_jettison_print_26154 = 0;

static void SortIntsAscending_TrackStreamer(int *values, int count) {
    for (int i = 1; i < count; i++) {
        int value = values[i];
        int n = i - 1;
        while (n >= 0 && value < values[n]) {
            values[n + 1] = values[n];
            n -= 1;
        }
        values[n + 1] = value;
    }
}

static char GetScenerySectionLetter_TrackStreamer(int section_number) {
    return static_cast<char>(section_number / 100 + 'A' - 1);
}

static bool IsRegularScenerySection_TrackStreamer(int section_number) {
    char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
    return section_letter >= 'A' && section_letter < 'U';
}

static bool IsLoadingBarSection_TrackStreamer(int section_number) {
    if (!IsRegularScenerySection_TrackStreamer(section_number)) {
        return false;
    }

    int subsection_number = section_number % 100;
    return (subsection_number > 0 && subsection_number < ScenerySectionLODOffset) ||
           (ScenerySectionLODOffset <= subsection_number && subsection_number < ScenerySectionLODOffset * 2);
}

struct bBitTableLayout_TrackStreamer {
    int NumBits;
    uint8 *Bits;
};

static void SetBit_TrackStreamer(bBitTable *bit_table, int bit) {
    bBitTableLayout_TrackStreamer *layout = reinterpret_cast<bBitTableLayout_TrackStreamer *>(bit_table);
    layout->Bits[bit >> 3] |= static_cast<uint8>(1 << (bit & 7));
}

static void ClearTable_TrackStreamer(bBitTable *bit_table) {
    bBitTableLayout_TrackStreamer *layout = reinterpret_cast<bBitTableLayout_TrackStreamer *>(bit_table);
    bMemSet(layout->Bits, 0, layout->NumBits >> 3);
}

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

int TSMemoryPool::GetAmountFree() {
    for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
    }
    return AmountFree;
}

TSMemoryNode *TSMemoryPool::GetNextNode(bool start_from_top, TSMemoryNode *node) {
    TSMemoryNode *next_node;
    if (!start_from_top) {
        next_node = node ? node->GetPrev() : NodeList.GetTail();
    } else {
        next_node = node ? node->GetNext() : NodeList.GetHead();
    }

    if (next_node == NodeList.EndOfList()) {
        return 0;
    }
    return next_node;
}

TSMemoryNode *TSMemoryPool::GetNextFreeNode(bool start_from_top, TSMemoryNode *node) {
    do {
        node = GetNextNode(start_from_top, node);
        if (!node) {
            return 0;
        }
    } while (node->Allocated);

    return node;
}

TSMemoryNode *TSMemoryPool::GetNextAllocatedNode(bool start_from_top, TSMemoryNode *node) {
    do {
        node = GetNextNode(start_from_top, node);
        if (!node) {
            return 0;
        }
    } while (!node->Allocated);

    return node;
}

unsigned int TSMemoryPool::GetPoolChecksum() {
    return 0;
}

void *TrackStreamer::AllocateMemory(TrackStreamingSection *section, int allocation_params) {
    void *memory = bMalloc(section->Size, allocation_params | 0x2007);
    if (!memory) {
        bBreak();
    }
    return memory;
}

int TrackStreamer::AllocateSectionMemory(int *ptotal_needing_allocation) {
    int total_out_of_memory = 0;
    int total_needing_allocation = 0;
    int total_sections_allocated = 0;

    if (LoadingPhase == ALLOCATING_REGULAR_SECTIONS && pDiscBundleSections < pLastDiscBundleSection) {
        for (DiscBundleSection *disc_bundle = pDiscBundleSections; disc_bundle < pLastDiscBundleSection;
             disc_bundle = reinterpret_cast<DiscBundleSection *>(
                 reinterpret_cast<char *>(disc_bundle) + disc_bundle->NumMembers * sizeof(DiscBundleSectionMember) + 0x14)) {
            int member_index = 0;
            if (disc_bundle->NumMembers > 0) {
                do {
                    TrackStreamingSection *section = disc_bundle->Members[member_index].pSection;
                    if (!section->CurrentlyVisible || section->Status != TrackStreamingSection::UNLOADED) {
                        break;
                    }
                    member_index += 1;
                } while (member_index < disc_bundle->NumMembers);
            }

            if (member_index == disc_bundle->NumMembers) {
                int largest_free_block = pMemoryPool->GetLargestFreeBlock();
                if (disc_bundle->FileSize <= largest_free_block) {
                    void *bundle_memory = pMemoryPool->Malloc(disc_bundle->FileSize, disc_bundle->SectionName, true, false, 0);
                    pMemoryPool->Free(bundle_memory);

                    for (int i = 0; i < disc_bundle->NumMembers; i++) {
                        DiscBundleSectionMember *member = &disc_bundle->Members[i];
                        TrackStreamingSection *section = member->pSection;
                        char *section_memory = static_cast<char *>(bundle_memory) + member->FileOffset * 0x80;

                        total_sections_allocated += 1;
                        section->pDiscBundle = disc_bundle;
                        section->Status = TrackStreamingSection::ALLOCATED;
                        section->pMemory = section_memory;
                        pMemoryPool->Malloc(section->Size, disc_bundle->SectionName, false, false, reinterpret_cast<int>(section_memory));
                    }

                    total_needing_allocation += disc_bundle->FileSize;
                }
            }
        }
    }

    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        if (section->Status != TrackStreamingSection::UNLOADED) {
            continue;
        }

        bool should_allocate = true;
        int section_number = section->SectionNumber;
        if (section_number != 0x9c4 && section_number != 0x960 && section_number != 0x8fc && section_number != 0x834 &&
            section_number != 0xa28) {
            if (LoadingPhase == ALLOCATING_TEXTURE_SECTIONS) {
                should_allocate = false;
                char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
                if (section_letter == 'Y' || section_letter == 'W') {
                    should_allocate = true;
                }
            }

            if (LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS) {
                should_allocate = false;
                char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
                if (section_letter == 'X' || section_letter == 'U') {
                    should_allocate = true;
                }
            }
        }

        if (!should_allocate) {
            continue;
        }

        total_needing_allocation += section->Size;
        if (bLargestMalloc(7) < section->Size) {
            total_out_of_memory += section->Size;
            NumSectionsOutOfMemory += 1;
        } else {
            total_sections_allocated += 1;
            section->pMemory = AllocateMemory(section, 0x80);
            section->Status = TrackStreamingSection::ALLOCATED;
            if (total_sections_allocated > 99999) {
                CurrentZoneAllocatedButIncomplete = true;
                return total_out_of_memory;
            }
        }
    }

    CurrentZoneAllocatedButIncomplete = false;
    *ptotal_needing_allocation = total_needing_allocation;
    return total_out_of_memory;
}

TrackStreamingSection *TrackStreamer::FindSectionByAddress(int address) {
    void *memory = reinterpret_cast<void *>(address);

    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        if (section->pMemory == memory) {
            return section;
        }
    }

    for (int i = 0; i < NumTrackStreamingSections; i++) {
        TrackStreamingSection *section = &pTrackStreamingSections[i];
        if (section->pMemory == memory) {
            return section;
        }
    }

    return 0;
}

int TrackStreamer::BuildHoleMovements(HoleMovement *hole_movements, int max_movements, int filler_method, int largest_free,
                                      int *pamount_moved, int max_amount_to_move) {
    int biggest_allocation = -1;
    bool failed = false;
    int num_movements = 0;
    int amount_moved = 0;

    while (true) {
        if (largest_free < 0) {
            int did_allocate = AllocateSectionMemory(&biggest_allocation);
            FreeSectionMemory();
            if (!did_allocate) {
                break;
            }
        } else if (largest_free <= pMemoryPool->GetLargestFreeBlock()) {
            break;
        }

        if (filler_method != 0) {
            if (pMemoryPool->GetAmountFree() == pMemoryPool->GetLargestFreeBlock()) {
                break;
            }
        }

        if (num_movements == max_movements) {
            break;
        }

        HoleMovement *movement = &hole_movements[num_movements];
        movement->Address = 0;

        if (filler_method <= 2) {
            bool start_from_top = filler_method == 2;
            TSMemoryNode *free_node = pMemoryPool->GetNextFreeNode(start_from_top, 0);
            TSMemoryNode *allocated_node = pMemoryPool->GetNextAllocatedNode(start_from_top, free_node);
            if (filler_method == 0 && !allocated_node) {
                break;
            }

            if (allocated_node && free_node) {
                movement->Size = allocated_node->Size;
                movement->Address = allocated_node->Address;
                movement->NewAddress =
                    start_from_top ? free_node->Address : free_node->Address + free_node->Size - movement->Size;
                if (filler_method == 0 && !FindSectionByAddress(movement->Address)) {
                    break;
                }
            }
        } else if (filler_method == 4 || filler_method == 5) {
            bool start_from_top = filler_method == 5;
            TSMemoryNode *free_node = pMemoryPool->GetNextFreeNode(start_from_top, 0);
            bool found = false;
            int best_combined_size = 0;

            for (TSMemoryNode *allocated_node = pMemoryPool->GetNextAllocatedNode(start_from_top, free_node); allocated_node;
                 allocated_node = pMemoryPool->GetNextAllocatedNode(start_from_top, allocated_node)) {
                TSMemoryNode *next_free = pMemoryPool->GetNextFreeNode(start_from_top, allocated_node);
                if (!next_free) {
                    continue;
                }
                if (!found || allocated_node->Size <= free_node->Size) {
                    int combined_size = allocated_node->Size;
                    TSMemoryNode *other_node = pMemoryPool->GetNextNode(!start_from_top, allocated_node);
                    TSMemoryNode *same_dir_node = pMemoryPool->GetNextNode(start_from_top, allocated_node);
                    if (other_node && !other_node->Allocated) {
                        combined_size += other_node->Size;
                    }
                    if (same_dir_node && !same_dir_node->Allocated) {
                        combined_size += same_dir_node->Size;
                    }
                    if (best_combined_size < combined_size) {
                        best_combined_size = combined_size;
                        movement->Size = allocated_node->Size;
                        movement->Address = allocated_node->Address;
                        movement->NewAddress =
                            start_from_top ? free_node->Address : free_node->Address + free_node->Size - movement->Size;
                        found = true;
                    }
                }
            }
        } else {
            bool found = false;
            int smallest_size = 0x3e8000;
            int best_gap = 0;
            int best_address = 0;
            TSMemoryNode *best_node = 0;

            for (TSMemoryNode *free_node = pMemoryPool->GetNextFreeNode(true, 0); free_node;
                 free_node = pMemoryPool->GetNextFreeNode(true, free_node)) {
                int gap_size = free_node->Size;
                TSMemoryNode *next_free = pMemoryPool->GetNextFreeNode(true, free_node);
                if (!next_free) {
                    break;
                }

                gap_size += next_free->Size;
                TSMemoryNode *node = pMemoryPool->GetNextNode(true, free_node);
                if (!node) {
                    continue;
                }

                int sizes[32];
                int count = 0;
                int total_size = node->Size;
                TSMemoryNode *largest_node = node;
                while (node && node != next_free && count < 32) {
                    sizes[count] = node->Size;
                    if (largest_node->Size < node->Size) {
                        largest_node = node;
                    }
                    count += 1;
                    node = pMemoryPool->GetNextNode(true, node);
                    if (node && node != next_free) {
                        total_size += node->Size;
                    }
                }

                if (count <= 0) {
                    continue;
                }

                int free_gap = gap_size - total_size;
                if (!found || best_gap < free_gap) {
                    SortIntsAscending_TrackStreamer(sizes, count);

                    bool started = false;
                    int chosen_count = 0;
                    int free_index = 0;
                    TSMemoryNode *candidate_free = pMemoryPool->GetNextFreeNode(true, 0);
                    while (0 < count - chosen_count && candidate_free) {
                        bool used = false;
                        int target_index = count - chosen_count - 1;
                        for (int i = 0; i < count; i++) {
                            if (sizes[target_index] == free_index) {
                                used = true;
                            }
                        }
                        if (candidate_free == free_node || candidate_free == next_free) {
                            used = true;
                        }
                        if (!used && sizes[target_index] <= candidate_free->Size) {
                            sizes[target_index] = free_index;
                            chosen_count += 1;
                            if (!started) {
                                best_address = candidate_free->Address;
                                started = true;
                            }
                            candidate_free = pMemoryPool->GetNextFreeNode(true, 0);
                        }
                        candidate_free = pMemoryPool->GetNextFreeNode(true, candidate_free);
                        free_index += 1;
                    }

                    if (count <= chosen_count) {
                        best_gap = free_gap;
                        best_address = best_address;
                        best_node = largest_node;
                        if (biggest_allocation <= gap_size + total_size) {
                            found = true;
                            smallest_size = total_size;
                        }
                    }
                }
            }

            if (found && best_node && FindSectionByAddress(best_node->Address)) {
                movement->Size = best_node->Size;
                movement->Address = best_node->Address;
                movement->NewAddress = best_address;
            }
        }

        if (movement->Address == 0) {
            failed = true;
            break;
        }

        num_movements += 1;
        movement->Checksum = pMemoryPool->GetPoolChecksum();
        pMemoryPool->Free(reinterpret_cast<void *>(movement->Address));
        pMemoryPool->Malloc(movement->Size, "HoleMovement", false, false, movement->NewAddress);
        amount_moved += movement->Size;
        if (max_amount_to_move < amount_moved) {
            failed = true;
            break;
        }
    }

    for (int i = num_movements - 1; i >= 0; i--) {
        HoleMovement *movement = &hole_movements[i];
        pMemoryPool->Free(reinterpret_cast<void *>(movement->NewAddress));
        TrackStreamingSection *section = FindSectionByAddress(movement->Address);
        const char *debug_name = "UndoHoleMovement";
        if (section) {
            debug_name = section->SectionName;
        }
        pMemoryPool->Malloc(movement->Size, debug_name, false, false, movement->Address);
    }

    if (pamount_moved) {
        *pamount_moved = amount_moved;
    }

    if (failed) {
        return -1;
    }
    return num_movements;
}

int TrackStreamer::DoHoleFilling(int largest_free) {
    const char *debug_name = 0;
    HoleMovement hole_movements[128];
    int amount_moved = 0;

    CountUserAllocations(&debug_name);
    if (debug_name) {
        pMemoryPool->DebugPrint();
        return 0;
    }

    int best_method = -1;
    int best_amount_moved = 0x7FFFFFFF;
    if (ForceHoleFillerMethod < 0) {
        for (int filler_method = 1; filler_method < 6; filler_method++) {
            int num_movements =
                BuildHoleMovements(hole_movements, 0x80, filler_method, largest_free, &amount_moved, best_amount_moved);
            if (num_movements > 0 && amount_moved < best_amount_moved) {
                best_method = filler_method;
                best_amount_moved = amount_moved;
            }
        }
    } else {
        int num_movements = BuildHoleMovements(hole_movements, 0x80, ForceHoleFillerMethod, largest_free, 0, 0x7FFFFFFF);
        if (num_movements > 0) {
            best_method = ForceHoleFillerMethod;
        }
    }

    if (best_method < 0) {
        return 0;
    }

    int num_movements = BuildHoleMovements(hole_movements, 0x80, best_method, largest_free, 0, 0x7FFFFFFF);
    for (int i = 0; i < num_movements; i++) {
        HoleMovement *movement = &hole_movements[i];
        TrackStreamingSection *section = FindSectionByAddress(movement->Address);
        if (LastWaitUntilRenderingDoneFrameCount != eFrameCounter) {
            unsigned int wait_start = bGetTicker();
            WaitForFrameBufferSwapDisabled = 1;
            eWaitUntilRenderingDone();
            LastWaitUntilRenderingDoneFrameCount = eFrameCounter;
            WaitForFrameBufferSwapDisabled = 0;
            bGetTickerDifference(wait_start);
        }

        unsigned int move_start = bGetTicker();
        int new_address = movement->NewAddress;
        pMemoryPool->Free(reinterpret_cast<void *>(movement->Address));
        pMemoryPool->Malloc(movement->Size, section ? section->SectionName : "HoleMovement", false, false, new_address);
        if (section && section->Status == TrackStreamingSection::ACTIVATED) {
            AllowDuplicateSolids += 1;
            SetDuplicateTextureWarning(false);
            MoveChunks(reinterpret_cast<bChunk *>(new_address), reinterpret_cast<bChunk *>(section->pMemory), section->LoadedSize,
                       section->SectionName);
            DCStoreRangeNoSync(reinterpret_cast<void *>(new_address), section->LoadedSize);
            AllowDuplicateSolids -= 1;
            SetDuplicateTextureWarning(true);
        } else if (section) {
            eWaitUntilRenderingDone();
            bOverlappedMemCpy(reinterpret_cast<void *>(new_address), section->pMemory, section->LoadedSize);
        }

        if (section) {
            section->pMemory = reinterpret_cast<void *>(new_address);
        }
        bGetTickerDifference(move_start);
        NumSectionsMoved += 1;
        PPCSync();
    }

    return 1;
}

void TrackStreamer::RemoveCurrentStreamingSections() {
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        CurrentStreamingSections[i]->CurrentlyVisible = 0;
    }

    NumCurrentStreamingSections = 0;
    ClearTable_TrackStreamer(&CurrentVisibleSectionTable);
}

void TrackStreamer::AddCurrentStreamingSections(short *section_numbers, int num_sections, int position_number) {
    if (num_sections <= 0) {
        return;
    }

    StreamingPositionEntry *streaming_position = &StreamingPositionEntries[position_number];
    for (int i = 0; i < num_sections; i++) {
        short &section_number = section_numbers[i];
        SetBit_TrackStreamer(&CurrentVisibleSectionTable, section_number);
        if (SplitScreen) {
            section_number = static_cast<short>(Get2PlayerSectionNumber(section_number));
        }

        TrackStreamingSection *section = FindSection(section_number);
        if (!section) {
            continue;
        }

        section->LastNeededTimestamp = RealTimeFrames;
        if (!section->CurrentlyVisible) {
            CurrentStreamingSections[NumCurrentStreamingSections] = section;
            NumCurrentStreamingSections += 1;
        }

        unsigned char position_bit = static_cast<unsigned char>(1 << (position_number & 0x1f));
        if ((section->CurrentlyVisible & position_bit) == 0) {
            section->CurrentlyVisible |= position_bit;
            if (section->Status < TrackStreamingSection::LOADED) {
                streaming_position->NumSectionsToLoad += 1;
                streaming_position->AmountToLoad += section->Size;
            }
        }
    }
}

void TrackStreamer::DetermineStreamingSections() {
    short section_numbers[384];
    int num_sections = 3;

    RemoveCurrentStreamingSections();

    section_numbers[0] = 0x9c4;
    section_numbers[1] = 0x960;
    section_numbers[2] = 0xa28;
    if (SeeulatorToolActive && ScenerySectionToBlink != 0) {
        section_numbers[3] = static_cast<short>(ScenerySectionToBlink);
        num_sections = 4;
    }

    for (int i = 0; i < 4; i++) {
        short keep_section = KeepSectionTable[i];
        if (keep_section != 0) {
            section_numbers[num_sections] = keep_section;
            num_sections += 1;
        }
    }

    AddCurrentStreamingSections(section_numbers, num_sections, 0);
    AddCurrentStreamingSections(section_numbers, num_sections, 1);

    for (int i = 0; i < 2; i++) {
        StreamingPositionEntry *streaming_position = &StreamingPositionEntries[i];
        if (streaming_position->CurrentZone > 0) {
            LoadingSection *loading_section = TheVisibleSectionManager.FindLoadingSection(streaming_position->CurrentZone);
            if (!loading_section) {
                DrivableScenerySection *drivable_section = TheVisibleSectionManager.FindDrivableSection(streaming_position->CurrentZone);
                num_sections = 0;
                if (drivable_section) {
                    for (int n = 0; n < drivable_section->NumVisibleSections; n++) {
                        section_numbers[num_sections] = drivable_section->VisibleSections[n];
                        num_sections += 1;
                    }
                }
            } else {
                num_sections = TheVisibleSectionManager.GetSectionsToLoad(loading_section, section_numbers, 0x180);
            }

            AddCurrentStreamingSections(section_numbers, num_sections, i);
        }
    }
}

void TrackStreamer::PlotLoadingMarker(StreamingPositionEntry *streaming_position) {}

void TrackStreamer::SwitchZones(short *current_zones) {
    StartLoadingTime = GetDebugRealTime();
    CurrentZoneNeedsRefreshing = false;

    for (int i = 0; i < 2; i++) {
        short current_zone = current_zones[i];
        StreamingPositionEntry *streaming_position = &StreamingPositionEntries[i];
        if (streaming_position->CurrentZone != current_zone) {
            PlotLoadingMarker(streaming_position);

            VisibleSectionBoundary *old_boundary = TheVisibleSectionManager.FindBoundary(streaming_position->CurrentZone);
            VisibleSectionBoundary *new_boundary = TheVisibleSectionManager.FindBoundary(current_zone);
            float distance_to_boundary = kMaxDistance_TrackStreamer;
            if (old_boundary && new_boundary) {
                for (int n = 0; n < old_boundary->NumPoints; n++) {
                    float boundary_distance =
                        new_boundary->GetDistanceOutside(&old_boundary->Points[n], kMaxDistance_TrackStreamer);
                    if (boundary_distance < distance_to_boundary) {
                        distance_to_boundary = boundary_distance;
                    }
                }
            }

            if (kSwitchZoneFarLoadThreshold_TrackStreamer < distance_to_boundary) {
                CurrentZoneFarLoad = true;
            }

            streaming_position->CurrentZone = current_zone;
            streaming_position->BeginLoadingPosition = streaming_position->Position;
            streaming_position->BeginLoadingTime = GetDebugRealTime();
            streaming_position->NumSectionsToLoad = 0;
            streaming_position->NumSectionsLoaded = 0;
            streaming_position->AmountToLoad = 0;
            streaming_position->AmountLoaded = 0;
        }

        if (i == 0) {
            GetScenerySectionName(CurrentZoneName, current_zone);
        } else if (current_zone > 0) {
            bSPrintf(CurrentZoneName, "%s - %s", CurrentZoneName, GetScenerySectionName(current_zone));
        }
    }

    int num_sections_unactivated = 0;
    DetermineStreamingSections();
    PostLoadFixupDisabled = true;
    for (int i = 0; i < NumTrackStreamingSections; i++) {
        TrackStreamingSection *section = &pTrackStreamingSections[i];
        if (section->Status == TrackStreamingSection::ACTIVATED && !section->CurrentlyVisible) {
            char section_letter = GetScenerySectionLetter_TrackStreamer(section->SectionNumber);
            if (section_letter != 'Y' && section_letter != 'W' && section_letter != 'X' && section_letter != 'U') {
                num_sections_unactivated += 1;
                UnactivateSection(section);
            }
        }
    }
    PostLoadFixupDisabled = false;

    if (num_sections_unactivated > 0) {
        PostLoadFixup();
        SkipNextHandleLoad = true;
    }

    FreeSectionMemory();
    SetLoadingPhase(ALLOCATING_TEXTURE_SECTIONS);
    NumJettisonedSections = 0;
    CurrentZoneOutOfMemory = false;
    CurrentZoneAllocatedButIncomplete = false;
    MemorySafetyMargin = 0;
    AmountJettisoned = 0;
    bMemSet(JettisonedSections, 0, sizeof(JettisonedSections));
    AssignLoadingPriority();
    CalculateLoadingBacklog();
}

int TrackStreamer::Loader(bChunk *chunk) {
    unsigned int chunk_id = chunk->GetID();
    if (chunk_id == 0x34110) {
        pTrackStreamingSections = reinterpret_cast<TrackStreamingSection *>(chunk->GetData());
        NumTrackStreamingSections = chunk->Size / sizeof(TrackStreamingSection);
        for (int i = 0; i < NumTrackStreamingSections; i++) {
            TrackStreamingSection *section = &pTrackStreamingSections[i];
            bEndianSwap16(&section->SectionNumber);
            bEndianSwap32(&section->Status);
            bEndianSwap32(&section->FileType);
            bEndianSwap32(&section->FileOffset);
            bEndianSwap32(&section->Size);
            bEndianSwap32(&section->CompressedSize);
            bEndianSwap32(&section->PermSize);
            bEndianSwap32(&section->SectionPriority);
            bPlatEndianSwap(&section->Centre);
            bEndianSwap32(&section->Radius);
            bEndianSwap32(&section->Checksum);
        }

        for (int i = 0; i < NumHibernatingSections; i++) {
            TrackStreamingSection *section = FindSection(HibernatingSections[i].SectionNumber);
            bMemCpy(section, &HibernatingSections[i], sizeof(TrackStreamingSection));
            NumSectionsLoaded += 1;
            ActivateSection(section);
            JettisonedSections[NumJettisonedSections] = section;
            NumJettisonedSections += 1;
        }

        NumHibernatingSections = 0;
    } else if (chunk_id == 0x34113) {
        pDiscBundleSections = reinterpret_cast<DiscBundleSection *>(chunk->GetData());
        pLastDiscBundleSection = reinterpret_cast<DiscBundleSection *>(reinterpret_cast<char *>(pDiscBundleSections) + chunk->Size);
        for (DiscBundleSection *disc_bundle = pDiscBundleSections; disc_bundle < pLastDiscBundleSection;
             disc_bundle = reinterpret_cast<DiscBundleSection *>(
                 reinterpret_cast<char *>(disc_bundle) + disc_bundle->NumMembers * sizeof(DiscBundleSectionMember) + 0x14)) {
            bEndianSwap32(&disc_bundle->FileOffset);
            bEndianSwap32(&disc_bundle->FileSize);
            for (int i = 0; i < disc_bundle->NumMembers; i++) {
                DiscBundleSectionMember *member = &disc_bundle->Members[i];
                bEndianSwap16(&member->SectionNumber);
                bEndianSwap16(&member->FileOffset);
                member->pSection = FindSection(member->SectionNumber);
            }
        }
    } else if (chunk_id == 0x34111) {
        pInfo = reinterpret_cast<TrackStreamingInfo *>(chunk->GetData());
        for (int i = 0; i < 2; i++) {
            bEndianSwap32(&pInfo->FileSize[i]);
        }
    } else {
        if (chunk_id != 0x34112) {
            return 0;
        }

        pBarriers = reinterpret_cast<TrackStreamingBarrier *>(chunk->GetData());
        NumBarriers = chunk->Size / sizeof(TrackStreamingBarrier);
        for (int i = 0; i < NumBarriers; i++) {
            TrackStreamingBarrier *barrier = &pBarriers[i];
            bPlatEndianSwap(&barrier->Points[0]);
            bPlatEndianSwap(&barrier->Points[1]);
        }
    }

    return 1;
}

bool TrackStreamer::NeedsGameStateActivation(TrackStreamingSection *section) {
    int subsection_number = section->SectionNumber % 100;
    if (!IsRegularScenerySection_TrackStreamer(section->SectionNumber) || subsection_number < ScenerySectionLODOffset ||
        ScenerySectionLODOffset * 2 <= subsection_number) {
        return false;
    }

    return true;
}

int TrackStreamer::GetSectionToActivate(int loaded_frames) {
    if (NumSectionsActivated < NumCurrentStreamingSections) {
        for (int i = 0; i < NumCurrentStreamingSections; i++) {
            TrackStreamingSection *section = CurrentStreamingSections[i];
            if (section->Status == TrackStreamingSection::LOADED && NeedsGameStateActivation(section) &&
                loaded_frames <= RealTimeFrames - section->LoadedTime) {
                return section->SectionNumber;
            }
        }
    }

    return 0;
}

void TrackStreamer::HandleSectionActivation() {
    short section_number = static_cast<short>(GetSectionToActivate(0));
    if (section_number != 0) {
        TrackStreamingSection *section = FindSection(section_number);
        if (section->Status != TrackStreamingSection::ACTIVATED) {
            if (section->Status != TrackStreamingSection::LOADED) {
                if (!section->CurrentlyVisible) {
                    return;
                }

                do {
                    HandleLoading();
                    ServiceResourceLoading();
                } while (section->Status != TrackStreamingSection::LOADED);
            }
            ActivateSection(section);
        }
    }
}

void TrackStreamer::ActivateSection(TrackStreamingSection *section) {
    NumSectionsActivated += 1;
    AllowDuplicateSolids += 1;
    SetDuplicateTextureWarning(false);

    bChunk *chunks = reinterpret_cast<bChunk *>(section->pMemory);
    int sizeof_chunks = section->LoadedSize;
    LoadTempPermChunks(&chunks, &sizeof_chunks, 0x2087, section->SectionName);

    AllowDuplicateSolids -= 1;
    section->LoadedTime = 0;
    section->pMemory = chunks;
    section->LoadedSize = sizeof_chunks;
    section->Status = TrackStreamingSection::ACTIVATED;
    SetDuplicateTextureWarning(true);
}

TrackStreamingSection *TrackStreamer::ChooseSectionToJettison() {
    TrackStreamingSection *best_section = 0;
    int best_priority = 0;

    last_jettison_print_26154 = RealLoopCounter;
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        short section_number = section->SectionNumber;
        char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
        bool is_y_or_w = section_letter == 'Y' || section_letter == 'W';
        bool is_x_or_u = section_letter == 'X' || section_letter == 'U';
        int priority = 0;

        if (is_y_or_w || is_x_or_u) {
            priority = 2;
            if (section_number == 0x9C4 || section_number == 0x8FC || section_number == 0x960) {
                priority = 1;
            } else if (LoadingPhase == ALLOCATING_TEXTURE_SECTIONS) {
                if (is_y_or_w && section->Status == TrackStreamingSection::ACTIVATED && !SplitScreen) {
                    priority = 10000;
                }
            } else if (LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS) {
                if (is_x_or_u && section->Status == TrackStreamingSection::ACTIVATED && !SplitScreen) {
                    priority = 10000;
                }
            }
        } else if (IsRegularScenerySection_TrackStreamer(section_number)) {
            priority = GetLoadingPriority(section, &StreamingPositionEntries[0], true);
            if (SplitScreen) {
                int player_two_priority = GetLoadingPriority(section, &StreamingPositionEntries[1], true);
                if (player_two_priority < priority) {
                    priority = player_two_priority;
                }
            }
            priority = priority * 10 + 100;
        }

        if (priority != 0 && section->Status != TrackStreamingSection::LOADED && section->Status != TrackStreamingSection::ACTIVATED) {
            priority += 1;
        }
        if (best_priority < priority) {
            best_priority = priority;
            best_section = section;
        }
    }

    return best_section;
}

void TrackStreamer::UnJettisonSections() {
    for (int i = 0; i < NumJettisonedSections; i++) {
        TrackStreamingSection *section = JettisonedSections[i];
        CurrentStreamingSections[NumCurrentStreamingSections] = section;
        NumCurrentStreamingSections += 1;
        section->CurrentlyVisible = true;
    }
    AmountJettisoned = 0;
    NumJettisonedSections = 0;
}

void TrackStreamer::HandleLoading() {
    if (!SkipNextHandleLoad) {
        if (LoadingPhase != LOADING_IDLE) {
            if ((LoadingPhase == LOADING_TEXTURE_SECTIONS || LoadingPhase == LOADING_GEOMETRY_SECTIONS ||
                 LoadingPhase == LOADING_REGULAR_SECTIONS) &&
                (StartLoadingSections(), NumSectionsLoading == 0)) {
                if (!CurrentZoneAllocatedButIncomplete) {
                    if (LoadingPhase == LOADING_REGULAR_SECTIONS) {
                        FinishedLoading();
                        return;
                    }
                    SetLoadingPhase(static_cast<eLoadingPhase>(LoadingPhase + 1));
                } else {
                    SetLoadingPhase(static_cast<eLoadingPhase>(LoadingPhase - 1));
                }
            }

            if ((LoadingPhase == ALLOCATING_TEXTURE_SECTIONS || LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS ||
                 LoadingPhase == ALLOCATING_REGULAR_SECTIONS) &&
                NumSectionsLoading < 1) {
                int num_sections_unactivated = 0;
                for (int i = 0; i < NumTrackStreamingSections; i++) {
                    TrackStreamingSection *section = &pTrackStreamingSections[i];
                    if (section->Status == TrackStreamingSection::ACTIVATED && !section->CurrentlyVisible) {
                        char section_letter = GetScenerySectionLetter_TrackStreamer(section->SectionNumber);
                        if (LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS) {
                            if (section_letter == 'Y' || section_letter == 'W') {
                                num_sections_unactivated += 1;
                                UnactivateSection(section);
                            }
                        } else if (LoadingPhase == ALLOCATING_REGULAR_SECTIONS) {
                            if (section_letter == 'X' || section_letter == 'U') {
                                num_sections_unactivated += 1;
                                UnactivateSection(section);
                            }
                        }
                    }
                }

                if (num_sections_unactivated < 1) {
                    PostLoadFixupDisabled = true;
                    int did_allocate = HandleMemoryAllocation();
                    PostLoadFixupDisabled = false;
                    if (NumSectionsMoved > 0) {
                        PostLoadFixup();
                    }
                    if (did_allocate != 0) {
                        SetLoadingPhase(static_cast<eLoadingPhase>(LoadingPhase + 1));
                        if (LoadingPhase == LOADING_TEXTURE_SECTIONS || LoadingPhase == LOADING_GEOMETRY_SECTIONS) {
                            UnJettisonSections();
                        }
                        HandleLoading();
                    }
                } else {
                    SkipNextHandleLoad = true;
                }
            }
        }
    } else {
        SkipNextHandleLoad = false;
    }
}

int TrackStreamer::GetLoadingPriority(TrackStreamingSection *section, StreamingPositionEntry *streaming_position, bool use_direction) {
    if (!section->pBoundary) {
        return 0;
    }

    float speed = bLength(&streaming_position->Velocity);
    if (use_direction) {
        speed = 100.0f;
    }

    if (speed < 1.0f) {
        return 0;
    }

    bVector2 lookahead(streaming_position->Position.x + streaming_position->Velocity.x,
                       streaming_position->Position.y + streaming_position->Velocity.y);
    float distance = section->pBoundary->GetDistanceOutside(&lookahead, 999.0f);

    bVector2 heading;
    if (use_direction) {
        bNormalize(&heading, &streaming_position->Direction);
    } else {
        bNormalize(&heading, &streaming_position->Velocity);
    }

    bVector2 to_section(section->Centre.x - lookahead.x, section->Centre.y - lookahead.y);
    bVector2 section_direction;
    bNormalize(&section_direction, &to_section);

    float speed_scale = speed * 0.016666668f;
    if (speed_scale > 1.0f) {
        speed_scale = 1.0f;
    }

    unsigned short angle = bASin(bDot(&section_direction, &heading));
    float degrees = static_cast<unsigned short>(0x4000 - angle) * 0.005493164f;
    if (degrees < 20.0f) {
        degrees = 20.0f;
    }
    if (degrees > 90.0f) {
        degrees = 90.0f;
    }

    int loading_priority =
        static_cast<int>(distance * (1.0f - (90.0f - degrees) * 0.014285714f * speed_scale * 0.66999996f) * 0.013333334f);
    if (loading_priority < 0) {
        loading_priority = 0;
    }
    if (loading_priority > 2) {
        loading_priority = 2;
    }

    return loading_priority;
}

void TrackStreamer::AssignLoadingPriority() {
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        int loading_priority = 99;
        for (unsigned int position_number = 0; position_number < 2; position_number++) {
            if (((section->CurrentlyVisible >> (position_number & 0x1f)) & 1U) != 0) {
                int position_priority = GetLoadingPriority(section, &StreamingPositionEntries[position_number], false);
                if (position_priority < loading_priority) {
                    loading_priority = position_priority;
                }
            }
        }

        section->LoadingPriority = loading_priority * 100000 + section->SectionPriority;
    }
}

void TrackStreamer::CalculateLoadingBacklog() {
    float loading_backlog = 0.0f;
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        if (section->CurrentlyVisible && section->Status != TrackStreamingSection::LOADED &&
            section->Status != TrackStreamingSection::ACTIVATED) {
            int rounded_size = section->Size;
            if (rounded_size < 0) {
                rounded_size += 0x3ff;
            }

            float section_backlog = static_cast<float>(rounded_size >> 10) * 0.0004f + 0.2f;
            if (section->BaseLoadingPriority == 1) {
                section_backlog *= 0.4f;
            }
            if (section->BaseLoadingPriority == 2) {
                section_backlog *= 0.2f;
            }
            loading_backlog += section_backlog;
        }
    }

    LoadingBacklog = loading_backlog;
}

bool TrackStreamer::AreAllSectionsActivated() {
    bool all_sections_activated = false;
    if (LoadingPhase == LOADING_IDLE) {
        all_sections_activated = NumCurrentStreamingSections <= NumSectionsActivated + NumSectionsOutOfMemory;
    }
    return all_sections_activated;
}

bool TrackStreamer::IsLoadingInProgress() {
    bool loading_in_progress = !AreAllSectionsActivated();

    if (!loading_in_progress && !AreAllSectionsActivated()) {
        while (!AreAllSectionsActivated()) {
            ServiceResourceLoading();
            ServiceNonGameState();
        }
    }

    return loading_in_progress;
}

void TrackStreamer::CheckLoadingBar() {
    float minimum_distance = kMaxDistance_TrackStreamer;

    for (int i = 0; i < 2; i++) {
        StreamingPositionEntry *entry = &StreamingPositionEntries[i];
        if (!IsLoadingInProgress()) {
            break;
        }

        bool far_load = false;
        if (CurrentZoneFarLoad && IsLoadingInProgress()) {
            far_load = true;
        }

        if (far_load || !entry->PositionSet || !entry->FollowingCar) {
            break;
        }

        float velocity_squared = entry->Velocity.x * entry->Velocity.x + entry->Velocity.y * entry->Velocity.y;
        float velocity_magnitude = 0.0f;
        if (velocity_squared > kVelocityEpsilon_TrackStreamer) {
            velocity_magnitude = bSqrt(velocity_squared);
        }

        if (velocity_magnitude > kLoadingBarSpeedThreshold_TrackStreamer) {
            break;
        }

        for (int n = 0; n < NumCurrentStreamingSections; n++) {
            TrackStreamingSection *section = CurrentStreamingSections[n];
            if (!section->pBoundary || section->Status == TrackStreamingSection::ACTIVATED ||
                !IsLoadingBarSection_TrackStreamer(section->SectionNumber)) {
                continue;
            }

            bVector2 future_position;
            future_position.x = entry->Position.x + entry->Velocity.x * kFuturePositionScale_TrackStreamer;
            future_position.y = entry->Position.y + entry->Velocity.y * kFuturePositionScale_TrackStreamer;

            float current_distance = section->pBoundary->GetDistanceOutside(&entry->Position, kMaxDistance_TrackStreamer);
            float future_distance = section->pBoundary->GetDistanceOutside(&future_position, kMaxDistance_TrackStreamer);
            float predicted_distance =
                current_distance - (current_distance - future_distance) * kPredictionScaleA_TrackStreamer * kPredictionScaleB_TrackStreamer;
            if (predicted_distance < minimum_distance) {
                minimum_distance = predicted_distance;
            }
        }
    }

    prev_need_loading_bar_26275 = minimum_distance < kLoadingBarDistanceThreshold_TrackStreamer;
}

int TrackStreamer::GetPredictedZone(StreamingPositionEntry *streaming_position) {
    float speed = bSqrt(streaming_position->Velocity.x * streaming_position->Velocity.x +
                        streaming_position->Velocity.y * streaming_position->Velocity.y);
    bool found_predicted_zone = false;
    short predicted_zone = 0;
    TrackPathZone *track_path_zone = 0;
    bVector2 predicted_position;

    do {
        track_path_zone =
            TheTrackPathManager.FindZone(&streaming_position->Position, TRACK_PATH_ZONE_STREAMER_PREDICTION, track_path_zone);
        if (!track_path_zone) {
            break;
        }

        float zone_elevation = track_path_zone->Elevation;
        if (((zone_elevation <= 0.0f) || (zone_elevation <= streaming_position->Elevation)) &&
            ((zone_elevation >= 0.0f) || (streaming_position->Elevation <= -zone_elevation))) {
            if (speed > kPredictedZoneStopProjectSpeed_TrackStreamer) {
                predicted_position = streaming_position->Position;
            } else if (speed * kPredictedZoneScale_TrackStreamer <= kPredictedZoneMaxDistance_TrackStreamer) {
                bScaleAdd(&predicted_position, &streaming_position->Position, &streaming_position->Velocity,
                          kPredictedZoneScale_TrackStreamer);
            } else {
                bScaleAdd(&predicted_position, &streaming_position->Position, &streaming_position->Velocity,
                          kPredictedZoneMaxDistance_TrackStreamer / speed);
            }

            DrivableScenerySection *drivable_section = TheVisibleSectionManager.FindDrivableSection(&predicted_position);
            if (drivable_section && track_path_zone->Data[0] != 0) {
                short section_number = drivable_section->SectionNumber;
                for (int i = 0; i < 4; i++) {
                    if (track_path_zone->Data[i] == section_number) {
                        found_predicted_zone = true;
                        predicted_zone = section_number;
                        break;
                    }
                    if (track_path_zone->Data[i] == 0) {
                        break;
                    }
                }
            }
        }
    } while (!found_predicted_zone);

    if (found_predicted_zone) {
        if (!bEqual(&predicted_position, &streaming_position->Position, kPredictedZoneEqualEpsilon_TrackStreamer)) {
            for (int i = 0; i < NumBarriers; i++) {
                if (DoLinesIntersect(pBarriers[i].Points[0], pBarriers[i].Points[1], streaming_position->Position,
                                     predicted_position)) {
                    found_predicted_zone = false;
                    predicted_zone = 0;
                }
            }
        }

        if (found_predicted_zone) {
            return predicted_zone;
        }
    }

    DrivableScenerySection *drivable_section = TheVisibleSectionManager.FindDrivableSection(&streaming_position->Position);
    if (drivable_section) {
        predicted_zone = drivable_section->SectionNumber;
    }
    return predicted_zone;
}
