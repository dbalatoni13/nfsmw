#include "OnlinePlayerMgr.hpp"

OnlinePlayer OnlinePlayerMgr::m_OnlinePlayer[3];
int OnlinePlayerMgr::m_numPlayers;

void OnlinePlayerMgr::Init() {
    for (int i = 0; i < 3; i++) {
        m_OnlinePlayer[i].Reset();
    }
}

void OnlinePlayerMgr::Close() {
    for (int i = 0; i < 3; i++) {
        m_OnlinePlayer[i].Reset();
    }
    m_numPlayers = 0;
}

OnlinePlayer *OnlinePlayerMgr::AddOnlinePlayer(int client_id) {
    OnlinePlayer *p_player = nullptr;
    OnlinePlayer *p_temp_player = m_OnlinePlayer;
    do {
        if (p_temp_player->GetClientId() == -1) {
            p_temp_player->SetClientId(client_id);
            p_player = p_temp_player;
            m_numPlayers++;
            break;
        }
        p_temp_player++;
    } while (reinterpret_cast<int>(p_temp_player) <
             reinterpret_cast<int>(m_OnlinePlayer + 3));
    return p_player;
}

void OnlinePlayerMgr::RemoveOnlinePlayer(int client_id) {
    OnlinePlayer *p_player = m_OnlinePlayer;
    while (reinterpret_cast<int>(p_player) <
           reinterpret_cast<int>(m_OnlinePlayer + 3)) {
        if (p_player->GetClientId() == client_id) {
            p_player->Reset();
            m_numPlayers--;
            break;
        }
        p_player++;
    }
}
