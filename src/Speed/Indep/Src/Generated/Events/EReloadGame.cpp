#include "EReloadGame.hpp"

#include <new>

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

void DismissChyron();

EReloadGame::EReloadGame(GRuntimeInstance *pRestartRace) : Event(0x10), fRestartRace(pRestartRace) {
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING && fRestartRace) {
        GActivity *activity = (GActivity *) fRestartRace;
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(activity);

        if (parms) {
            GManager::Get().SetRestartEvent(parms->GetEventHash());
        }
    }
}

EReloadGame::~EReloadGame() {
    if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING) {
        DismissChyron();

        if (cFEng::Get()->IsPackagePushed("FadeScreen.fng")) {
            cFEng::Get()->PopNoControlPackage("FadeScreen.fng");
        }

        TheGameFlowManager.ReloadTrack();
    }
}

const char *EReloadGame::GetEventName() const {
    return "EReloadGame";
}

void EReloadGame_MakeEvent_Callback(const void *staticData) {
    new EReloadGame(((EReloadGame::StaticData *) staticData)->fRestartRace);
}

int EReloadGame_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EReloadGame((GRuntimeInstance *) lua_tostring(L, 1));
    }
    return 0;
}

void EReloadGame_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EReloadGame::StaticData *) event)->fRestartRace) CARP::TagReference(group);
}
