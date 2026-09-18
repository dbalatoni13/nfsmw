#include "ENISFreeze.hpp"

#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

ENISFreeze::ENISFreeze(int pFreezeAnimation) : Event(0x10), fFreezeAnimation(pFreezeAnimation) {
    if (INIS::Get()) {
        if (fFreezeAnimation) {
            INIS::Get()->Pause();
        } else {
            INIS::Get()->UnPause();
        }
    }
}

ENISFreeze::~ENISFreeze() {}

const char *ENISFreeze::GetEventName() const {
    return "ENISFreeze";
}

void ENISFreeze_MakeEvent_Callback(const void *staticData) {
    new ENISFreeze(((ENISFreeze::StaticData *) staticData)->fFreezeAnimation);
}

int ENISFreeze_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISFreeze((int) lua_tonumber(L, 1));
    }
    return 0;
}

void ENISFreeze_ResolveEvent_Callback(void *event, const UGroup *group) {
}
