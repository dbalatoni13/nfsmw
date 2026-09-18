#include "EEndCarStop.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EEndCarStop::EEndCarStop(int pDontRetestInput, unsigned int phSimable) : Event(0x10), fDontRetestInput(pDontRetestInput), fhSimable(phSimable) {
}

EEndCarStop::~EEndCarStop() {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        IVehicle *ivehicle;

        if (isimable->QueryInterface(&ivehicle) && ivehicle) {
            ivehicle->ForceStopOff(3);
        }
    }
}

const char *EEndCarStop::GetEventName() const {
    return "EEndCarStop";
}

void EEndCarStop_MakeEvent_Callback(const void *staticData) {
    new EEndCarStop(((EEndCarStop::StaticData *) staticData)->fDontRetestInput, gEventDynamicData.fhSimable);
}

int EEndCarStop_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EEndCarStop((int) lua_tonumber(L, 1), gEventDynamicData.fhSimable);
    }
    return 0;
}

void EEndCarStop_ResolveEvent_Callback(void *event, const UGroup *group) {
}
