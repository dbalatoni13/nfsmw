#include "EWorldMapOn.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EWorldMapOn::EWorldMapOn() : Event(0x10) {
    if (FEManager::IsOkayToRequestPauseSimulation(0, true, false)) {
        unsigned long joyParam = FEngMapJoyportToJoyParam(FEDatabase->GetPlayersJoystickPort(0));
        cFEng::Get()->QueuePackageSwitch("WorldMapMain.fng", 0, joyParam, false);
    }
}

EWorldMapOn::~EWorldMapOn() {}

const char *EWorldMapOn::GetEventName() const {
    return "EWorldMapOn";
}

void EWorldMapOn_MakeEvent_Callback(const void *staticData) {
    new EWorldMapOn();
}

int EWorldMapOn_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EWorldMapOn();
    }
    return 0;
}

void EWorldMapOn_ResolveEvent_Callback(void *event, const UGroup *group) {}
