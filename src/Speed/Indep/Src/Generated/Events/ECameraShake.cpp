#include "ECameraShake.hpp"

#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Misc/Rumble.hpp"

ECameraShake::ECameraShake() : Event(0x10) {
}

ECameraShake::~ECameraShake() {
    if (IPlayer::Count(PLAYER_ALL) <= 1) {
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);

        if (player) {
            bVector3 shake(0.2f, 0.0f, 0.3f);
            ForceCameraShake(0, &shake);
        }
    }
}

const char *ECameraShake::GetEventName() const {
    return "ECameraShake";
}

void ECameraShake_MakeEvent_Callback(const void *staticData) {
    new ECameraShake();
}

int ECameraShake_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new ECameraShake();
    }
    return 0;
}

void ECameraShake_ResolveEvent_Callback(void *event, const UGroup *group) {
}
