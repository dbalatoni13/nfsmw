#ifndef ONLINE_INGAME_ONLINE_PLAYER_HPP
#define ONLINE_INGAME_ONLINE_PLAYER_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct OnlinePlayer {
  private:
    /* 0x00 */ int m_driverNumber;
    /* 0x04 */ int m_clientId;
    /* 0x08 */ uint16 m_oneWayLatencyMs;
    /* 0x0c */ uint32 m_ticker;
    /* 0x10 */ int m_clientState;
    /* 0x14 */ float m_clientTimestamp;
    /* 0x18 */ float m_lastClockSyncRealTimeReceived;

  public:
    OnlinePlayer();
    void Reset();
    void SetDriverNumber(int driver_number) { m_driverNumber = driver_number; }
    int GetDriverNumber() { return m_driverNumber; }
    int GetClientId() { return m_clientId; }
    int GetClientState() { return m_clientState; }
    float GetLastClockSyncRealTimeReceived() { return m_lastClockSyncRealTimeReceived; }
    void SetOneWayLatencyMs(int latency_ms) { m_oneWayLatencyMs = latency_ms; }
    void SetLastClockSyncRealTimeReceived(float value) {
        m_lastClockSyncRealTimeReceived = value;
    }
    void SetClientState(int new_state);
};

OnlinePlayer::OnlinePlayer()
    : m_clientId(-1) {
    Reset();
}

void OnlinePlayer::Reset() {
    m_oneWayLatencyMs = 0;
    m_lastClockSyncRealTimeReceived = -1.0f;
    m_driverNumber = -1;
    m_clientId = -1;
    m_clientState = 0;
    m_clientTimestamp = 0.0f;
}

void OnlinePlayer::SetClientState(int new_state) {
    m_clientState = new_state;
}

#endif
