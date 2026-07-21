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

LobbyAccount &LobbyAccount::Instance() {
    static LobbyAccount theLobbyAccount;
    return theLobbyAccount;
}

int32 LobbyAccount::CreateAccount(const LobbyAccountT &accountData, CommandCBFunc func,
                                  void *context) {
    lobbyMutex.Lock("LobbyAccount::CreateAccount");
    if (bStrLen(accountData.name) < 4 || bStrLen(accountData.name) > 16 ||
        bStrLen(accountData.password) < 4 || bStrLen(accountData.password) > 16 ||
        bStrLen(accountData.email) < 7 || bStrLen(accountData.email) > 50 ||
        static_cast<unsigned int>(accountData.birthDay - 1) >= 31 ||
        static_cast<unsigned int>(accountData.birthMonth - 1) >= 12 ||
        (accountData.parentEmail[0] &&
         (bStrLen(accountData.parentEmail) < 7 || bStrLen(accountData.parentEmail) > 50))) {
        lobbyMutex.Unlock("LobbyAccount::CreateAccount");
        return -1;
    }

    char dob[16] = "";
    bSPrintf(dob, "%04d%02d%02d", accountData.birthYear, accountData.birthMonth,
             accountData.birthDay);
    char gender[2] = "";
    bSPrintf(gender, "%c", accountData.male ? 'M' : 'F');
    char spam[3] = "";
    bSPrintf(spam, "%c%c", accountData.spamFromEA ? 'Y' : 'N',
             accountData.spamFromAll ? 'Y' : 'N');
    char buf[768] = "";
    TagFieldSetString(buf, sizeof(buf), "NAME", accountData.name);
    TagFieldSetString(buf, sizeof(buf), "PASS", accountData.password);
    TagFieldSetString(buf, sizeof(buf), "MAIL", accountData.email);
    TagFieldSetString(buf, sizeof(buf), "BORN", dob);
    TagFieldSetString(buf, sizeof(buf), "GEND", gender);
    TagFieldSetString(buf, sizeof(buf), "SPAM", spam);
    if (ISOCodes::GetCountryISOCode()) {
        TagFieldSetString(buf, sizeof(buf), "FROM", ISOCodes::GetCountryISOCode());
    }
    if (ISOCodes::GetLanguageISOCode()) {
        TagFieldSetString(buf, sizeof(buf), "LANG", ISOCodes::GetLanguageISOCode());
    }
    if (accountData.parentEmail[0]) {
        TagFieldSetString(buf, sizeof(buf), "PMAIL", accountData.parentEmail);
    }
    TagFieldSetNumber(buf, sizeof(buf), "TOS", 1);
    int32 rc = LobbyCore::Instance().QueueCommand('acct', buf, LobbyCore::DefaultCB, nullptr, func,
                                                   context, false);
    lobbyMutex.Unlock("LobbyAccount::CreateAccount");
    return rc;
}

int32 LobbyAccount::RequestLostUsername(const char *email, CommandCBFunc func, void *context) {
    lobbyMutex.Lock("LobbyAccount::RequestLostUsername");
    if (!email || !email[0]) {
        lobbyMutex.Unlock("LobbyAccount::RequestLostUsername");
        return -1;
    }

    char buf[256] = "";
    TagFieldSetString(buf, sizeof(buf), "MAIL", email);
    if (ISOCodes::GetCountryISOCode()) {
        TagFieldSetString(buf, sizeof(buf), "FROM", ISOCodes::GetCountryISOCode());
    }
    if (ISOCodes::GetLanguageISOCode()) {
        TagFieldSetString(buf, sizeof(buf), "LANG", ISOCodes::GetLanguageISOCode());
    }
    int32 rc = LobbyCore::Instance().QueueCommand('lost', buf, LobbyCore::DefaultCB, nullptr,
                                                   func, context, false);
    lobbyMutex.Unlock("LobbyAccount::RequestLostUsername");
    return rc;
}
