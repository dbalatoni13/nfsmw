#include "EShowMarketingScreen.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

EShowMarketingScreen::EShowMarketingScreen(int pShowFor) : Event(0x10), fShowFor(pShowFor) {
    const char *fScreenName = "Marketing1.fng";

    if (cFEng::Get()->IsPackageInControl(LoadingBootName)) {
        cFEng::Get()->QueuePackagePop(1);
    }

    if (!cFEng::Get()->IsPackageInControl(fScreenName)) {
        cFEng::Get()->QueuePackagePush(fScreenName, fShowFor, 0, false);
    }
}

EShowMarketingScreen::~EShowMarketingScreen() {
}

const char *EShowMarketingScreen::GetEventName() const {
    return "EShowMarketingScreen";
}

void EShowMarketingScreen_MakeEvent_Callback(const void *staticData) {
    new EShowMarketingScreen(((EShowMarketingScreen::StaticData *) staticData)->fShowFor);
}

int EShowMarketingScreen_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EShowMarketingScreen((int) lua_tonumber(L, 1));
    }
    return 0;
}

void EShowMarketingScreen_ResolveEvent_Callback(void *event, const UGroup *group) {
}
