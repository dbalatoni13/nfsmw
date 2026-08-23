#ifndef SPCH_SPCHLIB_H
#define SPCH_SPCHLIB_H

inline PhraseParmInfo *iSPCH_GetPhraseParmInfo(VoxPhrase *phrase, int index) {
    PhraseParmInfo *info;

    info = reinterpret_cast<PhraseParmInfo *>(phrase + 1);
    return info + index;
}

inline int VoxSentence_GetNumPhrases(VoxSentence *sentence) {
    return sentence->bitField >> 2;
}

inline RuleStatus VoxSentence_GetShortRule(VoxSentence *sentence) {
    return static_cast<RuleStatus>(sentence->bitField & 3);
}

inline int *VoxSentence_GetContextFlagsAddr(VoxSentence *sentence) {
    unsigned int offsetSize;

    offsetSize = VoxSentence_GetNumPhrases(sentence);
    offsetSize += 3;
    offsetSize &= ~3;
    offsetSize += 8;
    return reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(sentence) + offsetSize);
}

inline int *VoxSentence_GetMatchParmIO(VoxSentence *sentence) {
    unsigned int offsetSize;

    offsetSize = sentence->numContexts;
    offsetSize <<= 2;
    return reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(VoxSentence_GetContextFlagsAddr(sentence)) + offsetSize);
}

inline unsigned char VoxEvent_GetNumRules(VoxEvent *event) {
    return event->numRules;
}

inline unsigned char VoxEvent_GetNumContexts(VoxEvent *event) {
    return event->numRowContexts >> 4;
}

inline unsigned char *iSPCH_GetOffset8(unsigned char *basePtr, unsigned char *offsets, int index) {
    return basePtr + offsets[index] * 4;
}

inline unsigned char *iSPCH_GetOffset16(unsigned char *basePtr, unsigned short *offsets, int index) {
    return basePtr + offsets[index] * 4;
}

inline unsigned char *iSPCH_GetGlobalMatchParmAddr(VoxData *evtData) {
    unsigned int offset;

    offset = evtData->numEvents * 2;
    offset = (offset + 3) & ~3;
    return reinterpret_cast<unsigned char *>(evtData) + 0x18 + offset;
}

inline unsigned char *iSPCH_GetRuleDataAddr(VoxEvent *event) {
    unsigned int offset;

    offset = event->numSentences * 2;
    offset = (offset + 3) & ~3;
    return reinterpret_cast<unsigned char *>(event) + 0xC + offset;
}

inline unsigned char *iSPCH_GetSentenceRulesAddr(VoxEvent *event) {
    unsigned char *data;

    data = iSPCH_GetRuleDataAddr(event);
    data += (VoxEvent_GetNumRules(event) * 3 + 3) & ~3;
    return data;
}

inline unsigned char *iSPCH_GetContextDataAddr(VoxEvent *event) {
    unsigned char *data;

    data = iSPCH_GetSentenceRulesAddr(event);
    data += (event->numSentences * ((VoxEvent_GetNumRules(event) + 7) >> 3) * 2 + 3) & ~3;
    return data;
}

inline unsigned int ExactMatch_NumBytes(int numSentences) {
    unsigned int numBytes;

    numBytes = numSentences * 2;
    numBytes += 7;
    numBytes &= ~3;
    return numBytes;
}

inline unsigned char VoxEvent_GetFilterLengthFlag(VoxEvent *event) {
    unsigned char flag;

    flag = event->flags & 1;
    return flag;
}

inline unsigned char VoxEvent_GetExactParmMatchFlag(VoxEvent *event) {
    unsigned char flag;

    flag = (event->flags >> 3) & 1;
    return flag;
}

inline unsigned char VoxEvent_GetFollowGroupFlag(VoxEvent *event) {
    unsigned char flag;

    flag = event->flags & 0x10;
    return flag;
}

inline unsigned char VoxEvent_GetInterruptFlag(VoxEvent *event) {
    unsigned char flag;

    flag = (event->flags >> 5) & 1;
    return flag;
}

inline unsigned char VoxEvent_GetKeepTillExpiresFlag(VoxEvent *event) {
    unsigned char flag;

    flag = event->flags & 4;
    return flag;
}

inline unsigned char VoxEvent_GetFilterPriorityFlag(VoxEvent *event) {
    unsigned char flag;

    flag = event->flags & 2;
    return flag;
}

inline int VoxEvent_GetFollowEventData(VoxEvent *event, SPCHType_FollowData *followData) {
    unsigned char *ptr;
    int result;

    result = 0;
    if (VoxEvent_GetFollowGroupFlag(event) != 0) {
        result = 1;
        ptr = iSPCH_GetContextDataAddr(event);
        ptr += (VoxEvent_GetNumContexts(event) * 3 + 3) & ~3;
        if (VoxEvent_GetExactParmMatchFlag(event) != 0) {
            ptr += ExactMatch_NumBytes(event->numSentences);
        }
        followData->numEvents = *ptr;
        followData->ID = reinterpret_cast<unsigned short *>(ptr + 2);
    }
    return result;
}

inline unsigned int VoxEvent_GetParmTypesMask(VoxEvent *event) {
    unsigned char *data;
    unsigned int *parmTypesMask;

    data = iSPCH_GetContextDataAddr(event);
    parmTypesMask = reinterpret_cast<unsigned int *>(data + ((VoxEvent_GetNumContexts(event) * 3 + 3) & ~3));
    return *parmTypesMask;
}

inline unsigned short VoxEvent_GetSentenceParmMask(VoxEvent *event, int sentenceNum) {
    unsigned char *data;
    unsigned short *sentenceParmsMask;

    data = iSPCH_GetContextDataAddr(event);
    sentenceParmsMask = reinterpret_cast<unsigned short *>(data + ((VoxEvent_GetNumContexts(event) * 3 + 3) & ~3) + 4);
    return sentenceParmsMask[sentenceNum];
}

inline unsigned char VoxEvent_GetNumInARow(VoxEvent *event) {
    return event->numRowContexts & 0xF;
}

#endif
