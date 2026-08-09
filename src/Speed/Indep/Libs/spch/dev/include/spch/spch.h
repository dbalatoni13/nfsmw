
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

// Decl: 123
#define SPCH_SetVecsToReal(extVecs)                                                                                                                  \
    extVecs->spchAbortMessage = REAL_abortmessage;                                                                                                   \
    extVecs->spchPrint = printf;                                                                                                                     \
    extVecs->spchGetTick = TIMER_gettick

#endif
