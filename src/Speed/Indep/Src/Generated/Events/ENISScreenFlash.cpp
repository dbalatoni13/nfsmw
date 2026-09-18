#include "ENISScreenFlash.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/VisualTreatment.h"

ENISScreenFlash::ENISScreenFlash(float pLength) : Event(0x10), fLength(pLength) {
    IVisualTreatment *visualTreatment = IVisualTreatment::Get();

    if (visualTreatment) {
        visualTreatment->TriggerPulse(fLength);
    }
}

ENISScreenFlash::~ENISScreenFlash() {
}

const char *ENISScreenFlash::GetEventName() const {
    return "ENISScreenFlash";
}

void ENISScreenFlash_MakeEvent_Callback(const void *staticData) {
    new ENISScreenFlash(((ENISScreenFlash::StaticData *) staticData)->fLength);
}

int ENISScreenFlash_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISScreenFlash(lua_tonumber(L, 1));
    }
    return 0;
}

void ENISScreenFlash_ResolveEvent_Callback(void *event, const UGroup *group) {
}
