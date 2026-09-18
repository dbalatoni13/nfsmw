#include "GMilestone.h"

#include "GManager.h"
#include "GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/milestonetypes.h"
#include "Speed/Indep/Src/Generated/Events/EReportMilestoneAtStake.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMilestoneReached.h"
#include "Speed/Indep/Src/Lua/LuaGameHooks.h"


GMilestone::GMilestone() {
    mTypeKey = 0;
    mChallengeKey = 0;
    mState = kState_Invalid;
    mFlags = 0;
    mBinNumber = 0;
    mRequiredValue = 0.0f;
    mRecordedValue = 0.0f;
}

float GMilestone::GetCurrentValue() const {
    return GManager::Get().GetValue(mTypeKey);
}

float GMilestone::GetBounty() const {
    Attrib::Gen::gameplay challenge(mChallengeKey, 0, NULL);
    if (challenge.IsValid()) {
        return (float)challenge.Bounty();
    }
    return 0.0f;
}

int GMilestone::GetLocalizationTag() const {
    Attrib::Gen::milestonetypes type(mTypeKey, 0, NULL);
    if (type.IsValid()) {
        return type.LocalizationTag();
    }
    return 0;
}

unsigned int GMilestone::GetJumpMarkerKey() const {
    Attrib::Gen::gameplay challenge(mChallengeKey, 0, NULL);
    const GCollectionKey &marker = challenge.SpawnPoint();
    return marker.GetCollectionKey();
}

void GMilestone::Init(unsigned int challengeKey) {
    mChallengeKey = challengeKey;
    Reset();
}

void GMilestone::Reset() {
    Attrib::Gen::gameplay challenge(mChallengeKey, 0, NULL);

    mTypeKey = Attrib::StringToKey(challenge.MilestoneName());
    mState = kState_Locked;
    mBinNumber = (unsigned short)challenge.BinIndex();
    mRequiredValue = challenge.GoalEasy();
    mRecordedValue = 0.0f;
    if (GManager::Get().GetIsBiggerValueBetter(mTypeKey)) {
        mFlags |= kFlag_BiggerIsBetter;
    }
}

void GMilestone::Unlock() {
    if (mState == kState_Locked) {
        mState = kState_Available;
    }
}

bool GMilestone::ValueMeetsGoal(float value) {
    float diff = mRequiredValue - value;
    if (!(mFlags & kFlag_BiggerIsBetter)) {
        diff = -diff;
    }
    return diff <= 0.0f;
}

void GMilestone::NotifyProgress(float value) {
    if (mState == kState_Available) {
        if (ValueMeetsGoal(value)) {
            new EReportMilestoneAtStake(this);
            mState = kState_DonePendingEscape;
        }
    }
}

void GMilestone::NotifyPursuitOver(bool escaped) {
    if (mState == kState_DonePendingEscape) {
        if (escaped) {
            float value = GetCurrentValue();
            if (mFlags & kFlag_CompletionFaked) {
                value = mRequiredValue;
            }
            mRecordedValue = value;
            mState = kState_Awarded;

            Attrib::Gen::milestonetypes type(mTypeKey, 0, NULL);
            MNotifyMilestoneReached(type.CollectionName(), value).Post(0x20d60dbf);

            Attrib::Gen::gameplay challenge(mChallengeKey, 0, NULL);
            GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(mBinNumber);
            if (bin != NULL) {
                bin->RefreshProgress();
            }
            Game_AwardPlayerBounty((int)challenge.Bounty());
            Game_ChallengeCompleted();
        } else {
            mState = kState_Available;
        }
    }
}
