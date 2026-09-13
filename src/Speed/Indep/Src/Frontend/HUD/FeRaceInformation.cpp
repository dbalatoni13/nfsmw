#include "Speed/Indep/Src/Frontend/HUD/FeRaceInformation.hpp"

#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

RaceInformation::RaceInformation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x4000000), IRaceInformation(pOutter) {
    mNumRacers = -1;
    mNumLaps = -1;
    mPlayerPosition = -1;
    mPlayerLapNumber = -1;
    mPlayerLapTime = 0.0f;
    mSuddenDeath = false;
    mPlayerPercentComplete = 0.0f;
    mPlayerTollboothNumber = 0;
    mNumTollbooths = 0;
    RegisterGroup(FEHashUpper("RaceInformationGroup"));
    mDataCurrentLapTime = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("CurrentLapTime")));
    mDataCompleteText = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("Complete_Text")));
    mDataPositionGroup = static_cast<FEGroup *>(FEngFindObject(pkg_name, FEHashUpper("POSITION_GROUP")));
    mDataIconTollbooth = reinterpret_cast<FEImage *>(FEngFindObject(pkg_name, FEHashUpper("Milestone_TollBooth")));
    mpDataTollboothNumTop = FEngFindObject(pkg_name, 0x1CDDD8D0);
    mpDataTollboothNumBot = FEngFindObject(pkg_name, 0x18A7ACD2);
    mpDataRacePosNum = FEngFindObject(pkg_name, 0x9C183BF8);
    mpDataRacerCount = FEngFindObject(pkg_name, 0x3BBD6268);
    mpDataPercentComplete = FEngFindObject(pkg_name, 0x9CB5C95D);
}

void RaceInformation::Update(IPlayer *player) {
    if (GRaceStatus::IsTollboothRace()) {
        if (!FEngIsScriptSet(mDataPositionGroup, FEHASH_HIDE)) {
            FEngSetScript(mDataPositionGroup, FEHASH_HIDE, true);
        }
        if (!FEngIsScriptSet(mDataIconTollbooth, FEHASH_APPEAR)) {
            FEngSetScript(mDataIconTollbooth, FEHASH_APPEAR, true);
        }
        FEPrintf(GetPackageName(), mpDataTollboothNumTop, "%d", mPlayerTollboothNumber);
        FEPrintf(GetPackageName(), mpDataTollboothNumBot, "%d", mNumTollbooths);
    } else {
        if (!FEngIsScriptSet(mDataPositionGroup, FEHASH_INIT)) {
            FEngSetScript(mDataPositionGroup, FEHASH_INIT, true);
        }
        if (!FEngIsScriptSet(mDataIconTollbooth, FEHASH_HIDE)) {
            FEngSetScript(mDataIconTollbooth, FEHASH_HIDE, true);
        }
        FEPrintf(GetPackageName(), mpDataRacePosNum, "%d", mPlayerPosition);
        FEPrintf(GetPackageName(), mpDataRacerCount, "%d", mNumRacers);
    }

    switch (GRaceStatus::Get().GetRaceType()) {
        case GRace::kRaceType_P2P:
        case GRace::kRaceType_SpeedTrap:
        case GRace::kRaceType_Tollbooth:
            FEngSetLanguageHash(mDataCompleteText, 0x59BB1918);
            FEPrintf(GetPackageName(), mpDataPercentComplete, "%d %%", static_cast<int>(mPlayerPercentComplete));
            break;
        default:

            FEngSetLanguageHash(mDataCompleteText, 0xBF9C);
            FEPrintf(GetPackageName(), mpDataPercentComplete, "%d/%d", mPlayerLapNumber, mNumLaps);
    }

    if (mSuddenDeath) {
        FEngSetLanguageHash(mDataCurrentLapTime, 0x733C8147);
    } else {
        Timer timer = Timer(mPlayerLapTime);
        char timeToPrint[16];
        timer.PrintToString(timeToPrint, 4);
        FEPrintf(mDataCurrentLapTime, "%s", timeToPrint);
    }
}
