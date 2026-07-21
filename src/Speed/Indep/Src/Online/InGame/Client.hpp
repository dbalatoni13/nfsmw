#ifndef ONLINE_INGAME_CLIENT_HPP
#define ONLINE_INGAME_CLIENT_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Online/LobbyCore.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"

class IntQuantizer;

enum ClientStateEnum {
    CLIENTSTATE_INITIAL = 0,
    CLIENTSTATE_WAITING = 1,
    CLIENTSTATE_CONNECTED = 2,
    CLIENTSTATE_DESCRIBED = 3,
    CLIENTSTATE_LOADING = 4,
    CLIENTSTATE_NIS = 5,
    CLIENTSTATE_READY = 6,
    CLIENTSTATE_RACING = 7
};

struct Online {
    static IntQuantizer m_driverNumberQuantizer;
};

struct ByteCounter {
    ByteCounter(int LengthOfSecond)
        : m_lengthOfSecond(LengthOfSecond) //
        , m_time(0) //
        , m_bytes(0) //
        , m_bps(0) {}
    ~ByteCounter() {}

    void Increment(int now, int bytes) {
        int delta = now - m_time;
        RollCounter(delta);
        m_bytes += bytes;
    }

    int GetBytesPerSecond(int now) {
        int delta = now - m_time;
        RollCounter(delta);
        return m_bps;
    }

  private:
    void RollCounter(int delta) {
        if (delta >= m_lengthOfSecond) {
            m_bps = m_bytes * m_lengthOfSecond / delta;
            m_time += delta;
            m_bytes = 0;
        }
    }

    int m_lengthOfSecond;
    int m_time;
    int m_bytes;
    int m_bps;
};

struct Client {
    static void Init();
    static void Close();
    static void StartClientProcessing();

  private:
    static void ReadIncomingPackets();
    static ePosDataPriorityMask BuildPosDataPriorityMask();
    static void SendCarSpam();
    static void HandleIncomingPacket(char *data, int numBytes, bool isReliable);
    static void ProcessCarMessage(SmartBitStream &bitstream_data);
    static void ProcessCarSpamClockSyncMessage(SmartBitStream &bitstream_data);
    static void ProcessScoreMessage(SmartBitStream &bitstream_data);
    static void ProcessSyncAnimationMessage(SmartBitStream &bitstream_data);
    static void ProcessWelcomeMessage(SmartBitStream &bitstream_data);
    static void ProcessCarDescriptionMessage(SmartBitStream &bitstream_data);
    static void ProcessDriverFinishMessage(SmartBitStream &bitstream_data);
    static void ProcessStartRaceSyncMessage(SmartBitStream &bitstream_data);
    static void ProcessServerQuitMessage(SmartBitStream &bitstream_data);
    static void ProcessClientLeftMessage(SmartBitStream &bitstream_data);
    static void ProcessRestartLoad(SmartBitStream &bitstream_data);
    static void ProcessServerStateChangeMessage(SmartBitStream &bitstream_data);
    static void ProcessDataCRCMessage(SmartBitStream &bitstream_data);
    static bool IsConnected();
    static void SimpleSendMessage(uint8 msg_type);
    static void SendMessage(uint8 msg_type, SmartBitStream &bitstream_data, bool isReliable);
    static void ConnectionCoreCB(ConnApiRefT *connapi, ConnApiCbInfoT *cbinfo, void *context);
    static void SetState(ClientStateEnum new_state);

    static uint32 m_tSendCarUpdatesTimer[4];
    static float m_sendCarFrequencyHz[4];
    static int m_clientId;
    static int m_driverNumber;
    static uint16 m_oneWayLatencyMs;
    static float m_lastClockSyncRealTimeReceived;
    static ClientStateEnum m_state;
    static int m_timestampCorrectionCount;
    static int m_timestampErrorCount;
    static int m_lastErrorSign;
    static uint32 m_syncAnimationRecvId;
    static int m_serverDriverNumber;
    static int m_serverState;
    static float m_serverTimestamp;
    static ConnApiClientT *hostConnection;
};

#endif
