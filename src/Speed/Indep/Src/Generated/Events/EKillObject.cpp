#include "EKillObject.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EKillObject::EKillObject(unsigned int phSimable, unsigned int phModel) : Event(0x10), fhSimable(phSimable), fhModel(phModel) {
}

EKillObject::~EKillObject() {
    ISimable *simulation = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (simulation) {
        simulation->Kill();
    }
}

const char *EKillObject::GetEventName() const {
    return "EKillObject";
}

void EKillObject_MakeEvent_Callback(const void *staticData) {
    new EKillObject(gEventDynamicData.fhSimable, gEventDynamicData.fhModel);
}

int EKillObject_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EKillObject(gEventDynamicData.fhSimable, gEventDynamicData.fhModel);
    }
    return 0;
}

void EKillObject_ResolveEvent_Callback(void *event, const UGroup *group) {
}
