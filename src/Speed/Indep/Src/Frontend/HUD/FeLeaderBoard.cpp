#include "Speed/Indep/Src/Frontend/HUD/FeLeaderBoard.hpp"

LeaderBoard::LeaderBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ILeaderBoard(pOutter)
{
}

void LeaderBoard::Update(IPlayer *player) {
}

void LeaderBoard::SetNumRacers(int numRacers) {
    mNumRacers = numRacers;
}

void LeaderBoard::SetNumLaps(int numLaps) {
    mNumLaps = numLaps;
}

void LeaderBoard::SetPlayerIndex(int index) {
    mPlayerIndex = index;
}
