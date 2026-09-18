#include "ENISNeutralRev.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISNeutralRev::ENISNeutralRev(unsigned int pOn, float pThrottle, float pSpeed, unsigned int phSimable)
    : Event(0x20), fOn(pOn), fThrottle(pThrottle), fSpeed(pSpeed), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        INISCarEngine *inis;

        if (isimable->QueryInterface(&inis)) {
            if (fOn == 0) {
                inis->SetNeutralRev(false, 0.0f, 0.0f);
            } else {
                inis->SetNeutralRev(true, fThrottle, fSpeed);
            }
        }
    }
}

ENISNeutralRev::~ENISNeutralRev() {
}

const char *ENISNeutralRev::GetEventName() const {
    return "ENISNeutralRev";
}

void ENISNeutralRev_MakeEvent_Callback(const void *staticData) {
    new ENISNeutralRev(((ENISNeutralRev::StaticData *) staticData)->fOn, ((ENISNeutralRev::StaticData *) staticData)->fThrottle,
                       ((ENISNeutralRev::StaticData *) staticData)->fSpeed, gEventDynamicData.fhSimable);
}

int ENISNeutralRev_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 3) {
        new ENISNeutralRev((unsigned int) lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISNeutralRev_ResolveEvent_Callback(void *event, const UGroup *group) {
}
