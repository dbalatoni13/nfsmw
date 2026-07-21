#include "LobbyCore.hpp"

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
