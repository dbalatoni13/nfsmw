#include "LobbyCore.hpp"
#include "LobbyAccount.hpp"

LobbyRooms::LobbyRooms()
    : roomList(nullptr) //
    , roomUpdateCB(nullptr) //
    , roomUpdateContext(nullptr) {
    currentRoomName[0] = '\0';
}

LobbyRooms::~LobbyRooms() { Reset(); }

LobbyRooms &LobbyRooms::Instance() {
    static LobbyRooms theLobbyRooms;
    return theLobbyRooms;
}

int32 LobbyRooms::Init() {
    if (!roomList) {
        if (!LobbyCore::Instance().pLobbyRef) {
            return -1;
        }
        roomList = LobbyApiListAlloc(LobbyCore::Instance().pLobbyRef,
                                     static_cast<LobbyRanks::RankListMapping>(0), RoomsDispListCB,
                                     this);
        if (!roomList) {
            Reset();
            return -1;
        }
    }
    return 0;
}

void LobbyRooms::Reset() {
    if (!LobbyCore::Instance().pLobbyRef) {
        currentRoomName[0] = '\0';
    } else if (!roomList) {
        currentRoomName[0] = '\0';
    } else {
        LobbyApiListFree(LobbyCore::Instance().pLobbyRef, 0, roomList);
        roomList = nullptr;
        currentRoomName[0] = '\0';
    }
    roomUpdateContext = nullptr;
    roomUpdateCB = nullptr;
}

void LobbyRooms::RoomsDispListCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
    LobbyRooms *lobbyRooms = static_cast<LobbyRooms *>(pData);
    if (lobbyRooms->roomUpdateCB) {
        lobbyRooms->roomUpdateCB(lobbyRooms->roomUpdateContext, pMsg);
    }
}
