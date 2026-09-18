#include "ENISWolrdGeometry.hpp"

#include <new>

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

ENISWolrdGeometry::ENISWolrdGeometry(const char *pSceneryGroupName, int pEnable) : Event(0x10), fSceneryGroupName(EventManager::EmbedField(this, pSceneryGroupName)), fEnable(pEnable) {
    if (fSceneryGroupName && bStrLen(fSceneryGroupName)) {
        if (fEnable) {
            EnableBarrierSceneryGroup(fSceneryGroupName, false);
        } else {
            DisableSceneryGroup(bStringHash(fSceneryGroupName));
        }
    }
}

ENISWolrdGeometry::~ENISWolrdGeometry() {
}

const char *ENISWolrdGeometry::GetEventName() const {
    return "ENISWolrdGeometry";
}

void ENISWolrdGeometry_MakeEvent_Callback(const void *staticData) {
    new ENISWolrdGeometry(((ENISWolrdGeometry::StaticData *) staticData)->fSceneryGroupName, ((ENISWolrdGeometry::StaticData *) staticData)->fEnable);
}

int ENISWolrdGeometry_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ENISWolrdGeometry(lua_tostring(L, 1), (int) lua_tonumber(L, 2));
    }
    return 0;
}

void ENISWolrdGeometry_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ENISWolrdGeometry::StaticData *) event)->fSceneryGroupName) CARP::TagReference(group);
}
