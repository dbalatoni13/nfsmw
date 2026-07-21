#include "Client.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"

uint32 Client::m_tSendCarUpdatesTimer[4];
int Client::m_clientId;
int Client::m_driverNumber;
uint16 Client::m_oneWayLatencyMs;
float Client::m_lastClockSyncRealTimeReceived;
ClientStateEnum Client::m_state;
int Client::m_timestampCorrectionCount;
int Client::m_timestampErrorCount;
int Client::m_lastErrorSign;
uint32 Client::m_syncAnimationRecvId;
int Client::m_serverDriverNumber;
float Client::m_serverTimestamp;
ConnApiClientT *Client::hostConnection;

void Client::Init() {
    uint32 time = NetworkCore::Instance().GetTime();
    for (int i = 3; i >= 0; i--) {
        m_tSendCarUpdatesTimer[i] = time;
    }
    ConnectionCore::Instance().SetCallback(ConnectionCoreCB, nullptr);
    SetState(CLIENTSTATE_WAITING);
}

void Client::Close() {
    if (m_state > CLIENTSTATE_INITIAL) {
        ConnectionCore::Instance().SetCallback(nullptr, nullptr);
        if (IsConnected() == true) {
            SimpleSendMessage(8);
            NetworkCore::Instance().Flush(&hostConnection->pGameLinkRef, 1);
        }
        if (m_serverDriverNumber != -1) {
            TheOnlineManager.DriverLeft(m_serverDriverNumber, false);
        }
        m_serverDriverNumber = -1;
        m_lastClockSyncRealTimeReceived = -1.0f;
        m_clientId = -1;
        m_driverNumber = -1;
        m_oneWayLatencyMs = 0;
        m_serverTimestamp = 0.0f;
        m_timestampCorrectionCount = 0;
        m_timestampErrorCount = 0;
        m_lastErrorSign = 0;
        m_syncAnimationRecvId = 0;
        hostConnection = nullptr;
        SetState(CLIENTSTATE_INITIAL);
    }
}

void Client::ReadIncomingPackets() {
    if (IsConnected()) {
        NetGamePacketT sGamePacketInbound;
        bool reliable = false;
        NetworkCore &networkCore = NetworkCore::Instance();
        NetGameLinkRefT *linkRef = hostConnection->pGameLinkRef;

        while (networkCore.Recv(linkRef, sGamePacketInbound, reliable)) {
            HandleIncomingPacket(reinterpret_cast<char *>(sGamePacketInbound.body.data),
                                 sGamePacketInbound.head.len, reliable);
        }
    }
}
