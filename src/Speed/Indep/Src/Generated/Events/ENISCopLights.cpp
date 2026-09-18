#include "ENISCopLights.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

extern bool gTWEAKER_NISLightEnabled;
extern float gTWEAKER_NISLightIntensity;
extern float gTWEAKER_NISLightPosX;
extern float gTWEAKER_NISLightPosY;
extern float gTWEAKER_NISLightPosZ;

ENISCopLights::ENISCopLights(int pEnable, float pLightIntensity, float pLightPosX, float pLightPosY, float pLightPosZ) : Event(0x20), fEnable(pEnable), fLightIntensity(pLightIntensity), fLightPosX(pLightPosX), fLightPosY(pLightPosY), fLightPosZ(pLightPosZ) {
    gTWEAKER_NISLightEnabled = pEnable != 0;
    if (pEnable != 0) {
        gTWEAKER_NISLightIntensity = pLightIntensity;
        gTWEAKER_NISLightPosX = pLightPosX;
        gTWEAKER_NISLightPosY = pLightPosY;
        gTWEAKER_NISLightPosZ = pLightPosZ;
    }
}

ENISCopLights::~ENISCopLights() {}

const char *ENISCopLights::GetEventName() const {
    return "ENISCopLights";
}

void ENISCopLights_MakeEvent_Callback(const void *staticData) {
    new ENISCopLights(((ENISCopLights::StaticData *)staticData)->fEnable, ((ENISCopLights::StaticData *)staticData)->fLightIntensity, ((ENISCopLights::StaticData *)staticData)->fLightPosX, ((ENISCopLights::StaticData *)staticData)->fLightPosY, ((ENISCopLights::StaticData *)staticData)->fLightPosZ);
}

int ENISCopLights_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 5) {
        new ENISCopLights((int)lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
    }
    return 0;
}

void ENISCopLights_ResolveEvent_Callback(void *event, const UGroup *group) {}
