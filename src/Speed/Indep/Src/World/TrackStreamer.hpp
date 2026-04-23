#ifndef WORLD_TRACKSTREAMER_H
#define WORLD_TRACKSTREAMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "TrackPath.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

enum eTrackStreamingFileType {
    TRACK_STREAMING_FILE_PERMANENT = 0,
    TRACK_STREAMING_FILE_REGION = 1,
    NUM_TRACK_STREAMING_FILE_TYPES = 2,
};

struct DiscBundleSectionMember {
    // total size: 0x8
    int16 SectionNumber;                    // offset 0x0, size 0x2
    int16 FileOffset;                       // offset 0x2, size 0x2
    struct TrackStreamingSection *pSection; // offset 0x4, size 0x4
};

// total size: 0x114
struct DiscBundleSection {
    int GetMemoryImageSize() {
        return NumMembers * sizeof(DiscBundleSectionMember) + 0x14;
    }

    DiscBundleSection *GetMemoryImageNext() {
        return reinterpret_cast<DiscBundleSection *>(reinterpret_cast<char *>(this) + GetMemoryImageSize());
    }

    int32 FileOffset;                    // offset 0x0, size 0x4
    int32 FileSize;                      // offset 0x4, size 0x4
    char SectionName[11];                // offset 0x8, size 0xB
    int8 NumMembers;                     // offset 0x13, size 0x1
    DiscBundleSectionMember Members[32]; // offset 0x14, size 0x100
};

// total size: 0x5C
struct TrackStreamingSection {
    enum eStatus {
        UNLOADED = 0,
        ALLOCATED = 1,
        LOADING = 2,
        LOADED = 3,
        ACTIVATED = 4,
    };
    char SectionName[8];              // offset 0x0, size 0x8
    int16 SectionNumber;              // offset 0x8, size 0x2
    int8 WasRendered;                 // offset 0xA, size 0x1
    int8 CurrentlyVisible;            // offset 0xB, size 0x1
    eStatus Status;                   // offset 0xC, size 0x4
    eTrackStreamingFileType FileType; // offset 0x10, size 0x4
    int32 FileOffset;                 // offset 0x14, size 0x4
    int32 Size;                       // offset 0x18, size 0x4
    int32 CompressedSize;             // offset 0x1C, size 0x4
    int32 PermSize;                   // offset 0x20, size 0x4
    int32 SectionPriority;            // offset 0x24, size 0x4
    bVector2 Centre;                  // offset 0x28, size 0x8
    float Radius;                     // offset 0x30, size 0x4
    uint32 Checksum;                  // offset 0x34, size 0x4
    int32 LastNeededTimestamp;        // offset 0x38, size 0x4
    uint32 UnactivatedFrameCount;     // offset 0x3C, size 0x4
    int32 LoadedTime;                 // offset 0x40, size 0x4
    int32 BaseLoadingPriority;        // offset 0x44, size 0x4
    int32 LoadingPriority;            // offset 0x48, size 0x4
    void *pMemory;                    // offset 0x4C, size 0x4
    DiscBundleSection *pDiscBundle;   // offset 0x50, size 0x4
    int32 LoadedSize;                 // offset 0x54, size 0x4
#ifndef EA_BUILD_A124
    struct VisibleSectionBoundary *pBoundary; // offset 0x58, size 0x4
#endif
};

// total size: 0x68
struct StreamingPositionEntry {
    bVector2 Position;  // offset 0x0, size 0x8
    float Elevation;    // offset 0x8, size 0x4
    bVector2 Velocity;  // offset 0xC, size 0x8
    bVector2 Direction; // offset 0x14, size 0x8
    bool PositionSet;   // offset 0x1C, size 0x1
#ifndef EA_BUILD_A124
    bool FollowingCar; // offset 0x20, size 0x1
#endif
    int16 CurrentZone;              // offset 0x24, size 0x2
    int16 PredictedZone;            // offset 0x26, size 0x2
    int32 PredictedZoneValidTime;   // offset 0x28, size 0x4
    bVector2 BeginLoadingPosition;  // offset 0x2C, size 0x8
    float BeginLoadingTime;         // offset 0x34, size 0x4
    int NumSectionsToLoad;          // offset 0x38, size 0x4
    int NumSectionsLoaded;          // offset 0x3C, size 0x4
    int AmountToLoad;               // offset 0x40, size 0x4
    int AmountLoaded;               // offset 0x44, size 0x4
    bool AudioReading;              // offset 0x48, size 0x1
    float AudioReadingTime;         // offset 0x4C, size 0x4
    bVector2 AudioReadingPosition;  // offset 0x50, size 0x8
    bool AudioBlocking;             // offset 0x58, size 0x1
    float AudioBlockingTime;        // offset 0x5C, size 0x4
    bVector2 AudioBlockingPosition; // offset 0x60, size 0x8
};

// total size: 0x34
class TSMemoryNode : public bTNode<TSMemoryNode> {
  public:
    intptr_t Address;   // offset 0x8, size 0x4
    int32 Size;         // offset 0xC, size 0x4
    bool Allocated;     // offset 0x10, size 0x1
    char DebugName[32]; // offset 0x14, size 0x20

    bool IsFree() {
        return !Allocated;
    }

    bool IsAllocated() {
        return Allocated;
    }

    bool Contains(intptr_t address) {
        return address >= Address && address < Address + Size;
    }

    int GetAddress(bool start_from_top, int size) {
        if (start_from_top) {
            return Address;
        }
        return Address + Size - size;
    }
};

// total size: 0x2754
class TSMemoryPool {
  public:
    TSMemoryPool(intptr_t address, int size, const char *debug_name, int pool_num);
    void *Malloc(int size, const char *debug_name, bool best_fit, bool allocate_from_top, intptr_t address);
    void Free(void *ptr);
    int GetAmountFree();
    int GetLargestFreeBlock();
    TSMemoryNode *GetNextNode(bool start_from_top, TSMemoryNode *node);

    TSMemoryNode *GetFirstNode(bool start_from_top) {
        return GetNextNode(start_from_top, nullptr);
    }

    TSMemoryNode *GetNextFreeNode(bool start_from_top, TSMemoryNode *node);

    TSMemoryNode *GetFirstFreeNode(bool start_from_top) {
        return GetNextFreeNode(start_from_top, nullptr);
    }

    TSMemoryNode *GetNextAllocatedNode(bool start_from_top, TSMemoryNode *node);

    TSMemoryNode *GetFirstAllocatedNode(bool start_from_top) {
        return GetNextAllocatedNode(start_from_top, 0);
    }

    bool IsUpdated() {
        bool updated = Updated;
        Updated = false;
        return updated;
    }

    unsigned int GetPoolChecksum();
    void DebugPrint();

    void EnableTracing(bool enabled) {
        TracingEnabled = enabled;
    }

  private:
    static void *OverrideMalloc(void *pool, int size, const char *debug_text, int debug_line, int allocation_params) {
        int user_alignment_offset = bMemoryGetAlignmentOffset(allocation_params);

        if (user_alignment_offset != 0) {
            char *p = reinterpret_cast<char *>(static_cast<TSMemoryPool *>(pool)->Malloc(
                size + 0x80, debug_text, bMemoryGetBestFit(allocation_params), bMemoryGetTopBit(allocation_params), 0));
            return &p[0x80 - user_alignment_offset];
        }

        return static_cast<TSMemoryPool *>(pool)->Malloc(size, debug_text, bMemoryGetBestFit(allocation_params), bMemoryGetTopBit(allocation_params),
                                                         0);
    }

    static void OverrideFree(void *pool, void *ptr) {
        static_cast<TSMemoryPool *>(pool)->Free(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) & ~static_cast<uintptr_t>(0x7F)));
    }

    static int OverrideGetAmountFree(void *pool) {
        return static_cast<TSMemoryPool *>(pool)->GetAmountFree();
    }

    static int OverrideGetLargestFreeBlock(void *pool) {
        return static_cast<TSMemoryPool *>(pool)->GetLargestFreeBlock();
    }

    TSMemoryNode *GetNewNode(intptr_t address, int size, bool allocated, const char *debug_name);
    void RemoveNode(TSMemoryNode *node);

    int PoolNum;                         // offset 0x0, size 0x4
    const char *DebugName;               // offset 0x4, size 0x4
    int TotalSize;                       // offset 0x8, size 0x4
    bool TracingEnabled;                 // offset 0xC, size 0x1
    bool Updated;                        // offset 0x10, size 0x1
    int AllocationNumber;                // offset 0x14, size 0x4
    int AmountFree;                      // offset 0x18, size 0x4
    int LargestFree;                     // offset 0x1C, size 0x4
    bool NeedToRecalcLargestFree;        // offset 0x20, size 0x1
    bTList<TSMemoryNode> UnusedNodeList; // offset 0x24, size 0x8
    bTList<TSMemoryNode> NodeList;       // offset 0x2C, size 0x8
    TSMemoryNode MemoryNodes[192];       // offset 0x34, size 0x2700
    MemoryPoolOverrideInfo OverrideInfo; // offset 0x2734, size 0x20
};

struct TrackStreamingInfo {
    int FileSize[2]; // offset 0x0, size 0x8
};

// total size: 0x10
struct TrackStreamingBarrier {
    void EndianSwap() {
        bPlatEndianSwap(&Points[0]);
        bPlatEndianSwap(&Points[1]);
    }

    bool Intersects(const bVector2 *pointa, const bVector2 *pointb) {
        return DoLinesIntersect(Points[0], Points[1], *pointa, *pointb);
    }

    bVector2 Points[2]; // offset 0x0, size 0x10
};

// total size: 0x10
struct HoleMovement {
    intptr_t Address;    // offset 0x0, size 0x4
    intptr_t NewAddress; // offset 0x4, size 0x4
    int32 Size;          // offset 0x8, size 0x4
    uint32 Checksum;     // offset 0xC, size 0x4
};

enum HoleFillerMethod {
    HOLE_FILLER_METHOD_LINEAR_BOTTOM_ALL_THE_WAY = 0,
    HOLE_FILLER_METHOD_SUPER_SCOOPER = 1,
    HOLE_FILLER_METHOD_LINEAR_BOTTOM = 2,
    HOLE_FILLER_METHOD_LINEAR_TOP = 3,
    HOLE_FILLER_METHOD_BIGGEST_FIRST_BOTTOM = 4,
    HOLE_FILLER_METHOD_BIGGEST_FIRST_TOP = 5,
    NUM_HOLE_FILLER_METHODS = 6,
};

// total size: 0x888
class TrackStreamer {
  public:
    TrackStreamer();
    enum eLoadingPhase {
        LOADING_IDLE = 0,
        ALLOCATING_TEXTURE_SECTIONS = 1,
        LOADING_TEXTURE_SECTIONS = 2,
        ALLOCATING_GEOMETRY_SECTIONS = 3,
        LOADING_GEOMETRY_SECTIONS = 4,
        ALLOCATING_REGULAR_SECTIONS = 5,
        LOADING_REGULAR_SECTIONS = 6,
        NUM_LOADING_PHASES = 7,
    };

    int Loader(bChunk *chunk);

    int Unloader(bChunk *chunk);

    void InitMemoryPool(int size);

    void CloseMemoryPool();

    int GetMemoryPoolSize();

    int CountUserAllocations(const char **pfragmented_user_allocation);

    void AddKeepSection(int section_number);

    void RemoveKeepSection(int section_number);

    TrackStreamingSection *FindSection(int section_number);

    TrackStreamingSection *FindSectionByAddress(intptr_t address);

    void InitRegion(const char *region_stream_filename, bool split_screen);

    void StartPermFileLoading(const char *filename);

    void PermFileLoadedCallback();

    void HibernateStreamingSections();

    void FlushHibernatingSections();

    void *AllocateMemory(TrackStreamingSection *section, int allocation_params);

    void ActivateSection(TrackStreamingSection *section);

    void UnactivateSection(TrackStreamingSection *section);

    bool WillUnloadBlock(TrackStreamingSection *section);

    void UnloadSection(TrackStreamingSection *section);

    bool NeedsGameStateActivation(TrackStreamingSection *section);

    void EmptyCaffeineLayers();

    static char *GetLoadingPhaseName(enum eLoadingPhase phase);

    void SetLoadingPhase(enum eLoadingPhase phase);

    const char *GetPrintHeader();

    int UnloadLeastRecentlyUsedSection();

    void JettisonSection(TrackStreamingSection *section);

    bool JettisonLeastImportantSection();

    TrackStreamingSection *ChooseSectionToJettison();

    void UnJettisonSections();

    int BuildHoleMovements(HoleMovement *hole_movements, int max_movements, int filler_method, int largest_free, int *pamount_moved,
                           int max_amount_to_move);

    int DoHoleFilling(int largest_free);

    void SetStreamingPosition(int position_number, const bVector3 *position);

    void SetStreamingPosition(int position_number, const bVector2 *position);

    void PredictStreamingPosition(int position_number, const bVector3 *position, const bVector3 *velocity, const bVector3 *direction,
                                  bool following_car);

    short GetPredictedZone(StreamingPositionEntry *position_entry);

    void ClearStreamingPositions();

    void RemoveCurrentStreamingSections();

    void AddCurrentStreamingSections(short *sections_to_load, int num_sections_to_load, int position_number);

    void DetermineStreamingSections();

    int AllocateSectionMemory(int *ptotal_needing_allocation);

    void FreeSectionMemory();

    bool HandleMemoryAllocation();

    void *AllocateUserMemory(int size, const char *debug_name, int offset);

    void FreeUserMemory(void *mem);

    bool HasMemoryPool() {
        return pMemoryPoolMem != nullptr;
    }

    bool IsUserMemory(void *mem);

    bool MakeSpaceInPool(int size, bool force_unloading);

    void MakeSpaceInPool(int size, void (*callback)(intptr_t), intptr_t param);

    void ReadyToMakeSpaceInPool();

    void ServiceGameState();

    void PrintMemoryPool();

    void ServiceNonGameState();

    void BlockUntilLoadingComplete();

    void WaitForCurrentLoadingToComplete();

    bool IsLoadingInProgress();

    bool AreAllSectionsActivated();

    void RefreshLoading();

    void SetLoadingCallback(void (*callback)(intptr_t), intptr_t param);

    void SwitchZones(short *current_zones);

    void HandleLoading();

    int GetLoadingPriority(TrackStreamingSection *section, StreamingPositionEntry *position_entry, bool calculating_jettison);

    void AssignLoadingPriority();

    void CalculateLoadingBacklog();

    void StartLoadingSections();

    void FinishedLoading();

    void PlotLoadingMarker(StreamingPositionEntry *position_entry);

#ifdef EA_BUILD_A124
    void CheckLoadingBar();
#else
    bool CheckLoadingBar();
#endif

    int GetSectionToActivate(int activation_delay);

    void HandleSectionActivation();

    void UnloadEverything();

    void ForceSectionToUnload(int section_number);

    bool IsFarLoadingInProgress() {
        return CurrentZoneFarLoad && IsLoadingInProgress();
    }

    void DisableZoneSwitching() {
        ZoneSwitchingDisabled = true;
    }

    void EnableZoneSwitching() {
        ZoneSwitchingDisabled = false;
    }

    int IsSectionVisible(int section_number) {
        return CurrentVisibleSectionTable.IsSet(section_number);
    }

    bool IsPermFileLoading() {
        return PermFileLoading;
    }

  private:
    void ClearCurrentZones();
    bool DetermineCurrentZones(short *current_zones);
    void HandleZoneSwitching();
    int GetCombinedSectionNumber(int section_number);
    static void DiscBundleLoadedCallback(intptr_t param, int error_status);

    static void ReadyToMakeSpaceInPoolBridge(intptr_t param) {
        reinterpret_cast<TrackStreamer *>(param)->ReadyToMakeSpaceInPool();
    }

    void DiscBundleLoadedCallback(DiscBundleSection *disc_bundle);
    void LoadDiscBundle(DiscBundleSection *disc_bundle);
    void LoadSection(TrackStreamingSection *section);
    static void SectionLoadedCallback(intptr_t param, int error_status);
    void SectionLoadedCallback(TrackStreamingSection *section);

    TrackStreamingSection *pTrackStreamingSections;       // offset 0x0, size 0x4
    int NumTrackStreamingSections;                        // offset 0x4, size 0x4
    DiscBundleSection *pDiscBundleSections;               // offset 0x8, size 0x4
    DiscBundleSection *pLastDiscBundleSection;            // offset 0xC, size 0x4
    TrackStreamingInfo *pInfo;                            // offset 0x10, size 0x4
    int NumBarriers;                                      // offset 0x14, size 0x4
    TrackStreamingBarrier *pBarriers;                     // offset 0x18, size 0x4
    int NumSectionsLoaded;                                // offset 0x1C, size 0x4
    int NumSectionsLoading;                               // offset 0x20, size 0x4
    int NumSectionsActivated;                             // offset 0x24, size 0x4
    int NumSectionsOutOfMemory;                           // offset 0x28, size 0x4
    int NumSectionsMoved;                                 // offset 0x2C, size 0x4
    char StreamFilenames[2][64];                          // offset 0x30, size 0x80
    bool SplitScreen;                                     // offset 0xB0, size 0x1
    bool PermFileLoading;                                 // offset 0xB4, size 0x1
    const char *PermFilename;                             // offset 0xB8, size 0x4
    bChunk *PermFileChunks;                               // offset 0xBC, size 0x4
    int PermFileSize;                                     // offset 0xC0, size 0x4
    StreamingPositionEntry StreamingPositionEntries[2];   // offset 0xC4, size 0xD0
    eLoadingPhase LoadingPhase;                           // offset 0x194, size 0x4
    float LoadingBacklog;                                 // offset 0x198, size 0x4
    bool CurrentZoneAllocatedButIncomplete;               // offset 0x19C, size 0x1
    bool CurrentZoneOutOfMemory;                          // offset 0x1A0, size 0x1
    bool CurrentZoneNonReplayLoad;                        // offset 0x1A4, size 0x1
    bool CurrentZoneFarLoad;                              // offset 0x1A8, size 0x1
    char CurrentZoneName[8];                              // offset 0x1AC, size 0x8
    float StartLoadingTime;                               // offset 0x1B4, size 0x4
    int MemorySafetyMargin;                               // offset 0x1B8, size 0x4
    int AmountNotRendered;                                // offset 0x1BC, size 0x4
    int AmountJettisoned;                                 // offset 0x1C0, size 0x4
    int NumJettisonedSections;                            // offset 0x1C4, size 0x4
    TrackStreamingSection *JettisonedSections[64];        // offset 0x1C8, size 0x100
    bool CurrentZoneNeedsRefreshing;                      // offset 0x2C8, size 0x1
    bool ZoneSwitchingDisabled;                           // offset 0x2CC, size 0x1
    uint32 LastWaitUntilRenderingDoneFrameCount;          // offset 0x2D0, size 0x4
    uint32 LastPrintedFrameCount;                         // offset 0x2D4, size 0x4
    bool SkipNextHandleLoad;                              // offset 0x2D8, size 0x1
    int NumCurrentStreamingSections;                      // offset 0x2DC, size 0x4
    TrackStreamingSection *CurrentStreamingSections[256]; // offset 0x2E0, size 0x400
    int NumHibernatingSections;                           // offset 0x6E0, size 0x4
    TrackStreamingSection HibernatingSections[4];         // offset 0x6E4, size 0x170
    void *pMemoryPoolMem;                                 // offset 0x854, size 0x4
    int MemoryPoolSize;                                   // offset 0x858, size 0x4
    int UserMemoryAllocationSize;                         // offset 0x85C, size 0x4
    TSMemoryPool *pMemoryPool;                            // offset 0x860, size 0x4
    bBitTable CurrentVisibleSectionTable;                 // offset 0x864, size 0x8
    short KeepSectionTable[4];                            // offset 0x86C, size 0x8
    void (*pCallback)(intptr_t);                          // offset 0x874, size 0x4
    intptr_t CallbackParam;                               // offset 0x878, size 0x4
    void (*MakeSpaceInPoolCallback)(intptr_t);            // offset 0x87C, size 0x4
    intptr_t MakeSpaceInPoolCallbackParam;                // offset 0x880, size 0x4
    int32 MakeSpaceInPoolSize;                            // offset 0x884, size 0x4
};

extern TrackStreamer TheTrackStreamer;

void RefreshTrackStreamer();

#endif
