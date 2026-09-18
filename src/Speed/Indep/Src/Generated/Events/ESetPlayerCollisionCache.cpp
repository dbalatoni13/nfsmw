#include "ESetPlayerCollisionCache.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ESetPlayerCollisionCache::ESetPlayerCollisionCache(int pTriggerStimulus) : Event(0x10), fTriggerStimulus(pTriggerStimulus) {}

ESetPlayerCollisionCache::~ESetPlayerCollisionCache() {}

const char *ESetPlayerCollisionCache::GetEventName() const {
    return "ESetPlayerCollisionCache";
}

void ESetPlayerCollisionCache_MakeEvent_Callback(const void *staticData) {
    new ESetPlayerCollisionCache(gEventDynamicData.fTriggerStimulus);
}

int ESetPlayerCollisionCache_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new ESetPlayerCollisionCache(gEventDynamicData.fTriggerStimulus);
    }
    return 0;
}

void ESetPlayerCollisionCache_ResolveEvent_Callback(void *event, const UGroup *group) {}
