#include "LobbyRanks.hpp"

char *bStrChr(const char *string, int character);

extern "C" {
DispListRef *LobbyApiListAlloc(LobbyApiRefT *lobbyRef, LobbyRanks::RankListMapping mapping, LobbyApiCallbackT *callback, void *context);
void DispListSort(DispListRef *list, int sortcon, int sortdir, int (*sortfn)(void *, int, void *, void *));
void *DispListIndex(DispListRef *list, int index);
int DispListCount(DispListRef *list);
void LobbyApiListFree(LobbyApiRefT *lobbyRef, int selector, DispListRef *list);
char *TagFieldFind(const char *record, const char *name);
int TagFieldGetNumber(const char *field, int defaultValue);
int TagFieldGetString(const char *field, char *buffer, int bufferSize, const char *defaultValue);
}

unsigned int LobbyRanks::tierPoints[3][10];

LobbyRanks::LobbyRanks() {
    bMemSet(rankDispLists, 0, sizeof(rankDispLists));
    bMemSet(gotRankList, 0, sizeof(gotRankList));
    bMemSet(snapsTotal, 0, sizeof(snapsTotal));
    bMemSet(tierPoints, 0, sizeof(tierPoints));
}

LobbyRanks &LobbyRanks::Instance() {
    static LobbyRanks theLobbyRanks;
    return theLobbyRanks;
}

int32 LobbyRanks::GetRanks(RankListE rankList, CommandCBFunc func, void *context) {
    char buf[64];
    int32 commandID;
    int32 rc = -1;

    lobbyMutex.Lock(nullptr);
    if (rankList > RL_BOGUS && rankList < NUM_RANK_LISTS) {
        commandID = LobbyCore::Instance().FindCommandID('snap', nullptr, nullptr, nullptr, nullptr);
        if (commandID == -1) {
            int top100Index = RankList2DispListIndex(rankList, RLT_TOP100);
            if (!rankDispLists[top100Index]) {
                rankDispLists[top100Index] = LobbyApiListAlloc(LobbyCore::Instance().pLobbyRef,
                                                               RankList2SnapIndex(rankList, RLT_TOP100), SnapsDispListCB, this);
                if (!rankDispLists[top100Index]) {
                    Reset();
                    lobbyMutex.Unlock(nullptr);
                    return -1;
                }
                DispListSort(rankDispLists[top100Index], 0, 0, RankSort);
            }

            int personalIndex = RankList2DispListIndex(rankList, RLT_PERSONAL);
            if (!rankDispLists[personalIndex]) {
                rankDispLists[personalIndex] = LobbyApiListAlloc(LobbyCore::Instance().pLobbyRef,
                                                                 RankList2SnapIndex(rankList, RLT_PERSONAL), SnapsDispListCB, this);
                if (!rankDispLists[personalIndex]) {
                    Reset();
                    lobbyMutex.Unlock(nullptr);
                    return -1;
                }
                DispListSort(rankDispLists[personalIndex], 0, 0, RankSort);
            }

            if (gotRankList[top100Index] == true && gotRankList[personalIndex] == true) {
                lobbyMutex.Unlock(nullptr);
                return 0;
            }

            buf[0] = '\0';
            LobbyCore::Instance().StartGroup();
            TagFieldPrintf(buf, sizeof(buf), "INDEX=%d CHAN=%d RANGE=1 FIND=$", rankList,
                           RankList2SnapIndex(rankList, RLT_PERSONAL));
            commandID = LobbyCore::Instance().QueueCommand('snap', buf, SnapCB, this, nullptr, nullptr, false);
            if (commandID >= 0) {
                buf[0] = '\0';
                TagFieldPrintf(buf, sizeof(buf), "INDEX=%d CHAN=%d START=0 RANGE=100", rankList,
                               RankList2SnapIndex(rankList, RLT_TOP100));
                rc = LobbyCore::Instance().QueueCommand('snap', buf, SnapCB, this, func, context, false);
                if (rc < 0) {
                    LobbyCore::Instance().AbortGroup(commandID);
                }
                LobbyCore::Instance().EndGroup();
                lobbyMutex.Unlock(nullptr);
                return rc;
            }
            LobbyCore::Instance().EndGroup();
        }
    }
    lobbyMutex.Unlock(nullptr);
    return -1;
}

inline int32 LobbyRanks::GetAllMonthRanks(CommandCBFunc func, void *context) {
    int32 commandID = -1;
    for (RankListE rl = RL_FIRST_MONTH_LIST; rl <= RL_LAST_MONTH_LIST; rl = static_cast<RankListE>(rl + 1)) {
        commandID = GetRanks(rl, rl == RL_LAST_MONTH_LIST ? func : nullptr, rl == RL_LAST_MONTH_LIST ? context : nullptr);
        if (commandID < 0) {
            return commandID;
        }
    }
    return commandID;
}

inline int32 LobbyRanks::GetMonthRanks(OnlineRaceModeE mode, RankListTypeE type, CommandCBFunc func, void *context) {
    int32 rc = -1;
    for (RankListSubcategoryE category = RANKLIST_BY_RATING; category <= RANKLIST_BY_WIN_PERCENTAGE;
         category = static_cast<RankListSubcategoryE>(category + 1)) {
        RankListE rl = GetRankListFromMode(mode, category, 2);
        rc = GetRanks(rl, category == RANKLIST_BY_WIN_PERCENTAGE ? func : nullptr,
                      category == RANKLIST_BY_WIN_PERCENTAGE ? context : nullptr);
        if (rc < 0) {
            return rc;
        }
    }
    return rc;
}

inline int32 LobbyRanks::GetAllWeeksRanks(CommandCBFunc func, void *context) {
    int32 commandID = -1;
    for (RankListE rl = RL_FIRST_WEEK_LIST; rl <= RL_LAST_WEEK_LIST; rl = static_cast<RankListE>(rl + 1)) {
        commandID = GetRanks(rl, rl == RL_LAST_WEEK_LIST ? func : nullptr, rl == RL_LAST_WEEK_LIST ? context : nullptr);
        if (commandID < 0) {
            return commandID;
        }
    }
    return commandID;
}

inline int32 LobbyRanks::GetWeekRanks(OnlineRaceModeE mode, RankListTypeE type, CommandCBFunc func, void *context) {
    int32 rc = -1;
    for (RankListSubcategoryE category = RANKLIST_BY_RATING; category <= RANKLIST_BY_WIN_PERCENTAGE;
         category = static_cast<RankListSubcategoryE>(category + 1)) {
        RankListE rl = GetRankListFromMode(mode, category, 1);
        rc = GetRanks(rl, category == RANKLIST_BY_WIN_PERCENTAGE ? func : nullptr,
                      category == RANKLIST_BY_WIN_PERCENTAGE ? context : nullptr);
        if (rc < 0) {
            return rc;
        }
    }
    return rc;
}

LobbyApiRankT *LobbyRanks::GetRankData(RankListE rankList, RankListTypeE type, int index) {
    LobbyApiRankT *rank = static_cast<LobbyApiRankT *>(DispListIndex(rankDispLists[RankList2DispListIndex(rankList, type)], index));
    if (type == RLT_PERSONAL) {
        if (!rank || bStrCmp(rank->name, FEDatabase->OnlineSettings.GetLobbyPersona()) != 0) {
            rank = nullptr;
        }
    }
    return rank;
}

inline void LobbyRanks::AbortCommand(int32 commandID) { LobbyCore::Instance().AbortCommand(commandID); }

void LobbyRanks::ClearCache() { bMemSet(gotRankList, 0, sizeof(gotRankList)); }

int LobbyRanks::GetRankListSize(RankListE rankList, RankListTypeE type) {
    DispListRef *list = rankDispLists[RankList2DispListIndex(rankList, type)];
    if (list) {
        return DispListCount(list);
    }
    return 0;
}

RankListE LobbyRanks::GetRankListFromMode(OnlineRaceModeE raceMode, RankListSubcategoryE category, int weekOffset) {
    if (weekOffset == 1) {
        if (raceMode == OL_RACE_MODE_CIRCUIT) {
            return static_cast<RankListE>(category + RL_CIRCUIT_WEEK_BEST_BY_RATING);
        }
        if (raceMode == OL_RACE_MODE_SPRINT) {
            return static_cast<RankListE>(category + RL_SPRINT_WEEK_BEST_BY_RATING);
        }
        if (raceMode == OL_RACE_MODE_DRAG) {
            return static_cast<RankListE>(category + RL_DRAG_WEEK_BEST_BY_RATING);
        }
    }
    if (weekOffset == 2) {
        if (raceMode == OL_RACE_MODE_CIRCUIT) {
            return static_cast<RankListE>(category + RL_CIRCUIT_MONTH_BEST_BY_RATING);
        }
        if (raceMode == OL_RACE_MODE_SPRINT) {
            return static_cast<RankListE>(category + RL_SPRINT_MONTH_BEST_BY_RATING);
        }
        if (raceMode == OL_RACE_MODE_DRAG) {
            return static_cast<RankListE>(category + RL_DRAG_MONTH_BEST_BY_RATING);
        }
    }
    if (raceMode == OL_RACE_MODE_CIRCUIT) {
        return static_cast<RankListE>(category + RL_CIRCUIT_BY_RATING);
    }
    if (raceMode == OL_RACE_MODE_SPRINT) {
        return static_cast<RankListE>(category + RL_SPRINT_BY_RATING);
    }
    if (raceMode == OL_RACE_MODE_DRAG) {
        return static_cast<RankListE>(category + RL_DRAG_BY_RATING);
    }
    return RL_INVALID;
}

int32 LobbyRanks::Init() { return 0; }

void LobbyRanks::Reset() {
    if (LobbyCore::Instance().pLobbyRef) {
        for (int i = 0; i < 18; i++) {
            if (rankDispLists[i]) {
                LobbyApiListFree(LobbyCore::Instance().pLobbyRef, i + 5, rankDispLists[i]);
            }
        }
    }
    bMemSet(rankDispLists, 0, sizeof(rankDispLists));
    bMemSet(gotRankList, 0, sizeof(gotRankList));
    bMemSet(snapsTotal, 0, sizeof(snapsTotal));
    bMemSet(tierPoints, 0, sizeof(tierPoints));
}

void LobbyRanks::SnapsDispListCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyRanks *lobbyRanks = static_cast<LobbyRanks *>(pData);
    int index = RankListMapping2DispListIndex(static_cast<RankListMapping>(pMsg->kind));
    if (lobbyRanks->snapsTotal[index] == static_cast<unsigned int>(DispListCount(lobbyRanks->rankDispLists[index]))) {
        lobbyRanks->gotRankList[index] = true;
        LobbyCore &lobbyCore = LobbyCore::Instance();
        if (lobbyCore.currentCommand && lobbyCore.currentCommand->kind == 'snap') {
            lobbyCore.FinishCommand(pMsg, true);
        }
    }
}

void LobbyRanks::SnapCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyRanks *lobbyRanks = static_cast<LobbyRanks *>(pData);
    if (pMsg->code == 0) {
        RankListMapping mapping = static_cast<RankListMapping>(TagFieldGetNumber(TagFieldFind(pMsg->pData, "CHAN"), 0));
        TagFieldGetNumber(TagFieldFind(pMsg->pData, "START"), 0);
        int count = TagFieldGetNumber(TagFieldFind(pMsg->pData, "RANGE"), 0);
        int index = RankListMapping2DispListIndex(mapping);
        if (count == 0) {
            lobbyRanks->gotRankList[index] = true;
            LobbyCore::Instance().FinishCommand(pMsg, true);
        }
        lobbyRanks->snapsTotal[index] = count;
    } else {
        LobbyCore::Instance().FinishCommand(pMsg, true);
    }
}

int LobbyRanks::RankSort(void *sortref, int sortcon, void *recptr1, void *recptr2) {
    return static_cast<LobbyApiRankT *>(recptr2)->pts - static_cast<LobbyApiRankT *>(recptr1)->pts;
}

LobbyRanks::RankListMapping LobbyRanks::RankList2SnapIndex(RankListE rankList, RankListTypeE type) {
    if (type == RLT_TOP100) {
        if (rankList >= RL_CIRCUIT_BY_RATING && rankList <= RL_CIRCUIT_BY_WIN_PERCENTAGE) return static_cast<RankListMapping>(5);
        if (rankList >= RL_SPRINT_BY_RATING && rankList <= RL_SPRINT_BY_WIN_PERCENTAGE) return static_cast<RankListMapping>(6);
        if (rankList >= RL_DRAG_BY_RATING && rankList <= RL_DRAG_BY_FASTEST_FINISH) return static_cast<RankListMapping>(7);
        if (rankList >= RL_CIRCUIT_WEEK_BEST_BY_RATING && rankList <= RL_CIRCUIT_WEEK_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(11);
        if (rankList >= RL_SPRINT_WEEK_BEST_BY_RATING && rankList <= RL_SPRINT_WEEK_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(12);
        if (rankList >= RL_DRAG_WEEK_BEST_BY_RATING && rankList <= RL_DRAG_WEEK_BY_FASTEST_FINISH)
            return static_cast<RankListMapping>(13);
        if (rankList >= RL_CIRCUIT_MONTH_BEST_BY_RATING && rankList <= RL_CIRCUIT_MONTH_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(17);
        if (rankList >= RL_SPRINT_MONTH_BEST_BY_RATING && rankList <= RL_SPRINT_MONTH_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(18);
        if (rankList >= RL_DRAG_MONTH_BEST_BY_RATING && rankList <= RL_DRAG_MONTH_BY_FASTEST_FINISH)
            return static_cast<RankListMapping>(19);
    } else if (type == RLT_PERSONAL) {
        if (rankList >= RL_CIRCUIT_BY_RATING && rankList <= RL_CIRCUIT_BY_WIN_PERCENTAGE) return static_cast<RankListMapping>(8);
        if (rankList >= RL_SPRINT_BY_RATING && rankList <= RL_SPRINT_BY_WIN_PERCENTAGE) return static_cast<RankListMapping>(9);
        if (rankList >= RL_DRAG_BY_RATING && rankList <= RL_DRAG_BY_FASTEST_FINISH) return static_cast<RankListMapping>(10);
        if (rankList >= RL_CIRCUIT_WEEK_BEST_BY_RATING && rankList <= RL_CIRCUIT_WEEK_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(14);
        if (rankList >= RL_SPRINT_WEEK_BEST_BY_RATING && rankList <= RL_SPRINT_WEEK_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(15);
        if (rankList >= RL_DRAG_WEEK_BEST_BY_RATING && rankList <= RL_DRAG_WEEK_BY_FASTEST_FINISH)
            return static_cast<RankListMapping>(16);
        if (rankList >= RL_CIRCUIT_MONTH_BEST_BY_RATING && rankList <= RL_CIRCUIT_MONTH_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(20);
        if (rankList >= RL_SPRINT_MONTH_BEST_BY_RATING && rankList <= RL_SPRINT_MONTH_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(21);
        if (rankList >= RL_DRAG_MONTH_BEST_BY_RATING && rankList <= RL_DRAG_MONTH_BY_FASTEST_FINISH)
            return static_cast<RankListMapping>(22);
    } else if (type == RLT_CURRENT_WEEK) {
        if (rankList >= RL_CIRCUIT_WEEK_BEST_BY_RATING && rankList <= RL_CIRCUIT_WEEK_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(11);
        if (rankList >= RL_SPRINT_WEEK_BEST_BY_RATING && rankList <= RL_SPRINT_WEEK_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(12);
        if (rankList >= RL_DRAG_WEEK_BEST_BY_RATING && rankList <= RL_DRAG_WEEK_BY_FASTEST_FINISH)
            return static_cast<RankListMapping>(13);
    } else if (type == RLT_CURRENT_MONTH) {
        if (rankList >= RL_CIRCUIT_MONTH_BEST_BY_RATING && rankList <= RL_CIRCUIT_MONTH_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(17);
        if (rankList >= RL_SPRINT_MONTH_BEST_BY_RATING && rankList <= RL_SPRINT_MONTH_BEST_BY_WIN_PERCENTAGE)
            return static_cast<RankListMapping>(18);
        if (rankList >= RL_DRAG_MONTH_BEST_BY_RATING && rankList <= RL_DRAG_MONTH_BY_FASTEST_FINISH)
            return static_cast<RankListMapping>(19);
    }
    return RLM_INVALID;
}

int LobbyRanks::RankList2DispListIndex(RankListE rankList, RankListTypeE type) {
    RankListMapping mapping = RankList2SnapIndex(rankList, type);
    return RankListMapping2DispListIndex(mapping);
}

int LobbyRanks::RankListMapping2DispListIndex(RankListMapping mapping) { return mapping - 5; }

inline RankListTypeE LobbyRanks::RankListMapping2RankListType(RankListMapping mapping) {
    if (mapping >= static_cast<RankListMapping>(5) && mapping <= static_cast<RankListMapping>(7)) return RLT_TOP100;
    if (mapping >= static_cast<RankListMapping>(8) && mapping <= static_cast<RankListMapping>(10)) return RLT_PERSONAL;
    if (mapping >= static_cast<RankListMapping>(11) && mapping <= static_cast<RankListMapping>(13)) return RLT_CURRENT_WEEK;
    if (mapping >= static_cast<RankListMapping>(14) && mapping <= static_cast<RankListMapping>(16)) return RLT_PERSONAL_WEEK;
    if (mapping >= static_cast<RankListMapping>(17) && mapping <= static_cast<RankListMapping>(19)) return RLT_CURRENT_MONTH;
    if (mapping >= static_cast<RankListMapping>(20) && mapping <= static_cast<RankListMapping>(22)) return RLT_PERSONAL_MONTH;
    return RLT_INVALID;
}

inline int LobbyRanks::InitTierArray() {
    char *config = LobbyCore::Instance().GetServerConfig();
    if (!config) {
        return -1;
    }

    for (OnlineRaceModeE mode = OL_RACE_MODE_CIRCUIT; mode < NUM_OL_RANKED_RACE_MODES;
         mode = static_cast<OnlineRaceModeE>(mode + 1)) {
        char strMode[32];
        char buf[96];
        if (mode == OL_RACE_MODE_CIRCUIT) bStrCpy(strMode, "CIRCUIT_TIER_POINTS");
        else if (mode == OL_RACE_MODE_SPRINT) bStrCpy(strMode, "SPRINT_TIER_POINTS");
        else bStrCpy(strMode, "DRAG_TIER_POINTS");

        TagFieldGetString(TagFieldFind(config, strMode), buf, sizeof(buf), "");
        char *start = buf;
        char *end;
        for (int i = 0; i < 10; i++) {
            end = bStrChr(start, ',');
            if (end) {
                *end = '\0';
            }
            tierPoints[mode][i] = bStrToLong(start);
            if (!end) {
                break;
            }
            start = end + 1;
        }
    }
    return 0;
}

DispListRef *LobbyRanks::GetDisplayListReference(RankListE index) {
    return rankDispLists[RankList2DispListIndex(index, RLT_TOP100)];
}
