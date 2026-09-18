#include "ENISBrakelock.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISBrakelock::ENISBrakelock(unsigned int pFront, unsigned int pRear, unsigned int phSimable) : Event(0x20), fFront(pFront), fRear(pRear), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        INISCarControl *inis;

        if (isimable->QueryInterface(&inis)) {
            inis->SetBrakeLock(fFront, fRear);
        }
    }
}

ENISBrakelock::~ENISBrakelock() {
}

const char *ENISBrakelock::GetEventName() const {
    return "ENISBrakelock";
}

void ENISBrakelock_MakeEvent_Callback(const void *staticData) {
    new ENISBrakelock(((ENISBrakelock::StaticData *) staticData)->fFront, ((ENISBrakelock::StaticData *) staticData)->fRear, gEventDynamicData.fhSimable);
}

int ENISBrakelock_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ENISBrakelock((unsigned int) lua_tonumber(L, 1), (unsigned int) lua_tonumber(L, 2), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISBrakelock_ResolveEvent_Callback(void *event, const UGroup *group) {
}
