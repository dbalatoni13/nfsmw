#include "Speed/Indep/Src/Frontend/HUD/FeRaceOverMessage.hpp"

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

static const unsigned int RaceOverFinishStrings[8] = {
    0xE815BC6D, 0xC9A64EFB, 0x443FAB5A, 0xAC122037, 0x4342D430, 0x442E5F4F, 0xFFC2867C, 0xEAC720D3,
};

RaceOverMessage::RaceOverMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 4),   //
      IRaceOverMessage(pOutter), //
      bShowMessage(false),       //
      bShowTotalledMessage(false) {}

void RaceOverMessage::Update(IPlayer *player) {
    if (bShowTotalledMessage != 0) {
        eView *view = eGetView(player->GetRenderPort(), false);
        CameraMover *cammover = nullptr;

        if (view != nullptr) {
            cammover = view->GetCameraMover();
        }

        if (cammover != nullptr && cammover->GetType() == CM_DRIVE_CUBIC) {
            bShowTotalledMessage = 0;

            IGenericMessage *igenericmessage;
            if (player->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(GetTranslatedString(0x4BA0D22F), false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
            }
        }
    }
}

void RaceOverMessage::RequestRaceOverMessage(IPlayer *player) {
    bShowMessage = 1;

    ISimable *simable = player->GetSimable();
    GRacerInfo *info = GRaceStatus::Get().GetRacerInfo(simable);

    float racerTime = info->GetRaceTime();
    float raceTimeLimit = GRaceStatus::Get().GetRaceParameters()->GetTimeLimit();
    float rankByPoints = GRaceStatus::Get().GetRaceParameters()->GetRankPlayersByPoints();

    int wasVehicleTotalled = 0;

    IVehicle *ivehicle;
    if (info->GetSimable()->QueryInterface(&ivehicle)) {
        wasVehicleTotalled = ivehicle->IsDestroyed();
    }

    bool isCompletedChallengeRace = GRaceStatus::IsChallengeRace() && info->GetChallengeComplete();
    char raceOverMessage[64];

    if (wasVehicleTotalled != 0) {
        bShowTotalledMessage = 1;
    } else if (info->GetIsEngineBlown()) {
        IGenericMessage *igenericmessage;

        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            igenericmessage->RequestGenericMessage(GetTranslatedString(0x7449D26D), false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
        }
    } else if (info->GetIsTotalled()) {
        IGenericMessage *igenericmessage;

        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            igenericmessage->RequestGenericMessage(GetTranslatedString(0x4BA0D22F), false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
        }
    } else if (info->GetIsKnockedOut()) {
        IGenericMessage *igenericmessage;

        bSNPrintf(raceOverMessage, 64, "%s\n%s", GetTranslatedString(0x82E4DAFD), GetTranslatedString(0x1B59940C));

        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            igenericmessage->RequestGenericMessage(raceOverMessage, false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
        }
    } else if (0.0f < raceTimeLimit && racerTime > raceTimeLimit && rankByPoints == 0.0f && !isCompletedChallengeRace) {
        char timeLimitStr[16];
        Timer timerLimit = Timer(raceTimeLimit);

        timerLimit.PrintToString(timeLimitStr, 4);

        IGenericMessage *igenericmessage;
        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            igenericmessage->RequestGenericMessage(GetTranslatedString(0x556ED1B2), false, 0x9D73BC15, 0, 0, GenericMessage_Priority_1);
        }
    } else if (GRaceStatus::IsChallengeRace() != 0 && !info->GetChallengeComplete()) {
        IGenericMessage *igenericmessage;

        bSNPrintf(raceOverMessage, 64, "%s\n%s", GetTranslatedString(0x82E4DAFD), GetTranslatedString(0xF8ED7926));

        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            igenericmessage->RequestGenericMessage(raceOverMessage, false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
        }
    } else {
        char raceTimeStr[16];

        Timer timerRace = Timer(racerTime);

        timerRace.PrintToString(raceTimeStr, 4);

        int last_place = GRaceStatus::Get().GetRacerCount();

        int hash;

        if (info->GetRanking() > 1 && info->GetRanking() == last_place) {
            hash = 0xEAC720D3;
        } else {
            hash = RaceOverFinishStrings[info->GetRanking() - 1];
        }

        char botMessageString[48];
        bSPrintf(botMessageString, GetLocalizedString(0xC2878EBC), raceTimeStr);

        char messageString[64];
        bSPrintf(messageString, "%s\n%s", GetTranslatedString(hash), botMessageString);

        IGenericMessage *igenericmessage;
        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            igenericmessage->RequestGenericMessage(messageString, false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
        }
    }

    if (cFEng::Get()->IsPackagePushed("Pause_Main.fng")) {
        new EUnPause();
    }
}

void RaceOverMessage::DismissRaceOverMessage() {
    bShowMessage = 0;
    bShowTotalledMessage = 0;
}
