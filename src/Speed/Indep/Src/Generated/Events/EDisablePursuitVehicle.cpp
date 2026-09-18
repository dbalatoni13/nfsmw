#include "EDisablePursuitVehicle.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EDisablePursuitVehicle::EDisablePursuitVehicle(unsigned int phSimable) : Event(0x10), fhSimable(phSimable) {
}

EDisablePursuitVehicle::~EDisablePursuitVehicle() {
    ISimable *isimable = NULL;

    if (fhSimable) {
        isimable = ISimable::FindInstance((HSIMABLE) fhSimable);
    }

    if (isimable) {
        IPursuitAI *ipv;

        if (isimable->QueryInterface(&ipv)) {
            ipv->StartRoadBlock();
        }
    }
}

const char *EDisablePursuitVehicle::GetEventName() const {
    return "EDisablePursuitVehicle";
}

void EDisablePursuitVehicle_MakeEvent_Callback(const void *staticData) {
    new EDisablePursuitVehicle(gEventDynamicData.fhSimable);
}

int EDisablePursuitVehicle_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EDisablePursuitVehicle(gEventDynamicData.fhSimable);
    }
    return 0;
}

void EDisablePursuitVehicle_ResolveEvent_Callback(void *event, const UGroup *group) {
}
