#include "ENISNitro.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISNitro::ENISNitro(unsigned int pOn, unsigned int phSimable) : Event(0x10), fOn(pOn), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        INISCarEngine *inis;

        if (isimable->QueryInterface(&inis)) {
            if (fOn == 0) {
                inis->SetNitro(false);
            } else {
                inis->SetNitro(true);
            }
        }
    }
}

ENISNitro::~ENISNitro() {
}

const char *ENISNitro::GetEventName() const {
    return "ENISNitro";
}

void ENISNitro_MakeEvent_Callback(const void *staticData) {
    new ENISNitro(((ENISNitro::StaticData *) staticData)->fOn, gEventDynamicData.fhSimable);
}

int ENISNitro_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISNitro((unsigned int) lua_tonumber(L, 1), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISNitro_ResolveEvent_Callback(void *event, const UGroup *group) {
}
