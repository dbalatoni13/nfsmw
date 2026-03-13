#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"

MilestoneBoard::MilestoneBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IMilestoneBoard(pOutter)
{
}

void MilestoneBoard::Update(IPlayer *player) {
}

void MilestoneBoard::SetInPursuit(bool inPursuit) {
    mInPursuit = inPursuit;
}

void MilestoneBoard::SetChallengeSeries(bool challenge) {
    mChallengeSeries = challenge;
}

void MilestoneBoard::SetNumberOfMilestones(int num) {
    mNumMilestones = num;
}
