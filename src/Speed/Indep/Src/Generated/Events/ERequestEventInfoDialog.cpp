#include "ERequestEventInfoDialog.hpp"

#include <new>

#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"

ERequestEventInfoDialog::ERequestEventInfoDialog(int pJoyPort, GRuntimeInstance *pRaceActivity) : Event(0x10), fJoyPort(pJoyPort), fRaceActivity(pRaceActivity) {
}

ERequestEventInfoDialog::~ERequestEventInfoDialog() {
    cFEng::Get()->QueuePackagePush("EngageEventDialog.fng", (int) fRaceActivity, 0, false);
}

const char *ERequestEventInfoDialog::GetEventName() const {
    return "ERequestEventInfoDialog";
}

void ERequestEventInfoDialog_MakeEvent_Callback(const void *staticData) {
    new ERequestEventInfoDialog(((ERequestEventInfoDialog::StaticData *) staticData)->fJoyPort, ((ERequestEventInfoDialog::StaticData *) staticData)->fRaceActivity);
}

int ERequestEventInfoDialog_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ERequestEventInfoDialog((int) lua_tonumber(L, 1), (GRuntimeInstance *) lua_tostring(L, 2));
    }
    return 0;
}

void ERequestEventInfoDialog_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ERequestEventInfoDialog::StaticData *) event)->fRaceActivity) CARP::TagReference(group);
}
