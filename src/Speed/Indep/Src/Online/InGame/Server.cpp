#include "Server.hpp"
#include "Client.hpp"
#include "OnlinePlayerMgr.hpp"

void Server::Init() {
    OnlinePlayerMgr::Init();
    ConnectionCore::Instance().SetCallback(ConnectionCoreCB, nullptr);
    m_driverNumber = FindRandomAvailableDriverNumber();
    SetState(SERVERSTATE_WELCOME);
}
