#include "Speed/Indep/Src/Frontend/HUD/FeLeaderBoard.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

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

void LeaderBoard::SetRacerNum(int pos, int num) {
    if (pos > 3) return;
    mTopRacers[pos].mRacerNum = num;
}

void LeaderBoard::SetRacerTotalPoints(int pos, float points) {
    if (pos > 3) return;
    mTopRacers[pos].mTotalPoints = points;
}

void LeaderBoard::SetRacerHasHeadset(int pos, bool racerHasHeadset) {
    if (pos > 3) return;
    mTopRacers[pos].mHasHeadset = racerHasHeadset;
}

void LeaderBoard::SetRacerName(int pos, const char *name) {
    if (pos > 3) return;
    bStrCpy(mTopRacers[pos].mRacerName, name);
}
