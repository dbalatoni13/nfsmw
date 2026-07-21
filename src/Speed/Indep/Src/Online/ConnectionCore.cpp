#include "LobbyCore.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

extern "C" {
ConnApiRefT *ConnApiCreate(const char *sessionName, int gamePort, int maxClients,
                           ConnApiCallbackT *callback, void *userData);
void ConnApiDestroy(ConnApiRefT *connapi);
int ConnApiHost(ConnApiRefT *connapi, ConnApiUserInfoT *userInfo, int numClients, int sessionID);
int ConnApiConnect(ConnApiRefT *connapi, ConnApiUserInfoT *userInfo, int numClients, int sessionID);
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
