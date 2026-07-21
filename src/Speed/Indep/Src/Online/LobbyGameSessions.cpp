#include "LobbyGameSessions.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"

extern "C" {
void DispListFilt(DispListRef *list, int filtcon, int filtmask, int (*filtfn)(void *, int, void *));
int DispListShown(DispListRef *list);
void *DispListGet(DispListRef *list, int index);
int LobbyApiListFindByName(LobbyApiRefT *lobbyRef, int selector, const char *name);
}

bool LobbyGameSessions::mHurryTimerStarted = false;
bool LobbyGameSessions::mHurryTimerStopped = false;
LobbyGameSessions *TheLobbyGameSessions;
static int uniqueID;

FilterGameSessionParamsT::FilterGameSessionParamsT(uint32 count, GRace::Type gameMode, YesNoAny rankedGames,
                                                   YesNoAny collisionDetection, YesNoAny performanceMatching,
                                                   eOnlineDisconnectPerc disconnectPerc)
    : count(count),                               //
      gameMode(gameMode),                         //
      rankedGames(rankedGames),                   //
      collisionDetection(collisionDetection),     //
      performanceMatching(performanceMatching),   //
      disconnectPerc(disconnectPerc) {
    sessionNameSubstr[0] = '\0';
}

FilterGameSessionParamsT::FilterGameSessionParamsT(uint32 count, const char *sessionNameSubstring)
    : count(count),                                   //
      gameMode(GRace::kRaceType_NumTypes),            //
      rankedGames(ANY),                               //
      collisionDetection(ANY),                        //
      performanceMatching(ANY),                      //
      disconnectPerc(OLS_DISCONNECT_PERC_ANY) {
    bStrNCpy(sessionNameSubstr, sessionNameSubstring, 16);
}

LobbyGameSessions::LobbyGameSessions()
    : sessionList(nullptr),                 //
      sessionMembers(nullptr),              //
      sessionUpdateCB(nullptr),             //
      updateContext(nullptr),               //
      currentSortParams(0x80000000),        //
      lastSearchCount(0),                   //
      hostStartSessionTimer(0.0f),          //
      hostHurryTimer(0.0f),                 //
      hostInactiveTimer(0.0f) {
    bMemSet(&myCurrentSession, 0, sizeof(myCurrentSession));
    myCurrentSession.iIdent = -1;
    TheLobbyGameSessions = this;
}

LobbyGameSessions &LobbyGameSessions::Instance() {
    static LobbyGameSessions theLobbyGameSessions;
    return theLobbyGameSessions;
}

int32 LobbyGameSessions::CreateSession(const char *sessionName, CommandCBFunc createSessionCB, void *context) {
    CommandCBFunc localCreateSessionCB = createSessionCB;
    lobbyMutex.Lock("LobbyGameSessions::CreateSession");
    if (sessionName && sessionName[0] && myCurrentSession.iIdent == -1 &&
        LobbyCore::Instance().FindCommandID('ujoi', nullptr, nullptr, nullptr, nullptr) == -1 &&
        LobbyCore::Instance().FindCommandID('ucre', nullptr, nullptr, nullptr, nullptr) == -1) {
        MenuScreen::MaybeShutdownVoIPChat();

        char realSessionName[68] = "";
        bSPrintf(realSessionName, "%d%02d.%s", uniqueID++, bRandom(99), sessionName);
        realSessionName[32] = '\0';

        char buf[1024] = "";
        TagFieldSetString(buf, sizeof(buf), "NAME", realSessionName);
        TagFieldSetNumber(buf, sizeof(buf), "SIZE", cOnlineSettings::MaxOnlinePlayers);
        TagFieldSetNumber(buf, sizeof(buf), "TYPE", 0);
        TagFieldSetNumber(buf, sizeof(buf), "UPDATES", 0);
        TagFieldSetFlags(buf, sizeof(buf), "SYSFLAGS", 0x400800);

        uint32 custflags = 0;
        UpdateSessionFlags(custflags);
        TagFieldSetFlags(buf, sizeof(buf), "CUSTFLAGS", custflags);

        char params[68] = "";
        UpdateSessionParams(params, 64);
        TagFieldSetString(buf, sizeof(buf), "PARAMS", params);

        if (!sessionMembers) {
            sessionMembers = LobbyApiListAlloc(LobbyCore::Instance().pLobbyRef,
                                               static_cast<LobbyRanks::RankListMapping>(0x18),
                                               SessionMembersDispListCB, this);
        }

        int32 rc = LobbyCore::Instance().QueueCommand('ucre', buf, CreateSessionCB, this, localCreateSessionCB, context, false);
        lobbyMutex.Unlock("LobbyGameSessions::CreateSession");
        return rc;
    }
    lobbyMutex.Unlock("LobbyGameSessions::CreateSession");
    return -1;
}

int32 LobbyGameSessions::JoinSession(const char *sessionName, const char *password, CommandCBFunc joinSessionCB,
                                     void *context) {
    lobbyMutex.Lock("LobbyGameSessions::JoinSession");
    if (myCurrentSession.iIdent == -1 &&
        LobbyCore::Instance().FindCommandID('ujoi', nullptr, nullptr, nullptr, nullptr) == -1 &&
        LobbyCore::Instance().FindCommandID('ucre', nullptr, nullptr, nullptr, nullptr) == -1) {
        MenuScreen::MaybeShutdownVoIPChat();
        if (!sessionMembers) {
            sessionMembers = LobbyApiListAlloc(LobbyCore::Instance().pLobbyRef,
                                               static_cast<LobbyRanks::RankListMapping>(0x18),
                                               SessionMembersDispListCB, this);
        }
        LobbyUsers::Instance().ClearUserOnlineRecordCache();

        char buf[96] = "";
        TagFieldSetString(buf, sizeof(buf), "USERSET0", sessionName);
        LobbyCore::Instance().QueueCommand('sele', buf, nullptr, nullptr, nullptr, nullptr, false);

        buf[0] = '\0';
        TagFieldSetString(buf, sizeof(buf), "NAME", sessionName);
        if (password && password[0]) {
            TagFieldSetString(buf, sizeof(buf), "PASS", password);
        }

        int32 rc = LobbyCore::Instance().QueueCommand('ujoi', buf, JoinSessionCB, this, joinSessionCB, context, false);
        lobbyMutex.Unlock("LobbyGameSessions::JoinSession");
        return rc;
    }
    lobbyMutex.Unlock("LobbyGameSessions::JoinSession");
    return -1;
}

void LobbyGameSessions::CreateGameInSession() {
    lobbyMutex.Lock("LobbyGameSessions::CreateGameInSession");
    LobbyGames::Instance().CreateGame(myCurrentSession.strOwner, FEDatabase->OnlineSettings, RecreateGameCB, this);
    lobbyMutex.Unlock("LobbyGameSessions::CreateGameInSession");
}

int32 LobbyGameSessions::LeaveSession(CommandCBFunc leaveSessionCB, void *context) {
    lobbyMutex.Lock("LobbyGameSessions::LeaveSession");
    int32 rc = LeaveSession_HaveMutex(leaveSessionCB, context);
    if (sessionMembers) {
        LobbyApiListFree(LobbyCore::Instance().pLobbyRef, 0x18, sessionMembers);
        sessionMembers = nullptr;
    }
    bMemSet(&myCurrentSession, 0, sizeof(myCurrentSession));
    hostInactiveTimer = 0.0f;
    hostStartSessionTimer = 0.0f;
    hostHurryTimer = 0.0f;
    myCurrentSession.iIdent = -1;
    lobbyMutex.Unlock("LobbyGameSessions::LeaveSession");
    return rc;
}

inline int32 LobbyGameSessions::KickPlayer(const char *name, CommandCBFunc kickUserCB, void *context) {
    lobbyMutex.Lock("LobbyGameSessions::KickPlayer");
    if (name && name[0] && myCurrentSession.iIdent != -1) {
        char buf[128] = "";
        TagFieldSetString(buf, sizeof(buf), "NAME", myCurrentSession.strName);
        TagFieldSetString(buf, sizeof(buf), "PERS", name);
        int32 rc = LobbyCore::Instance().QueueCommand('ukik', buf, LobbyCore::DefaultCB, nullptr, kickUserCB,
                                                       context, false);
        lobbyMutex.Unlock("LobbyGameSessions::KickPlayer");
        return rc;
    }
    lobbyMutex.Unlock("LobbyGameSessions::KickPlayer");
    return -1;
}

void LobbyGameSessions::SettingsHaveChanged() {
    lobbyMutex.Lock("LobbyGameSessions::SettingsHaveChanged");
    UpdateSessionInfo(false);
    lobbyMutex.Unlock("LobbyGameSessions::SettingsHaveChanged");
}

void LobbyGameSessions::StartSessionChanges() {
    lobbyMutex.Lock("LobbyGameSessions::StartSessionChanges");
    LobbyUsers::Instance().SetSessionChangeFlag(true);
    lobbyMutex.Unlock("LobbyGameSessions::StartSessionChanges");
}

void LobbyGameSessions::FinishSessionChanges() {
    lobbyMutex.Lock("LobbyGameSessions::FinishSessionChanges");
    LobbyUsers::Instance().SetSessionChangeFlag(false);
    lobbyMutex.Unlock("LobbyGameSessions::FinishSessionChanges");
}

bool LobbyGameSessions::IsMemberMakingChanges(int32 index) {
    lobbyMutex.Lock("LobbyGameSessions::IsMemberMakingChanges");
    bool rc = IsMemberMakingChanges_HaveMutex(index);
    lobbyMutex.Unlock("LobbyGameSessions::IsMemberMakingChanges");
    return rc;
}

bool LobbyGameSessions::IsMemberMakingChanges_HaveMutex(int32 index) {
    GameSessionMember *member = GetMemberByIndex_HaveMutex(index);
    bool makingChanges = false;
    if (member) {
        makingChanges = TagFieldGetNumber(TagFieldFind(member->strAux, "SCF"), 0) != 0;
    }
    return makingChanges;
}

void LobbyGameSessions::SetSessionLatency(int late) {
    lobbyMutex.Lock("LobbyGameSessions::StartSessionChanges");
    LobbyUsers::Instance().SetSessionLatency(late);
    lobbyMutex.Unlock("LobbyGameSessions::StartSessionChanges");
}

void LobbyGameSessions::SetSessionRaceStatusInfo(int lap, int mapx, int mapy) {
    lobbyMutex.Lock("LobbyGameSessions::StartSessionChanges");
    LobbyUsers::Instance().SetSessionRaceStatusInfo(lap, mapx, mapy);
    lobbyMutex.Unlock("LobbyGameSessions::StartSessionChanges");
}

int LobbyGameSessions::GetMemberLatency(int32 index) {
    lobbyMutex.Lock("LobbyGameSessions::IsMemberMakingChanges");
    int rc = GetMemberLatency_HaveMutex(index);
    lobbyMutex.Unlock("LobbyGameSessions::IsMemberMakingChanges");
    return rc;
}

int LobbyGameSessions::GetMemberLatency_HaveMutex(int32 index) {
    GameSessionMember *member = GetMemberByIndex_HaveMutex(index);
    int late = -1;
    if (member) {
        late = TagFieldGetNumber(TagFieldFind(member->strAux, "LT"), -1);
    }
    return late;
}

void LobbyGameSessions::GetMemberRaceStatus(int32 index, int &lap, int &mapx, int &mapy) {
    lobbyMutex.Lock("LobbyGameSessions::IsMemberMakingChanges");
    GameSessionMember *member = GetMemberByIndex_HaveMutex(index);
    lap = 0;
    mapx = 0;
    mapy = 0;
    if (member) {
        lap = TagFieldGetNumber(TagFieldFind(member->strAux, "LP"), 0);
        mapx = TagFieldGetNumber(TagFieldFind(member->strAux, "MX"), 0);
        mapy = TagFieldGetNumber(TagFieldFind(member->strAux, "MY"), 0);
    }
    lobbyMutex.Unlock("LobbyGameSessions::IsMemberMakingChanges");
}

int8 LobbyGameSessions::GetSecsBeforeHostCanStart() {
    lobbyMutex.Lock("LobbyGameSessions::GetSecsBeforeHostCanStart");

    float MasterSecs = TheOnlineManager.GetMasterTime() * 0.001f;
    LobbyApiPlayT *myGame = LobbyGames::Instance().GetMyGame();
    if (myCurrentSession.iIdent == -1 || !myGame || myGame->iCount < 2 || (myGame->uSysflags & 0x80000) ||
        myGame->iCount < FEDatabase->OnlineSettings.MinOnlinePlayers) {
        lobbyMutex.Unlock("LobbyGameSessions::GetSecsBeforeHostCanStart");
        hostInactiveTimer.UnSet();
        hostStartSessionTimer.UnSet();
        hostHurryTimer.UnSet();
        return -1;
    }

    if (!hostStartSessionTimer.IsSet()) {
        hostStartSessionTimer.SetTime(MasterSecs + 60.0f);
    }

    bool memberIsMakingChanges = false;
    for (int i = 0; i < myCurrentSession.iCount; i++) {
        if (IsMemberMakingChanges_HaveMutex(i)) {
            memberIsMakingChanges = true;
            break;
        }
    }

    if (memberIsMakingChanges) {
        mHurryTimerStopped = true;
        hostInactiveTimer.UnSet();
        hostHurryTimer.UnSet();
    } else {
        if (!hostInactiveTimer.IsSet()) {
            hostInactiveTimer.SetTime(MasterSecs + 5.0f);
        }
        if (!hostHurryTimer.IsSet() && hostInactiveTimer.GetSeconds() - MasterSecs <= 0.0f) {
            mHurryTimerStarted = true;
            hostHurryTimer.SetTime(MasterSecs + 10.0f + 0.95f);
        }
    }

    float hurryDiff = 61.0f;
    if (hostHurryTimer.IsSet()) {
        hurryDiff = hostHurryTimer.GetSeconds() - MasterSecs;
        if (hurryDiff < 0.0f) {
            hurryDiff = 0.0f;
        }
    }

    float diff = hostStartSessionTimer.GetSeconds() - MasterSecs;
    if (diff < 0.0f) {
        diff = 0.0f;
    }

    lobbyMutex.Unlock("LobbyGameSessions::GetSecsBeforeHostCanStart");
    if (hostHurryTimer.IsSet() && (diff > 10.0f || hurryDiff <= diff)) {
        diff = hurryDiff;
    }

    int8 result = static_cast<int8>(static_cast<int>(diff));
    if (static_cast<int>(diff) == 0) {
        hostInactiveTimer.UnSet();
        hostStartSessionTimer.UnSet();
        hostHurryTimer.UnSet();
    }
    return result;
}

int32 LobbyGameSessions::FindSessions(const FilterGameSessionParamsT &filterParams,
                                      CommandCBFunc filterSessionsCB, void *context) {
    lobbyMutex.Lock("LobbyGameSessions::FindSessions");
    if (LobbyCore::Instance().FindCommandID('usea', nullptr, nullptr, nullptr, nullptr) != -1) {
        lobbyMutex.Unlock("LobbyGameSessions::FindSessions");
        return -1;
    }

    uint32 custflags = 0;
    uint32 custmask = 1;
    char buf[512] = "";
    if (filterParams.sessionNameSubstr[0]) {
        TagFieldSetString(buf, sizeof(buf), "NAME", filterParams.sessionNameSubstr);
    } else {
        if (filterParams.collisionDetection != ANY) {
            custmask |= 0x400;
            if (filterParams.collisionDetection == ON) {
                custflags |= 0x400;
            }
        }
        if (filterParams.performanceMatching != ANY) {
            custmask |= 0x2000;
            if (filterParams.performanceMatching == ON) {
                custflags |= 0x2000;
            }
        }
        switch (filterParams.gameMode) {
        case GRace::kRaceType_Circuit:
            custmask |= 0x80000000;
            custflags |= 0x80000000;
            break;
        case GRace::kRaceType_P2P:
            custmask |= 0x40000000;
            custflags |= 0x40000000;
            break;
        case GRace::kRaceType_Drag:
            custmask |= 0x20000000;
            custflags |= 0x20000000;
            break;
        default:
            break;
        }
        if (filterParams.rankedGames != ANY) {
            custmask |= 0x200;
            if (filterParams.rankedGames == ON) {
                custflags |= 0x200;
            }
        }
        if (filterParams.disconnectPerc > 0) {
            custmask |= 0x100000;
        }
        if (filterParams.disconnectPerc > 1) {
            custmask |= 0x200000;
        }
        if (filterParams.disconnectPerc > 2) {
            custmask |= 0x400000;
        }
        if (filterParams.disconnectPerc > 3) {
            custmask |= 0x800000;
        }
        if (filterParams.disconnectPerc > 4) {
            custmask |= 0x1000000;
        }
        if (filterParams.disconnectPerc == OLS_DISCONNECT_PERC_ANY) {
            custmask |= 0x2000000;
        }
    }

    TagFieldSetNumber(buf, sizeof(buf), "START", 0);
    TagFieldSetNumber(buf, sizeof(buf), "COUNT", filterParams.count);
    TagFieldSetNumber(buf, sizeof(buf), "CUSTFLAGS", custflags);
    TagFieldSetNumber(buf, sizeof(buf), "CUSTMASK", custmask);
    if (sessionList) {
        LobbyApiListFree(LobbyCore::Instance().pLobbyRef, 0x17, sessionList);
        sessionList = nullptr;
    }
    sessionList = LobbyApiListAlloc(LobbyCore::Instance().pLobbyRef,
                                    static_cast<LobbyRanks::RankListMapping>(0x17), SessionDispListCB, this);
    extraSessionDataMap.clear();
    DispListSort(sessionList, 0, currentSortParams, SortFunc);
    DispListFilt(sessionList, 0, 0, FilterFunc);
    int32 rc = LobbyCore::Instance().QueueCommand('usea', buf, FindSessionsCB, this, filterSessionsCB, context, false);
    lobbyMutex.Unlock("LobbyGameSessions::FindSessions");
    return rc;
}

void LobbyGameSessions::SetSortField(LobbyGameSessionsN::SortField sortField, bool ascending) {
    lobbyMutex.Lock("LobbyGameSessions::SetSortField");
    SetSortField_HaveMutex(sortField, ascending);
    lobbyMutex.Unlock("LobbyGameSessions::SetSortField");
}

void LobbyGameSessions::SetSortField_HaveMutex(LobbyGameSessionsN::SortField sortField, bool ascending) {
    if (sessionList && sortField >= LobbyGameSessionsN::SORT_PING) {
        uint32 sortParams = sortField | LobbyGameSessionsN::SORT_INVALID;
        if (!ascending) {
            sortParams = sortField;
        }
        currentSortParams = sortParams;
        DispListSort(sessionList, 0, sortParams, SortFunc);
    }
}

GameSession *LobbyGameSessions::GetMySession() const {
    GameSession *session = nullptr;
    if (myCurrentSession.iIdent != -1) {
        session = const_cast<GameSession *>(&myCurrentSession);
    }
    return session;
}

char *LobbyGameSessions::GetSessionDisplayName(const GameSession *session) {
    static char realName[36];
    const GameSession *theSession = session ? session : &myCurrentSession;
    char *tmp;
    if (theSession->iIdent == -1 || !(tmp = bStrChr(theSession->strName, '.'))) {
        return nullptr;
    }
    bStrNCpy(realName, tmp + 1, 32);
    return realName;
}

inline eOnlineDisconnectPerc LobbyGameSessions::GetSessionDisconnectPercentage(const GameSession *session) {
    const GameSession *theSession = session ? session : &myCurrentSession;
    eOnlineDisconnectPerc eDisconnectPercentage = OLS_DISCONNECT_PERC_5;
    if (theSession->uCustFlags & 0x2000000) {
        eDisconnectPercentage = OLS_DISCONNECT_PERC_ANY;
    } else if (theSession->uCustFlags & 0x1000000) {
        eDisconnectPercentage = OLS_DISCONNECT_PERC_50;
    } else if (theSession->uCustFlags & 0x800000) {
        eDisconnectPercentage = OLS_DISCONNECT_PERC_25;
    } else if (theSession->uCustFlags & 0x400000) {
        eDisconnectPercentage = OLS_DISCONNECT_PERC_20;
    } else if (theSession->uCustFlags & 0x200000) {
        eDisconnectPercentage = OLS_DISCONNECT_PERC_15;
    } else if (theSession->uCustFlags & 0x100000) {
        eDisconnectPercentage = OLS_DISCONNECT_PERC_10;
    }
    return eDisconnectPercentage;
}

ExtraSessionData *LobbyGameSessions::GetExtraSessionDataByIdent(int32 ident) {
    ExtraSessionData *rc = nullptr;
    lobbyMutex.Lock("LobbyGameSessions::GetExtraSessionDataByIdent");
    ExtraSessionDataMap::iterator esd = extraSessionDataMap.find(ident);
    if (esd != extraSessionDataMap.end()) {
        rc = &esd->second;
    }
    lobbyMutex.Unlock("LobbyGameSessions::GetExtraSessionDataByIdent");
    return rc;
}

OnlineRaceModeE LobbyGameSessions::GetRaceMode(const GameSession *session) {
    lobbyMutex.Lock("LobbyGameSessions::GetRaceMode");
    const GameSession *theSession = session ? session : &myCurrentSession;
    if (myCurrentSession.iIdent < 0 && !session) {
        lobbyMutex.Unlock("LobbyGameSessions::GetRaceMode");
        return OL_RACE_MODE_INVALID;
    }

    OnlineRaceModeE theMode = OL_RACE_MODE_INVALID;
    if (theSession->uCustFlags & 0x80000000) {
        theMode = OL_RACE_MODE_CIRCUIT;
    } else if (theSession->uCustFlags & 0x40000000) {
        theMode = OL_RACE_MODE_SPRINT;
    } else if (theSession->uCustFlags & 0x20000000) {
        theMode = OL_RACE_MODE_DRAG;
    }
    lobbyMutex.Unlock("LobbyGameSessions::GetRaceMode");
    return theMode;
}

int LobbyGameSessions::GetNumSessions() const {
    return sessionList ? DispListShown(sessionList) : 0;
}

GameSession *LobbyGameSessions::GetSessionByName(const char *sessionName) const {
    lobbyMutex.Lock("LobbyGameSessions::GetSessionByName");
    GameSession *session = static_cast<GameSession *>(
        DispListGet(sessionList, LobbyApiListFindByName(LobbyCore::Instance().pLobbyRef, 0x17, sessionName)));
    lobbyMutex.Unlock("LobbyGameSessions::GetSessionByName");
    return session;
}

GameSession *LobbyGameSessions::GetSessionByIndex(int32 index) const {
    lobbyMutex.Lock("LobbyGameSessions::GetSessionByIndex");
    GameSession *rc = GetSessionByIndex_HaveMutex(index);
    lobbyMutex.Unlock("LobbyGameSessions::GetSessionByIndex");
    return rc;
}

GameSession *LobbyGameSessions::GetSessionByIndex_HaveMutex(int32 index) const {
    return static_cast<GameSession *>(DispListIndex(sessionList, index));
}

GameSessionMember *LobbyGameSessions::GetMemberByIndex(int32 index) const {
    lobbyMutex.Lock("LobbyGameSessions::GetMemberByIndex");
    GameSessionMember *member = GetMemberByIndex_HaveMutex(index);
    lobbyMutex.Unlock("LobbyGameSessions::GetMemberByIndex");
    return member;
}

GameSessionMember *LobbyGameSessions::GetMemberByIndex_HaveMutex(int32 index) const {
    GameSessionMember *member;
    if (index == 0) {
        member = nullptr;
        for (int i = 0; i < myCurrentSession.iCount; i++) {
            member = static_cast<GameSessionMember *>(DispListIndex(sessionMembers, i));
            if (!member || bStrCmp(member->strPers, myCurrentSession.strOwner) == 0) {
                break;
            }
        }
    } else {
        member = static_cast<GameSessionMember *>(DispListIndex(sessionMembers, index));
        if (member && bStrCmp(member->strPers, myCurrentSession.strOwner) == 0) {
            member = static_cast<GameSessionMember *>(DispListIndex(sessionMembers, 0));
        }
    }
    return member;
}
