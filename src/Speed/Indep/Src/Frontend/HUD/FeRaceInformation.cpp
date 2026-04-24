#include "Speed/Indep/Src/Frontend/HUD/FeRaceInformation.hpp"

#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

unsigned long FEHashUpper(const char *name);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
int FEPrintf(FEString *text, const char *fmt, ...);
int FEPrintf(const char *pkg_name, FEObject *obj, const char *fmt, ...);
void FEngSetLanguageHash(FEString *text, unsigned int hash);

extern const char lbl_803E4CB4[];
extern const char lbl_803E4CF0[];
extern const char lbl_803E4F38[];
extern const char lbl_803E4F50[];
extern const char lbl_803E4F60[];
extern const char lbl_803E4F70[];
extern const char lbl_803E4F80[];
extern const float lbl_803E4F94;
extern const char lbl_803E4F98[];
extern const char lbl_803E4FA0[];

RaceInformation::RaceInformation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x4000000) //
    , IRaceInformation(pOutter) //
    , mNumRacers(-1) //
    , mNumLaps(-1) //
    , mPlayerPosition(-1) //
    , mPlayerLapNumber(-1) //
    , mPlayerLapTime(lbl_803E4F94) //
    , mSuddenDeath(false) //
    , mPlayerPercentComplete(lbl_803E4F94) //
    , mPlayerTollboothNumber(0) //
    , mNumTollbooths(0) //
{
    RegisterGroup(FEHashUpper(lbl_803E4F38));
    mDataCurrentLapTime = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4F50)));
    mDataCompleteText = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4F60)));
    mDataPositionGroup = static_cast<FEGroup *>(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4F70)));
    mDataIconTollbooth = static_cast<FEImage *>(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4F80)));
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
        FEPrintf(GetPackageName(), mpDataTollboothNumTop, lbl_803E4CB4, mPlayerTollboothNumber);
        FEPrintf(GetPackageName(), mpDataTollboothNumBot, lbl_803E4CB4, mNumTollbooths);
    } else {
        if (!FEngIsScriptSet(mDataPositionGroup, 0x1744B3)) {
            FEngSetScript(mDataPositionGroup, 0x1744B3, true);
        }
        if (!FEngIsScriptSet(mDataIconTollbooth, 0x16A259)) {
            FEngSetScript(mDataIconTollbooth, 0x16A259, true);
        }
        FEPrintf(GetPackageName(), mpDataRacePosNum, lbl_803E4CB4, mPlayerPosition);
        FEPrintf(GetPackageName(), mpDataRacerCount, lbl_803E4CB4, mNumRacers);
    }

    GRace::Type raceType2 = GRaceStatus::Get().GetRaceType();

    if (raceType2 == GRace::kRaceType_P2P || raceType2 == GRace::kRaceType_Tollbooth || raceType2 == GRace::kRaceType_SpeedTrap) {
        FEngSetLanguageHash(mDataCompleteText, 0x59BB1918);
        FEPrintf(GetPackageName(), mpDataPercentComplete, lbl_803E4F98, static_cast<int>(mPlayerPercentComplete));
    } else {
        FEngSetLanguageHash(mDataCompleteText, 0xBF9C);
        FEPrintf(GetPackageName(), mpDataPercentComplete, lbl_803E4FA0, mPlayerLapNumber, mNumLaps);
    }

    if (!mSuddenDeath) {
        Timer t;
        t.SetTime(mPlayerLapTime);
        char buf[16];
        t.PrintToString(buf, 4);
        FEPrintf(mDataCurrentLapTime, lbl_803E4CF0, buf);
    } else {
        FEngSetLanguageHash(mDataCurrentLapTime, 0x733C8147);
    }
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

void RaceInformation::SetPlayerLapNumber(int lap) {
    if (lap > mNumLaps) {
        lap = mNumLaps;
    }
    mPlayerLapNumber = lap;
}
