#include "Speed/Indep/Src/Frontend/HUD/FeRaceInformation.hpp"

RaceInformation::RaceInformation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IRaceInformation(pOutter)
{
}

void RaceInformation::Update(IPlayer *player) {
}

void RaceInformation::SetNumRacers(int numRacers) {
    mNumRacers = numRacers;
}

void RaceInformation::SetNumLaps(int numLaps) {
    mNumLaps = numLaps;
}

void RaceInformation::SetPlayerPosition(int position) {
    mPlayerPosition = position;
}

void RaceInformation::SetPlayerLapTime(float lapTime) {
    mPlayerLapTime = lapTime;
}

void RaceInformation::SetSuddenDeathMode(bool suddenDeath) {
    mSuddenDeath = suddenDeath;
}

void RaceInformation::SetPlayerPercentComplete(float percent) {
    mPlayerPercentComplete = percent;
}

void RaceInformation::SetPlayerTollboothsCrossed(int num) {
    mPlayerTollboothNumber = num;
}

void RaceInformation::SetNumTollbooths(int num) {
    mNumTollbooths = num;
}
