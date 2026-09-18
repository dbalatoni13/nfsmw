#include "EBecomePlayerCar.hpp"

#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

EBecomePlayerCar::EBecomePlayerCar() : Event(0x10) {
}

EBecomePlayerCar::~EBecomePlayerCar() {
    if (Sim::GetUserMode() == Sim::USER_SINGLE) {
        ISimable *simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
        IRBVehicle *body;
        IHumanAI *human_ai;

        if (simable) {
            if (simable->QueryInterface(&human_ai)) {
                if (simable->QueryInterface(&body)) {
                    if (human_ai->GetAiControl()) {
                        human_ai->SetAiControl(false);
                        body->EnableObjectCollisions(true);
                        body->SetInvulnerability(INVULNERABLE_FROM_CONTROL_SWITCH, 1.0f);
                    }
                }
            }
        }
    }
}

const char *EBecomePlayerCar::GetEventName() const {
    return "EBecomePlayerCar";
}

void EBecomePlayerCar_MakeEvent_Callback(const void *staticData) {
    new EBecomePlayerCar();
}

int EBecomePlayerCar_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EBecomePlayerCar();
    }
    return 0;
}

void EBecomePlayerCar_ResolveEvent_Callback(void *event, const UGroup *group) {
}
