#include "ENISBurnout.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISBurnout::ENISBurnout(float pSpeed, unsigned int phSimable) : Event(0x10), fSpeed(pSpeed), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        INISCarControl *inis;

        if (isimable->QueryInterface(&inis)) {
            inis->SetBurnout(fSpeed);
        }
    }
}

ENISBurnout::~ENISBurnout() {
}

const char *ENISBurnout::GetEventName() const {
    return "ENISBurnout";
}

void ENISBurnout_MakeEvent_Callback(const void *staticData) {
    new ENISBurnout(((ENISBurnout::StaticData *) staticData)->fSpeed, gEventDynamicData.fhSimable);
}

int ENISBurnout_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISBurnout(lua_tonumber(L, 1), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISBurnout_ResolveEvent_Callback(void *event, const UGroup *group) {
}
