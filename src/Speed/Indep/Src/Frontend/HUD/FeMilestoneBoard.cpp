#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

MilestoneBoard::MilestoneBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x400000000ULL), mInPursuit(false), mChallengeSeries(false), IMilestoneBoard(pOutter),
      mPlayerBinNumber(FEDatabase->GetCareerSettings()->GetCurrentBin()), mScrollTimer(), mMilestones(), mNumMilestones(0), mMilestoneSetVisible(0) {
    mpDataMilestoneInfoGroup = RegisterGroup(FEHashUpper("MILESTONE_INFO_GROUP"));
    mpDataMilestoneIconGroup = RegisterGroup(FEHashUpper("MILESTONE_ICON_GROUP"));
    mpDataMilestonesTotal = FEngFindString(GetPackageName(), 0x894662c5);

    for (int i = 0; i < 4; i++) {
        char sztemp[32];
        bSPrintf(sztemp, "MILSTONE_ICON_%d", i + 1);
        mpDataIcons[i] = FEngFindObject(GetPackageName(), FEHashUpper(sztemp));
        bSPrintf(sztemp, "MILESTONE_ICON_BACKING_%d", i + 1);
        mpDataIconBackings[i] = FEngFindObject(GetPackageName(), FEHashUpper(sztemp));
    }

    mpDataDetailsBacking = FEngFindObject(GetPackageName(), 0x5c697702);
    mpDataDetailsGroup = FEngFindObject(GetPackageName(), 0xf4405ec0);
    mpDataMilestoneGoal = FEngFindString(GetPackageName(), 0xc3e48fbf);
}

void MilestoneBoard::Update(IPlayer *player) {
    int numIncompleteMilestones;

    if (mInPursuit) {
        if (mNumMilestones >= 1) {
            numIncompleteMilestones = GetNumIncompleteMilestones();
            if (numIncompleteMilestones > 0) {
                if (FEngIsScriptSet(mpDataDetailsBacking, FEHASH_HIDE)) {
                    FEngSetScript(mpDataDetailsBacking, FEHASH_SHOW, true);
                }
                if (FEngIsScriptSet(mpDataDetailsGroup, FEHASH_HIDE)) {
                    FEngSetScript(mpDataDetailsGroup, FEHASH_SHOW, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataDetailsBacking, FEHASH_HIDE)) {
                    FEngSetScript(mpDataDetailsBacking, FEHASH_HIDE, true);
                }
                if (!FEngIsScriptSet(mpDataDetailsGroup, FEHASH_HIDE)) {
                    FEngSetScript(mpDataDetailsGroup, FEHASH_HIDE, true);
                }
            }

            if (!FEngIsScriptSet(mpDataMilestoneInfoGroup, FEHASH_APPEAR)) {
                FEngSetScript(mpDataMilestoneInfoGroup, FEHASH_APPEAR, true);
            }
            if (!FEngIsScriptSet(mpDataMilestoneIconGroup, FEHASH_APPEAR)) {
                FEngSetScript(mpDataMilestoneIconGroup, FEHASH_APPEAR, true);
            }

            FEPrintf(mpDataMilestonesTotal, "%d", GetNumCompleteMilestones());

            if (numIncompleteMilestones > 1) {
                if (!mScrollTimer.IsSet()) {
                    mScrollTimer = WorldTimer;
                    mMilestoneSetVisible = GetFirstIncompleteMilestone();
                } else {
                    if ((WorldTimer - mScrollTimer).GetSeconds() >= 5.0f) {
                        if (FEngIsScriptSet(mpDataDetailsGroup, FEHASH_SHOW)) {
                            FEngSetScript(mpDataDetailsGroup, 0xaff37f61, true);
                        } else if (FEngIsScriptSet(mpDataDetailsGroup, 0xaff37f61) && !FEngIsScriptRunning(mpDataDetailsGroup, 0xaff37f61)) {
                            FEngSetScript(mpDataDetailsGroup, 0xd6c950a0, true);
                            mScrollTimer = WorldTimer;
                            mMilestoneSetVisible = GetNextVisibleMilestone();
                        }
                    }
                }
            } else if (numIncompleteMilestones == 1) {
                mMilestoneSetVisible = GetFirstIncompleteMilestone();
            } else {
                mMilestoneSetVisible = -1;
            }

            for (int i = 0; i < 4; i++) {
                if (i < mNumMilestones) {
                    if (i == mMilestoneSetVisible) {
                        if (!FEngIsScriptSet(mpDataIconBackings[i], 0x249db7b7) && !FEngIsScriptRunning(mpDataIconBackings[i], 0x3826a28)) {
                            FEngSetScript(mpDataIconBackings[i], 0x249db7b7, true);
                        }
                    } else {
                        if (!FEngIsScriptSet(mpDataIconBackings[i], FEHASH_INIT)) {
                            FEngSetScript(mpDataIconBackings[i], FEHASH_INIT, true);
                        }
                    }
                } else {
                    if (!FEngIsScriptSet(mpDataIconBackings[i], FEHASH_INIT)) {
                        FEngSetScript(mpDataIconBackings[i], FEHASH_INIT, true);
                    }
                }
            }

            for (int i = 0; i < 4; i++) {
                if (i < mNumMilestones) {
                    if (!FEngIsScriptSet(mpDataIcons[i], FEHASH_SHOW)) {
                        FEngSetScript(mpDataIcons[i], FEHASH_SHOW, true);
                    }
                    FEngSetTextureHash(static_cast<FEImage *>(mpDataIcons[i]), mMilestones[i].mMilestoneIconHash);
                    float alpha = 0.5f;
                    if (GetIsMilestoneComplete(i)) {
                        alpha = 1.0f;
                    }
                    unsigned int colour = FEngGetColor(mpDataIcons[i]) & 0x00FFFFFF;
                    FEngSetColor(mpDataIcons[i], colour | (static_cast<int>(alpha * 255.0f) << 24));
                } else {
                    if (!FEngIsScriptSet(mpDataIcons[i], FEHASH_HIDE)) {
                        FEngSetScript(mpDataIcons[i], FEHASH_HIDE, true);
                    }
                }
            }

            if (mMilestoneSetVisible >= 0) {
                char outputStr[32];
                FEDatabase->SetMilestoneDescriptionString(outputStr, mMilestones[mMilestoneSetVisible].mType,
                                                          mMilestones[mMilestoneSetVisible].mCurrVal, mMilestones[mMilestoneSetVisible].mGoal, true);
                FEPrintf(mpDataMilestoneGoal, "%s", outputStr);
            }
            return;
        }
    }
    mScrollTimer.UnSet();
    if (FEngIsScriptSet(mpDataMilestoneInfoGroup, FEHASH_APPEAR)) {
        FEngSetScript(mpDataMilestoneInfoGroup, FEHASH_LEAVE, true);
    }
    if (FEngIsScriptSet(mpDataMilestoneIconGroup, FEHASH_APPEAR)) {
        FEngSetScript(mpDataMilestoneIconGroup, FEHASH_LEAVE, true);
    }
}

int MilestoneBoard::GetNumIncompleteMilestones() const {
    int numIncompleteMilestones = 0;
    for (int i = 0; i < mNumMilestones; i++) {
        if (!mMilestones[i].mComplete) {
            numIncompleteMilestones++;
        }
    }
    return numIncompleteMilestones;
}

int MilestoneBoard::GetNumCompleteMilestones() const {
    int numCompleteMilestones = 0;
    for (int i = 0; i < mNumMilestones; i++) {
        if (mMilestones[i].mComplete) {
            numCompleteMilestones++;
        }
    }
    return numCompleteMilestones;
}

int MilestoneBoard::GetNextVisibleMilestone() const {
    int maybeNextMilestone = mMilestoneSetVisible;
    if (GetNumIncompleteMilestones() > 1) {
        maybeNextMilestone++;
        if (maybeNextMilestone >= mNumMilestones) {
            maybeNextMilestone = 0;
        }
        while (mMilestones[maybeNextMilestone].mComplete) {
            maybeNextMilestone++;
            if (maybeNextMilestone >= mNumMilestones) {
                maybeNextMilestone = 0;
            }
        }
        return maybeNextMilestone;
    }
    return maybeNextMilestone;
}

int MilestoneBoard::GetFirstIncompleteMilestone() const {
    for (int i = 0; i < mNumMilestones; i++) {
        if (!mMilestones[i].mComplete) {
            return i;
        }
    }
    return 0;
}

void MilestoneBoard::SetMilestoneComplete(int milestoneNum, bool complete) {
    mMilestones[milestoneNum].mComplete = complete;
}

void MilestoneBoard::SetMilestoneCurrValue(int milestoneNum, float currVal) {
    if (currVal < 0.0f) {
        currVal = 0.0f;
    }
    if (currVal != mMilestones[milestoneNum].mCurrVal) {
        mMilestones[milestoneNum].mCurrVal = currVal;
        if (!mMilestones[milestoneNum].mComplete) {
            if (!FEDatabase->IsMilestoneTimeFormat(mMilestones[milestoneNum].mType)) {
                mMilestoneSetVisible = milestoneNum;
                FEngSetScript(mpDataIconBackings[milestoneNum], 0x3826a28, true);
                FEngSetScript(mpDataDetailsBacking, 0x3826a28, true);
                FEngSetScript(mpDataDetailsGroup, 0xD6C950A0, true);
                mScrollTimer = WorldTimer;
            }
        }
    }
}

bool MilestoneBoard::GetIsMilestoneComplete(int index) const {
    if (index < mNumMilestones)
        return mMilestones[index].mComplete;
    return true;
}
