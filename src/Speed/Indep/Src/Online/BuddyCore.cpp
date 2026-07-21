#include "BuddyCore.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern "C" {
int HLBBudIsTemporary(HLBBudT *buddy);
const char *HLBBudGetName(HLBBudT *buddy);
void HLBApiCancelOp(HLBApiRefT *api);
void HLBApiRegisterBuddyChangeCallback(HLBApiRefT *api, void *callback, void *context);
void HLBApiRegisterConnectCallback(HLBApiRefT *api, void *callback, void *context);
void HLBApiRegisterGameInviteCallback(HLBApiRefT *api, void *callback, void *context);
void HLBApiRegisterNewMsgCallback(HLBApiRefT *api, void *callback, void *context);
void LobbyChalDeclineChallenge(LobbyChalRefT *challenge, int reason);
void LobbyChalDestroy(LobbyChalRefT *challenge);
void HLBApiSuspend(HLBApiRefT *api);
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
