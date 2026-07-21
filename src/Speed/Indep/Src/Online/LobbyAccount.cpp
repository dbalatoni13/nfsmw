#include "LobbyCore.hpp"

LobbyRooms::LobbyRooms()
    : roomList(nullptr) //
    , roomUpdateCB(nullptr) //
    , roomUpdateContext(nullptr) {
    currentRoomName[0] = '\0';
}

LobbyRooms::~LobbyRooms() { Reset(); }
