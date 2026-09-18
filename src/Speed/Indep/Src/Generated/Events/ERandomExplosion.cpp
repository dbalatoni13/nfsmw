#include "ERandomExplosion.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"

#include "Speed/Indep/Src/Main/EventDynamicData.h"

ERandomExplosion::ERandomExplosion(int pClass, int pType, WTrigger * pTrigger) : Event(0x20), fClass(pClass), fType(pType), fTrigger(pTrigger) {}

ERandomExplosion::~ERandomExplosion() {}

const char *ERandomExplosion::GetEventName() const {
    return "ERandomExplosion";
}

void ERandomExplosion_MakeEvent_Callback(const void *staticData) {
    new ERandomExplosion(((ERandomExplosion::StaticData *)staticData)->fClass, ((ERandomExplosion::StaticData *)staticData)->fType, gEventDynamicData.fTrigger);
}

int ERandomExplosion_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ERandomExplosion((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), gEventDynamicData.fTrigger);
    }
    return 0;
}

void ERandomExplosion_ResolveEvent_Callback(void *event, const UGroup *group) {}
