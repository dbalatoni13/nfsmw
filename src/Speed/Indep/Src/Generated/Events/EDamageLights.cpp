#include "EDamageLights.hpp"

#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/World/VehicleFX.h"

EDamageLights::EDamageLights(UCrc32 pName, unsigned int phSimable) : Event(0x10), fName(pName), fhSimable(phSimable) {
}

EDamageLights::~EDamageLights() {
    ISimable *iowner = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (iowner) {
        IDamageableVehicle *idamage;

        if (iowner->QueryInterface(&idamage)) {
            VehicleFX::ID id = VehicleFX::LookupID(fName);

            if (id) {
                idamage->DamageLight(id, true);
            } else {
                bBreak();
            }
        }
    }
}

const char *EDamageLights::GetEventName() const {
    return "EDamageLights";
}

void EDamageLights_MakeEvent_Callback(const void *staticData) {
    new EDamageLights(((EDamageLights::StaticData *) staticData)->fName, gEventDynamicData.fhSimable);
}

int EDamageLights_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EDamageLights(UCrc32(lua_tostring(L, 1)), gEventDynamicData.fhSimable);
    }
    return 0;
}

void EDamageLights_ResolveEvent_Callback(void *event, const UGroup *group) {
}
