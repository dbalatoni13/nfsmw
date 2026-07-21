#ifndef ONLINE_INGAME_ONLINE_PLAYER_MGR_HPP
#define ONLINE_INGAME_ONLINE_PLAYER_MGR_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "OnlinePlayer.hpp"

struct OnlinePlayerMgr {
  private:
    static OnlinePlayer m_OnlinePlayer[3];
    static int m_numPlayers;

  public:
    static void Init();
    static void Close();
    static OnlinePlayer *GetOnlinePlayer(int player_num);
    static int GetNumPlayers() { return m_numPlayers; }
    static OnlinePlayer *AddOnlinePlayer(int client_id);
};

#endif
