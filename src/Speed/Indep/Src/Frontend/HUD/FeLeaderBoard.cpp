#include "Speed/Indep/Src/Frontend/HUD/FeLeaderBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

extern char *GetTranslatedString(int hash);
int bSNPrintf(char *dest, int maxlen, const char *fmt, ...);
unsigned int bStringHash(const char *str);

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

HudResourceManager::HudResourceManager() {
    mHudResourcesState = HRM_NOT_LOADED;
    pHudTextures = nullptr;
}
