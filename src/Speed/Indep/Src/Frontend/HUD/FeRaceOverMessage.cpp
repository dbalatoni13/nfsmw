#include "Speed/Indep/Src/Frontend/HUD/FeRaceOverMessage.hpp"

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

HINTERFACE IRaceOverMessage::_IHandle() {
    return (HINTERFACE)_IHandle;
}

RaceOverMessage::RaceOverMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 4) //
    , IRaceOverMessage(pOutter) //
    , bShowMessage(false) //
    , bShowTotalledMessage(false) {}

RaceOverMessage::~RaceOverMessage() {}

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

void RaceOverMessage::DismissRaceOverMessage() {
    bShowMessage = 0;
    bShowTotalledMessage = 0;
}

int RaceOverMessage::ShouldShowRaceOverMessage() {
    return bShowMessage;
}
