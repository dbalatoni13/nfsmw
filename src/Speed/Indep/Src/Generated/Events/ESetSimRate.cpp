#include "ESetSimRate.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ESetSimRate::ESetSimRate(float pSpeed, float pTransitionStepSize) : Event(0x10), fSpeed(pSpeed), fTransitionStepSize(pTransitionStepSize) {}

ESetSimRate::~ESetSimRate() {}

const char *ESetSimRate::GetEventName() const {
    return "ESetSimRate";
}

void ESetSimRate_MakeEvent_Callback(const void *staticData) {
    new ESetSimRate(((ESetSimRate::StaticData *)staticData)->fSpeed, ((ESetSimRate::StaticData *)staticData)->fTransitionStepSize);
}

int ESetSimRate_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ESetSimRate(lua_tonumber(L, 1), lua_tonumber(L, 2));
    }
    return 0;
}

void ESetSimRate_ResolveEvent_Callback(void *event, const UGroup *group) {}
