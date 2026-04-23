#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/FEng/feimage.h"

void FEngSetScript(FEObject *obj, unsigned int script_hash, bool start_at_beginning);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
bool FEngIsScriptRunning(FEObject *obj, unsigned int script_hash);
void FEngSetTextureHash(FEImage *image, unsigned int hash);
FEColor FEngGetObjectColor(FEObject *obj);
void FEngSetColor(FEObject *obj, unsigned int color);
int FEPrintf(FEString *str, const char *fmt, ...);

inline unsigned int FEngGetColor(FEObject *obj) {
    return FEngGetObjectColor(obj);
}

MilestoneBoard::MilestoneBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x400000000ULL) //
    , IMilestoneBoard(pOutter)
{
    mInPursuit = false;
    mChallengeSeries = false;
    mPlayerBinNumber = FEDatabase->GetCareerSettings()->GetCurrentBin();
    mScrollTimer = Timer(0);
    mNumMilestones = 0;
    mMilestoneSetVisible = 0;

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

            if (!FEngIsScriptSet(mpDataMilestoneInfoGroup, 0x5079c8f8)) {
                FEngSetScript(mpDataMilestoneInfoGroup, 0x5079c8f8, true);
            }
            if (!FEngIsScriptSet(mpDataMilestoneIconGroup, 0x5079c8f8)) {
                FEngSetScript(mpDataMilestoneIconGroup, 0x5079c8f8, true);
            }

            FEPrintf(mpDataMilestonesTotal, "%d", GetNumCompleteMilestones());

            if (numIncompleteMilestones > 1) {
                if (!mScrollTimer.IsSet()) {
                    mScrollTimer = WorldTimer;
                    mMilestoneSetVisible = GetFirstIncompleteMilestone();
                } else {
                    Timer elapsed = WorldTimer - mScrollTimer;
                    if (elapsed.GetSeconds() >= 5.0f) {
                        if (FEngIsScriptSet(mpDataDetailsGroup, 0x1ca7c0)) {
                            FEngSetScript(mpDataDetailsGroup, 0xaff37f61, true);
                        } else if (FEngIsScriptSet(mpDataDetailsGroup, 0xaff37f61) &&
                                   !FEngIsScriptRunning(mpDataDetailsGroup, 0xaff37f61)) {
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
                        if (!FEngIsScriptSet(mpDataIconBackings[i], 0x249db7b7) &&
                            !FEngIsScriptRunning(mpDataIconBackings[i], 0x3826a28)) {
                            FEngSetScript(mpDataIconBackings[i], 0x249db7b7, true);
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
                    float alpha = 0.5f;
                    if (GetIsMilestoneComplete(i)) {
                        alpha = 1.0f;
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
                FEDatabase->SetMilestoneDescriptionString(outputStr, mMilestones[idx].mType,
                    mMilestones[idx].mCurrVal, mMilestones[idx].mGoal, true);
                FEPrintf(mpDataMilestoneGoal, "%s", outputStr);
            }
            return;
        }
    }
    mScrollTimer.UnSet();
    if (FEngIsScriptSet(mpDataMilestoneInfoGroup, 0x5079c8f8)) {
        FEngSetScript(mpDataMilestoneInfoGroup, 0x33113ac, true);
    }
    if (FEngIsScriptSet(mpDataMilestoneIconGroup, 0x5079c8f8)) {
        FEngSetScript(mpDataMilestoneIconGroup, 0x33113ac, true);
    }
}

void MilestoneBoard::SetInPursuit(bool inPursuit) {
    mInPursuit = inPursuit;
}

void MilestoneBoard::SetChallengeSeries(bool challenge) {
    mChallengeSeries = challenge;
}

void MilestoneBoard::SetNumberOfMilestones(int num) {
    mNumMilestones = num;
}

void MilestoneBoard::SetMilestoneComplete(int milestoneNum, bool complete) {
    mMilestones[milestoneNum].mComplete = complete;
}

bool MilestoneBoard::GetIsMilestoneComplete(int index) const {
    if (index >= mNumMilestones) return true;
    return mMilestones[index].mComplete;
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

int MilestoneBoard::GetFirstIncompleteMilestone() const {
    for (int i = 0; i < mNumMilestones; i++) {
        if (!mMilestones[i].mComplete) {
            return i;
        }
    }
    return 0;
}

int MilestoneBoard::GetNextVisibleMilestone() const {
    if (GetNumIncompleteMilestones() > 1) {
        int next = mMilestoneSetVisible + 1;
        if (next >= mNumMilestones) {
            next = 0;
        }
        while (mMilestones[next].mComplete) {
            next++;
            if (next >= mNumMilestones) {
                next = 0;
            }
        }
        return next;
    }
    return mMilestoneSetVisible;
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
