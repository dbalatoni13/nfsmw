#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"

MilestoneBoard::MilestoneBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IMilestoneBoard(pOutter)
{
}

void MilestoneBoard::Update(IPlayer *player) {
}
