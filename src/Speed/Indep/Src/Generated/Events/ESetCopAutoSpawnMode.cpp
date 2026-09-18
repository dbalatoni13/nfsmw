#include "ESetCopAutoSpawnMode.hpp"

#include "Speed/Indep/Src/Generated/Messages/MSetCopAutoSpawnMode.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

ESetCopAutoSpawnMode::ESetCopAutoSpawnMode(int pAutoSpawn) : Event(0x10), fAutoSpawn(pAutoSpawn) {
}

ESetCopAutoSpawnMode::~ESetCopAutoSpawnMode() {
    MSetCopAutoSpawnMode(fAutoSpawn).Send(UCrc32("AICopManager"));
}

const char *ESetCopAutoSpawnMode::GetEventName() const {
    return "ESetCopAutoSpawnMode";
}

void ESetCopAutoSpawnMode_MakeEvent_Callback(const void *staticData) {
    new ESetCopAutoSpawnMode(((ESetCopAutoSpawnMode::StaticData *) staticData)->fAutoSpawn);
}

int ESetCopAutoSpawnMode_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ESetCopAutoSpawnMode((int) lua_tonumber(L, 1));
    }
    return 0;
}

void ESetCopAutoSpawnMode_ResolveEvent_Callback(void *event, const UGroup *group) {
}
