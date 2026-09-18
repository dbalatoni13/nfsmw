#include "ENISCarRoll.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISCarRoll::ENISCarRoll(float pRoll, float pTime, unsigned int phSimable) : Event(0x20), fRoll(pRoll), fTime(pTime), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        INISCarControl *inis;

        if (isimable->QueryInterface(&inis)) {
            inis->SetAnimRoll(fRoll, fTime);
        }
    }
}

ENISCarRoll::~ENISCarRoll() {
}

const char *ENISCarRoll::GetEventName() const {
    return "ENISCarRoll";
}

void ENISCarRoll_MakeEvent_Callback(const void *staticData) {
    new ENISCarRoll(((ENISCarRoll::StaticData *) staticData)->fRoll, ((ENISCarRoll::StaticData *) staticData)->fTime, gEventDynamicData.fhSimable);
}

int ENISCarRoll_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ENISCarRoll(lua_tonumber(L, 1), lua_tonumber(L, 2), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISCarRoll_ResolveEvent_Callback(void *event, const UGroup *group) {
}
