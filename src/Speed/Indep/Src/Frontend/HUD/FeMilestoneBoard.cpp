#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

MilestoneBoard::MilestoneBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x400000000ULL), IMilestoneBoard(pOutter), mInPursuit(false), mChallengeSeries(false),
      mPlayerBinNumber(FEDatabase->GetCareerSettings()->GetCurrentBin()), mNumMilestones(0), mMilestoneSetVisible(0) {
    {
        MilestoneBoard_Milestone *p = mMilestones;
        int m = 3;
        do {
            p->mMilestoneIconHash = 0;
            p->mType = 0;
            p->mGoal = 0.0f;
            p->mCurrVal = 0.0f;
            p->mHeaderHash = 0;
            p->mComplete = false;
            p++;
        } while (m--);
    }

    mpDataMilestoneInfoGroup = RegisterGroup(FEHashUpper("MILESTONE_INFO_GROUP"));
    mpDataMilestoneIconGroup = RegisterGroup(FEHashUpper("MILESTONE_ICON_GROUP"));
    mpDataMilestonesTotal = FEngFindString(GetPackageName(), 0x894662c5);

    for (int i = 0; i < 4; i++) {
        char buf[32];
        bSPrintf(buf, "MILSTONE_ICON_%d", i + 1);
        mpDataIcons[i] = FEngFindObject(GetPackageName(), FEHashUpper(buf));
        bSPrintf(buf, "MILESTONE_ICON_BACKING_%d", i + 1);
        mpDataIconBackings[i] = FEngFindObject(GetPackageName(), FEHashUpper(buf));
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
                if (FEngIsScriptSet(mpDataDetailsBacking, 0x16a259)) {
                    FEngSetScript(mpDataDetailsBacking, 0x1ca7c0, true);
                }
                if (FEngIsScriptSet(mpDataDetailsGroup, 0x16a259)) {
                    FEngSetScript(mpDataDetailsGroup, 0x1ca7c0, true);
                }
            } else {
                if (!FEngIsScriptSet(mpDataDetailsBacking, 0x16a259)) {
                    FEngSetScript(mpDataDetailsBacking, 0x16a259, true);
                }
                if (!FEngIsScriptSet(mpDataDetailsGroup, 0x16a259)) {
                    FEngSetScript(mpDataDetailsGroup, 0x16a259, true);
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
                    Timer elapsed = WorldTimer - mScrollTimer;
                    if (elapsed.GetSeconds() >= 3.0f) {
                        if (FEngIsScriptSet(mpDataDetailsGroup, 0x1ca7c0)) {
                            FEngSetScript(mpDataDetailsGroup, FEHASH_FLIP_OUT, true);
                        } else if (FEngIsScriptSet(mpDataDetailsGroup, FEHASH_FLIP_OUT) && !FEngIsScriptRunning(mpDataDetailsGroup, FEHASH_FLIP_OUT)) {
                            FEngSetScript(mpDataDetailsGroup, FEHASH_FLIP_IN, true);
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
                        if (!FEngIsScriptSet(mpDataIconBackings[i], FEHASH_HIGHLIGHT) && !FEngIsScriptRunning(mpDataIconBackings[i], 0x3826a28)) {
                            FEngSetScript(mpDataIconBackings[i], FEHASH_HIGHLIGHT, true);
                        }
                    } else {
                        if (!FEngIsScriptSet(mpDataIconBackings[i], 0x1744b3)) {
                            FEngSetScript(mpDataIconBackings[i], 0x1744b3, true);
                        }
                    }
                } else {
                    if (!FEngIsScriptSet(mpDataIconBackings[i], 0x1744b3)) {
                        FEngSetScript(mpDataIconBackings[i], 0x1744b3, true);
                    }
                }
            }

            for (int i = 0; i < 4; i++) {
                if (i < mNumMilestones) {
                    if (!FEngIsScriptSet(mpDataIcons[i], 0x1ca7c0)) {
                        FEngSetScript(mpDataIcons[i], 0x1ca7c0, true);
                    }
                    FEngSetTextureHash(static_cast<FEImage *>(mpDataIcons[i]), mMilestones[i].mMilestoneIconHash);
                    float alpha = 1.0f;
                    if (GetIsMilestoneComplete(i)) {
                        alpha = 0.25f;
                    }
                    unsigned int colour = FEngGetColor(mpDataIcons[i]);
                    FEngSetColor(mpDataIcons[i], (colour & 0x00FFFFFF) | (static_cast<int>(alpha * 255.0f) << 24));
                } else {
                    if (!FEngIsScriptSet(mpDataIcons[i], 0x16a259)) {
                        FEngSetScript(mpDataIcons[i], 0x16a259, true);
                    }
                }
            }

            if (mMilestoneSetVisible >= 0) {
                char outputStr[32];
                int idx = mMilestoneSetVisible;
                FEDatabase->SetMilestoneDescriptionString(outputStr, mMilestones[idx].mType, mMilestones[idx].mCurrVal, mMilestones[idx].mGoal, true);
                FEPrintf(mpDataMilestoneGoal, "%s", outputStr);
            }
            return;
        }
    }
    mScrollTimer.UnSet();
    if (FEngIsScriptSet(mpDataMilestoneInfoGroup, FEHASH_APPEAR)) {
        FEngSetScript(mpDataMilestoneInfoGroup, 0x33113ac, true);
    }
    if (FEngIsScriptSet(mpDataMilestoneIconGroup, FEHASH_APPEAR)) {
        FEngSetScript(mpDataMilestoneIconGroup, 0x33113ac, true);
    }
}

int MilestoneBoard::GetNumIncompleteMilestones() const {
    int count = 0;
    for (int i = 0; i < mNumMilestones; i++) {
        if (!mMilestones[i].mComplete) {
            count++;
        }
    }
    return count;
}

int MilestoneBoard::GetNumCompleteMilestones() const {
    int count = 0;
    for (int i = 0; i < mNumMilestones; i++) {
        if (mMilestones[i].mComplete) {
            count++;
        }
    }
    return count;
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
                FEngSetScript(mpDataDetailsGroup, FEHASH_FLIP_IN, true);
                mScrollTimer = WorldTimer;
            }
        }
    }
}

bool MilestoneBoard::GetIsMilestoneComplete(int index) const {
    if (index < mNumMilestones) {
        return mMilestones[index].mComplete;
    }
    return true;
}
