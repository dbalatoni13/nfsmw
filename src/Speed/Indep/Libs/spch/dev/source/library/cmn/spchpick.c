#include "spch/spch.h"

extern int gPreLoadTicks;
extern int gFilterSetting[8];
extern int gClearCycle;
unsigned short iSPCH_Rand(int max, int randHandle);
void *iSPCH_MemAlloc(unsigned int numBytes);
unsigned char *iSPCH_GetSampleParmAddr(VOXBANKHDR *hdr, int sampleIndex);
int iSPCH_GetSampleSizeData(VOXBANKHDR *hdr, int sampleIndex, unsigned int *sampleOffset, unsigned int *dataBytes);
int iSPCH_GetGlobalMatchParmsArray(EventSpec *eventSpec, unsigned char **matchParmArray);
int iSPCH_FindEventDatInfo(EventSpec *eventSpec, EventDatInfo **datInfo);
VoxEvent *iSPCH_FindEvent(EventSpec *eventSpec);
int iSPCH_FindEventChannel(EventSpec *eventSpec, unsigned int *channel);
int iSPCH_GetFilterLength(EventSpec *eventSpec);
unsigned int iSPCH_GetRuleSettings(EventSpec *eventSpec, VoxEvent *event, unsigned int *parms, unsigned int *userSettingsMask);
int iSPCH_CheckSentenceRules(VoxEvent *event, int sentenceNum, unsigned int eventSettings, unsigned int userSettingsMask);
int iSPCH_GetRuleID(VoxEvent *event, int ruleIndex);
void iSPCH_RuleSet(EventSpec *eventSpec, VoxEvent *event, int sentenceNum, unsigned int *parms);
static int iSPCH_MakeSampleRequests(VoxEvent *event, VoxSentence *sentence, EventSpec *eventSpec);
int iSPCH_FindBankIndexFromHandle(int bankHandle);
void iSPCHBank_AddToQueue(VOXBANKHDR *hdr, int sampleIndex);
int iSPCH_ChooseSentence(unsigned int *parms);
static int iSPCH_TestValidParm(unsigned int parmTypesMask, unsigned int parmValue, int parmIndex);
static int iSPCH_ConvertTime(int dataBytes);
static void iSPCH_ClearCycleBit(VOXBANKHDR *bank, int sampleIndex);
static int iSPCH_RepeatEvent(VoxEvent *event, unsigned int channel);
static void iSPCH_OrderSentences(VoxEvent *event, char *sentenceOrder);
static void iSPCH_InitSentencePickInfo(SentencePickInfo *info);
static int iSPCH_SentenceGetChoices(SentencePickInfo *sentenceInfo, VoxSentence *sentence, unsigned int *parms, int filterSetting);
static int iSPCH_SentenceMakeChoice(EventSpec *eventSpec, SentencePickInfo *sentenceInfo, VoxSentence *sentence, int filterSetting, int filterLength);
static void iSPCH_SaveChosenSentence(SentencePickInfo *sentenceInfo, unsigned int inChannel, VoxEvent *event, VoxSentence *sentence, int sentenceNum, unsigned int *parms);
static int iSPCH_SentenceIsContextMatch(VoxEvent *event, VoxSentence *sentence, unsigned int *inParms);
static int iSPCH_ChooseSamples(SentencePickInfo *sentenceInfo, VoxSentence *sentence, PhrasePickInfo *phraseInfo, VoxPhrase *phrase, unsigned int *parms);
int iSPCHBank_GetSampleTimeInQueue(VOXBANKHDR *hdr, int sampleIndex);
short iSPCH_FindBank(unsigned short bankType, int &bankHandle);
short iSPCH_FindSubBank(unsigned short bankID, unsigned short subBankID, int &bankHandle);
int iSPCH_TestSubBankBounds(int bankIndex, unsigned int subBankIndex);
inline int VoxSentence_GetNumPhrases(VoxSentence *sentence);

static EventChoice gEventChoice[8];

static unsigned int multiple[8] = {
    1,
    4,
    0x10,
    0x40,
    0x100,
    0x400,
    0x1000,
    0x4000,
};

void SPCH_GetEventDatInfo(char *eventData, int *projID, int *datID) {
    *projID = static_cast<unsigned char>(eventData[8]);
    *datID = static_cast<unsigned char>(eventData[9]);
}

static int iSPCH_TestBit(UInt8 *bitArray, int bitIndex) {
    UInt8 mask;
    int byteIndex;
    int bit;
    int result;

    byteIndex = bitIndex;
    if (bitIndex < 0) {
        byteIndex += 7;
    }
    byteIndex >>= 3;
    bit = bitIndex - byteIndex * 8;
    mask = 1 << bit;
    result = bitArray[byteIndex] & mask;
    return result;
}

static int iSPCH_ConvertTime(int dataBytes) {
    int centiSecs;

    centiSecs = 0;
    if (gDataRate == 0) {
        goto abort;
    }
    centiSecs = dataBytes * 100 / gDataRate;
abort:
    ;
    return centiSecs;
}

static int iSPCH_SentenceLength(VoxSentence *sentence, SentencePickInfo *sentenceInfo) {
    int i;
    int numPhrases;
    int dataBytes;
    int centiSecs;
    int pickedIndex;
    int sampleIndex;
    VOXBANKHDR *bank;

    dataBytes = 0;
    i = 0;
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    if (numPhrases != 0) {
        unsigned int sampleOffset;
        unsigned int sampleBytes;

        do {
            pickedIndex = sentenceInfo->phraseInfo[i].pickedIndex;
            sampleIndex = sentenceInfo->validSamples[pickedIndex];
            bank = gVoxBanks[sentenceInfo->phraseInfo[i].bankIndex].voxHdr;
            if (iSPCH_GetSampleSizeData(bank, sampleIndex, &sampleOffset, &sampleBytes) != 0) {
                dataBytes += sampleBytes;
            }
            i++;
        } while (i < numPhrases);
    }
    centiSecs = iSPCH_ConvertTime(dataBytes);
    return centiSecs;
}

unsigned int iSPCH_DecodeWeight(unsigned char weight) {
    unsigned int exp;
    unsigned int mant;

    exp = weight >> 5;
    mant = weight & 0x1F;
    unsigned int num;
    num = multiple[exp];
    return mant * num;
}

int iSPCH_OneChosen(unsigned int inChannel) {
    int validChoice;

    validChoice = gEventChoice[inChannel].validChoice;
    return validChoice;
}

void SPCH_SetPreLoadTicks(int ticks) {
    gPreLoadTicks = ticks;
}

static void iSPCH_InitSentencePickInfo(SentencePickInfo *info) {
    int i;

    info->pickIndex = 0;
    info->numPhrases = 0;
    i = 0;
    do {
        info->phraseInfo[i].done = 0;
        i++;
    } while (i < 12);
}

static void iSPCH_InitMatchParmIO(VoxSentence *sentence) {
    int i;
    MatchParmIO *matchParmIO;

    i = 0;
    matchParmIO = VoxSentence_GetMatchParmIO(sentence);
    if (i >= sentence->numMatchParms) {
        return;
    }
    do {
        matchParmIO[i] = 0;
        i++;
    } while (i < sentence->numMatchParms);
}

static void iSPCH_PostMatchParmValue(VoxSentence *sentence, VoxPhrase *phrase, unsigned char *sampleParms) {
    int *matchParmIO;
    int numFilters;
    int eventParmIndex;
    int i;

    matchParmIO = VoxSentence_GetMatchParmIO(sentence);
    numFilters = phrase->numFilters;
    i = 0;
    if (i >= numFilters) {
        return;
    }
    do {
        eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;
        if (eventParmIndex == 0xFE) {
            int matchParmIndex;

            matchParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->matchParmIndex;
            if ((matchParmIndex & 0x80) != 0) {
                int sampleBitFlag;

                matchParmIndex &= 0x7F;
                sampleBitFlag = 1 << sampleParms[i];
                matchParmIO[matchParmIndex] |= sampleBitFlag;
            }
        }
        i++;
    } while (i < numFilters);
}

static int iSPCH_MatchSample(VoxSentence *sentence, VoxPhrase *phrase, unsigned int *parms, unsigned char *sampleParms) {
    int i;
    int numRules;
    int match;
    int sampleParmValue;
    int eventParmIndex;
    unsigned int matchValue;

    numRules = phrase->numFilters;
    i = 0;
    match = 1;
    if (i >= numRules) {
        goto abort;
    }
    do {
        unsigned int sampleParmBitFlags;

        sampleParmValue = sampleParms[i];
        match = 0;
        if (static_cast<unsigned int>(sampleParmValue) > 0x1F) {
            goto abort;
        }
        sampleParmBitFlags = 1 << (sampleParmValue & 0x3F);
        eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;
        if ((sampleParmBitFlags & iSPCH_GetPhraseParmInfo(phrase, i)->matchValues) != 0) {
            if (eventParmIndex != 0 && eventParmIndex != 0xFE) {
                if (eventParmIndex == 0xFF) {
                    int matchParmIndex;

                    matchParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->matchParmIndex;
                    if ((matchParmIndex & 0x80) != 0) {
                        int *matchParmIO;

                        matchParmIndex &= 0x7F;
                        matchParmIO = VoxSentence_GetMatchParmIO(sentence);
                        matchValue = matchParmIO[matchParmIndex];
                        if ((sampleParmBitFlags & matchValue) == 0) {
                            goto abort;
                        }
                    } else {
                        EventSpec eventSpec;
                        unsigned char *globalMatchParmArray;

                        eventSpec = *reinterpret_cast<EventSpec *>(parms);
                        if (iSPCH_GetGlobalMatchParmsArray(&eventSpec, &globalMatchParmArray) == 0 ||
                            sampleParmValue != globalMatchParmArray[matchParmIndex]) {
                            goto abort;
                        }
                    }
                } else {
                    unsigned int inParm;

                    inParm = parms[eventParmIndex];
                    if ((sampleParmBitFlags & inParm) == 0) {
                        goto abort;
                    }
                }
            }
            match = 1;
        }
        if (match == 0) {
            goto abort;
        }
        i++;
    } while (i < numRules);
abort:
    return match;
}

static int iSPCH_GetPhraseBank(VoxPhrase *phrase, unsigned int *parms, PhrasePickInfo *phrasePick) {
    int bankHandle = -1;
    int bankIndex = -1;
    BankType bankType;
    int parmIndex;
    int success;
    unsigned short bankID = phrase->bankID;
    unsigned short subBankID;

    bankType = static_cast<BankType>(phrase->bankType);
    parmIndex = phrase->bankIDIndex;
    phrasePick->subBankIndex = bankIndex;
    phrasePick->bankHandle = bankIndex;
    phrasePick->bankIndex = bankIndex;
    switch (bankType) {
    case kBankType_Single:
        phrasePick->bankIndex = iSPCH_FindBank(bankID, bankHandle);
        phrasePick->bankHandle = static_cast<short>(bankHandle);
        break;
    case kBankType_Multi:
        subBankID = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(parms) + parmIndex * 4 + 2);
        phrasePick->bankIndex = iSPCH_FindSubBank(bankID, subBankID, bankHandle);
        phrasePick->bankHandle = bankHandle;
        break;
    case kBankType_Array:
        bankIndex = iSPCH_FindBank(bankID, bankHandle);
        if (iSPCH_TestSubBankBounds(bankIndex, parms[parmIndex]) != 0) {
            phrasePick->subBankIndex = *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(parms) + parmIndex * 4 + 2);
        } else {
            bankHandle = -1;
        }
        phrasePick->bankIndex = static_cast<short>(bankIndex);
        phrasePick->bankHandle = bankHandle;
        break;
    default:
        break;
    }
    success = phrasePick->bankHandle >= 0;
    return success;
}

static int iSPCH_MatchParmInputsSet(VoxSentence *sentence, VoxPhrase *phrase) {
    int i;
    int result;
    int numFilters;

    i = 0;
    result = 1;
    numFilters = phrase->numFilters;
    if (i < numFilters) {
        do {
            if (iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex == 0xFF) {
                int matchParmIndex;

                matchParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->matchParmIndex;
                if (matchParmIndex != 0xFF && (matchParmIndex & 0x80) != 0) {
                    int *matchParm;

                    matchParmIndex &= 0x7F;
                    matchParm = VoxSentence_GetMatchParmIO(sentence);
                    if (matchParm != 0 && matchParm[matchParmIndex] == 0) {
                        result = 0;
                        break;
                    }
                }
            }
            i++;
        } while (i < numFilters);
    }
    return result;
}

static int iSPCH_HasMatchParmToPost(VoxPhrase *phrase) {
    int numFilters;
    int eventParmIndex;
    int i;
    int result;

    numFilters = phrase->numFilters;
    result = 0;
    i = 0;
    if (i < numFilters) {
        eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;
        do {
            if (eventParmIndex == 0xFE) {
                result = 1;
                break;
            }
            i++;
            if (i >= numFilters) {
                break;
            }
            eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;
        } while (1);
    }
    return result;
}

static int iSPCH_CheckFrequency(VoxSentence *sentence) {
    int rand;

    rand = iSPCH_Rand(100, -1);
    return rand < sentence->frequency;
}

inline unsigned short iSPCH_MakeValidParmsMask(VoxEvent *event, unsigned int *parms) {
    unsigned short testMask;
    unsigned short validParmsMask;
    unsigned int parmTypesMask;
    int i;
    unsigned int parmValue;

    testMask = 1;
    parmTypesMask = VoxEvent_GetParmTypesMask(event);
    validParmsMask = 0;
    i = 0;
    do {
        parmValue = parms[i + 1];
        if (iSPCH_TestValidParm(parmTypesMask, parmValue, i) != 0) {
            validParmsMask |= testMask;
        }
        i++;
        testMask <<= 1;
    } while (i <= 15);
    return validParmsMask;
}

int iSPCH_ChooseSentence(unsigned int *parms) {
    char sentenceOrder[100];
    SentencePickInfo pickInfo;
    VoxEvent *event;
    EventSpec eventSpec;
    VoxSentence *sentence;
    int sentenceNum;
    int trySentence;
    int numSentences;
    int numTries;
    int validSentence;
    unsigned int eventSettings;
    unsigned int userSettingsMask;
    int filterSetting;
    int getChoices;
    unsigned short sentenceParmsMask;
    unsigned short validParmsMask;
    int tryExactMatch;
    unsigned int channel;
    int filterLength;

    validSentence = 0;
    eventSpec = *reinterpret_cast<EventSpec *>(parms);
    userSettingsMask = 0;
    numTries = 0;
    event = iSPCH_FindEvent(&eventSpec);
    validParmsMask = 0;
    filterLength = 0;
    if (event == 0) {
        goto abort;
    }
    if (iSPCH_FindEventChannel(&eventSpec, &channel) == 0) {
        goto abort;
    }
    if (iSPCH_RepeatEvent(event, channel) == 0) {
        goto abort;
    }
    filterSetting = gFilterSetting[channel];
    if (filterSetting == 1) {
        if (VoxEvent_GetFilterLengthFlag(event) == 0) {
            filterSetting = 0;
        } else {
            filterLength = iSPCH_GetFilterLength(&eventSpec);
        }
    }
    eventSettings = iSPCH_GetRuleSettings(&eventSpec, event, parms, &userSettingsMask);
    iSPCH_OrderSentences(event, sentenceOrder);
    numSentences = event->numSentences;
    if (numSentences <= 0) {
        validSentence = -1;
        goto abort;
    }
    tryExactMatch = VoxEvent_GetExactParmMatchFlag(event);
    if (tryExactMatch != 0) {
        validParmsMask = iSPCH_MakeValidParmsMask(event, parms);
    }
    validSentence = 0;
    if (numTries >= numSentences) {
        goto abort;
    }
    do {
        trySentence = 1;
        sentenceNum = static_cast<signed char>(sentenceOrder[numTries]);
        if (sentenceNum < 0 || sentenceNum >= numSentences) {
            goto abort;
        }
        sentence = reinterpret_cast<VoxSentence *>(iSPCH_GetOffset16(reinterpret_cast<unsigned char *>(event), reinterpret_cast<unsigned short *>(event + 1), sentenceNum));
        if (iSPCH_CheckFrequency(sentence) != 0) {
            if (iSPCH_SentenceIsContextMatch(event, sentence, parms) != 0) {
                if (iSPCH_CheckSentenceRules(event, sentenceNum, eventSettings, userSettingsMask) != 0) {
                    if (tryExactMatch != 0) {
                        sentenceParmsMask = VoxEvent_GetSentenceParmMask(event, sentenceNum);
                        trySentence = sentenceParmsMask == validParmsMask;
                    }
                    if (trySentence != 0) {
                        iSPCH_InitSentencePickInfo(&pickInfo);
                        getChoices = iSPCH_SentenceGetChoices(&pickInfo, sentence, parms, filterSetting);
                        if (getChoices > 0) {
                            validSentence = iSPCH_SentenceMakeChoice(&eventSpec, &pickInfo, sentence, filterSetting, filterLength);
                            if (validSentence != 0) {
                                iSPCH_SaveChosenSentence(&pickInfo, channel, event, sentence, sentenceNum, parms);
                            }
                        }
                    }
                }
            }
        }
        numTries++;
    } while (validSentence == 0 && numTries < numSentences);
abort:
    return validSentence;
}

inline unsigned int VOXSENTENCE_GetWeight(VoxSentence *sentence) {
    return iSPCH_DecodeWeight(sentence->expWeight);
}

static int iSPCH_RepeatEvent(VoxEvent *event, unsigned int channel) {
    int result;
    int eventType;

    result = 1;
    eventType = event->ID;
    if (eventType == gVoxInGame[channel].lastEventSpec.eventID) {
        int numInARow;

        numInARow = VoxEvent_GetNumInARow(event);
        if (numInARow > 0) {
            result = gVoxInGame[channel].numEventTimes < numInARow;
        }
    }
    return result;
}

static int iSPCH_ShortRuleStatus(VoxSentence *sentence, int filterSetting) {
    RuleStatus shortStatus;
    int result;

    shortStatus = VoxSentence_GetShortRule(sentence);
    result = 0;
    if (shortStatus == kRuleState_IfTrue) {
        if (filterSetting == 2) {
            result = 1;
            return result;
        }
    }
    if (shortStatus == kRuleState_IfFalse) {
        if (filterSetting != 2) {
            result = 1;
            return result;
        }
    }
    if (shortStatus == kRuleState_Ignore) {
        result = 1;
    }
    return result;
}

static int iSPCH_TestValidParm(unsigned int parmTypesMask, unsigned int parmValue, int parmIndex) {
    unsigned int testMask;
    int result;

    testMask = parmTypesMask >> (parmIndex << 1);
    testMask &= 3;
    result = 0;
    switch (testMask) {
    case 1:
        if (parmValue != 0) {
            result = 1;
        }
        break;
    case 2:
        if (parmValue != 0xFFFFFFFF) {
            result = 1;
        }
        break;
    default:
        break;
    }
    return result;
}

static int iSPCH_AddSampleToValidPicks(SentencePickInfo *info, int sampleIndex) {
    int result;

    result = 0;
    if (info->pickIndex < 200) {
        info->validSamples[info->pickIndex] = static_cast<unsigned char>(sampleIndex);
        info->pickIndex++;
        result = 1;
    }
    return result;
}

static int iSPCH_PickPhraseSample(PhrasePickInfo *phraseInfo) {
    VOXBANKHDR *bank;
    int pickNum;
    int samplePicked;

    pickNum = iSPCH_Rand(phraseInfo->numPicks, phraseInfo->bankIndex);
    phraseInfo->pickedIndex = phraseInfo->pickStart + pickNum;
    return 1;
}

void iSPCH_InitSentenceChoice() {
    int i;
    int j;

    i = 0;
    do {
        gEventChoice[i].event = 0;
        gEventChoice[i].sentence = 0;
        gEventChoice[i].sentenceNum = 0;
        gEventChoice[i].validChoice = 0;
        gEventChoice[i].numPhrases = 0;
        gEventChoice[i].pad1 = 0;
        gEventChoice[i].memParms = 0;
        j = 0;
        do {
            gEventChoice[i].phrases[j].bankHandle = -1;
            gEventChoice[i].phrases[j].bankIndex = -1;
            gEventChoice[i].phrases[j].subBankIndex = -1;
            gEventChoice[i].phrases[j].sampleIndex = 0;
            j++;
        } while (j < 12);
        i++;
    } while (i < 8);
}

static void iSPCH_SaveChosenSentence(SentencePickInfo *sentenceInfo, unsigned int inChannel, VoxEvent *event, VoxSentence *sentence, int sentenceNum, unsigned int *parms) {
    int i;
    int pickIndex;
    int numPhrases;

    if (gEventChoice[inChannel].memParms != 0) {
        iSPCH_MemFree(gEventChoice[inChannel].memParms);
    }
    gEventChoice[inChannel].memParms = reinterpret_cast<unsigned int *>(iSPCH_MemAlloc((event->numParms + 1) * 4));
    if (gEventChoice[inChannel].memParms == 0) {
        goto abort;
    }
    gEventChoice[inChannel].sentence = sentence;
    gEventChoice[inChannel].event = event;
    i = 0;
    gEventChoice[inChannel].sentenceNum = static_cast<unsigned char>(sentenceNum);
    if ((event->numParms & 0x80) == 0) {
        do {
            gEventChoice[inChannel].memParms[i] = parms[i];
            i++;
        } while (i <= event->numParms);
    }
    i = 0;
    gEventChoice[inChannel].validChoice = 1;
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    if (i < numPhrases) {
        do {
            gEventChoice[inChannel].phrases[i].bankIndex = sentenceInfo->phraseInfo[i].bankIndex;
            gEventChoice[inChannel].phrases[i].bankHandle = sentenceInfo->phraseInfo[i].bankHandle;
            gEventChoice[inChannel].phrases[i].subBankIndex = sentenceInfo->phraseInfo[i].subBankIndex;
            pickIndex = sentenceInfo->phraseInfo[i].pickedIndex;
            gEventChoice[inChannel].phrases[i].sampleIndex = sentenceInfo->validSamples[pickIndex];
            i++;
        } while (i < numPhrases);
    }
    gEventChoice[inChannel].numPhrases = static_cast<unsigned char>(numPhrases);
abort:
    ;
}

void iSPCH_ClearSentenceChoiceChannel(unsigned int channel) {
    int j;

    if (gEventChoice[channel].memParms != 0) {
        iSPCH_MemFree(gEventChoice[channel].memParms);
        gEventChoice[channel].memParms = 0;
    }
    gEventChoice[channel].sentence = 0;
    gEventChoice[channel].event = 0;
    gEventChoice[channel].pad1 = 0;
    gEventChoice[channel].sentenceNum = 0;
    gEventChoice[channel].validChoice = 0;
    gEventChoice[channel].numPhrases = 0;
    gEventChoice[channel].memParms = 0;
    j = 0;
    do {
        gEventChoice[channel].phrases[j].bankHandle = -1;
        gEventChoice[channel].phrases[j].bankIndex = -1;
        gEventChoice[channel].phrases[j].subBankIndex = -1;
        gEventChoice[channel].phrases[j].sampleIndex = 0;
        j++;
    } while (j < 12);
}

static void iSPCH_ClearCycleBit(VOXBANKHDR *bank, int sampleIndex) {
    unsigned char *bankBits;
    unsigned char mask;
    int byteIndex;
    int bit;

    byteIndex = sampleIndex;
    if (sampleIndex < 0) {
        byteIndex = sampleIndex + 7;
    } else {
        byteIndex = sampleIndex;
    }
    bankBits = BANKHDR_GetCycleBitsAddr(bank);
    byteIndex >>= 3;
    bit = sampleIndex - byteIndex * 8;
    mask = ~(1 << bit);
    byteIndex++;
    bankBits[byteIndex] &= mask;
}

inline unsigned char *BANKHDR_GetArrayBankBitsAddr(VOXBANKHDR *hdr) {
    unsigned char *addr;

    addr = BANKHDR_GetSampleRepeatAddr(hdr);
    if (hdr->sampleRepeat != 0) {
        addr += hdr->sampleRepeat + 1;
    }
    return addr;
}

static int iSPCH_CheckTemplateSample(PhrasePickInfo *phraseInfo, VOXBANKHDR *bank, int sampleIndex) {
    int result;
    unsigned char *arrayData;

    result = 0;
    if (bank->numSubBanks <= phraseInfo->subBankIndex) {
        goto abort;
    }
    sampleIndex += bank->numSamples * phraseInfo->subBankIndex;
    arrayData = BANKHDR_GetArrayBankBitsAddr(bank);
    result = iSPCH_TestBit(arrayData, sampleIndex);
abort:
    return result;
}

static int iSPCH_SampleExists(PhrasePickInfo *phraseInfo, VOXBANKHDR *bank, int sampleIndex) {
    int result;
    unsigned char *cycleData;

    result = 1;
    if (sampleIndex > bank->numSamples) {
        result = 0;
        goto abort;
    }
    if (phraseInfo->subBankIndex != -1) {
        result = iSPCH_CheckTemplateSample(phraseInfo, bank, sampleIndex);
        if (result == 0) {
            goto abort;
        }
    }
    if ((bank->parmFlags & 0x80) != 0) {
        cycleData = BANKHDR_GetCycleBitsAddr(bank);
        cycleData++;
        result = iSPCH_TestBit(cycleData, sampleIndex);
    }
abort:
    return result;
}

static int iSPCH_BankHasValidSamples(VoxPhrase *phrase, VOXBANKHDR *bank, unsigned int *parms) {
    unsigned int *validParmMask;
    int i;
    int numParms;
    int eventParmIndex;
    int match;

    validParmMask = reinterpret_cast<unsigned int *>(BANKHDR_GetValidParmMask(bank));
    i = 0;
    numParms = bank->parmFlags & 0x7F;
    match = 1;
    if (i >= numParms) {
        goto abort;
    }
    do {
        eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;
        if (eventParmIndex != 0 && eventParmIndex != 0xFF && eventParmIndex != 0xFE) {
            if ((validParmMask[i] & parms[eventParmIndex]) == 0) {
                match = 0;
                goto abort;
            }
        }
        i++;
    } while (i < numParms);
abort:
    return match;
}

static int iSPCH_ChooseSamples(SentencePickInfo *sentenceInfo, VoxSentence *sentence, PhrasePickInfo *phraseInfo, VoxPhrase *phrase, unsigned int *parms) {
    VOXBANKHDR *bank;
    int numSamples;
    int i;
    int numMatches;
    int bankHandle;
    int bankIndex;
    int sampleAge;
    int secondChoiceAge;
    int secondChoice;
    int postMatchParms;
    int sampleSize;
    unsigned char *sampleData;
    int numParms;

    postMatchParms = iSPCH_HasMatchParmToPost(phrase);
    numMatches = 0;
    bankIndex = phraseInfo->bankIndex;
    bank = gVoxBanks[bankIndex].voxHdr;
    secondChoiceAge = 0;
    secondChoice = -1;
    numSamples = bank->numSamples;
    if (iSPCH_BankHasValidSamples(phrase, bank, parms) == 0) {
        goto abort;
    }
        numParms = bank->parmFlags & 0x7F;
        sampleSize = numParms + 2;
        sampleData = reinterpret_cast<unsigned char *>(bank) + 0xE;
        i = 0;
        if (numMatches < numSamples) {
            do {
                if (iSPCH_MatchSample(sentence, phrase, parms, sampleData) != 0) {
                    if (iSPCH_SampleExists(phraseInfo, bank, i) != 0) {
                        sampleAge = iSPCHBank_GetSampleTimeInQueue(bank, i);
                        if (sampleAge > 0) {
                            if (sampleAge > secondChoiceAge) {
                                secondChoiceAge = sampleAge;
                                secondChoice = i;
                            }
                        } else {
                            if (iSPCH_AddSampleToValidPicks(sentenceInfo, i) != 0) {
                                numMatches++;
                            }
                            if (postMatchParms != 0) {
                                iSPCH_PostMatchParmValue(sentence, phrase, sampleData);
                            }
                        }
                    }
                }
                i++;
                sampleData += sampleSize;
            } while (i < numSamples);
        }
        if (numMatches == 0 && secondChoice != -1) {
            if (iSPCH_AddSampleToValidPicks(sentenceInfo, secondChoice) != 0) {
                numMatches = 1;
            }
            if (postMatchParms != 0) {
                iSPCH_PostMatchParmValue(sentence, phrase, reinterpret_cast<unsigned char *>(&bank[1]) + secondChoice * sampleSize + 2);
            }
        }
abort:
    return numMatches;
}

static void iSPCH_OrderSentences(VoxEvent *event, char *sentenceOrder) {
    VoxSentence *sentence;
    unsigned int weight[100];
    int numSentences;
    int numFound;
    int totalWeight;
    int rnd;
    int i;

    i = 0;
    numSentences = event->numSentences;
    totalWeight = 0;
    if (i < numSentences) {
        do {
            sentence = reinterpret_cast<VoxSentence *>(iSPCH_GetOffset16(reinterpret_cast<unsigned char *>(event), reinterpret_cast<unsigned short *>(event + 1), i));
            weight[i] = VOXSENTENCE_GetWeight(sentence);
            i++;
            totalWeight += VOXSENTENCE_GetWeight(sentence);
        } while (i < numSentences);
    }
    numFound = 0;
    while (totalWeight > 0) {
        rnd = iSPCH_Rand(totalWeight, -1);
        i = 0;
        if (i < numSentences) {
            rnd -= weight[0];
            if (rnd >= 0) {
                do {
                    i++;
                    if (i >= numSentences) {
                        break;
                    }
                    rnd -= weight[i];
                } while (rnd >= 0);
            }
        }
        sentenceOrder[numFound] = static_cast<char>(i);
        totalWeight -= weight[i];
        weight[i] = 0;
        numFound++;
    }
    i = 0;
    if (i < numSentences) {
        do {
            sentence = reinterpret_cast<VoxSentence *>(iSPCH_GetOffset16(reinterpret_cast<unsigned char *>(event), reinterpret_cast<unsigned short *>(event + 1), i));
            if (VOXSENTENCE_GetWeight(sentence) == 0) {
                sentenceOrder[numFound] = static_cast<char>(i);
                numFound++;
            }
            i++;
        } while (i < numSentences);
    }
}

static int iSPCH_TestMatchParms(SentencePickInfo *sentenceInfo, VoxSentence *sentence) {
    int i;
    int j;
    int numPhrases;
    int numFilters;
    int pickedIndex;
    int sampleIndex;
    int matchParmIndex;
    unsigned char *sampleParms;
    VOXBANKHDR *bank;
    int *matchParmIO;
    int result;
    int done;
    VoxPhrase *phrase;

    matchParmIO = VoxSentence_GetMatchParmIO(sentence);
    result = 1;
    done = 0;
    iSPCH_InitMatchParmIO(sentence);
    i = 0;
    do {
        sentenceInfo->phraseInfo[i].done = 0;
        i++;
    } while (i < 12);
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    if (done == 0) {
        do {
        i = 0;
        done = 1;
        if (i < numPhrases) {
            do {
                if (sentenceInfo->phraseInfo[i].done == 0) {
                    sentenceInfo->phraseInfo[i].done = 1;
                    bank = gVoxBanks[sentenceInfo->phraseInfo[i].bankIndex].voxHdr;
                    pickedIndex = sentenceInfo->phraseInfo[i].pickedIndex;
                    phrase = reinterpret_cast<VoxPhrase *>(iSPCH_GetOffset8(reinterpret_cast<unsigned char *>(sentence), reinterpret_cast<unsigned char *>(sentence) + 8, i));
                    numFilters = phrase->numFilters;
                    sampleIndex = sentenceInfo->validSamples[pickedIndex];
                    sampleParms = iSPCH_GetSampleParmAddr(bank, sampleIndex);
                    if (sampleParms == 0) {
                        result = 0;
                        goto exit;
                    }
                    j = 0;
                    if (j < numFilters) {
                        do {
                            matchParmIndex = iSPCH_GetPhraseParmInfo(phrase, j)->matchParmIndex;
                            if (matchParmIndex != 0xFF && (matchParmIndex & 0x80) != 0) {
                                matchParmIndex &= 0x7F;
                                if (iSPCH_GetPhraseParmInfo(phrase, j)->eventParmIndex == 0xFF) {
                                    if (matchParmIO[matchParmIndex] == 0) {
                                        sentenceInfo->phraseInfo[i].done = 0;
                                        done = 0;
                                    } else if (((matchParmIO[matchParmIndex] >> sampleParms[j]) & 1) == 0) {
                                        result = 0;
                                        goto exit;
                                    }
                                } else if (iSPCH_GetPhraseParmInfo(phrase, j)->eventParmIndex == 0xFE) {
                                    matchParmIO[matchParmIndex] = 1 << sampleParms[j];
                                }
                            }
                            j++;
                        } while (j < numFilters);
                    }
                }
                i++;
            } while (i < numPhrases);
        }
        } while (done == 0);
    }
exit:
    return result;
}

static int iSPCH_SentenceGetChoices(SentencePickInfo *sentenceInfo, VoxSentence *sentence, unsigned int *parms, int filterSetting) {
    int i;
    int numPhrases;
    int validSentence;
    int doneChecking;
    VoxPhrase *phrase;
    PhrasePickInfo *phraseInfo;
    int escapeHatch;

    validSentence = 1;
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    escapeHatch = 200;
    if (numPhrases > 12) {
        validSentence = 0;
        goto abort;
    }
    if (iSPCH_ShortRuleStatus(sentence, filterSetting) == 0) {
        validSentence = 0;
        goto abort;
    }
    iSPCH_InitMatchParmIO(sentence);
    sentenceInfo->numPhrases = static_cast<unsigned char>(numPhrases);
    do {
        i = 0;
        doneChecking = 1;
        escapeHatch--;
        if (i < numPhrases) {
            do {
                phraseInfo = &sentenceInfo->phraseInfo[i];
                {
                    int numValid;

                    phrase = reinterpret_cast<VoxPhrase *>(iSPCH_GetOffset8(reinterpret_cast<unsigned char *>(sentence), reinterpret_cast<unsigned char *>(sentence) + 8, i));
                    numValid = iSPCH_MatchParmInputsSet(sentence, phrase);
                    if (numValid == 0) {
                        doneChecking = 0;
                    } else if (phraseInfo->done == 0) {
                        phraseInfo->pickStart = sentenceInfo->pickIndex;
                        numValid = iSPCH_GetPhraseBank(phrase, parms, phraseInfo);
                        if (numValid == 0) {
                            validSentence = 0;
                            goto abort;
                        }
                        numValid = iSPCH_ChooseSamples(sentenceInfo, sentence, phraseInfo, phrase, parms);
                        if (numValid == 0) {
                            validSentence = 0;
                            goto abort;
                        }
                        phraseInfo->numPicks = static_cast<unsigned char>(numValid);
                        phraseInfo->done = 1;
                    }
                }
                i++;
            } while (i < numPhrases);
        }
    } while (doneChecking == 0 && escapeHatch > 0);
    if (escapeHatch == 0) {
        validSentence = 0;
    }
abort:
    return validSentence;
}

static void iSPCH_RandomizeSentencePicks(VoxSentence *sentence, SentencePickInfo *sentenceInfo) {
    char *picks;
    char temp;
    int randPickIndex;
    int i;
    int numPhrases;
    PhrasePickInfo *phraseInfo;

    i = 0;
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    if (i < numPhrases) {
        do {
            phraseInfo = &sentenceInfo->phraseInfo[i];
            picks = reinterpret_cast<char *>(sentenceInfo->validSamples) + phraseInfo->pickStart;
            {
                int j;
                int numPicks;

                numPicks = phraseInfo->numPicks;
                j = numPicks;
                if (j > 1) {
                    do {
                        randPickIndex = iSPCH_Rand(j, -1);
                        temp = picks[j - 1];
                        picks[j - 1] = picks[randPickIndex];
                        j--;
                        picks[randPickIndex] = temp;
                    } while (j > 1);
                }
            }
            i++;
        } while (i < numPhrases);
    }
}

static int iSPCH_IterateChoice(VoxSentence *sentence, SentencePickInfo *sentenceInfo) {
    int done;
    int doneIterate;
    int lastIndex;
    int lastPick;
    int numPicks;
    int numPhrases;
    PhrasePickInfo *phraseInfo;

    doneIterate = 0;
    done = 0;
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    lastIndex = numPhrases - 1;
    phraseInfo = &sentenceInfo->phraseInfo[lastIndex];
    lastPick = phraseInfo->numPicks + phraseInfo->pickStart;
    do {
        numPicks = phraseInfo->pickedIndex + 1;
        phraseInfo->pickedIndex = static_cast<unsigned char>(numPicks);
        if (static_cast<unsigned char>(numPicks) < lastPick) {
            doneIterate = 1;
        } else {
            lastIndex--;
            phraseInfo->pickedIndex = phraseInfo->pickStart;
            if (lastIndex < 0) {
                doneIterate = 1;
                done = 1;
            }
            phraseInfo = &sentenceInfo->phraseInfo[lastIndex];
            lastPick = phraseInfo->numPicks + phraseInfo->pickStart;
        }
    } while (doneIterate == 0);
    return done;
}

static int iSPCH_ChooseSentenceIteratively(VoxSentence *sentence, SentencePickInfo *sentenceInfo, int filterLength) {
    int done;
    int validSentence;
    int length;
    int i;
    int numPhrases;

    validSentence = 0;
    done = 0;
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    i = 0;
    if (i < numPhrases) {
        do {
            sentenceInfo->phraseInfo[i].pickedIndex = sentenceInfo->phraseInfo[i].pickStart;
            i++;
        } while (i < numPhrases);
    }
    while (done == 0 && validSentence == 0) {
        validSentence = 1;
        if (filterLength != 0) {
            length = iSPCH_SentenceLength(sentence, sentenceInfo);
            validSentence = length <= filterLength;
        }
        if (validSentence != 0) {
            if (sentence->numMatchParms != 0) {
                length = iSPCH_TestMatchParms(sentenceInfo, sentence);
                if (length == 0) {
                    validSentence = 0;
                }
            }
        }
        if (validSentence == 0) {
            done = iSPCH_IterateChoice(sentence, sentenceInfo);
        }
    }
    return validSentence;
}

static void iSPCH_PostGlobalMatchParms(EventSpec *eventSpec, SentencePickInfo *sentenceInfo, VoxSentence *sentence) {
    unsigned char *globalMatchParmsArray;
    int i;
    int j;
    int numPhrases;

    if (iSPCH_GetGlobalMatchParmsArray(eventSpec, &globalMatchParmsArray) == 0) {
        goto abort;
    }
    i = 0;
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    if (i >= numPhrases) {
        goto abort;
    }
    do {
        VoxPhrase *phrase;

        phrase = reinterpret_cast<VoxPhrase *>(iSPCH_GetOffset8(reinterpret_cast<unsigned char *>(sentence), reinterpret_cast<unsigned char *>(sentence) + 8, i));
        j = 0;
        if (j < phrase->numFilters) {
            do {
                int eventParmIndex;

                eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, j)->eventParmIndex;
                if (eventParmIndex == 0xFE && (iSPCH_GetPhraseParmInfo(phrase, j)->matchParmIndex & 0x80) == 0) {
                    VOXBANKHDR *bank;
                    unsigned char *sampleParms;
                    int pickedIndex;
                    int sampleIndex;

                    bank = gVoxBanks[sentenceInfo->phraseInfo[i].bankIndex].voxHdr;
                    pickedIndex = sentenceInfo->phraseInfo[i].pickedIndex;
                    sampleIndex = sentenceInfo->validSamples[pickedIndex];
                    sampleParms = iSPCH_GetSampleParmAddr(bank, sampleIndex);
                    if (sampleParms != 0) {
                        globalMatchParmsArray[iSPCH_GetPhraseParmInfo(phrase, j)->matchParmIndex] = sampleParms[j];
                    }
                }
                j++;
            } while (j < phrase->numFilters);
        }
        i++;
    } while (i < numPhrases);
abort:
    ;
}

static int iSPCH_SentenceMakeChoice(EventSpec *eventSpec, SentencePickInfo *sentenceInfo, VoxSentence *sentence, int filterSetting, int filterLength) {
    int i;
    int numPhrases;
    int foundSentence;
    PhrasePickInfo *phraseInfo;
    VoxPhrase *phrase;

    if (filterLength > 0 || sentence->numMatchParms != 0) {
        iSPCH_RandomizeSentencePicks(sentence, sentenceInfo);
        foundSentence = iSPCH_ChooseSentenceIteratively(sentence, sentenceInfo, filterLength);
    } else {
        i = 0;
        foundSentence = 1;
        numPhrases = VoxSentence_GetNumPhrases(sentence);
        if (i < numPhrases) {
            do {
                phraseInfo = &sentenceInfo->phraseInfo[i];
                phrase = reinterpret_cast<VoxPhrase *>(iSPCH_GetOffset8(reinterpret_cast<unsigned char *>(sentence), reinterpret_cast<unsigned char *>(sentence) + 8, i));
                if (phraseInfo->numPicks == 0) {
                    foundSentence = 0;
                    goto abort;
                }
                if (iSPCH_PickPhraseSample(phraseInfo) == 0) {
                    foundSentence = 0;
                    goto abort;
                }
                i++;
            } while (i < numPhrases);
        }
    }
    iSPCH_PostGlobalMatchParms(eventSpec, sentenceInfo, sentence);
abort:
    return foundSentence;
}

static void iSPCH_ConstantRuleSet(EventSpec *eventSpec, VoxEvent *event, VoxSentence *sentence) {
    int i;
    int j;
    int numPhrases;
    VoxPhrase *phrase;
    VOXBANKHDR *bank;
    unsigned char *sampleParms;
    int parmValue;
    int sampleIndex;
    int bankIndex;
    int ruleIndex;
    int ruleID;
    unsigned int datID;
    unsigned int channel;
    int numParms;
    EventDatInfo *datInfo;

    if (gCallbacks.setRule != 0) {
        if (iSPCH_FindEventDatInfo(eventSpec, &datInfo) != 0) {
            i = 0;
            numPhrases = VoxSentence_GetNumPhrases(sentence);
            channel = datInfo->channel;
            datID = datInfo->eventDat->datID;
            if (i < numPhrases) {
                do {
                    phrase = reinterpret_cast<VoxPhrase *>(iSPCH_GetOffset8(reinterpret_cast<unsigned char *>(sentence), reinterpret_cast<unsigned char *>(sentence) + 8, i));
                    j = 0;
                    numParms = phrase->numFilters;
                    if (j < numParms) {
                        do {
                            ruleIndex = iSPCH_GetPhraseParmInfo(phrase, j)->ruleIndex;
                            if (ruleIndex != 0xFF) {
                                ruleID = iSPCH_GetRuleID(event, ruleIndex);
                                bankIndex = gEventChoice[channel].phrases[i].bankIndex;
                                bank = gVoxBanks[bankIndex].voxHdr;
                                sampleIndex = gEventChoice[channel].phrases[i].sampleIndex;
                                sampleParms = iSPCH_GetSampleParmAddr(bank, sampleIndex);
                                if (sampleParms != 0) {
                                    parmValue = sampleParms[j];
                                    {
                                        int flag;

                                        flag = 1 << parmValue;
                                        gCallbacks.setRule(eventSpec, ruleID, flag, datID);
                                    }
                                }
                            }
                            j++;
                        } while (j < numParms);
                    }
                    i++;
                } while (i < numPhrases);
            }
        }
    }
abort:
    ;
}

static int iSPCH_MakeSampleRequests(VoxEvent *event, VoxSentence *sentence, EventSpec *eventSpec) {
    VOXBANKHDR *bank;
    SPCHType_SampleRequestData sampleRequestData;
    int sampleIndex;
    unsigned int channel;
    int i;
    int numPhrases;
    int bankIndex;
    int bankHandle;
    int totalBytes;
    int sentenceLength;
    PhraseChoice *phraseChoice;
    unsigned int datID;
    EventDatInfo *datInfo;

    totalBytes = 0;
    sentenceLength = 0;
    if (gCallbacks.request == 0) {
        goto abort;
    }
    if (iSPCH_FindEventDatInfo(eventSpec, &datInfo) == 0) {
        goto abort;
    }
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    channel = datInfo->channel;
    datID = datInfo->eventDat->datID;
    i = 0;
    if (i < numPhrases) {
        do {
            phraseChoice = &gEventChoice[channel].phrases[i];
            bankIndex = phraseChoice->bankIndex;
            if (gVoxBanks[bankIndex].bankHandle != phraseChoice->bankHandle) {
                phraseChoice->bankIndex = static_cast<short>(iSPCH_FindBankIndexFromHandle(phraseChoice->bankHandle));
                if ((phraseChoice->bankIndex & 0x8000) != 0) {
                    goto abort;
                }
            }
            i++;
        } while (i < numPhrases);
    }
    i = 0;
    if (i < numPhrases) {
        do {
            phraseChoice = &gEventChoice[channel].phrases[i];
            bankHandle = phraseChoice->bankHandle;
            bankIndex = phraseChoice->bankIndex;
            sampleIndex = phraseChoice->sampleIndex;
            bank = gVoxBanks[bankIndex].voxHdr;
            if ((bank->parmFlags & 0x80) != 0 && gClearCycle != 0) {
                iSPCH_ClearCycleBit(bank, sampleIndex);
            }
            {
                unsigned int sampleOffset;
                unsigned int sampleBytes;

                if (iSPCH_GetSampleSizeData(bank, sampleIndex, &sampleOffset, &sampleBytes) != 0) {
                    unsigned int dataOffset;
                    int bankBytes;

                    bankBytes = (bank->blockSize + 1) << 8;
                    dataOffset = bank->bankBlocks * bankBytes;
                    if (phraseChoice->subBankIndex != -1) {
                        dataOffset *= phraseChoice->subBankIndex;
                        sampleOffset += dataOffset;
                    }
                    totalBytes += sampleBytes;
                    sampleRequestData.bankNum = bankHandle;
                    sampleRequestData.sampleOffset = sampleOffset;
                    sampleRequestData.numBytes = sampleBytes;
                    sampleRequestData.eventSpec = *eventSpec;
                    sampleRequestData.channel = channel;
                    sampleRequestData.subID = datID;
                    sampleRequestData.datID = bank->subID;
                    if (i == 0) {
                        sampleRequestData.interruptFlag = VoxEvent_GetInterruptFlag(event);
                    } else {
                        sampleRequestData.interruptFlag = 0;
                    }
                    gCallbacks.request(&sampleRequestData);
                    iSPCHBank_AddToQueue(bank, sampleIndex);
                }
            }
            i++;
        } while (i < numPhrases);
    }
    sentenceLength = iSPCH_ConvertTime(totalBytes);
abort:
    return sentenceLength;
}

int iSPCH_PlayChosen(unsigned int inChannel) {
    int sentenceLength;
    EventSpec eventSpec;
    int numPhrases;

    numPhrases = 0;
    if (inChannel < 8) {
        sentenceLength = iSPCH_OneChosen(inChannel);
        if (sentenceLength != 0) {
            eventSpec = *reinterpret_cast<EventSpec *>(gEventChoice[inChannel].memParms);
            iSPCH_RuleSet(&eventSpec, gEventChoice[inChannel].event, gEventChoice[inChannel].sentenceNum, gEventChoice[inChannel].memParms);
            iSPCH_ConstantRuleSet(&eventSpec, gEventChoice[inChannel].event, gEventChoice[inChannel].sentence);
            iSPCH_MakeSampleRequests(gEventChoice[inChannel].event, gEventChoice[inChannel].sentence, &eventSpec);
            numPhrases = VoxSentence_GetNumPhrases(gEventChoice[inChannel].sentence);
            if (eventSpec.eventID == gVoxInGame[inChannel].lastEventSpec.eventID) {
                gVoxInGame[inChannel].numEventTimes++;
            } else {
                gVoxInGame[inChannel].numEventTimes = 1;
                gVoxInGame[inChannel].lastEventSpec = eventSpec;
            }
        }
    }
abort:
    iSPCH_ClearSentenceChoiceChannel(inChannel);
    return numPhrases;
}

int iSPCH_ChooseSingleSentence(int choice) {
    unsigned int *parms;
    int validSentence;
    int count;
    int retry;

    validSentence = 0;
    parms = gVoxEvents.events[choice].memParms;
    if (parms != 0) {
        validSentence = iSPCH_ChooseSentence(parms);
        if (validSentence == 0 && gCallbacks.reparm != 0) {
            count = 0;
            do {
                retry = gCallbacks.reparm(count, parms);
                if (retry >= 0) {
                    validSentence = iSPCH_ChooseSentence(parms);
                }
                count++;
            } while (validSentence == 0 && retry > 0);
        }
        if (validSentence < 0) {
            validSentence = 0;
        }
    }
abort:
    return validSentence;
}

static int iSPCH_SentenceIsContextMatch(VoxEvent *event, VoxSentence *sentence, unsigned int *inParms) {
    int i;
    int result;
    unsigned char *contextData;
    SRule rule;
    int contextParmIndex;
    int numContexts;
    int *sentenceContexts;

    result = 1;
    if ((numContexts = VoxEvent_GetNumContexts(event)) != 0) {
        sentenceContexts = VoxSentence_GetContextFlagsAddr(sentence);
        contextData = iSPCH_GetContextDataAddr(event);
        for (i = 0; i < numContexts; i++) {
            rule.ruleID = contextData[i * 3];
            rule.parmIndex = contextData[i * 3 + 1];
            rule.parmType = static_cast<ParmType>(contextData[i * 3 + 2]);
            if (sentenceContexts[i] != 0) {
                contextParmIndex = rule.parmIndex;
                if ((sentenceContexts[i] & inParms[contextParmIndex]) == 0) {
                    result = 0;
                    goto abort;
                }
            }
        }
    }
abort:
    return result;
}
