#include "ENISRain.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/Rain.hpp"

extern int PrecipitationEnable;

ENISRain::ENISRain(float pRainIntensity) : Event(0x10), fRainIntensity(pRainIntensity) {
    PrecipitationEnable = 1;
    SetOverRideRainIntensity(fRainIntensity);
}

ENISRain::~ENISRain() {
}

const char *ENISRain::GetEventName() const {
    return "ENISRain";
}

void ENISRain_MakeEvent_Callback(const void *staticData) {
    new ENISRain(((ENISRain::StaticData *) staticData)->fRainIntensity);
}

int ENISRain_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISRain(lua_tonumber(L, 1));
    }
    return 0;
}

void ENISRain_ResolveEvent_Callback(void *event, const UGroup *group) {
}
