#include "EEnterEngagableTrigger.hpp"

#include <new>

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/IMenuZoneTrigger.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

EEnterEngagableTrigger::EEnterEngagableTrigger(GRuntimeInstance *pRaceActivity) : Event(0x10), fRaceActivity(pRaceActivity) {
}

EEnterEngagableTrigger::~EEnterEngagableTrigger() {
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    if (player) {
        IMenuZoneTrigger *izone;

        if (player->GetHud() && player->GetHud()->QueryInterface(&izone)) {
            izone->EnterTrigger(fRaceActivity);
        }
    }
}

const char *EEnterEngagableTrigger::GetEventName() const {
    return "EEnterEngagableTrigger";
}

void EEnterEngagableTrigger_MakeEvent_Callback(const void *staticData) {
    new EEnterEngagableTrigger(((EEnterEngagableTrigger::StaticData *) staticData)->fRaceActivity);
}

int EEnterEngagableTrigger_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EEnterEngagableTrigger((GRuntimeInstance *) lua_tostring(L, 1));
    }
    return 0;
}

void EEnterEngagableTrigger_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EEnterEngagableTrigger::StaticData *) event)->fRaceActivity) CARP::TagReference(group);
}
