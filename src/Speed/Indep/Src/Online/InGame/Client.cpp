#include "Client.hpp"

uint32 Client::m_tSendCarUpdatesTimer[4];
ClientStateEnum Client::m_state;

void Client::Init() {
    uint32 time = NetworkCore::Instance().GetTime();
    for (int i = 3; i >= 0; i--) {
        m_tSendCarUpdatesTimer[i] = time;
    }
    ConnectionCore::Instance().SetCallback(ConnectionCoreCB, nullptr);
    SetState(CLIENTSTATE_WAITING);
}
