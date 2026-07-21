#include "BuddyCore.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Online/LobbyCore.hpp"

extern "C" {
void *NetAlloc(int size);
void NetFree(void *memory);
HLBApiRefT *HLBApiCreate2(void *(*alloc)(int), void (*free)(void *), const char *product, const char *version);
void HLBApiSetDebugFunction(HLBApiRefT *api, void *context, void (*callback)(void *, const char *));
int HLBApiInitialize(HLBApiRefT *api, const char *serverPort, int product, const char *language);
int HLBApiConnect(HLBApiRefT *api, const char *server, int port, const char *password, const char *context);
int HLBBudIsTemporary(HLBBudT *buddy);
unsigned int HLBBudGetGameInviteFlags(HLBBudT *buddy);
const char *HLBBudGetName(HLBBudT *buddy);
void HLBApiCancelOp(HLBApiRefT *api);
int HLBApiRegisterBuddyChangeCallback(HLBApiRefT *api, void (*callback)(HLBApiRefT *, int, int, void *), void *context);
int HLBApiRegisterConnectCallback(HLBApiRefT *api, void (*callback)(HLBApiRefT *, int, int, void *), void *context);
int HLBApiRegisterGameInviteCallback(HLBApiRefT *api, void (*callback)(HLBApiRefT *, HLBBudT *, int, void *), void *context);
int HLBApiRegisterNewMsgCallback(HLBApiRefT *api, void (*callback)(HLBApiRefT *, BuddyApiMsgT *, void *), void *context);
void LobbyChalDeclineChallenge(LobbyChalRefT *challenge, int reason);
void LobbyChalDestroy(LobbyChalRefT *challenge);
void HLBApiSuspend(HLBApiRefT *api);
void HLBApiResume(HLBApiRefT *api);
LobbyChalRefT *LobbyChalCreate(LobbyApiRefT *api, const char *persona,
                               void (*callback)(LobbyChalRefT *, LobbyApiMsgT *, void *), void *context, int timeout);
void HLBListFlagTempBuddy(HLBApiRefT *api, const char *name, int flags);
void HLBListAnswerGameInvite(HLBApiRefT *api, const char *name, int answer, int gameId, int flags);
void HLBApiDisconnect(HLBApiRefT *api);
void HLBApiDestroy(HLBApiRefT *api);
void *LobbyApiInfoPtr(LobbyApiRefT *api, int selector);
char *TagFieldFind(const char *record, const char *name);
int TagFieldGetNumber(const char *field, int defaultValue);
int TagFieldGetString(const char *field, char *buffer, int bufferSize, const char *defaultValue);
}

extern int gVOIP_InviteState;

static char BuddyProductString[9] = "NFS-2006";
char productString[32];
static char BuddyContextString[6] = "/CSO/";

BuddyCore::BuddyCore()
    : HLBud(nullptr) //
    , state(disconnected) //
    , connectcallback(nullptr) //
    , m_lobbyChalRefT(nullptr) //
    , m_lastVOIPstate(-1) //
    , m_InVoiceChat(false) //
    , m_myVoiceChatChannel(0) //
    , m_buddyVoiceChatChannel(0) //
    , m_networkCableUnplugged(false) //
    , m_paused(false) {
    bMemSet(savedNames, 0, sizeof(savedNames));
}

void BuddyCore::pause() {
    if (!m_paused) {
        int buddycount;
        int k = 0;
        clearEAMStatusIcons();
        buddycount = getBuddyCount();
        for (int i = 0; i < buddycount; i++) {
            HLBBudT *pBud = getBuddyByIndex(i);
            if (pBud && HLBBudIsTemporary(pBud) && k < 10) {
                bStrCpy(savedNames[k], HLBBudGetName(pBud));
                k++;
            }
        }
        HLBApiCancelOp(HLBud);
        HLBApiRegisterBuddyChangeCallback(HLBud, nullptr, nullptr);
        HLBApiRegisterConnectCallback(HLBud, nullptr, nullptr);
        HLBApiRegisterGameInviteCallback(HLBud, nullptr, nullptr);
        HLBApiRegisterNewMsgCallback(HLBud, nullptr, nullptr);
        gVOIP_InviteState = 0;
        m_InVoiceChat = false;
        if (m_lobbyChalRefT) {
            LobbyChalDeclineChallenge(m_lobbyChalRefT, 0);
            LobbyChalDestroy(m_lobbyChalRefT);
        }
        m_lobbyChalRefT = nullptr;
        HLBApiSuspend(HLBud);
        m_paused = true;
    }
}

int BuddyCore::resume() {
    int rc;
    HLBApiResume(HLBud);
    if ((rc = HLBApiRegisterBuddyChangeCallback(
             HLBud, buddy_op_callback<&BuddyCore::buddyListChangedCallback>::invoke, this)) > 0 ||
        (rc = HLBApiRegisterConnectCallback(HLBud, buddy_op_callback<&BuddyCore::connectchanged>::invoke, this)) > 0 ||
        (rc = HLBApiRegisterGameInviteCallback(HLBud, gameinvite, this)) > 0 ||
        (rc = HLBApiRegisterNewMsgCallback(
             HLBud, buddy_msg_callback<&BuddyCore::messageCallback>::invoke, this)) > 0) {
        disconnect();
        return rc;
    }

    m_lobbyChalRefT = LobbyChalCreate(
        LobbyCore::Instance().GetLobbyApiRef(), FEDatabase->OnlineSettings.GetLobbyPersona(),
        challenge_op_callback<&BuddyCore::LobbyChallengeCB>::invoke, this, 120000);
    LobbyChat::Instance().SetInviteCallback(InviteCB, instance());
    for (int i = 0; i < 10; i++) {
        if (bStrLen(savedNames[i]) > 0 &&
            bStrCmp(FEDatabase->OnlineSettings.GetLobbyPersona(), savedNames[i]) != 0) {
            HLBListFlagTempBuddy(HLBud, savedNames[i], 0x40);
        }
    }
    bMemSet(savedNames, 0, sizeof(savedNames));
    initVoiceAndPresence();
    m_paused = false;
    return 0;
}

int BuddyCore::startconnect(const char *name, const char *passwd, BuddyConnectCallback callback, void *context) {
    int rc;
    char strKey[256];
    char contextKey[256];
    char strBuddyServer[64];
    char *pConfig;
    int iPort;

    m_paused = false;
    HLBud = HLBApiCreate2(NetAlloc, NetFree, BuddyProductString, "0.1");
    HLBApiSetDebugFunction(HLBud, this, buddy_print_callback<&BuddyCore::debugoutput>::invoke);
    bSPrintf(productString, "%d", FEDatabase->OnlineSettings.GetLobbyServerPort());
    rc = HLBApiInitialize(HLBud, productString, 'nfs6', "en");
    if (rc > 0 ||
        (rc = HLBApiRegisterBuddyChangeCallback(
             HLBud, buddy_op_callback<&BuddyCore::buddyListChangedCallback>::invoke, this)) > 0 ||
        (rc = HLBApiRegisterConnectCallback(HLBud, buddy_op_callback<&BuddyCore::connectchanged>::invoke, this)) > 0 ||
        (rc = HLBApiRegisterGameInviteCallback(HLBud, gameinvite, this)) > 0 ||
        (rc = HLBApiRegisterNewMsgCallback(
             HLBud, buddy_msg_callback<&BuddyCore::messageCallback>::invoke, this)) > 0) {
        disconnect();
        return rc;
    }

    connectcontext = context;
    connectcallback = callback;
    bStrCpy(strKey, passwd);
    bSPrintf(contextKey, "%s%s", BuddyContextString, BuddyProductString);
    strBuddyServer[0] = '\0';
    pConfig = static_cast<char *>(LobbyApiInfoPtr(LobbyCore::Instance().pLobbyRef, 'conf'));
    iPort = TagFieldGetNumber(TagFieldFind(pConfig, "BUDDY_PORT"), 0x34c1);
    TagFieldGetString(TagFieldFind(pConfig, "BUDDY_SERVER"), strBuddyServer, sizeof(strBuddyServer), "");
    rc = HLBApiConnect(HLBud, strBuddyServer, iPort, strKey, contextKey);
    if (rc > 0) {
        disconnect();
        return rc;
    }

    state = connected;
    m_lobbyChalRefT = LobbyChalCreate(
        LobbyCore::Instance().GetLobbyApiRef(), FEDatabase->OnlineSettings.GetLobbyPersona(),
        challenge_op_callback<&BuddyCore::LobbyChallengeCB>::invoke, this, 120000);
    LobbyChat::Instance().SetInviteCallback(InviteCB, instance());
    clearEAMStatusIcons();
    bMemSet(savedNames, 0, sizeof(savedNames));
    return 0;
}

void BuddyCore::disconnect() {
    m_paused = false;
    if (HLBud) {
        m_InVoiceChat = false;
        gVOIP_InviteState = 0;
        clearEAMStatusIcons();
        int buddycount = getBuddyCount();
        for (int i = 0; i < buddycount; i++) {
            HLBBudT *pBud = getBuddyByIndex(i);
            if (pBud && (HLBBudGetGameInviteFlags(pBud) & 0x400000)) {
                HLBListAnswerGameInvite(HLBud, HLBBudGetName(pBud), 1, 0, 0);
            }
        }
        handledisconnect();
        HLBApiRegisterBuddyChangeCallback(HLBud, nullptr, nullptr);
        HLBApiRegisterConnectCallback(HLBud, nullptr, nullptr);
        HLBApiRegisterGameInviteCallback(HLBud, nullptr, nullptr);
        HLBApiDisconnect(HLBud);
        HLBApiDestroy(HLBud);
        gVOIP_InviteState = 0;
        state = disconnected;
        connectcallback = nullptr;
        connectcontext = nullptr;
        HLBud = nullptr;
        m_InVoiceChat = false;
        if (m_lobbyChalRefT) {
            LobbyChalDeclineChallenge(m_lobbyChalRefT, 0);
            LobbyChalDestroy(m_lobbyChalRefT);
        }
        m_lobbyChalRefT = nullptr;
        bMemSet(savedNames, 0, sizeof(savedNames));
    }
}
