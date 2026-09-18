#include "EDestroyVehicle.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EDestroyVehicle::EDestroyVehicle(unsigned int phSimable) : Event(0x10), fhSimable(phSimable) {
}

EDestroyVehicle::~EDestroyVehicle() {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        IDamageable *idamage;

        if (isimable->QueryInterface(&idamage)) {
            idamage->Destroy();
        }
    }
}

const char *EDestroyVehicle::GetEventName() const {
    return "EDestroyVehicle";
}

void EDestroyVehicle_MakeEvent_Callback(const void *staticData) {
    new EDestroyVehicle(gEventDynamicData.fhSimable);
}

int EDestroyVehicle_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EDestroyVehicle(gEventDynamicData.fhSimable);
    }
    return 0;
}

void EDestroyVehicle_ResolveEvent_Callback(void *event, const UGroup *group) {
}
