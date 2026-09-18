#include "EFadeScreenNoLoadingBarOff.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EFadeScreenNoLoadingBarOff::EFadeScreenNoLoadingBarOff() : Event(0x10) {
    if (cFEng::Get()->IsPackagePushed("FadeScreenNoLoadingBar.fng")) {
        cFEng::Get()->PopNoControlPackage("FadeScreenNoLoadingBar.fng");
    }
}

EFadeScreenNoLoadingBarOff::~EFadeScreenNoLoadingBarOff() {}

const char *EFadeScreenNoLoadingBarOff::GetEventName() const {
    return "EFadeScreenNoLoadingBarOff";
}

void EFadeScreenNoLoadingBarOff_MakeEvent_Callback(const void *staticData) {
    new EFadeScreenNoLoadingBarOff();
}

int EFadeScreenNoLoadingBarOff_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EFadeScreenNoLoadingBarOff();
    }
    return 0;
}

void EFadeScreenNoLoadingBarOff_ResolveEvent_Callback(void *event, const UGroup *group) {}
