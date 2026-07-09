#include "MusicFlow.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

extern int gXMP_DOWNSTATE;

namespace Speech {

MusicFlow::MusicFlow()
    : mStartDelay(false), //
      mStartEvent(0), //
      mTimer(0), //
      mBoostTimer(0), //
      mT_currPiece(0), //
      mElapsed(0.0f), //
      mIntensity(0.0f), //
      mAvgNumCopsInForm(0.0f), //
      mAvgNumCopsLOS(0.0f), //
      mAvgPlayerSpeed(0.0f), //
      mAvgPursuitDist(0.0f), //
      mCurrentPart(-1), //
      mRestrained(true), //
      mTopSpeed(0.0f), //
      mTimeInPiece(0.0f), //
      mRequestedSwap(false), //
      mX360UserTunes(false), //
      mMsgNewPart(0), //
      mMsgInitFlow(0), //
      mMsgTerminate(0), //
      mMsgDone(0), //
      mMsgX360UserTunes(0) {
    mMsgNewPart = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageNewPart, "PartUpdate", 0);
    mMsgInitFlow = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageInitFlow, "Init", 0);
    mMsgTerminate = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageTerminate, "Terminate", 0);
    mMsgDone = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageDone, "InteractiveDone", 0);
    mMsgX360UserTunes = Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageX360UserTunes, "X360UserTunes", 0);
    mState = kTransition;
    mBusy = 0;
}

MusicFlow::~MusicFlow() {
    if (mMsgNewPart) {
        Hermes::Handler::Destroy(mMsgNewPart);
        mMsgNewPart = 0;
    }
    if (mMsgInitFlow) {
        Hermes::Handler::Destroy(mMsgInitFlow);
        mMsgInitFlow = 0;
    }
    if (mMsgTerminate) {
        Hermes::Handler::Destroy(mMsgTerminate);
        mMsgTerminate = 0;
    }
    if (mMsgDone) {
        Hermes::Handler::Destroy(mMsgDone);
        mMsgDone = 0;
    }
    if (mMsgX360UserTunes) {
        Hermes::Handler::Destroy(mMsgX360UserTunes);
        mMsgX360UserTunes = 0;
    }
}

void MusicFlow::MessageNewPart(const MNotifyMusicFlow &message) {
    mCurrentPart = message.GetPart();
    mStartDelay = false;
    mT_currPiece = WorldTimer;
}

void MusicFlow::MessageInitFlow(const MNotifyMusicFlow &message) {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    IPlayer *player;
    IVehicleAI *vai;

    ChangeStateTo(kWaiting);
    player = IPlayer::First(PLAYER_LOCAL);
    player->GetSimable()->QueryInterface(&vai);
    if (vai) {
        float top_speed_mps = vai->GetTopSpeed();
        mTopSpeed = MPS2MPH(top_speed_mps);
    } else {
        mTopSpeed = 160.0f;
    }

    mTimer = WorldTimer;
    mStartDelay = true;
    mStartEvent = message.GetPart();
}

void MusicFlow::MessageTerminate(const MNotifyMusicFlow &) {
    ChangeStateTo(kTerminal);
}

void MusicFlow::MessageDone(const MNotifyMusicFlow &) {
    mRequestedSwap = false;
    mBusy = 0;
}

void MusicFlow::MessageX360UserTunes(const MNotifyMusicFlow &message) {
    mX360UserTunes = message.GetPart() != 0;
    mRequestedSwap = mX360UserTunes;
}

void MusicFlow::Reacquire() {
    mElapsed = 0.0f;
    mIntensity = 0.0f;
    mTopSpeed = 0.0f;
    mTimeInPiece = 0.0f;
    mRequestedSwap = false;
    mStartDelay = false;
    mT_currPiece = WorldTimer;
    if (mState == kTransition) {
        ChangeStateTo(kNeutral);
    }
}

void MusicFlow::Update() {
    if ((g_pEAXSound->GetCurMusicVolume() == 0.0f) ||
        (g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode == 0) || (gXMP_DOWNSTATE != 0)) {
        if (mState != kTransition) {
            Reset();
        }
        return;
    }

    if (mState != kTransition) {
        SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
        mElapsed = (WorldTimer - mTimer).GetSeconds();
        mTimeInPiece = (WorldTimer - mT_currPiece).GetSeconds();
        mAvgNumCopsInForm = mAvgNumCopsInForm * 0.97f + static_cast<float>(ai->GetCopsInFormation().size()) * 0.03f;
        mAvgNumCopsLOS = mAvgNumCopsLOS * 0.97f + static_cast<float>(ai->NumCopsWithLOS()) * 0.03f;
        mAvgPlayerSpeed = mAvgPlayerSpeed * 0.01f + ai->GetPlayerSpeed() * 0.99f;
        if (ai->GetPursuitDistance() > 0.0f) {
            mAvgPursuitDist = mAvgPursuitDist * 0.97f + ai->GetPursuitDistance() * 0.03f;
        } else {
            mAvgPursuitDist = mAvgPursuitDist * 0.97f + mAvgPursuitDist * 0.03f;
        }
        mTimer = WorldTimer;
    }

    switch (mState) {
    case kNeutral:
        Neutral();
        break;
    case kLose:
        Lose();
        break;
    case kWin:
        Win();
        break;
    case kElude:
        Elude();
        break;
    case kWaiting:
        Waiting();
        break;
    case kTerminal:
        Terminal();
        break;
    default:
        break;
    }

    if (mRequestedSwap) {
        if ((mState != kWaiting) && (mState != kTerminal) && (mState != kTransition)) {
            mRequestedSwap = false;
        }
    }

    if (mState != kTransition) {
        int intensity = static_cast<int>(mIntensity * 127.0f);
        MControlPathfinder message(false, 0x20, intensity, 0);
        message.Send(UCrc32("Control"));
    }
}

float MusicFlow::UpdateIntensity(float adj) {
    mIntensity += adj;
    if (mIntensity < static_cast<float>(kLow)) {
        mIntensity = static_cast<float>(kLow);
    }
    if (mIntensity > static_cast<float>(kHigh)) {
        mIntensity = static_cast<float>(kHigh);
    }
    return mIntensity;
}

void MusicFlow::Waiting() {
    if (mStartDelay && (mElapsed > 1.0f)) {
        mStartDelay = false;
        MControlPathfinder(false, 0x11, 0, 0).Send(UCrc32("Event"));
        mT_currPiece = WorldTimer;
    }

    if (mRequestedSwap) {
        mT_currPiece = WorldTimer;
    }
}

void MusicFlow::Neutral() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    bool in_cooldown;

    if (ai->GetPursuit()) {
        in_cooldown = ai->GetPursuit()->GetPursuitStatus() == 2;
    } else {
        in_cooldown = false;
    }

    int *busy = &mBusy;
    if (in_cooldown) {
        *busy = 0;
    } else {
        int busy_value;
        if (mElapsed < 30.0f || mRestrained) {
            busy_value = mBusy;
            mBusy = busy_value + 1;
        } else {
            busy_value = 0;
        }
        *busy = busy_value;
    }

    UpdateIntensity(mAvgPursuitDist < 32.0f ? 1.0f : 0.0f);
    UpdateIntensity(ai->GetTimeLastCrashed() >= 2.5f ? mIntensity : 1.0f);
    UpdateIntensity(ai->GetTimeLastNailedCop() >= 2.5f ? mIntensity : 1.0f);

    if (mRestrained) {
        mIntensity = bClamp(mIntensity, 0.0f, 0.5f);
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kTerminal);
        return;
    }

    if (mRestrained && !in_cooldown) {
        if (mElapsed <= 30.0f && ai->GetFocus() != 2) {
            return;
        }
        mRestrained = in_cooldown;
        mIntensity = 1.0f;
        mTimer = WorldTimer;
    }

    if (ai->GetPlayerSpeed() < mTopSpeed * 0.125f) {
        if (ai->GetPursuit()->IsCollapseActive()) {
            mIntensity = 0.86f;
            ChangeStateTo(kLose);
            return;
        }

        if (ai->GetPlayerSpeed() < mTopSpeed * 0.125f && ai->GetPursuitState() == SoundAI::kSearching &&
            ai->GetPerpLostTime() > 4.0f && ai->GetTimeLastCrashed() > 4.0f) {
            ChangeStateTo(kElude);
            return;
        }
    }

    if (mBusy != 0) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        return;
    }

    if (in_cooldown) {
        if (ai->GetPlayerSpeed() >= mTopSpeed * 0.35f) {
            ChangeStateTo(kWin);
        } else {
            ChangeStateTo(kElude);
        }
    } else {
        if (mAvgPlayerSpeed < mTopSpeed * 0.125f && ai->GetPursuitState() == SoundAI::kSearching) {
            ChangeStateTo(kElude);
            return;
        }

        if (mAvgPlayerSpeed > mTopSpeed * 0.5f) {
            ChangeStateTo(kWin);
            return;
        }

        if (mAvgPursuitDist >= 32.0f) {
            return;
        }
        ChangeStateTo(kLose);
        mIntensity = 0.35f;
    }
}

void MusicFlow::Lose() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    bool in_cooldown;

    int *busy = &mBusy;
    int busy_value;
    if (mElapsed < 60.0f) {
        busy_value = mBusy;
        mBusy = busy_value + 1;
    } else {
        busy_value = 0;
    }
    *busy = busy_value;

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kTerminal);
        return;
    }

    if (ai->GetPlayerSpeed() < mTopSpeed * 0.125f && ai->GetPursuit()->TimeUntilBusted() > 0.5f && ai->IsHighIntensity()) {
        mIntensity = 1.0f;
    } else {
        if (mIntensity >= 0.93f) {
            mIntensity = 0.8f;
        }
        UpdateIntensity(ai->GetPlayerSpeed() < mTopSpeed * 0.375f ? 0.86f : 0.0f);
        UpdateIntensity(ai->GetTimeLastCrashed() >= 2.5f ? mIntensity : 0.86f);
        UpdateIntensity(ai->GetTimeLastNailedCop() >= 2.5f ? mIntensity : 0.86f);
    }

    if (ai->GetPursuit()) {
        in_cooldown = ai->GetPursuit()->GetPursuitStatus() == 2;
    } else {
        in_cooldown = false;
    }

    if ((ai->GetPursuitState() == SoundAI::kSearching || in_cooldown) && ai->GetPerpLostTime() > 4.0f) {
        if (ai->GetPlayerSpeed() < mTopSpeed * 0.65f) {
            ChangeStateTo(kElude);
            mIntensity = 1.0f;
        } else {
            ChangeStateTo(kWin);
            mIntensity = 1.0f;
        }
    } else {
        if (mBusy != 0) {
            return;
        }

        if (mAvgPlayerSpeed <= mTopSpeed * 0.5f) {
            float t_lost = ai->GetPerpLostTime();
            if (t_lost <= 5.0f) {
                if (mElapsed <= 90.0f) {
                    return;
                }
            }
        }

        ChangeStateTo(kNeutral);
        mIntensity = 0.35f;
    }
}

void MusicFlow::RequestSwap() {
    mRequestedSwap = true;
}

void MusicFlow::Win() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    int *busy = &mBusy;
    int busy_value;
    if (mElapsed < 45.0f) {
        busy_value = mBusy;
        mBusy = busy_value + 1;
    } else {
        busy_value = 0;
    }
    *busy = busy_value;

    UpdateIntensity(mAvgPlayerSpeed > (mTopSpeed * 0.625f) ? 1.0f : 0.0f);

    if ((ai->GetTimeLastCrashed() < 5.0f) || (ai->GetTimeLastNailedCop() < 5.0f)) {
        mIntensity = 0.25f;
    }

    if ((ai->GetTimeLastCrashed() > 10.0f) && (ai->GetTimeLastNailedCop() > 10.0f) && (ai->GetPlayerSpeed() > (mTopSpeed * 0.8f))) {
        float t_lastboosted = (WorldTimer - mBoostTimer).GetSeconds();
        if (t_lastboosted > 10.0f) {
            mIntensity *= 1.15f;
            mBoostTimer = WorldTimer;
        }
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kTerminal);
        return;
    }

    if ((ai->GetPlayerSpeed() < (mTopSpeed * 0.125f)) && ai->GetPursuit()->IsCollapseActive()) {
        ChangeStateTo(kLose);
        return;
    }

    bool in_cooldown;
    if (ai->GetPursuit()) {
        in_cooldown = ai->GetPursuit()->GetPursuitStatus() == PS_COOL_DOWN;
    } else {
        in_cooldown = false;
    }

    if (mAvgPlayerSpeed < (mTopSpeed * 0.125f)) {
        if ((ai->GetPursuitState() == SoundAI::kSearching) || in_cooldown) {
            ChangeStateTo(kElude);
            return;
        }
    }

    if (mBusy != 0) {
        return;
    }

    if (mElapsed <= 90.0f) {
        return;
    }

    ChangeStateTo(kNeutral);
}

void MusicFlow::Elude() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();

    int *busy = &mBusy;
    int busy_value;
    if (mElapsed < 5.0f) {
        busy_value = mBusy;
        mBusy = busy_value + 1;
    } else {
        busy_value = 0;
    }
    *busy = busy_value;

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        ChangeStateTo(kTerminal);
        return;
    }

    UpdateIntensity(mAvgPlayerSpeed > 10.0f ? 1.0f : 0.0f);

    if (ai->GetPursuitState() != SoundAI::kActive) {
        goto not_active;
    }

    if (ai->GetPursuitDistance() > 50.0f) {
        goto neutral;
    }
    ChangeStateTo(kLose);
    return;

neutral:
    ChangeStateTo(kNeutral);
    mIntensity = 0.8f;
    return;

not_active:
    if (mAvgPlayerSpeed <= mTopSpeed * 0.45f) {
        return;
    }
    ChangeStateTo(kWin);
}

void MusicFlow::Terminal() {
    if (Manager::IsCopSpeechBusy()) {
        return;
    }
    ChangeStateTo(kTransition);
}

bool MusicFlow::IsTransitionable() {
    return mState == kTransition;
}

void MusicFlow::ChangeStateTo(int new_state) {
    if (new_state != mState) {
        SoundAI *ai = SoundAI::Get();
        SpeechFlow::ChangeStateTo(kWaiting);
        int nevt = -1;
        Timer timer = WorldTimer;
        mBusy = 1;
        mTimer = timer;
        switch (new_state) {
            case kNeutral:
                if (mCurrentPart != 4) {
                    nevt = 0xa;
                }
                break;
            case kLose:
                if (mCurrentPart != 1) {
                    nevt = 0xb;
                }
                break;
            case kWin:
                if (mCurrentPart != 3) {
                    nevt = 0xc;
                }
                break;
            case kElude:
                mAvgPlayerSpeed = 0.0f;
                if (mCurrentPart != 2) {
                    nevt = 0xd;
                }
                break;
            case kTerminal:
                if ((static_cast<unsigned int>(mCurrentPart - 5) > 2) && (ai->GetFocus() != kTerminal) && (ai->GetFocus() != kTransition)) {
                    nevt = 0xe;
                }
                break;
        }
        if (nevt != -1) {
            MControlPathfinder message(false, static_cast<unsigned int>(nevt), 0, 0);
            message.Send(UCrc32("Event"));
        }
    }
}

void MusicFlow::Reset() {
    mStartDelay = false;
    mStartEvent = 0;
    mTimer = Timer(0);
    mBoostTimer = Timer(0);
    mT_currPiece = Timer(0);
    mElapsed = 0.0f;
    mIntensity = 0.0f;
    mAvgNumCopsInForm = 0.0f;
    mAvgNumCopsLOS = 0.0f;
    mAvgPlayerSpeed = 0.0f;
    mAvgPursuitDist = 0.0f;
    mCurrentPart = -1;
    mRestrained = true;
    mTopSpeed = 0.0f;
    mTimeInPiece = 0.0f;
    mRequestedSwap = false;
    mX360UserTunes = false;
    mBusy = 0;
    mState = kTransition;
    mLastState = kTransition;
}

} // namespace Speech
