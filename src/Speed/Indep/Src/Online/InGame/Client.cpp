#include "Client.hpp"
#include "Speed/Indep/Src/Online/SmartBitstream.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"

uint32 Client::m_tSendCarUpdatesTimer[4];
float Client::m_sendCarFrequencyHz[4];
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
int Client::m_serverState;
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

ePosDataPriorityMask Client::BuildPosDataPriorityMask() {
    ePosDataPriorityMask return_mask = PDP_MASK_NONE;
    uint32 time = NetworkCore::Instance().GetTime();

    for (int i = 0; i < 4; i++) {
        if (m_sendCarFrequencyHz[i] > 0.0f &&
            static_cast<float>(time - m_tSendCarUpdatesTimer[i]) * 0.001f >=
                1.0f / m_sendCarFrequencyHz[i]) {
            if (i != 0 && return_mask == PDP_MASK_NONE) {
                return PDP_MASK_NONE;
            }
            m_tSendCarUpdatesTimer[i] = time;
            return_mask =
                static_cast<ePosDataPriorityMask>(return_mask | static_cast<int>(1U << i));
        }
    }
    return return_mask;
}

void Client::SendCarSpam() {
    if (IsConnected()) {
        ePosDataPriorityMask pdp_mask = BuildPosDataPriorityMask();
        if (m_driverNumber >= 0 && m_state >= CLIENTSTATE_READY &&
            m_serverState >= CLIENTSTATE_READY && pdp_mask != PDP_MASK_NONE) {
            int driver_number = m_driverNumber;
            if (TheOnlineManager.pRacers[driver_number]) {
                SmartBitStream bitstream_data;
                bitstream_data.AddByte(2);
                uint32 ret = bGetTicker();
                bitstream_data.AddInt(ret);
                bitstream_data.AddQuantizedInt(m_driverNumber, Online::m_driverNumberQuantizer);
                bitstream_data.AddByte(static_cast<uint8>(pdp_mask));
                TheOnlineManager.ExportPositionData(m_driverNumber, bitstream_data, pdp_mask);
                SendMessage(2, bitstream_data, false);
            }
        }
    }
}
