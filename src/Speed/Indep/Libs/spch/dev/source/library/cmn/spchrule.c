#include "./spchi.h"

// total size: 0x14
struct SPCH_Callbacks {
    int (*request)(SPCHType_SampleRequestData *);       // offset 0x0, size 0x4
    int (*testRule)(EventSpec *, int, int, int);        // offset 0x4, size 0x4
    void (*setRule)(EventSpec *, int, int, int);        // offset 0x8, size 0x4
    SPCHType_EventRuleResult (*eventRule)(EventSpec *); // offset 0xC, size 0x4
    int (*reparm)(int, unsigned int *);                 // offset 0x10, size 0x4
};

extern SPCH_Callbacks gCallbacks;

int iSPCH_GetDatID(EventSpec *eventSpec, unsigned int *datID);

static int iSPCH_SentenceUsesParm(VoxSentence *sentence, int parmIndex) {
    int result = 0;
    int i;
    int j;
    int numPhrases;
    int bankSelectIndex;
    VoxPhrase *phrase;
    int eventParmIndex;
    int numParms;

    numPhrases = VoxSentence_GetNumPhrases(sentence);

    for (i = 0; i < numPhrases; i++) {
        phrase = (VoxPhrase *)iSPCH_GetOffset8((unsigned char *)sentence,
                                               (unsigned char *)(sentence + 1), i);
        bankSelectIndex = phrase->bankIDIndex;
        if (bankSelectIndex == parmIndex) {
            result = 1;
            break;
        }

        numParms = phrase->numFilters;
        for (j = 0; j < numParms; j++) {
            eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, j)->eventParmIndex;
            if (eventParmIndex == parmIndex) {
                result = 1;
                break;
            }
        }
    }

    return result;
}

int iSPCH_GetRuleID(VoxEvent *event, int ruleIndex) {
    unsigned char *ruleData;
    int ruleID;
    SRule rule;

    ruleData = iSPCH_GetRuleDataAddr(event);

    iSPCH_GetRule(rule, ruleData, ruleIndex);

    ruleID = rule.ruleID;
    return ruleID;
}

void iSPCH_RuleSet(EventSpec *eventSpec, VoxEvent *event, int sentenceNum, unsigned int *parms) {
    unsigned char *ruleData;
    unsigned int parmValue;
    int ruleID;
    int parmIndex;
    int i;
    int numRules;
    VoxSentence *sentence;
    SRule rule;
    unsigned int datID;

    if (gCallbacks.setRule == 0) {
        return;
    }

    iSPCH_GetDatID(eventSpec, &datID);
    sentence = (VoxSentence *)iSPCH_GetOffset16((unsigned char *)event,
                                                (unsigned short *)(event + 1), sentenceNum);
    ruleData = iSPCH_GetRuleDataAddr(event);
    numRules = VoxEvent_GetNumRules(event);

    for (i = 0; i < numRules; i++) {
        iSPCH_GetRule(rule, ruleData, i);

        ruleID = rule.ruleID;
        parmIndex = rule.parmIndex;

        switch (rule.parmType) {
        case kParmType_Variable:
        case kParmType_BankID:
            if (iSPCH_SentenceUsesParm(sentence, parmIndex)) {
                parmValue = parms[parmIndex];
                gCallbacks.setRule(eventSpec, ruleID, parmValue, datID);
            }
            break;
        case kParmType_Constant:
        case kParmType_User:
            break;
        }
    }
}

unsigned int iSPCH_GetRuleSettings(EventSpec *eventSpec, VoxEvent *event, unsigned int *parms,
                                   unsigned int *userSettingsMask) {
    unsigned char *ruleData;
    unsigned int thisFlag;
    unsigned int flags = 0, uflags = 0;
    unsigned int bit;
    unsigned int parmValue;
    int ruleID;
    int parmIndex;
    int i;
    int numRules;
    int val;
    SRule rule;
    unsigned int datID;

    iSPCH_GetDatID(eventSpec, &datID);

    numRules = VoxEvent_GetNumRules(event);
    ruleData = iSPCH_GetRuleDataAddr(event);

    for (i = 0; i < numRules; i++) {
        thisFlag = 0;

        iSPCH_GetRule(rule, ruleData, i);

        parmIndex = rule.parmIndex;
        if (parmIndex != 0) {
            parmValue = parms[parmIndex];

            bit = 1 << (31 - i);

            ruleID = rule.ruleID;
            if (gCallbacks.testRule != 0) {
                val = gCallbacks.testRule(eventSpec, ruleID, parmValue, datID);
            } else {
                val = -1;
            }

            if (val == 0) {
                thisFlag = 0;
            } else if (val > 0) {
                thisFlag = bit;
            } else {
                uflags |= bit;
            }

            flags |= thisFlag;
        }
    }

    for (i = 0; i < numRules; i++) {
        thisFlag = 0;

        iSPCH_GetRule(rule, ruleData, i);

        parmIndex = rule.parmIndex;
        if (parmIndex == 0) {
            bit = 1 << (31 - i);

            ruleID = rule.ruleID;
            if (gCallbacks.testRule != 0) {
                val = gCallbacks.testRule(eventSpec, ruleID, 0, datID);
            } else {
                val = -1;
            }

            if (val == 0) {
                thisFlag = 0;
            } else if (val > 0) {
                thisFlag = bit;
            } else {
                uflags |= bit;
            }

            flags |= thisFlag;
        }
    }

    *userSettingsMask = uflags;
    return flags;
}

static void iSPCH_GetSentenceRuleSettings(VoxEvent *event, int sentenceNum,
                                          unsigned int *ioSettings, unsigned int *ioFlags) {
    unsigned char *flagsAddr;
    unsigned char *settingsAddr;
    int i;
    int bytesPerRule;
    int settingsSize;

    *ioSettings = 0;
    *ioFlags = 0;

    bytesPerRule = (VoxEvent_GetNumRules(event) + 7) / 8;

    settingsSize = bytesPerRule * event->numSentences;

    settingsAddr = iSPCH_GetSentenceRulesAddr(event);
    settingsAddr += sentenceNum * bytesPerRule;
    flagsAddr = settingsAddr + settingsSize;

    if (bytesPerRule > 4) {
        goto abort;
    }

    for (i = 0; i < bytesPerRule; i++) {
        *ioSettings += settingsAddr[i] << ((3 - i) * 8);
        *ioFlags += flagsAddr[i] << ((3 - i) * 8);
    }

abort:
    return;
}

int iSPCH_CheckSentenceRules(VoxEvent *event, int sentenceNum, unsigned int eventSettings,
                             unsigned int userSettingsMask) {
    int result;
    unsigned int sentenceFlags;
    unsigned int sentenceSettings;
    unsigned int eventSettingsToUse;

    iSPCH_GetSentenceRuleSettings(event, sentenceNum, &sentenceSettings, &sentenceFlags);

    sentenceFlags = sentenceFlags & ~userSettingsMask;
    sentenceSettings = sentenceSettings & sentenceFlags;
    eventSettingsToUse = sentenceFlags & eventSettings;

    result = (eventSettingsToUse == sentenceSettings);
    return result;
}
