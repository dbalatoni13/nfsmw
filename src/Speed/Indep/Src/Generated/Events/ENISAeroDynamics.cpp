#include "ENISAeroDynamics.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

extern float gNIS_AeroDynamics;

ENISAeroDynamics::ENISAeroDynamics(float pAerodynamics) : Event(0x10), fAerodynamics(pAerodynamics) {
    gNIS_AeroDynamics = pAerodynamics;
}

ENISAeroDynamics::~ENISAeroDynamics() {}

const char *ENISAeroDynamics::GetEventName() const {
    return "ENISAeroDynamics";
}

void ENISAeroDynamics_MakeEvent_Callback(const void *staticData) {
    new ENISAeroDynamics(((ENISAeroDynamics::StaticData *)staticData)->fAerodynamics);
}

int ENISAeroDynamics_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISAeroDynamics(lua_tonumber(L, 1));
    }
    return 0;
}

void ENISAeroDynamics_ResolveEvent_Callback(void *event, const UGroup *group) {}
