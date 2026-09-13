#include "Speed/Indep/Src/Frontend/HUD/FeTimeExtension.hpp"

#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

TimeExtension::TimeExtension(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x2000000), ITimeExtension(pOutter), mPlayerLapTime(0.0f), mTimeToShow(0.0f), mScriptHash(0), mTimerTimeExtension(),
      mTimerNextTollbooth() {}

// UNSOLVED
void TimeExtension::Update(IPlayer *player) {
    if (GRaceStatus::Get().GetRaceType() != 4)
        return;

    char messageString[32];
    if (mTimeToShow > 0.0f) {
        IGenericMessage *igenericmessage;
        if (player->GetHud()->QueryInterface(&igenericmessage) && !igenericmessage->IsGenericMessageShowing()) {
            mTimerTimeExtension = WorldTimer;
            Timer timer(GRaceStatus::Get().GetRaceTimeRemaining() - mTimeToShow);
            char timeToPrint[16];
            timer.PrintToString(timeToPrint, 4);
            bSPrintf(messageString, "%s\n+%s", GetTranslatedString(0x1c074e14), timeToPrint);
            igenericmessage->RequestGenericMessage(messageString, false, 0x8ab83edb, bStringHash("TIMER_ICON"), 0x609f6b15,
                                                   GenericMessage_Priority_3);
            mTimeToShow = 0.0f;
        }
        return;
    }

    if (mTimerTimeExtension.IsSet()) {
        if ((WorldTimer - mTimerTimeExtension).GetSeconds() >= 2.0f) {
            mTimerTimeExtension.UnSet();
            mTimerNextTollbooth = WorldTimer;
            return;
        }
    }

    if (mPlayerLapTime <= 0.0f)
        return;

    if (mPlayerLapTime > 10.0f) {
        if (mScriptHash == 0x821e6378 || mScriptHash == 0x4f79cba2) {
            mScriptHash = 0;
        }
    }

    if (mPlayerLapTime <= 10.0f) {
        Timer timer(mPlayerLapTime);
        char timeToPrint[16];
        timer.PrintToString(timeToPrint, 4);
        bSPrintf(messageString, "%s!\n%s", GetTranslatedString(0x862a0519), timeToPrint);

        if (mPlayerLapTime <= 5.0f) {
            if (mScriptHash == 0 || mScriptHash == 0x821e6378) {
                mScriptHash = 0x4f79cba2;
            }
            IGenericMessage *igenericmessage;
            if (player->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(messageString, true, mScriptHash, 0, 0, GenericMessage_Priority_5);
            }
        } else if (mPlayerLapTime <= 10.0f) {
            if (!mScriptHash) {
                mScriptHash = 0x821e6378;
            }
            IGenericMessage *igenericmessage;
            if (player->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(messageString, true, mScriptHash, 0, 0, GenericMessage_Priority_5);
            }
        }
    } else {
        if (mTimerNextTollbooth.IsSet()) {
            if ((WorldTimer - mTimerNextTollbooth).GetSeconds() >= 2.0f) {
                mScriptHash = 0;
                mTimerNextTollbooth.UnSet();
            } else {
                mScriptHash = 0x8ab83edb;
                Timer timer(GRaceStatus::Get().GetRaceTimeRemaining());
                char timeToPrint[16];
                timer.PrintToString(timeToPrint, 4);
                bSPrintf(messageString, "%s\n%s", GetTranslatedString(0x171471b4), timeToPrint);
                IGenericMessage *igenericmessage;
                if (player->GetHud()->QueryInterface(&igenericmessage)) {
                    igenericmessage->RequestGenericMessage(messageString, true, mScriptHash, bStringHash("TIMER_ICON"), 0x609f6b15,
                                                           GenericMessage_Priority_3);
                }
            }
        }
    }
}

void TimeExtension::RequestTimeExtensionMessage(IPlayer *iplayer, float timeToShow) {
    IGenericMessage *igenericmessage;
    if (iplayer->GetHud()->QueryInterface(&igenericmessage) && igenericmessage->IsGenericMessageShowing()) {
        igenericmessage->RequestGenericMessageZoomOut(0xE1C034FC);
    }
    mTimeToShow = timeToShow;
    mScriptHash = 0;
}
