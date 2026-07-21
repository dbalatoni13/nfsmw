#include "LobbyCore.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

extern "C" {
ConnApiRefT *ConnApiCreate(const char *sessionName, int gamePort, int maxClients,
                           ConnApiCallbackT *callback, void *userData);
void ConnApiDestroy(ConnApiRefT *connapi);
int ConnApiHost(ConnApiRefT *connapi, ConnApiUserInfoT *userInfo, int numClients, int sessionID);
int ConnApiConnect(ConnApiRefT *connapi, ConnApiUserInfoT *userInfo, int numClients, int sessionID);
int ConnApiAddClient(ConnApiRefT *connapi, ConnApiUserInfoT *userInfo);
ConnApiClientListT *ConnApiGetClientList(ConnApiRefT *connapi);
int ConnApiRemoveClient(ConnApiRefT *connapi, const char *clientName, int clientIndex);
int ConnApiControl(ConnApiRefT *connapi, int control, int value, int value2, void *pValue);
}

ConnectionCore *TheConnectionCore;

ConnectionCore::ConnectionCore() {
    connapi = nullptr;
    TheConnectionCore = this;
    connapiCallback = nullptr;
    callbackContext = nullptr;
    numConnectedPlayers = 0;
    isOnline = false;
}

ConnectionCore::~ConnectionCore() { Reset(); }

ConnectionCore &ConnectionCore::Instance() {
    static ConnectionCore theConnectionCore;
    return theConnectionCore;
}

void ConnectionCore::Init(int maxNumPlayers, ConnApiCallbackT *cbfunc, void *context) {
    char sessionname[64];

    networkMutex.Lock("ConnectionCore::Init");
    if (connapi) {
        networkMutex.Unlock("ConnectionCore::Init");
        return;
    }

    bSPrintf(sessionname, "%s-%s-%s", OLGetProductName(), OLGetPlatform(), OLGetProductYear());
    connapiCallback = cbfunc;
    callbackContext = context;
    numConnectedPlayers = 0;
    isOnline = false;
    VoiceCore::mInstance->Startup();
    VoiceCore::mInstance->SetHeadsetPort(FEDatabase->PlayerJoyports[0]);
    connapi = ConnApiCreate(sessionname, 3658, maxNumPlayers, ConnApiCallback, nullptr);
    ConnApiControl(connapi, 'mwid', 100, 0, nullptr);
    ConnApiControl(connapi, 'mout', 16, 0, nullptr);
    networkMutex.Unlock("ConnectionCore::Init");
}

void ConnectionCore::Reset() {
    networkMutex.Lock("ConnectionCore::Reset");
    if (connapi) {
        ConnApiDestroy(connapi);
    }
    connapi = nullptr;
    connapiCallback = nullptr;
    callbackContext = nullptr;
    numConnectedPlayers = 0;
    isOnline = false;
    VoiceCore::mInstance->Shutdown();
    networkMutex.Unlock("ConnectionCore::Reset");
}

void ConnectionCore::SetCallback(ConnApiCallbackT *cbfunc, void *context) {
    bool mutexLocked = false;
    if (connapi) {
        networkMutex.Lock("ConnectionCore::SetCallback");
        mutexLocked = true;
    }
    connapiCallback = cbfunc;
    callbackContext = context;
    if (mutexLocked == true) {
        networkMutex.Unlock("ConnectionCore::SetCallback");
    }
}

void ConnectionCore::HostSession(int sessionID, int connectionType) {
    ConnApiUserInfoT myUserInfo;

    networkMutex.Lock("ConnectionCore::HostSession");
    MaybeGoOnline();
    if (SkipFE) {
        myUserInfo.uAddr = NetworkCore::MyIPAddress();
        myUserInfo.uLocalAddr = myUserInfo.uAddr;
        bMemCpy(&myUserInfo.DirtyAddr, &NetworkCore::MyDirtyAddr(), sizeof(myUserInfo.DirtyAddr));
        bStrCpy(myUserInfo.strName, FEDatabase->OnlineSettings.GetLobbyPersona());
    } else {
        BuildUserInfo(myUserInfo, *LobbyUsers::Instance().GetMyUserRecord());
    }
    ConnApiControl(connapi, 'type', connectionType, 0, nullptr);
    ConnApiHost(connapi, &myUserInfo, 1, sessionID);
    networkMutex.Unlock("ConnectionCore::HostSession");
}

void ConnectionCore::JoinSession(ConnApiUserInfoT &hostInfo, int sessionID, int connectionType) {
    ConnApiUserInfoT userInfo[2];

    networkMutex.Lock("ConnectionCore::JoinSession");
    MaybeGoOnline();
    bMemCpy(userInfo, &hostInfo, sizeof(hostInfo));
    if (SkipFE) {
        userInfo[1].uAddr = NetworkCore::MyIPAddress();
        userInfo[1].uLocalAddr = userInfo[1].uAddr;
        bMemCpy(&userInfo[1].DirtyAddr, &NetworkCore::MyDirtyAddr(), sizeof(userInfo[1].DirtyAddr));
        bStrCpy(userInfo[1].strName, FEDatabase->OnlineSettings.GetLobbyPersona());
    } else {
        BuildUserInfo(userInfo[1], *LobbyUsers::Instance().GetMyUserRecord());
    }
    ConnApiControl(connapi, 'type', connectionType, 0, nullptr);
    ConnApiConnect(connapi, userInfo, 2, sessionID);
    networkMutex.Unlock("ConnectionCore::JoinSession");
}

void ConnectionCore::JoinSession(LobbyApiPlayerT &hostInfo, int sessionID, void *strSess,
                                 int connectionType) {
    ConnApiUserInfoT userInfo[2];

    networkMutex.Lock("ConnectionCore::JoinSession");
    MaybeGoOnline();
    BuildUserInfo(userInfo[0], hostInfo);
    LobbyApiUserT *myUser = LobbyUsers::Instance().GetMyUserRecord();
    BuildUserInfo(userInfo[1], *myUser);
    ConnApiControl(connapi, 'type', connectionType, 0, nullptr);
    ConnApiControl(connapi, 'sess', 0, 0, strSess);
    ConnApiConnect(connapi, userInfo, 2, sessionID);
    networkMutex.Unlock("ConnectionCore::JoinSession");
}

void ConnectionCore::LeaveSession() {
    networkMutex.Lock("ConnectionCore::LeaveSession");
    ResetSession_HaveMutex();
    networkMutex.Unlock("ConnectionCore::LeaveSession");
}

void ConnectionCore::AddPlayer(ConnApiUserInfoT &userInfo) {
    networkMutex.Lock("ConnectionCore::AddPlayer");
    if (bStrCmp(userInfo.strName, FEDatabase->OnlineSettings.GetLobbyPersona()) != 0) {
        ConnApiAddClient(connapi, &userInfo);
    }
    networkMutex.Unlock("ConnectionCore::AddPlayer");
}

void ConnectionCore::AddPlayer(LobbyApiUserT &userInfo) {
    ConnApiUserInfoT realUserInfo;

    networkMutex.Lock("ConnectionCore::AddPlayer");
    BuildUserInfo(realUserInfo, userInfo);
    if (bStrCmp(realUserInfo.strName, FEDatabase->OnlineSettings.GetLobbyPersona()) != 0) {
        ConnApiAddClient(connapi, &realUserInfo);
    }
    networkMutex.Unlock("ConnectionCore::AddPlayer");
}

void ConnectionCore::AddPlayer(LobbyApiPlayerT &userInfo) {
    networkMutex.Lock("ConnectionCore::AddPlayer");
    AddPlayer_HaveMutex(userInfo);
    networkMutex.Unlock("ConnectionCore::AddPlayer");
}

void ConnectionCore::UpdatePlayers(const LobbyApiPlayT &game) {
    bool madeChanges;

    networkMutex.Lock("ConnectionCore::UpdatePlayers");
    ConnApiClientListT *clientList = ConnApiGetClientList(connapi);
    if (!clientList) {
        networkMutex.Unlock("ConnectionCore::UpdatePlayers");
        return;
    }
    if (game.iIdent < 0 || game.iCount == 0) {
        ResetSession_HaveMutex();
        networkMutex.Unlock("ConnectionCore::UpdatePlayers");
        return;
    }

    madeChanges = false;
    for (int i = 0; i < clientList->iNumClients; i++) {
        bool foundPlayer = false;
        for (int j = 0; j < game.iCount; j++) {
            if (bStrCmp(clientList->Clients[i].UserInfo.strName, game.aOpponents[j].strPers) == 0) {
                foundPlayer = true;
                break;
            }
        }
        if (!foundPlayer) {
            madeChanges = true;
            VoiceCore::mInstance->RemovePlayer(clientList->Clients[i].UserInfo.strName);
            ConnApiRemoveClient(connapi, nullptr, i--);
            NetConnIdle();
            clientList = ConnApiGetClientList(connapi);
        }
    }

    for (int i = 0; i < game.iCount; i++) {
        bool addPlayer = true;
        for (int j = 0; j < clientList->iNumClients; j++) {
            if (bStrCmp(clientList->Clients[j].UserInfo.strName, game.aOpponents[i].strPers) == 0) {
                addPlayer = false;
                break;
            }
        }
        if (addPlayer) {
            madeChanges = true;
            AddPlayer_HaveMutex(game.aOpponents[i]);
        }
    }

    if (madeChanges == true) {
        UpdateNumConnectedPlayers();
    }
    networkMutex.Unlock("ConnectionCore::UpdatePlayers");
}

inline void ConnectionCore::RemovePlayer(char *playerName) {
    networkMutex.Lock("ConnectionCore::RemovePlayer");
    ConnApiClientListT *clientList = ConnApiGetClientList(connapi);
    if (clientList && bStrCmp(playerName, FEDatabase->OnlineSettings.GetLobbyPersona()) != 0) {
        for (int i = 0; i < clientList->iNumClients; i++) {
            if (bStrCmp(clientList->Clients[i].UserInfo.strName, playerName) == 0) {
                VoiceCore::mInstance->RemovePlayer(clientList->Clients[i].UserInfo.strName);
                ConnApiRemoveClient(connapi, nullptr, i);
                UpdateNumConnectedPlayers();
                break;
            }
        }
    }
    networkMutex.Unlock("ConnectionCore::RemovePlayer");
}

inline void ConnectionCore::RemovePlayer(int index) {
    networkMutex.Lock("ConnectionCore::RemovePlayer");
    ConnApiClientListT *clientList = ConnApiGetClientList(connapi);
    if (clientList && index >= 0 && index < clientList->iNumClients &&
        bStrCmp(clientList->Clients[index].UserInfo.strName,
                FEDatabase->OnlineSettings.GetLobbyPersona()) != 0) {
        VoiceCore::mInstance->RemovePlayer(clientList->Clients[index].UserInfo.strName);
        ConnApiRemoveClient(connapi, nullptr, index);
        UpdateNumConnectedPlayers();
    }
    networkMutex.Unlock("ConnectionCore::RemovePlayer");
}

int ConnectionCore::GetNumPlayers() {
    int tmp;

    networkMutex.Lock("ConnectionCore::GetNumPlayers");
    ConnApiClientListT *clientList = ConnApiGetClientList(connapi);
    if (!clientList) {
        networkMutex.Unlock("ConnectionCore::GetNumPlayers");
        tmp = 0;
    } else {
        tmp = clientList->iNumClients;
        networkMutex.Unlock("ConnectionCore::GetNumPlayers");
    }
    return tmp;
}

int ConnectionCore::GetNumConnectedPlayers() { return numConnectedPlayers; }
