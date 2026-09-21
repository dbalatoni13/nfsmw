#include "Speed/Indep/Src/Speech/MusicFlow.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

int gXMP_DOWNSTATE = 0;

int MUSICFLOW_DISPLAY = 0; // Decl: 51

namespace Speech {

MusicFlow::MusicFlow()
    : mStartDelay(false),                                                                                                                 //
      mStartEvent(0),                                                                                                                     //
      mTimer(0),                                                                                                                          //
      mBoostTimer(0),                                                                                                                     //
      mT_currPiece(0),                                                                                                                    //
      mElapsed(0.0f),                                                                                                                     //
      mIntensity(0.0f),                                                                                                                   //
      mAvgNumCopsInForm(0.0f),                                                                                                            //
      mAvgNumCopsLOS(0.0f),                                                                                                               //
      mAvgPlayerSpeed(0.0f),                                                                                                              //
      mAvgPursuitDist(0.0f),                                                                                                              //
      mCurrentPart(-1),                                                                                                                   //
      mRestrained(true),                                                                                                                  //
      mTopSpeed(0.0f),                                                                                                                    //
      mX360UserTunes(false),                                                                                                              //
      mMsgNewPart(Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageNewPart, "PartUpdate", 0)),    //
      mMsgInitFlow(Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageInitFlow, "Init", 0)),        //
      mMsgTerminate(Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageTerminate, "Terminate", 0)), //
      mMsgDone(Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageDone, "InteractiveDone", 0)),     //
      mMsgX360UserTunes(Hermes::Handler::Create<MNotifyMusicFlow, MusicFlow, MusicFlow>(this, &MusicFlow::MessageX360UserTunes, "X360UserTunes", 0)) {
    this->mState = kTransition;
    this->mBusy = 0;
}

MusicFlow::~MusicFlow() {
    if (this->mMsgNewPart != nullptr) {
        Hermes::Handler::Destroy(this->mMsgNewPart);
    }
    if (this->mMsgInitFlow != nullptr) {
        Hermes::Handler::Destroy(this->mMsgInitFlow);
    }
    if (this->mMsgTerminate != nullptr) {
        Hermes::Handler::Destroy(this->mMsgTerminate);
    }
    if (this->mMsgDone != nullptr) {
        Hermes::Handler::Destroy(this->mMsgDone);
    }
    if (this->mMsgX360UserTunes != nullptr) {
        Hermes::Handler::Destroy(this->mMsgX360UserTunes);
    }
}

// TODO magic
void MusicFlow::MessageNewPart(const MNotifyMusicFlow &message) {
    SoundAI *ai = SoundAI::Get();
    this->mCurrentPart = message.GetPart();

    switch (this->mCurrentPart) {
        case 4:
            this->SpeechFlow::ChangeStateTo(kNeutral);
            return;
        case 1:
            this->SpeechFlow::ChangeStateTo(kLose);
            return;
        case 3:
            this->SpeechFlow::ChangeStateTo(kWin);
            return;
        case 2:
            this->SpeechFlow::ChangeStateTo(kElude);
            return;
        case 5:
        case 6:
        case 7:
            this->SpeechFlow::ChangeStateTo(kTerminal);
            return;
        case -1:
        case 0:
        case 8:
            this->SpeechFlow::ChangeStateTo(kTerminal);
            return;
        default:
            return;
    }
}

void MusicFlow::MessageInitFlow(const MNotifyMusicFlow &message) {
    SoundAI *ai = SoundAI::Get();

    this->ChangeStateTo(kWaiting);
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    IVehicleAI *vai;
    player->GetSimable()->QueryInterface(&vai);
    if (vai != nullptr) {
        float top_speed_mps = vai->GetTopSpeed();
        this->mTopSpeed = MPS2MPH(top_speed_mps);
    } else {
        this->mTopSpeed = 160.0f;
    }

    this->mTimer = WorldTimer;
    this->mStartDelay = true;
    this->mStartEvent = message.GetPart();
}

void MusicFlow::MessageTerminate(const MNotifyMusicFlow &message) {
    this->ChangeStateTo(kTerminal);
}

void MusicFlow::MessageDone(const MNotifyMusicFlow &message) {
    this->Reset();
}

void MusicFlow::MessageX360UserTunes(const MNotifyMusicFlow &message) {
    this->mX360UserTunes = message.GetPart() > 0;
}

void MusicFlow::Reacquire() {
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr) {
        if (this->mState == kWin || this->mState == kElude) {
            if (ai->GetPursuitDistance() <= 50.0f) {
                this->ChangeStateTo(kLose);
                this->mIntensity = 0.5f;
            } else {
                this->ChangeStateTo(kNeutral);
                this->mIntensity = 0.8f;
            }
        } else if (this->mState == kNeutral) {
            this->mIntensity = 1.0f;
        }
    }
}

void MusicFlow::Update() {
    int y;
    int x;
    if ((g_pEAXSound->GetCurMusicVolume() == 0.0f) || (g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode == 0) || (gXMP_DOWNSTATE != 0)) {
        if (this->mState != kTransition) {
            this->Reset();
        }
        return;
    }

    if (this->mState != kTransition) {
        SoundAI *ai = SoundAI::Get();
        this->mElapsed = (WorldTimer - this->mTimer).GetSeconds();
        this->mTimeInPiece = (WorldTimer - this->mT_currPiece).GetSeconds();
        this->mAvgNumCopsInForm = this->mAvgNumCopsInForm * 0.97f + static_cast<float>(ai->GetCopsInFormation().size()) * 0.03f;
        this->mAvgNumCopsLOS = this->mAvgNumCopsLOS * 0.97f + static_cast<float>(ai->NumCopsWithLOS()) * 0.03f;
        this->mAvgPlayerSpeed = this->mAvgPlayerSpeed * 0.95f + ai->GetPlayerSpeed() * 0.05f;
        this->mAvgPursuitDist =
            this->mAvgPursuitDist * 0.97f + (ai->GetPursuitDistance() > 0.0f ? ai->GetPursuitDistance() : this->mAvgPursuitDist) * 0.03f;
    }

    switch (this->mState) {
        case kNeutral:
            this->Neutral();
            break;
        case kLose:
            this->Lose();
            break;
        case kWin:
            this->Win();
            break;
        case kElude:
            this->Elude();
            break;
        case kTerminal:
            this->Terminal();
            break;
        case kWaiting:
            this->Waiting();
            break;
        case kTransition:
        default:
            break;
    }

    if (this->mRequestedSwap) {
        if ((this->mState != kWaiting) && (this->mState != kTerminal) && (this->mState != kTransition)) {
            this->mRequestedSwap = false;
        }
    }

    if (this->mState != kTransition) {
        int intensity = static_cast<int>(this->mIntensity * 127.0f);
        MControlPathfinder(false, 0, intensity, 0).Send(UCrc32("Control"));
    }
}

float MusicFlow::UpdateIntensity(float adj) {
    this->mIntensity = this->mIntensity * 0.99f + adj * 0.01f;
    this->mIntensity = bClamp(this->mIntensity, 0.0f, 1.0f);
    return this->mIntensity;
}

void MusicFlow::Waiting() {
    if (this->mStartDelay && (this->mElapsed > 0.5f)) {
        this->mStartDelay = false;
        MControlPathfinder(false, 17, 0, 0).Send(UCrc32("Event"));
        this->mT_currPiece = WorldTimer;
    }

    if (this->mRequestedSwap) {
        this->mT_currPiece = WorldTimer;
    }
}

void MusicFlow::Neutral() {
    SoundAI *ai = SoundAI::Get();

    bool in_cooldown;
    if (ai->GetPursuit() != nullptr) {
        in_cooldown = ai->GetPursuit()->GetPursuitStatus() == 2;
    } else {
        in_cooldown = false;
    }

    this->mBusy = in_cooldown ? 0 : ((this->mElapsed < 30.0f || this->mRestrained) ? this->mBusy++ : 0);

    this->UpdateIntensity(this->mAvgPursuitDist < 32.0f ? 1.0f : 0.0f);
    this->UpdateIntensity(ai->GetTimeLastCrashed() >= 2.5f ? this->mIntensity : 1.0f);
    this->UpdateIntensity(ai->GetTimeLastNailedCop() >= 2.5f ? this->mIntensity : 1.0f);

    if (this->mRestrained) {
        this->mIntensity = bClamp(this->mIntensity, 0.0f, 0.5f);
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (this->mRestrained && !in_cooldown) {
        if (this->mElapsed <= 30.0f && ai->GetFocus() != 2) {
            return;
        }
        this->mRestrained = in_cooldown;
        this->mIntensity = 1.0f;
        this->mTimer = WorldTimer;
    }

    if (ai->GetPlayerSpeed() < this->mTopSpeed * 0.125f) {
        if (ai->GetPursuit()->IsCollapseActive()) {
            this->mIntensity = 0.86f;
            this->ChangeStateTo(kLose);
            return;
        }

        if (ai->GetPlayerSpeed() < this->mTopSpeed * 0.125f && ai->GetPursuitState() == SoundAI::kSearching && ai->GetPerpLostTime() > 4.0f &&
            ai->GetTimeLastCrashed() > 4.0f) {
            this->ChangeStateTo(kElude);
            return;
        }
    }

    if (this->mBusy != 0) {
        return;
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        return;
    }

    if (in_cooldown) {
        if (ai->GetPlayerSpeed() >= this->mTopSpeed * 0.35f) {
            this->ChangeStateTo(kWin);
        } else {
            this->ChangeStateTo(kElude);
        }
    } else {
        if (this->mAvgPlayerSpeed < this->mTopSpeed * 0.125f && ai->GetPursuitState() == SoundAI::kSearching) {
            this->ChangeStateTo(kElude);
            return;
        }

        if (this->mAvgPlayerSpeed > this->mTopSpeed * 0.5f) {
            this->ChangeStateTo(kWin);
            return;
        }

        if (this->mAvgPursuitDist >= 32.0f) {
            return;
        }
        this->ChangeStateTo(kLose);
        this->mIntensity = 0.35f;
    }
}

void MusicFlow::Lose() {
    SoundAI *ai = SoundAI::Get();
    bool in_cooldown;

    this->mBusy = this->mElapsed < 45.0f ? this->mBusy++ : 0;

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if (ai->GetPlayerSpeed() < this->mTopSpeed * 0.125f && ai->GetPursuit()->TimeUntilBusted() > 0.5f && ai->IsHighIntensity()) {
        this->mIntensity = 1.0f;
    } else {
        if (this->mIntensity >= 0.93f) {
            this->mIntensity = 0.8f;
        }
        this->UpdateIntensity(ai->GetPlayerSpeed() < this->mTopSpeed * 0.375f ? 0.86f : 0.0f);
        this->UpdateIntensity(ai->GetTimeLastCrashed() >= 2.5f ? this->mIntensity : 0.86f);
        this->UpdateIntensity(ai->GetTimeLastNailedCop() >= 2.5f ? this->mIntensity : 0.86f);
    }

    if (ai->GetPursuit() != nullptr) {
        in_cooldown = ai->GetPursuit()->GetPursuitStatus() == 2;
    } else {
        in_cooldown = false;
    }

    if ((ai->GetPursuitState() == SoundAI::kSearching || in_cooldown) && ai->GetPerpLostTime() > 4.0f) {
        if (ai->GetPlayerSpeed() < this->mTopSpeed * 0.35f) {
            this->ChangeStateTo(kElude);
            this->mIntensity = 1.0f;
        } else {
            this->ChangeStateTo(kWin);
            this->mIntensity = 1.0f;
        }
    } else {
        if (this->mBusy != 0) {
            return;
        }

        if (this->mAvgPlayerSpeed <= this->mTopSpeed * 0.5f) {
            float t_lost = ai->GetPerpLostTime();
            if (t_lost <= 5.0f) {
                if (this->mElapsed <= 90.0f) {
                    return;
                }
            }
        }

        this->ChangeStateTo(kNeutral);
        this->mIntensity = 0.65f;
    }
}

void MusicFlow::Win() {
    SoundAI *ai = SoundAI::Get();

    this->mBusy = this->mElapsed < 45.0f ? this->mBusy++ : 0;

    this->UpdateIntensity(this->mAvgPlayerSpeed > (this->mTopSpeed * 0.5f) ? 1.0f : 0.0f);

    if ((ai->GetTimeLastCrashed() < 2.0f) || (ai->GetTimeLastNailedCop() < 2.0f)) {
        this->mIntensity = 0.25f;
    }

    if ((ai->GetTimeLastCrashed() > 10.0f) && (ai->GetTimeLastNailedCop() > 10.0f) && (ai->GetPlayerSpeed() > (this->mTopSpeed * 0.8f))) {
        float t_lastboosted = (WorldTimer - this->mBoostTimer).GetSeconds();
        if (t_lastboosted > 10.0f) {
            this->mIntensity *= 1.15f;
            this->mBoostTimer = WorldTimer;
        }
    }

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    if ((ai->GetPlayerSpeed() < (this->mTopSpeed * 0.125f)) && ai->GetPursuit()->IsCollapseActive()) {
        this->ChangeStateTo(kLose);
        return;
    }

    bool in_cooldown;
    if (ai->GetPursuit() != nullptr) {
        in_cooldown = ai->GetPursuit()->GetPursuitStatus() == PS_COOL_DOWN;
    } else {
        in_cooldown = false;
    }

    if (this->mAvgPlayerSpeed < (this->mTopSpeed * 0.125f)) {
        if ((ai->GetPursuitState() == SoundAI::kSearching) || in_cooldown) {
            this->ChangeStateTo(kElude);
            return;
        }
    }

    if (this->mBusy != 0) {
        return;
    }

    if (this->mElapsed <= 90.0f) {
        return;
    }

    this->ChangeStateTo(kNeutral);
}

void MusicFlow::Elude() {
    SoundAI *ai = SoundAI::Get();

    this->mBusy = this->mElapsed < 5.0f ? this->mBusy++ : 0;

    if (ai->GetPursuitState() == SoundAI::kInactive) {
        this->ChangeStateTo(kTerminal);
        return;
    }

    this->UpdateIntensity(this->mAvgPlayerSpeed > 10.0f ? 1.0f : 0.0f);

    if (ai->GetPursuitState() == SoundAI::kActive) {
        if (ai->GetPursuitDistance() <= 50.0f) {
            this->ChangeStateTo(kLose);
            return;
        }
        this->ChangeStateTo(kNeutral);
        this->mIntensity = 0.8f;
        return;
    }

    if (this->mAvgPlayerSpeed <= this->mTopSpeed * 0.45f) {
        return;
    }
    this->ChangeStateTo(kWin);
}

void MusicFlow::Terminal() {
    switch (this->mCurrentPart) {
        case 5:
        case 6:
        case 7:
            break;
        default:
            this->Reset();
            break;
    }
}

bool MusicFlow::IsTransitionable() {
    return false;
}

void MusicFlow::ChangeStateTo(int new_state) {
    if (new_state == this->mState) {
        return;
    }

    SoundAI *ai = SoundAI::Get();
    this->SpeechFlow::ChangeStateTo(kWaiting);
    int nevt = -1;
    this->mTimer = WorldTimer;
    this->mBusy = 1;
    switch (new_state) {
        case kNeutral:
            if (this->mCurrentPart != 4) {
                nevt = 0xa;
            }
            break;
        case kLose:
            if (this->mCurrentPart != 1) {
                nevt = 0xb;
            }
            break;
        case kWin:
            if (this->mCurrentPart != 3) {
                nevt = 0xc;
            }
            break;
        case kElude:
            this->mAvgPlayerSpeed = 0.0f;
            if (this->mCurrentPart != 2) {
                nevt = 0xd;
            }
            break;
        case kTerminal:
            // TODO magic
            if ((static_cast<unsigned int>(this->mCurrentPart - 5) > 2) && (ai->GetFocus() != 999) && (ai->GetFocus() != -1)) {
                nevt = 0xe;
            }
            break;
    }
    if (nevt != -1) {
        MControlPathfinder(false, nevt, 0, 0).Send(UCrc32("Event"));
    }
}

void MusicFlow::Reset() {
    this->mState = kTransition;
    this->mTimer = Timer(0);
    this->mElapsed = 0.0f;
    this->mRequestedSwap = false;
    this->mRestrained = true;
    this->mIntensity = 0.0f;
    this->mAvgNumCopsInForm = 0.0f;
    this->mAvgNumCopsLOS = 0.0f;
    this->mAvgPlayerSpeed = 0.0f;
    this->mAvgPursuitDist = 0.0f;
}

}; // namespace Speech
