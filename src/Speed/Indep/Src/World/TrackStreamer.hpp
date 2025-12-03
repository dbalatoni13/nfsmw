#ifndef WORLD_TRACKSTREAMER_H
#define WORLD_TRACKSTREAMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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
    short SectionNumber;                    // offset 0x0, size 0x2
    short FileOffset;                       // offset 0x2, size 0x2
    struct TrackStreamingSection *pSection; // offset 0x4, size 0x4
};

struct DiscBundleSection {
    // total size: 0x114
    int FileOffset;                      // offset 0x0, size 0x4
    int FileSize;                        // offset 0x4, size 0x4
    char SectionName[11];                // offset 0x8, size 0xB
    char NumMembers;                     // offset 0x13, size 0x1
    DiscBundleSectionMember Members[32]; // offset 0x14, size 0x100
};

struct bBitTable {
    // total size: 0x8
    int NumBits;         // offset 0x0, size 0x4
    unsigned char *Bits; // offset 0x4, size 0x4
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
    char SectionName[8];                      // offset 0x0, size 0x8
    short SectionNumber;                      // offset 0x8, size 0x2
    char WasRendered;                         // offset 0xA, size 0x1
    char CurrentlyVisible;                    // offset 0xB, size 0x1
    eStatus Status;                           // offset 0xC, size 0x4
    eTrackStreamingFileType FileType;         // offset 0x10, size 0x4
    int FileOffset;                           // offset 0x14, size 0x4
    int Size;                                 // offset 0x18, size 0x4
    int CompressedSize;                       // offset 0x1C, size 0x4
    int PermSize;                             // offset 0x20, size 0x4
    int SectionPriority;                      // offset 0x24, size 0x4
    bVector2 Centre;                          // offset 0x28, size 0x8
    float Radius;                             // offset 0x30, size 0x4
    unsigned int Checksum;                    // offset 0x34, size 0x4
    int LastNeededTimestamp;                  // offset 0x38, size 0x4
    unsigned int UnactivatedFrameCount;       // offset 0x3C, size 0x4
    int LoadedTime;                           // offset 0x40, size 0x4
    int BaseLoadingPriority;                  // offset 0x44, size 0x4
    int LoadingPriority;                      // offset 0x48, size 0x4
    void *pMemory;                            // offset 0x4C, size 0x4
    DiscBundleSection *pDiscBundle;           // offset 0x50, size 0x4
    int LoadedSize;                           // offset 0x54, size 0x4
    struct VisibleSectionBoundary *pBoundary; // offset 0x58, size 0x4
};

// total size: 0x68
struct StreamingPositionEntry {
    bVector2 Position;              // offset 0x0, size 0x8
    float Elevation;                // offset 0x8, size 0x4
    bVector2 Velocity;              // offset 0xC, size 0x8
    bVector2 Direction;             // offset 0x14, size 0x8
    bool PositionSet;               // offset 0x1C, size 0x1
    bool FollowingCar;              // offset 0x20, size 0x1
    short CurrentZone;              // offset 0x24, size 0x2
    short PredictedZone;            // offset 0x26, size 0x2
    int PredictedZoneValidTime;     // offset 0x28, size 0x4
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
  private:
    int Address;        // offset 0x8, size 0x4
    int Size;           // offset 0xC, size 0x4
    bool Allocated;     // offset 0x10, size 0x1
    char DebugName[32]; // offset 0x14, size 0x20
};

// total size: 0x2754
class TSMemoryPool {
  public:
  private:
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

// total size: 0x888
class TrackStreamer {
  public:
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

    void ServiceGameState();
    void ServiceNonGameState();
    void SetStreamingPosition(int position_number, const bVector3 *position);
    void ClearStreamingPositions();
    void BlockUntilLoadingComplete();

  private:
    TrackStreamingSection *pTrackStreamingSections;       // offset 0x0, size 0x4
    int NumTrackStreamingSections;                        // offset 0x4, size 0x4
    DiscBundleSection *pDiscBundleSections;               // offset 0x8, size 0x4
    DiscBundleSection *pLastDiscBundleSection;            // offset 0xC, size 0x4
    struct TrackStreamingInfo *pInfo;                     // offset 0x10, size 0x4
    int NumBarriers;                                      // offset 0x14, size 0x4
    struct TrackStreamingBarrier *pBarriers;              // offset 0x18, size 0x4
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
    unsigned int LastWaitUntilRenderingDoneFrameCount;    // offset 0x2D0, size 0x4
    unsigned int LastPrintedFrameCount;                   // offset 0x2D4, size 0x4
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
    void (*pCallback)(int);                               // offset 0x874, size 0x4
    int CallbackParam;                                    // offset 0x878, size 0x4
    void (*MakeSpaceInPoolCallback)(int);                 // offset 0x87C, size 0x4
    int MakeSpaceInPoolCallbackParam;                     // offset 0x880, size 0x4
    int MakeSpaceInPoolSize;                              // offset 0x884, size 0x4
};

extern TrackStreamer TheTrackStreamer;

#endif
