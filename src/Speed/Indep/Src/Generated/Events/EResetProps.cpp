#include "EResetProps.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/World.hpp"

EResetProps::EResetProps() : Event(0x10) {
}

EResetProps::~EResetProps() {
    World_RestoreProps();
}

const char *EResetProps::GetEventName() const {
    return "EResetProps";
}

void EResetProps_MakeEvent_Callback(const void *staticData) {
    new EResetProps();
}

int EResetProps_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EResetProps();
    }
    return 0;
}

void EResetProps_ResolveEvent_Callback(void *event, const UGroup *group) {
}
