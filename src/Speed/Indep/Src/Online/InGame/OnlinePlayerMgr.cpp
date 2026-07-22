#include "OnlinePlayerMgr.hpp"

OnlinePlayer OnlinePlayerMgr::m_OnlinePlayer[3];
int OnlinePlayerMgr::m_numPlayers;

void OnlinePlayerMgr::Init() {
    for (int i = 0; i < 3; i++) {
        m_OnlinePlayer[i].Reset();
    }
}
