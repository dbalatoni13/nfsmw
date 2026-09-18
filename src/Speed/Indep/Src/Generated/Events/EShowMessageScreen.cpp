#include "EShowMessageScreen.hpp"

#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"

EShowMessageScreen::EShowMessageScreen(unsigned int pID) : Event(0x10), fID(pID) {
    FEManager::Get()->SuppressControllerError(true);
}

EShowMessageScreen::~EShowMessageScreen() {
    cFEng::Get()->QueuePackagePush("SixDaysLater.fng", fID, 0, false);
}

const char *EShowMessageScreen::GetEventName() const {
    return "EShowMessageScreen";
}

void EShowMessageScreen_MakeEvent_Callback(const void *staticData) {
    new EShowMessageScreen(((EShowMessageScreen::StaticData *) staticData)->fID);
}

int EShowMessageScreen_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EShowMessageScreen((unsigned int) lua_tonumber(L, 1));
    }
    return 0;
}

void EShowMessageScreen_ResolveEvent_Callback(void *event, const UGroup *group) {
}
