#include "Speed/Indep/Src/Frontend/HUD/FePursuitBoard.hpp"

PursuitBoard::PursuitBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IPursuitBoard(pOutter)
{
}

void PursuitBoard::Update(IPlayer *player) {
}
