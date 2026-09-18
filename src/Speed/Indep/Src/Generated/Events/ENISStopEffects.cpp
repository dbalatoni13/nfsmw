#include "ENISStopEffects.hpp"

#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISStopEffects::ENISStopEffects() : Event(0x10) {
    gEmitterSystem.KillEverything();
    gEmitterSystem.RefreshWorldEffects();
}

ENISStopEffects::~ENISStopEffects() {}

const char *ENISStopEffects::GetEventName() const {
    return "ENISStopEffects";
}

void ENISStopEffects_MakeEvent_Callback(const void *staticData) {
    new ENISStopEffects();
}

int ENISStopEffects_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new ENISStopEffects();
    }
    return 0;
}

void ENISStopEffects_ResolveEvent_Callback(void *event, const UGroup *group) {}
