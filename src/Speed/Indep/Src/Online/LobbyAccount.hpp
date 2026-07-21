#ifndef ONLINE_LOBBY_ACCOUNT_HPP
#define ONLINE_LOBBY_ACCOUNT_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct DispListRef;
struct LobbyApiRoomT;

typedef void (*RoomUpdateCBFunc)();

struct CreateRoomParamsT {
    char name[36];
    bool failIfExists;
    char password[20];
    unsigned int capacity;
    unsigned int flags;

    CreateRoomParamsT(const char *pName, bool shouldFailIfExists, const char *pPassword,
                      unsigned int roomCapacity, unsigned int roomFlags)
        : failIfExists(shouldFailIfExists) //
        , capacity(roomCapacity) //
        , flags(roomFlags) {
        bStrNCpy(name, pName, 35);
        bStrNCpy(password, pPassword, 19);
    }
};

struct LobbyRooms {
    static LobbyRooms &Instance();

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

#endif
