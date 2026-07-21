#include "BuddyCore.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Online/LobbyCore.hpp"
#include "Speed/Indep/Src/Online/VoiceCore.hpp"

extern "C" {
void *NetAlloc(int size);
void NetFree(void *memory);
HLBApiRefT *HLBApiCreate2(void *(*alloc)(int), void (*free)(void *), const char *product, const char *version);
void HLBApiSetDebugFunction(HLBApiRefT *api, void *context, void (*callback)(void *, const char *));
int HLBApiInitialize(HLBApiRefT *api, const char *serverPort, int product, const char *language);
int HLBApiConnect(HLBApiRefT *api, const char *server, int port, const char *password, const char *context);
void HLBMsgListDeleteAll(HLBBudT *buddy, int flags);
int HLBBudIsTemporary(HLBBudT *buddy);
unsigned int HLBBudGetGameInviteFlags(HLBBudT *buddy);
int HLBBudIsRealBuddy(HLBBudT *buddy);
int HLBBudIsWannaBeMyBuddy(HLBBudT *buddy);
int HLBBudIsIWannaBeHisBuddy(HLBBudT *buddy);
int HLBBudIsBlocked(HLBBudT *buddy);
int HLBBudGetState(HLBBudT *buddy);
const char *HLBBudGetName(HLBBudT *buddy);
void HLBApiCancelOp(HLBApiRefT *api);
int HLBApiRegisterBuddyChangeCallback(HLBApiRefT *api, void (*callback)(HLBApiRefT *, int, int, void *), void *context);
int HLBApiRegisterConnectCallback(HLBApiRefT *api, void (*callback)(HLBApiRefT *, int, int, void *), void *context);
int HLBApiRegisterGameInviteCallback(HLBApiRefT *api, void (*callback)(HLBApiRefT *, HLBBudT *, int, void *), void *context);
int HLBApiRegisterNewMsgCallback(HLBApiRefT *api, void (*callback)(HLBApiRefT *, BuddyApiMsgT *, void *), void *context);
void LobbyChalDeclineChallenge(LobbyChalRefT *challenge, int reason);
void LobbyChalDestroy(LobbyChalRefT *challenge);
void LobbyChalBlockChallenge(LobbyChalRefT *challenge);
void HLBApiSuspend(HLBApiRefT *api);
void HLBApiResume(HLBApiRefT *api);
LobbyChalRefT *LobbyChalCreate(LobbyApiRefT *api, const char *persona,
                               void (*callback)(LobbyChalRefT *, LobbyApiMsgT *, void *), void *context, int timeout);
void HLBListFlagTempBuddy(HLBApiRefT *api, const char *name, int flags);
void HLBListAnswerGameInvite(HLBApiRefT *api, const char *name, int answer, int gameId, int flags);
HLBBudT *HLBListGetBuddyByIndex(HLBApiRefT *api, int index);
HLBBudT *HLBListGetBuddyByName(HLBApiRefT *api, const char *name);
int HLBListGetBuddyCount(HLBApiRefT *api);
void HLBApiDisconnect(HLBApiRefT *api);
void HLBApiDestroy(HLBApiRefT *api);
void HLBApiPresenceVOIPSend(HLBApiRefT *api, int status);
void HLBApiUpdate(HLBApiRefT *api);
void *LobbyApiInfoPtr(LobbyApiRefT *api, int selector);
char *TagFieldFind(const char *record, const char *name);
int TagFieldGetNumber(const char *field, int defaultValue);
int TagFieldGetString(const char *field, char *buffer, int bufferSize, const char *defaultValue);
}

MenuScreen *FEngFindScreen(const char *packageName);

extern int gVOIP_InviteState;
extern int gBuddyListHasChanged;
extern BuddySettings gBuddySettings;
extern int gChallengeRecieved;

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

void BuddyCore::doprocessing() {
    if (HLBud) {
        if (m_networkCableUnplugged && NetConnStatus('plug', 0, nullptr) != 0) {
            m_networkCableUnplugged = false;
        }
        int voipStatus = 0;
        if (VoiceCore::mInstance->IsHeadsetConnected()) {
            voipStatus = 1;
        }
        if (m_lastVOIPstate != voipStatus) {
            HLBApiPresenceVOIPSend(HLBud, voipStatus);
            m_lastVOIPstate = voipStatus;
            gBuddyListHasChanged = 1;
        }
        if (m_InVoiceChat && !VoiceCore::mInstance->IsConnected(m_buddyVoiceChatChannel)) {
            m_InVoiceChat = false;
            if (!FEDatabase->OnlineSettings.inErrorState) {
                ConnectionCore::Instance().LeaveSession();
                DisplayVOIPChatEnded();
                gBuddyListHasChanged = 1;
            }
        }
        HLBApiUpdate(HLBud);
    }
}

void BuddyCore::handledisconnect() {
    if (HLBud) {
        state = disconnected;
        HLBApiCancelOp(HLBud);
    }
}

void BuddyCore::debugoutput(const char *str) {
    char time_buffer[16];
    bSPrintf(time_buffer, "%-6d: ", RealLoopCounter);
}

void BuddyCore::connectchanged(int op, int status) {
    if (connectcallback) {
        connectcallback(status, connectcontext);
        if (status != 2) {
            connectcallback = nullptr;
        }
    }
    if (op == 1 && status == 0) {
        handledisconnect();
    }
    gBuddyListHasChanged = 1;
}

void BuddyCore::gameinvite(HLBApiRefT *api, HLBBudT *bud, int action, void *context) {
    BuddyCore *pBuddyCore = static_cast<BuddyCore *>(context);
    int icon = 0;
    bool status = false;

    switch (action) {
    case 0x400:
        pBuddyCore->DisplayDeclinedInvite(HLBBudGetName(bud));
        return;
    case 0x4000000:
        pBuddyCore->DisplayTimedOutInvite(HLBBudGetName(bud));
    case 0x800:
        icon = 1;
        status = false;
        if (bStrCmp(gBuddySettings.buddyName, HLBBudGetName(bud)) == 0 &&
            FEngFindScreen("OL_FriendDialogue.fng")) {
            cFEng::Get()->QueuePackageMessage(0x1cad26e2, "OL_FriendDialogue.fng", nullptr);
        }
        if (LobbyChat::Instance().GetNumReceivedInvites()) {
            gBuddyListHasChanged = 1;
            return;
        }
        break;
    case 0x400000:
        icon = 1;
        status = true;
        break;
    }
    MenuScreen::UpdateStatusIcons(icon, status);
    gBuddyListHasChanged = 1;
}

BuddyCore *BuddyCore::instance() {
    static BuddyCore the;
    return &the;
}

HLBBudT *BuddyCore::getBuddyByIndex(int index) { return HLBListGetBuddyByIndex(HLBud, index); }

HLBBudT *BuddyCore::getBuddyByName(const char *name) { return HLBListGetBuddyByName(HLBud, name); }

void BuddyCore::buddyListChangedCallback(int op, int opStatus) {
    int i;
    int buddycount;
    bool budRequestRec = false;
    Invite *thisInvite;
    HLBBudT *thisBuddy;

    buddycount = getBuddyCount();
    for (i = 0; i < buddycount; i++) {
        HLBBudT *pBud = getBuddyByIndex(i);
        if (pBud && !HLBBudIsRealBuddy(pBud) && HLBBudIsWannaBeMyBuddy(pBud) && !HLBBudIsBlocked(pBud)) {
            budRequestRec = true;
            HLBListFlagTempBuddy(HLBud, HLBBudGetName(pBud), 1);
        }
    }
    for (i = 0; i < 10; i++) {
        thisInvite = LobbyChat::Instance().GetSentInvite(i);
        if (!thisInvite) {
            break;
        }
        thisBuddy = getBuddyByName(thisInvite->player);
        if (thisBuddy && HLBBudGetState(thisBuddy) == 0) {
            DisplayDeclinedInvite(thisInvite->player);
            LobbyChat::Instance().CancelInvite(thisInvite->player);
        }
    }
    if (budRequestRec) {
        g_pEAXSound->PlayUISoundFX(UISND_EA_MSGR_MAIL_RECEIVE);
    }
    MenuScreen::UpdateStatusIcons(3, budRequestRec);
    gBuddyListHasChanged = 1;
}

void BuddyCore::messageCallback(BuddyApiMsgT *pMsg) {
    int icon = 0;
    bool status = false;

    if (pMsg->iType != 'chat') {
        if (pMsg->iType == 'invt') {
            icon = 3;
            status = true;
        }
    } else {
        icon = 0;
        status = true;
        g_pEAXSound->PlayUISoundFX(UISND_EA_MSGR_MAIL_RECEIVE);
    }
    char sender[32] = "";
    HLBBudT *pBud;
    TagFieldGetString(TagFieldFind(pMsg->pData, "USER"), sender, sizeof(sender), "unknown sender");
    pBud = getBuddyByName(sender);
    if (pBud && HLBBudIsBlocked(pBud)) {
        HLBMsgListDeleteAll(pBud, 0);
    } else {
        MenuScreen::UpdateStatusIcons(icon, status);
        gBuddyListHasChanged = 1;
    }
}

void BuddyCore::LobbyChallengeCB(LobbyApiMsgT *pMesg) {
    char buf[512] = "";

    switch (pMesg->kind) {
    case 'chal': {
        char gameName[128] = "";
        TagFieldGetString(TagFieldFind(pMesg->pData, "PARM"), buf, sizeof(buf), "none");
        TagFieldGetString(TagFieldFind(buf, "GAMENAME"), gameName, sizeof(gameName), "voip");
        if (bStrCmp(gameName, "voip") == 0) {
            int slot = GetNextAvailableVOIPSlot();
            TagFieldGetString(TagFieldFind(pMesg->pData, "NAME"), gameName, sizeof(gameName), "unknown");
            AddTempBuddy(gameName, 2);
            HLBBudT *pBud = getBuddyByName(gameName);
            if (HLBBudIsBlocked(pBud)) {
                LobbyChalBlockChallenge(instance()->GetLobbyChallengeApiRef());
                return;
            }
            TagFieldGetString(TagFieldFind(pMesg->pData, "NAME"), m_voipChallenge[slot].challengerName,
                              sizeof(m_voipChallenge[slot].challengerName), "unknown challenger");
            TagFieldGetString(TagFieldFind(buf, "GAMENAME"), m_voipChallenge[slot].gameRoomName,
                              sizeof(m_voipChallenge[slot].gameRoomName), "unknown game");
            bStrCpy(m_voipChallenge[slot].hostName, buf);
            DirtyAddrT extaddr;
            DirtyAddrT intaddr;
            extaddr.strMachineAddr[0] = '\0';
            intaddr.strMachineAddr[0] = '\0';
            TagFieldGetString(TagFieldFind(buf, "EXTHOSTNAME"), extaddr.strMachineAddr,
                              sizeof(extaddr.strMachineAddr), "");
            TagFieldGetString(TagFieldFind(buf, "INTHOSTNAME"), intaddr.strMachineAddr,
                              sizeof(intaddr.strMachineAddr), "");
            bStrCpy(m_voipChallenge[slot].hostName, buf);
            MenuScreen::UpdateStatusIcons(2, true);
            g_pEAXSound->PlayUISoundFX(UISND_EA_MSGR_CHAT_REQ);
            HLBListFlagTempBuddy(HLBud, m_voipChallenge[slot].challengerName, 8);
            gBuddyListHasChanged = 1;
            gVOIP_InviteState = 2;
        }
        return;
    }
    case 'decl':
        TagFieldGetString(TagFieldFind(pMesg->pData, "NAME"), buf, sizeof(buf), "unknown");
        if (gVOIP_InviteState == 1) {
            cFEng::Get()->QueueGameMessage(0x39992a2c, "OL_VoiceChat.fng", 0xff);
        }
        gBuddyListHasChanged = 1;
        return;
    case 'acpt':
        TagFieldGetString(TagFieldFind(pMesg->pData, "NAME"), buf, sizeof(buf), "unknown");
        if (gVOIP_InviteState == 1) {
            cFEng::Get()->QueueGameMessage(0x7c1bb77f, "OL_VoiceChat.fng", 0xff);
        }
        gBuddyListHasChanged = 1;
        return;
    case 'cncl':
        TagFieldGetString(TagFieldFind(pMesg->pData, "NAME"), buf, sizeof(buf), "unknown");
        if (DoIHaveAVOIPInviteFromThisBuddy(pMesg->pData)) {
            RemoveVOIPChallenge(pMesg->pData);
            cFEng::Get()->QueueGameMessage(0x519201f5, nullptr, 0xff);
            gVOIP_InviteState = 0;
        }
        gBuddyListHasChanged = 1;
        MenuScreen::UpdateStatusIcons(2, false);
        return;
    case 'time':
        TagFieldGetString(TagFieldFind(pMesg->pData, "NAME"), buf, sizeof(buf), "unknown");
        gVOIP_InviteState = 0;
        gBuddyListHasChanged = 1;
        gChallengeRecieved = 0;
        return;
    case 'blck':
    case 'busy':
        TagFieldGetString(TagFieldFind(pMesg->pData, "NAME"), buf, sizeof(buf), "unknown");
        gVOIP_InviteState = 0;
        gBuddyListHasChanged = 1;
        return;
    case 'mesg':
    case 'play':
        TagFieldGetString(TagFieldFind(pMesg->pData, "NAME"), buf, sizeof(buf), "unknown");
        gBuddyListHasChanged = 1;
        return;
    }
}

LobbyChalRefT *BuddyCore::GetLobbyChallengeApiRef() { return m_lobbyChalRefT; }

int BuddyCore::GetNextAvailableVOIPSlot() {
    for (int i = 0; i < 6; i++) {
        if (!m_voipChallenge[i].isActive) {
            return i;
        }
    }
    return -1;
}

bool BuddyCore::DoIHaveAVOIPInviteFromThisBuddy(char *buddyName) {
    bool bHaveChallenge = false;
    for (int i = 0; i < 6; i++) {
        if (bStrCmp(buddyName, m_voipChallenge[i].challengerName) == 0) {
            bHaveChallenge = true;
        }
    }
    return bHaveChallenge;
}

VOIP_ChallengeInfo *BuddyCore::GetVOIPChallengeInfo(char *buddyName) {
    for (int i = 0; i < 6; i++) {
        if (bStrCmp(buddyName, m_voipChallenge[i].challengerName) == 0) {
            return &m_voipChallenge[i];
        }
    }
    return nullptr;
}

void BuddyCore::RemoveVOIPChallenge(char *buddyName) {
    for (int i = 0; i < 6; i++) {
        if (bStrCmp(buddyName, m_voipChallenge[i].challengerName) == 0) {
            bMemSet(&m_voipChallenge[i], 0, sizeof(VOIP_ChallengeInfo));
        }
    }
}

void BuddyCore::CountBuddys() {
    m_realBuddys = 0;
    m_tempBuddys = 0;
    m_blockedBuddys = 0;
    m_friendRequestBuddys = 0;
    for (int i = 0; i < HLBListGetBuddyCount(HLBud); i++) {
        HLBBudT *pBud = getBuddyByIndex(i);
        if (HLBBudIsRealBuddy(pBud)) {
            m_realBuddys++;
        }
        if (HLBBudIsBlocked(pBud)) {
            m_blockedBuddys++;
        }
        if (HLBBudIsTemporary(pBud)) {
            m_tempBuddys++;
        }
        if (HLBBudIsWannaBeMyBuddy(pBud) || HLBBudIsIWannaBeHisBuddy(pBud)) {
            m_friendRequestBuddys++;
        }
    }
}

int BuddyCore::GetNumRealBuddys() {
    CountBuddys();
    return m_realBuddys;
}

int BuddyCore::GetNumberTempBuddys() {
    CountBuddys();
    return m_tempBuddys;
}

inline int BuddyCore::GetNumBlockedBuddys() {
    CountBuddys();
    return m_blockedBuddys;
}

int BuddyCore::GetNumFriendRequests() {
    CountBuddys();
    return m_friendRequestBuddys;
}
