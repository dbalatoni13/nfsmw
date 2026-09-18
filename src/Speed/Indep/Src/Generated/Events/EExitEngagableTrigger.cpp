#include "EExitEngagableTrigger.hpp"

#include <new>

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/IMenuZoneTrigger.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

EExitEngagableTrigger::EExitEngagableTrigger(GRuntimeInstance *pRaceActivity) : Event(0x10), fRaceActivity(pRaceActivity) {
}

EExitEngagableTrigger::~EExitEngagableTrigger() {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    if (player) {
        IMenuZoneTrigger *izone;

        if (player->GetHud() && player->GetHud()->QueryInterface(&izone)) {
            izone->ExitTrigger();
        }
    }
}

const char *EExitEngagableTrigger::GetEventName() const {
    return "EExitEngagableTrigger";
}

void EExitEngagableTrigger_MakeEvent_Callback(const void *staticData) {
    new EExitEngagableTrigger(((EExitEngagableTrigger::StaticData *) staticData)->fRaceActivity);
}

int EExitEngagableTrigger_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EExitEngagableTrigger((GRuntimeInstance *) lua_tostring(L, 1));
    }
    return 0;
}

void EExitEngagableTrigger_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EExitEngagableTrigger::StaticData *) event)->fRaceActivity) CARP::TagReference(group);
}
