#include "EPursuitBreaker.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/Entities/LocalPlayer.hpp"

EPursuitBreaker::EPursuitBreaker(int pBegin) : Event(0x10), fBegin(pBegin) {
    static bool wason = false;
    bool ison = pBegin != 0;
    static eSndGameMode prev_mode = SND_STREETRACE;

    if (ison != wason) {
        wason = ison;

        if (ison) {
            prev_mode = g_pEAXSound->GetSndGameMode();
            g_pEAXSound->SetSndGameMode(SND_PURSUITBREAKER);
        } else {
            g_pEAXSound->SetSndGameMode(prev_mode);
        }

        IPlayer *player = IPlayer::First(PLAYER_LOCAL);

        if (player) {
            IHud *hud = player->GetHud();

            if (hud) {
                hud->FadeAll(!ison);
            }

            ISimable *isimable = player->GetSimable();

            if (isimable) {
                IRBVehicle *irbvehicle;

                if (isimable->QueryInterface(&irbvehicle)) {
                    if (ison) {
                        irbvehicle->SetCollisionMass(isimable->GetRigidBody()->GetMass() * Tweak_GameBreakerCollisionMass);
                    } else {
                        irbvehicle->SetCollisionMass(0.0f);
                    }
                }
            }
        }
    }
}

EPursuitBreaker::~EPursuitBreaker() {
}

const char *EPursuitBreaker::GetEventName() const {
    return "EPursuitBreaker";
}

void EPursuitBreaker_MakeEvent_Callback(const void *staticData) {
    new EPursuitBreaker(((EPursuitBreaker::StaticData *) staticData)->fBegin);
}
