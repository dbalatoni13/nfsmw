#include "Speed/Indep/Src/Frontend/HUD/FeTimeExtension.hpp"

#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

int bSPrintf(char *dest, const char *fmt, ...);
extern char *GetTranslatedString(int hash);
unsigned int bStringHash(const char *str);

extern const char lbl_803E5048[];
extern const char lbl_803E48E0[];
extern const char lbl_803E4CF4[];
extern const char lbl_803E5060[];

TimeExtension::TimeExtension(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x2000000) //
    , ITimeExtension(pOutter) //
    , mPlayerLapTime(0.0f) //
    , mTimeToShow(0.0f) //
    , mScriptHash(0)
{
    mTimerTimeExtension.ResetLow();
    mTimerNextTollbooth.ResetLow();
}

void TimeExtension::Update(IPlayer *player) {
    if (GRaceStatus::Get().GetRaceType() != 4)
        return;

    if (mTimeToShow > 0.0f) {
        IHud *hud = player->GetHud();
        IGenericMessage *igenericmessage;
        if (hud->QueryInterface(&igenericmessage) && !igenericmessage->IsGenericMessageShowing()) {
            mTimerTimeExtension = WorldTimer;
            char messageString[32];
            char timeToPrint[16];
            Timer timer(GRaceStatus::Get().GetRaceTimeRemaining() - mTimeToShow);
            timer.PrintToString(timeToPrint, 4);
            bSPrintf(messageString, lbl_803E5048, GetTranslatedString(0x1c074e14), timeToPrint);
            igenericmessage->RequestGenericMessage(
                messageString, false, 0x8ab83edb, bStringHash(lbl_803E5060), 0x609f6b15,
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
        char messageString[32];
        char timeToPrint[16];
        Timer timer(mPlayerLapTime);
        timer.PrintToString(timeToPrint, 4);
        bSPrintf(messageString, lbl_803E48E0, GetTranslatedString(0x862a0519), timeToPrint);

        if (mPlayerLapTime <= 5.0f) {
            if (mScriptHash == 0 || mScriptHash == 0x821e6378) {
                mScriptHash = 0x4f79cba2;
            }
            IHud *hud = player->GetHud();
            IGenericMessage *igenericmessage;
            if (hud->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(
                    messageString, true, mScriptHash, 0, 0, GenericMessage_Priority_5);
            }
        } else if (mPlayerLapTime <= 10.0f) {
            if (!mScriptHash) {
                mScriptHash = 0x821e6378;
            }
            IHud *hud = player->GetHud();
            IGenericMessage *igenericmessage;
            if (hud->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(
                    messageString, true, mScriptHash, 0, 0, GenericMessage_Priority_5);
            }
        }
    } else {
        if (mTimerNextTollbooth.IsSet()) {
            if ((WorldTimer - mTimerNextTollbooth).GetSeconds() >= 2.0f) {
                mScriptHash = 0;
                mTimerNextTollbooth.UnSet();
                return;
            }
            mScriptHash = 0x8ab83edb;
            char messageString[32];
            char timeToPrint[16];
            Timer timer(GRaceStatus::Get().GetRaceTimeRemaining());
            timer.PrintToString(timeToPrint, 4);
            bSPrintf(messageString, lbl_803E4CF4, GetTranslatedString(0x171471b4), timeToPrint);
            IHud *hud = player->GetHud();
            IGenericMessage *igenericmessage;
            if (hud->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(
                    messageString, true, mScriptHash, bStringHash(lbl_803E5060), 0x609f6b15,
                    GenericMessage_Priority_3);
            }
        }
        mScriptHash = 0;
    }
}

void TimeExtension::SetPlayerLapTime(float lapTime) {
    mPlayerLapTime = lapTime;
}

void TimeExtension::RequestTimeExtensionMessage(IPlayer *iplayer, float timeToShow) {
    IGenericMessage *igenericmessage;
    if (iplayer->GetHud()->QueryInterface(&igenericmessage) && igenericmessage->IsGenericMessageShowing()) {
        igenericmessage->RequestGenericMessageZoomOut(0xE1C034FC);
    }
    mTimeToShow = timeToShow;
    mScriptHash = 0;
}
