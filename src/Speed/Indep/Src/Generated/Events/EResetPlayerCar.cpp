#include "EResetPlayerCar.hpp"

#include "Speed/Indep/Src/Interfaces/Simables/IResetable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

EResetPlayerCar::EResetPlayerCar() : Event(0x10) {
}

EResetPlayerCar::~EResetPlayerCar() {
    IVehicle *ivehicle = IVehicle::First(VEHICLE_PLAYERS);

    if (ivehicle) {
        IResetable *ireset;

        if (ivehicle->QueryInterface(&ireset)) {
            ireset->ResetVehicle(false);
        }
    }
}

const char *EResetPlayerCar::GetEventName() const {
    return "EResetPlayerCar";
}

void EResetPlayerCar_MakeEvent_Callback(const void *staticData) {
    new EResetPlayerCar();
}

int EResetPlayerCar_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EResetPlayerCar();
    }
    return 0;
}

void EResetPlayerCar_ResolveEvent_Callback(void *event, const UGroup *group) {
}
