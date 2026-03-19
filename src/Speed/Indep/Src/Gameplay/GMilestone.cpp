#include "Speed/Indep/Src/Gameplay/GMilestone.h"

#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/milestonetypes.h"
#include "Speed/Indep/Src/Generated/Events/EReportMilestoneAtStake.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMilestoneReached.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

void Game_AwardPlayerBounty(int bounty);
void Game_ChallengeCompleted();

GMilestone::GMilestone()
    : mTypeKey(0), //
      mChallengeKey(0), //
      mState(0), //
      mFlags(0), //
      mBinNumber(0), //
      mRequiredValue(0.0f), //
      mRecordedValue(0.0f) {}

float GMilestone::GetCurrentValue() const {
    return GManager::Get().GetValue(mTypeKey);
}

float GMilestone::GetBounty() const {
    Attrib::Gen::gameplay gameplayObj(mChallengeKey, 0, nullptr);

    if (!gameplayObj.IsValid()) {
        return 0.0f;
    }

    return static_cast<float>(gameplayObj.Bounty(0));
}

int GMilestone::GetLocalizationTag() const {
    Attrib::Gen::milestonetypes milestoneType(mTypeKey, 0, nullptr);

    if (!milestoneType.IsValid()) {
        return 0;
    }

    return milestoneType.LocalizationTag();
}

unsigned int GMilestone::GetJumpMarkerKey() const {
    Attrib::Gen::gameplay gameplayObj(mChallengeKey, 0, nullptr);

    return gameplayObj.SpawnPoint(0).GetCollectionKey();
}

void GMilestone::DebugForceComplete() {
    mFlags |= kFlag_CompletionFaked;
    mState = kState_DonePendingEscape;
}

void GMilestone::Init(unsigned int challengeKey) {
    mChallengeKey = challengeKey;
    Reset();
}

void GMilestone::Reset() {
    Attrib::Gen::gameplay gameplayObj(mChallengeKey, 0, nullptr);

    mTypeKey = Attrib::StringToKey(gameplayObj.MilestoneName(0));
    mState = kState_Locked;
    mBinNumber = static_cast<unsigned short>(gameplayObj.BinIndex(0));
    mRequiredValue = gameplayObj.GoalEasy(0);
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
    float delta;

    delta = mRequiredValue - value;
    if (((mFlags ^ kFlag_BiggerIsBetter) & kFlag_BiggerIsBetter) != 0) {
        delta = -delta;
    }

    return delta <= 0.0f;
}

void GMilestone::NotifyProgress(float value) {
    if (mState == kState_Available && ValueMeetsGoal(value)) {
        new EReportMilestoneAtStake(this);
        mState = kState_DonePendingEscape;
    }
}

void GMilestone::NotifyPursuitOver(bool escaped) {
    if (mState == kState_DonePendingEscape) {
        if (escaped) {
            float currentValue = GetCurrentValue();

            if ((mFlags & kFlag_CompletionFaked) != 0) {
                currentValue = mRequiredValue;
            }

            mRecordedValue = currentValue;
            mState = kState_Awarded;

            {
                Attrib::Gen::milestonetypes milestoneType(mTypeKey, 0, nullptr);
                MNotifyMilestoneReached message(milestoneType.CollectionName(), currentValue);

                message.Post(UCrc32(0x20D60DBF));
            }

            {
                Attrib::Gen::gameplay gameplayObj(mChallengeKey, 0, nullptr);
                GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(mBinNumber);
                const int *bounty = reinterpret_cast<const int *>(gameplayObj.GetAttributePointer(0x8E1904C7, 0));

                if (bin) {
                    bin->RefreshProgress();
                }

                if (!bounty) {
                    bounty = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
                }

                Game_AwardPlayerBounty(*bounty);
                Game_ChallengeCompleted();
            }
        } else {
            mState = kState_Available;
        }
    }
}
