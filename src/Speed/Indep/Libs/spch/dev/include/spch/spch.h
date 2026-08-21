
#ifndef _SNDVOX_
#define _SNDVOX_ // Decl: 3

// TODO probably no version number in this game yet
// #define kSPCH_MajorRev 3  // Decl: 20
// #define kSPCH_MinorRev 20 // Decl: 21
// #define kSPCH_Patch 5     // Decl: 22

// Decl: 27
#define GMAKEID(a, b, c, d) (((int)(a) << 24) | ((int)(b) << 16) | ((int)(c) << 8) | (int)(d))

// Decl: 44
enum CompressionType {
    kSPCH_Compression_None = 0,
    kSPCH_Compression_MicroTalk = 1,
    kSPCH_Compression_XA = 2,
};

// Decl: 51
enum SPCHType_EventRuleResult {
    kSPCH_EventRule_Delete = 0,
    kSPCH_EventRule_OK = 1,
};

// total size: 0x4
// Decl: 74
typedef struct {
    unsigned short eventID;   // offset 0x0, size 0x2, Decl: 75
    unsigned char eventDatID; // offset 0x2, size 0x1, Decl: 76
    unsigned char projectID;  // offset 0x3, size 0x1, Decl: 77
} EventSpec;

// total size: 0x20
// Decl: 91
struct SPCHType_SampleRequestData {
    int bankNum;         // offset 0x0, size 0x4, Decl: 92
    int sampleOffset;    // offset 0x4, size 0x4, Decl: 93
    int numBytes;        // offset 0x8, size 0x4, Decl: 94
    EventSpec eventSpec; // offset 0xC, size 0x4, Decl: 95
    int channel;         // offset 0x10, size 0x4, Decl: 96
    int subID;           // offset 0x14, size 0x4, Decl: 97
    int datID;           // offset 0x18, size 0x4, Decl: 98
    int interruptFlag;   // offset 0x1C, size 0x4, Decl: 99
};

// total size: 0xC
// Decl: 114
typedef struct {
    void (*spchAbortMessage)(const char *, ...); // offset 0x0, size 0x4, Decl: 115
    int (*spchPrint)(const char *, ...);         // offset 0x4, size 0x4, Decl: 116
    int (*spchGetTick)();                        // offset 0x8, size 0x4, Decl: 117
} SPCHType_ExtVecs;

typedef int (*TestSentenceRuleFuncPtr)(EventSpec *, int, int, int);
typedef void (*SetSentenceRuleFuncPtr)(EventSpec *, int, int, int);
typedef int (*ReparmFuncPtr)(int, unsigned int *);
typedef SPCHType_EventRuleResult (*EventRuleFuncPtr)(EventSpec *);
typedef void *(*MemAllocFuncPtr)(unsigned int);
typedef void (*MemFreeFuncPtr)(void *);
typedef int (*AddEventFuncPtr)(int, int, ...);
typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;

struct SPCH_Callbacks {
    int (*request)(SPCHType_SampleRequestData *);
    TestSentenceRuleFuncPtr testRule;
    SetSentenceRuleFuncPtr setRule;
    EventRuleFuncPtr eventRule;
    ReparmFuncPtr reparm;
};

extern SPCHType_ExtVecs gExtVecs;
extern SPCH_Callbacks gCallbacks;
extern int gSPCH_Initialized;
extern int gDataRate;
extern unsigned int gLastTick;
extern unsigned short gLastSubTick;
extern MemAllocFuncPtr gMemAlloc;
extern MemFreeFuncPtr gMemFree;
void iSPCH_MemFree(void *data);

struct VoxBankInfo;
extern VoxBankInfo *gVoxBanks;
extern int gUniqueBankHandle;
extern int gNumBanks;
extern int gBankCount;
extern AddEventFuncPtr gSPCH_AddEvent;

struct VOXBANKHDR {
    unsigned short type;
    unsigned short subID;
    unsigned char parmFlags;
    unsigned char numSamples;
    unsigned char sampleRepeat;
    unsigned char blockSize;
    unsigned short bankBlocks;
    unsigned short numSubBanks;
};

typedef struct VOXBANKHDR VOXBANKHDR;

struct VoxBankInfo {
    int bankHandle;
    VOXBANKHDR *voxHdr;
};

struct VOXINGAME {
    EventSpec lastEventSpec;
    int numEventTimes;
};

struct VoxData;
struct VoxEvent;

struct EventDatInfo {
    VoxData *eventDat;
    unsigned int channel;
};

extern EventDatInfo gEventDats[8];

struct VoxEventItem {
    unsigned int entryTime;
    unsigned short subTicks;
    unsigned char pending;
    unsigned char channel;
    VoxEvent *event;
    unsigned int *memParms;
};

struct VoxPendingEvents {
    int numPending[8];
    int lastAddedEvent[8];
    VoxEventItem events[16];
};

extern VoxPendingEvents gVoxEvents;

struct VoxSentence;

struct PhraseChoice {
    int bankHandle;
    short bankIndex;
    short subBankIndex;
    unsigned short sampleIndex;
    unsigned short pad;
};

struct EventChoice {
    VoxEvent *event;
    VoxSentence *sentence;
    unsigned char sentenceNum;
    unsigned char validChoice;
    unsigned char numPhrases;
    unsigned char pad1;
    unsigned int *memParms;
    PhraseChoice phrases[12];
};

extern VOXINGAME gVoxInGame[8];

// Decl: 123
#define SPCH_SetVecsToReal(extVecs)                                                                                                                  \
    extVecs->spchAbortMessage = REAL_abortmessage;                                                                                                   \
    extVecs->spchPrint = printf;                                                                                                                     \
    extVecs->spchGetTick = TIMER_gettick

#endif
