#include "EEnableTrigger.hpp"

#include <new>

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/WTrigger.h"

EEnableTrigger::EEnableTrigger(CARP::Trigger *pTrigger) : Event(0x10), fTrigger(pTrigger) {
}

EEnableTrigger::~EEnableTrigger() {
    if (fTrigger) {
        static_cast<WTrigger *>(fTrigger)->Enable();
    }
}

const char *EEnableTrigger::GetEventName() const {
    return "EEnableTrigger";
}

void EEnableTrigger_MakeEvent_Callback(const void *staticData) {
    new EEnableTrigger(((EEnableTrigger::StaticData *) staticData)->fTrigger);
}

int EEnableTrigger_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EEnableTrigger((CARP::Trigger *) lua_tostring(L, 1));
    }
    return 0;
}

void EEnableTrigger_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EEnableTrigger::StaticData *) event)->fTrigger) CARP::TagReference(group);
}
