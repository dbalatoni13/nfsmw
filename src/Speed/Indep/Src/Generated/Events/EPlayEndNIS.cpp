#include "Speed/Indep/Src/Generated/Events/EPlayRaceNIS.hpp"

#include "EPlayEndNIS.hpp"

#include <new>

#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

EPlayEndNIS::EPlayEndNIS(const char *pSceneName) : Event(0x10), fSceneName(EventManager::EmbedField(this, pSceneName)) {
}

EPlayEndNIS::~EPlayEndNIS() {
    new EPlayRaceNIS(NULL, fSceneName, "Intro", 0, 0, "", "");
}

const char *EPlayEndNIS::GetEventName() const {
    return "EPlayEndNIS";
}

void EPlayEndNIS_MakeEvent_Callback(const void *staticData) {
    new EPlayEndNIS(((EPlayEndNIS::StaticData *) staticData)->fSceneName);
}

int EPlayEndNIS_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EPlayEndNIS(lua_tostring(L, 1));
    }
    return 0;
}

void EPlayEndNIS_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EPlayEndNIS::StaticData *) event)->fSceneName) CARP::TagReference(group);
}
