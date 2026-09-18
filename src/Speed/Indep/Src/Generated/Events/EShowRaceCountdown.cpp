#include "EShowRaceCountdown.hpp"

#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

EShowRaceCountdown::EShowRaceCountdown() : Event(0x10) {
}

EShowRaceCountdown::~EShowRaceCountdown() {
    for (IPlayer::List::const_iterator iter = IPlayer::GetList(PLAYER_LOCAL).begin(); iter != IPlayer::GetList(PLAYER_LOCAL).end(); ++iter) {
        IPlayer *player = *iter;
        ICountdown *icountdown;

        if (player->GetHud()->QueryInterface(&icountdown)) {
            icountdown->BeginCountdown();
        }
    }
}

const char *EShowRaceCountdown::GetEventName() const {
    return "EShowRaceCountdown";
}

void EShowRaceCountdown_MakeEvent_Callback(const void *staticData) {
    new EShowRaceCountdown();
}

int EShowRaceCountdown_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EShowRaceCountdown();
    }
    return 0;
}

void EShowRaceCountdown_ResolveEvent_Callback(void *event, const UGroup *group) {
}
