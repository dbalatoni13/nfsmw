#include "LobbyCore.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

extern "C" {
ConnApiRefT *ConnApiCreate(const char *sessionName, int gamePort, int maxClients,
                           ConnApiCallbackT *callback, void *userData);
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
