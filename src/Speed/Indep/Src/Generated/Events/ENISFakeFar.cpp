#include "ENISFakeFar.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISFakeFar::ENISFakeFar(float pFakeFar) : Event(0x10), fFakeFar(pFakeFar) {}

ENISFakeFar::~ENISFakeFar() {}

const char *ENISFakeFar::GetEventName() const {
    return "ENISFakeFar";
}

void ENISFakeFar_MakeEvent_Callback(const void *staticData) {
    new ENISFakeFar(((ENISFakeFar::StaticData *)staticData)->fFakeFar);
}

int ENISFakeFar_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISFakeFar(lua_tonumber(L, 1));
    }
    return 0;
}

void ENISFakeFar_ResolveEvent_Callback(void *event, const UGroup *group) {}
