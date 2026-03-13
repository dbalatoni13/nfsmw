#include "Speed/Indep/Src/Frontend/HUD/FeLeaderBoard.hpp"

LeaderBoard::LeaderBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ILeaderBoard(pOutter)
{
}

void LeaderBoard::Update(IPlayer *player) {
}
