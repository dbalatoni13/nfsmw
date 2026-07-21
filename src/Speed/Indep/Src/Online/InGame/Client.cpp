#include "Client.hpp"
#include "CSCommon.hpp"
#include "Speed/Indep/Src/Online/SmartBitstream.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/World.hpp"

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

void Client::StartClientProcessing() {
    hostConnection = ConnectionCore::Instance().GetPlayer(0);
    SetState(CLIENTSTATE_CONNECTED);
}

void Client::HandleIncomingPacket(char *data, int numBytes, bool isReliable) {
    if (CSCommon::GetDiagnosticLevel() >= DIAGNOSTICLEVEL_EXTREME) {
        CSCommon::DumpBytes(data, numBytes);
    }

    SmartBitStream bitstream_data;
    bitstream_data.PokeBytes(data, numBytes);
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 8);
    uint8 message_type = temp;

    switch (message_type) {
    case 2:
        ProcessCarMessage(bitstream_data);
        break;
    case 4:
        ProcessCarSpamClockSyncMessage(bitstream_data);
        break;
    case 5:
        ProcessScoreMessage(bitstream_data);
        break;
    case 6:
        ProcessSyncAnimationMessage(bitstream_data);
        break;
    case 0:
        ProcessWelcomeMessage(bitstream_data);
        break;
    case 1:
        ProcessCarDescriptionMessage(bitstream_data);
        break;
    case 9:
        ProcessServerQuitMessage(bitstream_data);
        break;
    case 10:
        ProcessClientLeftMessage(bitstream_data);
        break;
    case 7:
        ProcessDriverFinishMessage(bitstream_data);
        break;
    case 12:
        ProcessServerStateChangeMessage(bitstream_data);
        break;
    case 13:
        ProcessDataCRCMessage(bitstream_data);
        break;
    case 14:
        ProcessRestartLoad(bitstream_data);
        break;
    case 15:
        ProcessStartRaceSyncMessage(bitstream_data);
        break;
    }
}

void Client::ConnectionCoreCB(ConnApiRefT *connapi, ConnApiCbInfoT *cbinfo, void *context) {
    if (hostConnection && cbinfo->eType == CONNAPI_CBTYPE_GAMEEVENT && cbinfo->iClientId == 0 &&
        cbinfo->eNewStatus == CONNAPI_STATUS_DISC) {
        Close();
        return;
    }
}

void Client::ProcessCarMessage(SmartBitStream &bitstream_data) {
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 32);
    ProcessPartialCarMessage(RealTimer.GetSeconds(), bitstream_data);
}

void Client::ProcessPartialCarMessage(float player_timestamp, SmartBitStream &bitstream_data) {
    if (pCurrentWorld) {
        int driver_number =
            bitstream_data.GetQuantizedInt(Online::m_driverNumberQuantizer);
        ePosDataPriorityMask priority_mask =
            static_cast<ePosDataPriorityMask>(bitstream_data.GetByte());
        if (TheOnlineManager.GetOnlineRacer(driver_number)) {
            TheOnlineManager.ImportPositionData(driver_number, bitstream_data, player_timestamp,
                                                priority_mask);
        }
    }
}

void Client::ProcessCarSpamClockSyncMessage(SmartBitStream &bitstream_data) {
    uint32 start_ticks = bitstream_data.GetInt();
    uint32 time = bitstream_data.GetInt();
    TheOnlineManager.mPosUpdatePing = bGetTickerDifference(start_ticks) * 0.001f;
    TheOnlineManager.SetServerTime(time);
    ProcessPartialCarMessage(RealTimer.GetSeconds(), bitstream_data);
}

void Client::ProcessScoreMessage(SmartBitStream &bitstream_data) {
    TheOnlineManager.SignalScoreMessage(bitstream_data);
}

void Client::ProcessSyncAnimationMessage(SmartBitStream &bitstream_data) {
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 32);
    if (m_syncAnimationRecvId < temp) {
        m_syncAnimationRecvId = temp;
        TheOnlineManager.SignalSyncAnimationMessage(bitstream_data);
    }
}

void Client::ProcessDataCRCMessage(SmartBitStream &bitstream_data) {
    TheOnlineManager.SignalDataCRCMessage(bitstream_data);
}

void Client::ProcessWelcomeMessage(SmartBitStream &bitstream_data) {
    m_clientId = bitstream_data.GetInt();
    m_driverNumber = bitstream_data.GetInt();
    m_serverDriverNumber = bitstream_data.GetInt();
    TheOnlineManager.ImportRaceParams(m_driverNumber, m_serverDriverNumber, bitstream_data);
    SendCarDescriptionMessage();
    SetState(CLIENTSTATE_DESCRIBED);
}

void Client::ProcessCarDescriptionMessage(SmartBitStream &bitstream_data) {
    static SplitPacketList splitPackets;
    if (Online::ReceiveChunk(bitstream_data, splitPackets) == true) {
        SmartBitStream reassembledData;
        Online::JoinPackets(reassembledData, splitPackets);
        uint32 temp = 0;
        reassembledData.GetBits(temp, 32);
        TheOnlineManager.ImportDriverInfo(temp, reassembledData);
    }
}

void Client::ProcessDriverFinishMessage(SmartBitStream &bitstream_data) {
    static SplitPacketList splitPackets;
    if (Online::ReceiveChunk(bitstream_data, splitPackets) == true) {
        SmartBitStream reassembledData;
        Online::JoinPackets(reassembledData, splitPackets);
        TheOnlineManager.SignalDriverFinish(reassembledData);
    }
}

void Client::SendCarDescriptionMessage() {
    SmartBitStream payload_data;
    payload_data.AddInt(m_driverNumber);
    TheOnlineManager.ExportDriverInfo(m_driverNumber, payload_data);

    SplitPacketList splitPackets;
    Online::SplitPacket(MSG_R_BI_CARDESCRIPTION, payload_data, splitPackets);
    SplitPacketNode *node;
    while ((node = splitPackets.GetHead()) != splitPackets.EndOfList()) {
        SendMessage(1, node->data, true);
        splitPackets.RemoveHead();
        delete node;
    }
}

void Client::SendStartRaceSyncMessage() {
    if (m_state > CLIENTSTATE_INITIAL) {
        if (m_serverState == 6) {
            SmartBitStream bitstream_data;
            bitstream_data.AddByte(15);
            uint32 ret = bGetTicker();
            bitstream_data.AddInt(ret);
            SendMessage(15, bitstream_data, false);
        }
    }
}

void Client::ProcessStartRaceSyncMessage(SmartBitStream &bitstream_data) {
    uint32 ret = bGetTicker();
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 32);
    float time = bitstream_data.GetFloat();
    TheOnlineManager.SetStartRaceTime(ret, time, bGetTickerDifference(temp, ret) * 0.001f);
}

void Client::SignalStartClockSync() {
    SetState(CLIENTSTATE_NIS);
}

void Client::SignalReady() {
    SetState(CLIENTSTATE_READY);
}

void Client::SignalRestart() {
    SetState(CLIENTSTATE_LOADING);
}

void Client::SignalDriverFinish(SmartBitStream &payload_data) {
    SplitPacketList splitPackets;
    Online::SplitPacket(MSG_R_BI_DRIVERFINISH, payload_data, splitPackets);
    SplitPacketNode *node;
    while ((node = splitPackets.GetHead()) != splitPackets.EndOfList()) {
        SendMessage(7, node->data, true);
        splitPackets.RemoveHead();
        delete node;
    }
}

void Client::SignalDataCRCMessage(SmartBitStream &payload_data) {
    SmartBitStream bitstream_data;
    bitstream_data.AddByte(13);
    bitstream_data.AddRawDataWithoutSize(payload_data.GetBuffer(), payload_data.GetByteLength());
    SendMessage(13, bitstream_data, true);
}

void Client::ProcessServerQuitMessage(SmartBitStream &bitstream_data) {
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 1);
    Close();
}

void Client::ProcessClientLeftMessage(SmartBitStream &bitstream_data) {
    int driver_number = bitstream_data.GetInt();
    bool he_quit = bitstream_data.GetBool();
    TheOnlineManager.DriverLeft(driver_number, he_quit);
}

void Client::ProcessRestartLoad(SmartBitStream &bitstream_data) {
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 32);
    TheOnlineManager.SignalRestartLoad(temp);
}

void Client::ProcessServerStateChangeMessage(SmartBitStream &bitstream_data) {
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 32);
    m_serverState = temp;
    if (temp == 6) {
        uint32 ret = bGetTicker();
        TheOnlineManager.SetStartRaceTime(ret, 2.0f, 2.0f);
    } else if (static_cast<int>(temp) < 7) {
        if (temp == 3) {
            if (TheOnlineManager.GetRestartingRace() || static_cast<int>(m_state) > 3) {
                TheOnlineManager.RequestRestart();
            } else {
                TheOnlineManager.SignalLoad();
                SetState(CLIENTSTATE_LOADING);
            }
        }
    } else if (temp == 7) {
        SetState(CLIENTSTATE_RACING);
    }
}

void Client::SimpleSendMessage(uint8 msg_type) {
    SmartBitStream bitstream_data;
    bitstream_data.AddByte(msg_type);
    SendMessage(msg_type, bitstream_data, true);
}

void Client::SendMessage(uint8 msg_type, SmartBitStream &bitstream_data, bool is_reliable) {
    if (NetworkCore::Instance().Send(hostConnection->pGameLinkRef, bitstream_data.GetBuffer(),
                                     bitstream_data.GetByteLength(), is_reliable) != 0 &&
        CSCommon::GetDiagnosticLevel() >= DIAGNOSTICLEVEL_EXTREME) {
        CSCommon::DumpBytes(bitstream_data.GetBuffer(), bitstream_data.GetByteLength());
    }
}
