
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

inline unsigned char *BANKHDR_GetValidParmMask(VOXBANKHDR *hdr) {
    unsigned int offset;

    offset = (hdr->numSamples * ((hdr->parmFlags & 0x7F) + 2) + 0xF) & ~3;
    return reinterpret_cast<unsigned char *>(hdr) + offset;
}

inline unsigned char *BANKHDR_GetCycleBitsAddr(VOXBANKHDR *hdr) {
    unsigned char *addr;

    addr = BANKHDR_GetValidParmMask(hdr);
    addr += (hdr->parmFlags & 0x7F) * 4;
    return addr;
}

inline unsigned char *BANKHDR_GetSampleRepeatAddr(VOXBANKHDR *hdr) {
    int cycleBytes;
    unsigned char *addr;

    addr = BANKHDR_GetCycleBitsAddr(hdr);
    cycleBytes = 0;
    if ((hdr->parmFlags & 0x80) != 0) {
        cycleBytes = ((hdr->numSamples + 7) >> 3) + 1;
    }
    return addr + cycleBytes;
}

struct VoxBankInfo {
    int bankHandle;
    VOXBANKHDR *voxHdr;
};

struct VOXINGAME {
    EventSpec lastEventSpec;
    int numEventTimes;
};

enum BankType {
    kBankType_Single = 0,
    kBankType_Multi = 1,
    kBankType_Array = 2,
};

enum RuleStatus {
    kRuleState_Ignore = 0,
    kRuleState_IfTrue = 1,
    kRuleState_IfFalse = 2,
};

enum ParmType {
    kParmType_Variable = 0,
    kParmType_Constant = 1,
    kParmType_User = 2,
    kParmType_BankID = 3,
    kParmType_Context = 4,
    kParmType_Match = 5,
};

struct PhraseParmInfo {
    unsigned int matchValues;
    unsigned char eventParmIndex;
    unsigned char ruleIndex;
    unsigned char matchParmIndex;
    unsigned char pad;
};

struct VoxPhrase {
    unsigned short bankID;
    unsigned char bankIDIndex;
    unsigned char bankType;
    char numFilters;
    unsigned char pad1;
    unsigned char pad2;
    unsigned char pad3;
};

typedef int MatchParmIO;

struct VoxSentence {
    unsigned char expWeight;
    unsigned char frequency;
    unsigned char bitField;
    unsigned char numMatchParms;
    unsigned char numContexts;
    unsigned char pad0;
    unsigned char pad1;
    unsigned char pad2;
};

struct SRule {
    int ruleID;
    int parmIndex;
    ParmType parmType;
};

struct VoxEvent {
    unsigned short ID;
    unsigned short expiryTime;
    unsigned short priority;
    unsigned char numSentences;
    unsigned char numRules;
    unsigned char numRowContexts;
    char frequency;
    unsigned char flags;
    char numParms;
};

struct VoxData {
    char majorRev;
    char minorRev;
    char release;
    char prerelease;
    int csisOffset;
    unsigned char projectID;
    unsigned char datID;
    unsigned char bolloRev;
    unsigned char csisResolved;
    unsigned short saveIncrement;
    unsigned short generateID;
    unsigned short numEvents;
    unsigned char numGlobalMatchParms;
    unsigned char pad1;
    unsigned short eventFilterLength;
    unsigned short eventFilterPriority;
};

struct EventDatInfo {
    VoxData *eventDat;
    unsigned int channel;
};

extern EventDatInfo gEventDats[];

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

struct PhraseChoice {
    int bankHandle;
    short bankIndex;
    short subBankIndex;
    unsigned short sampleIndex;
    unsigned short pad;
};

struct SPCHType_FollowData {
    int numEvents;
    unsigned short *ID;
};

struct PhrasePickInfo {
    int bankHandle;
    short bankIndex;
    short subBankIndex;
    unsigned char pickStart;
    unsigned char numPicks;
    unsigned char pickedIndex;
    unsigned char done;
};

struct SentencePickInfo {
    PhrasePickInfo phraseInfo[12];
    unsigned char numPhrases;
    unsigned char pickIndex;
    unsigned char validSamples[200];
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

#include "spch/spchlib.h"

#endif
