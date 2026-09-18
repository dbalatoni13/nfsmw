#include "E911Call.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"

E911Call::E911Call() : Event(0x10) {}

E911Call::~E911Call() {}

const char *E911Call::GetEventName() const {
    return "E911Call";
}

void E911Call_MakeEvent_Callback(const void *staticData) {
    new E911Call();
}

int E911Call_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new E911Call();
    }
    return 0;
}

void E911Call_ResolveEvent_Callback(void *event, const UGroup *group) {}
