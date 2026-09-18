#include "EAddSMS.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"

EAddSMS::EAddSMS(int pNumber) : Event(0x10), fNumber(pNumber) {
}

EAddSMS::~EAddSMS() {
    if (GManager::Exists()) {
        GManager::Get().AddSMS(fNumber);
    }
}

const char *EAddSMS::GetEventName() const {
    return "EAddSMS";
}

void EAddSMS_MakeEvent_Callback(const void *staticData) {
    new EAddSMS(((EAddSMS::StaticData *) staticData)->fNumber);
}

int EAddSMS_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EAddSMS((int) lua_tonumber(L, 1));
    }
    return 0;
}

void EAddSMS_ResolveEvent_Callback(void *event, const UGroup *group) {
}
