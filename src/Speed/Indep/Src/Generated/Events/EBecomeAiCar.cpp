#include "EBecomeAiCar.hpp"

#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

EBecomeAiCar::EBecomeAiCar() : Event(0x10) {
}

EBecomeAiCar::~EBecomeAiCar() {
    if (Sim::GetUserMode() == Sim::USER_SINGLE) {
        ISimable *simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
        IRBVehicle *body;
        IHumanAI *human_ai;
        IVehicle *vehicle;

        if (simable) {
            if (simable->QueryInterface(&vehicle)) {
                if (simable->QueryInterface(&human_ai)) {
                    if (simable->QueryInterface(&body)) {
                        if (!human_ai->GetAiControl()) {
                            human_ai->SetAiControl(true);

                            IVehicleAI *ai_vehicle = vehicle->GetAIVehiclePtr();

                            if (ai_vehicle) {
                                ai_vehicle->ResetDriveToNav(SELECT_CURRENT_LANE);
                            }

                            body->EnableObjectCollisions(false);
                        }
                    }
                }
            }
        }
    }
}

const char *EBecomeAiCar::GetEventName() const {
    return "EBecomeAiCar";
}

void EBecomeAiCar_MakeEvent_Callback(const void *staticData) {
    new EBecomeAiCar();
}

int EBecomeAiCar_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EBecomeAiCar();
    }
    return 0;
}

void EBecomeAiCar_ResolveEvent_Callback(void *event, const UGroup *group) {
}
