#include "Speed/Indep/Src/Frontend/HUD/FeRaceInformation.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
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
    bool isTollbooth = false;
    if (GRaceStatus::Exists()) {
        GRace::Type raceType = GRaceStatus::Get().GetRaceType();
        if (raceType == GRace::kRaceType_Tollbooth) {
            isTollbooth = true;
        }
    }

    if (isTollbooth) {
        if (!FEngIsScriptSet(mDataPositionGroup, 0x16A259)) {
            FEngSetScript(mDataPositionGroup, 0x16A259, true);
        }
        if (!FEngIsScriptSet(mDataIconTollbooth, 0x5079C8F8)) {
            FEngSetScript(mDataIconTollbooth, 0x5079C8F8, true);
        }
        FEPrintf(GetPackageName(), mpDataTollboothNumTop, "%d", mPlayerTollboothNumber);
        FEPrintf(GetPackageName(), mpDataTollboothNumBot, "%d", mNumTollbooths);
    } else {
        if (!FEngIsScriptSet(mDataPositionGroup, 0x1744B3)) {
            FEngSetScript(mDataPositionGroup, 0x1744B3, true);
        }
        if (!FEngIsScriptSet(mDataIconTollbooth, 0x16A259)) {
            FEngSetScript(mDataIconTollbooth, 0x16A259, true);
        }
        FEPrintf(GetPackageName(), mpDataRacePosNum, "%d", mPlayerPosition);
        FEPrintf(GetPackageName(), mpDataRacerCount, "%d", mNumRacers);
    }

    GRace::Type raceType2 = GRaceStatus::Get().GetRaceType();

    if (raceType2 == GRace::kRaceType_P2P || raceType2 == GRace::kRaceType_Tollbooth || raceType2 == GRace::kRaceType_SpeedTrap) {
        FEngSetLanguageHash(mDataCompleteText, 0x59BB1918);
        FEPrintf(GetPackageName(), mpDataPercentComplete, "%d %%", static_cast<int>(mPlayerPercentComplete));
    } else {
        FEngSetLanguageHash(mDataCompleteText, 0xBF9C);
        FEPrintf(GetPackageName(), mpDataPercentComplete, "%d %%", mPlayerLapNumber, mNumLaps);
    }

    if (!mSuddenDeath) {
        Timer t = Timer(mPlayerLapTime);
        char buf[16];
        t.PrintToString(buf, 4);
        FEPrintf(mDataCurrentLapTime, "%s", buf);
    } else {
        FEngSetLanguageHash(mDataCurrentLapTime, 0x733C8147);
    }
}
