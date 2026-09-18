#include "EKnockoutRacer.hpp"

#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

EKnockoutRacer::EKnockoutRacer(GRacerInfo *pRacer) : Event(0x10), fRacer(pRacer) {
}

EKnockoutRacer::~EKnockoutRacer() {
    if (!GRaceStatus::Get().GetWinningPlayerInfo()) {
        char message[64];
        char finalMessage[64];
        IPlayer *player;

        // TODO hash
        bSNPrintf(message, 64, "%s", GetLocalizedString(0xc83f7261));
        bSNPrintf(finalMessage, 64, message, fRacer->GetName());

        for (player = IPlayer::First(PLAYER_ALL); player; player = player->Next(PLAYER_ALL)) {
            ISimable *sim = player->GetSimable();

            if (sim != fRacer->GetSimable()) {
                IGenericMessage *igenericmessage;

                if (player->GetHud()->QueryInterface(&igenericmessage)) {
                    igenericmessage->RequestGenericMessage(finalMessage, false, FEHASH_ZOOMIN, 0, 0, GenericMessage_Priority_1);
                }
            }
        }
    }
}

const char *EKnockoutRacer::GetEventName() const {
    return "EKnockoutRacer";
}

void EKnockoutRacer_MakeEvent_Callback(const void *staticData) {
    new EKnockoutRacer(((EKnockoutRacer::StaticData *) staticData)->fRacer);
}
