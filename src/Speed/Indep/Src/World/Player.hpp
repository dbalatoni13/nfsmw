#ifndef WORLD_PLAYER_H
#define WORLD_PLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class Player {
    // total size: 0x1
public:
    Player *GetPlayerByNumber(int number);
    Player *GetPlayerByIndex(int number);
    int GetNumPlayers();
};

#endif
