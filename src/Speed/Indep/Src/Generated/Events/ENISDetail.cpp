#include "ENISDetail.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/Scenery.hpp"

ENISDetail::ENISDetail(int pSceneryDetail) : Event(0x10), fSceneryDetail(pSceneryDetail) {}

ENISDetail::~ENISDetail() {
    switch (fSceneryDetail) {
    case SCENERY_DETAIL_NONE:
        ForceAllSceneryDetailLevels = SCENERY_DETAIL_NONE;
        break;
    case SCENERY_DETAIL_HIGH:
        ForceAllSceneryDetailLevels = SCENERY_DETAIL_HIGH;
        break;
    case SCENERY_DETAIL_MEDIUM:
        ForceAllSceneryDetailLevels = SCENERY_DETAIL_MEDIUM;
        break;
    case SCENERY_DETAIL_LOW:
        ForceAllSceneryDetailLevels = SCENERY_DETAIL_LOW;
        break;
    case SCENERY_DETAIL_REFLECTION:
        ForceAllSceneryDetailLevels = SCENERY_DETAIL_REFLECTION;
        break;
    default:
        ForceAllSceneryDetailLevels = SCENERY_DETAIL_NONE;
        break;
    }
}

const char *ENISDetail::GetEventName() const {
    return "ENISDetail";
}

void ENISDetail_MakeEvent_Callback(const void *staticData) {
    new ENISDetail(((ENISDetail::StaticData *) staticData)->fSceneryDetail);
}

int ENISDetail_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISDetail((int) lua_tonumber(L, 1));
    }
    return 0;
}

void ENISDetail_ResolveEvent_Callback(void *event, const UGroup *group) {
}
