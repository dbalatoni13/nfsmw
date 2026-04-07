#include "TrackStreamer.hpp"

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "TrackPath.hpp"
#include "VisibleSection.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/Espresso.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <algorithm>

// TODO move
extern BOOL bMemoryTracing;
extern int SeeulatorToolActive;
extern int ScenerySectionToBlink;
extern bool PostLoadFixupDisabled;
extern int ForceHoleFillerMethod;
extern int ShowSectionBoarder;
void NotifySkyLoader();

int TrackStreamerRemoteCaffeinating = 0;

int LoaderTrackStreamer(bChunk *chunk);
int UnloaderTrackStreamer(bChunk *chunk);

unsigned char CurrentVisibleSectionTableMem[350];
TrackStreamer TheTrackStreamer;
bChunkLoader bChunkLoaderTrackStreamingSection(BCHUNK_TRACK_STRAMING_SECTIONS, LoaderTrackStreamer, UnloaderTrackStreamer);
bChunkLoader bChunkLoaderTrackStreamingDiscBundle(BCHUNK_TRACK_STRAMING_DISC_BUNDLE, LoaderTrackStreamer, UnloaderTrackStreamer);
bChunkLoader bChunkLoaderTrackStreamingInfo(BCHUNK_TRACK_STRAMING_INFOS, LoaderTrackStreamer, UnloaderTrackStreamer);
bChunkLoader bChunkLoaderTrackStreamingBarriers(BCHUNK_TRACK_STRAMING_BARRIERS, LoaderTrackStreamer, UnloaderTrackStreamer);

unsigned int PlotLoadingMarker(const char *layer_name, const bVector2 *begin_position, const bVector2 *end_position, float load_time) {}

TSMemoryPool::TSMemoryPool(intptr_t address, int size, const char *debug_name, int pool_num) {
    PoolNum = pool_num;
    DebugName = debug_name;
    TotalSize = size;
    TracingEnabled = true;
    AllocationNumber = 0;
    Updated = false;
    AmountFree = size;
    LargestFree = size;
    NeedToRecalcLargestFree = false;

    for (int n = 0; n < 192; n++) {
        TSMemoryNode *node = &MemoryNodes[n];
        UnusedNodeList.AddTail(node);
    }

    if (TracingEnabled && bMemoryTracing) {
        bMemoryTraceNewPoolPacket packet;
        packet.PoolID = reinterpret_cast<intptr_t>(this);
        bMemSet(packet.Name, 0, sizeof(packet.Name));
        bStrNCpy(packet.Name, debug_name, sizeof(packet.Name) - 1);
        bFunkCallASync("CODEINE", 0x19, &packet, sizeof(packet));
    }

    TSMemoryNode *node = GetNewNode(address, size, false, nullptr);
    NodeList.AddTail(node);

    if (TracingEnabled && bMemoryTracing) {
        bMemoryTraceFreePacket packet = {};
        packet.PoolID = reinterpret_cast<intptr_t>(this);
        packet.MemoryAddress = static_cast<intptr_t>(address);
        packet.Size = size;
        bFunkCallASync("CODEINE", 0x1b, &packet, sizeof(packet));
    }

    MemoryPoolOverrideInfo *override_info = &OverrideInfo;
    bMemSet(&OverrideInfo, 0, sizeof(OverrideInfo));
#ifndef EA_BUILD_A124
    OverrideInfo.Name = DebugName;
#endif
    OverrideInfo.Pool = this;
    OverrideInfo.Address = address;
    OverrideInfo.Size = size;
    OverrideInfo.Malloc = OverrideMalloc;
    OverrideInfo.Free = OverrideFree;
    OverrideInfo.GetAmountFree = OverrideGetAmountFree;
    OverrideInfo.GetLargestFreeBlock = OverrideGetLargestFreeBlock;
    bSetMemoryPoolOverrideInfo(PoolNum, &OverrideInfo);
}

TSMemoryNode *TSMemoryPool::GetNewNode(intptr_t address, int size, bool allocated, const char *debug_name) {
    TSMemoryNode *node = UnusedNodeList.RemoveHead();

    node->Address = address;
    node->Size = size;
    node->Allocated = allocated;
    bSafeStrCpy(node->DebugName, debug_name, sizeof(node->DebugName));
    return node;
}

void TSMemoryPool::RemoveNode(TSMemoryNode *node) {
    UnusedNodeList.AddTail(node);
}

void *TSMemoryPool::Malloc(int size, const char *debug_name, bool best_fit, bool allocate_from_top, intptr_t address) {
    TSMemoryNode *found_node = nullptr;

    size = (size + 0x7f) & ~0x7f;

    if (address != 0) {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (node->IsFree() && node->Size >= size && node->Contains(address)) {
                found_node = node;
                break;
            }
        }
    } else if (best_fit) {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (node->IsFree() && node->Size >= size && (!found_node || found_node->Size - size > node->Size - size)) {
                found_node = node;
            }
        }
    } else if (allocate_from_top) {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (node->IsFree() && node->Size >= size) {
                found_node = node;
                break;
            }
        }
    } else {
        for (TSMemoryNode *node = NodeList.GetTail(); node != NodeList.EndOfList(); node = node->GetPrev()) {
            if (node->IsFree() && node->Size >= size) {
                found_node = node;
                break;
            }
        }
    }

    if (!found_node) {
        return nullptr;
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

    TSMemoryNode *new_node = GetNewNode(address, size, true, debug_name);
    new_node->AddAfter(found_node);

    int new_bottom_size = address - found_node->Address;
    int new_top_size = found_node->Address + found_node->Size - (address + size);
    found_node->Size = new_bottom_size;
    if (new_bottom_size == 0) {
        NodeList.Remove(found_node);
        RemoveNode(found_node);
    }

    if (new_top_size != 0) {
        TSMemoryNode *top_node = GetNewNode(address + size, new_top_size, false, 0);
        top_node->AddAfter(new_node);
    }

    if (TracingEnabled && bMemoryTracing) {
        bMemoryTraceAllocatePacket packet = {reinterpret_cast<intptr_t>(this), address, size, 0, AllocationNumber};

        bMemSet(packet.DebugText, 0, sizeof(packet.DebugText));
        if (debug_name) {
            bStrNCpy(packet.DebugText, debug_name, sizeof(packet.DebugText) - 1);
        }
        int extra_len = bStrLen(packet.DebugText) + 0x15;
        bFunkCallASync("CODEINE", 0x1c, &packet, extra_len);
    }

    for (int n = 0; n < 0; n++) {
        unsigned char *p;
    }

    Updated = true;
    AllocationNumber++;
    return reinterpret_cast<void *>(address);
}

void TSMemoryPool::Free(void *ptr) {
    intptr_t address = reinterpret_cast<intptr_t>(ptr);
    Updated = true;

    for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
        if (node->Address == address) {
            int size;
            TSMemoryNode *prev_node = node->GetPrev();

            size = node->Size;
            node->Allocated = false;
            node->DebugName[0] = 0;
            if (prev_node != NodeList.EndOfList() && prev_node->IsFree()) {
                node->Address = address - prev_node->Size;
                node->Size = size + prev_node->Size;
                NodeList.Remove(prev_node);
                RemoveNode(prev_node);
            }

            TSMemoryNode *next_node = node->GetNext();
            if (next_node != NodeList.EndOfList() && next_node->IsFree()) {
                node->Size += next_node->Size;
                NodeList.Remove(next_node);
                RemoveNode(next_node);
            }

            AmountFree += size;
            if (node->Size > LargestFree) {
                LargestFree = node->Size;
            }

            if (TracingEnabled && bMemoryTracing) {
                bMemoryTraceFreePacket packet = {};
                packet.PoolID = reinterpret_cast<intptr_t>(this);
                packet.MemoryAddress = reinterpret_cast<intptr_t>(ptr);
                packet.Size = size;
                bFunkCallASync("CODEINE", 0x1b, &packet, sizeof(packet));
            }
            return;
        }
    }
}

int TSMemoryPool::GetAmountFree() {
    for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
        node->IsFree();
    }
    int amount_free = AmountFree;
    return amount_free;
}

int TSMemoryPool::GetLargestFreeBlock() {
    if (NeedToRecalcLargestFree) {
        int largest_free = 0;
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (node->IsFree() && node->Size > largest_free) {
                largest_free = node->Size;
            }
        }
        LargestFree = largest_free;
        NeedToRecalcLargestFree = false;
    }

    int largest_free = 0;
    for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
        if (node->IsFree() && node->Size > largest_free) {
            largest_free = node->Size;
        }
    }
    return LargestFree;
}

TSMemoryNode *TSMemoryPool::GetNextNode(bool start_from_top, TSMemoryNode *node) {
    if (start_from_top) {
        if (node) {
            node = node->GetNext();
        } else {
            node = NodeList.GetHead();
        }
    } else {
        if (node) {
            node = node->GetPrev();
        } else {
            node = NodeList.GetTail();
        }
    }

    if (node == NodeList.EndOfList()) {
        return nullptr;
    }
    return node;
}

TSMemoryNode *TSMemoryPool::GetNextFreeNode(bool start_from_top, TSMemoryNode *node) {
    TSMemoryNode *next_node = node;
    while (next_node = GetNextNode(start_from_top, next_node), next_node) {
        if (next_node->IsFree()) {
            return next_node;
        }
    }
    // return nullptr; // TODO put behind SANE_CODE macro
}

TSMemoryNode *TSMemoryPool::GetNextAllocatedNode(bool start_from_top, TSMemoryNode *node) {
    TSMemoryNode *next_node = node;
    while (next_node = GetNextNode(start_from_top, next_node), next_node) {
        if (next_node->IsAllocated()) {
            return next_node;
        }
    }
    // return nullptr; // TODO put behind SANE_CODE macro
}

unsigned int TSMemoryPool::GetPoolChecksum() {
    return 0;
}

void TSMemoryPool::DebugPrint() {
    for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
        const char *name = node->DebugName;
        node->IsAllocated();
    }
}

int LoaderTrackStreamer(bChunk *chunk) {
    return TheTrackStreamer.Loader(chunk);
}

int UnloaderTrackStreamer(bChunk *chunk) {
    return TheTrackStreamer.Unloader(chunk);
}

void RefreshTrackStreamer() {
    TheTrackStreamer.RefreshLoading();
}

// UNSOLVED, ClearTable shouldn't inline
TrackStreamer::TrackStreamer() {
    pTrackStreamingSections = nullptr;
    NumTrackStreamingSections = 0;
    pDiscBundleSections = nullptr;
    pLastDiscBundleSection = nullptr;
    NumSectionsLoaded = 0;
    NumSectionsLoading = 0;
    NumSectionsActivated = 0;
    NumSectionsOutOfMemory = 0;
    NumSectionsMoved = 0;
    bMemSet(StreamFilenames, 0, sizeof(StreamFilenames));
    SplitScreen = false;
    PermFileLoading = false;
    PermFilename = nullptr;
    PermFileChunks = nullptr;
    PermFileSize = 0;
    NumBarriers = 0;
    pBarriers = nullptr;
    NumCurrentStreamingSections = 0;
    NumHibernatingSections = 0;
    CurrentZoneNeedsRefreshing = false;
    ZoneSwitchingDisabled = false;
    LastWaitUntilRenderingDoneFrameCount = 0;
    LastPrintedFrameCount = 0;
    SkipNextHandleLoad = false;

    ClearCurrentZones();
    ClearStreamingPositions();

    pMemoryPoolMem = nullptr;
    MemoryPoolSize = 0;
    UserMemoryAllocationSize = 0;
    pMemoryPool = nullptr;

    CurrentVisibleSectionTable.Init(CurrentVisibleSectionTableMem, 0xAF0);
    // TODO why doesn't this inline?
    CurrentVisibleSectionTable.ClearTable();
    bMemSet(KeepSectionTable, 0, sizeof(KeepSectionTable));
    pCallback = nullptr;
    CallbackParam = 0;
    MakeSpaceInPoolCallback = nullptr;
    MakeSpaceInPoolCallbackParam = 0;
    MakeSpaceInPoolSize = 0;
}

int TrackStreamer::Loader(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_TRACK_STRAMING_SECTIONS) {
        int n;
        pTrackStreamingSections = reinterpret_cast<TrackStreamingSection *>(chunk->GetData());
        NumTrackStreamingSections = chunk->GetSize() / sizeof(TrackStreamingSection);
        for (n = 0; n < NumTrackStreamingSections; n++) {
            TrackStreamingSection *section = &pTrackStreamingSections[n];
            bPlatEndianSwap(&section->SectionNumber);
            bPlatEndianSwap(reinterpret_cast<int *>(&section->FileType));
            bPlatEndianSwap(reinterpret_cast<int *>(&section->Status));
            bPlatEndianSwap(&section->FileOffset);
            bPlatEndianSwap(&section->Size);
            bPlatEndianSwap(&section->CompressedSize);
            bPlatEndianSwap(&section->PermSize);
            bPlatEndianSwap(&section->SectionPriority);
            bPlatEndianSwap(&section->Centre);
            bPlatEndianSwap(&section->Radius);
            bPlatEndianSwap(&section->Checksum);
        }

        for (n = 0; n < NumHibernatingSections; n++) {
            TrackStreamingSection *hibernating_section = &HibernatingSections[n];
            TrackStreamingSection *section = FindSection(hibernating_section->SectionNumber);
            bMemCpy(section, hibernating_section, sizeof(TrackStreamingSection));
            NumSectionsLoaded++;
            ActivateSection(section);
            CurrentStreamingSections[NumCurrentStreamingSections++] = section;
        }

        NumHibernatingSections = 0;
        return 1;
    } else if (chunk->GetID() == BCHUNK_TRACK_STRAMING_DISC_BUNDLE) {
        DiscBundleSection *disc_bundle;
        pDiscBundleSections = reinterpret_cast<DiscBundleSection *>(chunk->GetData());
        pLastDiscBundleSection = reinterpret_cast<DiscBundleSection *>(reinterpret_cast<char *>(chunk->GetData()) + chunk->GetSize());
        for (disc_bundle = pDiscBundleSections; disc_bundle < pLastDiscBundleSection; disc_bundle = disc_bundle->GetMemoryImageNext()) {
            bPlatEndianSwap(&disc_bundle->FileOffset);
            bPlatEndianSwap(&disc_bundle->FileSize);
            bPlatEndianSwap(&disc_bundle->NumMembers);
            for (int i = 0; i < disc_bundle->NumMembers; i++) {
                DiscBundleSectionMember *member = &disc_bundle->Members[i];
                bPlatEndianSwap(&member->SectionNumber);
                bPlatEndianSwap(&member->FileOffset);
                TrackStreamingSection *section = FindSection(member->SectionNumber);
                member->pSection = section;
            }
        }
        return 1;
    } else if (chunk->GetID() == BCHUNK_TRACK_STRAMING_INFOS) {
        pInfo = reinterpret_cast<TrackStreamingInfo *>(chunk->GetData());
        for (int n = 0; n < 2; n++) {
            bPlatEndianSwap(n + pInfo->FileSize);
        }
        return 1;
    } else if (chunk->GetID() == BCHUNK_TRACK_STRAMING_BARRIERS) {
        pBarriers = reinterpret_cast<TrackStreamingBarrier *>(chunk->GetData());
        NumBarriers = chunk->GetSize() / sizeof(TrackStreamingBarrier);
        for (int n = 0; n < NumBarriers; n++) {
            TrackStreamingBarrier *barrier = &pBarriers[n];
            barrier->EndianSwap();
        }
        return 1;
    } else {
        return 0;
    }
}

int TrackStreamer::Unloader(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_TRACK_STRAMING_SECTIONS) {
        UnloadEverything();
        pTrackStreamingSections = nullptr;
        NumTrackStreamingSections = 0;
        return 1;
    }

    if (chunk->GetID() == BCHUNK_TRACK_STRAMING_DISC_BUNDLE) {
        pDiscBundleSections = nullptr;
        pLastDiscBundleSection = nullptr;
        return 1;
    }

    if (chunk->GetID() == BCHUNK_TRACK_STRAMING_INFOS) {
        pInfo = nullptr;
        return 1;
    }

    if (chunk->GetID() == BCHUNK_TRACK_STRAMING_BARRIERS) {
        pBarriers = nullptr;
        NumBarriers = 0;
        return 1;
    }

    return 0;
}

void TrackStreamer::ClearCurrentZones() {
    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        position_entry->CurrentZone = 0;
        position_entry->BeginLoadingTime = 0.0f;
        position_entry->BeginLoadingPosition.x = 0.0f;
        position_entry->BeginLoadingPosition.y = 0.0f;
        position_entry->NumSectionsToLoad = 0;
        position_entry->NumSectionsLoaded = 0;
        position_entry->AmountToLoad = 0;
        position_entry->AmountLoaded = 0;
    }

    LoadingPhase = LOADING_IDLE;
    LoadingBacklog = 0.0f;
    CurrentZoneOutOfMemory = false;
    CurrentZoneAllocatedButIncomplete = false;
    CurrentZoneNonReplayLoad = false;
    CurrentZoneFarLoad = true;
    StartLoadingTime = 0.0f;
    CurrentZoneName[0] = 0;
    MemorySafetyMargin = 0;
    AmountJettisoned = 0;
    NumJettisonedSections = 0;
    bMemSet(JettisonedSections, 0, sizeof(JettisonedSections));
    RemoveCurrentStreamingSections();
}

void TrackStreamer::InitMemoryPool(int size) {
    MemoryPoolSize = size;
    pMemoryPoolMem = bMalloc(size, "Track Streaming Buffer", 0, 0x2000);
    pMemoryPool = new ("TSMemoryPool", 0) TSMemoryPool(reinterpret_cast<intptr_t>(pMemoryPoolMem), MemoryPoolSize, "Track Streaming", 7);
}

int TrackStreamer::GetMemoryPoolSize() {
    if (pMemoryPool->IsUpdated()) {
        UserMemoryAllocationSize = CountUserAllocations(nullptr);
    }
    return MemoryPoolSize - UserMemoryAllocationSize;
}

int TrackStreamer::CountUserAllocations(const char **pfragmented_user_allocation) {
    if (pfragmented_user_allocation) {
        *pfragmented_user_allocation = nullptr;
    }

    int num_fragmented_user_allocations = 0;
    int user_allocation_size = 0;
    bool start_from_top = false;
    TSMemoryNode *node = pMemoryPool->GetFirstAllocatedNode(start_from_top);
    while (node) {
        TrackStreamingSection *section = FindSectionByAddress(node->Address);
        if (!section) {
            user_allocation_size += node->Size;
            if (pMemoryPool->GetNextFreeNode(start_from_top, node) && pMemoryPool->GetNextFreeNode(!start_from_top, node) &&
                pfragmented_user_allocation) {
                *pfragmented_user_allocation = node->DebugName;
                num_fragmented_user_allocations++;
            }
        }

        node = pMemoryPool->GetNextAllocatedNode(start_from_top, node);
    }

    return user_allocation_size;
}

TrackStreamingSection *TrackStreamer::FindSection(int section_number) {
    for (int n = 0; n < NumTrackStreamingSections; n++) {
        TrackStreamingSection *section = &pTrackStreamingSections[n];
        if (section->SectionNumber == static_cast<short>(section_number)) {
            return section;
        }
    }
    return nullptr;
}

TrackStreamingSection *TrackStreamer::FindSectionByAddress(intptr_t address) {
    int n;
    for (n = 0; n < NumCurrentStreamingSections; n++) {
        TrackStreamingSection *section = CurrentStreamingSections[n];
        if (section->pMemory == reinterpret_cast<void *>(address)) {
            return section;
        }
    }

    for (n = 0; n < NumTrackStreamingSections; n++) {
        TrackStreamingSection *section = &pTrackStreamingSections[n];
        if (section->pMemory == reinterpret_cast<void *>(address)) {
            return section;
        }
    }

    return nullptr;
}

int TrackStreamer::GetCombinedSectionNumber(int section_number) {
    if (IsScenerySectionDrivable(section_number)) {
        int lod_section_number = GetLODScenerySectionNumber(section_number);
        if (!FindSection(section_number)) {
            if (FindSection(lod_section_number)) {
                return lod_section_number;
            }
        }
    }

    return section_number;
}

void TrackStreamer::InitRegion(const char *region_stream_filename, bool split_screen) {
    bool flush_hibernating_sections = false;

    if (SplitScreen != split_screen) {
        SplitScreen = split_screen;
        flush_hibernating_sections = true;
    }
    if (!bStrEqual(StreamFilenames[1], region_stream_filename)) {
        flush_hibernating_sections = true;
        bStrCpy(StreamFilenames[1], region_stream_filename);
    }
    if (flush_hibernating_sections) {
        FlushHibernatingSections();
    }
    if (PermFileLoading) {
        BlockWhileQueuedFileBusy();
    }

    ClearCurrentZones();
    ClearStreamingPositions();

    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];

        position_entry->PredictedZone = 0;
        position_entry->PredictedZoneValidTime = 0;
        position_entry->AudioReading = false;
        position_entry->AudioReadingTime = 0.0f;
        position_entry->AudioReadingPosition.x = 0.0f;
        position_entry->AudioReadingPosition.y = 0.0f;
        position_entry->AudioBlocking = false;
        position_entry->AudioBlockingTime = 0.0f;
        position_entry->AudioBlockingPosition.x = 0.0f;
        position_entry->AudioBlockingPosition.y = 0.0f;
    }

    for (int n = 0; n < NumTrackStreamingSections; n++) {
        TrackStreamingSection *section = &pTrackStreamingSections[n];
        int boundary_section_number = GetBoundarySectionNumber(section->SectionNumber, bGetPlatformName());
        VisibleSectionBoundary *boundary = TheVisibleSectionManager.FindBoundary(boundary_section_number);
#ifndef EA_BUILD_A124
        section->pBoundary = boundary;
#endif
    }

    EmptyCaffeineLayers();
}

// UNSOLVED because it's empty and the stack is too small
void TrackStreamer::HibernateStreamingSections() {
    int sections_to_hibernate[5];
    // I made these up
    GetScenerySectionNumber('A', 0);
    GetScenerySectionNumber('B', 1);
    GetScenerySectionNumber('C', 2);
    GetScenerySectionNumber('D', 3);
    GetScenerySectionNumber('E', 4);

    for (int n = 0; n < 5 && false; n++) {
        int section_number = sections_to_hibernate[n];
        {
            TrackStreamingSection *section;
            {
                TrackStreamingSection *hibernating_section;
            }
        }
    }

    return;
}

void TrackStreamer::FlushHibernatingSections() {
    for (int n = 0; n < NumHibernatingSections; n++) {
        TrackStreamingSection *section = &HibernatingSections[n];
        bFree(section->pMemory);
    }
    NumHibernatingSections = 0;
}

void *TrackStreamer::AllocateMemory(TrackStreamingSection *section, int allocation_params) {
    void *buf = bMalloc(section->Size, section->SectionName, 0, allocation_params | 0x2007);
    if (!buf) {
        bBreak();
    }
    return buf;
}

void TrackStreamer::LoadDiscBundle(DiscBundleSection *disc_bundle) {
    void *memory = nullptr;
    for (int i = 0; i < disc_bundle->NumMembers; i++) {
        DiscBundleSectionMember *member = &disc_bundle->Members[i];
        TrackStreamingSection *section = member->pSection;
        if (i == 0) {
            memory = section->pMemory;
        }
        section->Status = TrackStreamingSection::LOADING;
    }

    NumSectionsLoading++;
    AddQueuedFile(memory, StreamFilenames[1], disc_bundle->FileOffset, disc_bundle->FileSize, DiscBundleLoadedCallback,
                  reinterpret_cast<intptr_t>(disc_bundle), nullptr);
}

void TrackStreamer::DiscBundleLoadedCallback(intptr_t param, int error_status) {
    TheTrackStreamer.DiscBundleLoadedCallback(reinterpret_cast<DiscBundleSection *>(param));
}

void TrackStreamer::DiscBundleLoadedCallback(DiscBundleSection *disc_bundle) {
    NumSectionsLoading += disc_bundle->NumMembers - 1;
    for (int i = 0; i < disc_bundle->NumMembers; i++) {
        DiscBundleSectionMember *member = &disc_bundle->Members[i];
        TrackStreamingSection *section = member->pSection;
        section->pDiscBundle = nullptr;
        SectionLoadedCallback(section);
    }
}

void TrackStreamer::LoadSection(TrackStreamingSection *section) {
    NumSectionsLoading++;
    section->Status = TrackStreamingSection::LOADING;

    if (section->CompressedSize == section->Size) {
        AddQueuedFile(section->pMemory, StreamFilenames[section->FileType], section->FileOffset, section->CompressedSize, SectionLoadedCallback,
                      reinterpret_cast<intptr_t>(section), nullptr);
    } else {
        QueuedFileParams params;
        params.Compressed = true;
        params.UncompressedSize = section->Size;
        AddQueuedFile(section->pMemory, StreamFilenames[section->FileType], section->FileOffset, section->CompressedSize, SectionLoadedCallback,
                      reinterpret_cast<intptr_t>(section), &params);
    }
}

void TrackStreamer::ActivateSection(TrackStreamingSection *section) {
    ProfileNode profile_node("TODO", 0);
    int allocation_params = 0x2087;
    NumSectionsActivated++;
    eAllowDuplicateSolids(true);
    SetDuplicateTextureWarning(false);

    bChunk *chunks = reinterpret_cast<bChunk *>(section->pMemory);
    int sizeof_chunks = section->LoadedSize;
    LoadTempPermChunks(&chunks, &sizeof_chunks, allocation_params, section->SectionName);

    section->pMemory = chunks;
    section->LoadedSize = sizeof_chunks;
    section->Status = TrackStreamingSection::ACTIVATED;
    section->LoadedTime = 0;
    eAllowDuplicateSolids(false);
    SetDuplicateTextureWarning(true);
}

void TrackStreamer::UnactivateSection(TrackStreamingSection *section) {
    ProfileNode profile_node("TODO", 0);
    section->UnactivatedFrameCount = 0;
    DisableWaitUntilRenderingDone();
    section->UnactivatedFrameCount = eGetFrameCounter();
    UnloadChunks(reinterpret_cast<bChunk *>(section->pMemory), section->LoadedSize, section->SectionName);
    EnableWaitUntilRenderingDone();
    NumSectionsActivated--;
    section->Status = TrackStreamingSection::LOADED;
}

bool TrackStreamer::WillUnloadBlock(TrackStreamingSection *section) {
    if ((section->UnactivatedFrameCount != 0) && (section->UnactivatedFrameCount == eGetFrameCounter())) {
        if (LastWaitUntilRenderingDoneFrameCount != section->UnactivatedFrameCount) {
            return true;
        }
    }
    return false;
}

void TrackStreamer::UnloadSection(TrackStreamingSection *section) {
    ProfileNode profile_node("TODO", 0);
    if (section->Status == TrackStreamingSection::ACTIVATED) {
        UnactivateSection(section);
    }

    if (section->Status == TrackStreamingSection::LOADED) {
        if (WillUnloadBlock(section)) {
            DisableWaitForFrameBufferSwap();
            eWaitUntilRenderingDone();
            EnableWaitForFrameBufferSwap();
            LastWaitUntilRenderingDoneFrameCount = eGetFrameCounter();
        }

        section->UnactivatedFrameCount = 0;
        bFree(section->pMemory);
        section->pMemory = nullptr;
        section->Status = TrackStreamingSection::UNLOADED;
        section->LoadedTime = 0;
        NumSectionsLoaded--;
    }
}

bool TrackStreamer::NeedsGameStateActivation(TrackStreamingSection *section) {
    return false;

    if (IsRegularScenerySection(section->SectionNumber) && IsLODScenerySectionNumber(section->SectionNumber)) {
        return true;
    }
}

void TrackStreamer::SectionLoadedCallback(intptr_t param, int error_status) {
    TrackStreamingSection *section = reinterpret_cast<TrackStreamingSection *>(param);

    TheTrackStreamer.SectionLoadedCallback(section);
}

void TrackStreamer::SectionLoadedCallback(TrackStreamingSection *section) {
    section->Status = TrackStreamingSection::LOADED;
    section->LoadedSize = section->Size;
    EndianSwapChunkHeadersRecursive(reinterpret_cast<bChunk *>(section->pMemory), section->Size);
    NumSectionsLoading--;
    NumSectionsLoaded++;
    section->LoadedTime = RealTimeFrames;

    if (section->CurrentlyVisible && !NeedsGameStateActivation(section)) {
        ActivateSection(section);
    }

    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        if ((section->CurrentlyVisible >> position_number) & 1) {
            StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
            position_entry->NumSectionsLoaded++;
            position_entry->AmountLoaded += section->Size;
        }
    }

    CalculateLoadingBacklog();
#ifdef EA_PLATFORM_GAMECUBE
    DCStoreRange(section->pMemory, section->LoadedSize);
#endif
}

void TrackStreamer::EmptyCaffeineLayers() {
    TrackStreamerRemoteCaffeinating = 0;

    espEmptyLayer("TrackStreamingTrail");
    espEmptyLayer("TrackStreamingPredict");
    espEmptyLayer("TrackStreamingLoadedZone");
    espEmptyLayer("TrackStreamingAudioRead");
    espEmptyLayer("TrackStreamingAudioBlock");
    espEmptyLayer("TrackStreamingLoadingBar");

    if (espGetLayerState("TrackStreamingLoadedZone") != 0) {
        TrackStreamerRemoteCaffeinating = 1;
        ShowSectionBoarder = 1;
        return;
    }
    if (RemoteCaffeinating && TrackStreamerRemoteCaffeinating) {
        ShowSectionBoarder = 1;
    }
}

void TrackStreamer::SetLoadingPhase(eLoadingPhase phase) {
    LoadingPhase = phase;
    if (phase == LOADING_IDLE || phase == LOADING_REGULAR_SECTIONS) {
        SetQueuedFileMinPriority(0);
    } else {
        SetQueuedFileMinPriority(QueuedFileDefaultPriority);
    }
}

int TrackStreamer::UnloadLeastRecentlyUsedSection() {
    TrackStreamingSection *best_section = nullptr;

    for (int n = 0; n < NumTrackStreamingSections; n++) {
        TrackStreamingSection *section = &pTrackStreamingSections[n];
        if (section->Status == TrackStreamingSection::LOADED && !section->CurrentlyVisible &&
            (!best_section || section->LastNeededTimestamp < best_section->LastNeededTimestamp)) {
            best_section = section;
        }
    }

    if (!best_section) {
        return 0;
    }

    UnloadSection(best_section);
    return best_section->LoadedSize;
}

void TrackStreamer::JettisonSection(TrackStreamingSection *section) {
    AmountJettisoned += section->Size;
    JettisonedSections[NumJettisonedSections++] = section;

    if (section->Status == TrackStreamingSection::ACTIVATED) {
        UnactivateSection(section);
    }
    if (section->Status == TrackStreamingSection::LOADED) {
        UnloadSection(section);
    }

    section->CurrentlyVisible = false;

    int i = 0;
    while (CurrentStreamingSections[i] != section) {
        i++;
    }

    while (i < NumCurrentStreamingSections - 1) {
        CurrentStreamingSections[i] = CurrentStreamingSections[i + 1];
        i++;
    }

    NumCurrentStreamingSections--;
}

bool TrackStreamer::JettisonLeastImportantSection() {
    TrackStreamingSection *best_section = ChooseSectionToJettison();
    if (best_section) {
        JettisonSection(best_section);
        return true;
    }
    return false;
}

TrackStreamingSection *TrackStreamer::ChooseSectionToJettison() {
    TrackStreamingSection *best_section = 0;
    int best_discard_priority = 0;
    static int last_jettison_print;
    bool print_jettison_this_frame = false;

    last_jettison_print = RealLoopCounter;

    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        int discard_priority = 0;
        TrackStreamingSection *section = CurrentStreamingSections[i];

        if (IsTextureSection(section->SectionNumber) || IsLibrarySection(section->SectionNumber)) {
            discard_priority = 2;
            if (section->SectionNumber == GetScenerySectionNumber('Y', 0) || section->SectionNumber == GetScenerySectionNumber('W', 0) ||
                section->SectionNumber == GetScenerySectionNumber('X', 0)) {
                discard_priority = 1;
            } else if (LoadingPhase == ALLOCATING_TEXTURE_SECTIONS && IsTextureSection(section->SectionNumber) &&
                       section->Status == TrackStreamingSection::ACTIVATED && !SplitScreen) {
                discard_priority = 10000;
            } else if (LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS && IsLibrarySection(section->SectionNumber) &&
                       section->Status == TrackStreamingSection::ACTIVATED && !SplitScreen) {
                discard_priority = 10000;
            }
        } else if (IsRegularScenerySection(section->SectionNumber)) {
            int loading_priority = GetLoadingPriority(section, &StreamingPositionEntries[0], true);
            if (SplitScreen) {
                int loading_priority2 = GetLoadingPriority(section, &StreamingPositionEntries[1], true);
                if (loading_priority2 < loading_priority) {
                    loading_priority = loading_priority2;
                }
            }
            discard_priority = loading_priority * 10 + 100;
        }

        if (discard_priority != 0) {
            // TODO weird
            if (static_cast<unsigned int>(section->Status - TrackStreamingSection::LOADED) > 1) {
                discard_priority++;
            }
        }
        if (discard_priority > best_discard_priority) {
            best_section = section;
            best_discard_priority = discard_priority;
        }
    }

    return best_section;
}

void TrackStreamer::UnJettisonSections() {
    for (int n = 0; n < NumJettisonedSections; n++) {
        TrackStreamingSection *section = JettisonedSections[n];
        CurrentStreamingSections[NumCurrentStreamingSections++] = section;
        section->CurrentlyVisible = true;
    }
    NumJettisonedSections = 0;
    AmountJettisoned = 0;
}

// UNSOLVED, TODO this is pretty wrong
int TrackStreamer::BuildHoleMovements(HoleMovement *hole_movements, int max_movements, int filler_method, int largest_free, int *pamount_moved,
                                      int max_amount_to_move) {
    ProfileNode profile_node("TODO", 0);
    int ticks = bGetTicker();
    unsigned int checksum = pMemoryPool->GetPoolChecksum();
    bool failed;
    int num_movements;
    int amount_moved;
    int total_needing_allocation;

    pMemoryPool->EnableTracing(false);
    total_needing_allocation = -1;
    failed = false;
    num_movements = 0;
    amount_moved = 0;
    while (true) {
        if (largest_free != 0) {
            if (pMemoryPool->GetLargestFreeBlock() >= largest_free) {
                break;
            }
        } else {
            int out_of_memory_size = AllocateSectionMemory(&total_needing_allocation);
            FreeSectionMemory();
            if (out_of_memory_size == 0) {
                break;
            }
        }

        if (filler_method != 0) {
            if (pMemoryPool->GetAmountFree() == pMemoryPool->GetLargestFreeBlock()) {
                break;
            }
        }

        if (num_movements == max_movements) {
            failed = true;
            break;
        }

        HoleMovement *movement = &hole_movements[num_movements];
        movement->Address = 0;

        if (filler_method == HOLE_FILLER_METHOD_LINEAR_BOTTOM || filler_method == HOLE_FILLER_METHOD_LINEAR_BOTTOM_ALL_THE_WAY ||
            filler_method == HOLE_FILLER_METHOD_LINEAR_TOP) {
            bool start_from_top = filler_method == HOLE_FILLER_METHOD_LINEAR_TOP;
            TSMemoryNode *free_node = pMemoryPool->GetFirstFreeNode(start_from_top);
            TSMemoryNode *node = pMemoryPool->GetNextAllocatedNode(start_from_top, free_node);
            if (filler_method == HOLE_FILLER_METHOD_LINEAR_BOTTOM_ALL_THE_WAY && !node) {
                break;
            }

            if (node) {
                movement->Size = node->Size;
                movement->Address = node->Address;
                movement->NewAddress = free_node->GetAddress(start_from_top, movement->Size);
                if (filler_method == HOLE_FILLER_METHOD_LINEAR_BOTTOM_ALL_THE_WAY && !FindSectionByAddress(movement->Address)) {
                    break;
                }
            }
        } else if (filler_method == HOLE_FILLER_METHOD_BIGGEST_FIRST_BOTTOM || filler_method == HOLE_FILLER_METHOD_BIGGEST_FIRST_TOP) {
            bool start_from_top = filler_method == HOLE_FILLER_METHOD_BIGGEST_FIRST_TOP;
            TSMemoryNode *free_node = pMemoryPool->GetFirstFreeNode(start_from_top);
            int best_hole_size = 0;
            bool first = true;
            TSMemoryNode *next_node = free_node;

            while ((next_node = pMemoryPool->GetNextAllocatedNode(start_from_top, next_node)) != nullptr) {
                TSMemoryNode *next_free = pMemoryPool->GetNextFreeNode(start_from_top, next_node);
                if (!next_free) {
                    continue;
                }
                if (first || next_node->Size <= free_node->Size) {
                    first = false;
                    TSMemoryNode *node1 = pMemoryPool->GetNextNode(!start_from_top, next_node);
                    TSMemoryNode *node2 = pMemoryPool->GetNextNode(start_from_top, next_node);
                    int hole_size = next_node->Size;
                    if (node1 && node1->IsFree()) {
                        hole_size += node1->Size;
                    }
                    if (node2 && node2->IsFree()) {
                        hole_size += node2->Size;
                    }
                    if (hole_size > best_hole_size) {
                        best_hole_size = hole_size;
                        movement->Size = next_node->Size;
                        movement->Address = next_node->Address;
                        movement->NewAddress = free_node->GetAddress(start_from_top, movement->Size);
                    }
                }
            }
        } else if (filler_method == HOLE_FILLER_METHOD_SUPER_SCOOPER) {
            bool done = false;
            bool found_one = false;
            bool first_pass = true;
            bool largest_flag = false;
            bool found_big_enough = false;
            TSMemoryNode *top_free_top;
            TSMemoryNode *bottom_free_top;
            TSMemoryNode *top_allocated;
            TSMemoryNode *largest_allocated;
            TSMemoryNode *cursor;
            TSMemoryNode *evaluated_top_free;
            TSMemoryNode *evaluated_largest_allocated;
            int top_free_memory;
            int middle_allocated_memory;
            int bottom_free_memory;
            int total_free_memory;
            int current_best = 0;
            int position;
            int largest_moves_here = 0;
            int evaluated_largest_moves_here = 0;
            int size_checking[32];
            int found_nodes;
            int nodes_to_move;
            int current_best_middle_memory = 0x3E8000;

            do {
                if (first_pass) {
                    first_pass = false;
                    top_free_top = pMemoryPool->GetFirstNode(true);
                } else {
                    top_free_top = pMemoryPool->GetNextFreeNode(true, top_free_top);
                }

                if (!top_free_top) {
                    done = true;
                } else {
                    top_free_memory = top_free_top->Size;
                    bottom_free_top = pMemoryPool->GetNextFreeNode(true, top_free_top);
                    if (!bottom_free_top) {
                        done = true;
                    } else {
                        bottom_free_memory = bottom_free_top->Size;
                        total_free_memory = top_free_memory + bottom_free_memory;
                        top_allocated = pMemoryPool->GetNextNode(true, top_free_top);
                        pMemoryPool->GetNextNode(false, bottom_free_top);

                        middle_allocated_memory = top_allocated->Size;
                        for (int i = 0; i < 32; i++) {
                            size_checking[i] = 0;
                        }

                        size_checking[0] = top_allocated->Size;
                        largest_allocated = top_allocated;
                        cursor = top_allocated;
                        found_nodes = 1;
                        while (pMemoryPool->GetNextNode(true, cursor) != bottom_free_top && cursor && found_nodes < 32) {
                            cursor = pMemoryPool->GetNextNode(true, cursor);
                            if (cursor) {
                                size_checking[found_nodes] = cursor->Size;
                                middle_allocated_memory += cursor->Size;
                                found_nodes++;
                                if (cursor->Size > largest_allocated->Size) {
                                    largest_allocated = cursor;
                                }
                            }
                        }

                        if ((!found_big_enough && total_free_memory - middle_allocated_memory > current_best) ||
                            (found_big_enough && total_free_memory + middle_allocated_memory >= total_needing_allocation &&
                             middle_allocated_memory < current_best_middle_memory)) {
                            nodes_to_move = 0;
                            std::sort(size_checking, size_checking + found_nodes + 1);
                            largest_flag = false;
                            evaluated_top_free = pMemoryPool->GetFirstFreeNode(true);
                            position = 0;

                            while (found_nodes > nodes_to_move && evaluated_top_free) {
                                bool skip_flag = false;
                                int target_index = found_nodes - nodes_to_move;
                                for (int i = 0; i < found_nodes; i++) {
                                    if (size_checking[target_index] == position) {
                                        skip_flag = true;
                                    }
                                }
                                if (evaluated_top_free == top_free_top || evaluated_top_free == bottom_free_top) {
                                    skip_flag = true;
                                }
                                if (!skip_flag && evaluated_top_free->Size >= size_checking[target_index]) {
                                    size_checking[target_index] = position;
                                    nodes_to_move++;
                                    if (!largest_flag) {
                                        largest_moves_here = evaluated_top_free->Address;
                                        largest_flag = true;
                                    }
                                    evaluated_top_free = pMemoryPool->GetNextFreeNode(true, nullptr);
                                }
                                evaluated_top_free = pMemoryPool->GetNextFreeNode(true, evaluated_top_free);
                                position++;
                            }

                            if (nodes_to_move >= found_nodes) {
                                current_best = total_free_memory - middle_allocated_memory;
                                evaluated_largest_moves_here = largest_moves_here;
                                found_one = true;
                                evaluated_largest_allocated = largest_allocated;
                                if (total_needing_allocation <= total_free_memory + middle_allocated_memory) {
                                    found_big_enough = true;
                                    current_best_middle_memory = middle_allocated_memory;
                                }
                            }
                        }
                    }
                }
            } while (!done);

            if (found_one && evaluated_largest_allocated && FindSectionByAddress(evaluated_largest_allocated->Address)) {
                movement->Size = evaluated_largest_allocated->Size;
                movement->Address = evaluated_largest_allocated->Address;
                movement->NewAddress = evaluated_largest_moves_here;
            }
        }

        if (movement->Address == 0) {
            failed = true;
            break;
        }

        num_movements++;
        movement->Checksum = pMemoryPool->GetPoolChecksum();
        pMemoryPool->Free(reinterpret_cast<void *>(movement->Address));
        pMemoryPool->Malloc(movement->Size, "HoleMovement", false, false, movement->NewAddress);
        amount_moved += movement->Size;
        if (amount_moved > max_amount_to_move) {
            failed = true;
            break;
        }
    }

    for (int n = num_movements - 1; n >= 0; n--) {
        HoleMovement *movement = &hole_movements[n];
        pMemoryPool->Free(reinterpret_cast<void *>(movement->NewAddress));
        char *debug_name = "UndoHoleMovement";
        TrackStreamingSection *section = FindSectionByAddress(movement->Address);
        if (section) {
            debug_name = section->SectionName;
        }
        pMemoryPool->Malloc(movement->Size, debug_name, false, false, movement->Address);
    }

    pMemoryPool->EnableTracing(true);
    if (pamount_moved) {
        *pamount_moved = amount_moved;
    }
    if (failed) {
        return -1;
    }
    return num_movements;
}

int TrackStreamer::DoHoleFilling(int largest_free) {
    ProfileNode profile_node("TODO", 0);
    const char *fragmented_user_allocation;
    HoleMovement hole_movement_table[128];

    CountUserAllocations(&fragmented_user_allocation);
    if (fragmented_user_allocation) {
        pMemoryPool->DebugPrint();
#ifdef MILESTONE_OPT
        bReleasePrintf("WARNING:  TrackStreamer::DoHoleFilling() is aborting due to fragmented user alloc ation %s\n");
#endif
        return 0;
    }

    int best_method = -1;
    if (ForceHoleFillerMethod >= 0) {
        int filler_method = ForceHoleFillerMethod;
        int num_hole_movements = BuildHoleMovements(hole_movement_table, 0x80, filler_method, largest_free, 0, 0x7FFFFFFF);
        if (num_hole_movements > 0) {
            best_method = filler_method;
        }
    } else {
        int best_amount_moved = 0x7FFFFFFF;
        for (int filler_method = 1; filler_method < 6; filler_method++) {
            int amount_moved;
            int num_hole_movements = BuildHoleMovements(hole_movement_table, 0x80, filler_method, largest_free, &amount_moved, best_amount_moved);
            if (num_hole_movements > 0 && amount_moved < best_amount_moved) {
                best_method = filler_method;
                best_amount_moved = amount_moved;
            }
        }
    }

    if (best_method < 0) {
        return 0;
    }

    int num_hole_movements = BuildHoleMovements(hole_movement_table, 0x80, best_method, largest_free, 0, 0x7FFFFFFF);
    for (int n = 0; n < num_hole_movements; n++) {
        ProfileNode profile_node("TODO", 0);
        HoleMovement *movement = &hole_movement_table[n];
        TrackStreamingSection *section = FindSectionByAddress(movement->Address);
        if (LastWaitUntilRenderingDoneFrameCount != eGetFrameCounter()) {
            int start_ticks = bGetTicker();
            DisableWaitForFrameBufferSwap();
            eWaitUntilRenderingDone();
            LastWaitUntilRenderingDoneFrameCount = eGetFrameCounter();
            EnableWaitForFrameBufferSwap();
            float time = bGetTickerDifference(start_ticks);
        }

        int start_ticks = bGetTicker();
        void *new_memory = reinterpret_cast<void *>(movement->NewAddress);
        pMemoryPool->Free(reinterpret_cast<void *>(movement->Address));
        pMemoryPool->Malloc(movement->Size, section->SectionName, false, false, movement->NewAddress);
        if (section->Status == TrackStreamingSection::ACTIVATED) {
            eAllowDuplicateSolids(true);
            SetDuplicateTextureWarning(false);
            MoveChunks(reinterpret_cast<bChunk *>(new_memory), reinterpret_cast<bChunk *>(section->pMemory), section->LoadedSize,
                       section->SectionName);
#ifdef EA_PLATFORM_GAMECUBE
            DCStoreRangeNoSync(new_memory, section->LoadedSize);
#endif
            eAllowDuplicateSolids(false);
            SetDuplicateTextureWarning(true);
        } else {
            eWaitUntilRenderingDone();
            bOverlappedMemCpy(new_memory, section->pMemory, section->LoadedSize);
        }
        section->pMemory = new_memory;
        float move_time = bGetTickerDifference(start_ticks);
        NumSectionsMoved++;
#ifdef EA_PLATFORM_GAMECUBE
        PPCSync();
#endif
    }

    return 1;
}

void TrackStreamer::SetStreamingPosition(int position_number, const bVector3 *position) {
    StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
    position_entry->Position.x = position->x;
    position_entry->Position.y = position->y;
    position_entry->Elevation = position->z;
    position_entry->Velocity.x = 0.0f;
    position_entry->Velocity.y = 0.0f;
    position_entry->Direction.x = 0.0f;
    position_entry->Direction.y = 0.0f;
    position_entry->PredictedZoneValidTime = -1;
    position_entry->PositionSet = true;
#ifndef EA_BUILD_A124
    position_entry->FollowingCar = false;
#endif
    position_entry->PredictedZone = 0;
    CurrentZoneNeedsRefreshing = true;

    {
        // TODO
        float dist = bDistBetween(&position_entry->Position, &position_entry->Position);
    }
}

void TrackStreamer::PredictStreamingPosition(int position_number, const bVector3 *position, const bVector3 *velocity, const bVector3 *direction,
                                             bool following_car) {
    StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
    position_entry->Position.x = position->x;
    position_entry->Position.y = position->y;
    position_entry->Elevation = position->z;
    position_entry->Velocity.x = velocity->x;
    position_entry->Velocity.y = velocity->y;
    position_entry->Direction.x = direction->x;
    position_entry->Direction.y = direction->y;
    position_entry->PositionSet = true;
#ifndef EA_BUILD_A124
    position_entry->FollowingCar = following_car;
#endif

    {
        // TODO
        float dist = bDistBetween(&position_entry->Position, &position_entry->Position);
    }
}

// UNSOLVED regswap, branching and it uses a different MPH2MPS function...
short TrackStreamer::GetPredictedZone(StreamingPositionEntry *position_entry) {
    int predict_zone_number = 0;
    bVector2 predict_position;
    bool predict_position_used = false;
    float speed = bLength(&position_entry->Velocity);
    TrackPathZone *zone = nullptr;
    char predict_debug_name[64];

    while ((zone = TheTrackPathManager.FindZone(&position_entry->Position, TRACK_PATH_ZONE_STREAMER_PREDICTION, zone))) {
        float elevation = zone->GetElevation();
        // TODO branch targets
        if (((elevation <= 0.0f) || (position_entry->Elevation >= elevation)) &&
            (elevation >= 0.0f || (position_entry->Elevation > bAbs(elevation)))) {
            continue;
        }

        // TODO this MPH2MPS inline is in CarBasics.hpp
        float max_speed = MPH2MPS(400.0f);
        float distance = speed * 1.5f;
        DrivableScenerySection *scenery_section;
        if (speed > max_speed) {
            predict_position = position_entry->Position;
        } else if (100.0f < distance) {
            bScaleAdd(&predict_position, &position_entry->Position, &position_entry->Velocity, 100.0f / speed);
        } else {
            bScaleAdd(&predict_position, &position_entry->Position, &position_entry->Velocity, 1.5f);
        }

        scenery_section = TheVisibleSectionManager.FindDrivableSection(&predict_position);
        if (scenery_section) {
            for (int n = 0; n < 4; n++) {
                if (zone->Data[n] == 0) {
                    break;
                }
                if (scenery_section->SectionNumber == zone->Data[n]) {
                    predict_position_used = true;
                    predict_zone_number = scenery_section->SectionNumber;
                    break;
                }
            }
        }
    }

    if (predict_position_used) {
        if (!bEqual(&predict_position, &position_entry->Position, 0.001f)) {
            for (int barrier_num = 0; barrier_num < NumBarriers; barrier_num++) {
                TrackStreamingBarrier *barrier = &pBarriers[barrier_num];
                if (barrier->Intersects(&position_entry->Position, &predict_position)) {
                    predict_position_used = false;
                    predict_zone_number = 0;
                }
            }
        }

        if (predict_position_used) {
            return predict_zone_number;
        }
    }

    {
        DrivableScenerySection *scenery_section = TheVisibleSectionManager.FindDrivableSection(&position_entry->Position);
        if (scenery_section) {
            predict_zone_number = scenery_section->SectionNumber;
        }
    }

    {
        unsigned int obj = 0;
        espCreateObjectAsync("TrackStreamingTrail", "TrackStreamingTrailPoint", nullptr);
        FloatVector pos;
        espSetObjectPosition(1, &pos);
        char text[40];
        MPS2MPH(speed);
        espSetAttributeString(1, "PredictZones", text);
        espLinkObject(obj, 1);
    }

    {
        FloatVector pos1;
        FloatVector pos2;
        espCreateObjectAsync("TrackStreamingPredict", "TrackStreamingPredictPoint", &pos1);
        espCreateObjectAsync("TrackStreamingPredict", "TrackStreamingPredictPoint", &pos2);
        espLinkObject(0, 1);
    }

    return predict_zone_number;
}

void TrackStreamer::ClearStreamingPositions() {
    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        position_entry->PositionSet = false;
#ifndef EA_BUILD_A124
        position_entry->FollowingCar = false;
#endif
    }
}

void TrackStreamer::RemoveCurrentStreamingSections() {
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        CurrentStreamingSections[i]->CurrentlyVisible = 0;
    }

    NumCurrentStreamingSections = 0;
    CurrentVisibleSectionTable.ClearTable();
}

void TrackStreamer::AddCurrentStreamingSections(short *sections_to_load, int num_sections_to_load, int position_number) {
    for (int i = 0; i < num_sections_to_load; i++) {
        CurrentVisibleSectionTable.Set(sections_to_load[i]);

        // TODO this variable is fake
        short &section_number = sections_to_load[i];
        if (SplitScreen) {
            section_number = static_cast<short>(Get2PlayerSectionNumber(section_number));
        }

        TrackStreamingSection *section = FindSection(section_number);
        if (!section) {
            continue;
        }

        section->LastNeededTimestamp = RealTimeFrames;
        if (!section->CurrentlyVisible) {
            CurrentStreamingSections[NumCurrentStreamingSections++] = section;
        }

        // can this be simplified?
        if (((section->CurrentlyVisible >> position_number) ^ 1U) & 1) {
            section->CurrentlyVisible |= static_cast<unsigned char>(1 << position_number);
            if (section->Status < TrackStreamingSection::LOADED) {
                StreamingPositionEntry *streaming_position = &StreamingPositionEntries[position_number];
                streaming_position->NumSectionsToLoad++;
                streaming_position->AmountToLoad += section->Size;
            }
        }
    }
}

// UNSOLVED
void TrackStreamer::DetermineStreamingSections() {
    const int max_sections_to_load = 0x180;
    short sections_to_load[384];
    int num_sections_to_load = 3;

    RemoveCurrentStreamingSections();
    sections_to_load[0] = GetScenerySectionNumber('Y', 0);
    sections_to_load[1] = GetScenerySectionNumber('X', 0);
    sections_to_load[2] = GetScenerySectionNumber('Z', 0);

    if (SeeulatorToolActive && ScenerySectionToBlink != 0) {
        num_sections_to_load = 4;
        sections_to_load[3] = static_cast<short>(ScenerySectionToBlink);
    }

    short section_number;
    for (int n = 0; n < 4; n++) {
        section_number = KeepSectionTable[n];
        if (section_number != 0) {
            sections_to_load[num_sections_to_load] = section_number;
            num_sections_to_load++;
        }
    }

    AddCurrentStreamingSections(sections_to_load, num_sections_to_load, 0);
    AddCurrentStreamingSections(sections_to_load, num_sections_to_load, 1);
    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        if (position_entry->CurrentZone > 0) {
            LoadingSection *loading_section = TheVisibleSectionManager.FindLoadingSection(position_entry->CurrentZone);
            if (!loading_section) {
                DrivableScenerySection *drivable_section = TheVisibleSectionManager.FindDrivableSection(position_entry->CurrentZone);
                num_sections_to_load = 0;
                for (int i = 0; i < drivable_section->GetNumVisibleSections(); i++) {
                    int section_number = drivable_section->GetVisibleSection(i);
                    sections_to_load[num_sections_to_load] = section_number;
                    num_sections_to_load++;
                }
            } else {
                num_sections_to_load = TheVisibleSectionManager.GetSectionsToLoad(loading_section, sections_to_load, max_sections_to_load);
            }

            AddCurrentStreamingSections(sections_to_load, num_sections_to_load, position_number);
        }
    }
}

int TrackStreamer::AllocateSectionMemory(int *ptotal_needing_allocation) {
    ProfileNode profile_node("TODO", 0);
    int out_of_memory_size = 0;
    int total_needing_allocation = 0;
    int num_sections_allocated = 0;

    if (LoadingPhase == ALLOCATING_REGULAR_SECTIONS) {
        for (DiscBundleSection *disc_bundle = pDiscBundleSections; disc_bundle < pLastDiscBundleSection;) {
            int i;
            for (i = 0; i < disc_bundle->NumMembers; i++) {
                DiscBundleSectionMember *member = &disc_bundle->Members[i];
                TrackStreamingSection *section = member->pSection;
                if (!section->CurrentlyVisible || section->Status != TrackStreamingSection::UNLOADED) {
                    break;
                }
            }

            if (i == disc_bundle->NumMembers) {
                if (disc_bundle->FileSize <= pMemoryPool->GetLargestFreeBlock()) {
                    unsigned char *pmemory =
                        static_cast<unsigned char *>(pMemoryPool->Malloc(disc_bundle->FileSize, disc_bundle->SectionName, true, false, 0));
                    pMemoryPool->Free(pmemory);

                    for (i = 0; i < disc_bundle->NumMembers; i++) {
                        DiscBundleSectionMember *member = &disc_bundle->Members[i];
                        TrackStreamingSection *section = member->pSection;

                        num_sections_allocated++;
                        section->pDiscBundle = disc_bundle;

                        void *realloc_mem = pmemory + member->FileOffset * 0x80;
                        section->Status = TrackStreamingSection::ALLOCATED;
                        section->pMemory = realloc_mem;
                        pMemoryPool->Malloc(section->Size, disc_bundle->SectionName, false, false, reinterpret_cast<intptr_t>(realloc_mem));
                    }

                    total_needing_allocation += disc_bundle->FileSize;
                }
            }

            disc_bundle = disc_bundle->GetMemoryImageNext();
        }
    }

    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        if (section->Status != TrackStreamingSection::UNLOADED) {
            continue;
        }

        if (section->SectionNumber != GetScenerySectionNumber('Y', 0) && section->SectionNumber != GetScenerySectionNumber('X', 0) &&
            section->SectionNumber != GetScenerySectionNumber('W', 0) && section->SectionNumber != GetScenerySectionNumber('U', 0) &&
            section->SectionNumber != GetScenerySectionNumber('Z', 0)) {
            if (LoadingPhase == ALLOCATING_TEXTURE_SECTIONS) {
                if (!IsTextureSection(section->SectionNumber)) {
                    continue;
                }
            }

            if (LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS) {
                if (!IsLibrarySection(section->SectionNumber)) {
                    continue;
                }
            }
        }

        total_needing_allocation += section->Size;
        if (section->Size > bLargestMalloc(7)) {
            out_of_memory_size += section->Size;
            NumSectionsOutOfMemory++;
        } else {
            num_sections_allocated++;
            section->pMemory = AllocateMemory(section, 0x80);
            section->Status = TrackStreamingSection::ALLOCATED;
            if (num_sections_allocated > 99999) {
                CurrentZoneAllocatedButIncomplete = true;
                return out_of_memory_size;
            }
        }
    }

    CurrentZoneAllocatedButIncomplete = false;
    *ptotal_needing_allocation = total_needing_allocation;
    return out_of_memory_size;
}

void TrackStreamer::FreeSectionMemory() {
    NumSectionsOutOfMemory = 0;
    for (int n = 0; n < NumTrackStreamingSections; n++) {
        TrackStreamingSection *section = &pTrackStreamingSections[n];
        if (section->Status == TrackStreamingSection::ALLOCATED) {
            bFree(section->pMemory);
            section->pDiscBundle = nullptr;
            section->pMemory = nullptr;
            section->Status = TrackStreamingSection::UNLOADED;
        }
    }
}

bool TrackStreamer::HandleMemoryAllocation() {
    int out_of_memory_size;
    int total_amount_unloaded = 0;
    int total_amount_hole_filled = 0;

    {
        int total_needing_allocation;
        int amount_unloaded;

        do {
            amount_unloaded = UnloadLeastRecentlyUsedSection();
        } while (amount_unloaded != 0);

        NumSectionsMoved = 0;

        while (true) {
            FreeSectionMemory();
            out_of_memory_size = AllocateSectionMemory(&total_needing_allocation);
            if (out_of_memory_size == 0) {
                break;
            }
            if (total_amount_unloaded > 0x7FFFFFFF || total_amount_hole_filled > 0x200000) {
                return false;
            }
            if (NumSectionsMoved > 15) {
                return false;
            }

            FreeSectionMemory();
            amount_unloaded = UnloadLeastRecentlyUsedSection();
            total_amount_unloaded += amount_unloaded;
            if (amount_unloaded > 0) {
                continue;
            }

            while (total_needing_allocation + 0x4000 > bCountFreeMemory(7)) {
                CurrentZoneOutOfMemory = true;
                if (!JettisonLeastImportantSection()) {
                    break;
                }
                AllocateSectionMemory(&total_needing_allocation);
                FreeSectionMemory();
            }

            {
                int amount_hole_filled = DoHoleFilling(0);
                total_amount_hole_filled += amount_hole_filled;
                if (amount_hole_filled != 0) {
                    continue;
                }
            }

            CurrentZoneOutOfMemory = true;
            if (!JettisonLeastImportantSection()) {
                AllocateSectionMemory(&total_needing_allocation);
                break;
            }
        }
    }

    MemorySafetyMargin = 0;
    out_of_memory_size += AmountJettisoned;

    {
        int free_memory = bCountFreeMemory(7) - out_of_memory_size;

        for (int n = 0; n < NumTrackStreamingSections; n++) {
            TrackStreamingSection *section = &pTrackStreamingSections[n];
            if (!section->CurrentlyVisible && section->Status != TrackStreamingSection::UNLOADED) {
                free_memory += section->PermSize;
            }
        }
        MemorySafetyMargin = free_memory;
    }

    if (out_of_memory_size != 0) {
        for (int n = 0; n < NumJettisonedSections; n++) {
        }

        if (LoadingPhase == LOADING_REGULAR_SECTIONS) {
            for (int i = 0; i < NumCurrentStreamingSections; i++) {
                TrackStreamingSection *section;
            }
        }
    }

    return true;
}

void *TrackStreamer::AllocateUserMemory(int size, const char *debug_name, int offset) {
    if (size > bLargestMalloc(7)) {
        void *mem = bMalloc(size, debug_name, 0, (offset & 0x1FFC) << 17 | 0x2000);
        return mem;
    } else {
        void *mem = bMalloc(size, debug_name, 0, (offset & 0x1FFC) << 17 | 0x2047);
        return mem;
    }
}

void TrackStreamer::FreeUserMemory(void *mem) {
    int free_before = pMemoryPool->GetAmountFree();
    bFree(mem);
    int size = pMemoryPool->GetAmountFree();
}

bool TrackStreamer::IsUserMemory(void *mem) {
    int pos = static_cast<char *>(mem) - static_cast<char *>(pMemoryPoolMem);
    return pMemoryPoolMem && pos >= 0 && pos < MemoryPoolSize;
}

bool TrackStreamer::MakeSpaceInPool(int size, bool force_unloading) {
    WaitForCurrentLoadingToComplete();
    while (size > bCountFreeMemory(7)) {
        if (UnloadLeastRecentlyUsedSection() == 0 && (!force_unloading || !JettisonLeastImportantSection())) {
            break;
        }
    }

    ForceHoleFillerMethod = 0;
    DoHoleFilling(0x7FFFFFFF);
    ForceHoleFillerMethod = -1;
    return size <= bLargestMalloc(7);
}

void TrackStreamer::MakeSpaceInPool(int size, void (*callback)(intptr_t), intptr_t param) {
    if (LoadingPhase == LOADING_IDLE) {
        IsLoadingInProgress();
    }

    if (!IsLoadingInProgress()) {
        MakeSpaceInPool(size, true);
        callback(param);
    } else {
        MakeSpaceInPoolSize = size;
        MakeSpaceInPoolCallback = callback;
        MakeSpaceInPoolCallbackParam = param;
        pCallback = ReadyToMakeSpaceInPoolBridge;
        CallbackParam = reinterpret_cast<int>(this);
    }
}

void TrackStreamer::ReadyToMakeSpaceInPool() {
    MakeSpaceInPool(MakeSpaceInPoolSize, true);

    void (*callback)(intptr_t) = MakeSpaceInPoolCallback;
    intptr_t param = MakeSpaceInPoolCallbackParam;
    MakeSpaceInPoolCallback = nullptr;
    MakeSpaceInPoolCallbackParam = 0;
    MakeSpaceInPoolSize = 0;
    callback(param);
}

bool TrackStreamer::DetermineCurrentZones(short *current_zones) {
    bool current_zones_different = false;
    int position_number;
    for (position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        short predict_zone_number = -1;
        if (position_entry->PositionSet) {
            predict_zone_number = GetPredictedZone(position_entry);
        }
        short zone_number = predict_zone_number;

        if (zone_number == position_entry->PredictedZone) {
            position_entry->PredictedZoneValidTime++;
        } else if (position_entry->PredictedZoneValidTime == -1) {
            position_entry->PredictedZone = zone_number;
            position_entry->PredictedZoneValidTime = 1000;
        } else {
            position_entry->PredictedZone = zone_number;
            position_entry->PredictedZoneValidTime = 1;
        }

        if (zone_number != position_entry->CurrentZone) {
            if (position_entry->PredictedZoneValidTime < 0) {
                zone_number = position_entry->CurrentZone;
            }
            if (zone_number != position_entry->CurrentZone) {
                current_zones_different = true;
            }
        }

        current_zones[position_number] = zone_number;
    }

    return current_zones_different | CurrentZoneNeedsRefreshing;
}

void TrackStreamer::ServiceGameState() {
    float start_time = GetDebugRealTime();
    HandleZoneSwitching();
    HandleSectionActivation();
    float time = GetDebugRealTime();

    AmountNotRendered = 0;
    for (int n = 0; n < NumCurrentStreamingSections; n++) {
        TrackStreamingSection *section = CurrentStreamingSections[n];
        if (!section->WasRendered && IsRegularScenerySection(section->SectionNumber)) {
            AmountNotRendered += section->Size;
        }
        section->WasRendered = 0;
    }
}

void TrackStreamer::ServiceNonGameState() {
    ProfileNode profile_node("TODO", 0);
    float start_time = GetDebugRealTime();
    HandleLoading();
    float time = GetDebugRealTime();
}

void TrackStreamer::BlockUntilLoadingComplete() {
    RefreshLoading();
    WaitForCurrentLoadingToComplete();
}

void TrackStreamer::WaitForCurrentLoadingToComplete() {
    while (!AreAllSectionsActivated()) {
        HandleLoading();
        short section_to_activate = static_cast<short>(GetSectionToActivate(0));
        if (section_to_activate != 0) {
            ActivateSection(FindSection(section_to_activate));
        }
        ServiceResourceLoading();
        bThreadYield(8);
    }
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

bool TrackStreamer::AreAllSectionsActivated() {
    return LoadingPhase == LOADING_IDLE && NumSectionsActivated + NumSectionsOutOfMemory >= NumCurrentStreamingSections;
}

void TrackStreamer::RefreshLoading() {
    CurrentZoneNeedsRefreshing = true;
    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        position_entry->PredictedZoneValidTime = -1;
    }
    HandleZoneSwitching();
}

void TrackStreamer::HandleZoneSwitching() {
    ProfileNode profile_node("TODO", 0);
    short current_zones[2];
    bool current_zones_different;
    if (!ZoneSwitchingDisabled && pMemoryPoolMem) {
        current_zones_different = DetermineCurrentZones(current_zones);
        if (current_zones_different) {
            SwitchZones(current_zones);
        }
    }
}

void TrackStreamer::SwitchZones(short *current_zones) {
    StartLoadingTime = GetDebugRealTime();
    CurrentZoneNeedsRefreshing = false;

    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        int zone_number = current_zones[position_number];
        if (position_entry->CurrentZone != zone_number) {
            PlotLoadingMarker(position_entry);

            VisibleSectionBoundary *boundary1 = TheVisibleSectionManager.FindBoundary(position_entry->CurrentZone);
            VisibleSectionBoundary *boundary2 = TheVisibleSectionManager.FindBoundary(zone_number);
            float best_distance = 10000.0f;
            if (boundary1 && boundary2) {
                for (int n = 0; n < boundary1->GetNumPoints(); n++) {
                    float distance = boundary2->GetDistanceOutside(boundary1->GetPoint(n), 999.0f);
                    best_distance = bMin(best_distance, distance);
                }
            }

            if (0.1f < best_distance) {
                CurrentZoneFarLoad = true;
            }

            position_entry->CurrentZone = zone_number;
            position_entry->BeginLoadingPosition = position_entry->Position;
            position_entry->BeginLoadingTime = GetDebugRealTime();
            position_entry->NumSectionsToLoad = 0;
            position_entry->NumSectionsLoaded = 0;
            position_entry->AmountToLoad = 0;
            position_entry->AmountLoaded = 0;
        }

        if (position_number == 0) {
            GetScenerySectionName(CurrentZoneName, zone_number);
        } else if (zone_number > 0) {
            bSPrintf(CurrentZoneName, "%s - %s", CurrentZoneName, GetScenerySectionName(zone_number));
        }
    }

    int num_sections_unactivated = 0;
    DetermineStreamingSections();
    PostLoadFixupDisabled = true;
    for (int n = 0; n < NumTrackStreamingSections; n++) {
        TrackStreamingSection *section = &pTrackStreamingSections[n];
        if (section->Status == TrackStreamingSection::ACTIVATED && !section->CurrentlyVisible) {
            if (!IsTextureSection(section->SectionNumber) && !IsLibrarySection(section->SectionNumber)) {
                UnactivateSection(section);
                num_sections_unactivated++;
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
    CurrentZoneOutOfMemory = false;
    CurrentZoneAllocatedButIncomplete = false;
    MemorySafetyMargin = 0;
    AmountJettisoned = 0;
    NumJettisonedSections = 0;
    bMemSet(JettisonedSections, 0, sizeof(JettisonedSections));
    AssignLoadingPriority();
    CalculateLoadingBacklog();
}

// UNSOLVED regswaps
void TrackStreamer::HandleLoading() {
    if (SkipNextHandleLoad) {
        SkipNextHandleLoad = false;
        return;
    }
    if (LoadingPhase == LOADING_IDLE) {
        return;
    }

    if ((LoadingPhase == LOADING_TEXTURE_SECTIONS || LoadingPhase == LOADING_GEOMETRY_SECTIONS || LoadingPhase == LOADING_REGULAR_SECTIONS)) {
        StartLoadingSections();
        if (NumSectionsLoading == 0) {
            if (CurrentZoneAllocatedButIncomplete) {
                SetLoadingPhase(static_cast<eLoadingPhase>(LoadingPhase - 1));
            } else {
                if (LoadingPhase == LOADING_REGULAR_SECTIONS) {
                    FinishedLoading();
                    return;
                }
                SetLoadingPhase(static_cast<eLoadingPhase>(LoadingPhase + 1));
            }
        }
    }

    if ((LoadingPhase == ALLOCATING_TEXTURE_SECTIONS || LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS ||
         LoadingPhase == ALLOCATING_REGULAR_SECTIONS) &&
        NumSectionsLoading < 1) {
        int num_sections_unactivated = 0;
        for (int n = 0; n < NumTrackStreamingSections; n++) {
            TrackStreamingSection *section = &pTrackStreamingSections[n];
            if (section->Status == TrackStreamingSection::ACTIVATED && !section->CurrentlyVisible) {
                // TODO writing this as two different branches switches byteswaps, but messes with the instructions
                if ((LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS && IsTextureSection(section->SectionNumber)) ||
                    (LoadingPhase == ALLOCATING_REGULAR_SECTIONS && IsLibrarySection(section->SectionNumber))) {
                    UnactivateSection(section);
                    num_sections_unactivated++;
                }
            }
        }

        if (num_sections_unactivated > 0) {
            SkipNextHandleLoad = true;
            return;
        }
        PostLoadFixupDisabled = true;
        bool complete = HandleMemoryAllocation();
        PostLoadFixupDisabled = false;
        if (NumSectionsMoved > 0) {
            PostLoadFixup();
        }
        if (complete != 0) {
            SetLoadingPhase(static_cast<eLoadingPhase>(LoadingPhase + 1));
            if (LoadingPhase == LOADING_TEXTURE_SECTIONS || LoadingPhase == LOADING_GEOMETRY_SECTIONS) {
                UnJettisonSections();
            }
            HandleLoading();
        }
    }
}

// UNSOLVED because of debug stuff
int TrackStreamer::GetLoadingPriority(TrackStreamingSection *section, StreamingPositionEntry *position_entry, bool calculating_jettison) {
#ifdef EA_BUILD_A124
    return 0; // this entire function is different in A124
#else
    if (!section->pBoundary) {
        return 0;
    }

    float speed = bLength(position_entry->Velocity);
    if (calculating_jettison) {
        speed = 100.0f;
    }

    if (speed < 1.0f) {
        return 0;
    }

    bVector2 predict_pos = position_entry->Position + position_entry->Velocity * 1.0f;
    VisibleSectionBoundary *boundary = section->pBoundary;
    float distance = boundary->GetDistanceOutside(&predict_pos, 999.0f);

    bVector2 direction;
    if (calculating_jettison) {
        bNormalize(&direction, &position_entry->Direction);
    } else {
        bNormalize(&direction, &position_entry->Velocity);
    }

    bVector2 v = section->Centre - predict_pos;
    v = bNormalize(v);
    float dot = bDot(&direction, &v);
    float speed_factor = bMin(speed * 0.016666668f, 1.0f);
    float angle = bAngToDeg(bACos(dot));
    float angle_factor = bClamp(angle, 20.0f, 90.0f);
    float adjusted_distance = distance * (1.0f - (90.0f - angle_factor) * 0.014285714f * speed_factor * 0.66999996f);
    int priority = bClamp(static_cast<int>(adjusted_distance * 0.013333334f), 0, 2);

    // TODO
    if (RemoteCaffeinating && TrackStreamerRemoteCaffeinating) {
        char layer_name[32];
        if (section->Status == TrackStreamingSection::UNLOADED) {
            bSPrintf(layer_name, "LoadingPriority%d", priority);
        } else {
            bSPrintf(layer_name, "LoadingPriorityLoaded");
        }

        FloatVector pos;
        pos.x = section->Centre.x;
        pos.y = section->Centre.y;
        pos.z = 0.0f;
        espCreateObjectAsync(layer_name, "LoadingPriorityPoint", &pos);
        espSetAttributeString(1, "Section", section->SectionName);
        espCreateUserMesh(1, boundary->GetNumPoints());

        for (int n = 0; n < boundary->GetNumPoints(); n++) {
            bVector2 *point1 = boundary->GetPoint(0);
            bVector2 *point2 = boundary->GetPoint(n % boundary->GetNumPoints());
            FloatVector face[4];
            face[0].x = point1->x - section->Centre.x;
            face[0].y = point1->y - section->Centre.y;
            face[0].z = 0.0f;

            face[1].x = point1->x - section->Centre.x;
            face[1].y = point1->y - section->Centre.y;
            face[1].z = 0.0f;

            face[2].x = point2->x - section->Centre.x;
            face[2].y = point2->y - section->Centre.y;
            face[2].z = 0.0f;

            face[3].x = point2->x - section->Centre.x;
            face[3].y = point2->y - section->Centre.y;
            face[3].z = 0.0f;

            espSetUserMeshFace(1, n, face);
        }
        FloatVector face[4];
        // TODO
        // face[0].x = v32;
        // face[0].y = v33;
        face[0].z = 0.0f;

        // TODO
        // face[1].x = v32;
        // face[1].y = v33;
        face[1].z = 0.0f;

        face[2].x = 0.0f;
        face[2].y = 0.0f;
        face[2].z = 0.0f;

        face[3].x = 0.0f;
        face[3].y = 0.0f;
        face[3].z = 0.0f;

        espSetUserMeshFace(1, boundary->GetNumPoints(), face);
    }

    return priority;
#endif
}

// TODO move?
static const int NumLoadingPriorities = 3;

void TrackStreamer::AssignLoadingPriority() {
    if (RemoteCaffeinating && TrackStreamerRemoteCaffeinating) {
        espEmptyLayer("LoadingPriorityLoaded");
        for (int priority = 0; priority < NumLoadingPriorities; priority++) {
            char layer_name[32];
            bSPrintf(layer_name, "LoadingPriority%d", priority);
            espEmptyLayer(layer_name);
        }
    }

    for (int n = 0; n < NumCurrentStreamingSections; n++) {
        TrackStreamingSection *section = CurrentStreamingSections[n];
        int best_priority = 99;
        for (int position_number = 0; position_number < 2; position_number++) {
            StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
            if ((section->CurrentlyVisible >> position_number) & 1) {
                {
                    int priority = GetLoadingPriority(section, position_entry, false);
                    if (priority < best_priority) {
                        best_priority = priority;
                    }
                }
            }
        }

        section->LoadingPriority = best_priority * 100000 + section->SectionPriority;
    }
}

void TrackStreamer::CalculateLoadingBacklog() {
    float loading_backlog = 0.0f;
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        // TODO
        if (section->CurrentlyVisible && (section->Status - TrackStreamingSection::LOADED > 1U)) {
            // TODO get rid of temp
            int rounded_size = section->Size;
            if (rounded_size < 0) {
                rounded_size += 0x3ff;
            }

            float time = static_cast<float>(rounded_size >> 10) * 0.0004f + 0.15f;
            if (section->BaseLoadingPriority == 1) {
                time *= 0.4f;
            }
            if (section->BaseLoadingPriority == 2) {
                time *= 0.2f;
            }
            loading_backlog += time;
        }
    }

    LoadingBacklog = loading_backlog;
}

void TrackStreamer::StartLoadingSections() {
    bool something_to_load = true;
    while (NumSectionsLoading < 2 && something_to_load) {
        int best_priority = 0x7FFFFFFF;
        TrackStreamingSection *best_section = nullptr;
        for (int i = 0; i < NumCurrentStreamingSections; i++) {
            TrackStreamingSection *section = CurrentStreamingSections[i];
            if (section->Status == TrackStreamingSection::ALLOCATED) {
                int priority = section->LoadingPriority;
                if (section->pDiscBundle) {
                    priority = -1;
                }
                if (priority < best_priority) {
                    best_priority = priority;
                    best_section = section;
                }
            } else if (section->Status == TrackStreamingSection::LOADED && !NeedsGameStateActivation(section)) {
                TheTrackStreamer.ActivateSection(section);
            }
        }

        if (!best_section) {
            something_to_load = false;
        } else {
            if (best_section->pDiscBundle) {
                DiscBundleSection *disc_bundle = best_section->pDiscBundle;
                LoadDiscBundle(disc_bundle);
            } else {
                LoadSection(best_section);
            }
        }
    }
}

void TrackStreamer::FinishedLoading() {
    {
        float load_time;
        int position_number;
        StreamingPositionEntry *position_entry;
    }

    LoadingPhase = LOADING_IDLE;
    CurrentZoneNonReplayLoad = false;
    CurrentZoneFarLoad = false;
    NotifySkyLoader();

    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        if (position_entry->BeginLoadingTime != 0.0f) {
            PlotLoadingMarker(position_entry);
        }
        position_entry->BeginLoadingTime = 0.0f;
    }

    if (pCallback) {
        SetDelayedResourceCallback(pCallback, CallbackParam);
        pCallback = nullptr;
        CallbackParam = 0;
    }
}

void TrackStreamer::PlotLoadingMarker(StreamingPositionEntry *streaming_position) {
    if (RemoteCaffeinating && TrackStreamerRemoteCaffeinating && streaming_position->CurrentZone > 0 &&
        streaming_position->BeginLoadingTime != 0.0f) {
        float load_time = GetDebugRealTime();
        unsigned int obj =
            ::PlotLoadingMarker("TrackStreamingLoadedZone", &streaming_position->BeginLoadingPosition, &streaming_position->Position, load_time);
        char text[32];
        bSPrintf(text, "%d/%d", streaming_position->NumSectionsLoaded, streaming_position->NumSectionsToLoad);
        espSetAttributeString(obj, "NumSectionsLoaded", text);
        bSPrintf(text, "%d/%d", streaming_position->AmountLoaded / 1024, streaming_position->AmountToLoad / 1024);
        espSetAttributeString(obj, "AmountLoaded", text);
    }
}

#ifdef EA_BUILD_A124
void TrackStreamer::CheckLoadingBar() {}
#else
// UNSOLVED regswap
bool TrackStreamer::CheckLoadingBar() {
    ProfileNode profile_node("TODO", 0);
    float closest_distance = 999.0f;
    TrackStreamingSection *closest_section;
    StreamingPositionEntry *closest_position_entry;
    float closest_approach_speed;

    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        float speed;
        float max_speed;
        if (!IsLoadingInProgress()) {
            break;
        }

        if (IsFarLoadingInProgress() || !position_entry->PositionSet || !position_entry->FollowingCar) {
            break;
        }

        speed = bLength(&position_entry->Velocity);
        max_speed = MPH2MPS(400.0f);
        if (speed > max_speed) {
            break;
        }

        for (int n = 0; n < NumCurrentStreamingSections; n++) {
            TrackStreamingSection *section = CurrentStreamingSections[n];
            VisibleSectionBoundary *boundary = section->pBoundary;

            if (boundary) {
                bool may_contain_road = false;
                if (IsRegularScenerySection(section->SectionNumber)) {
                    if (IsScenerySectionDrivable(section->SectionNumber) || IsLODScenerySectionNumber(section->SectionNumber)) {
                        may_contain_road = true;
                    }
                }

                if (may_contain_road && section->Status != TrackStreamingSection::ACTIVATED) {
                    // TODO make const somehow
                    float small_test_time = 0.2f;
                    bVector2 test_pos = position_entry->Position + position_entry->Velocity * 0.01f;
                    float distance1 = boundary->GetDistanceOutside(&position_entry->Position, 999.0f);
                    float distance2 = boundary->GetDistanceOutside(&test_pos, 999.0f);
                    float approach_speed = small_test_time * (distance1 - distance2) * 100.0f;
                    float distance = distance1 - approach_speed;
                    if (distance < closest_distance) {
                        closest_distance = distance;
                        closest_section = section;
                        closest_position_entry = position_entry;
                        closest_approach_speed = approach_speed;
                    }
                }
            }
        }
    }

    bool need_loading_bar = closest_distance < 5.0f;

    static bool prev_need_loading_bar = false;

    prev_need_loading_bar = need_loading_bar;
    return need_loading_bar;
}
#endif

int TrackStreamer::GetSectionToActivate(int activation_delay) {
    if (NumSectionsActivated < NumCurrentStreamingSections) {
        for (int n = 0; n < NumCurrentStreamingSections; n++) {
            TrackStreamingSection *section = CurrentStreamingSections[n];
            if (section->Status == TrackStreamingSection::LOADED && TheTrackStreamer.NeedsGameStateActivation(section) &&
                RealTimeFrames - section->LoadedTime >= activation_delay) {
                return section->SectionNumber;
            }
        }
    }

    return 0;
}

void TrackStreamer::HandleSectionActivation() {
    ProfileNode profile_node("TODO", 0);
    int activation_delay;
    short section_to_activate = static_cast<short>(GetSectionToActivate(0));
    if (section_to_activate != 0) {
        TrackStreamingSection *section = FindSection(section_to_activate);
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

void TrackStreamer::UnloadEverything() {
    while (NumSectionsLoading != 0) {
        ServiceResourceLoading();
    }

    for (int n = 0; n < NumTrackStreamingSections; n++) {
        TrackStreamingSection *section = &pTrackStreamingSections[n];
        // TODO
        if (static_cast<unsigned int>(section->Status - TrackStreamingSection::LOADED) < 2U) {
            UnloadSection(section);
        }
    }

    FreeSectionMemory();
    ClearCurrentZones();
}
