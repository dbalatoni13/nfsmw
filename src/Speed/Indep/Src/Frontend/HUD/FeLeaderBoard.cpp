#include "Speed/Indep/Src/Frontend/HUD/FeLeaderBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

extern char *GetTranslatedString(int hash);
extern const char *GetLocalizedString(unsigned int hash);
int bSNPrintf(char *dest, int maxlen, const char *fmt, ...);
int FEngSNPrintf(char *dest, int size, const char *fmt, ...);
unsigned long FEHashUpper(const char *String);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
unsigned int bStringHash(const char *str);
int FEPrintf(FEString *str, const char *fmt, ...);
bool FEngIsScriptSet(FEObject *obj, unsigned int hash);
bool FEngIsScriptRunning(FEObject *obj, unsigned int hash);
void FEngSetScript(FEObject *obj, unsigned int hash, bool set);
void FEngSetInvisible(FEObject *obj);

LeaderBoard::LeaderBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x18) //
    , ILeaderBoard(pOutter)
{
    mNumRacers = -1;
    mPlayerIndex = -1;
    mSplitTimeQueued = false;
    mNumFramesBeforeTogglingPlayerTimes = 90;
    mShowingRacerTimes = false;
    mDataLeaderboardGroup = static_cast<FEGroup *>(RegisterGroup(FEHashUpper("LeaderBoardGroup")));

    char sztemp[32];
    for (int i = 0; i < 4; i++) {
        FEngSNPrintf(sztemp, 32, "LBData_%d", i + 1);
        mDataRacerText[i] = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper(sztemp)));
        FEngSNPrintf(sztemp, 32, "LeaderText_%d", i + 1);
        mDataRacerNum[i] = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper(sztemp)));
        FEngSNPrintf(sztemp, 32, "OL_ICON_%d", i + 2);
        mDataRacerIcon[i] = static_cast<FEImage *>(FEngFindObject(pkg_name, FEHashUpper(sztemp)));
        FEngSNPrintf(sztemp, 32, "LBBacking_%d", i + 1);
        mDataRacerTextBackings[i] = static_cast<FEImage *>(FEngFindObject(pkg_name, FEHashUpper(sztemp)));
    }

    for (int i = 0; i < 4; i++) {
        mTopRacers[i].mHasHeadset = false;
        mTopRacers[i].mIsBusted = false;
        mTopRacers[i].mIsKoed = false;
        mTopRacers[i].mNumLapsCompleted = 0;
        mTopRacers[i].mPercentComplete = 0.0f;
        mTopRacers[i].mRacerNum = 0;
        mTopRacers[i].mRaceTimeOfLastLap = 0.0f;
        mTopRacers[i].mTotalPoints = 0.0f;
        bStrCpy(mTopRacers[i].mRacerName, "");
        for (int j = 0; j < 20; j++) {
            mTopRacers[i].mRaceTimeOfSegment[j] = 0.0f;
        }
    }
}

extern const char lbl_803E4CF0[];
extern const char lbl_803E48C8[];
extern const char lbl_803E4FF8[];
extern const char lbl_803E5004[];
extern const char lbl_803E500C[];
extern const char lbl_803E5018[];

void LeaderBoard::Update(IPlayer *player) {
    if (player->GetSettings()->LeaderboardOn) {
        if (!FEngIsScriptSet(mDataLeaderboardGroup, 0x001744B3)) {
            FEngSetScript(mDataLeaderboardGroup, 0x001744B3, true);
        }

        --mNumFramesBeforeTogglingPlayerTimes;
        if (mNumFramesBeforeTogglingPlayerTimes <= 0) {
            bool toggleRacerTimesNow = false;
            for (int i = 0; i < mNumRacers && i < 4; i++) {
                if (mNumFramesBeforeTogglingPlayerTimes == 0) {
                    FEngSetScript(mDataRacerText[i], 0x033113AC, true);
                } else {
                    if (FEngIsScriptSet(mDataRacerText[i], 0x033113AC) &&
                        !FEngIsScriptRunning(mDataRacerText[i], 0x033113AC) &&
                        !FEngIsScriptSet(mDataRacerText[i], 0x5079C8F8)) {
                        FEngSetScript(mDataRacerText[i], 0x5079C8F8, true);
                        toggleRacerTimesNow = true;
                    }
                }
            }
            if (toggleRacerTimesNow) {
                mShowingRacerTimes ^= 1;
                mNumFramesBeforeTogglingPlayerTimes = 90;
            }
        }

        int numRacerNumToClearFrom = mNumRacers;
        if (mNumRacers > 1) {
            for (int i = 0; i < mNumRacers && i < 4; i++) {
                if (mShowingRacerTimes) {
                    if (mTopRacers[i].mIsBusted) {
                        FEPrintf(mDataRacerText[i], lbl_803E4CF0, GetTranslatedString(0x532b5186));
                    } else if (mTopRacers[i].mIsKoed) {
                        FEPrintf(mDataRacerText[i], lbl_803E4CF0, GetTranslatedString(0x5d82dba2));
                    } else if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_SpeedTrap) {
                        float val = mTopRacers[i].mTotalPoints;
                        unsigned int unit = 0x8569a25f;
                        if (!FEDatabase->GetGameplaySettings()->SpeedoUnits) {
                            val = MPS2MPH(KPH2MPS(val));
                            unit = 0x8569ab44;
                        }
                        FEPrintf(mDataRacerText[i], lbl_803E4FF8, val, GetLocalizedString(unit));
                    } else if (i == mPlayerIndex) {
                        FEPrintf(mDataRacerText[i], lbl_803E5004);
                    } else {
                        int unit = 0xe2078322;
                        float totalRaceLenMetres = GRaceStatus::Get().GetRaceLength();
                        if (!FEDatabase->GetGameplaySettings()->SpeedoUnits) {
                            totalRaceLenMetres = METERS2FT(totalRaceLenMetres);
                            unit = 0x2e8496b1;
                        }
                        if (mTopRacers[i].mPercentComplete >= mTopRacers[mPlayerIndex].mPercentComplete) {
                            float pctDiff = (mTopRacers[i].mPercentComplete - mTopRacers[mPlayerIndex].mPercentComplete) * 0.01f;
                            pctDiff = pctDiff * totalRaceLenMetres;
                            FEPrintf(mDataRacerText[i], lbl_803E500C, pctDiff, GetTranslatedString(unit));
                        } else {
                            float pctDiff = (mTopRacers[mPlayerIndex].mPercentComplete - mTopRacers[i].mPercentComplete) * 0.01f;
                            pctDiff = pctDiff * totalRaceLenMetres;
                            FEPrintf(mDataRacerText[i], lbl_803E5018, pctDiff, GetTranslatedString(unit));
                        }
                    }
                } else {
                    FEPrintf(mDataRacerText[i], lbl_803E4CF0, mTopRacers[i].mRacerName);
                    FEPrintf(mDataRacerNum[i], lbl_803E48C8, mTopRacers[i].mRacerNum);
                }
            }
            if (numRacerNumToClearFrom <= 1) {
                numRacerNumToClearFrom = 0;
            }
        } else {
            numRacerNumToClearFrom = 0;
        }

        for (int i = numRacerNumToClearFrom; i <= 3; i++) {
            FEngSetInvisible(mDataRacerText[i]);
            FEngSetInvisible(mDataRacerNum[i]);
            FEngSetInvisible(mDataRacerIcon[i]);
            FEngSetInvisible(mDataRacerTextBackings[i]);
        }

        for (int i = 0; i <= 3; i++) {
            if (i == mPlayerIndex) {
                if (!FEngIsScriptSet(mDataRacerTextBackings[i], 0x249db7b7)) {
                    FEngSetScript(mDataRacerTextBackings[i], 0x249db7b7, true);
                }
            } else {
                if (!FEngIsScriptSet(mDataRacerTextBackings[i], 0x001744B3)) {
                    FEngSetScript(mDataRacerTextBackings[i], 0x001744B3, true);
                }
            }
        }
    } else {
        if (!FEngIsScriptSet(mDataLeaderboardGroup, 0x0016A259)) {
            FEngSetScript(mDataLeaderboardGroup, 0x0016A259, true);
        }
    }

    if (mSplitTimeQueued && ShowSplitTime(player)) {
        mSplitTimeQueued = false;
    }
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

void LeaderBoard::SetRacerNumLapsCompleted(int pos, int numLaps, float time, IPlayer *player) {
    if (pos > 3) return;
    if (numLaps > 0 && numLaps < mNumLaps &&
        numLaps > mTopRacers[pos].mNumLapsCompleted && pos == mPlayerIndex) {
        ShowLapTime(player);
    }
    mTopRacers[pos].mRaceTimeOfLastLap = time;
    mTopRacers[pos].mNumLapsCompleted = numLaps;
}

void LeaderBoard::SetRacerName(int pos, const char *name) {
    if (pos > 3) return;
    bStrCpy(mTopRacers[pos].mRacerName, name);
}

void LeaderBoard::SetRacerIsBusted(int pos, bool busted) {
    mTopRacers[pos].mIsBusted = busted;
}

void LeaderBoard::SetRacerIsKoed(int pos, bool koed) {
    mTopRacers[pos].mIsKoed = koed;
}

void LeaderBoard::SetRacerPercentComplete(int pos, float percent, float time, IPlayer *player) {
    if (pos > 3) return;
    if (percent > 0.0f && percent < 100.0f) {
        int ipercent = static_cast<int>(percent * static_cast<float>(mNumLaps));
        if (ipercent > static_cast<int>(mTopRacers[pos].mPercentComplete * static_cast<float>(mNumLaps))) {
            bool showSplitTime = false;
            int index = 0;
            GRace::Type raceType = GRaceStatus::Get().GetRaceType();
            switch (raceType) {
            case GRace::kRaceType_Circuit:
            case GRace::kRaceType_Knockout: {
                int q = ipercent / 50;
                if (ipercent == q * 50) {
                    showSplitTime = true;
                    index = q;
                }
                break;
            }
            case GRace::kRaceType_P2P:
            case GRace::kRaceType_Tollbooth: {
                int q = ipercent / 25;
                if (ipercent == q * 25) {
                    showSplitTime = true;
                    index = q;
                }
                break;
            }
            default:
                break;
            }
            if (showSplitTime) {
                if (mTopRacers[pos].mRaceTimeOfSegment[index] == 0.0f) {
                    mTopRacers[pos].mRaceTimeOfSegment[index] = time;
                    if (mPlayerIndex == 0) {
                        if (pos == 1) {
                            mSplitTimeQueued = true;
                        }
                    } else if (pos == mPlayerIndex) {
                        mSplitTimeQueued = true;
                    }
                }
            }
        }
    }
    mTopRacers[pos].mPercentComplete = percent;
}

extern const char lbl_803E4CF4[];

bool LeaderBoard::ShowLapTime(IPlayer *player) const {
    IHud *hud = player->GetHud();
    if (!hud) return false;
    IGenericMessage *igenericmessage;
    if (!hud->QueryInterface(&igenericmessage)) return false;
    if (igenericmessage->IsGenericMessageShowing()) return false;

    Timer timer(mTopRacers[mPlayerIndex].mRaceTimeOfLastLap);
    char timeToPrint[16];
    char messageString[32];
    timer.PrintToString(timeToPrint, 4);
    const char *fmt = lbl_803E4CF4;
    char *translated = GetTranslatedString(0x7A6F9F0A);
    bSNPrintf(messageString, 32, fmt, translated, timeToPrint);
    igenericmessage->RequestGenericMessage(
        messageString, false, 0x8AB83EDB, bStringHash("TIMER_ICON"), 0x609F6B15,
        GenericMessage_Priority_3);
    return true;
}

extern const char lbl_803E5048[];
extern const char lbl_803E5050[];

bool LeaderBoard::ShowSplitTime(IPlayer *player) {
    if (player->GetSettings()->SplitTimeType == 4) {
        mSplitTimeQueued = false;
        return false;
    }

    IGenericMessage *igenericmessage;
    IHud *hud = player->GetHud();
    if (!hud) {
        return false;
    }

    if (!hud->QueryInterface(&igenericmessage)) {
        return false;
    }

    if (igenericmessage->IsGenericMessageShowing()) {
        return false;
    }

    Timer timer;
    int index;
    int divisor = 50;
    if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_P2P ||
        GRaceStatus::Get().GetRaceType() == GRace::kRaceType_Tollbooth) {
        divisor = 25;
    }

    if (mPlayerIndex == 0) {
        int ipercent = static_cast<int>(mTopRacers[1].mPercentComplete * static_cast<float>(mNumLaps));
        index = ipercent / divisor;
        timer = Timer(mTopRacers[1].mRaceTimeOfSegment[index] - mTopRacers[0].mRaceTimeOfSegment[index]);
    } else {
        int ipercent = static_cast<int>(mTopRacers[mPlayerIndex].mPercentComplete * static_cast<float>(mNumLaps));
        index = ipercent / divisor;
        timer = Timer(mTopRacers[mPlayerIndex].mRaceTimeOfSegment[index] - mTopRacers[0].mRaceTimeOfSegment[index]);
    }

    char timeToPrint[16];
    char messageString[32];
    timer.PrintToString(timeToPrint, 4);

    int hash;
    if (mPlayerIndex == 0) {
        bSNPrintf(messageString, 32, lbl_803E5048, GetTranslatedString(0x7771a159), timeToPrint);
        hash = 0xa19bb14c;
    } else {
        bSNPrintf(messageString, 32, lbl_803E5050, GetTranslatedString(0x7771a159), timeToPrint);
        hash = 0x5230faf6;
    }

    igenericmessage->RequestGenericMessage(messageString, false, hash, 0, 0, GenericMessage_Priority_3);
    return true;
}

HudResourceManager::HudResourceManager() {
    mHudResourcesState = HRM_NOT_LOADED;
    pHudTextures = nullptr;
}
