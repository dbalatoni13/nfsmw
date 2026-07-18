#include "LobbyGameSessions.hpp"

bool LobbyGameSessions::mHurryTimerStarted = false;
bool LobbyGameSessions::mHurryTimerStopped = false;
LobbyGameSessions *TheLobbyGameSessions;

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
