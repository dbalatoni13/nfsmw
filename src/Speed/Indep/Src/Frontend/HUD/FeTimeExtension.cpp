#include "Speed/Indep/Src/Frontend/HUD/FeTimeExtension.hpp"

#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

TimeExtension::TimeExtension(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x2000000), ITimeExtension(pOutter), mPlayerLapTime(0.0f), mTimeToShow(0.0f),
      mScriptHash(0) {
}

void TimeExtension::Update(IPlayer *player) {
    char messageString[32];

    if (GRaceStatus::Get().GetRaceType() != 4)
        return;

    if (mTimeToShow > 0.0f) {
        IGenericMessage *igenericmessage;
        if (player->GetHud()->QueryInterface(&igenericmessage) && !igenericmessage->IsGenericMessageShowing()) {
            mTimerTimeExtension = WorldTimer;
            char timeToPrint[16];
            Timer timer(GRaceStatus::Get().GetRaceTimeRemaining() - mTimeToShow);
            timer.PrintToString(timeToPrint, 4);
            bSPrintf(messageString, "%s\n+%s", GetTranslatedString(LANGUAGE_HUD_TIME_BONUS), timeToPrint);
            igenericmessage->RequestGenericMessage(messageString, false, FEHASH_ZOOMIN, bStringHash("TIMER_ICON"), FEHASH_TIMEBONUS,
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
        char timeToPrint[16];
        Timer timer(mPlayerLapTime);
        timer.PrintToString(timeToPrint, 4);
        bSPrintf(messageString, "%s!\n%s", GetTranslatedString(LANGUAGE_HUD_HURRY), timeToPrint);

        if (mPlayerLapTime <= 5.0f) {
            IGenericMessage *igenericmessage;
            if (mScriptHash == 0 || mScriptHash == 0x821e6378) {
                mScriptHash = 0x4f79cba2;
            }
            if (player->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(messageString, true, mScriptHash, 0, 0, GenericMessage_Priority_5);
            }
        } else if (mPlayerLapTime <= 10.0f) {
            IGenericMessage *igenericmessage;
            if (!mScriptHash) {
                mScriptHash = 0x821e6378;
            }
            if (player->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(messageString, true, mScriptHash, 0, 0, GenericMessage_Priority_5);
            }
        }
    } else {
        if (mTimerNextTollbooth.IsSet()) {
            if ((WorldTimer - mTimerNextTollbooth).GetSeconds() >= 2.0f) {
                mScriptHash = 0;
                mTimerNextTollbooth.UnSet();
                return;
            }
            mScriptHash = FEHASH_ZOOMIN;
            char timeToPrint[16];
            Timer timer(GRaceStatus::Get().GetRaceTimeRemaining());
            timer.PrintToString(timeToPrint, 4);
            bSPrintf(messageString, "%s\n%s", GetTranslatedString(LANGUAGE_HUD_NEXT_TOLLBOOTH), timeToPrint);
            IGenericMessage *igenericmessage;
            if (player->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(messageString, true, mScriptHash, bStringHash("TIMER_ICON"), FEHASH_TIMEBONUS,
                                                       GenericMessage_Priority_3);
            }
        } else {
            mScriptHash = 0;
        }
    }
}

void TimeExtension::RequestTimeExtensionMessage(IPlayer *iplayer, float timeToShow) {
    IGenericMessage *igenericmessage;
    if (iplayer->GetHud()->QueryInterface(&igenericmessage) && igenericmessage->IsGenericMessageShowing()) {
        igenericmessage->RequestGenericMessageZoomOut(FEHASH_ZOOMOUT);
    }
    mTimeToShow = timeToShow;
    mScriptHash = 0;
}
