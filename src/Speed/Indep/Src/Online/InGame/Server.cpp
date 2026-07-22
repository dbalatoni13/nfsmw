#include "Server.hpp"
#include "Client.hpp"
#include "CSCommon.hpp"
#include "OnlinePlayerMgr.hpp"

void Server::Init() {
    OnlinePlayerMgr::Init();
    ConnectionCore::Instance().SetCallback(ConnectionCoreCB, nullptr);
    m_driverNumber = FindRandomAvailableDriverNumber();
    SetState(SERVERSTATE_WELCOME);
}

void Server::Close() {
    if (m_state > SERVERSTATE_INITIAL) {
        int playersInArray = 0;
        ConnectionCore::Instance().SetCallback(nullptr, nullptr);
        SendQuitMessage();
        OnlinePlayerMgr::Close();
        m_driverNumber = -1;
        m_syncAnimationSendId = 0;

        int numPlayers = ConnectionCore::Instance().GetNumPlayers();
        NetGameLinkRefT **connections =
            new ("Server::Close", 0) NetGameLinkRefT *[numPlayers];
        for (int i = 0; i < numPlayers; i++) {
            ConnApiClientT *player = ConnectionCore::Instance().GetPlayer(i);
            if (player->GameInfo.eStatus == CONNAPI_STATUS_ACTV) {
                connections[playersInArray++] = player->pGameLinkRef;
            }
        }
        NetworkCore::Instance().Flush(connections, playersInArray);
        delete[] connections;
        SetState(SERVERSTATE_INITIAL);
    }
}

void Server::ReadIncomingPackets() {
    ConnectionCore &cc = ConnectionCore::Instance();
    if (cc.GetNumConnectedPlayers() == 0 && m_state > SERVERSTATE_INITIAL) {
        Close();
    } else {
        NetworkCore &networkCore = NetworkCore::Instance();
        int numPlayers = cc.GetNumPlayers();
        for (int clientID = 0; clientID < numPlayers; clientID++) {
            NetGamePacketT sGamePacketInbound;
            bool reliable = false;
            ConnApiClientT *player = cc.GetPlayer(clientID);
            if (player->GameInfo.eStatus == CONNAPI_STATUS_ACTV) {
                while (networkCore.Recv(player->pGameLinkRef, sGamePacketInbound, reliable)) {
                    HandleIncomingPacket(
                        clientID, reinterpret_cast<char *>(sGamePacketInbound.body.data),
                        sGamePacketInbound.head.len, reliable);
                }
            }
        }
        Think();
    }
}

void Server::Think() {
    switch (m_state) {
    case SERVERSTATE_INITIAL:
        break;

    case SERVERSTATE_WELCOME:
        for (int clientID = 1; clientID < ConnectionCore::Instance().GetNumPlayers(); clientID++) {
            ConnApiClientT *player = ConnectionCore::Instance().GetPlayer(clientID);
            if (player->GameInfo.eStatus == CONNAPI_STATUS_ACTV) {
                OnlinePlayer *p_new_player = OnlinePlayerMgr::AddOnlinePlayer(clientID);
                p_new_player->SetDriverNumber(FindRandomAvailableDriverNumber());
                SendWelcomeMessage(clientID);
                SendCarDescriptionMessage(clientID, m_driverNumber);
                for (int player_num = 0; player_num < 3; player_num++) {
                    OnlinePlayer *p_player = OnlinePlayerMgr::GetOnlinePlayer(player_num);
                    if (p_player && p_player->GetClientState() > CLIENTSTATE_CONNECTED) {
                        SendCarDescriptionMessage(clientID, p_player->GetDriverNumber());
                    }
                }
            }
        }
        SetState(SERVERSTATE_WAITJOINS);
        break;

    case SERVERSTATE_WAITJOINS: {
        int description_count = 0;
        for (int player_num = 0; player_num < 3; player_num++) {
            OnlinePlayer *p_player = OnlinePlayerMgr::GetOnlinePlayer(player_num);
            if (p_player && p_player->GetClientState() > CLIENTSTATE_CONNECTED) {
                description_count++;
            }
        }
        ConnectionCore &cc = ConnectionCore::Instance();
        if (description_count == cc.GetNumConnectedPlayers() && cc.GetNumPlayers() - 1 > 0) {
            TheOnlineManager.SignalLoad();
            SetState(SERVERSTATE_LOADING);
        }
        break;
    }

    case SERVERSTATE_LOADING:
        break;

    case SERVERSTATE_NIS:
        break;

    case SERVERSTATE_WAITLOADS: {
        int ready_count = 0;
        for (int player_num = 0; player_num < 3; player_num++) {
            OnlinePlayer *p_player = OnlinePlayerMgr::GetOnlinePlayer(player_num);
            if (p_player && p_player->GetClientState() == CLIENTSTATE_READY) {
                ready_count++;
            }
        }
        if (ready_count == OnlinePlayerMgr::GetNumPlayers() && ready_count > 0) {
            m_readyStateStartTime = RealTimer;
            m_ticker = bGetTicker();
            uint32 ret = bGetTicker();
            TheOnlineManager.SetStartRaceTime(ret, 2.0f, 0.0f);
            SetState(SERVERSTATE_READY);
        }
        break;
    }

    case SERVERSTATE_READY: {
        uint32 ret = bGetTicker();
        if (TheOnlineManager.GetStartRaceTime(ret) <= 0.0f) {
            SetState(SERVERSTATE_RACING);
        }
        break;
    }

    case SERVERSTATE_RACING:
        break;

    default:
        break;
    }
}

int Server::FindRandomAvailableDriverNumber() {
    int driver_number;
    do {
        driver_number = bRandom(4);
    } while (driver_number == m_driverNumber || OnlinePlayerMgr::FindPlayerWithDriverNumber(driver_number));
    return driver_number;
}

void Server::DisconnectLaggers() {
    for (int player_num = 0; player_num < 3; player_num++) {
        OnlinePlayer *p_player = OnlinePlayerMgr::GetOnlinePlayer(player_num);
        if (p_player && p_player->GetClientState() != CLIENTSTATE_READY) {
            HandleClientDeparture(p_player->GetClientId(), true);
        }
    }
}

void Server::StartServerProcessing() {}

void Server::HandleIncomingPacket(int client_id, char *raw_data, int num_bytes, bool is_reliable) {
    if (CSCommon::GetDiagnosticLevel() >= DIAGNOSTICLEVEL_EXTREME) {
        CSCommon::DumpBytes(raw_data, num_bytes);
    }

    SmartBitStream bitstream_data;
    bitstream_data.PokeBytes(raw_data, num_bytes);
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 8);
    uint8 message_type = temp;

    if (message_type < 16) {
        switch (message_type) {
        case 2:
            ProcessCarMessage(bitstream_data, client_id);
            break;
        case 3:
            ProcessClockSyncMessage(bitstream_data, client_id);
            break;
        case 5:
            ProcessScoreMessage(bitstream_data, client_id);
            break;
        case 1:
            ProcessCarDescriptionMessage(bitstream_data, client_id);
            break;
        case 8:
            HandleClientDeparture(client_id, true);
            break;
        case 7:
            ProcessDriverFinishMessage(bitstream_data, client_id);
            break;
        case 11:
            ProcessClientStateChangeMessage(bitstream_data, client_id);
            break;
        case 13:
            ProcessDataCRCMessage(bitstream_data, client_id);
            break;
        case 15:
            ProcessStartRaceSyncMessage(bitstream_data, client_id);
            break;
        }
    }
}

void Server::ProcessCarMessage(SmartBitStream &bitstream_data, int client_id) {
    OnlinePlayer *p_sender = OnlinePlayerMgr::FindPlayerWithClientId(client_id);
    if (m_state >= SERVERSTATE_READY && TheOnlineManager.GetState() == OLS_RACING && p_sender) {
        uint32 client_tick = bitstream_data.GetInt();
        bitstream_data.GetQuantizedInt(Online::m_driverNumberQuantizer);
        ePosDataPriorityMask pos_priority =
            static_cast<ePosDataPriorityMask>(bitstream_data.GetByte());
        int driver_number = p_sender->GetDriverNumber();
        float player_timestamp = RealTimer.GetSeconds();
        OnlineRacer *racer = TheOnlineManager.GetOnlineRacer(driver_number);
        if (racer && racer->IsConnected()) {
            TheOnlineManager.ImportPositionData(driver_number, bitstream_data,
                                                player_timestamp, pos_priority);
            OnlineRacer *sender = TheOnlineManager.GetOnlineRacer(driver_number);
            for (int player_num = 0; player_num < 3; player_num++) {
                OnlinePlayer *p_player = OnlinePlayerMgr::GetOnlinePlayer(player_num);
                if (p_player && p_player->GetClientId() != client_id &&
                    p_player->GetClientState() > CLIENTSTATE_LOADING) {
                    OnlineRacer *receiver = TheOnlineManager.GetOnlineRacer(player_num);
                    if (sender && receiver && receiver->IsAbleToSee(*sender)) {
                        SendMessageToOneClient(p_player->GetClientId(), 2, bitstream_data, false);
                    }
                }
            }
            SendCarSpamClockSyncMessage(client_id, client_tick, pos_priority);
        }
    }
}

void Server::ProcessClockSyncMessage(SmartBitStream &bitstream_data, int client_id) {
    float server_realtime = bitstream_data.GetFloat();
    float latency_s = (RealTimer.GetSeconds() - server_realtime) * 0.5f;
    int latency_ms = static_cast<int>(latency_s * 1000.0f + 0.5f);
    OnlinePlayer *p_player = OnlinePlayerMgr::FindPlayerWithClientId(client_id);
    if (p_player->GetLastClockSyncRealTimeReceived() < server_realtime) {
        p_player->SetLastClockSyncRealTimeReceived(server_realtime);
        p_player->SetOneWayLatencyMs(latency_ms);
    }
}

void Server::ProcessScoreMessage(SmartBitStream &bitstream_data, int client_id) {
    TheOnlineManager.SignalScoreMessage(bitstream_data);
    SendMessageToAlmostAllClients(client_id, 5, bitstream_data, false);
}

void Server::ProcessDataCRCMessage(SmartBitStream &bitstream_data, int client_id) {
    TheOnlineManager.SignalDataCRCMessage(bitstream_data);
    SendMessageToAlmostAllClients(client_id, 13, bitstream_data, true);
}

void Server::ProcessCarDescriptionMessage(SmartBitStream &bitstream_data, int client_id) {
    static SplitPacketList *splitPackets[4];
    static bool firstTime = true;

    if (firstTime == true) {
        firstTime = false;
        bMemSet(splitPackets, 0, sizeof(splitPackets));
    }
    if (!splitPackets[client_id]) {
        splitPackets[client_id] =
            new ("Server::ProcessCarDescriptionMessage", 0) SplitPacketList;
    }
    if (Online::ReceiveChunk(bitstream_data, *splitPackets[client_id]) == true) {
        SmartBitStream reassembledData;
        Online::JoinPackets(reassembledData, *splitPackets[client_id]);
        uint32 temp = 0;
        reassembledData.GetBits(temp, 32);
        int driver_number = temp;
        OnlinePlayerMgr::FindPlayerWithClientId(client_id);
        TheOnlineManager.ImportDriverInfo(driver_number, reassembledData);
        delete splitPackets[client_id];
        splitPackets[client_id] = nullptr;

        SplitPacketList packetsToSend;
        reassembledData.Rewind();
        Online::SplitPacket(MSG_R_BI_CARDESCRIPTION, reassembledData, packetsToSend);
        SplitPacketNode *node;
        while ((node = packetsToSend.GetHead()) != packetsToSend.EndOfList()) {
            SendMessageToAlmostAllClients(client_id, 1, node->data, true);
            packetsToSend.RemoveHead();
            delete node;
        }
    }
}
