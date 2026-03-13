#include "Speed/Indep/Src/Frontend/HUD/FeRaceOverMessage.hpp"

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

const char *GetLocalizedString(unsigned int hash);
int bSPrintf(char *dest, const char *fmt, ...);
int bSNPrintf(char *dest, int maxlen, const char *fmt, ...);

extern const char lbl_803E4CF4[];
extern const char lbl_803E4CFC[];

static const unsigned int RaceOverFinishStrings[8] = {
    0xE815BC6D,
    0xC9A64EFB,
    0x443FAB5A,
    0xAC122037,
    0x4342D430,
    0x442E5F4F,
    0xFFC2867C,
    0xEAC720D3,
};

template <typename T> static T ReadRaceOverField(const void *base, int offset) {
    return *reinterpret_cast<const T *>(reinterpret_cast<const unsigned char *>(base) + offset);
}

HINTERFACE IRaceOverMessage::_IHandle() {
    return (HINTERFACE)_IHandle;
}

RaceOverMessage::RaceOverMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 4) //
    , IRaceOverMessage(pOutter) //
    , bShowMessage(false) //
    , bShowTotalledMessage(false) {}

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
                igenericmessage->RequestGenericMessage(
                    GetTranslatedString(0x4BA0D22F), false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
            }
        }
    }
}

void RaceOverMessage::RequestRaceOverMessage(IPlayer *player) {
    GRaceStatus &race_status = GRaceStatus::Get();
    GRaceParameters *race_parameters = race_status.GetRaceParameters();
    GRacerInfo *info = race_status.GetRacerInfo(player->GetSimable());
    float racer_time = ReadRaceOverField< const GTimer >(info, 0x160).GetTime();
    float race_time_limit = race_parameters->GetTimeLimit();
    int rank_by_points = race_parameters->GetRankPlayersByPoints();
    int was_vehicle_totalled = 0;
    IVehicle *ivehicle;
    int is_challenge_race = GRaceStatus::IsChallengeRace();
    int challenge_complete = ReadRaceOverField< int >(info, 0x2C);

    bShowMessage = 1;

    if (ReadRaceOverField< int >(info, 0x0) != 0) {
        ISimable *simable = info->GetSimable();
        if (simable != nullptr && simable->QueryInterface(&ivehicle)) {
            was_vehicle_totalled = ivehicle->IsDestroyed();
        }
    }

    if (was_vehicle_totalled != 0) {
        bShowTotalledMessage = 1;
    } else if (ReadRaceOverField< int >(info, 0x24) == 0) {
        if (ReadRaceOverField< int >(info, 0x20) == 0) {
            if (ReadRaceOverField< int >(info, 0x1C) == 0) {
                if (0.0f < race_time_limit && race_time_limit < racer_time && rank_by_points == 0 &&
                    !(is_challenge_race != 0 && challenge_complete != 0)) {
                    Timer time_limit;
                    char race_time_str[16];
                    IGenericMessage *igenericmessage;

                    time_limit.SetTime(race_time_limit);
                    time_limit.PrintToString(race_time_str, 4);

                    if (player->GetHud()->QueryInterface(&igenericmessage)) {
                        igenericmessage->RequestGenericMessage(
                            GetTranslatedString(0x556ED1B2), false, 0x9D73BC15, 0, 0, GenericMessage_Priority_1);
                    }
                } else if (is_challenge_race == 0 || challenge_complete != 0) {
                    Timer race_time;
                    char race_time_str[16];
                    char bot_message_string[48];
                    char message_string[64];
                    unsigned int finish_hash;
                    IGenericMessage *igenericmessage;
                    int rank = ReadRaceOverField< int >(info, 0x10);

                    race_time.SetTime(racer_time);
                    race_time.PrintToString(race_time_str, 4);

                    if (rank < 2 || rank != race_status.GetRacerCount()) {
                        finish_hash = RaceOverFinishStrings[rank - 1];
                    } else {
                        finish_hash = 0xEAC720D3;
                    }

                    bSPrintf(bot_message_string, GetLocalizedString(0xC2878EBC), race_time_str);
                    bSPrintf(message_string, lbl_803E4CF4, GetTranslatedString(finish_hash), bot_message_string);

                    if (player->GetHud()->QueryInterface(&igenericmessage)) {
                        igenericmessage->RequestGenericMessage(
                            message_string, false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
                    }
                } else {
                    char race_over_message[64];
                    IGenericMessage *igenericmessage;

                    bSNPrintf(
                        race_over_message, 64, lbl_803E4CF4, GetTranslatedString(0x82E4DAFD),
                        GetTranslatedString(0xF8ED7926));

                    if (player->GetHud()->QueryInterface(&igenericmessage)) {
                        igenericmessage->RequestGenericMessage(
                            race_over_message, false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
                    }
                }
            } else {
                char race_over_message[64];
                IGenericMessage *igenericmessage;

                bSNPrintf(race_over_message, 64, lbl_803E4CF4, GetTranslatedString(0x82E4DAFD),
                          GetTranslatedString(0x1B59940C));

                if (player->GetHud()->QueryInterface(&igenericmessage)) {
                    igenericmessage->RequestGenericMessage(
                        race_over_message, false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
                }
            }
        } else {
            IGenericMessage *igenericmessage;

            if (player->GetHud()->QueryInterface(&igenericmessage)) {
                igenericmessage->RequestGenericMessage(
                    GetTranslatedString(0x4BA0D22F), false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
            }
        }
    } else {
        IGenericMessage *igenericmessage;

        if (player->GetHud()->QueryInterface(&igenericmessage)) {
            igenericmessage->RequestGenericMessage(
                GetTranslatedString(0x7449D26D), false, 0x8AB83EDB, 0, 0, GenericMessage_Priority_1);
        }
    }

    if (cFEng::Get()->IsPackagePushed(lbl_803E4CFC)) {
        new EUnPause();
    }
}

void RaceOverMessage::DismissRaceOverMessage() {
    bShowMessage = 0;
    bShowTotalledMessage = 0;
}

int RaceOverMessage::ShouldShowRaceOverMessage() {
    return bShowMessage;
}
