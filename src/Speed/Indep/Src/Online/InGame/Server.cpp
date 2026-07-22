#include "Server.hpp"
#include "Client.hpp"
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
