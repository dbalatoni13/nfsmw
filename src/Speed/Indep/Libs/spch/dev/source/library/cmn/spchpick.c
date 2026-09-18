#include "./spchi.h"

// total size: 0xC
struct PhraseChoice {
    int bankHandle;             // offset 0x0, size 0x4
    short bankIndex;            // offset 0x4, size 0x2
    short subBankIndex;         // offset 0x6, size 0x2
    unsigned short sampleIndex; // offset 0x8, size 0x2
    unsigned short pad;         // offset 0xA, size 0x2
};

// total size: 0xA0
struct EventChoice {
    VoxEvent *event;            // offset 0x0, size 0x4
    VoxSentence *sentence;      // offset 0x4, size 0x4
    unsigned char sentenceNum;  // offset 0x8, size 0x1
    unsigned char validChoice;  // offset 0x9, size 0x1
    unsigned char numPhrases;   // offset 0xA, size 0x1
    unsigned char pad1;         // offset 0xB, size 0x1
    unsigned int *memParms;     // offset 0xC, size 0x4
    PhraseChoice phrases[12];   // offset 0x10, size 0x90
};

// total size: 0xC
struct PhrasePickInfo {
    int bankHandle;             // offset 0x0, size 0x4
    short bankIndex;            // offset 0x4, size 0x2
    short subBankIndex;         // offset 0x6, size 0x2
    unsigned char pickStart;    // offset 0x8, size 0x1
    unsigned char numPicks;     // offset 0x9, size 0x1
    unsigned char pickedIndex;  // offset 0xA, size 0x1
    unsigned char done;         // offset 0xB, size 0x1
};

// total size: 0x15C
struct SentencePickInfo {
    PhrasePickInfo phraseInfo[12];    // offset 0x0, size 0x90
    unsigned char numPhrases;         // offset 0x90, size 0x1
    unsigned char pickIndex;          // offset 0x91, size 0x1
    unsigned char validSamples[200];  // offset 0x92, size 0xC8
};

// total size: 0x8
struct VOXINGAME {
    EventSpec lastEventSpec; // offset 0x0, size 0x4
    int numEventTimes;       // offset 0x4, size 0x4
};

// total size: 0x8
struct VoxBankInfo {
    int bankHandle;    // offset 0x0, size 0x4
    VOXBANKHDR *voxHdr; // offset 0x4, size 0x4
};

// total size: 0x14
struct SPCH_Callbacks {
    int (*request)(SPCHType_SampleRequestData *);       // offset 0x0, size 0x4
    int (*testRule)(EventSpec *, int, int, int);        // offset 0x4, size 0x4
    void (*setRule)(EventSpec *, int, int, int);        // offset 0x8, size 0x4
    SPCHType_EventRuleResult (*eventRule)(EventSpec *); // offset 0xC, size 0x4
    int (*reparm)(int, unsigned int *);                 // offset 0x10, size 0x4
};

// total size: 0x10
struct VoxEventItem {
    unsigned int entryTime;  // offset 0x0, size 0x4
    unsigned short subTicks; // offset 0x4, size 0x2
    unsigned char pending;   // offset 0x6, size 0x1
    unsigned char channel;   // offset 0x7, size 0x1
    VoxEvent *event;         // offset 0x8, size 0x4
    unsigned int *memParms;  // offset 0xC, size 0x4
};

// total size: 0x140
struct VoxPendingEvents {
    int numPending[8];       // offset 0x0, size 0x20
    int lastAddedEvent[8];   // offset 0x20, size 0x20
    VoxEventItem events[16]; // offset 0x40, size 0x100
};

extern SPCH_Callbacks gCallbacks;
extern VoxPendingEvents gVoxEvents;
extern VoxBankInfo *gVoxBanks;
extern VOXINGAME gVoxInGame[8];
extern int gDataRate;
extern int gFilterSetting[8];
extern int gPreLoadTicks;
extern int gClearCycle;

static EventChoice gEventChoice[8];
static int multiple[8] = {1, 4, 16, 64, 256, 1024, 4096, 16384};

void *iSPCH_MemAlloc(unsigned int numBytes);
void iSPCH_MemFree(void *data);
int iSPCH_Rand(int range, int seed);

int iSPCH_FindEventDatInfo(EventSpec *eventSpec, EventDatInfo **datInfo);
int iSPCH_GetGlobalMatchParmsArray(EventSpec *eventSpec, unsigned char **matchParmArray);
int iSPCH_GetRuleID(VoxEvent *event, int ruleIndex);
void iSPCH_RuleSet(EventSpec *eventSpec, VoxEvent *event, int sentenceNum, unsigned int *parms);

short iSPCH_FindBank(unsigned short bankID, int &bankHandle);
short iSPCH_FindSubBank(unsigned short bankID, unsigned short subBankID, int &bankHandle);
int iSPCH_TestSubBankBounds(int bankHandle, unsigned int subBankID);
short iSPCH_FindBankIndexFromHandle(int bankHandle);
int iSPCHBank_GetSampleTimeInQueue(VOXBANKHDR *bank, int sampleIndex);
void iSPCHBank_AddToQueue(VOXBANKHDR *bank, int sampleIndex);
int iSPCH_GetSampleSizeData(VOXBANKHDR *bank, int sampleIndex, unsigned int *sampleOffset,
                            unsigned int *sampleBytes);
unsigned char *iSPCH_GetSampleParmAddr(VOXBANKHDR *bank, int sampleIndex);

VoxEvent *iSPCH_FindEvent(EventSpec *eventSpec);
int iSPCH_FindEventChannel(EventSpec *eventSpec, unsigned int *channel);
int iSPCH_GetFilterLength(EventSpec *eventSpec);
unsigned int iSPCH_GetRuleSettings(EventSpec *eventSpec, VoxEvent *event, unsigned int *parms,
                                   unsigned int *userSettingsMask);
int iSPCH_CheckSentenceRules(VoxEvent *event, int sentenceNum, unsigned int eventSettings,
                             unsigned int userSettingsMask);

void SPCH_GetEventDatInfo(char *eventData, int *projID, int *datID);
int iSPCH_ChooseSentence(unsigned int *parms);
int iSPCH_ChooseSingleSentence(int choice);
void SPCH_SetPreLoadTicks(int ticks);
unsigned int iSPCH_DecodeWeight(unsigned char weight);
void iSPCH_ClearSentenceChoiceChannel(unsigned int channel);
void iSPCH_InitSentenceChoice(void);
int iSPCH_OneChosen(unsigned int inChannel);
int iSPCH_PlayChosen(unsigned int inChannel);

void SPCH_GetEventDatInfo(char *eventData, int *projID, int *datID) {
    *projID = ((VoxData *)eventData)->projectID;
    *datID = ((VoxData *)eventData)->datID;
}

static void iSPCH_PostMatchParmValue(VoxSentence *sentence, VoxPhrase *phrase,
                                     unsigned char *sampleParms) {
    int *matchParmIO;
    int numFilters;
    int eventParmIndex;
    int i;

    numFilters = phrase->numFilters;
    matchParmIO = VoxSentence_GetMatchParmIO(sentence);

    for (i = 0; i < numFilters; i++) {
        eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;
        if (eventParmIndex == 0xFE) {
            int matchParmIndex;

            matchParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->matchParmIndex;

            if (matchParmIndex & 0x80) {
                int sampleBitFlag;

                sampleBitFlag = 1 << sampleParms[i];

                matchParmIO[matchParmIndex & 0x7F] |= sampleBitFlag;
            }
        }
    }
}

static int iSPCH_HasMatchParmToPost(VoxPhrase *phrase) {
    int numFilters;
    int eventParmIndex;
    int i;
    int result;

    result = 0;

    numFilters = phrase->numFilters;

    for (i = 0; i < numFilters; i++) {
        eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;
        if (eventParmIndex == 0xFE) {
            result = 1;
            break;
        }
    }

    return result;
}

static int iSPCH_MatchSample(VoxSentence *sentence, VoxPhrase *phrase, unsigned int *parms,
                             unsigned char *sampleParms) {
    int i;
    int numRules;
    int match;
    int sampleParmValue;
    int eventParmIndex;
    unsigned int matchValue;

    match = 1;
    numRules = phrase->numFilters;

    for (i = 0; i < numRules; i++) {
        unsigned int sampleParmBitFlags;

        match = 0;

        sampleParmValue = sampleParms[i];
        if ((unsigned int)sampleParmValue > 31) {
            goto abort;
        }
        sampleParmBitFlags = 1 << sampleParmValue;

        eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;
        matchValue = iSPCH_GetPhraseParmInfo(phrase, i)->matchValues;

        if ((sampleParmBitFlags & matchValue) != 0) {
            if (eventParmIndex == 0 || eventParmIndex == 0xFE) {
                match = 1;
            } else if (eventParmIndex == 0xFF) {
                int matchParmIndex;

                matchParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->matchParmIndex;
                if (matchParmIndex & 0x80) {
                    int *matchParmIO;

                    matchParmIO = VoxSentence_GetMatchParmIO(sentence);
                    if ((sampleParmBitFlags & matchParmIO[matchParmIndex & 0x7F]) != 0) {
                        match = 1;
                    }
                } else {
                    EventSpec eventSpec;
                    unsigned char *globalMatchParmArray;

                    *(int *)&eventSpec = parms[0];

                    if (iSPCH_GetGlobalMatchParmsArray(&eventSpec, &globalMatchParmArray)) {
                        if (sampleParmValue == globalMatchParmArray[matchParmIndex]) {
                            match = 1;
                        }
                    }
                }
            } else {
                unsigned int inParm;

                inParm = parms[eventParmIndex];

                if ((sampleParmBitFlags & inParm) != 0) {
                    match = 1;
                }
            }
        }

        if (match == 0) {
            goto abort;
        }
    }

abort:
    return match;
}

static int iSPCH_GetPhraseBank(VoxPhrase *phrase, unsigned int *parms, PhrasePickInfo *phrasePick) {
    int bankHandle;
    int bankIndex;
    BankType bankType;
    int parmIndex;
    int success;
    unsigned short bankID;
    unsigned short subBankID;

    bankHandle = -1;
    bankIndex = -1;

    bankID = phrase->bankID;

    bankType = (BankType)phrase->bankType;
    parmIndex = phrase->bankIDIndex;

    phrasePick->subBankIndex = -1;
    phrasePick->bankHandle = -1;
    phrasePick->bankIndex = -1;

    switch (bankType) {
    case kBankType_Single:
        phrasePick->bankIndex = iSPCH_FindBank(bankID, bankHandle);
        phrasePick->bankHandle = (short)bankHandle;
        break;

    case kBankType_Multi:
        subBankID = parms[parmIndex];
        phrasePick->bankIndex = iSPCH_FindSubBank(bankID, subBankID, bankHandle);
        phrasePick->bankHandle = bankHandle;
        break;

    case kBankType_Array:
        bankIndex = iSPCH_FindBank(bankID, bankHandle);

        if (iSPCH_TestSubBankBounds(bankIndex, parms[parmIndex])) {
            phrasePick->subBankIndex = parms[parmIndex];
        } else {
            bankHandle = -1;
        }

        phrasePick->bankIndex = bankIndex;
        phrasePick->bankHandle = bankHandle;
        break;
    }

    success = (phrasePick->bankHandle >= 0);

    return success;
}

static void iSPCH_ClearCycleBit(VOXBANKHDR *bank, int sampleIndex) {
    unsigned char *bankBits;
    unsigned char mask;
    int byteIndex;
    int bit;

    byteIndex = sampleIndex / 8 + 1;
    bit = sampleIndex % 8;
    mask = ~(1 << bit);

    bankBits = BANKHDR_GetCycleBitsAddr(bank);

    bankBits[byteIndex] &= mask;
}

static int iSPCH_TestBit(unsigned char *bitArray, int bitIndex) {
    unsigned char mask;
    int byteIndex;
    int bit;
    int result;

    byteIndex = bitIndex / 8;
    bit = bitIndex % 8;
    mask = 1 << bit;

    result = bitArray[byteIndex];

    return result & mask;
}

static int iSPCH_CheckTemplateSample(PhrasePickInfo *phraseInfo, VOXBANKHDR *bank,
                                     int sampleIndex) {
    int result;
    unsigned char *arrayData;

    result = 0;

    if (bank->numSubBanks > phraseInfo->subBankIndex) {
        sampleIndex += bank->numSamples * phraseInfo->subBankIndex;

        arrayData = BANKHDR_GetArrayBankBitsAddr(bank);

        result = iSPCH_TestBit(arrayData, sampleIndex);
    }

    return result;
}

static int iSPCH_SampleExists(PhrasePickInfo *phraseInfo, VOXBANKHDR *bank, int sampleIndex) {
    int result;
    unsigned char *cycleData;

    result = 1;

    if (sampleIndex > bank->numSamples) {
        result = 0;
    } else {
        if (phraseInfo->subBankIndex != -1) {
            result = iSPCH_CheckTemplateSample(phraseInfo, bank, sampleIndex);
            if (result == 0) {
                goto abort;
            }
        }

        if (bank->parmFlags & 0x80) {
            cycleData = BANKHDR_GetCycleBitsAddr(bank);
            result = iSPCH_TestBit(cycleData + 1, sampleIndex);
        }
    }

abort:
    return result;
}

static int iSPCH_AddSampleToValidPicks(SentencePickInfo *info, int sampleIndex) {
    int result;

    result = 0;

    if (info->pickIndex <= 199) {
        info->validSamples[info->pickIndex] = sampleIndex;
        info->pickIndex++;
        result = 1;
    }

    return result;
}

static int iSPCH_BankHasValidSamples(VoxPhrase *phrase, VOXBANKHDR *bank, unsigned int *parms) {
    unsigned int *validParmMask;
    int i;
    int numParms;
    int eventParmIndex;
    int match;

    match = 1;

    validParmMask = (unsigned int *)BANKHDR_GetValidParmMask(bank);
    numParms = bank->parmFlags & 0x7F;

    for (i = 0; i < numParms; i++) {
        eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex;

        if (eventParmIndex != 0 && eventParmIndex != 0xFF && eventParmIndex != 0xFE) {
            if ((validParmMask[i] & parms[eventParmIndex]) == 0) {
                match = 0;
                goto abort;
            }
        }
    }

abort:
    return match;
}

static int iSPCH_ChooseSamples(SentencePickInfo *sentenceInfo, VoxSentence *sentence,
                               PhrasePickInfo *phraseInfo, VoxPhrase *phrase,
                               unsigned int *parms) {
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
    unsigned char *sampleTable;
    int numParms;

    numMatches = 0;

    secondChoiceAge = 0;
    secondChoice = -1;

    postMatchParms = iSPCH_HasMatchParmToPost(phrase);

    bankIndex = phraseInfo->bankIndex;
    bank = gVoxBanks[bankIndex].voxHdr;
    numSamples = bank->numSamples;

    if (iSPCH_BankHasValidSamples(phrase, bank, parms)) {
        numParms = bank->parmFlags & 0x7F;

        sampleData = (unsigned char *)bank + 0xE;
        sampleSize = numParms + 2;

        for (i = 0; i < numSamples; i++) {
            if (iSPCH_MatchSample(sentence, phrase, parms, sampleData)) {
                if (iSPCH_SampleExists(phraseInfo, bank, i)) {
                    sampleAge = iSPCHBank_GetSampleTimeInQueue(bank, i);
                    if (sampleAge > 0) {
                        if (sampleAge > secondChoiceAge) {
                            secondChoiceAge = sampleAge;
                            secondChoice = i;
                        }
                    } else {
                        if (iSPCH_AddSampleToValidPicks(sentenceInfo, i)) {
                            numMatches++;
                        }

                        if (postMatchParms) {
                            iSPCH_PostMatchParmValue(sentence, phrase, sampleData);
                        }
                    }
                }
            }

            sampleData += sampleSize;
        }

        if (numMatches == 0 && secondChoice != -1) {
            if (iSPCH_AddSampleToValidPicks(sentenceInfo, secondChoice)) {
                numMatches = 1;
            }

            if (postMatchParms) {
                sampleTable = (unsigned char *)bank + 0xC;
                sampleData = sampleTable + secondChoice * sampleSize;

                iSPCH_PostMatchParmValue(sentence, phrase, sampleData + 2);
            }
        }
    }

    return numMatches;
}

static int iSPCH_ConvertTime(int dataBytes) {
    int centiSecs;

    centiSecs = 0;

    if (gDataRate != 0) {
        centiSecs = dataBytes * 100 / gDataRate;
    }

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

    numPhrases = VoxSentence_GetNumPhrases(sentence);
    i = 0;
    if (numPhrases != 0) {
        do {
            unsigned int sampleOffset;
            unsigned int sampleBytes;

            pickedIndex = sentenceInfo->phraseInfo[i].pickedIndex;
            sampleIndex = sentenceInfo->validSamples[pickedIndex];
            bank = gVoxBanks[sentenceInfo->phraseInfo[i].bankIndex].voxHdr;

            if (iSPCH_GetSampleSizeData(bank, sampleIndex, &sampleOffset, &sampleBytes)) {
                dataBytes += sampleBytes;
            }
            i++;
        } while (i < numPhrases);
    }

    centiSecs = iSPCH_ConvertTime(dataBytes);

    return centiSecs;
}

unsigned int iSPCH_DecodeWeight(unsigned char weight) {
    int exp;
    unsigned int mant;
    int num;

    mant = weight & 0x1F;
    exp = weight >> 5;
    num = multiple[exp];

    return mant * num;
}

static void iSPCH_OrderSentences(VoxEvent *event, char *sentenceOrder) {
    VoxSentence *sentence;
    unsigned int weight[100];
    int numSentences;
    int numFound;
    int totalWeight;
    int rnd;
    int i;

    numSentences = event->numSentences;

    totalWeight = 0;
    for (i = 0; i < numSentences; i++) {
        sentence = (VoxSentence *)iSPCH_GetOffset16((unsigned char *)event,
                                                    (unsigned short *)(event + 1), i);
        weight[i] = iSPCH_DecodeWeight(sentence->expWeight);
        totalWeight += iSPCH_DecodeWeight(sentence->expWeight);
    }

    numFound = 0;
    while (totalWeight > 0) {
        rnd = iSPCH_Rand(totalWeight, -1);
        for (i = 0; i < numSentences; i++) {
            rnd -= weight[i];
            if (rnd < 0) {
                break;
            }
        }
        sentenceOrder[numFound] = i;
        totalWeight -= weight[i];
        weight[i] = 0;
        numFound++;
    }

    for (i = 0; i < numSentences; i++) {
        sentence = (VoxSentence *)iSPCH_GetOffset16((unsigned char *)event,
                                                    (unsigned short *)(event + 1), i);
        if (iSPCH_DecodeWeight(sentence->expWeight) == 0) {
            sentenceOrder[numFound] = i;
            numFound++;
        }
    }
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
            result = (gVoxInGame[channel].numEventTimes < numInARow);
        }
    }

    return result;
}

static int iSPCH_ShortRuleStatus(VoxSentence *sentence, int filterSetting) {
    int shortStatus;
    int result;

    result = 0;

    shortStatus = VoxSentence_GetShortRule(sentence);

    if (shortStatus == kRuleState_IfTrue && filterSetting == 2) {
        return 1;
    }
    if (shortStatus == kRuleState_IfFalse && filterSetting != 2) {
        return 1;
    }
    if (shortStatus == kRuleState_Ignore) {
        result = 1;
    }

    return result;
}

static void iSPCH_InitMatchParmIO(VoxSentence *sentence) {
    int i;
    int *matchParmIO;

    matchParmIO = VoxSentence_GetMatchParmIO(sentence);

    for (i = 0; i < sentence->numMatchParms; i++) {
        matchParmIO[i] = 0;
    }
}

static int iSPCH_MatchParmInputsSet(VoxSentence *sentence, VoxPhrase *phrase) {
    int i;
    int result;
    int numFilters;

    result = 1;

    numFilters = phrase->numFilters;

    for (i = 0; i < numFilters; i++) {
        if (iSPCH_GetPhraseParmInfo(phrase, i)->eventParmIndex == 0xFF) {
            int matchParmIndex;

            matchParmIndex = iSPCH_GetPhraseParmInfo(phrase, i)->matchParmIndex;
            if (matchParmIndex == 0xFF) {
                continue;
            }

            if (matchParmIndex & 0x80) {
                int *matchParm;

                matchParmIndex &= 0x7F;

                matchParm = VoxSentence_GetMatchParmIO(sentence);
                if (matchParm != 0) {
                    if (matchParm[matchParmIndex] == 0) {
                        result = 0;
                        break;
                    }
                }
            }
        }
    }

    return result;
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

    result = 1;
    done = 0;

    matchParmIO = VoxSentence_GetMatchParmIO(sentence);

    iSPCH_InitMatchParmIO(sentence);

    for (i = 0; i <= 11; i++) {
        sentenceInfo->phraseInfo[i].done = 0;
    }

    numPhrases = VoxSentence_GetNumPhrases(sentence);

    while (done == 0) {
        done = 1;

        for (i = 0; i < numPhrases; i++) {
            if (sentenceInfo->phraseInfo[i].done != 0) {
                continue;
            }

            sentenceInfo->phraseInfo[i].done = 1;

            phrase = (VoxPhrase *)iSPCH_GetOffset8((unsigned char *)sentence,
                                                   (unsigned char *)(sentence + 1), i);
            numFilters = phrase->numFilters;

            bank = gVoxBanks[sentenceInfo->phraseInfo[i].bankIndex].voxHdr;
            pickedIndex = sentenceInfo->phraseInfo[i].pickedIndex;
            sampleIndex = sentenceInfo->validSamples[pickedIndex];

            sampleParms = iSPCH_GetSampleParmAddr(bank, sampleIndex);
            if (sampleParms == 0) {
                result = 0;
                goto abort;
            }

            for (j = 0; j < numFilters; j++) {
                matchParmIndex = iSPCH_GetPhraseParmInfo(phrase, j)->matchParmIndex;
                if (matchParmIndex == 0xFF) {
                    continue;
                }

                if (matchParmIndex & 0x80) {
                    matchParmIndex &= 0x7F;

                    if (iSPCH_GetPhraseParmInfo(phrase, j)->eventParmIndex == 0xFF) {
                        if (matchParmIO[matchParmIndex] == 0) {
                            sentenceInfo->phraseInfo[i].done = 0;
                            done = 0;
                            continue;
                        }

                        if ((matchParmIO[matchParmIndex] >> sampleParms[j] & 1) == 0) {
                            result = 0;
                            goto abort;
                        }
                    } else if (iSPCH_GetPhraseParmInfo(phrase, j)->eventParmIndex == 0xFE) {
                        matchParmIO[matchParmIndex] = 1 << sampleParms[j];
                    }
                }
            }
        }
    }

abort:
    return result;
}

static int iSPCH_SentenceGetChoices(SentencePickInfo *sentenceInfo, VoxSentence *sentence,
                                    unsigned int *parms, int filterSetting) {
    int i;
    int numPhrases;
    int validSentence;
    int doneChecking;
    VoxPhrase *phrase;
    PhrasePickInfo *phraseInfo;
    int escapeHatch;

    validSentence = 1;

    escapeHatch = 200;

    numPhrases = VoxSentence_GetNumPhrases(sentence);

    if (numPhrases > 12) {
        goto abort;
    }

    if (!iSPCH_ShortRuleStatus(sentence, filterSetting)) {
        goto abort;
    }

    iSPCH_InitMatchParmIO(sentence);

    sentenceInfo->numPhrases = numPhrases;

    do {
        doneChecking = 1;
        escapeHatch--;

        for (i = 0; i < numPhrases; i++) {
            int numValid;

            phraseInfo = &sentenceInfo->phraseInfo[i];
            phrase = (VoxPhrase *)iSPCH_GetOffset8((unsigned char *)sentence,
                                                   (unsigned char *)(sentence + 1), i);

            if (!iSPCH_MatchParmInputsSet(sentence, phrase)) {
                doneChecking = 0;
                continue;
            }

            if (phraseInfo->done != 0) {
                continue;
            }

            phraseInfo->pickStart = sentenceInfo->pickIndex;

            if (!iSPCH_GetPhraseBank(phrase, parms, phraseInfo)) {
                goto abort;
            }

            numValid = iSPCH_ChooseSamples(sentenceInfo, sentence, phraseInfo, phrase, parms);
            if (numValid == 0) {
                goto abort;
            }

            phraseInfo->numPicks = numValid;
            phraseInfo->done = 1;
        }
    } while (doneChecking == 0 && escapeHatch > 0);

    if (escapeHatch != 0) {
        goto done;
    }

abort:
    validSentence = 0;

done:
    return validSentence;
}

static void iSPCH_RandomizeSentencePicks(VoxSentence *sentence, SentencePickInfo *sentenceInfo) {
    unsigned char *picks;
    unsigned char temp;
    int randPickIndex;
    int i;
    int numPhrases;
    PhrasePickInfo *phraseInfo;

    numPhrases = VoxSentence_GetNumPhrases(sentence);

    for (i = 0; i < numPhrases; i++) {
        int j;
        int numPicks;

        phraseInfo = &sentenceInfo->phraseInfo[i];

        picks = &sentenceInfo->validSamples[phraseInfo->pickStart];

        j = phraseInfo->numPicks;
        while (j > 1) {
            randPickIndex = iSPCH_Rand(j, -1);

            temp = picks[j - 1];
            picks[j - 1] = picks[randPickIndex];
            picks[randPickIndex] = temp;

            j--;
        }
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

    done = 0;

    doneIterate = 0;
    lastIndex = VoxSentence_GetNumPhrases(sentence) - 1;

    phraseInfo = &sentenceInfo->phraseInfo[lastIndex];
    numPicks = phraseInfo->numPicks;
    lastPick = phraseInfo->pickStart + numPicks;

    do {
        phraseInfo->pickedIndex++;
        if (phraseInfo->pickedIndex < lastPick) {
            doneIterate = 1;
        } else {
            phraseInfo->pickedIndex = phraseInfo->pickStart;

            lastIndex--;
            if (lastIndex < 0) {
                doneIterate = 1;
                done = 1;
            }

            phraseInfo = &sentenceInfo->phraseInfo[lastIndex];
            numPicks = phraseInfo->numPicks;
            lastPick = phraseInfo->pickStart + numPicks;
        }
    } while (doneIterate == 0);

    return done;
}

static int iSPCH_ChooseSentenceIteratively(VoxSentence *sentence, SentencePickInfo *sentenceInfo,
                                           int filterLength) {
    int done;
    int validSentence;
    int length;
    int i;
    int numPhrases;

    done = 0;
    validSentence = 0;

    numPhrases = VoxSentence_GetNumPhrases(sentence);

    for (i = 0; i < numPhrases; i++) {
        sentenceInfo->phraseInfo[i].pickedIndex = sentenceInfo->phraseInfo[i].pickStart;
    }

    while (done == 0 && validSentence == 0) {
        validSentence = 1;

        if (filterLength != 0) {
            length = iSPCH_SentenceLength(sentence, sentenceInfo);
            validSentence = (length <= filterLength);
        }

        if (validSentence != 0 && sentence->numMatchParms != 0) {
            if (!iSPCH_TestMatchParms(sentenceInfo, sentence)) {
                validSentence = 0;
            }
        }

        if (validSentence == 0) {
            done = iSPCH_IterateChoice(sentence, sentenceInfo);
        }
    }

    return validSentence;
}

static int iSPCH_PickPhraseSample(PhrasePickInfo *phraseInfo) {
    VOXBANKHDR *bank;
    int pickNum;
    int samplePicked;

    pickNum = iSPCH_Rand(phraseInfo->numPicks, phraseInfo->bankIndex);
    phraseInfo->pickedIndex = phraseInfo->pickStart + pickNum;

    return 1;
}

static void iSPCH_PostGlobalMatchParms(EventSpec *eventSpec, SentencePickInfo *sentenceInfo,
                                       VoxSentence *sentence) {
    unsigned char *globalMatchParmsArray;
    int i;
    int j;
    int numPhrases;

    if (!iSPCH_GetGlobalMatchParmsArray(eventSpec, &globalMatchParmsArray)) {
        return;
    }

    numPhrases = VoxSentence_GetNumPhrases(sentence);

    for (i = 0; i < numPhrases; i++) {
        VoxPhrase *phrase;

        phrase = (VoxPhrase *)iSPCH_GetOffset8((unsigned char *)sentence,
                                               (unsigned char *)(sentence + 1), i);

        for (j = 0; j < phrase->numFilters; j++) {
            int eventParmIndex;

            eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, j)->eventParmIndex;
            if (eventParmIndex == 0xFE) {
                if ((iSPCH_GetPhraseParmInfo(phrase, j)->matchParmIndex & 0x80) == 0) {
                    VOXBANKHDR *bank;
                    unsigned char *sampleParms;
                    int pickedIndex;
                    int sampleIndex;

                    bank = gVoxBanks[sentenceInfo->phraseInfo[i].bankIndex].voxHdr;
                    pickedIndex = sentenceInfo->phraseInfo[i].pickedIndex;

                    sampleIndex = sentenceInfo->validSamples[pickedIndex];
                    sampleParms = iSPCH_GetSampleParmAddr(bank, sampleIndex);
                    if (sampleParms != 0) {
                        globalMatchParmsArray[iSPCH_GetPhraseParmInfo(phrase, j)->matchParmIndex] =
                            sampleParms[j];
                    }
                }
            }
        }
    }
}

static int iSPCH_SentenceMakeChoice(EventSpec *eventSpec, SentencePickInfo *sentenceInfo,
                                    VoxSentence *sentence, int filterSetting, int filterLength) {
    int i;
    int numPhrases;
    int foundSentence;
    PhrasePickInfo *phraseInfo;
    VoxPhrase *phrase;

    if (filterLength > 0 || sentence->numMatchParms != 0) {
        iSPCH_RandomizeSentencePicks(sentence, sentenceInfo);

        foundSentence = iSPCH_ChooseSentenceIteratively(sentence, sentenceInfo, filterLength);
    } else {
        foundSentence = 1;

        numPhrases = VoxSentence_GetNumPhrases(sentence);

        for (i = 0; i < numPhrases; i++) {
            phraseInfo = &sentenceInfo->phraseInfo[i];

            if (phraseInfo->numPicks == 0) {
                foundSentence = 0;
                goto done;
            }

            if (!iSPCH_PickPhraseSample(phraseInfo)) {
                foundSentence = 0;
                goto done;
            }
        }
    }

    iSPCH_PostGlobalMatchParms(eventSpec, sentenceInfo, sentence);

done:
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

    if (gCallbacks.setRule == 0) {
        return;
    }

    if (!iSPCH_FindEventDatInfo(eventSpec, &datInfo)) {
        return;
    }

    channel = datInfo->channel;
    datID = datInfo->data->datID;

    numPhrases = VoxSentence_GetNumPhrases(sentence);

    for (i = 0; i < numPhrases; i++) {
        phrase = (VoxPhrase *)iSPCH_GetOffset8((unsigned char *)sentence,
                                               (unsigned char *)(sentence + 1), i);
        numParms = phrase->numFilters;

        for (j = 0; j < numParms; j++) {
            ruleIndex = iSPCH_GetPhraseParmInfo(phrase, j)->ruleIndex;
            if (ruleIndex == 0xFF) {
                continue;
            }

            ruleID = iSPCH_GetRuleID(event, ruleIndex);

            bankIndex = gEventChoice[channel].phrases[i].bankIndex;
            bank = gVoxBanks[bankIndex].voxHdr;

            sampleIndex = gEventChoice[channel].phrases[i].sampleIndex;
            sampleParms = iSPCH_GetSampleParmAddr(bank, sampleIndex);
            if (sampleParms == 0) {
                continue;
            }

            {
                int flag;

                flag = 1 << sampleParms[j];

                gCallbacks.setRule(eventSpec, ruleID, flag, datID);
            }
        }
    }
}

/* HISTORICO hasta r36f: 524 B al 98,3206%. spchpick tenia DOS sin cerrar
   (esta y iSPCH_ChooseSamples, 412 B al 95,5340), sin faltar datos.
   r38 cierra ambas: 7.836 B de codigo y DOL identico en enlace aislado.

   Las 132 instrucciones son las mismas. Las 8 filas que quedan estan todas en
   el bucle de bancos de aqui abajo, y son DOS cosas: el objetivo hace
   `lwzx r3` (leer bankHandle) ANTES de `add r30` (materializar phraseChoice),
   y usa r9/r11 donde nosotros r11/r10.

   CUATRO formas de orden medidas, ninguna gana:
     - sin la local `bankIndex` (indexar con phraseChoice->bankIndex): IDENTICA.
     - operandos de la comparacion al reves: 98,2443.
     - `bankHandle` a una local antes del if: 96,5649.
     - `bankHandle` leido antes de tomar el puntero: 95,1603.
   Ojo: este fichero es CRLF ENTERO; un parche con 
 casa 0 de N sin avisar.

   r36f, NUEVE formas mas y ninguna gana (todas a 524 B salvo donde se dice):
   barrera `"+r"` sobre phraseChoice delante del if 19 filas, sobre bankIndex 6
   (objeto identico), el eje "a quien" en los dos sentidos 21 y 19, la local de
   desplazamiento explicito del brief (`int off = i * sizeof(PhraseChoice)` y el
   cast) objeto IDENTICO, barrera de ranura delante del if 7 filas y detras de la
   toma del puntero 5 filas (97,9389 %, el mejor y aun asi no cierra), y
   `bankHandle` a local con y sin barrera 28 y 30 filas con 528 B.
   Ojo: `asm("" ::: "r0")` no compila aqui --GCC 2.9 en C++ lee `::` como el
   operador de ambito--; hay que escribir `__asm__("" : : : "r0")`. */
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

    if (!iSPCH_FindEventDatInfo(eventSpec, &datInfo)) {
        goto abort;
    }

    channel = datInfo->channel;
    datID = datInfo->data->datID;

    numPhrases = VoxSentence_GetNumPhrases(sentence);

    for (i = 0; i < numPhrases; i++) {
        phraseChoice = &gEventChoice[channel].phrases[i];

        bankIndex = phraseChoice->bankIndex;

        if (gVoxBanks[bankIndex].bankHandle != phraseChoice->bankHandle) {
            phraseChoice->bankIndex = iSPCH_FindBankIndexFromHandle(phraseChoice->bankHandle);

            if (phraseChoice->bankIndex & 0x8000) {
                goto abort;
            }
        }
    }

    for (i = 0; i < numPhrases; i++) {
        unsigned int sampleOffset;
        unsigned int sampleBytes;

        phraseChoice = &gEventChoice[channel].phrases[i];

        sampleIndex = phraseChoice->sampleIndex;

        bankHandle = phraseChoice->bankHandle;
        bank = gVoxBanks[phraseChoice->bankIndex].voxHdr;

        if ((bank->parmFlags & 0x80) && gClearCycle != 0) {
            iSPCH_ClearCycleBit(bank, sampleIndex);
        }

        if (iSPCH_GetSampleSizeData(bank, sampleIndex, &sampleOffset, &sampleBytes)) {
            unsigned int dataOffset;
            int bankBytes;

            bankBytes = bank->bankBlocks * ((bank->blockSize + 1) << 8);
            dataOffset = sampleOffset;
            if (phraseChoice->subBankIndex != -1) {
                dataOffset += phraseChoice->subBankIndex * bankBytes;
            }

            totalBytes += sampleBytes;

            sampleRequestData.bankNum = bankHandle;
            sampleRequestData.sampleOffset = dataOffset;
            sampleRequestData.numBytes = sampleBytes;
            sampleRequestData.eventSpec = *eventSpec;
            sampleRequestData.datID = datID;
            sampleRequestData.channel = channel;
            sampleRequestData.subID = bank->subID;

            if (i == 0) {
                sampleRequestData.interruptFlag = VoxEvent_GetInterruptFlag(event) >> 5;
            } else {
                sampleRequestData.interruptFlag = 0;
            }

            gCallbacks.request(&sampleRequestData);

            iSPCHBank_AddToQueue(bank, sampleIndex);
        }
    }

    sentenceLength = iSPCH_ConvertTime(totalBytes);

abort:
    return sentenceLength;
}

static void iSPCH_InitSentencePickInfo(SentencePickInfo *info) {
    int i;

    info->pickIndex = 0;
    info->numPhrases = 0;

    for (i = 0; i <= 11; i++) {
        info->phraseInfo[i].done = 0;
    }
}

void iSPCH_ClearSentenceChoiceChannel(unsigned int channel) {
    int j;

    if (gEventChoice[channel].memParms != 0) {
        iSPCH_MemFree(gEventChoice[channel].memParms);
        gEventChoice[channel].memParms = 0;
    }

    gEventChoice[channel].event = 0;
    gEventChoice[channel].sentence = 0;
    gEventChoice[channel].sentenceNum = 0;
    gEventChoice[channel].validChoice = 0;
    gEventChoice[channel].numPhrases = 0;
    gEventChoice[channel].pad1 = 0;
    gEventChoice[channel].memParms = 0;

    for (j = 0; j <= 11; j++) {
        gEventChoice[channel].phrases[j].bankHandle = -1;
        gEventChoice[channel].phrases[j].bankIndex = -1;
        gEventChoice[channel].phrases[j].subBankIndex = -1;
        gEventChoice[channel].phrases[j].sampleIndex = 0;
    }
}

void iSPCH_InitSentenceChoice(void) {
    int i;
    int j;

    for (i = 0; i <= 7; i++) {
        gEventChoice[i].event = 0;
        gEventChoice[i].sentence = 0;
        gEventChoice[i].sentenceNum = 0;
        gEventChoice[i].validChoice = 0;
        gEventChoice[i].numPhrases = 0;
        gEventChoice[i].pad1 = 0;
        gEventChoice[i].memParms = 0;

        for (j = 0; j <= 11; j++) {
            gEventChoice[i].phrases[j].bankHandle = -1;
            gEventChoice[i].phrases[j].bankIndex = -1;
            gEventChoice[i].phrases[j].subBankIndex = -1;
            gEventChoice[i].phrases[j].sampleIndex = 0;
        }
    }
}

static void iSPCH_SaveChosenSentence(SentencePickInfo *sentenceInfo, unsigned int inChannel,
                                     VoxEvent *event, VoxSentence *sentence, int sentenceNum,
                                     unsigned int *parms) {
    int i;
    int pickIndex;
    int numPhrases;

    if (gEventChoice[inChannel].memParms != 0) {
        iSPCH_MemFree(gEventChoice[inChannel].memParms);
    }

    gEventChoice[inChannel].memParms =
        (unsigned int *)iSPCH_MemAlloc((event->numParms + 1) * 4);

    if (gEventChoice[inChannel].memParms == 0) {
        goto abort;
    }

    gEventChoice[inChannel].event = event;
    gEventChoice[inChannel].sentence = sentence;
    gEventChoice[inChannel].sentenceNum = sentenceNum;

    for (i = 0; i <= event->numParms; i++) {
        gEventChoice[inChannel].memParms[i] = parms[i];
    }

    gEventChoice[inChannel].validChoice = 1;

    numPhrases = VoxSentence_GetNumPhrases(sentence);
    for (i = 0; i < numPhrases; i++) {
        gEventChoice[inChannel].phrases[i].bankIndex = sentenceInfo->phraseInfo[i].bankIndex;
        gEventChoice[inChannel].phrases[i].bankHandle = sentenceInfo->phraseInfo[i].bankHandle;
        gEventChoice[inChannel].phrases[i].subBankIndex = sentenceInfo->phraseInfo[i].subBankIndex;
        pickIndex = sentenceInfo->phraseInfo[i].pickedIndex;
        gEventChoice[inChannel].phrases[i].sampleIndex = sentenceInfo->validSamples[pickIndex];
    }

    gEventChoice[inChannel].numPhrases = numPhrases;

abort:
    return;
}

int iSPCH_OneChosen(unsigned int inChannel) {
    int validChoice;

    validChoice = gEventChoice[inChannel].validChoice;

    return validChoice;
}

int iSPCH_PlayChosen(unsigned int inChannel) {
    int sentenceLength;
    EventSpec eventSpec;
    int numPhrases;

    numPhrases = 0;

    if (inChannel > 7) {
        goto abort;
    }

    if (!iSPCH_OneChosen(inChannel)) {
        goto abort;
    }

    *(int *)&eventSpec = gEventChoice[inChannel].memParms[0];

    iSPCH_RuleSet(&eventSpec, gEventChoice[inChannel].event,
                  gEventChoice[inChannel].sentenceNum, gEventChoice[inChannel].memParms);

    iSPCH_ConstantRuleSet(&eventSpec, gEventChoice[inChannel].event,
                          gEventChoice[inChannel].sentence);

    sentenceLength = iSPCH_MakeSampleRequests(gEventChoice[inChannel].event,
                                              gEventChoice[inChannel].sentence, &eventSpec);

    numPhrases = VoxSentence_GetNumPhrases(gEventChoice[inChannel].sentence);

    if (eventSpec.eventID == gVoxInGame[inChannel].lastEventSpec.eventID) {
        gVoxInGame[inChannel].numEventTimes = gVoxInGame[inChannel].numEventTimes + 1;
    } else {
        *(int *)&gVoxInGame[inChannel].lastEventSpec = *(int *)&eventSpec;
        gVoxInGame[inChannel].numEventTimes = 1;
    }

abort:
    iSPCH_ClearSentenceChoiceChannel(inChannel);

    return numPhrases;
}

static int iSPCH_TestValidParm(unsigned int parmTypesMask, unsigned int parmValue, int parmIndex) {
    unsigned int testMask;
    int result;

    result = 0;

    testMask = parmTypesMask >> (parmIndex * 2);
    testMask = testMask & 3;

    switch (testMask) {
    case kExactMatch_GameValue:
        if (parmValue != 0) {
            result = 1;
        }
        break;

    case kExactMatch_BankID:
        if (parmValue != -1) {
            result = 1;
        }
        break;
    }

    return result;
}

static int iSPCH_SentenceIsContextMatch(VoxEvent *event, VoxSentence *sentence,
                                        unsigned int *inParms) {
    int i;
    int result;
    unsigned char *contextData;
    SRule rule;
    int contextParmIndex;
    int numContexts;
    int *sentenceContexts;

    result = 1;

    numContexts = VoxEvent_GetNumContexts(event);
    if (numContexts == 0) {
        goto abort;
    }

    sentenceContexts = VoxSentence_GetContextFlagsAddr(sentence);
    contextData = iSPCH_GetContextDataAddr(event);

    for (i = 0; i < numContexts; i++) {
        iSPCH_GetRule(rule, contextData, i);

        if (sentenceContexts[i] != 0 && (sentenceContexts[i] & inParms[rule.parmIndex]) == 0) {
            result = 0;
            goto abort;
        }
    }

abort:
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
    validParmsMask = 0;
    parmTypesMask = VoxEvent_GetParmTypesMask(event);
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

    numTries = 0;
    validSentence = 0;

    userSettingsMask = 0;

    validParmsMask = 0;

    filterLength = 0;

    *(int *)&eventSpec = parms[0];
    event = iSPCH_FindEvent(&eventSpec);
    if (event == 0) {
        goto abort;
    }

    if (!iSPCH_FindEventChannel(&eventSpec, &channel)) {
        goto abort;
    }

    if (!iSPCH_RepeatEvent(event, channel)) {
        goto abort;
    }

    filterSetting = gFilterSetting[channel];

    if (filterSetting == 1) {
        if (!VoxEvent_GetFilterLengthFlag(event)) {
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

    while (validSentence == 0 && numTries < numSentences) {
        getChoices = 1;

        sentenceNum = sentenceOrder[numTries];

        if (sentenceNum < 0 || sentenceNum >= numSentences) {
            goto abort;
        }

        sentence = (VoxSentence *)iSPCH_GetOffset16((unsigned char *)event,
                                                    (unsigned short *)(event + 1), sentenceNum);

        if (iSPCH_CheckFrequency(sentence)) {
            if (iSPCH_SentenceIsContextMatch(event, sentence, parms)) {
                if (iSPCH_CheckSentenceRules(event, sentenceNum, eventSettings, userSettingsMask)) {
                    if (tryExactMatch) {
                        sentenceParmsMask = VoxEvent_GetSentenceParmMask(event, sentenceNum);
                        getChoices = (sentenceParmsMask == validParmsMask);
                    }

                    if (getChoices) {
                        iSPCH_InitSentencePickInfo(&pickInfo);

                        if (iSPCH_SentenceGetChoices(&pickInfo, sentence, parms, filterSetting) >
                            0) {
                            validSentence = iSPCH_SentenceMakeChoice(&eventSpec, &pickInfo,
                                                                     sentence, filterSetting,
                                                                     filterLength);
                            if (validSentence) {
                                iSPCH_SaveChosenSentence(&pickInfo, channel, event, sentence,
                                                         sentenceNum, parms);
                            }
                        }
                    }
                }
            }
        }

        numTries++;
    }

abort:
    return validSentence;
}

int iSPCH_ChooseSingleSentence(int choice) {
    unsigned int *parms;
    int validSentence;
    int count;
    int retry;

    validSentence = 0;

    parms = gVoxEvents.events[choice].memParms;
    if (parms == 0) {
        goto abort;
    }

    validSentence = iSPCH_ChooseSentence(parms);
    if (validSentence != 0) {
        goto done;
    }

    if (gCallbacks.reparm == 0) {
        goto done;
    }

    count = 0;
    do {
        retry = gCallbacks.reparm(count, parms);
        if (retry >= 0) {
            validSentence = iSPCH_ChooseSentence(parms);
        }

        count++;
    } while (validSentence == 0 && retry > 0);

done:
    if (validSentence < 0) {
        validSentence = 0;
    }

abort:
    return validSentence;
}

void SPCH_SetPreLoadTicks(int ticks) {
    gPreLoadTicks = ticks;
}
