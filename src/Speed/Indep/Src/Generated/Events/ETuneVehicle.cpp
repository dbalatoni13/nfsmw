#include "ETuneVehicle.hpp"

#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

ETuneVehicle::ETuneVehicle(unsigned int pPlayer, const Physics::Tunings *pTunings) : Event(0x10), fPlayer(pPlayer), fTunings(pTunings) {
}

ETuneVehicle::~ETuneVehicle() {
    IPlayer *player = fPlayer == 0 ? IPlayer::First(PLAYER_LOCAL) : IPlayer::Last(PLAYER_LOCAL);

    if (player) {
        ISimable *simable = player->GetSimable();

        if (simable) {
            IVehicle *vehicle;

            if (simable->QueryInterface(&vehicle)) {
                vehicle->SetTunings(*fTunings);
            }
        }
    }
}

const char *ETuneVehicle::GetEventName() const {
    return "ETuneVehicle";
}

void ETuneVehicle_MakeEvent_Callback(const void *staticData) {
    new ETuneVehicle(((ETuneVehicle::StaticData *) staticData)->fPlayer, ((ETuneVehicle::StaticData *) staticData)->fTunings);
}
