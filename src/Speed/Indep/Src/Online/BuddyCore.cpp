#include "BuddyCore.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"

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
