#include "LobbyGameSessions.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

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
