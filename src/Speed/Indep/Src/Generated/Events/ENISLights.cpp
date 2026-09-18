#include "ENISLights.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISLights::ENISLights(unsigned int pCopLights, unsigned int pHeadLights, unsigned int phSimable) : Event(0x20), fCopLights(pCopLights), fHeadLights(pHeadLights), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        IVehicle *ivehicle;

        if (isimable->QueryInterface(&ivehicle)) {
            if (fCopLights == 0) {
                ivehicle->GlareOff(VehicleFX::LIGHT_COPS);
            } else {
                ivehicle->GlareOn(VehicleFX::LIGHT_COPS);
            }

            if (fHeadLights == 0) {
                ivehicle->GlareOff(VehicleFX::LIGHT_HEADLIGHTS);
            } else {
                ivehicle->GlareOn(VehicleFX::LIGHT_HEADLIGHTS);
            }
        }
    }
}

ENISLights::~ENISLights() {
}

const char *ENISLights::GetEventName() const {
    return "ENISLights";
}

void ENISLights_MakeEvent_Callback(const void *staticData) {
    new ENISLights(((ENISLights::StaticData *) staticData)->fCopLights, ((ENISLights::StaticData *) staticData)->fHeadLights, gEventDynamicData.fhSimable);
}

int ENISLights_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ENISLights((unsigned int) lua_tonumber(L, 1), (unsigned int) lua_tonumber(L, 2), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISLights_ResolveEvent_Callback(void *event, const UGroup *group) {
}
