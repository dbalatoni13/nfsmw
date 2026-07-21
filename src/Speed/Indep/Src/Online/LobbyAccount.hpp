#ifndef ONLINE_LOBBY_ACCOUNT_HPP
#define ONLINE_LOBBY_ACCOUNT_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "LobbyCore.hpp"

struct DispListRef;
struct LobbyApiRoomT;

typedef void (*RoomUpdateCBFunc)(void *context, LobbyApiMsgT *msg);

struct LobbyAccountT {
    char name[20];
    char password[20];
    char email[52];
    int birthDay;
    int birthMonth;
    int birthYear;
    bool male;
    bool spamFromEA;
    bool spamFromAll;
    char parentEmail[52];
};

extern "C" {
void TagFieldSetString(char *record, int recordLength, const char *name, const char *value);
void TagFieldSetNumber(char *record, int recordLength, const char *name, int value);
void TagFieldSetFlags(char *record, int recordLength, const char *name, int value);
char *TagFieldFind(const char *record, const char *name);
int TagFieldGetString(const char *field, char *buffer, int bufferSize, const char *defaultValue);
int DispListCount(DispListRef *list);
void *DispListIndex(DispListRef *list, int index);
}

struct CreateRoomParamsT {
    char name[36];
    bool failIfExists;
    char password[20];
    unsigned int capacity;
    unsigned int flags;

    CreateRoomParamsT(char *pName, bool shouldFailIfExists, char *pPassword,
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
    void SetRoomUpdateCB(RoomUpdateCBFunc func, void *context) {
        lobbyMutex.Lock("LobbyRooms::SetRoomUpdateCB");
        roomUpdateCB = func;
        roomUpdateContext = context;
        lobbyMutex.Unlock("LobbyRooms::SetRoomUpdateCB");
    }
    int32 CreateRoom(CreateRoomParamsT &roomParams, CommandCBFunc createRoomCB, void *context) {
        int32 rc = -1;
        if (roomParams.name[0] && !currentRoomName[0] &&
            LobbyCore::Instance().FindCommandID('rent', nullptr, nullptr, nullptr, nullptr) == -1 &&
            LobbyCore::Instance().FindCommandID('rcre', nullptr, nullptr, nullptr, nullptr) == -1) {
            char buf[128] = "";
            TagFieldSetString(buf, sizeof(buf), "NAME", roomParams.name);
            TagFieldSetNumber(buf, sizeof(buf), "FAIL", roomParams.failIfExists);
            if (roomParams.password[0]) {
                TagFieldSetString(buf, sizeof(buf), "PASS", roomParams.password);
            }
            TagFieldSetNumber(buf, sizeof(buf), "SIZE", roomParams.capacity);
            TagFieldSetFlags(buf, sizeof(buf), "FLAGS", roomParams.flags);
            rc = LobbyCore::Instance().QueueCommand('rcre', buf, EnterRoomCB, this, createRoomCB,
                                                     context, false);
        }
        return rc;
    }
    int32 EnterRoom(char *name, char *password, CommandCBFunc enterRoomCB, void *context) {
        int32 rc = -1;
        if (name && name[0] && !currentRoomName[0] &&
            LobbyCore::Instance().FindCommandID('rent', nullptr, nullptr, nullptr, nullptr) == -1 &&
            LobbyCore::Instance().FindCommandID('rcre', nullptr, nullptr, nullptr, nullptr) == -1) {
            char buf[96] = "";
            TagFieldSetString(buf, sizeof(buf), "NAME", name);
            if (password && password[0]) {
                TagFieldSetString(buf, sizeof(buf), "PASS", password);
            }
            rc = LobbyCore::Instance().QueueCommand('rent', buf, EnterRoomCB, this, enterRoomCB,
                                                     context, false);
        }
        return rc;
    }
    int GetNumRooms() {
        int rc = 0;
        if (roomList) {
            rc = DispListCount(roomList);
        }
        return rc;
    }
    LobbyApiRoomT *GetRoom(int index) {
        LobbyApiRoomT *rc = nullptr;
        if (roomList && index >= 0 && index < DispListCount(roomList)) {
            rc = static_cast<LobbyApiRoomT *>(DispListIndex(roomList, index));
        }
        return rc;
    }
    char *MyRoomName() {
        char *rc = nullptr;
        if (currentRoomName[0]) {
            rc = currentRoomName;
        }
        return rc;
    }
    static void EnterRoomCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData) {
        LobbyRooms *lobbyRooms = static_cast<LobbyRooms *>(pData);
        if (pMsg->code == 0) {
            TagFieldGetString(TagFieldFind(pMsg->pData, "NAME"), lobbyRooms->currentRoomName,
                              sizeof(lobbyRooms->currentRoomName), "");
        }
        LobbyCore::Instance().FinishCommand(pMsg, true);
    }

  private:
    LobbyRooms();
    ~LobbyRooms();
    int32 Init();
    void Reset();
    static void RoomsDispListCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();
    friend class LobbyCore;

    DispListRef *roomList;
    RoomUpdateCBFunc roomUpdateCB;
    void *roomUpdateContext;
    char currentRoomName[36];
};

struct LobbyAccount {
    static LobbyAccount &Instance();
    int32 CreateAccount(const LobbyAccountT &accountData, CommandCBFunc func, void *context);
    int32 RequestLostUsername(const char *email, CommandCBFunc func, void *context);
    int32 RequestLostPassword(const char *username, CommandCBFunc func, void *context);
    int32 CreatePersona(const char *name, CommandCBFunc func, void *context);

  private:
    LobbyAccount() { pendingPersona[0] = '\0'; }
    ~LobbyAccount() {}
    int32 Init();
    void Reset();
    static void CperCB(LobbyApiRefT *pRef, LobbyApiMsgT *pMsg, void *pData);
    friend int32 LobbyInit();
    friend void LobbyDisconnect();

    char pendingPersona[16];
};

#endif
