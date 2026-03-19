#include "TrackStreamer.hpp"

#include "TrackPath.hpp"
#include "VisibleSection.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/PPCArch.h"
#include "dolphin/os/OSCache.h"

#include <algorithm>

extern BOOL bMemoryTracing;
extern int ScenerySectionLODOffset;
extern int SeeulatorToolActive;
extern int ScenerySectionToBlink;
extern int RealLoopCounter;
extern bool PostLoadFixupDisabled;
extern int AllowDuplicateSolids;
extern int ForceHoleFillerMethod;
extern int WaitForFrameBufferSwapDisabled;
extern int WaitUntilRenderingDoneDisabled;
extern int TrackStreamerRemoteCaffeinating;
extern unsigned int eFrameCounter;
int Get2PlayerSectionNumber(int section_number);
void GetScenerySectionName(char *name, int section_number);
char *GetScenerySectionName(int section_number);
void PostLoadFixup();
void SetDuplicateTextureWarning(BOOL enabled);
bool LoadTempPermChunks(bChunk **ppchunks, int *psizeof_chunks, int allocation_params, const char *debug_name);
bool DoLinesIntersect(const bVector2 &line1_start, const bVector2 &line1_end, const bVector2 &line2_start, const bVector2 &line2_end);
void eWaitUntilRenderingDone();
void MoveChunks(bChunk *dest_chunks, bChunk *source_chunks, int sizeof_chunks, const char *debug_name);
void bSetMemoryPoolOverrideInfo(int pool_num, MemoryPoolOverrideInfo *override_info);
void UnloadChunks(bChunk *chunks, int sizeof_chunks, const char *debug_name);
void SetQueuedFileMinPriority(int priority);
void SetDelayedResourceCallback(void (*callback)(int), int param);
void NotifySkyLoader();
void BlockWhileQueuedFileBusy();
int GetBoundarySectionNumber(int section_number, const char *platform_name);
int LoaderTrackStreamer(bChunk *chunk);
int UnloaderTrackStreamer(bChunk *chunk);
extern int QueuedFileDefaultPriority;

static inline char *bGetPlatformName() {
    return "GAMECUBE";
}

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
static VisibleSectionBitTable CurrentVisibleSectionTableMem;
TrackStreamer TheTrackStreamer;
bChunkLoader bChunkLoaderTrackStreamingSection(0x34110, LoaderTrackStreamer, UnloaderTrackStreamer);
bChunkLoader bChunkLoaderTrackStreamingDiscBundle(0x34113, LoaderTrackStreamer, UnloaderTrackStreamer);
bChunkLoader bChunkLoaderTrackStreamingInfo(0x34111, LoaderTrackStreamer, UnloaderTrackStreamer);
bChunkLoader bChunkLoaderTrackStreamingBarriers(0x34112, LoaderTrackStreamer, UnloaderTrackStreamer);

static inline char GetScenerySectionLetter_TrackStreamer(int section_number) {
    return static_cast<char>(section_number / 100 + 'A' - 1);
}

static inline bool IsRegularScenerySection_TrackStreamer(int section_number) {
    char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
    return section_letter >= 'A' && section_letter < 'U';
}

static inline bool IsTextureSection_TrackStreamer(int section_number) {
    char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
    return section_letter == 'Y' || section_letter == 'W';
}

static inline bool IsLibrarySection_TrackStreamer(int section_number) {
    char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
    return section_letter == 'X' || section_letter == 'U';
}

static inline bool IsTextureSection(int section_number) {
    char section_letter = GetScenerySectionLetter(section_number);
    return section_letter == 'Y' || section_letter == 'W';
}

static inline bool IsLibrarySection(int section_number) {
    char section_letter = GetScenerySectionLetter(section_number);
    return section_letter == 'X' || section_letter == 'U';
}

static inline short GetScenerySectionNumber_TrackStreamer(char section_letter, int subsection_number) {
    return static_cast<short>((section_letter - 'A' + 1) * 100 + subsection_number);
}

static inline bool IsLODScenerySectionNumber(int section_number) {
    int subsection_number = GetScenerySubsectionNumber(section_number);
    return subsection_number >= ScenerySectionLODOffset && subsection_number < ScenerySectionLODOffset * 2;
}

static inline bool IsLoadingBarSection_TrackStreamer(int section_number) {
    if (!IsRegularScenerySection_TrackStreamer(section_number)) {
        return false;
    }

    int subsection_number = section_number % 100;
    return (subsection_number > 0 && subsection_number < ScenerySectionLODOffset) ||
           (ScenerySectionLODOffset <= subsection_number && subsection_number < ScenerySectionLODOffset * 2);
}

static inline void DisableWaitForFrameBufferSwap() {
    WaitForFrameBufferSwapDisabled = 1;
}

static inline void EnableWaitForFrameBufferSwap() {
    WaitForFrameBufferSwapDisabled = 0;
}

static inline void eAllowDuplicateSolids(bool enable) {
    if (enable) {
        AllowDuplicateSolids += 1;
    } else {
        AllowDuplicateSolids -= 1;
    }
}

bool DoLinesIntersect(
    const bVector2 &line1_start, const bVector2 &line1_end, const bVector2 &line2_start, const bVector2 &line2_end
) {
    float dy1 = line1_end.y - line1_start.y;
    float dx2 = line2_end.x - line2_start.x;
    float dx1 = line1_end.x - line1_start.x;
    float dy2 = line2_end.y - line2_start.y;
    float den = dx1 * dy2 - dy1 * dx2;

    if (den != 0.0f) {
        float dx3 = line1_start.x - line2_start.x;
        float dy3 = line1_start.y - line2_start.y;
        float r = (dy3 * dx2 - dx3 * dy2) / den;
        if (0.0f <= r && r <= 1.0f) {
            float s = (dy3 * dx1 - dx3 * dy1) / den;
            if (0.0f <= s && s <= 1.0f) {
                return true;
            }
        }
    }

    return false;
}

inline bool TrackStreamingBarrier::Intersects(const bVector2 *pointa, const bVector2 *pointb) {
    return DoLinesIntersect(Points[0], Points[1], *pointa, *pointb);
}

struct bBitTableLayout_TrackStreamer {
    int NumBits;
    uint8 *Bits;
};

void bBitTable::ClearTable() {
    bMemSet(Bits, 0, NumBits >> 3);
}

struct bMemoryTraceAllocatePacket {
    int PoolID;
    int MemoryAddress;
    int Size;
    int DebugLine;
    int AllocationNumber;
    char DebugText[48];
};

TSMemoryPool::TSMemoryPool(int address, int size, const char *debug_name, int pool_num) {
    PoolNum = pool_num;
    DebugName = debug_name;
    TotalSize = size;
    TracingEnabled = true;
    Updated = false;
    AllocationNumber = 0;
    AmountFree = size;
    LargestFree = size;
    NeedToRecalcLargestFree = false;

    for (int i = 0; i < 192; i++) {
        UnusedNodeList.AddTail(&MemoryNodes[i]);
    }

    if (TracingEnabled && bMemoryTracing) {
        bMemoryTraceNewPoolPacket packet;
        packet.PoolID = reinterpret_cast<uintptr_t>(this);
        bMemSet(packet.Name, 0, sizeof(packet.Name));
        bStrNCpy(packet.Name, debug_name, sizeof(packet.Name) - 1);
        bFunkCallASync("CODEINE", 0x19, &packet, sizeof(packet));
    }

    GetNewNode(address, size, false, 0);

    if (TracingEnabled && bMemoryTracing) {
        bMemoryTraceFreePacket packet;
        bMemSet(&packet, 0, sizeof(packet));
        packet.PoolID = reinterpret_cast<uintptr_t>(this);
        packet.MemoryAddress = static_cast<uintptr_t>(address);
        packet.Size = size;
        bFunkCallASync("CODEINE", 0x1b, &packet, sizeof(packet));
    }

    bMemSet(&OverrideInfo, 0, sizeof(OverrideInfo));
    OverrideInfo.Name = DebugName;
    OverrideInfo.Pool = this;
    OverrideInfo.Address = address;
    OverrideInfo.Size = size;
    OverrideInfo.Malloc = OverrideMalloc;
    OverrideInfo.Free = OverrideFree;
    OverrideInfo.GetAmountFree = OverrideGetAmountFree;
    OverrideInfo.GetLargestFreeBlock = OverrideGetLargestFreeBlock;
    bSetMemoryPoolOverrideInfo(PoolNum, &OverrideInfo);
}

TSMemoryNode *TSMemoryPool::GetNewNode(int address, int size, bool allocated, const char *debug_name) {
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

inline bool TSMemoryNode::IsFree() {
    return !Allocated;
}

inline bool TSMemoryNode::Contains(int address) {
    return address >= Address && address < Address + Size;
}

void *TSMemoryPool::Malloc(int size, const char *debug_name, bool best_fit, bool allocate_from_top, int address) {
    TSMemoryNode *found_node = 0;
    TSMemoryNode *new_node;
    int new_bottom_size;
    int new_top_size;

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
            if (node->IsFree() && node->Size >= size &&
                (!found_node || found_node->Size - size > node->Size - size)) {
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
    new_node->AddAfter(found_node);

    new_bottom_size = address - found_node->Address;
    new_top_size = found_node->Address + found_node->Size - (address + size);
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
        bMemoryTraceAllocatePacket send_packet;
        bMemoryTraceAllocatePacket packet;
        bMemoryTraceAllocatePacket *fake_match = &packet;
        int extra_len;
        int n;
        unsigned char *p;

        memset(fake_match, 0, sizeof(packet));
        packet.PoolID = reinterpret_cast<int>(this);
        packet.MemoryAddress = address;
        packet.Size = size;
        packet.AllocationNumber = AllocationNumber;
        send_packet = packet;
        p = reinterpret_cast<unsigned char *>(send_packet.DebugText);
        n = sizeof(send_packet.DebugText);
        bMemSet(p, 0, n);
        if (debug_name) {
            bStrNCpy(reinterpret_cast<char *>(p), debug_name, n - 1);
        }
        extra_len = bStrLen(reinterpret_cast<char *>(p)) + 0x15;
        bFunkCallASync("CODEINE", 0x1c, &send_packet, extra_len);
    }

    Updated = true;
    AllocationNumber += 1;
    return reinterpret_cast<void *>(address);
}

void *TSMemoryPool::OverrideMalloc(void *pool, int size, const char *debug_text, int debug_line, int allocation_params) {
    TSMemoryPool *memory_pool = static_cast<TSMemoryPool *>(pool);
    unsigned int align_offset = static_cast<unsigned int>(allocation_params >> 17) & 0x1FFC;
    bool best_fit = (allocation_params & 0x80) != 0;
    bool allocate_from_top = (allocation_params & 0x40) != 0;
    (void)debug_line;

    if (align_offset == 0) {
        return memory_pool->Malloc(size, debug_text, best_fit, allocate_from_top, 0);
    }

    int memory = reinterpret_cast<int>(memory_pool->Malloc(size + 0x80, debug_text, best_fit, allocate_from_top, 0));
    return reinterpret_cast<void *>((memory + 0x80) - align_offset);
}

void TSMemoryPool::OverrideFree(void *pool, void *ptr) {
    static_cast<TSMemoryPool *>(pool)->Free(
        reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) & ~static_cast<uintptr_t>(0x7F)));
}

int TSMemoryPool::OverrideGetAmountFree(void *pool) {
    return static_cast<TSMemoryPool *>(pool)->GetAmountFree();
}

int TSMemoryPool::OverrideGetLargestFreeBlock(void *pool) {
    return static_cast<TSMemoryPool *>(pool)->GetLargestFreeBlock();
}

int TSMemoryPool::GetAmountFree() {
    for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
    }
    return AmountFree;
}

int TSMemoryPool::GetLargestFreeBlock() {
    if (NeedToRecalcLargestFree) {
        int largest_free = 0;
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (node->Allocated != 1 && node->Size > largest_free) {
                largest_free = node->Size;
            }
        }
        LargestFree = largest_free;
        NeedToRecalcLargestFree = false;
    }

    int largest_free = 0;
    for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
        if (node->Allocated != 1 && node->Size > largest_free) {
            largest_free = node->Size;
        }
    }
    return LargestFree;
}

void TSMemoryPool::Free(void *memory) {
    int address = reinterpret_cast<int>(memory);
    Updated = true;

    for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
        if (node->Address == address) {
            int size = node->Size;
            TSMemoryNode *prev_node = node->GetPrev();

            node->DebugName[0] = 0;
            node->Allocated = false;
            if (prev_node != NodeList.EndOfList() && prev_node->IsFree()) {
                node->Address = address - prev_node->Size;
                node->Size = size + prev_node->Size;
                RemoveNode(prev_node);
            }

            TSMemoryNode *next_node = node->GetNext();
            if (next_node != NodeList.EndOfList() && next_node->IsFree()) {
                node->Size += next_node->Size;
                RemoveNode(next_node);
            }

            AmountFree += size;
            if (LargestFree < node->Size) {
                LargestFree = node->Size;
            }

            if (TracingEnabled && bMemoryTracing) {
                bMemoryTraceFreePacket packet;
                bMemSet(&packet, 0, sizeof(packet));
                packet.PoolID = reinterpret_cast<uintptr_t>(this);
                packet.MemoryAddress = static_cast<uintptr_t>(address);
                packet.Size = size;
                bFunkCallASync("CODEINE", 0x1b, &packet, sizeof(packet));
            }
            return;
        }
    }
}

TSMemoryNode *TSMemoryPool::GetNextNode(bool start_from_top, TSMemoryNode *node) {
    TSMemoryNode *next_node;
    if (start_from_top) {
        next_node = node ? node->GetNext() : NodeList.GetHead();
    } else {
        next_node = node ? node->GetPrev() : NodeList.GetTail();
    }

    if (next_node == NodeList.EndOfList()) {
        return 0;
    }
    return next_node;
}

TSMemoryNode *TSMemoryPool::GetNextFreeNode(bool start_from_top, TSMemoryNode *node) {
    while ((node = GetNextNode(start_from_top, node)) != 0) {
        if (!node->Allocated) {
            return node;
        }
    }
    return 0;
}

TSMemoryNode *TSMemoryPool::GetNextAllocatedNode(bool start_from_top, TSMemoryNode *node) {
    while ((node = GetNextNode(start_from_top, node)) != 0) {
        if (node->Allocated) {
            return node;
        }
    }
    return 0;
}

void TSMemoryPool::DebugPrint() {
    for (TSMemoryNode *node = reinterpret_cast<TSMemoryNode *>(NodeList.GetHead());
         node != reinterpret_cast<TSMemoryNode *>(&NodeList); node = reinterpret_cast<TSMemoryNode *>(node->GetNext())) {
    }
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

bool TrackStreamer::WillUnloadBlock(TrackStreamingSection *section) {
    unsigned int frame = section->UnactivatedFrameCount;
    if ((frame != 0) && (frame == eFrameCounter)) {
        if (LastWaitUntilRenderingDoneFrameCount != frame) {
            return true;
        }
    }
    return false;
}

void TrackStreamer::UnloadSection(TrackStreamingSection *section) {
    if (section->Status == TrackStreamingSection::ACTIVATED) {
        UnactivateSection(section);
    }

    if (section->Status == TrackStreamingSection::LOADED) {
        if (WillUnloadBlock(section)) {
            WaitForFrameBufferSwapDisabled = 1;
            eWaitUntilRenderingDone();
            WaitForFrameBufferSwapDisabled = 0;
            LastWaitUntilRenderingDoneFrameCount = eFrameCounter;
        }

        section->UnactivatedFrameCount = 0;
        bFree(section->pMemory);
        section->Status = TrackStreamingSection::UNLOADED;
        section->pMemory = 0;
        section->LoadedTime = 0;
        NumSectionsLoaded -= 1;
    }
}

int TrackStreamer::UnloadLeastRecentlyUsedSection() {
    TrackStreamingSection *best_section = 0;

    for (int i = 0; i < NumTrackStreamingSections; i++) {
        TrackStreamingSection *section = &pTrackStreamingSections[i];
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
    JettisonedSections[NumJettisonedSections] = section;
    NumJettisonedSections += 1;

    if (section->Status == TrackStreamingSection::ACTIVATED) {
        UnactivateSection(section);
    }
    if (section->Status == TrackStreamingSection::LOADED) {
        UnloadSection(section);
    }

    section->CurrentlyVisible = false;

    int index = 0;
    while (CurrentStreamingSections[index] != section) {
        index += 1;
    }

    while (index < NumCurrentStreamingSections - 1) {
        CurrentStreamingSections[index] = CurrentStreamingSections[index + 1];
        index += 1;
    }

    NumCurrentStreamingSections -= 1;
}

bool TrackStreamer::JettisonLeastImportantSection() {
    TrackStreamingSection *section = ChooseSectionToJettison();
    if (section) {
        JettisonSection(section);
    }
    return section != 0;
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

TrackStreamingSection *TrackStreamer::FindSection(int section_number) {
    for (int i = 0; i < NumTrackStreamingSections; i++) {
        TrackStreamingSection *section = &pTrackStreamingSections[i];
        if (section->SectionNumber == static_cast<short>(section_number)) {
            return section;
        }
    }
    return 0;
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
        if (largest_free >= 0) {
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
            break;
        }

        HoleMovement *movement = &hole_movements[num_movements];
        movement->Address = 0;

        if (filler_method == 2 || filler_method == 0 || filler_method == 3) {
            bool start_from_top = filler_method == 3;
            TSMemoryNode *free_node = pMemoryPool->GetFirstFreeNode(start_from_top);
            TSMemoryNode *node = pMemoryPool->GetNextAllocatedNode(start_from_top, free_node);
            if (filler_method == 0 && !node) {
                break;
            }

            if (node && free_node) {
                movement->Size = node->Size;
                movement->Address = node->Address;
                movement->NewAddress = free_node->GetAddress(start_from_top, movement->Size);
                if (filler_method == 0 && !FindSectionByAddress(movement->Address)) {
                    break;
                }
            }
        } else if (filler_method == 4 || filler_method == 5) {
            bool start_from_top = filler_method == 5;
            TSMemoryNode *free_node = pMemoryPool->GetFirstFreeNode(start_from_top);
            int best_hole_size = 0;
            bool first = true;

            for (TSMemoryNode *next_node = pMemoryPool->GetNextAllocatedNode(start_from_top, free_node); next_node;
                 next_node = pMemoryPool->GetNextAllocatedNode(start_from_top, next_node)) {
                TSMemoryNode *next_free = pMemoryPool->GetNextFreeNode(start_from_top, next_node);
                if (!next_free) {
                    continue;
                }
                if (first || next_node->Size <= free_node->Size) {
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
                        first = false;
                    }
                }
            }
        } else if (filler_method == 1) {
            bool done = false;
            bool found_one = false;
            bool found_big_enough = false;
            TSMemoryNode *largest_allocated = 0;
            int current_best = 0;
            int current_best_middle_memory = 0x3E8000;
            int best_address = 0;
            bool first_pass = true;
            TSMemoryNode *top_free_top = 0;

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
                    int top_free_memory = top_free_top->Size;
                    TSMemoryNode *bottom_free_top = pMemoryPool->GetNextFreeNode(true, top_free_top);
                    if (!bottom_free_top) {
                        done = true;
                    } else {
                        TSMemoryNode *top_allocated = pMemoryPool->GetNextNode(true, top_free_top);
                        pMemoryPool->GetNextNode(false, bottom_free_top);

                        int middle_allocated_memory = top_allocated->Size;
                        int total_free_memory = top_free_memory + bottom_free_top->Size;
                        int size_checking[32];
                        int i = 0;
                        do {
                            size_checking[i] = 0;
                            i += 1;
                        } while (i < 32);

                        size_checking[0] = top_allocated->Size;

                        TSMemoryNode *largest_allocated_here = top_allocated;
                        TSMemoryNode *cursor = top_allocated;
                        int found_nodes = 1;
                        while ((cursor = pMemoryPool->GetNextNode(true, top_allocated)) != bottom_free_top && top_allocated &&
                               found_nodes < 32) {
                            top_allocated = pMemoryPool->GetNextNode(true, top_allocated);
                            if (top_allocated) {
                                size_checking[found_nodes] = top_allocated->Size;
                                middle_allocated_memory += top_allocated->Size;
                                found_nodes += 1;
                                if (top_allocated->Size > largest_allocated_here->Size) {
                                    largest_allocated_here = top_allocated;
                                }
                            }
                        }

                        int free_gap = total_free_memory - middle_allocated_memory;
                        if ((!found_big_enough && current_best < free_gap) ||
                            (found_big_enough &&
                             total_free_memory + middle_allocated_memory >= total_needing_allocation &&
                             middle_allocated_memory < current_best_middle_memory)) {
                            std::sort(size_checking, size_checking + found_nodes);
                            int evaluated_best_address = 0;
                            bool largest_flag = false;
                            int nodes_to_move = 0;
                            int position = 0;

                            TSMemoryNode *evaluated_top_free = pMemoryPool->GetFirstFreeNode(true);
                            while (found_nodes > nodes_to_move && evaluated_top_free) {
                                bool skip_flag = false;
                                int target_index = found_nodes - nodes_to_move - 1;
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
                                    nodes_to_move += 1;
                                    if (!largest_flag) {
                                        evaluated_best_address = evaluated_top_free->Address;
                                        largest_flag = true;
                                    }
                                    evaluated_top_free = pMemoryPool->GetNextFreeNode(true, 0);
                                }
                                evaluated_top_free = pMemoryPool->GetNextFreeNode(true, evaluated_top_free);
                                position += 1;
                            }

                            if (nodes_to_move >= found_nodes) {
                                current_best = free_gap;
                                best_address = evaluated_best_address;
                                found_one = true;
                                largest_allocated = largest_allocated_here;
                                if (total_free_memory + middle_allocated_memory >= total_needing_allocation) {
                                    found_big_enough = true;
                                    current_best_middle_memory = middle_allocated_memory;
                                }
                            }
                        }
                    }
                }
            } while (!done);

            if (found_one && largest_allocated && FindSectionByAddress(largest_allocated->Address)) {
                movement->Size = largest_allocated->Size;
                movement->Address = largest_allocated->Address;
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

    for (int n = num_movements - 1; n >= 0; n--) {
        HoleMovement *movement = &hole_movements[n];
        pMemoryPool->Free(reinterpret_cast<void *>(movement->NewAddress));
        TrackStreamingSection *section = FindSectionByAddress(movement->Address);
        char *debug_name;
        if (section) {
            debug_name = section->SectionName;
        } else {
            debug_name = "UndoHoleMovement";
        }
        pMemoryPool->Malloc(movement->Size, debug_name, false, false, movement->Address);
    }

    if (pamount_moved) {
        *pamount_moved = amount_moved;
    }

    pMemoryPool->EnableTracing(true);
    if (failed) {
        return -1;
    }
    return num_movements;
}

TrackStreamer::TrackStreamer() {
    pTrackStreamingSections = 0;
    NumTrackStreamingSections = 0;
    pDiscBundleSections = 0;
    pLastDiscBundleSection = 0;
    pInfo = 0;
    NumBarriers = 0;
    pBarriers = 0;
    NumSectionsLoaded = 0;
    NumSectionsLoading = 0;
    NumSectionsActivated = 0;
    NumSectionsOutOfMemory = 0;
    NumSectionsMoved = 0;
    bMemSet(StreamFilenames, 0, sizeof(StreamFilenames));
    SplitScreen = false;
    PermFileLoading = false;
    PermFilename = 0;
    PermFileChunks = 0;
    PermFileSize = 0;
    CurrentZoneNeedsRefreshing = false;
    ZoneSwitchingDisabled = false;
    LastWaitUntilRenderingDoneFrameCount = 0;
    LastPrintedFrameCount = 0;
    SkipNextHandleLoad = false;
    NumCurrentStreamingSections = 0;
    NumHibernatingSections = 0;

    ClearCurrentZones();
    ClearStreamingPositions();

    bBitTableLayout_TrackStreamer *layout = reinterpret_cast<bBitTableLayout_TrackStreamer *>(&CurrentVisibleSectionTable);
    layout->NumBits = 0xAF0;
    layout->Bits = CurrentVisibleSectionTableMem.Bits;
    CurrentVisibleSectionTable.ClearTable();
    bMemSet(KeepSectionTable, 0, sizeof(KeepSectionTable));
    pCallback = 0;
    CallbackParam = 0;
    MakeSpaceInPoolCallback = 0;
    MakeSpaceInPoolCallbackParam = 0;
    MakeSpaceInPoolSize = 0;
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

void TrackStreamer::InitMemoryPool(int size) {
    MemoryPoolSize = size;
    pMemoryPoolMem = bMalloc(size, 0x2000);
    pMemoryPool = new TSMemoryPool(reinterpret_cast<int>(pMemoryPoolMem), MemoryPoolSize, "Track Streaming", 7);
}

int TrackStreamer::GetMemoryPoolSize() {
    if (pMemoryPool->IsUpdated()) {
        UserMemoryAllocationSize = CountUserAllocations(0);
    }
    return MemoryPoolSize - UserMemoryAllocationSize;
}

int TrackStreamer::CountUserAllocations(const char **pfragmented_user_allocation) {
    if (pfragmented_user_allocation) {
        *pfragmented_user_allocation = 0;
    }

    int total = 0;
    bool start_from_top = false;
    for (TSMemoryNode *node = pMemoryPool->GetNextAllocatedNode(start_from_top, 0); node;
         node = pMemoryPool->GetNextAllocatedNode(start_from_top, node)) {
        if (!FindSectionByAddress(node->Address)) {
            total += node->Size;
            if (pMemoryPool->GetNextFreeNode(start_from_top, node) && pMemoryPool->GetNextFreeNode(!start_from_top, node) &&
                pfragmented_user_allocation) {
                *pfragmented_user_allocation = node->DebugName;
            }
        }
    }

    return total;
}

int TrackStreamer::DoHoleFilling(int largest_free) {
    ProfileNode profile_node("TODO", 0);
    const char *debug_name;
    HoleMovement hole_movements[128];

    CountUserAllocations(&debug_name);
    if (debug_name) {
        pMemoryPool->DebugPrint();
        return 0;
    }

    int best_method = -1;
    int forced_hole_filler_method = ForceHoleFillerMethod;
    if (forced_hole_filler_method >= 0) {
        int num_movements =
            BuildHoleMovements(hole_movements, 0x80, forced_hole_filler_method, largest_free, 0, 0x7FFFFFFF);
        if (num_movements > 0) {
            best_method = forced_hole_filler_method;
        }
    } else {
        int best_amount_moved = 0x7FFFFFFF;
        for (int filler_method = 1; filler_method < 6; filler_method++) {
            int amount_moved;
            int num_movements =
                BuildHoleMovements(hole_movements, 0x80, filler_method, largest_free, &amount_moved, best_amount_moved);
            if (num_movements > 0 && amount_moved < best_amount_moved) {
                best_method = filler_method;
                best_amount_moved = amount_moved;
            }
        }
    }

    if (best_method < 0) {
        return 0;
    }

    int num_movements = BuildHoleMovements(hole_movements, 0x80, best_method, largest_free, 0, 0x7FFFFFFF);
    for (int i = 0; i < num_movements; i++) {
        ProfileNode profile_node("TODO", 0);
        HoleMovement *movement = &hole_movements[i];
        TrackStreamingSection *section = FindSectionByAddress(movement->Address);
        if (LastWaitUntilRenderingDoneFrameCount != eGetFrameCounter()) {
            unsigned int start_ticks = bGetTicker();
            DisableWaitForFrameBufferSwap();
            eWaitUntilRenderingDone();
            LastWaitUntilRenderingDoneFrameCount = eGetFrameCounter();
            EnableWaitForFrameBufferSwap();
            float time = bGetTickerDifference(start_ticks);
            (void)time;
        }

        unsigned int start_ticks = bGetTicker();
        void *new_memory = reinterpret_cast<void *>(movement->NewAddress);
        pMemoryPool->Free(reinterpret_cast<void *>(movement->Address));
        pMemoryPool->Malloc(movement->Size, section->SectionName, false, false, movement->NewAddress);
        if (section->Status == TrackStreamingSection::ACTIVATED) {
            eAllowDuplicateSolids(true);
            SetDuplicateTextureWarning(false);
            MoveChunks(reinterpret_cast<bChunk *>(new_memory), reinterpret_cast<bChunk *>(section->pMemory),
                       section->LoadedSize, section->SectionName);
            DCStoreRangeNoSync(new_memory, section->LoadedSize);
            eAllowDuplicateSolids(false);
            SetDuplicateTextureWarning(true);
        } else {
            eWaitUntilRenderingDone();
            bOverlappedMemCpy(new_memory, section->pMemory, section->LoadedSize);
        }
        section->pMemory = new_memory;
        float move_time = bGetTickerDifference(start_ticks);
        (void)move_time;
        NumSectionsMoved += 1;
        PPCSync();
    }

    return 1;
}

void TrackStreamer::ClearCurrentZones() {
    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        position_entry->AmountLoaded = 0;
        position_entry->CurrentZone = 0;
        position_entry->BeginLoadingTime = 0.0f;
        position_entry->BeginLoadingPosition.x = 0.0f;
        position_entry->BeginLoadingPosition.y = 0.0f;
        position_entry->NumSectionsToLoad = 0;
        position_entry->NumSectionsLoaded = 0;
        position_entry->AmountToLoad = 0;
    }

    CurrentZoneFarLoad = true;
    StartLoadingTime = 0.0f;
    NumJettisonedSections = 0;
    LoadingPhase = LOADING_IDLE;
    LoadingBacklog = 0.0f;
    CurrentZoneOutOfMemory = false;
    CurrentZoneAllocatedButIncomplete = false;
    CurrentZoneNonReplayLoad = false;
    CurrentZoneName[0] = 0;
    MemorySafetyMargin = 0;
    AmountJettisoned = 0;
    bMemSet(JettisonedSections, 0, sizeof(JettisonedSections));
    RemoveCurrentStreamingSections();
}

void TrackStreamer::RemoveCurrentStreamingSections() {
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        CurrentStreamingSections[i]->CurrentlyVisible = 0;
    }

    NumCurrentStreamingSections = 0;
    CurrentVisibleSectionTable.ClearTable();
}

void TrackStreamer::AddCurrentStreamingSections(short *section_numbers, int num_sections, int position_number) {
    if (num_sections <= 0) {
        return;
    }

    StreamingPositionEntry *streaming_position = &StreamingPositionEntries[position_number];
    for (int i = 0; i < num_sections; i++) {
        short &section_number = section_numbers[i];
        CurrentVisibleSectionTable.Set(section_number);
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
    short sections_to_load[384];
    int num_sections_to_load = 3;

    RemoveCurrentStreamingSections();

    sections_to_load[0] = 0x9c4;
    sections_to_load[1] = 0x960;
    sections_to_load[2] = 0xa28;
    if (SeeulatorToolActive && ScenerySectionToBlink != 0) {
        num_sections_to_load = 4;
        sections_to_load[3] = static_cast<short>(ScenerySectionToBlink);
    }

    for (int keep_section_number = 0; keep_section_number < 4; keep_section_number++) {
        short section_number = KeepSectionTable[keep_section_number];
        if (section_number != 0) {
            sections_to_load[num_sections_to_load] = section_number;
            num_sections_to_load += 1;
        }
    }

    AddCurrentStreamingSections(sections_to_load, num_sections_to_load, 0);
    int position_number = 0;
    AddCurrentStreamingSections(sections_to_load, num_sections_to_load, 1);
    do {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        if (position_entry->CurrentZone > 0) {
            LoadingSection *loading_section = TheVisibleSectionManager.FindLoadingSection(position_entry->CurrentZone);
            if (!loading_section) {
                DrivableScenerySection *drivable_section = TheVisibleSectionManager.FindDrivableSection(position_entry->CurrentZone);
                num_sections_to_load = 0;
                for (int visible_section_number = 0; visible_section_number < drivable_section->NumVisibleSections;
                     visible_section_number++) {
                    short section_number = drivable_section->VisibleSections[visible_section_number];
                    sections_to_load[num_sections_to_load] = section_number;
                    num_sections_to_load += 1;
                }
            } else {
                num_sections_to_load = TheVisibleSectionManager.GetSectionsToLoad(loading_section, sections_to_load, 0x180);
            }

            AddCurrentStreamingSections(sections_to_load, num_sections_to_load, position_number);
        }
        position_number += 1;
    } while (position_number < 2);
}

void TrackStreamer::InitRegion(const char *region_stream_filename, bool split_screen) {
    bool flush_hibernating_sections = false;

    if (SplitScreen != split_screen) {
        SplitScreen = split_screen;
        flush_hibernating_sections = true;
    }
    if (!bStrEqual(StreamFilenames[0], region_stream_filename)) {
        bStrCpy(StreamFilenames[0], region_stream_filename);
        flush_hibernating_sections = true;
    }
    if (flush_hibernating_sections) {
        FlushHibernatingSections();
    }
    if (PermFileLoading) {
        BlockWhileQueuedFileBusy();
    }

    ClearCurrentZones();
    ClearStreamingPositions();

    {
        int position_number = 0;
        do {
            StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];

            position_entry->AudioBlockingPosition.y = 0.0f;
            position_entry->PredictedZone = 0;
            position_entry->PredictedZoneValidTime = 0;
            position_entry->AudioReading = false;
            position_entry->AudioReadingTime = 0.0f;
            position_entry->AudioReadingPosition.x = 0.0f;
            position_entry->AudioReadingPosition.y = 0.0f;
            position_entry->AudioBlocking = false;
            position_entry->AudioBlockingTime = 0.0f;
            position_entry->AudioBlockingPosition.x = 0.0f;
            position_number += 1;
        } while (position_number < 2);
    }

    if (NumTrackStreamingSections > 0) {
        int n = 0;
        do {
            TrackStreamingSection *section = &pTrackStreamingSections[n];
            int boundary_section_number = GetBoundarySectionNumber(static_cast<int>(section->SectionNumber), bGetPlatformName());
            VisibleSectionBoundary *boundary = TheVisibleSectionManager.FindBoundary(boundary_section_number);

            section->pBoundary = boundary;
            n += 1;
        } while (n < NumTrackStreamingSections);
    }

    EmptyCaffeineLayers();
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
                    distance_to_boundary = bMin(distance_to_boundary, boundary_distance);
                }
            }

            if (kSwitchZoneFarLoadThreshold_TrackStreamer < distance_to_boundary) {
                CurrentZoneFarLoad = true;
            }

            streaming_position->CurrentZone = current_zone;
            streaming_position->BeginLoadingPosition.x = streaming_position->Position.x;
            streaming_position->BeginLoadingPosition.y = streaming_position->Position.y;
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

int TrackStreamer::Unloader(bChunk *chunk) {
    unsigned int chunk_id = chunk->GetID();
    if (chunk_id == 0x34110) {
        UnloadEverything();
        pTrackStreamingSections = 0;
        NumTrackStreamingSections = 0;
        return 1;
    }

    if (chunk_id == 0x34113) {
        pDiscBundleSections = 0;
        pLastDiscBundleSection = 0;
        return 1;
    }

    if (chunk_id == 0x34111) {
        pInfo = 0;
        return 1;
    }

    if (chunk_id == 0x34112) {
        pBarriers = 0;
        NumBarriers = 0;
        return 1;
    }

    return 0;
}

void TrackStreamer::HibernateStreamingSections() {
    volatile char unused[0x30];
    (void)unused;
}

void TrackStreamer::FlushHibernatingSections() {
    for (int n = 0; n < NumHibernatingSections; n++) {
        TrackStreamingSection *section = &HibernatingSections[n];
        bFree(section->pMemory);
    }
    NumHibernatingSections = 0;
}

bool TrackStreamer::NeedsGameStateActivation(TrackStreamingSection *section) {
    return false;
}

void TrackStreamer::FreeSectionMemory() {
    NumSectionsOutOfMemory = 0;
    for (int i = 0; i < NumTrackStreamingSections; i++) {
        TrackStreamingSection *section = &pTrackStreamingSections[i];
        if (section->Status == TrackStreamingSection::ALLOCATED) {
            bFree(section->pMemory);
            section->pDiscBundle = 0;
            section->pMemory = 0;
            section->Status = TrackStreamingSection::UNLOADED;
        }
    }
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

int TrackStreamer::GetCombinedSectionNumber(int section_number) {
    bool use_combined_section = false;
    if ((static_cast<unsigned int>(section_number / 100 - 1) & 0xFF) < 0x14) {
        int subsection_number = section_number % 100;
        use_combined_section = subsection_number > 0 && subsection_number < ScenerySectionLODOffset;
    }

    if (use_combined_section) {
        int combined_section_number = section_number + ScenerySectionLODOffset;
        TrackStreamingSection *section = FindSection(section_number);
        if (!section) {
            section = FindSection(combined_section_number);
            if (section) {
                return combined_section_number;
            }
        }
    }

    return section_number;
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

void TrackStreamer::UnloadEverything() {
    while (NumSectionsLoaded != 0) {
        ServiceResourceLoading();
    }

    for (int i = 0; i < NumTrackStreamingSections; i++) {
        TrackStreamingSection *section = &pTrackStreamingSections[i];
        if (static_cast<unsigned int>(section->Status - TrackStreamingSection::LOADED) < 2U) {
            UnloadSection(section);
        }
    }

    FreeSectionMemory();
    ClearCurrentZones();
}

void TrackStreamer::ActivateSection(TrackStreamingSection *section) {
    NumSectionsActivated += 1;
    AllowDuplicateSolids += 1;
    SetDuplicateTextureWarning(false);

    bChunk *chunks = reinterpret_cast<bChunk *>(section->pMemory);
    int sizeof_chunks = section->LoadedSize;
    LoadTempPermChunks(&chunks, &sizeof_chunks, 0x2087, section->SectionName);

    section->LoadedTime = 0;
    section->pMemory = chunks;
    section->LoadedSize = sizeof_chunks;
    section->Status = TrackStreamingSection::ACTIVATED;
    AllowDuplicateSolids -= 1;
    SetDuplicateTextureWarning(true);
}

void TrackStreamer::UnactivateSection(TrackStreamingSection *section) {
    section->UnactivatedFrameCount = 0;
    WaitUntilRenderingDoneDisabled = 1;
    section->UnactivatedFrameCount = eFrameCounter;
    UnloadChunks(reinterpret_cast<bChunk *>(section->pMemory), section->LoadedSize, section->SectionName);
    WaitUntilRenderingDoneDisabled = 0;
    NumSectionsActivated -= 1;
    section->Status = TrackStreamingSection::LOADED;
}

void TrackStreamer::LoadDiscBundle(DiscBundleSection *disc_bundle) {
    void *memory = 0;
    for (int i = 0; i < disc_bundle->NumMembers; i++) {
        DiscBundleSectionMember *member = &disc_bundle->Members[i];
        TrackStreamingSection *section = member->pSection;
        if (i == 0) {
            memory = section->pMemory;
        }
        section->Status = TrackStreamingSection::LOADING;
    }

    NumSectionsLoading += 1;
    AddQueuedFile(memory, StreamFilenames[1], disc_bundle->FileOffset, disc_bundle->FileSize, DiscBundleLoadedCallback,
                  reinterpret_cast<int>(disc_bundle), 0);
}

void TrackStreamer::DiscBundleLoadedCallback(int param, int error_status) {
    (void)error_status;
    TheTrackStreamer.DiscBundleLoadedCallback(reinterpret_cast<DiscBundleSection *>(param));
}

void TrackStreamer::DiscBundleLoadedCallback(DiscBundleSection *disc_bundle) {
    NumSectionsLoading += -1 + disc_bundle->NumMembers;
    for (int i = 0; i < disc_bundle->NumMembers; i++) {
        DiscBundleSectionMember *member = &disc_bundle->Members[i];
        TrackStreamingSection *section = member->pSection;
        section->pDiscBundle = 0;
        SectionLoadedCallback(section);
    }
}

void TrackStreamer::LoadSection(TrackStreamingSection *section) {
    NumSectionsLoading += 1;
    section->Status = TrackStreamingSection::LOADING;

    if (section->CompressedSize == section->Size) {
        AddQueuedFile(section->pMemory, StreamFilenames[section->FileType], section->FileOffset, section->CompressedSize,
                      SectionLoadedCallback,
                      reinterpret_cast<int>(section), 0);
    } else {
        QueuedFileParams params;
        params.BlockSize = 0x7fffffff;
        params.Priority = QueuedFileDefaultPriority;
        params.Compressed = false;
        params.Compressed = true;
        params.UncompressedSize = section->Size;
        AddQueuedFile(section->pMemory, StreamFilenames[section->FileType], section->FileOffset, section->CompressedSize,
                      SectionLoadedCallback, reinterpret_cast<int>(section), &params);
    }
}

void TrackStreamer::SectionLoadedCallback(int param, int error_status) {
    (void)error_status;
    TheTrackStreamer.SectionLoadedCallback(reinterpret_cast<TrackStreamingSection *>(param));
}

void TrackStreamer::SectionLoadedCallback(TrackStreamingSection *section) {
    section->Status = TrackStreamingSection::LOADED;
    section->LoadedSize = section->Size;
    EndianSwapChunkHeadersRecursive(reinterpret_cast<bChunk *>(section->pMemory), section->Size);
    NumSectionsLoading -= 1;
    NumSectionsLoaded += 1;
    section->LoadedTime = RealTimeFrames;

    if (section->CurrentlyVisible && !NeedsGameStateActivation(section)) {
        ActivateSection(section);
    }

    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        if (((section->CurrentlyVisible >> position_number) & 1) != 0) {
            position_entry->NumSectionsLoaded += 1;
            position_entry->AmountLoaded += section->Size;
        }
    }

    CalculateLoadingBacklog();
    DCStoreRange(section->pMemory, section->LoadedSize);
}

TrackStreamingSection *TrackStreamer::ChooseSectionToJettison() {
    TrackStreamingSection *best_section = 0;
    int best_discard_priority = 0;

    last_jettison_print_26154 = RealLoopCounter;
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        short section_number = section->SectionNumber;
        int discard_priority = 0;

        if (IsTextureSection_TrackStreamer(section_number) || IsLibrarySection_TrackStreamer(section_number)) {
            discard_priority = 2;
            if (section_number == GetScenerySectionNumber_TrackStreamer('Y', 0) ||
                section_number == GetScenerySectionNumber_TrackStreamer('W', 0) ||
                section_number == GetScenerySectionNumber_TrackStreamer('X', 0)) {
                discard_priority = 1;
            } else if (LoadingPhase == ALLOCATING_TEXTURE_SECTIONS) {
                if (IsTextureSection_TrackStreamer(section_number) && section->Status == TrackStreamingSection::ACTIVATED &&
                    !SplitScreen) {
                    discard_priority = 10000;
                }
            } else if (LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS) {
                if (IsLibrarySection_TrackStreamer(section_number) && section->Status == TrackStreamingSection::ACTIVATED &&
                    !SplitScreen) {
                    discard_priority = 10000;
                }
            }
        } else if (IsRegularScenerySection_TrackStreamer(section_number)) {
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
            if (static_cast<unsigned int>(section->Status - TrackStreamingSection::LOADED) > 1) {
                discard_priority += 1;
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
    for (int i = 0; i < NumJettisonedSections; i++) {
        TrackStreamingSection *section = JettisonedSections[i];
        CurrentStreamingSections[NumCurrentStreamingSections] = section;
        NumCurrentStreamingSections += 1;
        section->CurrentlyVisible = true;
    }
    NumJettisonedSections = 0;
    AmountJettisoned = 0;
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

        do {
            do {
                FreeSectionMemory();
                out_of_memory_size = AllocateSectionMemory(&total_needing_allocation);
                if (out_of_memory_size == 0) {
                    goto done;
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
            } while (amount_unloaded > 0);

            {
                int amount_hole_filled;

                while (bCountFreeMemory(7) < total_needing_allocation + 0x4000) {
                    CurrentZoneOutOfMemory = true;
                    if (!JettisonLeastImportantSection()) {
                        break;
                    }
                    AllocateSectionMemory(&total_needing_allocation);
                    FreeSectionMemory();
                }

                amount_hole_filled = DoHoleFilling(0);
                total_amount_hole_filled += amount_hole_filled;
                if (amount_hole_filled != 0) {
                    continue;
                }
            }

            CurrentZoneOutOfMemory = true;
        } while (JettisonLeastImportantSection());

        out_of_memory_size = AllocateSectionMemory(&total_needing_allocation);
    }

done:
    MemorySafetyMargin = 0;

    {
        int free_memory = bCountFreeMemory(7) - (out_of_memory_size + AmountJettisoned);
        {
            int n = 0;

            if (NumTrackStreamingSections > 0) {
                do {
                    TrackStreamingSection *section = &pTrackStreamingSections[n];
                    if (!section->CurrentlyVisible && section->Status != TrackStreamingSection::UNLOADED) {
                        free_memory += section->PermSize;
                    }
                    n += 1;
                } while (n < NumTrackStreamingSections);
            }
        }
        MemorySafetyMargin = free_memory;
    }

    if (out_of_memory_size + AmountJettisoned != 0) {
        {
            int n = 0;

            if (NumJettisonedSections > 0) {
                do {
                    n += 1;
                } while (n < NumJettisonedSections);
            }
        }

        if (LoadingPhase == LOADING_REGULAR_SECTIONS) {
            int i = 0;

            if (NumCurrentStreamingSections > 0) {
                do {
                    TrackStreamingSection *section;
                    i += 1;
                } while (i < NumCurrentStreamingSections);
            }
        }
    }

    return true;
}

void TrackStreamer::StartLoadingSections() {
    bool keep_loading = true;
    while (NumSectionsLoading < 2 && keep_loading) {
        TrackStreamingSection *section_to_load = 0;
        int best_loading_priority = 0x7FFFFFFF;
        for (int i = 0; i < NumCurrentStreamingSections; i++) {
            TrackStreamingSection *section = CurrentStreamingSections[i];
            if (section->Status == TrackStreamingSection::ALLOCATED) {
                int loading_priority = section->LoadingPriority;
                if (section->pDiscBundle) {
                    loading_priority = -1;
                }
                if (loading_priority < best_loading_priority) {
                    section_to_load = section;
                    best_loading_priority = loading_priority;
                }
            } else if (section->Status == TrackStreamingSection::LOADED && !NeedsGameStateActivation(section)) {
                ActivateSection(section);
            }
        }

        if (!section_to_load) {
            keep_loading = false;
        } else if (!section_to_load->pDiscBundle) {
            LoadSection(section_to_load);
        } else {
            LoadDiscBundle(section_to_load->pDiscBundle);
        }
    }
}

void TrackStreamer::FinishedLoading() {
    float load_time;
    (void)load_time;

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
        pCallback = 0;
        CallbackParam = 0;
    }
}

void TrackStreamer::EmptyCaffeineLayers() {
    TrackStreamerRemoteCaffeinating = 0;
}

void TrackStreamer::HandleLoading() {
    if (SkipNextHandleLoad) {
        SkipNextHandleLoad = false;
    } else {
        if (LoadingPhase != LOADING_IDLE) {
            if ((LoadingPhase == LOADING_TEXTURE_SECTIONS || LoadingPhase == LOADING_GEOMETRY_SECTIONS ||
                 LoadingPhase == LOADING_REGULAR_SECTIONS) &&
                (StartLoadingSections(), NumSectionsLoading == 0)) {
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

            if ((LoadingPhase == ALLOCATING_TEXTURE_SECTIONS || LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS ||
                 LoadingPhase == ALLOCATING_REGULAR_SECTIONS) &&
                NumSectionsLoading < 1) {
                int num_sections_unactivated = 0;
                for (int n = 0; n < NumTrackStreamingSections; n++) {
                    TrackStreamingSection *section = &pTrackStreamingSections[n];
                    if (section->Status == TrackStreamingSection::ACTIVATED && !section->CurrentlyVisible) {
                        if (LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS && IsTextureSection(section->SectionNumber)) {
                            num_sections_unactivated += 1;
                            UnactivateSection(section);
                        } else if (LoadingPhase == ALLOCATING_REGULAR_SECTIONS && IsLibrarySection(section->SectionNumber)) {
                            num_sections_unactivated += 1;
                            UnactivateSection(section);
                        }
                    }
                }

                if (num_sections_unactivated > 0) {
                    SkipNextHandleLoad = true;
                } else {
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
                }
            }
        }
    }
}

int TrackStreamer::GetLoadingPriority(TrackStreamingSection *section, StreamingPositionEntry *position_entry, bool calculating_jettison) {
    VisibleSectionBoundary *boundary = section->pBoundary;
    if (!boundary) {
        return 0;
    }

    float speed = bLength(&position_entry->Velocity);
    if (calculating_jettison) {
        speed = 100.0f;
    }

    if (speed < 1.0f) {
        return 0;
    }

    bVector2 predict_pos = position_entry->Position + position_entry->Velocity;
    float distance = boundary->GetDistanceOutside(&predict_pos, 999.0f);

    bVector2 direction;
    if (calculating_jettison) {
        direction = bNormalize(position_entry->Direction);
    } else {
        direction = bNormalize(position_entry->Velocity);
    }

    bVector2 v = section->Centre - predict_pos;
    float dot = bDot(&direction, &bNormalize(v));
    float speed_factor = bMin(speed * 0.016666668f, 1.0f);
    float angle = bAngToDeg(bACos(dot));
    float angle_factor = bClamp(angle, 20.0f, 90.0f);
    float adjusted_distance = distance * (1.0f - (90.0f - angle_factor) * 0.014285714f * speed_factor * 0.66999996f);
    int priority = static_cast<int>(adjusted_distance * 0.013333334f);

    return bClamp(priority, 0, 2);
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

bool TrackStreamer::CheckLoadingBar() {
    ProfileNode profile_node("TODO", 0);
    float closest_distance = kMaxDistance_TrackStreamer;
    TrackStreamingSection *closest_section;
    StreamingPositionEntry *closest_position_entry;
    float closest_approach_speed;
    bool need_loading_bar;

    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        float speed;
        float max_speed;
        float prediction_scale_a = kPredictionScaleA_TrackStreamer;
        float prediction_scale_b = kPredictionScaleB_TrackStreamer;

        if (!IsLoadingInProgress()) {
            break;
        }

        if (IsFarLoadingInProgress() || !position_entry->PositionSet || !position_entry->FollowingCar) {
            break;
        }

        speed = bLength(&position_entry->Velocity);
        max_speed = MPH2MPS(kLoadingBarSpeedThreshold_TrackStreamer);
        if (speed > max_speed) {
            break;
        }

        for (int n = 0; n < NumCurrentStreamingSections; n++) {
            TrackStreamingSection *section = CurrentStreamingSections[n];
            VisibleSectionBoundary *boundary = section->pBoundary;

            if (boundary) {
                bool may_contain_road = false;
                if (IsRegularScenerySection(section->SectionNumber)) {
                    if (IsScenerySectionDrivable(section->SectionNumber) ||
                        IsLODScenerySectionNumber(section->SectionNumber)) {
                        may_contain_road = true;
                    }
                }

                if (may_contain_road && section->Status != TrackStreamingSection::ACTIVATED) {
                    const float small_test_time = kFuturePositionScale_TrackStreamer;
                    bVector2 test_pos = position_entry->Position + position_entry->Velocity * small_test_time;
                    float distance1 = boundary->GetDistanceOutside(&position_entry->Position, kMaxDistance_TrackStreamer);
                    float distance2 = boundary->GetDistanceOutside(&test_pos, kMaxDistance_TrackStreamer);
                    float approach_speed = (distance1 - distance2) * prediction_scale_a * prediction_scale_b;
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

    need_loading_bar = closest_distance < kLoadingBarDistanceThreshold_TrackStreamer;
    prev_need_loading_bar_26275 = need_loading_bar;
    return need_loading_bar;
}

void *TrackStreamer::AllocateUserMemory(int size, const char *debug_name, int offset) {
    (void)debug_name;

    int allocation_params;
    if (bLargestMalloc(7) < size) {
        allocation_params = (offset & 0x1FFC) << 17 | 0x2000;
    } else {
        allocation_params = (offset & 0x1FFC) << 17 | 0x2047;
    }
    return bMalloc(size, allocation_params);
}

void TrackStreamer::FreeUserMemory(void *mem) {
    int free_before = pMemoryPool->GetAmountFree();
    bFree(mem);
    int size = pMemoryPool->GetAmountFree();
    (void)free_before;
    (void)size;
}

bool TrackStreamer::IsUserMemory(void *mem) {
    int pos = static_cast<char *>(mem) - static_cast<char *>(pMemoryPoolMem);
    if (pMemoryPoolMem && pos >= 0) {
        return pos < MemoryPoolSize;
    }
    return false;
}

bool TrackStreamer::MakeSpaceInPool(int size, bool force_unloading) {
    WaitForCurrentLoadingToComplete();
    while (bCountFreeMemory(7) < size) {
        int amount_unloaded = UnloadLeastRecentlyUsedSection();
        if (amount_unloaded == 0 && (!force_unloading || !JettisonLeastImportantSection())) {
            break;
        }
    }

    ForceHoleFillerMethod = 0;
    DoHoleFilling(0x7FFFFFFF);
    ForceHoleFillerMethod = -1;
    return size <= bLargestMalloc(7);
}

void TrackStreamer::MakeSpaceInPool(int size, void (*callback)(int), int param) {
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

    void (*callback)(int) = MakeSpaceInPoolCallback;
    int param = MakeSpaceInPoolCallbackParam;
    MakeSpaceInPoolCallback = 0;
    MakeSpaceInPoolCallbackParam = 0;
    MakeSpaceInPoolSize = 0;
    callback(param);
}

void TrackStreamer::ReadyToMakeSpaceInPoolBridge(int param) {
    reinterpret_cast<TrackStreamer *>(param)->ReadyToMakeSpaceInPool();
}

short TrackStreamer::GetPredictedZone(StreamingPositionEntry *position_entry) {
    float speed = bLength(&position_entry->Velocity);
    int predicted_zone = 0;
    bool found_predicted_zone = false;
    TrackPathZone *zone = 0;
    bVector2 predict_position;

    while ((zone = TheTrackPathManager.FindZone(&position_entry->Position, TRACK_PATH_ZONE_STREAMER_PREDICTION, zone))) {
        float elevation = zone->GetElevation();
        if ((0.0f < elevation && position_entry->Elevation < elevation) || (elevation < 0.0f && -elevation < position_entry->Elevation)) {
            continue;
        }

        float max_speed = kPredictedZoneStopProjectSpeed_TrackStreamer * kPredictedZoneScale_TrackStreamer;
        float distance = speed * kPredictedZoneScale_TrackStreamer;
        DrivableScenerySection *scenery_section;
        if (max_speed < speed) {
            predict_position = position_entry->Position;
        } else if (kPredictedZoneMaxDistance_TrackStreamer < distance) {
            bScaleAdd(&predict_position, &position_entry->Position, &position_entry->Velocity,
                      kPredictedZoneMaxDistance_TrackStreamer / speed);
        } else {
            bScaleAdd(&predict_position, &position_entry->Position, &position_entry->Velocity,
                      kPredictedZoneScale_TrackStreamer);
        }

        scenery_section = TheVisibleSectionManager.FindDrivableSection(&predict_position);
        if (scenery_section && zone->Data[0] != 0) {
            short section_number = scenery_section->SectionNumber;
            for (int i = 0; i <= 3; i++) {
                if (zone->Data[i] == 0) {
                    break;
                }
                if (zone->Data[i] == section_number) {
                    found_predicted_zone = true;
                    predicted_zone = section_number;
                    break;
                }
            }
        }
    }

    if (found_predicted_zone) {
        if (!bEqual(&predict_position, &position_entry->Position, kPredictedZoneEqualEpsilon_TrackStreamer)) {
            for (int barrier_num = 0; barrier_num < NumBarriers; barrier_num++) {
                TrackStreamingBarrier *barrier = &pBarriers[barrier_num];
                if (barrier->Intersects(&position_entry->Position, &predict_position)) {
                    found_predicted_zone = false;
                    predicted_zone = 0;
                }
            }
        }

        if (found_predicted_zone) {
            return predicted_zone;
        }
    }

    DrivableScenerySection *scenery_section = TheVisibleSectionManager.FindDrivableSection(&position_entry->Position);
    if (scenery_section) {
        predicted_zone = scenery_section->SectionNumber;
    }
    return predicted_zone;
}

void TrackStreamer::ClearStreamingPositions() {
    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        position_entry->PositionSet = false;
        position_entry->FollowingCar = false;
    }
}

void TrackStreamer::SetStreamingPosition(int position_number, const bVector3 *position) {
    StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
    position_entry->Position.x = position->x;
    position_entry->Position.y = position->y;
    position_entry->PredictedZone = 0;
    position_entry->Elevation = position->z;
    position_entry->Direction.y = 0.0f;
    position_entry->PredictedZoneValidTime = -1;
    position_entry->Velocity.x = 0.0f;
    position_entry->Velocity.y = 0.0f;
    position_entry->Direction.x = 0.0f;
    position_entry->PositionSet = true;
    position_entry->FollowingCar = false;
    CurrentZoneNeedsRefreshing = true;
}

void TrackStreamer::PredictStreamingPosition(
    int position_number, const bVector3 *position, const bVector3 *velocity, const bVector3 *direction, bool following_car
) {
    StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
    position_entry->Position.x = position->x;
    position_entry->Position.y = position->y;
    position_entry->Elevation = position->z;
    position_entry->Velocity.x = velocity->x;
    position_entry->Velocity.y = velocity->y;
    position_entry->Direction.x = direction->x;
    position_entry->FollowingCar = following_car;
    position_entry->Direction.y = direction->y;
    position_entry->PositionSet = true;
}

bool TrackStreamer::DetermineCurrentZones(short *current_zones) {
    bool changed = false;
    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        short current_zone = -1;
        if (position_entry->PositionSet) {
            current_zone = GetPredictedZone(position_entry);
        }

        if (current_zone == position_entry->PredictedZone) {
            position_entry->PredictedZoneValidTime += 1;
        } else if (position_entry->PredictedZoneValidTime == -1) {
            position_entry->PredictedZone = current_zone;
            position_entry->PredictedZoneValidTime = 1000;
        } else {
            position_entry->PredictedZone = current_zone;
            position_entry->PredictedZoneValidTime = 1;
        }

        short section_number = position_entry->CurrentZone;
        if (current_zone != section_number) {
            if (position_entry->PredictedZoneValidTime < 0) {
                current_zone = section_number;
            }
            if (current_zone != section_number) {
                changed = true;
            }
        }

        current_zones[position_number] = current_zone;
    }

    return changed || CurrentZoneNeedsRefreshing;
}

void TrackStreamer::ServiceGameState() {
    GetDebugRealTime();
    HandleZoneSwitching();
    HandleSectionActivation();
    GetDebugRealTime();

    AmountNotRendered = 0;
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        if (!section->WasRendered && IsRegularScenerySection_TrackStreamer(section->SectionNumber)) {
            AmountNotRendered += section->Size;
        }
        section->WasRendered = 0;
    }
}

void TrackStreamer::ServiceNonGameState() {
    GetDebugRealTime();
    HandleLoading();
    GetDebugRealTime();
}

void TrackStreamer::SetLoadingPhase(eLoadingPhase phase) {
    LoadingPhase = phase;
    if (phase == LOADING_IDLE || phase == LOADING_REGULAR_SECTIONS) {
        SetQueuedFileMinPriority(0);
    } else {
        SetQueuedFileMinPriority(QueuedFileDefaultPriority);
    }
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

void TrackStreamer::RefreshLoading() {
    CurrentZoneNeedsRefreshing = true;
    for (int position_number = 0; position_number < 2; position_number++) {
        StreamingPositionEntry *position_entry = &StreamingPositionEntries[position_number];
        position_entry->PredictedZoneValidTime = -1;
    }
    HandleZoneSwitching();
}

void TrackStreamer::HandleZoneSwitching() {
    if (!ZoneSwitchingDisabled && pMemoryPoolMem) {
        short current_zones[4];
        if (DetermineCurrentZones(current_zones)) {
            SwitchZones(current_zones);
        }
    }
}
