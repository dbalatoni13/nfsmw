#include "EStopObjectEffects.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EStopObjectEffects::EStopObjectEffects(unsigned int phModel) : Event(0x10), fhModel(phModel) {
}

EStopObjectEffects::~EStopObjectEffects() {
    IModel *model = IModel::FindInstance((HMODEL) fhModel);

    if (model) {
        model->StopEffects();
    }
}

const char *EStopObjectEffects::GetEventName() const {
    return "EStopObjectEffects";
}

void EStopObjectEffects_MakeEvent_Callback(const void *staticData) {
    new EStopObjectEffects(gEventDynamicData.fhModel);
}

int EStopObjectEffects_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EStopObjectEffects(gEventDynamicData.fhModel);
    }
    return 0;
}

void EStopObjectEffects_ResolveEvent_Callback(void *event, const UGroup *group) {
}
