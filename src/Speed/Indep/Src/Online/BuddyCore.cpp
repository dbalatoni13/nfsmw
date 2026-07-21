#include "BuddyCore.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Online/LobbyCore.hpp"

extern "C" {
int HLBBudIsTemporary(HLBBudT *buddy);
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
}

extern int gVOIP_InviteState;

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
