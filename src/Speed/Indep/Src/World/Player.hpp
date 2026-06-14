#ifndef PLAYER_HPP
#define PLAYER_HPP

enum PlayerZones {
    PLAYER_ZONE_NONE = 0,
    PLAYER_ZONE_FREEZE = 1,
    PLAYER_ZONE_PREVIEW = 2,
    PLAYER_ZONE_SLOMO = 3,
    PLAYER_ZONE_SLOMO2 = 4,
    PLAYER_ZONE_WARP = 5,
    PLAYER_ZONE_JUMPVIEW = 6,
    PLAYER_ZONE_JUMPVIEW2 = 7,
    PLAYER_ZONE_COUNT = 8,
};

// total size: 0x1
class Player {
  public:
    static Player *GetPlayerByNumber(int number);
    static Player *GetPlayerByIndex(int number);
    static int GetNumPlayers();

  private:
    static Player *pPlayersByNumber[2]; // size: 0x8
    static Player *pPlayersByIndex[2];  // size: 0x8
    static int NumPlayers;              // size: 0x4
    static int nJoysticksChanged;       // size: 0x4
};

#endif
