#include "Speed/Indep/Src/Frontend/HUD/FeCountdown.hpp"
#include "Speed/Indep/Src/Animation/AnimWorldTypes.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Generated/Events/ENISWorldAnimTrigger.hpp"
#include "Speed/Indep/Src/Generated/Messages/MCountdownDone.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
void FEngSetLanguageHash(FEString *text, unsigned int hash);
int FEPrintf(FEString *text, const char *fmt, ...);

Countdown::Countdown(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x400) //
    , ICountdown(pOutter) //
    , mCountdown(RACE_COUNTDOWN_NUMBER_NONE) //
    , mSecondTimer(0)
{
    pMessageGroup = RegisterGroup(FEHashUpper("321_GO_GROUP"));
    pMessage = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("321_GO")));
    pMessageShadow = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("321_GO_SHADOW")));
}

void Countdown::Update(IPlayer *player) {
    if (!pMessageGroup) return;
    if (!pMessage) return;
    if (!pMessageShadow) return;

    if (WorldTimer.IsSet()) {
        int countdown = mCountdown;
        bool shouldStep = false;

        if (countdown == RACE_COUNTDOWN_NUMBER_4) {
            shouldStep = true;
        } else if (countdown > 0) {
            if ((WorldTimer - mSecondTimer).GetSeconds() >= 1.0f) {
                shouldStep = true;
            }
        } else if (!countdown) {
            if ((WorldTimer - mSecondTimer).GetSeconds() >= 6.0f) {
                shouldStep = true;
            }
        }

        if (!shouldStep) return;

        mCountdown = static_cast<eRaceCountdownNumber>(countdown - 1);

        if (mCountdown == RACE_COUNTDOWN_NUMBER_NONE) {
            FEngSetScript(pMessageGroup, 0x16a259, true);
            mSecondTimer.UnSet();
            return;
        }

        if (mCountdown == RACE_COUNTDOWN_NUMBER_GO) {
            FEngSetScript(pMessageGroup, 0x535, true);
            FEngSetLanguageHash(pMessage, 0x61223ab5);
            FEngSetLanguageHash(pMessageShadow, 0x61223ab5);

            {
                MCountdownDone msg;
                msg.Post(UCrc32(0x20d60dbf));
                mSecondTimer = WorldTimer;
            }

            new ENISWorldAnimTrigger("EN_TollBoothArm_01", 0.0f, 0, 0);
            new ENISWorldAnimTrigger("EN_TollBoothArm_02", 0.0f, 0, 0);
            new ENISWorldAnimTrigger("EN_TollBoothArm_03", 0.0f, 0, 0);
            new ENISWorldAnimTrigger("EN_TollBoothArm_04", 0.0f, 0, 0);

            SetSoundControlState(false, SNDSTATE_NIS_321, "NIS 321");
            return;
        }

        if (mCountdown == RACE_COUNTDOWN_NUMBER_3) {
            FEngSetScript(pMessageGroup, 0x3c3c2f7, true);

            new ENISWorldAnimTrigger("EN_TollBoothArm_01", 0.0f, 1, 0);
            new ENISWorldAnimTrigger("EN_TollBoothArm_02", 0.0f, 1, 0);
            new ENISWorldAnimTrigger("EN_TollBoothArm_03", 0.0f, 1, 0);
            new ENISWorldAnimTrigger("EN_TollBoothArm_04", 0.0f, 1, 0);

            g_pEAXSound->START_321Countdown();
        } else if (mCountdown == RACE_COUNTDOWN_NUMBER_2) {
            FEngSetScript(pMessageGroup, 0xe479, true);
        } else if (mCountdown == RACE_COUNTDOWN_NUMBER_1) {
            FEngSetScript(pMessageGroup, 0xce01, true);
        }

        FEPrintf(pMessage, "%d", mCountdown);
        FEPrintf(pMessageShadow, "%d", mCountdown);
        mSecondTimer = WorldTimer;
    } else {
        FEngSetScript(pMessageGroup, 0x16a259, true);
        mSecondTimer.UnSet();
    }
}

void Countdown::BeginCountdown() {
    mCountdown = RACE_COUNTDOWN_NUMBER_4;
    SetSoundControlState(true, SNDSTATE_NIS_321, "BeginCountdown");
}

bool Countdown::IsActive() {
    return mCountdown > 0;
}

float Countdown::GetSecondsBeforeRaceStart() {
    if (mCountdown == RACE_COUNTDOWN_NUMBER_4) {
        return 3.0f;
    }
    if (mCountdown > 0) {
        return static_cast<float>(mCountdown) - (WorldTimer - mSecondTimer).GetSeconds();
    }
    return 0.0f;
}
