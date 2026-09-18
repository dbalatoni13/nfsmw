#include "ETerminateMusic.hpp"

#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

ETerminateMusic::ETerminateMusic() : Event(0x10) {
}

ETerminateMusic::~ETerminateMusic() {
    MNotifyMusicFlow(0).Send(UCrc32("Terminate"));
}

const char *ETerminateMusic::GetEventName() const {
    return "ETerminateMusic";
}

void ETerminateMusic_MakeEvent_Callback(const void *staticData) {
    new ETerminateMusic();
}

int ETerminateMusic_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new ETerminateMusic();
    }
    return 0;
}

void ETerminateMusic_ResolveEvent_Callback(void *event, const UGroup *group) {
}
