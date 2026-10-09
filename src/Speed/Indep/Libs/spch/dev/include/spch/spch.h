//
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

typedef void (*AbortMessageVec)(const char *, ...); // Decl: 102
typedef int (*DebugPrintVec)(const char *, ...);    // Decl: 103
typedef int (*GetTicksVec)();                       // Decl: 104

// total size: 0xC
// Decl: 114
typedef struct {
    AbortMessageVec spchAbortMessage; // offset 0x0, size 0x4, Decl: 115
    DebugPrintVec spchPrint;          // offset 0x4, size 0x4, Decl: 116
    GetTicksVec spchGetTick;          // offset 0x8, size 0x4, Decl: 117
} SPCHType_ExtVecs;

// Decl: 123
#define SPCH_SetVecsToReal(extVecs)                                                                                                                  \
    extVecs->spchAbortMessage = REAL_abortmessage;                                                                                                   \
    extVecs->spchPrint = printf;                                                                                                                     \
    extVecs->spchGetTick = TIMER_gettick

typedef int (*AddEventFuncPtr)(int, int, ...); // Decl: 134

typedef int (*SampleRequestFuncPtr)(SPCHType_SampleRequestData *);      // Decl: 137
typedef int (*TestSentenceRuleFuncPtr)(EventSpec *, int, int, int);     // Decl: 138
typedef void (*SetSentenceRuleFuncPtr)(EventSpec *, int, int, int);     // Decl: 139
typedef int (*ReparmFuncPtr)(int, unsigned int *);                      // Decl: 140
typedef enum SPCHType_EventRuleResult (*EventRuleFuncPtr)(EventSpec *); // Decl: 141

typedef void *(*MemAllocFuncPtr)(unsigned int); // Decl: 147
typedef void (*MemFreeFuncPtr)(void *);         // Decl: 148

void SPCH_ReinitBanks(unsigned int gameSeed);
int SPCH_GetBankPtrMemSize(int numBanks);
void SPCH_InitBankMem(int numBanks, char *bankMem);
char *SPCH_GetBankPtr(int bankHandle);
void SPCH_InitSampleRepeat(int bankHandle);
void SPCH_FreeBank(int bankHandle);
int SPCH_AddBank(char *bankHdr);

void SPCH_PlayLastEvent(unsigned int inChannel);
int SPCH_Play(unsigned int inChannel);
void SPCH_PlaySpeech();
int SPCH_Choose(unsigned int inChannel);
int SPCH_ChooseSpeech();
void SPCH_SetFilter(int filterSetting, unsigned int channel);
void SPCH_SetMemCallbacks(MemAllocFuncPtr memAlloc, MemFreeFuncPtr memFree);

int SPCH_GetSampleDataRate(int sampleRate, int sampleBits, CompressionType type);
void SPCH_InitRuleCallbacks(TestSentenceRuleFuncPtr ruleTest, SetSentenceRuleFuncPtr ruleSet);
void SPCH_InitReparmCallback(ReparmFuncPtr reparmer);
void SPCH_InitEventRuleCallback(EventRuleFuncPtr eventRuleTest);
int SPCH_Init(SampleRequestFuncPtr sampleRequest, unsigned int gameSeed, int sampleDataRate);
void SPCH_Deinit();

SPCHType_ExtVecs *SPCH_GetExtVecs();

void SPCH_ClearMatchParmSettings(unsigned long inChannel);
int SPCH_AddEventDB(char *dataFile, unsigned int channel);
int SPCH_RemoveEventDB(char *dataFile, unsigned int channel);
int SPCH_ResolveData(char *dataFile);

#endif
