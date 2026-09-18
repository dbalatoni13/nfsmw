#include "EDisableTrigger.hpp"

#include <new>

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/WTrigger.h"

EDisableTrigger::EDisableTrigger(CARP::Trigger *pTrigger) : Event(0x10), fTrigger(pTrigger) {
}

EDisableTrigger::~EDisableTrigger() {
    if (fTrigger) {
        static_cast<WTrigger *>(fTrigger)->Disable();
    }
}

const char *EDisableTrigger::GetEventName() const {
    return "EDisableTrigger";
}

void EDisableTrigger_MakeEvent_Callback(const void *staticData) {
    new EDisableTrigger(((EDisableTrigger::StaticData *) staticData)->fTrigger);
}

int EDisableTrigger_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EDisableTrigger((CARP::Trigger *) lua_tostring(L, 1));
    }
    return 0;
}

void EDisableTrigger_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EDisableTrigger::StaticData *) event)->fTrigger) CARP::TagReference(group);
}
