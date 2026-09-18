#include "EReportInfraction.hpp"

#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

EReportInfraction::EReportInfraction(GInfractionManager::InfractionType pInfraction) : Event(0x10), fInfraction(pInfraction) {
}

EReportInfraction::~EReportInfraction() {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);
    char buf[128];
    unsigned int hash = 0;

    if (player) {
        IInfractions *iinfractions;

        switch (fInfraction) {
        case GInfractionManager::kInfraction_Speeding:
            hash = 0x1F87313A;
            break;
        case GInfractionManager::kInfraction_Racing:
            hash = 0x6DF08AFF;
            break;
        case GInfractionManager::kInfraction_Reckless:
            hash = 0x21857819;
            break;
        case GInfractionManager::kInfraction_Assault:
            hash = 0x66432B02;
            break;
        case GInfractionManager::kInfraction_HitAndRun:
            hash = 0x3E1B41F3;
            break;
        case GInfractionManager::kInfraction_Damage:
            hash = 0x98AC2BAE;
            break;
        case GInfractionManager::kInfraction_Resist:
            hash = 0xC03AA693;
            break;
        case GInfractionManager::kInfraction_OffRoad:
            hash = 0x50F83C6C;
            break;
        }

        bSNPrintf(buf, 128, "%s", GetLocalizedString(hash));

        if (player->GetHud()->QueryInterface(&iinfractions)) {
            iinfractions->RequestInfraction(buf);
        }

        MMiscSound msg(fInfraction);

        msg.Send(UCrc32("Infraction"));
    }
}

const char *EReportInfraction::GetEventName() const {
    return "EReportInfraction";
}

void EReportInfraction_MakeEvent_Callback(const void *staticData) {
    new EReportInfraction(((EReportInfraction::StaticData *) staticData)->fInfraction);
}
