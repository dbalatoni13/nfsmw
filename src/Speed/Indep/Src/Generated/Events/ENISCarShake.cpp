#include "ENISCarShake.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISCarShake::ENISCarShake(float pShake, float pRate, float pRamp, float pTime, unsigned int phSimable) : Event(0x20), fShake(pShake), fRate(pRate), fRamp(pRamp), fTime(pTime), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        INISCarControl *inis;

        if (isimable->QueryInterface(&inis)) {
            inis->SetAnimShake(fShake, fRate, fRamp, fTime);
        }
    }
}

ENISCarShake::~ENISCarShake() {
}

const char *ENISCarShake::GetEventName() const {
    return "ENISCarShake";
}

void ENISCarShake_MakeEvent_Callback(const void *staticData) {
    new ENISCarShake(((ENISCarShake::StaticData *) staticData)->fShake, ((ENISCarShake::StaticData *) staticData)->fRate, ((ENISCarShake::StaticData *) staticData)->fRamp, ((ENISCarShake::StaticData *) staticData)->fTime, gEventDynamicData.fhSimable);
}

int ENISCarShake_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 4) {
        new ENISCarShake(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISCarShake_ResolveEvent_Callback(void *event, const UGroup *group) {
}
