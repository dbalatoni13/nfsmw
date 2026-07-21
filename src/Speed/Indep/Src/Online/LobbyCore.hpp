#ifndef ONLINE_LOBBY_CORE_HPP
#define ONLINE_LOBBY_CORE_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "NetworkCore.hpp"

struct LobbyApiRefT;
struct LobbyApiMsgT {
    int id;
    int type;
    int kind;
    int code;
    char *pData;
    void *pMisc;
};
struct LobbyApiServerStatT;
struct LobbyPingManagerRefT;
struct DispListRef;
struct LobbyApiRoomT;
struct ConnApiRefT;
struct NetGameUtilRefT;
struct NetGameLinkRefT;
struct NetGameDistRefT;

enum ConnApiCbTypeE {
    CONNAPI_CBTYPE_GAMEEVENT = 0,
    CONNAPI_CBTYPE_VOIPEVENT = 1,
    CONNAPI_CBTYPE_SESSEVENT = 2,
    CONNAPI_NUMCBTYPES = 3
};

enum ConnApiConnStatusE {
    CONNAPI_STATUS_INIT = 0,
    CONNAPI_STATUS_CONN = 1,
    CONNAPI_STATUS_MNGL = 2,
    CONNAPI_STATUS_ACTV = 3,
    CONNAPI_STATUS_CLSE = 4,
    CONNAPI_STATUS_DISC = 5,
    CONNAPI_NUMSTATUSTYPES = 6
};

struct ConnApiCbInfoT {
    int iClientId;
    ConnApiCbTypeE eType;
    ConnApiConnStatusE eOldStatus;
    ConnApiConnStatusE eNewStatus;
};

struct ConnApiUserInfoT {
    unsigned int uAddr;
    unsigned int uLocalAddr;
    DirtyAddrT DirtyAddr;
    char strName[32];
    char strUniqueId[32];
};

struct ConnApiConnInfoT {
    ConnApiConnStatusE eStatus;
    unsigned short uMnglPort;
    unsigned short bDemangling;
};

struct ConnApiClientT {
    ConnApiUserInfoT UserInfo;
    ConnApiConnInfoT GameInfo;
    ConnApiConnInfoT VoipInfo;
    NetGameUtilRefT *pGameUtilRef;
    NetGameLinkRefT *pGameLinkRef;
    NetGameDistRefT *pGameDistRef;
    int iConnStart;
    int iVoipConnId;
    unsigned short uConnFlags;
    unsigned short uFlags;
};

struct ConnApiClientListT {
    int iNumClients;
    int iMaxClients;
    ConnApiClientT Clients[1];
};

typedef void ConnApiCallbackT(ConnApiRefT *, ConnApiCbInfoT *, void *);
typedef void LobbyApiCallbackT(LobbyApiRefT *, LobbyApiMsgT *, void *);

#include "LobbyRanks.hpp"
#include "LobbyChat.hpp"

enum LobbyApiCBTypeE {
    LOBBYAPI_CBTYPE_INVALID = -1,
    LOBBYAPI_CBTYPE_RESP = 0,
    LOBBYAPI_CBTYPE_CHAT = 1,
    LOBBYAPI_CBTYPE_DISP = 2,
    LOBBYAPI_CBTYPE_CONN = 3,
    LOBBYAPI_CBTYPE_EVNT = 4,
    LOBBYAPI_CBTYPE_IDLE = 5,
    LOBBYAPI_CBTYPE_TOTAL = 6
};

namespace LobbyCoreN {
enum OnlineDisconnectReason {
    DISC_DISCONNECTED = 0,
    DISC_SERVER_DOWN = 1,
    DISC_IDLE_TIMEOUT = 2,
    DISC_MAX_TIMEOUT = 3,
    DISC_NEED_UPDATE = 4
};
}

struct LobbyApiUserSetT {
    int iIdent;
    int iType;
    uint32 uSysFlags;
    uint32 uCustFlags;
    char strOwner[16];
    int iSize;
    int iCount;
    char strName[36];
    char strDesc[68];
    char strParams[68];
};

typedef LobbyApiUserSetT GameSession;

struct LobbyApiPartitionT {
    int iSize;
    char strParams[36];
};

struct LobbyApiPlayerT {
    int iIdent;
    char strPers[16];
    uint32 uAddr;
    uint32 uLocalAddr;
    char strMachineAddr[64];
    int iPartition;
    uint32 uFlags;
    int iPresence;
    char strParams[36];
};

struct LobbyApiPlayT {
    int iIdent;
    char strName[36];
    char strSelf[16];
    char strHost[16];
    int iRoom;
    char strSess[128];
    char strParams[132];
    char strPlatParams[16];
    uint32 uCustflags;
    uint32 uSysflags;
    int iMinsize;
    int iMaxsize;
    uint32 uSeed;
    uint32 uWhen;
    char strAuth[64];
    int iNumPartitions;
    LobbyApiPartitionT aPartitions[2];
    int iCount;
    int iPrivSlots;
    LobbyApiPlayerT aOpponents[8];
};

struct LobbyApiColorT {
    unsigned char uRed;
    unsigned char uGreen;
    unsigned char uBlue;
    unsigned char uAlpha;
};

struct LobbyApiUserT {
    int ident;
    int flags;
    char name[16];
    char ping[8];
    unsigned int addr;
    int rank;
    char stat[132];
    char aux[132];
    int game;
    unsigned int attr;
    LobbyApiColorT Color;
    unsigned int uLevel;
    unsigned int uMedals;
    char aUserSets[4][36];
    unsigned int uHwFlags;
    int iReputation;
    char strReputation[4];
    DirtyAddrT MachineAddr;
    unsigned int uLocalAddr;
    unsigned int uLocality;
    char strClubID[20];
    char strClubTag[8];
};

int32 LobbyInit();
void LobbyDisconnect();

struct LobbyGames;

struct LobbyGameSessions;

struct LobbyUsers {
    static LobbyUsers &Instance();
    void ClearUserOnlineRecordCache();
    void ClearUserOnlineRecordCache(const LobbyApiPlayT &game);
    void SetSessionChangeFlag(bool changing);
    void SetSessionLatency(int latency);
    void SetSessionRaceStatusInfo(int lap, int mapx, int mapy);
    LobbyApiUserT *GetMyUserRecord() const;

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
};

struct LobbyAccount {
    static LobbyAccount &Instance();

  private:
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
};

struct LobbyRooms {
    static LobbyRooms &Instance();

    typedef void (*RoomUpdateCBFunc)();

  private:
    LobbyRooms();
    ~LobbyRooms();
    int32 Init();
    void Reset();
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend class LobbyCore;

    DispListRef *roomList;
    RoomUpdateCBFunc roomUpdateCB;
    void *roomUpdateContext;
    char currentRoomName[36];
};

struct ConnectionCore {
    ConnectionCore();
    ~ConnectionCore();

    static ConnectionCore &Instance();
    void Init(int maxNumPlayers, ConnApiCallbackT *cbfunc, void *context);
    void Reset();
    void SetCallback(ConnApiCallbackT *cbfunc, void *context);
    void HostSession(int sessionID, int connectionType);
    void JoinSession(ConnApiUserInfoT &hostInfo, int sessionID, int connectionType);
    void JoinSession(LobbyApiUserT &hostInfo, int sessionID, int connectionType) {
        ConnApiUserInfoT userInfo[2];
        BuildUserInfo(userInfo[0], hostInfo);
        JoinSession(userInfo[0], sessionID, connectionType);
    }
    void JoinSession(LobbyApiPlayerT &hostInfo, int sessionID, void *strSess, int connectionType);
    bool IsSessionStarted();
    void LeaveSession();
    void AddPlayer(ConnApiUserInfoT &userInfo);
    void AddPlayer(LobbyApiUserT &userInfo);
    void AddPlayer(LobbyApiPlayerT &userInfo);
    void UpdatePlayers(const LobbyApiPlayT &game);
    void RemovePlayer(char *playerName);
    void RemovePlayer(int index);
    int GetNumPlayers();
    int GetNumConnectedPlayers();
    ConnApiClientT *GetPlayer(char *name);
    ConnApiClientT *GetPlayer(int index);
    ConnApiClientT *GetHost();
    static void BuildUserInfo(ConnApiUserInfoT &dest, const LobbyApiUserT &src);
    static void BuildUserInfo(ConnApiUserInfoT &dest, const LobbyApiPlayerT &src);
    int GetStatus();

  private:
    void UpdateNumConnectedPlayers();
    void MaybeGoOnline();
    void AddPlayer_HaveMutex(const LobbyApiPlayerT &userInfo);
    void ResetSession_HaveMutex();
    static void ConnApiCallback(ConnApiRefT *connapi, ConnApiCbInfoT *cbinfo, void *context);
    static void FrontEndCB(ConnApiRefT *connapi, ConnApiCbInfoT *cbinfo, void *context);

    ConnApiRefT *connapi;
    ConnApiCallbackT *connapiCallback;
    void *callbackContext;
    int numConnectedPlayers;
    bool isOnline;
};

class LobbyCore {
  public:
    struct GlobalCB : bTNode<GlobalCB> {
        LobbyApiCallbackT *cbFunc;
        void *context;
    };

    struct LobbyCommand : bTNode<LobbyCommand> {
        LobbyCommand(int commandKind, const char *request, bool commandGroup, LobbyApiCallbackT *callback, void *callbackContext,
                     CommandCBFunc completionCallback, void *completionContext);

        int kind;
        char *req;
        LobbyApiCallbackT *lobbyCB;
        void *lobbyContext;
        CommandCBFunc commandCB;
        void *commandContext;
        int32 commandID;
        int32 groupID;
        int lobbyCallbackID;
    };

    struct RaceResults {
        void Clear();

        char auth[64];
        uint64 when;
        char rept[16];
        char name[4][16];
        char results[1024];
    };

    typedef void (*DisconnectCBFunc)(LobbyCoreN::OnlineDisconnectReason, LobbyApiMsgT *, void *);

    LobbyCore();
    ~LobbyCore();

    static LobbyCore &Instance();
    void DoProcessing();
    int32 SetAsyncServerMessages();
    void SaveRaceResults(const char *results);
    int32 SendSavedRaceResults(CommandCBFunc func, void *context);
    LobbyApiServerStatT *GetServerStats();
    void AbortCommand(int32 commandID);
    void AbortCommand(CommandCBFunc commandCB, bool abortAll);
    void AbortGroup(int32 commandID);
    void AbortAllCommands();
    void SetDisconnectCallback(DisconnectCBFunc func, void *context);
    char *GetServerConfig();
    LobbyApiRefT *GetLobbyApiRef();
    void Suspend();
    void Resume(bool fullResume);
    int RegisterGlobalCallback(LobbyApiCBTypeE type, LobbyApiCallbackT *func, void *context);
    void UnregisterGlobalCallback(LobbyApiCBTypeE type, LobbyApiCallbackT *func, void *context);
    static void LobbyPrintf(void *ref, const char *text);

  private:
    int32 Init();
    void Reset();
    int32 SetAsyncServerMessages_HaveMutex();
    int32 QueueCommand(int kind, const char *request, LobbyApiCallbackT *lobbyCB, void *lobbyContext,
                       CommandCBFunc commandCB, void *commandContext, bool makeNextCommand);
    int32 FindCommandID(int kind, LobbyApiCallbackT *lobbyCB, void *lobbyContext, CommandCBFunc commandCB, void *commandContext);
    static void RankCB(LobbyApiRefT *lobbyRef, LobbyApiMsgT *msg, void *context);
    static void GlobalConnStatusCB(LobbyApiRefT *lobbyRef, LobbyApiMsgT *msg, void *context);
    static void GlobalResponseCB(LobbyApiRefT *lobbyRef, LobbyApiMsgT *msg, void *context);
    static void GlobalEventCB(LobbyApiRefT *lobbyRef, LobbyApiMsgT *msg, void *context);
    static void DefaultCB(LobbyApiRefT *lobbyRef, LobbyApiMsgT *msg, void *context);
    void StartGroup() { groupStarted = true; }
    void EndGroup() { groupStarted = false; }
    void AbortCurrentCommand();
    void AbortAllCommands_HaveMutex();
    void FinishCommand(LobbyApiMsgT *msg, bool doCallback);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend struct LobbyRanks;
    friend struct LobbyChat;
    friend struct LobbyGames;
    friend struct LobbyGameSessions;
    friend struct ConnectionCore;
    friend struct BuddyCore;

    bTList<GlobalCB> globalCBList[6];
    bTList<LobbyCommand> lobbyCommandQueue;
    LobbyApiRefT *pLobbyRef;
    LobbyPingManagerRefT *pingManagerRef;
    LobbyCommand *currentCommand;
    static RaceResults raceResults;
    DisconnectCBFunc disconnectCB;
    void *disconnectContext;
    bool groupStarted;
    bool raceResultsAccepted;
    bool cheatResponseActive;
    char cheatResponse[33];
    bool sendCDKey;
    bool mIsSuspended;
};

#endif
