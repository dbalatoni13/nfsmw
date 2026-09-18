#include "ENISMotionBlur.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISMotionBlur::ENISMotionBlur(int pEnableMotionBlur) : Event(0x10), fEnableMotionBlur(pEnableMotionBlur) {}

ENISMotionBlur::~ENISMotionBlur() {}

const char *ENISMotionBlur::GetEventName() const {
    return "ENISMotionBlur";
}

void ENISMotionBlur_MakeEvent_Callback(const void *staticData) {
    new ENISMotionBlur(((ENISMotionBlur::StaticData *)staticData)->fEnableMotionBlur);
}

int ENISMotionBlur_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISMotionBlur((int)lua_tonumber(L, 1));
    }
    return 0;
}

void ENISMotionBlur_ResolveEvent_Callback(void *event, const UGroup *group) {}
