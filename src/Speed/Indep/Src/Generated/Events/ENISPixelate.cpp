#include "ENISPixelate.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/FacePixelate.hpp"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISPixelate::ENISPixelate(int pEnable, float pWidth, float pHeight) : Event(0x20), fEnable(pEnable), fWidth(pWidth), fHeight(pHeight) {
    if (pEnable == 0) {
        FacePixelation::Disable();
    } else {
        FacePixelation::Enable();
        FacePixelation::SetDimensions(pWidth, pHeight);
    }
}

ENISPixelate::~ENISPixelate() {}

const char *ENISPixelate::GetEventName() const {
    return "ENISPixelate";
}

void ENISPixelate_MakeEvent_Callback(const void *staticData) {
    new ENISPixelate(((ENISPixelate::StaticData *)staticData)->fEnable, ((ENISPixelate::StaticData *)staticData)->fWidth, ((ENISPixelate::StaticData *)staticData)->fHeight);
}

int ENISPixelate_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 3) {
        new ENISPixelate((int)lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
    }
    return 0;
}

void ENISPixelate_ResolveEvent_Callback(void *event, const UGroup *group) {}
