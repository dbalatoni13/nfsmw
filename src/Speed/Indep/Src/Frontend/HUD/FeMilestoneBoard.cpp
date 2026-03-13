#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

void FEngSetScript(FEObject *obj, unsigned int script_hash, bool start_at_beginning);

MilestoneBoard::MilestoneBoard(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , IMilestoneBoard(pOutter)
{
}

void MilestoneBoard::Update(IPlayer *player) {
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
