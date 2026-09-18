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

void Server::HandleClientDeparture(int client_id, bool he_quit) {
    OnlinePlayer *p_player = OnlinePlayerMgr::FindPlayerWithClientId(client_id);
    if (p_player) {
        int driver_number = p_player->GetDriverNumber();
        OnlinePlayerMgr::RemoveOnlinePlayer(client_id);
        TheOnlineManager.DriverLeft(driver_number, he_quit);
        SendClientLeftMessage(client_id, driver_number, he_quit);
    }
}

void Server::SendWelcomeMessage(int client_id) {
    int value = OnlinePlayerMgr::FindPlayerWithClientId(client_id)->GetDriverNumber();
    SmartBitStream bitstream_data;
    bitstream_data.AddByte(0);
    bitstream_data.AddInt(client_id);
    bitstream_data.AddInt(value);
    bitstream_data.AddInt(m_driverNumber);
    TheOnlineManager.ExportRaceParams(m_driverNumber, bitstream_data);
    SendMessageToOneClient(client_id, 0, bitstream_data, true);
}

void Server::SendQuitMessage() {
    SmartBitStream bitstream_data;
    bitstream_data.AddByte(9);
    bitstream_data.AddBool(m_bForceClientShutdown);
    SendMessageToAllClients(9, bitstream_data, true);
}

void Server::SendClientLeftMessage(int leaving_client_id, int driver_number, bool he_quit) {
    SmartBitStream bitstream_data;
    bitstream_data.AddByte(10);
    bitstream_data.AddInt(driver_number);
    bitstream_data.AddBool(he_quit);
    SendMessageToAlmostAllClients(leaving_client_id, 10, bitstream_data, true);
}

void Server::SendCarSpamClockSyncMessage(int to_client_id, uint32 client_tick,
                                         ePosDataPriorityMask priority_mask) {
    if (pCurrentWorld && TheOnlineManager.pRacers[m_driverNumber]) {
        SmartBitStream bitstream_data;
        bitstream_data.AddByte(4);
        bitstream_data.AddInt(client_tick);
        bitstream_data.AddInt(TheOnlineManager.GetMasterTime());
        bitstream_data.AddQuantizedInt(m_driverNumber, Online::m_driverNumberQuantizer);
        bitstream_data.AddByte(static_cast<uint8>(priority_mask));
        TheOnlineManager.ExportPositionData(m_driverNumber, bitstream_data, priority_mask);
        SendMessageToOneClient(to_client_id, 4, bitstream_data, false);
    }
}

void Server::SignalStartClockSync() { SetState(SERVERSTATE_NIS); }

void Server::SignalReady() { SetState(SERVERSTATE_WAITLOADS); }

void Server::SignalRestart() { SetState(SERVERSTATE_LOADING); }

void Server::SignalDriverFinish(SmartBitStream &payload_data) {
    SplitPacketList splitPackets;
    Online::SplitPacket(MSG_R_BI_DRIVERFINISH, payload_data, splitPackets);
    SplitPacketNode *node;
    while ((node = splitPackets.GetHead()) != splitPackets.EndOfList()) {
        SendMessageToAllClients(7, node->data, true);
        splitPackets.RemoveHead();
        delete node;
    }
}

void Server::SignalDataCRCMessage(SmartBitStream &payload_data) {
    SmartBitStream bitstream_data;
    bitstream_data.AddByte(13);
    bitstream_data.AddRawDataWithoutSize(payload_data.GetBuffer(),
                                         payload_data.GetByteLength());
    SendMessageToAllClients(13, bitstream_data, true);
}

void Server::SignalSyncAnimationMessage(SmartBitStream &payload_data) {
    SmartBitStream bitstream_data;
    bitstream_data.AddByte(6);
    bitstream_data.AddInt(++m_syncAnimationSendId);
    bitstream_data.AddRawDataWithoutSize(payload_data.GetBuffer(),
                                         payload_data.GetByteLength());
    SendMessageToAllClients(6, bitstream_data, false);
}

void Server::ProcessDriverFinishMessage(SmartBitStream &bitstream_data, int client_id) {
    static SplitPacketList *splitPackets[4];
    static bool firstTime = true;

    if (firstTime == true) {
        firstTime = false;
        bMemSet(splitPackets, 0, sizeof(splitPackets));
    }
    if (!splitPackets[client_id]) {
        splitPackets[client_id] =
            new ("Server::ProcessDriverFinishMessage", 0) SplitPacketList;
    }
    if (Online::ReceiveChunk(bitstream_data, *splitPackets[client_id]) == true) {
        SmartBitStream reassembledData;
        Online::JoinPackets(reassembledData, *splitPackets[client_id]);
        TheOnlineManager.SignalDriverFinish(reassembledData);
        delete splitPackets[client_id];
        splitPackets[client_id] = nullptr;

        SplitPacketList packetsToSend;
        reassembledData.Rewind();
        Online::SplitPacket(MSG_R_BI_DRIVERFINISH, reassembledData, packetsToSend);
        SplitPacketNode *node;
        while ((node = packetsToSend.GetHead()) != packetsToSend.EndOfList()) {
            SendMessageToAlmostAllClients(client_id, 7, node->data, true);
            packetsToSend.RemoveHead();
            delete node;
        }
    }
}

void Server::ProcessClientStateChangeMessage(SmartBitStream &bitstream_data, int client_id) {
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 32);
    uint32 new_state = temp;
    OnlinePlayer *p_player = OnlinePlayerMgr::FindPlayerWithClientId(client_id);
    int old_state = p_player->GetClientState();
    p_player->SetClientState(new_state);
    if (new_state == CLIENTSTATE_LOADING && old_state != CLIENTSTATE_DESCRIBED) {
        SendClientLoadMessage(client_id, p_player->GetDriverNumber());
    }
    if (new_state == CLIENTSTATE_READY) {
        uint32 temp = 0;
        bitstream_data.GetBits(temp, 32);
        p_player->SetTicker(temp);
    }
}

void Server::ProcessStartRaceSyncMessage(SmartBitStream &bitstream_data, int client_id) {
    uint32 temp = 0;
    bitstream_data.GetBits(temp, 32);
    uint32 value = temp;
    OnlinePlayerMgr::FindPlayerWithClientId(client_id);
    if (m_state == SERVERSTATE_READY) {
        SmartBitStream response_data;
        response_data.AddByte(15);
        response_data.AddInt(value);
        response_data.AddFloat(TheOnlineManager.GetStartRaceTime(bGetTicker()));
        SendMessageToOneClient(client_id, 15, response_data, false);
    }
}

void Server::SendClientLoadMessage(int client_id, int driver_num) {
    TheOnlineManager.SignalRestartLoad(driver_num);
    SmartBitStream bitstream_data;
    bitstream_data.AddByte(14);
    bitstream_data.AddInt(driver_num);
    SendMessageToAlmostAllClients(client_id, 14, bitstream_data, true);
}

void Server::SendCarDescriptionMessage(int to_client_id, int driver_number) {
    SmartBitStream payload_data;
    payload_data.AddInt(driver_number);
    TheOnlineManager.ExportDriverInfo(driver_number, payload_data);

    SplitPacketList splitPackets;
    Online::SplitPacket(MSG_R_BI_CARDESCRIPTION, payload_data, splitPackets);
    SplitPacketNode *node;
    while ((node = splitPackets.GetHead()) != splitPackets.EndOfList()) {
        SendMessageToOneClient(to_client_id, 1, node->data, true);
        splitPackets.RemoveHead();
        delete node;
    }
}

void Server::SendMessageToAlmostAllClients(int exclude_client_id, uint8 msg_type,
                                           SmartBitStream &bitstream_data,
                                           bool is_reliable) {
    for (int player_num = 0; player_num < 3; player_num++) {
        OnlinePlayer *p_player = OnlinePlayerMgr::GetOnlinePlayer(player_num);
        if (p_player && p_player->GetClientId() != exclude_client_id) {
            SendMessageToOneClient(p_player->GetClientId(), msg_type, bitstream_data,
                                   is_reliable);
        }
    }
}

void Server::SendMessageToAllClients(uint8 msg_type, SmartBitStream &bitstream_data,
                                     bool is_reliable) {
    for (int player_num = 0; player_num < 3; player_num++) {
        OnlinePlayer *p_player = OnlinePlayerMgr::GetOnlinePlayer(player_num);
        if (p_player) {
            SendMessageToOneClient(p_player->GetClientId(), msg_type, bitstream_data,
                                   is_reliable);
        }
    }
}

void Server::SendMessageToOneClient(int client_id, uint8 msg_type,
                                    SmartBitStream &bitstream_data, bool is_reliable) {
    ConnApiClientT *player = ConnectionCore::Instance().GetPlayer(client_id);
    if (player) {
        if (player->GameInfo.eStatus == CONNAPI_STATUS_ACTV) {
            if (NetworkCore::Instance().Send(player->pGameLinkRef, bitstream_data.GetBuffer(),
                                             bitstream_data.GetByteLength(), is_reliable) != 0 &&
                CSCommon::GetDiagnosticLevel() >= DIAGNOSTICLEVEL_EXTREME) {
                CSCommon::DumpBytes(bitstream_data.GetBuffer(), bitstream_data.GetByteLength());
            }
        }
    }
}

void Server::SetState(ServerStateEnum new_state) {
    m_state = new_state;
    if (new_state > SERVERSTATE_INITIAL) {
        SmartBitStream bitstream_data;
        bitstream_data.AddByte(12);
        bitstream_data.AddInt(m_state);
        SendMessageToAllClients(12, bitstream_data, true);
    }
}

char *Server::DescribeState(int state) {
    switch (state) {
    case SERVERSTATE_INITIAL:
        return "INITIAL";
    case SERVERSTATE_WELCOME:
        return "WELCOME";
    case SERVERSTATE_WAITJOINS:
        return "WAITJOINS";
    case SERVERSTATE_LOADING:
        return "LOADING";
    case SERVERSTATE_NIS:
        return "NIS";
    case SERVERSTATE_WAITLOADS:
        return "WAITLOADS";
    case SERVERSTATE_READY:
        return "READY";
    case SERVERSTATE_RACING:
        return "RACING";
    default:
        return "Unknown";
    }
}

void Server::ConnectionCoreCB(ConnApiRefT *connapi, ConnApiCbInfoT *cbinfo, void *context) {
    if (cbinfo->eType == CONNAPI_CBTYPE_GAMEEVENT &&
        cbinfo->eNewStatus == CONNAPI_STATUS_DISC) {
        ConnectionCore::Instance().GetPlayer(cbinfo->iClientId);
        HandleClientDeparture(cbinfo->iClientId, false);
        return;
    }
}

void Server::ShowDiagnostics() {}

char *DescribeMessageType(int msg) {
    switch (msg) {
    case 0:
        return "R_SC_WELCOME";
    case 1:
        return "R_BI_CARDESCRIPTION";
    case 2:
        return "U_BI_CARSPAM";
    case 3:
        return "U_CS_CLOCKSYNC";
    case 4:
        return "U_SC_CARSPAMCLOCKSYNC";
    case 5:
        return "U_BI_SCORE";
    case 6:
        return "U_SC_SYNCANIMATION";
    case 7:
        return "R_BI_DRIVERFINISH";
    case 8:
        return "R_CS_CLIENTQUIT";
    case 9:
        return "R_SC_SERVERQUIT";
    case 10:
        return "R_SC_CLIENTLEFT";
    case 11:
        return "R_CS_CSTATECHANGE";
    case 12:
        return "R_SC_SSTATECHANGE";
    case 13:
        return "R_BI_DATACRC";
    case 14:
        return "R_SC_RESTARTLOAD";
    case 15:
        return "U_BI_STARTRACESYNC";
    default:
        return "Unknown";
    }
}
