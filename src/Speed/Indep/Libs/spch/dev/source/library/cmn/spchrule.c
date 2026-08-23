
#include "spch/spch.h"

void iSPCH_GetDatID(EventSpec *eventSpec, unsigned int *datID);
static int iSPCH_SentenceUsesParm(VoxSentence *sentence, int parmIndex);

int iSPCH_GetRuleID(VoxEvent *event, int ruleIndex) {
    unsigned char *ruleData;
    int ruleID;
    SRule rule;

    ruleData = iSPCH_GetRuleDataAddr(event);
    rule.ruleID = ruleData[ruleIndex * 3];
    rule.parmIndex = ruleData[ruleIndex * 3 + 1];
    rule.parmType = static_cast<ParmType>(ruleData[ruleIndex * 3 + 2]);
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

    if (gCallbacks.setRule != 0) {
        iSPCH_GetDatID(eventSpec, &datID);
        i = 0;
        numRules = VoxEvent_GetNumRules(event);
        ruleData = iSPCH_GetRuleDataAddr(event);
        sentence = reinterpret_cast<VoxSentence *>(iSPCH_GetOffset16(reinterpret_cast<unsigned char *>(event), reinterpret_cast<unsigned short *>(event + 1), sentenceNum));
        if (i < numRules) {
            do {
                rule.ruleID = ruleData[i * 3];
                rule.parmIndex = ruleData[i * 3 + 1];
                rule.parmType = static_cast<ParmType>(ruleData[i * 3 + 2]);
                ruleID = rule.ruleID;
                parmIndex = rule.parmIndex;
                if ((rule.parmType < kParmType_Constant && rule.parmType == kParmType_Variable) ||
                    rule.parmType == kParmType_BankID) {
                    if (iSPCH_SentenceUsesParm(sentence, parmIndex) != 0) {
                        parmValue = parms[parmIndex];
                        gCallbacks.setRule(eventSpec, ruleID, parmValue, datID);
                    }
                }
                i++;
            } while (i < numRules);
        }
    }
}

unsigned int iSPCH_GetRuleSettings(EventSpec *eventSpec, VoxEvent *event, unsigned int *parms, unsigned int *userSettingsMask) {
    unsigned char *ruleData;
    unsigned int thisFlag;
    unsigned int flags;
    unsigned int uflags;
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
    flags = 0;
    uflags = 0;
    i = 0;
    numRules = VoxEvent_GetNumRules(event);
    ruleData = iSPCH_GetRuleDataAddr(event);
    if (i < numRules) {
        do {
            thisFlag = 0;
            rule.ruleID = ruleData[i * 3];
            rule.parmIndex = ruleData[i * 3 + 1];
            rule.parmType = static_cast<ParmType>(ruleData[i * 3 + 2]);
            parmIndex = rule.parmIndex;
            if (parmIndex != 0) {
                parmValue = parms[parmIndex];
                bit = 1 << (31 - i);
                if (gCallbacks.testRule != 0) {
                    val = gCallbacks.testRule(eventSpec, rule.ruleID, parmValue, datID);
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
            i++;
        } while (i < numRules);
    }
    i = 0;
    if (i < numRules) {
        do {
            thisFlag = 0;
            rule.ruleID = ruleData[i * 3];
            rule.parmIndex = ruleData[i * 3 + 1];
            rule.parmType = static_cast<ParmType>(ruleData[i * 3 + 2]);
            parmIndex = rule.parmIndex;
            if (parmIndex == 0) {
                bit = 1 << (31 - i);
                if (gCallbacks.testRule != 0) {
                    val = gCallbacks.testRule(eventSpec, rule.ruleID, 0, datID);
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
            i++;
        } while (i < numRules);
    }
    *userSettingsMask = uflags;
    return flags;
}

static int iSPCH_SentenceUsesParm(VoxSentence *sentence, int parmIndex) {
    int i;
    int j;
    int numPhrases;
    int result;
    int bankSelectIndex;
    VoxPhrase *phrase;
    int eventParmIndex;
    int numParms;

    i = 0;
    result = 0;
    numPhrases = VoxSentence_GetNumPhrases(sentence);
    while (i < numPhrases) {
        phrase = reinterpret_cast<VoxPhrase *>(iSPCH_GetOffset8(reinterpret_cast<unsigned char *>(sentence), reinterpret_cast<unsigned char *>(sentence) + 8, i));
        bankSelectIndex = phrase->bankIDIndex;
        if (bankSelectIndex == parmIndex) {
            return 1;
        }
        numParms = i + 1;
        j = 0;
        while (j < phrase->numFilters) {
            eventParmIndex = iSPCH_GetPhraseParmInfo(phrase, j)->eventParmIndex;
            if (eventParmIndex == parmIndex) {
                result = 1;
                break;
            }
            j++;
        }
        i = numParms;
    }
    return result;
}

static void iSPCH_GetSentenceRuleSettings(VoxEvent *event, int sentenceNum, unsigned int *ioSettings, unsigned int *ioFlags) {
    unsigned char *flagsAddr;
    unsigned char *settingsAddr;
    int i;
    int bytesPerRule;
    int settingsSize;

    *ioSettings = 0;
    *ioFlags = 0;
    bytesPerRule = (VoxEvent_GetNumRules(event) + 7) >> 3;
    settingsAddr = iSPCH_GetSentenceRulesAddr(event) + sentenceNum * bytesPerRule;
    settingsSize = bytesPerRule * event->numSentences;
    flagsAddr = settingsAddr + settingsSize;
    if (bytesPerRule > 4) {
        goto abort;
    }
    i = 0;
    if (bytesPerRule == 0) {
        goto abort;
    }
    do {
        *ioSettings += settingsAddr[i] << ((3 - i) * 8);
        *ioFlags += flagsAddr[i] << ((3 - i) * 8);
        i++;
    } while (i < bytesPerRule);
abort:
    ;
}

int iSPCH_CheckSentenceRules(VoxEvent *event, int sentenceNum, unsigned int eventSettings, unsigned int userSettingsMask) {
    int result;
    unsigned int sentenceFlags;
    unsigned int sentenceSettings;
    unsigned int eventSettingsToUse;

    eventSettingsToUse = eventSettings;
    iSPCH_GetSentenceRuleSettings(event, sentenceNum, &sentenceSettings, &sentenceFlags);
    sentenceFlags &= ~userSettingsMask;
    sentenceSettings &= sentenceFlags;
    eventSettingsToUse = sentenceFlags;
    eventSettingsToUse = eventSettings & eventSettingsToUse;
    result = eventSettingsToUse == sentenceSettings;
    return result;
}
