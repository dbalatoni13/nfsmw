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
