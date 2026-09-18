#include "ENISCarDamageReset.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISCarDamageReset::ENISCarDamageReset(unsigned int phSimable) : Event(0x10), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        IDamageable *idamage;

        if (isimable->QueryInterface(&idamage)) {
            idamage->ResetDamage();
        }
    }
}

ENISCarDamageReset::~ENISCarDamageReset() {
}

const char *ENISCarDamageReset::GetEventName() const {
    return "ENISCarDamageReset";
}

void ENISCarDamageReset_MakeEvent_Callback(const void *staticData) {
    new ENISCarDamageReset(gEventDynamicData.fhSimable);
}

int ENISCarDamageReset_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new ENISCarDamageReset(gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISCarDamageReset_ResolveEvent_Callback(void *event, const UGroup *group) {
}
