#include "ENISNukeSmack.hpp"

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

void NIS_NukeSmackablesWithinRange(const UMath::Vector3 &position, float radius);

ENISNukeSmack::ENISNukeSmack(float pPositionX, float pPositionY, float pPositionZ, float pRadius)
    : Event(0x20), fPositionX(pPositionX), fPositionY(pPositionY), fPositionZ(pPositionZ), fRadius(pRadius) {
}

ENISNukeSmack::~ENISNukeSmack() {
    UMath::Vector3 position = {-fPositionY, fPositionZ, fPositionX};

    NIS_NukeSmackablesWithinRange(position, fRadius);
}

const char *ENISNukeSmack::GetEventName() const {
    return "ENISNukeSmack";
}

void ENISNukeSmack_MakeEvent_Callback(const void *staticData) {
    new ENISNukeSmack(((ENISNukeSmack::StaticData *) staticData)->fPositionX, ((ENISNukeSmack::StaticData *) staticData)->fPositionY, ((ENISNukeSmack::StaticData *) staticData)->fPositionZ, ((ENISNukeSmack::StaticData *) staticData)->fRadius);
}

int ENISNukeSmack_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 4) {
        new ENISNukeSmack(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    return 0;
}

void ENISNukeSmack_ResolveEvent_Callback(void *event, const UGroup *group) {
}
