#ifndef SPCHI_H
#define SPCHI_H

#include "spch/spch.h"
#include <csis/csis.h>

// total size: 0xC
struct VOXBANKHDR {
    unsigned short type;       // offset 0x0, size 0x2
    unsigned short subID;      // offset 0x2, size 0x2
    unsigned char parmFlags;   // offset 0x4, size 0x1
    unsigned char numSamples;  // offset 0x5, size 0x1
    unsigned char sampleRepeat;// offset 0x6, size 0x1
    unsigned char blockSize;   // offset 0x7, size 0x1
    unsigned short bankBlocks; // offset 0x8, size 0x2
    unsigned short numSubBanks;// offset 0xA, size 0x2
};

// total size: 0xC
struct VoxEvent {
    unsigned short ID;           // offset 0x0
    unsigned short expiryTime;   // offset 0x2
    unsigned short priority;     // offset 0x4
    unsigned char numSentences;  // offset 0x6
    unsigned char numRules;      // offset 0x7
    unsigned char numRowContexts;// offset 0x8
    char frequency;              // offset 0x9
    unsigned char flags;         // offset 0xA
    char numParms;               // offset 0xB
};

// total size: 0x8
struct PhraseParmInfo {
    unsigned int matchValues;      // offset 0x0, size 0x4
    unsigned char eventParmIndex;  // offset 0x4, size 0x1
    unsigned char ruleIndex;       // offset 0x5, size 0x1
    unsigned char matchParmIndex;  // offset 0x6, size 0x1
    unsigned char pad;             // offset 0x7, size 0x1
};

// total size: 0x8
struct VoxPhrase {
    unsigned short bankID;       // offset 0x0, size 0x2
    unsigned char bankIDIndex;   // offset 0x2, size 0x1
    unsigned char bankType;      // offset 0x3, size 0x1
    char numFilters;             // offset 0x4, size 0x1
    unsigned char pad1;          // offset 0x5, size 0x1
    unsigned char pad2;          // offset 0x6, size 0x1
    unsigned char pad3;          // offset 0x7, size 0x1
};

// total size: 0x8
struct VoxSentence {
    unsigned char expWeight;     // offset 0x0, size 0x1
    unsigned char frequency;     // offset 0x1, size 0x1
    unsigned char bitField;      // offset 0x2, size 0x1
    unsigned char numMatchParms; // offset 0x3, size 0x1
    unsigned char numContexts;   // offset 0x4, size 0x1
    unsigned char pad0;          // offset 0x5, size 0x1
    unsigned char pad1;          // offset 0x6, size 0x1
    unsigned char pad2;          // offset 0x7, size 0x1
};

enum ParmType {
    kParmType_Variable = 0,
    kParmType_Constant = 1,
    kParmType_User = 2,
    kParmType_BankID = 3,
    kParmType_Context = 4,
    kParmType_Match = 5,
};

// total size: 0xC
struct SRule {
    int ruleID;         // offset 0x0, size 0x4
    int parmIndex;      // offset 0x4, size 0x4
    ParmType parmType;  // offset 0x8, size 0x4
};

enum RuleStatus {
    kRuleState_Ignore = 0,
    kRuleState_IfTrue = 1,
    kRuleState_IfFalse = 2,
};

enum BankType {
    kBankType_Single = 0,
    kBankType_Multi = 1,
    kBankType_Array = 2,
};

enum ExactMatchParmType {
    kExactMatch_NoCompare = 0,
    kExactMatch_GameValue = 1,
    kExactMatch_BankID = 2,
};

// total size: 0x18
struct VoxData {
    char majorRev;               // offset 0x0
    char minorRev;               // offset 0x1
    char release;                // offset 0x2
    char prerelease;             // offset 0x3
    int csisOffset;              // offset 0x4
    unsigned char projectID;     // offset 0x8
    unsigned char datID;         // offset 0x9
    unsigned char bolloRev;      // offset 0xA
    unsigned char csisResolved;  // offset 0xB
    unsigned short saveIncrement;// offset 0xC
    unsigned short generateID;   // offset 0xE
    unsigned short numEvents;    // offset 0x10
    unsigned char numGlobalMatchParms; // offset 0x12
    unsigned char pad1;          // offset 0x13
    unsigned short eventFilterLength; // offset 0x14
    unsigned short eventFilterPriority; // offset 0x16
};

// total size: 0x20
struct CSIS_Data {
    Csis::InterfaceId interfaceID; // offset 0x0, size 0x8
    Csis::FunctionHandle handle;   // offset 0x8, size 0x8
    Csis::FunctionClient client;   // offset 0x10, size 0x10
};

// total size: 0x8
struct EventDatInfo {
    VoxData *data;          // offset 0x0
    unsigned int channel;   // offset 0x4
};

extern SPCHType_ExtVecs gExtVecs;
extern EventDatInfo gEventDats[8];

inline unsigned char *iSPCH_GetGlobalMatchParmAddr(VoxData *evtData) {
    unsigned int offset;
    offset = evtData->numEvents * 2;
    offset = (offset + 3) & ~3;
    return (unsigned char *)evtData + (offset + 0x18);
}

inline unsigned char *iSPCH_GetOffset8(unsigned char *basePtr, unsigned char *offsets, int index) {
    return basePtr + (offsets[index] << 2);
}

inline unsigned char *iSPCH_GetOffset16(unsigned char *basePtr, unsigned short *offsets, int index) {
    return basePtr + (offsets[index] << 2);
}

inline int VoxSentence_GetNumPhrases(VoxSentence *sentence) {
    return sentence->bitField >> 2;
}

inline unsigned char VoxEvent_GetNumRules(VoxEvent *event) {
    return event->numRules;
}

inline PhraseParmInfo *iSPCH_GetPhraseParmInfo(VoxPhrase *phrase, int index) {
    PhraseParmInfo *info = (PhraseParmInfo *)(phrase + 1);
    return &info[index];
}

inline unsigned char *iSPCH_GetRuleDataAddr(VoxEvent *event) {
    unsigned int offset;
    offset = event->numSentences * 2;
    offset = (offset + 3) & ~3;
    return (unsigned char *)event + (offset + 0xC);
}

inline unsigned char *iSPCH_GetSentenceRulesAddr(VoxEvent *event) {
    unsigned char *data = iSPCH_GetRuleDataAddr(event);
    return data + ((VoxEvent_GetNumRules(event) * 3 + 3) & ~3);
}

inline unsigned char VoxEvent_GetNumSentences(VoxEvent *event) {
    return event->numSentences;
}

inline unsigned char VoxEvent_GetNumContexts(VoxEvent *event) {
    return event->numRowContexts >> 4;
}

inline unsigned char VoxEvent_GetFilterPriorityFlag(VoxEvent *event) {
    unsigned char flag;
    flag = event->flags & 0x2;
    return flag;
}

inline unsigned char VoxEvent_GetKeepTillExpiresFlag(VoxEvent *event) {
    unsigned char flag;
    flag = event->flags & 0x4;
    return flag;
}

inline unsigned char VoxEvent_GetExactParmMatchFlag(VoxEvent *event) {
    unsigned char flag;
    flag = (event->flags >> 3) & 1;
    return flag;
}

inline unsigned char VoxEvent_GetFilterLengthFlag(VoxEvent *event) {
    unsigned char flag;
    flag = event->flags & 0x1;
    return flag;
}

inline unsigned char VoxEvent_GetFollowGroupFlag(VoxEvent *event) {
    unsigned char flag;
    flag = event->flags & 0x10;
    return flag;
}

inline unsigned char VoxEvent_GetInterruptFlag(VoxEvent *event) {
    unsigned char flag;
    flag = event->flags & 0x20;
    return flag;
}

inline unsigned char *iSPCH_GetContextDataAddr(VoxEvent *event) {
    unsigned char *data;
    data = iSPCH_GetSentenceRulesAddr(event);
    data += ((VoxEvent_GetNumRules(event) + 7) / 8 * event->numSentences * 2 + 3) & ~3;
    return data;
}

inline enum RuleStatus VoxSentence_GetShortRule(VoxSentence *sentence) {
    return (RuleStatus)(sentence->bitField & 3);
}

inline int *VoxSentence_GetContextFlagsAddr(VoxSentence *sentence) {
    unsigned int offsetSize;
    offsetSize = (VoxSentence_GetNumPhrases(sentence) + 3) & ~3;

    return (int *)((unsigned char *)sentence + (offsetSize + 8));
}

inline int *VoxSentence_GetMatchParmIO(VoxSentence *sentence) {
    unsigned int offsetSize;
    offsetSize = sentence->numContexts * 4;

    return (int *)((unsigned char *)VoxSentence_GetContextFlagsAddr(sentence) + offsetSize);
}

inline unsigned char VoxEvent_GetNumInARow(VoxEvent *event) {
    return event->numRowContexts & 0xF;
}

inline unsigned char *BANKHDR_GetValidParmMask(VOXBANKHDR *hdr) {
    unsigned int offset;
    offset = hdr->numSamples * ((hdr->parmFlags & 0x7F) + 2);

    offset += 15;
    offset &= ~3;

    return (unsigned char *)hdr + offset;
}

inline unsigned char *BANKHDR_GetCycleBitsAddr(VOXBANKHDR *hdr) {
    unsigned char *addr;
    addr = BANKHDR_GetValidParmMask(hdr) + (hdr->parmFlags & 0x7F) * 4;

    return addr;
}

inline unsigned char *BANKHDR_GetSampleRepeatAddr(VOXBANKHDR *hdr) {
    int cycleBytes;
    unsigned char *addr;

    cycleBytes = 0;
    addr = BANKHDR_GetCycleBitsAddr(hdr);

    if (hdr->parmFlags & 0x80) {
        cycleBytes = ((hdr->numSamples + 7) / 8) + 1;
    }

    return addr + cycleBytes;
}

inline unsigned char *BANKHDR_GetArrayBankBitsAddr(VOXBANKHDR *hdr) {
    unsigned char *addr;
    addr = BANKHDR_GetSampleRepeatAddr(hdr);

    if (hdr->sampleRepeat != 0) {
        addr += hdr->sampleRepeat + 1;
    }

    return addr;
}

inline unsigned int VoxEvent_GetParmTypesMask(VoxEvent *event) {
    unsigned char *data;
    unsigned int *parmTypesMask;

    data = iSPCH_GetContextDataAddr(event);

    parmTypesMask = (unsigned int *)(data + ((VoxEvent_GetNumContexts(event) * 3 + 3) & ~3));

    return *parmTypesMask;
}

inline unsigned short VoxEvent_GetSentenceParmMask(VoxEvent *event, int sentenceNum) {
    unsigned char *data;
    unsigned short *sentenceParmsMask;

    data = iSPCH_GetContextDataAddr(event);
    data += (VoxEvent_GetNumContexts(event) * 3 + 3) & ~3;

    sentenceParmsMask = (unsigned short *)(data + 4);

    return sentenceParmsMask[sentenceNum];
}

inline unsigned int ExactMatch_NumBytes(int numSentences) {
    unsigned int numBytes;
    numBytes = numSentences * 2;
    return (numBytes + 7) & ~3;
}

#define iSPCH_GetRule(rule, ruleData, index)                                                        \
    (rule).ruleID = (ruleData)[(index) * 3];                                                        \
    (rule).parmIndex = (ruleData)[(index) * 3 + 1];                                                 \
    (rule).parmType = (ParmType)(ruleData)[(index) * 3 + 2]

#endif
