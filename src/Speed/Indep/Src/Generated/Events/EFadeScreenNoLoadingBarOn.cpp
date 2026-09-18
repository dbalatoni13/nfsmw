#include "EFadeScreenNoLoadingBarOn.hpp"

#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EFadeScreenNoLoadingBarOn::EFadeScreenNoLoadingBarOn() : Event(0x10) {
    if (!cFEng::Get()->IsPackagePushed("FadeScreenNoLoadingBar.fng")) {
        FEManager::Get()->SuppressControllerError(true);
        cFEng::Get()->PushNoControlPackage("FadeScreenNoLoadingBar.fng", FE_PACKAGE_PRIORITY_SECOND_CLOSEST);
    }
}

EFadeScreenNoLoadingBarOn::~EFadeScreenNoLoadingBarOn() {}

const char *EFadeScreenNoLoadingBarOn::GetEventName() const {
    return "EFadeScreenNoLoadingBarOn";
}

void EFadeScreenNoLoadingBarOn_MakeEvent_Callback(const void *staticData) {
    new EFadeScreenNoLoadingBarOn();
}

int EFadeScreenNoLoadingBarOn_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EFadeScreenNoLoadingBarOn();
    }
    return 0;
}

void EFadeScreenNoLoadingBarOn_ResolveEvent_Callback(void *event, const UGroup *group) {}
