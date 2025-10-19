#include "./RaceParameters.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void RaceParameters::InitWithDefaults() {
    bMemSet(this, 0, 0xA0);

    this->TrackNumber = 0x7D0;
    this->TrackDirection = eDIRECTION_FORWARD;
    this->TrafficDensity = eTRAFFICDENSITY_HIGH;
    this->TrafficOncoming = 0.5f;
    this->AIDemoMode = false;
    this->ReplayDemoMode = false;
    this->RaceType = RACE_TYPE_NONE;
    this->Point2Point = 0;
    this->RollingStartSpeed = 0.0f;
    this->NumLapsInRace = 2;
    this->NumPlayerCars = 1;
    this->NumAICars = 7;
    this->PlayerStartPosition[0] = 7;
    this->PlayerStartPosition[1] = 8;
    this->NumOnlinePlayerCars = 0;
    this->NumOnlineAICars = 0;
    this->DamageEnabled = 1;
    this->HandlingMode = HANDLING_MODE_EXTREME;
    this->FinishLineNumber = 0;
    this->bDragRaceFlag = false;
    this->bDriftRaceFlag = false;
    this->bShortRaceFlag = false;
    this->bBurnoutFlag = false;
    this->bOnlineRace = false;
    this->bCarShowFlag = false;
    this->bGamebreakerOn = true;
    this->PlayerJoyports[0] = -1;
    this->PlayerJoyports[1] = -1;
    this->nMaxCops = 0;
    this->CopStrength = eOPPONENTSTRENGTH_LOW;
    this->DriftOpponentScoreMultiplier = 1.0f;
    this->Boost = 1;
    this->OpponentStrength = eOPPONENTSTRENGTH_LOW;
    this->BoostScale[0] = 1.0f;
    this->BoostScale[1] = 1.0f;
    this->AIDifficultyModifier = eAI_DIFFICULTY_MODIFIER_NORMAL;
    this->PlayerDriverNumber[0] = -1;
    this->PlayerDriverNumber[1] = -1;
    this->NumDriverInfo = 0;
    this->TimeTrialTime.ResetLow();
    this->bCareerEventRace = false;
}
